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

#ifndef MP4ISOFULLBOX_H
#define MP4ISOFULLBOX_H

#include "mp4isobox.h"
#include "mp4fourcc.h"

namespace TagLib
{
  namespace MP4
  {
    class TAGLIB_EXPORT Mp4IsoFullBox : public Mp4IsoBox
    {
    public:
      //! constructor for full box
      Mp4IsoFullBox( TagLib::File* file, MP4::Fourcc fourcc, uint size, long offset );
      //! destructor for mp4 iso full box
      virtual ~Mp4IsoFullBox();

      //! function to get the version of box
      uchar version();
      //! functions to get/set the flag map
      uint  flags();
      void setFlags( uint flags );

      //! parse wrapper to get common interface for both box and fullbox
      virtual void  parsebox();

    protected:
      class Mp4IsoFullBoxPrivate;
      Mp4IsoFullBoxPrivate* d;
    };

  } // namespace MP4
} // namespace TagLib

#endif // MP4ISOFULLBOX_H

