/***************************************************************************
    copyright            : (C) 2002-2008 by Jochen Issing
    email                : jochen.issing@isign-softart.de
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

#include "mp4sampleentry.h"
#include "mp4isobox.h"
#include "mp4file.h"

using namespace TagLib;

class MP4::Mp4SampleEntry::Mp4SampleEntryPrivate
{
public:
  TagLib::uint data_reference_index;
};

MP4::Mp4SampleEntry::Mp4SampleEntry( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset )
	:Mp4IsoBox(file, fourcc, size, offset)
{
  d = new MP4::Mp4SampleEntry::Mp4SampleEntryPrivate();
}

MP4::Mp4SampleEntry::~Mp4SampleEntry()
{
  delete d;
}

//! parse the content of the box
void MP4::Mp4SampleEntry::parse()
{
  TagLib::MP4::File* mp4file = dynamic_cast<TagLib::MP4::File*>(file());
  if(!mp4file)
    return;

  // skip the first 6 bytes
  mp4file->seek( 6, TagLib::File::Current );
  // read data reference index
  if(!mp4file->readShort( d->data_reference_index))
    return;
  parseEntry();
}

