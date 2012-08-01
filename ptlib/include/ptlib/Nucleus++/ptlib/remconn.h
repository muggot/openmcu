/*
 * remconn.h
 *
 * Remote network connection (ppp) class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: remconn.h,v $
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.7  1998/12/04 12:12:58  robertj
 * Fixed missing forward reference.
 *
 * Revision 1.6  1998/11/30 22:07:03  robertj
 * New directory structure.
 *
 * Revision 1.5  1998/09/24 04:11:50  robertj
 * Added open software license.
 *
 * Revision 1.4  1996/09/21 05:42:12  craigs
 * Changes for new common files, PConfig changes and signal handling
 *
 * Revision 1.3  1996/08/03 12:08:19  craigs
 * Changed for new common directories
 *
 * Revision 1.2  1996/04/15 10:50:48  craigs
 * Last revision prior to release of MibMaster
 *
 * Revision 1.1  1996/01/26 11:06:31  craigs
 * Initial revision
 *
 */

#ifndef _PREMOTECONNECTION

#pragma interface

class PPipeChannel;
class PXRemoteThread;


#include "../../remconn.h"
  protected:
    PString        pppDeviceName;
    PPipeChannel * pipeChannel;
    BOOL           wasConnected;
    Status         status;
    PString        deviceStr;
};

#endif
