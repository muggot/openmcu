/* h341.cxx
 *
 * Copyright (c) 2007 ISVO (Asia) Pte Ltd. All Rights Reserved.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is derived from and used in conjunction with the 
 * H323plus Project (www.h323plus.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h341.cxx,v $
 * Revision 1.3  2008/01/05 11:36:01  shorne
 * More Fixes for Linux
 *
 * Revision 1.2  2008/01/05 07:11:39  shorne
 * Major overhaul of H.341 code
 *
 * Revision 1.1  2007/08/06 20:51:38  shorne
 * First commit of h323plus
 *
 * Revision 1.1.2.1  2007/08/02 20:12:05  shorne
 * Added H.341 Support
 *
 *
 *
 *
 */

#include <ptlib.h>

#include "openh323buildopts.h"

#ifdef H323_H341

#include <ptclib/psnmp.h>
#include "h341/h341.h"
#include "h341/h341_oid.h"


H323_H341Server::H323_H341Server(WORD listenPort)
: PSNMPServer(PIPSocket::GetDefaultIpAny(), listenPort)   
{

}

H323_H341Server::~H323_H341Server()
{

}


static BOOL ValidateOID(H323_H341Server::messagetype reqType,
						        PSNMP::BindingList & varlist,
								PSNMP::ErrorType & errCode)
{

    PSNMP::BindingList::const_iterator Iter = varlist.begin();
    BOOL found = FALSE;
    do {
     for (Iter = varlist.begin(); Iter != varlist.end(); ++Iter) {
       for (PINDEX i = 0; i< PARRAYSIZE(H341_Field); i++) {
          if (H341_Field[i].oid != Iter->first) 
			   continue;

	      found = TRUE;
		  switch (reqType) {
            case H323_H341Server::e_request:
            case H323_H341Server::e_nextrequest:
			  if (H341_Field[i].access == H341_NoAccess) {
				  PTRACE(4,"H341\tAttribute request FAILED: No permitted access " << Iter->first );
                  errCode = PSNMP::GenErr;
                  return FALSE;    
              }
              break;
            case H323_H341Server::e_set:
              if (H341_Field[i].access == H341_ReadOnly) {
				  PTRACE(4,"H341\tAttribute set FAILED: Read Only " << Iter->first );
                  errCode = PSNMP::ReadOnly;
                  return FALSE;    
              }
              break;
            default:   // Unknown request
			  PTRACE(4,"H341\tGENERAL FAILURE: Unknown request");
              errCode = PSNMP::GenErr;
              return FALSE;    
		  }
             
		  if (Iter->second.GetTag() != (unsigned)H341_Field[i].type ) {
			  PTRACE(4,"H341\tAttribute FAILED Not valid field type " << Iter->first);
              errCode = PSNMP::BadValue;
              return FALSE;    
		  }
		  break;
	   }
	   if (found) break;
	 }
	} while (Iter != varlist.end() && !found);

	if (!found) {
	   	PTRACE(4,"H341\tRequest FAILED: Not valid attribute " << Iter->first);
        errCode = PSNMP::NoSuchName;
        return FALSE;
	}

    return TRUE;
}

BOOL H323_H341Server::OnGetRequest(PINDEX /*reqID*/, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode)
{
	messagetype reqType = H323_H341Server::e_request;
	if (!ValidateOID(reqType,vars, errCode))
		     return FALSE;

	return OnRequest(reqType, vars,errCode);

}

BOOL H323_H341Server::OnGetNextRequest(PINDEX /*reqID*/, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode)
{
	messagetype reqType = H323_H341Server::e_nextrequest;
	if (!ValidateOID(reqType,vars, errCode))
		     return FALSE;

	return OnRequest(reqType, vars,errCode);
}

BOOL H323_H341Server::OnSetRequest(PINDEX /*reqID*/, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode)
{
	messagetype reqType = H323_H341Server::e_set;
	if (!ValidateOID(reqType,vars, errCode))
		     return FALSE;

	return OnRequest(reqType, vars,errCode);
}

#endif



