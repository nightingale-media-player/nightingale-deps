/***************************************************************************
copyright            : (C) 2009 by Pioneers of the Inevitable
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

#include "tdebug.h"
#include "tlist.h"
#include "mp4containerbox.h"
#include "mp4containerboxprivate.h"

using namespace TagLib;

MP4::Mp4ContainerBox::Mp4ContainerBox( TagLib::File* file, MP4::Fourcc fourcc, TagLib::uint size, long offset )
: Mp4IsoBox( file, fourcc, size, offset )
{
  d = new MP4::Mp4ContainerBox::Mp4ContainerBoxPrivate();
}

MP4::Mp4ContainerBox::~Mp4ContainerBox()
{
  TagLib::List<Mp4IsoBox*>::Iterator delIter;
  for( delIter  = d->boxes.begin();
    delIter != d->boxes.end();
    ++delIter )
  {
    delete *delIter;
  }
  delete d;
}

MP4::Mp4IsoBox* MP4::Mp4ContainerBox::getChildBox( MP4::Fourcc fourcc, Mp4IsoBox* offset ) const
{
  TagLib::List<Mp4IsoBox*>::Iterator iter = d->boxes.begin();
  if ( offset )
  {
    for ( ; *iter != offset; ++iter )
      if ( iter == d->boxes.end() )
        return NULL;
    ++iter; // go past the offset
  }
  for ( ; iter != d->boxes.end(); ++iter )
  {
    if ( !fourcc || fourcc == (*iter)->fourcc() )
      return *iter;
  }
  return NULL;
}
