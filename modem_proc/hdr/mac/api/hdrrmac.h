#ifndef HDRRMAC_H
#define HDRRMAC_H
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
    
  hdrrmac_process_timer
    Process the timer expiry events for Reverse Traffic Channel MAC protocol.

  hdrrmac_msg_cb
    Callback function that queues received Reverse Traffic Channel MAC 
    messages onto the task message queue.
    
  hdrrmac_start_flow_control - to be removed (use hdrrmac_flow_control)
    Enables RL flow control.  This disables data on non-SLP carriers in RevB.

  hdrrmac_stop_flow_control  - to be removed (use hdrrmac_flow_control)
    Disables RL flow control.
  
  hdrrmac_flow_control
    Enable/Disable RL flow control.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.

  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrmac.h_v   1.17   06 Mar 2003 13:45:02   kevins  $
$Header
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/03/14   arm     HDR QTA: check if TX is on before starting QTA
07/16/14   arm     Added NV control to enable/disable QTA.
06/04/14   arm     Changes to support DO to G QTA
11/14/13   arm     Enabled tDTX by default. 
12/10/13   rmg     Added interface to obtain RL rate info.
10/02/13   rmg     Added support for RL rate estimate and RLP queue info for
                   modem API.
08/06/13   rmg     Added power down reason to RMAC power down command.
07/10/13   arm     Supported traffic DTX. 
05/15/13   rmv     Added updates for Cost of Modem Power API support 
02/17/13   arm     Supported network dtx and device only dtx co-existence.
04/10/13   smd     Enabled DTX/FC by default (for DI PLs).
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
07/27/12   rmg     Added support for Enhanced PA Backoff.
06/20/12   smd     Supported Device only DTX and FlexCONN.
01/04/12   rmg     Changed hdrrmac_process_fw_msg() to accept received 
                   message pointer as an argument.
10/24/11   smd     Moved more to SU API header files.
09/23/11   smd     SU API cleanup.
08/08/11   rmg     Added changes to process suspend/resume in HDRTX context.
04/25/11   ljl     Aded tx powerdown callback.
09/23/10   rmg     Added support for FTM client for RL throttling.
06/21/10   cnx     Fixed compiler warnings.
06/10/10   rmg     Added hdrrmac_flow_control to support thermal flow control.
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac_enter/exit_drc_supervision().
12/01/09   rmg     Added support to call a cb once TCA is processed by MC RMAC.
11/11/09   etv     Initialize RMAC MSGR module only in online mode.
09/22/09   rmg     Added support for CPU based RL flow control.
09/17/09   etv     Added support for RMAC0 Q6 interface changes.
03/11/09   kss     Add Rev B support for hdrrmac_get_current_tx_packet_info().
03/10/09   rmg     Added reportable field to subaset_info_type.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
11/29/07   cc      Minor code review comment change.
09/05/07   kss     Added Rev A support to hdrrmac_get_current_tx_packet_info().
09/05/07   kss     Added hdrrmac_get_current_tx_packet_info() function.
08/08/07   cc      Defined common flow mapping structures used by both rmac3/4.
07/30/07   cc      Added protocol timer support.
07/17/07   cc      Code review comments.
07/12/07   cc      Added hdrrmac_update_rl_carriers() & 
                   hdrrmac_get_slp_carrier_and_tx_pilot_pwr_info support for
                   MSM7500 Rev. B integration.
03/03/07   rmg     Added overhead channel gains compensation
09/14/06   kss     Add ability to cancel suspend.
08/22/06   kss     Added hdrrmac_notify_suspend().
06/22/06   mpa     Removed FEATURE_HDR_RL_RATE_INERTIA featurization.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
10/13/05   kss     Added hdrrmac_config_early_termination() test function.
09/06/05   kss     Updated hdrrmac_set_fixed_rri() to return boolean.
08/02/05   kss     Added hdrrmac_powerdown_tx command.
07/25/05   kss     Added RTAP id enum for hdrrmac_disable_throttling().
01/25/05   etv     Cleaned lint errors. 
01/12/05   kss     Added hdrrmac_disable_throttling() function.
11/29/04   grl     Added connected state stat logging support.
10/08/04   kss     Added GAUP support.
11/14/03   kss     Added time parameter to hdrrmac_resume_connection().
03/06/03   kss     Added hdrrmac_pa_is_available() query function.
01/10/03   kss     Added rate inertia interface functions.
01/08/02   kss     Added hdrrmac_suspend_connection(), _resume_connection(),
                   _powerdown_tx(), and _powerup_tx() function prototypes.
