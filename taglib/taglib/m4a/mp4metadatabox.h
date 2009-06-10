/***************************************************************************
copyright            : (C) 2009 Pioneers of the Inevitable
email                : songbird@songbirdnest.com
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

#ifndef METADATABOX_H
#define METADATABOX_H

#include "mp4isobox.h"
#include "mp4fourcc.h"
#include "itunesdatabox.h"

namespace TagLib
{
  namespace MP4
  {
    class TAGLIB_EXPORT Mp4MetadataBox : public Mp4IsoBox
      //! a box that is for metadata (artist, album, etc.)
      //  this is an internal class, don't use outside of MP4.
    {
    public:
      //! constructor for metadata box
      Mp4MetadataBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset );
      //! destructor for mp4 metadata box
      virtual ~Mp4MetadataBox();

      //! serialize the box into a binary blob
      virtual ByteVector render();

      //! access to the data in the box
      virtual ITunesDataBox* data() const;
      //! set the data in the box
      virtual void setData( ITunesDataBox* data );

    protected:
      class Mp4MetadataBoxPrivate;
      Mp4MetadataBoxPrivate* d;
    };

  } // namespace MP4
} // namespace TagLib

#endif // METADATABOX_H

