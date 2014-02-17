// H460.h:
/*
 * Virteos H.460 Implementation for the h323plus Library.
 *
 * Virteos is a Trade Mark of ISVO (Asia) Pte Ltd.
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
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
 * OpenH323 Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h460.h,v $
 * Revision 1.1  2007/08/06 20:50:51  shorne
 * First commit of h323plus
 *
 *
 *
*/


#ifdef P_USE_PRAGMA
#pragma interface
#endif

class H460_MessageType
{
  public:
    enum {
      e_gatekeeperRequest           = 0xf0,
      e_gatekeeperConfirm           = 0xf1,
      e_gatekeeperReject            = 0xf2,
      e_registrationRequest         = 0xf3,
      e_registrationConfirm         = 0xf4, 
      e_registrationReject          = 0xf5,
      e_admissionRequest            = 0xf6,
      e_admissionConfirm            = 0xf7,
      e_admissionReject             = 0xf8,
      e_locationRequest             = 0xf9,
      e_locationConfirm             = 0xfa,
      e_locationReject              = 0xfb,
      e_nonStandardMessage          = 0xfc,
      e_serviceControlIndication    = 0xfd,
      e_serviceControlResponse      = 0xfe,
      e_unregistrationRequest       = 0xe0,
      e_inforequest                 = 0xe1,
      e_inforequestresponse         = 0xe2,
      e_disengagerequest            = 0xe3,
      e_disengageconfirm            = 0xe4,
      e_setup			            = 0x05,   // Match Q931 message id
      e_callProceeding	            = 0x02,   // Match Q931 message id
      e_connect	                    = 0x07,   // Match Q931 message id
      e_alerting                    = 0x01,   // Match Q931 message id
      e_facility                    = 0x62,   // Match Q931 message id
      e_releaseComplete	            = 0x5a,   // Match Q931 message id
      e_unallocated                 = 0xff
    };
};

