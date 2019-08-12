#ifndef DSGCSDFATRANS_H
#define DSGCSDFATRANS_H

/*===========================================================================

            Transparent Fax Adaptation Subsystem Header File

Description:
  This source file contains the definitions and function prototypes used by
  GSM Fax Adaptation for the transparent mode of the Bearer Services Quality
  of Service attribute.  It implements the following specification:

  ETSI GSM 03.45 Version 7.0.1 Release 1998
  Technical realization of facsimile group 3 transparent

Externalized Functions:
  dsgcsdfatrans_call_init()         - Perform inits on call setup.
  dsgcsdfatrans_process_uplink()    - Process uplink fax data.
  dsgcsdfatrans_process_downlink()  - Process downlink fax data.
  dsgcsdfatrans_return_to_idle()    - Return Fax Adaptor state to idle.
  dsgcsdfatrans_process_bcs_frame() - Process fax HDLC BCS frame.
  dsgcsdfatrans_timer_expired()     - Handle expiration of timed event.
  dsgcsdfa_tx_ready()               - Indicates if FA TX state is ok to xmit.
  dsgcsdfa_is_idle()                - Indicates if FA is in IDLE state or not.

Initialization and Sequencing Requirements:
  dsgcsdfatrans_call_init() must be called at the beginning of every fax call.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Copyright (c) 2002 - 2006 Qualcomm Technologies Incorporated. All Rights Reserved
                     Qualcomm Proprietary

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdfatrans.h#1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------------
02/09/06  AR    Lint corrections.
10/26/05  AR    Add support for multiple call instances.
04/25/03  TMR   Changes for new UMTS CS data architecture
03/19/03  RLP   GCF certification changes to fix MT fax issue.
12/20/02  RLP   Added new interface function dsgcsdfa_is_idle().
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.

===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX

#include "dsm.h"
#include "rex.h"
#include "cm.h"
#include "dsucsd.h"
#include "dsgcsdfa.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* Transparent Fax Adaptation states, as described in GSM 03.45, 5.2.3. */
typedef enum
{
  FA_STATE_TRANS_IDLE = 0,
  FA_STATE_TRANS_BCS_REC,
  FA_STATE_TRANS_BCS_TRA,
  FA_STATE_TRANS_MSG_REC,
  FA_STATE_TRANS_MSG_TRA,
  FA_TRANS_NUM_STATES
}
fa_state_trans_enum_type;

typedef enum
{
  FA_NO_TIMER_TYPE,
  FA_DCS_TIMER_TYPE,
  FA_SYNC_LOSS_TIMER_TYPE,
  FA_TRAINING_TIMER_TYPE,
  FA_CED_TIMER_TYPE,
  FA_NUM_TIMER_TYPES
} fa_timer_type_enum_type;


