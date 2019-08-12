#ifndef HDRHMPI_H
#define HDRHMPI_H
/*===========================================================================

                 H D R   M E S S A G I N G   P R O T O C O L
           ( S I G N A L I N G   N E T W O R K   P R O T O C O L )
                           D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for HMP (SNP in IS-856).

Copyright (c) 2000,2001,2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003,2004,2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006           by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrhmpi.h_v   1.12   09 Apr 2003 09:18:56   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrhmpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/08   sju     Added support to bundle AC messages 
11/15/06   sju     Add new hdrhmp cmd: HDRHMP_RASM_CB_CMD.
11/15/06   sju     Added support to bundle ROR with CR and DOS messages.
09/14/06   sju     Added support to send ReservationOnRequest by using RASM 
                   when ReservationKKIdleState == 2. 
07/18/06   dna     Major RASM changes to allow reliable QoS call setup.
05/10/06   yll/dna Reliable Access Signaling Messages (RASM)
04/02/03   mpa     Added field for sending on current rtc only in
                   hdrhmp_send_msg_params_type.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Added field for stream in hdrhmp_send_msg_params_type.
06/06/01   dna     new param for hmp_send_msg()
05/03/01   dna     Re-naming for consistency.
05/01/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "dsm.h"
#include "rex.h"
#include "hdrhmp.h"
#include "hdrslp.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                                  HMP COMMANDS

--------------------------------------------------------------------------*/


/* Command types */
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRHMP_SEND_MSG_CMD=0, /* Send a message on behalf of another protocol */
  HDRHMP_RASM_ACKED_CMD, /* Notification of layer 3 ack to best effort msg */
  HDRHMP_RASM_CB_CMD     /* Notification of rasm cb */
} hdrhmp_cmd_name_enum_type;

/* HMP_SEND_MSG_CMD */

typedef struct
{
  /* RASM fields */
  boolean is_enabled; /* If RASM is active for this message */
  boolean is_rexmit; /* outcome_tag_ptr points to rasm_list entry */
  uint32 max_retries; /* How many times to try sending the best effort msg */
  uint32 retry_interval; /* Time to wait for layer 3 ACK, In milliseconds */
  uint32 overall_timeout; /* Time to give up on transmitting */
  uint32 transaction_id; /* For app to inform HMP that layer 3 ACK was rxed */
  boolean is_implicit_on_connection_setup; /* TRUE if ReservationKKIdleState == 2 */
  void *self_ptr; /* Pointer to rasm_list entry holding this command */
} hdrhmp_send_rasm_params_type;

typedef struct 
{                    
  /* Protocol sending the message */
  hdrhai_protocol_name_enum_type protocol_name; 

  /* This field is set to the Stream for which the protocol is sending the
  message. (Stream 0, 1, 2, or 3 ) */
  hdrhai_stream_enum_type stream;

  /* Channel the message must go out on (AC or RTC) */
  hdrhai_channel_enum_type chan; 

  boolean rtc_is_reliable; /* Whether or not the message is reliable */
  uint8 priority; /* Priority of the message, as defined in the HAI */
  dsm_item_type *msg_ptr; /* The message to be sent */

  hdrhai_protocol_instance_enum_type protocol_instance; /* in-use:in-config */

  /* Pointer to function called to announce outcome of transmission */
  hdrslp_outcome_cb_type outcome_cb;

  /* A pointer owned by sender, returned with the callback */
  void *outcome_tag_ptr; 

  /* Only send the RTC only message on the existing RTC (the AT will not try to 
     reopen the connection to send the message if the current connection is closed
     before we get to send out the message */
  boolean send_on_curr_rtc_only;

  hdrhmp_send_rasm_params_type rasm; /* Extra fields for RASM */

  uint8 bundle_mask; /* AC messages needed to be bundled in the same AC Capsule*/

} hdrhmp_send_msg_params_type;

/* HDRHMP_RASM_ACKED */
typedef struct 
{ 
  /* Protocol sending the message */
  hdrhai_protocol_name_enum_type protocol_name; 
  uint32 transaction_id; /* For app to inform HMP that layer 3 ACK was rxed */
} hdrhmp_rasm_acked_params_type;

typedef struct 
{ 
  /* Protocol sending the message */
  hdrhai_protocol_name_enum_type protocol_name; 
  void *data_ptr; /* A pointer owned by sender, returned with the callback */
  hdrerrno_enum_type status;
} hdrhmp_rasm_cb_params_type;

typedef union 
{                        /* Common command fields */
  hdrhmp_send_msg_params_type send_msg;  /* HMP_SEND_MSG_CMD */
  hdrhmp_rasm_acked_params_type rasm_acked; /* HDRHMP_RASM_ACKED */
  hdrhmp_rasm_cb_params_type rasm_cb; /* HDRHMP_RASM_CB */
} hdrhmp_cmd_params_union_type;

typedef struct
{
  hdrhmp_cmd_name_enum_type name;         /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
  hdrhmp_cmd_params_union_type params;    /* command parameters */
} hdrhmp_cmd_type;

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRHMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to HMP.

DEPENDENCIES
  None

PARAMETERS
  hmp_cmd_ptr - Pointer to the command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmp_process_cmd( hdrhmp_cmd_type * hmp_cmd_ptr );

/*===========================================================================

FUNCTION HDRHMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to HMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for HMP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmp_process_ind( hdrind_ind_name_enum_type ind_name,
       hdrind_ind_data_union_type * ind_data_ptr );

/*===========================================================================

FUNCTION HDRHMP_PROCESS_SIG

DESCRIPTION
  This function processes signals for HMP.  

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input 
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmp_process_sig( rex_sigs_type sig );

#endif /* HDRHMPI_H */
