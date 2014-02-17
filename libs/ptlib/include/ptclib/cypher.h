/*
 * cypher.h
 *
 * Encryption support classes.
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
 * $Log: cypher.h,v $
 * Revision 1.24  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.23  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.22  2005/01/26 05:37:40  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.21  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.20  2004/03/23 05:59:17  csoutheren
 * Moved the Base64 routines into cypher.cxx, which is a more sensible
 * place and reduces the inclusion of unrelated code
 *
 * Revision 1.19  2004/02/04 02:31:34  csoutheren
 * Remove SHA-1 functions when OpenSSL is disabled
 *
 * Revision 1.18  2003/04/17 03:34:07  craigs
 * Fixed problem with delete'ing a void *
 *
 * Revision 1.17  2003/04/10 07:02:38  craigs
 * Fixed link problem in MD5 class
 *
 * Revision 1.16  2003/04/10 06:16:30  craigs
 * Added SHA-1 digest
 *
 * Revision 1.15  2002/11/06 22:47:23  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.14  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.13  2001/09/10 00:28:21  robertj
 * Fixed extra CR in comments.
 *
 * Revision 1.12  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.11  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.10  1998/09/23 06:19:24  robertj
 * Added open source copyright license.
 *
 * Revision 1.9  1997/10/10 10:44:01  robertj
 * Fixed bug in password encryption, missing string terminator.
 *
 * Revision 1.8  1996/11/16 10:50:24  robertj
 * Fixed bug in registration order form showing incorrect check code when have key.
 *
 * Revision 1.7  1996/07/15 10:29:38  robertj
 * Changed memory block cypher conversion functions to be void *.
 * Changed key types to be structures rather than arrays to avoid pinter/reference confusion by compilers.
 *
 * Revision 1.6  1996/03/17 05:47:00  robertj
 * Changed secured config to allow for expiry dates.
 *
 * Revision 1.5  1996/03/16 04:36:43  robertj
 * Redesign of secure config to accommodate expiry dates and option values passed in security key code.
 *
 * Revision 1.4  1996/02/25 02:52:46  robertj
 * Further secure config development.
 *
 * Revision 1.3  1996/01/28 14:16:11  robertj
 * Further implementation of secure config.
 *
 * Revision 1.2  1996/01/28 02:41:00  robertj
 * Removal of MemoryPointer classes as usage didn't work for GNU.
 * Added the secure configuration mechanism for protecting applications.
 *
 * Revision 1.1  1996/01/23 13:04:20  robertj
 * Initial revision
 *
 */


#ifndef _PCYPHER
#define _PCYPHER

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

/** This class is used to encode/decode data using the MIME standard base64
   encoding mechanism as defined in RFC1521.

   To encode a large block of data use the following seqeunce:
\begin{verbatim}
      PBase64 base;
      base.StartEncoding();
      while (Read(dataChunk)) {
        base.ProcessEncoding(dataChunk);
        out << base.GetEncodedString();
      }
      out << base.CompleteEncoding();
\end{verbatim}

    if smaller blocks that fit easily in memory are to be encoded the
    #Encode()# functions can be used to everything in one go.

    To decode a large block of data use the following sequence:
\begin{verbatim}

      PBase64 base;
      base.StartDecoding();
      while (Read(str) && ProcessDecoding(str))
        Write(base.GetDecodedData());
      Write(base.GetDecodedData());
\end{verbatim}

    if smaller blocks that fit easily in memory are to be decoded the
    #Decode()# functions can be used to everything in one go.
 */
class PBase64 : public PObject
{
  PCLASSINFO(PBase64, PObject);

  public:
    /** Construct a base 64 encoder/decoder and initialise both encode and
       decode members as in #StartEncoding()# and #StartDecoding()#.
     */
    PBase64();

    void StartEncoding(
      BOOL useCRLFs = TRUE  // Use CR, LF pairs in end of line characters.
    );
    // Begin a base 64 encoding operation, initialising the object instance.

