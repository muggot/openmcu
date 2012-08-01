/*
 * dynalink.h
 *
 * Dynamic Link Library abstraction class.
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
 * $Log: dynalink.h,v $
 * Revision 1.18  2007/02/12 23:19:55  csoutheren
 * Applied 1636264 - dynalink.h doc update
 * Thanks to RM Fleming
 *
 * Revision 1.17  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.16  2004/07/11 07:56:35  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.15  2004/02/22 02:38:33  ykiryanov
 * Removed ifndef BEOS when declaring PDynalink
 *
 * Revision 1.14  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.13  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.12  2003/05/14 00:42:32  rjongbloed
 * Added missing virtual keyword to PDynaLink functions.
 *
 * Revision 1.11  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.10  2002/10/10 04:43:43  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.9  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2001/06/30 06:59:06  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.7  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.6  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.5  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.4  1998/09/23 06:20:29  robertj
 * Added open source copyright license.
 *
 * Revision 1.3  1997/06/16 13:15:52  robertj
 * Added function to get a dyna-link libraries name.
 *
 * Revision 1.2  1997/06/08 04:49:20  robertj
 * Added DLL file extension string function.
 *
 * Revision 1.1  1995/03/14 12:44:08  robertj
 * Initial revision
 *
 */

#ifndef _PDYNALINK
#define _PDYNALINK

#if !defined(P_RTEMS)

#ifdef P_USE_PRAGMA
#pragma interface
#endif

/**A dynamic link library. This allows the loading at run time of code
   modules for use by an application.
   MacOS X/darwin supports plugins linked as object file image (linked with the -bundle arg to ld) or
   dynamic libraries (-dynamic).
   On all Unix platforms the file name should end in ".so". 
   On Windows the filename should end in ".dll"
*/

class PDynaLink : public PObject
{
  PCLASSINFO(PDynaLink, PObject);

  public:
  /**@name Construction */
  //@{
    /**Create a new dyna-link, loading the specified module. The first,
       parameterless, form does load a library.
     */
    PDynaLink();
    /**Create a new dyna-link, loading the specified module. The first,
       parameterless, form does load a library.
     */
    PDynaLink(
      const PString & name    ///< Name of the dynamically loadable module.
    );

    /**Destroy the dyna-link, freeing the module.
     */
    ~PDynaLink();
  //@}

  /**@name Load/Unload function */
  //@{
    /* Open a new dyna-link, loading the specified module.

       @return
       TRUE if the library was loaded.
     */
    virtual BOOL Open(
      const PString & name    ///< Name of the dynamically loadable module.
    );

    /**Close the dyna-link library.
     */
    virtual void Close();

    /**Dyna-link module is loaded and may be accessed.
     */
    virtual BOOL IsLoaded() const;

    /**Get the name of the loaded library. If the library is not loaded
       this may return an empty string.

       If #full# is TRUE then the full pathname of the library
       is returned otherwise only the name part is returned.

       @return
       String for the library name.
     */
    virtual PString GetName(
      BOOL full = FALSE  ///< Flag for full or short path name
    ) const;

    /**Get the extension used by this platform for dynamic link libraries.

       @return
       String for file extension.
     */
    static PString GetExtension();
  //@}

  /**@name DLL entry point functions */
  //@{
    /// Primitive pointer to a function for a dynamic link module.
    typedef void (*Function)();


    /**Get a pointer to the function in the dynamically loadable module.

       @return
       TRUE if function was found.
     */
    BOOL GetFunction(
      PINDEX index,    ///< Ordinal number of the function to get.
      Function & func  ///< Refrence to point to function to get.
    );

    /**Get a pointer to the function in the dynamically loadable module.

       @return
       TRUE if function was found.
     */
    BOOL GetFunction(
      const PString & name,  ///< Name of the function to get.
      Function & func        ///< Refrence to point to function to get.
    );
  //@}


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/dynalink.h"
#else
#include "unix/ptlib/dynalink.h"
#endif
};

#endif // !defined(P_RTEMS)

#endif

// End Of File ///////////////////////////////////////////////////////////////
