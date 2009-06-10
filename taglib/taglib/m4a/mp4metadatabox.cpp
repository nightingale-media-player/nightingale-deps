/***************************************************************************
copyright            : (C) 2009 Pioneers of the Inevitable
email                : songbird@songbirdnest.com
***************************************************************************/

/***************************************************************************
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Lesser General Public License version   *
*   2.1 as published by the Free Software Foundation.                     *
*                                                                         *
*   This library is distributed in the hope that it will be useful, but   *
*   WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
*   Lesser General Public License for more details.                       *
*                                                                         *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library; if not, write to the Free Software   *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
*   USA                                                                   *
*                                                                         *
*   Alternatively, this file is available under the Mozilla Public        *
*   License Version 1.1.  You may obtain a copy of the License at         *
*   http://www.mozilla.org/MPL/                                           *
***************************************************************************/

#include "mp4metadatabox.h"
#include "mp4metadataboxprivate.h"
#include "tbytevector.h"
#include "mp4isobox.h"
#include "tfile.h"

using namespace TagLib;

MP4::Mp4MetadataBox::Mp4MetadataBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset )
:Mp4IsoBox(file, fourcc, size, offset)
{
  d = new Mp4MetadataBoxPrivate;
  d->dataBox = 0;
}

MP4::Mp4MetadataBox::~Mp4MetadataBox()
{
  if (d->dataBox)
    delete d->dataBox;
  delete d;
}

ByteVector MP4::Mp4MetadataBox::render()
{
  // this renders text
  ITunesDataBox* databox = data();
  if (!databox)
  {
    // we lost our data!?
    return ByteVector();
  }

  ByteVector containedData;
  // the "data" box is a full box, serialize the version + flags
  containedData.append( ByteVector::fromUInt( databox->version() << 24 |
                                              databox->flags() ) );
  // there is an unknown four bytes; all samples have it set to be all zeros.
  // it appears to be some sort of encoding identifier in the cases of strings
  // where 0 is UTF-8, 1 is UTF-16be, and others are invalid.  This does not explain
  // what it means when it's not a string...
  containedData.append( ByteVector::fromUInt(0) );
  // and the real data
  containedData.append( databox->data() );
  
  ByteVector resultData;
  // start with the size of the metadata block
  uint dataSize = containedData.size() + 8; // size + fourcc for the 'data' box
  uint metaSize = dataSize + 8;  // size + fourcc for the metadata box
  resultData.append( ByteVector::fromUInt(metaSize) );
  resultData.append( ByteVector::fromUInt( fourcc() ) );
  resultData.append( ByteVector::fromUInt(dataSize) );
  resultData.append( ByteVector::fromUInt( databox->fourcc() ) );
  resultData.append( containedData );

  return resultData;
}

MP4::ITunesDataBox* MP4::Mp4MetadataBox::data() const
{
  return d->dataBox;
}

void MP4::Mp4MetadataBox::setData( MP4::ITunesDataBox* data )
{
  if (d->dataBox)
    delete d->dataBox;
  d->dataBox = data;
}