    void ProcessEncoding(
      const PString & str      // String to be encoded
    );
    void ProcessEncoding(
      const char * cstr        // C String to be encoded
    );
    void ProcessEncoding(
      const PBYTEArray & data  // Data block to be encoded
    );
    void ProcessEncoding(
      const void * dataBlock,  // Pointer to data to be encoded
      PINDEX length            // Length of the data block.
    );
    // Incorporate the specified data into the base 64 encoding.

    /** Get the partial Base64 string for the data encoded so far.
    
       @return
       Base64 encoded string for the processed data.
     */
    PString GetEncodedString();

    /** Complete the base 64 encoding and return the remainder of the encoded
       Base64 string. Previous data may have been already removed by the
       #GetInterim()# function.
    
       @return
       Base64 encoded string for the processed data.
     */
    PString CompleteEncoding();


    static PString Encode(
      const PString & str     // String to be encoded to Base64
    );
    static PString Encode(
      const char * cstr       // C String to be encoded to Base64
    );
    static PString Encode(
      const PBYTEArray & data // Data block to be encoded to Base64
    );
    static PString Encode(
      const void * dataBlock, // Pointer to data to be encoded to Base64
      PINDEX length           // Length of the data block.
    );
    // Encode the data in memory to Base 64 data returnin the string.


    void StartDecoding();
    // Begin a base 64 decoding operation, initialising the object instance.

    /** Incorporate the specified data into the base 64 decoding.
    
       @return
       TRUE if block was last in the Base64 encoded string.
     */
    BOOL ProcessDecoding(
      const PString & str      // String to be encoded
    );
    BOOL ProcessDecoding(
      const char * cstr        // C String to be encoded
    );

    /** Get the data decoded so far from the Base64 strings processed.
    
       @return
       Decoded data for the processed Base64 string.
     */
    BOOL GetDecodedData(
      void * dataBlock,    // Pointer to data to be decoded from base64
      PINDEX length        // Length of the data block.
    );
    PBYTEArray GetDecodedData();

    /** Return a flag to indicate that the input was decoded without any
       extraneous or illegal characters in it that were ignored. This does not
       mean that the data is not valid, only that it is suspect.
    
       @return
       Decoded data for the processed Base64 string.
     */
    BOOL IsDecodeOK() { return perfectDecode; }


    /** Convert a printable text string to binary data using the Internet MIME
       standard base 64 content transfer encoding.

       The base64 string is checked and TRUE returned if all perfectly correct.
       If FALSE is returned then the string had extraneous or illegal
       characters in it that were ignored. This does not mean that the data is
       not valid, only that it is suspect.
    
       @return
       Base 64 string decoded from input string.
     */
    static PString Decode(
      const PString & str // Encoded base64 string to be decoded.
    );
    static BOOL Decode(
      const PString & str, // Encoded base64 string to be decoded.
      PBYTEArray & data    // Converted binary data from base64.
    );
    static BOOL Decode(
      const PString & str, // Encoded base64 string to be decoded.
      void * dataBlock,    // Pointer to data to be decoded from base64
      PINDEX length        // Length of the data block.
    );



  private:
    void OutputBase64(const BYTE * data);

    PString encodedString;
    PINDEX  encodeLength;
    BYTE    saveTriple[3];
    PINDEX  saveCount;
    PINDEX  nextLine;
    BOOL    useCRLFs;

    BOOL       perfectDecode;
    PINDEX     quadPosition;
    PBYTEArray decodedData;
    PINDEX     decodeSize;
};

class PMessageDigest : public PObject
{
  PCLASSINFO(PMessageDigest, PObject)

  public:
    /// Create a new message digestor
    PMessageDigest();

    class Result {
      public:
        PINDEX GetSize() const          { return value.GetSize(); }
        const BYTE * GetPointer() const { return (const BYTE *)value; }

      private:
        PBYTEArray value;
        friend class PMessageDigest5;
        friend class PMessageDigestSHA1;
    };

