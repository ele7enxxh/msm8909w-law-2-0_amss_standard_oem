#ifndef HDRRMAC4_H
#define HDRRMAC4_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   R E V E R S E   T R A F F I C   C H A N N E L   M A C   
                            
                             P R O T O C O L  (Subtype 4)      

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Reverse Traffic Channel 
  Medium Access Control (MAC) Protocol for an HDR access terminal.

EXTERNALIZED FUNCTIONS
  hdrrmac_init
    Initialize the Reverse Traffic Channel MAC protocol.
    
  hdrrmac_activate
    Activate the Reverse Traffic Channel MAC protocol.

  hdrrmac_deactivate
    Deactivate the Reverse Traffic Channel MAC protocol.

  hdrrmac_is_active
    Determine if protocol is active.
    
  hdrrmac_process_cmd
    Process Forward Reverse Channel MAC commands from the task command queue.
    
  hdrrmac_process_sig
    Process Forward Reverse Channel MAC signals received at task level.
    
  hdrrmac_process_msg
    Process received Reverse Traffic Channel MAC messages.
    
  hdrrmac_msg_cb
    Callback function that queues received Reverse Traffic Channel MAC 
    messages onto the task message queue.
    

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.

  Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/inc/hdrrmac4.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/04/14   arm     Changes to support DO to G QTA
11/25/13   rmg     Added changes to reduce idle subframes before tuneaway.
10/02/13   rmg     Added support for RL rate estimate and RLP queue info for
                   modem API.
07/10/13   arm     Supported traffic DTX. 
05/15/13   rmv     Added updates for Cost of Modem Power API support 
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
01/25/13   smd     Removed hdrrmac4_all_carriers_exit_drc_supervision().
10/03/12   rmg     Corrected tx power initialization during optimized handoff.
07/27/12   rmg     Added hdrrmac4_config_funnel_mode.
06/20/12   smd     Supported Device only DTX and FlexCONN.
11/07/11   vlc     Fixed Klocwork warnings. 
10/27/11   kss     Added support for LTE to DO optimized handoff.
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac4_enter/exit_drc_supervision().
09/22/09   rmg     Added support for RL flow control.
07/29/09   rmg     Added support for max transmit pilot power differential.
03/11/09   kss     Add Rev B support for hdrrmac_get_current_tx_packet_info().
06/26/08   rmg     Support for SLP carrier forced switch upon DRC supervision.
11/30/07   cc      Moved hdrrmac4_common pointer initialization from _init
                   to powerup_init function.
10/29/07   cc      Added hdrrmac4_get_changed_flow_mapping() declaration.
10/15/07   cc      Added init_attrib argument in powerup_init function.
09/27/07   rmg     Removed older permitted_payload definition in comment.
09/20/07   rmg     Fixed circular inclusions.
08/08/07   cc      DOrB updates; running rmac4 over rmac3drv.
07/17/07   cc      Code review comments.
07/12/07   cc      Added hdrrmac_update_rl_carriers() & 
                   hdrrmac_get_slp_carrier_and_tx_pilot_pwr_info support for
                   MSM7500 Rev. B integration.
07/07/07   cc      Initial creation.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"


#ifdef FEATURE_HDR_REVB_SUPPORTED

#include "comdef.h"
#include "dsm.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrrmaci.h"
#include "hdrrmac.h"
#include "hdrrmaccmn.h"
#include "hdrscmrmac4.h"

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* DataTokenBucket Attribubte Structure */
typedef struct
{
  uint32  data_token_inflow;
    /* in unit of octets/subframe */

  uint32  data_bucket_level_max;
    /* in unit of octets */

} hdrrmac4_data_token_bucket_struct_type;


