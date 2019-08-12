/*=============================================================================
**
**
**                        D A T A   S E R V I C E S
**
**           G S M  C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                     T 3 1  F A X  S E R V I C E S
**
**
** DESCRIPTION:
**
**   This file contains source code for the state initialization functions
**   for the GCSD-FPS T31 Fax Services layer.  These functions are exported for 
**   use solely by T31.  Each function performs state start-of-procedure (SOP)
**   initialization for each of the defined state:
**
** EXTERNALIZED FUNCTIONS USED BY T31 ONLY:
**
**   dst31i_sop_idle_state()
**      - performs start-of procedure initialization for the
**          DST31_IDLE_STATE state.
**
**   dst31i_sop_wait_for_order_state()
**      - performs start-of-procedure initialization for the
**          DST31_WAIT_FOR_ORDER_STATE state.
**
**   dst31i_sop_fth_state()
**      - performs start-of-procedure initialization for the
**          DST31_FTH_STATE state.
**
**   dst31i_sop_frh_state()
**      - performs start-of-procedure initialization for the
**          DST31_FRH_STATE state.
**
**   dst31i_sop_ftm_state()
**      - performs start-of-procedure initialization for the
**          DST31_FTM_STATE state.
**
**   dst31i_sop_frm_state()
**      - performs start-of-procedure initialization for the
**          DST31_FRM_STATE state.
**
**   dst31i_sop_fts_state()
**      - performs start-of-procedure initialization for the
**          DST31_FTS_STATE state.
**
**   dst31i_sop_frs_state()
**      - performs start-of-procedure initialization for the
**          DST31_FRS_STATE state.
**
** EXTERNALIZED DATA:
**
** INITIALIZATION AND SEQUENCING REQUIREMENTS:
**
**   dst31i_powerup_init() must be called at powerup.
**
** DOCUMENTATION:
**
**   GSM Circuit-Switched Fax Services, Volumes 1 & 2
**      80-V3512-1, 80-V3512-2
**
**   ITU-T Recommendation T.31: Asynchronous Facsimile DCE Control -
**      Service Class 1.
**

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2006 Qualcomm Technologies Incorporated. All Rights Reserved
                     Qualcomm Proprietary

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31sop.c#1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------
04/01/11  TTV   Support for Q6 free floating.
04/06/06  AR    Replace use of ATCOP globals with control block elements.
06/24/05  AR    Replace REX call with marco to clear t31_ctrl_blk.timer_ptr.
05/30/04  AR    Lint corrections.
01/14/03  RLP   Code review changes.
12/20/02  RLP   Reset frh_wait_for_fa_idle flag in *_sop_wait_for_order_state
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/25/02  RLP   Replaced dsm_pushdown calls with dst31i_dsm_pushdown wrapper
                  function calls.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX
#include "amssassert.h"
#include "data_msg.h"
#include <stringl/stringl.h>
#include "err.h"
#include "dst31i.h"
#include "dst31hdlci.h"

/*=============================================================================
** REGIONAL VARIABLES
**===========================================================================*/


/*=============================================================================
** REGIONAL FUNCTIONS
**===========================================================================*/

/*=============================================================================
** EXPORTED VARIABLES FOR USE SOLELY BY T31
**===========================================================================*/

/*=============================================================================
** EXPORTED FUNCTIONS FOR USE SOLEY BY T31
**===========================================================================*/


/*=============================================================================
** FUNCTION: dst31i_sop_idle_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_IDLE_STATE state.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_idle_state
(
  void
)
{
  /* Everything required for this state is handled in the dst31i_context_reset
  ** routine. No additional processing is required.  This state handler simply
  ** exists for completeness of the API.
  */
  DATA_MSG0_ERROR("dst31i_sop_idle_state() erroneously accessed!");
} /* dst31i_sop_idle_state */


/*=============================================================================
** FUNCTION: dst31i_sop_wait_for_order_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization
**   whenever T31 enters the DST31_WAIT_FOR_ORDER_STATE state.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_wait_for_order_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_LOW("WFO start state initialization");

  /* Reset any context indicators */
  DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

  if (t31_ctrl_blk->flags.at_cmd_abort == TRUE)
  {
    t31_ctrl_blk->flags.at_cmd_abort = FALSE;
  }

  if (t31_ctrl_blk->flags.timeout == TRUE)
  {
    t31_ctrl_blk->flags.timeout = FALSE;
  }

  if (t31_ctrl_blk->flags.cd_timeout == TRUE)
  {
    t31_ctrl_blk->flags.cd_timeout = FALSE;
  }

  if (t31_ctrl_blk->flags.frh_wait_for_fa_idle == TRUE)
  {
    t31_ctrl_blk->flags.frh_wait_for_fa_idle = FALSE;
  }

  if (t31_ctrl_blk->current_ul_data_ptr != NULL)
  {
    dsm_free_packet(&(t31_ctrl_blk->current_ul_data_ptr));
  }

  /* Make sure SIO flow control is not enabled */
  DST31_SIO_FLOWCTL_DEASSERT();

  /* Reset the internal data buffer to clear out any old data */
  DST31_RESET_IO_BUFFER();

  /* Start the WFO timer - limits how long to wait to receive an 
  ** AT command from the local TE
  */
  DST31_TIMER_SET("WFO", t31_ctrl_blk->timer_ptr,
		  DST31_WFO_DEFAULT_TIMEOUT,
		  DST31_WFO_TIMER_TYPE); /*lint !e506 !e774*/

  DATA_MSG0_LOW("WFO state initialized");
} /* dst31i_sop_wait_for_order_state */


