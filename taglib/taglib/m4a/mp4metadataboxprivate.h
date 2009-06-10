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

#ifndef MP4METADATABOXPRIVATE_H
#define MP4METADATABOXPRIVATE_H

#include "mp4metadatabox.h"

namespace TagLib {
  namespace MP4 {
    class Mp4MetadataBox::Mp4MetadataBoxPrivate
      //! This is the private data of a metadata box (which has containers)
      //  This is _not_ for the use of consumers of TagLib
    {
    public:
      ITunesDataBox* dataBox;
    }; // class Mp4MetadataBoxPrivate
  }
}

#endif // MP4METADATABOXPRIVATE_H