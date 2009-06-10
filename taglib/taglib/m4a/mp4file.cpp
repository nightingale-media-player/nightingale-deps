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

#include <tbytevector.h>
#include <tstring.h>
#include <tdebug.h>
#include "tlist.h"
#include <vector>
#include <set>
#include <tlocalfileio.h>

#include "id3v1genres.h"

#include "mp4itunestag.h"
#include "mp4file.h"
#include "boxfactory.h"
#include "mp4tagsproxy.h"
#include "mp4propsproxy.h"
#include "mp4audioproperties.h"
#include "mp4metadatabox.h"
#include "mp4ilstbox.h"
#include "itunesdatabox.h"

using namespace TagLib;

class MP4::File::FilePrivate
{
public:
  //! list for all boxes of the mp4 file
  TagLib::List<MP4::Mp4IsoBox*> boxes;
  //! proxy for the tags is filled after parsing
  MP4::Mp4TagsProxy           tagsProxy;
  //! proxy for audio properties
  MP4::Mp4PropsProxy          propsProxy;
  //! the tag returned by tag() function
  MP4::Tag                    mp4tag;
  //! container for the audio properties returned by properties() function
  MP4::AudioProperties        mp4audioproperties;
  //! is set to valid after successful parsing
  bool                        isValid;
  //! the name to use as a scratch file
  FileName                    scratchName;

  FilePrivate() : scratchName(""){}

  //! function to adjust offsets in any tables in the file
  //! \param box The box that will change in length
  //! \param difference The number of bytes the box will grow
  bool adjustOffsets( TagLib::FileIO* file,
                      MP4::Mp4IsoBox* box,
                      TagLib::ulonglong difference );

  class Mp4FileBox : public Mp4IsoBox
  {
  public:
    Mp4FileBox( MP4::File* file, FilePrivate* privateData )
      : Mp4IsoBox( file, Fourcc(), 0, 0 ),
        d( privateData )
    {
    }
    virtual void parse() {}
    virtual ByteVector render() 
    {
      return ByteVector();
    }
    virtual Mp4IsoBox* getChildBox( MP4::Fourcc fourcc, Mp4IsoBox* offset = NULL ) const
    {
      TagLib::List<Mp4IsoBox*>::Iterator iter = d->boxes.begin();
      if ( offset )
      {
        for ( ; *iter != offset; ++iter )
          if ( iter == d->boxes.end() )
            return NULL;
        ++iter; // go past the offset
      }
      for ( ; iter != d->boxes.end(); ++iter )
      {
        if ( !fourcc || fourcc == (*iter)->fourcc() )
          return *iter;
      }
      return NULL;
    }
  private:
    FilePrivate* d;
  };
  Mp4FileBox *fileBox;
};

//! function to fill the tags with converted proxy data, which has been parsed out of the file previously
static void fillTagFromProxy( MP4::Mp4TagsProxy& proxy, MP4::Tag& mp4tag );

//! function to create a ByteVector for the given box
//! \return false to copy the original box, true to use the new data
//! \param fourcc the fourcc to look at
//! \param proxy the proxy to read old tags from
//! \param mp4tag the tags to read new tags from
//! \param newData [out] the new data; may be ByteVector::null to indicate the box should be deleted
//! \param oldData [out] if not null, the old data that existed
static bool rebuildDataForBox( const MP4::Fourcc fourcc,
                               MP4::Mp4TagsProxy* const proxy,
                               MP4::Tag* mp4tag,
                               ByteVector& newData );

MP4::File::File() : TagLib::File()
{
  // create member container
  d = new MP4::File::FilePrivate();
  d->isValid = false;
  d->fileBox = NULL;
}

MP4::File::File( FileName file, bool , AudioProperties::ReadStyle,
                 FileName scratchFile )
	:TagLib::File( file )
{
  // create member container
  d = new MP4::File::FilePrivate();
  d->fileBox = NULL;
  d->scratchName = scratchFile;

  read();
}

MP4::File::~File()
{
  TagLib::List<Mp4IsoBox*>::Iterator delIter;
  for( delIter  = d->boxes.begin();
       delIter != d->boxes.end();
       delIter++ )
  {
    delete *delIter;
  }
  if (d->fileBox)
    delete d->fileBox;
  delete d;
}

Tag *MP4::File::tag() const
{
  return &d->mp4tag;
}

