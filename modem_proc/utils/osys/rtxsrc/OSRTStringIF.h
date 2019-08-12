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
 * @file OSRTStringIF.h
 * C++ string class interface.  This defines an interface to allow
 * different types of string derived classes to be implemented.  Currently,
 * implementations include a standard string class (OSRTString) which
 * deep-copies all values using new/delete, and a fast string class
 * (OSRTFastString) that just copies pointers (i.e does no memory
 * management).
 *
 * These classes can be used to hold standard ASCII or UTF-8 strings.
 */
#ifndef _OSRTSTRINGIF_H_
#define _OSRTSTRINGIF_H_

#include "rtxsrc/rtxCommon.h"
#include "rtxsrc/rtxPrint.h"

/**
 * C++ string class interface. This defines an interface to allow
 * different types of string derived classes to be implemented.  Currently,
 * implementations include a standard string class (OSRTString) which
 * deep-copies all values using new/delete, and a fast string class
 * (OSRTFastString) that just copies pointers (i.e does no memory
 * management).
 */
class EXTRTCLASS OSRTStringIF {
 protected:
   /**
    * The default constructor creates an empty string.
    */
   OSRTStringIF () {}

   /**
    * This constructor initializes the string to contain the given
    * standard ASCII string value.
    *
    * @param      - Null-terminated C string value
    */
   OSRTStringIF (const char*) {}

   /**
    * This constructor initializes the string to contain the given
    * UTF-8 string value.
    *
    * @param       - Null-terminated C string value
    */
   OSRTStringIF (const OSUTF8CHAR*) {}

 public:
   /**
    * The destructor frees string memory using the standard 'delete'
    * operator.
    */
   virtual ~OSRTStringIF() {}

   /**
    * This method creates a copy of the given string object.
    */
   virtual OSRTStringIF* clone () = 0;

   /**
    * This method returns the pointer to UTF-8 null terminated string
    * as a standard ASCII string.
    */
   virtual const char* getValue () const = 0;

   /**
    * This method returns the pointer to UTF-8 null terminated string
    * as a UTF-8 string.
    */
   virtual const OSUTF8CHAR* getUTF8Value () const = 0;

   /**
    * This method prints the string value to standard output.
    *
    * @param name - Name of generated string variable.
    */
   virtual void print (const char* name) = 0;

   /**
    * This method sets the string value to the given string.
    *
    * @param str - C null-terminated string.
    */
   virtual void setValue (const char* str) = 0;

   /**
    * This method sets the string value to the given UTF-8 string value.
    *
    * @param utf8str - C null-terminated UTF-8 string.
    */
   virtual void setValue (const OSUTF8CHAR* utf8str) = 0;

} ;

#endif