/* Transparent Fax Adaptation control block.  Maintains state information. */
typedef struct
{
  /* Current transparent Fax Adaptor state. */
  fa_state_trans_enum_type   fa_state_trans;


  /* Timer used to, guess what, time stuff. */
  rex_timer_type       *timer_ptr;
  rex_sigs_type         timer_sig;
  fa_timer_type_enum_type timer_type;

  /* Watermarks for exchanging data with GCSD Transport Protocol Stack. */
  dsm_watermark_type   *uplink_wm_ptr;
  dsm_watermark_type   *downlink_wm_ptr;

  /* Sometimes we gotta hang onto uplink and/or downlink data between calls
  ** to our uplink and downlink data processing functions.  These do that. */
  dsm_item_type        *uplink_buf_dsm_ptr;
  dsm_item_type        *downlink_buf_dsm_ptr;

  /* This maintains the last matched SYNC frame index to try to speed up
  ** locating sync frames ... we start searching with the last known sync 
  ** frame pattern.
  */
  int8                  last_sync_frame_index;

  /* For the current OTA rate, this is the number of STATUS frames duplicated
  ** for each BCS byte.  BCS data is 300 bps, byte the OTA rate is greater.
  ** Initialized upon entry to BCS_REC and BCS_TRA states. */
  uint16                ui16_frames_per_bcs_byte;

  /* This holds the number of sync frames to send based on the OTA rate to 
  ** ensure we fill the channel.
  */
  uint16                ui16_syncs_per_ota_rate;

  /* These values are only used when transmitting a fax at 7,200 or 12,000 bps.
  ** At 7,200 bps, we transmit 1 SYNC frame per 3 DATA frames.  At 12,000 bps,
  ** we transmit 1 SYNC frame per 5 DATA frames.  These values maintain the
  ** number of bytes to send between SYNC frames and the number of bytes
  ** remaining to send until we send the next SYNC frame.  In addition,
  ** w_bytes_between_sync_frames = 0 means we're using a more standard rate,
  ** requiring no SYNC frame stuffing at all.  And -1 means we don't know the
  ** fax rate, which is bad.  But it should never happen. */
  int16                 i16_bytes_between_sync_frames;
  int16                 i16_bytes_to_next_sync;

  /* This is a goofy value only used to compensate for the goofy fax
  ** rates of 7,200 and 12,000 bps.  At normal rates, the average number of
  ** useful data bytes per frame is just the frame size.  At goofy rates,
  ** SYNC frames are periodically thrown in, pulling the average down. */
  uint16                ui16_data_per_frame;

  /* This also only applies to the 7,200 and 12,000 bps fax rates.  When
  ** receiving DATA frames at these speeds, singleton SYNC frames are used
  ** to fill up the data channel.  Two SYNC frames back-to-back mean data
  ** transmission has terminated.  This keeps track of that on fax receive. */
  boolean               f_got_sync_last_time;

  /* This is to TRUE when T.30 training has been completed when receiving
  ** a fax and we need to wait for the remote FA to signal it is done 
  ** sending training data. Once we get a SYNC we can exit the MSG_TRA
  ** state and return to IDLE.
  */
  boolean               wait_for_sync;
 
  /* This is set to TRUE when a DCN HDLC frame is detected meaning a
  ** disconnect is in progress.
  */
  boolean               dcn_pending;

  /* This only applies to the BCS_TRA state, when we are receiving BCS data
  ** from the remote FA.  If we lose synchronization, we throw away bytes as
  ** we try to establish synchronization again.  Here we count those bytes.
  ** GSM 03.45, 5.2.3.3 says we're still supposed to return "an estimate of
  ** the INFO octet value in the received STATUS frames" to the attached DTE
  ** even if we can't identify any STATUS frames.  Kinda weird.  Anyway this
  ** byte count allows us to determine how many byte estimates to make. */
  uint16                ui16_bytes_tossed;

  /* These 2 variables are used to keep track of the number of training
  ** NULLs that should be sent to the DTE for a MT facsimile call.
  */
  uint16                ui16_max_training_nulls;
  uint16                ui16_training_nulls_cnt;

  /* This is basically just a debug counter, for counting DATA bytes during
  ** MSG phases.  A fax page byte counter, essentially. */
  uint32                ui32_msg_byte_count;

  /* Number of sync frames to send per 20ms time slot */
  uint8                 num_sync_frames;

  /* These are used when the incoming data is NOT octet aligned. We have
  ** to shift the data back into alignment.
  */
  int                   bits_to_shift;
  uint8                 leftover_byte;
  boolean               have_leftover_byte;


  /* Tracks the number of consecutive good SYNC frames that must be received
  ** at the start of a call in order for the CONNECT to be sent to the local TE
  */
  uint8                 sync_hysterisis_cnt;
  uint8                 sync_loss_hysterisis_cnt;

  /* This is used to keep track of consecutive bad frames */
  uint8                 bad_frame_cnt;

  /* This flag is set to TRUE if the call is MO else FALSE for MT */
  boolean               f_call_is_MO;

  /* When we are in state BCS_TRA, it is possible for a 2.5-second loss of 
  ** synchronization timer to expire.  Also in state BCS_TRA, we may receive
  ** DCS from the remote FA, meaning we should go IDLE and wait for training.
  ** When we are in state MSG_TRA, it is possible for a 1.5-second training
  ** duration timer to expire.  These things happen between calls to
  ** dsgcsdfa_process_downlink().  But we must wait to perform the transition
  ** until dsgcsdfa_process_downlink() gets called again, so we can return the
  ** proper status back to T31. */
  boolean               f_goto_idle;

  /* When T31 builds a valid HDLC frame, it will give it back to the FA so
  ** it can interrogate the frame.  The FA must act on certain frames, namely
  ** detecting a DCS frame at the start of the T.30 training phase.  When
  ** this frame is detected, the FA will set this flag so it knows it must
  ** start the 75ms DCS->TCF timer when it leaves the BCS_TRA state and 
  ** returns to IDLE.
  */
  boolean               f_start_dcs_timer;

  /* Once we have detected a SYNC frame from the remote FA, this gets set.
  ** With frame boundary info, means we can look for other frames now too. */
  boolean               f_synchronized;

  /* This flag only applies to the MSG_REC state.  It is set once we detect
  ** that the remote FA has transitioned from IDLE to MSG_TRA.  That means it
  ** is OK for us to send DATA frames on the uplink. */
  boolean               f_remote_fa_in_msg_tra;

  /* This flag only applies to the BCS_REC state.  It is set once we detect
  ** that the remote FA has transitioned from IDLE to BCS_TRA.  That means it
  ** is OK for us to send BCS frames on the uplink. */
  boolean               f_remote_fa_in_bcs_tra;

  /* This flag applies to the MSG_REC and MSG_TRA states.  Once we detect
  ** T.30's DCS frame going in either direction, we set this flag.  This
  ** means the next blast of higher speed data is training, not page data.
  ** The two phases are handled slightly differently. */
  boolean               f_training_phase;

  /* This flag is used to determine when it is ok to send the first CONNECT
  ** back to the local TE.  The connect should only be sent when the FA has
  ** synchronized with the remote FA.  Since we are the ones that know when
  ** this occurs we have to handle it.
  */
  boolean               f_send_connect;

  /* If instantiating more than one Transparent Fax Adaptor in the build,
  ** like having 2 talk to each other, maintain unique ID string for each. */
  char                 *trans_id;
  
  /* Whether the current call is transparent or non-transparent.  Value will
  ** either be TRANSPARENT_DATA or NON_TRANSPARENT_DATA
  */
  uint8                 quality_of_service;
  
  /* Data rate of current call */
  ds_ucsd_data_rate_T   data_rate;

  /* Call Manager call ID */
  cm_call_id_type                   call_id;
}
fa_trans_ctrl_blk_struct_type;

