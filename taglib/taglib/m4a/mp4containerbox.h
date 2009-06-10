/***************************************************************************
copyright            : (C) 2009 by Pioneers of the Inevitable
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


#ifndef MP4CONTAINERBOX_H
#define MP4CONTAINERBOX_H

#include "mp4isobox.h"

namespace TagLib {
  namespace MP4 {
    class TAGLIB_EXPORT Mp4ContainerBox : public Mp4IsoBox
      //! This is an interface for a container box (which has child boxes)
    {
    public:
      Mp4ContainerBox( TagLib::File* file, MP4::Fourcc fourcc, TagLib::uint size, long offset );
      virtual ~Mp4ContainerBox();

      //! find the next child box of the given fourcc
      //  returns null if not found
      //  @param offset Where to start searching (NULL to search from the start)
      virtual Mp4IsoBox* getChildBox( MP4::Fourcc fourcc, Mp4IsoBox* offset = NULL ) const;

    protected:
      class Mp4ContainerBoxPrivate;
      Mp4ContainerBoxPrivate *d;
    }; // class Mp4ContainerBox
  }
}

#endif // MP4CONTAINERBOX_H