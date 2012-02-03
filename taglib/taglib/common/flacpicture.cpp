/***************************************************************************
    copyright            : (C) 2009 by Pioneers of the Inevitable, Inc.
    email                : stevel@songbirdnest.com
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

/* This source incorporates René Nyffenegger's base64 encode/decode routines
 * which are covered under his copyright and 'as-is' license below.  The source
 * has been altered to fit this FlacPicture class.
 */

/*
 * base64 encode/decode routines:
 * Copyright (C) 2004-2008 René Nyffenegger
 *
 * This source code is provided 'as-is', without any express or implied
 * warranty. In no event will the author be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented; you must not
 *    claim that you wrote the original source code. If you use this source code
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original source code.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include <tbytevector.h>
#include <tdebug.h>

#include <flacpicture.h>
#include <stdio.h>

using namespace TagLib;

std::string FlacPicture::base64_encode(unsigned char const* bytes_to_encode,
                                       unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

static const unsigned char base64lookup[256] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff,   62, 0xff, 0xff, 0xff,   63,
    52,   53,   54,   55,   56,   57,   58,   59,
    60,   61, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff,    0,    1,    2,    3,    4,    5,    6,
     7,    8,    9,   10,   11,   12,   13,   14,
    15,   16,   17,   18,   19,   20,   21,   22,
    23,   24,   25, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff,   26,   27,   28,   29,   30,   31,   32,
    33,   34,   35,   36,   37,   38,   39,   40,
    41,   42,   43,   44,   45,   46,   47,   48,
    49,   50,   51, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

std::string FlacPicture::base64_decode(std::string const& encoded_string) {
  int i = 0;
  std::string ret;

  int in_len = encoded_string.size();
  if (in_len % 4 != 0) {
    // base-64 encoded data must always be a multiple of 4 bytes
    return ret;
  }

  while (in_len > 0) 
  {
    unsigned char vals[4];

    vals[0] = base64lookup[(unsigned char)encoded_string[i]];
    vals[1] = base64lookup[(unsigned char)encoded_string[i+1]];
    vals[2] = base64lookup[(unsigned char)encoded_string[i+2]];
    vals[3] = base64lookup[(unsigned char)encoded_string[i+3]];

    if (in_len > 4) 
    {
      /* Check that all input bytes are legal */
      if (vals[0] == 0xff || vals[1] == 0xff || vals[2] == 0xff ||
          vals[3] == 0xff) 
        return std::string();
    }
    else {
      // Final chunk may have one or two padding '=' bytes
      if (vals[0] == 0xff || vals[1] == 0xff)
        return std::string();
      if (vals[2] == 0xff || vals[3] == 0xff)
      {
        if (encoded_string[i+3] != '=' || 
            (vals[2] == 0xff && encoded_string[i+2] != '='))
          return std::string();

        ret += vals[0]<<2 | vals[1]>>4;
        if (vals[2] != 0xff)
          ret += (vals[1]&0x0F)<<4 | vals[2]>>2;
        // Done!
        return ret;
      }
    }

    ret += vals[0]<<2 | vals[1]>>4;
    ret += (vals[1]&0x0F)<<4 | vals[2]>>2;
    ret += (vals[2]&0x03)<<6 | vals[3];

    in_len -= 4;
    i += 4;
  }

  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

FlacPicture::FlacPicture()
{
}

FlacPicture::FlacPicture(const ByteVector &data)
{
  parse(data);
}

FlacPicture::FlacPicture(const String &encodedData)
{
  if (!encodedData.isNull())
  {
    std::string decodedData = base64_decode(encodedData.to8Bit());
    
    if (decodedData.empty())
      return;

    ByteVector bv;
    bv.setData(decodedData.data(), decodedData.size());
    
    parse(bv);
  }
}

FlacPicture::~FlacPicture()
{
}

FlacPicture::Type FlacPicture::type() const
{
  return mType;
}

void FlacPicture::setType(const Type &t)
{
  mType = t;
}


String FlacPicture::mimeType() const
{
  return mMimeType;
}

void FlacPicture::setMimeType(const String &s)
{
  mMimeType = s;
}

String FlacPicture::description() const
{
  return mDescription;
}

void FlacPicture::setDescription(const String &s)
{
  mDescription = s;
}

ByteVector FlacPicture::picture() const
{
  return mPicture;
}

void FlacPicture::setPicture(const ByteVector &bv)
{
  mPicture = ByteVector(bv);
}

bool FlacPicture::parse(const String &encodedData)
{
  if (!encodedData.isNull())
  {
    std::string decodedData = base64_decode(encodedData.to8Bit());
    
    if (decodedData.empty())
      return false;

    ByteVector bv;
    bv.setData(decodedData.data(), decodedData.size());
    
    return parse(bv);
  }
}
bool FlacPicture::parse(const ByteVector &bv)
{
  int mimeTypeLength;
  int descrLength;
  int pictureLength;
  int i;

  if (bv.size() < 32) {
    // Minimum size of a METADATA_PICTURE_BLOCK element is 32 bytes
    // assuming no mimetype, no description, and no picture data
    return false;
  }
  
  mType = (FlacPicture::Type)bv.mid(0, 4).toUInt();
  mimeTypeLength = bv.mid(4,4).toUInt();
  if (bv.size() < (32+mimeTypeLength))
    return false;
  mMimeType = String(bv.mid(8, mimeTypeLength));
  mMimeType.append('\0');
  i = 8 + mimeTypeLength;

  descrLength = bv.mid(i, 4).toUInt();
  if (bv.size() < (32+mimeTypeLength+descrLength))
    return false;
  mDescription = String(bv.mid(i+4, descrLength));
  mDescription.append('\0');
  i += 4 + descrLength;

  // We do not use or require these fields, so we don't bother parsing them.
  /*
  mWidth  = bv.mid(i, 4).toUInt();
  mHeight = bv.mid(i+4, 4).toUInt();
  mDepth  = bv.mid(i+8, 4).toUInt();
  mColors = bv.mid(i+12, 4).toUInt();
  */
  pictureLength = bv.mid(i+16, 4).toUInt();
  if (bv.size() != (32+mimeTypeLength+descrLength+pictureLength))
    return false;
  
  mPicture = bv.mid(i+20, pictureLength);

  return true;
}

ByteVector FlacPicture::render(bool b64Encode)
{
  ByteVector bv;
  
  // append the type
  bv.append(ByteVector::fromUInt(mType));
  
  // append the mime-type
  bv.append(ByteVector::fromUInt(mMimeType.to8Bit(true).size()));
  bv.append(ByteVector::fromCString(mMimeType.toCString(true)));

  // append the description length
  mDescription = String("Set from Songbird (http://getsongbird.com)");
  bv.append(ByteVector::fromUInt(mDescription.to8Bit(true).size()));
  bv.append(ByteVector::fromCString(mDescription.toCString(true)));

  // don't have this data easily and don't want to create an image object
  // just to parse these fields... that's a lot of overhead
  bv.append(ByteVector::fromUInt(0)); // width
  bv.append(ByteVector::fromUInt(0)); // height
  bv.append(ByteVector::fromUInt(0)); // depth
  bv.append(ByteVector::fromUInt(0)); // # of colors for indexed images

  // the picture data itself
  bv.append(ByteVector::fromUInt(mPicture.size()));
  bv.append(mPicture);

  if (b64Encode) {
    std::string encodedData = base64_encode((const unsigned char *)bv.data(), bv.size());
    bv = ByteVector(encodedData.data(), encodedData.length());
  }
  return bv;
}
