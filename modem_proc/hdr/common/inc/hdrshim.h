#ifndef HDRSHIM_H
#define HDRSHIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   H D R   S H I M   H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for using the HDR 
  Shim layer for HDR functions.
  
Copyright (c) 2000-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/inc/hdrshim.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/09   suren   Fixes for MOB build break.
05/27/09   srf     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrmc_v.h"
#include "hdrerrno.h"
#include "hdrind.h"
#include "hdrovhd.h"
#include "hdrmod.h"
#include "hdridles.h"
#include "hdrdos.h"
#include "hdridle.h"
#include "qw.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRSHIM_HDRMC_CMD

DESCRIPTION
  This is the outside world's interface to the HDRMC task and subtasks.
  this function takes an already filled out hdrmc_msg_type and places it
  on the hdrmc command queue.  The function returns to the caller after
  setting the status to busy indicaton.  This does _not_ mean that the
  command has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The HDRMC task must have been started up already.  All of the items in
  the hdrmc_cmd_type must be already set.

RETURN VALUE
  None

SIDE EFFECTS
  The hdrmc_cmd_type is placed on the HDRMC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/
void hdrshim_hdrmc_cmd
(
  HDRMC_CMD_TYPE  *msg_ptr   /* Pointer to MC message buffer */
);

/*===========================================================================

FUNCTION HDRSHIM_HDRMC_GET_CMD_BUF

DESCRIPTION
  This function allocates a HDRMC task command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to allocated CM task command buffer if allocation succeed.
  NULL if allocation failed.

SIDE EFFECTS
  None.
===========================================================================*/
extern HDRMC_CMD_TYPE *hdrshim_hdrmc_get_cmd_buf(void);

/*===========================================================================

FUNCTION HDRSHIM_HDRMC_GET_CMD_FREE_Q_PTR

DESCRIPTION
  This function return the the HDRMC free queue pointer
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to HDRMC cmd free queue


SIDE EFFECTS
  None.
===========================================================================*/
extern q_type *hdrshim_hdrmc_get_cmd_free_q_ptr(void);


/*===========================================================================

FUNCTION HDRSHIM_HDRRUP_IS_TC_ASSIGNED

DESCRIPTION
  This function checks if the AT has been assigned a Reverse Traffic channel
  
DEPENDENCIES
  None

PARAMETERS
  caller - Protocol calling this function 

RETURN VALUE
  TRUE - If RTC has been assigned, FALSE otherwise

SIDE EFFECTS
===========================================================================*/

extern boolean hdrshim_hdrrup_is_tc_assigned
(
  hdrhai_protocol_name_enum_type caller 
);


/*===========================================================================

FUNCTION HDRSHIM_HDRTS_GET_CYCLE_AND_SLOT

DESCRIPTION
  This function returns the current time in terms of arbitrary length cycles
  and slots into the cycle

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrshim_hdrts_get_cycle_and_slot
(
  qword                           cycle,
    /* Output: arbitrary length cycle index since UTC=0 */

  uint32                        * slot,
    /* Output: slot in the above cycle */

  uint32                          slots_per_cycle
    /* Arbitrary cycle length, in slots */
);

/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_SET_TX_FRAME_START_OFFSET

DESCRIPTION
  Sets the tx frame start offset to a designated slot.
  
DEPENDENCIES
  None.

PARAMETERS
  slot_offset - The slot on which transmit frames will start (0 - 15)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_set_tx_frame_start_offset(
  byte slot_offset 
);


/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_SET_TX_FRAME_INTERRUPT_OFFSET

DESCRIPTION
  Sets the tx frame interrupt to assert on a designated slot.

DEPENDENCIES
  None.

PARAMETERS
  offset - The slot on which to assert the transmit interrupt (0 - 15)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_set_tx_frame_interrupt_offset(
  uint16 int_offset
);


/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_PROCESS_ACCESS_ATTEMPT

DESCRIPTION
  This function handles the access attempt and is called each frame in
  response to the signal that is received from the interrupt service routine
  which is attached to the modulator transmit frame interrupt.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdramac_process_access_attempt ( void );

