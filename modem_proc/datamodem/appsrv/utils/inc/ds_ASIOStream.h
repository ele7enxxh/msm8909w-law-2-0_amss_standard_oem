/*==============================================================================

                              ds_ASIOStream.h

GENERAL DESCRIPTION
  I/O Stream base class. Simultaneous I/O is not supported.

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
#ifndef DS_ASIOSTREAM_H
#define DS_ASIOSTREAM_H

#include "ds_ASStream.h"

#include "ds_ASBuffer.h"

class ASString;
// class ASBuffer;

namespace Appsrv
{
namespace Utils
{





// class ASStringBuffer : public ASStreamBuffer
// {
// public:
//   ASStringBuffer(const ASString& str);
//   bool read();
//   bool write();


//   int      filedes;
//   uint32   file_size;
//   ASString string_buffer;
// };



struct ASIStreamDelimiter
{
  virtual bool operator()(char c) const = 0;
};


class ASIStream : public ASStream
{
public:
  ASIStream(ASStreamBuffer* buf);
  virtual ~ASIStream();

  char       get();
  ASIStream& get(char& chr);
  ASIStream& getline(ASString& buf, char delim='\n');

  ASIStream& read(ASString& buf, uint32 len);
  ASIStream& read(ASBuffer& buf, uint32 len);

  char       peek();

  // Ostream for arithmetic  types
  ASIStream& operator>>(bool& value);
  ASIStream& operator>>(char& value);
  ASIStream& operator>>(sint15& value);
  ASIStream& operator>>(sint31& value);
  ASIStream& operator>>(uint16& value);
  ASIStream& operator>>(uint32& value);
  // Ostream for string/buffer class types
  ASIStream& operator>>(ASBuffer& value);
  ASIStream& operator>>(ASString& value);


protected:
  ASIStream();

private:
  void   skip_whitespace();
  uint32 get_next_pos_len();
  uint32 get_next_pos_len(char delim);
  uint32 get_next_pos_len(ASIStreamDelimiter& delim_fcn);
  uint32 get_next_digit_pos_len(bool is_signed);


private:
  // Hide copy/assignment
  ASIStream(const ASIStream& os);
  ASIStream& operator=(const ASIStream& os);
};


ASIStream& skipws(ASIStream& is);



class ASOStream : public ASStream
{
public:
  ASOStream(ASStreamBuffer* buf);
  virtual ~ASOStream();

  // ostream
  ASOStream& write(const ASString& buffer);
  ASOStream& write(const ASBuffer& buffer);
  ASOStream& write(const char* buffer, uint32 len);
  ASOStream& write(const uint8* buffer, uint32 len);


  // Ostream for arithmetic  types
  ASOStream& operator<<(const bool       value);
  ASOStream& operator<<(const char       value);
  ASOStream& operator<<(const char*      value);
  ASOStream& operator<<(const sint15     value);
  ASOStream& operator<<(const sint31     value);
  ASOStream& operator<<(const uint16     value);
  ASOStream& operator<<(const uint32     value);
  // Ostream for string/buffer class types
  ASOStream& operator<<(const ASBuffer& value);
  ASOStream& operator<<(const ASString& value);
  // Manipulators
  ASOStream& operator<<(ASOStream& (*pf)(ASOStream&));

  ASOStream& flush();

protected:
  ASOStream();

private:
  // Hide copy/assignment
  ASOStream(const ASOStream& os);
  ASOStream& operator=(const ASOStream& os);
  ASOStream& write_to_buffer(const char* buf, uint32 len);
  ASOStream& write_to_buffer(const uint8* buf, uint32 len);

private:
  ASBuffer os_buffer;
};

ASOStream& endl(ASOStream& os);
ASOStream& ends(ASOStream& os);
ASOStream& flush(ASOStream& os);





} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_ASIOSTREAM_H */
