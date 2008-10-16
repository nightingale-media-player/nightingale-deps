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

#ifndef MP4FOURCC_H
#define MP4FOURCC_H

#include "tstring.h"

namespace TagLib
{
  namespace MP4
  {
    /*! union for easy fourcc / type handling */
    class TAGLIB_EXPORT Fourcc
    {
    public:
      //! std constructor
      Fourcc();
      //! string constructor
      Fourcc(TagLib::String fourcc);

      //! destructor
      ~Fourcc();

      //! function to get a string version of the fourcc
      TagLib::String toString() const;
      //! cast operator to unsigned int
      operator unsigned int() const;
      //! comparison operator
      bool operator == (unsigned int fourccB ) const;
      //! comparison operator
      bool operator != (unsigned int fourccB ) const;
      //! assigment operator for unsigned int
      Fourcc& operator = (unsigned int fourcc );
      //! assigment operator for character string
      Fourcc& operator = (char fourcc[4]);

    private:
      uint m_fourcc;     /*!< integer code of the fourcc */
    };

  } // namespace MP4
} // namespace TagLib

#endif // MP4FOURCC_H
