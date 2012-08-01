/*
 * args.h
 *
 * Program Argument Parsing class
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
 * $Log: args.h,v $
 * Revision 1.28  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.27  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.26  2005/11/20 22:02:59  dereksmithies
 * Fix tags so they work with doxygen.
 *
 * Revision 1.25  2005/01/26 05:37:43  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.24  2003/09/17 01:18:01  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.23  2003/03/27 07:27:07  robertj
 * Added function to get a bunch of arguments as a string array.
 *
 * Revision 1.22  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.21  2001/12/15 04:49:06  robertj
 * Added stream I/O functions for argument list.
 *
 * Revision 1.20  2000/05/25 11:05:31  robertj
 * Added PConfigArgs class so can save program arguments to config files.
 *
 * Revision 1.19  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.18  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.17  1998/11/01 04:56:51  robertj
 * Added BOOl return value to Parse() to indicate there are parameters available.
 *
 * Revision 1.16  1998/10/30 11:22:14  robertj
 * Added constructors that take strings as well as const char *'s.
 *
 * Revision 1.15  1998/10/30 05:24:29  robertj
 * Added return value to << and >> operators for shifting arguments.
 *
 * Revision 1.14  1998/10/29 05:35:14  robertj
 * Fixed porblem with GetCount() == 0 if do not call Parse() function.
 *
 * Revision 1.13  1998/10/28 03:26:41  robertj
 * Added multi character arguments (-abc style) and options precede parameters mode.
 *
 * Revision 1.12  1998/10/28 00:59:46  robertj
 * New improved argument parsing.
 *
 * Revision 1.11  1998/09/23 06:20:14  robertj
 * Added open source copyright license.
 *
 * Revision 1.10  1995/12/10 11:26:38  robertj
 * Fixed signed/unsigned bug in shift count.
 *
 * Revision 1.9  1995/06/17 11:12:17  robertj
 * Documentation update.
 *
 * Revision 1.8  1995/03/14 12:40:58  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.7  1994/12/05  11:15:13  robertj
 * Documentation.
 *
 * Revision 1.6  1994/11/26  03:44:19  robertj
 * Documentation.
 *
 * Revision 1.6  1994/11/24  11:48:26  robertj
 * Documentation.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.2  1994/07/17  10:46:06  robertj
 * Changed to use container classes to plug memory leak.
 *
 * Revision 1.1  1994/04/01  14:08:52  robertj
 * Initial revision
 *
 */

#ifndef _PARGLIST
#define _PARGLIST

#ifdef P_USE_PRAGMA
#pragma interface
#endif

/** This class allows the parsing of a set of program arguments. This translates
   the standard argc/argv style variables passed into the main() function into
   a set of options (preceded by a '-' character) and parameters.
*/
class PArgList : public PObject
{
  PCLASSINFO(PArgList, PObject);

