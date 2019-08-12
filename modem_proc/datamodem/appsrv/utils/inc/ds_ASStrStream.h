/*==============================================================================

                              ds_ASSStream.h

GENERAL DESCRIPTION
  Wrapper class to handle EFS reads/writes. Class usage similar to <stringstream>
  Simultaneous I/O is not supported.

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
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ASSTRSTREAM_H
#define DS_ASSTRSTREAM_H

#include "ds_ASIOStream.h"
#include "ds_ASString.h"


namespace Appsrv
{
namespace Utils
{


class ASStringBuffer : public ASStreamBuffer
{
public:
  ASStringBuffer();
  ASStringBuffer(const ASString& s);
  ASStringBuffer(const char* s, uint32 len);
  ~ASStringBuffer();

  // Virtual functions from base
  virtual bool read(ASString& buf, uint32 len);
  virtual bool read(ASBuffer& buf, uint32 len);
  virtual char peek();

  virtual bool write(const char* buf, uint32 len);
  virtual bool write(const uint8* buf, uint32 len);

  const char* str() const;
  uint32 size() const
  { return current_size; }

  void str(const ASString& s);

private:
  bool validate_read();
  void clear();


  void init(uint32 len);
  bool copy(const char* buf, uint32 len);
  bool append(const char* buf, uint32 len);
  bool resize(const uint32 req_size);

private:
  // ASString internal_buffer;

  char*  internal_buffer;
  uint32 current_size;
  uint32 buffer_capacity;
};



class ASIStringStream : public ASIStream
{
public:
  ASIStringStream();
  ASIStringStream(const ASString& str);
  ASIStringStream(const char* str, uint32 len);
  ~ASIStringStream();

  const char* str() const;
  uint32 size() const;
  void str(const ASString& s);

private:
  ASStringBuffer istringstream;
};





// Note: Current behavior if initial string is set -> append
class ASOStringStream : public ASOStream
{
public:
  ASOStringStream();
  ASOStringStream(const ASString& str);
  ASOStringStream(const char* str, uint32 len);
  ~ASOStringStream();

  const char* str() const;
  uint32 size() const;
  void str(const ASString& s);

private:
  ASStringBuffer ostringstream;
};




} /* namespace Utils */
} /* namespace Appsrv */


#endif /* DS_ASSTRSTREAM_H */
