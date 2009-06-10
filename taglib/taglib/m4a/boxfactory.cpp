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
#include "tstring.h"
#include "boxfactory.h"
#include "mp4skipbox.h"
#include "mp4moovbox.h"
#include "mp4mvhdbox.h"
#include "mp4trakbox.h"
#include "mp4mdiabox.h"
#include "mp4minfbox.h"
#include "mp4stblbox.h"
#include "mp4stsdbox.h"
#include "mp4hdlrbox.h"
#include "mp4udtabox.h"
#include "mp4metabox.h"
#include "mp4ilstbox.h"
#include "itunesnambox.h"
#include "itunesartbox.h"
#include "itunesaartbox.h"
#include "itunesalbbox.h"
#include "itunesgenbox.h"
#include "itunesdaybox.h"
#include "itunestrknbox.h"
#include "itunescmtbox.h"
#include "itunesgrpbox.h"
#include "ituneswrtbox.h"
#include "itunesdiskbox.h"
#include "itunestmpobox.h"
#include "itunescvrbox.h"
#include "itunesdatabox.h"

using namespace TagLib;

//! factory function
MP4::Mp4IsoBox* MP4::BoxFactory::createInstance( TagLib::File* anyfile, MP4::Fourcc fourcc, uint size, long offset )
{
  MP4::File * file = dynamic_cast<MP4::File *>(anyfile);
  if(!file)
    return 0;

  //std::cout << "creating box for: " << fourcc.toString() << std::endl;

  switch( fourcc )
  {
  case TAGLIB_FOURCC('m','o','o','v'):
    return new MP4::Mp4MoovBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('m','v','h','d'):
    return new MP4::Mp4MvhdBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('t','r','a','k'):
    return new MP4::Mp4TrakBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('m','d','i','a'):
    return new MP4::Mp4MdiaBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('m','i','n','f'):
    return new MP4::Mp4MinfBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('s','t','b','l'):
    return new MP4::Mp4StblBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('s','t','s','d'):
    return new MP4::Mp4StsdBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('h','d','l','r'):
    return new MP4::Mp4HdlrBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('u','d','t','a'):
    return new MP4::Mp4UdtaBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('m','e','t','a'):
    return (MP4::Mp4IsoFullBox*)new MP4::Mp4MetaBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('i','l','s','t'):
    return new MP4::Mp4IlstBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'n','a','m'):
    return new MP4::ITunesNamBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'A','R','T'):
    return new MP4::ITunesArtBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('a','A','R','T'):
    return new MP4::ITunesAArtBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'a','l','b'):
    return new MP4::ITunesAlbBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'g','e','n'):
    return new MP4::ITunesGenBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('g','n','r','e'):
    return new MP4::ITunesGenBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'d','a','y'):
    return new MP4::ITunesDayBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('t','r','k','n'):
    return new MP4::ITunesTrknBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'c','m','t'):
    return new MP4::ITunesCmtBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'g','r','p'):
    return new MP4::ITunesGrpBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC(0xA9,'w','r','t'):
    return new MP4::ITunesWrtBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('d','i','s','k'):
    return new MP4::ITunesDiskBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('t','m','p','o'):
    return new MP4::ITunesTmpoBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('c','o','v','r'):
    return new MP4::ITunesCvrBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('d','a','t','a'):
    return new MP4::ITunesDataBox( file, fourcc, size, offset );
    break;
  case TAGLIB_FOURCC('f','r','e','e'): // 'free'
  case TAGLIB_FOURCC('s','k','i','p'): // 'skip'
    return new MP4::Mp4SkipBox( file, fourcc, size, offset );
    break;
  default:
    return new MP4::Mp4UnknownBox( file, fourcc, size, offset );
  }
}
