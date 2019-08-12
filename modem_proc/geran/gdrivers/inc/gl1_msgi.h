#ifndef GL1_MSGI_H
#define GL1_MSGI_H
/*========================================================================
                    INTERNAL GSM GPRS NPL1 MSG LAYER HEADER FILE
DESCRIPTION
   This file contains all definitions that are required internally by
   the NPL1 message layer.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msgi.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
19/10/15   br       CR926522 GL1 to Pass the frequency offset to rotator to correct in GBTA.
17/10/14   nm       CR729413 increment suspension timer for W2G BPLMN power scan if meas in progress true before terminating
12/09/14   nm       CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
05/09/13   nm       CR538507 Remove Null of handlers when calling gl1_msg_reset 
                             and resetting of num_ft_handlers  again as this is causing more crashes
29/07/10   ws       Added Missing PACKED_POST for TX schedule log packet
07/29/05   gfr      DTM support
06/21/05   gfr      Added gl1_msg_tt_do_pending_adjust
06/02/05   gfr      Add time_stamp to interference measure handler
06/02/05   gfr      Make handler static
04/21/05   yh       Support for panic reset
04/20/05   yh       Remove use of RF fifo
04/12/05   gfr      Removed obselete structures
04/08/05   gfr      Combined GSM and GPRS channel types
07/22/04   gfr      EGPRS support.
06/15/04   gfr      Added async CCCH functions, and moved local AFC/TT types
                    to gl1_msg_algs.c
06/12/04   gfr      Added init functions for GSM channels.
05/20/04   gfr      Mainlined FEATURE_COMMON_PWR_HANDLER
03/05/04   gfr      Lint cleanup.
02/17/04   gfr      AFC cleanup.
01/27/04   gfr      Support for logging tx scheduled results.
11/05/03   gfr      Support for quad-band.
10/21/03   gsc      Added SMS CB support.
08/29/03   jc       Removed ypf functions no longer needed by PLT.
05/13/03   gfr      Common power handler support.
05/12/03   gfr      Remove burst_schedule code.
04/16/03   gfr      Validate the function pointer when removing the ncell
                    handler to avoid removing the wrong one.
04/04/03   gfr      Update the GPRS burst schedule structure.
02/24/03   gfr      Lint cleanup.
01/31/03   gfr      Move rxlev handler prototype here from gl1_msg.h.
01/15/03   gfr      Replaced old GPRS burst indication function to take
                    the timeslot of the burst.
11/07/02   gfr      Renamed some internal GPRS functions for consistency.
10/16/02   gr       Moved rxlev add/remove handler functions to this file.
08/29/02   JC       Added prototype for gl1_msgi_map_txlev_to_pwr_idx() and
                    removed prototype for gl1_msg_power_to_dBm().
08/13/02   dp       Added sch_attempts to acq struct.  Interface changes for
                    parallel decodes
06/28/02   thh      Featurize this whole module under FEATURE_GSM.  This is
                    required to allow dualmode build to make single mode
                    (WCDMA) executable.
06/04/02   bbd      Changed FEATURE_GPLT to FEATURE_GSM_PLT
                    Changed FEATURE_GPRS to FEATURE_GSM_GPRS
03/20/02   dp       Changed tx signal strength to power index
03/15/02   mk       Added support for direct frequency correction.
03/08/02   sg       Added a call for GPRS dynamic allocation
02/14/02   gw       Added support for AGC in ncell functions.
02/04/02   JC       Modified ACQ interfaces to accomodate new AGC design.
02/01/02   mk       Updated to support the latest afc/tt algorithms.
01/16/02   mk       Added support for afc/tt.
12/18/01   dp       Renamed gl1_msg_add_ncell_hdlr as gl1_msg_add_acq_ncell_hdlr
12/13/01   gw       Changes to support message/frame layer acquisition.
12/12/01   gw       Added support for returning the YPF word returned by the
                    mDSP with decoded data.  To use, FEATURE_GPLT must be
                    defined.
11/9/01    bk       Added GPRS support
10/31/01   dp       Removed gl1_hw_convert_rx_hdr prototypes since the fix
                    for eliminating multiple copies is now in place
10/16/01   gw       Added prototype for linear power to dBm conversion
                    function.
10/10/01   gw       Added support for Ncell SCH and BCCH reception.
09/27/01   gw       Added support for passing the channel type into
                    gl1_msg_rx_ccch and returning it in the callback.
09/25/01   mk       Added support for CCCH/BCCH.
08/31/01   jc       Initial version.
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

#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"
#include "gl1_sys_algo.h"
#include "log.h"
#include "log_codes_gsm_v.h"

#ifndef L1_STATE_CHANGE
#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #define L1_STATE_CHANGE(action, debugText)\
  action;
#endif
#endif

/* -----------------------------------------------------------------------
** GSM/GPRS Type Declarations
** ----------------------------------------------------------------------- */