12/06/01   kss     Added hdrrmac_shutdown() command.
11/07/01   kss     Added hdrrmac_set_fixed_rri() for test.
10/29/01   mpa     Added hdrrmac_reset_dm_rtc_rate_count().
09/07/01   kss     Added hdrrmac_set_silence_params() function.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
05/08/01   kss     Added prototype for hdrrmac_set_frame_offset().
12/07/00   kss     Added logging interface.
03/30/00   st      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#ifdef FEATURE_HDR_REVB
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#endif /* FEATURE_HDR_REVB */
#include "dsm.h"
#include "rex.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrtx.h"
#ifdef FEATURE_HDR_THERMAL_BACKOFF
#include "ds707_ccm.h"
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

#include "hdrl1_api.h"

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRRMAC_MAX_NUM_RL_CARRIERS         3


#if defined(FEATURE_HDR_FLEX_CONN)|| defined(FEATURE_HDR_DEVICE_ONLY_DTX)|| \
    defined( FEATURE_HDR_TRAFFIC_DTX )
#define HDRRMAC_EFS_NV_DTX_CFG "/nv/item_files/modem/hdr/mac/hdr_dtx_cfg"

/* bit 0 is to disable device only dtx */
#define HDRRMAC_EFS_NV_DEVICE_ONLY_DTX_DISABLED   0x01
/* bit 1 is to disable flexconn */
#define HDRRMAC_EFS_NV_FLEX_CONN_DISABLED         0x02
/* bit 2 is to disable/enable  tDTX */
#define HDRRMAC_EFS_NV_TRAFFIC_DTX_DISABLED       0x04
/* bit 3 is to enable QTA */
#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
|| FEATURE_HDR_TRAFFIC_DTX */

/*---------------------------------------------------------------------------
                              PROTOCOL COMMANDS

  The Reverse Traffic Channel MAC protocol can receive the following commands.
---------------------------------------------------------------------------*/

/*
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRRMAC_DEACTIVATE_CMD=0,
  HDRRMAC_ACTIVATE_CMD=1,
  HDRRMAC_START_TX_CMD=2,
  HDRRMAC_STOP_TX_CMD=3,
  HDRRMAC_SHUTDOWN_CMD=4,
  HDRRMAC_POWERUP_TX_CMD=5,
  HDRRMAC_POWERDOWN_TX_CMD=6,
  HDRRMAC_CONFIGURE = 7,
#ifdef FEATURE_HDR_REVB
  HDRRMAC_UPDATE_RL_CARRIERS_CMD=8,
#endif /* FEATURE_HDR_REVB */
  HDRRMAC_ENTER_DRC_SUPERVISION_CMD=9,
  HDRRMAC_EXIT_DRC_SUPERVISION_CMD=10,
#ifdef FEATURE_HDR_THERMAL_BACKOFF
  HDRRMAC_CTRL_PA_BACKOFF_CMD=11,
#endif
  HDRRMAC_NOTIFY_SUSPEND_CMD=12,
  HDRRMAC_SUSPEND_CMD=13,
  HDRRMAC_RESUME_CMD=14
#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
  ,HDRRMAC_SET_DTX_CMD=15
#endif
#ifdef FEATURE_HDR_FLEX_CONN
  ,HDRRMAC_SET_FLEXCONN_CMD=16
#endif
#ifdef FEATURE_HDR_MODEM_API
  ,HDRRMAC_REPORT_COST_OF_MODEM_POWER=17
#endif /* FEATURE_HDR_MODEM_API */

} 
hdrrmac_cmd_enum_type;

/* Command header type. */
typedef struct                         
{                                      
  hdrrmac_cmd_enum_type          cmd;           
  hdrhai_protocol_name_enum_type sender; /* protocol giving command */
} 
hdrrmac_cmd_hdr_type;


/* ReverseRateLimit message RateLimit field values.  The values assigned in
   this enum type match those used in the ReverseRateLimit message. */
typedef enum
{
  HDRRMAC_PILOT_ONLY = 0x0,
  HDRRMAC_9K6_BPS    = 0x1,
  HDRRMAC_19K2_BPS   = 0x2,
  HDRRMAC_38K4_BPS   = 0x3,
  HDRRMAC_76K8_BPS   = 0x4,
  HDRRMAC_153K6_BPS  = 0x5,
  HDRRMAC_NUM_RATES,

/* Defined for using hdrrmac_set_fixed_rri() to return to 
   normal operation */
  HDRRMAC_IS856_RRI = 0xF

} 
hdrrmac_rate_enum_type;

#ifdef FEATURE_HDR_MODEM_API

typedef enum
{
   HDRRMAC_EST_COST_OF_POWER_IS_INVALID=0,
   HDRRMAC_EST_COST_OF_POWER_IS_LOW=1,
   HDRRMAC_EST_COST_OF_POWER_IS_MEDIUM=2,
   HDRRMAC_EST_COST_OF_POWER_IS_HIGH=3
} hdrrmac_cost_of_power_est_enum_type;

