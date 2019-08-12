/*==============================================================================

                        ds_ASStringStream.h

GENERAL DESCRIPTION
  Utility class that functions like an input string stream.

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
04/21/21    ml     Created file/Initial version.
==============================================================================*/

#ifndef __AS_STRING_STREAM_H__
#define __AS_STRING_STREAM_H__

class ASString;

/*==============================================================================
CLASS ASStringStream

DESCRIPTION
  Utility class for AS decoder that functions like an input stream.
==============================================================================*/
class ASStringStream
{
public:
  /*===========================================================================
  FUNCTION ASStringStream CONSTRUCTOR

  DESCRIPTION
    Create a new ASStringStream object

  DEPENDENCIES
    If constructor by char*, it MUST be a proper c-string ending with a null
    character at end

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASStringStream() { }
  ASStringStream(ASString& str);
  ASStringStream(const char* str);
  ASStringStream(ASString& str, bool skip_space);
  ASStringStream(const char* str, bool skip_space);

  /*===========================================================================
  FUNCTION ASStringStream::next

  DESCRIPTION
    Increase the stream pointer by one

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool next_ptr();

  /*===========================================================================
  FUNCTION ASStringStream::back

  DESCRIPTION
    Decrease the stream pointer by one

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void back();

  /*===========================================================================
  FUNCTION ASStringStream::peek

  DESCRIPTION
    Get the next char in the stream without increasing the stream pointer

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  char peek();

  /*===========================================================================
  FUNCTION ASStringStream::get_next

  DESCRIPTION
    Get the entire string from the current stream pointer until the given delim
    or end of stream. Returns true if the stream has reached the end.

  PARAMETERS
    [Out] dst   - The string to store the string from the stream
    [In]  delim - The delimeter to stop the stream at

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool get_next(ASString& dst);
  bool get_next(ASString& dst, const char delim);
  bool get_next(ASString& dst, const char* delim);

  /*===========================================================================
  FUNCTION ASStringStream::step_back

  DESCRIPTION
    Return to the previous location before get_next() was called

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void step_back();

  /*===========================================================================
  FUNCTION ASStringStream::reset

  DESCRIPTION
    Reset the stream

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void reset();

  /*===========================================================================
  FUNCTION ASStringStream::good

  DESCRIPTION
    Returns true if the stream hasn't reached end, else false

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool good() const;

  /*===========================================================================
  FUNCTION ASStringStream::eof

  DESCRIPTION
    Returns true if the stream reached end, else false

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool eof() const;

private:
  const char* iss;          // The string stream
  const char* start;        // The beginning of the stream
  const char* prev;         // Previous location
  bool        end_of_file;  // Indicator for reaching end of stream
  bool        skip_space;   // True if the string stream should ignore spaces before parsing
};

#endif /* __AS_STRING_STREAM_H__ */