  public:
  /**@name Construction */
  //@{
    /** Create an argument list.
        An argument list is created given the standard arguments and a
       specification for options. The program arguments are parsed from this
       into options and parameters.

       The specification string consists of case significant letters for each
       option. If the letter is followed by the ':' character then the option
       has an associated string. This string must be in the argument or in the
       next argument.
     */
    PArgList(
      const char * theArgPtr = NULL,        ///< A string constituting the arguments 
      const char * argumentSpecPtr = NULL,  ///< The specification C string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE       ///< Parse options only before parameters 
    );
    /** Create an argument list. */
    PArgList(
      const PString & theArgStr,             ///< A string constituting the arguments 
      const char * argumentSpecPtr = NULL,   ///< The specification C string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE        ///< Parse options only before parameters 
    );
    /** Create an argument list. */
    PArgList(
      const PString & theArgStr,             ///< A string constituting the arguments 
      const PString & argumentSpecStr,       ///< The specification string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE        ///< Parse options only before parameters 
    );
    /** Create an argument list. */
    PArgList(
      int theArgc,                           ///< Count of argument strings in theArgv 
      char ** theArgv,                       ///< An array of strings constituting the arguments 
      const char * argumentSpecPtr = NULL,   ///< The specification C string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE        ///< Parse options only before parameters 
    );
    /** Create an argument list. */
    PArgList(
      int theArgc,                           ///< Count of argument strings in theArgv 
      char ** theArgv,                       ///< An array of strings constituting the arguments 
      const PString & argumentSpecStr,       ///< The specification string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE        ///< Parse options only before parameters 
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Output the string to the specified stream.
     */
    virtual void PrintOn(
      ostream & strm  ///< I/O stream to output to.
    ) const;

    /**Input the string from the specified stream. This will read all
       characters until a end of line is reached, then parsing the arguments.
     */
    virtual void ReadFrom(
      istream & strm  ///< I/O stream to input from. 
    );
  //@}

  /**@name Setting & Parsing */
  //@{
    /** Set the internal copy of the program arguments.
    */
    void SetArgs(
      const PString & theArgStr ///< A string constituting the arguments 
    );
    /** Set the internal copy of the program arguments. */
    void SetArgs(
      int theArgc,     ///< Count of argument strings in theArgv 
      char ** theArgv  ///< An array of strings constituting the arguments 
    );
    /** Set the internal copy of the program arguments. */
    void SetArgs(
      const PStringArray & theArgs ///< A string array constituting the arguments
    );

    /** Parse the arguments.
       Parse the standard C program arguments into an argument of options and
       parameters. Consecutive calls with #optionsBeforeParams# set
       to TRUE will parse out different options and parameters. If SetArgs()
       function is called then the Parse() function will restart from the
       beginning of the argument list.

       The specification string consists of case significant letters for each
       option. If the letter is followed by a '-' character then a long name
       version of the option is present. This is terminated either by a '.' or
       a ':' character. If the single letter or long name is followed by the
       ':' character then the option has may have an associated string. This
       string must be within the argument or in the next argument. If a single
       letter option is followed by a ';' character, then the option may have
       an associated string but this MUST follow the letter immediately, if
       it is present at all.

       For example, "ab:c" allows for "-a -b arg -barg -c" and
       "a-an-arg.b-option:c;" allows for "-a --an-arg --option arg -c -copt".

       @return TRUE if there is at least one parameter after parsing.
     */
    virtual BOOL Parse(
      const char * theArgumentSpec,    ///< The specification string for argument options. See description for details.
      BOOL optionsBeforeParams = TRUE  ///< Parse options only before parameters
    );
    /** Parse the arguments. */
    virtual BOOL Parse(
      const PString & theArgumentStr,  ///< The specification string for argument options. See description for details.       
      BOOL optionsBeforeParams = TRUE  ///< Parse options only before parameters
    );
  //@}

  /**@name Getting parsed arguments */
  //@{
    /** Get the count of the number of times the option was specified on the
       command line.

       @return option repeat count.
     */
    virtual PINDEX GetOptionCount(
      char optionChar        ///< Character letter code for the option 
    ) const;
    /** Get the count of option */
    virtual PINDEX GetOptionCount(
      const char * optionStr ///< String code for the option 
    ) const;
    /** Get the count of option */
    virtual PINDEX GetOptionCount(
      const PString & optionName ///< String code for the option 
    ) const;

    /** Get if option present.
      Determines whether the option was specified on the command line.

       @return TRUE if the option was present.
     */
    BOOL HasOption(
      char optionChar             ///< Character letter code for the option 
    ) const;
    /** Get if option present. */
    BOOL HasOption(
      const char * optionStr     ///< String letter code for the option 
    ) const;
    /** Get if option present. */
    BOOL HasOption(
      const PString & optionName ///<  String code for the option 
    ) const;

    /** Get option string.
       Gets the string associated with an option e.g. -ofile or -o file
       would return the string "file". An option may have an associated string
       if it had a ':' character folowing it in the specification string passed
       to the Parse() function.

       @return the options associated string.
     */
    virtual PString GetOptionString(
      char optionChar,          ///< Character letter code for the option 
      const char * dflt = NULL  ///< Default value of the option string 
    ) const;
    /** Get option string. */
    virtual PString GetOptionString(
      const char * optionStr,   ///< String letter code for the option 
      const char * dflt = NULL  ///<Default value of the option string 
    ) const;
    /** Get option string. */
    virtual PString GetOptionString(
      const PString & optionName, ///< String code for the option 
      const char * dflt = NULL    ///< Default value of the option string 
    ) const;

    /** Get the argument count.
       Get the number of parameters that may be obtained via the
       #GetParameter()# function. Note that this does not include options
       and option strings.

       @return count of parameters.
     */
    PINDEX GetCount() const;

    /** Get the parameters that were parsed in the argument list.

       @return array of parameter strings at the specified index range.
     */
    PStringArray GetParameters(
      PINDEX first = 0,
      PINDEX last = P_MAX_INDEX
    ) const;

    /** Get the parameter that was parsed in the argument list.

       @return parameter string at the specified index.
     */
    PString GetParameter(
      PINDEX num   ///< Number of the parameter to retrieve. 
    ) const;

    /** Get the parameter that was parsed in the argument list. The argument
       list object can thus be treated as an "array" of parameters.

       @return parameter string at the specified index.
     */
    PString operator[](
      PINDEX num   ///< Number of the parameter to retrieve. 
    ) const;

    /** Shift the parameters by the specified amount. This allows the parameters
       to be parsed at the same position in the argument list "array".
     */
    void Shift(
      int sh ///< Number of parameters to shift forward through list 
    );

    /** Shift the parameters by the specified amount. This allows the parameters
       to be parsed at the same position in the argument list "array".
     */
    PArgList & operator<<(
      int sh ///< Number of parameters to shift forward through list 
    );

    /** Shift the parameters by the specified amount. This allows the parameters
       to be parsed at the same position in the argument list "array".
     */
    PArgList & operator>>(
      int sh ///< Number of parameters to shift backward through list 
    );
  //@}

  /**@name Errors */
  //@{
    /** This function is called when access to illegal parameter index is made
       in the GetParameter function. The default behaviour is to output a
       message to the standard #PError# stream.
     */
    virtual void IllegalArgumentIndex(
      PINDEX idx ///< Number of the parameter that was accessed. 
    ) const;

    /** This function is called when an unknown option was specified on the
       command line. The default behaviour is to output a message to the
       standard #PError# stream.
     */
    virtual void UnknownOption(
      const PString & option   ///< Option that was illegally placed on command line. 
    ) const;

    /** This function is called when an option that requires an associated
       string was specified on the command line but no associated string was
       provided. The default behaviour is to output a message to the standard
       #PError# stream.
     */
    virtual void MissingArgument(
      const PString & option  ///< Option for which the associated string was missing. 
    ) const;
  //@}

  protected:
    /// The original program arguments.
    PStringArray argumentArray;
    /// The specification letters for options
    PString      optionLetters;
    /// The specification strings for options
    PStringArray optionNames;
    /// The count of the number of times an option appeared in the command line.
    PIntArray    optionCount;
    /// The array of associated strings to options.
    PStringArray optionString;
    /// The index of each .
    PIntArray    parameterIndex;
    /// Shift count for the parameters in the argument list.
    int          shift;

  private:
    BOOL ParseOption(PINDEX idx, PINDEX offset, PINDEX & arg, const PIntArray & canHaveOptionString);
    PINDEX GetOptionCountByIndex(PINDEX idx) const;
    PString GetOptionStringByIndex(PINDEX idx, const char * dflt) const;
};


#ifdef P_CONFIG_FILE

/**This class parse command line arguments with the ability to override them
   from a PConfig file/registry.
  */
class PConfigArgs : public PArgList
{
    PCLASSINFO(PConfigArgs, PArgList);
  public:
  /**@name Construction */
  //@{
    PConfigArgs(
      const PArgList & args   ///< Raw argument list.
    );
  //@}

