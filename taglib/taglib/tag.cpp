/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
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

#include "tag.h"

using namespace TagLib;

class Tag::TagPrivate
{

};

Tag::Tag()
{

}

Tag::~Tag()
{

}

bool Tag::isEmpty() const
{
  return (title().isEmpty() &&
          artist().isEmpty() &&
          albumArtist().isEmpty() &&
          album().isEmpty() &&
          comment().isEmpty() &&
          lyrics().isEmpty() &&
          genre().isEmpty() &&
          producer().isEmpty() &&
          composer().isEmpty() &&
          conductor().isEmpty() &&
          lyricist().isEmpty() &&
          recordLabel().isEmpty() &&
          rating().isEmpty() &&
          language().isEmpty() &&
          key().isEmpty() &&
          license().isEmpty() &&
          licenseUrl().isEmpty() &&
          year() == 0 &&
          track() == 0 &&
          totalTracks() == 0 &&
          disc() == 0 &&
          totalDiscs() == 0 &&
          bpm() == 0 &&
          !isCompilation());
}

void Tag::duplicate(const Tag *source, Tag *target, bool overwrite) // static
{
  if(overwrite) {
    target->setTitle(source->title());
    target->setArtist(source->artist());
    target->setAlbumArtist(source->albumArtist());
    target->setAlbum(source->album());
    target->setComment(source->comment());
    target->setLyrics(source->lyrics());
    target->setGenre(source->genre());
    target->setProducer(source->producer());
    target->setComposer(source->composer());
    target->setConductor(source->conductor());
    target->setLyricist(source->lyricist());
    target->setRecordLabel(source->recordLabel());
    target->setRating(source->rating());
    target->setLanguage(source->language());
    target->setKey(source->key());
    target->setLicense(source->license());
    target->setLicenseUrl(source->licenseUrl());
    target->setYear(source->year());
    target->setTrack(source->track());
    target->setTotalTracks(source->totalTracks());
    target->setDisc(source->disc());
    target->setTotalDiscs(source->totalDiscs());
    target->setBpm(source->bpm());
  }
  else {
    if(target->title().isEmpty())
      target->setTitle(source->title());
    if(target->artist().isEmpty())
      target->setArtist(source->artist());
    if(target->albumArtist().isEmpty())
      target->setAlbumArtist(source->albumArtist());
    if(target->album().isEmpty())
      target->setAlbum(source->album());
    if(target->comment().isEmpty())
      target->setComment(source->comment());
    if(target->lyrics().isEmpty())
      target->setLyrics(source->lyrics());
    if(target->genre().isEmpty())
      target->setGenre(source->genre());
    if(target->producer().isEmpty())
      target->setProducer(source->producer());
    if(target->composer().isEmpty())
      target->setConductor(source->conductor());
    if(target->lyricist().isEmpty())
      target->setLyricist(source->lyricist());
    if(target->recordLabel().isEmpty())
      target->setRecordLabel(source->recordLabel());
    if(target->rating().isEmpty())
      target->setRating(source->rating());
    if(target->language().isEmpty())
      target->setLanguage(source->language());
    if(target->key().isEmpty())
      target->setKey(source->key());
    if(target->license().isEmpty())
      target->setLicense(source->license());
    if(target->licenseUrl().isEmpty())
      target->setLicenseUrl(source->licenseUrl());
    if(target->year() <= 0)
      target->setYear(source->year());
    if(target->track() <= 0)
      target->setTrack(source->track());
    if(target->totalTracks() <= 0)
      target->setTotalTracks(source->totalTracks());
    if(target->disc() <= 0)
      target->setDisc(source->disc());
    if(target->totalDiscs() <= 0)
      target->setTotalDiscs(source->totalDiscs());
    if(target->bpm() <= 0)
      target->setBpm(source->bpm());
  }
}

String Tag::splitNumberRender(uint first, uint second)
{
  String firstString = first ? String::number(first) : String("");
  String secondString = second ? String::number(second) : String("");
  return firstString + String("/") + secondString;
}
