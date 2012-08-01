/*
 * custom.h
 *
 * PWLib application header file for xmlrpcsrvr
 *
 * Customisable application configurationfor OEMs.
 *
 * Copyright 2002 Equivalence
 *
 * $Log: custom.h,v $
 * Revision 1.1  2002/10/02 08:58:20  craigs
 * Initial version
 *
 */

#include <ptclib/httpsvc.h>

enum {
  SkName, SkCompany, SkEMail,
  NumSecuredKeys
};


extern PHTTPServiceProcess::Info ProductInfo;


// End of File ///////////////////////////////////////////////////////////////