/*===========================================================================
                FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
** The following 6 functions implement the Transparent Fax Adaptation API.
** They are the only functions externalized by this module, and they can
** only be called by T31.
===========================================================================*/

/*========================================================================
  Function: dsgcsdfatrans_call_init

  Description:
    This function is invoked by T31, through dsgcsdfa_call_init(), at the
    beginning of every fax call.  It is responsible for doing Fax Adaptation
    initialization for transparent mode.

  Dependencies:
    None.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    None.

  Side Effects:
    Initializes transparent Fax Adaptation state.
========================================================================*/
void dsgcsdfatrans_call_init
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_process_uplink

  Description:
    This function performs Fax Adaptation on the uplink.  It is invoked
    periodically by T31, through dsgcsdfa_process_uplink(), to take data
    destined for the remote fax, perform transparent Fax Adaptation
    processing on it, and pass it down the protocol stack to GCSD
    Transport Protocol Stack.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    fax_uplink_data   - Pointer to DSM item containing fax data destined
                        for the remote fax device, if any data available.
    fax_type_of_data  - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE - No data at this time.
      DS_FA_DATA_BCS  - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG  - MSG (higher speed image) fax data.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_UPLINK_COMPLETE    - Function completed successfully, AND there's
                                 no use calling us again with no data to
                                 generate more uplink data.  We are in a
                                 state where we send DATA frames, and we
                                 need data to do that.
      DS_FA_SUCCESS            - Function completed successfully, AND T31 may
                                 call us again to generate more uplink data
                                 to fill the channel, if necessary.
      DS_FA_UPLINK_DATA_QUEUED - Same as DS_FA_SUCCESS, AND we were unable
                                 to send all uplink data to the remote FA.
                                 Remote FA is not accepting DATA frames
                                 yet.  If T31 calls return_to_idle() at this
                                 point, all queued data is freed and is never
                                 transmitted to the remote FA.
      DS_FA_INVALID_DATA_TYPE  - Data type, BCS or MSG, invalid for current
                                 Fax Adaptation state.
      DS_FA_FAILURE            - Fatal error.  Call should be aborted.

  Side Effects:
    Potentially takes uplink data from T31, performs a transparent Fax
    Adaptation massage on it, and sends it to GCSD Transport Protocol Stack.

    Transparent Fax Adaptation state may change.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_uplink