/*-----------------*/
/* Flow attributes */
/*-----------------*/
typedef struct
{
 /*--------------------------------------------------------------------------*/
 /*----------------------------- Simple Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/
  uint32                                bucket_level_max;
    /* 
     * Maximum T2P resource that can be accumulated by a flow. 0 means flow is
     * inactive
     */

  uint16                                merge_thresh;
    /* 
     * Per flow threshold in octets. If this HiCap MAC flow  has more than
     * MergeThreshold octets to send then it is eligible for transmission in a 
     * new packet under certain conditions (see comment under eligibility 
     * thresholds below
     */

  hdrrmaccmn_tx_mode_enum_type          tx_mode;
    /* LoLat or HiCap transmission mode. Default is HiCap */

  hdrrmaccmn_qrab_select_enum_type      qrab_select;
    /* 
     * Switch to control whether AT should use QRAB or QRABps as indication of
     * short term sector loading for this MAC flow
     */ 

  uint16                                burst_duration_factor;
    /* 
     * Maximum duration (in subframes) this MAC flow can sustain a peak rate 
     * based on its T2P allocation
     */

  uint16                                t2p_filt_tc;
    /* 
     * T2PInflow IIR filter time constant for this MAC flow: 16, 24 (default),
     * 32, 64 or 128 subframes
     */

 /*--------------------------------------------------------------------------*/
 /*---------------------------- Complex Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmaccmn_t2p_frab_grid_struct_type    bucket_factor;
    /* 
     * Standard allows for up to a 255x15 grid. 8 bit values in [1..32.875] 
     * represented in 1/8 dB units, so actual value is (OTA value * 1/8) + 1
     */
  
  hdrrmaccmn_flow_mapping_struct_type     flow_mapping;
    /* AssociatedFlowsNN */

  hdrrmac4_data_token_bucket_struct_type  data_token_bucket;
    /* DataTokenBucketNN */

} hdrrmac4_flow_config_struct_type;


/*--------------------------*/
/* Configuration attributes */
/*--------------------------*/
typedef struct
{
  /*--------------------------------------------------------------------------*/
  /*---------------------------- Complex Attributes --------------------------*/
  /*--------------------------------------------------------------------------*/


  hdrrmaccmn_pilot_strength_grid_struct_type pilotstrength;
    /* 
     * Scaling factor in 4 bit 2's complement format used to scale T2PInflow
     * if filtered PilotStrength equals corresponding PilotStrength axis point
     */

  hdrrmaccmn_txt2p_max_grid_struct_type    txt2p_max;
    /* 
     * Grid that defines the maximum TxT2P AT is allowed to transmit if 
     * filtered serving sector * PilotStrength equals corresponding 
     * PilotStrengthAxis value. 
     */

  hdrrmaccmn_gu_gd_struct_type            gu_gd;
    /* 
     * Used to determine T2PInflow up/down increments. They are aka Gu/Gd 
     * functions. Values are in 8 bit 2's complement form in 1/8 dB units 
     */
  
  hdrrmaccmn_t2p_inflow_range_struct_type t2p_inflow_range;
    /* T2PInflow min and max for this flow: 8 bit values in 1/8 dB units */

} hdrrmac4_config_config_struct_type;


/*--------------------*/
/* Channel attributes */
/*--------------------*/
typedef struct
{
 /*--------------------------------------------------------------------------*/
 /*----------------------------- Simple Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmaccmn_arq_mode_enum_type          arq_mode;
    /* Bipolar or On/Off keying */


  /*--------------------------------------------------------------------------*/
  /*---------------------------- Complex Attributes --------------------------*/
  /*--------------------------------------------------------------------------*/

  hdrrmaccmn_ps_qrab_thresh_struct_type  ps_qrab_thresh;

  hdrrmaccmn_pilot_strength_grid_struct_type *pilotstrength_ptr;
    /* 
     * Pointer to structure containing scaling factor in 4 bit 2's complement 
     * format used to scale T2PInflow if filtered PilotStrength equals 
     * corresponding PilotStrength axis point
     */

  hdrrmaccmn_txt2p_max_grid_struct_type  *txt2p_max_ptr;
    /* 
     * Pointer to structure containing grid that defines the maximum TxT2P AT 
     * is allowed to transmit if filtered serving sector * PilotStrength 
     * equals corresponding PilotStrengthAxis value. 
     */

} hdrrmac4_channel_config_struct_type;


/*---------------*/
/* NC attributes */
/*---------------*/
typedef struct
{
  /*--------------------------------------------------------------------------*/
  /*---------------------------- Complex Attributes --------------------------*/
  /*--------------------------------------------------------------------------*/


  hdrrmaccmn_gu_gd_struct_type            *gu_gd_ptr;
    /* 
     * Pointer to the structure used to determine T2PInflow up/down increments.
     * They are aka Gu/Gd functions. Values are in 8 bit 2's complement form in 
     * 1/8 dB units 
     */

  hdrrmaccmn_t2p_inflow_range_struct_type *t2p_inflow_range_ptr;
    /* 
     * Pointer to T2PInflow min and max for this flow: 8 bit values in 1/8 dB 
     * units 
     */

} hdrrmac4_nc_config_struct_type;


