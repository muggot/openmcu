/*
 * main.h
 *
 * PWLib application header file for StringTest
 *
 * Copyright (c) 2006 Indranet Technologies Ltd.
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
 * The Initial Developer of the Original Code is Derek J Smithies
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.h,v $
 * Revision 1.1  2006/06/26 02:16:09  dereksmithies
 * Initial release of a program to test the speed and performance of the PString class.
 *

 *
 */

#ifndef _StringTest_MAIN_H
#define _StringTest_MAIN_H


class StringTest : public PProcess
{
  PCLASSINFO(StringTest, PProcess);
    
 public:
  StringTest();
  
  void Main();
  
  void HandleUserInterface();
  
 protected:
  
  void TestPwlibAssign();
  void TestPwlibCopy();
  void TestPwlibEverything();
  void TestPwlibJoin();
  void TestPwlibLength();
  void TestPwlibNone();
  void TestStandardAssign();
  void TestStandardCopy();
  void TestStandardEverything();
  void TestStandardJoin();
  void TestStandardLength();
  void TestStandardNone();
  
  PINDEX              loopCount;
  PTime               startTime;
  
  enum TestToDo {
    None,
    Assign,
    Copy,
    Everything,
    Join,
    Length
  };
  
};


#endif  // _StringTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
