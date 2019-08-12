/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           L O C A T I O N   U P D A T E   P R O T O C O L

GENERAL DESCRIPTION
  The Link Update Protocol (LUP) defines location update procedures and
  messages for mobility management for RLP and negotiates a PDSN selection
  method and provides data required for the PDSN selection.

EXTERNALIZED FUNCTIONS (Global)

  hdrlup_msg_cb() - Callback registered with HMP to deliver messages
  hdrlup_ind_cb() - Callback registered for delivery of indications
  hdrlup_activate() - Command function to activate LUP
  hdrlup_deactivate() - Command function to deactivate LUP
  hdrlup_configure() - Command function to tell LUP to perform configuration

EXTERNALIZED FUNCTIONS (Regional)
  hdrlup_process_cmd() - Function called by hdrmc task to process
                         a command for LUP
  hdrlup_process_ind() - Function called by hdrmc task to process
                         an indication for LUP
  hdrlup_process_msg() - Function called by hdrmc task to process
                         a message for LUP
  hdrlup_process_sig() - Function called by hdrmc task to process
                         a signal for LUP

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrlup.c_v   1.19   24 Apr 2003 17:49:24   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrlup.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/21/10   cnx     Fixed compiler warnings. 
04/12/10   sju     Removed FEATURE_HDR_CP_9K_BRINGUP
01/12/10   sju     Added FEATURE_HDR_CP_9K_BRINGUP
12/20/09   pba     Send ULN only when data session is transferable
12/09/09   ljl     Fixed no data in TAP call.
07/30/09   wsh     Fixed LUP/DOS related message not being processed in MMPA
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/10/08   smd     Replaced assert.h with amssassert.h
09/11/07   wsh     Removed lint high warnings
06/04/07   mpa     Added support for location notification status update to cm
04/13/07   pba     Added check for session change reason 
                   HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS.
07/05/06   mpa     Use SCM_SESSION_CHANGED ind payload to determine session status
06/20/06   mpa     Fixed HDR_QOS featurization issue.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
06/05/06   pba     Added support to store session data in EFS.
05/10/06   yll/ksu EMPA: octet stream, octet sequencing
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/04/06   dna     Update to support StorageBLOB
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
03/09/05   pba     Lint cleanup
12/10/04   hal     Added header file hdrutil.h.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
09/09/04   mpa     Preliminary QOS support.
                   Move Events handling to HDRPAC.
01/28/03   mpa     LUP must check last and current data networks before
                   deciding to send a LocationNotification.
10/08/03   mpa     Added hdrlup_get_location()
10/03/03   mpa     Use new ds707 interface to check for data session state.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDR SSID
07/23/03   mpa     Mainlined MSM6500 baseline
07/01/03   mpa     Update location after session is opened if we have a dormant
                   data session on 1X
05/15/03   mpa     Added FEATURE_HDR_ALN
05/07/03   mpa     Use CM wrapper functions to check for dormant data session
04/22/03   mpa     Don't NULL LocNotif info when session closes
                   Only send LocNotif when opening session if dormant data
                   call wasn't on 1X system (handled by DS already)
06/26/02   mpa     Process ind cb inline if called from within the same task.
01/30/02   sh      Fixed incorrect SID/NID info sent in Location Notification
09/25/01   vas     Free the dsm item containing message after processing it.
09/25/01   dna     Removed transaction ID from location request/response
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Changes for msgs to be sent for correct streams
07/10/01   dna     Change LUP to not do AT initiated negotiation
06/06/01   dna     new param for hmp_send_msg()
05/01/01   dna     Call to hdrlup_process_sig() -> hdrlupc_process_sig()
03/25/01   dna     Created module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "cmxll.h"

#include "hdrdebug.h"
#include "hdrutil.h"
#include "err.h"
#include "amssassert.h"

#include "hdrhai.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrlup.h"
#include "hdrlupi.h"
#include "hdrlupc.h"
#include "hdrtrace.h"
#include "hdrdsm.h"
#include "mccsyobj.h"
#include "ds707_extif.h"
#include "hdrscmdb.h"
#include "hdrmrlpc.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/*--------------------
     Location
--------------------*/
/* Unpacking Location Assignment */
#define HDRLUP_GET_LOC_ASSIGN_TRANS_ID( msg_ptr, val_ptr ) \
              HDRBIT_UNPACK8( msg_ptr, val_ptr, 8, 8 )

