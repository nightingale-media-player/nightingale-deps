/***************************************************************************
    copyright            : (C) 2004 by Allan Sandfeld Jensen
    email                : kde@carewolf.com
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

#ifdef __SUNPRO_CC
// Sun Studio finds multiple specializations of Map because
// it considers specializations with and without class types
// to be different; this define forces Map to use only the
// specialization with the class keyword.
#define WANT_CLASS_INSTANTIATION_OF_MAP (1)
#endif

#include <tfile.h>
#include <tstring.h>
#include <tmap.h>

#include "apetag.h"
#include "apefooter.h"
#include "apeitem.h"

using namespace TagLib;
using namespace APE;

class APE::Tag::TagPrivate
{
public:
  TagPrivate() : file(0), footerLocation(-1), tagLength(0),
  track(0), totalTracks(0), disc(0), totalDiscs(0) {}

  File *file;
  long footerLocation;
  long tagLength;

  Footer footer;
  
  uint track;
  uint totalTracks;
  uint disc;
  uint totalDiscs;

  ItemListMap itemListMap;
};

////////////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////////////

APE::Tag::Tag() : TagLib::Tag()
{
  d = new TagPrivate;
}

APE::Tag::Tag(File *file, long footerLocation) : TagLib::Tag()
{
  d = new TagPrivate;
  d->file = file;
  d->footerLocation = footerLocation;

  read();
}

APE::Tag::~Tag()
{
  delete d;
}

ByteVector APE::Tag::fileIdentifier()
{
  return ByteVector::fromCString("APETAGEX");
}

String APE::Tag::stringItem(const String &item) const
{
  if(d->itemListMap[item].isEmpty())
    return String::null;
  return d->itemListMap[item].toString();  
}

String APE::Tag::title() const
{
  return stringItem("TITLE");
}

String APE::Tag::artist() const
{
  return stringItem("ARTIST");
}

String APE::Tag::albumArtist() const
{
  return stringItem("ALBUMARTIST");
}

String APE::Tag::album() const
{
  return stringItem("ALBUM");
}

String APE::Tag::comment() const
{
  return stringItem("COMMENT");
}

String APE::Tag::lyrics() const
{
  return stringItem("LYRICS");
}

String APE::Tag::genre() const
{
  return stringItem("GENRE");
}

String APE::Tag::producer() const
{
  return stringItem("PRODUCER");
}

String APE::Tag::composer() const
{
  return stringItem("COMPOSER");
}

String APE::Tag::conductor() const
{
  return stringItem("CONDUCTOR");
}

String APE::Tag::lyricist() const
{
  return stringItem("LYRICIST");
}

String APE::Tag::recordLabel() const
{
  return stringItem("RECORDLABEL");
}

String APE::Tag::rating() const
{
  return stringItem("RATING");
}

String APE::Tag::language() const
{
  return stringItem("LANGUAGE");
}

String APE::Tag::key() const
{
  return stringItem("KEY");
}

String APE::Tag::license() const
{
  return stringItem("LICENSE");
}

String APE::Tag::licenseUrl() const
{
  return stringItem("LICENSEURL");
}

TagLib::uint APE::Tag::year() const
{
  if(d->itemListMap["YEAR"].isEmpty())
    return 0;
  return d->itemListMap["YEAR"].toString().toInt();
}


// TODO: probably ought to not require people to read these values
//       prior to being able to store them.
TagLib::uint APE::Tag::track() const
{
  if(d->itemListMap["TRACK"].isEmpty())
    return 0;

  String trackDetails = d->itemListMap["TRACK"].toString();

  uint value = 0;
  bool isNumber = true;

  for(String::ConstIterator charIt = trackDetails.begin();
      isNumber && charIt != trackDetails.end();
      ++charIt)
  {
      isNumber = *charIt >= '0' && *charIt <= '9';
      if (isNumber) {
        value = value * 10 + (*charIt - '0');
      }
  }
  
  d->track = value;

  return value;
}

TagLib::uint APE::Tag::totalTracks() const
{
  if(d->itemListMap["TRACK"].isEmpty())
    return 0;  
  
  String trackDetails = d->itemListMap["TRACK"].toString();

  uint value = 0;
  bool isNumber = true;
  bool reachedSecondNumber = false;
  bool foundSeparator = false;

  for(String::ConstIterator charIt = trackDetails.begin();
      (!reachedSecondNumber || isNumber) && charIt != trackDetails.end();
      ++charIt)
  {
      isNumber = *charIt >= '0' && *charIt <= '9';
      if (isNumber && foundSeparator) {
        reachedSecondNumber = true;
      }
      else if (!isNumber) {
        foundSeparator = true;
      }
      
      if (reachedSecondNumber && isNumber) {
        value = value * 10 + (*charIt - '0');
      }
  }

  d->totalTracks = value;

  return value;
}

TagLib::uint APE::Tag::disc() const
{
  if(d->itemListMap["DISC"].isEmpty())
    return 0;

  String trackDetails = d->itemListMap["DISC"].toString();

  uint value = 0;
  bool isNumber = true;
  for(String::ConstIterator charIt = trackDetails.begin();
      isNumber && charIt != trackDetails.end();
      ++charIt)
  {
      isNumber = *charIt >= '0' && *charIt <= '9';
      if (isNumber) {
        value = value * 10 + (*charIt - '0');
      }
  }
  
  d->disc = value;

  return value;
}

TagLib::uint APE::Tag::totalDiscs() const
{
  if(d->itemListMap["DISC"].isEmpty())
    return 0;  
  
  String trackDetails = d->itemListMap["DISC"].toString();

  uint value = 0;
  bool isNumber = true;
  bool reachedSecondNumber = false;
  bool foundSeparator = false;

  for(String::ConstIterator charIt = trackDetails.begin();
      (!reachedSecondNumber || isNumber) && charIt != trackDetails.end();
      ++charIt)
  {
      isNumber = *charIt >= '0' && *charIt <= '9';
      if (isNumber && foundSeparator) {
        reachedSecondNumber = true;
      }
      else if (!isNumber) {
        foundSeparator = true;
      }
      
      if (reachedSecondNumber && isNumber) {
        value = value * 10 + (*charIt - '0');
      }
  }

  d->totalDiscs = value;

  return value;
}

TagLib::uint APE::Tag::bpm() const
{
  if(d->itemListMap["BPM"].isEmpty())
    return 0;
  return d->itemListMap["BPM"].toString().toInt();
}

bool APE::Tag::isCompilation() const
{
  if(d->itemListMap["COMPILATION"].isEmpty())
    return false;
  return d->itemListMap["COMPILATION"].toString() == "true";
}

void APE::Tag::setTitle(const String &s)
{
  addValue("TITLE", s, true);
}

void APE::Tag::setArtist(const String &s)
{
  addValue("ARTIST", s, true);
}

void APE::Tag::setAlbumArtist(const String &s)
{
  addValue("ALBUMARTIST", s, true);
}

void APE::Tag::setAlbum(const String &s)
{
  addValue("ALBUM", s, true);
}

void APE::Tag::setComment(const String &s)
{
  addValue("COMMENT", s, true);
}

void APE::Tag::setLyrics(const String &s)
{
  addValue("LYRICS", s, true);
}

void APE::Tag::setGenre(const String &s)
{
  addValue("GENRE", s, true);
}

void APE::Tag::setProducer(const String &s)
{
  addValue("PRODUCER", s, true);
}

void APE::Tag::setComposer(const String &s)
{
  addValue("COMPOSER", s, true);
}

void APE::Tag::setConductor(const String &s)
{
  addValue("CONDUCTOR", s, true);
}

void APE::Tag::setLyricist(const String &s)
{
  addValue("LYRICIST", s, true);
}

void APE::Tag::setRecordLabel(const String &s)
{
  addValue("RECORDLABEL", s, true);
}

void APE::Tag::setRating(const String &s)
{
  addValue("RATING", s, true);
}

void APE::Tag::setLanguage(const String &s)
{
  addValue("LANGUAGE", s, true);
}

void APE::Tag::setKey(const String &s)
{
  addValue("KEY", s, true);
}

void APE::Tag::setLicense(const String &s)
{
  addValue("LICENSE", s, true);
}

void APE::Tag::setLicenseUrl(const String &s)
{
  addValue("LICENSEURL", s, true);
}

void APE::Tag::setYear(uint i)
{
  if(i <= 0)
    removeItem("YEAR");
  else
    addValue("YEAR", String::number(i), true);
}

void APE::Tag::setTrack(uint i)
{
  if(i <= 0 && d->totalTracks == 0) {
    removeItem("TRACK");
    return;
  }
  
  d->track = i;
  addValue("TRACK", Tag::splitNumberRender(i, d->totalTracks));
}

void APE::Tag::setTotalTracks(uint i)
{
  if(i <= 0 && d->track == 0) {
    removeItem("TRACK");
    return;
  }
  
  d->totalTracks = i;
  addValue("TRACK", Tag::splitNumberRender(d->track, i));
}

void APE::Tag::setDisc(uint i)
{
  if(i <= 0 && d->totalDiscs == 0) {
    removeItem("DISC");
    return;
  }
  
  d->disc = i;
  addValue("DISC", Tag::splitNumberRender(i, d->totalDiscs));
}

void APE::Tag::setTotalDiscs(uint i)
{
  if(i <= 0 && d->disc == 0) {
    removeItem("DISC");
    return;
  }
  
  addValue("DISC", Tag::splitNumberRender(d->disc, i));
}

void APE::Tag::setBpm(uint i)
{
  if(i <= 0)
    removeItem("BPM");
  else
    addValue("BPM", String::number(i), true);
}

void APE::Tag::setIsCompilation(bool i)
{
  if(!i)
    removeItem("COMPILATION");
  else
    addValue("COMPILATION", "true", true);
}

APE::Footer *APE::Tag::footer() const
{
  return &d->footer;
}

const APE::ItemListMap& APE::Tag::itemListMap() const
{
  return d->itemListMap;
}

void APE::Tag::removeItem(const String &key)
{
  Map<const String, Item>::Iterator it = d->itemListMap.find(key.upper());
  if(it != d->itemListMap.end())
    d->itemListMap.erase(it);
}

void APE::Tag::addValue(const String &key, const String &value, bool replace)
{
  if(replace)
    removeItem(key);
  if(!value.isEmpty()) {
    if(d->itemListMap.contains(key) || !replace)
      d->itemListMap[key.upper()].appendValue(value);
    else
      setItem(key, Item(key, value));
  }
}

void APE::Tag::setItem(const String &key, const Item &item)
{
  d->itemListMap.insert(key.upper(), item);
}

////////////////////////////////////////////////////////////////////////////////
// protected methods
////////////////////////////////////////////////////////////////////////////////

void APE::Tag::read()
{
  if(d->file && d->file->isValid()) {

    d->file->seek(d->footerLocation);
    d->footer.setData(d->file->readBlock(Footer::size()));

    if(d->footer.tagSize() <= Footer::size() ||
       d->footer.tagSize() > uint(d->file->length()))
      return;

    d->file->seek(d->footerLocation + Footer::size() - d->footer.tagSize());
    parse(d->file->readBlock(d->footer.tagSize() - Footer::size()));
  }
}

ByteVector APE::Tag::render() const
{
  ByteVector data;
  uint itemCount = 0;

  {
    for(Map<const String, Item>::ConstIterator it = d->itemListMap.begin();
        it != d->itemListMap.end(); ++it)
    {
      data.append(it->second.render());
      itemCount++;
    }
  }

  d->footer.setItemCount(itemCount);
  d->footer.setTagSize(data.size() + Footer::size());
  d->footer.setHeaderPresent(true);

  return d->footer.renderHeader() + data + d->footer.renderFooter();
}

void APE::Tag::parse(const ByteVector &data)
{
  uint pos = 0;

  // 11 bytes is the minimum size for an APE item

  for(uint i = 0; i < d->footer.itemCount() && pos <= data.size() - 11; i++) {
    APE::Item item;
    item.parse(data.mid(pos));

    d->itemListMap.insert(item.key().upper(), item);

    pos += item.size();
  }
}
