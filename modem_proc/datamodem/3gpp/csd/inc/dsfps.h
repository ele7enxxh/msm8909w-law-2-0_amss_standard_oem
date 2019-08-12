#ifndef DSFPS_H
#define DSFPS_H

/*=============================================================================
**
**
**
**                        D A T A   S E R V I C E S
**
**                C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                           S E R V I C E S
**
**                E X T E R N A L   H E A D E R   F I L E
**
** DESCRIPTION:
**
**   This file is the header file for the GCSD-FPS Fax Services layer.
**   It contains the definitions of data structures, defined and
**   enumerated constants, and function prototypes required to interface
**   to this protocol layer.
**
**   The interface is used by GCSDPS to invoke the Fax Protocol Stack
**   to perform facsimile data processing during a facsimile data call.
**
**    dsfps_at_cmd_abort()
**      - Invoked by ATCOP when a abort command sequence is detected to
**        notify T31 that it should abort the current AT receive command
**        (i.e. +FRM, +FRH, +FRS).
**
**    dsfps_at_exec_action_cmd()
**      - Invoked by ATCOP to initiate the execution of a T.31 AT action
**        command by T31.
**
**    dsfps_call_connect_init()
**      - Invoked via the mode specific handler by DS3GMGR to initialize 
**        the FPS when a fax data call has reached the 'CALL_CONNECTED'
**        state.
**
**    dsfps_powerup_init()
**      - Invoked by GCSDPS to initialize the T31 services layer at powerup.
**
**    dsfps_process_downlink()
**      - Invoked by GCSDPS to initiate processing of data received via the 
**        FA on the downlink to be transmitted to the local DTE.
**
**    dsfps_process_uplink()
**      - Invoked by GCSDPS to initiate processing of data received from the 
**        local DTE via SIO to be transmitted on the uplink.
**
**    dsfps_process_event()
**      - Invoked by GCSDPS to allow the FPS to perform event specific 
**        processing of events received by GCSDPS during a facsimile call.
**
**    dsfps_process_signals()
**      - Invoked by GCSDPS to allow the FPS to process fax related protocol
**        signals during a facsimile call.
**

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2009 Qualcomm Technologies Incorporated. All Rights Reserved
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsfps.h_v   1.1   24 Jun 2002 10:11:20   rpalumbo  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsfps.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   --------------------------------------------------------------
03/04/09  SA    AU level CMI modifications.
09/01/06  AR    Add support for multiprocessor build.
10/26/05  AR    Add support for multiple call instances.
05/23/03  TMR   Added dsfps_powerup_wm_init() and dsfps_call_init()
04/25/03  TMR   Changes for new UMTS CS data architecture
01/14/03  RLP   Code review changes.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
10/09/02  TMR   Removed GCSD_config_data * parameter from dsfps_powerup_init()
09/11/02  RLP   MMO2 IOT changes.
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

#include "cm.h"
#include "dsm.h"
#include "dsgcsd_task_v.h"

/*=============================================================================
** DEFINES
**===========================================================================*/
typedef struct fps_params
{
  uint16 fdd;
  uint16 fcl;
  uint16 fts;
  uint16 frs;
  uint16 fit[2];
} dsfps_params_type;


/*=============================================================================
** EXPORTED VARIABLES
**===========================================================================*/

/*=============================================================================
** EXPORTED FUNCTIONS
**===========================================================================*/


