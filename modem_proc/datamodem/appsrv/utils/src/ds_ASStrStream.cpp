/*==============================================================================

                        ds_ASSStream.h

GENERAL DESCRIPTION
  String stream

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/26/16    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASStrStream.h"

#include "ds_ASUtility.h"
#include "ds_appsrv_mem.h"
#include <stringl/stringl.h>
#include <cctype>
#include <cstdlib>


namespace Appsrv
{
namespace Utils
{

const uint32 AS_STR_BUFFER_DEFAULT_SIZE = 32;

/*==============================================================================
                                ASStringBuffer
==============================================================================*/
ASStringBuffer::ASStringBuffer()
: internal_buffer(NULL),
  current_size(0),
  buffer_capacity(AS_STR_BUFFER_DEFAULT_SIZE)
{
  init(AS_STR_BUFFER_DEFAULT_SIZE);
}


ASStringBuffer::ASStringBuffer(const ASString& str)
: internal_buffer(NULL),
  current_size(0),
  buffer_capacity(AS_STR_BUFFER_DEFAULT_SIZE)
{
  init(str.size());
  copy(str.c_str(), str.size());
}


ASStringBuffer::ASStringBuffer(const char* str, uint32 len)
: internal_buffer(NULL),
  current_size(0),
  buffer_capacity(AS_STR_BUFFER_DEFAULT_SIZE)
{
  init(len);
  copy(str, len);
}


ASStringBuffer::~ASStringBuffer()
{
  if(NULL != internal_buffer)
  {
    ds_appsrv_free(internal_buffer);
  }
}


const char* ASStringBuffer::str() const
{
  return internal_buffer;
}


void ASStringBuffer::str(const ASString& s)
{
  copy(s.c_str(), s.size());
}


bool ASStringBuffer::read(ASString& buf, uint32 len)
{
  if(!validate_read())
    return false;

  uint32 read_size = get_min(len, current_size - stream_position);
  buf = ASString(internal_buffer + stream_position, read_size);
  stream_position += read_size;

  return true;
}


bool ASStringBuffer::read(ASBuffer& buf, uint32 len)
{
  if(!validate_read())
    return false;

  uint32 read_size = get_min(len, current_size - stream_position);
  buf = ASBuffer((const uint8*)internal_buffer + stream_position, read_size);
  stream_position += read_size;

  return true;
}


char ASStringBuffer::peek()
{
  if(current_size <= stream_position)
    return '\0';

  return *(internal_buffer + stream_position);
}


bool ASStringBuffer::write(const char* buf, uint32 len)
{
  return append(buf, len);
}


bool ASStringBuffer::write(const uint8* buf, uint32 len)
{
  return append((const char*)buf, len);
}


bool ASStringBuffer::validate_read()
{
  if(current_size <= stream_position)
  {
    end_of_file = true;
    return false;
  }

  return true;
}


void ASStringBuffer::clear()
{
  stream_position = 0;
  current_size = 0;

  if(NULL != internal_buffer)
  {
    memset(internal_buffer, 0, buffer_capacity);
  }
}



void ASStringBuffer::init(uint32 len)
{
  if(buffer_capacity < len)
    buffer_capacity = len;

  internal_buffer = (char*)ds_appsrv_alloc(sizeof(char), buffer_capacity+1);
}



bool ASStringBuffer::copy(const char* buf, uint32 len)
{
  clear();

  if(NULL == internal_buffer)
  {
    init(len);
    if(NULL == internal_buffer) // memory alloc failed
    {
      return false;
    }
  }

  if(buffer_capacity < len && !resize(len))
  {
    return false;
  }

  memscpy(internal_buffer, buffer_capacity, buf, len);
  current_size = len;
  return true;
}



bool ASStringBuffer::append(const char* buf, uint32 len)
{
  if(NULL == internal_buffer)
  {
    init(len);
  }

  uint32 new_size = current_size + len;

  if(buffer_capacity < new_size && !resize(new_size))
  {
    return false;
  }

  memscpy(internal_buffer + current_size, buffer_capacity, buf, len);
  current_size += len;
  return true;
}



// temp name
const uint32 DO_NOT_DOUBLE_IF_LARGER = 2147483647;
bool ASStringBuffer::resize(const uint32 req_size)
{
  uint32 new_capacity = buffer_capacity;
  char* tmp          = NULL;

  while(req_size > new_capacity)
  {
    // will cause overflow if doubled
    if(DO_NOT_DOUBLE_IF_LARGER < new_capacity)
      return false;

    new_capacity *= 2;
  }

  tmp = (char*)ds_appsrv_alloc(sizeof(char), new_capacity+1);
  if(NULL == tmp)
    return false;

  memset(tmp, 0, new_capacity+1);
  memscpy(tmp, new_capacity, internal_buffer, buffer_capacity);
  ds_appsrv_free(internal_buffer);
  internal_buffer = tmp;
  buffer_capacity = new_capacity;

  return true;
}

/*==============================================================================
                                ASIStringStream
==============================================================================*/
ASIStringStream::ASIStringStream()
{
  init(&istringstream);
}


ASIStringStream::ASIStringStream(const ASString& str)
: istringstream(str)
{
  init(&istringstream);
}


ASIStringStream::ASIStringStream(const char* str, uint32 len)
: istringstream(str, len)
{
  init(&istringstream);
}


ASIStringStream::~ASIStringStream()
{ }


const char* ASIStringStream::str() const
{
  return istringstream.str();
}


uint32 ASIStringStream::size() const
{
  return istringstream.size();
}


void ASIStringStream::str(const ASString& s)
{
  istringstream.str(s);
}




/*==============================================================================
                                ASOStringStream
==============================================================================*/
ASOStringStream::ASOStringStream()
{
  init(&ostringstream);
}


ASOStringStream::ASOStringStream(const ASString& str)
: ostringstream(str)
{
  init(&ostringstream);
}


ASOStringStream::ASOStringStream(const char* str, uint32 len)
: ostringstream(str, len)
{
  init(&ostringstream);
}


ASOStringStream::~ASOStringStream()
{ }


const char* ASOStringStream::str() const
{
  return ostringstream.str();
}

uint32 ASOStringStream::size() const
{
  return ostringstream.size();
}


void ASOStringStream::str(const ASString& s)
{
  ostringstream.str(s);
}




} /* namespace Utils */
} /* namespace Appsrv */

