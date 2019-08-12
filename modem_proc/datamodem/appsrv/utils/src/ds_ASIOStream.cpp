/*==============================================================================

                        ds_ASIOStream.cpp

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
#include "ds_ASIOStream.h"

#include "ds_ASString.h"
#include "ds_ASUtility.h"
#include <stringl/stringl.h>
#include <cctype>
#include <cstdlib>



namespace Appsrv
{
namespace Utils
{


const uint32 BUFFER_MAX_SIZE = 128;

int ds_istream_validator_default_fcn(int c)
{
  return 1;
}


struct ASCharDelimiter : public ASIStreamDelimiter
{
  char delim;

  ASCharDelimiter(char c)
  : delim(c)
  { }

  bool operator()(char c) const
  { return '\0' != c && delim != c; }
};

struct ASSpaceDelimiter : public ASIStreamDelimiter
{
  ASSpaceDelimiter()
  { }

  bool operator()(char c) const
  { return '\0' != c && 0 == isspace(c); }
};



/*==============================================================================
                                      ASIStream
==============================================================================*/
ASIStream::ASIStream()
: ASStream()
{ }



ASIStream::ASIStream(ASStreamBuffer* buf)
: ASStream(buf)
{ }



ASIStream::~ASIStream()
{ }


void ASIStream::skip_whitespace()
{
  ASStreamBuffer* buf = rdbuf();
  char c = buf->peek();
  while(c != '\0' && 0 != isspace(c))
  {
    ++(*buf);
    c = buf->peek();
  }
}


uint32 ASIStream::get_next_pos_len()
{
  ASSpaceDelimiter delimiter;
  return get_next_pos_len(delimiter);
}


uint32 ASIStream::get_next_pos_len(char delim)
{
  ASCharDelimiter delimiter(delim);
  return get_next_pos_len(delimiter);
}


uint32 ASIStream::get_next_pos_len(ASIStreamDelimiter& delim_fcn)
{
  if(!good())
    return 0;

  if(skipws())
    skip_whitespace();

  ASStreamBuffer* buf = rdbuf();
  uint32 pos = buf->streampos();
  char   c   = buf->peek();

  // Seek delim. Returns false if delim condition is met or NULL char is found(eof)
  while(delim_fcn(c))
  {
    ++(*buf);
    c = buf->peek();
  }

  // Get end position and revert stream position
  pos = buf->streampos(pos);
  return pos - buf->streampos();
}


uint32 ASIStream::get_next_digit_pos_len(bool is_signed)
{
  if(!good())
    return 0;

  ASStreamBuffer* buf = rdbuf();
  if(skipws())
    skip_whitespace();

  bool   is_negative = false;
  uint32 pos         = buf->streampos();
  char   c           = buf->peek();

  if(is_signed && '-' == c)
  {
    ++(*buf);
    is_negative = true;
    c = buf->peek();
  }

  while( 0 != isdigit(c) )
  {
    ++(*buf);
    c = buf->peek();
  }

  // Get end position and revert stream position
  pos = buf->streampos(pos);
  uint32 result = pos - buf->streampos();

  if(is_negative && 1 >= result)
    result = 0;

  return result;
}



char ASIStream::get()
{
  ASString str("");
  char chr = 0;

  if( ( read(str, 1) ).good() )
  {
    chr = str[0];
  }

  return chr;
}


ASIStream& ASIStream::get(char& chr)
{
  ASString str("");

  if( ( read(str, 1) ).good() )
  {
    chr = str[0];
  }

  return *this;
}


ASIStream& ASIStream::getline(ASString& buf, char delim)
{
  if(!good())
    return *this;

  ASStreamBuffer* bufstream = rdbuf();
  bool            wsflag    = skipws(false); // unset skip ws flag if set
  uint32          nextlen   = get_next_pos_len(delim);

  if(0 == nextlen || !good() || !bufstream->read(buf, nextlen))
  {
    set_state(AS_STREAM_BAD_BIT);
    if(bufstream->eof())
    {
      set_state(AS_STREAM_EOF_BIT);
    }
  }

  skipws(wsflag); // revert skip ws flag
  return *this;
}


