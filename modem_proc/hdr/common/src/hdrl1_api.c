/*===========================================================================

           H D R   C P   A P I   W R A P P E R   M O D U L E 


DESCRIPTION
  This file is the interface exported by HDR CP 

EXTERNALIZED FUNCTIONS (Global)
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2011-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/12/15   bb/vlc  Added hdrl1_srch_is_in_suspend_state(). 
05/11/15   kss     Added support for modem tx power stats.
02/25/15   ljl     Fixed compiler warning. 
12/17/14   vke     Added API to handle the flow control command from DS
12/19/13   rmv     Added interface to obtain Cost of Modem Power estimate 
12/10/13   rmg     Added interface to obtain RL rate info.
02/14/13   dsp     Added hdrsrch_acq_search_active().  
12/20/12   vko     Fixed compiler warnings
06/13/12   dsp     Used ceiling in linear to dB conversion API
04/10/12   dsp     Added hdrl1_srch_time_until_tune_complete()  
04/09/12   smd     Added two new SU APIs.
02/16/12   smd     Added hdrl1_rmac_start_flow_control. 
02/13/12   smd     Moved hdrl1_rmac_stop_flow_control() prototype to header file.
11/15/11   smd     Added hdrl1_srch_get_band,,hdrl1_enc_eram_lock/unlock.  
11/14/11   smd     Fixed two link errors.
10/24/11   smd     Moved more to SU API header files.
10/19/11   smd     SU API cleanup update.    
09/15/11   smd     Created this file.

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdrl1_api.h"
#include "hdrfmac.h"
#include "hdrsrch.h"
#include "hdrrx.h"
#include "hdrtx.h"
#include "hdrrmac.h"
#include "hdrrmac3.h"
#include "hdrmdspmcr.h"
#include "hdrlmac.h"
#include "hdrrmac0.h"
#include "hdrsrchstates.h"
#include "hdrsrchset.h"
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
extern void hdrenc_eram_lock( void );
extern void hdrenc_eram_unlock( void );
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION HDRL1_SRCH_GET_SINR_AND_RXAGC

DESCRIPTION
  This is a wrapper function. 
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrl1_srch_get_sinr_and_rxagc 
(
  byte    *sinr, 
    /* pointer to SINR indicator value */

  int     *rxagc
    /* pointer to RxAgc value   */    
)
{
  hdrsrch_get_sinr_and_rxagc 
  (
    sinr, 
    rxagc
  );
} /* hdrl1_srch_get_sinr_and_rxagc */


/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_ECIO_AND_IO

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

INPUTS
  Pointers to the locations where EcIo and Io will be stored

RETURN VALUE
  EcIo and Io are returned through the passed pointers.

SIDE EFFECTS
  None

=========================================================================*/
void hdrl1_srch_get_ecio_and_io( int16* ecio, int* io )
{
  hdrsrch_get_ecio_and_io( ecio, io );
} /* hdrl1_srch_get_ecio_and_io */

/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_HDR_RSSI

DESCRIPTION  : This is a wrapper function.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

byte hdrl1_srch_get_hdr_rssi
( 
  void 
)
{
  return hdrsrch_get_hdr_rssi();
} /* hdrl1_srch_get_hdr_rssi */


/*=========================================================================

FUNCTION     : HDRL1_SRCH_LINEAR_ENG_TO_DB

DESCRIPTION  : This function converts linear energy to db in unit of -0.5db

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : energy in db

SIDE EFFECTS : None

=========================================================================*/

int hdrl1_srch_linear_eng_to_db
( 
  int                             lin_eng
    /* Linear energy to be converted to -0.5dB units */
)
{
  return hdrsrchset_eng_to_ecio_ceiling_convert( lin_eng );
} /* hdrl1_srch_linear_eng_to_db */

/*============================================================================

FUNCTION HDRL1_SRCH_GET_BAND

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The band HDR is operating on.

SIDE EFFECTS
  None

============================================================================*/
sys_band_class_e_type hdrl1_srch_get_band( void )
{
  return hdrsrch_get_band();
}/* hdrl1_srch_get_band */

