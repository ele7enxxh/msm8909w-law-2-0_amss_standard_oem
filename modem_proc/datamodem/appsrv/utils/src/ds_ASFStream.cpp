/*==============================================================================

                        ds_ASFStream.cpp

GENERAL DESCRIPTION
  Timer extension

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
11/07/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASFStream.h"

#include "ds_ASString.h"
#include "ds_ASUtility.h"
#include <stringl/stringl.h>
#include <cctype>
#include <cstdlib>




namespace Appsrv
{
namespace Utils
{

/*==============================================================================
                                ASFileBuffer
==============================================================================*/
ASFileBuffer::ASFileBuffer()
: filedes(-1), file_size(0)
{ }


ASFileBuffer::ASFileBuffer(const char* path, int oflag, int mode)
: filedes(-1), file_size(0)
{
  open_file(path, oflag, mode);
}

ASFileBuffer::~ASFileBuffer()
{
  close_file();
}


bool ASFileBuffer::open_file(const char* path, int oflag, int mode)
{
  // A file is already open
  if(-1 != filedes)
    return true;

  if( 0 == (oflag & O_CREAT) )
    filedes = efs_open(path, oflag);
  else
    filedes = efs_open(path, oflag, mode);

  if(-1 == filedes)
  {
    return false;
  }

  struct fs_stat file_stat;
  if(0 != efs_fstat(filedes, &file_stat))
  {
    return false;
  }

  file_size  = file_stat.st_size;
  return true;
}


void ASFileBuffer::close_file()
{
  if(-1 != filedes)
  {
    efs_close(filedes);
  }

  clear();
}


bool ASFileBuffer::is_open()
{
  return (-1 != filedes);
}


bool ASFileBuffer::read(ASString& buf, uint32 len)
{
  if(!validate_read())
    return false;

  uint32 read_size = get_min(len, internal_buffer.size() - stream_position);
  buf = ASString(internal_buffer.c_str() + stream_position, read_size);
  stream_position += read_size;

  return true;
}


bool ASFileBuffer::read(ASBuffer& buf, uint32 len)
{
  if(!validate_read())
    return false;

  uint32 read_size = get_min(len, internal_buffer.size() - stream_position);
  buf = ASBuffer(internal_buffer.content() + stream_position, read_size);
  stream_position += read_size;

  return true;
}


char ASFileBuffer::peek()
{
  if(!validate_read() || internal_buffer.size() <= stream_position)
    return '\0';

  return *(internal_buffer.c_str() + stream_position);
}


bool ASFileBuffer::write(const char* buf, uint32 len)
{
  if(-1 == filedes)
    return false;

  uint32 total_write = 0;

  while(total_write < len)
  {
    int32 written = efs_write(filedes, (const void*)(buf + total_write), len - total_write);
    if(0 >= written)
    {
      return false;
    }

    total_write += written;
  }

  return true;
}


bool ASFileBuffer::write(const uint8* buf, uint32 len)
{
  if(-1 == filedes)
    return false;

  uint32 total_write = 0;

  while(total_write < len)
  {
    int32 written = efs_write(filedes, (const void*)(buf + total_write), len - total_write);
    if(0 >= written)
    {
      return false;
    }

    total_write += written;
  }

  return true;
}


bool ASFileBuffer::validate_read()
{
  if(-1 == filedes)
  {
    return false;
  }
  else if(internal_buffer.size() == file_size && internal_buffer.size() <= stream_position)
  {
    end_of_file = true;
    return false;
  }

  if(internal_buffer.size() < file_size)
  {
    return read_file();
  }
  return true;
}


bool ASFileBuffer::read_file()
{
  if(-1 == filedes)
    return false;

  uint8  buffer[200];
  uint32 buffer_size = 200;

  while(internal_buffer.size() < file_size)
  {
    // read file
    int32 read = efs_read(filedes, buffer, buffer_size);

    if(-1 == read)
    {
      return false;
    }
    else if(0 == read)
    {
      break;
    }

    internal_buffer.append(buffer, read);
  }
  return true;
}


void ASFileBuffer::clear()
{
  filedes         = -1;
  file_size       = 0;
  stream_position = 0;
  internal_buffer.clear();
}




/*==============================================================================
                                ASIFStream
==============================================================================*/
ASIFStream::ASIFStream()
{
  init(&ifstream);
  set_state(AS_STREAM_BAD_BIT); // Since no file is open for read
}


ASIFStream::ASIFStream(const char* path, int oflag, int mode)
{
  init(&ifstream);
  open_file(path, oflag, mode);
}


ASIFStream::~ASIFStream()
{
  close_file();
}


void ASIFStream::open_file(const char* path, int oflag, int mode)
{
  if( ifstream.open_file(path, oflag, mode) )
  {
    clear_state();
  }
  else
  {
    set_state(AS_STREAM_BAD_BIT);
  }
}


void ASIFStream::close_file()
{
  ifstream.close_file();
}


bool ASIFStream::is_open()
{
  return ifstream.is_open();
}


/*==============================================================================
                                ASOFStream
==============================================================================*/
ASOFStream::ASOFStream()
{
  init(&ofstream);
  set_state(AS_STREAM_BAD_BIT); // Since no file is open for read
}


ASOFStream::ASOFStream(const char* path, int oflag, int mode)
{
  init(&ofstream);
  open_file(path, oflag, mode);
}


ASOFStream::~ASOFStream()
{
  close_file();
}


void ASOFStream::open_file(const char* path, int oflag, int mode)
{
  if( ofstream.open_file(path, oflag, mode) )
  {
    clear_state();
  }
  else
  {
    set_state(AS_STREAM_BAD_BIT);
  }
}


void ASOFStream::close_file()
{
  if(ofstream.is_open())
  {
    flush();
    ofstream.close_file();
  }
}

bool ASOFStream::is_open()
{
  return ofstream.is_open();
}




} /* namespace Utils */
} /* namespace Appsrv */