/*==========================================================================

FUNCTION HDRSHIM_HDRTS_GET_SLOT_TIME

DESCRIPTION
  This function returns the current slot (1.67ms) index into the current
  5.12s "paging period"  [Range: 0..3071]

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  Slots (1.67ms) since latest page boundary.

SIDE EFFECTS
  None

==========================================================================*/

uint16 hdrshim_hdrts_get_slot_time( void );

/*==========================================================================

FUNCTION HDRSHIM_HDR_TS_GET_PAGE_CYCLE

DESCRIPTION
  This function returns the number of paging cycle periods (5.12 seconds)
  since the beginning of time (UTC=0)

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  Paging cycles since UTC=0

SIDE EFFECTS
  None

==========================================================================*/

uint32 hdrshim_hdrts_get_page_cycle( void );

/*===========================================================================

FUNCTION HDRSHIM_HDRIND_GIVE_IND

DESCRIPTION
  This function gives the specified indication to all the protocols that
  are registered to receive it.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Name of the indication to be broadcast
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrshim_hdrind_give_ind
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_LOG_ACCESS_ATTEMPT                                  

DESCRIPTION
  Logs data relating to an access attempt.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdramac_log_access_attempt( void );


/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_ATTEMPT_CLEANUP                                     

DESCRIPTION
  General clean-up for ending an access attempt.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdramac_attempt_cleanup( void );

/*=============================================================================

FUNCTION HDRSHIM_HDROVHD_GET_COLOR_CODE

DESCRIPTION
  This function is used to retrieve the color code of the current sector.

DEPENDENCIES
  This function only returns a value when overhead information is current.
  
PARAMETERS
  color_code_ptr - Pointer to allocated memory where the color code 
  will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has not been received yet
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrshim_hdrovhd_get_color_code
( 
  uint8 *color_code_ptr,
  hdrhai_protocol_name_enum_type caller
);


/*==========================================================================

FUNCTION HDRSHIM_HDRTS_GET_FRAME_TIME

DESCRIPTION
  This function gets the current HDR time (since THE BEGINNING) in frames.

DEPENDENCIES
  hdrts_set()

RETURN VALUE
  TRUE if time is available, otherwise FALSE

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrshim_hdrts_get_frame_time
(
  qword                           frame_time
);

/*=============================================================================

FUNCTION HDRSHIM_HDROVHD_GET_AP_MSG

DESCRIPTION
  This function is used to access the current AccessParameters message.
  If the AccessParameters are not current & the AccessParameters supervision
  timer is not already running, it starts a the timer.

DEPENDENCIES
  Access Parameters should be current when calling this function.
  The input parameter ap_msg_ptr should point to valid memory, ie.,
  memory for storing the AccessParameters message should be allocated before
  calling this function.
  
PARAMETERS
  ap_msg_ptr - Pointer to allocated memory where the current AcccessParamters
  message will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NOT_ALLOWED - If AccessParameters are not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrshim_hdrovhd_get_ap_msg
( 
  hdrovhd_ap_msg_type *ap_msg_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================

FUNCTION HDRSHIM_HDRSCP_GET_CURRENT_SUBTYPE

DESCRIPTION
  This accessor function returns the InUse subtype of the requested protocol

DEPENDENCIES
  None

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol

SIDE EFFECTS
  None

===========================================================================*/

extern uint16 hdrshim_hdrscp_get_current_subtype
(
  hdrhai_protocol_name_enum_type protocol
);

/*===========================================================================
FUNCTION HDRSHIM_HDRAMAC_ADD_PILOT_STRENGTH_CORR

DESCRIPTION
  This function computes pilot strength correction value and updates
  probe_init_adjust value in hdramac struct.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Pilot strength correction value.

SIDE EFFECTS
  probe_init_adjust in hdramac struct is updated.
===========================================================================*/
void hdrshim_hdramac_add_pilot_strength_corr (void);

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_PREPARE_PKT                                         

