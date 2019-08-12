#ifndef GARB_UTILS_H
#define GARB_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                    G L 1  A R B I T R A T O R  U T I L S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_arbitrator_utils.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
04/011/14  ab        CR646584: GSM Timeline reporting for WLAN COEX
02/07/14   mc        CR.687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
27/05/14   cs/ch     CR651795 TRM Updated changes for DR-DSDS
12/02/14   am        CR611265 RACH timing update to GARB
09/11/13   mc        CR.572746 : COEX : DSDA G+G VBATT power back-offs for TxTx collisions are not correctly applied
09/11/13   mc        CR.556742 : COEX : G+G : Enhancements for monitor positioning and improved SDCCH/FACCH conflict detection
01/10/13   sk        CR552259 CR sync from 1.0/1.1
15/08/13   sk        CR524039 GBTA bringup changes
29/07/13   sk        CR519663 Partial QBTA bringup changes
05/04/13   mc        CR.470155 : COEX Support - GERAN arbitrator framework - PS integration
25/03/13   mc        CR.464496 : COEX Support - GERAN arbitrator SW CXM integration
13/03/13   mc        CR.459487 : COEX Support - GERAN arbitrator framework
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_arbitrator_types.h"
#include "gl1_arbitrator_tables.h"

#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                        PUBLIC DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION      garb_peer_gas_id

DESCRIPTION   Returns the GAS ID of the peer SUB

RETURN VALUE  gas_id

