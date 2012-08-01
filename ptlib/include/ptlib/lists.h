/*
 * lists.h
 *
 * List Container Classes
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
 * $Log: lists.h,v $
 * Revision 1.33  2007/08/01 05:20:48  rjongbloed
 * Changes to container classes to become compatible with advanced DevStudio 2005 "Visualizers".
 *
 * Revision 1.32  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.31  2005/01/25 06:35:27  csoutheren
 * Removed warnings under MSVC
 *
 * Revision 1.30  2005/01/09 06:35:03  rjongbloed
 * Fixed ability to make Clone() or MakeUnique() of a sorted list.
 *
 * Revision 1.29  2004/04/09 03:42:34  csoutheren
 * Removed all usages of "virtual inline" and "inline virtual"
 *
 * Revision 1.28  2004/04/04 07:39:57  csoutheren
 * Fixed cut-and-paste typo in VS.net 2003 changes that made all PLists sorted. Yikes!
 *
 * Revision 1.27  2004/04/03 23:53:09  csoutheren
 * Added various changes to improce compatibility with the Sun Forte compiler
 *   Thanks to Brian Cameron
 * Added detection of readdir_r version
 *
 * Revision 1.26  2004/04/03 06:54:21  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.25  2004/02/15 03:04:52  rjongbloed
 * Fixed problem with PSortedList nil variable and assignment between instances,
 *   pointed out by Ben Lear.
 *
 * Revision 1.24  2004/02/09 06:23:32  csoutheren
 * Added fix for gcc 3.3.1 problem. Apparently, it is unable to correctly resolve
 * a function argument that is a reference to a const pointer. Changing the argument
 * to be a pointer to a pointer solves the problem. Go figure
 *
 * Revision 1.23  2004/02/08 11:13:10  rjongbloed
 * Fixed crash in heavily loaded multi-threaded systems using simultaneous sorted
 *   lists, Thanks Federico Pinna, Fabrizio Ammollo and the gang at Reitek S.p.A.
 *
 * Revision 1.22  2003/08/31 22:11:29  dereksmithies
 * Fix from Diego Tartara for the SetAt function. Many thanks.
 *
 * Revision 1.21  2002/11/12 08:55:53  robertj
 * Changed scope of PAbstraSortedList::Element class so descendant classes
 *   can get at it.
 *
 * Revision 1.20  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.19  2000/04/14 07:19:32  craigs
 * Fixed problem with assert when dequeueing from an empty queue
 *
 * Revision 1.18  1999/08/22 12:13:43  robertj
 * Fixed warning when using inlines on older GNU compiler
 *
 * Revision 1.17  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.16  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.15  1998/09/23 06:20:49  robertj
 * Added open source copyright license.
 *
 * Revision 1.14  1997/06/08 04:49:12  robertj
 * Fixed non-template class descendent order.
 *
 * Revision 1.13  1997/04/27 05:50:10  robertj
 * DLL support.
 *
 * Revision 1.12  1997/02/14 13:53:59  robertj
 * Major rewrite of sorted list to use sentinel record instead of NULL pointers.
 *
 * Revision 1.11  1996/07/15 10:32:50  robertj
 * Fixed bug in sorted list (crash on remove).
 *
 * Revision 1.10  1996/05/26 03:25:13  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.9  1996/01/23 13:13:32  robertj
 * Fixed bug in sorted list GetObjectsIndex not checking if is same object
 *
 * Revision 1.8  1995/08/24 12:35:00  robertj
 * Added assert for list index out of bounds.
 *
 * Revision 1.7  1995/06/17 11:12:43  robertj
 * Documentation update.
 *
 * Revision 1.6  1995/03/14 12:41:41  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.5  1995/02/22  10:50:30  robertj
 * Changes required for compiling release (optimised) version.
 *
 * Revision 1.4  1995/02/05  00:48:05  robertj
 * Fixed template version.
 *
 * Revision 1.3  1995/01/15  04:49:23  robertj
 * Fixed errors in template version.
 *
 * Revision 1.2  1994/12/21  11:53:12  robertj
 * Documentation and variable normalisation.
 *
 * Revision 1.1  1994/12/12  09:59:35  robertj
 * Initial revision
 *
 */

#ifdef P_USE_PRAGMA
#pragma interface
#endif


///////////////////////////////////////////////////////////////////////////////
// PList container class

