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

#include "trace.h"

#ifndef _WIN32
#include <libgen.h>
#endif

unsigned TraceLevel = 0;
unsigned TraceLevelUserPlane = 0;

void Trace::SetLevel (unsigned level)
{
  TraceLevel = level;
}

unsigned Trace::GetLevel ()
{
  return (TraceLevel);
}

bool Trace::CanTrace (unsigned level)
{
  return level <= TraceLevel;
}

void Trace::SetLevelUserPlane (unsigned level)
{
  TraceLevelUserPlane = level;
}

unsigned Trace::GetLevelUserPlane ()
{
  return (TraceLevelUserPlane);
}

bool Trace::CanTraceUserPlane (unsigned level)
{
  return level <= TraceLevelUserPlane;
}

ostream & Trace::Start(const char* file, int line)
{
#if (defined(WIN32) || defined(SOLARIS))
  cerr << setw(16) << file << '(' << line << ")\t";
#else
  cerr << setw(16) << basename((char *)file) << '(' << line << ")\t";
#endif /* _WIN32 */
  return cerr;
}
