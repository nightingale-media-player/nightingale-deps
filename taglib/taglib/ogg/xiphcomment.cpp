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

#include <tbytevector.h>
#include <tdebug.h>

#include <xiphcomment.h>

using namespace TagLib;

class Ogg::XiphComment::XiphCommentPrivate
{
public:
  FieldListMap fieldListMap;
  String vendorID;
  String commentField;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Ogg::XiphComment::XiphComment() : TagLib::Tag()
{
  d = new XiphCommentPrivate;
}

Ogg::XiphComment::XiphComment(const ByteVector &data) : TagLib::Tag()
{
  d = new XiphCommentPrivate;
  parse(data);
}

Ogg::XiphComment::~XiphComment()
{
  delete d;
}

/* Standard, no-brainer version of the get property function */
String Ogg::XiphComment::stringProperty(const String &property) const
{
  if(d->fieldListMap[property].isEmpty())
    return String::null;
  return d->fieldListMap[property].front();
}

String Ogg::XiphComment::title() const
{
  return stringProperty("TITLE");
}

String Ogg::XiphComment::artist() const
{
  return stringProperty("ARTIST");
}

String Ogg::XiphComment::albumArtist() const
{
  return stringProperty("ALBUMARTIST");
}

String Ogg::XiphComment::album() const
{
  return stringProperty("ALBUM");
}

String Ogg::XiphComment::comment() const
{
  if(!d->fieldListMap["DESCRIPTION"].isEmpty()) {
    d->commentField = "DESCRIPTION";
    return d->fieldListMap["DESCRIPTION"].front();
  }

  if(!d->fieldListMap["COMMENT"].isEmpty()) {
    d->commentField = "COMMENT";
    return d->fieldListMap["COMMENT"].front();
  }

  return String::null;
}

String Ogg::XiphComment::lyrics() const
{
  return stringProperty("LYRICS");
}

String Ogg::XiphComment::genre() const
{
  return stringProperty("GENRE");
}

String Ogg::XiphComment::producer() const
{
  return stringProperty("PRODUCER");
}

String Ogg::XiphComment::composer() const
{
  return stringProperty("COMPOSER");
}

String Ogg::XiphComment::conductor() const
{
  return stringProperty("CONDUCTOR");
}

String Ogg::XiphComment::lyricist() const
{
  return stringProperty("LYRICIST");
}

String Ogg::XiphComment::recordLabel() const
{
  return stringProperty("RECORDLABEL");
}

String Ogg::XiphComment::rating() const
{
  return stringProperty("RATING");
}

String Ogg::XiphComment::language() const
{
  return stringProperty("LANGUAGE");
}

String Ogg::XiphComment::key() const
{
  return stringProperty("KEY");
}

String Ogg::XiphComment::license() const
{
  return stringProperty("LICENSE");
}

String Ogg::XiphComment::licenseUrl() const
{
  return stringProperty("LICENSEURL");
}

TagLib::uint Ogg::XiphComment::year() const
{
  if(!d->fieldListMap["DATE"].isEmpty())
    return d->fieldListMap["DATE"].front().toInt();
  if(!d->fieldListMap["YEAR"].isEmpty())
    return d->fieldListMap["YEAR"].front().toInt();
  return 0;
}

TagLib::uint Ogg::XiphComment::track() const
{
  if(!d->fieldListMap["TRACKNUMBER"].isEmpty())
    return d->fieldListMap["TRACKNUMBER"].front().toInt();
  if(!d->fieldListMap["TRACKNUM"].isEmpty())
    return d->fieldListMap["TRACKNUM"].front().toInt();
  return 0;
}

List<TagLib::FlacPicture*> Ogg::XiphComment::artwork() const
{
  List<TagLib::FlacPicture*> artwork;
  
  StringList artworkList = d->fieldListMap["METADATA_BLOCK_PICTURE"];
  if(artworkList.isEmpty())
    return artwork;

  for (StringList::Iterator it = artworkList.begin();
       it != artworkList.end();
       ++it)
  {
    TagLib::FlacPicture *picture = new TagLib::FlacPicture();
    if (!picture->parse(*it))
    {
      delete picture;
      return artwork;
    }
    artwork.append(picture);
  }
  return artwork;
}

void Ogg::XiphComment::addArtwork(TagLib::FlacPicture &pic)
{
  ByteVector bv = pic.render(true);
  addField("METADATA_BLOCK_PICTURE", bv.data());
}

void Ogg::XiphComment::setArtwork(List<TagLib::FlacPicture*> artworkList)
{
  // first remove all artwork fields
  removeField("METADATA_BLOCK_PICTURE");

  // next add all artwork in the given list
  for (List<TagLib::FlacPicture*>::Iterator it = artworkList.begin();
       it != artworkList.end();
       ++it)
  {
    addArtwork(**it);
  }
}

TagLib::uint Ogg::XiphComment::totalTracks() const
{
  if(d->fieldListMap["TOTALTRACKS"].isEmpty())
    return 0;
  return d->fieldListMap["TOTALTRACKS"].front().toInt();
}

TagLib::uint Ogg::XiphComment::disc() const
{
  if(d->fieldListMap["DISCNUMBER"].isEmpty())
    return 0;
  return d->fieldListMap["DISCNUMBER"].front().toInt();
}

TagLib::uint Ogg::XiphComment::totalDiscs() const
{
  if(d->fieldListMap["TOTALDISCS"].isEmpty())
    return 0;
  return d->fieldListMap["TOTALDISCS"].front().toInt();
}

TagLib::uint Ogg::XiphComment::bpm() const
{
  if(d->fieldListMap["BPM"].isEmpty())
    return 0;
  return d->fieldListMap["BPM"].front().toInt();
}

bool Ogg::XiphComment::isCompilation() const
{
  // we only test for the *existence* of the COMPILATION field
  if(d->fieldListMap["COMPILATION"].isEmpty())
    return false;
  return true;
}

void Ogg::XiphComment::setTitle(const String &s)
{
  addField("TITLE", s);
}

void Ogg::XiphComment::setArtist(const String &s)
{
  addField("ARTIST", s);
}

void Ogg::XiphComment::setAlbumArtist(const String &s)
{
  addField("ALBUMARTIST", s);
}

void Ogg::XiphComment::setAlbum(const String &s)
{
  addField("ALBUM", s);
}

void Ogg::XiphComment::setComment(const String &s)
{
  addField(d->commentField.isEmpty() ? "DESCRIPTION" : d->commentField, s);
}

void Ogg::XiphComment::setLyrics(const String &s)
{
  addField("LYRICS", s);
}

void Ogg::XiphComment::setGenre(const String &s)
{
  addField("GENRE", s);
}

void Ogg::XiphComment::setProducer(const String &s)
{
  addField("PRODUCER", s);
}

void Ogg::XiphComment::setComposer(const String &s)
{
  addField("COMPOSER", s);
}

void Ogg::XiphComment::setConductor(const String &s)
{
  addField("CONDUCTOR", s);
}

void Ogg::XiphComment::setLyricist(const String &s)
{
  addField("LYRICIST", s);
}

void Ogg::XiphComment::setRecordLabel(const String &s)
{
  addField("RECORDLABEL", s);
}

void Ogg::XiphComment::setRating(const String &s)
{
  addField("RATING", s);
}

void Ogg::XiphComment::setLanguage(const String &s)
{
  addField("LANGUAGE", s);
}

void Ogg::XiphComment::setKey(const String &s)
{
  addField("KEY", s);
}

void Ogg::XiphComment::setLicense(const String &s)
{
  addField("LICENSE", s);
}

void Ogg::XiphComment::setLicenseUrl(const String &s)
{
  addField("LICENSEURL", s);
}

void Ogg::XiphComment::setYear(uint i)
{
  removeField("YEAR");
  if(i == 0)
    removeField("DATE");
  else
    addField("DATE", String::number(i));
}

void Ogg::XiphComment::setTrack(uint i)
{
  removeField("TRACKNUM");
  if(i == 0)
    removeField("TRACKNUMBER");
  else
    addField("TRACKNUMBER", String::number(i));
}

void Ogg::XiphComment::setTotalTracks(uint i)
{
  if(i == 0)
    removeField("TOTALTRACKS");
  else
    addField("TOTALTRACKS", String::number(i));
}

void Ogg::XiphComment::setDisc(uint i)
{
  if(i == 0)
    removeField("DISCNUMBER");
  else
    addField("DISCNUMBER", String::number(i));
}

void Ogg::XiphComment::setTotalDiscs(uint i)
{
  if(i == 0)
    removeField("TOTALDISCS");
  else
    addField("TOTALDISCS", String::number(i));
}

void Ogg::XiphComment::setBpm(uint i)
{
  if(i == 0)
    removeField("BPM");
  else
    addField("BPM", String::number(i));
}

void Ogg::XiphComment::setIsCompilation(bool i)
{
  if(i == false)
    removeField("COMPILATION");
  else
    addField("COMPILATION", "YES"); // TODO: check this
}


bool Ogg::XiphComment::isEmpty() const
{
  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it)
    if(!(*it).second.isEmpty())
      return false;

