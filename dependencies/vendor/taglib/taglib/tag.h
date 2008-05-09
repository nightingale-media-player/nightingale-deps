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

#ifndef TAGLIB_TAG_H
#define TAGLIB_TAG_H

#include "taglib_export.h"
#include "tstring.h"

namespace TagLib {

  //! A simple, generic interface to common audio meta data fields

  /*!
   * This is an attempt to abstract away the difference in the meta data formats
   * of various audio codecs and tagging schemes.  As such it is generally a
   * subset of what is available in the specific formats but should be suitable
   * for most applications.  This is meant to compliment the generic APIs found
   * in TagLib::AudioProperties, TagLib::File and TagLib::FileRef.
   */

  class TAGLIB_EXPORT Tag
  {
  public:

    /*!
     * Detroys this Tag instance.
     */
    virtual ~Tag();

    /*!
     * Returns the track name; if no track name is present in the tag
     * String::null will be returned.
     */
    virtual String title() const = 0;

    /*!
     * Returns the artist name; if no artist name is present in the tag
     * String::null will be returned.
     */
    virtual String artist() const = 0;

    /*!
     * Returns the album artist name; if no artist name is present in the tag
     * String::null will be returned.
     */
    virtual String albumArtist() const = 0;

    /*!
     * Returns the album name; if no album name is present in the tag
     * String::null will be returned.
     */
    virtual String album() const = 0;

    /*!
     * Returns the track comment; if no comment is present in the tag
     * String::null will be returned.
     */
    virtual String comment() const = 0;

    /*!
     * Returns the track lyrics; if no lyrics are present in the tag
     * String::null will be returned.
     */
    virtual String lyrics() const = 0;

    /*!
     * Returns the genre name; if no genre is present in the tag String::null
     * will be returned.
     */
    virtual String genre() const = 0;

    /*!
     * Returns the producer name; if no producer is present in the tag String::null
     * will be returned.
     */
    virtual String producer() const = 0;

    /*!
     * Returns the composer name; if no composer is present in the tag String::null
     * will be returned.
     */
    virtual String composer() const = 0;

    /*!
     * Returns the conductor name; if no conductor is present in the tag String::null
     * will be returned.
     */
    virtual String conductor() const = 0;
    
    /*!
     * Returns the lyricist name; if no lyricist is present in the tag String::null
     * will be returned.
     */
    virtual String lyricist() const = 0;

    /*!
     * Returns the record label name; if no record label is present in the tag String::null
     * will be returned.
     */
    virtual String recordLabel() const = 0;

    /*!
     * Returns the rating; if no rating label is present in the tag String::null
     * will be returned.
     * NOTE: rating is a string. no standard format has been specified.
     *       (might be 0-5 / 0-100 / *s, etc.)
     */
    virtual String rating() const = 0;

    /*!
     * Returns the language; if no language is present in the tag String::null
     * will be returned.
     */
    virtual String language() const = 0;
    
    /*!
     * Returns the key; if no key is present in the tag String::null
     * will be returned.
     */
    virtual String key() const = 0;
    
    /*!
     * Returns the license; if no license is present in the tag String::null
     * will be returned.
     */
    virtual String license() const = 0;
    
    /*!
     * Returns the license; if no license is present in the tag String::null
     * will be returned.
     */
    virtual String licenseUrl() const = 0;
    
    /*!
     * Returns the year; if there is no year set, this will return 0.
     */
    virtual uint year() const = 0;

    /*!
     * Returns the track number; if there is no track number set, this will
     * return 0.
     */
    virtual uint track() const = 0;

    /*!
     * Returns the total track number; if there is no total track number set, this will
     * return 0.
     */
    virtual uint totalTracks() const = 0;
    
    /*!
     * Returns the disc; if there is no disc set, this will
     * return 0.
     */
    virtual uint disc() const = 0;

    /*!
     * Returns the total discs; if there is no total discs set, this will
     * return 0.
     */
    virtual uint totalDiscs() const = 0;
    