    /// Begin a Message Digest operation, initialising the object instance.
    virtual void Start() = 0;

    virtual void Process(
      const void * dataBlock,  ///< Pointer to data to be part of the MD5
      PINDEX length            ///< Length of the data block.
    );

    /** Incorporate the specified data into the message digest. */
    virtual void Process(
      const PString & str      ///< String to be part of the MD5
    );
    /** Incorporate the specified data into the message digest. */
    virtual void Process(
      const char * cstr        ///< C String to be part of the MD5
    );
    /** Incorporate the specified data into the message digest. */
    virtual void Process(
      const PBYTEArray & data  ///< Data block to be part of the MD5
    );

    /**
    Complete the message digest and return the magic number result.
    The parameterless form returns the MD5 code as a Base64 string.
    
    @return
       Base64 encoded MD5 code for the processed data.
    */
    virtual PString CompleteDigest();
    virtual void CompleteDigest(
      Result & result   ///< The resultant 128 bit MD5 code
    );

  protected:
    virtual void InternalProcess(
       const void * dataBlock,  ///< Pointer to data to be part of the MD5
      PINDEX length            ///< Length of the data block.
    ) = 0;

    virtual void InternalCompleteDigest(
      Result & result   ///< The resultant 128 bit MD5 code
    ) = 0;
};


/** MD5 Message Digest.
 A class to produce a Message Digest for a block of text/data using the
 MD5 algorithm as defined in RFC1321 by Ronald Rivest of MIT Laboratory
 for Computer Science and RSA Data Security, Inc.
 */
class PMessageDigest5 : public PMessageDigest
{
  PCLASSINFO(PMessageDigest5, PMessageDigest)

  public:
    /// Create a new message digestor
    PMessageDigest5();

    /// Begin a Message Digest operation, initialising the object instance.
    void Start();

    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const PString & str      ///< String to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PString & str,     ///< String to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const char * cstr        ///< C String to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const char * cstr,       ///< C String to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const PBYTEArray & data  ///< Data block to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PBYTEArray & data, ///< Data block to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const void * dataBlock,  ///< Pointer to data to be encoded to MD5
      PINDEX length            ///< Length of the data block.
    );
    /** Encode the data in memory to and MD5 hash value.
    
    @return
       Base64 encoded MD5 code for the processed data.
    */
    static void Encode(
      const void * dataBlock,  ///< Pointer to data to be encoded to MD5
      PINDEX length,           ///< Length of the data block.
      Result & result            ///< The resultant 128 bit MD5 code
    );

    // backwards compatibility functions
    class Code {
      private:
        PUInt32l value[4];
        friend class PMessageDigest5;
    };

    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PString & str,     ///< String to be encoded to MD5
      Code & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const char * cstr,       ///< C String to be encoded to MD5
      Code & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PBYTEArray & data, ///< Data block to be encoded to MD5
      Code & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value.
   
    @return
       Base64 encoded MD5 code for the processed data.
    */
    static void Encode(
      const void * dataBlock,  ///< Pointer to data to be encoded to MD5
      PINDEX length,           ///< Length of the data block.
      Code & result            ///< The resultant 128 bit MD5 code
    );
    virtual void Complete(
      Code & result   ///< The resultant 128 bit MD5 code
    );
    virtual PString Complete();

  protected:
    virtual void InternalProcess(
       const void * dataBlock,  ///< Pointer to data to be part of the MD5
      PINDEX length            ///< Length of the data block.
    );

    virtual void InternalCompleteDigest(
      Result & result   ///< The resultant 128 bit MD5 code
    );

  private:
    void Transform(const BYTE * block);

    /// input buffer
    BYTE buffer[64];
    /// state (ABCD)
    DWORD state[4];
    /// number of bits, modulo 2^64 (lsb first)
    PUInt64 count;
};

#if P_SSL

/** SHA1 Digest.
 A class to produce a Message Digest for a block of text/data using the
 SHA-1 algorithm 
 */
