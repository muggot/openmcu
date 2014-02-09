/*
 * pt_atl.h
 *
 * File needed to fake some Win32 ATL stuff.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2001 Equivalence Pty. Ltd.
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
 * Contributor(s):
 *
 * $Revision: 26307 $
 * $Author: ededu $
 * $Date: 2011-08-14 17:32:03 -0500 (Sun, 14 Aug 2011) $
 */

#ifndef PTLIB_ATL_H
#define PTLIB_ATL_H

#ifdef P_ATL
  #pragma warning(disable:4127)
  #include <cguid.h>
  #include <atlcomcli.h>
  #pragma warning(default:4127)
#else

// We are using express edition of MSVC which does not come with ATL support
// So hand implement just enough for some code (e.g. SAPI) to work.
#define __ATLBASE_H__

#include <objbase.h>

typedef WCHAR OLECHAR;
typedef OLECHAR *LPOLESTR;
typedef const OLECHAR *LPCOLESTR;
typedef struct IUnknown IUnknown;
typedef IUnknown *LPUNKNOWN;

template <class T> class CComPtr
{
    T * m_pointer;
  public:
    CComPtr()                    : m_pointer(NULL) { }
    CComPtr(T * ptr)             : m_pointer(NULL) { Attach(ptr); }
    CComPtr(const CComPtr & ptr) : m_pointer(NULL) { Attach(ptr.m_pointer); }
    ~CComPtr() { Release(); }

    operator T *()          const { return  m_pointer; }
    T & operator*()         const { return *m_pointer; }
    T* operator->()         const { return  m_pointer; }
    T** operator&()               { return &m_pointer; }
    bool operator!()        const { return  m_pointer == NULL; }
    bool operator<(T* ptr)  const { return  m_pointer <  ptr; }
    bool operator==(T* ptr) const { return  m_pointer == ptr; }
    bool operator!=(T* ptr) const { return  m_pointer != ptr; }

    CComPtr & operator=(T * ptr)
    {
      Attach(ptr);
      return *this;
    }
    CComPtr & operator=(const CComPtr & ptr)
    {
      if (&ptr != this)
        Attach(ptr.m_pointer);
      return *this;
    }

    void Attach(T * ptr)
    {
      Release();
      if (ptr != NULL)
        ptr->AddRef();
      m_pointer = ptr;
    }

    T * Detach()
    {
      T * ptr = m_pointer;
      m_pointer = NULL;
      return ptr;
    }

    void Release()
    {
      T * ptr = m_pointer;
      if (ptr != NULL) {
        m_pointer = NULL;
        ptr->Release();
      }
    }

// mingw32 does not know __ parameters (yet)
#ifndef __MINGW32__
    __checkReturn HRESULT CoCreateInstance(
      __in     REFCLSID  rclsid,
      __in_opt LPUNKNOWN pUnkOuter    = NULL,
      __in     DWORD     dwClsContext = CLSCTX_ALL,
      __in     REFIID    riid         = __uuidof(T))
    {
      return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, (void**)&m_pointer);
    }
#endif
};

#endif // P_ATL

#endif //PTLIB_ATL_H
