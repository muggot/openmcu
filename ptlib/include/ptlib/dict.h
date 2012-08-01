/*
 * dict.h
 *
 * Dictionary (hash table) Container classes.
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
 * $Log: dict.h,v $
 * Revision 1.38  2007/08/01 05:20:48  rjongbloed
 * Changes to container classes to become compatible with advanced DevStudio 2005 "Visualizers".
 *
 * Revision 1.37  2006/06/30 00:56:31  csoutheren
 * Applied 1494931 - various pwlib bug fixes and enhancement
 * Thanks to Frederich Heem
 *
 * Revision 1.36  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.35  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.34  2004/11/23 11:33:08  csoutheren
 * Fixed problem with RemoveAt returning invalid pointer in some cases,
 * and added extra documentation on this case.
 * Thanks to Diego Tartara for pointing out this potential problem
 *
 * Revision 1.33  2004/04/09 03:42:34  csoutheren
 * Removed all usages of "virtual inline" and "inline virtual"
 *
 * Revision 1.32  2004/04/03 23:53:09  csoutheren
 * Added various changes to improce compatibility with the Sun Forte compiler
 *   Thanks to Brian Cameron
 * Added detection of readdir_r version
 *
 * Revision 1.31  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.30  2003/03/31 01:23:56  robertj
 * Added ReadFrom functions for standard container classes such as
 *   PIntArray and PStringList etc
 *
 * Revision 1.29  2002/10/04 01:47:29  robertj
 * Added various increment and decrement operators to POrdinalKey.
 *
 * Revision 1.28  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.27  2002/06/14 13:22:12  robertj
 * Fixed ability to remove elements from a PSet by value.
 * Added by value add and remove functions to a PSet.
 * Added a POrdinalSet class.
 * Fixed some documentation.
 *
 * Revision 1.26  2002/02/06 00:53:25  robertj
 * Fixed missing const on PSet::Contains and operator[], thanks Francisco Olarte Sanz
 *
 * Revision 1.25  1999/11/30 00:22:54  robertj
 * Updated documentation for doc++
 *
 * Revision 1.24  1999/08/22 12:13:43  robertj
 * Fixed warning when using inlines on older GNU compiler
 *
 * Revision 1.23  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.22  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.21  1998/09/23 06:20:27  robertj
 * Added open source copyright license.
 *
 * Revision 1.20  1998/01/05 10:39:34  robertj
 * Fixed "typesafe" templates/macros for dictionaries, especially on GNU.
 *
 * Revision 1.19  1997/12/11 10:27:16  robertj
 * Added type correct Contains() function to dictionaries.
 *
 * Revision 1.18  1997/07/08 13:15:05  robertj
 * DLL support.
 *
 * Revision 1.17  1997/06/08 04:49:11  robertj
 * Fixed non-template class descendent order.
 *
 * Revision 1.16  1996/08/17 10:00:22  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.15  1996/03/31 08:44:10  robertj
 * Added RemoveAt() function to remove entries from dictionaries.
 *
 * Revision 1.14  1996/02/08 11:50:01  robertj
 * Moved Contains function from PSet to PHashTable so available for dictionaries.
 * Added print for dictionaries key=data\n.
 * Added GetAt(PINDEX) to template classes to make identical to macro.
 *
 * Revision 1.13  1996/02/03 11:00:28  robertj
 * Temporary removal of SetAt() and GetAt() functions in dictionary macro.
 *
 * Revision 1.12  1996/01/24 14:43:11  robertj
 * Added initialisers to string dictionaries.
 *
 * Revision 1.11  1996/01/23 13:11:12  robertj
 * Mac Metrowerks compiler support.
 *
 * Revision 1.10  1995/06/17 11:12:29  robertj
 * Documentation update.
 *
 * Revision 1.9  1995/06/04 08:45:57  robertj
 * Better C++ compatibility (with BC++)
 *
 * Revision 1.8  1995/03/14 12:41:19  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.7  1995/02/22  10:50:29  robertj
 * Changes required for compiling release (optimised) version.
 *
 * Revision 1.6  1995/02/11  04:10:35  robertj
 * Fixed dictionary MACRO for templates.
 *
 * Revision 1.5  1995/02/05  00:48:03  robertj
 * Fixed template version.
 *
 * Revision 1.4  1995/01/09  12:35:31  robertj
 * Removed unnecesary return value from I/O functions.
 * Changes due to Mac port.
 *
 * Revision 1.3  1994/12/21  11:52:51  robertj
 * Documentation and variable normalisation.
 *
 * Revision 1.2  1994/12/17  01:36:57  robertj
 * Fixed memory leak in PStringSet
 *
 * Revision 1.1  1994/12/12  09:59:32  robertj
 * Initial revision
 *
 */


#ifndef __DICT_H__
#define __DICT_H__

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/array.h>

