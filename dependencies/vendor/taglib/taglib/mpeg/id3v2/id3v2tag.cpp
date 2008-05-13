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

#include <tfile.h>
#include <tdebug.h>

#include "id3v2tag.h"
#include "id3v2header.h"
#include "id3v2extendedheader.h"
#include "id3v2footer.h"
#include "id3v2synchdata.h"

#include "id3v1genres.h"

#include "frames/textidentificationframe.h"
#include "frames/unsynchronizedlyricsframe.h"
#include "frames/commentsframe.h"

using namespace TagLib;
using namespace ID3v2;

class ID3v2::Tag::TagPrivate
{
public:
  TagPrivate() : file(0), tagOffset(-1), extendedHeader(0), footer(0), paddingSize(0),
                 track(0), totalTracks(0), disc(0), totalDiscs(0)
  {
    frameList.setAutoDelete(true);
  }
  ~TagPrivate()
  {
    delete extendedHeader;
    delete footer;
  }

  File *file;
  long tagOffset;
  const FrameFactory *factory;

  Header header;
  ExtendedHeader *extendedHeader;
  Footer *footer;

  int paddingSize;

  FrameListMap frameListMap;
  FrameList frameList;
  
  uint track;
  uint totalTracks;
  uint disc;
  uint totalDiscs;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

ID3v2::Tag::Tag() : TagLib::Tag()
{
  d = new TagPrivate;
  d->factory = FrameFactory::instance();
}

ID3v2::Tag::Tag(File *file, long tagOffset, const FrameFactory *factory) :
  TagLib::Tag()
{
  d = new TagPrivate;

  d->file = file;
  d->tagOffset = tagOffset;
  d->factory = factory;

  read();
}

ID3v2::Tag::~Tag()
{
  delete d;
}

/* Standard, no-brainer version of the get property function */
String ID3v2::Tag::getTextFrame(const String &property) const
{
  if(!d->frameListMap[property.data(String::UTF8)].isEmpty())
    return d->frameListMap[property.data(String::UTF8)].front()->toString();
  return String::null;
}

String ID3v2::Tag::title() const
{
  return getTextFrame("TIT2");
}

String ID3v2::Tag::artist() const
{
  return getTextFrame("TPE1");
}

String ID3v2::Tag::albumArtist() const
{
  return getTextFrame("TPE2");
}

String ID3v2::Tag::album() const
{
  return getTextFrame("TALB");
}

String ID3v2::Tag::comment() const
{
  const FrameList &comments = d->frameListMap["COMM"];

  if(comments.isEmpty())
    return String::null;

  for(FrameList::ConstIterator it = comments.begin(); it != comments.end(); ++it)
  {
    if(static_cast<CommentsFrame *>(*it)->description().isEmpty())
      return (*it)->toString();
  }

  return comments.front()->toString();
}

String ID3v2::Tag::lyrics() const
{
  if(!d->frameListMap["USLT"].isEmpty())
    return (static_cast<ID3v2::UnsynchronizedLyricsFrame *>(d->frameListMap["USLT"].front())->text());
  return String::null;
}

String ID3v2::Tag::genre() const
{
  // TODO: In the next major version (TagLib 2.0) a list of multiple genres
  // should be separated by " / " instead of " ".  For the moment to keep
  // the behavior the same as released versions it is being left with " ".

  if(d->frameListMap["TCON"].isEmpty() || !(d->frameListMap["TCON"].front()))
  {
    return String::null;
  }

  // ID3v2.4 lists genres as the fields in its frames field list.  If the field
  // is simply a number it can be assumed that it is an ID3v1 genre number.
  // Here was assume that if an ID3v1 string is present that it should be
  // appended to the genre string.  Multiple fields will be appended as the
  // string is built.

  TextIdentificationFrame *f = static_cast<TextIdentificationFrame *>(
    d->frameListMap["TCON"].front());

  StringList fields = f->fieldList();

  StringList genres;

  for(StringList::Iterator it = fields.begin(); it != fields.end(); ++it) {

    bool isNumber = true;

    for(String::ConstIterator charIt = (*it).begin();
        isNumber && charIt != (*it).end();
        ++charIt)
    {
      isNumber = *charIt >= '0' && *charIt <= '9';
    }

    if(isNumber) {
      int number = (*it).toInt();
      if(number >= 0 && number <= 255)
        *it = ID3v1::genre(number);
    }

    if(std::find(genres.begin(), genres.end(), *it) == genres.end())
      genres.append(*it);
  }

  return genres.toString();
}

String ID3v2::Tag::producer() const
{
  /*
   * ID3v2.4 lists "Involved People" (non-performers) in an alternating list of
   * role/person, role/person.
   * ID3v2.3 mooshes everyone together into IPLS (involved person list)
   */
  
  /*
   * DISABLED: See bug 9086.
   *
   * 
  if(!d->frameListMap["TIPL"].isEmpty())
  {
    printf("TIPL\n");
    TextIdentificationFrame *f = static_cast<TextIdentificationFrame *>(
      d->frameListMap["TIPL"].front());
  
    StringList fields = f->fieldList();
    // loop increments twice to skip involved-person names and only check roles.
    // you know, just in case someone named "Producer" is your recording tech.
    for(StringList::Iterator it = fields.begin(); it != fields.end(); ++it) {
      
      StringList::Iterator next = it;
      if (it != fields.end()) { next++; }
      printf("\tTIPL: %s=>%s\n", *it, *next);
      
      if((*it).upper() == String("PRODUCER")) {
        return *(++it); // the next field will be the producer
      }
      // don't run over the end, but do skip over the names
      else if (it != fields.end()) {
          ++it;
      }
    }
  }
 */

  /*
   * Now try the ID3v2.3 "IPLS" frame.
   */
  /*
   * DISABLED: See bug 9086.
   *
   * 
  
  if(!d->frameListMap["IPLS"].isEmpty())
  {
    printf("IPLS\n");
    TextIdentificationFrame *f = static_cast<TextIdentificationFrame *>(
      d->frameListMap["IPLS"].front());
    printf("IPLSFRAME\n");
    StringList fields = f->fieldList();
    printf("IPLSFIELDLIST\n");
    // loop increments twice to skip involved-person names and only check roles.
    // you know, just in case someone named "Producer" is your recording tech.
    for(StringList::Iterator it = fields.begin(); it != fields.end(); ++it) {
      StringList::Iterator next = it;
      if (it != fields.end()) { next++; }
      printf("\tIPLS: %s=>%s\n", *it, *next);
      
      if((*it).upper() == String("PRODUCER")) {
        return *(++it); // the next field will be the producer
      }
      // don't run over the end, but do skip over the names
      else if (it != fields.end()) {
          ++it;
      }
    }
  }
  */
  /*
   * Give up. We can't find it.
   */
  return String::null;
}

String ID3v2::Tag::composer() const
{
  return getTextFrame("TCOM");
}

String ID3v2::Tag::conductor() const
{
  return getTextFrame("TPE3");
}

String ID3v2::Tag::lyricist() const
{
  return getTextFrame("TEXT");
}

String ID3v2::Tag::recordLabel() const
{
  return getTextFrame("TPUB");
}

String ID3v2::Tag::rating() const
{
  /* TODO: SEE BUG 9084
   * should use a TXXX field and a TextInformationFrame
   * or Popularimeter (which is not implemented in TagLib and is insane)
   */
  
  return String::null;
}

String ID3v2::Tag::language() const
{
  return getTextFrame("TLAN");
}

String ID3v2::Tag::key() const
{
  return getTextFrame("TKEY");
}

String ID3v2::Tag::license() const
{
  return getTextFrame("TCOP");
}

String ID3v2::Tag::licenseUrl() const
{
  return getTextFrame("WCOP");
}

TagLib::uint ID3v2::Tag::year() const
{
  if(!d->frameListMap["TDRC"].isEmpty())
    return d->frameListMap["TDRC"].front()->toString().substr(0, 4).toInt();
  return 0;
}

// TODO: probably ought to not require people to read these values
//       prior to being able to store them.
TagLib::uint ID3v2::Tag::track() const
{
  if(d->frameListMap["TRCK"].isEmpty())
    return 0;

  String trackDetails = d->frameListMap["TRCK"].front()->toString();

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

TagLib::uint ID3v2::Tag::totalTracks() const
{
  if(d->frameListMap["TRCK"].isEmpty())
    return 0;  
  
  String trackDetails = d->frameListMap["TRCK"].front()->toString();

  uint value = 0;
  bool isNumber = true;
  bool reachedSecondNumber = false;
  bool foundSeparator = false;

  // TODO: right now we treat " 33" as a totalTracks number
  //       but not as a track number. this might be ungood.
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

TagLib::uint ID3v2::Tag::disc() const
{
  if(d->frameListMap["TPOS"].isEmpty())
    return 0;

  String trackDetails = d->frameListMap["TPOS"].front()->toString();

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

TagLib::uint ID3v2::Tag::totalDiscs() const
{
  if(d->frameListMap["TPOS"].isEmpty())
    return 0;  
  
  String trackDetails = d->frameListMap["TPOS"].front()->toString();

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

TagLib::uint ID3v2::Tag::bpm() const
{
  if(!d->frameListMap["TBPM"].isEmpty())
    return d->frameListMap["TBPM"].front()->toString().toInt();
  return 0;
}

// TODO: weak
bool ID3v2::Tag::isCompilation() const
{
  if(!d->frameListMap["TCMP"].isEmpty())
    return (d->frameListMap["TCMP"].front()->toString() == "true");
  return false;
}

void ID3v2::Tag::setTitle(const String &s)
{
  setTextFrame("TIT2", s);
}

void ID3v2::Tag::setArtist(const String &s)
{
  setTextFrame("TPE1", s);
}

void ID3v2::Tag::setAlbumArtist(const String &s)
{
  setTextFrame("TPE2", s);
}

void ID3v2::Tag::setAlbum(const String &s)
{
  setTextFrame("TALB", s);
}

void ID3v2::Tag::setComment(const String &s)
{
  if(s.isEmpty()) {
    removeFrames("COMM");
    return;
  }

  if(!d->frameListMap["COMM"].isEmpty())
    d->frameListMap["COMM"].front()->setText(s);
  else {
    CommentsFrame *f = new CommentsFrame(d->factory->defaultTextEncoding());
    addFrame(f);
    f->setText(s);
  }
}

void ID3v2::Tag::setLyrics(const String &s)
{
  if(s.isEmpty()) {
    removeFrames("USLT");
    return;
  }

  if(!d->frameListMap["USLT"].isEmpty())
    d->frameListMap["USLT"].front()->setText(s);
  else {
    UnsynchronizedLyricsFrame *f = new UnsynchronizedLyricsFrame(d->factory->defaultTextEncoding());
    addFrame(f);
    f->setText(s);
  }
}

// TODO: this is (oddly) no good either
void ID3v2::Tag::setGenre(const String &s)
{
  if(s.isEmpty()) {
    removeFrames("TCON");
    return;
  }

  // iTunes can't handle correctly encoded ID3v2.4 numerical genres.  Just use
  // strings until iTunes sucks less.

#ifdef NO_ITUNES_HACKS

  int index = ID3v1::genreIndex(s);

  if(index != 255)
    setTextFrame("TCON", String::number(index));
  else
    setTextFrame("TCON", s);

#else

  setTextFrame("TCON", s);

#endif
}

void ID3v2::Tag::setProducer(const String &s)
{
  /*
   * DISABLED: SEE BUG 9086
   * 
  if(d->frameListMap["TIPL"].isEmpty())
  {
    return String::null;
  }

  // ID3v2.4 lists "Involved People" (non-performers) in an alternating list of
  // role/person, role/person.
  // here we look up the producer in the first TIPL frame.

  TextIdentificationFrame *f = static_cast<TextIdentificationFrame *>(
    d->frameListMap["TIPL"].front());

  StringList fields = f->fieldList();
  // loop increments twice to skip involved-person names and only check roles.
  // you know, just in case someone named "Producer" is your recording tech.
  for(StringList::Iterator it = fields.begin(); it != fields.end(); ++it, ++it) {
    if(*it.upper() == String("PRODUCER")) {
      ++it; // the next field will be the producer
    }
  }

  // i guess we didn't find it.
  return String::null;
  */

  // no, don't do that, that's ridiculous.  
  //setTextFrame("TIPL", s);
}


void ID3v2::Tag::setComposer(const String &s)
{
  setTextFrame("TCOM", s);
}

void ID3v2::Tag::setConductor(const String &s)
{
  setTextFrame("TPE3", s);
}

void ID3v2::Tag::setLyricist(const String &s)
{
  setTextFrame("TEXT", s);
}

void ID3v2::Tag::setRecordLabel(const String &s)
{
  setTextFrame("TPUB", s);
}

void ID3v2::Tag::setRating(const String &s)
{
  /* TODO: SEE BUG 9084
   * should use a TXXX field and a TextInformationFrame
   * or Popularimeter (which is not implemented in TagLib and is insane)
   */
  return;
}

void ID3v2::Tag::setLanguage(const String &s)
{
  setTextFrame("TLAN", s);
}

void ID3v2::Tag::setKey(const String &s)
{
  setTextFrame("TKEY", s);
}

void ID3v2::Tag::setLicense(const String &s)
{
  setTextFrame("TCOP", s);
}

void ID3v2::Tag::setLicenseUrl(const String &s)
{
  setTextFrame("WCOP", s);
}

void ID3v2::Tag::setYear(uint i)
{
  if(i <= 0) {
    removeFrames("TDRC");
    return;
  }
  setTextFrame("TDRC", String::number(i));
}

void ID3v2::Tag::setTrack(uint i)
{
  if(i <= 0 && d->totalTracks == 0) {
    removeFrames("TRCK");
    return;
  }
  
  d->track = i;
  setTextFrame("TRCK", Tag::splitNumberRender(i, d->totalTracks));
}

void ID3v2::Tag::setTotalTracks(uint i)
{
  if(i <= 0 && d->track == 0) {
    removeFrames("TRCK");
    return;
  }
  
  d->totalTracks = i;

  setTextFrame("TRCK", Tag::splitNumberRender(d->track, i));
}

void ID3v2::Tag::setDisc(uint i)
{
  if(i <= 0 && d->totalDiscs == 0) {
    removeFrames("TPOS");
    return;
  }
  
  d->disc = i;
  setTextFrame("TPOS", Tag::splitNumberRender(i, d->totalDiscs));
}

void ID3v2::Tag::setTotalDiscs(uint i)
{
  if(i <= 0 && d->disc == 0) {
    removeFrames("TPOS");
    return;
  }
  
  setTextFrame("TPOS", Tag::splitNumberRender(d->disc, i));
}

void ID3v2::Tag::setBpm(uint i)
{
  if(i <= 0) {
    removeFrames("TBPM");
    return;
  }
  setTextFrame("TBPM", String::number(i));
}

// TODO: make this an iTunes hack thing and use something more
//       "speccish"? compiletime option?
void ID3v2::Tag::setIsCompilation(bool i)
{
  if(!i) {
    removeFrames("TCMP");
    return;
  }
  setTextFrame("TCMP", "true");
}

bool ID3v2::Tag::isEmpty() const
{
  return d->frameList.isEmpty();
}

Header *ID3v2::Tag::header() const
{
  return &(d->header);
}

ExtendedHeader *ID3v2::Tag::extendedHeader() const
{
  return d->extendedHeader;
}

Footer *ID3v2::Tag::footer() const
{
  return d->footer;
}

const FrameListMap &ID3v2::Tag::frameListMap() const
{
  return d->frameListMap;
}

const FrameList &ID3v2::Tag::frameList() const
{
  return d->frameList;
}

const FrameList &ID3v2::Tag::frameList(const ByteVector &frameID) const
{
  return d->frameListMap[frameID];
}

void ID3v2::Tag::addFrame(Frame *frame)
{
  d->frameList.append(frame);
  d->frameListMap[frame->frameID()].append(frame);
}

void ID3v2::Tag::removeFrame(Frame *frame, bool del)
{
  // remove the frame from the frame list
  FrameList::Iterator it = d->frameList.find(frame);
  d->frameList.erase(it);

  // ...and from the frame list map
  it = d->frameListMap[frame->frameID()].find(frame);
  d->frameListMap[frame->frameID()].erase(it);

  // ...and delete as desired
  if(del)
    delete frame;
}

void ID3v2::Tag::removeFrames(const ByteVector &id)
{
    FrameList l = d->frameListMap[id];
    for(FrameList::Iterator it = l.begin(); it != l.end(); ++it)
      removeFrame(*it, true);
}

ByteVector ID3v2::Tag::render() const
{
  // We need to render the "tag data" first so that we have to correct size to
  // render in the tag's header.  The "tag data" -- everything that is included
  // in ID3v2::Header::tagSize() -- includes the extended header, frames and
  // padding, but does not include the tag's header or footer.

  ByteVector tagData;

  // TODO: Render the extended header.

  // Loop through the frames rendering them and adding them to the tagData.

  for(FrameList::Iterator it = d->frameList.begin(); it != d->frameList.end(); it++) {
    if(!(*it)->header()->tagAlterPreservation())
      tagData.append((*it)->render());
  }

  // Compute the amount of padding, and append that to tagData.

  uint paddingSize = 0;
  uint originalSize = d->header.tagSize();

  if(tagData.size() < originalSize)
    paddingSize = originalSize - tagData.size();
  else
    paddingSize = 1024;

  tagData.append(ByteVector(paddingSize, char(0)));

  // Set the tag size.
  d->header.setTagSize(tagData.size());

  // TODO: This should eventually include d->footer->render().
  return d->header.render() + tagData;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void ID3v2::Tag::read()
{
  if(d->file && d->file->isOpen()) {

    d->file->seek(d->tagOffset);
    d->header.setData(d->file->readBlock(Header::size()));

    // if the tag size is 0, then this is an invalid tag (tags must contain at
    // least one frame)

    if(d->header.tagSize() == 0)
      return;

    parse(d->file->readBlock(d->header.tagSize()));
  }
}

void ID3v2::Tag::parse(const ByteVector &origData)
{
  ByteVector data = origData;

  if(d->header.unsynchronisation() && d->header.majorVersion() <= 3)
    data = SynchData::decode(data);

  uint frameDataPosition = 0;
  uint frameDataLength = data.size();

  // check for extended header

  if(d->header.extendedHeader()) {
    if(!d->extendedHeader)
      d->extendedHeader = new ExtendedHeader;
    d->extendedHeader->setData(data);
    if(d->extendedHeader->size() <= data.size()) {
      frameDataPosition += d->extendedHeader->size();
      frameDataLength -= d->extendedHeader->size();
    }
  }

  // check for footer -- we don't actually need to parse it, as it *must*
  // contain the same data as the header, but we do need to account for its
  // size.

  if(d->header.footerPresent() && Footer::size() <= frameDataLength)
    frameDataLength -= Footer::size();

  // parse frames

  // Make sure that there is at least enough room in the remaining frame data for
  // a frame header.

  while(frameDataPosition < frameDataLength - Frame::headerSize(d->header.majorVersion())) {

    // If the next data is position is 0, assume that we've hit the padding
    // portion of the frame data.

    if(data.at(frameDataPosition) == 0) {
      if(d->header.footerPresent())
        debug("Padding *and* a footer found.  This is not allowed by the spec.");

      d->paddingSize = frameDataLength - frameDataPosition;
      return;
    }

    Frame *frame = d->factory->createFrame(data.mid(frameDataPosition),
                                           &d->header);

    if(!frame)
      return;

    // Checks to make sure that frame parsed correctly.

    if(frame->size() <= 0) {
      delete frame;
      return;
    }

    frameDataPosition += frame->size() + Frame::headerSize(d->header.majorVersion());
    addFrame(frame);
  }
}

void ID3v2::Tag::setTextFrame(const ByteVector &id, const String &value)
{
  if(value.isEmpty()) {
    removeFrames(id);
    return;
  }

  if(!d->frameListMap[id].isEmpty())
    d->frameListMap[id].front()->setText(value);
  else {
    const String::Type encoding = d->factory->defaultTextEncoding();
    TextIdentificationFrame *f = new TextIdentificationFrame(id, encoding);
    addFrame(f);
    f->setText(value);
  }
}
