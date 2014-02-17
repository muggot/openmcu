/*
 * pssl.h
 *
 * Secure Sockets Layer channel interface class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
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
 * $Log: pssl.h,v $
 * Revision 1.20  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.19  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.18  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.17  2002/03/28 07:26:36  robertj
 * Added Diffie-Hellman parameters wrapper class.
 *
 * Revision 1.16  2001/12/13 09:15:20  robertj
 * Added function to get private key as ray DER binary data or as base64 string.
 *
 * Revision 1.15  2001/12/06 04:06:03  robertj
 * Removed "Win32 SSL xxx" build configurations in favour of system
 *   environment variables to select optional libraries.
 *
 * Revision 1.14  2001/10/31 01:30:40  robertj
 * Added enhancements for saving/loading/creating certificates and keys.
 *
 * Revision 1.13  2001/09/10 02:51:22  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.12  2001/06/01 00:53:59  robertj
 * Added certificate constructor that takes a PBYTEArray
 *
 * Revision 1.11  2001/05/16 06:02:05  craigs
 * Changed to allow detection of non-SSL connection to SecureHTTPServiceProcess
 *
 * Revision 1.10  2000/11/14 08:33:16  robertj
 * Added certificate and private key classes.
 *
 * Revision 1.9  2000/08/25 08:11:02  robertj
 * Fixed OpenSSL support so can operate as a server channel.
 *
 * Revision 1.8  2000/08/04 12:52:18  robertj
 * SSL changes, added error functions, removed need to have openssl include directory in app.
 *
 * Revision 1.7  2000/01/10 02:23:18  craigs
 * Update for new OpenSSL
 *
 * Revision 1.6  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.5  1998/12/04 13:01:51  craigs
 * Changed for SSLeay 0.9
 *
 * Revision 1.4  1998/09/23 06:19:50  robertj
 * Added open source copyright license.
 *
 * Revision 1.3  1997/05/04 02:49:52  craigs
 * Added support for client and server certificates
 *
 * Revision 1.1  1996/11/15 07:37:48  craigs
 * Initial revision
 *
 */

#ifndef _PSSL_H
#define _PSSL_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/sockets.h>


struct ssl_st;
struct ssl_ctx_st;
struct x509_st;
struct evp_pkey_st;
struct dh_st;

enum PSSLFileTypes {
  PSSLFileTypePEM,
  PSSLFileTypeASN1,
  PSSLFileTypeDEFAULT
};


/**Private key for SSL.
   This class embodies a common environment for all private keys used by the
   PSSLContext and PSSLChannel classes.
  */
class PSSLPrivateKey : public PObject
{
  PCLASSINFO(PSSLPrivateKey, PObject);
  public:
    /**Create an empty private key.
      */
    PSSLPrivateKey();

    /**Create a new RSA private key.
      */
    PSSLPrivateKey(
      unsigned modulus,   ///< Number of bits
      void (*callback)(int,int,void *) = NULL,  ///< Progress callback function
      void *cb_arg = NULL                       ///< Argument passed to callback
    );

    /**Create a new private key given the file.
       The type of the private key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    PSSLPrivateKey(
      const PFilePath & keyFile,  ///< Private key file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

    /**Create private key from the binary ASN1 DER encoded data specified.
      */
    PSSLPrivateKey(
      const BYTE * keyData,   ///< Private key data
      PINDEX keySize          ///< Size of private key data
    );

    /**Create private key from the binary ASN1 DER encoded data specified.
      */
    PSSLPrivateKey(
      const PBYTEArray & keyData  ///< Private key data
    );

    /**Create a copy of the private key.
      */
    PSSLPrivateKey(
      const PSSLPrivateKey & privKey
    );

    /**Create a copy of the private key.
      */
    PSSLPrivateKey & operator=(
      const PSSLPrivateKey & privKay
    );

    /**Destroy and release storage for private key.
      */
    ~PSSLPrivateKey();

    /**Get internal OpenSSL private key structure.
      */
    operator evp_pkey_st *() const { return key; }

    /**Create a new private key.
     */
    BOOL Create(
      unsigned modulus,   ///< Number of bits
      void (*callback)(int,int,void *) = NULL,  ///< Progress callback function
      void *cb_arg = NULL                       ///< Argument passed to callback
    );

    /**Get the certificate as binary ASN1 DER encoded data.
      */
    PBYTEArray GetData() const;

    /**Get the certificate as ASN1 DER base64 encoded data.
      */
    PString AsString() const;

