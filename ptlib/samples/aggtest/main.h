/*
 * main.h
 *
 * PWLib application header file for aggregator test
 *
 * Copyright (C) 2005 Post Increment
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
 * Revision 1.1  2005/12/22 03:55:52  csoutheren
 * Added initial version of socket aggregation classes
 *
 */

#ifndef _Emailtest_MAIN_H
#define _Emailtest_MAIN_H




class AggTest : public PProcess
{
  PCLASSINFO(AggTest, PProcess)

  public:
    AggTest();
    virtual void Main();
};


#endif  // _AggTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
