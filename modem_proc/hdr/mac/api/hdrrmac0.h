
#ifndef HDRRMAC0_H
#define HDRRMAC0_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   R E V E R S E   T R A F F I C   C H A N N E L   M A C   
                            
                             P R O T O C O L       

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

  Copyright (c) 2005 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrrmac0.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/04/14   arm     Changes to support DO to G QTA
10/02/13   rmg     Added support for RL rate estimate and RLP queue info for
                   modem API.
05/15/13   rmv     Added updates for Cost of Modem Power API support 
01/04/12   rmg     Changed hdrrmac0_process_fw_msg() to accept received 
                   message pointer as an argument.
07/28/11   ssu     Added hdrrmac0_get_hdrrmac_pkt().
05/31/11   cnx     Added hdrrmac0_get_max_pkt_rri() and hdrrmac0_get_current_pkt_rri().
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac0_enter/exit_drc_supervision().
12/17/09   etv     Cleaned up throttling interface.
12/11/09   rmg     Fixed MAC flow ID initialization for RMAC subtype 1.
09/17/09   etv     Added support for RMAC0 Q6 interface changes.
07/09/08   etv     Moved initialization of pointer to common info from 
                   hdrrmac0_powerup_init to hdrrmac0_init.
11/26/07   cws     Added support for hdrrmac0_set_target_ps_index().
11/30/07   cc      Moved hdrrmac_common pointer initialization from _init
                   to powerup_init function.
10/15/07   cc      Added init_attrib argument in powerup_init function.
06/23/07   kss     Added hdrrmac_get_current_tx_packet_info() function.
06/22/06   mpa     Removed FEATURE_HDR_RL_RATE_INERTIA featurization.
09/09/05   etv     Added SCM support.
09/06/05   kss     Updated hdrrmac0_set_fixed_rri() to return boolean.
08/08/05   kss     Remove hdrrmac0_start_tx().

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrrmaci.h"
#include "hdrrmac.h"
#include "hdrtx.h"


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Power Parameters configuration data type. */
typedef struct
{
  int8  data_offset_rate[6]; /*Index 0 refers to nom offset*/
  uint8 rpc_step;
} hdrrmac0_power_params_struct_type;


/* Rate Parameters configuration data type. */
typedef struct
{
  uint8 transition[8];
} hdrrmac0_rate_params_struct_type;

/* Structure for all config data */
typedef struct
{ 
  hdrrmac0_power_params_struct_type power_params;
  hdrrmac0_rate_params_struct_type  rate_params;
} hdrrmac0_config_struct_type;


/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION HDRRMAC0_POWERUP_INIT

DESCRIPTION
  This function initializes RMAC session configuration.

DEPENDENCIES
  Note that this is run in HDRMC context.
  Should be run BEFORE hdrrmac0_init().
 
DEPENDENCIES
  None.

PARAMETERS
  init_attrib - TRUE- RMAC0 is the current InUse subtype.  Attributes need
                to be initialized; FALSE otherwies.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_powerup_init
(
  boolean init_attrib
);

/*===========================================================================

FUNCTION HDRRMAC0_COMPUTE_SILENCE_TIME

DESCRIPTION
  Computes the next Reverse Silence Interval time

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  hdrrmac0.silence_begin_time is updated with the frame time for the
  next silence interval.

===========================================================================*/
extern void hdrrmac0_compute_silence_time( void );


/*===========================================================================

FUNCTION HDRRMAC0_IS _SILENCE_FRAME

DESCRIPTION
  Checks whether the given frame is in Silence Interval

DEPENDENCIES
  None.

PARAMETERS
  frame_num   - frame which needs to be checked for RLSI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrrmac0_is_silence_frame
( 
  qword frame_start   /* frame which needs to be checked for RLSI */
);


