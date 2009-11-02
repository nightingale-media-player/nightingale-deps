/* GStreamer
 * Copyright (C) 2009 Pioneers of the Inevitable <songbird@songbirdnest.com>
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

#include <windows.h>
#include <dmo.h>
#include <wmcodecdsp.h>
#include <uuids.h>
#include <wmsdk.h>

#include <gst/gst.h>
#include <gst/riff/riff-media.h>

#include "comtaskthread.h"

#define GST_CAT_DEFAULT wmadmoenc_debug
GST_DEBUG_CATEGORY_STATIC (wmadmoenc_debug);

#define GST_TYPE_WMADMOENC \
      (wmadmoenc_get_type ())
#define GST_WMADMOENC(obj) \
      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_WMADMOENC, WMADMOEnc))
#define GST_WMADMOENC_CLASS(klass) \
      (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_WMADMOENC, WMADMOEncClass))
#define GST_IS_WMADMOENC(obj) \
      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_WMADMOENC))
#define GST_IS_WMADMOENC_CLASS(klass) \
      (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_WMADMOENC))

#define DEFAULT_BITRATE 128000

/* IMediaBuffer implementation, based on sample implementation in docs */
class MediaBuffer : public IMediaBuffer
{
private:
  GstBuffer   *m_buf;
  const DWORD  m_maxlength;
  LONG         m_refcount;

  MediaBuffer(GstBuffer *buf) :
      m_buf(buf),
      m_maxlength(GST_BUFFER_SIZE (buf)),
      m_refcount(1)
  {
    gst_buffer_ref (m_buf);
  }

  virtual ~MediaBuffer()
  {
    gst_buffer_unref (m_buf);
  }

public:

  // Function to create a new IMediaBuffer object and return 
  // an AddRef'd interface pointer.
  static IMediaBuffer * Create(GstBuffer *buf)
  {
    IMediaBuffer *buffer = new MediaBuffer (buf);
    return buffer;
  }

  // IUnknown methods.
  STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
  {
    if (ppv == NULL) 
    {
      return E_POINTER;
    }
    else if (IsEqualGUID (riid, IID_IMediaBuffer) ||
             IsEqualGUID (riid, IID_IUnknown))
    {
      *ppv = static_cast<IMediaBuffer *>(this);
      AddRef();
      return S_OK;
    }
    else
    {
      *ppv = NULL;
      return E_NOINTERFACE;
    }
  }

  STDMETHODIMP_(ULONG) AddRef()
  {
    return InterlockedIncrement(&m_refcount);
  }

  STDMETHODIMP_(ULONG) Release()
  {
    LONG lRef = InterlockedDecrement(&m_refcount);
    if (lRef == 0) 
    {
      delete this;
      // m_cRef is no longer valid! Return lRef.
    }
    return lRef;  
  }

  // IMediaBuffer methods.
  STDMETHODIMP SetLength(DWORD cbLength)
  {
    if (cbLength > m_maxlength) 
    {
      return E_INVALIDARG;
    }
    GST_BUFFER_SIZE (m_buf) = cbLength;
    return S_OK;
  }

  STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength)
  {
    if (pcbMaxLength == NULL) 
    {
      return E_POINTER;
    }
    *pcbMaxLength = m_maxlength;
    return S_OK;
  }

  STDMETHODIMP GetBufferAndLength(BYTE **ppbBuffer, DWORD *pcbLength)
  {
    // Either parameter can be NULL, but not both.
    if (ppbBuffer == NULL && pcbLength == NULL) 
    {
      return E_POINTER;
    }
    if (ppbBuffer) 
    {
      *ppbBuffer = GST_BUFFER_DATA (m_buf);
    }
    if (pcbLength) 
    {
      *pcbLength = GST_BUFFER_SIZE (m_buf);
    }
    return S_OK;
  }
};

/* GObject properties */
enum
{
  ARG_0,
  ARG_BITRATE
};

static GstStaticPadTemplate wmadmoenc_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "depth = (int)16, "
        "width = (int)16, "
        "endianness = (int)" G_STRINGIFY (G_BYTE_ORDER) ", "
        "signed = (boolean)TRUE, "
        "channels = (int) [1,2], " "rate = (int)[1, MAX]"));

static GstStaticPadTemplate wmadmoenc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-wma, "
	"channels = (int) [1,2], "
	"rate = (int)[1, MAX], " 
	"wmaversion = (int)2, "
    "block_align = (int)[1, MAX], "
	"bitrate = (int)[0, MAX]"));

