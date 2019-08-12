#ifndef DSGCSDFA_H
#define DSGCSDFA_H

/*===========================================================================

                Fax Adaptation Subsystem Header File

Description:
  This header file contains the definitions and function prototypes used
  by the GSM Fax Adaptation software layer.  The interface is the same for
  both Quality of Service modes, transparent and non-transparent.
  A QOS-specific handler is invoked wherever mode-specific processing must
  be performed.

  Virtually all processing is performed by mode-specific routines because
  fax in the two modes is handled very differently.  The following
  specifications are implemented:

  ETSI GSM 03.45 Version 7.0.1 Release 1998
  Technical realization of facsimile group 3 transparent

  ETSI GSM 03.46 Version 7.0.0 Release 1998
  Technical realization of facsimile group 3 non-transparent

Externalized Functions:
  Externalized Functions:
  dsgcsdfa_powerup_init()      - Perform one-time initializations.
  dsgcsdfa_call_init()         - Get ready to process a call.
  dsgcsdfa_process_uplink()    - Process uplink fax data.
  dsgcsdfa_process_downlink()  - Process downlink fax data.
  dsgcsdfa_return_to_idle()    - Return Fax Adaptor state to idle.
  dsgcsdfa_process_bcs_frame() - Process fax HDLC BCS frame.
  dsgcsdfa_timer_expired()     - Handle expiration of timed event.
  dsgcsdfa_tx_ready()          - Indicate if FA layer is ready to transmit.
  dsgcsdfa_is_idle()           - Indicate if FA layer is IDLE.

Initialization and Sequencing Requirements:
  dsgcsdfa_powerup_init() must be called once shortly after powering up.
  dsgcsdfa_call_init() must be called at the beginning of every fax call.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2008 Qualcomm Technologies Incorporated. All Rights Reserved
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
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdfa.h#1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------------
03/13/08  SA    Fixing Klocwork Critical issues.
09/01/06  AR    Add support for multiprocessor build.
10/26/05  AR    Add support for multiple call instances.
07/12/05  AR    Add macro DSGCSDFA_TIMER_CLR.  Lint correction.
06/04/03  RLP   Removed sprintf()s from the DEBUG message macros as they were
                causing some performance issues and are not needed.
04/25/03  TMR   Changes for new UMTS CS data architecture
03/19/03  RLP   GCF certification changes to fix MT fax issue.
12/20/02  RLP   Added new interface function dsgcsdfa_is_idle().
                Added 'extern' specifiers to global functions.
12/06/02  RLP   Changed DCS_TO_TRAIN_DELAY back to 75ms
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
10/09/02  TMR   Changed #include from dsgcsd.h to dsgcsdi.h
09/11/02  RLP   MMO2 IOT changes.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.

===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_GCSD_FAX

#include "cm.h"
#include "dsm.h"
#include "dsgcsdi.h"
#include "rex.h"
#include "msg.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* The following set of defines deals with T.30 HDLC BCS frames, which
** look as follows:
**   -----------------------------------------------------------------
**   | Flag | ... | Flag | Address| Control | FCF | FIF | FCS | Flag |
**   -----------------------------------------------------------------
**
** All fields are 1 byte, except FIF, which has arbitrary length.  The FCF
** identifies the frame type, i.e. DIS, DCS, CFR, etc. */

/* Values of HDLC address and control bytes concatenated together. */
#define HDLC_ADDR_AND_CONTROL         0x13ff

/* Final frame bit within the control field. */
#define HDLC_CONTROL_FINAL_FRAME_BIT  0x1000

/* Some HDLC FCF bytes are identified with upper 7 bits. */
#define HDLC_FCF_MASK                 0xfe

/* This is the identifier of the HDLC DCS frame. */
#define HDLC_DCS_FRAME                0x82

/* This is the identifier of the HDLC DCN frame. */
#define HDLC_DCN_FRAME                0xfa

/* The fax rate bits are the 4 middle bits of the 2nd HDLC DCS FIF byte,
** as illustrated in Table 2/T.30. */
#define DIS_DTC_FAX_RATE_BITS         0x3c

