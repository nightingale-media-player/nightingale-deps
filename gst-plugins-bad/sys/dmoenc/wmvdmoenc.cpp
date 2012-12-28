/* Windows DMO encoder wrapper for GStreamer
 * Copyright (C) 2009,2010 Pioneers of the Inevitable <songbird@songbirdnest.com>
 *
 * Authors: Michael Smith <msmith@songbirdnest.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "comtaskthread.h"
#include "dmoenc.h"

#include <amvideo.h>

#define DEFAULT_BITRATE 500000 

/* GObject properties */
enum
{
  ARG_0,
  ARG_BITRATE
};

static GstStaticPadTemplate wmvdmoenc_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-yuv, "
        "format = (fourcc)I420, "
        "width = (int)[1, 4096], "
        "height = (int)[1, 4096]"));

static GstStaticPadTemplate wmvdmoenc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-wmv, "
    	"width = (int) [1,4096], "
	    "height = (int) [1,4096], "
    	"framerate = (fraction)[1/1, MAX], " 
	    "wmvversion = (int)3"));

typedef struct _WMVDMOEncClass
{
  GstElementClass parent_class;
} WMVDMOEncClass;

typedef struct _WMVDMOEnc
{
  GstElement parent;

  GstPad *sinkpad;
  GstPad *srcpad;

  gboolean is_setup;

  /* Output format */
  GstCaps *output_caps;

  /* From received caps */
  int width;
  int height;
  int framerate_num;
  int framerate_denom;

  /* Bitrate set via property */
  int bitrate;

  /* COM thread helper. */
  GstCOMTaskThread *comthread;

  /* All the following objects MUST only be used from the COM thread */

  /* The actual encoder object */
  IMediaObject *dmo;
} WMVDMOEnc;

GST_BOILERPLATE (WMVDMOEnc, wmvdmoenc, GstElement, GST_TYPE_ELEMENT);

static GstBuffer *
wmvdmoenc_get_codec_data (DMO_MEDIA_TYPE *fmt, IMediaObject *enc)
{
  IWMCodecPrivateData* privdata = NULL;
  HRESULT hr;
  GstBuffer *buffer = NULL;
  DWORD datalen = 0;
  BYTE* data = NULL;

  // Get the private data interface.
  hr = enc->QueryInterface(IID_IWMCodecPrivateData,
                            (void**)&privdata);
  if (FAILED (hr)) {
    GST_WARNING ("Could not QI encoder to WMCodecPrivateData: %x", hr);
    goto fail;
  }

  // Set the partial media type.
  hr = privdata->SetPartialOutputType(fmt);
  if (FAILED (hr)) {
    GST_WARNING ("Could not set partial output type: %x", hr);
    goto fail;
  }

  // Get the size of the private data.
  hr = privdata->GetPrivateData(NULL, &datalen);
  if (FAILED (hr)) {
    GST_WARNING ("Could not get length of private data from encoder: %x", hr);
    goto fail;
  }

  // Allocate memory for the private data.
  buffer = gst_buffer_new_and_alloc (datalen);
  data = GST_BUFFER_DATA (buffer);

  // Get the private data.
  hr = privdata->GetPrivateData(data, &datalen);
  if (FAILED (hr)) {
    GST_WARNING ("Could not get private data from encoder: %x", hr);
    goto fail;
  }

  /* It turns out that when we got the size earlier, that just gave us a
     bigger-than-needed value, but it wasn't _actually_ the amount of data
     returned. When we got the actual data, it gave us the real size - so adjust
     the buffer size to match. */
  GST_BUFFER_SIZE (buffer) = datalen;

  privdata->Release();

  return buffer;

fail:
  if (privdata)
    privdata->Release();
  if (buffer)
    gst_buffer_unref (buffer);
  return NULL;
}