ASIStream& ASIStream::read(ASString& buf, uint32 len)
{
  ASStreamBuffer* bufstream = rdbuf();

  if(!good() || !bufstream->read(buf, len))
  {
    set_state(AS_STREAM_BAD_BIT);
    if(bufstream->eof())
    {
      set_state(AS_STREAM_EOF_BIT);
    }
  }

  return *this;
}


ASIStream& ASIStream::read(ASBuffer& buf, uint32 len)
{
  ASStreamBuffer* bufstream = rdbuf();

  if(!good() || !bufstream->read(buf, len))
  {
    set_state(AS_STREAM_BAD_BIT);
    if(bufstream->eof())
    {
      set_state(AS_STREAM_EOF_BIT);
    }
  }

  return *this;
}


char ASIStream::peek()
{
  ASStreamBuffer* bufstream = rdbuf();

  if(!good())
  {
    set_state(AS_STREAM_BAD_BIT);
    if(bufstream->eof())
    {
      set_state(AS_STREAM_EOF_BIT);
    }
    return '\0';
  }

  return bufstream->peek();
}



ASIStream& ASIStream::operator>>(bool& value)
{
  ASString str("0");

  if(skipws())
    skip_whitespace();

  if( ( read(str, 1) ).good() )
  {
    value = ("0" != str); // 0 = false, else true
  }

  return *this;
}


ASIStream& ASIStream::operator>>(char& value)
{
  ASString str("");

  if(skipws())
    skip_whitespace();


  if( ( read(str, 1) ).good() )
  {
    value = str[0];
  }

  return *this;
}


ASIStream& ASIStream::operator>>(sint15& value)
{
  ASString str("");
  uint32   nextlen = get_next_digit_pos_len(true);

  if(0 == nextlen)
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  else if( ( read(str, nextlen) ).good() )
  {
    value = strtol(str.c_str(), NULL, 10);
  }

  return *this;
}


ASIStream& ASIStream::operator>>(sint31& value)
{
  ASString str("");
  uint32   nextlen = get_next_digit_pos_len(true);

  if(0 == nextlen)
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  else if( ( read(str, nextlen) ).good() )
  {
    value = strtol(str.c_str(), NULL, 10);
  }

  return *this;
}


ASIStream& ASIStream::operator>>(uint16& value)
{
  ASString str("");
  uint32   nextlen = get_next_digit_pos_len(false);

  if(0 == nextlen)
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  else if( ( read(str, nextlen) ).good() )
  {
    value = strtol(str.c_str(), NULL, 10);
  }

  return *this;
}


ASIStream& ASIStream::operator>>(uint32& value)
{
  ASString str("");
  uint32   nextlen = get_next_digit_pos_len(false);

  if(0 == nextlen)
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  else if( ( read(str, nextlen) ).good() )
  {
    value = strtol(str.c_str(), NULL, 10);
  }

  return *this;
}


ASIStream& ASIStream::operator>>(ASBuffer& value)
{
  uint32 nextlen = get_next_pos_len();

  if( 0 == nextlen || !( read(value, nextlen) ).good() )
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  return *this;
}


ASIStream& ASIStream::operator>>(ASString& value)
{
  uint32 nextlen = get_next_pos_len();

  if( 0 == nextlen || !( read(value, nextlen) ).good() )
  {
    set_state(AS_STREAM_FAIL_BIT);
  }
  return *this;
}




/*==============================================================================
                                      ASOstream
==============================================================================*/
ASOStream::ASOStream()
: ASStream()
{ }



ASOStream::ASOStream(ASStreamBuffer* buf)
: ASStream(buf)
{ }



ASOStream::~ASOStream()
{
  flush();
}