DESCRIPTION
  Retrieves a packet from the upper layer (if one is available) and prepares
  it for transmission over the access channel.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - a packet was retrieved and is ready to send.
  FALSE - no packet was available.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrshim_hdramac_prepare_pkt( void );

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_COMPUTE_PERSISTENCE_LIMIT

DESCRIPTION
  Computes the maximum number of times the persistence test may be run before
  an automatic pass.

DEPENDENCIES
  None.

PARAMETERS
  n - Persistence value from AP message.

RETURN VALUE
  perist_limit - The max peristence tests allowed by standard.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrshim_hdramac_compute_persistence_limit(
  int n
    /* persistence val */
);

/*=============================================================================

FUNCTION HDRSHIM_HDROVHD_GET_SECTOR_ID_24

DESCRIPTION
  This function is used to retrieve the lowest 24 bits of the sector ID in
  the last received quick config message.

DEPENDENCIES
  This function only returns a value when overhead information is current.
  
PARAMETERS
  sector_id_24_ptr - Pointer to allocated memory where the lowest 24 bits of 
  the current sector's sector ID will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the sector ID has not been received yet
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrshim_hdrovhd_get_sector_id_24
( 
  uint32 *sector_id_24_ptr,
  hdrhai_protocol_name_enum_type caller
);


/*===========================================================================

FUNCTION HDRSHIM_HDRPCP_GET_PKT

DESCRIPTION
  This function causes PCP to create an packet of up to max_bytes length
  from pending TX data.

DEPENDENCIES
  None

PARAMETERS
  slp_sdu   - Pointer to SDU struct to contain results
  max_bytes - Maximum number of bytes current TX frame can accommodate

RETURN VALUE
  Number of bytes in SDU

SIDE EFFECTS

===========================================================================*/

extern uint16 hdrshim_hdrpcp_get_pkt
(
  uint16                 max_bytes,
  hdrhai_channel_enum_type  channel,
  uint16                 time_since_prev_req,
  dsm_item_type**        pkt_ptr,
  boolean*               is_fmt_b_ptr
);

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_COMPUTE_NEXT_SILENCE_INTERVAL

DESCRIPTION
  Computes the next Reverse Silence Interval time

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrshim_hdramac_compute_silence_time( void );


/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_CHECK_REVERSE_SILENCE_WAIT

DESCRIPTION
  Computes whether the ACMAC must wait before sending a probe due
  to a conflict with the next RSI (Reverse Silence Interval).

DEPENDENCIES
  None.

PARAMETERS
  time - The frame time to check.

RETURN VALUE
  wait - If there is a conflict, the number of frames to wait.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrshim_hdramac_check_reverse_silence_wait
(
  qword time
    /* The frametime to check */
);

/*===========================================================================

FUNCTION HDRSHIM_HDRSTREAM_STREAM_IS_ASSIGNED

DESCRIPTION
  This boolean accessor function determines whether there has been
    at least one stream allocated for a packet application.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - Stream has been allocation for a Packet Application
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrshim_hdrstream_stream_is_assigned
(
  hdrhai_app_subtype_enum_type   app_subtype
);


/*===========================================================================
FUNCTION HDRSHIM_HDRAMAC_CHECK_TUNEAWAY_OVERLAP

DESCRIPTION
  This functions checks if a probe overlaps with tune away time.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if probe overlaps with tune away, else FALSE.

SIDE EFFECTS
  probe_init_adjust in hdramac struct is updated.
===========================================================================*/
boolean hdrshim_hdramac_check_tuneaway_overlap
(
  uint32  remaining_probe_length
);

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_UPDATE_PROBE_NUM_AND_FCS                           

DESCRIPTION
  Updates probe num in packet header and recomputes FCS.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Access packet header contains updated probe number. Updated FCS.

===========================================================================*/
void hdrshim_hdramac_update_probe_num_and_fcs (void);


/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_SETUP_ACCESS_PROBE

DESCRIPTION
  Sets up the modulator for an access probe. This function must be called 
  before each probe. 
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_setup_access_probe( void );

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_SET_IQ_MASK

DESCRIPTION
  This function computes and sets the IQ mask.

