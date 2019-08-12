#ifndef SRCH_MDSP_I_H
#define SRCH_MDSP_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S E A R C H   T A S K  --  M D S P   S U B S Y S T E M

GENERAL DESCRIPTION
  This module contains functions pertaining to the MDSP used by the
  demodulator and searcher.

EXTERNALIZED FUNCTIONS/MACROS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_mdsp_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   pk      1x2GTA Feature changes
09/10/13   vks     Export api to return lc state, rtc at last sync80 boundary
                   read from firmware
08/19/13   cjb     Mainline FEATURE_MODEM_1X_QOF_PILOT_NOISE_EST
05/10/13   cjb     Fixed mempool capture functionality.
04/19/13   pap     Adding mempool capture capability for diag and fade timers.
02/25/13   trc     Disable QLIC QOF NE if FEATURE_MODEM_1X_QOF_PILOT_NOISE_EST
02/08/13   vks     Shutdown FW when exiting 1x service
01/25/13   vks     Cleanup srch mdsp module
12/11/12   as      Added srch_mdsp_update_fw_pkt_logging to update the firmware
                   packet logging.
04/16/12   jtm     Moved QOF FCH Noise Estimation to SRCH.
08/18/11   vks     Clean up references to sample server which does not exist
                   anymore (since genesis).
08/18/11   adw     Mainline 1x message router support.
07/07/11   vks     Export an api to read the stmrVsrcDiff from firmware.
04/25/11   jtm     CMI Phase 4 SU API changes.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/14/11   vks     Fix compiler issues - export srch_mdsp_state_cfg_msg_handler
02/11/11   sst     Add new func for mdsp enable with state specification
01/05/11   cjb     Compiler warning fixes.
12/11/10   vks     Add support to do mdsp disable from suspend state.
11/30/10   cjb     Compiler warning fixes.
11/05/10   vks     Add support for FTM mdsp enable/disable (decouple from
                   srch_mdsp_enable/disable that FTM is using till now).
11/01/10   jtm     Modem 1x SU API changes.
08/09/10   cjb     Added MDSP API to printout PC in case of a timeout.
07/14/10   sst     Remove srch_mdsp_[sleep|wakeup] API's
04/19/10   adw     Added interface to enable / disable mDSP control of the MND.
03/24/10   vks     Add api to export mdsp shared memory address.
03/05/10   vks     Remove the support for CDMA App config message handler (we
                   are using CDMA state config message now).
01/09/10   vks     Add support for 1X Demod State config message.
12/22/09   mca     Added modem_1x_defs.h
12/14/09   vks     Add support for FWS app config message.
12/08/09   vks     Export srch_mdsp_download for Q6/MSGR interface (for FTM).
11/11/09   vks     Clean up on the previous changes
11/09/09   vks     Added Message Router and Q6 FW support under feature
                   FEATURE_1X_SUPPORTS_MSGR.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Eliminated implicit includes.
03/26/09   adw     Remove modem_1x_defs.h include.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
03/16/09   jtm     Lint fixes.
03/05/07   aps     Added srch_mdsp_add_isr_cb().
03/05/07   nnv     Added function srch_mdsp_mnd_ctrl
01/23/07   rkc     Added srch_mdsp_enable().
10/31/06   tjc     Changed srch_mdsp_sleep/wakeup macros to functions and
                   added srch_mdsp_awake() check function.
07/13/05   grl     Added flushing the sample ram of the requested chain.
06/15/05   bt      Removed MDSP_CMD_FLUSH_SR.
04/20/05   sst     Removed srch_mdsp_isr
03/10/05   ddh     Added Featurized support for executing on 7500 RUMI station
02/07/05   sst     Created srch_mdsp_disable() to disable app and update vars
                   Extern'ed srch_mdsp_pre1x()
02/04/05   bt      Add scheduled sample ram flush.
08/09/04   sst     Extern'ed new generic mdsp isr func srch_mdsp_isr()
06/21/04   sst     Corrected naming of srch_mdsp_check_cmd_ functions
                   Updated srch_mdsp_check_cmd_ functions and their macros