class PMessageDigestSHA1 : public PMessageDigest
{
  PCLASSINFO(PMessageDigestSHA1, PMessageDigest)

  public:
    /// Create a new message digestor
    PMessageDigestSHA1();
    ~PMessageDigestSHA1();

    /// Begin a Message Digest operation, initialising the object instance.
    void Start();

    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const PString & str      ///< String to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PString & str,     ///< String to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const char * cstr        ///< C String to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const char * cstr,       ///< C String to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const PBYTEArray & data  ///< Data block to be encoded to MD5
    );
    /** Encode the data in memory to and MD5 hash value. */
    static void Encode(
      const PBYTEArray & data, ///< Data block to be encoded to MD5
      Result & result            ///< The resultant 128 bit MD5 code
    );
    /** Encode the data in memory to and MD5 hash value. */
    static PString Encode(
      const void * dataBlock,  ///< Pointer to data to be encoded to MD5
      PINDEX length            ///< Length of the data block.
    );
    /** Encode the data in memory to and MD5 hash value.
    
    @return
       Base64 encoded MD5 code for the processed data.
    */
    static void Encode(
      const void * dataBlock,  ///< Pointer to data to be encoded to MD5
      PINDEX length,           ///< Length of the data block.
      Result & result            ///< The resultant 128 bit MD5 code
    );

  protected:
    virtual void InternalProcess(
       const void * dataBlock,  ///< Pointer to data to be part of the MD5
      PINDEX length            ///< Length of the data block.
    );

    void InternalCompleteDigest(
      Result & result   ///< The resultant 128 bit MD5 code
    );

  private:
    void * shaContext;
};

#endif

/**This abstract class defines an encryption/decryption algortihm.
A specific algorithm is implemented in a descendent class.
*/
class PCypher : public PObject
{
  PCLASSINFO(PCypher, PObject)

  public:
    /// Mechanism by which sequential blocks are linked.
    enum BlockChainMode {
      ElectronicCodebook,
        ECB = ElectronicCodebook,
      CypherBlockChaining,
        CBC = CypherBlockChaining,
      OutputFeedback,
        OFB = OutputFeedback,
      CypherFeedback,
        CFB = CypherFeedback,
      NumBlockChainModes
    };

  // New functions for class
    /**Encode the data. */
    PString Encode(
      const PString & str       ///< Clear text string to be encoded.
    );
    /**Encode the data. */
    PString Encode(
      const PBYTEArray & clear  ///< Clear text binary data to be encoded.
    );
    /**Encode the data. */
    PString Encode(
      const void * data,        ///< Clear text binary data to be encoded.
      PINDEX length             ///< Number of bytes of data to be encoded.
    );
    /**Encode the data. */
    void Encode(
      const PBYTEArray & clear, ///< Clear text binary data to be encoded.
      PBYTEArray & coded        ///< Encoded data.
    );
    /**Encode the data.
    The data is encoded using the algorithm embodied by the descendent class
    and the key specifed in the construction of the objects instance.

    The first form takes a string and returns an encoded string. The second
    form takes arbitrary binary data bytes and returns an encoded string. In
    both cases the encoded string is always 7 bit printable ASCII suitable
    for use in mail systems etc.

    The final form takes and arbitrary block of bytes and encodes them into
    another block of binary data.
    
    @return
      encoded string.
    */
    void Encode(
      const void * data,        // Clear text binary data to be encoded.
      PINDEX length,            // Number of bytes of data to be encoded.
      PBYTEArray & coded        // Encoded data.
    );

