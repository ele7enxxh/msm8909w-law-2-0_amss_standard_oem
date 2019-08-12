#ifndef DIAG_F3_TRACEI_H
#define DIAG_F3_TRACEI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Diag debug subsystem dispatch handling

GENERAL DESCRIPTION
  Packet processing routines for FEATURE_SAVE_DEBUG_TRACE and
  FEATURE_SAVE_DEBUG_TRACE.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004 - 2014,2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/f3_trace/src/diag_f3_tracei.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/18/16   nk      Made changes in F3 trace logic for extended msgs
12/17/14   ph      Mainlined the feature FEATURE_SAVE_TRACE_ON_BY_DEFAULT.
11/19/13   rh      Added tracking for current source of F3 Trace settings 
11/18/13   ph      Remove intermediate buffer in F3 Trace as part of optimization.
04/24/13   rh      QShrink 3.0 support
03/06/13   sa      Using the reserved bit in the diag detail nv item to save the custom F3s
12/21/12   rh      Renamed several err_f3_trace_ types to diag_f3_trace_
12/19/12   rh      File renamed to diag_f3_tracei.h and moved to Services
12/14/09   tbg     File renamed to err_f3_trace.c; APIs and variables
                   renamed accordingly.
11/30/09   ps      Fixed compiler warnings & added F3trace support(SW).
09/09/09   jwh     Add FEATURE_DOGTEST_TASK code.
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/24/08   tbg     Created err_f3_trace_id_type to help id source build for logs
12/19/06   as      Reverted FEATURE_ERR_SMALL_BUFFER change.
12/18/06   as      Added support to decrease buffer size under
                   FEATURE_ERR_SMALL_BUFFER.
04/06/06   tbg     Allow optional external definitions of default CONTROL and
                   DETAIL values. Only applicable if the feature
                   FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined.
11/11/05   tbg     Changed naming conventions of err f3 trace files.
10/04/05   tbg     Added ability to turn on F3 trace saving by default,
                   if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined
08/11/05   eav     Added ERR_F3_TRACE_BUFFER_CORRUPTED
11/30/04   as      Added FEATURE_AUTO_RESET. This feature resets the phone
                   automatically without user intervention if the phone
                   encounters a critical failure
08/26/04   eav     Added support  for timestamp-saving optimizations
05/20/04   eav     Added supp rt to event-tracing.  Added support to put
                   event trace and F3 trace into a single trace buffer.
05/07/04   eav     File created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_ERR_HAS_F3_TRACE
#if defined(FEATURE_ERR_SMEM_CRASH_REPORT) && defined(FEATURE_SMEM)
#define ERR_F3_TRACE_TO_SMEM
#endif
#include "diagpkt.h"
#include "qw.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Unified trace buffer constants */
#ifndef DIAG_F3_TRACE_BUFFER_SIZE
#define DIAG_F3_TRACE_BUFFER_SIZE      131072
#endif

#define DIAG_F3_TRACE_BUFFER_CORRUPTED -2
#define DIAG_F3_TRACE_FAILURE          -1
#define DIAG_F3_TRACE_SUCCESS           0

   /* Allow targets to define these values in other header files if needed.
 * Values will not be redefined if they already exist. */
   #ifndef DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL
   /* For backwards compatability, check old define first */
     #ifdef ERR_DEBUG_CONTROL_MASK_DEFAULT_VAL
       #define DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL ERR_DEBUG_CONTROL_MASK_DEFAULT_VAL
     #else
       #define DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL 0x1
     #endif
   #endif

   #ifndef DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL
   /* For backwards compatability, check old define first */
     #ifdef ERR_DEBUG_DETAIL_MASK_DEFAULT_VAL
       #define DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL ERR_DEBUG_DETAIL_MASK_DEFAULT_VAL
     #else
       #define DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL 0xFF
     #endif
   #endif



#define MAX_8BIT_VALUE                      255
#define MAX_16BIT_VALUE                     65535
#define MAX_24BIT_VALUE                     16777215

