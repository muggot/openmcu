/*
 * psoap.h
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
 * $Log: psoap.h,v $
 * Revision 1.5  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.4  2003/03/31 06:21:19  craigs
 * Split the expat wrapper from the XML file handling to allow reuse of the parser
 *
 * Revision 1.3  2003/02/09 23:31:39  robertj
 * Added referention PString's for efficiency.
 *
 * Revision 1.2  2003/02/09 23:22:37  robertj
 * Fixed spelling errors, and setting return values, thanks Andreas Sikkema
 *
 * Revision 1.1  2003/02/04 22:46:48  robertj
 * Added basic SOAP support, thanks Andreas Sikkema
 *
 */


#ifndef _PSOAP_H
#define _PSOAP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#if P_EXPAT

#include <ptclib/pxml.h>
#include <ptclib/http.h>


#define DEFAULT_SOAP_URL "/soap"


/**
 SOAP Message classes
 ####################
 */

//! SOAP message according to http://www.w3.org/TR/SOAP/
class PSOAPMessage : public PXML
{
  PCLASSINFO(PSOAPMessage, PXML);
public:
  
  //! Construct a SOAP message 
  PSOAPMessage( int options = PXMLParser::Indent + PXMLParser::NewLineAfterElement );

  //! Construct a SOAP message with method name and namespace already provided
  PSOAPMessage( const PString & method, const PString & nameSpace );

  //! Set the method name and namespace
  void SetMethod( const PString & name, const PString & nameSpace );

  //! Get the method name and namespace
  void GetMethod( PString & name, PString & nameSpace );
  
  //! Add a simple parameter called name, with type type and value value
  void AddParameter( PString name, PString type, PString value );

  //! Add a parameter using a PXMLElement
  void AddParameter( PXMLElement* parameter, BOOL dirty = TRUE );

  //! Get parameter "name" with type "string"
  BOOL GetParameter( const PString & name, PString & value );

  //! Get parameter "name" with type "int"
  BOOL GetParameter( const PString & name, int & value );

  //! Get parameter "name"
  PXMLElement* GetParameter( const PString & name );

  //! Print the contents of this SOAP message on an ostream
  void PrintOn(ostream & strm) const;

  //! Output the SOAP message to a string
  PString AsString( void );
  
  //! Parse a string for a valid SOAP message
  BOOL Load(const PString & str);

  //! State of the PSOAPMessage when used as a response
  enum 
  {
    //! Everything is alright
    NoFault,
    //! Invalid namespace for SOAP Envelope
    VersionMismatch,
    //! Error processing SOAP Header field "mustUnderstand"
    MustUnderstand,
    //! The request was incorrectly formed or did not contain the appropriate information in order to succeed
    Client,
    //! The request could not be processed for reasons not directly attributable to the contents of the message itself but rather to the processing of the message
    Server
  };

  PINDEX  GetFaultCode() const                     { return faultCode; }
  PString GetFaultText() const                     { return faultText; }
  void SetFault( PINDEX code, const PString & text );

private:
  PXMLElement* pSOAPBody;
  PXMLElement* pSOAPMethod;
  PString faultText;
  PINDEX  faultCode;
};


/**
 SOAP Server classes
 ####################
 */

class PSOAPServerRequestResponse : public PObject 
{
  PCLASSINFO( PSOAPServerRequestResponse, PObject );
  public:
    PSOAPServerRequestResponse( PSOAPMessage & _request )
      : request( _request ) { }

    PSOAPMessage & request;
    PSOAPMessage response;
};


//! Create an association between a method and its "notifier", the handler function
class PSOAPServerMethod : public PString
{
  PCLASSINFO( PSOAPServerMethod, PString );
  public:
    PSOAPServerMethod( const PString & name ) 
      : PString( name ) { }

    PNotifier methodFunc;
};

PSORTED_LIST(PSOAPServerMethodList, PSOAPServerMethod);


//! This resource will bind the methods to an http resource (a url)
class PSOAPServerResource : public PHTTPResource
{
  PCLASSINFO( PSOAPServerResource, PHTTPResource );
  public:
    PSOAPServerResource();
    PSOAPServerResource(
      const PHTTPAuthority & auth    ///< Authorisation for the resource.
    );
    PSOAPServerResource(
      const PURL & url               ///< Name of the resource in URL space.
    );
    PSOAPServerResource(
      const PURL & url,              ///< Name of the resource in URL space.
      const PHTTPAuthority & auth    ///< Authorisation for the resource.
    );

    // overrides from PHTTPResource
    BOOL LoadHeaders( PHTTPRequest & request );
    BOOL OnPOSTData( PHTTPRequest & request, const PStringToString & data );

    // new functions
    virtual BOOL OnSOAPRequest( const PString & body, PString & reply );
    virtual BOOL SetMethod( const PString & methodName, const PNotifier & func );
    BOOL OnSOAPRequest( const PString & methodName, PSOAPMessage & request, PString & reply );

    virtual PSOAPMessage FormatFault( PINDEX code, const PString & str );

    //! Use this method to have the server check for SOAPAction field in HTTP header
    /*! Default is " ", which means don't care, more or less. Anything else means 
        the header has to be filled in with something meaningful. It has to exist 
        anyway. */
    void SetSOAPAction( PString saction ) { soapAction = saction; }

  protected:
    PMutex methodMutex;
    PSOAPServerMethodList methodList;
  private:
    PString soapAction;
};


/**
 SOAP client classes
 ####################
 */

class PSOAPClient : public PObject
{
  PCLASSINFO( PSOAPClient, PObject );
  public:

    PSOAPClient( const PURL & url );

    void SetTimeout( const PTimeInterval & _timeout ) { timeout = _timeout; }

    BOOL MakeRequest( const PString & method, const PString & nameSpace );
    BOOL MakeRequest( const PString & method, const PString & nameSpace,  PSOAPMessage & response );
    BOOL MakeRequest( PSOAPMessage  & request, PSOAPMessage & response );

    PString GetFaultText() const { return faultText; }
    PINDEX  GetFaultCode() const { return faultCode; }

    //! Set a specific SOAPAction field in the HTTTP header, default = " " 
    void setSOAPAction( PString saction ) { soapAction = saction; }
  protected:
    BOOL PerformRequest( PSOAPMessage & request, PSOAPMessage & response );

    PURL url;
    PINDEX  faultCode;
    PString faultText;
    PTimeInterval timeout;
  private:
    PString soapAction;
};


#endif // P_EXPAT


#endif // _PSOAP_H


// End of file ////////////////////////////////////////////////////////////////
