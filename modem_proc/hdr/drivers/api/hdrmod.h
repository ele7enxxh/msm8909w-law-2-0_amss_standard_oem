#ifndef HDRMOD_H
#define HDRMOD_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   H D R    M O D U L A T O R    D R I V E R
                    
                           H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for using the HDR 
  modulator driver.
  
  Copyright (c) 2000-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/api/hdrmod.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/15   wsh     Fixed crash when ASDiv is sent between DAC start/resp
11/27/13   arm     Fixed compilation error due to tDTX. 
06/09/14   kss     Added support for handling TX_RESET_REQ_IND processing.
05/27/14   wsh     Fixed compile error for Jolokia build
11/26/13   rmg     Supported enabling carriers in FW without a frame delay.
10/08/13   mbs     Added support for BOLT interface changes
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
08/06/13   rmg     Added API to wait for PA off.
02/17/13   arm     Supported network based dtx. 
03/26/13   smd     Fixed RevB MC call.
03/15/13   cnx     Fix Triton compiler errors.
03/12/13   smd     Added Triton fixes.
01/17/13   rmg     Removed references to feature HDRMOD_PA_UNDER_FW_CONTROL.
03/19/12   ljl     Fixed Dime compiler errors.
01/26/12   ljl     Replaced tramp with DAL.
01/04/12   rmg     Added hdrmod_process_fw_msgs().
06/21/11   rmg     Made changes to process tx_start/stop commands in hdrtx ctxt
05/04/11   kss     Added hdrmod_init().
04/27/11   kss     Added hdrmod_tx_start() and hdrmod_tx_stop().
04/16/11   kss     Supported FW-controlled Tx enable.
12/07/09   kss     Rename hdrmod_reset() as hdrmod_enter(). 
09/25/07   rmg     Added changes to support RevB RMAC DRV.
04/28/05   kss     Added hdrmod_set_rev_a_tx_frame_interrupt_offset().
02/10/05   kss     Initial changes for Rev A support.
11/12/04   kss     Changes for MSM6800 Rev 0 support.
06/15/03   kss     Added hdrmod_exit().
02/11/02   kss     Added interface for Long Code mask=0 test mode.
11/06/00   kss     Added interface for frame rate counter.
05/25/00   kss     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "qw.h"
#include "hdrhai.h"
#include "hdrtx.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Enumeration of reverse rate types */
typedef enum
{
  REVERSE_RATE_PILOT = 0,  
  REVERSE_RATE_9600 = 1,
  REVERSE_RATE_19200 = 2,
  REVERSE_RATE_38400 = 3,
  REVERSE_RATE_76800 = 4,
  REVERSE_RATE_153600 = 5,
  HDRMOD_NUM_REVERSE_RATES
}
hdrmod_rri_enum_type;


/* Type for exporting the reverse rate frame counts */
typedef struct {
  uint32 rate9600_cnt;
  uint32 rate19200_cnt;
  uint32 rate38400_cnt;
  uint32 rate76800_cnt;
  uint32 rate153600_cnt;
} hdrmod_reverse_frame_count_type;

/* Modulator modes */
typedef enum
{
#ifdef FEATURE_HDR_UNIFIED_TX
  HDRMOD_TRAFFIC_MODE_REV0 = 0,
  HDRMOD_TRAFFIC_MODE_REVA = 1,
  HDRMOD_TRAFFIC_MODE_REVB = 2,
  HDRMOD_TRAFFIC_MODE_REVAB = 1
#elif defined(FEATURE_HDR_BOLT_MODEM)
   HDRMOD_TRAFFIC_MODE_REV0 = 0,
   HDRMOD_TRAFFIC_MODE_REVA = 1,
   HDRMOD_TRAFFIC_MODE_REVB = 2
#else
   HDRMOD_TRAFFIC_MODE_REV0 = 0,
   HDRMOD_TRAFFIC_MODE_REVA = 1,
   HDRMOD_TRAFFIC_MODE_REVB = 1
#endif /* FEATURE_HDR_UNIFIED_TX */
}
hdrmod_traffic_mode_enum_type;

