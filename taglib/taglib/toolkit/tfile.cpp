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

#include "tfile.h"
#include "tlocalfileio.h"
#include "tstring.h"
#include "tdebug.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
# include <wchar.h>
# include <windows.h>
# include <io.h>
# define ftruncate _chsize
#else
# include <unistd.h>
#endif

#include <stdlib.h>

#ifndef R_OK
# define R_OK 4
#endif
#ifndef W_OK
# define W_OK 2
#endif

using namespace TagLib;

class File::FilePrivate
{
public:
  FilePrivate(FileName fileName);

  FileIO *fileIO;
  bool valid;
  ulong size;
  static const uint bufferSize = 1024;
};

File::FilePrivate::FilePrivate(FileName fileName) :
  fileIO(NULL),
  valid(true),
  size(0)
{
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

File::File(FileName file)
{
  d = new FilePrivate(file);

  d->fileIO = new LocalFileIO(file);

  if (d->fileIO && !d->fileIO->isOpen()) {
    delete d->fileIO;
    d->fileIO = NULL;
  }

  if(!d->fileIO)
    debug("Could not open file " + String((const char *) file));
}

File::~File()
{
  if(d->fileIO)
    delete d->fileIO;
  delete d;
}

FileName File::name() const
{
  if(!d->fileIO) {
    debug("File::name() -- Invalid File");
    return (char *) NULL;
  }

  return d->fileIO->name();
}

ByteVector File::readBlock(ulong length)
{
  if(!d->fileIO) {
    debug("File::readBlock() -- Invalid File");
    return ByteVector::null;
  }

  if(length == 0)
    return ByteVector::null;

  return d->fileIO->readBlock(length);
}

void File::writeBlock(const ByteVector &data)
{
  if(!d->fileIO)
    return;

  d->fileIO->writeBlock(data);
}

long File::find(const ByteVector &pattern, long fromOffset, const ByteVector &before)
{
  if(!d->fileIO || pattern.size() > d->bufferSize)
      return -1;

  // The position in the file that the current buffer starts at.

  long bufferOffset = fromOffset;
  ByteVector buffer;

  // These variables are used to keep track of a partial match that happens at
  // the end of a buffer.

  int previousPartialMatch = -1;
  int beforePreviousPartialMatch = -1;

  // Save the location of the current read pointer.  We will restore the
  // position using seek() before all returns.

  long originalPosition = tell();

  // Start the search at the offset.

  seek(fromOffset);

  // This loop is the crux of the find method.  There are three cases that we
  // want to account for:
  //
  // (1) The previously searched buffer contained a partial match of the search
  // pattern and we want to see if the next one starts with the remainder of
  // that pattern.
  //
  // (2) The search pattern is wholly contained within the current buffer.
  //
  // (3) The current buffer ends with a partial match of the pattern.  We will
  // note this for use in the next itteration, where we will check for the rest
  // of the pattern.
  //
  // All three of these are done in two steps.  First we check for the pattern
  // and do things appropriately if a match (or partial match) is found.  We
  // then check for "before".  The order is important because it gives priority
  // to "real" matches.

  for(buffer = readBlock(d->bufferSize); buffer.size() > 0; buffer = readBlock(d->bufferSize)) {

    // (1) previous partial match

    if(previousPartialMatch >= 0 && int(d->bufferSize) > previousPartialMatch) {
      const int patternOffset = (d->bufferSize - previousPartialMatch);
      if(buffer.containsAt(pattern, 0, patternOffset)) {
        seek(originalPosition);
        return bufferOffset - d->bufferSize + previousPartialMatch;
      }
    }

    if(!before.isNull() && beforePreviousPartialMatch >= 0 && int(d->bufferSize) > beforePreviousPartialMatch) {
      const int beforeOffset = (d->bufferSize - beforePreviousPartialMatch);
      if(buffer.containsAt(before, 0, beforeOffset)) {
        seek(originalPosition);
        return -1;
      }
    }

    // (2) pattern contained in current buffer

    long location = buffer.find(pattern);
    if(location >= 0) {
      seek(originalPosition);
      return bufferOffset + location;
    }

    if(!before.isNull() && buffer.find(before) >= 0) {
      seek(originalPosition);
      return -1;
    }

    // (3) partial match

    previousPartialMatch = buffer.endsWithPartialMatch(pattern);

    if(!before.isNull())
      beforePreviousPartialMatch = buffer.endsWithPartialMatch(before);

    bufferOffset += d->bufferSize;
  }

  // Since we hit the end of the file, reset the status before continuing.

  clear();

  seek(originalPosition);

  return -1;
}


long File::rfind(const ByteVector &pattern, long fromOffset, const ByteVector &before)
{
  if(!d->fileIO || pattern.size() > d->bufferSize)
      return -1;

  // The position in the file that the current buffer starts at.

  ByteVector buffer;

  // These variables are used to keep track of a partial match that happens at
  // the end of a buffer.

  /*
  int previousPartialMatch = -1;
  int beforePreviousPartialMatch = -1;
  */

  // Save the location of the current read pointer.  We will restore the
  // position using seek() before all returns.

  long originalPosition = tell();

  // Start the search at the offset.

  long bufferOffset;
  if(fromOffset == 0) {
    seek(-1 * int(d->bufferSize), End);
    bufferOffset = tell();
  }
  else {
    seek(fromOffset + -1 * int(d->bufferSize), Beginning);
    bufferOffset = tell();
  }

  // See the notes in find() for an explanation of this algorithm.

  for(buffer = readBlock(d->bufferSize); buffer.size() > 0; buffer = readBlock(d->bufferSize)) {

    // TODO: (1) previous partial match

    // (2) pattern contained in current buffer

    long location = buffer.rfind(pattern);
    if(location >= 0) {
      seek(originalPosition);
      return bufferOffset + location;
    }

    if(!before.isNull() && buffer.find(before) >= 0) {
      seek(originalPosition);
      return -1;
    }

    // TODO: (3) partial match

    bufferOffset -= d->bufferSize;
    seek(bufferOffset);
  }

  // Since we hit the end of the file, reset the status before continuing.

  clear();

  seek(originalPosition);

  return -1;
}

void File::insert(const ByteVector &data, ulong start, ulong replace)
{
  if(!d->fileIO)
    return;

  d->fileIO->insert(data, start, replace);
}

void File::removeBlock(ulong start, ulong length)
{
  if(!d->fileIO)
    return;

  d->fileIO->removeBlock(start, length);
}

bool File::readOnly() const
{
  if(!d->fileIO)
    return true;

  return d->fileIO->readOnly();
}

bool File::isReadable()
{
  if(!d->fileIO)
    return false;

  return d->fileIO->isReadable();
}

bool File::isOpen() const
{
  if(!d->fileIO)
    return false;

  return d->fileIO->isOpen();
}

bool File::isValid() const
{
  return isOpen() && d->valid;
}

void File::seek(long offset, Position p)
{
  if(!d->fileIO) {
    debug("File::seek() -- trying to seek in a file that isn't opened.");
    return;
  }

  d->fileIO->seek(offset, p);
}

void File::clear()
{
  if(!d->fileIO)
    return;

  d->fileIO->clear();
}

long File::tell() const
{
  if(!d->fileIO)
    return -1;

  return d->fileIO->tell();
}

long File::length()
{
  if(!d->fileIO)
    return 0;

  return d->fileIO->length();
}

bool File::isWritable()
{
  if(!d->fileIO)
    return false;

  return d->fileIO->isWritable();
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void File::setValid(bool valid)
{
  d->valid = valid;
}

void File::truncate(long length)
{
  if(!d->fileIO)
    return;

  d->fileIO->truncate(length);
}

TagLib::uint File::bufferSize()
{
  return FilePrivate::bufferSize;
}
