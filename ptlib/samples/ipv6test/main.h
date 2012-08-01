/*
 * main.h
 *
 * PWLib application header file for dtmftest
 *
 * Copyright (C) 2004 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.h,v $
 * Revision 1.2  2006/06/21 03:28:42  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.1  2004/12/14 06:50:59  csoutheren
 * Initial version
 *
 *
 */

#ifndef _IPV6Test_MAIN_H
#define _IPV6Test_MAIN_H

#include <ptlib/pprocess.h>

class IPV6Test : public PProcess
{
  PCLASSINFO(IPV6Test, PProcess)

  public:
    IPV6Test();
    virtual void Main();

 protected:

};

#endif  // _Dtmftest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