/* Temporary define for backwards compatibility */
#define hdrmod_reset hdrmod_enter

/* HDRMOD commands */
typedef enum 
{
  HDRMOD_TX_START      =0,
  HDRMOD_TX_STOP       =1

} hdrmod_cmd_enum_type;

/* Command header type. */
typedef struct 
{
  hdrmod_cmd_enum_type           cmd;    /* Command to execute */
  hdrhai_protocol_name_enum_type sender; /* Protocol sending the command */

} hdrmod_cmd_hdr_type;

/* Command union type. */
typedef union                   
{                                      
  hdrmod_cmd_hdr_type                          hdr;

} hdrmod_cmd_union_type;

#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
/* PA on/off Monitor Type */
typedef void (*hdrmod_pa_mode_monitor_type)(boolean enabled);
hdrmod_pa_mode_monitor_type hdrmod_pa_mode_monitor;
#endif /* FEATURE_HDR_REVB_DTX_MODE || FEATURE_HDR_TRAFFIC_DTX */ 


#ifdef FEATURE_HDR_UNIFIED_TX
#define HDRMOD_MOD_CFG_REVA    ( HDRMOD_TRAFFIC_MODE_REVAB << HDRMODE_MODULATOR_SHIFT )
#define HDRMOD_MOD_CFG_REVB    ( HDRMOD_TRAFFIC_MODE_REVAB << HDRMODE_MODULATOR_SHIFT )
#elif defined( FEATURE_HDR_BOLT_MODEM )
#define HDRMOD_MOD_CFG_REVA    HDRMOD_TRAFFIC_MODE_REVA
#define HDRMOD_MOD_CFG_REVB    HDRMOD_TRAFFIC_MODE_REVB
#else
#define HDRMOD_MOD_CFG_REVA    ( HDRMOD_TRAFFIC_MODE_REVA << HDRMODE_MODULATOR_SHIFT )
#define HDRMOD_MOD_CFG_REVB    ( HDRMOD_TRAFFIC_MODE_REVB << HDRMODE_MODULATOR_SHIFT )
#endif /* FEATURE_HDR_UNIFIED_TX */

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRMOD_INIT

DESCRIPTION
  Performs any one-time init needed by modualtor driver.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_init( void );

/*===========================================================================
FUNCTION HDRMOD_ENTER

DESCRIPTION
  Places the modulator into a known state. Resets PA control bits.
  (Does not reset overall modulator.)
    
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_enter( void );


/*===========================================================================
FUNCTION HDRMOD_SET_PN_LONG_CODE_MASK

DESCRIPTION
  Load the 42-bit I and Q long code masks into the modulator.
  
DEPENDENCIES
  None.

PARAMETERS
  i            - PN long code mask for I
  q            - PN long code mask for Q
  carrier_mask - Carriers for which PN generator is to be enabled

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_set_pn_long_code_mask( 
  qword i,
  qword q
#ifdef FEATURE_HDR_BOLT_MODEM
  ,uint8 carrier_mask
#endif /* FEATURE_HDR_BOLT_MODEM */
);