struct PListElement
{
    PListElement(PObject * theData);
    PListElement * prev;
    PListElement * next;
    PObject * data;
};

struct PListInfo
{
    PListInfo() { head = tail = lastElement = NULL; }
    PListElement * head;
    PListElement * tail;
    PListElement * lastElement;
    PINDEX    lastIndex;
};

/**This class is a collection of objects which are descendents of the
   #PObject# class. It is implemeted as a doubly linked list.

   The implementation of a list allows very fast inserting and deleting of
   objects in the collection, but has severe penalties for random access. All
   object access should be done sequentially to avoid these speed penalties.

   The class remembers the last accessed element. This state information is
   used to optimise access by the "virtual array" model of collections. If
   access via ordinal index is made sequentially there is little overhead.

   The PAbstractList class would very rarely be descended from directly by
   the user. The #PDECLARE_LIST# and #PLIST# macros would normally
   be used to create descendent classes. They will instantiate the template
   based on #PList# or directly declare and define the class (using
   inline functions) if templates are not being used.

   The #PList# class or #PDECLARE_LIST# macro will define the
   correctly typed operators for subscript access (#operator[]#).
 */
class PAbstractList : public PCollection
{
  PCONTAINERINFO(PAbstractList, PCollection);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, list.

       Note that by default, objects placed into the list will be deleted when
       removed or when all references to the list are destroyed.
     */
    PINLINE PAbstractList();
  //@}

  // Overrides from class PObject
    /**Get the relative rank of the two lists. The following algorithm is
       employed for the comparison:
\begin{description}
       \item[#EqualTo#] if the two lists are identical in length
       and each objects values, not pointer, are equal.

       \item[#LessThan#] if the instances object value at an
       ordinal position is less than the corresponding objects value in the
       #obj# parameters list.
                          
       This is also returned if all objects are equal and the instances list
       length is less than the #obj# parameters list length.

       \item[#GreaterThan#] if the instances object value at an
       ordinal position is greater than the corresponding objects value in the
       #obj# parameters list.
                          
       This is also returned if all objects are equal and the instances list
       length is greater than the #obj# parameters list length.
\end{description}

       @return
       comparison of the two objects, #EqualTo# for same,
       #LessThan# for #obj# logically less than the
       object and #GreaterThan# for #obj# logically
       greater than the object.
     */
    virtual Comparison Compare(const PObject & obj) const;

  /**@name Overrides from class PContainer */
  //@{
    /**This function is meaningless for lists. The size of the collection is
       determined by the addition and removal of objects. The size cannot be
       set in any other way.

       @return
       Always TRUE.
     */
    virtual BOOL SetSize(
      PINDEX newSize  ///< New size for the list, this is ignored.
    );
  //@}

  /**@name Overrides from class PCollection */
  //@{
    /**Append a new object to the collection. This places a new link at the
       "tail" of the list.
    
       @return
       index of the newly added object.
     */
    virtual PINDEX Append(
      PObject * obj   ///< New object to place into the collection.
    );

    /**Insert a new object immediately before the specified object. If the
       object to insert before is not in the collection then the equivalent of
       the #Append()# function is performed.
       
       Note that the object values are compared for the search of the
       #before# parameter, not the pointers. So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function.

       @return
       index of the newly inserted object.
     */
    virtual PINDEX Insert(
      const PObject & before,   ///< Object value to insert before.
      PObject * obj             ///< New object to place into the collection.
    );

    /**Insert a new object at the specified ordinal index. If the index is
       greater than the number of objects in the collection then the
       equivalent of the #Append()# function is performed.

       @return
       index of the newly inserted object.
     */
    virtual PINDEX InsertAt(
      PINDEX index,   ///< Index position in collection to place the object.
      PObject * obj   ///< New object to place into the collection.
    );

    /**Remove the object from the collection. If the AllowDeleteObjects option
       is set then the object is also deleted.

       @return
       TRUE if the object was in the collection.
     */
    virtual BOOL Remove(
      const PObject * obj   ///< Existing object to remove from the collection.
    );

    /**Remove the object at the specified ordinal index from the collection.
       If the AllowDeleteObjects option is set then the object is also deleted.

       Note if the index is beyond the size of the collection then the
       function will assert.

       @return
       pointer to the object being removed, or NULL if it was deleted.
     */
    virtual PObject * RemoveAt(
      PINDEX index   ///< Index position in collection to place the object.
    );