/* Possible fax transmission rates, negotiated by the two fax devices,
** as specified in Table 2/T.30. */
#define FA_FAX_RATE_2400_V27ter       0x00
#define FA_FAX_RATE_4800_V27ter       0x08
#define FA_FAX_RATE_9600_V29          0x04
#define FA_FAX_RATE_7200_V29          0x0c
#define FA_FAX_RATE_14400_V17         0x20
#define FA_FAX_RATE_12000_V17         0x28
#define FA_FAX_RATE_9600_V17          0x24
#define FA_FAX_RATE_7200_V17          0x2c
#define FA_FAX_RATE_UNKNOWN           0xff

/* T.30 delay between DCS frame (300 bps Digital Command Signal) and
** TCF (higher speed Training Check Frame) is 75 +/- 20 msec. and
** the 300ms MSG_TRA delay
*/
#define DCS_TO_TRAIN_DELAY            75

/* This is a shortened DCS timer used in cases where we start receiving
** training but aren't ready to process it -- we simply set a short DCS
** timer to maintain state transitioning consistency.
*/
#define DCS_TO_TRAIN_SHORT_DELAY      20

/* T.30 duration of TCF (higher speed Training Check Frame). */
#define TRAINING_DURATION             1500

/* Type of data passed between T31 and Fax Adaptation software layers. */
typedef enum
{
  DS_FA_DATA_NONE = 0,  /* No data available.                           */
  DS_FA_DATA_BCS,       /* Data from T.30's 300 bps signaling phase.    */
  DS_FA_DATA_MSG,       /* Data from T.30's higher speed message phase. */
  DS_FA_DATA_NUM_TYPES
} ds_fa_data_enum_type;

/* Status type returned by Fax Adaptation functions to T31. */
typedef enum
{
  DS_FA_SUCCESS,
  DS_FA_UPLINK_COMPLETE,
  DS_FA_UPLINK_DATA_QUEUED,
  DS_FA_SWITCHING_TO_IDLE,
  DS_FA_INVALID_DATA_TYPE,
  DS_FA_FAILURE
} ds_fa_status_enum_type;

/* Compile-time switch for lower priority Fax Adaptation debug messages. */
#define FAX_ADAPT_DEBUG

#ifdef FAX_ADAPT_DEBUG
  #define FA_MSG_LOW(str, a, b, c)
//  #define FA_MSG_LOW(str, a, b, c)    MSG_LOW(str, a, b, c) 
  #define FA_MSG_MED(str, a, b, c)    MSG_MED(str, a, b, c)
#else
  #define FA_MSG_LOW(str, a, b, c)
  #define FA_MSG_MED(str, a, b, c)
#endif  /* #ifdef FAX_ADAPT_DEBUG */

/* High priority debug messages always turned on. */
#define FA_MSG_HIGH(str, a, b, c)   MSG_HIGH(str, a, b, c)

#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE

#define FA_MSG_STATE(old_state, new_state,in_sync)    \
  FA_MSG_HIGH("State change: S%d -> S%d sync=%d",     \
    old_state,new_state,in_sync);

#else

#define FA_MSG_STATE(old_state, new_state,in_sync)    \
{ static char buf[100];                       \
  (void) snprintf(buf, 100, "State change. %s->%s sync=%d", \
         fa_state_trans_names[(old_state)],   \
         fa_state_trans_names[(new_state)], in_sync); \
  FA_MSG_HIGH(buf, 0,0,0);                    \
}

#endif


/* Macro to reset a FA timer */
#define DSGCSDFA_TIMER_CLR(_timer_ptr, _signal) \
  if (_timer_ptr) \
  { \
    (void) rex_clr_timer(_timer_ptr); \
    (void) rex_clr_sigs(rex_self(), _signal); \
  } \
  else \
  { \
    MSG_ERROR("Attempt to clear NULL FA timer",0,0,0); \
  }


/*===========================================================================
                FUNCTION DECLARATIONS
===========================================================================*/


/*========================================================================
  Function: dsgcsdfa_wm_init

  Description:
    This function is invoked at call connect to initialize internal
    watermark pointers

  Dependencies:
    None

  Parameters:
    tps_dl_wm_ptr - Transport protocol stack downlink watermark pointer
    tps_ul_wm_ptr - Transport protocol stack uplink watermark pointer
    
  Return Value:
    None.

  Side Effects:
    Gets internal watermark pointers initialized
========================================================================*/
void dsgcsdfa_wm_init 
(
  dsm_watermark_type  *tps_dl_wm_ptr,
  dsm_watermark_type  *tps_ul_wm_ptr
);

