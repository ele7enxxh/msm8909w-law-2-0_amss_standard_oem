#ifndef DST31_H
#define DST31_H

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
**                E X T E R N A L   H E A D E R   F I L E
**
** DESCRIPTION:
**
**   This file is the header file for the GCSD-FPS T31 Fax Services layer.
**   It contains the definitions of data structures, defined and
**   enumerated constants, and function prototypes required to interface
**   to the T31 layer.
**
**    dst31_at_cmd_abort()
**      - Invoked via the FPS layer by ATCOP when a abort command sequence 
**        is detected.  This allows T31 to abort +FRM, +FRH, and +FRS actions.
**
**    dst31_call_connect_init()
**      - Invoked via the FPS layer to perform final call initialization 
**        when a fax data call has reached the 'CALL_CONNECTED' state.
**
**    dst31_powerup_init()
**      - Invoked via the FPS layer by GCSDPS to initialize the T31 services 
**        layer at powerup.
**
**    dst31_process_downlink()
**      - Invoked via the FPS layer by GCSDPS to process data received OTA from 
**        the TPS on the downlink.  The received data is then transmitted to 
**        the local DTE.
**
**    dst31_process_uplink()
**      - Invoked via the FPS layer by GCSDPS to process data received from the
**        local DTE to be transmitted on the uplink to the remote facsimile 
**        device.
**
**    dst31_process_event()
**      - Invoked via the FPS layer by GCSDPS to perform specific T31 event 
**        processing (e.g. T.31 AT command processing, call event processing).
**
**    dst31_process_signals()
**      - Invoked via the FPS layer by GCSDPS to process T31 signals (e.g.
**        protocol timeouts).
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31.h#1 $

when      who   what, where, why
--------  ---   --------------------------------------------------------------
04/06/06  AR    Replace use of ATCOP globals with control block elements.
03/01/06  AR    Add CM call ID to control block structure.
04/25/03  TMR   Changes for new UMTS CS data architecture
01/14/03  RLP   Code review updates.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
10/09/02  TMR   Updated for new Multi-mode architecture (FEATURE_DATA_MM)
09/11/02  RLP   MMO2 IOT changes.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**============================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX
#include "dsm.h"
#include "dsfps.h"

/*=============================================================================
** DEFINES
**===========================================================================*/

/*=============================================================================
** EXPORTED VARIABLES
**===========================================================================*/

/*=============================================================================
** EXPORTED FUNCTIONS
**===========================================================================*/


/*=============================================================================
** FUNCTION: dst31_at_cmd_abort
**
** DESCRIPTION:
**   This function is called by GCSD Fax Services when it receives 
**   notification from ATCOP that the current T.31 AT action command 
**   should be aborted.  T31 will clear any pending timers and set a flag
**   to indicate to T31 to abort command processing upon the next time the 
**   state processing is entered for the action to be aborted.
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
extern void dst31_at_cmd_abort
(
  void
);


/*=============================================================================
** FUNCTION: dst31_set_at_stack_params
**
** DESCRIPTION:
**   This function is invoked to set the AT command stack parameters
**   during call.  A seperate interface is provided as the application
**   may change parameters after call initialization.
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
extern void dst31_set_at_stack_params
(
  const dsfps_params_type  *param_ptr
);



/*===========================================================================
** FUNCTION: dst31_call_connect_init
**
** DESCRIPTION:
**   This function is used to perform any remaining FPS initialization
**   once the fax data call has reached the CALL_CONNECTED state, meaning
**   the traffic channel is ready for data transfers.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
extern void dst31_call_connect_init 
(
   void
);


/*=============================================================================
** FUNCTION: dst31_call_init
**
** DESCRIPTION:
**   This function is used to initialize the necessary configuration to
**   allow fascimile data call processing to begin for both mobile
**   terminated and originated fascimile data calls.
**
** PARAMETERS:
**   call_is_MO == whether or not call is Mobile Originated
**                
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_call_init
(
  boolean             call_is_MO,
  cm_call_id_type     call_id
);


/*=============================================================================
** FUNCTION: dst31_poweup_init
**
** DESCRIPTION:
**   This function is used to initialize the T31 layer of the FPS. It should
**   be called at the beginning of a mobile originated or mobile terminated 
**   facsimile call prior to any facsimile data processing.
**
** PARAMETERS:
**   None.
**                
** RETURNS:
**   signal mask used to indicate to GCSDPS which T31 signal to monitor for
**   protocol timeouts.
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
extern rex_sigs_type dst31_powerup_init
(
  void
);


/*=============================================================================
** FUNCTION: dst31_wm_init
**
** DESCRIPTION:
**   This function is used to initialize the DSM watermarks used by the FPS 
**   
**
**   Note that GCSDFPS doesn't actually "owns" these watermarks so T31 just sets
**   local pointers to them.  
**
** PARAMETERS:
**  dsm_watermark_type  *tps_dl_wm_ptr     - TPS downlink WM
**  dsm_watermark_type  *tps_ul_wm_ptr     - TPS uplink WM
**  dsm_watermark_type  *fps_sio_dl_wm_ptr - FAX sio downlink WM
**  dsm_watermark_type  *fps_sio_ul_wm_ptr - FAX sio uplink WM
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
void dst31_wm_init
(
  dsm_watermark_type  *tps_dl_wm_ptr,
  dsm_watermark_type  *tps_ul_wm_ptr,
  dsm_watermark_type  *fps_sio_dl_wm_ptr,
  dsm_watermark_type  *fps_sio_ul_wm_ptr
);


/*=============================================================================
** FUNCTION: dst31_process_downlink
**
** DESCRIPTION:
**   This function is used to process facsimile data received on the downlink. 
**   Data is received by T31 from the FA and queued for transmission to the 
**   local DTE using SIO.  T31 is responsbile for filtering and preprocessing
**   the data prior to queuing it to the local DTE.
**
**   T31 will also check the SIO RX watermark during downlink processing.  If
**   any data is received on the watermark it is assumed that the local DTE
**   has requested that the current receive command be aborted.  If so, T31
**   will abort receive processing and will return to the WAIT_FOR_ORDER state.
**
** PARAMETERS:
**   void
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
extern void dst31_process_downlink
(
  void
);


/*=============================================================================
** FUNCTION: dst31_process_uplink
**
** DESCRIPTION:
**   This function is used process facsimile data received from the local DTE
**   for transmission on the uplink.  Data is received by T31 from the DTE via
**   the SIO watermark.  After preprocessing the data, T31 will pass the data
**   on to the FA which queues it to the TPS for OTA transmission.
**
** PARAMETERS:
**   void
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
extern void dst31_process_uplink
(
  void
);


/*=============================================================================
** FUNCTION: dst31_process_event
**
** DESCRIPTION:
**   This function is used to perform event processing for events received
**   via GCSDPS during a facsimile data call.  These include, call processing 
**   T.31 AT action commands, and online-command processing events.
**
** PARAMETERS:
**   event_ptr == Pointer to the structure containing the event context.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
extern void dst31_process_event
(
  ds_gcsd_cmd_type *  event_ptr
);


/*=============================================================================
** FUNCTION: dst31_process_signals
**
** DESCRIPTION:
**   This function is used to process a T31 protocol related signals.
**
** PARAMETERS:
**   sig_mask == identifies the signals which need servicing
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
extern void dst31_process_signals
(
  rex_sigs_type sig_mask
);

#endif /* FEATURE_DATA_GCSD_FAX */
#endif /* DST31_H */