    /**Set the object at the specified ordinal position to the new value. This
       will overwrite the existing entry. 
       This method will NOT delete the old object independently of the 
       AllowDeleteObjects option. Use #ReplaceAt()# instead.

       Note if the index is beyond the size of the collection then the
       function will assert.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL SetAt(
      PINDEX index,   ///< Index position in collection to set.
      PObject * val   ///< New value to place into the collection.
    );
    
    /**Set the object at the specified ordinal position to the new value. This
       will overwrite the existing entry. If the AllowDeleteObjects option is
       set then the old object is also deleted.
    
       Note if the index is beyond the size of the collection then the
       function will assert.
       
       @return
       TRUE if the object was successfully replaced.
     */   
    virtual BOOL ReplaceAt(
      PINDEX index,   ///< Index position in collection to set.
      PObject * val   ///< New value to place into the collection.
    );

    /**Get the object at the specified ordinal position. If the index was
       greater than the size of the collection then NULL is returned.

       The object accessed in this way is remembered by the class and further
       access will be fast. Access to elements one either side of that saved
       element, and the head and tail of the list, will always be fast.

       @return
       pointer to object at the specified index.
     */
    virtual PObject * GetAt(
      PINDEX index  ///< Index position in the collection of the object.
    ) const;

    /**Search the collection for the specific instance of the object. The
       object pointers are compared, not the values. A simple linear search
       from "head" of the list is performed.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetObjectsIndex(
      const PObject * obj  ///< Object to find.
    ) const;

    /**Search the collection for the specified value of the object. The object
       values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. A simple linear search from "head" of the list is performed.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetValuesIndex(
      const PObject & obj  ///< Object to find value of.
    ) const;
  //@}


  protected:
    /**Get the object at the specified ordinal position. If the index was
       greater than the size of the collection then this asserts.

       The object accessed in this way is remembered by the class and further
       access will be fast. Access to elements one either side of that saved
       element, and the head and tail of the list, will always be fast.

       @return
       reference to object at the specified index.
     */
    PINLINE PObject & GetReferenceAt(
      PINDEX index  ///< Ordinal index of the list element to set as current.
    ) const;

    /**Move the internal "cursor" to the index position specified. This
       function will optimise the sequential move taking into account the
       previous current position and the position at the head and tail of the
       list. Whichever of these three points is closes is used as the starting
       point for a sequential move to the required index.

       @return
       TRUE if the index could be set as the current element.
     */
    BOOL SetCurrent(
      PINDEX index  ///< Ordinal index of the list element to set as current.
    ) const;

    // The types below cannot be nested as DevStudio 2005 AUTOEXP.DAT doesn't like it
    typedef PListElement Element;
    PListInfo * info;
};


#ifdef PHAS_TEMPLATES

/**This template class maps the PAbstractList to a specific object type. The
   functions in this class primarily do all the appropriate casting of types.

   Note that if templates are not used the #PDECLARE_LIST# macro will
   simulate the template instantiation.
 */