#define HDRLUP_GET_LOC_ASSIGN_LOC_TYPE( msg_ptr, val_ptr ) \
              HDRBIT_UNPACK8( msg_ptr, val_ptr, 16, 8 )

#define HDRLUP_GET_LOC_ASSIGN_LOC_LEN( msg_ptr, val_ptr ) \
             HDRBIT_UNPACK8( msg_ptr, val_ptr, 24, 8 )

#define HDRLUP_GET_LOC_ASSIGN_LOC_VAL_BYTE( msg_ptr, val_ptr, offset ) \
        HDRBIT_UNPACK8( msg_ptr, val_ptr, (uint16)(32 + ((offset) * 8)), 8 )

/* Packing Location Response */
#define HDRLUP_PACK_LOC_RESP_MSG_ID( msg_ptr, val_ptr ) \
               HDRBIT_PACK8( msg_ptr, val_ptr, 0, 8 )

#define HDRLUP_PACK_LOC_RESP_LOC_TYPE( msg_ptr, val_ptr ) \
                 HDRBIT_PACK8( msg_ptr, val_ptr, 8, 8 )

#define HDRLUP_PACK_LOC_RESP_LOC_LEN( msg_ptr, val_ptr ) \
                HDRBIT_PACK8( msg_ptr, val_ptr, 16, 8 )

#define HDRLUP_PACK_LOC_RESP_LOC_VAL_BYTE( msg_ptr, val_ptr, offset ) \
        HDRBIT_PACK8( msg_ptr, val_ptr, (uint16)(24 + ((offset) * 8)), 8 )

/* Packing Location Complete */
#define HDRLUP_PACK_LOC_COMP_MSG_ID( msg_ptr, val_ptr ) \
               HDRBIT_PACK8( msg_ptr, val_ptr, 0, 8 )

#define HDRLUP_PACK_LOC_COMP_TRANS_ID( msg_ptr, val_ptr ) \
                 HDRBIT_PACK8( msg_ptr, val_ptr, 8, 8 )
            
#ifdef FEATURE_HDR_QOS
/*--------------------
  Service Network ID
--------------------*/
/* Unpacking Service Network ID Assignment */
#define HDRLUP_GET_SNID_ASSIGN_TRANS_ID( msg_ptr, val_ptr ) \
          HDRBIT_UNPACK8( msg_ptr, val_ptr, 8, 8 )

#define HDRLUP_GET_SNID_ASSIGN_TYPE( msg_ptr, val_ptr ) \
          HDRBIT_UNPACK16( msg_ptr, val_ptr, 16, 16 )

#define HDRLUP_GET_SNID_ASSIGN_LEN( msg_ptr, val_ptr ) \
          HDRBIT_UNPACK8( msg_ptr, val_ptr, 32, 8 )

#define HDRLUP_GET_SNID_ASSIGN_BYTE( msg_ptr, val_ptr, offset ) \
        HDRBIT_UNPACK8( msg_ptr, val_ptr, (uint16)(40 + ((offset) * 8)), 8 )

/* Packing Service Network ID Notification */
#define HDRLUP_PACK_SNID_NOTIF_MSG_ID( msg_ptr, val_ptr ) \
               HDRBIT_PACK8( msg_ptr, val_ptr, 0, 8 )

#define HDRLUP_PACK_SNID_NOTIF_TYPE( msg_ptr, val_ptr ) \
                 HDRBIT_PACK16( msg_ptr, val_ptr, 8, 16 )

#define HDRLUP_PACK_SNID_NOTIF_LEN( msg_ptr, val_ptr ) \
                 HDRBIT_PACK8( msg_ptr, val_ptr, 24, 8 )

#define HDRLUP_PACK_SNID_NOTIF_BYTE( msg_ptr, val_ptr, offset ) \
        HDRBIT_PACK8( msg_ptr, val_ptr, (uint16)(32 + ((offset) * 8)), 8 )

/* Packing Service Network ID Complete */
#define HDRLUP_PACK_SNID_COMP_MSG_ID( msg_ptr, val_ptr ) \
               HDRBIT_PACK8( msg_ptr, val_ptr, 0, 8 )

#define HDRLUP_PACK_SNID_COMP_TRANS_ID( msg_ptr, val_ptr ) \
                 HDRBIT_PACK8( msg_ptr, val_ptr, 8, 8 )