/* Lower and Upper Boundaries to categorize the estimated cost of modem power
   <0 dBm  EST_COST_OF_POWER_IS_LOW
   0 to 10 dBm EST_COST_OF_POWER_IS_MEDIUM
   >10 dBm EST_COST_OF_POWER_IS_HIGH
*/
#define HDRRMAC_COST_OF_POWER_L_BOUND 0
#define HDRRMAC_COST_OF_POWER_U_BOUND 10

/* DRC Channel Gain and Ack Channel gain in 0.25dB (Q.2) to use in
   Cost of Modem power estmate calculation */
#define HDRRMAC_IDLE_DRC_GAIN_FOR_POWER_COST_EST -6
#define HDRRMAC_IDLE_ACK_GAIN_FOR_POWER_COST_EST 12

#endif /* FEATURE_HDR_MODEM_API */

#ifdef FEATURE_HDR_REVB
#define HDRRMAC_MC_MAX_NUM_RL_CHANS 3
#define HDRRMAC_MC_MAX_NUM_SUBASETS 3

/* RL channel information */
typedef struct
{
  sys_channel_type            channel_record;
    /* BandClass and ChannelNumber */

  uint8                       drop_rank;
    /* DroppingRank from TCA.  Lower value to be dropped first. */

  uint8                       rl_chan_index;
    /* Physical channel index for this RL */

  uint8                       subaset_index;
    /* Index into subaset_info[].  Specifies the associated FL for this RL. */
} hdrrmac_rl_carrier_info_type;

/* Multicarrier Channel Gain Structure */
typedef struct
{
  int8                        drc;
    /* DRCChannelGainBase from TCA in 0.5 dB*/

  int8                        ack;
    /* ACKChannelGain from TCA in 0.5 dB */
} hdrrmac_mc_channel_gains_struct_type;

/* Sub-Active Set Info. Structure */
typedef struct
{
  uint8                                 subaset_index;
    /* Sub-Active Set Index */

  hdrrmac_mc_channel_gains_struct_type  gains;
    /* DRC/Ack Channel Gains for this sub-active set */

  boolean                               dsc_enabled;
    /* if DSC channel enabled for this sub-active set */

  sys_channel_type                      feedback_rl_channel_record;
    /* RL channel record on which DRC/DSC/Ack for this sub-active set to be
       transmitted */

  boolean                                reportable;
    /* if the subactive set is reportable */
} hdrrmac_subaset_info_type;

/* TCA processed callback pointer type.  The callback is called after 
   TCA is processed by MC RMAC subframe processing. */
typedef void ( *hdrrmac_tca_processed_cb_type ) ( void );

/* HDRRMAC_UPDATE_RL_CARRIERS_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  uint8                                 num_rls;
    /* Number of RL channels contained in this command */

  hdrrmac_rl_carrier_info_type          rl_carrier[HDRRMAC_MC_MAX_NUM_RL_CHANS];
    /* Info. for each RL */

  uint8                                 frame_offset;
    /* Frame Offset in uint of slots */

  uint8                                 dsc_gain_base;
    /* DSC Channel Gain from TCA in unit of -0.5dB */

  uint8                                 num_subasets;
    /* Number of sub-active set info. contained in this command */

  hdrrmac_subaset_info_type             subaset_info[HDRRMAC_MC_MAX_NUM_SUBASETS];
    /* Info. for each sub-active set. */

  hdrrmac_tca_processed_cb_type         tca_processed_cb_ptr;
    /* MC RMAC calls this callback once it processes TCA */

} hdrrmac_cmd_update_rl_carriers_struct_type;
#endif /* FEATURE_HDR_REVB */

/* HDRRMAC_ENTER/EXIT_DRC_SUPERVISION_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  uint8                                 rl_carrier_mask;
    /* Physical channel ID mask.  Bit 'n' should be set to indicate that 
       reverse channel 'n' is now in /out of DRC supervision. */

} hdrrmac_cmd_drc_supervision_struct_type;

/* HDRRMAC_POWERDOWN_TX_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  hdrrmac_tx_stop_reason_enum_type      reason;
    /* Tx powerdown reason */

  hdrrmac_tx_powerdown_cb_type          cb;
    /* Tx powerdown callback function */
} hdrrmac_cmd_tx_powerdown_struct_type;
 
#ifdef FEATURE_HDR_THERMAL_BACKOFF
/* HDRRMAC_CNTRL_PA_BACKOFF_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  ds707_ccm_flow_control_cmd_enum_type  cmd;
    /* Command from CCM */

} hdrrmac_cmd_thermal_pa_backoff_struct_type;
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

/* HDRRMAC_NOTIFY_SUSPEND_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  int                                   slots;
    /* Slots until suspend */

} hdrrmac_cmd_notify_suspend_struct_type;

/* HDRRMAC_RESUME_CMD command structure */
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  int                                   ms;
    /* Time in ms the connection was suspended */
#ifdef FEATURE_HDR_QTA
  boolean                               is_qta;
#endif /* FEATURE_HDR_QTA */

} hdrrmac_cmd_resume_struct_type;