// ostream
ASOStream& ASOStream::write(const ASString& buf)
{
  return write_to_buffer( buf.c_str(), buf.size() );
}


ASOStream& ASOStream::write(const ASBuffer& buf)
{
  return write_to_buffer( buf.content(), buf.size() );
}


ASOStream& ASOStream::write(const char* buf, uint32 len)
{
  return write_to_buffer(buf, len);
}


ASOStream& ASOStream::write(const uint8* buf, uint32 len)
{
  return write_to_buffer(buf, len);
}





// Ostream for arithmetic  types
ASOStream& ASOStream::operator<<(const bool value)
{
  // Null termninator not needed
  char buf[1];
  if(value)
    buf[0] = '0';
  else
    buf[0] = '1';

  return write_to_buffer(buf, 1);
}


ASOStream& ASOStream::operator<<(const char value)
{
  // Null termninator not needed
  char buf[1];
  buf[0] = value;

  return write_to_buffer(buf, 1);
}

ASOStream& ASOStream::operator<<(const char* value)
{
  return write_to_buffer(value, strlen(value));
}


ASOStream& ASOStream::operator<<(const sint15 value)
{
  char   buf[DS_UTILS_SINT31_MAX_LEN] = {0};
  uint16 len = sltostr(value, buf);

  return write_to_buffer(buf, len);
}


ASOStream& ASOStream::operator<<(const sint31 value)
{
  char   buf[DS_UTILS_SINT31_MAX_LEN] = {0};
  uint16 len = sltostr(value, buf);

  return write_to_buffer(buf, len);
}


ASOStream& ASOStream::operator<<(const uint16 value)
{
  char   buf[DS_UTILS_UINT32_MAX_LEN] = {0};
  uint16 len = ultostr(value, buf);

  return write_to_buffer(buf, len);
}


ASOStream& ASOStream::operator<<(const uint32 value)
{
  char   buf[DS_UTILS_UINT32_MAX_LEN] = {0};
  uint16 len = ultostr(value, buf);

  return write_to_buffer(buf, len);
}


// Ostream for custom class types
ASOStream& ASOStream::operator<<(const ASBuffer& value)
{
  return write_to_buffer( value.c_str(), value.size() );
}


ASOStream& ASOStream::operator<<(const ASString& value)
{
  return write_to_buffer( value.c_str(), value.size() );
}

ASOStream& ASOStream::operator<<(ASOStream& (*pf)(ASOStream&))
{
  return pf(*this);
}


ASOStream& ASOStream::flush()
{
  if(0 == os_buffer.size())
    return *this;

  ASStreamBuffer* bufstream = rdbuf();

  if( !bufstream->write( os_buffer.content(), os_buffer.size() ) )
  {
    set_state(AS_STREAM_BAD_BIT);
  }

  os_buffer.clear();
  return *this;
}




ASOStream& ASOStream::write_to_buffer(const char* buf, uint32 len)
{
  if(NULL == buf)
    return *this;

  if(BUFFER_MAX_SIZE <= os_buffer.size() + len)
  {
    flush();
    if(!good())
    {
      return *this;
    }
  }

  os_buffer.append(buf, len);

  return *this;
}


ASOStream& ASOStream::write_to_buffer(const uint8* buf, uint32 len)
{
  if(NULL == buf)
    return *this;

  if(BUFFER_MAX_SIZE <= os_buffer.size() + len)
  {
    flush();
    if(!good())
    {
      return *this;
    }
  }

  os_buffer.append(buf, len);

  return *this;
}









/*==============================================================================
                                ASIOstream Utility
==============================================================================*/
ASOStream& endl(ASOStream& os)
{
  os << '\n';
  return os.flush();
}

ASOStream& ends(ASOStream& os)
{
  os << '\0';
  return os.flush();
}

ASOStream& flush(ASOStream& os)
{
  return os.flush();
}


} /* namespace Utils */
} /* namespace Appsrv */