===========================================================================*/
gas_id_t garb_peer_gas_id(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_is_g_plus_g

DESCRIPTION   This function indicates that G+G is active

RETURN VALUE  gas_id

===========================================================================*/
boolean garb_is_g_plus_g(void);

/*===========================================================================

FUNCTION      garb_get_channel_type

DESCRIPTION   This function maps the channel type reported by the message
              layer (gl1_hw_channel_type) to the arbitrator's internal
              representation of channel type (garb_channel_t).

RETURN VALUE  garb_channel_t

===========================================================================*/
garb_channel_t garb_get_channel_type(gl1_hw_channel_type gl1_hw_channel, garb_rxtx_t rxtx, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_rx_channel_type

DESCRIPTION   Returns the channel type associated with the specified frame
              and timeslot.

RETURN VALUE  channel type

===========================================================================*/
garb_channel_t garb_get_rx_channel_type(uint32 fn, uint8 ts, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_tx_channel_type

DESCRIPTION   Returns the channel type associated with the specified frame
              and timeslot.

RETURN VALUE  channel type

===========================================================================*/
garb_channel_t garb_get_tx_channel_type(uint32 fn, uint8 ts, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_arfcn_to_centre_freq_kHz

DESCRIPTION   Returns the centre frequency in kHz for the specified ARFCN
              and band.

RETURN VALUE  Frequency in kHz

===========================================================================*/
uint32 garb_arfcn_to_centre_freq_kHz(ARFCN_T arfcn, garb_rxtx_t rxtx , gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_is_access_burst

DESCRIPTION   This function indicates if the specified slot is an access
              burst. This is used to determing if a timing advance should
              be applied.

RETURN VALUE  TRUE if the specified burst is an access burst

===========================================================================*/
boolean garb_is_access_burst(garb_channel_t channel, uint32 fn, uint8 slot, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_slot_s_time

DESCRIPTION   This function gets the USTMR start time of the specified slot,
              adjusted to account for Guard Time, RF Tuning Time and Search
              Width and Timing Advance.

RETURN VALUE  USTMR

===========================================================================*/
uint32 garb_get_slot_s_time(garb_frame_t* frame, garb_rxtx_t rxtx, uint8 slot, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_slot_e_time

DESCRIPTION   This function gets the USTMR start time of the specified slot,
              adjusted to account for Guard Time, RF Tuning Time and Search
              Width and Timing Advance.

RETURN VALUE  USTMR

===========================================================================*/
uint32 garb_get_slot_e_time(garb_frame_t* frame, garb_rxtx_t rxtx, uint8 slot, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_vbatt_tx_power_backoffs

DESCRIPTION   This function takes the Tx power (dBm) of the P1 (high tier)
              and P2 (low tier) SUBs along with a total Tx power budget. If
              the total Tx power is in excess of the specified budget the
              budgeted power will be distributed between the two SUBs
              accoring to priority.

              This function passes back the adjusted Tx power (dBm) and
              backoff (dB) required to mitigate VBATT droop.

RETURN VALUE  TRUE is power backoff should be applied

===========================================================================*/
boolean garb_get_vbatt_tx_power_backoffs(double  TxP_SUB1_dBm, double TxP_SUB2_dBm, uint8 pri_SUB1, uint8 pri_SUB2, double* TxP_SUB1_backoff_dB, double* TxP_SUB2_backoff_dB, gas_id_t gas_id );

/*===========================================================================

FUNCTION      garb_get_multi_rat_mode

DESCRIPTION   This function determines the current Multi-RAT Mode

RETURN VALUE  Multi-RAT Mode

===========================================================================*/
garb_multi_sim_mode_t garb_get_multi_rat_mode(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_cxm_priority_tier

DESCRIPTION   This function determines the CXM priority tier for a specified
              slot based on the channel type and Multi-RAT mode.

              For G+G modes the CXM priority is left undecided as this will
              later be determined by G+G conflict detection.

RETURN VALUE  Multi SIM Mode

===========================================================================*/
cxm_activity_type garb_get_cxm_priority_tier(garb_rxtx_t rxtx, garb_channel_t channel_type, uint8 ts, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_is_sdcch_saach_frame

DESCRIPTION   Returns TRUE if the FN is a SDCCH or SACCH frame. This
              function also updates the SDCCH parameters with information
              about the current block type (SDCCH Rx, SDCCH Tx, SACCH Rx or
              SACCH Tx).

RETURN VALUE  TRUE if the FN is a SDCCH or SACCH frame

===========================================================================*/
boolean garb_is_sdcch_saach_frame(uint32 fn, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_is_tch_frame

DESCRIPTION   Returns TRUE if the FN is a TCH, SACCH or FACCH frame. This
              function also updates the CS parameters to indicate with
              information about the current block type (TCH, SACCH or FACCH)

RETURN VALUE  TRUE if the FN is an TCH, SACCH or FACCH frame

===========================================================================*/
boolean garb_is_tch_frame(uint32 fn, gas_id_t gas_id);

boolean garb_is_rach_frame( uint32 fn, gas_id_t gas_id );

/*===========================================================================

FUNCTION      garb_is_candidate_rrbp_block

DESCRIPTION   Returns TRUE if the FN is in a candidate RRBP block

RETURN VALUE  TRUE if the FN is in a candidate RRBP block

===========================================================================*/
boolean garb_is_rrbp_block_boundary(uint32 fn, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_next_rrbp_fn

DESCRIPTION   Returns the FN of the next RRBP and updates the PS parameters
              with the RRBP message type, burst type and slammed TN.

RETURN VALUE  FN of the next RRBP

===========================================================================*/
uint32 garb_get_next_rrbp_fn(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_mon_s_time

DESCRIPTION   Returns Monitor s_time.  ( Same for DSDS/DSDA/SS )

RETURN VALUE  Returns Monitor s_time.

===========================================================================*/
uint32 garb_get_mon_s_time( garb_frame_t* frame,gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_mon_e_time

DESCRIPTION   Returns Monitor e_time.  ( Same for DSDS/DSDA/SS )

RETURN VALUE  Returns Monitor e_time.

===========================================================================*/
uint32 garb_get_mon_e_time( garb_frame_t* frame,gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_update_scell_ncell_arfcns

DESCRIPTION   Updated SCELL and NCELL arfcns as per FN scheduling.

RETURN VALUE  

===========================================================================*/
void  garb_update_scell_ncell_arfcns(ARFCN_T  *arfcns,  uint32  num_arfcns,gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_get_monscan_arfcn

DESCRIPTION   Returns NCELL Monitor ARFCN as per Fn MOD 13/26 in PS/CS

RETURN VALUE  Returns Monitor e_time.

===========================================================================*/
ARFCN_T garb_get_monscan_arfcn(uint32 FN,gas_id_t gas_id);

/*===========================================================================

FUNCTION      gpl1_get_serving_cell_next_arfcn

DESCRIPTION   Returns SCELl ARFCN as per FN MOD 13/26 based on CS/PS/SDCCH

RETURN VALUE  

===========================================================================*/
ARFCN_T gpl1_get_serving_cell_next_arfcn(uint32 FN,gas_id_t gas_id);

#endif /* GARB_UTILS_H */