#ifdef FEATURE_HDR_DEVICE_ONLY_DTX 
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  boolean                               dtx_mode;
    /* whether to enable dtx mode */

} hdrrmac_cmd_set_dtx_struct_type;
#endif /*  FEATURE_HDR_DEVICE_ONLY_DTX  */


#ifdef FEATURE_HDR_FLEX_CONN
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  boolean                               flexconn_mode;
    /* whether to enable flexconn mode */

} hdrrmac_cmd_set_flexconn_struct_type;
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_QTA
typedef struct
{
  hdrrmac_cmd_hdr_type                  hdr;
    /* command header */

  boolean                               is_qta;
    /* Are we enabling TX after QTA */

} hdrrmac_cmd_powerup_tx_struct_type;
#endif /* FEATURE_HDR_QTA */

/* Command union type. */
typedef union                   
{                                      
  hdrrmac_cmd_hdr_type                          hdr;           
#ifdef FEATURE_HDR_REVB
  hdrrmac_cmd_update_rl_carriers_struct_type    update_rl_carriers;
#endif /* FEATURE_HDR_REVB */
  hdrrmac_cmd_drc_supervision_struct_type       drc_supervision;
  hdrrmac_cmd_tx_powerdown_struct_type          tx_powerdown;
#ifdef FEATURE_HDR_THERMAL_BACKOFF
  hdrrmac_cmd_thermal_pa_backoff_struct_type    thermal_pa_backoff;
#endif
  hdrrmac_cmd_notify_suspend_struct_type        notify_suspend;
  hdrrmac_cmd_resume_struct_type                resume;
#ifdef FEATURE_HDR_DEVICE_ONLY_DTX 
  hdrrmac_cmd_set_dtx_struct_type               set_dtx;
#endif
#ifdef FEATURE_HDR_FLEX_CONN
  hdrrmac_cmd_set_flexconn_struct_type          set_flexconn;
#endif
#ifdef FEATURE_HDR_QTA
  hdrrmac_cmd_powerup_tx_struct_type            tx_powerup;
#endif
} hdrrmac_cmd_union_type;


/* Channel gains*/
typedef struct
{
  int8   drc; /* DRC channel gain in  0.5 dB */
  int8   ack; /* ACK channel gain in  0.5 dB */
  uint8  dsc; /* DSC channel gain in -0.5 dB */
}
hdrrmac_channel_gains_struct_type;

/* Value used to cancel a suspend time set with hdrrmac_notify_suspend(). */
#define HDRRMAC_CANCEL_SUSPEND -1

/* Structure for info on the packet currently being tx'd on a carrier. */
typedef struct {

  sys_channel_type channel_record;
    /* Carrier Channel. UNDEFINED for Rev 0 or Rev A! */

  uint8 ps_index;            
    /* Packet size index (also known as RRI). 0-5 in Rev 0, 
                                              0-12 in Rev A/B */

  boolean packet_is_lolat; 
    /* Indicates that the current subframe of Rev A/B is using boosted LoLat 
       parameters. Only valid for Rev A/B. */

  uint16 t2p;             
    /* Traffic-to-pilot ratio of data channel. In 1/4 dB (Q2) units. */

} hdrrmac_carrier_tx_packet_info_type;


/* Structure for returning info on all packets currently being tx'd. */
typedef struct {

  uint8 subtype;
    /* Current subtype.  0 or 1 for Rev 0, 3 for Rev A, 4 for Rev B.
      Only carrier_info[0] will be filled in for Rev 0/A.  */

  uint8 num_carriers;
    /* Number of active RL carriers. */

  hdrrmac_carrier_tx_packet_info_type carrier_info[HDRRMAC_MC_MAX_NUM_RL_CHANS];
    /* Info on each carriers currently tx'ing packets. */

} hdrrmac_current_tx_packet_info_type;


/*===========================================================================
FUNCTION HDRRMAC_POWERUP_INIT

DESCRIPTION
  This function initializes RMAC session configuration.

DEPENDENCIES
  Note that this is run in HDRMC context.
 
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_powerup_init ( void );

/*===========================================================================
FUNCTION HDRRMAC_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  Sets the RRI to a fixed value for testing purposes.
  
DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xf, or HDRRMAC_IS856_RRI for
             normal operation.
  
RETURN VALUE
  TRUE If the given RRI is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac_set_fixed_rri
(
  byte rri
);    


/*===========================================================================
FUNCTION HDRRMAC_CONFIG_EARLY_TERMINATION                       EXTERNAL

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
extern boolean hdrrmac_config_early_termination
(
  int config_mode
);

/*===========================================================================
FUNCTION HDRRMAC_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The supervision timer is initialized and the
  protocol state is set to Inactive.

DEPENDENCIES
  Should be run in HDRTx context, since timers are defined.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_init ( void );



/*===========================================================================
FUNCTION HDRRMAC_IS_ACTIVE

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
extern boolean hdrrmac_is_active ( hdrhai_protocol_name_enum_type caller );


/*===========================================================================
FUNCTION HDRRMAC_ACTIVATE

DESCRIPTION
  This command transitions the protocol to the Setup Rate state.  If the 
  protocol is already in the Setup or Open state then no action is taken. 
  Hdrrmac_start_tx() must be called after activatation to enable 
  transmission.

DEPENDENCIES
  The protocol must be initialized by calling the hdrrmac_init function prior
  to activation.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  Simon might get stressed out.
===========================================================================*/
extern void hdrrmac_activate ( hdrhai_protocol_name_enum_type sender );