/*========================================================================
  Function: dsgcsdfa_powerup_init

  Description:
    This function is invoked by T31 one time shortly after powering up.
    It is responsible for doing one-time Fax Adaptation initialization
    for both transparent and non-transparent modes.

  Dependencies:
    None

  Parameters:
    None

  Return Value:
    None.

  Side Effects:
    Gets stuff ready to go.
========================================================================*/
extern rex_sigs_type dsgcsdfa_powerup_init
(
  void
);

/*========================================================================
  Function: dsgcsdfa_call_init

  Description:
    This function is invoked by T31 at the beginning of every fax call.
    It is responsible for doing Fax Adaptation initialization for both
    transparent and non-transparent modes.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    call_is_MO == TRUE if call is MO FALSE if MT

  Return Value:
    None.

  Side Effects:
    Initializes Fax Adaptation state.
========================================================================*/
extern void dsgcsdfa_call_init
(
  cm_call_id_type                     call_id,
  boolean                             call_is_MO,
  const cm_bearer_capability_s_type  *bearer_cap_ptr,
  ds_ucsd_data_rate_T                 data_rate
);

/*========================================================================
  Function: dsgcsdfa_process_uplink

  Description:
    This function performs Fax Adaptation on the uplink.  It is invoked
    periodically by T31 to take data destined for the remote fax, perform
    Fax Adaptation processing on it, and pass it down the protocol stack
    to GCSD Transport Protocol Stack.

    When transmitting BCS data, caller must provide exactly one BCS frame.
    Not a piece of a frame and not multiple frames.

    Unfortunately T31 must check the QOS before invoking this function for
    BCS data transmit.  Transparent mode expects the BCS frame to have
    address, control, FCS, beginning and ending flags (possibly), and bit
    stuffing.  The whole deal.  Non-transparent mode requires that only
    address, control, and the payload (FCF + [FIF]) be present.  No FCS.
    No flags.  And no bit stuffing.
  
  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.
  
    CALLER MUST CHECK QOS FOR BCS DATA AS DESCRIBED ABOVE!!!
  
  Parameters:                         
    fax_uplink_data   - Pointer to DSM item containing fax data destined
                        for the remote fax device, if any data available.
    fax_type_of_data  - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE - No data at this time.
      DS_FA_DATA_BCS  - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG  - MSG (higher speed image) fax data.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS           - Used only in transparent mode.  Call
                                successful, and T31 may call again, if
                                necessary, to fill reverse link with data.
      DS_FA_UPLINK_COMPLETE   - Used only in non-transparent mode.  Call
                                successful, and T31 need not call again
                                to fill reverse link with data.
      DS_FA_INVALID_DATA_TYPE - Data type, BCS or MSG, invalid for current
                                Fax Adaptation state.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Potentially takes uplink data from T31, performs a Fax Adaptation
    massage on it, and sends it to GCSD Transport Protocol Stack.
========================================================================*/
extern ds_fa_status_enum_type dsgcsdfa_process_uplink
(
  dsm_item_type       *fax_uplink_data,
  ds_fa_data_enum_type fax_type_of_data
);

/*========================================================================
  Function: dsgcsdfa_process_downlink

  Description:
    This function performs Fax Adaptation on the downlink.  It is invoked
    periodically by T31 to take data received from the remote fax, perform
    Fax Adaptation processing on it, and pass it up the protocol stack
    to T31.

    When returning BCS data, it returns whatever BCS data is available.
    This is probably not an entire BCS frame, just a piece of one.

    Unfortunately T31 must check the QOS before invoking this function
    for BCS data receive.  Transparent mode returns BCS data with
    address, control, FCS, beginning and ending flags (possibly), and bit
    stuffing.  The whole deal.  Non-transparent mode only returns
    address, control, and the payload (FCF + [FIF]).  No FCS.  No flags.
    And no bit stuffing.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

    CALLER MUST CHECK QOS FOR BCS DATA AS DESCRIBED ABOVE!!!

  Parameters:
    fax_downlink_data   - Pointer to DSM item containing fax data received
                          from the remote fax device, if any data available.
    fax_type_of_data    - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE   - No data at this time.
      DS_FA_DATA_BCS    - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG    - MSG (higher speed image) fax data.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS           - Call successful.
      DS_FA_SWITCHING_TO_IDLE - Call successful, and Fax Adaptor switching
                                its state to idle because of state change
                                in T.30 protocol.  Keep T31 layer informed.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Takes downlink data from GCSD Transport Protocol Statck, if available,
    and returns it up to T31.
========================================================================*/
extern ds_fa_status_enum_type dsgcsdfa_process_downlink
(
  dsm_item_type       **fax_downlink_data,
  ds_fa_data_enum_type *fax_type_of_data
);