/*=============================================================================
** FUNCTION: dst31i_sop_fth_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FTH_STATE state (transmit T.30 HDLC data).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_fth_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();


  DATA_MSG0_LOW("+FTH start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();

  /* Our initial data state is set to locate the <DLE> character
  ** so we can find the end-of-data marker.
  */
  t31_ctrl_blk->io_data.dle_data_state = DST31_FIND_DLE_STATE;

  /* Since we will be entering the state for the first time we 
  ** have to perform some special processing while we wait for the
  ** DTE to start sending the data so we have to tell the state
  ** handler.
  */
  t31_ctrl_blk->flags.wait_for_first_data_byte = TRUE;

  /* We haven't transmitted any HDLC framee bytes yet */
  if (t31_ctrl_blk->flags.fth_frame_complete == TRUE)
  {
    t31_ctrl_blk->flags.fth_frame_complete = FALSE;
  }

  if (t31_ctrl_blk->flags.fth_last_frame == TRUE)
  {
    t31_ctrl_blk->flags.fth_last_frame = FALSE;
  }

  if (t31_ctrl_blk->byte_cnts.fth_byte_cnt > 0)
  {
    t31_ctrl_blk->byte_cnts.fth_byte_cnt = 0;
  }

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed but we don't send it at the start of
  ** the call since the CONNECT will have already been sent in response
  ** to the ATA
  */
  if (t31_ctrl_blk->flags.first_dsat_connect == FALSE)
  {
    t31_ctrl_blk->flags.send_connect = TRUE;
  }
  else
  {
    t31_ctrl_blk->flags.first_dsat_connect = FALSE;
    t31_ctrl_blk->flags.send_connect = FALSE;
  }

#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE
  T31_MSG_HIGH("+FTH: preamble active, size=%d",
    DST31_FTH_MAX_PREAMBLE_SIZE,0,0);

  t31_ctrl_blk->flags.fth_preamble_active = TRUE;
#else
  t31_ctrl_blk->flags.fth_preamble_active = FALSE;
#endif

  t31_ctrl_blk->byte_cnts.fth_byte_cnt = 0;
  t31_ctrl_blk->byte_cnts.fth_preamble_cnt = 0;

  /* Initialize HDLC processing for zero-bit deletion on the frames.
  ** We do the init in this state to guarantee that we are ready when 
  ** the next +FRH command is given.
  */
  dst31hdlci_zbd_init();

  /* Start the +FIT timer - limits how long to wait on the DTE for data */
  DST31_TIMER_SET("+FIT", t31_ctrl_blk->timer_ptr,
    t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);

  DATA_MSG0_LOW("+FTH state initialized");
} /* dst31i_sop_fth_state */


/*=============================================================================
** FUNCTION: dst31i_sop_frh_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FRH_STATE state (receive T.30 HDLC data).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_frh_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* If for some reason the DTE has already aborted this command
  ** we simply skip initialization.  The state handler will take
  ** care of the rest of the abort processing.
  */
  if (t31_ctrl_blk->flags.at_cmd_abort == TRUE)
  {
    return;
  }
  
  DATA_MSG0_LOW("+FRH start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();

  /* Our initial data state is set to locate the <DLE> character
  ** so we can find the end-of-data marker.
  */
  t31_ctrl_blk->io_data.dle_data_state = DST31_FIND_DLE_STATE;

  /* Make sure we dont inadvertantly wait for an FA IDLE at the wrong time */
  t31_ctrl_blk->flags.frh_wait_for_fa_idle = FALSE;

  /* Carrier has not been detected yet */
  t31_ctrl_blk->carrier_detected = FALSE;

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed.
  */
  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed but we don't send it at the start of
  ** the call since the CONNECT will have already been sent in response
  ** to the ATD
  */
  if (t31_ctrl_blk->flags.first_dsat_connect == FALSE)
  {
    t31_ctrl_blk->flags.send_connect = TRUE;
  }
  else
  {
    t31_ctrl_blk->flags.first_dsat_connect = FALSE;
    t31_ctrl_blk->flags.send_connect = FALSE;
  }

  /* Start the CARRIER_DETECT timer - this timer is used to control how long to
  ** wait before going ahead and sending the CONNECT response to the local TE
  ** in the event RX data is not received.  If the timer does expire, the CONNECT
  ** is sent out and the timer is reset to the +FCL value to await the arrival of
  ** the data.  This gives extra processing delay time to the local TE while we
  ** continue to wait for data.
  */
  DATA_MSG1_HIGH( "+FRH::Setting +CD timer=%dms", DST31_CD_TIMEOUT);

  DST31_TIMER_SET("+CD", t31_ctrl_blk->timer_ptr,
    DST31_CD_TIMEOUT, DST31_CD_TIMER_TYPE);  /*lint !e506 !e774*/

  DATA_MSG0_LOW("+FRH state initialized");
} /* dst31i_sop_frh_state */


