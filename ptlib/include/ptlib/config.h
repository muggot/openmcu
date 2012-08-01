/*
 * config.h
 *
 * Application/System configuration access class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: config.h,v $
 * Revision 1.28  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.27  2005/01/26 05:37:44  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.26  2004/01/06 21:16:38  csoutheren
 * Added "virtual" keyword to methods to allow descendant classes.
 * Thanks to Cristian Bullokles for pointing out this issue
 *
 * Revision 1.25  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.24  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.23  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.22  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.21  2000/05/25 11:07:26  robertj
 * Added PConfig::HasKey() function to determine if value actually set.
 *
 * Revision 1.20  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.19  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.18  1998/10/30 12:23:55  robertj
 * Added ability to get all key values as a dictionary.
 *
 * Revision 1.17  1998/09/23 06:20:21  robertj
 * Added open source copyright license.
 *
 * Revision 1.16  1998/01/26 00:29:26  robertj
 * Added functions to get/set 64bit integers from a PConfig.
 *
 * Revision 1.15  1997/08/07 11:58:01  robertj
 * Added ability to get registry data from other applications and anywhere in system registry.
 *
 * Revision 1.14  1996/02/25 02:50:33  robertj
 * Added consts to all GetXxxx functions.
 *
 * Revision 1.13  1996/01/28 14:10:10  robertj
 * Added time functions to PConfig.
 *
 * Revision 1.12  1995/12/10 11:54:30  robertj
 * Added WIN32 registry support for PConfig objects.
 *
 * Revision 1.11  1995/03/14 12:41:12  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.10  1995/01/27  11:06:20  robertj
 * Changed single string default constructor to be section name not file name.
 *
 * Revision 1.9  1994/12/12  10:11:59  robertj
 * Documentation.
 *
 * Revision 1.8  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.7  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.6  1994/08/21  23:43:02  robertj
 * Removed default argument when of PString type (MSC crashes).
 *
 * Revision 1.5  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.4  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.3  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.2  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */


#ifndef _PCONFIG
#define _PCONFIG

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include "ptbuildopts.h"
#ifdef P_CONFIG_FILE

class PXConfig;

/** A class representing a configuration for the application.
There are four sources of configuration information. The system environment,
a system wide configuration file, an application specific configuration file
or an explicit configuration file.

Configuration information follows a three level hierarchy: {\it file},
{\it section} and {\it variable}. Thus, a configuration file consists of
a number of sections each with a number of variables selected by a
{\it key}. Each variable has an associated value.

Note that the evironment source for configuration information does not have
sections. The section is ignored and the same set of keys are available.

The configuration file is a standard text file for the platform with its
internals appearing in the form:
\begin{verbatim}
     [Section String]
     Key Name=Value String
\end{verbatim}
*/
class PConfig : public PObject
{
  PCLASSINFO(PConfig, PObject);

  public:
  /**@name Construction */
  //@{
    /** Description of the standard source for configuration information.
     */
    enum Source {
      /** The platform specific environment. For Unix, MSDOS, NT etc this is
         {\bf the} environment current when the program was run. For the
         MacOS this is a subset of the Gestalt and SysEnviron information.
       */
      Environment,
      /** The platform specific system wide configuration file. For MS-Windows
         this is the WIN.INI file. For Unix, plain MS-DOS, etc this is a
         configuration file similar to that for applications except there is
         only a single file that applies to all PWLib applications.
       */
      System,
      /** The application specific configuration file. This is the most common
         source of configuration for an application. The location of this file
         is platform dependent, but its contents are always the same. For
         MS-Windows the file should be either in the same directory as the
         executable or in the Windows directory. For the MacOS this would be
         either in the System Folder or the Preferences folder within it. For
         Unix this would be the users home directory.
       */
      Application,
      NumSources
    };

    /** Create a new configuration object. Once a source is selected for the
       configuration it cannot be changed. Only at the next level of the
       hierarchy (sections) are selection able to be made dynamically with an
       active PConfig object.
     */
    PConfig(
      Source src = Application  ///< Standard source for the configuration.
    );
    /** Create a new configuration object. */
    PConfig(
      Source src,               ///< Standard source for the configuration.
      const PString & appname   ///< Name of application
    );
    /** Create a new configuration object. */
    PConfig(
      Source src,               ///< Standard source for the configuration.
      const PString & appname,  ///< Name of application
      const PString & manuf     ///< Manufacturer
    );
    /** Create a new configuration object. */
    PConfig(
      const PString & section,  ///< Default section to search for variables.
      Source src = Application  ///< Standard source for the configuration.
    );
    /** Create a new configuration object. */
    PConfig(
      const PString & section,  ///< Default section to search for variables.
      Source src,               ///< Standard source for the configuration.
      const PString & appname   ///< Name of application
    );
    /** Create a new configuration object. */
    PConfig(
      const PString & section,  ///< Default section to search for variables.
      Source src,               ///< Standard source for the configuration.
      const PString & appname,  ///< Name of application
      const PString & manuf     ///< Manufacturer
    );
    /** Create a new configuration object. */
    PConfig(
      const PFilePath & filename, ///< Explicit name of the configuration file.
      const PString & section     ///< Default section to search for variables.
    );
  //@}