#endif /* FEATURE_HDR_QOS */

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING

===========================================================================*/


/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* See IS-856 table 3.3.6.3-2 for location type 0x01 */

#define HDRLUP_LOC_TYPE_NONE 0x00
#define HDRLUP_LOC_TYPE_IS2000 0x01

static hdrlup_session_info_type hdrlup;

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to LUP.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlup_msg_cb (dsm_item_type * msg_ptr)
{
  HDR_MSG_PROT (MSG_LEGACY_LOW, "hdrlup_msg_cb");

  /* This protocol runs in the HDRMC task. */
  hdrmc_queue_msg (HDRHAI_LOCATION_UPDATE_PROTOCOL, HDRHAI_IN_USE, msg_ptr);
} /* hdrlup_msg_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRLUP_SEND_LOC_NOTIF_STATUS_TO_CM

DESCRIPTION
  This function sends a location nofication update command to the CM.

DEPENDENCIES
  None.

PARAMETERS
  ln_status - Whether location notification updates succeeded 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrlup_send_loc_notif_update_to_cm
(
  sys_hdr_location_notification_status_e_type ln_status
)
{
    cm_mc_rpt_type *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->hdr.cmd = CM_HDR_LOCATION_NOTIFICATION_STATUS_F;
      cmd_ptr->hdr.task_ptr = NULL;
      cmd_ptr->ln_status_info.ln_status = ln_status;

      cm_mc_rpt( cmd_ptr );

      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  
                      "Sent CM_HDR_LOCATION_NOTIFICATION_STATUS_F (%d)",
                      ln_status);
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

} /* hdrlup_send_loc_notif_update_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_LOC_NOTIF_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  location notification message.

DEPENDENCIES
  None

PARAMETERS
  data_ptr    - user data, whether CM/data need to be notified of the outcome
  err_val     - message transmission status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrlup_loc_notif_outcome_cb
(
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{

  /* User payload - whether or not to notify CM/data of the outcome of the 
     location notification update.  
     
     Note that this is just a notification that the access probe for the
     location notification went through. */
  boolean send_loc_notif_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  send_loc_notif_status = (boolean)((dword)data_ptr);

  if (err_val == E_SUCCESS)
  {
    HDRTRACE_TXMSG_ACKED( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                          HDRLUP_LOC_RESP_MSG, 0);

    if (send_loc_notif_status)
    {
      hdrlup_send_loc_notif_update_to_cm(SYS_HDR_LOCATION_NOTIFICATION_STATUS_SUCCESSFUL);
    }
  }
  else
  { 
    HDRTRACE_TXMSG_FAILED( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                           HDRLUP_LOC_RESP_MSG, 0);

    if (send_loc_notif_status)
    {
      hdrlup_send_loc_notif_update_to_cm(SYS_HDR_LOCATION_NOTIFICATION_STATUS_FAILED);
    }

    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   "LocationNotification xmit failure (%d), notify (%d)",
                   err_val, send_loc_notif_status);
  }

} /* hdrlup_loc_notif_outcome_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Location Update Protocol.

DEPENDENCIES
  None 

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void hdrlup_powerup_init ( void )
{

  /* Get LUP's current session parameters from SCMDB */
  hdrscmdb_get_lup_session_info(&hdrlup);

} /* hdrlup_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_SEND_LOC_RESP_MSG

DESCRIPTION
  This function builds and sends a location response message.

DEPENDENCIES
  None

PARAMETERS
  stream_id - Stream for which the location request message was received.
            Used in formulating response message.
  send_loc_notif_status
       TRUE - Notify CM/data of the location notification update outcome
       FALSE - No need to notify CM/data of the location udpate outcome

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_send_loc_resp_msg
(
  hdrhai_stream_enum_type stream_id,
  boolean send_loc_notif_status
)
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
  int i; /* Loop through bytes of location value */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRLUP_PACK_LOC_RESP_MSG_ID( send_msg_ptr, HDRLUP_LOC_RESP_MSG );
  if (hdrlup.location.is_valid)
  {
    HDRLUP_PACK_LOC_RESP_LOC_TYPE( send_msg_ptr, hdrlup.location.type );
    HDRLUP_PACK_LOC_RESP_LOC_LEN( send_msg_ptr, hdrlup.location.length );
    for (i = 0; i < hdrlup.location.length; i++ )
    {
      HDRLUP_PACK_LOC_RESP_LOC_VAL_BYTE( send_msg_ptr,
                                 hdrlup.location.value[i], i );
    }
  }
  else
  {
    /* Zero is designated to mean, "we don't know our location" */
    HDRLUP_PACK_LOC_RESP_LOC_TYPE( send_msg_ptr, HDRLUP_LOC_TYPE_NONE );
  }
  /* Send location response on RTC if available, otherwise AC.
   * It is sent reliably if sent on the RTC
   */
  hdrhmp_app_send_msg( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                   stream_id,
                   HDRHAI_AC_OR_RTC,
                   TRUE,
                   HDRLUP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   hdrlup_loc_notif_outcome_cb,
                   (void*) ((dword) send_loc_notif_status ));

} /* hdrlup_send_loc_resp_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_SEND_LOC_COMP_MSG

DESCRIPTION
  This function builds and sends a location complete message.

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the corresponding location request message.
  stream_id - Stream for which the location assignment message was received.
            Used in formulating response message.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_send_loc_comp_msg
(
  uint8 trans_id,
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRLUP_PACK_LOC_COMP_MSG_ID( send_msg_ptr, HDRLUP_LOC_COMP_MSG );
  HDRLUP_PACK_LOC_COMP_TRANS_ID( send_msg_ptr, trans_id );

  /* Send location complete on RTC if available, otherwise AC.
   * It is sent reliably if sent on the RTC
   */
  hdrhmp_app_send_msg( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                   stream_id,
                   HDRHAI_AC_OR_RTC,
                   TRUE,
                   HDRLUP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);
} /* hdrlup_send_loc_comp_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_UPDATE_LOCATION

DESCRIPTION
  This function updates the current location from a received location
  assignment message

DEPENDENCIES
  None

PARAMETERS
  loc_assign_msg_ptr - Pointer to DSM item chain holding received location
                       assignment message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_update_location
(
  dsm_item_type *loc_assign_msg_ptr
)
{
  int i; /* Loop through bytes of location value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  err = (int) HDRLUP_GET_LOC_ASSIGN_LOC_TYPE( loc_assign_msg_ptr,
                                       &hdrlup.location.type );
  err |= (int) HDRLUP_GET_LOC_ASSIGN_LOC_LEN( loc_assign_msg_ptr,
                                       &hdrlup.location.length);
  if ((hdrlup.location.type != HDRLUP_LOC_TYPE_IS2000) ||
      (hdrlup.location.length != HDRLUP_LOC_TYPE_IS2000_LEN))
  {
    /* An unknown location type was given or an invalid length was given
     * for a known location type.  This forces our location to be invalid.
     */
    hdrlup.location.is_valid = FALSE;
  }
  else
  {
    hdrlup.location.is_valid = TRUE;

    for (i = 0; i < hdrlup.location.length; i++ )
    {
      err |= (int) HDRLUP_GET_LOC_ASSIGN_LOC_VAL_BYTE( loc_assign_msg_ptr,
                                       &hdrlup.location.value[i], i );
    }
  }

  if (err)
  {
    ERR("Error unpacking loc assign",0,0,0);
    hdrlup.location.is_valid = FALSE;
  }
} /* hdrlup_update_location() */

