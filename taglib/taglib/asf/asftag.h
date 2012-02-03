/**************************************************************************
    copyright            : (C) 2005-2007 by Lukáš Lalinský
    email                : lalinsky@gmail.com
 **************************************************************************/

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

#ifndef TAGLIB_ASFTAG_H
#define TAGLIB_ASFTAG_H

#include "tag.h"
#include "tlist.h"
#include "tmap.h"
#include "taglib_export.h"
#include "asfattribute.h"

namespace TagLib {

  namespace ASF {

    typedef List<Attribute> AttributeList;
    typedef Map<String, AttributeList> AttributeListMap;

    class TAGLIB_EXPORT Tag : public TagLib::Tag {

      friend class File;

    public:

      Tag();

      virtual ~Tag();

      // Reimplementations.
      virtual String title() const;
      virtual String artist() const;
      virtual String albumArtist() const;
      virtual String album() const;
      virtual String comment() const;
      virtual String lyrics() const;
      virtual String genre() const;
      virtual String producer() const;
      virtual String composer() const;
      virtual String conductor() const;
      virtual String lyricist() const;
      virtual String recordLabel() const;
      virtual String rating() const;
      virtual String language() const { return String::null; }; // xx todo
      virtual String key() const { return String::null; };      /// todo
      virtual String license() const;
      virtual String licenseUrl() const { return String::null; }; // todo
      virtual uint year() const;
      virtual uint track() const;
      virtual uint totalTracks() const { return 0; }; // todo
      virtual uint disc() const;
      virtual uint totalDiscs() const { return 0; }; // todo
      virtual uint bpm() const;
      virtual bool isCompilation() const { return false; }; // todo
  
  
      virtual void setTitle(const String &s);
      virtual void setArtist(const String &s);
      virtual void setAlbumArtist(const String &s);
      virtual void setAlbum(const String &s);
      virtual void setComment(const String &s);
      virtual void setLyrics(const String &s);
      virtual void setGenre(const String &s);
      virtual void setProducer(const String &s);
      virtual void setComposer(const String &s);
      virtual void setConductor(const String &s);
      virtual void setLyricist(const String &s);
      virtual void setRecordLabel(const String &s);
      virtual void setRating(const String &s);
      virtual void setLanguage(const String &s) {};
      virtual void setKey(const String &s) {};
      virtual void setLicense(const String &s);
      virtual void setLicenseUrl(const String &s) {};
      virtual void setYear(uint i);
      virtual void setTrack(uint i);
      virtual void setTotalTracks(uint i) {};
      virtual void setDisc(uint i);
      virtual void setTotalDiscs(uint i) {};
      virtual void setBpm(uint i);
      virtual void setIsCompilation(bool i) {};

      /*!
       * Returns true if the tag does not contain any data.  This should be
       * reimplemented in subclasses that provide more than the basic tagging
       * abilities in this class.
       */
      virtual bool isEmpty() const;

      /*!
       * Returns a reference to the item list map.  This is an AttributeListMap of
       * all of the items in the tag.
       *
       * This is the most powerfull structure for accessing the items of the tag.
       */
      AttributeListMap &attributeListMap();

      /*!
       * Removes the \a key attribute from the tag
       */
      void removeItem(const String &name);

      /*!
       * Sets the \a key attribute to the value of \a attribute. If an attribute
       * with the \a key is already present, it will be replaced.
       */
      void setAttribute(const String &name, const Attribute &attribute);

      /*!
       * Sets the \a key attribute to the value of \a attribute. If an attribute
       * with the \a key is already present, it will be added to the list.
       */
      void addAttribute(const String &name, const Attribute &attribute);

    private:

      class TagPrivate;
      TagPrivate *d;
    };
  }
}
#endif
