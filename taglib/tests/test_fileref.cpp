#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdio.h>
#include <tag.h>
#include <fileref.h>
#include <oggflacfile.h>
#include <vorbisfile.h>
#include "utils.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;
using namespace TagLib;

class TestFileRef : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestFileRef);
#ifdef TAGLIB_WITH_ASF
  CPPUNIT_TEST(testASF);
#endif
  CPPUNIT_TEST(testMusepack);
  CPPUNIT_TEST(testVorbis);
  CPPUNIT_TEST(testSpeex);
  CPPUNIT_TEST(testFLAC);
  CPPUNIT_TEST(testMP3);
  CPPUNIT_TEST(testOGA_FLAC);
  CPPUNIT_TEST(testOGA_Vorbis);
#ifdef TAGLIB_WITH_MP4
  CPPUNIT_TEST(testMP4_1);
  CPPUNIT_TEST(testMP4_2);
  CPPUNIT_TEST(testMP4_3);
#endif
  CPPUNIT_TEST(testTrueAudio);
  CPPUNIT_TEST_SUITE_END();

public:

  void fileRefSave(const string &filename, const string &ext)
  {
    string newname = copyFile(filename, ext);

    FileRef *f = new FileRef(newname.c_str());
    CPPUNIT_ASSERT(!f->isNull());
    f->tag()->setArtist("test artist");
    f->tag()->setTitle("test title");
    f->tag()->setAlbumArtist("test album artist");
    f->tag()->setComment("test comment");
    f->tag()->setGenre("Test!");
    f->tag()->setAlbum("albummmm");
    f->tag()->setComposer("composer");
    f->tag()->setConductor("conductor");
    f->tag()->setProducer("the pro");
    f->tag()->setLyrics("lyrics");
    f->tag()->setLyricist("lyricist");
    
    f->tag()->setLicense("license");
    //f->tag()->setLicenseUrl("licenseURL");
    
    f->tag()->setBpm(121);
    f->tag()->setDisc(2);
    
    f->tag()->setTrack(5);
    f->tag()->setYear(2020);
    f->save();
    
    delete f;

    f = new FileRef(newname.c_str());
    CPPUNIT_ASSERT(!f->isNull());
    CPPUNIT_ASSERT_EQUAL(f->tag()->artist(), String("test artist"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->title(), String("test title"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->albumArtist(), String("test album artist"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->genre(), String("Test!"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->album(), String("albummmm"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->composer(), String("composer"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->conductor(), String("conductor"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->producer(), String("the pro"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->lyrics(), String("lyrics"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->comment(), String("test comment"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->lyricist(), String("lyricist"));
    
    CPPUNIT_ASSERT_EQUAL(f->tag()->license(), String("license"));
    // CPPUNIT_ASSERT_EQUAL(f->tag()->licenseUrl(), String("licenseURL"));
    
    CPPUNIT_ASSERT_EQUAL(f->tag()->bpm(), TagLib::uint(121));
    CPPUNIT_ASSERT_EQUAL(f->tag()->disc(), TagLib::uint(2));    
    CPPUNIT_ASSERT_EQUAL(f->tag()->track(), TagLib::uint(5));
    CPPUNIT_ASSERT_EQUAL(f->tag()->year(), TagLib::uint(2020));
    f->tag()->setArtist("ttest artist");
    f->tag()->setTitle("ytest title");
    f->tag()->setGenre("uTest!");
    f->tag()->setAlbum("ialbummmm");
    f->tag()->setTrack(7);
    f->tag()->setDisc(3);
        
    f->tag()->setComposer("icomposer");
    f->tag()->setConductor("iconductor");
    f->tag()->setProducer("ithe pro");
    f->tag()->setLyrics("ilyrics");
    f->tag()->setComment("itest comment");
    f->tag()->setLyricist("ilyricist");
    
    f->tag()->setLicense("ilicense");
    // f->tag()->setLicenseUrl("ilicenseURL");
    
    f->tag()->setBpm(42);
    
    f->tag()->setYear(2080);
    f->save();
    delete f;

    f = new FileRef(newname.c_str());
    CPPUNIT_ASSERT(!f->isNull());
    CPPUNIT_ASSERT_EQUAL(f->tag()->artist(), String("ttest artist"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->title(), String("ytest title"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->genre(), String("uTest!"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->album(), String("ialbummmm"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->track(), TagLib::uint(7));
    
    CPPUNIT_ASSERT_EQUAL(f->tag()->composer(), String("icomposer"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->conductor(), String("iconductor"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->producer(), String("ithe pro"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->lyrics(), String("ilyrics"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->comment(), String("itest comment"));
    CPPUNIT_ASSERT_EQUAL(f->tag()->lyricist(), String("ilyricist"));

    CPPUNIT_ASSERT_EQUAL(f->tag()->license(), String("ilicense"));
    // CPPUNIT_ASSERT_EQUAL(f->tag()->licenseUrl(), String("ilicenseURL"));

    CPPUNIT_ASSERT_EQUAL(f->tag()->bpm(), TagLib::uint(42));
    CPPUNIT_ASSERT_EQUAL(f->tag()->disc(), TagLib::uint(3));
    CPPUNIT_ASSERT_EQUAL(f->tag()->year(), TagLib::uint(2080));
    delete f;

    deleteFile(newname);
  }

  void testMusepack()
  {
    // TODO: uncomment this
    //fileRefSave("click", ".mpc");
  }

#ifdef TAGLIB_WITH_ASF
  void testASF()
  {
    fileRefSave("silence-1", ".wma");
  }
#endif

  void testVorbis()
  {
    fileRefSave("empty", ".ogg");
  }

  void testSpeex()
  {
    fileRefSave("empty", ".spx");
  }

  void testFLAC()
  {
    fileRefSave("no-tags", ".flac");
  }

  void testMP3()
  {
    fileRefSave("xing", ".mp3");
  }

  void testTrueAudio()
  {
    fileRefSave("empty", ".tta");
  }

#ifdef TAGLIB_WITH_MP4
  void testMP4_1()
  {
    fileRefSave("has-tags", ".m4a");
  }

  void testMP4_2()
  {
    fileRefSave("no-tags", ".m4a");
  }

  void testMP4_3()
  {
    fileRefSave("no-tags", ".3g2");
  }
#endif

  void testOGA_FLAC()
  {
      FileRef *f = new FileRef("data/empty_flac.oga");
      CPPUNIT_ASSERT(dynamic_cast<Ogg::Vorbis::File *>(f->file()) == NULL);
      CPPUNIT_ASSERT(dynamic_cast<Ogg::FLAC::File *>(f->file()) != NULL);
  }

  void testOGA_Vorbis()
  {
      FileRef *f = new FileRef("data/empty_vorbis.oga");
      CPPUNIT_ASSERT(dynamic_cast<Ogg::Vorbis::File *>(f->file()) != NULL);
      CPPUNIT_ASSERT(dynamic_cast<Ogg::FLAC::File *>(f->file()) == NULL);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestFileRef);
