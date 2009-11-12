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

#include <tagunion.h>
#include <id3v2tag.h>
#include <id3v2header.h>
#include <id3v1tag.h>
#include <apefooter.h>
#include <apetag.h>
#include <tdebug.h>

#include <bitset>

#include "mpegfile.h"
#include "mpegheader.h"

using namespace TagLib;

namespace
{
  enum { ID3v2Index = 0, APEIndex = 1, ID3v1Index = 2 };
}

class MPEG::File::FilePrivate
{
public:
  FilePrivate(ID3v2::FrameFactory *frameFactory = ID3v2::FrameFactory::instance()) :
    ID3v2FrameFactory(frameFactory),
    ID3v2Location(-1),
    ID3v2OriginalSize(0),
    APELocation(-1),
    APEFooterLocation(-1),
    APEOriginalSize(0),
    ID3v1Location(-1),
    hasID3v2(false),
    hasID3v1(false),
    hasAPE(false),
    properties(0)
  {

  }

  ~FilePrivate()
  {
    delete properties;
  }

  const ID3v2::FrameFactory *ID3v2FrameFactory;

  long ID3v2Location;
  uint ID3v2OriginalSize;

  long APELocation;
  long APEFooterLocation;
  uint APEOriginalSize;

  long ID3v1Location;

  TagUnion tag;

  // These indicate whether the file *on disk* has these tags, not if
  // this data structure does.  This is used in computing offsets.

  bool hasID3v2;
  bool hasID3v1;
  bool hasAPE;

