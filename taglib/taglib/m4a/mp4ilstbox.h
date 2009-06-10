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

#ifndef MP4ILSTBOX_H
#define MP4ILSTBOX_H

#include "mp4containerbox.h"
#include "mp4fourcc.h"

namespace TagLib
{
  namespace MP4
  {
    class TAGLIB_EXPORT Mp4IlstBox: public Mp4ContainerBox
    {
    public:
      Mp4IlstBox( TagLib::File* file, MP4::Fourcc fourcc, TagLib::uint size, long offset );
      ~Mp4IlstBox();

      //! parse ilst contents
      void parse();

      //! add a child to this box
      //  NOTE: this causes the box data to be out of sync with disk data
      //  @param child The child to add.  This class takes ownership.
      virtual void addChildBox( Mp4IsoBox* child );

      //! set the size of this box
      //  NOTE: this causes the box data to be out of sync with disk data
      //  @param size The new size
      void setSize( ulonglong size );

    private:
      class Mp4IlstBoxPrivate;
      Mp4IlstBoxPrivate* d;
    }; // Mp4IlstBox

  } // namespace MP4
} // namespace TagLib

#endif // MP4ILSTBOX_H
