/*==============================================================================

                              ds_ASString.h

GENERAL DESCRIPTION
  A string class with utility functions for parsing AS.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_STRING_H
#define DS_AS_STRING_H

#include "comdef.h"

/*==============================================================================
CLASS ASString

DESCRIPTION
  A string class with utility functions for parsing AS.
==============================================================================*/
class ASString
{
public:
  /*===========================================================================
  FUNCTION ASString CONSTRUCTOR

  DESCRIPTION
    Creates a new ASString. Default constructor will create an empty string.
    The other constructors will make a copy of the given string.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASString();
  ASString(const char* src);
  ASString(const char* src, uint32 len);

  // copy constructor
  ASString(ASString& src);
  ASString(const ASString& src);

  ~ASString();


  ASString& operator=(const ASString& rhs);
  ASString& operator=(const char* rhs);
  char      operator[](const int index) const;
  char&     operator[](const int index);


  /*============================================================================
  FUNCTION ASString::c_str

  DESCRIPTION
    Returns the raw c-string

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  const char* c_str() const;

  /*============================================================================
  FUNCTION ASString::size
  FUNCTION ASString::length

  DESCRIPTION
    Returns the length of the string

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  uint32 size() const;
  uint32 length() const;

  /*============================================================================
  FUNCTION ASString::empty

  DESCRIPTION
    Returns true if the string is empty or NULL

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool empty() const;

  /*============================================================================
  FUNCTION ASString::remove_trailing_spaces

  DESCRIPTION
    Removes all whiltespace, including tabs and newlines, at the end of the string.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void remove_trailing_spaces();

  /*============================================================================
  FUNCTION ASString::resolve_xml_escapes

  DESCRIPTION
    Replaces XML escape strings with the corresponding character.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool resolve_xml_escapes();

  /*============================================================================
  FUNCTION ASString::to_lower

  DESCRIPTION
    Converts all characters in the string to lower case.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void to_lower();

  /*============================================================================
  FUNCTION ASString::limit_cmp

  DESCRIPTION
    Partial string compare up to len. Returns true if same, else false.

  PARAMETERS
    [In] cstr - The string to compare
    [In] len  - The number of characters to compare.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool limit_cmp(const char* cstr, const uint32 len) const;

  /*============================================================================
  FUNCTION ASString::append

  DESCRIPTION
    Appends the given string to the string it holds.

  PARAMETERS
    [In] append_str - The string to append
    [In] len        - The number of characters to append

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void append(const char* append_str);
  void append(const char* append_str, const uint32 len);
  void append(const ASString& append_str);

protected:
  /*============================================================================
  FUNCTION ASString::copy_string

  DESCRIPTION
    Partial string compare up to len. Returns true if same, else false.

  PARAMETERS
    [In] src - The string to copy
    [In] len - The number of characters to copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void copy_string(const char* src, uint32 len);

  char* str; // C-string object. Memory dynamically allocated as necessary
};



/*============================================================================
FUNCTION operator==
FUNCTION operator!=
FUNCTION operator<
FUNCTION operator>

DESCRIPTION
  Compares the two strings and returns true/false based on the operator. The
  comparison will be case insensitive.

PARAMETERS
  [In] lhs - The string to compare
  [In] rhs - The other string to compare

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
bool operator== (const ASString& lhs, const ASString& rhs);
bool operator!= (const ASString& lhs, const ASString& rhs);

bool operator== (const ASString& lhs, const char* rhs);
bool operator!= (const ASString& lhs, const char* rhs);

bool operator< (const ASString& lhs, const ASString& rhs);
bool operator> (const ASString& lhs, const ASString& rhs);

#endif /* DS_AS_STRING_H */