/*===========================================================================
** FUNCTION: dsfps_at_exec_action_cmd
**
** DESCRIPTION:
**   This function is called by ATCOP to initiate execution of a T.31 action
**   command.  The specified command is queued to GCSDSPS.  When GCSDPS 
**   reads the command off the queue, it will pass it on to T31 for 
**   execution.  The callback function is called by T31 to inform ATCOP when
**   the action command has completed execution and what the final result
**   code was for the command.
**
** PARAMETERS:
**   cmd == T.31 AT action command to process
**   dsat_callback == ATCOP callback function used by T31 to notify ATCOP
**     when the action has completed.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_at_exec_action_cmd
(
  ds_gcsd_cmd_enum_type     cmd,
  dsat_fps_callback_type    dsat_callback
);


/*===========================================================================
** FUNCTION: dsfps_at_cmd_abort
**
** DESCRIPTION:
**   This function is called by ATCOP when it detects that the current AT
**   receive action command should be aborted.  The function invokes the T31
**   layer to handle the abort processing.
**
**   This function only applies to aborting +FRH, +FRM, and +FRS AT commands.
**
** PARAMETERS:
**   None
**
** RETURNS:
**    DSAT_NO_RSP always
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern dsat_result_enum_type dsfps_at_cmd_abort
(
  void
);



/*===========================================================================
** FUNCTION: dsfps_set_stack_params
**
** DESCRIPTION:
**   This function is invoked to set the AT command stack parameters
**   during call.  A seperate interface is provided as the application
**   may change parameters after call initialization.
**
** PARAMETERS:
**   param_ptr - structure of ATCOP parameters
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
void dsfps_set_at_stack_params
(
  const dsfps_params_type  *param_ptr
);


/*===========================================================================
** FUNCTION: dsfps_call_init
**
** DESCRIPTION:
**   This function is invoked to initialize the FPS when a call is first 
**   brought up.
**
** PARAMETERS:
**   call_is_MO            - Whether or not call is Mobile Originated
**   negotiated_capability - The negotiated set of bearer capabilities 
**   data_rate             - The OTA data rate
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
void dsfps_call_init
(
  cm_call_id_type                      call_id,
  boolean                              call_is_MO,
  ds_ucsd_data_rate_T                  data_rate,
  const cm_bearer_capability_s_type   *bearer_cap_ptr
);



/*===========================================================================
** FUNCTION: dsfps_call_connect_init
**
** DESCRIPTION:
**   This function is invoked via the mode specific handler by DS3GMGR 
**   to initialize the FPS when a fax data call has reached the 
**   'CALL_CONNECTED' state.
**
** PARAMETERS:
**   None.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_call_connect_init
(
   void   
);



/*===========================================================================
** FUNCTION: dsfps_wm_init
**
** DESCRIPTION:
**   This function is used to initialize the watermark pointers used by 
**   the FAX layer
**
** PARAMETERS:
**   FAX uplink/downlink SIO watermark pointers, and TPS uplink/downlink
**   watermark pointers
**
** RETURNS:
**   None.
**
** SIDE EFFECTS:
**   The T31 and FA layers each store internally kept pointers to these
**   watermarks for use during FAX processing.
**=========================================================================*/
void dsfps_wm_init
(
  dsm_watermark_type  *tps_dl_wm_ptr,
  dsm_watermark_type  *tps_ul_wm_ptr,
  dsm_watermark_type  *fps_sio_dl_wm_ptr,
  dsm_watermark_type  *fps_sio_ul_wm_ptr
);


/*===========================================================================
** FUNCTION: dsfps_powerup_init
**
** DESCRIPTION:
**   This function is used to initialize the GCSD Fax Services layer of the 
**   FPS.  It is only called once at powerup of the mobile station.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   signal mask used to indicate to GCSDPS which FPS signals to monitor.
**
** SIDE EFFECTS:
**   The T31 and FA layers each store the address of the GCSD_config_data 
**   structure for accessing (read-only) critical data elements required 
**   to perform facsimile data processing.
**=========================================================================*/
extern rex_sigs_type dsfps_powerup_init
(
  void
);


/*===========================================================================
** FUNCTION: dsfps_process_downlink
**
** DESCRIPTION:
**   This function is used to initiate downlink processing within the FPS.
**   It is called by GCSDPS when signaled to perform downlink processing.
**   All downlink data is received via the FA on the downlink watermark.
**
** PARAMETERS:
**   void
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_process_downlink
(
  void
);


/*===========================================================================
** FUNCTION: dst31_process_uplink
**
** DESCRIPTION:
**   This function is used to initiate uplink processing within the FPS.
**   It is called by GCSDPS when signaled to perform uplink processing.
**   All uplink data is received via SIO on the uplink watermark.
**
** PARAMETERS:
**   void
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_process_uplink
(
  void
);


/*===========================================================================
** FUNCTION: dsfps_process_event
**
** DESCRIPTION:
**   This function is used to perform any command specific processing for 
**   those command requests received by GCSDPS.  The function is called by
**   GCDSPS whenever it receives a command event.  This allows the FPS to
**   perform any additional event specific processing required by the 
**   protocol.
**
** PARAMETERS:
**   event_ptr == Pointer to the structure containing the event context.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_process_event
(
  ds_gcsd_cmd_type * event_ptr
);


/*===========================================================================
** FUNCTION: dsfps_process_signals
**
** DESCRIPTION:
**   This function is used to process any FPS related signals.  The signals
**   generated for this implementation are related soley to protocol timeouts.
**
** PARAMETERS:
**   sig_mask == identifies the protocol layer(s) for which a signal has
**     been detected.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dsfps_process_signals
(
	rex_sigs_type sig_mask
);

#endif /* FEATURE_DATA_GCSD_FAX */
#endif /* DSFPS_H */