/*===========================================================================
FUNCTION HDRRMAC0_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  Sets the RRI to a fixed value for testing purposes.
  
DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xF, or HDRRMAC_IS856_RRI for
             normal operation.
  
RETURN VALUE
  TRUE If the given RRI is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac0_set_fixed_rri
(
  byte rri
);    

/*===========================================================================
FUNCTION HDRRMAC0_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The supervision timer is initialized and the
  protocol state is set to Inactive.

DEPENDENCIES
  Should be run in HDRTx context, since timers are defined.
  Should be run AFTER hdrrmac0_powerup_init().

PARAMETERS
  common - Pointer to the higher level RMAC common parameter block.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_init(hdrrmac_common_type *common);



/*===========================================================================
FUNCTION HDRRMAC0_IS_ACTIVE

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
extern boolean hdrrmac0_is_active ( void );




/*===========================================================================
FUNCTION HDRRMAC0_SHUTDOWN                                            EXTERNAL

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
extern void hdrrmac0_shutdown ( void );



/*===========================================================================
FUNCTION HDRRMAC0_STOP_TX

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
extern void hdrrmac0_stop_tx( void );


/*===========================================================================
FUNCTION HDRRMAC0_PROCESS_CMD

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
extern void hdrrmac0_process_cmd 
( 
  hdrrmac_cmd_union_type * cmd_ptr                   
);


/*===========================================================================
FUNCTION HDRRMAC0_PROCESS_SIG

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
extern void hdrrmac0_process_sig 
( 
  rex_sigs_type signal  
);


/*===========================================================================
FUNCTION HDRRMAC0_PROCESS_MSG                                         EXTERNAL 

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
extern void hdrrmac0_process_msg 
( 
  dsm_item_type * rcv_msg_ptr,              
  hdrhai_protocol_instance_enum_type protocol_instance
);



/*===========================================================================
FUNCTION HDRRMAC0_RESET_RTC_RATE_COUNT                               

DESCRIPTION
  This function resets the count of reverse traffic frames of each rate.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_reset_rtc_rate_count( void );

/*===========================================================================
FUNCTION HDRRMAC0_RESET_DM_RTC_RATE_COUNT

DESCRIPTION
  This function resets the count of reverse traffic frames of each rate and
  commits the changes to the DM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_reset_dm_rtc_rate_count( void );

/*===========================================================================
FUNCTION HDRRMAC0_LOG_RTC_RATE_COUNT

DESCRIPTION
  This function logs the current rate count.  Note that this may be more
  appropriately implemented as a request/response type message.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_log_rtc_rate_count( void );

/*===========================================================================

FUNCTION HDRRMAC0_SET_FRAME_OFFSET                                    EXTERNAL

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
extern void hdrrmac0_set_frame_offset
(            
  uint8 frame_offset
);


/*===========================================================================

FUNCTION HDRRMAC0_PROCESS_IND                                    

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
extern void hdrrmac0_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRRMAC0_SUSPEND_CONNECTION                            

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
void hdrrmac0_suspend_connection( void );

/*===========================================================================

FUNCTION HDRRMAC0_RESUME_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer that it may resume sending data over the 
  reverse link.  
    
DEPENDENCIES
  None.

PARAMETERS
  ms - the time (in mS) that the link was off.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac0_resume_connection
( 
  int ms  /* time, in mS, that connection was suspended. */
); 

/*===========================================================================

FUNCTION HDRRMAC0_POWERDOWN_TX

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
void hdrrmac0_powerdown_tx
( 
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
);

/*===========================================================================

FUNCTION HDRRMAC0_POWERUP_TX

DESCRIPTION
  Powers up the Tx section of RF.  hdrrmac0_powerdown_tx() must have been 
  called first.
      
DEPENDENCIES
  Hdrrmac0_powerdown_tx() must have been called.
  
PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac0_powerup_tx( void );


/*===========================================================================
FUNCTION HDRRMAC0_ENABLE_RATE_INERTIA                                EXTERNAL

DESCRIPTION
  Enables HDR Reverse Link Rate inertia.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac0_enable_rate_inertia( void );

/*===========================================================================
FUNCTION HDRRMAC0_DISABLE_RATE_INERTIA                               EXTERNAL

DESCRIPTION
  Disables HDR Reverse Link Rate inertia.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac0_disable_rate_inertia( void );


/*===========================================================================
FUNCTION HDRRMAC0_PA_IS_AVAILABLE                                    EXTERNAL

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
extern boolean hdrrmac0_pa_is_available
(
  hdrhai_protocol_name_enum_type caller
);

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION HDRRMAC0_UPDATE_CONFIG_INFO                               EXTERNAL

DESCRIPTION
  Updates the rate parameter info based on new configuration.
  Command is run inline (in calling task).  Note that this may be called 
  in hdrmc task context.  (HDRTX task only reads this structure, and the 
  read is atomic).

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_update_config_info( void );
#endif /* FEATURE_HDR_QOS */


