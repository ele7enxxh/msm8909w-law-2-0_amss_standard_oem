/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
          A T   C O M M A N D   R E S P O N S E  G E N E R A T O R

GENERAL DESCRIPTION
  This module provides the functionality to build, format, and send
  AT command responses to the appropriate destination; TE or PS
  protocol stack.

EXTERNALIZED FUNCTIONS
  dsat_reg_rsp_router
    Registers a function that provides AT response routing based on
    operating service mode and AT command state.  Responses are routed
    to TE or PS protocol stack.

  dsat_send_intermediate_result_code
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0. Also, modifies ATCOP internal state variables.  This function
    should be used when the result code to be sent is an intermediate code.
    That is, one that occurs before the final AT response is sent when 
    the call terminates such as with a T.30 FAX call which sends multiple
    AT result codes throughout the duration of the call.

  dsat_send_result_code
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0. Also, modifies ATCOP internal state variables.

  dsat_report_rate_on_connect
    Writes the value to the current rate variable dsat_report_rate_val. 
    If dsat_x_val, set by the ATX command, is greater than 0.this rate will 
    be reported with the next CONNECT result. The dsat_report_rate_val 
    variable will be reset to 0 on the next call to 
    dsat_sio_is_free().

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatrsp_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#include <stdio.h>
#include <string.h>

#include "target.h"
#include "err.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsatcmif.h"
#include "dsatdl.h"
#include "dsm.h"
#include "sio.h"
#include "ds3gsiolib.h"
#include "amssassert.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_MDM_FUSION
#include "ds707_rmsm_proxy.h"
#endif /* FEATURE_DSAT_MDM_FUSION */


/*===========================================================================

                         EXTERNAL DECLARATIONS

===========================================================================*/


/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/* Tracks call mode for special response handling */
dsat_call_mode_e_type dsat_rsp_call_mode;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Pointer to function defining how responses should be routed. */
dsat_routing_func_type route_at_cmd_rsp_func_ptr = NULL;

/* Pointer to function defining how responses should be formated.          */
dsat_fmting_func_type fmt_at_cmd_rsp_func_ptr = NULL;

/* Flag to indicate if tx (dsat_to_sio) watermark hit high.
   If this flag is high, ATCOP should halt current commands that
   requesting data from the subsystem
*/
boolean dsati_tx_wm_high = FALSE;

/* Function to reenter current command processing which has halted
   when tx (dsat_to_sio) watermark hit high.
*/
dsati_async_event_handler_type dsati_to_sio_low_handler = NULL;

/* Timer to delay when listing large quantities of PB entries or SMSes
   and DSM large items are close to being exhausted: this needed as USB 
   dequeues from DS3GSIOLIB WM as quickly as queued so normal flow control
   doesn't kick in, USB can acquire chain of all available DSM large items
   and never have a chance to free them as its task doesn't get a chance 
   to run while DS and WMS or PBM are busy with this. */
rex_timer_type dsati_free_item_timer = {0};

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
    Prototypes for local functions:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION DSAT_REG_RSP_ROUTER

DESCRIPTION
  This function registers a function that provides AT response routing
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.  Registered function routes AT
  command response to either SIO or PS protocol stack.  

  If service type is IS-707 related and AT operating mode is online data
  mode, responses are routed to PS protocol stack, otherwise they are
  routed to SIO.  This provides support for IS-707 command reflection in
  future releases and makes it independent of ATCOP operation.  If no
  routing function is registered, response is sent directly to SIO.

  The response routing function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_reg_rsp_router
(
  dsat_routing_func_type func_ptr       /* Function providing mode specific
                                           AT command response routing or
                                           NULL to deregister routing
                                           function. */
)
{
  /* Set the response routing function pointer to the new response
     routing function. A value of NULL deregisters the routing. */
  route_at_cmd_rsp_func_ptr = func_ptr;

} /* dsat_reg_rsp_router( ) */


/*===========================================================================

FUNCTION DSAT_REG_RSP_FMTER

DESCRIPTION
  This function registers a function that provides AT response formating
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.

  If service type is IS-707 related and AT operating mode is online data
  mode, responses to reflected AT commands have to be formated with
  EIA-617 codes. So the default fmt_response function cannot be used and
  the 707 mode-specific handler should use this function to register a
  function that does appropriate formatting.

  The response formating function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_rsp_fmter
(
  dsat_fmting_func_type func_ptr    /* Function providing mode specific
                                       AT command response formating or
                                       NULL to deregister routing function */
)
{
  /*-------------------------------------------------------------------------
    Set the response formating function pointer to the new response
    routing function. A value of NULL deregisters the routing. 
  -------------------------------------------------------------------------*/
  fmt_at_cmd_rsp_func_ptr = func_ptr;

} /* dsat_reg_rsp_fmter() */

/*===========================================================================
FUNCTION  DSAT_SEND_INTERMEDIATE_RESULT_CODE
DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.  This function
  should be used when the result code to be sent is an intermediate code.
  That is, one that occurs before the final AT response is sent when 
  the call terminates such as with a T.30 FAX call which sends multiple
  AT result codes throughout the duration of the call.
  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.
  Extended cellular result codes are returned if AT+CRC is 1.
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsat_send_intermediate_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_send_intermediate_result_code_fp);
  dsatdl_vtable.dsatrsp_send_intermediate_result_code_fp(result_code);

  return;
} /* dsat_send_intermediate_result_code( )  */


