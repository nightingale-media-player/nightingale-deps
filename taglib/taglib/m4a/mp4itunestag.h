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

#ifndef MP4ITUNESTAG_H
#define MP4ITUNESTAG_H

#include "taglib.h"
#include "tstring.h"
#include "tag.h"

namespace TagLib
{
  namespace MP4
  {
    class File;

    class TAGLIB_EXPORT Tag : public TagLib::Tag
    {
    public:
      /*!
       * Constructs an empty MP4 iTunes tag.
       */
      Tag( );

      /*!
       * Destroys this Tag instance.
       */
      virtual ~Tag();

      // Reimplementations.

      virtual String title() const;
      virtual String artist() const;
      virtual String albumArtist() const;
      virtual String album() const;
      virtual String comment() const;
      virtual String lyrics() const { return String::null; };
      virtual String genre() const;
      virtual String producer() const { return String::null; };
      virtual String composer() const;
      virtual String conductor() const { return String::null; };
      virtual String lyricist() const { return String::null; };
      virtual String recordLabel() const { return String::null; };
      virtual String rating() const { return String::null; };
      virtual String language() const { return String::null; };
      virtual String key() const { return String::null; };
      virtual String license() const { return String::null; };
      virtual String licenseUrl() const { return String::null; };
      virtual uint year() const;
      virtual uint track() const;
      virtual uint totalTracks() const;
      virtual uint disc() const;
      virtual uint totalDiscs() const;
      virtual uint bpm() const;
      virtual bool isCompilation() const { return false; };
    
      virtual void setTitle(const String &s);
      virtual void setArtist(const String &s);
      virtual void setAlbumArtist(const String &s);
      virtual void setAlbum(const String &s);
      virtual void setComment(const String &s);
      virtual void setLyrics(const String &s) {};
      virtual void setGenre(const String &s);
      virtual void setProducer(const String &s) {};
      virtual void setComposer(const String &s);
      virtual void setConductor(const String &s) {};
      virtual void setLyricist(const String &s) {};
      virtual void setRecordLabel(const String &s) {};
      virtual void setRating(const String &s) {};
      virtual void setLanguage(const String &s) {};
      virtual void setKey(const String &s) {};
      virtual void setLicense(const String &s) {};
      virtual void setLicenseUrl(const String &s) {};
      virtual void setYear(uint i);
      virtual void setTrack(uint i);
      virtual void setTotalTracks(uint i);
      virtual void setDisc(uint i);
      virtual void setTotalDiscs(uint i);
      virtual void setBpm(uint i);
      virtual void setIsCompilation(bool i) {};


      // MP4 specific fields

      String     grouping() const;
      ByteVector cover() const;

      void setGrouping(const String &s);
      void setCover( const ByteVector& cover );

      virtual bool isEmpty() const;

    private:
      Tag(const Tag &);
      Tag &operator=(const Tag &);

      class TagPrivate;
      TagPrivate *d;
    };
  } // namespace MP4

} // namespace TagLib

#endif // MP4ITUNESTAG_H