/*========================================================================
  Function: dsgcsdfa_return_to_idle

  Description:
    This function is invoked by T31 when it decides that the Fax
    Adaptation layer should return to idle.  This happens normally during
    the T.30 protocol and can also happen for error and timeout handling.
    Always happens on call termination.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    f_call_ended - TRUE means call has terminated.
                   FALSE means call is still up.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SWITCHING_TO_IDLE - Call successful and switching to IDLE.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.
  
  Side Effects:
    Fax Adaptation state returns to idle, if not there already.
========================================================================*/
extern ds_fa_status_enum_type dsgcsdfa_return_to_idle
(
  boolean f_call_ended
);

/*========================================================================
  Function: dsgcsdfa_process_bcs_frame

  Description:
    This function is called by T31 once it packages up an entire BCS frame
    received on the downlink.  T31 also calls this before it calls
    dsgcsdfatrans_process_uplink(), to give us a chance to look at the
    BCS frame before it gets bit-stuffed.  Bit-stuffing only occurs in
    transparent mode, but this function is invoked the same way in both
    modes for consistency.  This function then performs any massaging and
    parsing of the BCS frame it needs to, if any.

    Non-transparent Fax Adaptation identifies BCS frames received on the
    downlink as they come in.  Transparent Fax Adaptation does not.
    However, this function is called by T31 in both cases for consistency.

    This function does absolutely no error checking on the BCS frame.  If
    it's corrupt, we just return, assuming it will be dealt with elsewhere.

    A T.30 HDLC BCS frame looks as follows:
      ------------------------------------------------------------------
      | Flag | ... | Flag | Address | Control | FCF | FIF | FCS | Flag |
      ------------------------------------------------------------------

    All fields are 1 byte, except FIF, which has arbitrary length.  The FCF
    identifies the frame.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    fax_bcs_frame  - Pointer to DSM item containing received BCS
                     downlink frame.
    f_uplink_frame - TRUE = uplink frame, FALSE = downlink frame.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Call successful.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Frame may or may not be inspected and relevant information recorded.
    In addition, frame may or may not be altered according to Fax
    Adaptation specification.
========================================================================*/
extern ds_fa_status_enum_type dsgcsdfa_process_bcs_frame
(
  uint8  *ui8_data_ptr,
  uint16  ui16_data_count,
  boolean f_uplink_frame
);

/*========================================================================
  Function: dsgcsdfa_timer_expired

  Description:
    This function is invoked by the main GCSDPS task loop when it detects
    that the Fax Adaptor timer signal is set.  This allows Fax
    Adaptation software to service the timer in the context of the
    GCSDPS task.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    None.

  Return Value:
    None.

  Side Effects:
    Different stuff, depending upon transparent vs. non-transparent, but
    could result in a Fax Adaptor state change.
========================================================================*/
extern void dsgcsdfa_timer_expired
(
  void
);

/*========================================================================
  Function: dsgcsdfa_tx_ready

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not it is ok to transmit data to the remote FA.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    None.

  Return Value:
    TRUE if it is ok to transmit, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
extern boolean dsgcsdfa_tx_ready
(
  void
);

/*========================================================================
  Function: dsgcsdfa_is_idle

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not the FA layer is currently IDLE.

  Dependencies:
    This function depends on the Bearer Services Quality of Service
    attribute.  It calls different routines for transparent vs.
    non-transparent.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE if the FA is IDLE, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
extern boolean dsgcsdfa_is_idle
(
  void
);

#endif /* FEATURE_DATA_GCSD_FAX */
#endif  /* #ifndef DSGCSDFA_H */