  /**@name Overrides from class PArgList */
  //@{
    /** Get the count of the number of times the option was specified on the
       command line.

       @return option repeat count.
     */
    virtual PINDEX GetOptionCount(
      char optionChar  ///< Character letter code for the option
    ) const;
    /** Get the count of option */
    virtual PINDEX GetOptionCount(
      const char * optionStr ///< String code for the option
    ) const;
    /** Get the count of option */
    virtual PINDEX GetOptionCount(
      const PString & optionName ///< String code for the option
    ) const;

    /** Get option string.
       Gets the string associated with an option e.g. -ofile or -o file
       would return the string "file". An option may have an associated string
       if it had a ':' character folowing it in the specification string passed
       to the Parse() function.

       @return the options associated string.
     */
    virtual PString GetOptionString(
      char optionChar,          ///< Character letter code for the option 
      const char * dflt = NULL  ///< Default value of the option string 
    ) const;

    /** Get option string. */
    virtual PString GetOptionString(
      const char * optionStr,   ///< String letter code for the option 
      const char * dflt = NULL  ///< Default value of the option string 
    ) const;

    /** Get option string. */
    virtual PString GetOptionString(
      const PString & optionName, ///< String code for the option 
      const char * dflt = NULL    ///< Default value of the option string 
    ) const;
  //@}

  /**@name Overrides from class PArgList */
  //@{
    /**Save the current options to the PConfig.
       This function will check to see if the option name is present and if
       so, save to the PConfig all of the arguments present in the currently
       parsed list. Note that the optionName for saving is not saved to the
       PConfig itself as this would cause the data to be saved always!
      */
    void Save(
      const PString & optionName   ///< Option name for saving.
    );

    /**Set the PConfig section name for options.
      */
    void SetSectionName(
      const PString & section ///< New section name 
    ) { sectionName = section; }

    /**Get the PConfig section name for options.
      */
    const PString & GetSectionName() const { return sectionName; }

    /**Set the prefix for option negation.
       The default is "no-".
      */
    void SetNegationPrefix(
      const PString & prefix ///< New prefix string 
    ) { negationPrefix = prefix; }

    /**Get the prefix for option negation.
       The default is "no-".
      */
    const PString & GetNegationPrefix() const { return negationPrefix; }
  //@}


  protected:
    PString CharToString(char ch) const;
    PConfig config;
    PString sectionName;
    PString negationPrefix;
};

#endif // P_CONFIG_FILE

#endif

// End Of File ///////////////////////////////////////////////////////////////
