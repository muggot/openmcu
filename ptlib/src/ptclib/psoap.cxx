/*
 * psoap.cxx
 *
 * SOAP client / server classes.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2003 Andreas Sikkema
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
 * The Initial Developer of the Original Code is Andreas Sikkema
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psoap.cxx,v $
 * Revision 1.11  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.10  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.9  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2004/04/24 01:06:32  rjongbloed
 * Apploed  patch that impliments a number of checks to avoid segfaults when dealing with
 *   various clients. Thanks Ben Lear
 *
 * Revision 1.7  2004/01/17 17:45:59  csoutheren
 * Changed to use PString::MakeEmpty
 *
 * Revision 1.6  2003/10/08 21:58:13  dereksmithies
 * Add client authentication support. many thanks to Ben Lear.
 *
 * Revision 1.5  2003/04/28 00:09:14  craigs
 * Patches from Andreas Sikkema
 *
 * Revision 1.4  2003/03/31 06:20:56  craigs
 * Split the expat wrapper from the XML file handling to allow reuse of the parser
 *
 * Revision 1.3  2003/02/09 23:31:54  robertj
 * Added referention PString's for efficiency.
 *
 * Revision 1.2  2003/02/09 23:22:46  robertj
 * Fixed spelling errors, and setting return values, thanks Andreas Sikkema
 *
 * Revision 1.1  2003/02/04 22:46:48  robertj
 * Added basic SOAP support, thanks Andreas Sikkema
 *
 */

#ifdef __GNUC__
#pragma implementation "psoap.h"
#endif


#include <ptlib.h>


#if P_EXPAT

#include <ptclib/psoap.h>


#define new PNEW



/*
 SOAP message classes
 ####################
 */


PSOAPMessage::PSOAPMessage( int options ) : 
  PXML( options ),
  pSOAPBody( 0 ),
  pSOAPMethod( 0 ),
  faultCode( PSOAPMessage::NoFault )
{
}

PSOAPMessage::PSOAPMessage( const PString & method, const PString & nameSpace ) :
  PXML( PXMLParser::Indent + PXMLParser::NewLineAfterElement ),
  pSOAPBody( 0 ),
  pSOAPMethod( 0 ),
  faultCode( PSOAPMessage::NoFault )
{
    SetMethod( method, nameSpace );
}



void PSOAPMessage::SetMethod( const PString & name, const PString & nameSpace )
{
  PXMLElement* rtElement = 0;
  
  if ( pSOAPBody == 0 )
  {
    SetRootElement("SOAP-ENV:Envelope");
    
    rtElement = GetRootElement();

    rtElement->SetAttribute("xmlns:SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", TRUE );
    rtElement->SetAttribute("xmlns:xsi", "http://www.w3.org/1999/XMLSchema-instance", TRUE );
    rtElement->SetAttribute("xmlns:xsd", "http://www.w3.org/1999/XMLSchema", TRUE );
    rtElement->SetAttribute("xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", TRUE );

    pSOAPBody = new PXMLElement( rtElement, "SOAP-ENV:Body");

    rtElement->AddChild( pSOAPBody, TRUE );
  }

  if ( pSOAPMethod == 0 )
  {
    rtElement = GetRootElement();

    pSOAPMethod = new PXMLElement( rtElement, PString( "m:") + name );
    if ( nameSpace != "" )
    {
      pSOAPMethod->SetAttribute("xmlns:m", nameSpace, TRUE );
    }
    pSOAPBody->AddChild( pSOAPMethod, TRUE );
  }

}

void PSOAPMessage::GetMethod( PString & name, PString & nameSpace )
{
  PString fullMethod = pSOAPMethod->GetName();
  PINDEX sepLocation = fullMethod.Find(':');
  if (sepLocation != P_MAX_INDEX) {
    PString methodID = fullMethod.Left(sepLocation);
    name = fullMethod.Right(fullMethod.GetSize() - 2 - sepLocation);
    nameSpace = pSOAPMethod->GetAttribute( "xmlns:" + methodID );
  }
}


void PSOAPMessage::AddParameter( PString name, PString type, PString value )
{
  if ( pSOAPMethod )
  {
    PXMLElement* rtElement = GetRootElement();
    
    PXMLElement* pParameter = new PXMLElement( rtElement, name);
    PXMLData* pParameterData = new PXMLData( pParameter, value);
    
    if ( type != "" )
    {
      pParameter->SetAttribute( "xsi:type", PString( "xsd:" ) + type );
    }
    
    pParameter->AddChild( pParameterData, TRUE );

    AddParameter( pParameter, TRUE );
  }
}

void PSOAPMessage::AddParameter( PXMLElement* parameter, BOOL dirty )
{
  if ( pSOAPMethod )
  {
    pSOAPMethod->AddChild( parameter, dirty );
  }
}

