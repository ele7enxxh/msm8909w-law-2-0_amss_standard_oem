#ifndef L1_SMSCB_H__
#define L1_SMSCB_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1   S M S  CB  H E A D E R   F I L E

DESCRIPTION
   Layer 1 SMS CB support


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_smscb.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
01/05/12   ws       CR308681 - CBCH integration from MDM6610
04/01/12   cs       CR297422 defer the EXPIRY_IND until after last CB block decode
04/08/11   sjw      CR299790 - reset CBCH scheduling after reselection
18/07/11   cs       CR297428 Add in TC0 synchronisation for first CB block in Non-DRX
11/07/11   sjw      DSDS - enable high priority Cell Broadcast support
15/06/11   cs       DSDS Add a utility func to give correct FN
16/05/11   cs       DSDS CR282434 Reenable CBCH support on DSDS
13/07/10   cs       Updates for CB_DRX support
03/11/05   sv       Mainlined FEATURE_GSM_CB
08/09/04   gsc      Added curr_skip_fn_index to support skip in DRX mode.
10/22/03   gsc      Added Quad band support. Return type for the function used
                    to calculate gap is now uint32.
10/22/03   gsc      Added support for skip request in non-drx mode
10/09/03   gsc      First revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include "cbl1if.h"
#include "sys_cnst.h"
#include "sys_type.h"

#include "geran_multi_sim.h"

/* #define GL1_EXTENDED_CBCH_DBG */

/*
 * Maximum number of CBCH blocks possible in a scheduling period.
 */
#define CBCH_LENGTH_OF_EACH_BLOCK         4

#define MAX_CBCH_MESSAGES_IN_SKIP_REQ     48

#define MAX_CBCH_BLOCKS_IN_SKIP_REQ       \
         ( CBCH_LENGTH_OF_EACH_BLOCK * MAX_CBCH_MESSAGES_IN_SKIP_REQ )

/* Needs to be +1 to allow for the next sched period element */
#define MAX_CBCH_MESSAGES_IN_SCHED_PERIOD \
         ( MAX_CBCH_MESSAGES_IN_SKIP_REQ + 1 )

#define MAX_CBCH_BLOCKS_IN_SCHED_PERIOD   \
         ( CBCH_LENGTH_OF_EACH_BLOCK * MAX_CBCH_MESSAGES_IN_SCHED_PERIOD )

#define MAX_CBCH_GAP_PERIOD               0xFFFFFFFF

#define L1_CBCH_HDR                       "L1_CBCH: "

/*---------------------------------------------------------------------------
 * L1 SMSCB Mode Type
 *-------------------------------------------------------------------------*/
typedef enum
{
  L1_SMSCB_DRX_MODE,
  L1_SMSCB_NON_DRX_MODE,
  L1_SMSCB_INIT_MODE
} l1_smscb_mode_type;

/*---------------------------------------------------------------------------
 * L1 SMSCB Service Type
 *-------------------------------------------------------------------------*/
typedef enum
{
  L1_SMSCB_BASIC_CBCH,
  L1_SMSCB_EXTENDED_CBCH
} l1_smscb_service_type;

/*---------------------------------------------------------------------------
 * L1 SMSCB CBCH Information Type
 *-------------------------------------------------------------------------*/
typedef struct
{
  l1_smscb_service_type service;
  boolean               cbch_supported;
  boolean               decoding_cbch;
  uint32                curr_cbch_fn;
  uint32                fn_list[MAX_CBCH_BLOCKS_IN_SCHED_PERIOD];
  uint8                 fn_list_len;
  uint32                skip_fn_list[MAX_CBCH_BLOCKS_IN_SKIP_REQ];
  uint8                 skip_fn_list_len;
  uint8                 curr_fn_index;
  uint8                 curr_skip_fn_index;
  channel_type_T        chan;
  l1_smscb_mode_type    sched_mode;
  ARFCN_T               arfcn;
  uint8                 bsic;
  boolean               high_priority;
  boolean               non_drx_tc0;
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
  boolean               deferred_expiry;
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */
} l1_smscb_cbch_info_type;

