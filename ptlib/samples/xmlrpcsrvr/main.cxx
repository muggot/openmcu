/*
 * main.cxx
 *
 * PWLib application source file for xmlrpcsrvr
 *
 * Main program entry point.
 *
 * Copyright 2002 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.5  2006/07/22 07:27:26  rjongbloed
 * Fixed various compilation issues
 *
 * Revision 1.4  2003/09/26 13:41:32  rjongbloed
 * Added special test to give more indicative error if try to compile without Expat support.
 *
 * Revision 1.3  2003/04/17 00:03:23  craigs
 * Changed default port from 6666 to 8000 to remove conflicts with other programs
 * that use that port by default
 *
 * Revision 1.2  2002/10/23 15:58:18  craigs
 * Fixed problem with parsing requests, and added sample return value
 *
 * Revision 1.1  2002/10/02 08:58:20  craigs
 * Initial version
 *
 */

#include <ptlib.h>
#include "main.h"
#include "custom.h"

#if !P_EXPAT
#error Must have Expat XML support for this application
#endif



PCREATE_PROCESS(Xmlrpcsrvr);

const WORD DefaultHTTPPort = 8000;


Xmlrpcsrvr::Xmlrpcsrvr()
  : PHTTPServiceProcess(ProductInfo)
{
  xmlrpcServer = NULL;
}


BOOL Xmlrpcsrvr::OnStart()
{
  GetFile().GetDirectory().Change();

  httpNameSpace.AddResource(new PHTTPDirectory("data", "data"));
  httpNameSpace.AddResource(new PServiceHTTPDirectory("html", "html"));

  xmlrpcServer = new PXMLRPCServerResource();

  xmlrpcServer->SetMethod("Function1", PCREATE_NOTIFIER(FunctionNotifier));

  return PHTTPServiceProcess::OnStart();
}


void Xmlrpcsrvr::OnStop()
{
  PHTTPServiceProcess::OnStop();

  delete xmlrpcServer;
  xmlrpcServer = NULL;
}



void Xmlrpcsrvr::OnConfigChanged()
{
}


void Xmlrpcsrvr::OnControl()
{
}


PString Xmlrpcsrvr::GetPageGraphic()
{
  return PHTTPServiceProcess::GetPageGraphic();
}


void Xmlrpcsrvr::AddUnregisteredText(PHTML &)
{
}


BOOL Xmlrpcsrvr::Initialise(const char * initMsg)
{
  //  create the home page
  static const char welcomeHtml[] = "welcome.html";
  if (PFile::Exists(welcomeHtml))
    httpNameSpace.AddResource(new PServiceHTTPFile(welcomeHtml, TRUE), PHTTPSpace::Overwrite);
  else {
    PHTML html;
    html << PHTML::Title("Welcome to "+GetName())
         << PHTML::Body()
         << "<CENTER>\r\n"
         << PHTML::Heading(1) << "Welcome to "
         << gifHTML
         << PHTML::Heading(1)
         << PProcess::GetOSClass() << ' ' << PProcess::GetOSName()
         << " Version " << GetVersion(TRUE) << PHTML::BreakLine()
         << ' ' << compilationDate.AsString("d MMMM yy")
         << PHTML::BreakLine()
         << "by"
         << PHTML::BreakLine()
         << PHTML::Heading(3)
         << PHTML::HotLink(GetHomePage()) << GetManufacturer() << PHTML::HotLink()
         << PHTML::Heading(3)
         << PHTML::HotLink(PString("mailto:")+GetEMailAddress()) << GetEMailAddress() << PHTML::HotLink()
         << PHTML::Paragraph()
         << PHTML::HRule()
         << PHTML::Paragraph()

         << PHTML::HotLink("http://www.equival.com.au/xmlrpcsrvr/relnotes/" + GetVersion(TRUE) + ".html")
         << "Release notes" << PHTML::HotLink()
         << " on this version of " << GetProductName() << " are available."
         << PHTML::Paragraph()
         << PHTML::HRule()
         << GetCopyrightText()
         << PHTML::Body();
    httpNameSpace.AddResource(new PServiceHTTPString("welcome.html", html), PHTTPSpace::Overwrite);
  }

  // note we do NOT use Overwrite
  httpNameSpace.AddResource(xmlrpcServer);

  // set up the HTTP port for listening & start the first HTTP thread
  if (ListenForHTTP(DefaultHTTPPort))
    PSYSTEMLOG(Info, "Opened master socket for HTTP: " << httpListeningSocket->GetPort());
  else {
    PSYSTEMLOG(Fatal, "Cannot run without HTTP port: " << httpListeningSocket->GetErrorText());
    return FALSE;
  }

  PSYSTEMLOG(Info, "Service " << GetName() << ' ' << initMsg);
  return TRUE;
}


void Xmlrpcsrvr::Main()
{
  Suspend();
}

void Xmlrpcsrvr::FunctionNotifier(PXMLRPCServerParms & args, INT)
{
  PTRACE(1, "XMLRPC function called");

  PINDEX i;
  for (i = 0; i < args.request.GetParamCount(); i++) {
    PStringToString dict;
    PString type;
    PString value;
    if (args.request.GetParam(i, dict))
      PTRACE(2, "XMLRPC argument " << i << " is struct: " << dict);
    else if (args.request.GetParam(i, type, value))
      PTRACE(2, "XMLRPC argument " << i << " is " << type << " with value " << value);
    else
      PTRACE(2, "Cannot parse XMLRPC argument " << i);
  }

  args.response.AddParam("return value");
}

// End of File ///////////////////////////////////////////////////////////////