static GstCaps *
wmvdmoenc_caps_from_format (WMVDMOEnc *enc, DMO_MEDIA_TYPE *fmt)
{
  GstBuffer *extradata = NULL;

  if (IsEqualGUID (fmt->formattype, FORMAT_VideoInfo) && 
	fmt->cbFormat >= sizeof (VIDEOINFOHEADER))
  {
    GstCaps *caps;
    VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)fmt->pbFormat;
    BITMAPINFOHEADER *bmi = &vih->bmiHeader;

    /* Generate caps from the VIDEOINFOHEADER. Note that we take the framerate
       from our encoder object, not the VIDEOINFOHEADER, as we store it more
       precisely as a fraction in the encoder object */
    caps = gst_caps_new_simple ("video/x-wmv",
            "wmvversion", G_TYPE_INT, 3,
            "width", G_TYPE_INT, bmi->biWidth,
            "height", G_TYPE_INT, bmi->biHeight,
            "framerate", GST_TYPE_FRACTION, enc->framerate_num,
            enc->framerate_denom,
            NULL);

    /* Now we need to get the codec_data and attach that to the caps */
    extradata = wmvdmoenc_get_codec_data (fmt, enc->dmo);

    if (extradata) {
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, extradata,
              NULL);
      gst_buffer_unref (extradata);
    }

    return caps;
  }
  else {
    return NULL;
  }
}

static gboolean
wmvdmoenc_set_input_format (WMVDMOEnc * enc)
{
  HRESULT hr;
  DMO_MEDIA_TYPE fmt;
  VIDEOINFOHEADER *vih;
  BITMAPINFOHEADER *bmi;

  memset (&fmt, 0, sizeof (fmt));
  hr = MoInitMediaType (&fmt, sizeof (VIDEOINFOHEADER));

  if (FAILED (hr))
    return FALSE;

  fmt.majortype = WMMEDIATYPE_Video;
  fmt.subtype = WMMEDIASUBTYPE_I420;
  fmt.formattype = FORMAT_VideoInfo;

  vih = (VIDEOINFOHEADER *) fmt.pbFormat;
  bmi = &vih->bmiHeader;

  vih->rcSource.left = 0;
  vih->rcSource.right = enc->width;
  vih->rcSource.top = 0;
  vih->rcSource.bottom = enc->height;
  vih->rcTarget.left = 0;
  vih->rcTarget.right = enc->width;
  vih->rcTarget.top = 0;
  vih->rcTarget.bottom = enc->height;

  /* 12 bpp for YUV */
  vih->dwBitRate = enc->height * enc->width * 12 *
      enc->framerate_num / enc->framerate_denom;
  vih->dwBitErrorRate = 0;

  /* In units of 100 ns */
  vih->AvgTimePerFrame = GST_SECOND * enc->framerate_denom / enc->framerate_num;

  bmi->biSize = sizeof (BITMAPINFOHEADER);
  bmi->biWidth = enc->width;
  bmi->biHeight = enc->height;
  bmi->biPlanes = 1;
  bmi->biBitCount = 24;
  bmi->biCompression = GST_MAKE_FOURCC ('I', '4', '2', '0'); // ??
  bmi->biSizeImage = 0;
  bmi->biXPelsPerMeter = 0;
  bmi->biYPelsPerMeter = 0;
  bmi->biClrUsed = 0;
  bmi->biClrImportant = 0;

  hr = enc->dmo->SetInputType (0, &fmt, 0);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Couldn't set input format: %x", hr);
    return FALSE;
  }

  return TRUE;
}

static gboolean
wmvdmoenc_set_output_format (WMVDMOEnc * enc)
{
  DMO_MEDIA_TYPE mt;
  DMO_MEDIA_TYPE outfmt;
  VIDEOINFOHEADER *vih;
  int i;
  HRESULT hr;
  GstBuffer *extradata = NULL;
  int extradata_size;
  gboolean ret = FALSE;

  memset (&mt, 0, sizeof(mt));
  memset (&outfmt, 0, sizeof(outfmt));

  hr = enc->dmo->GetOutputType (0, 0, &mt);
  if (FAILED (hr)) {
    GST_WARNING ("Could not get output type from encoder: %x", hr);
    goto done;
  }

  vih = (VIDEOINFOHEADER *)mt.pbFormat;

  /* Configure the bitrate we want. We must do this before we get the codec
     private data */
  vih->dwBitRate = enc->bitrate;

  extradata = wmvdmoenc_get_codec_data (&mt, enc->dmo);
  if (extradata) {
    extradata_size = GST_BUFFER_SIZE (extradata);
  }
  else
    extradata_size = 0;

  /* Create a new DMO_MEDIA_TYPE. We allocate a new VIDEOINFOHEADER block that
     includes the private codec data blob.  */
  memcpy (&outfmt, &mt, sizeof (DMO_MEDIA_TYPE));
  outfmt.cbFormat = sizeof (VIDEOINFOHEADER) + extradata_size;
  outfmt.pbFormat = new BYTE[outfmt.cbFormat];

  /* Copy the VIDEOINFOHEADER part */
  memcpy (outfmt.pbFormat, mt.pbFormat, sizeof (VIDEOINFOHEADER));
  if (extradata)
    memcpy (outfmt.pbFormat + sizeof (VIDEOINFOHEADER),
          GST_BUFFER_DATA (extradata), extradata_size);

  hr = enc->dmo->SetOutputType (0, &outfmt, 0);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Failed to set output type: %x", hr);
    goto done;
  }

  enc->output_caps = wmvdmoenc_caps_from_format (enc, &outfmt);

  ret = TRUE;

