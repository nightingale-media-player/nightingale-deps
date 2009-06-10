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

#include "tlist.h"
#include <iostream>
#include "mp4minfbox.h"
#include "mp4stblbox.h"
#include "boxfactory.h"
#include "mp4file.h"
#include "mp4containerboxprivate.h"

using namespace TagLib;

class MP4::Mp4MinfBox::Mp4MinfBoxPrivate
{
public:
  //! stores the handler type of the current trak
  MP4::Fourcc            handler_type;
}; // class Mp4MinfBoxPrivate

MP4::Mp4MinfBox::Mp4MinfBox( TagLib::File* file, MP4::Fourcc fourcc, TagLib::uint size, long offset )
	: Mp4ContainerBox( file, fourcc, size, offset )
{
  d = new MP4::Mp4MinfBox::Mp4MinfBoxPrivate();
}

MP4::Mp4MinfBox::~Mp4MinfBox()
{
  delete d;
}

void MP4::Mp4MinfBox::setHandlerType( MP4::Fourcc fourcc )
{
  d->handler_type = fourcc;
}

void MP4::Mp4MinfBox::parse()
{
  TagLib::MP4::File* mp4file = static_cast<MP4::File*>( file() );

  TagLib::uint totalsize = 8;
  // parse all contained boxes
  TagLib::uint size;
  MP4::Fourcc  fourcc;

  while( (mp4file->readSizeAndType( size, fourcc ) == true)  )
  {
    totalsize += size;

    // check for errors
    if( totalsize > MP4::Mp4IsoBox::size() )
    {
      std::cerr << "Error in mp4 file " << mp4file->name() << " minf box contains bad box with name: " << fourcc.toString() << std::endl;
      return;
    }

    // create the appropriate subclass and parse it
    MP4::Mp4IsoBox* curbox = MP4::BoxFactory::createInstance( mp4file, fourcc, size, mp4file->tell() );
    if( fourcc == TAGLIB_FOURCC('s','t','b','l') )
    {
      // cast to hdlr box
      Mp4StblBox* stblbox = dynamic_cast<Mp4StblBox*>( curbox );
      if(!stblbox)
	return;
      // set handler type
      stblbox->setHandlerType( d->handler_type );
    }

    curbox->parsebox();
    Mp4ContainerBox::d->boxes.append( curbox );

    // check for end of minf box
    if( totalsize == MP4::Mp4IsoBox::size() )
      break;
  }
}