  Properties *properties;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

MPEG::File::File(ID3v2::FrameFactory *frameFactory) : TagLib::File()
{
  if (frameFactory)
    d = new FilePrivate(frameFactory);
  else
    d = new FilePrivate;
}

MPEG::File::File(FileName file, bool readProperties,
                 Properties::ReadStyle propertiesStyle) : TagLib::File(file)
{
  d = new FilePrivate;
  read(readProperties, propertiesStyle);
}

MPEG::File::File(FileName file, ID3v2::FrameFactory *frameFactory,
                 bool readProperties, Properties::ReadStyle propertiesStyle) :
  TagLib::File(file)
{
  d = new FilePrivate(frameFactory);
  read(readProperties, propertiesStyle);
}

MPEG::File::~File()
{
  delete d;
}

TagLib::Tag *MPEG::File::tag() const
{
  return &d->tag;
}

MPEG::Properties *MPEG::File::audioProperties() const
{
  return d->properties;
}

bool MPEG::File::save()
{
  return save(AllTags);
}

bool MPEG::File::save(int tags)
{
  return save(tags, true);
}

bool MPEG::File::save(int tags, bool stripOthers)
{
  if(tags == NoTags && stripOthers)
    return strip(AllTags);

  if(!ID3v2Tag() && !ID3v1Tag() && !APETag()) {

    if((d->hasID3v1 || d->hasID3v2 || d->hasAPE) && stripOthers)
      return strip(AllTags);

    return true;
  }

  if(readOnly()) {
    debug("MPEG::File::save() -- File is read only.");
    return false;
  }

  // Create the tags if we've been asked to.  Copy the values from the tag that
  // does exist into the new tag.

  if((tags & ID3v2) && ID3v1Tag())
    Tag::duplicate(ID3v1Tag(), ID3v2Tag(true), false);

  if((tags & ID3v1) && d->tag[ID3v2Index])
    Tag::duplicate(ID3v2Tag(), ID3v1Tag(true), false);

  bool success = true;
  TagLib::FileIO *outfile = this;

  if(ID3v2 & tags) {

    if(!ID3v2Tag() || ID3v2Tag()->isEmpty()) {
      if(stripOthers && d->ID3v2OriginalSize > 0) {
        // we need to write a tag out, but there is no tag;
        // strip it out from the file and replace with padding instead
        TagLib::ID3v2::Header header;
        header.setTagSize(d->ID3v2OriginalSize);
        outfile->insert(header.render(), d->ID3v2Location, d->ID3v2OriginalSize);
      }
    }
    else {
      // we were asked to write a tag, and there is data to write

      if(!d->hasID3v2)
        d->ID3v2Location = 0;

      ByteVector id3data = ID3v2Tag()->render();
      if (id3data.size() > d->ID3v2OriginalSize) {
        outfile = this->tempFile();
        if (!outfile) {
          debug("ERROR: failed to create temporary file!");
          return false;
        }
        this->seek( 0 );
        outfile->seek( 0 );
        for ( ulonglong readIndex = 0; readIndex < d->ID3v2Location; )
        {
          ulonglong size = std::min( (ulonglong)4096, d->ID3v2Location - readIndex );
          ByteVector buffer = this->readBlock(size);
          outfile->writeBlock(buffer);
          readIndex += buffer.size();
        }
      }

      outfile->insert(id3data, d->ID3v2Location, d->ID3v2OriginalSize);

      d->hasID3v2 = true;

      if (outfile != this) {
        // we're using a scratch file (because the tag needed to grow)
        // copy the old data over (including APE + id3v1 tags; we can
        // delete it later if necessary, they're near end of file and easy)
        ulonglong end = this->length();
        this->seek( d->ID3v2OriginalSize, FileIO::Current );
        for ( ulonglong readIndex = this->tell(); readIndex < end; )
        {
          ulonglong size = std::min( (ulonglong)4096, end - readIndex );
          ByteVector buffer = this->readBlock(size);
          outfile->writeBlock(buffer);
          readIndex += buffer.size();
        }
        // things have moved, shift them over
        long id3v2Difference = id3data.size() - d->ID3v2OriginalSize;
        if (d->hasID3v1)
          d->ID3v1Location += id3v2Difference;
        if (d->hasAPE) {
          d->APELocation += id3v2Difference;
          d->APEFooterLocation += id3v2Difference;
        }
      }
    }
  }
  else if(d->hasID3v2 && stripOthers) {
    d->tag.set(ID3v2Index, 0);
    if(d->ID3v2OriginalSize > 0) {
      // for efficiency, avoid rewriting the entire file;
      // replace the original id3v2 tag with all padding instead
      TagLib::ID3v2::Header header;
      header.setTagSize(d->ID3v2OriginalSize);
      ByteVector headerBytes = header.render();
      headerBytes.append(ByteVector(d->ID3v2OriginalSize - headerBytes.size()));
      outfile->insert(headerBytes, d->ID3v2Location, d->ID3v2OriginalSize);
    }
  }

  if(ID3v1 & tags) {
    if(ID3v1Tag() && !ID3v1Tag()->isEmpty()) {
      int offset = d->hasID3v1 ? -128 : 0;
      outfile->seek(offset, End);
      d->ID3v1Location = outfile->tell();
      outfile->writeBlock(ID3v1Tag()->render());
      d->hasID3v1 = true;
    }
    else if(d->hasID3v1 && stripOthers) {
      outfile->removeBlock(d->ID3v1Location, 128);
      d->hasID3v1 = false;
      d->ID3v1Location = -1;
      d->tag.set(ID3v1Index, 0);
    }
  }
  else if(d->hasID3v1 && stripOthers) {
    // strip the id3v1 tag
    outfile->removeBlock(d->ID3v1Location, 128);
    d->hasID3v1 = false;
    d->ID3v1Location = -1;
  }

  // Dont save an APE-tag unless one has been created

  if((APE & tags) && APETag()) {
    if(d->hasAPE)
      outfile->insert(APETag()->render(), d->APELocation, d->APEOriginalSize);
    else {
      if(d->hasID3v1) {
        outfile->insert(APETag()->render(), d->ID3v1Location, 0);
        d->APEOriginalSize = APETag()->footer()->completeTagSize();
        d->hasAPE = true;
        d->APELocation = d->ID3v1Location;
        d->ID3v1Location += d->APEOriginalSize;
      }
      else {
        outfile->seek(0, End);
        d->APELocation = outfile->tell();
        d->APEFooterLocation = d->APELocation
          + d->tag.access<APE::Tag>(APEIndex, false)->footer()->completeTagSize()
          - APE::Footer::size();
        outfile->writeBlock(APETag()->render());
        d->APEOriginalSize = APETag()->footer()->completeTagSize();
        d->hasAPE = true;
      }
    }
  }
  else if(d->hasAPE && stripOthers) {
    removeBlock(d->APELocation, d->APEOriginalSize);
    if(d->hasID3v1) {
      if(d->ID3v1Location > d->APELocation)
        d->ID3v1Location -= d->APEOriginalSize;
    }
    d->APELocation = -1;
    d->APEFooterLocation = -1;
    d->hasAPE = false;
  }

  if (outfile != this) {
    success &= this->closeTempFile(success);
  }

  return success;
}

ID3v2::Tag *MPEG::File::ID3v2Tag(bool create)
{
  return d->tag.access<ID3v2::Tag>(ID3v2Index, create);
}

ID3v1::Tag *MPEG::File::ID3v1Tag(bool create)
{
  return d->tag.access<ID3v1::Tag>(ID3v1Index, create);
}

APE::Tag *MPEG::File::APETag(bool create)
{
  return d->tag.access<APE::Tag>(APEIndex, create);
}

bool MPEG::File::strip(int tags)
{
  return strip(tags, true);
}

bool MPEG::File::strip(int tags, bool freeMemory)
{
  if(readOnly()) {
    debug("MPEG::File::strip() - Cannot strip tags from a read only file.");
    return false;
  }

  // might need to strip:
  //   from the front: id3v2 only (taglib doesn't find ape at front)
  //   from the back: id3v1, ape

  ulonglong endOffset = length();

  FileIO *file = this;

  if((tags & ID3v2) && d->hasID3v2) {
    file = this->tempFile();
    if (file) {
      // successfully created a temp file; copy the area before the id3v2 tag
      this->seek( 0 );
      file->seek( 0 );
      for ( ulonglong readIndex = 0; readIndex < d->ID3v2Location; )
      {
        ulonglong size = std::min( (ulonglong)4096, d->ID3v2Location - readIndex );
        ByteVector buffer = this->readBlock(size);
        file->writeBlock(buffer);
        readIndex += buffer.size();
      }
      this->seek( d->ID3v2OriginalSize, FileIO::Current );
      long readLength = endOffset - this->tell();
      for ( ulonglong readIndex = 0; readIndex < readLength ; )
      {
        ulonglong size = std::min( (ulonglong)4096, readLength - readIndex );
        ByteVector buffer = this->readBlock(size);
        file->writeBlock(buffer);
        readIndex += buffer.size();
      }
    }
    else {
      // failed to create a temp file; just abort, avoid touching anything
      return false;
    }

    long ID3v2RemovedSize = d->ID3v2OriginalSize;
    d->ID3v2Location = -1;
    d->ID3v2OriginalSize = 0;
    d->hasID3v2 = false;

    if(freeMemory)
      d->tag.set(ID3v2Index, 0);

    // v1 tag location has changed, update if it exists

    if(this->ID3v1Tag())
      d->ID3v1Location -= ID3v2RemovedSize;

    // APE tag location has changed, update if it exists

   if(this->APETag())
      d->APELocation -= ID3v2RemovedSize;
  }

  if((tags & ID3v1) && d->hasID3v1) {
    file->removeBlock(d->ID3v1Location, 128);
    if (d->hasAPE) {
      if (d->APELocation > d->ID3v1Location) {
        d->APELocation -= 128;
        d->APEFooterLocation -= 128;
      }
    }
    d->ID3v1Location = -1;
    d->hasID3v1 = false;

    if(freeMemory)
      d->tag.set(ID3v1Index, 0);
  }

  if((tags & APE) && d->hasAPE) {
    file->removeBlock(d->APELocation, d->APEOriginalSize);
    if(d->hasID3v1) {
      if(d->ID3v1Location > d->APELocation)
        d->ID3v1Location -= d->APEOriginalSize;
    }
    d->APELocation = -1;
    d->APEFooterLocation = -1;
    d->hasAPE = false;

    if(freeMemory)
      d->tag.set(APEIndex, 0);
  }

  if (file != this) {
    // used a temp file
    return this->closeTempFile(true);
  }

  return true;
}

void MPEG::File::setID3v2FrameFactory(const ID3v2::FrameFactory *factory)
{
  d->ID3v2FrameFactory = factory;
}

long MPEG::File::nextFrameOffset(long position)
{
  bool foundLastSyncPattern = false;

  long endPosition;
  long maxScanBytes = getMaxScanBytes();
  if (maxScanBytes > 0)
    endPosition = position + maxScanBytes;
  else
    endPosition = 0;

  ByteVector buffer;

  while(true) {
    seek(position);
    buffer = readBlock(bufferSize());

    if(buffer.size() <= 0)
      return -1;

    if(foundLastSyncPattern && secondSynchByte(buffer[0]))
      return position - 1;

    for(uint i = 0; i < buffer.size() - 1; i++) {
      if(uchar(buffer[i]) == 0xff && secondSynchByte(buffer[i + 1]))
        return position + i;
    }

    foundLastSyncPattern = uchar(buffer[buffer.size() - 1]) == 0xff;
    position += buffer.size();

    if (endPosition && (position >= endPosition))
      break;
  }
  return -1;
}

long MPEG::File::previousFrameOffset(long position)
{
  bool foundFirstSyncPattern = false;
  ByteVector buffer;

  long endPosition;
  long maxScanBytes = getMaxScanBytes();
  if ((maxScanBytes > 0) && (position > maxScanBytes))
    endPosition = position - maxScanBytes;
  else
    endPosition = 0;

  while (position > 0) {
    long size = ulong(position) < bufferSize() ? position : bufferSize();
    position -= size;

    seek(position);
    buffer = readBlock(size);

    if(buffer.size() <= 0)
      break;

    if(foundFirstSyncPattern && uchar(buffer[buffer.size() - 1]) == 0xff)
      return position + buffer.size() - 1;

    for(int i = buffer.size() - 2; i >= 0; i--) {
      if(uchar(buffer[i]) == 0xff && secondSynchByte(buffer[i + 1]))
        return position + i;
    }

    foundFirstSyncPattern = secondSynchByte(buffer[0]);

    if (endPosition && (position <= endPosition))
      break;
  }
  return -1;
}

long MPEG::File::firstFrameOffset()
{
  long position = 0;

  if(ID3v2Tag())
    position = d->ID3v2Location + ID3v2Tag()->header()->completeTagSize();

  return nextFrameOffset(position);
}

long MPEG::File::lastFrameOffset()
{
  return previousFrameOffset(ID3v1Tag() ? d->ID3v1Location - 1 : length());
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

/*XXXeps public method but kept here to ease merging. */
void MPEG::File::read(bool readProperties, Properties::ReadStyle propertiesStyle)
{
  if (!isOpen())
    return;

  // Look for an ID3v2 tag

  d->ID3v2Location = findID3v2();

  if(d->ID3v2Location >= 0) {

    d->tag.set(ID3v2Index, new ID3v2::Tag(this, d->ID3v2Location, d->ID3v2FrameFactory));

    d->ID3v2OriginalSize = ID3v2Tag()->header()->completeTagSize();

    if(ID3v2Tag()->header()->tagSize() <= 0)
      d->tag.set(ID3v2Index, 0);
    else
      d->hasID3v2 = true;
  }

  // Look for an ID3v1 tag

  d->ID3v1Location = findID3v1();

  if(d->ID3v1Location >= 0) {
    d->tag.set(ID3v1Index, new ID3v1::Tag(this, d->ID3v1Location));
    d->hasID3v1 = true;
  }

  // Look for an APE tag

  findAPE();

  if(d->APELocation >= 0) {

    d->tag.set(APEIndex, new APE::Tag(this, d->APEFooterLocation));
    d->APEOriginalSize = APETag()->footer()->completeTagSize();
    d->hasAPE = true;
  }

  if(readProperties)
    d->properties = new Properties(this, propertiesStyle);

  // Make sure that we have our default tag types available.

  ID3v2Tag(true);
  ID3v1Tag(true);
}

long MPEG::File::findID3v2()
{
  // This method is based on the contents of TagLib::File::find(), but because
  // of some subtlteies -- specifically the need to look for the bit pattern of
  // an MPEG sync, it has been modified for use here.

  if(isValid() && ID3v2::Header::fileIdentifier().size() <= bufferSize()) {

    // The position in the file that the current buffer starts at.

    long maxScanBytes = getMaxScanBytes();
    long bufferOffset = 0;
    long endBufferOffset;
    ByteVector buffer;

    // These variables are used to keep track of a partial match that happens at
    // the end of a buffer.

    int previousPartialMatch = -1;
    bool previousPartialSynchMatch = false;

    // Save the location of the current read pointer.  We will restore the
    // position using seek() before all returns.

    long originalPosition = tell();

    // Determine where to end search.

    if (maxScanBytes > 0)
      endBufferOffset = bufferOffset + maxScanBytes;
    else
      endBufferOffset = 0;

    // Start the search at the beginning of the file.

    seek(0);

    // This loop is the crux of the find method.  There are three cases that we
    // want to account for:
    // (1) The previously searched buffer contained a partial match of the search
    // pattern and we want to see if the next one starts with the remainder of
    // that pattern.
    //
    // (2) The search pattern is wholly contained within the current buffer.
    //
    // (3) The current buffer ends with a partial match of the pattern.  We will
    // note this for use in the next itteration, where we will check for the rest
    // of the pattern.

    for(buffer = readBlock(bufferSize()); buffer.size() > 0; buffer = readBlock(bufferSize())) {

      // (1) previous partial match

      if(previousPartialSynchMatch && secondSynchByte(buffer[0]))
        return -1;

      if(previousPartialMatch >= 0 && int(bufferSize()) > previousPartialMatch) {
        const int patternOffset = (bufferSize() - previousPartialMatch);
        if(buffer.containsAt(ID3v2::Header::fileIdentifier(), 0, patternOffset)) {
          seek(originalPosition);
          return bufferOffset - bufferSize() + previousPartialMatch;
        }
      }

      // (2) pattern contained in current buffer

      long location = buffer.find(ID3v2::Header::fileIdentifier());
      if(location >= 0) {
        seek(originalPosition);
        return bufferOffset + location;
      }

      int firstSynchByte = buffer.find(char(uchar(255)));

      // Here we have to loop because there could be several of the first
      // (11111111) byte, and we want to check all such instances until we find
      // a full match (11111111 111) or hit the end of the buffer.

      while(firstSynchByte >= 0) {

        // if this *is not* at the end of the buffer

        if(firstSynchByte < int(buffer.size()) - 1) {
          if(secondSynchByte(buffer[firstSynchByte + 1])) {
            // We've found the frame synch pattern.
            seek(originalPosition);
            return -1;
          }
          else {

            // We found 11111111 at the end of the current buffer indicating a
            // partial match of the synch pattern.  The find() below should
            // return -1 and break out of the loop.

            previousPartialSynchMatch = true;
          }
        }

        // Check in the rest of the buffer.

        firstSynchByte = buffer.find(char(uchar(255)), firstSynchByte + 1);
      }

      // (3) partial match

      previousPartialMatch = buffer.endsWithPartialMatch(ID3v2::Header::fileIdentifier());

      bufferOffset += bufferSize();

      if (endBufferOffset && (bufferOffset >= endBufferOffset))
        break;
    }

    // Since we hit the end of the file, reset the status before continuing.

    clear();

    seek(originalPosition);
  }

  return -1;
}

long MPEG::File::findID3v1()
{
  if(isValid()) {
    seek(-128, End);
    long p = tell();

    if(readBlock(3) == ID3v1::Tag::fileIdentifier())
      return p;
  }
  return -1;
}

void MPEG::File::findAPE()
{
  if(isValid()) {
    seek(d->hasID3v1 ? -160 : -32, End);

    long p = tell();

    if(readBlock(8) == APE::Tag::fileIdentifier()) {
      d->APEFooterLocation = p;
      seek(d->APEFooterLocation);
      APE::Footer footer(readBlock(APE::Footer::size()));
      d->APELocation = d->APEFooterLocation - footer.completeTagSize()
	+ APE::Footer::size();
      return;
    }
  }

  d->APELocation = -1;
  d->APEFooterLocation = -1;
}

bool MPEG::File::secondSynchByte(char byte)
{
  if(uchar(byte) == 0xff)
    return false;

  std::bitset<8> b(byte);

  // check to see if the byte matches 111xxxxx
  return b.test(7) && b.test(6) && b.test(5);
}
