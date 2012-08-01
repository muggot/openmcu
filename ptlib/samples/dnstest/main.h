/*
 * main.h
 *
 * PWLib application header file for DNSTest
 *
 * Copyright 2003 Equivalence
 *
 * $Log: main.h,v $
 * Revision 1.1  2003/04/15 04:12:38  craigs
 * Initial version
 *
 */

#ifndef _DNSTest_MAIN_H
#define _DNSTest_MAIN_H




class DNSTest : public PProcess
{
  PCLASSINFO(DNSTest, PProcess)

  public:
    DNSTest();
    void Main();
};


#endif  // _DNSTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