/*===========================================================================
FUNCTION HDRRMAC_DEACTIVATE

DESCRIPTION
  This command transitions the protocol to the Inactive state and clears the 
  Reverse Traffic Channel supervision timer.  If the protocol is already in
  the Inactive state then no action is taken.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_deactivate ( hdrhai_protocol_name_enum_type sender );


/*===========================================================================
FUNCTION HDRRMAC_SHUTDOWN                                            EXTERNAL

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
extern void hdrrmac_shutdown ( hdrhai_protocol_name_enum_type sender );


/*===========================================================================
FUNCTION HDRRMAC_START_TX

DESCRIPTION
  This command starts the transmitter after a hdrrmac_activate() command,
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
extern void hdrrmac_start_tx(hdrhai_protocol_name_enum_type sender);


/*===========================================================================
FUNCTION HDRRMAC_STOP_TX

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
extern void hdrrmac_stop_tx(hdrhai_protocol_name_enum_type sender);


/*===========================================================================
FUNCTION HDRRMAC_PROCESS_CMD

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
extern void hdrrmac_process_cmd 
( 
  hdrrmac_cmd_union_type * cmd_ptr                   
);


/*===========================================================================
FUNCTION HDRRMAC_PROCESS_SIG

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
extern void hdrrmac_process_sig 
( 
  rex_sigs_type signal  
);


/*===========================================================================
FUNCTION HDRRMAC_PROCESS_MSG                                         EXTERNAL 

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
extern void hdrrmac_process_msg 
( 
  dsm_item_type * rcv_msg_ptr,              
  hdrhai_protocol_instance_enum_type protocol_instance
);


/*===========================================================================
FUNCTION HDRRMAC_PROCESS_TIMER                                       EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac_process_timer(uint32 timer_id);


/*===========================================================================
FUNCTION HDRRMAC_MSG_CB

DESCRIPTION
  This function processes incoming messagess for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr  - Pointer to dsm item chain holding the msg
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_msg_cb
(
  dsm_item_type* msg_ptr
);

/*===========================================================================
FUNCTION HDRRMAC_CONFIG_MSG_CB                                       EXTERNAL

DESCRIPTION
  This callback function queues incoming messages for the in config instance
  of this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_config_msg_cb 
( 
  dsm_item_type * msg_ptr
);


/*===========================================================================
FUNCTION HDRRMAC_IND_CB

DESCRIPTION
  This function processes incoming indications for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication from hdrhai.h
  ind_data_ptr - Pointer to any indication-specific data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);


/*===========================================================================
FUNCTION HDRRMAC_RESET_RTC_RATE_COUNT                               

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
extern void hdrrmac_reset_rtc_rate_count( void );

/*===========================================================================
FUNCTION HDRRMAC_RESET_DM_RTC_RATE_COUNT

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
extern void hdrrmac_reset_dm_rtc_rate_count( void );

/*===========================================================================
FUNCTION HDRRMAC_LOG_RTC_RATE_COUNT

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
extern void hdrrmac_log_rtc_rate_count( void );


/*===========================================================================

FUNCTION HDRRMAC_SET_CHANNEL_PARAMS                                  EXTERNAL

DESCRIPTION
  This function 
  - conveys DRC, ACK and DSC channel gain information to RMAC3
  - sets RMAC frame offset

DEPENDENCIES
  None
  
PARAMETERS
  channel_gains - DRC, ACK and DSC channel gains received over Traffic 
                  channel Assignment message
  frame_offset  - Frame Offset in slots.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_set_channel_params
(            
  hdrrmac_channel_gains_struct_type *channel_gains,
  uint8 frame_offset
);


/*===========================================================================

FUNCTION HDRRMAC_SET_FRAME_OFFSET                                    EXTERNAL

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
extern void hdrrmac_set_frame_offset
(            
  uint8 frame_offset
);

/*===========================================================================

FUNCTION HDRRMAC_SET_SILENCE_DURATION                               EXTERNAL

DESCRIPTION
  This function sets the RMAC silence duration and offset.

DEPENDENCIES
  The parameter must not be NULL.
  
PARAMETERS
  silence_duration 
  silence_period 

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_set_silence_params
( 
  uint8 silence_duration,
  uint8 silence_period 
);

/*===========================================================================

FUNCTION HDRRMAC_PROCESS_IND                                    

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
extern void hdrrmac_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

#ifdef FEATURE_HDR_MODEM_API
/*==========================================================================

FUNCTION  HDRRMAC_PROCESS_REPORT_COST_OF_MODEM_POWER_EST_CMD       EXTERNAL

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
extern hdrrmac_cost_of_power_est_enum_type 
hdrrmac_process_report_cost_of_modem_power_est_cmd( void );
#endif /* FEATURE_HDR_MODEM_API */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC_GET_RL_RATE_INFO

