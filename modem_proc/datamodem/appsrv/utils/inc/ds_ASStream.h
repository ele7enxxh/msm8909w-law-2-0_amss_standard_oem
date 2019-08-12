/*==============================================================================

                              ds_ASStream.h

GENERAL DESCRIPTION
  Wrapper class to handle EFS reads/writes. Class usage similar to <fstream>

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
#ifndef DS_ASSTREAM_H
#define DS_ASSTREAM_H

#include "comdef.h"

class ASString;
class ASBuffer;

namespace Appsrv
{
namespace Utils
{


const uint8 AS_STREAM_NOERR    = 0x00;
const uint8 AS_STREAM_EOF_BIT  = 0x01;
const uint8 AS_STREAM_FAIL_BIT = 0x02;
const uint8 AS_STREAM_BAD_BIT  = 0x04;


/* Base class for stream buffer */
class ASStreamBuffer
{
public:
  ASStreamBuffer();

  ASStreamBuffer& operator++();
  ASStreamBuffer& operator--();
  uint32 streampos();
  uint32 streampos(uint32 pos);

  bool eof();
  bool eof(bool eof_flag);


  // Virtual functions for derived class to override. Default behavior is noop.
  virtual bool read(ASString& buf, uint32 len);
  virtual bool read(ASBuffer& buf, uint32 len);
  virtual char peek();

  virtual bool write(const char* buf, uint32 len);
  virtual bool write(const uint8* buf, uint32 len);


protected:
  /* ASStreamBuffer members */
  uint32 stream_position;
  bool   end_of_file;
};


// Base class for streams
class ASStream
{
public:
  ASStream(ASStreamBuffer* buf);
  virtual ~ASStream();

  bool good();
  bool eof();
  bool fail();
  bool bad();
  bool operator!();
  uint8 get_state();
  void set_state(uint8 error_state);
  void clear_state();
  ASStreamBuffer* rdbuf();
  ASStreamBuffer* rdbuf(ASStreamBuffer* buf);

  bool skipws();
  bool noskipws();
  bool skipws(bool wsflag);


protected:
  ASStream();
  void init(ASStreamBuffer* buf);

private:
  // Hide copy/assignment
  ASStream(const ASStream& stream);
  ASStream& operator=(const ASStream& stream);

protected:

private:
  ASStreamBuffer* stream_buffer;
  uint8           errflag;
  bool            skip_ws;
};



ASStream& skipws(ASStream& str);
ASStream& noskipws(ASStream& str);

} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_ASSTREAM_H */