AudioProperties * MP4::File::audioProperties() const
{
  d->mp4audioproperties.setProxy( &d->propsProxy );
  return &d->mp4audioproperties;
}

void MP4::File::read( bool, TagLib::AudioProperties::ReadStyle  )
{
  d->isValid = false;
  TagLib::uint size;
  MP4::Fourcc  fourcc;

  while( readSizeAndType( size, fourcc ) == true )
  {
    // create the appropriate subclass and parse it
    MP4::Mp4IsoBox* curbox = MP4::BoxFactory::createInstance( this, fourcc, size, tell() );
    curbox->parsebox();
    d->boxes.append( curbox );
  }

  for( TagLib::List<MP4::Mp4IsoBox*>::Iterator iter  = d->boxes.begin();
                                               iter != d->boxes.end();
					       iter++ )
  {
    if( (*iter)->fourcc() == MP4::Fourcc("moov") )
    {
      d->isValid = true;
      break;
    }
  }

  if( d->isValid )
    debug( "file is valid" );
  else
    debug( "file is NOT valid" );

  // fill tags from proxy data
  fillTagFromProxy( d->tagsProxy, d->mp4tag );
}

bool MP4::File::save()
{
  Mp4IsoBox* moovBox = NULL;
  for( TagLib::List<MP4::Mp4IsoBox*>::Iterator iter  = d->boxes.begin();
                                               iter != d->boxes.end();
                                               ++iter )
  {
    if( (*iter)->fourcc() == MP4::Fourcc("moov") )
    {
      moovBox = *iter;
      break;
    }
  }
  if (!moovBox)
    return false;
  Mp4IsoBox* udtaBox = moovBox->getChildBox(MP4::Fourcc("udta"));
  if (!udtaBox)
    return false;
  Mp4IsoBox* metaBox = udtaBox->getChildBox(MP4::Fourcc("meta"));
  if (!metaBox)
    return false;
  Mp4IsoBox* ilstIsoBox = metaBox->getChildBox(MP4::Fourcc("ilst"));
  if (!ilstIsoBox)
    return false;
  Mp4IlstBox* ilstBox = dynamic_cast<Mp4IlstBox*>(ilstIsoBox);
  if (!ilstBox)
  {
    debug( "got an ilst box that is not a Mp4IlstBox!  Bailing from MP4::File::save()" );
    return false;
  }

  // get the box directly after the ilst box, in the hopes of finding a free box to clobber
  Mp4IsoBox* ilstSiblingBox = metaBox->getChildBox( MP4::Fourcc(), ilstBox );
  
  // keep track of which tags we've handled
  std::set<MP4::Fourcc> seenFourccs;

  TagLib::List<MP4::Mp4IsoBox*> newList;
  Mp4IsoBox* box = NULL;
  bool hasAnyChanged = false;
  bool boxChanged = false;
  ByteVector newData;
  Fourcc fourcc;
  while ( (box = ilstBox->getChildBox(MP4::Fourcc(), box) ) )
  {
    fourcc = box->fourcc();
    seenFourccs.insert( fourcc );
    boxChanged = rebuildDataForBox( fourcc, &d->tagsProxy, &d->mp4tag, newData );
    if ( boxChanged )
    {
      hasAnyChanged = true;
      if ( newData.isNull() )
      {
        // delete this box
        continue;
      }
      MP4::Mp4MetadataBox* metabox = dynamic_cast<MP4::Mp4MetadataBox*>(box);
      if (metabox)
      {
        metabox->data()->setData( newData );
      }
      else
      {
        debug( "ilst has a child box that isn't a Mp4MetadataBox!" );
        return false;
      }
    }

    newList.append( box );
  }

  // append new metadata
  for ( unsigned int i = 0; i < MP4::Mp4TagsProxy::END; ++i )
  {
    fourcc = d->tagsProxy.getFourccForType( (Mp4TagsProxy::EBoxType)i );
    if ( !fourcc )
    {
      // this tag has no useful fourcc, we can't write it anyway
      continue;
    }
    if ( seenFourccs.find(fourcc) != seenFourccs.end() )
    {
      // already saw this one
      continue;
    }
    Mp4IsoBox* isobox = MP4::BoxFactory::createInstance( this, fourcc, 0, 0 );
    Mp4MetadataBox* box = dynamic_cast<Mp4MetadataBox*>(isobox);
    if (box)
    {
      boxChanged = rebuildDataForBox( fourcc, &d->tagsProxy, &d->mp4tag, newData );
      if ( boxChanged && !newData.isNull() )
      {
        Mp4IsoBox* newBox = MP4::BoxFactory::createInstance( this, 
                                                             TAGLIB_FOURCC('d','a','t','a'), 
                                                             0, 
                                                             0 );
        ITunesDataBox* data = dynamic_cast<ITunesDataBox*>(newBox);
        if ( !data )
        {
          debug( "we created a data box that's not an ITunesDataBox!" );
          delete newBox;
          delete isobox;
        }
        else
        {
          hasAnyChanged = true;
          data->setData( newData );
          box->setData( data );
          newList.append( box );
          ilstBox->addChildBox( box );
        }
      }
      else
      {
        delete isobox;
      }
    }
    else
    {
      debug( "somehow we got a metadata box that isn't a Mp4MetadataBox!" );
      delete isobox;
    }
  }

  if ( !hasAnyChanged )
  {
    // we have successfully decided nothing of interest has changed.
    return true;
  }

  // okay, now we need to serialize it all
  ByteVector outData;
  outData.append( ByteVector::fromUInt(0) ); // size, stubbed out for now
  outData.append( ByteVector::fromUInt( ilstBox->fourcc() ) );

  for ( TagLib::List<Mp4IsoBox*>::ConstIterator iter  = newList.begin();
                                                iter != newList.end();
                                                ++iter )
  {
    outData.append( (*iter)->render() );
  }
  ulonglong newSize = outData.size();
  ulonglong oldSize = ilstBox->size();

  ilstBox->setSize( newSize );
  
  if ( ilstSiblingBox )
  {
    switch ( ilstSiblingBox->fourcc() )
    {
      case TAGLIB_FOURCC('f','r','e','e'):
      case TAGLIB_FOURCC('s','k','i','p'):
        // we can eat a sibling free box, yay!
        oldSize += ilstSiblingBox->size();
        break;
      default:
        break;
    }
  }

  outData[0] = (newSize >> 24) & 0xFF;
  outData[1] = (newSize >> 16) & 0xFF;
  outData[2] = (newSize >>  8) & 0xFF;
  outData[3] = (newSize >>  0) & 0xFF;

  // check if there's room for a new "free" block
  // (at least 4 bytes size + 4 bytes fourcc)
  if ( oldSize > newSize + 8 )
  {
    // insert a "free" block in the list
    // (this block lives outside the ilst)
    outData.append( ByteVector::fromUInt(oldSize - newSize) );
    outData.append( ByteVector::fromCString("free") );
    outData.resize( oldSize );
    newSize = outData.size(); // adjust the size
  }

  TagLib::FileIO *file = this;

  if ( oldSize != newSize )
  {
    if ( !d->fileBox )
      d->fileBox = new MP4::File::FilePrivate::Mp4FileBox(this, this->d);

    ulonglong difference = newSize - oldSize;
    // XXX Mook: TODO: make this fail safer (use a scratch file)
    bool success = d->adjustOffsets( file, ilstBox, difference );
    if ( ! success )
    {
      debug("Failed to adjust the file; it is now corrupt!");
      return false;
    }
    // update the parent box sizes
    file->seek( moovBox->offset() - 8 ); // -8 = size + fourcc
    file->writeBlock( ByteVector::fromUInt( moovBox->size() + difference) );
    file->seek( udtaBox->offset() - 8 ); // -8 = size + fourcc
    file->writeBlock( ByteVector::fromUInt( udtaBox->size() + difference) );
    file->seek( metaBox->offset() - 8 ); // offset() only excludes size + fourcc, not iso box header
    file->writeBlock( ByteVector::fromUInt( metaBox->size() + difference) );
    file->insert( ByteVector( (uint)difference ), ilstBox->offset() );
  }

  // good, the ilst is big enough
  // since offset() returns the offset to start of data excluding the box header,
  // subtract 8 from it so we can write out size + fourcc
  file->seek( ilstBox->offset() - 8, File::Beginning );
  file->writeBlock( outData );
  return true;
}