done:
  if (extradata)
    gst_buffer_unref (extradata);
  MoFreeMediaType (&mt);
  if (outfmt.pbFormat)
    delete[] outfmt.pbFormat;


  return ret;
}

static void
wmvdmoenc_setup_task (void *data, void *ret)
{
  WMVDMOEnc *enc = (WMVDMOEnc *)data;
  gboolean *retval = (gboolean *)ret;
  HRESULT hr;

  hr = CoCreateInstance (CLSID_CWMV9EncMediaObject, NULL, CLSCTX_INPROC,
      IID_IMediaObject, (void **) &enc->dmo);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Could not create DMO Encoder object: %x", hr);
    *retval = FALSE;
    return;
  }

  if (!wmvdmoenc_set_input_format (enc))
  {
    GST_WARNING_OBJECT (enc, "Could not set up input format");
    *retval = FALSE;
    return;
  }

  if (!wmvdmoenc_set_output_format (enc))
  {
    GST_WARNING_OBJECT (enc, "Could not set up output format");
    *retval = FALSE;
    return;
  }

  gst_pad_set_caps (enc->srcpad, enc->output_caps);

  enc->is_setup = TRUE;
  *retval = TRUE;
  return;
}

static gboolean
wmvdmoenc_setup (WMVDMOEnc * enc)
{
  gboolean ret;
  gst_comtaskthread_do_task (enc->comthread, wmvdmoenc_setup_task, enc, &ret);
  return ret;
}

static void
wmvdmoenc_teardown_task (void *data, void *ret)
{
  WMVDMOEnc * enc = (WMVDMOEnc *)data;

  if (enc->output_caps) {
    gst_caps_unref (enc->output_caps);
    enc->output_caps = NULL;
  }

  if (enc->dmo) {
    enc->dmo->Release ();
    enc->dmo = NULL;
  }

  enc->is_setup = FALSE;
}

static void
wmvdmoenc_teardown (WMVDMOEnc * enc)
{
  gst_comtaskthread_do_task (enc->comthread, wmvdmoenc_teardown_task, enc,
          NULL);
}

static gboolean
wmvdmoenc_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  WMVDMOEnc *enc = GST_WMVDMOENC (gst_pad_get_parent (pad));
  GstStructure *structure;
  gboolean ret;

  structure = gst_caps_get_structure (caps, 0);
  // We don't currently check the format because we only support I420. Will need
  // to do that if we add more formats.

  gst_structure_get_int (structure, "width", &enc->width);
  gst_structure_get_int (structure, "height", &enc->height);
  gst_structure_get_fraction (structure, "framerate", &enc->framerate_num,
          &enc->framerate_denom);

  if (enc->is_setup)
    wmvdmoenc_teardown (enc);
  ret = wmvdmoenc_setup (enc);

  gst_object_unref (enc);

  return ret;
}

