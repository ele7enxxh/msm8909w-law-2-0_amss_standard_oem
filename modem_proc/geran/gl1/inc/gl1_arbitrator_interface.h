#ifndef GARB_INTERFACE_H
#define GARB_INTERFACE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                    G L 1  A R B I T R A T O R  I N T E R F A C E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_arbitrator_interface.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
27/10/14   am        CR646584 WLAN coex
02/07/14   mc        CR.687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
13/03/14   mc        CR.602368 : COEX : Support for Frequency Hopping over large ARFCN lists
14/02/14   mc        CR.572523 : COEX : Ensure all DL bands are registered with CXM on transition to FIND_BCCH
12/02/14   am        CR.611265 : RACH timing update to GARB
02/10/13   pjr       CR.552448 : DDR Frequency plan implementation - l1_arbitration modification
05/04/13   mc        CR.470155 : COEX Support - GERAN arbitrator framework - PS integration
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

#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "gl1_msg.h"
#include "gl1_hw.h"
#include "rr_l1.h"
#include "gl1_msg_arbitration.h"
#include "gl1_arbitrator_types.h"
#include "l1_sc.h" /* cell_T */

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION      garb_intf_register

DESCRIPTION   This function is called by a GAS to register for arbitration

RETURN VALUE  None

===========================================================================*/
void garb_intf_register(gl1_msg_arbitration_t* gl1_msg_arbitration, sys_modem_device_mode_e_type mode, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_unregister

DESCRIPTION   This function is called by an arbitrated GAS to unregister from
              arbitration.

RETURN VALUE  None

===========================================================================*/
void garb_intf_unregister(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_gstmr_tick

DESCRIPTION   This function is called by GL1 prior to ISR processing. This
              takes a USTMR reference for the on air start time of the
              frame.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_gstmr_tick(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_gstmr_post_tick

DESCRIPTION   The timeline is re-built here : we maintain a timeline of two
              frames duration for each SUB.

              The N+0 frame ("this frame") is built based on the FW commands
              that have just been intercepted on this frame tick. This is an
              actualrepresentation of the frame that will go out over the air
              in 4.615ms + 576us (FTSM) time.

              The N+1 frame ("next frame") is predicted based on GL1 state
              and other parameters.

              This function also triggers any pending frequency, timeline
              or power reports to be sent to MCS.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_gstmr_post_tick(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_find_bcch

DESCRIPTION   This function notifies the arbitrator of a transition into
              the FIND BCCH state.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_find_bcch(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_idle

DESCRIPTION   This function notifies the arbitrator of a transition into
              the IDLE state.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_idle(ARFCN_T arfcn, boolean ccch_sdcch_comb, paging_mode_T paging_mode,
                           byte paging_mfrms, byte paging_mfrm, byte opb, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_exit

DESCRIPTION   This function notifies the arbitrator of an exit from the
              current state.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_exit( gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_access

DESCRIPTION   This function notifies the arbitrator of a transition into
              the RACH state.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_access(ARFCN_T arfcn, uint8 delay, int16 txlev, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_access_update

DESCRIPTION   This function updates the RACH parameters

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_access_update( ARFCN_T arfcn, uint8 delay, int16 txlev, gas_id_t gas_id );

/*===========================================================================

FUNCTION      garb_intf_notify_twophase_access

DESCRIPTION   This function notifies the arbitrator in ACCESS state freq information.


RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_twophase_access(frequency_list_T frequency_list, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_cs_assignment

DESCRIPTION   This function notifies the arbitrator of a CS assignment.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_cs_assignment(uint8 TN, frequency_list_T frequency_list,
                                    uint8 subchannel, boolean is_fr, uint8 sacch_frame,
                                    uint8 idle_frame, garb_get_arfcns_t get_arfcns, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_ps_assignment

DESCRIPTION   This function notifies the arbitrator of a PS assignment.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_ps_assignment(frequency_list_T frequency_list, uint8 num_ul_slots,
                                    uint8 num_dl_slots, garb_get_arfcns_t get_arfcns, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_sdcch_assignment

DESCRIPTION   This function notifies the arbitrator of an SDCCH assignment.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_sdcch_assignment(frequency_list_T frequency_list, boolean is_sdcch_8,
                                       uint8 subchannel, garb_get_arfcns_t get_arfcns, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_monscan_ba_list

DESCRIPTION   This function notifies the arbitrator that a monitor scan has
              started on a new BA list.

RETURN VALUE  None

===========================================================================*/
void garb_intf_notify_monscan_ba_list(cell_T** cell_pptr, uint8 num_cells, gas_id_t gas_id );

/*===========================================================================

FUNCTION      garb_intf_is_monitor_coex_protected

DESCRIPTION   This function is called to determine if the specified monitor
              will be protected from COEX interference.

              The determination is made according to a blanking pattern which
              maps TIER_10 and TIER_20 allocation across the 102 (SDCCH) or
              104 (TCH) multiframe.

              The blanking pattern is selected to meet the following
              criterea:

              No more than 3 x TIER_10 monitors per cell per 102(SDCCH)
              or 104(TCH) MF.

              Monitor reduction of 50% i.e. maximum monitor cadence of
              1:2 for TIER_10 vs TIER_20 allocation.

              For BA List Size > 16 :

              It is not possible to meet both the 3 x TIER 10 monitors per
              cell per MF and the 50% criterea here. In these scenarios we
              will be under-monitoring to the 3 monitors per cell per MF
              criterea.

              The blanking mask will give every other monitor position a
              TIER_10 allocation in a 1:2 cadence.

              For BA List Size <=16 :

              For TCH there are 96 monitor positions in the 104MF (IDLE and
              IDLE - 1 frames are excluded). The blanking pattern is defined
              across 32 monitor positions (3 x 32 = 96). The blanking pattern
              allows each ARFCN to have one TIER_10 allocation - which will
              result in 3 x protected meas per ARFCN per 104MF.

RETURN VALUE  TRUE if the monitor is COEX protected, FALSE otherwise.

===========================================================================*/
boolean garb_intf_is_monitor_coex_protected(uint8 FNMod_102_104, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_notify_block_tx

DESCRIPTION   This function notifies the arbitrator of a block Tx.

RETURN VALUE  TRUE if the block Tx can proceed, FALSE otherwise

===========================================================================*/
boolean garb_intf_notify_block_tx(gl1_msg_chan_type channel_type, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_gas_id_xpg_coex_agressor

DESCRIPTION   This function indicates if the GAS is a COEX agressor in XPG
              DSDA mode.

RETURN VALUE  TRUE if the GAS is a COEX agressor

===========================================================================*/
boolean garb_intf_gas_is_xpg_coex_agressor(gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_intf_gas_id_xpg_coex_victim

DESCRIPTION   This function indicates if the GAS is a COEX victim in XPG
              DSDA mode.

RETURN VALUE  TRUE if the GAS is a COEX victim

===========================================================================*/
boolean garb_intf_gas_is_xpg_coex_victim(gas_id_t gas_id);

#ifdef FEATURE_WLAN_COEX_SW_CXM
void garb_intf_coex_update_active_policy( gas_id_t gas_id );
#endif

#endif /*GARB_INTERFACE_H*/