    /**Decode the data. */
    PString Decode(
      const PString & cypher   ///< Base64 Cypher text string to be decoded.
    );
    /**Decode the data. */
    BOOL Decode(
      const PString & cypher,  ///< Base64 Cypher text string to be decoded.
      PString & clear          ///< Clear text string decoded.
    );
    /**Decode the data. */
    BOOL Decode(
      const PString & cypher,  ///< Base64 Cypher text string to be decoded.
      PBYTEArray & clear       ///< Clear text binary data decoded.
    );
    /**Decode the data. */
    PINDEX Decode(
      const PString & cypher,  ///< Base64 Cypher text string to be decoded.
      void * data,             ///< Clear text binary data decoded.
      PINDEX length            ///< Maximum number of bytes of data decoded.
    );
    /**Decode the data. */
    PINDEX Decode(
      const PBYTEArray & coded, ///< Encoded data (cyphertext).
      void * data,              ///< Clear text binary data decoded.
      PINDEX length             ///< Maximum number of bytes of data decoded.
    );
    /**Decode the data.
    Decode the data using the algorithm embodied by the descendent class
    and the key specifed in the construction of the objects instance.

    The first form takes a string and returns a decoded string. The second
    form takes an encoded string and returns arbitrary binary data bytes. In
    both cases the encoded string is always 7 bit printable ASCII suitable
    for use in mail systems etc.

    The final form takes and arbitrary block of bytes and decodes them into
    another block of binary data.
    
    @return
      decoded string.
    */
    BOOL Decode(
      const PBYTEArray & coded, ///< Encoded data (cyphertext).
      PBYTEArray & clear       ///< Clear text binary data decoded.
    );


  protected:
    /**
    Create a new encryption object instance.
    */
    PCypher(
      PINDEX blockSize,          ///< Size of encryption blocks (in bits)
      BlockChainMode chainMode   ///< Block chain mode
    );
    PCypher(
      const void * keyData,    ///< Key for the encryption/decryption algorithm.
      PINDEX keyLength,        ///< Length of the key.
      PINDEX blockSize,        ///< Size of encryption blocks (in bits)
      BlockChainMode chainMode ///< Block chain mode
    );


    /** Initialise the encoding/decoding sequence. */
    virtual void Initialise(
      BOOL encoding   ///< Flag for encoding/decoding sequence about to start.
    ) = 0;

    /** Encode an n bit block of memory according to the encryption algorithm. */
    virtual void EncodeBlock(
      const void * in,    ///< Pointer to clear n bit block.
      void * out          ///< Pointer to coded n bit block.
    ) = 0;


    /** Dencode an n bit block of memory according to the encryption algorithm. */
    virtual void DecodeBlock(
      const void * in,  ///< Pointer to coded n bit block.
      void * out        ///< Pointer to clear n bit block.
    ) = 0;


    /// Key for the encryption/decryption.
    PBYTEArray key;
    /// Size of each encryption block in bytes
    PINDEX blockSize;
    /// Mode for sequential encryption each block
    BlockChainMode chainMode;
};


/** Tiny Encryption Algorithm.
This class implements the Tiny Encryption Algorithm by David Wheeler and
Roger Needham at Cambridge University.

This is a simple algorithm using a 128 bit binary key and encrypts data in
64 bit blocks.
*/
class PTEACypher : public PCypher
{
  PCLASSINFO(PTEACypher, PCypher)

  public:
    struct Key {
      BYTE value[16];
    };

    /**
    Create a new TEA encryption object instance. The parameterless version
    automatically generates a new, random, key.
    */
    PTEACypher(
      BlockChainMode chainMode = ElectronicCodebook   ///< Block chain mode
    );
    PTEACypher(
      const Key & keyData,     ///< Key for the encryption/decryption algorithm.
      BlockChainMode chainMode = ElectronicCodebook   ///< Block chain mode
    );


    /** Set the key used by this encryption method. */
    void SetKey(
      const Key & newKey    ///< Variable to take the key used by cypher.
    );

    /** Get the key used by this encryption method. */
    void GetKey(
      Key & newKey    ///< Variable to take the key used by cypher.
    ) const;


    /** Generate a new key suitable for use for encryption using random data. */
    static void GenerateKey(
      Key & newKey    ///< Variable to take the newly generated key.
    );


