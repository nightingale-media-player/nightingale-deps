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

#include "mp4tagsproxy.h"
#include "itunesdatabox.h"

using namespace TagLib;

class MP4::Mp4TagsProxy::Mp4TagsProxyPrivate
{
public:
  ITunesDataBox* titleData;
  ITunesDataBox* artistData;
  ITunesDataBox* albumArtistData;
  ITunesDataBox* albumData;
  ITunesDataBox* coverData;
  ITunesDataBox* genreData;
  ITunesDataBox* yearData;
  ITunesDataBox* trknData;
  ITunesDataBox* commentData;
  ITunesDataBox* groupingData;
  ITunesDataBox* composerData;
  ITunesDataBox* diskData;
  ITunesDataBox* bpmData;
};

MP4::Mp4TagsProxy::Mp4TagsProxy()
{
  d = new MP4::Mp4TagsProxy::Mp4TagsProxyPrivate();
  d->titleData       = 0;
  d->artistData      = 0;
  d->albumArtistData = 0;
  d->albumData       = 0;
  d->coverData       = 0;
  d->genreData       = 0;
  d->yearData        = 0;
  d->trknData        = 0;
  d->commentData     = 0;
  d->groupingData    = 0;
  d->composerData    = 0;
  d->diskData        = 0;
  d->bpmData         = 0;
}

MP4::Mp4TagsProxy::~Mp4TagsProxy()
{
  delete d;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::titleData() const
{
  return d->titleData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::artistData() const
{
  return d->artistData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::albumArtistData() const
{
  return d->albumArtistData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::albumData() const
{
  return d->albumData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::genreData() const
{
  return d->genreData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::yearData() const
{
  return d->yearData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::trknData() const
{
  return d->trknData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::commentData() const
{
  return d->commentData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::groupingData() const
{
  return d->groupingData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::composerData() const
{
  return d->composerData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::diskData() const
{
  return d->diskData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::bpmData() const
{
  return d->bpmData;
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::coverData() const
{
  return d->coverData;
}

void MP4::Mp4TagsProxy::registerBox( EBoxType boxtype, ITunesDataBox* databox )
{
  switch( boxtype )
  {
    case title:
      d->titleData = databox;
      break;
    case artist:
      d->artistData = databox;
      break;
    case albumartist:
      d->albumArtistData = databox;
      break;
    case album:
      d->albumData = databox;
      break;
    case cover:
      d->coverData = databox;
      break;
    case genre:
      d->genreData = databox;
      break;
    case year:
      d->yearData = databox;
      break;
    case trackno:
      d->trknData = databox;
      break;
    case comment:
      d->commentData = databox;
      break;
    case grouping:
      d->groupingData = databox;
      break;
    case composer:
      d->composerData = databox;
      break;
    case disk:
      d->diskData = databox;
      break;
    case bpm:
      d->bpmData = databox;
      break;
  }
}

MP4::ITunesDataBox* MP4::Mp4TagsProxy::getDataByType( EBoxType boxtype )
{
  switch( boxtype )
  {
  case title:
    return d->titleData;
  case artist:
    return d->artistData;
  case album:
    return d->albumData;
  case cover:
    return d->coverData;
  case genre:
    return d->genreData;
  case year:
    return d->yearData;
  case trackno:
    return d->trknData;
  case comment:
    return d->commentData;
  case grouping:
    return d->groupingData;
  case composer:
    return d->composerData;
  case disk:
    return d->diskData;
  case bpm:
    return d->bpmData;
  }
  return NULL;
}

MP4::Fourcc MP4::Mp4TagsProxy::getFourccForType( EBoxType boxtype )
{
  switch( boxtype )
  {
  case title:
    return TAGLIB_FOURCC(0xA9,'n','a','m');
  case artist:
    return TAGLIB_FOURCC(0xA9,'A','R','T');
  case album:
    return TAGLIB_FOURCC(0xA9,'a','l','b');
  case cover:
    return TAGLIB_FOURCC('c','o','v','r');
  case genre:
    return TAGLIB_FOURCC('g','n','r','e');
  case gen:
    return TAGLIB_FOURCC(0xA9,'g','e','n');
  case year:
    return TAGLIB_FOURCC(0xA9,'d','a','y');
  case trackno:
    return TAGLIB_FOURCC('t','r','k','n');
  case comment:
    return TAGLIB_FOURCC(0xA9,'c','m','t');
  case grouping:
    return TAGLIB_FOURCC(0xA9,'g','r','p');
  case composer:
    return TAGLIB_FOURCC(0xA9,'w','r','t');
  case disk:
    return TAGLIB_FOURCC('d','i','s','k');
  case bpm:
    return TAGLIB_FOURCC('t','m','p','o');
  }
  return Fourcc();
}