/*=========================================================================

FUNCTION     : HDRL1_SRCH_RUNTIME_DIVERSITY_PREF_CTRL

DESCRIPTION  : This is a wrapper function. 

DEPENDENCIES : 

INPUT        : Whether diversity is on.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrl1_srch_runtime_diversity_pref_ctrl
(
  hdrl1_srch_rx_div_enum_type       div_ctrl
    /* Runtime diversity control */
)
{
  hdrsrch_runtime_diversity_pref_ctrl( div_ctrl );
} /* hdrl1_srch_runtime_diversity_pref_ctrl  */

/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_RX_TX_POWER_INFO

DESCRIPTION
  This is a wrapper function.
    
DEPENDENCIES
  None

INPUTS
  Pointers to the location where Rx and Tx power info structure will be stored

RETURN VALUE
  Rx and Tx agc value are returned through the passed hdrsrch_rx_tx_power_info_type
  pointer.

SIDE EFFECTS
  None

=========================================================================*/

void hdrl1_srch_get_rx_tx_power_info
(
  hdrl1_srch_rx_pwr_info_struct_type   *rx_power_info,
  hdrl1_srch_tx_pwr_info_struct_type   *tx_power_info
)
{
  hdrsrch_get_rx_tx_power_info( rx_power_info, tx_power_info );  
}
/* hdrsrch_get_rx_tx_power_info */
/*==========================================================================

FUNCTION HDRL1_LMAC_GET_SCHDGRP_INFO

DESCRIPTION
  Get the scheduler group report.

DEPENDENCIES

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/

void hdrl1_lmac_get_schdgrp_info
( 
  hdrl1_srch_schdgrp_rpt_type *schdgrp_info_ptr 
)
{

  hdrlmac_get_schdgrp_info(schdgrp_info_ptr);

} /* hdrl1_lmac_get_schdgrp_info */

/*===========================================================================
FUNCTION HDRL1_FMAC_ENABLE_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_fmac_enable_flow_control(uint32 mask)
{
  hdrfmac_enable_flow_control( mask );
} /* hdrl1_fmac_enable_flow_control */


/*===========================================================================
FUNCTION HDRL1_FMAC_DISABLE_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control disabling
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_fmac_disable_flow_control(uint32 mask)
{
  hdrfmac_disable_flow_control( mask );
} /* hdrl1_fmac_disable_flow_control */

/*===========================================================================

FUNCTION HDRL1_FMAC_SET_DRC_VALUE                                EXTERNAL

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link Carrier ID.
  drc - DRC value to set.

RETURN VALUE
  TRUE If the given DRC is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrl1_fmac_set_drc_value
( 
  uint8 drc 
)
{
  return hdrfmac_set_drc_value( drc );
} /* hdrl1_fmac_set_drc_value */

/*===========================================================================

FUNCTION HDRL1_FMAC_SET_ACK_MODE                                 EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link carrier id.
  ack_mode - The mode to enable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrl1_fmac_set_ack_mode
( 
  hdrl1_fmac_ack_mode_enum_type ack_mode
)
{
  hdrfmac_set_ack_mode( ack_mode );
} /* hdrl1_fmac_set_ack_mode */


/*===========================================================================

FUNCTION HDRL1_FMAC_GET_FC_PATTERN                                     EXTERNAL

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The current FC pattern.

SIDE EFFECTS
  None.
===========================================================================*/

hdrl1_fmac_flow_control_pattern_enum_type hdrl1_fmac_get_fc_pattern( void )
{
 return hdrfmac_get_fc_pattern();
} /* hdrl1_fmac_get_fc_pattern */


/*===========================================================================
FUNCTION HDRL1_FMAC_FLOW_CONTROL_CMD

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - Flow control command from DS
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrl1_fmac_flow_control_cmd(uint32 ds_flow_control_cmd)
{
  hdrfmac_control_flow( ds_flow_control_cmd );
} /* hdrl1_fmac_flow_control_cmd */


/*=========================================================================

FUNCTION     : HDRL1_RMAC_GET_CHANGED_FLOW_MAPPING

DESCRIPTION  : 
  This is a wrapper function.

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
void hdrl1_rmac_get_changed_flow_mapping
(
  hdrl1_rmac_flow_mapping_struct_type *unbound_flows,
  hdrl1_rmac_flow_mapping_struct_type *bound_flows
)
{
  hdrrmac_get_changed_flow_mapping
  (
    unbound_flows,
    bound_flows
  );

} /* hdrl1_rmac_get_changed_flow_mapping  */