void PSOAPMessage::PrintOn(ostream & strm) const
{
  BOOL newLine = ( options & PXMLParser::NewLineAfterElement ) != 0;

  PString ver = version;
  PString enc = encoding;
  int salone = standAlone;

  if ( ver.IsEmpty() )
    ver= "1.0";
  if ( enc.IsEmpty() )
    enc = "UTF-8";
  if ( salone == -2 )
    salone = -1;

  strm << "<?xml version=\"" << ver << "\" encoding=\"" << enc << "\"";
  switch ( salone ) {
    case 0:
      strm << " standalone=\"no\"";
      break;
    case 1:
      strm << " standalone=\"yes\"";
      break;
    default:
      break;
  }

  strm << "?>";
  if ( newLine )
    strm << endl;

  if ( rootElement != NULL ) {
    rootElement->Output(strm, *(this), 2 );
  }
}

PString PSOAPMessage::AsString( void )
{
  PStringStream stringStream;
  PrintOn( stringStream );

  PString SOAPString = stringStream;

  return SOAPString;
}


PString faultCodeToString( PINDEX faultCode )
{
  PString faultCodeStr;
  switch ( faultCode )
  {
  case PSOAPMessage::VersionMismatch:
    faultCodeStr = "VersionMisMatch";
    break;
  case PSOAPMessage::MustUnderstand:
    faultCodeStr = "MustUnderstand";
    break;
  case PSOAPMessage::Client:
    faultCodeStr = "Client";
    break;
  case PSOAPMessage::Server:
    faultCodeStr = "Server";
    break;
  default:
    // Default it's the server's fault. Can't blame it on the customer, because he/she is king ;-)
    faultCodeStr = "Server";
    break;
  }

  return faultCodeStr;
}

PINDEX stringToFaultCode( PString & faultStr )
{
  if ( faultStr == "VersionMisMatch" )
    return PSOAPMessage::VersionMismatch;

  if ( faultStr == "MustUnderstand" )
    return PSOAPMessage::MustUnderstand;

  if ( faultStr == "Client" )
    return PSOAPMessage::Client;
  
  if ( faultStr == "Server" )
    return PSOAPMessage::Server;

  return PSOAPMessage::Server;
}

BOOL PSOAPMessage::GetParameter( const PString & name, PString & value )
{
  PXMLElement* pElement = GetParameter( name );
  if(pElement == NULL)
    return FALSE;


  if ( pElement->GetAttribute( "xsi:type") == "xsd:string" )
  {
    value = pElement->GetData();
    return TRUE;
  }

  value.MakeEmpty();
  return FALSE;
}

BOOL PSOAPMessage::GetParameter( const PString & name, int & value )
{
  PXMLElement* pElement = GetParameter( name );
  if(pElement == NULL)
    return FALSE;

  if ( pElement->GetAttribute( "xsi:type") == "xsd:int" )
  {
    value = pElement->GetData().AsInteger();
    return TRUE;
  }

  value = -1;
  return FALSE;
}

PXMLElement* PSOAPMessage::GetParameter( const PString & name )
{
  if ( pSOAPMethod )
  {
    return pSOAPMethod->GetElement( name, 0 );
  }
  else
  {
    return 0;
  }
}