DESCRIPTION
  This function populates the following in the passed struct:
  1. Estimated reverse link throughput in kbps
  2. Flag indicating if throughput greater than estimated rate may be possible
  3. Current reverse link throughput in kbps
  4. RLP queue length
  
DEPENDENCIES
  None
 
PARAMETERS
  Pointer to location where reverse link rate info is to be populated

RETURN VALUE 
  E_SUCCESS       - if valid rate information is populated at the passed 
                    pointer location
  E_NOT_AVAILABLE – Rate information is not available cannot be returned 
                    since AT is not in traffic state
  E_BAD_ADDRESS   - Passed argument is an invalid/NULL pointer

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type hdrrmac_get_rl_rate_info
(
  hdrrmac_rl_rate_info_struct_type * rl_info_ptr
);
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

#ifdef FEATURE_HDR_REVB
/*===========================================================================
FUNCTION HDRRMAC_UPDATE_RL_CARRIERS                                 EXTERNAL

DESCRIPTION
  Update the RMAC with RL carrier information from TCA.

DEPENDENCIES
  None.

PARAMETERS
  sender          - The protocol giving this command (only for software trace)
  num_carriers    - Number of RL records
  rl_carriers_ptr - Pointer to "num_carriers" instnaces of 
                    hdrrmac_rl_carrier_info_type
  frame_offset    - Frame offset in unit of slots
  dsc_gain_base   - DSC channel gain in unit of -0.5dB
  num_subasets    - Number of sub-active sets
  subaset_ptr     - Pointer to "num_subasets" instances of
                    hdrrmac_subaset_info_type
  tca_proc_cb_ptr - Pointer to function to be called once TCA is processed by 
                    RMAC.  This is only used in RevB.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac_update_rl_carriers
(
  hdrhai_protocol_name_enum_type  sender,
  uint8                           num_carriers,
  hdrrmac_rl_carrier_info_type    *rl_carrier_ptr,
  uint8                           frame_offset,
  uint8                           dsc_gain_base,
  uint8                           num_subasets,
  hdrrmac_subaset_info_type       *subaset_ptr,
  hdrrmac_tca_processed_cb_type   tca_proc_cb_ptr
);

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC_GET_SLP_CARRIER_AND_TX_PILOT_PWR_INFO              EXTERNAL

DESCRIPTION
  Retrieve the current SLP carrier and the TX pilot power from all the 
  assigned reverse CDMA Channels.  This function currently is only intended
  to be called by RUP1 when RMAC4 is in use.

DEPENDENCIES
  None.

PARAMETERS
  slp_carrier_ptr - Pointer to the SLP carrier structure to be filled.
  total_tx_pilot_pwr_ptr - Pointer to the total average TX pilot power variable
                           to be filled.  Variable is in unit of 0.5dBm in 2's
                           complement format.

RETURN VALUE
  success - TRUE if function executes successfully;
            FALSE if function failes (due to wrong subtype).

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac_get_slp_carrier_and_tx_pilot_pwr_info
(
  sys_channel_type  *slp_carrier_ptr,
  int8              *total_tx_pilot_pwr_ptr
);
#endif /* FEATURE_HDR_REVB */

/*===========================================================================
FUNCTION HDRRMAC_ENABLE_RATE_INERTIA                                EXTERNAL

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
void hdrrmac_enable_rate_inertia( void );

/*===========================================================================
FUNCTION HDRRMAC_DISABLE_RATE_INERTIA                               EXTERNAL

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
void hdrrmac_disable_rate_inertia( void );


/*===========================================================================
FUNCTION HDRRMAC_PA_IS_AVAILABLE                                    EXTERNAL

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
extern boolean hdrrmac_pa_is_available
(
  hdrhai_protocol_name_enum_type caller
);

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION HDRRMAC_UPDATE_CONFIG_INFO                               EXTERNAL

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
extern void hdrrmac_update_config_info( void );
#endif /* FEATURE_HDR_QOS */


