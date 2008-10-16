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

#include "mp4isofullbox.h"
#include "tfile.h"

using namespace TagLib;

class MP4::Mp4IsoFullBox::Mp4IsoFullBoxPrivate
{
public:
	uchar version;
	uint  flags;
}; // Mp4IsoFullBoxPrivate


MP4::Mp4IsoFullBox::Mp4IsoFullBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset )
: Mp4IsoBox( file, fourcc, size, offset )
{
  d = new MP4::Mp4IsoFullBox::Mp4IsoFullBoxPrivate();
}

MP4::Mp4IsoFullBox::~Mp4IsoFullBox()
{
  delete d;
}

void  MP4::Mp4IsoFullBox::parsebox()
{
  // seek to offset
  Mp4IsoBox::file()->seek(Mp4IsoBox::offset(), File::Beginning );
  // parse version and flags
  ByteVector version_flags = Mp4IsoBox::file()->readBlock(4);
  d->version = version_flags[0];
  d->flags   = version_flags[1] << 16 || version_flags[2] << 8 || version_flags[3];
  // call parse method of subclass
  parse();
}

TagLib::uchar MP4::Mp4IsoFullBox::version()
{
  return d->version;
}

TagLib::uint  MP4::Mp4IsoFullBox::flags()
{
  return d->flags;
}