  return true;
}

TagLib::uint Ogg::XiphComment::fieldCount() const
{
  uint count = 0;

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it)
    count += (*it).second.size();

  return count;
}

const Ogg::FieldListMap &Ogg::XiphComment::fieldListMap() const
{
  return d->fieldListMap;
}

String Ogg::XiphComment::vendorID() const
{
  return d->vendorID;
}

void Ogg::XiphComment::addField(const String &key, const String &value, bool replace)
{
  if(replace)
    removeField(key.upper());

  if(!key.isEmpty() && !value.isEmpty())
    d->fieldListMap[key.upper()].append(value);
}

void Ogg::XiphComment::removeField(const String &key, const String &value)
{
  if(!value.isNull()) {
    StringList::Iterator it = d->fieldListMap[key].begin();
    while(it != d->fieldListMap[key].end()) {
      if(value == *it)
        it = d->fieldListMap[key].erase(it);
      else
        it++;
    }
  }
  else
    d->fieldListMap.erase(key);
}

bool Ogg::XiphComment::contains(const String &key) const
{
  return d->fieldListMap.contains(key) && !d->fieldListMap[key].isEmpty();
}

ByteVector Ogg::XiphComment::render() const
{
  return render(true);
}

ByteVector Ogg::XiphComment::render(bool addFramingBit) const
{
  ByteVector data;

  // Add the vendor ID length and the vendor ID.  It's important to use the
  // length of the data(String::UTF8) rather than the length of the the string
  // since this is UTF8 text and there may be more characters in the data than
  // in the UTF16 string.

  ByteVector vendorData = d->vendorID.data(String::UTF8);

  data.append(ByteVector::fromUInt(vendorData.size(), false));
  data.append(vendorData);

  // Add the number of fields.

  data.append(ByteVector::fromUInt(fieldCount(), false));

  // Iterate over the the field lists.  Our iterator returns a
  // std::pair<String, StringList> where the first String is the field name and
  // the StringList is the values associated with that field.

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it) {

    // And now iterate over the values of the current list.

    String fieldName = (*it).first;
    StringList values = (*it).second;

    StringList::ConstIterator valuesIt = values.begin();
    for(; valuesIt != values.end(); ++valuesIt) {
      ByteVector fieldData = fieldName.data(String::UTF8);
      fieldData.append('=');
      fieldData.append((*valuesIt).data(String::UTF8));

      data.append(ByteVector::fromUInt(fieldData.size(), false));
      data.append(fieldData);
    }
  }

  // Append the "framing bit".

  if(addFramingBit)
    data.append(char(1));

  return data;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void Ogg::XiphComment::parse(const ByteVector &data)
{
  // The first thing in the comment data is the vendor ID length, followed by a
  // UTF8 string with the vendor ID.

  int pos = 0;

  int vendorLength = data.mid(0, 4).toUInt(false);
  pos += 4;

  d->vendorID = String(data.mid(pos, vendorLength), String::UTF8);
  pos += vendorLength;

  // Next the number of fields in the comment vector.

  int commentFields = data.mid(pos, 4).toUInt(false);
  pos += 4;

  for(int i = 0; i < commentFields; i++) {

    // Each comment field is in the format "KEY=value" in a UTF8 string and has
    // 4 bytes before the text starts that gives the length.

    int commentLength = data.mid(pos, 4).toUInt(false);
    pos += 4;

    String comment = String(data.mid(pos, commentLength), String::UTF8);
    pos += commentLength;

    int commentSeparatorPosition = comment.find("=");

    String key = comment.substr(0, commentSeparatorPosition);
    String value = comment.substr(commentSeparatorPosition + 1);

    addField(key, value, false);
  }
}
