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

#include "mpi.h"
#include <math.h>
#include <stdlib.h>

MPIList::MPIList()
{
  desiredWidth = 176;
  desiredHeight = 144;
  minWidth = 176;
  minHeight = 144;
  maxWidth = 704;
  maxHeight = 576;
  frameTime = 3003;
}

void MPIList::addMPI (unsigned width, unsigned height, unsigned interval)
{
  MPI newMPI;

  if (interval == PLUGINCODEC_MPI_DISABLED)
    return;

  newMPI.width = width;
  newMPI.height = height;
  newMPI.interval = interval;

  MPIs.push_back (newMPI);
}

void MPIList::setDesiredWidth (unsigned width)
{
  desiredWidth = width;
}

void MPIList::setDesiredHeight (unsigned height)
{
  desiredHeight = height;
}

void MPIList::setMinWidth (unsigned width)
{
  minWidth = width;
}

void MPIList::setMinHeight (unsigned height)
{
  minHeight = height;
}

void MPIList::setMaxWidth (unsigned width)
{
  maxWidth = width;
}

void MPIList::setMaxHeight (unsigned height)
{
  maxHeight = height;
}

void MPIList::setFrameTime (unsigned _frameTime)
{
  frameTime = _frameTime;
}

unsigned MPIList::getSupportedMPI( unsigned width, unsigned height){
  unsigned i = 0;

  // No Resolution supported at all
  if (MPIs.size() == 0) {
    return PLUGINCODEC_MPI_DISABLED;
  }

  // Resolution higher than maximum
  if ((width > maxWidth) || (height > maxHeight))
    return PLUGINCODEC_MPI_DISABLED;

  // Resolution lower than minimum
  if ((width < minWidth) || (height < minHeight))
    return PLUGINCODEC_MPI_DISABLED;

  // look for the respective MPI
  for (i=0; i < MPIs.size(); i++) {
    if ( (MPIs[i].width == width) && (MPIs[i].height == height) ) {
       return (((MPIs[i].interval * 3003) > frameTime) ? MPIs[i].interval : frameTime / 3003);
    }
  }
  return PLUGINCODEC_MPI_DISABLED;
}

bool MPIList::getNegotiatedMPI( unsigned* width, unsigned* height, unsigned* _frameTime) 
{
  unsigned i = 0;
  unsigned minDistance = -1;
  unsigned minIndex = 0;
  unsigned distance = 0;

  if (MPIs.size() == 0) {
    return false;
  }
  // we first find out which supported resolution is closest 
  // to the desired one or matches it
  for (i=0; i < MPIs.size(); i++) {
    // we square the value in order to get absolute distances
    distance = ( abs(MPIs[i].width  - desiredWidth ) *
                 abs(MPIs[i].height - desiredHeight) );

    if (distance < minDistance) {
      minDistance = distance;
      minIndex = i;
    }
  }

  *width  = MPIs[minIndex].width;
  *height = MPIs[minIndex].height;

  // possibly the supported frame rate is lower than the desired one
  // however we prefer to stay at the desired resolution with a lower framerate
  // instead of selecting a different resolution where the framerate is supported
  if ((MPIs[minIndex].interval * 3003) > frameTime) 
    *_frameTime  = MPIs[minIndex].interval * 3003;
   else
    *_frameTime  = frameTime;
  return true;
}
