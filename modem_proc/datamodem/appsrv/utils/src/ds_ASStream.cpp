/*==============================================================================

                        ds_ASStream.cpp

GENERAL DESCRIPTION
  Stream base class

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
#include "ds_ASStream.h"

#include "ds_ASString.h"
#include "ds_ASBuffer.h"
#include "ds_ASUtility.h"
#include <stringl/stringl.h>


namespace Appsrv
{
namespace Utils
{

// Default buffer to use for NULL or unset buffer stream object
ASStreamBuffer noop_buffer;


/* ASStreamBuffer base behavior (noop) */
ASStreamBuffer::ASStreamBuffer()
: stream_position(0), end_of_file(false)
{ }

ASStreamBuffer& ASStreamBuffer::operator++()
{
  ++stream_position;

  return *this;
}
ASStreamBuffer& ASStreamBuffer::operator--()
{
  if(0 != stream_position)
    --stream_position;

  return *this;
}


uint32 ASStreamBuffer::streampos()
{
  return stream_position;
}


uint32 ASStreamBuffer::streampos(uint32 pos)
{
  uint32 tmp = stream_position;
  stream_position = pos;
  return tmp;
}


bool ASStreamBuffer::eof()
{
  return end_of_file;
}

bool ASStreamBuffer::eof(bool eof_flag)
{
  bool tmp = end_of_file;
  end_of_file = eof_flag;
  return tmp;
}


bool ASStreamBuffer::read(ASString& buf, uint32 len)
{ return false; }

bool ASStreamBuffer::read(ASBuffer& buf, uint32 len)
{ return false; }

char ASStreamBuffer::peek()
{ return 0; } // null char

bool ASStreamBuffer::write(const char* buf, uint32 len)
{ return false; }

bool ASStreamBuffer::write(const uint8* buf, uint32 len)
{ return false; }









ASStream::ASStream()
: stream_buffer(&noop_buffer), errflag(0), skip_ws(true)
{
  init(NULL);
}


ASStream::ASStream(ASStreamBuffer* buf)
: stream_buffer(&noop_buffer), errflag(0), skip_ws(true)
{
  init(buf);
}


ASStream::~ASStream()
{ }


bool ASStream::good()
{
  return errflag == 0;
}


bool ASStream::eof()
{
  return 0 != (errflag & AS_STREAM_EOF_BIT);
}


bool ASStream::fail()
{
  return 0 != (errflag & (AS_STREAM_FAIL_BIT|AS_STREAM_BAD_BIT) );
}


bool ASStream::bad()
{
  return 0 != (errflag & AS_STREAM_BAD_BIT);
}


bool ASStream::operator!()
{
  return fail();
}


void ASStream::init(ASStreamBuffer* buf)
{
  rdbuf(buf);
}


uint8 ASStream::get_state()
{
  return errflag;
}


void ASStream::set_state(uint8 error_state)
{
  errflag |= error_state;
}


void ASStream::clear_state()
{
  errflag = 0;
}


bool ASStream::skipws()
{
  bool tmp = skip_ws;
  skip_ws  = true;
  return tmp;
}


bool ASStream::noskipws()
{
  bool tmp = skip_ws;
  skip_ws  = false;
  return tmp;
}


bool ASStream::skipws(bool wsflag)
{
  bool tmp = skip_ws;
  skip_ws = wsflag;
  return tmp;
}


ASStreamBuffer* ASStream::rdbuf()
{
  return stream_buffer;
}


ASStreamBuffer* ASStream::rdbuf(ASStreamBuffer* buf)
{
  // stream_buffer guaranteed not NULL
  if(NULL == buf)
  {
    stream_buffer = &noop_buffer;
    set_state(AS_STREAM_FAIL_BIT);
  }
  else
  {
    stream_buffer = buf;
    clear_state();
  }
  return stream_buffer;
}







ASStream& skipws(ASStream& str)
{
  str.skipws();
  return str;
}



ASStream& noskipws(ASStream& str)
{
  str.noskipws();
  return str;
}



} /* namespace Utils */
} /* namespace Appsrv */

