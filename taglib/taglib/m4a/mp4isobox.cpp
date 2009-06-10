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

#include "mp4isobox.h"
#include "tfile.h"

using namespace TagLib;

class MP4::Mp4IsoBox::Mp4IsoBoxPrivate
{
public:
  MP4::Fourcc   fourcc;
  TagLib::uint  size;
  long          offset;
  TagLib::File* file;
};

MP4::Mp4IsoBox::Mp4IsoBox( TagLib::File* file, MP4::Fourcc fourcc, TagLib::uint size, long offset )
{
  d = new MP4::Mp4IsoBox::Mp4IsoBoxPrivate();
  d->file   = file;
  d->fourcc = fourcc;
  d->size   = size;
  d->offset = offset;
}

MP4::Mp4IsoBox::~Mp4IsoBox()
{
  delete d;
}

void MP4::Mp4IsoBox::parsebox()
{
  // seek to offset
  file()->seek( offset(), File::Beginning );
  // simply call parse method of sub class
  parse();
}

ByteVector MP4::Mp4IsoBox::render()
{
  if ( size() == 0 )
  {
    // optimization (avoid seek) for boxes that are not on disk
    return ByteVector();
  }
  // seek to offset, minus header size
  // offset() excludes size and fourcc
  file()->seek( offset() - 8, File::Beginning );
  return file()->readBlock( size() );
}

MP4::Fourcc MP4::Mp4IsoBox::fourcc() const
{
  return d->fourcc;
}

TagLib::uint MP4::Mp4IsoBox::size() const
{
  return d->size;
}

long MP4::Mp4IsoBox::offset() const
{
  return d->offset;
}

MP4::Mp4IsoBox* MP4::Mp4IsoBox::getChildBox( MP4::Fourcc fourcc, Mp4IsoBox* offset ) const
{
  return NULL;
}

TagLib::File* MP4::Mp4IsoBox::file() const
{
  return d->file;
}

//! set the size of this box
//  NOTE: this causes the in-memory data to be out of sync with the disk data
//  @param size The new size
void MP4::Mp4IsoBox::setSize( ulonglong size )
{
  d->size = size;
}
