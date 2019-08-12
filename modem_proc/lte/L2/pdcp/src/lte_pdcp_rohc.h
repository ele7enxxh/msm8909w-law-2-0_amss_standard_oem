 /*!
   @file
   lte_pdcp_rohc.h
 
   @brief
   RoHC wrapper for ease of stubing out on off-target build.
 
 */
 
 /*==============================================================================
 
   Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved
 
   Qualcomm Proprietary
 
   Export of this technology or software is regulated by the U.S. Government.
   Diversion contrary to U.S. law prohibited.
 
   All ideas, data and information contained in or disclosed by
   this document are confidential and proprietary information of
   Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
   By accepting this material the recipient agrees that this material
   and the information contained therein are held in confidence and in
   trust and will not be used, copied, reproduced in whole or in part,
   nor its contents revealed in any manner to others without the express
   written permission of Qualcomm Technologies Incorporated.
 
 ==============================================================================*/
 
 /*==============================================================================
 
                         EDIT HISTORY FOR MODULE
 
 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_rohc.h#1 $
 
 when       who     what, where, why
 --------   ---     ------------------------------------------------------------ 
 10/10/14   sb      CR656234 - For consecutive SDU discards, call rohc_reset() with
                    new IOCT to ensure compressor starts from FO-state
09/19/14    sb      CR706535: Defense mechanism in UE to counter HFN mismatch
04/02/14    sb      CR642230: REL11 RoHC context continuity during HO 
                    within same eNB 
06/19/13    sb      CR458155: Set the reason for RoHC Reset (HO/Discard) so that
                    OFFLOAD calls the correct RoHC IOCTL 
 01/10/12   ax      CR328903: set feedback_for for decompressor  
 09/06/11   ax      add support for piggyback RoHC feedback
 03/28/11   ax      initial version
 ==============================================================================*/
 
 #ifndef LTE_PDCP_ROHC_H
 #define LTE_PDCP_ROHC_H
 
 /*==============================================================================
 
                            INCLUDE FILES
 
 ==============================================================================*/
 
#include "lte_variation.h"
 #include "comdef.h"
 #include "ps_rohc.h"
 #include "lte_pdcp_offload_protocol.h"
 
 
 /*==============================================================================
 
                    EXTERNAL DEFINITIONS AND TYPES
 
 ==============================================================================*/
 

 /*==============================================================================
 
                     EXTERNAL FUNCTION PROTOTYPES
 
 ==============================================================================*/
 
#ifdef TEST_FRAMEWORK
#error code not present
#else

#define lte_pdcp_rohc_powerup_init() \
  rohc_powerup_init()

#define lte_pdcp_rohc_decompress(rohc_handle, pkt_ptr, ipver, fb_ptr, err_no) \
  rohc_decompress(rohc_handle, NULL, NULL, pkt_ptr, ipver, fb_ptr, 0, err_no)

#define lte_pdcp_rohc_compress(rohc_handle, pkt_ptr, err_no) \
  rohc_compress(rohc_handle,NULL, NULL, pkt_ptr, err_no)

#define lte_pdcp_rohc_sink_feedback(handle, fb_cid, pkt, err) \
  rohc_comp_sink_feedback(handle,fb_cid, pkt, err)

#define lte_pdcp_rohc_alloc(comp_chn, comp_flow, decomp_chn, decomp_flow, err_no) \
  rohc_alloc(comp_chn, comp_flow, decomp_chn, decomp_flow, err_no)

#define lte_pdcp_rohc_free(rohc_handle) \
  rohc_free(rohc_handle)

#define lte_pdcp_rohc_soft_reset(rohc_handle, err_no) \
  rohc_ioctl(rohc_handle, ROHC_IOCTL_SET_PDCP_DISCARD_HANDLE, NULL, err_no)

#define lte_pdcp_rohc_ho_reset(rohc_handle, err_no) \
  rohc_ioctl(rohc_handle, ROHC_IOCTL_REINIT_COMP_CHANNEL, NULL, err_no)

#define lte_pdcpdl_rohc_ho_reset(rohc_handle, decomp_handle_ptr, err_no) \
  rohc_ioctl(rohc_handle, ROHC_IOCTL_REINIT_DECOMP_CHANNEL, decomp_handle_ptr, err_no)

#define lte_pdcp_rohc_set_fb_for(rohc_handle, comp_handle_ptr, err_no) \
  rohc_ioctl(rohc_handle, ROHC_IOCTL_SET_FEEDBACK_FOR, comp_handle_ptr, err_no)

#define lte_pdcp_snack_event_cb_register(rohc_handle, ul_snack, dl_snack, cb_fun, cb_data, err_no) \
  rohc_cli_snack_event_cb_register(rohc_handle, ul_snack, dl_snack, cb_fun, cb_data, err_no)

#endif

#endif /* LTE_PDCP_ROHC_H */
