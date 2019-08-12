#ifndef MSG_PKT_DEFS_H
#define MSG_PKT_DEFS_H

/*===========================================================================

                    Debug Message Service Packet Definitions

  OVERVIEW:    Packet definitions for the Debug Messaging diagnostic
               service.

               The debug message service provides printf()-style debugging
               with build-time and run-time granularity.  This granularity
               is achieved by assigning various technology areas to a 
               subsystem within the service.  Each subsystem defines up to
               32 categories (denoted by a 32-bit mask) by which debug 
               messages may be identified.  The external device may 
               configure each subsystem individually using a 32-bit mask
               for a each subsystem.

  
  TERMINOLOGY: 
               Subsystem ID (SSID):
               Unique identifer given to each subsystem within the target.
               
               Subsystem Mask:
               32-bit value with each bit denotes a category assigned by
               the technology area assigned to the subsystem ID.
               '1' denotes the category is enabled.
               '0' denotes the category is disabled.
               This mask is specified with the definition of each message.
               
               Build Mask:
               A 32-bit mask specified at compile-time to determine which
               messages are to be compiled in for each subsystem.  This
               build-time granularity is available to enable build managers
               to compile out certain categories of messages to save ROM.
               During compilation, a bitwise AND ('&') is performed between
               the Build Mask and the Subsystem Mask of each message.  If
               the '&' operation is non-zero, the message is compiled in.
               
               Run-Time Mask (RT Mask):
               A 32-bit mask specified at by the external device at run-time
               to configure messages for a subsystem.  At run-time, the 
               message's Subsystem Mask the subsystem's RT Mask are compared
               with a bitwise AND ('&').  If the result if non-zero, the 
               message service attempts to send the message.
               
               Dropped Message:
               If insuffient resources exist to send a message that is 
               enabled, the message is dropped.  The next successful message
               sent will contain a count of the number of messages dropped 
               since the last successful message.

   
  NOTE:        These packet definitions are part of an externalized 
               diagnostic command interface.  These definitions must not be
               changed.

Copyright (c) 2002, 2007, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                            Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/msg_pkt_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sg      Moved msg_desc_type, msg_hdr_type here
05/15/09   mad     Moved msg_desc_type, msg_hdr_type to msg.h. Included msg.h.
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/23/07   as      Enabled pragma pack support for WINCE targets
11/04/04   as      Added GW field for WCDMA phones in msg_ts_type
07/23/02   lad     Updated to reflect requirements changes and final 
                   implementation.
03/22/02   igt     Created file.
===========================================================================*/
#include "comdef.h"
#include "qw.h"

/* --------------------------------------------------------------------------
   Definitions and Declarations
   ----------------------------------------------------------------------- */


/*===========================================================================

PACKET       DIAG_EXT_MSG_CONFIG_F

SUBCOMMAND   MSG_EXT_SUBCMD_GET_RANGES

PURPOSE  Sent by the external device to query the SSID Ranges of the mobile.

===========================================================================*/
typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
}
msg_get_ssid_ranges_req_type;

typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint8 status;			/* Status of operation */
  uint8 rsvd;			/* Unused */

  uint32 range_cnt;		/* # of Subsystem ID ranges that follow */

  struct
  {
    uint16 ssid_first;		/* First Subsystem ID for range */
    uint16 ssid_last;		/* Last Subsystem ID for range */
  }
  ssid_ranges[1];
}
msg_get_ssid_ranges_rsp_type;

/*===========================================================================

PACKET       DIAG_EXT_MSG_CONFIG_F

SUBCOMMAND   MSG_EXT_SUBCMD_GET_BUILD_MASK
             MSG_EXT_SUBCMD_GET_RT_MASK

PURPOSE  Sent by the DM to query the build mask for a specific SSID

===========================================================================*/
typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint16 ssid_start;		/* Start of subsystem ID range */
  uint16 ssid_end;              /* End of subsystem ID range */
}
msg_get_mask_req_type;

typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint16 ssid_start;		/* Start of subsystem ID range */
  uint16 ssid_end;              /* End of subsystem ID range */

  uint8 status;			/* Status of operation */
  uint8 pad;       		/* unused */

  uint32 bld_mask[1];		/* Array of buildtime masks for subsystems in 
                                   range.  Array size is:
                                   'ssid_end - ssid_start + 1' */
}
msg_get_mask_rsp_type;

/*===========================================================================

PACKET       DIAG_EXT_MSG_CONFIG_F

SUBCOMMAND   MSG_EXT_SUBCMD_SET_RT_MASK

PURPOSE  Sent by the DM to SET the runtime masks for a range of SSIDs.

===========================================================================*/
typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint16 ssid_start;		/* Start of subsystem ID range */
  uint16 ssid_end;              /* End of subsystem ID range */

  uint16 pad;                   /* unused */

  uint32 rt_mask[1];		/* Array of runtime masks for subsystems in 
                                   range.  Array size is:
                                   'ssid_end - ssid_start + 1' */
}
msg_set_rt_mask_req_type;

typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint16 ssid_start;		/* Start of subsystem ID range */
  uint16 ssid_end;              /* End of subsystem ID range */

  uint8 status;			/* Status of operation */
  uint8 pad;                    /* unused */

  uint32 rt_mask[1];		/* Array of runtime masks for subsystems in 
                                   range.  Array size is:
                                   'ssid_end - ssid_start + 1' */
}
msg_set_rt_mask_rsp_type;

/*===========================================================================

PACKET       DIAG_EXT_MSG_CONFIG_F

SUBCOMMAND   MSG_EXT_SUBCMD_SET_ALL_RT_MASKS

PURPOSE  Sent by the DM to SET the runtime masks for ALL SSIDs

===========================================================================*/
typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */

  uint16 rsvd;			/* unused */

  uint32 rt_mask;		/* Run-time mask to set in all Subsystems */
}
msg_set_all_masks_req_type;

typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  
  uint8 status;			/* Status of operation */
  uint8 rsvd;                   /* unused */

  uint32 rt_mask;		/* New Run-time Mask for all SSIDs */
}
msg_set_all_masks_rsp_type;
/*---------------------------------------------------------------------------
  This structure is stored in ROM and is copied blindly by the phone.   
  The values for the fields of this structure are known at compile time. 
  So this is to be defined as a "static const" in the MACRO, so it ends up
  being defined and initialized at compile time for each and every message 
  in the software. This minimizes the amount of work to do during run time.
  
  So this structure is to be used in the "caller's" context. "Caller" is the
  client of the Message Services.
---------------------------------------------------------------------------*/
typedef struct
{
  uint16 line;			/* Line number in source file */
  uint16 ss_id;			/* Subsystem ID               */
  uint32 ss_mask;		/* Subsystem Mask             */
}
msg_desc_type;

/*---------------------------------------------------------------------------
  This is the message HEADER type. It contains the beginning fields of the 
  packet and is fixed length.  These fields are filled by the calling task.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 cmd_code;		/* Command code */
  uint8 ts_type;		/* Time stamp type */
  uint8 num_args;		/* Number of arguments in message */
  uint8 drop_cnt;		/* # dropped since last successful message */

  qword ts;			/* Time stamp */
}
msg_hdr_type;

/*===========================================================================

PACKET    DIAG_MSG_EXT_F

PURPOSE   Provide messages with NULL terminated filename and format fields,
          a variable number of 32-bit parameters, an originator-specific
          timestamp format, and filterability via subsystem ids and masks.

          For simplicity, only 'long' arguments are supported.  This packet
          allows for N arguments, though the macros support a finite number.
          
          'msg_ext_type' contains that packet format.
                    
==========================================================================*/

/*---------------------------------------------------------------------------
  This is the enumeration type for all  different timestamps supported.
  At this time timestamps are 8 bytes but that may change in the future
---------------------------------------------------------------------------*/
typedef enum
{
  MSG_TS_TYPE_CDMA_FULL = 0,
  MSG_TS_TYPE_WIN32,
  MSG_TS_TYPE_GW          /* WCDMA and GSM mode */
}
msg_ts_type;

/*---------------------------------------------------------------------------
  This is the structure that is used to represent the final structure that
  is sent to the external device.  'msg_ext_store_type' is expanded to this
  structure in DIAG task context at the time it is sent to the comm layer.
---------------------------------------------------------------------------*/
typedef struct
{
  msg_hdr_type hdr;
  msg_desc_type desc;

  uint32 args[1];		/* Array of long args, specified by
				   'hdr.num_args' */

  /* followed by NULL terminated format and file strings */
}
msg_ext_type;

/*===========================================================================

PACKET   DIAG_MSG_F
PURPOSE  Sent by the DM to request a buffered msg

============================================================================*/
#define MSG_FILE_LEN (12+1)	       /* Allows full DOS file name + NULL */
#define MSG_FMT_LEN  40		       /* Max len of format string + NULL */

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#pragma pack(push, msg_pkt_defs_h)
#pragma pack (1)
#endif

typedef PACK(struct)
{
  byte cmd_code;
  word msg_level;		/* highest type desired */
}
msg_legacy_req_type;

typedef PACK(struct)
{
  byte cmd_code;
  word qty;			/* number of msg packets in the msg buffer
				   including the msg packet returned in this
				   response  -- if this field is 0, no msg
				   packet is included in this response       */
  dword drop_cnt;		/* number of msgs dropped by error services  */
  dword total_msgs;		/* total number of msgs that have occured    */
  byte level;			/* Severity level / Priority of this message */
  char file[MSG_FILE_LEN];
  /* Holds source file name                    */
  word line;			/* Line number in source file                */
  char fmt[MSG_FMT_LEN];
  /* printf style format string                */
  dword code1;			/* parameters to go with the format string   */
  dword code2;
  dword code3;
  qword time;			/* Time at which message was generated       */
}
msg_legacy_rsp_type;

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#pragma pack (pop, msg_pkt_defs_h)
#endif

#endif		   /* MSG_PKT_DEFS_H */
