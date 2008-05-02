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
      virtual String albumArtist() const { return String::null; };
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
      virtual uint totalTracks() const { return 0; };
      virtual uint disc() const { return 0; };
      virtual uint totalDiscs() const { return 0; };
      virtual uint bpm() const;
      virtual bool isCompilation() const { return false; };
  
  
      virtual void setTitle(const String &s);
      virtual void setArtist(const String &s);
      virtual void setAlbumArtist(const String &s) {};
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
      virtual void setTotalTracks(uint i) {};
      virtual void setDisc(uint i) {};
      virtual void setTotalDiscs(uint i) {};
      virtual void setBpm(uint i);
      virtual void setIsCompilation(bool i) {};
    

      // MP4 specific fields

      String     grouping() const;
      //String     composer() const;
      uint       numTracks() const;
      uint       disk() const;
      uint       numDisks() const;
      //uint       bpm() const;
      ByteVector cover() const;
      
      void setGrouping(const String &s);
      //void setComposer(const String &s);
      // TODO: rename these
      void setNumTracks(const uint i);
      void setDisk(const uint i);
      void setNumDisks(const uint i);
      //void setBpm(const uint i);
      // TODO: bubble this up?
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
