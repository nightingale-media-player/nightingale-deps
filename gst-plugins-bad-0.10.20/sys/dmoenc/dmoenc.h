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

#define GST_CAT_DEFAULT dmoenc_debug
GST_DEBUG_CATEGORY_EXTERN (dmoenc_debug);

GType wmadmoenc_get_type (void);
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

GType wmvdmoenc_get_type (void);
#define GST_TYPE_WMVDMOENC \
      (wmvdmoenc_get_type ())
#define GST_WMVDMOENC(obj) \
      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_WMVDMOENC, WMVDMOEnc))
#define GST_WMVDMOENC_CLASS(klass) \
      (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_WMVDMOENC, WMVDMOEncClass))
#define GST_IS_WMVDMOENC(obj) \
      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_WMVDMOENC))
#define GST_IS_WMVDMOENC_CLASS(klass) \
      (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_WMVDMOENC))

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

