/*
 * main.h
 *
 * PWLib application header file for xmlrpcsrvr
 *
 * Copyright 2002 Equivalence
 *
 * $Log: main.h,v $
 * Revision 1.2  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.1  2002/10/02 08:58:20  craigs
 * Initial version
 *
 */

#ifndef _Xmlrpcsrvr_MAIN_H
#define _Xmlrpcsrvr_MAIN_H

#include <ptlib/pprocess.h>
#include <ptclib/httpsvc.h>
#include <ptclib/pxmlrpcs.h>

class Xmlrpcsrvr : public PHTTPServiceProcess
{
  PCLASSINFO(Xmlrpcsrvr, PHTTPServiceProcess)

  public:
    Xmlrpcsrvr();
    void Main();
    BOOL OnStart();
    void OnStop();
    void OnConfigChanged();
    void OnControl();
    PString GetPageGraphic();
    void AddUnregisteredText(PHTML & html);
    BOOL Initialise(const char * initMsg);

    PDECLARE_NOTIFIER(PXMLRPCServerParms, Xmlrpcsrvr, FunctionNotifier);

  protected:
    PXMLRPCServerResource * xmlrpcServer;
};


#endif  // _Xmlrpcsrvr_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
