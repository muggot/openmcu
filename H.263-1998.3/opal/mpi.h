/*****************************************************************************/
/* The contents of this file are subject to the Mozilla Public License       */
/* Version 1.0 (the "License"); you may not use this file except in          */
/* compliance with the License.  You may obtain a copy of the License at     */
/* http://www.mozilla.org/MPL/                                               */
/*                                                                           */
/* Software distributed under the License is distributed on an "AS IS"       */
/* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the  */
/* License for the specific language governing rights and limitations under  */
/* the License.                                                              */
/*                                                                           */
/* The Original Code is the Open H323 Library.                               */
/*                                                                           */
/* The Initial Developer of the Original Code is Matthias Schneider          */
/* Copyright (C) 2007 Matthias Schneider, All Rights Reserved.               */
/*                                                                           */
/* Contributor(s): Matthias Schneider (ma30002000@yahoo.de)                  */
/*                                                                           */
/* Alternatively, the contents of this file may be used under the terms of   */
/* the GNU General Public License Version 2 or later (the "GPL"), in which   */
/* case the provisions of the GPL are applicable instead of those above.  If */
/* you wish to allow use of your version of this file only under the terms   */
/* of the GPL and not to allow others to use your version of this file under */
/* the MPL, indicate your decision by deleting the provisions above and      */
/* replace them with the notice and other provisions required by the GPL.    */
/* If you do not delete the provisions above, a recipient may use your       */
/* version of this file under either the MPL or the GPL.                     */
/*                                                                           */
/* The Original Code was written by Matthias Schneider <ma30002000@yahoo.de> */
/*****************************************************************************/

#ifndef __MPI_H__
#define __MPI_H__ 1

#include <vector>
#include "opalplugin.h"

typedef struct MPI {
  unsigned width;
  unsigned height;
  unsigned interval;
} MPI;

class MPIList
{
  public:
    MPIList();
    void setMinWidth (unsigned width);
    void setMinHeight (unsigned height);
    void setMaxWidth (unsigned width);
    void setMaxHeight (unsigned height);
    unsigned getSupportedMPI( unsigned width, unsigned height);

    void setFrameTime (unsigned _frameTime);
    void addMPI (unsigned width, unsigned height, unsigned interval);

    void setDesiredWidth (unsigned width);
    void setDesiredHeight (unsigned height);
    bool getNegotiatedMPI( unsigned* width, unsigned* height, unsigned* _frameTime);

    unsigned size () { return MPIs.size(); }
  protected:
    std::vector <MPI> MPIs;

    unsigned minWidth;
    unsigned minHeight;
    unsigned maxWidth;
    unsigned maxHeight;
    unsigned frameTime;
    unsigned desiredWidth;
    unsigned desiredHeight;
};

#endif /* __MPI_H__ */
