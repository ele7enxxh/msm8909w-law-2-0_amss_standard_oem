/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ D P M _ DPL . C

GENERAL DESCRIPTION
  This is the implementation of the Data Path Manager module

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_dpl.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/08/14    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_svc.h"
#include "ps_logging.h"
#include "ps_iface.h"
#include "ps_logging_helper.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_dpmi.h"
#include "ps_dpm_dpl.h"
#ifdef FEATURE_DATA_IPA
#include "ipa.h"
#include "ipa_ipfltr.h"
#endif /* FEATURE_DATA_IPA */
#include "dsm.h"
#include "ps_dpm_opt_defs.h"

/*===========================================================================
                            LOCAL DEFINITIONS
===========================================================================*/
#define PS_DPM_DPL_WM_DEFAULT_LO   175000 //10ms*100Mbps
#define PS_DPM_DPL_WM_DEFAULT_HI   350000 //20ms*100Mbps
#define PS_DPM_DPL_WM_DEFAULT_DNE  400000 //HI+50k
/*Discard  received DSM item when DNE packet count hit*/
#define PS_DPM_DPL_WM_DNE_PKT_COUNT  500 

dsm_watermark_type ps_dpm_dpl_wmk;
q_type dpm_dpl_q;

/*===========================================================================
FUNCTION   PS_DPM_DPL_WMK_NON_EMPTY_FUNCTION()

DESCRIPTION
  This function is called whenever IPA DPL watermark becomes non-empty. 

DEPENDENCIES
  None.

PARAMETERS - FIX
  dsm_watermark_type: the Watermark  that is handled
  user_data_ptr: NOT USED

RETURN VALUE
..None

SIDE EFFECTS
  None.
===========================================================================*/
static void  ps_dpm_dpl_wmk_non_empty_function
(
  dsm_watermark_type  * dpm_dpl_wmk,
  void                * user_data_ptr
)
{
  PS_SET_EXT1_SIGNAL(PS_DPM_DPL_SIGNAL);
} /* ps_dpm_dpl_wmk_non_empty_function() */

void ps_dpm_dpl_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize queue */
  dsm_queue_init ( &ps_dpm_dpl_wmk, 0x7FFFFFFF, &dpm_dpl_q );

  dsm_set_low_wm( &ps_dpm_dpl_wmk, PS_DPM_DPL_WM_DEFAULT_LO );
  dsm_set_hi_wm( &ps_dpm_dpl_wmk, PS_DPM_DPL_WM_DEFAULT_HI );
  dsm_set_dne( &ps_dpm_dpl_wmk, PS_DPM_DPL_WM_DEFAULT_DNE );
  dsm_set_dne_q_cnt( &ps_dpm_dpl_wmk, PS_DPM_DPL_WM_DNE_PKT_COUNT );

  /* Set the non-empty function ptr */
  ps_dpm_dpl_wmk.non_empty_func_ptr = &ps_dpm_dpl_wmk_non_empty_function;

  /* Register DPM DPL WMK with specific hardware */
  ps_dpm_hw_reg_dpl_wmk(&ps_dpm_dpl_wmk);

  (void) ps_set_sig_handler(PS_DPM_DPL_SIGNAL, ps_dpm_hw_dpl_sig_handler, NULL);
  ps_enable_sig( PS_DPM_DPL_SIGNAL );
} /* ps_dpm_dpl_init() */

void ps_dpm_dpl_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_queue_destroy(&ps_dpm_dpl_wmk);
} /*ps_dpm_dpl_deinit() */

/*===========================================================================
FUNCTION   PS_DPMI_DPL_GET_WMK

DESCRIPTION 
  This function is used to return pointer to DPM DPL WM

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  dsm_watermark_type* - Pointer to ps_dpm_dpl_wmk
===========================================================================*/
dsm_watermark_type *ps_dpmi_dpl_get_wmk
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (&ps_dpm_dpl_wmk);
} /*ps_dpmi_dpl_get_wmk() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/