05/04/04   sst     Created functions out of MDSP_CHECK_CMD_E/NE
02/19/04   sst     Lint corrections in MDSP_CHECK_CMD_E/NE
01/28/04   ejv     Mainline T_MSM6500.
12/15/03   sst     Added 2 macro's MDSP_CHECK_CMD_*() to check the MDSP cmd
                   semaphore, in which a timeout and checking of MDSP_dspHalt
                   reg is used to to report MDSP (fatal) errors
09/18/03   bt      Wait for the command semaphore after waking up the dsp srch
                   part. Also delete unused macros.
06/25/03   ejv     Update srch_mdsp_sleep() and srch_mdsp_wakeup().
06/05/03   ejv     Replace old function srch_mdsp_current_image() with
                   new function srch_mdsp_demod_running().
01/23/03   bt      Added srch_mdsp_sleep/wakeup macros.Cleaned up func names.
10/29/02   bt      add srch_mdsp_flush_samp_ram command
09/11/02   va      Added function to get the current mdsp image.
07/23/02   bt      implementation , first cut
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Other */
#include "cdmafw_msg.h"
#include "vstmr_1x.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants & Macros
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* MDSP_MOD_READY callback function type */
typedef void (*srch_mdsp_mod_ready_cb_t) (void);

typedef struct
{
  vstmr_1xscomb_t scombTime;
  uint32          rtc;
  uint32          xo;
  uint64          lcstate;
} rtc_lcstate_type;


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_MDSP_AWAKE

DESCRIPTION    Checks if demod is running on the MDSP and MDSP is awake.

DEPENDENCIES   None.

RETURN VALUE   TRUE if demod is running and MDSP is awake; FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_mdsp_awake(void);

/*===========================================================================


FUNCTION       SRCH_MDSP_ENABLE[_STATE]

DESCRIPTION    This function enables the CDMA 1X DEMOD MDSP App and registers
               a callback function to be called when app_cfg message is
               received. The _state API passes in the requested mDSP state.

DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_mdsp_enable
(
  srch_mdsp_mod_ready_cb_t mod_ready_cb
);
extern void srch_mdsp_enable_state
(
  srch_mdsp_mod_ready_cb_t mod_ready_cb,         /* ready callback */
  cdmafw_state_t           state                 /* fw enabled state */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_APP_STATE_CFG_MSG_HANDLER

DESCRIPTION    This function handles the state config message from the FW.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   1X Demod FW state change.

===========================================================================*/
extern void srch_mdsp_state_cfg_msg_handler
(
  cdmafw_state_cfg_rsp_msg_t *config_rsp_msg_ptr
);

/*===========================================================================

FUNCTION       SRCH_MDSP_GET_SMEM_ADDR

DESCRIPTION    This function returns the FW shared memory address.

DEPENDENCIES   FW should have been initialized (in ACTIVE state).

RETURN VALUE   Address to the FW shared memory shared memory.

SIDE EFFECTS   None.

===========================================================================*/
extern cdmafw_shared_memory_interface_t* srch_mdsp_get_smem_addr( void );


/*===========================================================================

FUNCTION       SRCH_MDSP_DISABLE

DESCRIPTION    This function disables the searcher MDSP App.

DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_mdsp_disable( void );

/*===========================================================================

FUNCTION       SRCH_MDSP_SHUTDOWN

DESCRIPTION    This function disables the searcher MDSP App.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_mdsp_shutdown( void );

/*===========================================================================

FUNCTION       SRCH_MDSP_GET_STMRVSRCDIFF

DESCRIPTION    This function is an API to return the stmrVsrcDiff value from
               the firmware shared memory. It is used in GPS time transfer.

DEPENDENCIES   None.

RETURN VALUE   stmrVsrcDiff if FW is up.
               0 otherwise.
               Please note that 0 would be returned for non-msgr interface
               as well.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_mdsp_get_stmrVsrcDiff( void );

/*===========================================================================

FUNCTION srch_mdsp_update_fw_pkt_logging

DESCRIPTION
  This function updates the firmware packet logging.  It checks the
  log status from CDMAFW_LOG_DIAG_PKT_START_ID to CDMAFW_LOG_DIAG_PKT_END_ID
  and if its status has changed from the last sent value, this function will
  send the new value to the FW.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void srch_mdsp_update_fw_pkt_logging( void );

/*===========================================================================

FUNCTION srch_mdsp_mempool_capture

DESCRIPTION
  This function issues a mempool capture command to the firmware based on
  RF chain specified by the parameter antenna. 0 Primary chain, 1 secondary

DEPENDENCIES
  None

RETURN VALUE
  TRUE in case of success sending mempool capture message to FW.
  FALSE in case of failure due to not owning specified RF chain.
SIDE EFFECTS
  None

===========================================================================*/
extern boolean srch_mdsp_mempool_capture( uint8 chain );

