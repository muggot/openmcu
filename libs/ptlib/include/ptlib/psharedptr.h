/*
 * psharedptr.h
 *
 * SharedPtr template
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psharedptr.h,v $
 * Revision 1.3  2004/10/21 09:20:33  csoutheren
 * Fixed compile problems on gcc 2.95.x
 *
 * Revision 1.2  2004/10/01 08:08:50  csoutheren
 * Added Reset and auto_ptr conversions
 *
 * Revision 1.1  2004/10/01 07:17:18  csoutheren
 * Added PSharedptr class
 *
 */

#ifndef _PSHAREDPTR_H
#define _PSHAREDPTR_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
#include <memory>

/**
 *
 * These templates implement an pointner class with an integral reference count
 * based on the PContainer base class. This allows the easy creation of an
 * a reference counted ptr that will autodestruct when the last reference 
 * goes out of scope.
 */

template <class T>
class PSharedPtr : public PContainer
{
  PCLASSINFO(PSharedPtr, PContainer);
  public:
    typedef T element_type;

    PSharedPtr(element_type * _ptr = NULL)
    { ptr = _ptr; }

    PSharedPtr(const PSharedPtr & c)
      : PContainer(c)
    { CopyContents(c); } 

    PSharedPtr(std::auto_ptr<element_type> & v)
    { ptr = v.release(); }

    PSharedPtr & operator=(const PSharedPtr & c) 
    { AssignContents(c); return *this; } 

    virtual ~PSharedPtr()
    { Destruct(); } 

    virtual BOOL MakeUnique() 
    { if (PContainer::MakeUnique()) return TRUE; CloneContents(this); return FALSE; } 

    BOOL SetSize(PINDEX)
    { return false; }

    T * Get() const
    { return ptr; }

    void Reset() const
    { AssignContents(PSharedPtr()); }

    T & operator*() const
    { return *ptr; }

    T * operator->() const
    { return ptr; }


  protected: 
    PSharedPtr(int dummy, const PSharedPtr * c)
    : PContainer(dummy, c)
    { CloneContents(c); } 

    void AssignContents(const PContainer & c) 
    { PContainer::AssignContents(c); CopyContents((const PSharedPtr &)c); }

    void DestroyContents()
    { delete(ptr); }

    void CloneContents(const PContainer * src)
    { ptr = new element_type(*((const PSharedPtr *)src)->ptr); }

    void CopyContents(const PContainer & c)
    { ptr = ((const PSharedPtr &)c).ptr; }

  protected:
    T * ptr;
};

#endif // _PSHAREDPTR_H