typedef struct _WMADMOEncClass
{
  GstElementClass parent_class;
} WMADMOEncClass;

typedef struct _WMADMOEnc
{
  GstElement parent;

  GstPad *sinkpad;
  GstPad *srcpad;

  gboolean is_setup;

  /* Output format */
  GstCaps *output_caps;

  /* From received caps */
  int rate;
  int channels;

  /* Bitrate: chosen via properties, and the actual value from the encoder */
  int selected_bitrate;
  int actual_bitrate;

  /* COM thread helper. */
  GstCOMTaskThread *comthread;

  /* All the following objects MUST only be used from the COM thread */

  /* The actual encoder object */
  IMediaObject *dmo;

  /* DMO format descriptions */
  DMO_MEDIA_TYPE input_format;
  DMO_MEDIA_TYPE output_format;

} WMADMOEnc;

GST_BOILERPLATE (WMADMOEnc, wmadmoenc, GstElement, GST_TYPE_ELEMENT);

static GstCaps *
wmadmoenc_caps_from_format (DMO_MEDIA_TYPE *fmt)
{
  GstBuffer *extradata = NULL;

  if (IsEqualGUID (fmt->formattype, FORMAT_WaveFormatEx) && 
	fmt->cbFormat >= sizeof (WAVEFORMATEX))
  {
    GstCaps *caps;
    WAVEFORMATEX *wave = (WAVEFORMATEX *)fmt->pbFormat;

    /* cbSize is the size of any codec-specific extra data; get it (as a 
       GstBuffer) if there is any */
    if (wave->cbSize) {
      extradata = gst_buffer_new_and_alloc (wave->cbSize);
      memcpy (GST_BUFFER_DATA (extradata),
              (guint8 *)wave + sizeof (WAVEFORMATEX), wave->cbSize);
    }

    caps = gst_riff_create_audio_caps (wave->wFormatTag, NULL,
        (gst_riff_strf_auds *) wave, extradata, NULL, NULL);

    if (extradata)
      gst_buffer_unref (extradata);

    return caps;
  }
  else {
    return NULL;
  }
}

static gboolean
wmadmoenc_set_input_format (WMADMOEnc * enc)
{
  HRESULT hr;
  WAVEFORMATEX *wave;

  memset (&enc->input_format, 0, sizeof (enc->input_format));
  hr = MoInitMediaType (&enc->input_format, sizeof (WAVEFORMATEX));

  if (FAILED (hr))
    return FALSE;

  enc->input_format.majortype = WMMEDIATYPE_Audio;
  enc->input_format.subtype = WMMEDIASUBTYPE_PCM;
  enc->input_format.formattype = FORMAT_WaveFormatEx;

  wave = (WAVEFORMATEX *) enc->input_format.pbFormat;

  wave->wFormatTag = WAVE_FORMAT_PCM;
  wave->nChannels = enc->channels;
  wave->nSamplesPerSec = enc->rate;
  wave->nAvgBytesPerSec = 2 * enc->channels * enc->rate;
  wave->nBlockAlign = 2 * enc->channels;
  wave->wBitsPerSample = 16;
  wave->cbSize = 0;

  hr = enc->dmo->SetInputType (0, &enc->input_format, 0);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Couldn't set input format: %x", hr);
    return FALSE;
  }

  return TRUE;
}