    /**Load private key from file.
       The type of the private key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    BOOL Load(
      const PFilePath & keyFile,  ///< Private key file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

    /**Save private key to file.
       The type of the private key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    BOOL Save(
      const PFilePath & keyFile,  ///< Private key file
      BOOL append = FALSE,        ///< Append to file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to write
    );


  protected:
    evp_pkey_st * key;
};


/**Certificate for SSL.
   This class embodies a common environment for all certificates used by the
   PSSLContext and PSSLChannel classes.
  */
class PSSLCertificate : public PObject
{
  PCLASSINFO(PSSLCertificate, PObject);
  public:
    /**Create an empty certificate.
      */
    PSSLCertificate();

    /**Create a new certificate given the file.
       The type of the certificate key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    PSSLCertificate(
      const PFilePath & certFile, ///< Certificate file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

    /**Create certificate from the binary ASN1 DER encoded data specified.
      */
    PSSLCertificate(
      const BYTE * certData,  ///< Certificate data
      PINDEX certSize        ///< Size of certificate data
    );

    /**Create certificate from the binary ASN1 DER encoded data specified.
      */
    PSSLCertificate(
      const PBYTEArray & certData  ///< Certificate data
    );

    /**Create certificate from the ASN1 DER base64 encoded data specified.
      */
    PSSLCertificate(
      const PString & certString  ///< Certificate data as string
    );

    /**Create a copy of the certificate.
      */
    PSSLCertificate(
      const PSSLCertificate & cert
    );

    /**Create a copy of the certificate.
      */
    PSSLCertificate & operator=(
      const PSSLCertificate & cert
    );

    /**Destroy and release storage for certificate.
      */
    ~PSSLCertificate();

    /**Get internal OpenSSL X509 structure.
      */
    operator x509_st *() const { return certificate; }

    /**Create a new root certificate.
       The subject name is a string of the form "/name=value/name=value" where
       name is a short name for the field and value is a string value for the
       field for example:
          "/C=ZA/SP=Western Cape/L=Cape Town/O=Thawte Consulting cc"
          "/OU=Certification Services Division/CN=Thawte Server CA"
          "/Email=server-certs@thawte.com"
     */
    BOOL CreateRoot(
      const PString & subject,    ///< Subject name for certificate
      const PSSLPrivateKey & key  ///< Key to sign certificate with
    );

    /**Get the certificate as binary ASN1 DER encoded data.
      */
    PBYTEArray GetData() const;

    /**Get the certificate as ASN1 DER base64 encoded data.
      */
    PString AsString() const;

    /**Load certificate from file.
       The type of the certificate key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    BOOL Load(
      const PFilePath & certFile, ///< Certificate file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

    /**Save certificate to file.
       The type of the certificate key can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    BOOL Save(
      const PFilePath & keyFile,  ///< Certificate key file
      BOOL append = FALSE,        ///< Append to file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to write
    );


  protected:
    x509_st * certificate;
};


/**Diffie-Hellman parameters for SSL.
   This class embodies a set of Diffie Helman parameters as used by
   PSSLContext and PSSLChannel classes.
  */
class PSSLDiffieHellman : public PObject
{
  PCLASSINFO(PSSLDiffieHellman, PObject);
  public:
    /**Create an empty set of Diffie-Hellman parameters.
      */
    PSSLDiffieHellman();

    /**Create a new set of Diffie-Hellman parameters given the file.
       The type of the file can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    PSSLDiffieHellman(
      const PFilePath & dhFile, ///< Diffie-Hellman parameters file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

    /**Create a set of Diffie-Hellman parameters.
      */
    PSSLDiffieHellman(
      const BYTE * pData, ///< P data
      PINDEX pSize,       ///< Size of P data
      const BYTE * gData, ///< G data
      PINDEX gSize        ///< Size of G data
    );

    /**Create a copy of the Diffie-Hellman parameters.
      */
    PSSLDiffieHellman(
      const PSSLDiffieHellman & dh
    );

    /**Create a copy of the Diffie-Hellman parameters.
      */
    PSSLDiffieHellman & operator=(
      const PSSLDiffieHellman & dh
    );

    /**Destroy and release storage for Diffie-Hellman parameters.
      */
    ~PSSLDiffieHellman();

    /**Get internal OpenSSL DH structure.
      */
    operator dh_st *() const { return dh; }

    /**Load Diffie-Hellman parameters from file.
       The type of the file can be specified explicitly, or if
       PSSLFileTypeDEFAULT it will be determined from the file extension,
       ".pem" is a text file, anything else eg ".der" is a binary ASN1 file.
      */
    BOOL Load(
      const PFilePath & dhFile, ///< Diffie-Hellman parameters file
      PSSLFileTypes fileType = PSSLFileTypeDEFAULT  ///< Type of file to read
    );