/* Frame tick handler type - note the order cannot be changed without also
   changing the order of execution in gl1_msg_frame_tick_notification */
typedef enum
{
   GL1_MSG_FT_HDLR_NULL,
   GL1_MSG_FT_HDLR_DL,               /* DL TCH, SDCCH, SACCH/51 */
   GL1_MSG_FT_HDLR_DL_PDCH,          /* DL PDTCH, PBCCH, PCCCH */
   GL1_MSG_FT_HDLR_DL_PTCCH_SACCH,   /* UL PTCCH, SACCH/26 */
   GL1_MSG_FT_HDLR_UL,               /* UL TCH, RACH, PRACH, SDCCH, SACCH/51 */
   GL1_MSG_FT_HDLR_UL_PDCH,          /* UL PDTCH, PBCCH, PCCCH, PACCH */
   GL1_MSG_FT_HDLR_UL_PTCCH_SACCH,   /* UL PTCCH, SACCH/26 */
   GL1_MSG_FT_HDLR_ACQ_NCELL,        /* NCELL activity */
   GL1_MSG_FT_HDLR_INT_MEAS,         /* Interference measures */
   GL1_MSG_FT_HDLR_PWR_MEAS          /* Power measures */
} gl1_msgi_ft_handler_type;

typedef boolean (*gl1_msgi_ft_handler_func_type)
(
   boolean start_of_block,
   uint32 time_stamp,
   gas_id_t    gas_id
);


/* Transmit log information */
typedef PACKED struct PACKED_POST
{
   gl1_defs_tn_type      tn;
   gl1_msg_ul_chn_type   msg_type;
   boolean               dynamic_alloc;
   int16                 power_index;
   uint8                 timing_advance;
   gl1_defs_coding_type  coding_scheme;
} log_gprs_tx_scheduled_result_type;