static gboolean
wmadmoenc_set_output_format (WMADMOEnc * enc)
{
  DMO_MEDIA_TYPE mt;
  /* For now, we only do WMA2 - we could easily extend this to WMAv3 
     (aka WMA9 Pro) and WMA lossless. */
  GUID subtype = WMMEDIASUBTYPE_WMAudioV2;
  int i;
  int best_br_diff = -1;
  int best_format = -1;
  int best_br = -1;
  HRESULT hr;

  for (i = 0; SUCCEEDED (enc->dmo->GetOutputType (0, i, &mt)); i++) {
    if (IsEqualGUID (mt.majortype, WMMEDIATYPE_Audio) &&
        IsEqualGUID (mt.subtype, subtype)) 
    {
      WAVEFORMATEX *wave;
      if (mt.cbFormat >= sizeof (WAVEFORMATEX)) {
        wave = (WAVEFORMATEX *) mt.pbFormat;
        if (wave->nChannels == enc->channels &&
            wave->nSamplesPerSec == enc->rate &&
            wave->wBitsPerSample == 16)
        {
           int br_diff = ABS (enc->selected_bitrate - 
                   (int)(wave->nAvgBytesPerSec * 8));
           if (best_br_diff < 0 || br_diff < best_br_diff)
           {
             best_br_diff = br_diff;
             best_format = i;
             best_br = wave->nAvgBytesPerSec * 8;
           }
        }
      }
    }

    MoFreeMediaType (&mt);
  }

  if (best_format >= 0) {
    GST_INFO_OBJECT (enc, "Selected format %d for output", best_format);

    hr = enc->dmo->GetOutputType (0, best_format, &mt);
    if (FAILED (hr))
      return FALSE;

    hr = enc->dmo->SetOutputType (0, &mt, 0);
    if (FAILED (hr)) {
      GST_WARNING_OBJECT (enc, "Could not set output type: %x", hr);
      return FALSE;
    }

    MoCopyMediaType (&enc->output_format, &mt);
    MoFreeMediaType (&mt);

    enc->actual_bitrate = best_br;

    return TRUE;
  }
  else {
    GST_WARNING_OBJECT (enc, "Couldn't find a compatible output format");
    return FALSE;
  }
}

static void
wmadmoenc_setup_task (void *data, void *ret)
{
  WMADMOEnc *enc = (WMADMOEnc *)data;
  gboolean *retval = (gboolean *)ret;
  HRESULT hr;

  hr = CoCreateInstance (CLSID_CWMAEncMediaObject, NULL, CLSCTX_INPROC,
      IID_IMediaObject, (void **) &enc->dmo);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Could not create DMO Encoder object: %x", hr);
    *retval = FALSE;
    return;
  }

  if (!wmadmoenc_set_output_format (enc))
  {
    GST_WARNING_OBJECT (enc, "Could not set up output format");
    *retval = FALSE;
    return;
  }

  if (!wmadmoenc_set_input_format (enc))
  {
    GST_WARNING_OBJECT (enc, "Could not set up input format");
    *retval = FALSE;
    return;
  }

  enc->output_caps = wmadmoenc_caps_from_format (&enc->output_format);

  gst_pad_set_caps (enc->srcpad, enc->output_caps);

  enc->is_setup = TRUE;
  *retval = TRUE;
  return;
}

static gboolean
wmadmoenc_setup (WMADMOEnc * enc)
{
  gboolean ret;
  gst_comtaskthread_do_task (enc->comthread, wmadmoenc_setup_task, enc, &ret);
  return ret;
}

static void
wmadmoenc_teardown_task (void *data, void *ret)
{
  WMADMOEnc * enc = (WMADMOEnc *)data;

  MoFreeMediaType (&enc->input_format);
  memset (&enc->input_format, 0, sizeof (enc->input_format));

  MoFreeMediaType (&enc->output_format);
  memset (&enc->output_format, 0, sizeof (enc->output_format));

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
wmadmoenc_teardown (WMADMOEnc * enc)
{
  gst_comtaskthread_do_task (enc->comthread, wmadmoenc_teardown_task, enc,
          NULL);
}

static gboolean
wmadmoenc_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  WMADMOEnc *enc = GST_WMADMOENC (gst_pad_get_parent (pad));
  GstStructure *structure;
  gboolean ret;

  structure = gst_caps_get_structure (caps, 0);
  gst_structure_get_int (structure, "channels", &enc->channels);
  gst_structure_get_int (structure, "rate", &enc->rate);

  if (enc->is_setup)
    wmadmoenc_teardown (enc);
  ret = wmadmoenc_setup (enc);

  gst_object_unref (enc);

  return ret;
}

static GstFlowReturn
wmadmoenc_do_output (WMADMOEnc *enc)
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
  WMADMOEnc *enc;
  GstBuffer *buf;
};

static void
wmadmoenc_chain_task (void *data, void *ret)
{
  struct ChainData *cdata = (struct ChainData *)data;
  WMADMOEnc *enc = cdata->enc;
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

  *retval = wmadmoenc_do_output (enc);
}

static void
wmadmoenc_finish_stream_task (void *data, void *ret)
{
  WMADMOEnc * enc = (WMADMOEnc *)data;
  GstFlowReturn *retval = (GstFlowReturn *)ret;

  HRESULT hr = enc->dmo->Discontinuity (0);
  if (FAILED (hr)) {
    GST_WARNING_OBJECT (enc, "Failed in Discontinuity(): %x", hr);
    *retval = GST_FLOW_ERROR;
    return;
  }

  *retval = wmadmoenc_do_output(enc);
  return;
}