/*===========================================================================
FUNCTION HDRL1_RMAC_GET_FLOW_MAPPING                                   EXTERNAL

DESCRIPTION
  This is a wrapper function. 

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
void hdrl1_rmac_get_flow_mapping
( 
  hdrl1_rmac_flow_mapping_struct_type *mapping, 
  uint8 flow_id 
)
{
  hdrrmac_get_flow_mapping
  ( 
    mapping, 
    flow_id 
  );
} /* hdrl1_rmac_get_flow_mapping */

/*===========================================================================
FUNCTION HDRL1_RMAC_IS_FLOW_ACTIVE                                     

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  MAC flow ID in the range [0x0 - 0xF]

RETURN VALUE
  TRUE if flow is active; false otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrl1_rmac_is_flow_active( uint8 flow )
{
  return hdrrmac_is_flow_active( flow );
} /* hdrl1_rmac_is_flow_active */


/*===========================================================================
FUNCTION HDRL1_RMAC_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This is a wrapper function

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
void hdrl1_rmac_disable_throttling
(
  hdrl1_rmac_caller_id_enum_type caller_id,
  boolean                        disable_request
)
{
  hdrrmac_disable_throttling( caller_id, disable_request );
} /* hdrl1_rmac_disable_throttling */


/*===========================================================================
FUNCTION HDRL1_RMAC_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  This is a wrapper function.

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
boolean hdrl1_rmac_set_fixed_rri
(
  byte rri
)
{
  return hdrrmac_set_fixed_rri( rri );
} /* hdrl1_rmac_set_fixed_rri */

/*===========================================================================
FUNCTION HDRL1_RMAC_CONFIG_EARLY_TERMINATION                       EXTERNAL

DESCRIPTION
  This is a wrapper function.  

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
boolean hdrl1_rmac_config_early_termination
(
  int config_mode
)
{
  return hdrrmac_config_early_termination( config_mode );
} /* hdrl1_rmac_config_early_termination */


/*===========================================================================
FUNCTION HDRL1_RMAC_STOP_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_stop_flow_control( void )
{
  hdrrmac_stop_flow_control();
} /* hdrl1_rmac_stop_flow_control */


/*===========================================================================
FUNCTION HDRL1_RMAC_STOP_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_start_flow_control( void )
{
  hdrrmac_start_flow_control();
} /* hdrl1_rmac_start_flow_control */

/*===========================================================================
FUNCTION HDRL1_RMAC_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function. 
  
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
void hdrl1_rmac_flow_control
(
  uint8 payload_restriction_level,
  boolean enable_cpu_flow_control
)
{
  hdrrmac_flow_control( payload_restriction_level,enable_cpu_flow_control );
} /* hdrl1_rmac_flow_control */

/*===========================================================================
FUNCTION HDRL1_RMAC_GET_RL_RATE_INFO

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
errno_enum_type hdrl1_rmac_get_rl_rate_info
(
  hdrl1_rmac_rl_rate_info_struct_type * rl_info_ptr
)
{
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  return hdrrmac_get_rl_rate_info(rl_info_ptr);
#else
  return E_NOT_AVAILABLE;
#endif

} /* hdrl1_rmac_get_rl_rate_info */

/*===========================================================================
FUNCTION HDRL1_RMAC0_ENABLE_RATE_INERTIA                                EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac0_enable_rate_inertia( void )
{
  hdrrmac0_enable_rate_inertia();
} /* hdrl1_rmac0_enable_rate_inertia */


/*===========================================================================
FUNCTION HDRL1_RMAC0_DISABLE_RATE_INERTIA                               EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac0_disable_rate_inertia( void )
{                 
  hdrrmac0_disable_rate_inertia();            
}/* hdrl1_rmac0_disable_rate_inertia */

/*===========================================================================
FUNCTION HDRL1_RMAC3_GET_MAC_INFO                                       EXTERNAL

DESCRIPTION
  This is a wrapper function.

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
boolean hdrl1_rmac3_get_mac_info
( 
  uint8 flow, 
  hdrl1_rmac3_mac_info_type *mac_info 
)
{
  return hdrrmac3_get_mac_info ( flow, mac_info );
} /* hdrl1_rmac3_get_mac_info */


/*===========================================================================
FUNCTION HDRL1_DRIVERS_GET_HSTR_COUNT

DESCRIPTION
  This is a wrapper function to return hdr hstr number.

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  current HSTR number

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrl1_drivers_get_hstr_count ( void )
{
  return HDRHSTR_GET_COUNT();
} /* hdrl1_drivers_get_hstr_count */

