#ifndef TZOS_SCM_DEFS_H
#define TZOS_SCM_DEFS_H

/**
@file TZOS_SCM_DEFS.h
@brief Trust Zone Secure Channel Manager Definitions

Defines data types for use by secure channel manager control
queues.
*/

/*===========================================================================
   Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_scm_defs.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when         who      what, where, why
----------   ---      ------------------------------------
02/15/2010   ssm      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include "comdef.h"


/*===========================================================================

          PUBLIC DATA TYPES

============================================================================*/

/** Default MSG_Q size if application has not specified size */
#define TZOS_SCM_DEFAULT_ALLOC_SIZE      4096   /* 4k size */

/** Size of control queues...both request and response */
#define TZOS_SCM_CTRL_Q_SIZE     4096

/** SCM slot size */
#define TZOS_SCM_SLOT_SIZE       128

#define TZOS_SCM_SLOT_CNT        (TZOS_SCM_CTRL_Q_SIZE/TZOS_SCM_SLOT_SIZE)

/** Maximum number of commands supported by SCM */
#define TZOS_SCM_MAX_NUM_CMDS     16

/** Maximum number of sessions */
#define TZOS_SCM_MAX_NUM_SESSIONS 32

/** First 2 slots are always reserved for meta info in each q */
#define TZOS_SCM_META_INFO_SLOT_CNT  2

/** Meta Data struct for CTRL_Q_REQ and CTRL_Q_RSP */
typedef struct
{
  uint32   magic_no;
  uint32   num_cmds_pend;
  uint8    cmd_idx[TZOS_SCM_MAX_NUM_CMDS];
} tzos_scm_meta_data_type;

/**
 * Message types for Control Cmds
 */
typedef enum
{
  TZOS_SCM_CMD_INVALID    = 0,
  TZOS_SCM_CMD_OPEN       = 1,
  TZOS_SCM_CMD_CLOSE      = 2,
  TZOS_SCM_CMD_REQ_CMD    = 3,
  TZOS_SCM_CMD_ABORT      = 4,   /* NOT SUPPORTED */
  TZOS_SCM_CMD_COUNT      = 5,
  TZOS_SCM_CMD_END        = 0x7FFFFFFF
} tzos_scm_cmd_enum_type;

#define TZOS_SCM_CMD_USED_MASK       0x00000001
#define TZOS_SCM_CMD_CONTINUE_MASK   0x00000002
#define TZOS_SCM_CMD_ABORT_MASK      0x00000004

/**
 * Common data struct for SCM commands
 */
typedef struct
{
  /** Field mask to indicate whether a command is in use etc */
  uint32                  field_mask;
  /** Secure Channel manager command type */
  tzos_scm_cmd_enum_type  cmd;
  /** Total length of command including common header */
  uint32                  total_len;
} tzos_scm_cmd_common_type;

typedef struct
{
  uint8*    addr_ptr;
  uint32    len;
} tzos_scm_buf_type;

/**
 * Request data structures for SCM OpenSvc command
 */
typedef struct
{
  /** Common header */
  tzos_scm_cmd_common_type   common;
  /** Service id for which an open is requested */
  uint32                     svc_id;
  /** Array of pointers to msg_q  */
  tzos_scm_buf_type          buf[64];
  /** Number of buffers in buf array */
  uint32                     no_of_bufs;
} tzos_scm_open_req;

/**
 * Response data structures for SCM OpenSvc command
 */
typedef struct
{
  /** Common header */
  tzos_scm_cmd_common_type   common;
  /** Session ID that must match an open session to a service */
  uint32                     session_id;
  /** HLOS specific data that can be used as a cookie */
  uint32                     hlos_data;
} tzos_scm_open_rsp;

/**
 * Request and response data structures for close and req_cmd in
 * SCM
 */
typedef struct
{
  /** Common header */
  tzos_scm_cmd_common_type   common;
  /** Session ID that must match an open session to a service */
  uint32                     session_id;
} tzos_scm_close_req,
  tzos_scm_close_rsp,
  tzos_scm_req_cmd_req,
  tzos_scm_req_cmd_rsp;

#endif /* SCMDEFS_H */