template <class T> class PList : public PAbstractList
{
  PCLASSINFO(PList, PAbstractList);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, list.

       Note that by default, objects placed into the list will be deleted when
       removed or when all references to the list are destroyed.
     */
    PList()
      : PAbstractList() { }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the list. Note that all objects in the
       array are also cloned, so this will make a complete copy of the list.
     */
    virtual PObject * Clone() const
      { return PNEW PList(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Retrieve a reference  to the object in the list. If there was not an
       object at that ordinal position or the index was beyond the size of the
       array then the function asserts.

       The object accessed in this way is remembered by the class and further
       access will be fast. Access to elements one either side of that saved
       element, and the head and tail of the list, will always be fast.

       @return
       reference to the object at #index# position.
     */
    T & operator[](PINDEX index) const
      { return (T &)GetReferenceAt(index); }
  //@}

  protected:
    PList(int dummy, const PList * c)
      : PAbstractList(dummy, c) { }
};


/**Declare a list class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}. This macro closes the
   class declaration off so no additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PList# template class.

   See the #PList# class and #PDECLARE_LIST# macro for more
   information.
 */
#define PLIST(cls, T) typedef PList<T> cls

/**Begin declaration of list class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}.

   If the compilation is using templates then this macro produces a descendent
   of the #PList# template class. If templates are not being used then the
   macro defines a set of inline functions to do all casting of types. The
   resultant classes have an identical set of functions in either case.

   See the #PList# and #PAbstractList# classes for more information.
 */
#define PDECLARE_LIST(cls, T) \
  PLIST(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, PList<T>) \
  protected: \
    cls(int dummy, const cls * c) \
      : PList<T>(dummy, c) { } \
  public: \
    cls() \
      : PList<T>() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


/**This template class maps the PAbstractList to a specific object type, and
   adds functionality that allows the list to be used as a first in first out
   queue. The functions in this class primarily do all the appropriate casting
   of types.

   By default, objects placed into the set will {\bf not} be deleted when
   removed or when all references to the set are destroyed. This is different
   from the default on most collection classes.

   Note that if templates are not used the #PDECLARE_QUEUE# macro will
   simulate the template instantiation.
 */
template <class T> class PQueue : public PAbstractList
{
  PCLASSINFO(PQueue, PAbstractList);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, queue.

       Note that by default, objects placed into the queue will {\bf not} be
       deleted when removed or when all references to the queue are destroyed.
       This is different from the default on most collection classes.
     */
    PQueue()
      : PAbstractList() { DisallowDeleteObjects(); }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the list. Note that all objects in the
       array are also cloned, so this will make a complete copy of the list.
     */
    virtual PObject * Clone() const
      { return PNEW PQueue(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Add a new object to the queue. This places a new link at the "tail" of
       the list, which is the "in" side of the queue.
     */
    virtual void Enqueue(
      T * obj   ///< Object to add to the queue.
    ) { PAbstractList::Append(obj); }
    /**Remove an object that was added to the queue.

       @return
       first object added to the queue or NULL if queue empty.
     */
    virtual T * Dequeue()
      { if (GetSize() == 0) return NULL; else return (T *)PAbstractList::RemoveAt(0);}
  //@}

  protected:
    PQueue(int dummy, const PQueue * c)
      : PAbstractList(dummy, c)
      { reference->deleteObjects = c->reference->deleteObjects; }
};


/**Declare a queue class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}, and adds functionality
   that allows the list to be used as a first in first out queue. This macro
   closes the class declaration off so no additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PQueue# template class.

   See the #PList# class and #PDECLARE_QUEUE# macro for more
   information.
 */
#define PQUEUE(cls, T) typedef PQueue<T> cls


/**Begin declataion of a queue class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}, and adds functionality
   that allows the list to be used as a first in first out queue.

   If the compilation is using templates then this macro produces a descendent
   of the #PQueue# template class. If templates are not being used then
   the macro defines a set of inline functions to do all casting of types. The
   resultant classes have an identical set of functions in either case.

   See the #PQueue# and #PAbstractList# classes for more information.
 */
#define PDECLARE_QUEUE(cls, T) \
  PQUEUE(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


/**This template class maps the PAbstractList to a specific object type, and
   adds functionality that allows the list to be used as a last in first out
   stack. The functions in this class primarily do all the appropriate casting
   of types.

   By default, objects placed into the set will {\bf not} be deleted when
   removed or when all references to the set are destroyed. This is different
   from the default on most collection classes.

   Note that if templates are not used the #PDECLARE_STACK# macro will
   simulate the template instantiation.
 */
template <class T> class PStack : public PAbstractList
{
  PCLASSINFO(PStack, PAbstractList);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, stack.

       Note that by default, objects placed into the stack will {\bf not} be
       deleted when removed or when all references to the stack are destroyed.
       This is different from the default on most collection classes.
     */
    PStack()
      : PAbstractList() { DisallowDeleteObjects(); }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the stack. Note that all objects in the
       array are also cloned, so this will make a complete copy of the stack.
     */
    virtual PObject * Clone() const
      { return PNEW PStack(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Add an object to the stack. This object will be on "top" of the stack
       and will be the object returned by the #Pop()#
       function.
     */
    virtual void Push(
      T * obj    ///< Object to add to the stack.
    ) { PAbstractList::InsertAt(0, obj); }

    /**Remove the last object pushed onto the stack.

       @return
       object on top of the stack.
     */
    virtual T * Pop()
      { return (T *)PAbstractList::RemoveAt(0); }

    /**Get the element that is currently on top of the stack without removing
       it.

       @return
       reference to object on top of the stack.
     */
    virtual T & Top()
      { PAssert(GetSize() > 0, PStackEmpty); return *(T *)GetAt(0); }
  //@}

  protected:
    PStack(int dummy, const PStack * c)
      : PAbstractList(dummy, c)
      { reference->deleteObjects = c->reference->deleteObjects; }
};


/**Declare a stack class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}, and adds functionality
   that allows the list to be used as a last in first out stack. This macro
   closes the class declaration off so no additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PStack# template class.

   See the #PStack# class and #PDECLARE_STACK# macro for more
   information.
 */
#define PSTACK(cls, T) typedef PStack<T> cls


/**Begin declaration of a stack class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular object type {\bf T}, and adds functionality
   that allows the list to be used as a last in first out stack.

   If the compilation is using templates then this macro produces a descendent
   of the #PStack# template class. If templates are not being used then
   the macro defines a set of inline functions to do all casting of types. The
   resultant classes have an identical set of functions in either case.

   See the #PStack# and #PAbstractList# classes for more information.
 */
#define PDECLARE_STACK(cls, T) \
  PSTACK(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#else // PHAS_TEMPLATES


#define PLIST(cls, T) \
  class cls : public PAbstractList { \
  PCLASSINFO(cls, PAbstractList); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractList(dummy, c) { } \
  public: \
    inline cls() \
      : PAbstractList() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    inline T & operator[](PINDEX index) const \
      { return (T &)GetReferenceAt(index); } \
  }

#define PDECLARE_LIST(cls, T) \
  PLIST(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#define PQUEUE(cls, T) \
  class cls : public PAbstractList { \
  PCLASSINFO(cls, PAbstractList); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractList(dummy, c) \
      { reference->deleteObjects = c->reference->deleteObjects; } \
  public: \
    inline cls() \
      : PAbstractList() { DisallowDeleteObjects(); } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    virtual void Enqueue(T * t) \
      { PAbstractList::Append(t); } \
    virtual T * Dequeue() \
      { if (GetSize() == 0) return NULL; else return (T *)PAbstractList::RemoveAt(0);} \
  }

#define PDECLARE_QUEUE(cls, T) \
  PQUEUE(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \

#define PSTACK(cls, T) \
  class cls : public PAbstractList { \
  PCLASSINFO(cls, PAbstractList); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractList(dummy, c) \
      { reference->deleteObjects = c->reference->deleteObjects; } \
  public: \
    inline cls() \
      : PAbstractList() { DisallowDeleteObjects(); } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    virtual void Push(T * t) \
      { PAbstractList::InsertAt(0, t); } \
    virtual T * Pop() \
      { PAssert(GetSize() > 0, PStackEmpty); return (T *)PAbstractList::RemoveAt(0); } \
    virtual T & Top() \
      { PAssert(GetSize() > 0, PStackEmpty); return *(T *)GetAt(0); } \
  }

#define PDECLARE_STACK(cls, T) \
  PSTACK(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#endif // PHAS_TEMPLATES


///////////////////////////////////////////////////////////////////////////////
// Sorted List of PObjects

struct PSortedListElement
{
  PSortedListElement * parent;
  PSortedListElement * left;
  PSortedListElement * right;
  PObject            * data;
  PINDEX               subTreeSize;
  enum { Red, Black }  colour;
};

struct PSortedListInfo
{
  PSortedListInfo();

  PSortedListElement * root;
  PSortedListElement * lastElement;
  PINDEX               lastIndex;
  PSortedListElement   nil;

  PSortedListElement * Successor(const PSortedListElement * node) const;
  PSortedListElement * Predecessor(const PSortedListElement * node) const;
  PSortedListElement * OrderSelect(PSortedListElement * node, PINDEX index) const;

  typedef PSortedListElement Element;
};

/**This class is a collection of objects which are descendents of the
   #PObject# class. It is implemeted as a Red-Black binary tree to
   maintain the objects in rank order. Note that this requires that the
   #PObject::Compare()# function be fully implemented oin objects
   contained in the collection.

   The implementation of a sorted list allows fast inserting and deleting as
   well as random access of objects in the collection. As the objects are being
   kept sorted, "fast" is a relative term. All operations take o(lg n) unless
   a particular object is repeatedly accessed.

   The class remembers the last accessed element. This state information is
   used to optimise access by the "virtual array" model of collections. If
   repeated access via ordinal index is made there is little overhead. All
   other access incurs a minimum overhead, but not insignificant.

   The PAbstractSortedList class would very rarely be descended from directly
   by the user. The #PDECLARE_LIST# and #PLIST# macros would normally
   be used to create descendent classes. They will instantiate the template
   based on #PSortedList# or directly declare and define the class (using
   inline functions) if templates are not being used.

   The #PSortedList# class or #PDECLARE_SORTED_LIST# macro will
   define the correctly typed operators for subscript access
   (#operator[]#).
 */
class PAbstractSortedList : public PCollection
{
  PCONTAINERINFO(PAbstractSortedList, PCollection);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, sorted list.

       Note that by default, objects placed into the list will be deleted when
       removed or when all references to the list are destroyed.
     */
    PAbstractSortedList();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Get the relative rank of the two lists. The following algorithm is
       employed for the comparison:
\begin{descriptions}
       \item[#EqualTo#] if the two lists are identical in length
       and each objects values, not pointer, are equal.

       \item[#LessThan#] if the instances object value at an
       ordinal position is less than the corresponding objects value in the
       #obj# parameters list.
                          
       This is also returned if all objects are equal and the instances list
       length is less than the #obj# parameters list length.

       \item[#GreaterThan#] if the instances object value at an
       ordinal position is greater than the corresponding objects value in the
       #obj# parameters list.
                          
       This is also returned if all objects are equal and the instances list
       length is greater than the #obj# parameters list length.
\end{descriptions}

       @return
       comparison of the two objects, #EqualTo# for same,
       #LessThan# for #obj# logically less than the
       object and #GreaterThan# for #obj# logically
       greater than the object.
     */
    virtual Comparison Compare(const PObject & obj) const;
  //@}

  /**@name Overrides from class PContainer */
  //@{
    /**This function is meaningless for lists. The size of the collection is
       determined by the addition and removal of objects. The size cannot be
       set in any other way.

       @return
       Always TRUE.
     */
    virtual BOOL SetSize(
      PINDEX newSize  // New size for the sorted list, this is ignored.
    );
  //@}

  /**@name Overrides from class PCollection */
  //@{
    /**Add a new object to the collection. The object is always placed in the
       correct ordinal position in the list. It is not placed at the "end".

       @return
       index of the newly added object.
     */
    virtual PINDEX Append(
      PObject * obj   // New object to place into the collection.
    );

    /**Add a new object to the collection.
    
       The object is always placed in the correct ordinal position in the list.
       It is not placed at the specified position. The #before#
       parameter is ignored.

       @return
       index of the newly inserted object.
     */
    virtual PINDEX Insert(
      const PObject & before,   // Object value to insert before.
      PObject * obj             // New object to place into the collection.
    );

    /**Add a new object to the collection.
    
       The object is always placed in the correct ordinal position in the list.
       It is not placed at the specified position. The #index#
       parameter is ignored.

       @return
       index of the newly inserted object.
     */
    virtual PINDEX InsertAt(
      PINDEX index,   // Index position in collection to place the object.
      PObject * obj   // New object to place into the collection.
    );

    /**Remove the object from the collection. If the AllowDeleteObjects option
       is set then the object is also deleted.

       Note that the comparison for searching for the object in collection is
       made by pointer, not by value. Thus the parameter must point to the
       same instance of the object that is in the collection.

       @return
       TRUE if the object was in the collection.
     */
    virtual BOOL Remove(
      const PObject * obj   // Existing object to remove from the collection.
    );

    /**Remove the object at the specified ordinal index from the collection.
       If the AllowDeleteObjects option is set then the object is also deleted.

       Note if the index is beyond the size of the collection then the
       function will assert.

       @return
       pointer to the object being removed, or NULL if it was deleted.
     */
    virtual PObject * RemoveAt(
      PINDEX index   // Index position in collection to place the object.
    );

    /**Remove all of the elements in the collection. This operates by
       continually calling #RemoveAt()# until there are no objects left.

       The objects are removed from the last, at index
       #(GetSize()-1)# toward the first at index zero.
     */
    virtual void RemoveAll();

    /**This method simply returns FALSE as the list order is mantained by the 
       class. Kept to mimic #PAbstractList# interface.
       
       @return
       FALSE allways
     */
    virtual BOOL SetAt(
      PINDEX index,   // Index position in collection to set.
      PObject * val   // New value to place into the collection.
    );

    /**Get the object at the specified ordinal position. If the index was
       greater than the size of the collection then NULL is returned.

       @return
       pointer to object at the specified index.
     */
    virtual PObject * GetAt(
      PINDEX index  // Index position in the collection of the object.
    ) const;

    /**Search the collection for the specific instance of the object. The
       object pointers are compared, not the values. A binary search is
       employed to locate the entry.
       
       Note that that will require value comparisons to be made to find the
       equivalent entry and then a final check is made with the pointers to
       see if they are the same instance.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetObjectsIndex(
      const PObject * obj
    ) const;

    /**Search the collection for the specified value of the object. The object
       values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. A binary search is employed to locate the entry.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetValuesIndex(
      const PObject & obj
    ) const;
  //@}

    // The type below cannot be nested as DevStudio 2005 AUTOEXP.DAT doesn't like it
    typedef PSortedListElement Element;

  protected:
    
    // New functions for class
    void RemoveElement(Element * node);
    void LeftRotate(Element * node);
    void RightRotate(Element * node);
    void DeleteSubTrees(Element * node, BOOL deleteObject);
    PINDEX ValueSelect(const Element * node, const PObject & obj, const Element ** lastElement) const;

    // The type below cannot be nested as DevStudio 2005 AUTOEXP.DAT doesn't like it
    PSortedListInfo * info;
};


#ifdef PHAS_TEMPLATES

/**This template class maps the PAbstractSortedList to a specific object type.
   The functions in this class primarily do all the appropriate casting of
   types.

   Note that if templates are not used the #PDECLARE_SORTED_LIST# macro
   will simulate the template instantiation.
 */
template <class T> class PSortedList : public PAbstractSortedList
{
  PCLASSINFO(PSortedList, PAbstractSortedList);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, sorted list.

       Note that by default, objects placed into the list will be deleted when
       removed or when all references to the list are destroyed.
     */
    PSortedList()
      : PAbstractSortedList() { }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the list. Note that all objects in the
       array are also cloned, so this will make a complete copy of the list.
     */
    virtual PObject * Clone() const
      { return PNEW PSortedList(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Retrieve a reference  to the object in the list. If there was not an
       object at that ordinal position or the index was beyond the size of the
       array then the function asserts.

       The object accessed in this way is remembered by the class and further
       access will be fast.

       @return
       reference to the object at #index# position.
     */
    T & operator[](PINDEX index) const
      { return *(T *)GetAt(index); }
  //@}

  protected:
    PSortedList(int dummy, const PSortedList * c)
      : PAbstractSortedList(dummy, c) { }
};


/**Declare a sorted list class.
   This macro is used to declare a descendent of PAbstractSortedList class,
   customised for a particular object type {\bf T}. This macro closes the
   class declaration off so no additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PSortedList# template class.

   See the #PSortedList# class and #PDECLARE_SORTED_LIST# macro for
   more information.
 */
#define PSORTED_LIST(cls, T) typedef PSortedList<T> cls


/**Begin declaration of a sorted list class.
   This macro is used to declare a descendent of PAbstractSortedList class,
   customised for a particular object type {\bf T}.

   If the compilation is using templates then this macro produces a descendent
   of the #PSortedList# template class. If templates are not being used
   then the macro defines a set of inline functions to do all casting of types.
   The resultant classes have an identical set of functions in either case.

   See the #PSortedList# and #PAbstractSortedList# classes for more
   information.
 */
#define PDECLARE_SORTED_LIST(cls, T) \
  PSORTED_LIST(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, PSortedList<T>) \
  protected: \
    cls(int dummy, const cls * c) \
      : PSortedList<T>(dummy, c) { } \
  public: \
    cls() \
      : PSortedList<T>() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#else // PHAS_TEMPLATES


#define PSORTED_LIST(cls, T) \
  class cls : public PAbstractSortedList { \
  PCLASSINFO(cls, PAbstractSortedList); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractSortedList(dummy, c) { } \
  public: \
    inline cls() \
      : PAbstractSortedList() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    inline T & operator[](PINDEX index) const \
      { return *(T *)GetAt(index); } \
  }

#define PDECLARE_SORTED_LIST(cls, T) \
  PSORTED_LIST(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#endif  // PHAS_TEMPLATES


// End Of File ///////////////////////////////////////////////////////////////
