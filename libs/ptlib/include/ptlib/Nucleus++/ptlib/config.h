/*
 * config.h
 *
 * System and application configuration class.
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
 * $Log: config.h,v $
 * Revision 1.1  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.8  1998/11/30 22:06:32  robertj
 * New directory structure.
 *
 * Revision 1.7  1998/09/24 04:11:30  robertj
 * Added open software license.
 *
 */

#ifndef _PCONFIG

#ifndef __NUCLEUS_MNT__
#pragma interface
#endif

class PXConfig;

///////////////////////////////////////////////////////////////////////////////
// PConfiguration

#include "../../config.h"
  public:
    PConfig(int, const PString & name);
    ~PConfig();
  protected:
    PXConfig * config;
};


#endif
