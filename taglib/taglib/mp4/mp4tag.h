/**************************************************************************
    copyright            : (C) 2007 by Lukáš Lalinský
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

#ifndef TAGLIB_MP4TAG_H
#define TAGLIB_MP4TAG_H

#include "tag.h"
#include "tbytevectorlist.h"
#include "tfile.h"
#include "tmap.h"
#include "tstringlist.h"
#include "taglib_export.h"
#include "mp4atom.h"
#include "mp4item.h"

namespace TagLib {

  namespace MP4 {

    typedef TagLib::Map<String, Item> ItemListMap;

    class TAGLIB_EXPORT Tag: public TagLib::Tag
    {
    public:
        Tag(TagLib::File *file, Atoms *atoms);
        ~Tag();
        bool save();

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
      virtual String rating() const { return String::null; };
      virtual String language() const { return String::null; };
      virtual String key() const { return String::null; };
      virtual String license() const;
      virtual String licenseUrl() const { return String::null; };
      virtual uint year() const;
      virtual uint track() const;
      virtual uint totalTracks() const { return 0; };
      virtual uint disc() const;
      virtual uint totalDiscs() const { return 0; };
      virtual uint bpm() const;
      virtual bool isCompilation() const { return false; };
  
  
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
      virtual void setRating(const String &s) {};
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

        ItemListMap &itemListMap();

    private:
        TagLib::ByteVectorList parseData(Atom *atom, TagLib::File *file, int expectedFlags = -1, bool freeForm = false);
        void parseText(Atom *atom, TagLib::File *file, int expectedFlags = 1);
        void parseFreeForm(Atom *atom, TagLib::File *file);
        void parseInt(Atom *atom, TagLib::File *file);
        void parseGnre(Atom *atom, TagLib::File *file);
        void parseIntPair(Atom *atom, TagLib::File *file);
        void parseBool(Atom *atom, TagLib::File *file);
        void parseCovr(Atom *atom, TagLib::File *file);

        TagLib::ByteVector padIlst(const ByteVector &data, int length = -1);
        TagLib::ByteVector renderAtom(const ByteVector &name, const TagLib::ByteVector &data);
        TagLib::ByteVector renderData(const ByteVector &name, int flags, const TagLib::ByteVectorList &data);
        TagLib::ByteVector renderText(const ByteVector &name, Item &item, int flags = 1);
        TagLib::ByteVector renderFreeForm(const String &name, Item &item);
        TagLib::ByteVector renderBool(const ByteVector &name, Item &item);
        TagLib::ByteVector renderInt(const ByteVector &name, Item &item);
        TagLib::ByteVector renderIntPair(const ByteVector &name, Item &item);
        TagLib::ByteVector renderIntPairNoTrailing(const ByteVector &name, Item &item);
        TagLib::ByteVector renderCovr(const ByteVector &name, Item &item);

        void updateParents(AtomList &path, long delta, int ignore = 0);
        void updateOffsets(long delta, unsigned long offset);

        void saveNew(TagLib::ByteVector &data);
        void saveExisting(TagLib::ByteVector &data, AtomList &path);

        class TagPrivate;
        TagPrivate *d;
    };

  }

}

#endif
