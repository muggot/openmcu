/*
 * psnmp.cxx
 *
 * SNMP base and support classes.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psnmp.cxx,v $
 * Revision 1.9  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.8  1998/11/30 04:52:06  robertj
 * New directory structure
 *
 * Revision 1.7  1998/10/13 14:06:32  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.6  1998/09/23 06:22:33  robertj
 * Added open source copyright license.
 *
 * Revision 1.5  1998/01/26 02:50:17  robertj
 * GNU Support
 *
 * Revision 1.4  1997/07/20 08:50:04  craigs
 * Changed var binding list to use ASN NULL rather than empty string
 *
 * Revision 1.3  1997/07/14 11:47:17  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.2  1996/11/04 03:59:19  robertj
 * Added selectable read buffer size.
 *
 * Revision 1.1  1996/09/14 13:02:18  robertj
 * Initial revision
 *
 * Revision 1.9  1996/05/29 10:44:51  craigs
 * Latest version wil traps and discovery
 *
 * Revision 1.8  1996/05/09 13:23:49  craigs
 * Added trap functions
 *
 * Revision 1.7  1996/04/23 12:12:46  craigs
 * Changed to use GetErrorText function
 *
 * Revision 1.6  1996/04/16 13:20:43  craigs
 * Final version prior to beta1 release
 *
 * Revision 1.5  1996/04/15 09:05:30  craigs
 * Latest version prior to integration with Robert's changes
 *
 * Revision 1.4  1996/04/06 11:38:35  craigs
 * Lots of changes - working version prior to discover changes
 *
 * Revision 1.3  1996/04/01 12:50:44  craigs
 * CHanged for clean compile under NT
 *
 * Revision 1.2  1996/04/01 12:34:06  craigs
 * Added RCS header
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "psnmp.h"
#endif

#include <ptlib.h>
#include <ptclib/psnmp.h>

#define new PNEW


static char const * const SnmpErrorCodeTable[] = 
{
  "no error",
  "too big",
  "no such name",
  "bad value",
  "read only",
  "gen err",

  "no response",
  "malformed response",
  "send failed",
  "rx buff too small",
  "tx data too big"
};

static char const * const TrapCodeToText[PSNMP::NumTrapTypes] = {
  "Cold Start",
  "Warm Start",
  "Link Down",
  "Link Up",
  "Auth Fail",
  "EGP Loss",
  "Enterprise"
};


///////////////////////////////////////////////////////////////
//
//  PSNMPVarBindingList
//

void PSNMPVarBindingList::Append(const PString & objectID)
{
  objectIds.AppendString(objectID);
  values.Append(new PASNNull());
}


void PSNMPVarBindingList::Append(const PString & objectID, PASNObject * obj)
{
  objectIds.AppendString(objectID);
  values.Append(obj);
}


void PSNMPVarBindingList::AppendString(const PString & objectID, const PString & str)
{
  Append(objectID, new PASNString(str));
}


void PSNMPVarBindingList::RemoveAll()
{
  objectIds.RemoveAll();
  values.RemoveAll();
}


PINDEX PSNMPVarBindingList::GetSize() const
{
  return objectIds.GetSize();
}


PASNObject & PSNMPVarBindingList::operator[](PINDEX idx) const
{
  return values[idx];
}


PString PSNMPVarBindingList::GetObjectID(PINDEX idx) const
{ 
  return objectIds[idx];
}


void PSNMPVarBindingList::PrintOn(ostream & strm) const
{
  for (PINDEX i = 0; i < GetSize(); i++) 
    strm << objectIds[i] 
         << " = "
         << values[i];
}


PString PSNMP::GetTrapTypeText(PINDEX code)
{
  PString str;
  if (code >= NumTrapTypes)
    return "Unknown";
  else
    return TrapCodeToText[code];
}


PString PSNMP::GetErrorText(ErrorType err) 
{
  if (err >= NumErrors)
    return "unknown error";
  else
    return SnmpErrorCodeTable[err];
}


// End Of File ///////////////////////////////////////////////////////////////