/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsat_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_send_result_code_fp);
  dsatdl_vtable.dsatrsp_send_result_code_fp(result_code);

  return;
} /* dsat_send_result_code( )  */

/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsat_send_result_code_ex
(
  dsat_result_enum_type result_code,         /* Command result code */
  ds3g_siolib_port_e_type port /*Port Id*/
  
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_send_result_code_ex_fp);
  dsatdl_vtable.dsatrsp_send_result_code_ex_fp(result_code, port);

  return;
} /* dsat_send_result_code_ex( )  */

/*===========================================================================

FUNCTION DSAT_SEND_UNSOLICITED_RESULT_CODE

DESCRIPTION
  Send unsolicited result code to TE.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsat_send_unsolicited_result_code
(
  dsat_result_enum_type result_code
)
{
  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_send_urc_fp))
  {
    dsatdl_vtable.dsatrsp_send_urc_fp(result_code);
  }

  return;
} /* dsat_send_unsolicited_result_code */

#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) )

/*===========================================================================

FUNCTION  DSAT_SEND_DR_RESULT_EX

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_send_dr_result_ex
(
  dsat_dr_mode_e_type compression,          /*  Compression mode            */
  ds3g_siolib_port_e_type port /*Port Id*/
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_send_dr_result_ex_fp);
  dsatdl_vtable.dsatrsp_send_dr_result_ex_fp(compression, port);

  return;
} /* dsat_send_dr_result_ex( ) */

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */


/*===========================================================================

FUNCTION DSAT_REPORT_RATE_ON_CONNECT

DESCRIPTION
  This function is called by other modules to write the value to the current 
  rate variable dsat_report_rate_val. If dsat_x_val, set by the ATX command,
  is greater than 0 this rate will be reported with the next CONNECT result
  and during the call with any CONNECT result due to re-entry of online data
  mode. The dsat_report_rate_val variable will be reset to 0
  on the next call to dsat_sio_is_free(). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the value of the dsat_report_rate_val.
===========================================================================*/
void dsat_report_rate_on_connect
(
    dsat_connect_rate_s_type connect_rate   /* Rate to report with next CONNECT */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_report_rate_on_connect_fp);
  dsatdl_vtable.dsatrsp_report_rate_on_connect_fp(connect_rate);

  return;
}/* dsat_report_rate_on_connect */


/*===========================================================================

FUNCTION  DSAT_TO_SIO_LOWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits low.

  It sets dsati_tx_wm_high flag to be FALSE and inform ds task with
  DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
void dsat_to_sio_lowater_cb (   
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  ds_cmd_type * cmd_ptr;

  dsati_tx_wm_high = FALSE;
  /* Send a DS_AT_TO_SIO_LOW_CMD cmd to DS task */
  cmd_ptr = dsat_get_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO, FALSE);
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_TO_SIO_LOW_CMD;

  ds_put_cmd(cmd_ptr);
}


/*===========================================================================

FUNCTION  DSAT_TO_SIO_HIWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits high.

  It sets dsati_tx_wm_high flag to be TRUE.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
void dsat_to_sio_hiwater_cb (
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  dsati_tx_wm_high = TRUE;
}


/*===========================================================================

FUNCTION  DSATI_TO_SIO_LOW_CMD_HANDLER

DESCRIPTION
  This is the handler function for DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: if the dsat_to_sio low handler succeeds.
  DSAT_ERROR    : if the dsat_to_sio low handler fails.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsati_to_sio_low_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  if(dsati_to_sio_low_handler != NULL)
  {
    result = dsati_to_sio_low_handler(cmd_ptr);
  }
  return result;
}

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DSAT_PROCESS_PROXY_RESPONSE

DESCRIPTION
  This functions receives the fully formatted response from Proxy interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void dsat_process_proxy_response
(
  dsm_watermark_type *wm_item_ptr       /* Response Watermark pointer */
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_process_proxy_response_fp))
  {
    dsatdl_vtable.dsatrsp_process_proxy_response_fp(wm_item_ptr);
  }
  else
  {
    while ( NULL != (dsm_item_ptr = dsm_dequeue(wm_item_ptr)) )
    {
      dsm_free_packet(&dsm_item_ptr);
    }
  }

  return;
}/* dsat_process_proxy_response */

/*===========================================================================

FUNCTION DSAT_PROCESS_PROXY_URC

DESCRIPTION
  This function process the data received from the other Processor and packs 
  the unsolicited response and sends it out. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None  

===========================================================================*/
extern void dsat_process_proxy_urc
(
  dsm_watermark_type *wm_item_ptr       /* Response Watermark pointer */
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatrsp_process_proxy_urc_fp))
  {
    dsatdl_vtable.dsatrsp_process_proxy_urc_fp(wm_item_ptr);
  }
  else
  {
    while ( NULL != (dsm_item_ptr = dsm_dequeue(wm_item_ptr)) )
    {
      dsm_free_packet(&dsm_item_ptr);
    }
  }

  return;
}/* dsat_process_proxy_urc */

#endif /* FEATURE_DSAT_MDM_FUSION */