void MP4::File::remove()
{
}

TagLib::uint MP4::File::readSystemsLen()
{
  TagLib::uint length = 0;
  TagLib::uint nbytes = 0;
  ByteVector   input;
  TagLib::uchar tmp_input;

  do
  {
    input = readBlock(1);
    tmp_input = static_cast<TagLib::uchar>(input[0]);
    nbytes++;
    length = (length<<7) | (tmp_input&0x7F);
  } while( (tmp_input&0x80) && (nbytes<4) );

  return length;
}

bool MP4::File::readSizeAndType( TagLib::uint& size, MP4::Fourcc& fourcc )
{
  // read the two blocks from file
  ByteVector readsize = readBlock(4);
  ByteVector readtype = readBlock(4);

  if( (readsize.size() != 4) || (readtype.size() != 4) )
    return false;

  // set size
  size = static_cast<unsigned char>(readsize[0]) << 24 |
         static_cast<unsigned char>(readsize[1]) << 16 |
         static_cast<unsigned char>(readsize[2]) <<  8 |
         static_cast<unsigned char>(readsize[3]);
  if (size == 0)
    return false;

  // set fourcc
  fourcc = readtype.data();

  return true;
}

bool MP4::File::readInt( TagLib::uint& toRead )
{
  ByteVector readbuffer = readBlock(4);
  if( readbuffer.size() != 4 )
    return false;

  toRead = static_cast<unsigned char>(readbuffer[0]) << 24 |
           static_cast<unsigned char>(readbuffer[1]) << 16 |
           static_cast<unsigned char>(readbuffer[2]) <<  8 |
           static_cast<unsigned char>(readbuffer[3]);
  return true;
}

