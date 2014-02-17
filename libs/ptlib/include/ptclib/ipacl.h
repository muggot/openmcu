/*
 * ipacl.h
 *
 * IP Access Control Lists
 *
 * Portable Windows Library
 *
 * Copyright (c) 1998-2002 Equivalence Pty. Ltd.
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
 * $Log: ipacl.h,v $
 * Revision 1.11  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.10  2005/01/26 05:37:42  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.9  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.8  2002/07/17 02:54:24  robertj
 * Added access functions for member variables.
 *
 * Revision 1.7  2002/06/19 05:43:17  robertj
 * Added missing return for getting default allowance flag
 *
 * Revision 1.6  2002/06/19 04:02:58  robertj
 * Added default allowance boolean if ACL empty.
 * Added ability to override the creation of ACL entry objects with descendents
 *   so an application can add information/functionality to each entry.
 *
 * Revision 1.5  2002/02/13 02:07:14  robertj
 * Added const to IsAllowed() function
 *
 * Revision 1.4  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.3  1999/02/25 05:05:15  robertj
 * Added missing test for hidden entries not to be written to config file
 *
 * Revision 1.2  1999/02/08 08:05:39  robertj
 * Changed semantics of IP access control list for empty list.
 *
 * Revision 1.1  1999/01/31 00:59:26  robertj
 * Added IP Access Control List class to PTLib Components
 *
 */

#ifndef _IPACL_H
#define _IPACL_H


#include <ptlib/sockets.h>


/** This class is a single IP access control specification.
 */
class PIpAccessControlEntry : public PObject
{
  PCLASSINFO(PIpAccessControlEntry, PObject)

  public:
    /** Create a new IP access control specification. See the Parse() function
       for more details on the format of the <CODE>description</CODE>
       parameter.
     */
    PIpAccessControlEntry(
      PIPSocket::Address addr,
      PIPSocket::Address msk,
      BOOL allow
    );
    PIpAccessControlEntry(
      const PString & description
    );

    /** Set a new IP access control specification. See the Parse() function
       for more details on the format of the <CODE>pstr</CODE> and
       <CODE>cstr</CODE> parameters.
     */
    PIpAccessControlEntry & operator=(
      const PString & pstr
    );
    PIpAccessControlEntry & operator=(
      const char * cstr
    );

    /** Compare the two objects and return their relative rank.

       @return
       <CODE>LessThan</CODE>, <CODE>EqualTo</CODE> or <CODE>GreaterThan</CODE>
       according to the relative rank of the objects.
     */
    virtual Comparison Compare(
      const PObject & obj   ///< Object to compare against.
    ) const;

    /** Output the contents of the object to the stream. This outputs the same
       format as the AsString() function.
     */
    virtual void PrintOn(
      ostream &strm   ///< Stream to print the object into.
    ) const;

    /** Input the contents of the object from the stream. This expects the
       next space delimited entry in the stream to be as described in the
       Parse() function.
     */
    virtual void ReadFrom(
      istream &strm   ///< Stream to read the objects contents from.
    );

    /** Convert the specification to a string, that can be processed by the
       Parse() function.

       @return
       PString representation of the entry.
     */
    PString AsString() const;

    /** Check the internal fields of the specification for validity.

       @return
       TRUE if entry is valid.
     */
    BOOL IsValid();

    /** Parse the description string into this IP access control specification.
       The string may be of several forms:
          n.n.n.n         Simple IP number, this has an implicit mask of
                          255.255.255.255
          n.n.            IP with trailing dot, assumes a mask equal to the
                          number of specified octets eg 10.1. is equivalent
                          to 10.1.0.0/255.255.0.0
          n.n.n.n/b       An IP network using b bits of mask, for example
                          10.1.0.0/14 is equivalent to 10.0.1.0/255.248.0.0
          n.n.n.n/m.m.m.m An IP network using the specified mask
          hostname        A specific host name, this has an implicit mask of
                          255.255.255.255
          .domain.dom     Matches an IP number whose cannonical name (found
                          using a reverse DNS lookup) ends with the specified
                          domain.

       @return
       TRUE if entry is valid.
     */
    BOOL Parse(
      const PString & description   ///< Description of the specification
    );


    /** Check to see if the specified IP address match any of the conditions
       specifed in the Parse() function for this entry.

       @return
       TRUE if entry can match the address.
     */
    BOOL Match(
      PIPSocket::Address & address    ///< Address to search for
    );

    /**Get the domain part of entry.
      */
    const PString & GetDomain() const { return domain; }

    /**Get the address part of entry.
      */
    const PIPSocket::Address & GetAddress() const { return address; }

    /**Get the mask part of entry.
      */
    const PIPSocket::Address & GetMask() const { return mask; }

    /**Get the allowed flag of entry.
      */
    BOOL IsAllowed() const { return allowed; }

    /**Get the hidden flag of entry.
      */
    BOOL IsHidden()  const { return hidden; }

  protected:
    PString            domain;
    PIPSocket::Address address;
    PIPSocket::Address mask;
    BOOL               allowed;
    BOOL               hidden;
};

PSORTED_LIST(PIpAccessControlList_base, PIpAccessControlEntry);


