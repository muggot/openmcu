/* h341.h
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
 * h323plus library (www.h323plus.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h341.h,v $
 * Revision 1.4  2008/01/05 11:35:59  shorne
 * More Fixes for Linux
 *
 * Revision 1.3  2008/01/05 07:11:39  shorne
 * Major overhaul of H.341 code
 *
 * Revision 1.2  2008/01/02 19:24:02  shorne
 * Removed P_SNMP check
 *
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.1.2.1  2007/08/02 20:11:58  shorne
 * Added H.341 Support
 *
 *
 *
 */

#pragma once

#ifdef H323_H341

#ifndef _H323_H341
#define _H323_H341

class H323_H341Server : public PSNMPServer
{
  public:
    H323_H341Server(WORD listenport = 161);
    ~H323_H341Server();

   enum messagetype {
     e_request,
     e_nextrequest,
     e_set
   };
 
    // Inherited from PSNMPServer
    BOOL OnGetRequest     (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);
    BOOL OnGetNextRequest (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);
    BOOL OnSetRequest     (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);

    //Events
    virtual BOOL Authorise(const PIPSocket::Address & /*received*/) 
				                             { return FALSE; }

	virtual BOOL OnRequest(H323_H341Server::messagetype /*msgtype*/, 
		                              PSNMP::BindingList & /*vars*/,
						                PSNMP::ErrorType & /*errCode*/) 
	                                          { return FALSE; }

  protected:

};

#endif // _H323_H341

#endif


