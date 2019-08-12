/*==============================================================================

                            ds_ASBuffer.cpp

GENERAL DESCRIPTION
  A buffer class with utility functions for parsing raw bytes.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/03/15    ml     Remove memory allocation on default constructor
05/20/15    ml     Use memory from modem heap
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASBuffer.h"

#include "ds_appsrv_mem.h"
#include <stringl/stringl.h>

#include <cctype> // isspace


// temp
#include "ds_ASString.h"

#define DEFAULT_BUFFER_SIZE 256

// static const uint8 EMPTY_STRING[] = "";



ASBuffer::ASBuffer()
: buffer(NULL), buffer_size(0), buffer_capacity(DEFAULT_BUFFER_SIZE), error_status(false)
{ }



ASBuffer::ASBuffer(uint32 capacity)
: buffer(NULL), buffer_size(0), buffer_capacity(capacity), error_status(false)
{ }



ASBuffer::ASBuffer(ASBuffer &buf)
: buffer(NULL), buffer_size(0), buffer_capacity(buf.buffer_capacity), error_status(false)
{
  init();
  append(buf);
}



ASBuffer::ASBuffer(const ASBuffer &buf)
: buffer(NULL), buffer_size(0), buffer_capacity(buf.buffer_capacity), error_status(false)
{
  init();
  append(buf);
}



ASBuffer::ASBuffer(const uint8* buf, uint32 buf_size)
: buffer(NULL), buffer_size(0), buffer_capacity(buf_size), error_status(false)
{
  init();
  append(buf, buf_size);
}



ASBuffer::~ASBuffer()
{
  if(NULL != buffer)
  {
    ds_appsrv_free(buffer);
  }
}



ASBuffer& ASBuffer::operator=(const ASBuffer &rhs)
{
  clear();
  append(rhs);
  return *this;
}




void ASBuffer::init()
{
  if(0 == buffer_capacity)
  {
    error_status = true;
  }
  else if(NULL == buffer)
  {
    buffer = (uint8*)ds_appsrv_alloc(sizeof(uint8), buffer_capacity+1);
    error_status = (NULL == buffer);
  }
}



// temp name
#define DO_NOT_DOUBLE_IF_LARGER 2147483647
bool ASBuffer::increment_buffer_capacity(const uint32 req_size)
{
  uint32 new_capacity = buffer_capacity;
  uint8* tmp          = NULL;

  while(req_size > new_capacity)
  {
    // will cause overflow if doubled
    if(DO_NOT_DOUBLE_IF_LARGER < new_capacity)
      return false;

    new_capacity *= 2;
  }

  tmp = (uint8*)ds_appsrv_alloc(sizeof(uint8), new_capacity+1);
  if(NULL == tmp)
    return false;

  memset(tmp, 0, new_capacity+1);
  memscpy(tmp, new_capacity, buffer, buffer_size);
  ds_appsrv_free(buffer);
  buffer          = tmp;
  buffer_capacity = new_capacity;

  return true;
}



uint8* ASBuffer::non_const_content()
{
  return buffer;
}


const uint8* ASBuffer::content() const
{
  return buffer;
}


const char* ASBuffer::c_str() const
{
  if(NULL == buffer)
    return "";
  else
    return (const char*)buffer;
}



uint32 ASBuffer::size() const
{
  return buffer_size;
}



bool ASBuffer::empty() const
{
  return (0 == buffer_size);
}



bool ASBuffer::error() const
{
  return error_status;
}



void ASBuffer::clear()
{
  if(NULL == buffer)
  {
    init();
  }
  else
  {
    memset(buffer, 0, buffer_capacity+1);
  }
  buffer_size = 0;
}



bool ASBuffer::append(const uint8* append_buffer, const uint32 append_size)
{
  if(NULL == append_buffer || 0 == append_size)
    return true;

  if(NULL == buffer)
    init();

  if(error_status) // do nothing if in error state
    return false;

  uint32 new_size = buffer_size + append_size;
  if(new_size > buffer_capacity)
  {
    if(!increment_buffer_capacity(new_size))
    {
      error_status = true;
      return false;
    }
  }

  memscpy(
          buffer + buffer_size,
          buffer_capacity - buffer_size,
          append_buffer,
          append_size
          );
  buffer_size = new_size;

  return true;
}



bool ASBuffer::append(const char* append_buffer)
{
  if(NULL == append_buffer)
    return false;
  else
    return append((uint8*)append_buffer, strlen(append_buffer));
}


bool ASBuffer::append(const char append_buffer)
{
  char buffer[1];
  buffer[0] = append_buffer;

  return append((uint8*)buffer, 1);
}


bool ASBuffer::append(const char* append_buffer, const uint32 size)
{
  if(NULL == append_buffer)
    return false;
  else
    return append((uint8*)append_buffer, size);
}



bool ASBuffer::append(const ASBuffer& append_buffer)
{
  if(append_buffer.empty())
    return false;
  else
    return append(append_buffer.content(), append_buffer.size());
}



bool ASBuffer::append(const ASString& append_buffer)
{
  if(append_buffer.empty())
    return false;
  else
    return append((uint8*)append_buffer.c_str(), append_buffer.size());
}