static GstFlowReturn
wmvdmoenc_do_output (WMVDMOEnc *enc)
{
  HRESULT hr;
  DMO_OUTPUT_DATA_BUFFER outbuf;
  GstFlowReturn ret = GST_FLOW_OK;

  DWORD outsize, alignment;
  enc->dmo->GetOutputSizeInfo (0, &outsize, &alignment);

  do {
    DWORD reserved = 0;
    BYTE *data;
    DWORD length, maxlength;
    GstBuffer *out = gst_buffer_new_and_alloc (outsize);

    memset (&outbuf, 0, sizeof(outbuf));

    /* Create an IMediaBuffer pointing at our GstBuffer.
       ::Create takes an extra ref to the buffer here, but we
       retain our initial ref - which we then give up in
       gst_pad_push () */
    outbuf.pBuffer = MediaBuffer::Create (out);
    outbuf.pBuffer->SetLength (0);

    outbuf.pBuffer->GetBufferAndLength(&data, &length);
    outbuf.pBuffer->GetMaxLength(&maxlength);

    hr = enc->dmo->ProcessOutput (DMO_PROCESS_OUTPUT_DISCARD_WHEN_NO_BUFFER,
            1, &outbuf, &reserved);

    if (hr == S_FALSE)
    {
      GST_LOG_OBJECT (enc, "Encoder did not produce any output");
      gst_buffer_unref (out);
      outbuf.pBuffer->Release();
      break;
    }

    if (FAILED (hr)) {
      GST_WARNING_OBJECT (enc, "Failed to process output buffer: %x", hr);
      gst_buffer_unref (out);
      outbuf.pBuffer->Release();
      return GST_FLOW_ERROR;
    }

    outbuf.pBuffer->GetBufferAndLength(&data, &length);

    if (data && length) {
      /* Mark keyframe/not keyframe appropriately */
      if (!(outbuf.dwStatus & DMO_OUTPUT_DATA_BUFFERF_SYNCPOINT))
        GST_BUFFER_FLAG_SET (out, GST_BUFFER_FLAG_DELTA_UNIT);

      /* Convert timestamps from REFERENCE_TIME (100ns units) to
         GstClockTime (ns units) */
      if (outbuf.dwStatus & DMO_OUTPUT_DATA_BUFFERF_TIME)
        GST_BUFFER_TIMESTAMP (out) = outbuf.rtTimestamp * 100;
      if (outbuf.dwStatus & DMO_OUTPUT_DATA_BUFFERF_TIMELENGTH)
        GST_BUFFER_DURATION (out) = outbuf.rtTimelength * 100;

      gst_buffer_set_caps (out, enc->output_caps);

      ret = gst_pad_push (enc->srcpad, out);
    }
    else {
      gst_buffer_unref (out);
      GST_DEBUG_OBJECT (enc, "Skipping: encoder output %p is length %d",
              data, length);
    }

    outbuf.pBuffer->Release();

  } while (ret == GST_FLOW_OK && 
          (outbuf.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE));

  return ret;
}

struct ChainData
{
  WMVDMOEnc *enc;
  GstBuffer *buf;
};

static void
wmvdmoenc_chain_task (void *data, void *ret)
{
  struct ChainData *cdata = (struct ChainData *)data;
  WMVDMOEnc *enc = cdata->enc;
  GstBuffer * buf = cdata->buf;
  GstFlowReturn *retval = (GstFlowReturn *)ret;

  HRESULT hr;
  REFERENCE_TIME timestamp = 0, duration = 0;
  DWORD flags = 0;

  flags = DMO_INPUT_DATA_BUFFERF_SYNCPOINT;

  if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_TIMESTAMP (buf))) {
    /* Convert to 100 ns units */
    flags = flags | DMO_INPUT_DATA_BUFFERF_TIME;
    timestamp = GST_BUFFER_TIMESTAMP (buf) / 100; 
  }

  if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_DURATION (buf))) {
    /* Convert to 100 ns units */
    flags = flags | DMO_INPUT_DATA_BUFFERF_TIMELENGTH;
    duration = GST_BUFFER_DURATION (buf) / 100; 
  }

  IMediaBuffer *inbuf = MediaBuffer::Create (buf);

  /* ::Create added a ref to the buffer, so we can unref it now */
  gst_buffer_unref (buf);

  hr = enc->dmo->ProcessInput (0, inbuf, flags, timestamp, duration);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Failed to process input buffer: %x", hr);
    inbuf->Release();
    *retval = GST_FLOW_ERROR;
    return;
  }

  inbuf->Release();

  *retval = wmvdmoenc_do_output (enc);
}

static void
wmvdmoenc_finish_stream_task (void *data, void *ret)
{
  WMVDMOEnc * enc = (WMVDMOEnc *)data;
  GstFlowReturn *retval = (GstFlowReturn *)ret;

  if (!enc->dmo) {
    GST_WARNING_OBJECT (enc, "Empty encoder object");
    *retval = GST_FLOW_OK;
    return;
  }

  HRESULT hr = enc->dmo->Discontinuity (0);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Failed in Discontinuity(): %x", hr);
    *retval = GST_FLOW_ERROR;
    return;
  }

  *retval = wmvdmoenc_do_output(enc);
  return;
}

