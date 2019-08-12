/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file OSRTUTF8String.h
 * C++ UTF-8 string class definition.
 */
#ifndef _OSRTUTF8STRING_H_
#define _OSRTUTF8STRING_H_

#include "rtxsrc/OSRTBaseType.h"
#include "rtxsrc/rtxPrint.h"
#include "rtxsrc/rtxUTF8.h"

/**
 * UTF-8 string. This is the base class for generated C++ data type
 * classes for XSD string types (string, token, NMTOKEN, etc.).
 */
class EXTERNRT OSRTUTF8String : public OSRTBaseType {
   const OSUTF8CHAR* mValue;

 public:
   /**
    * The default constructor creates an empty string.
    */
   OSRTUTF8String();

   /**
    * This constructor initializes the string to contain the given
    * character string value.
    *
    * @param strval       - String value
    */
   OSRTUTF8String (const char* strval);

   /**
    * This constructor initializes the string to contain the given
    * UTF-8 character string value.
    *
    * @param strval       - String value
    */
   OSRTUTF8String (const OSUTF8CHAR* strval);

   /**
    * Copy constructor.
    *
    * @param str          - C++ XML string class.
    */
   OSRTUTF8String (const OSRTUTF8String& str);

   /**
    * The destructor frees string memory if the memory ownership flag
    * is set.
    */
   virtual ~OSRTUTF8String();

   /**
    * Clone method. Creates a copied instance and returns pointer to
    * OSRTBaseType.
    */
   OSRTBaseType* clone () const { return new OSRTUTF8String (*this); }

   /**
    * This method copies the given string value to the internal
    * string storage variable.  A deep-copy of the given value is done;
    * the class will delete this memory when the object is deleted.
    *
    * @param str - C null-terminated string.
    */
   void copyValue (const char* str);

   /**
    * This method returns the pointer to C null terminated string.
    */
   inline const char* c_str () const {
      return mValue;
   }

   /**
    * This method returns the pointer to UTF-8 null terminated string.
    */
   inline const char* getValue () const {
      return mValue;
   }

   /**
    * This method prints the string value to standard output.
    *
    * @param name - Name of generated string variable.
    */
   inline void print (const char* name) { rtxPrintCharStr (name, mValue); }

   /**
    * This method sets the string value to the given string.
    * A deep-copy of the given value is not done; the pointer is
    * stored directly in the class member variable.
    *
    * @param str - C null-terminated string.
    */
   void setValue (const char* str);

   /**
    * Assignment operator.
    */
   OSRTUTF8String& operator= (const OSRTUTF8String& original);
} ;

#endif