bool MP4::File::readShort( TagLib::uint& toRead )
{
  ByteVector readbuffer = readBlock(2);
  if( readbuffer.size() != 2 )
    return false;

  toRead = static_cast<unsigned char>(readbuffer[0]) <<  8 |
           static_cast<unsigned char>(readbuffer[1]);
  return true;
}

bool MP4::File::readLongLong( TagLib::ulonglong& toRead )
{
  ByteVector readbuffer = readBlock(8);
  if( readbuffer.size() != 8 )
    return false;

  toRead = static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[0])) << 56 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[1])) << 48 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[2])) << 40 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[3])) << 32 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[4])) << 24 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[5])) << 16 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[6])) <<  8 |
           static_cast<ulonglong>(static_cast<unsigned char>(readbuffer[7]));
  return true;
}

bool MP4::File::readFourcc( TagLib::MP4::Fourcc& fourcc )
{
  ByteVector readtype = readBlock(4);

  if( readtype.size() != 4)
    return false;

  // set fourcc
  fourcc = readtype.data();

  return true;
}

MP4::Mp4TagsProxy* MP4::File::tagProxy() const
{
  return &d->tagsProxy;
}

MP4::Mp4PropsProxy* MP4::File::propProxy() const
{
  return &d->propsProxy;
}