///////////////////////////////////////////////////////////////////////////////
// PDictionary classes

/**This class is used when an ordinal index value is the key for #PSet#
   and #PDictionary# classes.
 */
class POrdinalKey : public PObject
{
  PCLASSINFO(POrdinalKey, PObject);

  public:
  /**@name Construction */
  //@{
    /** Create a new key for ordinal index values.
     */
    PINLINE POrdinalKey(
      PINDEX newKey = 0   ///< Ordinal index value to use as a key.
    );

    /**Operator to assign the ordinal.
      */
    PINLINE POrdinalKey & operator=(PINDEX);
  //@}

  /**@name Overrides from class PObject */
  //@{
    /// Create a duplicate of the POrdinalKey.
    virtual PObject * Clone() const;

    /* Get the relative rank of the ordinal index. This is a simpel comparison
       of the objects PINDEX values.

       @return
       comparison of the two objects, #EqualTo# for same,
       #LessThan# for #obj# logically less than the
       object and #GreaterThan# for #obj# logically
       greater than the object.
     */
    virtual Comparison Compare(const PObject & obj) const;

    /**This function calculates a hash table index value for the implementation
       of #PSet# and #PDictionary# classes.

       @return
       hash table bucket number.
     */
    virtual PINDEX HashFunction() const;

    /**Output the ordinal index to the specified stream. This is identical to
       outputting the PINDEX, ie integer, value.

       @return
       stream that the index was output to.
     */
    virtual void PrintOn(ostream & strm) const;
  //@}

  /**@name New functions for class */
  //@{
    /** Operator so that a POrdinalKey can be used as a PINDEX value.
     */
    PINLINE operator PINDEX() const;

    /**Operator to pre-increment the ordinal.
      */
    PINLINE PINDEX operator++();

    /**Operator to post-increment the ordinal.
      */
    PINLINE PINDEX operator++(int);

    /**Operator to pre-decrement the ordinal.
      */
    PINLINE PINDEX operator--();

    /**Operator to post-decrement the ordinal.
      */
    PINLINE PINDEX operator--(int);

    /**Operator to add the ordinal.
      */
    PINLINE POrdinalKey & operator+=(PINDEX);

    /**Operator to subtract from the ordinal.
      */
    PINLINE POrdinalKey & operator-=(PINDEX );
  //@}

  private:
    PINDEX theKey;
};


//////////////////////////////////////////////////////////////////////////////

// Member variables
struct PHashTableElement
{
    PObject * key;
    PObject * data;
    PHashTableElement * next;
    PHashTableElement * prev;
};

PDECLARE_BASEARRAY(PHashTableInfo, PHashTableElement *)
#ifdef DOC_PLUS_PLUS
{
#endif
  public:
    virtual ~PHashTableInfo() { Destruct(); }
    virtual void DestroyContents();

    PINDEX AppendElement(PObject * key, PObject * data);
    PObject * RemoveElement(const PObject & key);
    BOOL SetLastElementAt(PINDEX index);
    PHashTableElement * GetElementAt(const PObject & key);
    PINDEX GetElementsIndex(const PObject*obj,BOOL byVal,BOOL keys) const;

    PINDEX lastIndex;
    PINDEX lastBucket;
    PHashTableElement * lastElement;

    BOOL deleteKeys;

  typedef PHashTableElement Element;
  friend class PHashTable;
  friend class PAbstractSet;
};


/**The hash table class is the basis for implementing the #PSet# and
   #PDictionary# classes.

   The hash table allows for very fast searches for an object based on a "hash
   function". This function yields an index into an array which is directly
   looked up to locate the object. When two key values have the same hash
   function value, then a linear search of a linked list is made to locate
   the object. Thus the efficiency of the hash table is highly dependent on the
   quality of the hash function for the data being used as keys.
 */
class PHashTable : public PCollection
{
  PCONTAINERINFO(PHashTable, PCollection);

  public:
  /**@name Construction */
  //@{
    /// Create a new, empty, hash table.
    PHashTable();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Get the relative rank of the two hash tables. Actally ranking hash
       tables is really meaningless, so only equality is returned by the
       comparison. Equality is only achieved if the two instances reference the
       same hash table.

       @return
       comparison of the two objects, #EqualTo# if the same
       reference and #GreaterThan# if not.
     */
    virtual Comparison Compare(
      const PObject & obj   ///< Other PHashTable to compare against.
    ) const;
  //@}


  protected:
  /**@name Overrides from class PContainer */
  //@{
    /**This function is meaningless for hash table. The size of the collection
       is determined by the addition and removal of objects. The size cannot be
       set in any other way.

       @return
       Always TRUE.
     */
    virtual BOOL SetSize(
      PINDEX newSize  ///< New size for the hash table, this is ignored.
    );
  //@}


