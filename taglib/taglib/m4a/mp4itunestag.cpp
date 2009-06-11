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

#include "mp4itunestag.h"

using namespace TagLib;

class MP4::Tag::TagPrivate
{
public:
  MP4::File*         mp4file;
  TagLib::String     title;
  TagLib::String     artist;
  TagLib::String     albumArtist;
  TagLib::String     album;
  TagLib::String     genre;
  TagLib::uint       year;
  TagLib::uint       track;
  TagLib::uint       totalTracks;
  TagLib::String     comment;
  TagLib::String     grouping;
  TagLib::String     composer;
  TagLib::uint       disc;
  TagLib::uint       totalDiscs;
  TagLib::uint       bpm;
  bool               isEmpty;
  TagLib::ByteVector cover;
};


MP4::Tag::Tag( )
{
  d = new TagPrivate();
  d->year         = 0;
  d->track        = 0;
  d->totalTracks  = 0;
  d->disc         = 0;
  d->totalDiscs   = 0;
  d->bpm          = 0;
  d->isEmpty      = true;
  d->cover        = ByteVector::null;
}

MP4::Tag::~Tag()
{
  delete d;
}

String MP4::Tag::title() const
{
  return d->title;
}

String MP4::Tag::artist() const
{
  return d->artist;
}

String MP4::Tag::albumArtist() const
{
  return d->albumArtist;
}

String MP4::Tag::album() const
{
  return d->album;
}

String MP4::Tag::comment() const
{
  return d->comment;
}

String MP4::Tag::genre() const
{
  return d->genre;
}

TagLib::uint MP4::Tag::year() const
{
  return d->year;
}

TagLib::uint MP4::Tag::track() const
{
  return d->track;
}

TagLib::uint MP4::Tag::totalTracks() const
{
  return d->totalTracks;
}

String MP4::Tag::grouping() const
{
  return d->grouping;
}

String MP4::Tag::composer() const
{
  return d->composer;
}

TagLib::uint MP4::Tag::disc() const
{
  return d->disc;
}

TagLib::uint MP4::Tag::totalDiscs() const
{
  return d->totalDiscs;
}

TagLib::uint MP4::Tag::bpm() const
{
  return d->bpm;
}

TagLib::ByteVector MP4::Tag::cover() const
{
  return d->cover;
}

void MP4::Tag::setTitle(const String &s)
{
  d->title = s;
  d->isEmpty = false;
}

void MP4::Tag::setArtist(const String &s)
{
  d->artist = s;
  d->isEmpty = false;
}

void MP4::Tag::setAlbumArtist(const String &s)
{
  d->albumArtist = s;
  d->isEmpty = false;
}

void MP4::Tag::setAlbum(const String &s)
{
  d->album = s;
  d->isEmpty = false;
}

void MP4::Tag::setComment(const String &s)
{
  d->comment = s;
  d->isEmpty = false;
}

void MP4::Tag::setGenre(const String &s)
{
  d->genre = s;
  d->isEmpty = false;
}

void MP4::Tag::setYear(const TagLib::uint i)
{
  d->year = i;
  d->isEmpty = false;
}

void MP4::Tag::setTrack(const TagLib::uint i)
{
  d->track = i;
  d->isEmpty = false;
}

void MP4::Tag::setTotalTracks(TagLib::uint i)
{
  d->totalTracks = i;
  d->isEmpty = false;
}

void MP4::Tag::setGrouping(const String &s)
{
  d->grouping = s;
  d->isEmpty = false;
}

void MP4::Tag::setComposer(const String &s)
{
  d->composer = s;
  d->isEmpty = false;
}

void MP4::Tag::setDisc(const TagLib::uint i)
{
  d->disc = i;
  d->isEmpty = false;
}

void MP4::Tag::setTotalDiscs(const TagLib::uint i)
{
  d->totalDiscs = i;
  d->isEmpty = false;
}

void MP4::Tag::setBpm(const TagLib::uint i)
{
  d->bpm = i;
  d->isEmpty = false;
}

void MP4::Tag::setCover(const TagLib::ByteVector& c)
{
  d->cover = c;
  d->isEmpty = false;
}

bool MP4::Tag::isEmpty() const
{
  return d->isEmpty;
}

