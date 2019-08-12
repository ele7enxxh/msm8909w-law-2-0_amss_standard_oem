 /*===========================================================================
                  D O W N L I N K   T R A N S P A R E N T   M O D E   

                          R A D I O   L I N K   C O N T R O L  

DESCRIPTION
  This file provides the functions to process RLC Transparent Mode 
  Downlink logical channels.  
    
EXTERNALIZED FUNCTIONS
  List any functions that are externalized in this file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2004, 2007-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdltm.c_v   1.9   08 May 2002 19:13:34   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcdltm.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
11/20/13   kv      Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_RLC
02/01/12   grk     Feature cleanup
03/22/10   grk     Added debug prints for TM.
09/07/09   pj      Reverting discard-PDUs changes
09/31/09   pj      Added changes to perform discard_dsm API, whenever WM is not registered
06/22/09   ssg     Reverted the previous check-in.
06/17/09   ssg     Mainlining FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
05/11/09   ssg     Updated Copyright Information
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
           vg      1. Fixed issues during DL integration testing
           vg/sk   2. Added check for water mark for tm pdu processing and release
                      dsm item if water mark is NULL
07/11/07   ssg     Added F3 msgs under FEATURE_RLC_TM_LOGGING.
08/20/04   mv      Changed the storing of pdu_len so that only the last 16 
                   bits of the app field are used if the pdus are processed 
                   in the interrupt context under a feature 
                   FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
06/30/04   mv      Changed a message error to print argument name.
01/20/04   mv      Replaced the FEATURE_RLC_ENHANCED_CB with the
                   FEATURE_RLC_SDU_RCVD_ENHANCED_CB.
12/29/03   vsk     Replace FEATURE_MULTIPLE_PDP with FEATURE_RLC_ENHANCED_CB
09/23/03   mv      Added a third parameter to the rlc_post_rx_func_ptr()
                   to support multiple PDP contexts under the feature
                   FEATURE_MULTIPLE_PDP.
10/06/02   ttl     Added an internal PDUs receving queue rlci_rcvd_q to replace 
                   the structure pointer dl_data_ptr shared by MAC and RLC.
05/08/02   ttl     Give the number of SDU being enqueued into WM when call
                   rlc_post_rx_func_ptr().
11/04/01   ttl     Called rlc_post_rx_func_ptr() only when PDU is enqueued.
10/26/01   ttl     Moved TM PDUs porcessing back to L2 context.
10/12/01   ttl     Moved TM PDUs processing into interrupt context, but leaving
                   the Upper layer informing in L2 context.
08/27/01   rc      Removed Watermark Check.
08/10/01   rc      Fixed a comment.
05/21/01   rc      Added more comments.
01/31/01   rc      Changed function name from rlci_reassemble_dl_tm_pdus() to
                   rlci_dl_reassemble_tm_pdus().
01/08/01   rc      Cleaned up code.
11/22/00   rc      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */


#include "wcdma_variation.h"
#include "rlcdltm.h"

/* =======================================================================
**                            Function Definitions
** ======================================================================= */




/*===========================================================================

FUNCTION rlci_enh_dl_reassemble_tm_pdus

DESCRIPTION
  Enqueues the Downlink Transparent Mode PDUs that arrive from MAC into 
  the downlink queue for that channel and calls a callback function, if 
  the higher layer has registered for it.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_reassemble_tm_pdus(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                    complete_pdu_type *complete_pdu_ptr)
{
  mcalwcdma_dec_iovec_type *iovec_ptr = &(complete_pdu_ptr->iovec);
  
  /* If Water mark is not registered, drop the PDU and return */
  if (ctrl_blk_ptr->dl_wm_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: WM Not registered - Dropping the PDU");
    if (iovec_ptr->ptr != NULL)
    {
      dsm_free_packet(&(iovec_ptr->ptr));
    }
    return;
  }
  
  /*-------------------------------------------------------------------------
    Assign the pdu size(in bits) to the app field of the dsm_item_type 
    structure. Enqueue the dsm item on the watermark queue and call the 
    callback function (if registered).
  -------------------------------------------------------------------------*/
  if (iovec_ptr->ptr != NULL)
  {

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING  
    /*-----------------------------------------------------------------------
      If the processing is in task context, overwrite the app field, else
      write the pdu length to the 16 Least significant bits, as the other
      bits may be used by some other app
    -----------------------------------------------------------------------*/
    if (ctrl_blk_ptr->context == FALSE)
    {
      iovec_ptr->ptr->app_field = iovec_ptr->length;
    }
    else
    {
      iovec_ptr->ptr->app_field &= 0xFFFF0000;
      iovec_ptr->ptr->app_field |= iovec_ptr->length;
    }
#else
    iovec_ptr->ptr->app_field = iovec_ptr->length;
#endif
    if (rlc_debug_var & RLC_DBG_DL_TM)
    {
      MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,
              "TM DL PDU Payload: 0x%x  0x%x  0x%x pdu len %d",
               rlc_copy_to_word(iovec_ptr->ptr->data_ptr), 
               rlc_copy_to_word(iovec_ptr->ptr->data_ptr+4), 
               rlc_copy_to_word(iovec_ptr->ptr->data_ptr+8), iovec_ptr->ptr->app_field, 0, 0, 0, 0, 0);
    }
    dsm_enqueue(ctrl_blk_ptr->dl_wm_ptr, &(iovec_ptr->ptr));
    if (ctrl_blk_ptr->rlc_post_rx_func_ptr != NULL)
    {
      ctrl_blk_ptr->rlc_post_rx_func_ptr(complete_pdu_ptr->rlc_id, 1, 
                                  ctrl_blk_ptr->rlc_post_rx_func_ptr_para);

    }
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: NULL TM PDU, LC %d",
                                                complete_pdu_ptr->rlc_id);
  }
}