/* This function has been updated based on information at */
/* "http://atomicparsley.sourceforge.net/mpeg-4files.html". */
void fillTagFromProxy( MP4::Mp4TagsProxy& proxy, MP4::Tag& mp4tag )
{
  // tmp buffer for each tag
  MP4::ITunesDataBox* databox;

  databox = proxy.titleData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setTitle( datastring );
  }

  databox = proxy.artistData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setArtist( datastring );
  }

  databox = proxy.albumArtistData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setAlbumArtist( datastring );
  }

  databox = proxy.albumData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setAlbum( datastring );
  }

  databox = proxy.genreData();
  if( databox != 0 )
  {
    if (databox->flags() == 0)
    {
      // convert data to uint
      TagLib::ByteVector datavec = databox->data();
      int genreVal = static_cast<int>( datavec[1] );
      if (genreVal > 0)
      {
        TagLib::String datastring = ID3v1::genre( genreVal - 1 );
        // check if string was set
        if( !(datastring == "") )
          mp4tag.setGenre( datastring );
      }
    }
    else
    {
      // convert data to string
      TagLib::String datastring( databox->data(), String::UTF8 );
      // check if string was set
      if( !(datastring == "") )
        mp4tag.setGenre( datastring );
    }
  }

  databox = proxy.yearData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setYear( datastring.toInt() );
  }

  databox = proxy.trknData();
  if( databox != 0 )
  {
    // convert data to uint
    TagLib::ByteVector datavec = databox->data();
    if( datavec.size() >= 6 )
    {
      TagLib::uint notracks = static_cast<TagLib::uint>( datavec[5] );
      mp4tag.setTotalTracks( notracks );
    }
    if( datavec.size() >= 4 )
    {
      TagLib::uint trackno = static_cast<TagLib::uint>( datavec[3] );
      mp4tag.setTrack( trackno );
    }
    else
      mp4tag.setTrack( 0 );
  }

  databox = proxy.commentData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setComment( datastring );
  }

  databox = proxy.groupingData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setGrouping( datastring );
  }

  databox = proxy.composerData();
  if( databox != 0 )
  {
    // convert data to string
    TagLib::String datastring( databox->data(), String::UTF8 );
    // check if string was set
    if( !(datastring == "") )
      mp4tag.setComposer( datastring );
  }

  databox = proxy.diskData();
  if( databox != 0 )
  {
    // convert data to uint
    TagLib::ByteVector datavec = databox->data();
    if( datavec.size() >= 6 )
    {
      TagLib::uint nodiscs = static_cast<TagLib::uint>( datavec[5] );
      mp4tag.setTotalDiscs( nodiscs );
    }
    if( datavec.size() >= 4 )
    {
      TagLib::uint discno = static_cast<TagLib::uint>( datavec[3] );
      mp4tag.setDisc( discno );
    }
    else
      mp4tag.setDisc( 0 );
  }

  databox = proxy.bpmData();
  if( databox != 0 )
  {
    // convert data to uint
    TagLib::ByteVector datavec = databox->data();

    if( datavec.size() >= 2 )
    {
      TagLib::uint bpm = static_cast<TagLib::uint>( static_cast<unsigned char>(datavec[0]) <<  8 |
	                                            static_cast<unsigned char>(datavec[1]) );
      mp4tag.setBpm( bpm );
    }
    else
      mp4tag.setBpm( 0 );
  }

  databox = proxy.coverData();
  if( databox != 0 )
  {
    // get byte vector
    mp4tag.setCover( databox->data() );
  }
}