/*---------------------------*/
/* RMAC Subtype 4 Attributes */
/*---------------------------*/
typedef struct
{
 /*--------------------------------------------------------------------------*/
 /*---------------------------- Simple Attributes ---------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmaccmn_payload_size_enum_type      max_packet_size;
    /* MaxPacketSize */

  boolean                                is_max_sub_pkts_gaupable;
    /* Is MaxNumSubpackets(0xFFF2) attribute GAUPable */

  hdrrmaccmn_time_const_struct_type      filt_time_const;

  hdrrmaccmn_elig_thresh_struct_type     flow_elig_thresh;
    /* 
     * These 2 thresholds are used to determine whether all active flows are
     * eligible for transmission in the new packet. If the transmission mode is
     * LoLat and FRAB >= FRABLow and SumQOutflow < MergeThreshold then only 
     * LoLat flows or HiCap flows with more octets than the MergeThreshold for
     * that flow are eligible for transmission in the new packet
     */

  hdrrmaccmn_payload_size_enum_type      payload_thresh;
    /* 
     * Minimum packets size permitted to be transmitted in LoLat mode when it
     * does not contain LoLat flows
     */

  int32                                  max_rl_pilot_pwr_diff;
    /*
     * Maximum allowable differential ( in Q.10 dB units ) between filtered 
     * transmit pilot powers of reverse link carriers
     */

 /*--------------------------------------------------------------------------*/
 /*---------------------------- Complex Attributes --------------------------*/
 /*--------------------------------------------------------------------------*/

  hdrrmaccmn_aux_pilot_struct_type       aux_pilot_info;
    /* 
     * minimum payload requiring transmission of Aux Pilot and its gain 
     * relative to Traffic Channel
     */

  hdrrmaccmn_permitted_payload_size_type permitted_payload;
    /*
     * 12 + 1 possible payload sizes, 0 meaning no packet was transmitted.
     * The second dimension stands for subframes n-1, n-2 and n-3,
     * respectively. So permitted_payload[1][2], which has a default value of
     * 5 (standing for a payload size of 1024) means that the maximum permitted
     * payload size the AT can transmit in subframe n is 1024 bits if it
     * transmitted a payload of 128 bits in subframe n-2
     */ 

  hdrrmaccmn_t2p_info_type               t2p_info[HDRRMACCMN_NUM_TX_MODES];
    /* Size =  2 * 379 = 758 bytes. We don't need to store PowerParams (above)
     * separately when we have this
     */

  hdrrmaccmn_req_params_struct_type      req_params;
    /* ReqRatio and MaxReqInterval */

  hdrrmaccmn_rri_power_params_struct_type  rri_chan_power_params
                                           [HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS];
    /* 
     * RRI channels gains corresponding to the various combinations of pre and 
     * post T2P transition points
     */

  hdrrmaccmn_common_pow_params_struct_type common_pow_params;

  hdrrmac4_flow_config_struct_type       flow[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

  hdrrmac4_config_config_struct_type     config[HDRSCMRMAC4_MAX_NUM_CONFIGS];

  hdrrmac4_channel_config_struct_type    channel[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

  hdrrmac4_nc_config_struct_type         nc[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
                                            [HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

} hdrrmac4_config_struct_type;

/* This variable is set to TRUE by the following functions when the 
   current SLP carrier needs to be force switched:
   ~hdrrmac4_tca_update() - when the current SLP carrier is removed by TCA
   ~hdrrmac4drv_check_drc_supervision() - when the current SLP carrier is
    under DRC supervision */
extern boolean hdrrmac4_force_slp_switch;


/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION HDRRMAC4_POWERUP_INIT

DESCRIPTION
  This function initializes RMAC session configuration.

DEPENDENCIES
  Note that this is run in HDRMC context.
  Should be run BEFORE hdrrmac4_init().
 
DEPENDENCIES
  None.

PARAMETERS
  init_attrib - TRUE- RMAC4 is the current InUse subtype.  Attributes need
                to be initialized; FALSE otherwies.
  common_parameters - Pointer to the higher level RMAC common parameter
                block.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_powerup_init
(
  boolean init_attrib,
  hdrrmac_common_type * common_parameters
);


/*===========================================================================
FUNCTION HDRRMAC4_RESET_RTC_RATE_COUNT                           EXTERNAL

DESCRIPTION
  Wrapper function to reset the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_reset_rtc_rate_count( void );


/*===========================================================================
FUNCTION HDRRMAC4_LOG_RTC_RATE_COUNT                             EXTERNAL

DESCRIPTION
  Wrapper function for logging the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_log_rtc_rate_count( void );


/*===========================================================================
FUNCTION HDRRMAC4_CONFIG_EARLY_TERMINATION                       EXTERNAL

DESCRIPTION
  Configures early termination on the RL link. This is a test mode function.
  
DEPENDENCIES
  None.

PARAMETERS
  config_mode: 0 - Default behavior, RL early termination on.
               1 - Forced early termination after 1 subpacket.
               2 - Forced early termination after 2 subpackets.
               3 - Forced early termination after 3 subpackets.
               4 - Forced early termination after 4 subpackets.

RETURN VALUE
  TRUE (config mode accepted),
  FALSE (config mode not valid).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_config_early_termination
(
  int config_mode
);


/*===========================================================================

FUNCTION HDRRMAC4_COMPUTE_SILENCE_TIME

DESCRIPTION
  Computes the next Reverse Silence Interval time

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  hdrrmac4.silence_begin_time is updated with the frame time for the
  next silence interval.

===========================================================================*/
extern void hdrrmac4_compute_silence_time( void );

/*===========================================================================
FUNCTION HDRRMAC4_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  Sets the RRI to a fixed value for testing purposes. Since in IS856-A we deal
  with packet sizes rather than rates, rri actually refers to the payload size 
  index.

DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xf, or HDRRMAC4_IS856__A_RRI for
             normal operation.

RETURN VALUE
  Boolean status indicating whether input is valid or not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_set_fixed_rri
(
  uint8 rri
);    

/*===========================================================================
FUNCTION HDRRMAC4_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The supervision timer is initialized and the
  protocol state is set to Inactive.

DEPENDENCIES
  Should be run in HDRTx context, since timers are defined.
  Should be run AFTER hdrrmac4_powerup_init().

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_init (void);



/*===========================================================================
FUNCTION HDRRMAC4_IS_ACTIVE

DESCRIPTION
  This function queries the current state of the protocol.
  
DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = protocol is Active.
  FALSE = protocol is Inactive.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_is_active ( void );




/*===========================================================================
FUNCTION HDRRMAC4_SHUTDOWN                                            EXTERNAL

DESCRIPTION
  This command tells the RTCMAC to shutdown after the current Txing frame 
  has completed.  It does NOT deactivate the protocol.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_shutdown ( void );


/*===========================================================================
FUNCTION HDRRMAC4_START_TX

DESCRIPTION
  This command starts the transmitter after a hdrrmac4_activate() command,
  or restarts the transmitter if it has been stopped.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_start_tx( void );


/*===========================================================================
FUNCTION HDRRMAC4_STOP_TX

DESCRIPTION
  This command stops the transmitter.  If a data packet is being 
  transmitted at the time the command is processed, the protocol will
  wait until that packet tranmission is complete before shutting down.
  Otherwise, the PA is shut down immediately.  A TxStopped indication 
  is given once the transmitter has been shut down.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_stop_tx( void );


/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_process_cmd 
( 
  hdrrmac_cmd_union_type * cmd_ptr                   
);


/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_SIG

DESCRIPTION
  This function handles the task signals assigned to this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  signal - task signal to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_process_sig 
( 
  rex_sigs_type signal  
);


/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_MSG                                         EXTERNAL 

DESCRIPTION
  This function processes incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.
  protocol_instance - Instance of the protocol the message is for
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_process_msg 
( 
  dsm_item_type * rcv_msg_ptr,              
  hdrhai_protocol_instance_enum_type protocol_instance
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_TIMER                                      EXTERNAL

DESCRIPTION
  This function processes the timer expiry events for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_process_timer(uint32 timer_id);



/*===========================================================================

FUNCTION HDRRMAC4_SET_FRAME_OFFSET                                    EXTERNAL

DESCRIPTION
  This function sets the RMAC frame offset.

DEPENDENCIES
  The parameter must not be NULL.
  
PARAMETERS
  frame_offset - the Frame offset

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_set_frame_offset
(            
  uint8 frame_offset
);


/*===========================================================================

FUNCTION HDRRMAC4_PROCESS_IND                                    

DESCRIPTION
  This function processes indications given to RMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for RMAC to process
  ind_data_ptr - pointer to data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrrmac4_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);


/*===========================================================================

FUNCTION HDRRMAC4_NOTIFY_SUSPEND

DESCRIPTION
  This function tells the RTC MAC about an upcoming tune away.
    
DEPENDENCIES
  None.

PARAMETERS
  cancel_suspend - TRUE when scheduled suspend is to be cancelled
  suspend_slot   - If cancel_suspend is FALSE, slot when link is scheduled
                   to be suspended.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac4_notify_suspend
(
  boolean cancel_suspend,
  uint32  suspend_slot
);


/*===========================================================================

FUNCTION HDRRMAC4_SUSPEND_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer to cease sending data over the reverse
  link.  After the currently Tx'ing frame is complete, the PA will be turned
  off.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac4_suspend_connection( void );

/*===========================================================================

FUNCTION HDRRMAC4_RESUME_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer that it may resume sending data over the 
  reverse link.  
    
DEPENDENCIES
  None.

PARAMETERS
  ms - the time (in mS) that the link was off.
  hho_flag - FALSE -> resume from tune-away
             TRUE  -> resume from hard-handoff
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac4_resume_connection
( 
  int ms,
    /* time, in mS, that connection was suspended. */

  boolean hho_flag
    /* FALSE - resume from tune-away
     * TRUE  - resume from hard-handoff
     */
); 

/*===========================================================================

FUNCTION HDRRMAC4_POWERDOWN_TX

DESCRIPTION
  Powers down the Tx section of RF.  Must only be called when the RF is 
  needed while an HDR connection is up.
      
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac4_powerdown_tx
( 
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
);

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_RL_CARRIERS                                 EXTERNAL

DESCRIPTION
  Update the RMAC4 with RL information from TCA.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_update_rl_carriers
(
  hdrrmac_cmd_union_type  *cmd_ptr
);

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_SLP_CARRIER_AND_TX_PILOT_PWR_INFO              EXTERNAL

DESCRIPTION
  Retrieve the current SLP carrier and the TX pilot power from all the 
  assigned reverse CDMA Channels.

DEPENDENCIES
  None.

PARAMETERS
  slp_carrier_ptr - Pointer to the SLP carrier structure to be filled.
  total_tx_pilot_pwr_ptr - Pointer to the total average TX pilot power variable
                           to be filled.  Variable is in unit of 0.5dBm in 2's
                           complement format.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_get_slp_carrier_and_tx_pilot_pwr_info
(
  sys_channel_type  *slp_carrier_ptr,
  int8              *total_tx_pilot_pwr_ptr
);


/*===========================================================================
FUNCTION HDRRMAC4_PA_IS_AVAILABLE                                    EXTERNAL

DESCRIPTION
  This function queries the current availability of the PA.
  
DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = PA is available
  FALSE = PA is not currently available, due to 1) RTC MAC is not active or
          2) RTC MAC is in DRC supervision.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_pa_is_available
(
  hdrhai_protocol_name_enum_type caller
);


/*===========================================================================
FUNCTION HDRRMAC4_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This function allows the caller to request that Reverse Link throttling
  be disabled.  If the RTC MAC is not active at the time of the call, the
  throttle state will be changed the next time it becomes active.
  
  A caller ID is taken as a parameter, along with a boolean to indicate 
  whether the caller is registering the request or de-registering the 
  request.
  
  Note that since this function takes a boolean to vote on whether
  throttling is disables or not, there is no separate 
  "hdrrmac4_enable_throttling" function.

DEPENDENCIES
  None.

PARAMETERS
  caller_id       - The entity requesting that throttling be disabled.
  disable_request - TRUE indicates the caller is requesting disable.
                  - FALSE indicates the caller is ending the request. 
                    (RL throttling may be re-enabled).
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_disable_throttling
(
  hdrrmac_caller_id_enum_type caller_id,
  boolean                      disable_request
);


/*===========================================================================
FUNCTION HDRRMAC4_ENTER_OPEN_STATE                                    INTERNAL

DESCRIPTION
  Enter the Open state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_enter_open_state (void);


/*===========================================================================
FUNCTION HDRRMAC4_ENTER_INACTIVE_STATE                              INTERNAL

DESCRIPTION
  Enter the inactive state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_enter_inactive_state (void);

/*===========================================================================
FUNCTION HDRRMAC4_START_RMAC

DESCRIPTION
  This function is invoked by either hdrrmac4_start_tx() or the RF tx complete
  callback function to initialize and start the RMAC isr.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_start_rmac( void );

#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRRMAC4_SET_TARGET_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to set the maximum RRI value on the 
  specified channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index
  max_rri       - maximum RRI value.

RETURN VALUE
  rri_valid - TRUE if the specified RRI is valid; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_set_target_ps_index
(
  uint8 phy_chan_idx,
  uint8 max_rri
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_SELECTED_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to get the payload size index selected 
  by RTCMAC3.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index

RETURN VALUE
  payload size index in the range [1..12]

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 hdrrmac4_get_selected_ps_index
(
  uint8 phy_chan_idx
);
#endif /* FEATURE_HDR_IS890A */


/*===========================================================================
FUNCTION HDRRMAC4_GET_FLOW_MAPPING                                   EXTERNAL

DESCRIPTION
  Returns the AssociatedFlowsNN attribute to caller. 

DEPENDENCIES
  None.

PARAMETERS
  mapping - Pointer to structure giving the set of upper layer flows mapped 
            to a given MAC flow. 
  flow_id - MAC Flow Id 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_get_flow_mapping
( 
  hdrrmac_flow_mapping_struct_type *mapping,
  uint8 flow_id
);


/*===========================================================================
FUNCTION hdrrmac4_IS_FLOW_ACTIVE                                     EXTERNAL

DESCRIPTION
  Function to find out if a given flow is active. A flow is considered active
  if its BucketLevelMax attribute is > 0

DEPENDENCIES
  None.

PARAMETERS
  MAC flow ID in the range [0x0 - 0xF]

RETURN VALUE
  TRUE if flow is active; false otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_is_flow_active( uint8 flow );

 /* EJECT */
/*=========================================================================

FUNCTION     : HDRRMAC4_GET_CHANGED_FLOW_MAPPING

DESCRIPTION  : 
  This function finds a set of App flows that are either bound or unbound from
  a particular MAC flow and stores it in the arrays bound_flows and 
  unbound_flows respectively.

Note: This function is called from DS task.

DEPENDENCIES : None

PARAMETERS   :
  unbound_flows - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are unbound from this MAC flow due to 
                  recent GAUPing
  bound_flows   - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are bound due to recent GAUPing

RETURN VALUE : 

SIDE EFFECTS : None

=========================================================================*/
extern void hdrrmac4_get_changed_flow_mapping
(
  hdrrmac_flow_mapping_struct_type *unbound_flows,
  hdrrmac_flow_mapping_struct_type *bound_flows
);



/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_THROTTLING

DESCRIPTION
  This function updates the Reverse Link throttling state.
  (Note that normal operation has RL throttling on as per standard.)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_update_throttling( void );


/*===========================================================================
FUNCTION HDRRMAC4_GET_MAC_INFO                                       EXTERNAL

DESCRIPTION
  Returns MAC payload size equivalents (in bytes) for PA headroom, bucket
  level and T2PInflow to caller. 

DEPENDENCIES
  None.

PARAMETERS
  flow     - MAC Flow Id
  mac_info - Pointer to structure with a set of MAC payload size equivalents 
             for PA headroom, bucket level and T2PInflow

RETURN VALUE
  TRUE  - If the fuction succeeds in getting the requested values
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
#if 0 /* TODO */
extern boolean hdrrmac4_get_mac_info
( 
  uint8                  mac_flow_id, 
  hdrrmac4_mac_info_type *mac_info 
);
#endif /* 0 */

/*===========================================================================
FUNCTION HDRRMAC4_GET_CURRENT_TX_PACKET_INFO

DESCRIPTION
  Returns the currently transmitting RRI or packet size. 
  
DEPENDENCIES
  None.

PARAMETERS
  Point to packet info structure to be filled out.

RETURN VALUE
  TRUE if valid data was filled in. 
  FALSE if there was no data (due to PA not on, or RTCMAC not 
    active, for instance).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
);                     


/*===========================================================================
FUNCTION HDRRMAC4_ENTER_DRC_SUPERVISION

DESCRIPTION
  Performs sanity checks on enter drc supervision command and then 
  passes it to RMAC driver which in turn disables RL transmission and 
  reverse power control on the specified carrier.


DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_enter_drc_supervision
(
  hdrrmac_cmd_union_type  *cmd_ptr
);


/*===========================================================================
FUNCTION HDRRMAC4_EXIT_DRC_SUPERVISION

DESCRIPTION
  Performs sanity checks on exit drc supervision command and then 
  passes it to RMAC driver which in turn re-enables RL transmission and 
  reverse power control on the specified carrier.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_exit_drc_supervision
(
  hdrrmac_cmd_union_type  *cmd_ptr
);


/*===========================================================================
FUNCTION HDRRMAC4_START_FLOW_CONTROL

DESCRIPTION
  This function stops RMAC from making packets on non-SLP carriers.  
  hdrrmac4_stop_flow_control must be called to re-enable data on non-SLP 
  carriers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_start_flow_control( void );


/*===========================================================================
FUNCTION HDRRMAC4_STOP_FLOW_CONTROL

DESCRIPTION
  This function brings RMAC out of flow control mode ( Calling 
  hdrrmac4_start_flow_control() puts RMAC in flow control mode ) and 
  re-enables data on non-SLP carriers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac4_stop_flow_control( void );

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*===========================================================================
FUNCTION HDRRMAC4_GET_FRAME_OFFSET                                   EXTERNAL

DESCRIPTION
  This function gets the RMAC frame offset.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  uint8 frame_offset.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4_get_frame_offset ( void );

/*==========================================================================

FUNCTION  HDRRMAC4_UPDATE_DTX_MODE

DESCRIPTION
  This function calls driver to send a message to FW to enable or disable
  DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4_update_dtx_mode( void );
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

/*===========================================================================
FUNCTION HDRRMAC4_CONFIG_FUNNEL_MODE

DESCRIPTION
  This function configures funnel mode in FW.

DEPENDENCIES
  None.

PARAMETERS
  enable - TRUE  => Enable Funnel mode
           FALSE => Disable funnel mode
  data_active_carrier_idx 
         - Index of data active carrier to be conveyed to FW

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_config_funnel_mode
(
  boolean  enable,
  uint8    data_active_carrier_idx
);

#ifdef FEATURE_HDR_MODEM_API
/*==========================================================================

FUNCTION  HDRRMAC4_REPORT_COST_OF_MODEM_POWER_EST

DESCRIPTION
  This function calculates and reports estimated cost of modem power
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Estimated cost of modem power LOW, MEDIUM, HIGH

SIDE EFFECTS
  None

==========================================================================*/
hdrrmac_cost_of_power_est_enum_type 
hdrrmac4_report_cost_of_modem_power_est( void );
#endif /* FEATURE_HDR_MODEM_API */


#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC4_GET_RL_RATE_INFO

DESCRIPTION
  This function populates the following in the passed struct:
  1. Estimated reverse link throughput in kbps
  2. Flag indicating if throughput greater than estimated rate may be possible
  3. Current reverse link throughput in kbps
  4. RLP queue length in octets
  
DEPENDENCIES
  None
 
PARAMETERS
  Pointer to location where reverse link rate info is to be populated

RETURN VALUE 
  None

SIDE EFFECTS
  None
 
NOTE 
  This API should not be called directly by external entities.
  hdrrmac_get_rl_rate_info() should be called instead.
===========================================================================*/
void hdrrmac4_get_rl_rate_info
(
  hdrrmac_rl_rate_info_struct_type * rl_info_ptr
);
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */


#ifdef FEATURE_HDR_FLEX_CONN
/*==========================================================================

FUNCTION  HDRRMAC4_SET_FLEXCONN_MODE

DESCRIPTION
  This function calls appropriate driver function to send a message to FW to
  enable or disable FLEXCONN mode

    
DEPENDENCIES
  None

INPUTS
  dtx_mode:  dtx mode, TRUE means enabling DTX
  frame_offset: frame offset

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4_set_flexconn_mode( boolean flexconn_mode );
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_TRAFFIC_DTX
/*===========================================================================
FUNCTION HDRRMAC4_CHECK_TDTX_FRAB_THRESH                              EXTERNAL

DESCRIPTION
  Check whether FRAB of all carriers are less than than the threshold.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE: if FRAB of all carriers are less than than the threshold.
  otherwise FALSE.
  

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_check_tdtx_frab_thresh ( void );

#endif /* FEATURE_HDR_TRAFFIC_DTX */

#endif /* FEATURE_HDR_REVB_SUPPORTED */

#endif /* HDRRMAC4_H */