/*===========================================================================
FUNCTION HDRMOD_SET_TX_FRAME_START_OFFSET

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
extern void hdrmod_set_tx_frame_start_offset(
  byte slot_offset 
);


/*===========================================================================
FUNCTION HDRMOD_SET_TX_FRAME_INTERRUPT_OFFSET

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
extern void hdrmod_set_tx_frame_interrupt_offset(
  uint16 int_offset
);


/*===========================================================================
FUNCTION HDRMOD_ENTER_ACCESS_MODE

DESCRIPTION
  Enters access mode.  In preparation for an access probe, the DRC is turned
  off.  Once access mode is entered, hdrmod_exit_access_mode() must be called
  to exit before a reverse traffic link can be established.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_enter_access_mode( void );


/*===========================================================================
FUNCTION HDRMOD_EXIT_ACCESS_MODE

DESCRIPTION
  Disables access mode.  The PA control bits are returned to the default
  state, and DRC channel tranmission is re-eneabled.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_exit_access_mode( void );


/*===========================================================================
FUNCTION HDRMOD_SETUP_ACCESS_PROBE

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
extern void hdrmod_setup_access_probe( void );
 

/*===========================================================================
FUNCTION HDRMOD_BEGIN_ACCESS_PROBE

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
extern void hdrmod_begin_access_probe( void );


/*===========================================================================
FUNCTION HDRMOD_END_ACCESS_PROBE

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
extern void hdrmod_end_access_probe( void );


/*===========================================================================
FUNCTION HDRMOD_ENABLE_PA

DESCRIPTION
  Turns on the PA for the reverse traffic channel.  The PA is turned on
  immediately.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_enable_pa( void );


/*===========================================================================
FUNCTION HDRMOD_DISABLE_PA

DESCRIPTION
  Turns off the PA immediately.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_disable_pa( void );


/*===========================================================================
FUNCTION HDRMOD_SET_REVERSE_RATE

DESCRIPTION
  Sets the modulator reverse rate based on the given frame size.
  
DEPENDENCIES
  None.

PARAMETERS
  frame_size - the size (in bytes) of the next frame.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_set_reverse_rate(
  uint16 frame_size
);


/*===========================================================================
FUNCTION HDRMOD_SET_RRI

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
extern void hdrmod_set_rri(
  uint16 rri
);



/*===========================================================================
FUNCTION HDRMOD_GET_TX_MAX_FRAME_SIZE

DESCRIPTION
  Returns the current maximum allowed frame size (in bytes) due to signal
  conditions.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  The maximum allowed frame size for Tx.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 hdrmod_get_tx_max_frame_size( void );



/*===========================================================================
FUNCTION HDRMOD_PA_IS_ON

DESCRIPTION
  Indicates whether the PA is currently on.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if PA is on, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmod_pa_is_on( void );


/*===========================================================================
FUNCTION HDRMOD_RESET_FRAME_COUNT

DESCRIPTION
  Resets internal modulator count of frames sent.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_reset_reverse_frame_count( void );


/*===========================================================================
FUNCTION HDRMOD_GET_REVERSE_FRAME_COUNT

DESCRIPTION
  Fills in a structure with the number of reverse link frames transmitted 
  for each rate.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_get_reverse_frame_count (
  hdrmod_reverse_frame_count_type * frame_count_ptr
);


/*===========================================================================
FUNCTION HDRMOD_FRAME_SIZE_TO_RRI                                 

DESCRIPTION
  Converts a frame size to the corresponding modulator RRI value.
  
DEPENDENCIES
  None.

PARAMETERS
  frame_size - the frame size to find the RRI value for.

RETURN VALUE
  The RRI value.

SIDE EFFECTS
  None.
===========================================================================*/
hdrmod_rri_enum_type hdrmod_frame_size_to_rri( 
  uint16 frame_size
);


/*===========================================================================
FUNCTION HDRMOD_SET_ZERO_LC_MASK

DESCRIPTION
  Enables the long code mask to always be set to zero, for testing purposes.

DEPENDENCIES
  None.

PARAMETERS
  test_val - 1 uses zero for the PN long code mask,
             anything else disables the "zero test mode".

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_set_zero_lc_mask(
  int8 test_val
);


/*===========================================================================
FUNCTION HDRMOD_EXIT

DESCRIPTION
  Disables HDR mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_exit( void );



/*===========================================================================
FUNCTION HDRMOD_ENABLE_FRAME_ISR

DESCRIPTION
  Enables the Tx frame interrupt. 
  
DEPENDENCIES
  None.

PARAMETERS
  isr_ptr - the routine to use as the ISR

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_enable_frame_isr
(
  void (*isr_ptr)( uint32 ) /* The routine to use as the ISR */
);
   