  protected:
    dh_st * dh;
};


/**Context for SSL channels.
   This class embodies a common environment for all connections made via SSL
   using the PSSLChannel class. It includes such things as the version of SSL
   and certificates, CA's etc.
  */
class PSSLContext {
  public:
    /**Create a new context for SSL channels.
       An optional session ID may be provided in the context. This is used
       to identify sessions across multiple channels in this context. The
       session ID is a completely arbitrary block of data. If sessionId is
       non NULL and idSize is zero, then sessionId is assumed to be a pointer
       to a C string.
      */
    PSSLContext(
      const void * sessionId = NULL,  ///< Pointer to session ID
      PINDEX idSize = 0               ///< Size of session ID
    );

    /**Clean up the SSL context.
      */
    ~PSSLContext();

    /**Get the internal SSL context structure.
      */
    operator ssl_ctx_st *() const { return context; }

    /**Set the path to locate CA certificates.
      */
    BOOL SetCAPath(
      const PDirectory & caPath   ///< Directory for CA certificates
    );

    /**Set the CA certificate file.
      */
    BOOL SetCAFile(
      const PFilePath & caFile    ///< CA certificate file
    );

    /**Use the certificate specified.
      */
    BOOL UseCertificate(
      const PSSLCertificate & certificate
    );

    /**Use the private key specified.
      */
    BOOL UsePrivateKey(
      const PSSLPrivateKey & key
    );

    /**Use the Diffie-Hellman parameters specified.
      */
    BOOL UseDiffieHellman(
      const PSSLDiffieHellman & dh
    );

    /**Set the available ciphers to those listed.
      */
    BOOL SetCipherList(
      const PString & ciphers   ///< List of cipher names.
    );

  protected:
    ssl_ctx_st * context;
};


/**This class will start a secure SSL based channel.
  */
class PSSLChannel : public PIndirectChannel
{
  PCLASSINFO(PSSLChannel, PIndirectChannel)
  public:
    /**Create a new channel given the context.
       If no context is given a default one is created.
      */
    PSSLChannel(
      PSSLContext * context = NULL,   ///< Context for SSL channel
      BOOL autoDeleteContext = FALSE  ///< Flag for context to be automatically deleted.
    );
    PSSLChannel(
      PSSLContext & context           ///< Context for SSL channel
    );

    /**Close and clear the SSL channel.
      */
    ~PSSLChannel();

    // Overrides from PChannel
    virtual BOOL Read(void * buf, PINDEX len);
    virtual BOOL Write(const void * buf, PINDEX len);
    virtual BOOL Close();
    virtual BOOL Shutdown(ShutdownValue) { return TRUE; }
    virtual PString GetErrorText(ErrorGroup group = NumErrorGroups) const;
    virtual BOOL ConvertOSError(int error, ErrorGroup group = LastGeneralError);

    // New functions
    /**Accept a new inbound connection (server).
       This version expects that the indirect channel has already been opened
       using Open() beforehand.
      */
    BOOL Accept();

    /**Accept a new inbound connection (server).
      */
    BOOL Accept(
      PChannel & channel  ///< Channel to attach to.
    );

    /**Accept a new inbound connection (server).
      */
    BOOL Accept(
      PChannel * channel,     ///< Channel to attach to.
      BOOL autoDelete = TRUE  ///< Flag for if channel should be automatically deleted.
    );


    /**Connect to remote server.
       This version expects that the indirect channel has already been opened
       using Open() beforehand.
      */
    BOOL Connect();

    /**Connect to remote server.
      */
    BOOL Connect(
      PChannel & channel  ///< Channel to attach to.
    );

    /**Connect to remote server.
      */
    BOOL Connect(
      PChannel * channel,     ///< Channel to attach to.
      BOOL autoDelete = TRUE  ///< Flag for if channel should be automatically deleted.
    );

    /**Use the certificate specified.
      */
    BOOL UseCertificate(
      const PSSLCertificate & certificate
    );

    /**Use the private key file specified.
      */
    BOOL UsePrivateKey(
      const PSSLPrivateKey & key
    );

    enum VerifyMode {
      VerifyNone,
      VerifyPeer,
      VerifyPeerMandatory,
    };

    void SetVerifyMode(
      VerifyMode mode
    );

    PSSLContext * GetContext() const { return context; }

    virtual BOOL RawSSLRead(void * buf, PINDEX & len);

  protected:
    /**This callback is executed when the Open() function is called with
       open channels. It may be used by descendent channels to do any
       handshaking required by the protocol that channel embodies.

       The default behaviour "connects" the channel to the OpenSSL library.

       @return
       Returns TRUE if the protocol handshaking is successful.
     */
    virtual BOOL OnOpen();

  protected:
    PSSLContext * context;
    BOOL          autoDeleteContext;
    ssl_st      * ssl;
};

#endif // _PSSL_H
