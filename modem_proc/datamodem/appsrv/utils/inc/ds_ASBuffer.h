/*==============================================================================

                              ds_ASBuffer.h

GENERAL DESCRIPTION
  A buffer class with utility functions for parsing raw bytes.

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

#ifndef __DS_AS_BUFFER_H__
#define __DS_AS_BUFFER_H__

#include "comdef.h"
class ASString;


/*==============================================================================
CLASS ASBuffer

DESCRIPTION
  A buffer class with utility functions for parsing raw bytes.
==============================================================================*/
class ASBuffer
{
public:
  /*===========================================================================
  FUNCTION ASBuffer CONSTRUCTOR

  DESCRIPTION
    Creates a new ASBuffer.

  DEPENDENCIES
    capacity - The initial buffer capacity to set.

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASBuffer();
  ASBuffer(uint32 capacity);

  ASBuffer(ASBuffer &buf);
  ASBuffer(const ASBuffer &buf);
  ASBuffer(const uint8* buf, uint32 capacity);

  ~ASBuffer();


  ASBuffer& operator=(const ASBuffer &rhs);

  /*============================================================================
  FUNCTION ASBuffer::content

  DESCRIPTION
    Returns a pointer to the buffer content

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  const uint8* content() const;



  /*============================================================================
  FUNCTION ASBuffer::non_const_content

  DESCRIPTION
    Returns a pointer to the buffer content. Use this function ONLY if you need
    a non-const version and it is guaranteed that the returned pointer will
    never be modified.

    Note: Workaround function for MD5 auth-int. Otherwise shouldn't be used.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  uint8* non_const_content();



  /*============================================================================
  FUNCTION ASBuffer::c_str

  DESCRIPTION
    Returns a pointer to the buffer content. This assumes the buffer content is
    in c-string format

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  const char* c_str() const;



  /*============================================================================
  FUNCTION ASBuffer::size

  DESCRIPTION
    Returns the size of the buffer

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  uint32 size() const;



  /*============================================================================
  FUNCTION ASBuffer::empty

  DESCRIPTION
    Returns true if the buffer is empty

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool empty() const;



  /*============================================================================
  FUNCTION ASBuffer::error

  DESCRIPTION
    Returns true if the buffer is in error status due to internal memory
    allocation failure.

    This can happen when
      - Constructor fails to allocate memory for the new buffer
      - Append requires to resize the buffer but fails

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool error() const;



  /*============================================================================
  FUNCTION ASBuffer::append

  DESCRIPTION
    Appends the given data to the buffer. Returns true if append is successful,
    false if append fails.

  PARAMETERS
    [In] append_buffer - The data to append
    [In] size          - The number of bytes to append

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool append(const uint8* append_buffer, const uint32 size);
  bool append(const char append_buffer);
  bool append(const char* append_buffer);
  bool append(const char* append_buffer, const uint32 size);
  bool append(const ASBuffer& append_buffer);
  bool append(const ASString& append_buffer); // temp


  /*============================================================================
  FUNCTION ASBuffer::clear

  DESCRIPTION
    Returns true if the buffer is in error status due to internal memory
    allocation failure.

    This can happen when
      - Constructor fails to allocate memory for the new buffer
      - Append requires to resize the buffer but fails

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void clear();



private:
  /*============================================================================
  FUNCTION ASBuffer::init

  DESCRIPTION
    Initialize buffer

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  void init();
  bool increment_buffer_capacity(const uint32 req_size);


  uint8* buffer; // Buffer object. Memory dynamically allocated as necessary
  uint32 buffer_size;
  uint32 buffer_capacity;
  bool   error_status; // true if error, else false
};



#endif /* __DS_AS_BUFFER_H__ */