/*===========================================================================
FUNCTION HDRMOD_DISABLE_FRAME_ISR

DESCRIPTION
  Disables the Tx frame interrupt.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_disable_frame_isr(void );


/*===========================================================================
FUNCTION HDRMOD_FRAME_INT_INIT

DESCRIPTION
  Initializes the second-tier frame interrupt.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_frame_int_init( void );


/*===========================================================================
FUNCTION HDRMOD_FRAME_INT_CLEAR

DESCRIPTION
  Clears the second-tier frame interrupt so the next interrupt can occur.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_frame_int_clear( void );



#ifdef FEATURE_HDR_REVA_L1
/*===========================================================================
FUNCTION HDRMOD_SET_TRAFFIC_MODE

DESCRIPTION
  Sets the modulator to the specified traffic mode, either Rev 0 or Rev A.
  
DEPENDENCIES
  None.

PARAMETERS
  MODE - the mode to set the modulator.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_set_traffic_mode( hdrmod_traffic_mode_enum_type mode );



/*===========================================================================
FUNCTION HDRMOD_SET_TX_SUBFRAME_INTERRUPT_OFFSET

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
extern void hdrmod_set_tx_subframe_interrupt_offset(
  uint16 int_offset
);



/*===========================================================================
FUNCTION HDRMOD_CONFIG_CARRIER

DESCRIPTION
  Configures reverse link carrier.

DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The MCDO carrier to be configured (0-2)
  enable   - TRUE  - enable transmission on the carrier specified by 
                     chan_idx
             FALSE - disable transmission on the carrier
  foc_mask - Specifies the forward overhead channel bit mask
             000 - Transmit no FOC
             001 - Enable carrier 0 FOC transmission
             010 - Enable carrier 1 FOC transmission
             011 - Enable carrier 0 and carrier 1 FOC transmission
             100 - Enable carrier 2 FOC transmission
             101 - Enable carrier 0 and carrier 2 FOC transmission
             110 - Enable carrier 1 and carrier 2 FOC transmission
             111 - Enable carrier 0, 1 and 2 FOC transmission

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrmod_config_carrier
(
  uint8   chan_idx, /* Physical channel index (0-2) */
  boolean enable,   /* Whether the channel should be enabled or disabled */
  uint8   foc_mask  /* FOC (Forward Overhead Channel) bit mask */
);

#endif /* FEATURE_HDR_REVA_L1 */

/*===========================================================================

FUNCTION HDRMOD_SETUP_TX_TIMED_ENABLE

DESCRIPTION 
  Sends message to firmware to enable carrier and TX_ON at the slot
  offset indicated.

DEPENDENCIES
  Time at which to enable Tx must be at least 2 slots in future.

PARAMETERS
  immediate   - Indicates that the disable should happen ASAP (will still 
                take at least couple slots )
  slot_offset - Should be 0-15; PA will enable on next occurence 
                (ie, when lower bits of hstr matches offset in halfslots).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmod_setup_tx_timed_enable
(
  boolean immediate,
    /* Whether to disable PA/carrier ASAP */
  uint8 slot_offset
    /* Slot (0-15) at which to turn on PA. */
);

/*===========================================================================

FUNCTION HDRMOD_SETUP_TX_TIMED_DISABLE

DESCRIPTION 
  Sends message to firmware to disable carrier and PA at the slot
  offset indicated, or immediately.

DEPENDENCIES
  If not immediate, slot_offset should be at least 2 slots in future.

PARAMETERS
  immediate   - Indicates that the disable should happen immediately.
  slot_offset - Should be 0-15; PA will disable on next occurence 
                (ie, when lower bits of hstr matches offset in halfslots).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmod_setup_tx_timed_disable
(
   boolean immediate,
     /* Whether to disable PA/carrier immediately */
   uint8   slot_offset
     /* Slot (0-15) at which to turn on PA. */
);