    /*!
     * Returns the beats per minute; if there is no beats per minute set, this will
     * return 0.
     */
    virtual uint bpm() const = 0;
    
    /*!
     * Returns true if this is part of a complation; if there is no flag set, this will
     * return false.
     */
    virtual bool isCompilation() const = 0;
    
    /*!
     * Sets the title to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setTitle(const String &s) = 0;

    /*!
     * Sets the artist to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setArtist(const String &s) = 0;

    /*!
     * Sets the artist to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setAlbumArtist(const String &s) {}; //TODO: stubs! = 0;

    /*!
     * Sets the album to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setAlbum(const String &s) = 0;

    /*!
     * Sets the comment to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setComment(const String &s) = 0;

    /*!
     * Sets the lyrics to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setLyrics(const String &s) = 0;

    /*!
     * Sets the genre to \a s.  If \a s is String::null then this value will be
     * cleared.  For tag formats that use a fixed set of genres, the appropriate
     * value will be selected based on a string comparison.  A list of available
     * genres for those formats should be available in that type's
     * implementation.
     */
    virtual void setGenre(const String &s) = 0;

    /*!
     * Sets the producer to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setProducer(const String &s) = 0;
    
    /*!
     * Sets the composer to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setComposer(const String &s) = 0;
    
    /*!
     * Sets the conductor to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setConductor(const String &s) = 0;
    
    /*!
     * Sets the lyricist to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setLyricist(const String &s) = 0;
    
    /*!
     * Sets the record label to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setRecordLabel(const String &s) = 0;
    
    /*!
     * Sets the rating to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setRating(const String &s) = 0;
    
    /*!
     * Sets the language to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setLanguage(const String &s) = 0;
    
    /*!
     * Sets the musical key to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setKey(const String &s) = 0;
    
    /*!
     * Sets the license to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setLicense(const String &s) = 0;
    
    /*!
     * Sets the license url to \a s.  If \a s is String::null then this value will be
     * cleared.
     */
    virtual void setLicenseUrl(const String &s) = 0;
    
    /*!
     * Sets the year to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setYear(uint i) = 0;

    /*!
     * Sets the track to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setTrack(uint i) = 0;
    
    /*!
     * Sets the total tracks to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setTotalTracks(uint i) = 0;
    
    /*!
     * Sets the disc to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setDisc(uint i) = 0;

    /*!
     * Sets the track to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setTotalDiscs(uint i) = 0;
    
    /*!
     * Sets the beats per minute to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setBpm(uint i) = 0;
    
    /*!
     * Sets isCompilation to \a i.  If \a s is 0 then this value will be cleared.
     */
    virtual void setIsCompilation(bool i) = 0;
    
    /*!
     * Returns true if the tag does not contain any data.  This should be
     * reimplemented in subclasses that provide more than the basic tagging
     * abilities in this class.
     */
    virtual bool isEmpty() const;

    /*!
     * Copies the generic data from one tag to another.
     *
     * \note This will no affect any of the lower level details of the tag.  For
     * instance if any of the tag type specific data (maybe a URL for a band) is
     * set, this will not modify or copy that.  This just copies using the API
     * in this class.
     *
     * If \a overwrite is true then the values will be unconditionally copied.
     * If false only empty values will be overwritten.
     */
    static void duplicate(const Tag *source, Tag *target, bool overwrite = true);

  protected:
    /*!
     * Construct a Tag.  This is protected since tags should only be instantiated
     * through subclasses.
     */
    Tag();
    
    /*!
     * Render a pair of uints to a "/" separated string.
     * Inputs equal to 0 will be rendered as an empty string for consistency
     * with the Tag interface.
     */
    static String splitNumberRender(uint first, uint second);

  private:
    Tag(const Tag &);
    Tag &operator=(const Tag &);

    class TagPrivate;
    TagPrivate *d;
  };
}

#endif