static GstFlowReturn
wmadmoenc_finish_stream (WMADMOEnc * enc)
{
  GstFlowReturn ret;
  gst_comtaskthread_do_task (enc->comthread, wmadmoenc_finish_stream_task,
          enc, &ret);
  return ret;
}

static GstFlowReturn
wmadmoenc_chain (GstPad * pad, GstBuffer * buf)
{
  GstFlowReturn ret;
  WMADMOEnc *enc = GST_WMADMOENC (gst_pad_get_parent (pad));

  struct ChainData chain_data;
  chain_data.enc = enc;
  chain_data.buf = buf;

  gst_comtaskthread_do_task (enc->comthread, wmadmoenc_chain_task, &chain_data,
          &ret);

  gst_object_unref (enc);

  return ret;
}

static gboolean
wmadmoenc_sink_event (GstPad * pad, GstEvent * event)
{
  WMADMOEnc *enc = GST_WMADMOENC (gst_pad_get_parent (pad));
  gboolean res;
  GstFlowReturn ret;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_EOS:
      ret = wmadmoenc_finish_stream (enc);
      if (ret != GST_FLOW_OK) {
        return FALSE;
      }
      wmadmoenc_teardown (enc);

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
wmadmoenc_finalize (GObject * obj)
{
  WMADMOEnc *enc = GST_WMADMOENC (obj);

  wmadmoenc_teardown (enc);

  gst_comtaskthread_destroy (enc->comthread);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
wmadmoenc_init (WMADMOEnc * enc, WMADMOEncClass *klass)
{
  enc->sinkpad =
      gst_pad_new_from_static_template (&wmadmoenc_sink_template, "sink");

  gst_pad_set_setcaps_function (enc->sinkpad, wmadmoenc_sink_setcaps);
  gst_pad_set_chain_function (enc->sinkpad, wmadmoenc_chain);
  gst_pad_set_event_function (enc->sinkpad, wmadmoenc_sink_event);

  gst_element_add_pad (GST_ELEMENT (enc), enc->sinkpad);

  enc->srcpad =
      gst_pad_new_from_static_template (&wmadmoenc_src_template, "src");
  gst_element_add_pad (GST_ELEMENT (enc), enc->srcpad);

  enc->selected_bitrate = DEFAULT_BITRATE;

  enc->comthread = gst_comtaskthread_init ();
}

static void
wmadmoenc_set_property (GObject * obj, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  WMADMOEnc *enc = GST_WMADMOENC (obj);

  switch (prop_id) {
    case ARG_BITRATE:
      enc->selected_bitrate = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
  }
}

static void
wmadmoenc_get_property (GObject * obj, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  WMADMOEnc *enc = GST_WMADMOENC (obj);

  switch (prop_id) {
    case ARG_BITRATE:
      g_value_set_int (value, enc->selected_bitrate);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
  }
}

static void
wmadmoenc_class_init (WMADMOEncClass * klass)
{
  GObjectClass *gobjectclass = (GObjectClass *) klass;
  parent_class = (GstElementClass *) g_type_class_peek_parent (klass);

  gobjectclass->finalize = wmadmoenc_finalize;
  gobjectclass->set_property = wmadmoenc_set_property;
  gobjectclass->get_property = wmadmoenc_get_property;

  g_object_class_install_property (gobjectclass, ARG_BITRATE,
      g_param_spec_int ("bitrate", "Bitrate", "Bitrate to encode at (in bps)",
          0, 1000000, DEFAULT_BITRATE, (GParamFlags)G_PARAM_READWRITE));
}

static void
wmadmoenc_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstElementDetails details;

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&wmadmoenc_sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&wmadmoenc_src_template));

  details.longname = "WMA DMO Encoder";
  details.klass = "Codec/Encoder/Audio";
  details.description = "WMA encoder wrapping native windows DMO";
  details.author = "Pioneers of the Inevitable <songbird@songbirdnest.com>";

  gst_element_class_set_details (element_class, &details);
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (wmadmoenc_debug, "wmadmoenc", 0, "WMA DMO Encoder");

  return gst_element_register (plugin, "wmadmoenc", GST_RANK_NONE,
      wmadmoenc_get_type ());
}

extern "C" {

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR,
    "wmadmoenc",
    "WMA DMO Encoder", plugin_init, VERSION, "LGPL", "GStreamer",
    "http://gstreamer.net/")
}