static bool rebuildDataForBox( MP4::Fourcc fourcc,
                               MP4::Mp4TagsProxy* const proxy,
                               MP4::Tag* mp4tag,
                               ByteVector& newData )
{
  // see if it's something we changed
  MP4::Mp4TagsProxy::EBoxType boxType = MP4::Mp4TagsProxy::END;
  TagLib::uint firstInt, secondInt;
  MP4::ITunesDataBox* oldData = NULL;
  enum mode_t {
    MODE_PASSTHROUGH,
    MODE_BINARY,
    MODE_INT_PAIR
  } mode = MODE_PASSTHROUGH;
  
  newData.clear();
  
  switch (fourcc) 
  {
    case TAGLIB_FOURCC(0xA9,'n','a','m'):
      newData = mp4tag->title().data(String::UTF8);
      oldData = proxy->titleData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::title;
      break;
    case TAGLIB_FOURCC(0xA9,'A','R','T'):
      newData = mp4tag->artist().data(String::UTF8);
      oldData = proxy->artistData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::artist;
      break;
    case TAGLIB_FOURCC(0xA9,'a','l','b'):
      newData = mp4tag->album().data(String::UTF8);
      oldData = proxy->albumData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::album;
      break;
    case TAGLIB_FOURCC('c','o','v','r'):
      newData = mp4tag->cover();
      oldData = proxy->coverData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::cover;
      break;
    case TAGLIB_FOURCC('g','n','r','e'):
      newData = mp4tag->genre().data(String::UTF8);
      oldData = proxy->genreData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::genre;
      break;
    case TAGLIB_FOURCC(0xA9,'d','a','y'):
    {
      TagLib::uint year = mp4tag->year();
      boxType = MP4::Mp4TagsProxy::year;
      if (year == 0)
      {
        // delete the year, if it exists
        newData.clear();
        mode = MODE_BINARY;
      }
      else
      {
        // While the creation date format can be really complicated, it is also
        // acceptable to just have a simple string of the year.  Since we only
        // have the year anyway, just use that.
        TagLib::uint oldYear = 0;
        oldData = proxy->yearData();
        if ( oldData )
        {
          oldYear = String(oldData->data(), String::Latin1).toInt();
        }
        if (year == oldYear)
        {
          // it didn't change; keep the box as-is to try to preserve data
          mode = MODE_PASSTHROUGH;
        }
        else
        {
          // we are modifying the data.  Make up the string.
          String tempString = String::number(year);
          if ( tempString.length() < 4 )
          {
            // need to pad
            newData = ByteVector(4 - tempString.length(), '0');
          }
          newData.append( tempString.data( String::UTF8 ) );
          mode = MODE_BINARY;
        }
      }
      break;
    }
    case TAGLIB_FOURCC('t','r','k','n'):
      firstInt = mp4tag->track();
      secondInt = mp4tag->totalTracks();
      oldData = proxy->trknData();
      mode = MODE_INT_PAIR;
      boxType = MP4::Mp4TagsProxy::trackno;
      break;
    case TAGLIB_FOURCC(0xA9,'c','m','t'):
      newData = mp4tag->comment().data(String::UTF8);
      oldData = proxy->commentData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::comment;
      break;
    case TAGLIB_FOURCC(0xA9,'g','r','p'):
      newData = mp4tag->grouping().data(String::UTF8);
      oldData = proxy->groupingData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::grouping;
      break;
    case TAGLIB_FOURCC(0xA9,'w','r','t'):
      newData = mp4tag->composer().data(String::UTF8);
      oldData = proxy->composerData();
      mode = MODE_BINARY;
      boxType = MP4::Mp4TagsProxy::composer;
      break;
    case TAGLIB_FOURCC('d','i','s','k'):
      firstInt = mp4tag->disc();
      secondInt = mp4tag->totalDiscs();
      oldData = proxy->diskData();
      mode = MODE_INT_PAIR;
      boxType = MP4::Mp4TagsProxy::disk;
      break;
    case TAGLIB_FOURCC('t','m','p','o'):
    {
      boxType = MP4::Mp4TagsProxy::bpm;
      uint bpm = mp4tag->bpm();
      if (bpm == 0)
      {
        // delete the tempo, if it exists
        mode = MODE_BINARY;
      }
      else
      {
        // this is a 16-bit uint
        oldData = proxy->bpmData();
        uint oldBpm = oldData->data().toShort( true );
        if ( bpm == oldBpm )
        {
          // didn't change, try to preserve the data
          mode = MODE_PASSTHROUGH;
        }
        else
        {
          // we are modifying the data
          newData = ByteVector::fromShort( oldBpm, true );
          mode = MODE_BINARY;
        }
      }
    }
    case TAGLIB_FOURCC('f','r','e','e'):
    case TAGLIB_FOURCC('s','k','i','p'):
      // drop free/skip boxes
      return false;
  }
  
  switch (mode)
  {
  case MODE_PASSTHROUGH:
    return false;
  case MODE_INT_PAIR:
    {
      if ( firstInt )
      {
        newData = ByteVector(2, '\0'); // two bytes of null, unknown.
        newData.append( ByteVector::fromShort( firstInt, true ) );
        if ( secondInt != 0 )
        {
          newData.append( ByteVector::fromShort( secondInt, true ) );
        }
      }
    }
    // fall through
  case MODE_BINARY:
    {
      if ( newData.isEmpty() )
      {
        // delete the old data
        newData = ByteVector::null;
        return true;
      }
      else
      {
        // check if old == new
        if ( oldData && (newData == oldData->data()) )
        {
          // has old data, which is the same as new data
          // don't touch anything
          return false;
        }
      }
      break;
    }
  }
  return true;
}

//! This is a helper class to find boxes following a specified chain of FourCCs.
//! The path must be followed exactly (there can be no skips, even at the top of
//! the chain).  Use this like a STL iterator to find multiple boxes in the file
//! which satisfy the parameters.
class ChildIterator {
public:
  //! \param box the box to act as the root of the chain
  ChildIterator(MP4::Mp4IsoBox* box)
    : m_hasInited(false)
  {
    m_stack.push_back( box );
  }

  //! Add a new FourCC to the chain
  //! \param fourcc The FourCC to add
  //! \note This does not expect to be called once iteration has started
  void addFourcc( const MP4::Fourcc fourcc )
  {
    m_fourccs.push_back( fourcc );
  }
  
