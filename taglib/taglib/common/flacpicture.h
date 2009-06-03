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

#ifndef TAGLIB_FLACPICTURE_H
#define TAGLIB_FLACPICTURE_H

#include "tag.h"
#include "tlist.h"
#include "tmap.h"
#include "tstring.h"
#include "tstringlist.h"
#include "tbytevector.h"
#include "taglib_export.h"
    
const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
    
namespace TagLib {

  /*!
   * This class is an implementation of the METADATA_BLOCK_PICTURE
   * specification as used by Ogg Vorbis, and Flac and detailed here:
   * http://flac.sourceforge.net/format.html#metadata_block_picture
   */

  class TAGLIB_EXPORT FlacPicture
  {
  public:
    enum Type {
      //! A type not enumerated below
      Other              = 0x00,
      //! 32x32 PNG image that should be used as the file icon
      FileIcon           = 0x01,
      //! File icon of a different size or format
      OtherFileIcon      = 0x02,
      //! Front cover image of the album
      FrontCover         = 0x03,
      //! Back cover image of the album
      BackCover          = 0x04,
      //! Inside leaflet page of the album
      LeafletPage        = 0x05,
      //! Image from the album itself
      Media              = 0x06,
      //! Picture of the lead artist or soloist
      LeadArtist         = 0x07,
      //! Picture of the artist or performer
      Artist             = 0x08,
      //! Picture of the conductor
      Conductor          = 0x09,
      //! Picture of the band or orchestra
      Band               = 0x0A,
      //! Picture of the composer
      Composer           = 0x0B,
      //! Picture of the lyricist or text writer
      Lyricist           = 0x0C,
      //! Picture of the recording location or studio
      RecordingLocation  = 0x0D,
      //! Picture of the artists during recording
      DuringRecording    = 0x0E,
      //! Picture of the artists during performance
      DuringPerformance  = 0x0F,
      //! Picture from a movie or video related to the track
      MovieScreenCapture = 0x10,
      //! Picture of a large, coloured fish
      ColouredFish       = 0x11,
      //! Illustration related to the track
      Illustration       = 0x12,
      //! Logo of the band or performer
      BandLogo           = 0x13,
      //! Logo of the publisher (record company)
      PublisherLogo      = 0x14
    };

    /*!
     * Constructs an empty Artwork object
     */
    FlacPicture();

    /*!
     * Constructs an Artwork object from \a data.
     */
    FlacPicture(const ByteVector &data);

    /*!
     * Constructs an Artwork object from base64 encoded string \a s.
     */
    FlacPicture(const String &s);

    /*!
     * Destroys this instance of the XiphComment.
     */
    ~FlacPicture();
   
    /*!
     * Returns the mime type of the image.  This should in most cases be
     * "image/png" or "image/jpeg".  Can also be "-->" to indicate that
     * the data stream is an URL to the picture itself
     */
    String mimeType() const;

    /*!
     * Sets the mime type of the image.  This should in most cases be
     * "image/png" or "image/jpeg".
     */
    void setMimeType(const String &m);

    /*!
     * Returns the type of the image.
     *
     * \see Type
     * \see setType()
     */
    Type type() const;

    /*!
     * Sets the type of the image.
     *
     * \see Type
     */
    void setType(const Type &t);

    /*!
     * Returns a text description of the image.
     *
     * \see setDescription()
     * \see textEncoding()
     * \see setTextEncoding()
     */

    String description() const;

    /*!
     * Sets a textual description of the image to \a desc.
     *
     * \see description()
     * \see textEncoding()
     * \see setTextEncoding()
     */

    void setDescription(const String &desc);

    /*!
     * Returns the image data as a ByteVector.
     *
     * \note ByteVector has a data() method that returns a const char * which
     * should make it easy to export this data to external programs.
     *
     * \see setPicture()
     * \see mimeType()
     */
    ByteVector picture() const;

    /*!
     * Sets the image data to \a p.  \a p should be of the type specified in
     * this frame's mime-type specification.
     *
     * \see picture()
     * \see mimeType()
     * \see setMimeType()
     */
    void setPicture(const ByteVector &p);

    /*!
     * Renders the picture block to a ByteVector suitable for inserting into
     * a file.
     *
     * If \a b64Encode is true, then the returned ByteVector will be encoded
     * as base64 output.
     */
    ByteVector render(bool b64Encode = false);

    /*!
     * Reads the picture from the given bytevector and parses it into this
     */
    bool parse(const ByteVector &data);
    bool parse(const String &encodedString);
    
  private:
    Type mType;
    String mMimeType;
    String mDescription;
    ByteVector mPicture;

    std::string base64_decode(std::string const& encoded_string);
    std::string base64_encode(unsigned char const* bytes_to_encode,
                              unsigned int in_len);
  };
}

#endif