/*===========================================================================

FUNCTION      SRCH_UPDATE_MEMPOOL_CAPTURE

DESCRIPTION   This function updates the control flag for FW mempool capture.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Used to enable/disable the FW mempool capture at key failure
              scenarios like reacq failure,...

===========================================================================*/
extern void srch_update_mempool_capture
(
  boolean enable /* TRUE => Allow mempool capture; FALSE => Disable */
);

/*===========================================================================

FUNCTION      SRCH_MDSP_GET_RTC_LCSTATE_AT_SYNC80

DESCRIPTION   This function returns the rtc and long code state at the last 
              sync80 boundary. 

DEPENDENCIES  None

RETURN VALUE  Implicitly through the passed in pointer

SIDE EFFECTS  None

===========================================================================*/
extern void srch_mdsp_get_rtc_lcstate_at_sync80
(
  rtc_lcstate_type *rtc_lcstate /* RTC/Lc_State at last 80ms boundary */
);


/*===========================================================================
FUNCTION      SRCH_MDSP_START_1X2X_QTA

DESCRIPTION   This function sends 1X2X QTA START message to FW.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void srch_mdsp_start_1x2x_qta( void );

/*===========================================================================
FUNCTION       SRCH_MDSP_RXAGC_ACQ_DONE_IND_HANDLER

DESCRIPTION    This function dergister QTA_RXAGC_ACQ_DONE INDICATION msg with
               message router and infomrs Traffic SM for further processing of
               the message.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
extern void srch_mdsp_rxagc_acq_done_ind_handler( void );

/*===========================================================================
FUNCTION      SRCH_MDSP_STOP_1X2X_QTA

DESCRIPTION   This function sends 1X2X QTA STOP message to FW.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void srch_mdsp_stop_1x2x_qta( void );

/*===========================================================================
FUNCTION      SRCH_MDSP_QUERY_EMPTY_QTA_GAP

DESCRIPTION   This function sends 1X2X QTA STOP message to FW.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern boolean srch_mdsp_query_empty_qta_gap( void );

/*===========================================================================
FUNCTION      SRCH_MDSP_QUERY_EMPTY_QTA_GAP

DESCRIPTION   This function sends 1X2X QTA STOP message to FW.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern boolean srch_mdsp_query_empty_qta_gap( void );

/*===========================================================================
FUNCTION       SRCH_MDSP_QTA_STATUS_QUERY_RSP_HANDLER

DESCRIPTION    This function processes QTA STATUS QUERY RESPONSE msg from FW
               and updates onex2x_qta_status field of srch_mdsp struct.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS  srch_mdsp gets modified.
===========================================================================*/
extern void srch_mdsp_qta_status_query_rsp_handler
( 
  cdmafw_1x2x_qta_status_rsp_t qta_status 
);

/*===========================================================================
FUNCTION      SRCH_MDSP_IS_EMPTY_QTA_GAP

DESCRIPTION   This function returns the current state of QTA Gap

DEPENDENCIES  None

RETURN VALUE  TRUE: QTA is ON
              FALSE QTA is OFF

SIDE EFFECTS  None
===========================================================================*/
extern boolean srch_mdsp_is_empty_qta_gap( void );
#endif /* SRCH_MDSP_I_H */
