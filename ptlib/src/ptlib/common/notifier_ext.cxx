/*
 * notifier_ext.cxx
 *
 * Smart Notifiers and Notifier Lists
 *
 * Portable Windows Library
 *
 * Copyright (c) 2004 Reitek S.p.A.
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
 * $Log: notifier_ext.cxx,v $
 * Revision 1.2  2007/04/20 07:20:46  csoutheren
 * Applied 1703639 - PNotifierList::Move() leaks memory
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.1  2004/04/22 12:31:01  rjongbloed
 * Added PNotifier extensions and XMPP (Jabber) support,
 *   thanks to Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "notifier_ext.h"
#endif

#include <ptlib.h>
#include <ptlib/notifier_ext.h>

//////////////////////////////////////////////////////////////////////////////

class PPointer : public PObject
{
  PCLASSINFO(PPointer, PObject);
protected:
  void *  m_Pointer;
public:
  PPointer(void * pointer) : m_Pointer(pointer) { }
  void * GetPointer() const { return m_Pointer; }
};

PDICTIONARY(PNotifierBroker, POrdinalKey, PPointer);

static unsigned s_ID = 0;
static PNotifierBroker s_Broker;
static PMutex s_BrokerLock;


unsigned PSmartNotifieeRegistrar::RegisterNotifiee(void * obj)
{
  s_BrokerLock.Wait();
  unsigned id = ++s_ID;
  s_Broker.SetAt(POrdinalKey(id), new PPointer(obj));
  s_BrokerLock.Signal();
  return id;
}


BOOL PSmartNotifieeRegistrar::UnregisterNotifiee(unsigned id)
{
  PWaitAndSignal l(s_BrokerLock);
  if (s_Broker.Contains(id))
  {
    s_Broker.RemoveAt(id);
    return TRUE;
  }
  else
    return FALSE;
}


BOOL PSmartNotifieeRegistrar::UnregisterNotifiee(void * /*obj*/)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


void * PSmartNotifieeRegistrar::GetNotifiee(unsigned id)
{
  void * obj = 0;

  s_BrokerLock.Wait();
  if (s_Broker.Contains(id))
  {
    obj = s_Broker.GetAt(id)->GetPointer();
  }
  s_BrokerLock.Signal();

  return obj;
}

//////////////////////////////////////////////////////////////////////////////

class PSmartFuncInspector : public PNotifierFunction
{
  PCLASSINFO(PSmartFuncInspector, PNotifierFunction);

public:
  PSmartFuncInspector(void *obj) : PNotifierFunction(obj) { }

  void * GetTarget() const { return object; }
  virtual void Call(PObject &, INT) const {}
};


class PSmartPtrInspector : public PSmartPointer
{
  PCLASSINFO(PSmartPtrInspector, PSmartPointer);

public:
  PSmartPtrInspector(const PNotifier& ptr) : PSmartPointer(ptr) { }

  void * GetObject() const { return object; }
  void * GetTarget() const;
};

void * PSmartPtrInspector::GetTarget() const
{
  if (!object)
    return 0;

  PObject * ptr = (PObject *)object;

  if (PIsDescendant(ptr, PSmartNotifierFunction))
    return ((PSmartNotifierFunction *)ptr)->GetNotifiee();
  else
    return ((PSmartFuncInspector *)ptr)->GetTarget();
}

//////////////////////////////////////////////////////////////////////////////

BOOL PNotifierList::RemoveTarget(PObject * obj)
{
  Cleanup();

  for (PINDEX i = 0 ; i < m_TheList.GetSize() ; i++)
  {
    PSmartPtrInspector sptr(m_TheList[i]);

    void * target = sptr.GetTarget();

    if (target == obj)
    {
      m_TheList.RemoveAt(i);
      return TRUE;
    }
  }

  return FALSE;
}


void PNotifierList::Move(PNotifierList& that)
{
  Cleanup();
  that.Cleanup();

  that.m_TheList.DisallowDeleteObjects();

  while (that.m_TheList.GetSize())
    m_TheList.Append(that.m_TheList.RemoveAt(0));

  that.m_TheList.AllowDeleteObjects();
}


void PNotifierList::Cleanup()
{
  for (PINDEX i = 0 ; i < m_TheList.GetSize() ; i++)
  {
    PNotifier& n = m_TheList[i];
    PSmartPtrInspector sptr(n);
    PObject * ptr = (PObject *)sptr.GetObject();

    if (!ptr || (PIsDescendant(ptr, PSmartNotifierFunction) &&
      ((PSmartNotifierFunction *)ptr)->GetNotifiee() == 0))
    {
      PTRACE(2, "PNotifierList\tRemoving invalid notifier " << ((PSmartNotifierFunction *)ptr)->GetNotifieeID());
      m_TheList.RemoveAt(i);
      i--;
    }
  }
}


BOOL PNotifierList::Fire(PObject& obj, INT val)
{
  if (!m_TheList.GetSize()) return FALSE;

  for (PINDEX i = 0 ; i < m_TheList.GetSize() ; i++)
  {
    PNotifier& n = m_TheList[i];

#ifdef _DEBUG
    if (PTrace::CanTrace(6)) // Debug only
    {
      PSmartPtrInspector sptr(n);
      PObject * obj = (PObject *)sptr.GetTarget();

      if (obj)
      {
        PTRACE(6, "PNotifierList\tInvoking on " << obj->GetClass());
      }
    }
#endif

    n(obj, val);
  }

  return TRUE;
}

// End of File ///////////////////////////////////////////////////////////////