#define DIAG_F3_TRACE_SAVECUSTOM_BIT 0x4
#define DIAG_F3_TRACE_SAVETIME 0x2
#define DIAG_F3_TRACE_SAVEVARS 0x1

/* Format of diag_f3_trace_hdr_type:
 *  header_flag: 32 bit value indicating the start of a message
 *
 *  config: 16 bit field containing the following information:
 *  Bits 15:13    Type Field (Event, Msg, etc.)
 *  Bits 11:8     Number of arguments Field
 *  Bit  7        Extended message
 *  Bits  6:4     Message Type Field (Standard MSG/Qshrink MSG)
 *  Bits  3:0     Details Field
 *                Bit 3:2 "Byte Sized Arguments" Field
 *                Bit 1   Save Time flag
 *                Bit 0   Save Vars flag
 */
#define DIAG_F3_TRACE_VALID_HEADER_FLAG 0xA55A6BB6
typedef PACK(struct)
{
   uint32 header_flag;
   uint16 config;
} diag_f3_trace_hdr_type;

typedef PACK(struct)
{
   uint16 lo;        /* Most significant of the lower 32 bits */
   uint32 hi;        /* Most significant bytes of a qword timestamp */
} diag_f3_trace_tstamp_type;

typedef PACK(struct)
{
  uint8 version;
  uint32 id; 
}diag_f3_trace_ext_type;

#define DIAG_F3_TRACE_MAX_ID_LENGTH 40

typedef struct
{
   uint64 timestamp;
   uint32 buffer_ptr;
   uint8  build[DIAG_F3_TRACE_MAX_ID_LENGTH];
} err_f3_trace_id_type;

typedef uint32 msg_arg_type;

typedef uint16 evt_id_type;
typedef uint16 evt_length_type;
typedef uint8  evt_payload_type;
typedef uint8  evt_payload_len_type;

/* This enum is kept the way it is to ensure that legacy files do not misidentify the latest QShrink version. */
enum
{
   DIAG_F3_TRACE_STANDARD = 0,
   DIAG_F3_TRACE_QSHRINK,
   DIAG_F3_TRACE_QSHRINK20,
   DIAG_F3_TRACE_QSHRINK30,
   DIAG_F3_TRACE_LAST
};


#define DIAG_F3_TRACE_MSG_BIT 0x1
#define DIAG_F3_TRACE_LOG_BIT 0x2
#define DIAG_F3_TRACE_EVT_BIT 0x4
#define DIAG_F3_TRACE_DIAG_MASK_BIT 0x8

/* Return the number of events in the stored trace.
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
} diag_serv_range_req_type;

typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint32 status;     /* 0 if no error; otherwise an error code */
   uint32 num_types;   /* Number of events */
} diag_serv_range_rsp_type;

/* Retrieve a range of types.
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint32 index;      /* Index of event to retrieve     */
} diag_serv_fetch_req_type;

/* Holds all information the DM needs to display an event
 */
typedef PACK(struct)
{
   uint16 evt_id;              /* ID of the event sent over       */
   uint64 ts;                  /* FULL timestamp (CDMA format)    */
   /* length and payload follow if included in this event_id      */
} diag_serv_evt_struct_type;

/* Holds all information the DM needs to display a message.  Conforms
 * to the Extended Message packet format described in the DMSS-SICD,
 * section 3.4.121.
 */
typedef PACK(struct)
{
   uint8  cmd_code;            /* 121 (0x79)                      */
   uint8  ts_type;             /* indicates format of the TIME field */
   uint8  num_args;            /* Number of arguments             */
   uint8  drop_cnt;            /* Number of messages dropped      */
   uint64 ts;                  /* Message timestamp (CDMA format) */
   uint16 line;                /* Line number                     */
   uint16 ss_id;
   uint32 ss_mask;
   uint32 args[1];             /* Zero or more arguments          */
/* This field is followed by NULL-terminated format and filename
 * strings.
 */
} diag_serv_full_msg_struct_type;

/* Holds all information the DM needs to display a qsr message.  Conforms
 * to the Qshrink Extended Message packet format described in the DMSS-SICD,
 * section 3.4.146.
 */