/*======================================================================
 *                       GLOBAL FUNCTIONS
 *
 *=====================================================================*/

#if defined( FEATURE_DUAL_SIM )
/*===========================================================================

FUNCTION gl1_ms_switch_l1_cbch_info

DESCRIPTION
         Only used for Dual SIM/Dual data space.  Switches the working
         data pointer to the data space indicated by gas_id.

PARAMS   gas_id - geran access stratum id.

DEPENDENCIES

RETURN VALUE None

===========================================================================*/
extern void gl1_ms_switch_l1_cbch_info( gas_id_t gas_id );
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

/*===========================================================================
FUNCTION: l1_smscb_init

DESCRIPTION:
  This function intializes the L1 for CBCH processing. It puts the SMSCB
  state machine in Init mode.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_init( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_process_scheduling_info

DESCRIPTION:
  This function handles the schedule request received from the CB module.
  The shedule request primitive either puts the CB state machine in NON-DRX
  mode or in DRX mode. In DRX mode this primitive contains a schedule
  that is followed by the L1 to wake up, schedule and decode CBCH.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_process_scheduling_info(
              cb_channel_ind_type      chan_ind,
              cb_scheduling_mode_type  sched_mode,
              uint32                  *fn_list,
              uint8                    fn_list_len,
              gas_id_t                 gas_id );

/*===========================================================================
FUNCTION: l1_smscb_process_skip_info

DESCRIPTION:
  This function handles the skip request received from the CB module. It is
  used by the CB to tell L1 to skip some CBCH decodes from the schedule list
  or eliminate the entire schedule.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_process_skip_info( cb_channel_ind_type  chan_ind,
                                        uint32              *fn_list,
                                        uint8                fn_list_len,
                                        cb_skip_ind_type     skip_ind,
                                        gas_id_t             gas_id );

/*===========================================================================
FUNCTION: l1_smscb_process_deactivate_info

DESCRIPTION:
  This function handles the deactivate request received from the CB module.
  This reset the L1 CB state machine and wipes out any schedule that
  was currently active.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_process_deactivate_info( cb_channel_ind_type chan_ind, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_cb_supported

DESCRIPTION:
  This function stores whether CBCH is supported in the current cell based on
  SI4 received from the network or not.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_cb_supported( boolean val, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_chan_type

DESCRIPTION:
  This function stores the channel type combination (SDCCH_4/SDDCH_8) based
  on SI4 received from the network.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_chan_type( channel_type_T chan, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_get_chan_type

DESCRIPTION:
  This function returns the channel type combination (SDCCH_4/SDDCH_8) used
  in the current cell for CBCH.

DEPENDENCIES:
  None

RETURN VALUE:
  Channel type (SDCCH_4/SDDCH_8)

SIDE EFFECTS:
  None
===========================================================================*/
extern channel_type_T l1_smscb_get_chan_type( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_cbch_expected

DESCRIPTION:
  This function determines based on the frame number passed in whether cbch
  is expected in the next TDMA frame based on the Scheduling mode (DRX/NON-DRX)
  and cbch service type (Basic CBCH/Extended CBCH)

DEPENDENCIES:
  None

RETURN VALUE:
  True, if cbch is expected; otherwise False.

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean l1_smscb_cbch_expected( uint32 fn, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_gap_to_next_activity

DESCRIPTION:
  This function determines the gap in terms of TDMA frame to the next CBCH
  activity so that we can wake up to decode cbch. The gap is computed
  based on the SMSCB mode (Non-DRX/DRX), Channel Combination Type
  (SDCCH_4/SDCCH_8) and the service type (Basic/Extended CBCH).

DEPENDENCIES:
  None

RETURN VALUE:
  The gap to the next cbch block.

SIDE EFFECTS:
  None
===========================================================================*/
extern uint32 l1_smscb_gap_to_next_activity( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_decoding_basic_cbch

DESCRIPTION:
  This function returns True if we are decoding Basic CBCH, otherwise False

DEPENDENCIES:
  None

RETURN VALUE:
  True, if decoding basic CBCH; otherwise False.

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean l1_smscb_decoding_basic_cbch( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_decoding_extended_cbch

DESCRIPTION:
  This function returns True if we are decoding Extended CBCH, otherwise False

DEPENDENCIES:
  None

RETURN VALUE:
  True, if decoding extended CBCH; otherwise False.

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean l1_smscb_decoding_extended_cbch( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_decoding_extended_cbch

DESCRIPTION:
  This function is used to remember that we are decoding Extended CBCH

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_decoding_extended_cbch( boolean val, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_decoding_basic_cbch

DESCRIPTION:
  This function is used to remember that we are decoding Basic CBCH

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_decoding_basic_cbch( boolean val, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_get_cbch_fn

DESCRIPTION:
  This function returns the frame number of the start of the CBCH
  block that was decoded. This frame number is sent to L2 in PH_DATA_IND.

DEPENDENCIES:
  None

RETURN VALUE:
  The starting frame number of the decoded CBCH block.

SIDE EFFECTS:
  None
===========================================================================*/
extern uint32 l1_smscb_get_cbch_fn( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_arfcn

DESCRIPTION:
  This function saves the ARFCN of the serving cell, so that a CBCH schedule
  can be associated with a cell.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_arfcn( ARFCN_T arfcn, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_set_bsic

DESCRIPTION:
  This function saves the BSIC of the serving cell, so that a CBCH schedule
  can be associated with a cell.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_bsic( uint8 bsic, gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_get_arfcn

DESCRIPTION:
  This function returns the ARFCN of the cell in which we received a valid
  schedule.

DEPENDENCIES:
  None

RETURN VALUE:
  The ARFCN of the cell.

SIDE EFFECTS:
  None
===========================================================================*/
extern ARFCN_T l1_smscb_get_arfcn( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_get_bsic

DESCRIPTION:
  This function returns the BSIC of the cell in which we received a valid
  schedule.

DEPENDENCIES:
  None

RETURN VALUE:
  The BSIC of the cell

SIDE EFFECTS:
  None
===========================================================================*/
extern uint8 l1_smscb_get_bsic( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_is_active

DESCRIPTION:
  This function returns True if SMSCB is currently active i.e. either
  we are in NON-DRX mode or DRX mode with a valid schedule. This routine
  is used to determine if the SMSCB state machine needs to be reset, after
  a cell-reselection or handover when we end up in a new cell carrying the
  schedule received in the old cell which is no-longer valid.

DEPENDENCIES:
  None

RETURN VALUE:
  True, if CBCH is active; otherwise False

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean l1_smscb_is_active( gas_id_t gas_id );

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
/*===========================================================================
FUNCTION: l1_smscb_reset_schedule_info

DESCRIPTION:
  This function resets the CB schedule in GL1

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_reset_schedule_info( gas_id_t gas_id );

/*===========================================================================
FUNCTION: l1_smscb_send_deferred_expiry_ind

DESCRIPTION:
  Send the EXPIRY_IND after the last CB block read has been performed.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_send_deferred_expiry_ind( gas_id_t gas_id );
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */

/*===========================================================================
FUNCTION: l1_smscb_set_first_non_drx_block_read

DESCRIPTION:
  This function sets the TC0 align flag to TRUE once we have received the first
  valid CB block on a TC0 after a schedule req is sent in Non-DRX mode.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void l1_smscb_set_first_non_drx_block_read( gas_id_t gas_id );

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
/*===========================================================================
FUNCTION: l1_smscb_use_high_priority

DESCRIPTION:
  This function returns the currently active high_priority flag

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE / FALSE value of high_priority flag

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean l1_smscb_use_high_priority( gas_id_t gas_id );
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#endif /* L1_SMSCB_H__ */