  /**@name Section functions */
  //@{
    /** Set the default section for variable operations. All functions that deal
       with keys and get or set configuration values will use this section
       unless an explicit section name is specified.

       Note when the #Environment# source is being used the default
       section may be set but it is ignored.
     */
    virtual void SetDefaultSection(
      const PString & section  ///< New default section name.
    );

    /** Get the default section for variable operations. All functions that deal
       with keys and get or set configuration values will use this section
       unless an explicit section name is specified.

       Note when the #Environment# source is being used the default
       section may be retrieved but it is ignored.

       @return default section name string.
     */
    virtual PString GetDefaultSection() const;

    /** Get all of the section names currently specified in the file. A section
       is the part specified by the [ and ] characters.

       Note when the #Environment# source is being used this will
       return an empty list as there are no section present.

       @return list of all section names.
     */
    virtual PStringList GetSections() const;

    /** Get a list of all the keys in the section. If the section name is not
       specified then use the default section.

       @return list of all key names.
     */
    virtual PStringList GetKeys() const;
    /** Get a list of all the keys in the section. */
    virtual PStringList GetKeys(
      const PString & section   ///< Section to use instead of the default.
    ) const;

    /** Get all of the keys in the section and their values. If the section
       name is not specified then use the default section.

       @return Dictionary of all key names and their values.
     */
    virtual PStringToString GetAllKeyValues() const;
    /** Get all of the keys in the section and their values. */
    virtual PStringToString GetAllKeyValues(
      const PString & section   ///< Section to use instead of the default.
    ) const;


    /** Delete all variables in the specified section. If the section name is
       not specified then the default section is deleted.

       Note that the section header is also removed so the section will not
       appear in the GetSections() function.
     */
    virtual void DeleteSection();
    /** Delete all variables in the specified section. */
    virtual void DeleteSection(
      const PString & section   ///< Name of section to delete.
    );

    /** Delete the particular variable in the specified section. If the section
       name is not specified then the default section is used.

       Note that the variable and key are removed from the file. The key will
       no longer appear in the GetKeys() function. If you wish to delete the
       value without deleting the key, use SetString() to set it to the empty
       string.
     */
    virtual void DeleteKey(
      const PString & key       ///< Key of the variable to delete.
    );
    /** Delete the particular variable in the specified section. */
    virtual void DeleteKey(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key       ///< Key of the variable to delete.
    );

    /**Determine if the particular variable in the section is actually present.

       This function allows a caller to distinguish between getting a saved
       value or using the default value. For example if you called
       GetString("MyKey", "DefVal") there is no way to distinguish between
       the default "DefVal" being used, or the user had explicitly saved the
       value "DefVal" into the PConfig.
     */
    virtual BOOL HasKey(
      const PString & key       ///< Key of the variable.
    ) const;
    /**Determine if the particular variable in the section is actually present. */
    virtual BOOL HasKey(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key       ///< Key of the variable.
    ) const;
  //@}

  /**@name Get/Set variables */
  //@{
    /** Get a string variable determined by the key in the section. If the
       section name is not specified then the default section is used.
       
       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case an empty string is
       returned.

       @return string value of the variable.
     */
    virtual PString GetString(
      const PString & key       ///< The key name for the variable.
    ) const;
    /** Get a string variable determined by the key in the section. */
    virtual PString GetString(
      const PString & key,      ///< The key name for the variable.
      const PString & dflt      ///< Default value for the variable.
    ) const;
    /** Get a string variable determined by the key in the section. */
    virtual PString GetString(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      const PString & dflt      ///< Default value for the variable.
    ) const;

    /** Set a string variable determined by the key in the section. If the
       section name is not specified then the default section is used.
     */
    virtual void SetString(
      const PString & key,      ///< The key name for the variable.
      const PString & value     ///< New value to set for the variable.
    );
    /** Set a string variable determined by the key in the section. */
    virtual void SetString(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      const PString & value     ///< New value to set for the variable.
    );


    /** Get a boolean variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       The boolean value can be specified in a number of ways. The TRUE value
       is returned if the string value for the variable begins with either the
       'T' character or the 'Y' character. Alternatively if the string can
       be converted to a numeric value, a non-zero value will also return TRUE.
       Thus the values can be Key=True, Key=Yes or Key=1 for TRUE and
       Key=False, Key=No, or Key=0 for FALSE.

       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case FALSE is returned.

       @return boolean value of the variable.
     */
    virtual BOOL GetBoolean(
      const PString & key,      ///< The key name for the variable.
      BOOL dflt = FALSE         ///< Default value for the variable.
    ) const;
    /** Get a boolean variable determined by the key in the section. */
    virtual BOOL GetBoolean(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      BOOL dflt = FALSE         ///< Default value for the variable.
    ) const;