/* <EJECT> */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION HDRLUP_SEND_SNID_NOTIF_MSG

DESCRIPTION
  This function builds and sends a service network ID notification message.

DEPENDENCIES
  None

PARAMETERS
  stream_id - Stream for which the snid request message was received.
              Used in formulating response message.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_send_snid_notif_msg
(
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */
  int i; /* Loop through bytes of location value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRLUP_PACK_SNID_NOTIF_MSG_ID( send_msg_ptr, HDRLUP_SNID_NOTIF_MSG );
  if (hdrlup.snid.is_valid)
  {
    HDRLUP_PACK_SNID_NOTIF_TYPE( send_msg_ptr, hdrlup.snid.type );
    HDRLUP_PACK_SNID_NOTIF_LEN( send_msg_ptr, hdrlup.snid.length );

    for (i = 0; i < hdrlup.snid.length; i++ )
    {
      HDRLUP_PACK_SNID_NOTIF_BYTE( send_msg_ptr,
                                   hdrlup.snid.value[i], i );
    }
  }
  else
  {
    /* else - AT does not know StorageBLOB */
    HDRLUP_PACK_SNID_NOTIF_TYPE( send_msg_ptr, 0 );
    HDRLUP_PACK_SNID_NOTIF_LEN( send_msg_ptr, 0 );
  }
  
  /* Send service network ID notification on RTC if available, otherwise AC.
   * It is sent reliably if sent on the RTC
   */
  hdrhmp_app_send_msg( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                   stream_id,
                   HDRHAI_AC_OR_RTC,
                   FALSE, /* Best Effort */
                   HDRLUP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);
} /* hdrlup_send_snid_notif_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_SEND_SNID_COMP_MSG

DESCRIPTION
  This function builds and sends a snid complete message.

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the corresponding snid request message.
  stream_id - Stream for which the snid assignment message was received.
            Used in formulating response message.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_send_snid_comp_msg
(
  uint8 trans_id,
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *send_msg_ptr; /* dsm buffer for sent message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get new buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack fields */
  HDRLUP_PACK_SNID_COMP_MSG_ID( send_msg_ptr, HDRLUP_SNID_COMP_MSG );
  HDRLUP_PACK_SNID_COMP_TRANS_ID( send_msg_ptr, trans_id );

  /* Send snid complete on RTC if available, otherwise AC.
   * It is sent reliably if sent on the RTC
   */
  hdrhmp_app_send_msg( HDRHAI_LOCATION_UPDATE_PROTOCOL,
                   stream_id,
                   HDRHAI_AC_OR_RTC,
                   FALSE, /* Best Effort */
                   HDRLUP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);
} /* hdrlup_send_snid_comp_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_UPDATE_SNID

DESCRIPTION
  This function updates the current service network ID from a received 
  Service Network ID assignment message

DEPENDENCIES
  None

PARAMETERS
  snid_assign_msg_ptr - Pointer to DSM item chain holding received snid 
                        assignment message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void hdrlup_update_snid
(
  dsm_item_type *snid_assign_msg_ptr
)
{
  int i; /* Loop through bytes of snid value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  err = (int) HDRLUP_GET_SNID_ASSIGN_TYPE( snid_assign_msg_ptr,
                                           &hdrlup.snid.type);

  err = (int) HDRLUP_GET_SNID_ASSIGN_LEN( snid_assign_msg_ptr,
                                       &hdrlup.snid.length);

  /* No need to range check hdrlup.snid.length because the data type range is
     the same as the possible valid values */
  for (i = 0; i < hdrlup.snid.length; i++)
  {
    err |= (int) HDRLUP_GET_SNID_ASSIGN_BYTE( snid_assign_msg_ptr,
                                              &hdrlup.snid.value[i], 
                                              i );
  }
  hdrlup.snid.is_valid = TRUE;

  if (err)
  {
    ERR("Error unpacking snid assign",0,0,0);
    hdrlup.snid.is_valid = FALSE;
  }

} /* hdrlup_update_snid() */
#endif /* FEATURE_HDR_QOS*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to LUP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlup_ind_cb (hdrind_ind_name_enum_type ind_name, void *ind_data)
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "hdrlup_ind_cb %d", ind_name);

  /*-------------------------------------------------------------------------
  If the ind callback called from within the same task, process it
  immediately, else queue it & tag it for the location update protocol.
  -------------------------------------------------------------------------*/
  if (rex_self() == HDRLUP_TASK_PTR)
  {
    hdrlup_process_ind( ind_name, (hdrind_ind_data_union_type*)ind_data );
  }
  else
  {
    hdrmc_queue_ind (HDRHAI_LOCATION_UPDATE_PROTOCOL, ind_name, ind_data);
  }

} /* hdrlup_ind_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to LUP.

DEPENDENCIES
  None

PARAMETERS
  lup_cmd_ptr - Pointer to the command for LUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_process_cmd (hdrlup_cmd_type *lup_cmd_ptr)
{
    uint16 sid; /* SID of current 1x data session */
    uint16 nid; /* NID of current 1x data session */
    hdrhai_stream_enum_type stream_id; /* ID of stream to Service Network */
    hdrhai_app_subtype_enum_type app_subtype; /* Application Subtype */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDRTRACE_CMD( HDRHAI_LOCATION_UPDATE_PROTOCOL, lup_cmd_ptr->name,
    lup_cmd_ptr->sender, 0 );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch (lup_cmd_ptr->name)
  {
    case HDRLUP_UPDATE_LOC_CMD:

#ifdef FEATURE_HDR_ALN
      /* ANID should be set to zero if unsolicited location notification is
         not supported.  This makes sure that the location will be updated
         if we move from 1X->EVDO->1X. Since we will not get a location
         assignment on the EVDO side to update our location, we need to let
         the 1X system know that we moved away.  */
      if (!hdrlupc_unsolicited_is_allowed())
      {
        hdrlup.location.is_valid = FALSE;
        break;
      }
#endif /* FEATURE_HDR_ALN */

      hdrlup.location.is_valid = TRUE;
      hdrlup.location.type = HDRLUP_LOC_TYPE_IS2000;
      hdrlup.location.length = HDRLUP_LOC_TYPE_IS2000_LEN;

      /*
       *  --------------- - ---------------- --------
       * |      SID      |R|       NID      |  PZID  |
       *  --------------- - ---------------- --------
       *         15       1         16           8
       */
//     b_packw( (mccsyobj_get_sid() < 1), hdrlup.location.value, 0, 16 );
//     b_packw( mccsyobj_get_nid(), hdrlup.location.value, 16, 16 );
//     b_packb( mccsyobj_get_pzid(), hdrlup.location.value, 32, 8 );

      sid = mccsyobj_get_sid() << 1;
      hdrlup.location.value[0] = (uint8) ((sid & 0xFF00) >> 8);
      hdrlup.location.value[1] = (uint8) ( sid & 0x00FF );

      nid = mccsyobj_get_nid();
      hdrlup.location.value[2] = (uint8) ((nid & 0xFF00) >> 8);
      hdrlup.location.value[3] = (uint8) (nid & 0x00FF);

      hdrlup.location.value[4] = mccsyobj_get_pzid();

      break;

    case HDRLUP_SEND_LOC_CMD:
      /* If the Default SN stream is negotiated, then we want to send the
       * Location notification on the default SN stream. If not, but the MRLP SN
       * stream is negotiated, then we send the Location nofication on the MRLP
       * SN stream  */
      if(hdrstream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
      {
        app_subtype = HDRHAI_DEF_PACKET_APP_SN;
      }
#ifdef FEATURE_HDR_QOS
      else if(hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
      {
        app_subtype = HDRHAI_MULTIFLOW_PACKET_APP_SN;
      }
      else if(hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
      {
        app_subtype = HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN;
      }
      else if(hdrstream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        app_subtype = HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN;
      }
#endif /* FEATURE_HDR_QOS */
      else
      {
        app_subtype = HDRHAI_APP_STREAM_NOT_USED;
      }

      if(app_subtype != HDRHAI_APP_STREAM_NOT_USED)
      {
        if ( hdrstream_get_stream(app_subtype, &stream_id) ) 
        {
          if (hdrlupc_unsolicited_is_allowed())
          {
             /* We already have a dormant data call.
              * Only send our location if it is allowed */
  
            /* If the location notification send request came from HDRMC, it was
               due to 1X->HDR handup origination (DRS=0 or 1).  In this case, we
               should notify CM/data of the outcome so data could manage PPP 
               resyncs based on whether the LocationNotification made it to the 
               base station.
  
               Note: Even if the probe goes through, it is still not an absolute
               guarantee that the base station moved it's session from 1X to HDR.
               In the worst case scenario, we would get an extra PPP resync 
               generated by the AT. */
            if (lup_cmd_ptr->sender == HDRHAI_HDRMC)
            {
              /* Send location notification and notify CM/data of the
                 outcome */
              hdrlup_send_loc_resp_msg( stream_id, TRUE );
            }
            else
            {
              /* Send location notification but do not notify CM/data
                 of the outcome */
              hdrlup_send_loc_resp_msg( stream_id, FALSE );
            }
          }
#ifdef FEATURE_HDR_ALN
          else
          {
            /* Notify DS that the unsolicited location notification
               wasn't sent because it wasn't allowed */
            hdrind_give_ind(HDRIND_LUP_ALN_TRIGGER, NULL);
          }
#endif /* FEATURE_HDR_ALN */
        }
      }

      break;

    default:
      ERR_FATAL( "Unknown command", 0, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrlup_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_PROCESS_IND

DESCRIPTION
  This function processes indications given to LUP.

DEPENDENCIES
  Session Configuration already updated through hdrpacc.

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_process_ind (hdrind_ind_name_enum_type ind_name,
        hdrind_ind_data_union_type * ind_data_ptr)
{
  ds707_extif_system_info curr_sys_info = DS707_EXTIF_SYSTEM_NON_EPC;
   /* Current system information */
/*-------------------------------------------------------------------------*/

  HDRTRACE_IND( HDRHAI_LOCATION_UPDATE_PROTOCOL, ind_name, 0 );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      /* Get LUP's current session parameters from SCMDB */
      hdrscmdb_get_lup_session_info(&hdrlup);

      if ( (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_NORMAL) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS)
         )
      {
        /* When we re-establish a session we can't be sure the current BSC
         * is aware of our dormant data session because we may have crossed
         * a subnet boundary without sending a UATI request.
         *
         * If the data session was on 1X, update the location in case the
         * base station requests the location before the fake orig is sent
         * else make sure the data session is transferable and then send LN.
         */

        if (
#ifdef FEATURE_HDR_QOS
             (hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
             (hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) ||
             (hdrstream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) ||
#endif /* FEATURE_HDR_QOS */
             (hdrstream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
           )
        {
#ifdef FEATURE_EHRPD
          if (hdrmrlpc_current_personality_is_ehrpd())
          {
            curr_sys_info = DS707_EXTIF_SYSTEM_EPC;
          }
#endif /* FEATURE_EHRPD */

          /* Last data network = 1X, current = either */
          if ( ds707_extif_was_last_data_network_cdma() )
          {
            hdrlup_update_loc( HDRHAI_LOCATION_UPDATE_PROTOCOL );
          }
          else if ( ds707_extif_is_pkt_data_sess_transferable(curr_sys_info) )
          {
            hdrlup_send_loc(HDRHAI_LOCATION_UPDATE_PROTOCOL);
          }
        }
      }
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrlup_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_PROCESS_MSG

DESCRIPTION
  This function processes messages for LUP.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_process_msg
(
  dsm_item_type * msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;     /* Message identifier */
  uint8 trans_id;   /* Transaction ID of received message, if appropraite */
  uint8 protocol_id; /* Protocol Id of received message */

  /* Stream associated with message */
  hdrhai_stream_enum_type stream_id = HDRHAI_STREAM0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /* MessageID is the first uint8 */
  if (dsm_extract (msg_ptr, 0, &msg_id, 1) == 0)
  {
    ERR ("Could not extract msg_id", 0, 0, 0);
    dsm_free_packet( &msg_ptr );
    return;
  }

  /* Get the protocol id associated with the message. This protocol id is used
     in sending the response message */
  protocol_id = (uint8)HDRDSM_GET_PROT_ID(msg_ptr) ;

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch( protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;

    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;

    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;

    default:
      /* Should never happen, since Stream 0 message should never be routed
      ** to LUP */
      ERR_FATAL("Invalid Protocol ID %d", protocol_id, 0, 0 );
      break;
  }

  HDRTRACE_MSG( HDRHAI_LOCATION_UPDATE_PROTOCOL, msg_id, 0 );
  if (protocol_instance == HDRHAI_IN_USE)
  {
    switch (msg_id)
    {
      case HDRLUP_LOC_REQ_MSG:
        /* Send location notif. but do not notify CM/data of the outcome */           
        hdrlup_send_loc_resp_msg( stream_id, FALSE );
        break;

      case HDRLUP_LOC_ASSIGN_MSG:
        if (HDRLUP_GET_LOC_ASSIGN_TRANS_ID( msg_ptr, &trans_id ) == E_SUCCESS)
        {
          hdrlup_send_loc_comp_msg( trans_id, stream_id );
        }

        hdrlup_update_location( msg_ptr );

        /* Update the LUP's session parameters in SCMDB */ 
        hdrscmdb_set_lup_session_info(&hdrlup);
        break;

#ifdef FEATURE_HDR_QOS
      case HDRLUP_SNID_REQ_MSG:
        hdrlup_send_snid_notif_msg( stream_id );
        break;

      case HDRLUP_SNID_ASSIGN_MSG:
        if (HDRLUP_GET_SNID_ASSIGN_TRANS_ID( msg_ptr, & trans_id) == E_SUCCESS)
        {
          hdrlup_send_snid_comp_msg( trans_id, stream_id );
        }
        hdrlup_update_snid( msg_ptr );

        /* Update the LUP's session parameters in SCMDB */ 
        hdrscmdb_set_lup_session_info(&hdrlup);
        break;

#endif /* FEATURE_HDR_QOS */
      default:
        ERR( "Unknown in use message %d", msg_id, 0, 0 );
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    ERR( "Unknown in config message %d", msg_id, 0, 0 );
  }

  /*-----------------------------------------------------------------------
  Free the dsm item containing the message
  -----------------------------------------------------------------------*/
  dsm_free_packet( &msg_ptr );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrlup_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_SEND_LOC

DESCRIPTION
  This function commands LUP to send an unsolicited LocationNotification
  message, if allowed by the current configuration.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_send_loc
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrlup_cmd_type hdrlup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the lup_cmd_type for the send location command
  ---------------------------------------------------------------------------*/
  hdrlup_cmd.name = HDRLUP_SEND_LOC_CMD;
  hdrlup_cmd.sender = sender;

  if (rex_self() == HDRLUP_TASK_PTR)
  {
    hdrlup_process_cmd( &hdrlup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
    Currently no other task ever commands LUP, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("send_location() given from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_LOCATION_UPDATE_PROTOCOL, &hdrlup_cmd ,
                     sizeof(hdrlup_cmd_type) );
  }
} /* hdrlup_send_loc() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_UPDATE_LOC

DESCRIPTION
  This function commands LUP to update its location to the current
  SID/NID/PZID in use by the dormant 1x data session.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_update_loc
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrlup_cmd_type hdrlup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the lup_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrlup_cmd.name = HDRLUP_UPDATE_LOC_CMD;
  hdrlup_cmd.sender = sender;

  if (rex_self() == HDRLUP_TASK_PTR)
  {
    hdrlup_process_cmd( &hdrlup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
    Currently no other task ever commands LUP, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("update_location() given from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_LOCATION_UPDATE_PROTOCOL, &hdrlup_cmd ,
                     sizeof(hdrlup_cmd_type) );
  }
} /* hdrlup_update_loc() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_GET_LOCATION

DESCRIPTION
  This function is used to query the current SID/NID/PZID location if valid.
  It is needed by 1X to update its location when moving a dormant data session
  from EVDO to 1X.

DEPENDENCIES
  Should not be queried directly outside of the EVDO protocols.

PARAMETERS
  sid - current Sector ID
  nid - current Network ID
  pzid - current Packet Zone ID

RETURN VALUE
  TRUE - the current location value is valid and compatible with IS2000
  FALSE - the current location is either unavailable or not compatible with
          the IS2000 format (SID/NID/PZID).

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrlup_get_location
(
  uint16 *sid,
  uint16 *nid,
  uint8 *pzid
)
{
   if (hdrlup.location.is_valid &&
       hdrlup.location.type == HDRLUP_LOC_TYPE_IS2000)
   {
     /*
      *  --------------- - ---------------- --------
      * |      SID      |R|       NID      |  PZID  |
      *  --------------- - ---------------- --------
      *         15       1         16           8
      */

     *sid = (( ((uint16) hdrlup.location.value[0]) << 8 ) |
             (  (uint16) hdrlup.location.value[1]       )) >> 1;


     *nid = ( ((uint16) hdrlup.location.value[2]) << 8 ) |
               (uint16) hdrlup.location.value[3];

     *pzid = hdrlup.location.value[4];


     HDR_MSG_PROT_3(MSG_LEGACY_MED,"Report HDR sid: %d, nid: %d, pzid: %d",
                    *sid, *nid, *pzid);
     return TRUE;
   }

   return FALSE;
} /* hdrlup_get_location() */

/*===========================================================================

FUNCTION HDRLUP_SET_SESSION_INFO_TO_DEFAULT

DESCRIPTION
  This function sets the Location Update Protocol's session parameters
  to default values. 

DEPENDENCIES
  None

PARAMETERS
  curr_config - Pointer to Location Update Protocol's session parameter
                structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlup_set_session_info_to_default(hdrlup_session_info_type *curr_cfg)
{
  curr_cfg->location.is_valid = FALSE;
#ifdef FEATURE_HDR_QOS
  curr_cfg->snid.is_valid = FALSE;
#endif /* FEATURE_HDR_QOS */
} /* hdrlup_set_session_info_to_default() */