typedef PACK(struct)
{
   uint8  cmd_code;            /* 146 (0x92)                      */
   uint8  ts_type;             /* indicates format of the TIME field */
   uint8  num_args;            /* Number of arguments             */
   uint8  drop_cnt;            /* Number of messages dropped      */
   uint64 ts;                  /* Message timestamp (CDMA format) */
   uint16 line;                /* Line number                     */
   uint16 ss_id;
   uint32 ss_mask;
   uint32 hash;
   uint32 args[1];             /* Zero or more arguments          */
} diag_serv_qsr_msg_struct_type;

/* Holds all information the DM needs to display an event.  Conforms
 * to the event packet format described in the DMSS-ICD, section 3.4.95.
 */
typedef PACK(struct)
{
   uint8  cmd_code;                       /* 96 (0x60)            */
   uint16 length;                         /* Length of packet     */
   diag_serv_evt_struct_type event;
} diag_serv_full_evt_struct_type;

/* Return a list of all F3 filenames
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint32 status;
   uint8  num_files;
   uint64 timestamps[1];
} diag_serv_file_names_rsp_type;

/* Request the number of records stored in filename.  All subsequent calls
 * of get_next will be retrieved from this filename.  The response to this
 * packet will be diag_serv_range_rsp_type.
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint8 file_index;                    /* unique file index */
} diag_serv_file_range_req_type;

/* Delete specified trace file by filename index.  Response packet will be
 * the diag_serv_reset_info_rsp_type.
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint8 file_index;                    /* unique file index */
} diag_serv_file_reset_info_req_type;

typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint32 index;      /* Index of retreived event                */
   uint8  type;
   uint32 status;     /* 0 if no error; otherwise an error code  */

   PACK(union)
   {
      diag_serv_full_msg_struct_type full_msg;  /* Retrieved full F3 message */
      diag_serv_qsr_msg_struct_type qsr_msg;  /* Retrieved full qsr F3 message */
      diag_serv_full_evt_struct_type full_evt;  /* Retrieved full event      */
   } diag_serv_fetch_type;

} diag_serv_fetch_rsp_type;

/* Delete all trace information by deleting the trace file.
 */
typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
} diag_serv_reset_info_req_type;

typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;
   uint32  status;    /* 0 if no error; otherwise an error code  */
} diag_serv_reset_info_rsp_type;

/* Enum and variable for tracking what the current source of configuration values is
 */
enum {
   DIAG_F3_TRACE_CONFIG_DEFAULT,
   DIAG_F3_TRACE_CONFIG_NV,
   DIAG_F3_TRACE_CONFIG_DEVCFG,
   DIAG_F3_TRACE_CONFIG_CMD
};
extern uint32 diag_f3_trace_config_source;
/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT

DESCRIPTION
  Initialization function for this module.  Registers the packet handlers.
  Reads the diag_debug NV Item to set the run time mask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init(void);

// #warning migration: rename this (has diag dependencies)
// MIGRATION NOTE: recover_f3.cmm uses existence of this symbol to identify lockless design
void err_f3_save_msg(const void * constant_data_ptr, msg_arg_type * args, uint8 nargs, void * msg, uint32 type, uint8 version, void* param);

/*===========================================================================

FUNCTION DIAG_F3_TRACE_SAVE_TO_SMEM

DESCRIPTION
  Saves the F3 trace to smem buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_save_to_smem(void);

/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT_CONFIGS

DESCRIPTION
  Initializes the configuration values.
  This is designed as a singleton operation.  The first call will lock the
  configuration.  Thus allowing a priority chain, with highest priority
  method called first.

DEPENDENCIES
  This function is not mutex protected.  It is assumed it will only be
  called from serialized init functions (tmc/rcinit).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init_configs(uint32 control, uint32 detail);
//accessor for lock state
boolean diag_f3_trace_configs_locked(void);

#endif /* FEATURE_ERR_HAS_F3_TRACE */

#endif /* DIAG_F3_TRACEI_H */