BOOL PSOAPMessage::Load( const PString & str )
{
  if ( !PXML::Load( str ) )
    return FALSE;
 
  if ( rootElement != NULL )
  {
    PString soapEnvelopeName = rootElement->GetName();
    PString soapEnvelopeID = soapEnvelopeName.Left( soapEnvelopeName.Find(':') );
    
    pSOAPBody = rootElement->GetElement( soapEnvelopeID + ":Body", 0 );
    
    if ( pSOAPBody != NULL )
    {
      PXMLObjectArray  subObjects = pSOAPBody->GetSubObjects() ;

      PINDEX idx;
      PINDEX size = subObjects.GetSize();
      
      for ( idx = 0; idx < size; idx++ ) {
        if ( subObjects[ idx ].IsElement() ) {
          // First subobject being an element is the method
          pSOAPMethod = ( PXMLElement * ) &subObjects[ idx  ];

          PString method;
          PString nameSpace;

          GetMethod( method, nameSpace );

          // Check if method name is "Fault"
          if ( method == "Fault" )
          {
            // The SOAP server has signalled an error
            PString faultCodeData = GetParameter( "faultcode" )->GetData();
            faultCode = stringToFaultCode( faultCodeData );
            faultText = GetParameter( "faultstring" )->GetData();
          }
          else
          {
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}

void PSOAPMessage::SetFault( PINDEX code, const PString & text) 
{ 
  faultCode = code; 
  faultText = text; 

  PString faultCodeStr = faultCodeToString( code );

  SetMethod( "Fault", "" );

  AddParameter( "faultcode", "", faultCodeStr );
  AddParameter( "faultstring", "", text );

}



/*
 SOAP server classes
 ####################
 */



PSOAPServerResource::PSOAPServerResource()
  : PHTTPResource( DEFAULT_SOAP_URL ),
  soapAction( " " )
{
}

PSOAPServerResource::PSOAPServerResource(
      const PHTTPAuthority & auth )    // Authorisation for the resource.
  : PHTTPResource( DEFAULT_SOAP_URL, auth ),
  soapAction( " " )
{
}
PSOAPServerResource::PSOAPServerResource(
      const PURL & url )               // Name of the resource in URL space.
  : PHTTPResource(url )
{
}

PSOAPServerResource::PSOAPServerResource(
      const PURL & url,              // Name of the resource in URL space.
      const PHTTPAuthority & auth    // Authorisation for the resource.
    )
  : PHTTPResource( url, auth )
{
}

BOOL PSOAPServerResource::SetMethod(const PString & methodName, const PNotifier & func)
{
  // Set the method for the notifier function and add it to the list
  PWaitAndSignal m( methodMutex );

  // Find the method, or create a new one
  PSOAPServerMethod * methodInfo;
  
  PINDEX pos = methodList.GetValuesIndex( methodName );
  if (pos != P_MAX_INDEX)
  {
    methodInfo = ( PSOAPServerMethod *) methodList.GetAt( pos );
  }
  else 
  {
    methodInfo = new PSOAPServerMethod( methodName );
    methodList.Append( methodInfo );
  }

  // set the function
  methodInfo->methodFunc = func;

  return TRUE;
}

BOOL PSOAPServerResource::LoadHeaders( PHTTPRequest& /* request */ )    // Information on this request.
{
  return TRUE;
}

BOOL PSOAPServerResource::OnPOSTData( PHTTPRequest & request,
                                const PStringToString & /*data*/)
{
  PTRACE( 2, "PSOAPServerResource\tReceived post data, request: " << request.entityBody );

  PString reply;

  BOOL ok = FALSE;

  // Check for the SOAPAction header
  PString* pSOAPAction = request.inMIME.GetAt( "SOAPAction" );
  if ( pSOAPAction )
  {
    // If it's available check if we are expecting a special header value
    if ( soapAction.IsEmpty() || soapAction == " " )
    {
      // A space means anything goes
      ok = OnSOAPRequest( request.entityBody, reply );
    }
    else
    {
      // Check if the incoming header is the same as we expected
      if ( *pSOAPAction == soapAction )
      {
        ok = OnSOAPRequest( request.entityBody, reply );
      }
      else
      {
        ok = FALSE;
        reply = FormatFault( PSOAPMessage::Client, "Incorrect SOAPAction in HTTP Header: " + *pSOAPAction ).AsString();
      }
    }
  }
  else
  {
    ok = FALSE;
    reply = FormatFault( PSOAPMessage::Client, "SOAPAction is missing in HTTP Header" ).AsString();
  }

  // If everything went OK, reply with ReturnCode 200 (OK)
  if ( ok )
    request.code = PHTTP::RequestOK;
  else
    // Reply with InternalServerError (500)
    request.code = PHTTP::InternalServerError;

  // Set the correct content-type
  request.outMIME.SetAt(PHTTP::ContentTypeTag(), "text/xml");

  // Start constructing the response
  PINDEX len = reply.GetLength();
  request.server.StartResponse( request.code, request.outMIME, len );
  
  // Write the reply to the client
  return request.server.Write( (const char* ) reply, len );
}


BOOL PSOAPServerResource::OnSOAPRequest( const PString & body, PString & reply )
{
  // Load the HTTP body into the SOAP (XML) parser
  PSOAPMessage request;
  BOOL ok = request.Load( body );

  // If parsing the XML to SOAP failed reply with an error
  if ( !ok ) 
  { 
    reply = FormatFault( PSOAPMessage::Client, "XML error:" + request.GetErrorString() ).AsString();
    return FALSE;
  }


  PString method;
  PString nameSpace;

  // Retrieve the method from the SOAP messsage
  request.GetMethod( method, nameSpace );

  PTRACE( 3, "PSOAPServerResource\tReceived SOAP message for method " << method);

  return OnSOAPRequest( method, request, reply );
}

BOOL PSOAPServerResource::OnSOAPRequest( const PString & methodName, 
                                            PSOAPMessage & request,
                                            PString & reply )
{
  methodMutex.Wait();

  // Find the method information
  PINDEX pos = methodList.GetValuesIndex( methodName );

  if ( pos == P_MAX_INDEX ) 
  {
    reply = FormatFault( PSOAPMessage::Client, "Unknown method = " + methodName ).AsString();
    return FALSE;
  }
  
  PSOAPServerMethod * methodInfo = ( PSOAPServerMethod * )methodList.GetAt( pos );
  PNotifier notifier = methodInfo->methodFunc;
  
  methodMutex.Signal();

  // create a request/response container to be passed to the notifier function
  PSOAPServerRequestResponse p( request );

  // call the notifier
  notifier( p, 0 );

  // get the reply

  reply = p.response.AsString();

  return p.response.GetFaultCode() == PSOAPMessage::NoFault;
}


PSOAPMessage PSOAPServerResource::FormatFault( PINDEX code, const PString & str )
{
  PTRACE(2, "PSOAPServerResource\trequest failed: " << str);

  PSOAPMessage reply;

  PString faultCodeStr = faultCodeToString( code );

  reply.SetMethod( "Fault", "" );

  reply.AddParameter( "faultcode", "", faultCodeStr );
  reply.AddParameter( "faultstring", "", str );

  return reply;
}


/*
 SOAP client classes
 ####################
 */


PSOAPClient::PSOAPClient( const PURL & _url )
  : url(_url),
  soapAction( " " )
{
  timeout = 10000;
}

BOOL PSOAPClient::MakeRequest( const PString & method, const PString & nameSpace )
{
  PSOAPMessage request( method, nameSpace );
  PSOAPMessage response;

  return MakeRequest( request, response );
}

BOOL PSOAPClient::MakeRequest( const PString & method, const PString & nameSpace, PSOAPMessage & response )
{
  PSOAPMessage request( method, nameSpace );

  return MakeRequest( request, response );
}

BOOL PSOAPClient::MakeRequest( PSOAPMessage & request, PSOAPMessage & response )
{
  return  PerformRequest( request, response );
}

BOOL PSOAPClient::PerformRequest( PSOAPMessage & request, PSOAPMessage & response )
{
  // create SOAP request
  PString soapRequest;

  PStringStream txt;
  
  if ( !request.Save( soapRequest ) ) 
  {
    
    txt << "Error creating request XML ("
        << request.GetErrorLine() 
        << ") :" 
        << request.GetErrorString();
    return FALSE;
  }

  // End with a newline
  soapRequest += "\n";

  PTRACE( 5, "SOAPClient\tOutgoing SOAP is " << soapRequest );

  // do the request
  PHTTPClient client;
  PMIMEInfo sendMIME, replyMIME;
  sendMIME.SetAt( "Server", url.GetHostName() );
  sendMIME.SetAt( PHTTP::ContentTypeTag(), "text/xml" );
  sendMIME.SetAt( "SOAPAction", soapAction );

  if(url.GetUserName() != "") {
      PStringStream SoapAuthToken;
      SoapAuthToken << url.GetUserName() << ":" << url.GetPassword();
      sendMIME.SetAt( "Authorization", PBase64::Encode(SoapAuthToken) );
  }

  // Set thetimeout
  client.SetReadTimeout( timeout );

  // Send the POST request to the server
  BOOL ok = client.PostData( url, sendMIME, soapRequest, replyMIME );

  // Find the length of the response
  PINDEX contentLength;
  if ( replyMIME.Contains( PHTTP::ContentLengthTag() ) )
    contentLength = ( PINDEX ) replyMIME[ PHTTP::ContentLengthTag() ].AsUnsigned();
  else if ( ok)
    contentLength = P_MAX_INDEX;
  else
    contentLength = 0;

  // Retrieve the response
  PString replyBody = client.ReadString( contentLength );

  PTRACE( 5, "PSOAP\tIncoming SOAP is " << replyBody );

  // Check if the server really gave us something
  if ( !ok || replyBody.IsEmpty() ) 
  {
    txt << "HTTP POST failed: "
        << client.GetLastResponseCode() << ' '
        << client.GetLastResponseInfo();
  }

  // Parse the response only if the response code from the server
  // is either 500 (Internal server error) or 200 (RequestOK)

  if ( ( client.GetLastResponseCode() == PHTTP::RequestOK ) ||
       ( client.GetLastResponseCode() == PHTTP::InternalServerError ) )
  {
    if (!response.Load(replyBody)) 
    {
      txt << "Error parsing response XML ("
        << response.GetErrorLine() 
        << ") :" 
        << response.GetErrorString();
      
      PStringArray lines = replyBody.Lines();
      for ( int offset = -2; offset <= 2; offset++ ) {
        int line = response.GetErrorLine() + offset;
        
        if ( line >= 0 && line < lines.GetSize() )
          txt << lines[ ( PINDEX ) line ];
      }
    }
  }


  if ( ( client.GetLastResponseCode() != PHTTP::RequestOK ) &&
       ( client.GetLastResponseCode() != PHTTP::InternalServerError ) && 
       ( !ok ) )
  {
    response.SetFault( PSOAPMessage::Server, txt );
    return FALSE;
  }


  return TRUE;
}


#endif // P_EXPAT


// End of File ////////////////////////////////////////////////////////////////