    /** Set a boolean variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       If value is TRUE then the string "True" is written to the variable
       otherwise the string "False" is set.
     */
    virtual void SetBoolean(
      const PString & key,      ///< The key name for the variable.
      BOOL value                ///< New value to set for the variable.
    );
    /** Set a boolean variable determined by the key in the section. */
    virtual void SetBoolean(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      BOOL value                ///< New value to set for the variable.
    );


    /* Get an integer variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case zero is returned.

       @return integer value of the variable.
     */
    virtual long GetInteger(
      const PString & key,      ///< The key name for the variable.
      long dflt = 0             ///< Default value for the variable.
    ) const;
    /* Get an integer variable determined by the key in the section. */
    virtual long GetInteger(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      long dflt = 0             ///< Default value for the variable.
    ) const;

    /** Set an integer variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       The value is always formatted as a signed number with no leading or
       trailing blanks.
     */
    virtual void SetInteger(
      const PString & key,      ///< The key name for the variable.
      long value                ///< New value to set for the variable.
    );
    /** Set an integer variable determined by the key in the section. */
    virtual void SetInteger(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      long value                ///< New value to set for the variable.
    );


    /** Get a 64 bit integer variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case zero is returned.

       @return integer value of the variable.
     */
    virtual PInt64 GetInt64(
      const PString & key,      ///< The key name for the variable.
      PInt64 dflt = 0           ///< Default value for the variable.
    ) const;
    /** Get a 64 bit integer variable determined by the key in the section. */
    virtual PInt64 GetInt64(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      PInt64 dflt = 0           ///< Default value for the variable.
    ) const;

    /** Set a 64 bit integer variable determined by the key in the section. If the
       section name is not specified then the default section is used.

       The value is always formatted as a signed number with no leading or
       trailing blanks.
     */
    virtual void SetInt64(
      const PString & key,      ///< The key name for the variable.
      PInt64 value              ///< New value to set for the variable.
    );
    /** Set a 64 bit integer variable determined by the key in the section. */
    virtual void SetInt64(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      PInt64 value              ///< New value to set for the variable.
    );


    /** Get a floating point variable determined by the key in the section. If
       the section name is not specified then the default section is used.

       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case zero is returned.

       @return floating point value of the variable.
     */
    virtual double GetReal(
      const PString & key,      ///< The key name for the variable.
      double dflt = 0           ///< Default value for the variable.
    ) const;
    /** Get a floating point variable determined by the key in the section. */
    virtual double GetReal(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      double dflt = 0           ///< Default value for the variable.
    ) const;

    /** Set a floating point variable determined by the key in the section. If
       the section name is not specified then the default section is used.

       The value is always formatted as a signed decimal or exponential form
       number with no leading or trailing blanks, ie it uses the %g formatter
       from the printf() function.
     */
    virtual void SetReal(
      const PString & key,      ///< The key name for the variable.
      double value              ///< New value to set for the variable.
    );
    /** Set a floating point variable determined by the key in the section. */
    virtual void SetReal(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      double value              ///< New value to set for the variable.
    );

    /** Get a #PTime# variable determined by the key in the section. If
       the section name is not specified then the default section is used.

       If the key is not present the value returned is the that provided by
       the #dlft# parameter. Note that this is different from the
       key being present but having no value, in which case zero is returned.

       @return time/date value of the variable.
     */
    virtual PTime GetTime(
      const PString & key       ///< The key name for the variable.
    ) const;
    /** Get a #PTime# variable determined by the key in the section. */
    virtual PTime GetTime(
      const PString & key,      ///< The key name for the variable.
      const PTime & dflt        ///< Default value for the variable.
    ) const;
    /** Get a #PTime# variable determined by the key in the section. */
    virtual PTime GetTime(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key       ///< The key name for the variable.
    ) const;
    /** Get a #PTime# variable determined by the key in the section. */
    virtual PTime GetTime(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      const PTime & dflt        ///< Default value for the variable.
    ) const;

    /** Set a #PTime# variable determined by the key in the section. If
       the section name is not specified then the default section is used.
     */
    virtual void SetTime(
      const PString & key,      ///< The key name for the variable.
      const PTime & value       ///< New value to set for the variable.
    );
    /** Set a #PTime# variable determined by the key in the section. */
    virtual void SetTime(
      const PString & section,  ///< Section to use instead of the default.
      const PString & key,      ///< The key name for the variable.
      const PTime & value       ///< New value to set for the variable.
    );
  //@}


  protected:
    // Member variables
    /// The current section for variable values.
    PString defaultSection;


  private:
    // Do common construction code.
    void Construct(
      Source src,               ///< Standard source for the configuration.
      const PString & appname,  ///< Name of application
      const PString & manuf     ///< Manufacturer
    );
    void Construct(
      const PFilePath & filename  ///< Explicit name of the configuration file.
    );


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/config.h"
#else
#include "unix/ptlib/config.h"
#endif
};

#endif // P_CONFIG_FILE

#endif

// End Of File ///////////////////////////////////////////////////////////////
