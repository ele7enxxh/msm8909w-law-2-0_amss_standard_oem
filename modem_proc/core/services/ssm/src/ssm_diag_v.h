#ifndef SSM_DIAG_H
#define SSM_DIAG_H
/*==========================================================================

               Diagnostic Packet Definitions for SSM

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.

  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2011 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/14   din     Added diag test command to test secure channel.
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
04/29/13   din     Changes pertaining to ADSP were done.
09/03/11   din     File created.

===========================================================================*/

#include "comdef.h"

#include "ssm_diag_cmd_v.h"
#include "msg.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */
#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#define DIAG_SSM_DATA_SIZE 7


/*==========================================================================

PACKET   SSM_DIAG_F

PURPOSE  Sent from the DM to the DMSS to request execution of a SSM
         command. Usage: send_data 75 69 <SSCmd code> 0

============================================================================*/

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_SSM_DATA_SIZE];  /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}SSM_DIAG_F_req_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_SSM_DATA_SIZE];  /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}SSM_DIAG_F_rsp_type;


#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/*
 * Open File Packet
 *
 * Used to open a named file.
 *
 * Note: If a request is received to open an already open file, the file
 * is closed and reopened.
 */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 oflag;             /* Open flags                                   */
  int32 mode;              /* Mode                                         */
  char  path[1];           /* Pathname (null-terminated string)            */
} ssm_diag_open_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 fd;                /* File descriptor if successful, -1 otherwise  */
  int32 diag_errno;        /* Error code if error, 0 otherwise             */
} ssm_diag_open_rsp_type;

/*
 * Close File Packet
 *
 * Used to close a file descriptor.
 *
 * Note: If a request is received to close an already closed file, nothing
 * is done but an error is not reported.
 */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 fd;                /* File descriptor                              */
} ssm_diag_close_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;        /* Error code if error, 0 otherwise             */
} ssm_diag_close_rsp_type;


/*
 * Read File Packet
 *
 * Used to read the contents of an open file. Note that the response packet
 * does not explicitly contain the number of bytes read, since this can be
 * computed from the packet size. The number of bytes actually read may be
 * less than the number specified in the request packet if EOF is encountered
 * (this is not an error).
 *
 * NOTE 1: The fd is part of the response packet because we want to allow for
 * the possibility of more than one file being open for reading. The fd
 * allows the tool to match a response with a request.
 *
 * NOTE 2: This is one of the packets that support windowing. Thus there can
 * be several requests outstanding before a response is sent.
 */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32  fd;               /* File descriptor                              */
  uint32 nbyte;            /* Number of bytes to read                      */
  uint32 offset;           /* Offset in bytes from the origin              */
} ssm_diag_read_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32  fd;               /* File descriptor                              */
  uint32 offset;           /* Requested offset in bytes from the origin    */
  int32  bytes_read;       /* bytes read if successful, -1 otherwise       */
  int32  diag_errno;       /* Error code if error, 0 otherwise             */
  char   data[1];          /* The data read out                            */
} ssm_diag_read_rsp_type;


/*
 * Write File Packet
 *
 * Used to write data into a given offset in an open file. If the offset is
 * past the current end of the file, the file is zero-filled until the offset
 * and then the data is written to the file. Note that the request packet
 * does not explicitly contain the number of bytes to be written, since this
 * can be computed from the packet size. Unless an error occurs, all the
 * bytes specified in the data field of the request packet are written to
 * the file.
 *
 * NOTE 1: The fd is part of the response packet because we want to allow for
 * the possibility of more than one file being open for writing. The fd allows
 * the tool to match a response with a request.
 *
 * NOTE 2: This is one of the packets that support windowing. Thus there can
 * be several requests outstanding before a response is sent.
 */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 fd;                /* File descriptor                              */
  uint32 offset;           /* Offset in bytes from the origin              */
  uint32 nbyte;            /* Bytes to be written                          */
  char  data[1];           /* The data to be written                       */
} ssm_diag_write_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  int32 fd;                /* File descriptor                              */
  uint32 offset;           /* Requested offset in bytes from the origin    */
  int32 bytes_written;     /* The number of bytes written                  */
  int32 diag_errno;        /* Error code if error, 0 otherwise             */
} ssm_diag_write_rsp_type;

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_get_mode_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  uint32 mode;             /* Mode                                         */
} ssm_diag_get_mode_rsp_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_test_secure_channel_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  /* 0 indicates success 
   * 1 indicates encryption failed
   * 2 indicates decryption failed
   * 3 indicates string compare failed
   * 4 malloc failures */
  uint32 success;             
} ssm_diag_test_secure_channel_rsp_type;

void ssm_diag_init( void );

#endif /* SSM_DIAG_H */
