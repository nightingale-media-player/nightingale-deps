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

#ifndef MP4ISOBOX_H
#define MP4ISOBOX_H

#include "taglib.h"
#include "mp4fourcc.h"

namespace TagLib
{
  class File;
  typedef unsigned long long ulonglong;

  namespace MP4
  {
    class TAGLIB_EXPORT Mp4IsoBox
    {
    public:
      //! constructor for base class
      Mp4IsoBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset );
      //! destructor - simply freeing private ptr
      virtual ~Mp4IsoBox();

      //! function to get the fourcc code
      MP4::Fourcc fourcc() const;
      //! function to get the size of the atom/box
      uint size() const;
      //! function to get the offset of the atom in the mp4 file
      long offset() const;

      //! parse wrapper to get common interface for both box and fullbox
      virtual void  parsebox();
      //! pure virtual function for all subclasses to implement
      virtual void parse() = 0;
      //! serialize the box into a binary blob
      virtual ByteVector render();

      //! find the next child box of the given fourcc
      //  returns null if not found
      //  @param offset Where to start searching (NULL to search from the start)
      virtual Mp4IsoBox* getChildBox( MP4::Fourcc fourcc, Mp4IsoBox* offset = NULL ) const;

    protected:
      //! function to get the file pointer
      TagLib::File* file() const;

      //! set the size of this box
      //  NOTE: this causes the in-memory data to be out of sync with the disk data
      //  @param size The new size
      virtual void setSize( TagLib::ulonglong size );

    protected:
      class Mp4IsoBoxPrivate;
      Mp4IsoBoxPrivate* d;
    };

  } // namespace MP4
} // namespace TagLib

#endif // MP4ISOBOX_H