  /**@name New functions for class */
  //@{
    /**Determine if the value of the object is contained in the hash table. The
       object values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       TRUE if the object value is in the set.
     */
    PINLINE BOOL AbstractContains(
      const PObject & key   ///< Key to look for in the set.
    ) const;

    /**Get the key in the hash table at the ordinal index position.
    
       The ordinal position in the hash table is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       This function is primarily used by the descendent template classes, or
       macro, with the appropriate type conversion.

       @return
       reference to key at the index position.
     */
    virtual const PObject & AbstractGetKeyAt(
      PINDEX index  ///< Ordinal position in the hash table.
    ) const;

    /**Get the data in the hash table at the ordinal index position.
    
       The ordinal position in the hash table is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       This function is primarily used by the descendent template classes, or
       macro, with the appropriate type conversion.

       @return
       reference to key at the index position.
     */
    virtual PObject & AbstractGetDataAt(
      PINDEX index  ///< Ordinal position in the hash table.
    ) const;
  //@}

    // The type below cannot be nested as DevStudio 2005 AUTOEXP.DAT doesn't like it
    typedef PHashTableElement Element;
    typedef PHashTableInfo Table;
    PHashTableInfo * hashTable;
};


//////////////////////////////////////////////////////////////////////////////

/** Abstract set of PObjects.
 */
class PAbstractSet : public PHashTable
{
  PCONTAINERINFO(PAbstractSet, PHashTable);
  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, set.

       Note that by default, objects placed into the list will be deleted when
       removed or when all references to the list are destroyed.
     */
    PINLINE PAbstractSet();
  //@}

  /**@name Overrides from class PCollection */
  //@{
    /**Add a new object to the collection. If the objects value is already in
       the set then the object is {\bf not} included. If the
       #AllowDeleteObjects# option is set then the #obj# parameter
       is also deleted.

       @return
       hash function value of the newly added object.
     */
    virtual PINDEX Append(
      PObject * obj   ///< New object to place into the collection.
    );

    /**Add a new object to the collection. If the objects value is already in
       the set then the object is {\bf not} included. If the
       AllowDeleteObjects option is set then the #obj# parameter is
       also deleted.
       
       The object is always placed in the an ordinal position dependent on its
       hash function. It is not placed at the specified position. The
       #before# parameter is ignored.

       @return
       hash function value of the newly added object.
     */
    virtual PINDEX Insert(
      const PObject & before,   ///< Object value to insert before.
      PObject * obj             ///< New object to place into the collection.
    );

    /**Add a new object to the collection. If the objects value is already in
       the set then the object is {\bf not} included. If the
       AllowDeleteObjects option is set then the #obj# parameter is
       also deleted.
       
       The object is always placed in the an ordinal position dependent on its
       hash function. It is not placed at the specified position. The
       #index# parameter is ignored.

       @return
       hash function value of the newly added object.
     */
    virtual PINDEX InsertAt(
      PINDEX index,   ///< Index position in collection to place the object.
      PObject * obj   ///< New object to place into the collection.
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
      const PObject * obj   ///< Existing object to remove from the collection.
    );

    /**Remove an object at the specified index. If the #AllowDeleteObjects#
       option is set then the object is also deleted.

       @return
       pointer to the object being removed, or NULL if it was deleted.
     */
    virtual PObject * RemoveAt(
      PINDEX index   ///< Index position in collection to place the object.
    );

    /**This function is the same as PHashTable::AbstractGetKeyAt().

       @return
       Always NULL.
     */
    virtual PObject * GetAt(
      PINDEX index  ///< Index position in the collection of the object.
    ) const;

