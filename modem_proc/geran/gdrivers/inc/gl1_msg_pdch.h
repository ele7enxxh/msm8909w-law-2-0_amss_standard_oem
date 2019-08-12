#ifndef GL1_MSG_PDCH_H
#define GL1_MSG_PDCH_H
/*========================================================================
                   GPRS NPL1 CHANNEL MSG LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces and data structures that are common to
   all GPRS channels. GPRS logical channels are collectively termed as Packet
   Data Channels(PDCH)

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msg_pdch.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/09/14    fm      CR718734 to avoid RX window is stuck in close state, open rx window when GL1 is in L1_TRANSFER_INIT state 
29/04/13    ws      CR481359 enable GTA support
04/03/12    pg      CR458994: fix UPDATE_RX_HDLR_REG macro 
04/10/10    ab      HMSC updates.
06/05/10    dv      CR237642 - Add ASRB funtionality
10-04-01    tjw     Add customer.h and comdef.h to all SU API files
13/12/07    agv     CR 131982 Fix for EGPRS DM causing sleep nacks.
08/05/05    gfr     Added gl1_msg_pdch_get_mac
06/23/05    gfr     Added gl1_msg_pdch_set_camp_tn
06/02/05    gfr     Make handler static
04/21/05    gfr     Move tags into local structure to avoid extra copy
04/20/05    yh      Remove use of RF fifo
04/14/05    gfr     Changed search width to uint16
04/08/05    gfr     Combined GSM and GPRS channel types
07/22/04    gfr     EGPRS support.
11/05/03    gfr     Support for quad-band.
02/24/03    gfr     Lint cleanup.
09/06/02    gr      Added support for GPRS AGC.
08/28/02    gr      Added abort functionality.
06/28/02   thh      Featurize this whole module under FEATURE_GSM.  This is
                    required to allow dualmode build to make single mode
                    (WCDMA) executable.
04/16/02    bk      Added API to set MAC.
                    ...
            bk      Initial Revision
===========================================================================*/


/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */


#include "geran_variation.h"
#ifdef CUST_H
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

 /* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

enum param_set_enum
{
  PARAMS_TO_USE_THIS_BLOCK,
  PARAMS_TO_USE_NEXT_BLOCK,
  NUM_SETS_OF_PARAMS
};


typedef struct
{
   uint8                      num_msgs;
   gl1_hw_rx_signal_type      signal_info[ GL1_DEFS_MAX_DL_MSGS ];
   ARFCN_T                    arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ];
   uint16                     search_width;
   sys_algo_agc_T             *agc_ptr;
#ifdef FEATURE_GSM_RX_DIVERSITY
   sys_algo_agc_T             *agc_ptr_divrx;
#endif
} sm_rx_params_type;


typedef struct
{
   uint8                    tsc;
   gl1_defs_rx_pkt_cb_type  rx_pkt_cb;
   gl1_hw_channel_type      chan_type;
} sm_cfg_params_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* Macros to keep track of handler registration */
#define IS_TX_HDLR_REG(gas_id) reg_tx_hdlr_flag[gas_id]
#define DONT_ALLOW_TX_HDLR_TO_BE_REG(gas_id) UPDATE_TX_HDLR_REG( gas_id,TRUE );
#define ALLOW_TX_HDLR_TO_BE_REG(gas_id) UPDATE_TX_HDLR_REG( gas_id,FALSE );
#define UPDATE_TX_HDLR_REG( gas_id,flag )\
{ \
   reg_tx_hdlr_flag[gas_id] = flag;\
}

/* Macros to handle the window for message submission */
#define IS_TX_WINDOW_OPEN(gas_id) tx_window_open[gas_id]
#define OPEN_TX_WINDOW(gas_id) UPDATE_TX_WINDOW_STATE( gas_id,TRUE )
#define CLOSE_TX_WINDOW(gas_id) UPDATE_TX_WINDOW_STATE( gas_id,FALSE )
#define UPDATE_TX_WINDOW_STATE( gas_id,flag )\
{ \
   tx_window_open[gas_id] = flag;\
}

/* Macros to handle if messages were submitted in Frame 2 and 3 of block*/


#define MSG_SUBMITTED_IN_THIS_BLOCK(gas_id) submitted_tx_msg[gas_id]
#define REG_MSG_WAS_SUBMITTED(gas_id) UPDATE_MSG_SUBMISSION_STATE( gas_id,TRUE )
#define CLEAR_MSGS_SUBMITTED_FLAG(gas_id) UPDATE_MSG_SUBMISSION_STATE( gas_id,FALSE )
#define UPDATE_MSG_SUBMISSION_STATE( gas_id,flag )\
{ \
  submitted_tx_msg[gas_id] = flag;\
}


#define IS_RX_HDLR_REG(gas_id) reg_rx_hdlr_flag[gas_id]
#define DONT_ALLOW_RX_HDLR_TO_BE_REG(gas_id) UPDATE_RX_HDLR_REG( gas_id,TRUE );
#define ALLOW_RX_HDLR_TO_BE_REG(gas_id) UPDATE_RX_HDLR_REG( gas_id,FALSE );
#define UPDATE_RX_HDLR_REG( gas_id,flag )\
{ \
   reg_rx_hdlr_flag[gas_id] = flag;\
}