  //! Find the next box
  ChildIterator& operator++() {
    if ( m_hasInited )
    {
      // already initialized
      // need to push a null, so we will step to the next item
      // in the not initialized case, we need to look at the children of the
      // top element, and not try to skip it
      if ( m_stack.back() )
      {
        m_stack.push_back( NULL );
      }
    }

    do
    {
      if ( !m_stack.back() )
      {
        // the last search found nothing, pop back up
        m_stack.pop_back();
        if ( m_stack.size() > 1 )
        {
          MP4::Mp4IsoBox* back = m_stack.back();
          m_stack.pop_back();
          MP4::Fourcc fourcc = m_fourccs[ m_stack.size() - 1 ];
          m_stack.push_back( m_stack.back()->getChildBox(fourcc, back) );
          continue;
        }
        else
        {
          // we're out of boxes to look at
          m_stack.clear();
          return *this;
        }
      }
      // the top box is not null
      if ( m_stack.size() < m_fourccs.size() + 1 )
      {
        // we need to find more children
        MP4::Fourcc fourcc = m_fourccs[ m_stack.size() - 1 ];
        m_stack.push_back( m_stack.back()->getChildBox(fourcc, NULL) );
        continue;
      }
      if ( m_stack.back() )
      {
        break;
      }
    }
    while ( !m_stack.empty() && 
            (!m_stack.back() || m_stack.size() < m_fourccs.size() + 1 ) );
    return *this;
  }

  //! Get the current box
  operator MP4::Mp4IsoBox*()
  {
    if ( !m_hasInited )
    {
      ++*this;
      m_hasInited = true;
    }
    if (m_stack.empty())
    {
      // we've run off the end already
      return NULL;
    }
    return m_stack.back();
  }
protected:
  bool m_hasInited;
  std::vector<MP4::Fourcc> m_fourccs;
  std::vector<MP4::Mp4IsoBox*> m_stack;
};

bool MP4::File::FilePrivate::adjustOffsets( TagLib::FileIO* file,
                                            MP4::Mp4IsoBox* affectedBox,
                                            TagLib::ulonglong difference )
{
  // find the first byte at which point we may need to adjust things
  ulonglong start = affectedBox->offset() + affectedBox->size() - 8;

  // find any /moov/trak/mdia/minf/stbl/stco boxes and adjust them

  MP4::Mp4IsoBox* box;
  ChildIterator iter(fileBox);
  iter.addFourcc(TAGLIB_FOURCC('m','o','o','v'));
  iter.addFourcc(TAGLIB_FOURCC('t','r','a','k'));
  iter.addFourcc(TAGLIB_FOURCC('m','d','i','a'));
  iter.addFourcc(TAGLIB_FOURCC('m','i','n','f'));
  iter.addFourcc(TAGLIB_FOURCC('s','t','b','l'));
  iter.addFourcc(TAGLIB_FOURCC('s','t','c','o'));
  for (; (box = iter) != NULL; ++iter)
  {
    int seekResult = file->seek( box->offset() + 4 ); // +4 for IsoFullBox version/flags
    if ( seekResult < 0 )
    {
      // failed to seek
      return false;
    }
    uint entryCount = file->readBlock(4).toUInt();
    if ( entryCount * 4 + 16 > box->size() )
    {
      // entry is too large
      // +16 is 4 each for box size, fourcc, FullIsoBox header, and entry count
      debug( String(__FUNCTION__) +
             ": entry count " + String::number(entryCount) +
             " is too large for box size " + String::number(box->size()) );
      entryCount = (box->size() - 16) / 4;
    }

    if ( entryCount > (1 << 30) )
    {
      debug( String(__FUNCTION__) +
             ": " + String::number(entryCount) + " is too many entries" );
      return false;
    }

    ByteVector data = file->readBlock( 4 * entryCount );
    bool hasChanged = false;
    for ( uint i = 0; i < entryCount; ++i )
    {
      uint offset = data.mid(i * 4, 4).toUInt();
      if ( offset > start )
      {
        offset += difference;
        data[i * 4 + 0] = (offset >> 24) & 0xFF;
        data[i * 4 + 1] = (offset >> 16) & 0xFF;
        data[i * 4 + 2] = (offset >>  8) & 0xFF;
        data[i * 4 + 3] = (offset >>  0) & 0xFF;
        hasChanged = true;
      }
    }
    if ( !hasChanged )
      continue;

    seekResult = file->seek( box->offset() + 8 ); // as above, plus the size
    if ( seekResult < 0 )
    {
      // failed to seek
      return false;
    }
    file->writeBlock( data );
    // err... I guess we can't tell if the write was successful!?
  }
  return true;
}
