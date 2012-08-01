/*
 * pnat.h
 *
 * NAT Strategy support for Portable Windows Library.
 *
 * Virteos is a Trade Mark of ISVO (Asia) Pte Ltd.
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
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
 *
 * The Original Code is derived from and used in conjunction with the 
 * OpenH323 Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pnat.h,v $
 * Revision 1.11  2007/08/22 05:04:39  rjongbloed
 * Added ability to set a specific local port for STUN created sockets.
 *
 * Revision 1.10  2007/07/22 03:07:30  rjongbloed
 * Added parameter so can bind STUN socket to specific interface.
 *
 * Revision 1.9  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.8  2007/02/11 13:07:37  shorne
 * Added GetName function
 *
 * Revision 1.7  2007/01/05 04:09:05  dereksmithies
 * Get it to compile on linux correctly.
 * Previous code failed, with message about pasting STUN and > together.
 *
 * Revision 1.6  2006/12/23 15:08:00  shorne
 * Now Factory loaded for ease of addition of new NAT Methods
 *
 * Revision 1.5  2006/02/26 09:29:10  shorne
 * Added GetMethodName and GetList functions
 *
 * Revision 1.4  2006/01/26 03:23:41  shorne
 * Fix compile error when merging code
 *
 * Revision 1.3  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.2  2005/07/13 11:15:14  csoutheren
 * Backported NAT abstraction files from isvo branch
 *
 * Revision 1.1.2.1  2005/04/25 13:23:19  shorne
 * Initial version
 *
 *
*/

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <ptlib/sockets.h>

#ifndef P_NATMETHOD
#define P_NATMETHOD

#include <ptlib/plugin.h>
#include <ptlib/pluginmgr.h>

/** PNatMethod
    Base Network Address Traversal Method class
    All NAT Traversal Methods are derived off this class. 
    There are quite a few methods of NAT Traversal. The 
    only purpose of this class is to provide a common 
    interface. It is intentionally minimalistic.
*/
class PNatMethod  : public PObject
{
  PCLASSINFO(PNatMethod,PObject);

public:

  /**@name Construction */
  //@{
  /** Default Contructor
  */
  PNatMethod();

  /** Deconstructor
  */
  ~PNatMethod();
  //@}


    /** Factory Create
      */
    static PNatMethod * Create(
      const PString & name,        ///< Feature Name Expression
      PPluginManager * pluginMgr = NULL   ///< Plugin Manager
    );

  /**@name General Functions */
  //@{

  /**  GetExternalAddress
    Get the acquired External IP Address.
  */
   virtual BOOL GetExternalAddress(
      PIPSocket::Address & externalAddress, /// External address of router
      const PTimeInterval & maxAge = 1000   /// Maximum age for caching
   ) = 0;

  /**  CreateSocketPair
    Create the UDP Socket pair
  */
   virtual BOOL CreateSocketPair(
      PUDPSocket * & socket1,
      PUDPSocket * & socket2,
      const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny()
   ) = 0;

  /**Returns whether the Nat Method is ready and available in
     assisting in NAT Traversal. The principal is function is
     to allow the EP to detect various methods and if a method
     is detected then this method is available for NAT traversal
     The Order of adding to the PNstStrategy determines which method
     is used
  */
   virtual BOOL IsAvailable() { return FALSE; };

    /**Set the port ranges to be used on local machine.
       Note that the ports used on the NAT router may not be the same unless
       some form of port forwarding is present.

       If the port base is zero then standard operating system port allocation
       method is used.

       If the max port is zero then it will be automatically set to the port
       base + 99.
      */
   virtual void SetPortRanges(
      WORD portBase,          /// Single socket port number base
      WORD portMax = 0,       /// Single socket port number max
      WORD portPairBase = 0,  /// Socket pair port number base
      WORD portPairMax = 0    /// Socket pair port number max
   );

   /** Get the Method String Name
   */
   static PStringList GetNatMethodName() { return PStringList(); };

   virtual PStringList GetName() const
      { return GetNatMethodName(); }

  //@}

protected:
  struct PortInfo {
      PortInfo(WORD port = 0)
        : basePort(port)
        , maxPort(port)
        , currentPort(port)
      {
      }

      PMutex mutex;
      WORD   basePort;
      WORD   maxPort;
      WORD   currentPort;
    } singlePortInfo, pairedPortInfo;

};

/////////////////////////////////////////////////////////////

PLIST(PNatList, PNatMethod);

/////////////////////////////////////////////////////////////

/** PNatStrategy
  The main container for all
  NAT traversal Strategies. 
*/

class PNatStrategy : public PObject
{
  PCLASSINFO(PNatStrategy,PObject);

public :

  /**@name Construction */
  //@{
  /** Default Contructor
  */
  PNatStrategy();

  /** Deconstructor
  */
  ~PNatStrategy();
  //@}

  /**@name Method Handling */
  //@{
  /** AddMethod
    This function is used to add the required NAT
    Traversal Method. The Order of Loading is important
    The first added has the highest priority.
  */
  void AddMethod(PNatMethod * method);

  /** GetMethod
    This function retrieves the first available NAT
    Traversal Method. If no available NAT Method is found
    then NULL is returned. 
  */
  PNatMethod * GetMethod();


  /** RemoveMethod
    This function removes a NAT method from the NATlist matching the supplied method name
   */
  BOOL RemoveMethod(const PString & meth);

    /**Set the port ranges to be used on local machine.
       Note that the ports used on the NAT router may not be the same unless
       some form of port forwarding is present.

       If the port base is zero then standard operating system port allocation
       method is used.

       If the max port is zero then it will be automatically set to the port
       base + 99.
      */
    void SetPortRanges(
      WORD portBase,          /// Single socket port number base
      WORD portMax = 0,       /// Single socket port number max
      WORD portPairBase = 0,  /// Socket pair port number base
      WORD portPairMax = 0    /// Socket pair port number max
    );

    /** Get Loaded NAT Method List
     */
    PNatList GetNATList() {  return natlist; };

	PNatMethod * LoadNatMethod(const PString & name);

    PStringList GetRegisteredList();

  //@}

private:
  PNatList natlist;
};

////////////////////////////////////////////////////////
//
// declare macros and structures needed for NAT plugins
//

typedef PFactory<PNatMethod> NatFactory;

template <class className> class PNatMethodServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return className::GetNatMethodName(); }
};

#define PCREATE_NAT_PLUGIN(name) \
  static PNatMethodServiceDescriptor<PNatMethod_##name> PNatMethod_##name##_descriptor; \
  PCREATE_PLUGIN(name, PNatMethod, &PNatMethod_##name##_descriptor)

#endif