DEPENDENCIES
  Hdramac_schedule() must have been called for the access cycle we are
  going to transmit in.  hdramac.sector_id, .color_code, and _cycle_num
  must be valid.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdramac_set_iq_mask( void );


/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_BEGIN_ACCESS_PROBE

DESCRIPTION
  Turns on the PA and turns off IQ masking for an access probe.  This
  function should be called shortly before the frame in which the probe is
  to begin.  (The PA is turned on early to allow it to warm up.)
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_begin_access_probe( void );


/*===========================================================================
FUNCTION HDRSHIM_HDRAMAC_EAC_FRAME_RATE

DESCRIPTION
  This function determines the rate of an enhanced access channel frame
   
DEPENDENCIES
  None.

PARAMETERS
  Returns size of data and rate of frame to be transmitted in the next 
  access probe frame.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrshim_hdramac_eac_frame_rate
(
 uint8                 *payload_size,
 hdrmod_rri_enum_type  *probe_frame_rate
);


/*===========================================================================
FUNCTION HDRSHIM_HDRENC_SEND_FRAME

DESCRIPTION
  Writes packet data to the encoder input buffer, and send the frame at the
  RRI indicated. Padding is added if needed. 
  
DEPENDENCIES
  The encoder must have been initialized via hdrenc_init().

PARAMETERS
  data_ptr - pointer to the data to load into the encoder
  last_byte- the last byte (containing the last 2 bits) of the frame
  length   - the byte length of the data to load from *data_ptr
  rri      - RRI at which the packet should be sent.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrenc_send_frame(
  uint8         *data_ptr,
  uint16         length,
  uint8          last_byte,
  uint8          rri
);

/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_SET_RRI

DESCRIPTION
  Sets the modulator reverse rate based on the RRI.
  
DEPENDENCIES
  None.

PARAMETERS
  RRI - the RRI index of the frame.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_set_rri(
  uint16 rri
);

/*===========================================================================
FUNCTION HDRSHIM_HDRMOD_END_ACCESS_PROBE

DESCRIPTION
  Ends an access probe by configuring the PA to turn off at the next frame
  boundary.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrshim_hdrmod_end_access_probe( void );

/*===========================================================================

FUNCTION HDRSHIM_HDRAMAC_LOG_ACCESS_PROBE

DESCRIPTION
  Logs data relating to an access probe.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdramac_log_access_probe( void );

/*===========================================================================

FUNCTION HDRSHIM_HDRTX_REPORT_AGC

DESCRIPTION
  This function reports Tx agc values.  No more, no less.  The parameter
  is not used (pass 0).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrtx_report_agc( 
  int32 ms 
);

/*===========================================================================

FUNCTION HDRSHIM_HDRAMPATI_RECEIVE_ATI_GET

DESCRIPTION
  Returns the latest ATI of type specified if one exists in the receive 
  structure

DEPENDENCIES
  None

PARAMETERS
  ati_type - type of ATI to be fetched from the receive ATI structure

RETURN VALUE
  ati - returns an ATI value of type ati_type, if found

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrshim_hdrampati_receive_ati_get
( 
  hdrhai_ati_enum_type ati_type
);

/*===========================================================================

FUNCTION HDRBCFRAME_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast flow.  The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow ID to disable.
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdrbcframe_flow_disable
(
  uint32 flow_id,
  hdrhai_protocol_name_enum_type sender
);


/*===========================================================================

FUNCTION HDRSHIM_BCFRAME_FLOW_ENABLE

DESCRIPTION
  This is a wrapper function for hdrbcframe_flow_enable()
  This command enables a broadcast flow. The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The Broadcast flow ID to enable
  data_cb - A callback function for deframed broadcast data. 
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdrbcframe_flow_enable
(
  uint32 flow_id,
  void (*data_cb)(dsm_item_type *ip_pkt),
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSHIM_RX_QUEUE_MSG

DESCRIPTION
  This is wrapper function for hdrrx_queue_msg()
  This function queues a message on the hdrrx_event_q and sets the signal.
  The queued buffer simply holds a pointer to the DSM item chain that is
  the message.  This function is called in the task context of whoever is 
  giving the message, which is always the task in which SLP runs.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrshim_hdrrx_queue_msg
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type               *item_ptr 
);

/*===========================================================================

FUNCTION HDRDOS_TIMER_HANDLER

DESCRIPTION
  This is wrapperf unction for hdrdos_timer_handler()
  
DEPENDENCIES
  None

PARAMETERS
  seq_no - Sequence number of the new DOS message or of an earlier DOS message
           thats being ACK'ed 

  event  - DOS protocol event that occurred.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrshim_hdrdos_timer_handler
(
  hdrdos_route_enum_type route,
  uint8 seq_no,
  hdrdos_dos_event_type event
);

/*===========================================================================

FUNCTION HDRSHIM_DOS_SEND_STATUS_TO_CM

DESCRIPTION
  This is wrapper function for hdrdos_send_status_to_cm()
  This function sends status regarding DOS messages to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  msg_seq  - Sequence number of the DOS message
  route    - Route instance on which the DOS message was sent
  err_code - Error code describing the status of MO DOS message to CM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdrdos_send_status_to_cm
(
  uint8 msg_seq,
  hdrdos_route_enum_type route,
  cm_dbm_mo_status_e_type err_code
);


/*===========================================================================

FUNCTION HDRSHIM_SRCHSLEEP_FAST_WAKEUP

DESCRIPTION
  This is wrapper function for hdrsrchsleep_fast_wakeup()
  Reprograms the sleep controller to wake up as soon as possible.

DEPENDENCIES
  Must only be used as callback to sleepctl_set_warmup( )

PARAMETERS
  None

RETURN VALUE
  Whether the fast wake up has been programmed into sleep controller. 

SIDE EFFECTS
  None

===========================================================================*/
void hdrshim_hdrsrchsleep_fast_wakeup(void);