/*============================================================================

FUNCTION HDRL1_ENC_ERAM_LOCK

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

============================================================================*/
void hdrl1_enc_eram_lock( void )
{
  hdrenc_eram_lock();
}/* hdrl1_enc_eram_lock */

/*============================================================================

FUNCTION HDRL1_ENC_ERAM_UNLOCK

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

============================================================================*/
void hdrl1_enc_eram_unlock( void )
{
  hdrenc_eram_unlock();
}/* hdrl1_enc_eram_unlock */
 
/*============================================================================


FUNCTION HDRL1_SRCH_TIME_UNTIL_TUNE_COMPLETE

DESCRIPTION
  This is a wrapper function. If the HDR tune is happening within 1x tune duration,
  return the time that tune will be completed. Otherwise, return 0ms.
  
DEPENDENCIES
  None
 
PARAMETERS
 tune_duration_1x_us: 1x tune duration in usec

RETURN VALUE
  0: There is no conflict between 1x and HDR RF tune.
  >0 : There is conflict and the time in usec until the HDR tune completes
 
SIDE EFFECTS
  None
 
============================================================================*/
uint32 hdrl1_srch_time_until_tune_complete
(
  uint32 tune_duration_1x_us
  /* 1x tune duration in usec*/
)
{
  uint32 time_to_complete = 0;
  /* Time in usec until to tune complete */

  time_to_complete = hdrsrchsleep_time_until_tune_complete( tune_duration_1x_us );

  return time_to_complete;
}/* hdrl1_srch_time_until_tune_complete */

/*=========================================================================

FUNCTION HDRL1_ACQ_SEARCH_ACTIVE

DESCRIPTION 
  This is wrapper function which returns whether HDR Acq search is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Acq srch is active.
  FALSE: Otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/
boolean hdrl1_acq_search_active(void)
{
    return( hdrsrch_acq_search_active() );
}

/*=========================================================================

FUNCTION     : HDRL1_LMAC_GET_MODEM_API_DATA

DESCRIPTION
  Retrieves Modem API data.
  During connection, data is collected at that instant.
  In idle, data collected is from last wakeup.
  Can be called from any task.
 
DEPENDENCIES
  None

INPUTS
  Pointer to structure in which Modem API data values to be populated
 
RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrl1_lmac_get_modem_api_data
(
  hdrl1_lmac_modem_api_data_type * modem_api_data
)
{
   hdrlmac_get_modem_api_data(modem_api_data);
}

/*============================================================================

FUNCTION HDRL1_SRCH_IS_IN_SUSPEND_STATE

DESCRIPTION
  This function provide a boolean value indicating if HDR is in suspend
  states.  Currently, there are 3 suspend states as listed below.
  
DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE:  HDR is in one of these states:
         HDRSRCH_SUSPENDED_TC_STATE    
         HDRSRCH_SUSPENDED_IDLE_STATE
         HDRSRCH_SUSPENDED_SYNC_STATE
  FALSE: HDR is in another state
 
SIDE EFFECTS
  None
 
============================================================================*/
boolean hdrl1_srch_is_in_suspend_state(void)
{

  return hdrsrch_is_in_suspend();
}/* hdrl1_srch_is_traffic_suspend */

/*===========================================================================
FUNCTION HDRL1_CONFIG_MODEM_POWER_STATS

DESCRIPTION
  This function enables or disables tx duration logging.
 
DEPENDENCIES
  None

PARAMETERS
  enable - Whether to start or stop logging

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrl1_config_modem_power_stats
(
  boolean enable
    /* Enable/disable statistic collection */  
)
{

  hdrtx_config_modem_tx_power_stats( enable );
    /* Call function in hdrtx. */

}


/*===========================================================================
FUNCTION HDRL1_GET_MODEM_POWER_STATS

DESCRIPTION
  This function fills Tx duration values into the provided table.
 
DEPENDENCIES
  None

PARAMETERS
  tx_power_bins - array of max_bins items, to be filled with duration stats.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrl1_get_modem_power_stats
(

  uint32 tx_power_bins[]
    /* Pointer to the bin array */
)
{
  hdrtx_get_modem_tx_power_stats( tx_power_bins);
    /* Call function in hdrtx */

}