    /**Add a new object to the collection. If the objects value is already in
       the set then the object is {\bf not} included. If the
       AllowDeleteObjects option is set then the #obj# parameter is
       also deleted.

       The object is always placed in the an ordinal position dependent on its
       hash function. It is not placed at the specified position. The
       #index# parameter is ignored.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL SetAt(
      PINDEX index,   ///< Index position in collection to set.
      PObject * val   ///< New value to place into the collection.
    );

    /**Search the collection for the specific instance of the object. The
       object pointers are compared, not the values. The hash table is used
       to locate the entry.

       Note that that will require value comparisons to be made to find the
       equivalent entry and then a final check is made with the pointers to
       see if they are the same instance.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetObjectsIndex(
      const PObject * obj   ///< Object to find.
    ) const;

    /**Search the collection for the specified value of the object. The object
       values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetValuesIndex(
      const PObject & obj   ///< Object to find equal value.
    ) const;
  //@}
};


#ifdef PHAS_TEMPLATES

/**This template class maps the PAbstractSet to a specific object type. The
   functions in this class primarily do all the appropriate casting of types.

   By default, objects placed into the set will {\bf not} be deleted when
   removed or when all references to the set are destroyed. This is different
   from the default on most collection classes.

   Note that if templates are not used the #PDECLARE_SET# macro will
   simulate the template instantiation.
 */
template <class T> class PSet : public PAbstractSet
{
  PCLASSINFO(PSet, PAbstractSet);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, dictionary. The parameter indicates whether to
       delete objects that are removed from the set.

       Note that by default, objects placed into the set will {\bf not} be
       deleted when removed or when all references to the set are destroyed.
       This is different from the default on most collection classes.
     */
    inline PSet(BOOL initialDeleteObjects = FALSE)
      : PAbstractSet() { AllowDeleteObjects(initialDeleteObjects); }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the set. Note that all objects in the
       array are also cloned, so this will make a complete copy of the set.
     */
    virtual PObject * Clone() const
      { return PNEW PSet(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Include the specified object into the set. If the objects value is
       already in the set then the object is {\bf not} included. If the
       AllowDeleteObjects option is set then the #obj# parameter is
       also deleted.

       The object values are compared, not the pointers.  So the objects in
       the collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.
     */
    void Include(
      const T * obj   // New object to include in the set.
    ) { Append((PObject *)obj); }

    /**Include the specified objects value into the set. If the objects value
       is already in the set then the object is {\bf not} included.

       The object values are compared, not the pointers.  So the objects in
       the collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.
     */
    PSet & operator+=(
      const T & obj   // New object to include in the set.
    ) { Append(obj.Clone()); return *this; }

    /**Remove the object from the set. If the AllowDeleteObjects option is set
       then the object is also deleted.

       The object values are compared, not the pointers.  So the objects in
       the collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.
     */
    void Exclude(
      const T * obj   // New object to exclude in the set.
    ) { Remove(obj); }

    /**Remove the objects value from the set. If the AllowDeleteObjects
       option is set then the object is also deleted.

       The object values are compared, not the pointers.  So the objects in
       the collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.
     */
    PSet & operator-=(
      const T & obj   // New object to exclude in the set.
    ) { RemoveAt(GetValuesIndex(obj)); return *this; }

    /**Determine if the value of the object is contained in the set. The
       object values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       TRUE if the object value is in the set.
     */
    BOOL Contains(
      const T & key  ///< Key to look for in the set.
    ) const { return AbstractContains(key); }

    /**Determine if the value of the object is contained in the set. The
       object values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       TRUE if the object value is in the set.
     */
    BOOL operator[](
      const T & key  ///< Key to look for in the set.
    ) const { return AbstractContains(key); }

    /**Get the key in the set at the ordinal index position.
    
       The ordinal position in the set is determined by the hash values of the
       keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to key at the index position.
     */
    virtual const T & GetKeyAt(
      PINDEX index    ///< Index of value to get.
    ) const
      { return (const T &)AbstractGetKeyAt(index); }
  //@}


  protected:
    PSet(int dummy, const PSet * c)
      : PAbstractSet(dummy, c)
      { reference->deleteObjects = c->reference->deleteObjects; }
};


/**Declare set class.
   This macro is used to declare a descendent of PAbstractSet class,
   customised for a particular object type {\bf T}. This macro closes the
   class declaration off so no additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PSet# template class.

   See the #PSet# class and #PDECLARE_SET# macro for more
   information.
 */
#define PSET(cls, T) typedef PSet<T> cls


/**Begin declaration of a set class.
   This macro is used to declare a descendent of PAbstractSet class,
   customised for a particular object type {\bf T}.

   If the compilation is using templates then this macro produces a descendent
   of the #PSet# template class. If templates are not being used then the
   macro defines a set of inline functions to do all casting of types. The
   resultant classes have an identical set of functions in either case.

   See the #PSet# and #PAbstractSet# classes for more information.
 */
#define PDECLARE_SET(cls, T, initDelObj) \
  PSET(cls##_PTemplate, T); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls(BOOL initialDeleteObjects = initDelObj) \
      : cls##_PTemplate(initialDeleteObjects) { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#else // PHAS_TEMPLATES


#define PSET(cls, K) \
  class cls : public PAbstractSet { \
  PCLASSINFO(cls, PAbstractSet); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractSet(dummy, c) \
      { reference->deleteObjects = c->reference->deleteObjects; } \
  public: \
    inline cls(BOOL initialDeleteObjects = FALSE) \
      : PAbstractSet() { AllowDeleteObjects(initialDeleteObjects); } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    inline void Include(const PObject * key) \
      { Append((PObject *)key); } \
    inline void Exclude(const PObject * key) \
      { Remove(key); } \
    inline BOOL operator[](const K & key) const \
        { return AbstractContains(key); } \
    inline BOOL Contains(const K & key) const \
        { return AbstractContains(key); } \
    virtual const K & GetKeyAt(PINDEX index) const \
      { return (const K &)AbstractGetKeyAt(index); } \
  }

#define PDECLARE_SET(cls, K, initDelObj) \
 PSET(cls##_PTemplate, K); \
 PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    inline cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    inline cls(BOOL initialDeleteObjects = initDelObj) \
      : cls##_PTemplate() { AllowDeleteObjects(initialDeleteObjects); } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#endif  // PHAS_TEMPLATES


PSET(POrdinalSet, POrdinalKey);


//////////////////////////////////////////////////////////////////////////////

/**An abstract dictionary container.
*/
class PAbstractDictionary : public PHashTable
{
  PCLASSINFO(PAbstractDictionary, PHashTable);
  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, dictionary.

       Note that by default, objects placed into the dictionary will be deleted
       when removed or when all references to the dictionary are destroyed.
     */
    PINLINE PAbstractDictionary();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Output the contents of the object to the stream. The exact output is
       dependent on the exact semantics of the descendent class. This is
       primarily used by the standard ##operator<<## function.

       The default behaviour is to print the class name.
     */
    virtual void PrintOn(
      ostream &strm   ///< Stream to print the object into.
    ) const;
  //@}

  /**@name Overrides from class PCollection */
  //@{
    /**Insert a new object into the dictionary. The semantics of this function
       is different from that of the #PCollection# class. This function is
       exactly equivalent to the SetAt() function that sets a data value at
       the key value location.

       @return
       Always zero.
     */
    virtual PINDEX Insert(
      const PObject & key,   ///< Object value to use as the key.
      PObject * obj          ///< New object to place into the collection.
    );

    /**Insert a new object at the specified index. The index is as is used in
       the #GetKeyAt()# function.

       @return
       #index# parameter.
     */
    virtual PINDEX InsertAt(
      PINDEX index,   ///< Index position in collection to place the object.
      PObject * obj   ///< New object to place into the collection.
    );

    /**Remove an object at the specified index. The index is as is used in
       the #GetKeyAt()# function. The returned pointer is then removed using
       the #SetAt()# function to set that key value to NULL. If the
       #AllowDeleteObjects# option is set then the object is also
       deleted.

       @return
       pointer to the object being removed, or NULL if it was deleted.
     */
    virtual PObject * RemoveAt(
      PINDEX index   ///< Index position in collection to place the object.
    );

    /**Set the object at the specified index to the new value. The index is
       as is used in the #GetKeyAt()# function. This will overwrite the
       existing entry. If the AllowDeleteObjects option is set then the old
       object is also deleted.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL SetAt(
      PINDEX index,   ///< Index position in collection to set.
      PObject * val   ///< New value to place into the collection.
    );

    /**Get the object at the specified index position. The index is as is
       used in the #GetKeyAt()# function. If the index was not in the
       collection then NULL is returned.

       @return
       pointer to object at the specified index.
     */
    virtual PObject * GetAt(
      PINDEX index  ///< Index position in the collection of the object.
    ) const;

    /**Search the collection for the specific instance of the object. The
       object pointers are compared, not the values. The hash table is used
       to locate the entry.

       Note that that will require value comparisons to be made to find the
       equivalent entry and then a final check is made with the pointers to
       see if they are the same instance.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetObjectsIndex(
      const PObject * obj  ///< Object to find.
    ) const;

    /**Search the collection for the specified value of the object. The object
       values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       ordinal index position of the object, or P_MAX_INDEX.
     */
    virtual PINDEX GetValuesIndex(
      const PObject & obj  ///< Object to find value of.
    ) const;
  //@}


  /**@name New functions for class */
  //@{
    /**Set the data at the specified ordinal index position in the dictionary.

       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       @return
       TRUE if the new object could be placed into the dictionary.
     */
    virtual BOOL SetDataAt(
      PINDEX index,   ///< Ordinal index in the dictionary.
      PObject * obj   ///< New object to put into the dictionary.
    );

    /**Add a new object to the collection. If the objects value is already in
       the dictionary then the object is overrides the previous value. If the
       AllowDeleteObjects option is set then the old object is also deleted.

       The object is placed in the an ordinal position dependent on the keys
       hash function. Subsequent searches use the has function to speed access
       to the data item.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL AbstractSetAt(
      const PObject & key,  ///< Key for position in dictionary to add object.
      PObject * obj         ///< New object to put into the dictionary.
    );

    /**Get the object at the specified key position. If the key was not in the
       collection then this function asserts.

       This function is primarily for use by the #operator[]# function is
       descendent template classes.

       @return
       reference to object at the specified key.
     */
    virtual PObject & GetRefAt(
      const PObject & key   ///< Key for position in dictionary to get object.
    ) const;

    /**Get the object at the specified key position. If the key was not in the
       collection then NULL is returned.

       @return
       pointer to object at the specified key.
     */
    virtual PObject * AbstractGetAt(
      const PObject & key   ///< Key for position in dictionary to get object.
    ) const;
  //@}

  protected:
    PINLINE PAbstractDictionary(int dummy, const PAbstractDictionary * c);

  private:
    virtual PINDEX Append(
      PObject * obj   // New object to place into the collection.
    );
    /* This function is meaningless and will assert.

       @return
       Always zero.
     */

    virtual BOOL Remove(
      const PObject * obj   // Existing object to remove from the collection.
    );
    /* Remove the object from the collection. If the AllowDeleteObjects option
       is set then the object is also deleted.

       Note that the comparison for searching for the object in collection is
       made by pointer, not by value. Thus the parameter must point to the
       same instance of the object that is in the collection.

       @return
       TRUE if the object was in the collection.
     */

};


#ifdef PHAS_TEMPLATES

/**This template class maps the PAbstractDictionary to a specific key and data
   types. The functions in this class primarily do all the appropriate casting
   of types.

   Note that if templates are not used the #PDECLARE_DICTIONARY# macro
   will simulate the template instantiation.
 */
template <class K, class D> class PDictionary : public PAbstractDictionary
{
  PCLASSINFO(PDictionary, PAbstractDictionary);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, dictionary.

       Note that by default, objects placed into the dictionary will be
       deleted when removed or when all references to the dictionary are
       destroyed.
     */
    PDictionary()
      : PAbstractDictionary() { }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the dictionary. Note that all objects in
       the array are also cloned, so this will make a complete copy of the
       dictionary.
     */
    virtual PObject * Clone() const
      { return PNEW PDictionary(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Get the object contained in the dictionary at the #key#
       position. The hash table is used to locate the data quickly via the
       hash function provided by the #key#.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to the object indexed by the key.
     */
    D & operator[](
      const K & key   ///< Key to look for in the dictionary.
    ) const
      { return (D &)GetRefAt(key); }

    /**Determine if the value of the object is contained in the hash table. The
       object values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       TRUE if the object value is in the dictionary.
     */
    BOOL Contains(
      const K & key   ///< Key to look for in the dictionary.
    ) const { return AbstractContains(key); }

    /**Remove an object at the specified key. The returned pointer is then
       removed using the #SetAt()# function to set that key value to
       NULL. If the #AllowDeleteObjects# option is set then the
       object is also deleted.

       @return
       pointer to the object being removed, or NULL if the key was not 
       present in the dictionary. If the dictionary is set to delete objects
       upon removal, the value -1 is returned if the key existed prior to removal
       rather than returning an illegal pointer
     */
    virtual D * RemoveAt(
      const K & key   ///< Key for position in dictionary to get object.
    ) {
        D * obj = GetAt(key); AbstractSetAt(key, NULL); 
        return reference->deleteObjects ? (obj ? (D *)-1 : NULL) : obj;
      }

    /**Add a new object to the collection. If the objects value is already in
       the dictionary then the object is overrides the previous value. If the
       AllowDeleteObjects option is set then the old object is also deleted.

       The object is placed in the an ordinal position dependent on the keys
       hash function. Subsequent searches use the has function to speed access
       to the data item.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL SetAt(
      const K & key,  // Key for position in dictionary to add object.
      D * obj         // New object to put into the dictionary.
    ) { return AbstractSetAt(key, obj); }

    /**Get the object at the specified key position. If the key was not in the
       collection then NULL is returned.

       @return
       pointer to object at the specified key.
     */
    virtual D * GetAt(
      const K & key   // Key for position in dictionary to get object.
    ) const { return (D *)AbstractGetAt(key); }

    /**Get the key in the dictionary at the ordinal index position.
    
       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to key at the index position.
     */
    const K & GetKeyAt(
      PINDEX index  ///< Ordinal position in dictionary for key.
    ) const
      { return (const K &)AbstractGetKeyAt(index); }

    /**Get the data in the dictionary at the ordinal index position.
    
       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to data at the index position.
     */
    D & GetDataAt(
      PINDEX index  ///< Ordinal position in dictionary for data.
    ) const
      { return (D &)AbstractGetDataAt(index); }
  //@}

  protected:
    PDictionary(int dummy, const PDictionary * c)
      : PAbstractDictionary(dummy, c) { }
};


/**Declare a dictionary class.
   This macro is used to declare a descendent of PAbstractDictionary class,
   customised for a particular key type {\bf K} and data object type {\bf D}.
   This macro closes the class declaration off so no additional members can
   be added.

   If the compilation is using templates then this macro produces a typedef
   of the #PDictionary# template class.

   See the #PDictionary# class and #PDECLARE_DICTIONARY# macro for
   more information.
 */
#define PDICTIONARY(cls, K, D) typedef PDictionary<K, D> cls


/**Begin declaration of dictionary class.
   This macro is used to declare a descendent of PAbstractDictionary class,
   customised for a particular key type {\bf K} and data object type {\bf D}.

   If the compilation is using templates then this macro produces a descendent
   of the #PDictionary# template class. If templates are not being used
   then the macro defines a set of inline functions to do all casting of types.
   The resultant classes have an identical set of functions in either case.

   See the #PDictionary# and #PAbstractDictionary# classes for more
   information.
 */
#define PDECLARE_DICTIONARY(cls, K, D) \
  PDICTIONARY(cls##_PTemplate, K, D); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


/**This template class maps the #PAbstractDictionary# to a specific key
   type and a #POrdinalKey# data type. The functions in this class
   primarily do all the appropriate casting of types.

   Note that if templates are not used the #PDECLARE_ORDINAL_DICTIONARY#
   macro will simulate the template instantiation.
 */
template <class K> class POrdinalDictionary : public PAbstractDictionary
{
  PCLASSINFO(POrdinalDictionary, PAbstractDictionary);

  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, dictionary.

       Note that by default, objects placed into the dictionary will be
       deleted when removed or when all references to the dictionary are
       destroyed.
     */
    POrdinalDictionary()
      : PAbstractDictionary() { }
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Make a complete duplicate of the dictionary. Note that all objects in
       the array are also cloned, so this will make a complete copy of the
       dictionary.
     */
    virtual PObject * Clone() const
      { return PNEW POrdinalDictionary(0, this); }
  //@}

  /**@name New functions for class */
  //@{
    /**Get the object contained in the dictionary at the #key#
       position. The hash table is used to locate the data quickly via the
       hash function provided by the key.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to the object indexed by the key.
     */
    PINDEX operator[](
      const K & key   // Key to look for in the dictionary.
    ) const
      { return (POrdinalKey &)GetRefAt(key); }

    /**Determine if the value of the object is contained in the hash table. The
       object values are compared, not the pointers.  So the objects in the
       collection must correctly implement the #PObject::Compare()#
       function. The hash table is used to locate the entry.

       @return
       TRUE if the object value is in the dictionary.
     */
    BOOL Contains(
      const K & key   ///< Key to look for in the dictionary.
    ) const { return AbstractContains(key); }

    virtual POrdinalKey * GetAt(
      const K & key   ///< Key for position in dictionary to get object.
    ) const { return (POrdinalKey *)AbstractGetAt(key); }
    /* Get the object at the specified key position. If the key was not in the
       collection then NULL is returned.

       @return
       pointer to object at the specified key.
     */

    /**Set the data at the specified ordinal index position in the dictionary.

       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       @return
       TRUE if the new object could be placed into the dictionary.
     */
    virtual BOOL SetDataAt(
      PINDEX index,   ///< Ordinal index in the dictionary.
      PINDEX ordinal  ///< New ordinal value to put into the dictionary.
      ) { return PAbstractDictionary::SetDataAt(index, PNEW POrdinalKey(ordinal)); }

    /**Add a new object to the collection. If the objects value is already in
       the dictionary then the object is overrides the previous value. If the
       AllowDeleteObjects option is set then the old object is also deleted.

       The object is placed in the an ordinal position dependent on the keys
       hash function. Subsequent searches use the has function to speed access
       to the data item.

       @return
       TRUE if the object was successfully added.
     */
    virtual BOOL SetAt(
      const K & key,  ///< Key for position in dictionary to add object.
      PINDEX ordinal  ///< New ordinal value to put into the dictionary.
    ) { return AbstractSetAt(key, PNEW POrdinalKey(ordinal)); }

    /**Remove an object at the specified key. The returned pointer is then
       removed using the #SetAt()# function to set that key value to
       NULL. If the #AllowDeleteObjects# option is set then the
       object is also deleted.

       @return
       pointer to the object being removed, or NULL if it was deleted.
     */
    virtual PINDEX RemoveAt(
      const K & key   ///< Key for position in dictionary to get object.
    ) { PINDEX ord = *GetAt(key); AbstractSetAt(key, NULL); return ord; }

    /**Get the key in the dictionary at the ordinal index position.
    
       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to key at the index position.
     */
    const K & GetKeyAt(
      PINDEX index  ///< Ordinal position in dictionary for key.
    ) const
      { return (const K &)AbstractGetKeyAt(index); }

    /**Get the data in the dictionary at the ordinal index position.
    
       The ordinal position in the dictionary is determined by the hash values
       of the keys and the order of insertion.

       The last key/data pair is remembered by the class so that subseqent
       access is very fast.

       @return
       reference to data at the index position.
     */
    PINDEX GetDataAt(
      PINDEX index  ///< Ordinal position in dictionary for data.
    ) const
      { return (POrdinalKey &)AbstractGetDataAt(index); }
  //@}

  protected:
    POrdinalDictionary(int dummy, const POrdinalDictionary * c)
      : PAbstractDictionary(dummy, c) { }
};


/**Declare an ordinal dictionary class.
   This macro is used to declare a descendent of PAbstractDictionary class,
   customised for a particular key type {\bf K} and data object type of
   #POrdinalKey#. This macro closes the class declaration off so no
   additional members can be added.

   If the compilation is using templates then this macro produces a typedef
   of the #POrdinalDictionary# template class.

   See the #POrdinalDictionary# class and
   #PDECLARE_ORDINAL_DICTIONARY# macro for more information.
 */
#define PORDINAL_DICTIONARY(cls, K) typedef POrdinalDictionary<K> cls


/**Begin declaration of an ordinal dictionary class.
   This macro is used to declare a descendent of PAbstractList class,
   customised for a particular key type {\bf K} and data object type of
   #POrdinalKey#.

   If the compilation is using templates then this macro produces a descendent
   of the #POrdinalDictionary# template class. If templates are not being
   used then the macro defines a set of inline functions to do all casting of
   types. The resultant classes have an identical set of functions in either
   case.

   See the #POrdinalDictionary# and #PAbstractDictionary# classes
   for more information.
 */
#define PDECLARE_ORDINAL_DICTIONARY(cls, K) \
  PORDINAL_DICTIONARY(cls##_PTemplate, K); \
  PDECLARE_CLASS(cls, POrdinalDictionary<K>) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#else // PHAS_TEMPLATES


#define PDICTIONARY(cls, K, D) \
  class cls : public PAbstractDictionary { \
  PCLASSINFO(cls, PAbstractDictionary); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractDictionary(dummy, c) { } \
  public: \
    cls() \
      : PAbstractDictionary() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    D & operator[](const K & key) const \
      { return (D &)GetRefAt(key); } \
    virtual BOOL Contains(const K & key) const \
      { return AbstractContains(key); } \
    virtual D * RemoveAt(const K & key) \
      { D * obj = GetAt(key); AbstractSetAt(key, NULL); return obj; } \
    virtual BOOL SetAt(const K & key, D * obj) \
      { return AbstractSetAt(key, obj); } \
    virtual D * GetAt(const K & key) const \
      { return (D *)AbstractGetAt(key); } \
    const K & GetKeyAt(PINDEX index) const \
      { return (const K &)AbstractGetKeyAt(index); } \
    D & GetDataAt(PINDEX index) const \
      { return (D &)AbstractGetDataAt(index); } \
  }

#define PDECLARE_DICTIONARY(cls, K, D) \
  PDICTIONARY(cls##_PTemplate, K, D); \
  PDECLARE_CLASS(cls, cls##_PTemplate) \
  protected: \
    cls(int dummy, const cls * c) \
      : cls##_PTemplate(dummy, c) { } \
  public: \
    cls() \
      : cls##_PTemplate() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \


#define PORDINAL_DICTIONARY(cls, K) \
  class cls : public PAbstractDictionary { \
  PCLASSINFO(cls, PAbstractDictionary); \
  protected: \
    inline cls(int dummy, const cls * c) \
      : PAbstractDictionary(dummy, c) { } \
  public: \
    inline cls() \
      : PAbstractDictionary() { } \
    virtual PObject * Clone() const \
      { return PNEW cls(0, this); } \
    inline PINDEX operator[](const K & key) const \
      { return (POrdinalKey &)GetRefAt(key); } \
    virtual BOOL Contains(const K & key) const \
      { return AbstractContains(key); } \
    virtual POrdinalKey * GetAt(const K & key) const \
      { return (POrdinalKey *)AbstractGetAt(key); } \
    virtual BOOL SetDataAt(PINDEX index, PINDEX ordinal) \
      { return PAbstractDictionary::SetDataAt(index, PNEW POrdinalKey(ordinal)); } \
    virtual BOOL SetAt(const K & key, PINDEX ordinal) \
      { return AbstractSetAt(key, PNEW POrdinalKey(ordinal)); } \
    virtual PINDEX RemoveAt(const K & key) \
      { PINDEX ord = *GetAt(key); AbstractSetAt(key, NULL); return ord; } \
    inline const K & GetKeyAt(PINDEX index) const \
      { return (const K &)AbstractGetKeyAt(index); } \
    inline PINDEX GetDataAt(PINDEX index) const \
      { return (POrdinalKey &)AbstractGetDataAt(index); } \
  }

#define PDECLARE_ORDINAL_DICTIONARY(cls, K) \
  PORDINAL_DICTIONARY(cls##_PTemplate, K); \
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

#endif // #ifndef __DICT_H__

// End Of File ///////////////////////////////////////////////////////////////