/** This class is a list of IP address mask specifications used to validate if
   an address may or may not be used in a connection.

   The list may be totally internal to the application, or may use system
   wide files commonly use under Linux (hosts.allow and hosts.deny file). These
   will be used regardless of the platform.

   When a search is done using IsAllowed() function, the first entry that
   matches the specified IP address is found, and its allow flag returned. The
   list sorted so that the most specific IP number specification is first and
   the broadest onse later. The entry with the value having a mask of zero,
   that is the match all entry, is always last.
 */
class PIpAccessControlList : public PIpAccessControlList_base
{

  PCLASSINFO(PIpAccessControlList, PIpAccessControlList_base)

  public:
    /** Create a new, empty, access control list.
     */
    PIpAccessControlList(
      BOOL defaultAllowance = TRUE
    );

    /** Load the system wide files commonly use under Linux (hosts.allow and
       hosts.deny file) for IP access. See the Linux man entries on these
       files for more information. Note, these files will be loaded regardless
       of the actual platform used. The directory returned by the
       PProcess::GetOSConfigDir() function is searched for the files.

       The <CODE>daemonName</CODE> parameter is used as the search argument in
       the hosts.allow/hosts.deny file. If this is NULL then the
       PProcess::GetName() function is used.

       @return
       TRUE if all the entries in the file were added, if any failed then
       FALSE is returned.
     */
    BOOL LoadHostsAccess(
      const char * daemonName = NULL    ///< Name of "daemon" application
    );

#ifdef P_CONFIG_FILE

    /** Load entries in the list from the configuration file specified. This is
       equivalent to Load(cfg, "IP Access Control List").

       @return
       TRUE if all the entries in the file were added, if any failed then
       FALSE is returned.
     */
    BOOL Load(
      PConfig & cfg   ///< Configuration file to load entries from.
    );

    /** Load entries in the list from the configuration file specified, using
       the base name for the array of configuration file values. The format of
       entries in the configuration file are suitable for use with the
       PHTTPConfig classes.

       @return
       TRUE if all the entries in the file were added, if any failed then
       FALSE is returned.
     */
    BOOL Load(
      PConfig & cfg,            ///< Configuration file to load entries from.
      const PString & baseName  ///< Base name string for each entry in file.
    );

    /** Save entries in the list to the configuration file specified. This is
       equivalent to Save(cfg, "IP Access Control List").
     */
    void Save(
      PConfig & cfg   ///< Configuration file to save entries to.
    );

    /** Save entries in the list to the configuration file specified, using
       the base name for the array of configuration file values. The format of
       entries in the configuration file are suitable for use with the
       PHTTPConfig classes.
     */
    void Save(
      PConfig & cfg,            ///< Configuration file to save entries to.
      const PString & baseName  ///< Base name string for each entry in file.
    );

#endif // P_CONFIG_FILE

    /** Add the specified entry into the list. See the PIpAccessControlEntry
       class for more details on the format of the <CODE>description</CODE>
       field.

       @return
       TRUE if the entries was successfully added.
     */
    BOOL Add(
      PIpAccessControlEntry * entry ///< Entry for IP match parameters
    );
    BOOL Add(
      const PString & description   ///< Description of the IP match parameters
    );
    BOOL Add(
      PIPSocket::Address address,   ///< IP network address
      PIPSocket::Address mask,      ///< Mask for IP network
      BOOL allow                    ///< Flag for if network is allowed or not
    );

    /** Remove the specified entry into the list. See the PIpAccessControlEntry
       class for more details on the format of the <CODE>description</CODE>
       field.

       @return
       TRUE if the entries was successfully removed.
     */
    BOOL Remove(
      const PString & description   ///< Description of the IP match parameters
    );
    BOOL Remove(
      PIPSocket::Address address,   ///< IP network address
      PIPSocket::Address mask       ///< Mask for IP network
    );


    /**Create a new PIpAccessControl specification entry object.
       This may be used by an application to create descendents of
       PIpAccessControlEntry when extra information/functionality is required.

       The default behaviour creates a PIpAccessControlEntry.
      */
    virtual PIpAccessControlEntry * CreateControlEntry(
      const PString & description
    );

    /**Find the PIpAccessControl specification for the address.
      */
    PIpAccessControlEntry * Find(
      PIPSocket::Address address    ///< IP Address to find
    ) const;

    /** Test the address/connection for if it is allowed within this access
       control list. If the <CODE>socket</CODE> form is used the peer address
       of the connection is tested.

       If the list is empty then TRUE is returned. If the list is not empty,
       but the IP address does not match any entries in the list, then FALSE
       is returned. If a match is made then the allow state of that entry is
       returned.

       @return
       TRUE if the remote host address is allowed.
     */
    BOOL IsAllowed(
      PTCPSocket & socket           ///< Socket to test
    ) const;
    BOOL IsAllowed(
      PIPSocket::Address address    ///< IP Address to test
    ) const;


    /**Get the default state for allowed access if the list is empty.
      */
    BOOL GetDefaultAllowance() const { return defaultAllowance; }

    /**Set the default state for allowed access if the list is empty.
      */
    void SetDefaultAllowance(BOOL defAllow) { defaultAllowance = defAllow; }

  private:
    BOOL InternalLoadHostsAccess(const PString & daemon, const char * file, BOOL allow);
    BOOL InternalRemoveEntry(PIpAccessControlEntry & entry);

  protected:
    BOOL defaultAllowance;
};


#endif  // _IPACL_H


// End of File ///////////////////////////////////////////////////////////////
