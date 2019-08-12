/*==============================================================================

                              ds_ASFStream.h

GENERAL DESCRIPTION
  Wrapper class to handle EFS reads/writes. Class usage similar to <fstream>
  Simultaneous I/O is not supported.

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
#ifndef DS_ASFSTREAM_H
#define DS_ASFSTREAM_H

#include "ds_ASIOStream.h"

extern "C"
{
  #include "fs_public.h"
  #include "fs_sys_types.h"
}

class ASString;
// class ASBuffer;

namespace Appsrv
{
namespace Utils
{


class ASFileBuffer : public ASStreamBuffer
{
public:
  ASFileBuffer();
  ASFileBuffer(const char* path, int oflag, int mode=0);
  ~ASFileBuffer();

  // Virtual functions from base
  virtual bool read(ASString& buf, uint32 len);
  virtual bool read(ASBuffer& buf, uint32 len);
  virtual char peek();

  virtual bool write(const char* buf, uint32 len);
  virtual bool write(const uint8* buf, uint32 len);

  bool open_file(const char* path, int oflag, int mode=0);
  void close_file();
  bool is_open();

private:
  bool validate_read();
  bool read_file();
  void clear();


private:
  int      filedes;
  uint32   file_size;
  ASBuffer internal_buffer;
};



class ASIFStream : public ASIStream
{
public:
  ASIFStream();
  ASIFStream(const char* path, int oflag, int mode=0);
  ~ASIFStream();

  void open_file(const char* path, int oflag, int mode=0);
  void close_file();
  bool is_open();


private:
  ASFileBuffer ifstream;
};






class ASOFStream : public ASOStream
{
public:
  ASOFStream();
  ASOFStream(const char* path, int oflag, int mode=0);
  ~ASOFStream();

  void open_file(const char* path, int oflag, int mode=0);
  void close_file();
  bool is_open();

private:
  ASFileBuffer ofstream;
};




} /* namespace Utils */
} /* namespace Appsrv */




/*
References for EFS Open

@param oflag
  The 'oflag' argument describes how this file should be opened, and
  contains one of the following values:
    O_RDONLY  - Open for reading only.
    O_WRONLY  - Open for writing only.
    O_RDWR    - Open for reading and writing.
  In addition, the following flags can be bitwise OR'd with this value:
    O_APPEND  - All writes will self-seek to the end of the file before writing.
    O_CREAT   - Create the file if it does not exist.
    O_EXCL    - Fail if the file exists.
    O_TRUNC   - Truncate the file to zero bytes on successful open.
  These flags can be used to specify common desired ways of opening files:
    O_CREAT | O_TRUNC     - Normal for writing a file.  Creates if doesn't
                            exist.  Resulting file is zero bytes long.
    O_CREAT | O_EXCL      - Creates a file but fails if it exists.

@param mode
  If O_CREAT is a part of 'oflag', a third argument (named mode) must be passed
  to efs_open to define the file permissions given to the newly created file.
  One or more of the following permission bits can be OR'd as the mode
  parameter:
  @par
  ::S_IRUSR   : Read permission for User
  ::S_IWUSR   : Write permission for User
  ::S_IXUSR   : Execute permission for User
  ::S_IRGRP   : Read permission for Group
  ::S_IWGRP   : Write permission for Group
  ::S_IXGRP   : Execute permission for Group
  ::S_IROTH   : Read permission for Other
  ::S_IWOTH   : Write permission for Other
  ::S_IXOTH   : Execute permission for Other
  ::S_ISUID   : Set UID on execution
  ::S_ISGID   : Set GID on execution
  ::S_ISVTX   : Sticky bit (Hidden attrib in FAT)
*/


#endif /* DS_ASFSTREAM_H */