/*===========================================================================
FUNCTION HDRRMAC_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This function allows the caller to request that Reverse Link throttling
  be disabled.  If the RTC MAC is not active at the time of the call, the
  throttle state will be changed the next time it becomes active.
  
  A caller ID is taken as a parameter, along with a boolean to indicate 
  whether the caller is registering the request or de-registering the 
  request.
  
  Note that since this function takes a boolean to vote on whether
  throttling is disables or not, there is no separate 
  "hdrrmac_enable_throttling" function.

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
extern void hdrrmac_disable_throttling
(
  hdrrmac_caller_id_enum_type caller_id,
  boolean                     disable_request
);


/*===========================================================================

FUNCTION HDRRMAC_CONFIGURE

DESCRIPTION
  This function is called by SCP to start AT-initiated configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The calling protocol.

RETURN VALUE
  TRUE if the configuration is required; FALSE if configuration is not
  needed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrmac_configure
(
  hdrhai_protocol_name_enum_type sender
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC_GET_FLOW_MAPPING                                   EXTERNAL

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
void hdrrmac_get_flow_mapping
( 
  hdrrmac_flow_mapping_struct_type *mapping, 
  uint8 flow_id 
);


 /* EJECT */
/*=========================================================================

FUNCTION     : HDRRMAC_GET_CHANGED_FLOW_MAPPING

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

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmac_get_changed_flow_mapping
(
  hdrrmac_flow_mapping_struct_type *unbound_flows,
  hdrrmac_flow_mapping_struct_type *bound_flows
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC_IS_FLOW_ACTIVE                                     EXTERNAL

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
boolean hdrrmac_is_flow_active
(
  uint8 flow
);


 /* EJECT */
#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRRMAC_SET_TARGET_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to set the maximum RRI value on the 
  specified channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.  Ignore if subtype is not rmac4.
  max_rri       - maximum RRI value.

RETURN VALUE
  rri_valid - TRUE if the specified RRI is valid; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac_set_target_ps_index
(
  uint8 phy_chan_idx,
  uint8 max_rri
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC_GET_SELECTED_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to get the payload size index selected 
  by RTCMAC3 or RTCMAC4.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.  Ignored if subtype is not rmac4.

RETURN VALUE
  payload size index in the range [1..12]

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac_get_selected_ps_index
(
  uint8 phy_chan_idx
);
#endif /* FEATURE_HDR_IS890A */


/*===========================================================================
FUNCTION HDRRMAC_GET_CURRENT_TX_PACKET_INFO

DESCRIPTION
  Returns the currently transmitting RRI or packet size, as well as the
  T2P ratio. 

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
extern boolean hdrrmac_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
);


/*===========================================================================
FUNCTION HDRRMAC_START_FLOW_CONTROL

DESCRIPTION
  This function is used to reduce CPU consumption by stopping the production
  of reverse link packets on non-SLP carriers.  Once in this mode, RMAC will 
  not support data on non-SLP carriers till hdrrmac_stop_flow_control
  is called to bring RMAC out of flow conrol mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_start_flow_control( void );


/*===========================================================================
FUNCTION HDRRMAC_STOP_FLOW_CONTROL

DESCRIPTION
  This function brings RMAC out of flow control mode ( Calling 
  hdrrmac_start_flow_control() puts RMAC in flow control mode ) and 
  re-enables data on non-SLP flows.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_stop_flow_control( void );


/*===========================================================================
FUNCTION HDRRMAC_ENTER_DRC_SUPERVISION                              EXTERNAL

DESCRIPTION
  The function queues DRC supervision entry command to RMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  sender        - The protocol giving this command (only for software trace)
  phy_chan_mask - Physical channel ID mask
                  Set 'n'th bit to indicate that reverse channel 'n' is 
                  now in DRC supervision

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_enter_drc_supervision
(
  hdrhai_protocol_name_enum_type  sender,
  uint16                          phy_chan_mask
);


/*===========================================================================
FUNCTION HDRRMAC_EXIT_DRC_SUPERVISION                               EXTERNAL

DESCRIPTION
  The function queues DRC supervision exit command to RMAC.
    
DEPENDENCIES
  None.

PARAMETERS
  sender        - The protocol giving this command (only for software trace)
  phy_chan_mask - Physical channel ID mask
                  Set 'n'th bit to indicate that reverse channel 'n' is 
                  now out of DRC supervision

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_exit_drc_supervision
(
  hdrhai_protocol_name_enum_type  sender,
  uint16 phy_chan_mask
);


/*===========================================================================
FUNCTION HDRRMAC_FLOW_CONTROL

DESCRIPTION
  This function is used to flow control by  - 
  1. restricting transmit packet sizes.  Once in this mode, RMAC would 
     continue to restrict transmit packet sizes till hdrrmac_flow_control 
     is called with the first argument set to '12' to  bring RMAC out of 
     packet size based flow control mode.
  2. disabling the production of reverse link packets on non-SLP carriers 
     [only applicable when multicarrier RMAC is operational].  Once in this 
     mode, RMAC will not support data on non-SLP carriers till 
     hdrrmac_flow_control is called with the second argument set to FALSE.
  Examples: 
  1. hdrrmac_flow_control(12,TRUE) stops data on non-SLP carriers.  No 
     restriction is placed on SLP carrier payload.
  2. hdrrmac_flow_control(5,FALSE) enforces payload restriction leve 5 on all
     RL carriers.
  3. hdrrmac_flow_control(5,TRUE) stops data on non-SLP carriers and set the
     payload restriction level on SLP carrier to 5.

DEPENDENCIES
  None.

PARAMETERS
  payload_restriction_level - This is a number from 0 to 12.  This is 
    internally mapped to the max payload level RMAC can transmit.  Number 12 
    removes payload restriction.
  enable_cpu_flow_control - When set to TRUE, data is stopped on non-SLP 
    carriers.  When set to FALSE, data is re-enabled on non-SLP carriers.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_flow_control
(
  uint8 payload_restriction_level,
  boolean enable_cpu_flow_control
);


/*============================================================================
FUNCTION HDRRMAC_PROCESS_FW_MSG

DESCRIPTION
  This function propagates received firmware indication/message to currently
  active RMAC subtype for processing.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrmac_process_fw_msg
(
  hdrtx_msg_s_type * msg_ptr 
);


/*===========================================================================
FUNCTION HDRRMAC_CONFIG_FUNNEL_MODE

DESCRIPTION
  In FTM mode, this function configures funnel mode.

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
void hdrrmac_config_funnel_mode
(
  boolean  enable,
  uint8    data_active_carrier_idx
);


/*============================================================================
  HDRRMAC_MSG_INIT
*/
/**
  Initializes the message interface. 

  Initializes message interface by doing the following. 
    - Creates a messenger Client ID for RMAC module. 
    - Initializes the free queue and event queue. 
    - Registers the rex queues for the client with the messenger. 
    - Registers to receive indication messages. 

  @context HDRTX */
/*===========================================================================*/
extern void hdrrmac_msg_init ( void );

/*============================================================================
  HDRRMAC_MSG_STOP
*/
/**
  Cleans up the message interface. 

  Cleans up the message interface by doing the following. 
    - De-registers the registered messages. 
    - Deletes the messenger Client ID for Decoder module. 

  @context HDRTX */
/*===========================================================================*/
extern void hdrrmac_msg_stop ( void );

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)