/* Macros to handle the window for message submission */
#define IS_RX_WINDOW_OPEN(gas_id ) rx_window_open[gas_id]
#define OPEN_RX_WINDOW(gas_id) UPDATE_RX_WINDOW_STATE( gas_id,TRUE )
#define CLOSE_RX_WINDOW(gas_id) UPDATE_RX_WINDOW_STATE( gas_id,FALSE )
#define UPDATE_RX_WINDOW_STATE( gas_id,flag)\
{ \
   rx_window_open[gas_id] = flag;\
}

/* Macros to handle if the rx cmd was issued in the 1st 3 frames of a block*/
#define RX_CMD_ISSUED_IN_THIS_BLOCK(gas_id) issued_rx_cmd[gas_id]
#define REG_RX_CMD_WAS_ISSUED(gas_id) UPDATE_RX_CMD_ISSUED_STATE( gas_id,TRUE )
#define CLEAR_RX_CMD_ISSUED_FLAG(gas_id) UPDATE_RX_CMD_ISSUED_STATE( gas_id,FALSE )
#define UPDATE_RX_CMD_ISSUED_STATE( gas_id,flag )\
{ \
   issued_rx_cmd[gas_id] = flag;\
}

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION gl1_msg_dm_complete

DESCRIPTION
  This function reads the exported value of dm_complete which tells
  the Host that Data Mover (DM) is complete.

DEPENDENCIES
  None

RETURN VALUE
  boolean         DM finished or not.

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_dm_complete (gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_pdch_init

DESCRIPTION
   This function initializes data structures that are used by all PDCH.

DEPENDENCIES
None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msgi_pdch_init( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_pdch_terminate

DESCRIPTION
   Terminate the Uplink and Downlink logical channels. No further channel
   operations or monitoring may take place after this function is called.


DEPENDENCIES
   PDCH must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_pdch_terminate( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_store_mem_info

DESCRIPTION
   Stores information about the data. metrics and log pointers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_pdch_store_sm_info
(
   const gl1_defs_rx_pkt_mem_type *mem_info,
   const sm_rx_params_type        *rx_params,
   const sm_cfg_params_type       *cfg_params,
   gas_id_t                        gas_id
);


/*===========================================================================

FUNCTION gl1_msg_pdch_abort_dl_pbcch_pccch_pacch_pdtch

DESCRIPTION
  This function immediately aborts the specified DL channel.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_pdch_abort_dl_pbcch_pccch_pacch_pdtch
(
   gl1_hw_channel_type   chan_type,
   gas_id_t              gas_id
);


/*===========================================================================

FUNCTION gl1_msg_pdch_store_ta

DESCRIPTION
  This function stores the ta.
  The timing advance changes infrequently about every 104 frames.


DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_pdch_store_ta ( uint8 ta,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_pdch_retrieve_ta

DESCRIPTION
  This function retieves the stored ta.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 gl1_msg_pdch_retrieve_ta ( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_pdch_set_mac

DESCRIPTION
  This function sets the medium access mode to FIXED, DYNAMIC or EXT DYNAMIC.

===========================================================================*/
extern void gl1_msg_pdch_set_mac ( gl1_defs_access_mode_type mode,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_pdch_get_mac

DESCRIPTION
  This function gets the medium access mode.

===========================================================================*/
gl1_defs_access_mode_type gl1_msg_pdch_get_mac (gas_id_t gas_id);


/*===========================================================================

FUNCTION gl1_msg_pdch_set_camp_tn, gl1_msg_pdch_get_camp_tn

DESCRIPTION
  This function sets the timeslot that we are camped on.  This value is
  used to fill in the camp_tn field in the metrics and data callbacks.

===========================================================================*/
void gl1_msg_pdch_set_camp_tn (gl1_defs_tn_type tn,gas_id_t gas_id);
gl1_defs_tn_type gl1_msg_pdch_get_camp_tn (gas_id_t gas_id);

extern void gl1_msgi_pdch_open_rx_win( gas_id_t gas_id );
extern void gl1_msgi_ptcch_open_rx_win( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_pdch_set_loopback

DESCRIPTION
  This function enables or disables SRB loopback.

===========================================================================*/
void gl1_msg_pdch_set_loopback (boolean enable, gas_id_t gas_id);


/*===========================================================================

FUNCTION gl1_msg_pdch_get_loopback_data

DESCRIPTION
  Retrieves the loopback data for the given frame and timeslot.  Returns
  the length of the data, or 0 if not found.

===========================================================================*/
uint8 gl1_msg_pdch_get_loopback_data
(
   uint8 frame_index,
   gl1_defs_tn_type tn,
   uint8 **data,
   gas_id_t gas_id
);

/*===========================================================================

FUNCTION gl1_srb_set_ul_mod_mode

DESCRIPTION
  Sets the mode of the Uplink Modulation for ASRB/SRB loopback. This is
  described in 44014, sec.5.5.4. A value of 0 indicates that the UL
  modulation should be the same as that detected on the DL. A value of
  1 indicates that the UL modulation should be GMSK within the context
  of 8psk on the DL.

===========================================================================*/
void gl1_srb_set_ul_mod_mode(boolean srb_m_bit,gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_srb_get_loopback_state

DESCRIPTION
 Returns state of srb loopback mode. TRUE = enabled, FALSE = disabled
===========================================================================*/
boolean gl1_srb_get_loopback_state(gas_id_t gas_id);



#endif /* GL1_MSG_PDCH_H */



