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

#include <iostream>
#include "itunesaartbox.h"
#include "itunesdatabox.h"
#include "mp4isobox.h"
#include "mp4file.h"
#include "tfile.h"
#include "mp4tagsproxy.h"

using namespace TagLib;

class MP4::ITunesAArtBox::ITunesAArtBoxPrivate
{
public:
  ITunesDataBox* dataBox;
};

MP4::ITunesAArtBox::ITunesAArtBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset )
	:Mp4IsoBox(file, fourcc, size, offset)
{
  d = new MP4::ITunesAArtBox::ITunesAArtBoxPrivate();
  d->dataBox = 0;
}

MP4::ITunesAArtBox::~ITunesAArtBox()
{
  if( d->dataBox != 0 )
    delete d->dataBox;
  delete d;
}

//! parse the content of the box
void MP4::ITunesAArtBox::parse()
{
  TagLib::MP4::File* mp4file = static_cast<MP4::File*>( file() );

  // parse data box
  TagLib::uint size;
  MP4::Fourcc  fourcc;

  if(mp4file->readSizeAndType( size, fourcc ) == true)
  {
    // check for type - must be 'data'
    if( fourcc != MP4::Fourcc("data") )
    {
      std::cerr << "bad atom in itunes tag - skipping it." << std::endl;
      // jump over data tag
      mp4file->seek( size-8, TagLib::File::Current );
      return;
    }
    d->dataBox = new ITunesDataBox( mp4file, fourcc, size, mp4file->tell() );
    d->dataBox->parsebox();
  }
  else
  {
    // reading unsuccessful - serious error!
    std::cerr << "Error in parsing ITunesAArtBox - serious Error in taglib!" << std::endl;
    return;
  }
  // register data box
  mp4file->tagProxy()->registerBox( Mp4TagsProxy::albumartist, d->dataBox );

#if 0
  // get data pointer - just for debuging...
  TagLib::String dataString( d->dataBox->data() );
  std::cout << "Content of artist box: " << dataString << std::endl;
#endif
}