/*===========================================================================

FUNCTION HDRSHIM_IDLE_SEND_CALL_ORIG_FAIL_TO_CM

DESCRIPTION
  This is wrapper function for hdridle_send_call_orig_fail_to_cm()
  This function sends a orig fail report to CM.

DEPENDENCIES
  None.

PARAMETERS
 reason - reason for the orig failure


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrshim_hdridle_send_call_orig_fail_to_cm
(
  hdridle_conn_fail_enum_type reason
);

/*===========================================================================

FUNCTION HDRSHIM_IDLES_GET_STATE

DESCRIPTION
  This is wrapper function for hdridles_get_state()
  Called to retrieve the current state of the IDLE.
  This funciton is only to be used by other code in the IDLE.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of IDLE

SIDE EFFECTS
  None

===========================================================================*/
hdridles_state_enum_type hdrshim_hdridles_get_state( void );

/*=============================================================================

FUNCTION HDRSHIM_HDROVHD_GET_LAST_CAPSULE_LENGTH_MAX

DESCRIPTION
  This is wrapper function for hdrovhd_get_last_capsule_length_max()
  This function returns the most recent capsule length max to allow upper
  layers to make a "best estimate" of the largest message size that can be
  sent on the current access channel.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  last_capsule_length_max - most recent capsule_length_max field value

SIDE EFFECTS
  None

=============================================================================*/
extern uint8 hdrshim_hdrovhd_get_last_capsule_length_max( void );
 
/*=============================================================================

FUNCTION HDRSHIM_HDROVHD_GET_SECTOR_ID

DESCRIPTION
  This is wrapper function for hdrovhd_get_sector_id()
  This function is used to retrieve the 128 bit SectorID of the current sector

DEPENDENCIES
  None

PARAMETERS
  sector_id - Pointer to allocated memory where the sector id will get copied.
  subnet_mask_ptr - Pointer to where the subnet_mask will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - if overhead parameters are not current, so the current
               128 bit sector id is unknown

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrshim_hdrovhd_get_sector_id
(
  hdraddr_type sector_id,
  uint8 * subnet_mask_ptr,
  hdrhai_protocol_name_enum_type caller
);

#endif   /* HDRSHIM_H */