/*=============================================================================
** FUNCTION: dst31i_sop_ftm_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FTM_STATE state (transmit T.30 message data).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_ftm_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_LOW("+FTM start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();
  
  /* Our initial data state is set to locate the <DLE> character
  ** so we can find the end-of-data marker.
  */
  t31_ctrl_blk->io_data.dle_data_state = DST31_FIND_DLE_STATE;

  /* Since we will be entering the state for the first time we 
  ** have to perform some special processing while we wait for the
  ** DTE to start sending the data so we have to tell the state
  ** handler.
  */
  t31_ctrl_blk->flags.wait_for_first_data_byte = TRUE;

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed.
  */
  t31_ctrl_blk->flags.send_connect = TRUE;

  /* Start the +FIT timer - limits how long to wait on the DTE for data */
  DST31_TIMER_SET("+FIT", t31_ctrl_blk->timer_ptr,
    t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);

  DATA_MSG0_LOW("+FTM state initialized");
} /* dst31i_sop_ftm_state */


/*=============================================================================
** FUNCTION: dst31i_sop_frm_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FRM_STATE state (receive T.30 message data).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_frm_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* If for some reason the DTE has already aborted this command
  ** we simply skip initialization.  The state handler will take
  ** care of the rest of the abort processing.
  */
  if (t31_ctrl_blk->flags.at_cmd_abort == TRUE)
  {
    return;
  }
  
  DATA_MSG0_LOW("+FRM start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();

  /* Our initial data state is set to locate the <DLE> character
  ** so we can find the end-of-data marker.
  */
  t31_ctrl_blk->io_data.dle_data_state = DST31_FIND_DLE_STATE;

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed.
  */
  t31_ctrl_blk->flags.send_connect = TRUE;

  /* Start the CARRIER_DETECT timer - this timer is used to control how long to
  ** wait before going ahead and sending the CONNECT response to the local TE
  ** in the event RX data is not received.  If the timer does expire, the CONNECT
  ** is sent out and the timer is reset to the +FCL value to await the arrival of
  ** the data.  This gives extra processing delay time to the local TE while we
  ** continue to wait for data.
  */
  DATA_MSG1_HIGH("+FRM::Setting +CD timer=%dms", DST31_CD_TIMEOUT);

  DST31_TIMER_SET("+CD", t31_ctrl_blk->timer_ptr,
    DST31_CD_TIMEOUT, DST31_CD_TIMER_TYPE);  /*lint !e506 !e774*/

  DATA_MSG0_LOW("+FRM state initialized");
} /* dst31i_sop_frm_state */


/*=============================================================================
** FUNCTION: dst31i_sop_fts_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FTS_STATE state (transmit silence).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_fts_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_LOW("+FTS start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed.
  */
  t31_ctrl_blk->flags.send_connect = TRUE;
    
  /* Start the +FTS timer */
  DST31_TIMER_SET("+FTS", t31_ctrl_blk->timer_ptr,
    t31_ctrl_blk->at_params.fts, DST31_FTS_TIMER_TYPE);

  DATA_MSG0_HIGH("+FTS state initialized");
} /* dst31i_sop_fts_state */


/*=============================================================================
** FUNCTION: dst31i_sop_frs_state
**
** DESCRIPTION:
**   This function is used to perform start-of-procedure initialization 
**   whenever T31 enters the DST31_FRS_STATE state (receive silence).
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_sop_frs_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* If for some reason the DTE has already aborted this command
  ** we simply skip initialization.  The state handler will take
  ** care of the rest of the abort processing.
  */
  if (t31_ctrl_blk->flags.at_cmd_abort == TRUE)
  {
    return;
  }
  
  DATA_MSG0_LOW("+FRS start state initialization");

  /* Make sure we start with a clean data buffer before we
  ** we start receiving SIO/TPS data and building data frames
  */
  DST31_RESET_IO_BUFFER();

  /* Make sure to indicate the CONNECT response should be sent once the
  ** switch to OLDM is completed.
  */
  t31_ctrl_blk->flags.send_connect = TRUE;
    
  /* Start the +FRS timer */
  DST31_TIMER_SET("+FRS", t31_ctrl_blk->timer_ptr,
    t31_ctrl_blk->at_params.frs, DST31_FRS_TIMER_TYPE);

  DATA_MSG0_LOW("+FRS state initialized");
} /* dst31i_sop_frs_state */

#endif /* FEATURE_DATA_GCSD_FAX */