/*===========================================================================

FUNCTION HDRMOD_TX_START

DESCRIPTION 
  Prefills and triggers the TX DAC.  Call after enabling RF TX.

  Tx start processing is executed in HDRTX task context.  The caller, if
  executing from a non-HDRTX task, must wait for at least two frames 
  ( ~53 ms ) for tx start processing to complete when using this function.

  The function uses frame ISR to time sending of TX DAC start msg to 
  firmware and to wait till next frame boundary for DAC start to take 
  an effect.  If a frame ISR is registered with hdrmod, it restored at
  the end of this function.
 
DEPENDENCIES
  None.
 
PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  Uninstalls frame ISR.  The ISR is restored at the end of tx start 
  processing.
===========================================================================*/
void hdrmod_tx_start( void );

/*===========================================================================

FUNCTION HDRMOD_TX_STOP

DESCRIPTION 
  Disables Tx DAC via FW. Call before disabling RF TX.
  Tx stop processing is executed in HDRTX task context.
 
DEPENDENCIES
  Assumes TX DAC was enabled.
 
PARAMETERS
  None. 
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmod_tx_stop( void );

/*===========================================================================

FUNCTION HDRMOD_SEND_TX_STOP

DESCRIPTION 
  Sends Tx DAC stop message.
  
DEPENDENCIES
  Assumes TX DAC was enabled.
 
PARAMETERS
  None. 
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmod_send_tx_stop( void );

/*===========================================================================
FUNCTION HDRRMOD_PROCESS_CMD

DESCRIPTION
  This function processes the commands for modulator that were queued
  on the HDRTX task command queue.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_process_cmd
(
  hdrmod_cmd_union_type * cmd_ptr
);


/*============================================================================
FUNCTION HDRMOD_PROCESS_FW_MSG

DESCRIPTION
  This function processes firmware indication/message destined for HDR MOD.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_process_fw_msg
(
  hdrtx_msg_s_type * msg_ptr 
);
#ifdef FEATURE_HDR_UNIFIED_TX
/*============================================================================
FUNCTION HDRMOD_CLEAR_CARRIER_ENABLED

DESCRIPTION
  This function clears carrier enable registers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_clear_carrier_enabled( void );

#endif /* FEATURE_HDR_UNIFIED_TX */

#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
/*===========================================================================
FUNCTION HDRMOD_REGISTER_PA_MODE_MONITOR                          EXTERNAL

DESCRIPTION
  This function is used to register callback to monitor changes to PA.
  Note only one callback can be registered at a time.
  

DEPENDENCIES
  None.

PARAMETERS
  cb - PA mode monitor

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_register_pa_mode_monitor 
( 
   hdrmod_pa_mode_monitor_type cb 
 );

/*===========================================================================
FUNCTION HDRMOD_DEREGISTER_PA_MODE_MONITOR                          EXTERNAL

DESCRIPTION
  This function is used to deregister callback to monitor changes to PA.
  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmod_deregister_pa_mode_monitor ( void );
#endif /* FEATURE_HDR_REVB_DTX_MODE || FEATURE_HDR_TRAFFIC_DTX */


/*===========================================================================
FUNCTION HDRMOD_WAIT_FOR_PA_OFF

DESCRIPTION
  If no PA state change request is pending, the function returns immediately
    If PA is off, the function returns success/TRUE.
    If PA is on, the function returns failure/FALSE.
  If PA state change request is pending, the function waits for PA response.
    If the response is PA off, the function returns success/TRUE.
    If the response is PA on, the function returns failure/FALSE.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if the wait has resulted in PA off response
  FALSE - otherwise

SIDE EFFECTS
  None.
 
NOTE 
  The function must be executed in HDRTX task context. 
===========================================================================*/
boolean hdrmod_wait_for_pa_off ( void );

#ifdef FEATURE_HDR_SELECTIVE_TXD
/*===========================================================================

FUNCTION HDRMOD_DAC_RESP_IS_PENDING

DESCRIPTION
  This function returns if DAC resp is pending. It is pending if
  1) DAC start has been sent and resp hasn't been received.
  2) DAC stop has been sent and resp hasn't been received.

DEPENDENCIES
  None

RETURN VALUE
  True - If DAC start/stop resp is pending
  False - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmod_dac_resp_is_pending( void );
#endif /* FEATURE_HDR_SELECTIVE_TXD */
#endif   /* HDRMOD_H */