  protected:
    /** Initialise the encoding/decoding sequence. */
    virtual void Initialise(
      BOOL encoding   ///< Flag for encoding/decoding sequence about to start.
    );

    /** Encode an n bit block of memory according to the encryption algorithm. */
    virtual void EncodeBlock(
      const void * in,  ///< Pointer to clear n bit block.
      void * out        ///< Pointer to coded n bit block.
    );

    /** Decode an n bit block of memory according to the encryption algorithm. */
    virtual void DecodeBlock(
      const void * in,  ///< Pointer to coded n bit block.
      void * out        ///< Pointer to clear n bit block.
    );

  private:
    DWORD k0, k1, k2, k3;
};


#ifdef P_CONFIG_FILE

class PSecureConfig : public PConfig
{
  PCLASSINFO(PSecureConfig, PConfig)
/* This class defines a set of configuration keys which may be secured by an
   encrypted hash function. Thus values contained in keys specified by this
   class cannot be changed without invalidating the hash function.
 */

  public:
    PSecureConfig(
      const PTEACypher::Key & productKey,    // Key to decrypt validation code.
      const PStringArray    & securedKeys,   // List of secured keys.
      Source src = Application        // Standard source for the configuration.
    );
    PSecureConfig(
      const PTEACypher::Key & productKey,   // Key to decrypt validation code.
      const char * const * securedKeyArray, // List of secured keys.
      PINDEX count,                         // Number of secured keys in list.
      Source src = Application        // Standard source for the configuration.
    );
    /* Create a secured configuration. The default section for the
       configuration keys is "Secured Options", the default security key is
       "Validation" and the defualt prefix string is "Pending:".

       The user can descend from this class and change any of the member
       variable for the names of keys or the configuration file section.
     */


  // New functions for class
    const PStringArray & GetSecuredKeys() const { return securedKeys; }
    /* Get the list of secured keys in the configuration file section.

       @return
       Array of  strings for the secured keys.
     */

    const PString & GetSecurityKey() const { return securityKey; }
    /* Get the security keys name in the configuration file section.

       @return
       String for the security values key.
     */

    const PString & GetExpiryDateKey() const { return expiryDateKey; }
    /* Get the expiry date keys name in the configuration file section.

       @return
       String for the expiry date values key.
     */

    const PString & GetOptionBitsKey() const { return optionBitsKey; }
    /* Get the Option Bits keys name in the configuration file section.

       @return
       String for the Option Bits values key.
     */

    const PString & GetPendingPrefix() const { return pendingPrefix; }
    /* Get the pending prefix name in the configuration file section.

       @return
       String for the pending prefix.
     */

    void GetProductKey(
      PTEACypher::Key & productKey  // Variable to receive the product key.
    ) const;
    /* Get the pending prefix name in the configuration file section.

       @return
       String for the pending prefix.
     */


    enum ValidationState {
      Defaults,
      Pending,
      IsValid,
      Expired,
      Invalid
    };
    ValidationState GetValidation() const;
    /* Check the current values attached to the keys specified in the
       constructor against an encoded validation key.

       @return
       State of the validation keys.
     */

    BOOL ValidatePending();
    /* Validate a pending secured option list for the product. All secured
       keys with the <CODE>pendingPrefix</CODE> name will be checked against
       the value of the field <CODE>securityKey</CODE>. If they match then
       they are copied to the secured variables.

       @return
       TRUE if secure key values are valid.
     */

    void ResetPending();
    /* "Unvalidate" a security configuration going back to a pending state,
       usually used after an <CODE>Invalid</CODE> response was recieved from
       the <A>GetValidation()</A> function.
     */


  protected:
    PTEACypher::Key productKey;
    PStringArray    securedKeys;
    PString         securityKey;
    PString         expiryDateKey;
    PString         optionBitsKey;
    PString         pendingPrefix;
};

#endif // P_CONFIG_FILE

#endif // _PCYPHER


// End Of File ///////////////////////////////////////////////////////////////