static GstFlowReturn
wmvdmoenc_finish_stream (WMVDMOEnc * enc)
{
  GstFlowReturn ret;
  gst_comtaskthread_do_task (enc->comthread, wmvdmoenc_finish_stream_task,
          enc, &ret);
  return ret;
}

static GstFlowReturn
wmvdmoenc_chain (GstPad * pad, GstBuffer * buf)
{
  GstFlowReturn ret;
  WMVDMOEnc *enc = GST_WMVDMOENC (gst_pad_get_parent (pad));

  struct ChainData chain_data;
  chain_data.enc = enc;
  chain_data.buf = buf;

  gst_comtaskthread_do_task (enc->comthread, wmvdmoenc_chain_task, &chain_data,
          &ret);

  gst_object_unref (enc);

  return ret;
}

static gboolean
wmvdmoenc_sink_event (GstPad * pad, GstEvent * event)
{
  WMVDMOEnc *enc = GST_WMVDMOENC (gst_pad_get_parent (pad));
  gboolean res;
  GstFlowReturn ret;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_EOS:
      ret = wmvdmoenc_finish_stream (enc);
      if (ret != GST_FLOW_OK) {
        return FALSE;
      }
      wmvdmoenc_teardown (enc);

      res = gst_pad_push_event (enc->srcpad, event);
      break;
    default:
      res = gst_pad_push_event (enc->srcpad, event);
      break;
  }

  gst_object_unref (enc);

  return res;
}

static void
wmvdmoenc_finalize (GObject * obj)
{
  WMVDMOEnc *enc = GST_WMVDMOENC (obj);

  wmvdmoenc_teardown (enc);

  gst_comtaskthread_destroy (enc->comthread);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
wmvdmoenc_init (WMVDMOEnc * enc, WMVDMOEncClass *klass)
{
  enc->sinkpad =
      gst_pad_new_from_static_template (&wmvdmoenc_sink_template, "sink");

  gst_pad_set_setcaps_function (enc->sinkpad, wmvdmoenc_sink_setcaps);
  gst_pad_set_chain_function (enc->sinkpad, wmvdmoenc_chain);
  gst_pad_set_event_function (enc->sinkpad, wmvdmoenc_sink_event);

  gst_element_add_pad (GST_ELEMENT (enc), enc->sinkpad);

  enc->srcpad =
      gst_pad_new_from_static_template (&wmvdmoenc_src_template, "src");
  gst_element_add_pad (GST_ELEMENT (enc), enc->srcpad);

  enc->bitrate = DEFAULT_BITRATE;

  enc->comthread = gst_comtaskthread_init ();
}

static void
wmvdmoenc_set_property (GObject * obj, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  WMVDMOEnc *enc = GST_WMVDMOENC (obj);

  switch (prop_id) {
    case ARG_BITRATE:
      enc->bitrate = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
  }
}

static void
wmvdmoenc_get_property (GObject * obj, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  WMVDMOEnc *enc = GST_WMVDMOENC (obj);

  switch (prop_id) {
    case ARG_BITRATE:
      g_value_set_int (value, enc->bitrate);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
  }
}

static void
wmvdmoenc_class_init (WMVDMOEncClass * klass)
{
  GObjectClass *gobjectclass = (GObjectClass *) klass;
  parent_class = (GstElementClass *) g_type_class_peek_parent (klass);

  gobjectclass->finalize = wmvdmoenc_finalize;
  gobjectclass->set_property = wmvdmoenc_set_property;
  gobjectclass->get_property = wmvdmoenc_get_property;

  g_object_class_install_property (gobjectclass, ARG_BITRATE,
      g_param_spec_int ("bitrate", "Bitrate", "Bitrate to encode at (in bps)",
          0, 10000000, DEFAULT_BITRATE, (GParamFlags)G_PARAM_READWRITE));
}

static void
wmvdmoenc_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstElementDetails details;

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&wmvdmoenc_sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&wmvdmoenc_src_template));

  details.longname = "WMV DMO Encoder";
  details.klass = "Codec/Encoder/Audio";
  details.description = "WMV encoder wrapping native windows DMO";
  details.author = "Pioneers of the Inevitable <songbird@songbirdnest.com>";

  gst_element_class_set_details (element_class, &details);
}