(
  dsm_item_type                 *fax_uplink_data,
  ds_fa_data_enum_type           fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_process_downlink

  Description:
    This function performs transparent Fax Adaptation on the downlink.
    It is invoked periodically by T31, through dsgcsdfa_process_downlink(), to
    take data received from the remote fax, perform transparent Fax
    Adaptation processing on it, and pass it up the protocol stack to T31.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    fax_downlink_data   - Pointer to DSM item containing fax data received
                          from the remote fax device, if any data available.
    fax_type_of_data    - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE   - No data at this time.
      DS_FA_DATA_BCS    - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG    - MSG (higher speed image) fax data.
    ctrl_blk_ptr        - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS           - Call successful.
      DS_FA_SWITCHING_TO_IDLE - Call successful, and transparent Fax Adaptor
                                switching its state to IDLE because of state
                                change in T.30 protocol.  Keep T31 layer
                                informed.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Potentially takes downlink data from GCSD Transport Protocol Statck,
    performs a transparent Fax Adaptation massage on it, and returns it up
    to T31.

    Transparent Fax Adaptation state may change.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_downlink
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_return_to_idle

  Description:
    This function is invoked by T31, through dsgcsdfa_return_to_idle(), when
    it decides that the Fax Adaptation layer should return to IDLE.  This
    happens normally during the T.30 protocol and can also happen for
    error and timeout handling.  Always happens on call termination.

  Dependencies:
    None.

  Parameters:
    f_call_ended - TRUE means call has terminated.
                   FALSE means call is still up.
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SWITCHING_TO_IDLE - Call successful and switching to IDLE.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Transparent Fax Adaptation state returns to IDLE, if not there already.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_return_to_idle
(
  boolean                        f_call_ended,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_process_bcs_frame

  Description:
    This function is called by T31, through
    dsgcsdfa_process_bcs_frame(), once it packages up an entire BCS
    frame received on the downlink.  T31 also calls this before it calls
    dsgcsdfatrans_process_uplink(), to give us a chance to look at the
    BCS frame before it gets bit-stuffed.

    Its job is to look at the BCS frame and determine if it's a DCS
    (Digital Command Signal).  The DCS frame identifies the fax rate
    negotiated by the two fax devices.  If it is a DCS, the fax rate is
    stored away in the control block for use later.  If the fax rate is
    one of the goofy ones, 12000 or 7200 bps, additional info about the
    number of DATA frames per SYNC frame is also stored in the control block.
    If this is not a DCS, no action is necessary.

    This function does absolutely no error checking on the BCS frame.  If
    it's corrupt, we just return, assuming it will be dealt with elsewhere.

    A T.30 HDLC BCS frame looks as follows:
      -----------------------------------------------------------------
      | Flag | ... | Flag | Address | Control | FCF | FIF | FCS | Flag |
      -----------------------------------------------------------------

    All fields are 1 byte, except FIF, which has arbitrary length.  The FCF
    identifies the frame.  We're looking for the DCS identifier in this
    function.  If it is a DCS, the middle 4 bits of the 2nd byte of the FIF
    identify the fax rate as follows (from Table 2/T.30):

      X X 0 0 0 0 X X -  2400 bps, Rec. V.27 ter
      X X 0 1 0 0 X X -  4800 bps, Rec. V.27 ter
      X X 1 0 0 0 X X -  9600 bps, Rec. V.29
      X X 1 1 0 0 X X -  7200 bps, Rec. V.29
      X X 0 0 0 1 X X - 14400 bps, Rec. V.17
      X X 0 1 0 1 X X - 12000 bps, Rec. V.17
      X X 1 0 0 1 X X -  9600 bps, Rec. V.17
      X X 1 1 0 1 X X -  7200 bps, Rec. V.17

  Dependencies:
    None.

  Parameters:
    fax_bcs_frame  - Pointer to DSM item containing BCS frame.
    f_uplink_frame - TRUE = uplink frame, FALSE = downlink frame.
    ctrl_blk_ptr   - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_SUCCESS.

  Side Effects:
    Frame may or may not be inspected and relevant information recorded.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_bcs_frame
(
  const uint8                   *ui8_data_ptr,
  uint16                         ui16_data_count,
  boolean                        f_uplink_frame, 
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_timer_expired

  Description:
    This function is invoked by the main GCSDPS task loop, through
    dsgcsdfa_timer_expired(), when it detects that the Fax Adaptor timer
    signal is set.  This allows transparent Fax Adaptation software to
    service the timer in the context of the GCSDPS task.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    None.

  Side Effects:
    Transparent Fax Adaptation state may change.
========================================================================*/
void dsgcsdfatrans_timer_expired
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_tx_ready

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not it is ok to transmit data to the remote FA.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE if it is ok to transmit, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
boolean dsgcsdfatrans_tx_ready
(
  const fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

/*========================================================================
  Function: dsgcsdfatrans_is_idle

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not the FA layer is currently IDLE.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE if the FA is IDLE, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
extern boolean dsgcsdfatrans_is_idle
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
);

#endif  /* FEATURE_DATA_GCSD_FAX */
#endif  /* #ifndef DSGCSDFATRANS_H */
