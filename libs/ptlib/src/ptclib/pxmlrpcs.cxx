/*
 * pxmlrpcs.cxx
 *
 * XML/RPC support
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: pxmlrpcs.cxx,v $
 * Revision 1.10  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.9  2007/08/20 08:43:00  shorne
 * Fixed issue compiling without Expat
 *
 * Revision 1.8  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.7  2006/11/27 11:37:25  csoutheren
 * Applied 1595552 - Fix for XMLRPC server
 * Thanks to Dave Parr
 *
 * Revision 1.6  2003/02/19 01:51:18  robertj
 * Change to make it easier to set a fault from the server function handler.
 *
 * Revision 1.5  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.4  2002/10/23 15:57:28  craigs
 * Fixed problem where no params specified
 *
 * Revision 1.3  2002/10/17 12:51:01  rogerh
 * Add a newline at the of the file to silence a gcc compiler warning.
 *
 * Revision 1.2  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.1  2002/10/02 08:54:01  craigs
 * Added support for XMLRPC server
 *
 */

// This depends on the expat XML library by Jim Clark
// See http://www.jclark.com/xml/expat.html for more information

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "pxmlrpcs.h"
#endif

#define   DEFAULT_XMPRPC_URL  "/RPC2"

#if P_EXPAT

#include <ptclib/pxmlrpcs.h>

#define new PNEW


PXMLRPCServerResource::PXMLRPCServerResource()
  : PHTTPResource(DEFAULT_XMPRPC_URL)
{
}

PXMLRPCServerResource::PXMLRPCServerResource(
      const PHTTPAuthority & auth)    // Authorisation for the resource.
  : PHTTPResource(DEFAULT_XMPRPC_URL, auth)
{
}
PXMLRPCServerResource::PXMLRPCServerResource(
      const PURL & url)               // Name of the resource in URL space.
  : PHTTPResource(url)
{
}

PXMLRPCServerResource::PXMLRPCServerResource(
      const PURL & url,              // Name of the resource in URL space.
      const PHTTPAuthority & auth    // Authorisation for the resource.
    )
  : PHTTPResource(url, auth)
{
}

BOOL PXMLRPCServerResource::SetMethod(const PString & methodName, const PNotifier & func)
{
  PWaitAndSignal m(methodMutex);

  // find the method, or create a new one
  PXMLRPCServerMethod * methodInfo;
  PINDEX pos = methodList.GetValuesIndex(methodName);
  if (pos != P_MAX_INDEX)
    methodInfo = (PXMLRPCServerMethod *)methodList.GetAt(pos);
  else {
    methodInfo = new PXMLRPCServerMethod(methodName);
    methodList.Append(methodInfo);
  }

  // set the function
  methodInfo->methodFunc = func;

  return TRUE;
}

BOOL PXMLRPCServerResource::LoadHeaders(PHTTPRequest & /*request*/)    // Information on this request.
{
  return TRUE;
}

BOOL PXMLRPCServerResource::OnPOSTData(PHTTPRequest & request,
                                const PStringToString & /*data*/)
{
  PString reply;

  OnXMLRPCRequest(request.entityBody, reply);

  request.code = PHTTP::RequestOK;
  request.outMIME.SetAt(PHTTP::ContentTypeTag(), "text/xml");

  PINDEX len = reply.GetLength();
  request.server.StartResponse(request.code, request.outMIME, len);
  return request.server.Write((const char *)reply, len);
}


void PXMLRPCServerResource::OnXMLRPCRequest(const PString & body, PString & reply)
{
  // get body of message here
  PXMLRPCBlock request;
  BOOL ok = request.Load(body);

  // if cannot parse XML, set return
  if (!ok) {
    reply = FormatFault(PXMLRPC::CannotParseRequestXML, "XML error:" + request.GetErrorString());
    return;
  }

  // make sure methodCall is specified as top level
  if ((request.GetDocumentType() != "methodCall") || (request.GetNumElements() < 1)) {
    reply = FormatFault(PXMLRPC::RequestHasWrongDocumentType, "document type is not methodCall");
    return;
  }

  // make sure methodName is speciified
  PXMLElement * methodName = request.GetElement("methodName");
  if (methodName == NULL) {
    reply = FormatFault(PXMLRPC::RequestHasNoMethodName, "methodCall has no methodName");
    return;
  }

  // extract method name
  if ((methodName->GetSize() != 1) || (methodName->GetElement(0)->IsElement())) {
    reply = FormatFault(PXMLRPC::MethodNameIsEmpty, "methodName is empty");
    return;
  }
  PString method = ((PXMLData *)methodName->GetElement(0))->GetString();

  // extract params
  PTRACE(3, "XMLRPC\tReceived XMLRPC request for method " << method);

  OnXMLRPCRequest(method, request, reply);
}

void PXMLRPCServerResource::OnXMLRPCRequest(const PString & methodName,
                                            PXMLRPCBlock & request,
                                            PString & reply)
{
  methodMutex.Wait();

  // find the method information
  PINDEX pos = methodList.GetValuesIndex(methodName);
  if (pos == P_MAX_INDEX) {
    reply = FormatFault(PXMLRPC::UnknownMethod, "unknown method " + methodName);
    methodMutex.Signal();
    return;
  }
  PXMLRPCServerMethod * methodInfo = (PXMLRPCServerMethod *)methodList.GetAt(pos);
  PNotifier notifier = methodInfo->methodFunc;
  methodMutex.Signal();

  // create paramaters
  PXMLRPCServerParms p(*this, request);

  // call the notifier
  notifier(p, 0);

  // get the reply
  if (request.GetFaultCode() != P_MAX_INDEX)
    reply = FormatFault(request.GetFaultCode(), request.GetFaultText());
  else {
    PStringStream r; r << p.response;
    reply = r;
  }
}


PString PXMLRPCServerResource::FormatFault(PINDEX code, const PString & str)
{
  PTRACE(2, "XMLRPC\trequest failed: " << str);

  PStringStream reply;
  reply << "<?xml version=\"1.0\"?>\n"
           "<methodResponse>"
             "<fault>"
               "<value>"
                 "<struct>"
                   "<member>"
                     "<name>faultCode</name>"
                     "<value><int>" << code << "</int></value>"
                   "</member>"
                   "<member>"
                     "<name>faultString</name>"
                     "<value><string>" << str << "</string></value>"
                   "</member>"
                 "</struct>"
               "</value>"
             "</fault>"
           "</methodResponse>";
  return reply;
}

#endif