/*===========================================================================
FUNCTION HDRRMAC0_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This function allows the caller to request that Reverse Link throttling
  be disabled.  If the RTC MAC is not active at the time of the call, the
  throttle state will be changed the next time it becomes active.
  
  A caller ID is taken as a parameter, along with a boolean to indicate 
  whether the caller is registering the request or de-registering the 
  request.
  
  Note that since this function takes a boolean to vote on whether
  throttling is disables or not, there is no separate 
  "hdrrmac0_enable_throttling" function.

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
extern void hdrrmac0_disable_throttling
(
  hdrrmac_caller_id_enum_type caller_id,
  boolean                      disable_request
);

/*===========================================================================
FUNCTION HDRRMAC0_UPDATE_THROTTLING                                 INTERNAL

DESCRIPTION
  This function checks whether Reverse Link throttling should be disabled
  and enables/disables based on this. (Note that normal operation has RL 
  throttling on as per standard.)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_update_throttling ( void );


/*===========================================================================
FUNCTION HDRRMAC0_ENTER_OPEN_STATE                                    INTERNAL

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
extern void hdrrmac0_enter_open_state (void);


/*===========================================================================
FUNCTION HDRRMAC0_ENTER_INACTIVE_STATE                              INTERNAL

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
extern void hdrrmac0_enter_inactive_state (void);

/*===========================================================================
FUNCTION HDRRMAC0_START_RMAC

DESCRIPTION
  This function is invoked by either hdrrmac0_start_tx() or the RF tx complete
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
extern void hdrrmac0_start_rmac( void );


/*===========================================================================
FUNCTION HDRRMAC0_GET_CURRENT_TX_PACKET_INFO

DESCRIPTION
  Returns the currently transmitting RRI or packet size. For Rev 0,
  0-5 corresponds to pilot only, 9.6, 19.2, 38.4, 76.8, and 153.6 kpbs 
  respectively.  

DEPENDENCIES
  None.

PARAMETERS
  Pointer to packet info structure to be filled out.

RETURN VALUE
  TRUE if valid data was filled in. 
  FALSE if there was no data (due to PA not on, or RTCMAC not 
    active, for instance).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac0_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
);

#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRRMAC0_SET_TARGET_PS_INDEX

DESCRIPTION
  This function is invoked by RTAP to set the maximum RRI value.

DEPENDENCIES
  None.

PARAMETERS
  max_rri       - maximum RRI value.

RETURN VALUE
  rri_valid - TRUE if the specified RRI is valid; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac0_set_target_ps_index
(
  uint8 max_rri
);
#endif /* FEATURE_HDR_IS890 */


/*===========================================================================
FUNCTION HDRRMAC0_ENTER_DRC_SUPERVISION

DESCRIPTION
  Performs drc supervision entry processing - disables RL transmission and 
  reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_enter_drc_supervision( void );


/*===========================================================================
FUNCTION HDRRMAC0_EXIT_DRC_SUPERVISION

DESCRIPTION
  Performs drc supervision exit processing - re-enables RL transmission and
  reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac0_exit_drc_supervision( void );


/*===========================================================================
FUNCTION HDRRMAC0_INIT_MAC_FLOW_IDS

DESCRIPTION
  This function initializes the mac flow ids based on the current subtype.
  This function should be called on power-up init and every time the RMAC
  subtype changes.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac0_init_mac_flow_ids(void);

/*============================================================================
FUNCTION HDRRMAC0_PROCESS_FW_MSG

DESCRIPTION
  This function processes firmware indication/message destined for RMAC0.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac0_process_fw_msg 
(
  hdrtx_msg_s_type * msg_ptr 
);

/*===========================================================================
FUNCTION HDRRMAC0_GET_MAX_PKT_RRI

DESCRIPTION
  This function returns maximum allowed packet RRI. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Maximum allowed value of packet RRI.

SIDE EFFECTS
  None.
===========================================================================*/
hdrrmac_rate_enum_type hdrrmac0_get_max_pkt_rri( void );

/*===========================================================================
FUNCTION HDRRMAC0_GET_CURRENT_PKT_RRI

DESCRIPTION
  This function returns current packet RRI. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  current value of packet RRI.

SIDE EFFECTS
  None.
===========================================================================*/
hdrrmac_rate_enum_type hdrrmac0_get_current_pkt_rri( void );


#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================
FUNCTION HDRRMAC0_GET_HDRRMAC_PKT

DESCRIPTION
  This function returns the address of the hdrrmac_pkt array. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Address of the hdrrmac_pkt array.

SIDE EFFECTS
  None.
===========================================================================*/
uint8* hdrrmac0_get_hdrrmac_pkt( void );
#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_HDR_MODEM_API
/*==========================================================================

FUNCTION  HDRRMAC0_REPORT_COST_OF_MODEM_POWER_EST

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
hdrrmac0_report_cost_of_modem_power_est( void );
#endif /* FEATURE_HDR_MODEM_API */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC0_INIT_MODEMAPI

DESCRIPTION
  This function initializes the variable used for ModemAPI implementation.
  This function should be called every time a new data call is made.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac0_init_modemapi(void);


/*===========================================================================
FUNCTION HDRRMAC0_GET_RL_RATE_INFO

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
void hdrrmac0_get_rl_rate_info
(
  hdrrmac_rl_rate_info_struct_type * rl_info_ptr
);
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

#endif /* HDRRMAC0_H */