/*==========================================================================

FUNCTION  HDRRMAC_START_FLEXCONN_DTX_MONITOR

DESCRIPTION
  Start to monitor RL/FL and decide whether to turn on/off FLEXCONN mode or
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
void hdrrmac_start_flexconn_dtx_monitor( boolean resume_timer_for_flexconn );


/*==========================================================================

FUNCTION  HDRRMAC_STOP_FLEXCONN_DTX_MONITOR

DESCRIPTION
  Stop monitoring RL/FL and turn off FLEXCONN mode or DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_stop_flexconn_dtx_monitor( boolean suspend_timer_for_flexconn );

#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*==========================================================================

FUNCTION  HDRRMAC_CHECK_DTX_AFTER_TCA

DESCRIPTION
  This function update DTXFrameOffset based on the received TCA.It may disable
  dtx mode if conditions are met. It checks drc_length too.
    
DEPENDENCIES
  None

INPUTS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_check_dtx_after_tca( boolean tca_for_soft_ho );
#endif /*  FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_FLEX_CONN
/*==========================================================================

FUNCTION  HDRRMAC_UPDATE_NUM_MAC_INDEXES

DESCRIPTION
  This function updates number of mac indexes to rmac
    
DEPENDENCIES
  None

INPUTS
  uint16: num_mac_indexes

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_update_num_mac_indexes( uint16 num_mac_indexes );

/*===========================================================================

FUNCTION HDRRMAC_GET_FLEXCONN_MODE                                    EXTERNAL

DESCRIPTION
  This function gets the RMAC current flexconn_mode

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  flexconn_mode.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac_get_flexconn_mode( void );

#endif /* FEATURE_HDR_FLEX_CONN */

#if defined(FEATURE_HDR_REVB_DTX_MODE)&& defined(FEATURE_HDR_DEVICE_ONLY_DTX)
/*==========================================================================

FUNCTION  HDRRMAC_STOP_DEVICE_ONLY_DTX_MONITOR

DESCRIPTION
  Stop monitoring RL/FL and turn off DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_stop_device_only_dtx_monitor( void );
 
/*==========================================================================

FUNCTION  HDRRMAC_START_DEVICE_ONLY_DTX_MONITOR

DESCRIPTION
  Start to monitor RL/FL and decide whether to turn on/off DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_start_device_only_dtx_monitor ( void );

#endif /* defined(FEATURE_HDR_REVB_DTX_MODE)&& defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

/*===========================================================================

FUNCTION HDRRMAC_GET_FRAME_OFFSET                                    EXTERNAL

DESCRIPTION
  This function gets the RMAC frame offset.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  frame_offset.
  
SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac_get_frame_offset( void );

#endif /* HDRRMAC_H */