LOG_RECORD_DEFINE(LOG_GPRS_TX_SCHEDULED_RESULTS_C)
   uint32                             FN;
   uint8                              num_msg_tx;
   uint16                             arfcn[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
   log_gprs_tx_scheduled_result_type  msg_info[GL1_DEFS_MAX_ASSIGNED_UL_TS];
LOG_RECORD_END


 /* -----------------------------------------------------------------------
** GSM Type Declarations
** ----------------------------------------------------------------------- */

/* Common information for all async CCCH receives (ncell BCCH, CBCH) */
typedef struct
{
   uint8               TSC;
   boolean             DRX;
   sys_algo_agc_T      *agc_ptr;
   ARFCN_T             ARFCN[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
   uint16              timing_offset;
   gl1_msg_rx_cb_type  callbacks;
   gl1_msg_rx_rpt      rpt;
   gl1_hw_channel_type hw_channel_type;
   gl1_hw_rx_tags_type tags[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
   async_ccch_type_t   async_ccch_type;
} gl1_msgi_async_ccch_params_type;



/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*=============================================================================
FUNCTION  gl1_msgi_add_ft_handler

DESCRIPTION
   This function adds a new frame tick handlers of the given type.

============================================================================= */
void gl1_msgi_add_ft_handler
(
   gl1_msgi_ft_handler_func_type handler,
   gl1_msgi_ft_handler_type      type,
   gas_id_t                      gas_id
);


/*=============================================================================
FUNCTION  gl1_msgi_remove_ft_handler

DESCRIPTION
   This function removes the given frame tick handler

============================================================================= */
void gl1_msgi_remove_ft_handler (gl1_msgi_ft_handler_func_type frame_handler,gas_id_t  gas_id);

/*===========================================================================

FUNCTION gl1_msg_tt_do_pending_adjust

DESCRIPTION
  Attempts to perform any pending time tracking adjustments.

===========================================================================*/
void gl1_msg_tt_do_pending_adjust (gas_id_t gas_id);


/*-----------------------------------------------------------------------------
                  GSM Functions
------------------------------------------------------------------------------*/

/* Initialization functions for the GSM channels */
void gl1_msgi_init_ncell_fcch (gas_id_t gas_id);
void gl1_msgi_init_ncell_sch (gas_id_t gas_id);
void gl1_msgi_init_ncell_bcch (gas_id_t gas_id);
void gl1_msgi_init_fcch_sch (gas_id_t gas_id);
void gl1_msgi_init_cbch (gas_id_t gas_id);
void gl1_msgi_init_ccch (gas_id_t gas_id);
void gl1_msgi_init_async_ccch (gas_id_t gas_id);
void gl1_msgi_init_dl_tch (gas_id_t gas_id);
void gl1_msgi_init_dl_tch_amr (gas_id_t gas_id);
void gl1_msgi_init_ul_tch (gas_id_t gas_id);
void gl1_msgi_init_ul_tch_amr (gas_id_t gas_id);
void gl1_msgi_init_sacch (gas_id_t gas_id);
void gl1_msgi_init_sdcch (gas_id_t gas_id);
void gl1_msgi_init_rach  (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msgi_map_txlev_to_pwr_idx

DESCRIPTION
  This function maps the TXLEV based on RF band information to a power
  index used by the HW layer.
  NOTE: The power index is mapped in revers order from TXLEV.  It is
  also offset by the min TXLEV for the power class.  So for power class 4
  in GSM900 the power index is offset by 5 such that power index 0
  is TXLEV 5.


DEPENDENCIES

  arfcn       - input, channel to determine the RF band class so that the
                mapping is done correctly.
                NOTE: single ARFCN is sufficient since you can't hop from
                band to band in a message.
  txlev       - input, as specified in 3GPP specs (05 series)
  power_idx   - output, Index used by HW layer.
  num_entries - input, number of items to map.

RETURN VALUE
  None

SIDE EFFECTS
  Internal YPF variable is updated by stores.

===========================================================================*/
extern void gl1_msgi_map_txlev_to_pwr_idx
(
  ARFCN_T     arfcn,
  const int16 txlev[],
  int16       power_idx[],
  uint8       num_entries,
  gas_id_t    gas_id
);




/*===========================================================================

FUNCTION gl1_msgi_abort_rx_async_ccch

DESCRIPTION
  This function aborts an ongoing async ccch.

===========================================================================*/
void gl1_msgi_abort_rx_async_ccch (gl1_msg_chan_type channel_type,gas_id_t gas_id);


/*===========================================================================

FUNCTION gl1_msgi_rx_async_ccch

DESCRIPTION
  This function is called to start an async CCCH receive.

===========================================================================*/
void gl1_msgi_rx_async_ccch
(
   gl1_msg_chan_type               channel_type,
   gl1_msgi_async_ccch_params_type *params,
   gas_id_t                         gas_id
);


void gl1_msg_update_bcch_rotator_value(int32 freq_error,gas_id_t gas_id);

/*-----------------------------------------------------------------------------
                  GPRS Functions
------------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION gl1_msgi_pdch_init

DESCRIPTION
   This function initializes data structures that are used by all PDCHs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pdch_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_pdch_terminate

DESCRIPTION
   Terminates all PDCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pdch_terminate( gas_id_t gas_id );


/*===========================================================================

FUNCTION gl1_msgi_ul_pacch_pdtch_init

DESCRIPTION
   This function initializes data structures that are used by PACCH/PDTCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_ul_pacch_pdtch_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_set_ul_alloc_info

DESCRIPTION
   gl1_msg_dl_cfg_pacch_pdtch() calls to set alloc information passed from PL1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the state info
===========================================================================*/
void gl1_msg_set_ul_alloc_info
(
   gl1_defs_rx_pkt_alloc_type alloc_info[GL1_DEFS_MAX_ASSIGNED_DL_TS],
   uint8   num_alloc_tn,
   boolean usf_gran_set,
   gas_id_t gas_id
);

/*===========================================================================

FUNCTION gl1_msg_set_usf_alloc_info

DESCRIPTION
   gl1_msgi_pbcch_pccch_pacch_pdtch_handler() in gl1_msg_pdch.c calls to set
   the USF chn alloc information obtained from mDSP interface layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the state info
===========================================================================*/
void gl1_msg_set_usf_alloc_info
(
    gl1_defs_rx_pkt_alloc_type usfs[GL1_DEFS_MAX_ASSIGNED_DL_TS],
    uint32 num_usfs,
    gas_id_t gas_id
);


/*===========================================================================
FUNCTION gl1_msgi_ul_pacch_pdtch_terminate

DESCRIPTION
   Terminates all PACCH/PDTCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_ul_pacch_pdtch_terminate( gas_id_t gas_id );


/*===========================================================================

FUNCTION gl1_msgi_pbcch_init

DESCRIPTION
   This function initializes data structures that are used by PBCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pbcch_init( void );

/*===========================================================================

FUNCTION gl1_msgi_pbcch_terminate

DESCRIPTION
   Terminates all PBCCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pbcch_terminate( void );

/*===========================================================================

FUNCTION gl1_msg_abort_pbcch

DESCRIPTION
   Aborts all PBCCH processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_abort_pbcch( void );

/*===========================================================================

FUNCTION gl1_msgi_pccch_init

DESCRIPTION
   This function initializes data structures that are used by PBCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pccch_init( void );

/*===========================================================================

FUNCTION gl1_msgi_pccch_terminate

DESCRIPTION
   Terminates all PCCCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pccch_terminate( void );

/*===========================================================================

FUNCTION gl1_msg_abort_pccch

DESCRIPTION
   Aborts all PCCCH processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_abort_pccch( void );


/*===========================================================================

FUNCTION gl1_msgi_dl_ptcch_init

DESCRIPTION
   This function initializes data structures that are used by DL PTCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_dl_ptcch_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_dl_ptcch_terminate

DESCRIPTION
   Terminates all DL PTCCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_dl_ptcch_terminate( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_dl_pacch_pdtch_init

DESCRIPTION
   This function initializes data structures that are used by dl_pacch_pdtch.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_dl_pacch_pdtch_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_dl_pacch_pdtch_terminate

DESCRIPTION
   Terminates all dl_pacch_pdtch processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_dl_pacch_pdtch_terminate( gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msgi_prach_init

DESCRIPTION
   This function initializes data structures that are used by all PDCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_prach_init( void );

/*===========================================================================

FUNCTION gl1_msgi_prach_terminate

DESCRIPTION
   Terminates all common PDCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_prach_terminate( void );

/*===========================================================================

FUNCTION gl1_msgi_ul_ptcch_init

DESCRIPTION
   This function initializes data structures that are used by all PDCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_ul_ptcch_init( gas_id_t  gas_id );

/*===========================================================================

FUNCTION gl1_msgi_ul_ptcch_terminate

DESCRIPTION
   Terminates all common PDCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_ul_ptcch_terminate( gas_id_t  gas_id );

/*===========================================================================

FUNCTION gl1_msg_ul_pacch_pdtch_init

DESCRIPTION
   This function initializes data structures that are used by all PDCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_ul_pacch_pdtch_init( void );

/*===========================================================================

FUNCTION gl1_msg_ul_pacch_pdtch_terminate

DESCRIPTION
   Terminates all common PDCH processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_ul_pacch_pdtch_terminate (void);


/*===========================================================================
FUNCTION gl1_msgi_int_meas_init

DESCRIPTION
   This function initializes data structures used for interference measurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  State reset
===========================================================================*/
void gl1_msgi_int_meas_init (gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_wcdma_int

DESCRIPTION
   This function initializes data structures used for wcdma measurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  State reset
===========================================================================*/
void gl1_msg_wcdma_init (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msgi_int_meas_terminate

DESCRIPTION
   Terminates processing of any current interference measurements


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_int_meas_terminate (gas_id_t gas_id);


/*===========================================================================

FUNCTION gl1_msg_abort_int_meas

DESCRIPTION
   Aborts processing of any current interference measurements

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_abort_int_meas (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_log_ft_handlers

DESCRIPTION
   logs the currently installed FThandlers at msg layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_log_ft_handlers (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_get_meas_in_progress

DESCRIPTION
   returns if meas_in_progress is true for any of the state machines

DEPENDENCIES
  None

RETURN VALUE
  TRUE if any of the state machine has meas_in_progress

SIDE EFFECTS
  None
===========================================================================*/

boolean gl1_msg_get_meas_in_progress(gas_id_t gas_id);



/*===========================================================================
FUNCTION gl1_msgi_pwr_meas_init

DESCRIPTION
   This function initializes data structures used for power measurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pwr_meas_init(gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msgi_pwr_meas_terminate

DESCRIPTION
   Terminates processing of any current power measurements


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msgi_pwr_meas_terminate(gas_id_t gas_id);



#endif /* GL1_MSGI_H */



