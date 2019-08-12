#ifndef RLCULAM_H
#define RLCULAM_H
/*===========================================================================
                  U P L I N K   N A C K N O W L E D G E D   M O D E   

               R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  Uplink acknowledged Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2004, 2006-2009  Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlculam.h_v   1.5   24 Apr 2002 22:18:10   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlculam.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ---------------------------------------------------------- 
10/12/15  scv     Prevent taking DACC Mutex after EUL ISR Mutex in Task context
10/12/15  scv     Changes for DSM Multiple DeQ/EnQ
05/25/15  kc      RLC to consider status_rpt_cnt for building CTRL PDU
01/14/15  aa      Changes for new ULC log packet(0x41C5)
12/09/14  aa      Fixed compiler warnings
09/29/14  kc      Reverting RLC_OOS: Changes to re-enqueue the SDU's that are waiting for ACK back to the RLC UL 
                  Watermark during RLC RESET/RE-establishment procedure.
08/12/14  scv     ULC: Correct handling of partially transmitted SDUs on Reset/Re-establish
07/24/14  scv     MAC_I: Fix to ensure correct CTRL PDU size selection & building logics
05/13/14  aa      Code changes to poll for SN VT(MS)-1 rather than VT(S)-1 if VT(MS)<VT(S)
05/06/14  kv      Replacing dsm_queue_cnt API with DSM_GET_WM_CURRENT_CNT MACRO 
04/04/14  scv     Code changes for UL Compression Feature
04/02/14  scv     Code changes for UL Compression Feature
02/10/14  as      Fix to avoid writing array out of bounds during AM logging
01/09/14  scv     Code changes for UL Compression Feature
12/09/13  as      Added code change not to disable RACH if there is any pending data in DL 
                  that needs to be received which in turn would not trigger EDRX full sleep.
12/04/13  scv     Code changes to correctly free DSMs for UL Compression Feature
11/27/13  scv     Code changes for UL Compression feature
11/20/13  kv      Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_RLC
11/20/13  as      RLC_OOS: Changes to re-enqueue the SDU's that are waiting for ACK back to the RLC UL 
                  Watermark during RLC RESET/RE-establishment procedure.
02/21/13  as      MAC-I  Added code change to avoid building POLL SUFI control 
                  PDU when data PDU is sent.
01/15/13  as      Fix to send PDU with VT_MS - 1 SN instead of re-transmitting VT_S - 1 
                  PDU during TX window shrinkage condition.
01/07/13  as      (1) Limit the number of SDU per PDU to 30
                     (considering the higher PDU Sizes with MAC-I and assuming all the UL traffic is of TCP ACKs 65bytes)
                  (2). Limit the number of SDU per TTI 60 
11/06/12  as      Fix to update special LI header only if possible based upon PDU size.
09/27/12   as     MAC-i-is:  SDUs added after RLC-Buff-status computation needs to 
                  be de-queued  before BFI and en-queue them in same order back to WM
09/21/12   as     Cleaned up code with proper identation and added missing function headers. 
07/16/12  mrg     MAC_I & HS-RACH Compilation Warnings fixed
08/23/12  mrg     MAC-iis: Special HE functionality with LI 7 configured and 
                  RLC PDU size greater than 126 octets
08/13/12  mrg     HS-RACH & MAC-I Feature updated code check-in.
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/18/12  mrg     L2 - MAC-I and HS-RACH code changes
02/01/12  grk     Feature cleanup
01/23/12   as     Support for Free Floating Task changes
7/30/11   ksr     MC FC Mutex allocation, free mechanism
01/17/11  vp      CMI Phase-4 clean-up of header files
10/07/09  av      Multi-client flow control algorithm 
08/13/09  grk     Changes for bitmap based status tracking.
05/04/09  ssg     Added support for Timer Based SDU Discard for L2-opt.
06/26/08  av      l1-l2 interface cleanup 
05/29/09  ssg     Added support for SNOW_3G algo.
05/14/09  ssg     Correct the Macro Value. 
05/13/09  av      Fixed compiler error in non-datapath opt target 
04/29/09  av      Fixed compiler warnings in L2 
12/08/08  pj      Added prototype for rlci_get_retx_bytes()
12/08/08  ssg     Added fixes for UL-PP-HW.
11/25/08  pj      Removed FEATURE_RLC_DBG_UL_PDU_DUMP flag, as the dumping is already
                  under rlc_debug_var
11/07/08  pj      Added prototype for PDU dump function
05/05/08  pj      Added Flag protection for UL logging
04/18/08  ssg     Added new LI related macro definitions.
03/24/08  pj      Added support for data path optimizations under 
                  FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
          ssg     1. Added MACROs for partly and Newly txd SDU.
09/06/07  ssg     Added support to enhanced buffer occupancy computation 
                  under the flag FEATURE_RLC_ENH_BUFFER_STATUS.
02/16/07  ttl     function declaration.
08/01/06  ttl     Add support to EUL.
11/04/04  mv      Moved the rlci_post_rlc_reset_cmd() function definition to
                  rlci.h.
08/01/03  mv      Added rlci_re_establish_ul_am() definition.
02/14/03  ttl     Defined rlci_post_rlc_reset_cmd() and rlci_am_reset().
04/24/02  ttl     Rewrote the statistic log code.
03/15/02  sk      Added function definition for rlci_ul_am_handle_qxdm_log_timer
                  to handle the case whenthe log timer expires.
05/21/01  ttl     Modified code for compiler level 4 warning messages.
05/21/01  rc      Removed a compiler warning.
05/02/01  ttl     Added rlci_reset_ul_am() definition.
02/28/01  ttl     Created file.

===========================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "macrlcif.h"
#include "rlcul.h"

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/*---------------------------------------------------------------------------
  app_field(0)    : 1 - SDU cnf required, 0 - Otherwise
  app_field(1)    : 1 - Partly txd SDU, 0 - New SDU
  app_field(15-8) : MUI of the SDU.
  app_field(27-16): last_sn in which this SDU ends.
---------------------------------------------------------------------------*/
#define   RLC_UL_AM_SDU_TX_CNF(x)             ((x) & 0x1)
#define   RLC_UL_AM_GET_SDU_MUI(x)            (((x) & 0x0000FF00) >> 8)
#define   RLC_UL_AM_GET_SDU_PDU_END_SN(x)     (((x) & 0x0FFF0000) >> 16)
#define   RLC_UL_AM_SET_SDU_PDU_END_SN(x, y)  ((x) |= ((y) << 16))
#define   RLC_UL_AM_SET_PARTLY_TXD_SDU(x)     ((x) |= 0x00000002)
#define   RLC_UL_AM_PARTLY_TXD_SDU(x)         ((x) & 0x00000002)
#define   RLC_UL_AM_NEWLY_TXD_SDU(x)          (!((x) & 0x00000002))

#ifdef FEATURE_WCDMA_UL_COMPR
#define RLC_UL_COMP_CTRL_SDU_PRESENT_MASK      0x10000000
#define RLC_GET_UL_COMP_CTRL_SDU_PRESENT(a)    ((a) & RLC_UL_COMP_CTRL_SDU_PRESENT_MASK)
#define RLC_SET_UL_COMP_CTRL_SDU_PRESENT(a)    ((a) |= RLC_UL_COMP_CTRL_SDU_PRESENT_MASK)

#define RLC_UL_COMP_DATA_SDU_PRESENT_MASK      0x20000000
#define RLC_GET_UL_COMP_DATA_SDU_PRESENT(a)    ((a) & RLC_UL_COMP_DATA_SDU_PRESENT_MASK)
#define RLC_SET_UL_COMP_DATA_SDU_PRESENT(a)    ((a) |= RLC_UL_COMP_DATA_SDU_PRESENT_MASK)
#endif

/*---------------------------------------------------------------------------
  PDU ctrl_info related definitions.
---------------------------------------------------------------------------*/
#define RLC_SPL_LI_OCTET_LESS_IN_NEXT_PDU_MASK    0x00200
#define RLC_SPL_LI_OCTET_LESS_IN_NEXT_PDU(a)      ((a) & RLC_SPL_LI_OCTET_LESS_IN_NEXT_PDU_MASK)
#define RLC_SET_SPL_LI_OCTET_LESS_IN_NEXT_PDU(a)  ((a) |= RLC_SPL_LI_OCTET_LESS_IN_NEXT_PDU_MASK)

#define RLC_PDU_FIT_MASK                          0x00100
#define RLC_PDU_FIT(a)                            ((a) & RLC_PDU_FIT_MASK)
#define RLC_SET_PDU_FIT(a)                        ((a) |= RLC_PDU_FIT_MASK)

#define RLC_PDU_FIT_HE_MASK                       0x00400
#define RLC_PDU_FIT_HE(a)                         ((a) & RLC_PDU_FIT_HE_MASK)
#define RLC_SET_FIT_HE(a)                         ((a) |= RLC_PDU_FIT_HE_MASK)

#define RLC_SPL_LI_EXACT_IN_NEXT_PDU_MASK         0x0080
#define RLC_SPL_LI_EXACT_IN_NEXT_PDU(a)           ((a) & RLC_SPL_LI_EXACT_IN_NEXT_PDU_MASK)
#define RLC_SET_SPL_LI_EXACT_IN_NEXT_PDU(a)       ((a) |= RLC_SPL_LI_EXACT_IN_NEXT_PDU_MASK)

#define RLC_LI_EXACT_PRESENT_MASK                 0x0040
#define RLC_LI_EXACT_PRESENT(a)                   ((a) & RLC_LI_EXACT_PRESENT_MASK)
#define RLC_SET_LI_EXACT_PRESENT(a)               ((a) |= RLC_LI_EXACT_PRESENT_MASK)

#define RLC_LI_OCTET_LESS_PRESENT_MASK            0x0020
#define RLC_LI_OCTET_LESS_PRESENT(a)              ((a) & RLC_LI_OCTET_LESS_PRESENT_MASK)
#define RLC_SET_LI_OCTET_LESS_PRESENT(a)          ((a) |= RLC_LI_OCTET_LESS_PRESENT_MASK)

#define RLC_PADDING_LI_PRESENT_MASK               0x0010
#define RLC_PADDING_LI_PRESENT(a)                 ((a) & RLC_PADDING_LI_PRESENT_MASK)
#define RLC_SET_PADDING_LI_PRESENT(a)             ((a) |= RLC_PADDING_LI_PRESENT_MASK)

#define RLC_PIGGYBACK_STATUS_LI_PRESENT_MASK      0x1000
#define RLC_PIGGYBACK_STATUS_LI_PRESENT(a)        ((a) & RLC_PIGGYBACK_STATUS_LI_PRESENT_MASK)
#define RLC_SET_PIGGYBACK_STATUS_LI_PRESENT(a)    ((a) |= RLC_PIGGYBACK_STATUS_LI_PRESENT_MASK)

#define RLC_PDU_STATUS_MASK                       0x0800
#define RLC_GET_PDU_STATUS(a)                     ((a) & RLC_PDU_STATUS_MASK)
#define RLC_SET_PDU_NAK(a)                        (a |= RLC_PDU_STATUS_MASK)
#define RLC_RESET_PDU_NAK(a)                      (a &= (~RLC_PDU_STATUS_MASK))

#define RLC_NUM_SDUS_MASK               0x00FF
#define RLC_GET_NUM_SDUS(a)             ((a) & RLC_NUM_SDUS_MASK)
#define RLC_SET_NUM_SDUS(a,val)         ((a) |= ((val) & RLC_NUM_SDUS_MASK))

#define RLC_LI_PRESENT_MASK             0x0170
#define RLC_LI_PRESENT(a)               (((a) & RLC_LI_PRESENT_MASK))

#define RLC_NO_PART_SDU_MASK            0x0390
#define RLC_NO_PART_SDU(a)              ((a) & RLC_NO_PART_SDU_MASK)

#ifdef FEATURE_MAC_I
#define RLC_AM_IS_CTRL_PDU_PENDING(am_ctrl_ptr, ul_state_ptr) \
       (ul_state_ptr->status_rpt      || \
        ul_state_ptr->status_rpt_cnt  || \
        ul_state_ptr->tx_wins         || \
        ul_state_ptr->tx_mrw          || \
        (q_cnt(&am_ctrl_ptr->mrw_ack_q) > 0)  || \
        ul_state_ptr->owe_reset       || \
        ul_state_ptr->owe_reset_ack  \
        || ul_state_ptr->poll_sufi_reported \
        )
#else
#define RLC_AM_IS_CTRL_PDU_PENDING(am_ctrl_ptr, ul_state_ptr) \
       (ul_state_ptr->status_rpt      || \
        ul_state_ptr->status_rpt_cnt  || \       
        ul_state_ptr->tx_wins         || \
        ul_state_ptr->tx_mrw          || \
        (q_cnt(&am_ctrl_ptr->mrw_ack_q) > 0)  || \
        ul_state_ptr->owe_reset       || \
        ul_state_ptr->owe_reset_ack  \
        )

#endif
#define RLC_AM_IS_RETX_PDU_PENDING(ul_state_ptr) \
               ((FALSE == ul_state_ptr->retx_q_sema) && \
                (((!RLCI_SEQ_GT(ul_state_ptr->vt_s, ul_state_ptr->vt_ms, RLC_AM_MODULUS)) && \
                  (ul_state_ptr->retx_size > 0)) || \
                 ((RLCI_SEQ_GT(ul_state_ptr->vt_s, ul_state_ptr->vt_ms, RLC_AM_MODULUS)) && \
                  (ul_state_ptr->retx_size_reduced > 0)))\
                )

#define RLC_UL_AM_RLC_RET_MAX_SN_VAL(vt_ms,vt_s) (RLCI_SEQ_GE(vt_ms,vt_s, RLC_AM_MODULUS)? \
                                                               (vt_s):(vt_ms))

#ifdef FEATURE_RLC_QXDM_LOGGING
#define RLC_AM_UPD_NUM_CTRL_PDU_CNT(status, am_ctrl_ptr) \
  (status == TRUE) ? (am_ctrl_ptr->ul_chan_am_stat.tot_num_ctrl_pdu_txd++) : (TRUE)

#define RLC_AM_UPD_NUM_NEW_PDU_CNT(status, am_ctrl_ptr) \
  (status == TRUE) ? (am_ctrl_ptr->ul_chan_am_stat.tot_num_new_data_pdu_txd++) : (TRUE)


#else
#define RLC_AM_UPD_NUM_CTRL_PDU_CNT(status, am_ctrl_ptr) ()
#define RLC_AM_UPD_NUM_NEW_PDU_CNT(status, am_ctrl_ptr) ()
#endif

#define RLC_ENH_RETX_PDU_INFO_DUMP(num_pdu_built, rlc_flex_retx_pdu_info_ptr, temp_pdu_info_index) \
  do \
  { \
    MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,                                                       \
         "num_pdu %d - (SN, Size) (%d, %d) (%d, %d) (%d, %d) (%d, %d)",                        \
         num_pdu_built,                                                                        \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index].pdu_sn,            \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index].pdu_size_bits,     \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 1].pdu_sn,        \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 1].pdu_size_bits, \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 2].pdu_sn,        \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 2].pdu_size_bits, \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 3].pdu_sn,        \
         rlc_flex_retx_pdu_info_ptr->rlc_retx_pdu_info[temp_pdu_info_index + 3].pdu_size_bits  \
        );                                                                                     \
  } while (0);

#define RLC_UL_CHECK_RETX_DUE_TO_POLL(ctrl_blk, status_ptr) \
            (!(ctrl_blk->lc_state & UL_RESET_PENDING) && \
             /*lint +e655 */                             \
              !(rlci_chk_dl_reset_pending(ctrl_blk->ctrl.am_ctrl.am_common->dl_data_id)) && \
             (status_ptr->new_data_bytes == 0 || status_ptr->effect_win_size == 0) && \
             (FALSE == ctrl_blk->ctrl.am_ctrl.ul_state.retx_q_sema) &&  \
             status_ptr->retx_data_bytes == 0 && ctrl_blk->ctrl.am_ctrl.ul_state.poll_next && \
             !ctrl_blk->ctrl.am_ctrl.ul_state.poll_prohibit) 

#ifdef FEATURE_WCDMA_UL_COMPR

rlc_ctrl_pdu_type* rlc_ul_free_ul_comp_ctrl_dsm_pool(rlc_lc_id_type   rlc_id);

#define RACH_REPORT_UNCOMP_BO_AT_START(status_ptr, am_ctrl_ptr, query_from_bfi_flag)\
do\
{\
  if ((GET_UL_CCTRCH_TYPE == RACH) && (TVM_QUERY == query_from_bfi_flag))\
  {\
    if ((status_ptr->new_data_uncomp_bytes == 0) && (am_ctrl_ptr->ul_comp_sdu_bytes == 0))\
    {\
      num_tti_zero_bo_reported = 0;\
    }\
    else if ((num_tti_zero_bo_reported < num_tti_uncomp_report_zero_bo))\
    {\
      if (status_ptr->new_data_uncomp_bytes != 0)\
      {\
        status_ptr->new_data_uncomp_bytes = 0;\
        MSG_PTP_MED("RLC_PTP: Report %d Uncompressed data. Actual Wm Cnt %d num_tti_zero_bo_reported %d",\
                     status_ptr->new_data_uncomp_bytes, DSM_GET_WM_CURRENT_CNT(ctrl_blk->ul_wm_ptr),\
                     num_tti_zero_bo_reported);\
      }\
      num_tti_zero_bo_reported++;\
    }\
  }\
}\
while(0);

#define RLC_FREE_SDU_DSM(sdu_ptr,am_ctrl_ptr)\
do\
{\
  if (RLC_GET_UL_COMP_CTRL_SDU_PRESENT(sdu_ptr->app_field))\
  {\
    rlc_ul_free_ul_comp_ctrl_dsm_pool(am_ctrl_ptr->am_common->ul_data_id);\
  }\
  else\
  {\
    dsm_free_packet(&sdu_ptr);\
  }\
}\
while(0);

#else

#define RLC_FREE_SDU_DSM(sdu_ptr,am_ctrl_ptr) dsm_free_packet(&sdu_ptr)

#endif

#define RLC_UL_AM_SEND_SDU_CNF(am_ctrl_ptr,sdu_ptr,status,fail_reason) \
do\
{\
  if ((sdu_ptr = RLCI_Q_GET(&(am_ctrl_ptr->internal_q))) == NULL)\
  {\
     ERR_FATAL("RLC_ERR: RLC_ID %d Internal Q empty",am_ctrl_ptr->am_common->ul_data_id, 0, 0);\
  }\
  else\
  {\
    if ((RLC_UL_AM_SDU_TX_CNF(sdu_ptr->app_field)) && \
        (am_ctrl_ptr->sdu_cnf_cb != NULL))\
    {\
      am_ctrl_ptr->sdu_cnf_cb(am_ctrl_ptr->am_common->ul_data_id, status, \
                            RLC_UL_AM_GET_SDU_MUI(sdu_ptr->app_field), fail_reason);\
    }\
    RLC_FREE_SDU_DSM(sdu_ptr,am_ctrl_ptr);\
  }\
}while(0);

/* Macros needed for DSM Multiple EnQueue / Multiple DeQueue support */
#define RLCI_TOTAL_BYTES_UL_WM_PLUS_DEQUEUED(ul_ctrl_blk_ptr) (ul_ctrl_blk_ptr->ul_wm_ptr->current_cnt + ul_ctrl_blk_ptr->length_sdus_dequeued)

#define RLCI_TOTAL_SDUS_UL_WM_PLUS_DEQUEUED(ul_ctrl_blk_ptr)  (q_cnt(ul_ctrl_blk_ptr->ul_wm_ptr->q_ptr) + ul_ctrl_blk_ptr->num_sdus_dequeued)

#define RLC_NUM_SDUS_MULTIPLE_DEQUEUE 1
#define RLC_NUM_SDUS_MULTIPLE_DEQUEUE_DACC 25

/*===========================================================================
** Public Data Declaration                 
**=========================================================================*/


/*===========================================================================
** Public Function Declaration                 
**=========================================================================*/
 /*===========================================================================
 
 FUNCTION RLC_ENH_INITIALIZE_MUTEX
 
 DESCRIPTION   Initialize RLC Mutex
 
 DEPENDENCIES  None
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 
 ===========================================================================*/
 void rlc_enh_initialize_mutex(void);

/*===========================================================================

FUNCTION RLCI_UL_AM_INIT

DESCRIPTION
  Initialize the Up-link AM RLC.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_am_init(void);
/*===========================================================================

FUNCTION RLC_UL_AM_UPDATE_LI_LEN

DESCRIPTION
  This API updates the LI Length based upon the RRC configuration.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_update_li_len(rlci_ul_am_ctrl_type *am_ctl_blk_ptr , uint8 old_li_len);

/*===========================================================================

FUNCTION  RLCI_ESTABLISH_UL_AM

DESCRIPTION
  Config and establish an RLC AM in Uplink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
byte rlci_establish_ul_am
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
);

/*==========================================================================

FUNCTION RLCI_RE_ESTABLISH_UL_AM

DESCRIPTION
  Re-establish RLC AM entity in Uplink per RRC's request.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rlci_re_establish_ul_am
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
);

/*===========================================================================

FUNCTION RLCI_RELEASE_UL_AM

DESCRIPTION
  Release an RLC AM entity in Uplink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
byte rlci_release_ul_am
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr
);

/*===========================================================================

FUNCTION RLCI_MODIFY_UL_AM

DESCRIPTION
  Modify an RLC AM entity in Uplink per RRC's request.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_modify_ul_am
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  rlc_am_config_type *am_ptr
);

/*===========================================================================

FUNCTION RLCI_GET_AM_BUFFER_STATUS

DESCRIPTION
 This function gets the total number of bytes needed to be transmitted in 
 AM Uplink.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_get_am_buffer_status
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_lc_buf_status_type *status_ptr
);

/*===========================================================================

FUNCTION RLC_UL_BUILD_AM_PDUS

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI for AM RLC. 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_build_am_pdus 
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_logchan_format_type *chan_info,
  rlc_ul_logchan_data_type *data_ptr
);

/*===========================================================================

FUNCTION RLCI_RESET_UL_AM

DESCRIPTION
  Reset UL AM RLC. If the dl_reset_req is TRUE, post a RESET request message
  to the DL AM RLC as well.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_reset_ul_am
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  boolean dl_reset_req
);


/*===========================================================================

FUNCTION RLCI_AM_RESET

DESCRIPTION
  Reset RLC AM (UL&DL). This function is excuted in the task context.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_am_reset
(
  void
);

#ifdef FEATURE_WCDMA_HSUPA
/*===========================================================================

FUNCTION BUILD_STATUS_PDU

DESCRIPTION
  Builds status PDU for AM in Uplink.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *build_status_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  uint16  pdu_size                      /* PDU size in bits */
);

/*===========================================================================

FUNCTION BUILD_RETX_DATA_PDU

DESCRIPTION
  Builds rexmit data PDU for AM in Uplink.
  
  For NAK LIST type: The SN is the one that is missing. The distance is the
    additional number of missing PDUs following the SN.
  
  For NAK BITMAP type: The SN is the first bit in the bitmap.
  
DEPENDENCIES
  Assume that the PDU size is always bigger or exact the size of the retx PDU.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *build_retx_data_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_pdu_data_type  *pdu_data_ptr,
  uint16                 pdu_size,                   /* PDU size in bytes  */
  boolean                *error
);

/*===========================================================================

FUNCTION BUILD_NEW_DATA_PDU

DESCRIPTION
  Builds new data PDU for AM in Uplink.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *build_new_data_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_pdu_data_type  *pdu_data_ptr,
  uint16  pdu_size                     /* PDU size in bytes  */
);

/*===========================================================================

FUNCTION BUILD_TXD_DATA_PDU

DESCRIPTION
  Builds data PDU that is txd before. 
  
  When polling timer expired and there is no new data to go, a poll needs 
  to be sent the peer side for status report. We decide to send the latest
  sent PDU (VT_S -1) to remind the peer what is the max number of PDUs sent.
  
DEPENDENCIES
  Assume that the PDU size is always bigger or exact the size of the retx PDU.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *build_txd_data_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_pdu_data_type  *pdu_data_ptr,
  uint16  pdu_size                      /* PDU size in bytes  */
);
#endif //FEATURE_WCDMA_HSUPA

/*===========================================================================

FUNCTION  RLCI_UL_INIT_RESET_PARAM

DESCRIPTION
This function will initialize the uplink reset parameters to their default
values.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_init_reset_param
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr
);

/*===========================================================================

FUNCTION LOOK_FOR_1ST_EMPTY_BIT

DESCRIPTION
  Locate the first bit that is 0 in the passed-in bitmap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
uint8 look_for_1st_empty_bit(uint8 bitmap);

/*===========================================================================

FUNCTION RLC_ENH_UPDATE_RETX_BYTES

DESCRIPTION
  Updates the size of the pending to be re-transmitted for RLC AM mode entity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void rlc_enh_update_retx_bytes(rlci_ul_ctrl_blk_type *ctrl_blk, 
                               rlc_ul_lc_buf_status_type *status_ptr,
                               uint16     max_sn,
                               rlc_ul_bo_query_source  query_from_bfi_flag);

#ifdef FEATURE_MAC_I
/*===========================================================================

FUNCTION RLC_ENH_UPDATE_RETX_BYTES_DUE_TO_POLLING

DESCRIPTION
  Updates the size of the Polling PDU that should be re-transmitted for RLC AM mode entity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void rlc_enh_update_retx_bytes_due_to_polling(rlci_ul_ctrl_blk_type *ctrl_blk, 
                                              rlc_ul_lc_buf_status_type *status_ptr,
                                              rlc_ul_bo_query_source  query_from_bfi_flag,
                                              uint16 sn);
#endif

/*===========================================================================

FUNCTION RLCI_ENH_GET_AM_BUFFER_STATUS

DESCRIPTION
 This function gets the total number of bytes needed to be transmitted in
 AM Uplink.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_get_am_buffer_status
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_lc_buf_status_type *status_ptr,
  rlc_ul_bo_query_source    query_from_bfi_flag,
  boolean                   *rlc_wait_for_ack	 
);

/*===========================================================================

FUNCTION RLC_UL_ENH_PRINT_RLC_HDR

DESCRIPTION

  This prints the updated RLC header information.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_print_rlc_hdr(rlc_ul_hdr_buff_type    *ul_hdr_ptr);
#ifdef FEATURE_MAC_I
/*===========================================================================

FUNCTION RLC_ENH_BUILD_PARTIAL_UPDATE_CIPH_PARAMS

DESCRIPTION

  This API updates the ciphering parameters for the Partial PDU's.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_build_partial_update_ciph_params(rlci_ul_ctrl_blk_type       *ctrl_blk,  
                                                     rlc_ul_logchan_format_type  *chan_info);

/*===========================================================================

FUNCTION RLC_ENH_BUILD_PARTIAL_PDU_END

DESCRIPTION

  This API updates the PDU building IOVEC, ciperhing related parameters for the Partial end PDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_build_partial_pdu_end(rlci_ul_ctrl_blk_type       *ctrl_blk,  
                                   rlc_ul_logchan_format_type  *chan_info, 
                                   l1_l2_data_type             *l1_ul_tx_buf_ptr,
                                   uint16                       src_desc_idx);
/*===========================================================================

FUNCTION RLC_ENH_BUILD_PARTIAL_PDU_MIDDLE

DESCRIPTION

  This API updates the PDU building IOVEC, ciperhing related parameters for the Partial middle PDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_build_partial_pdu_middle(rlci_ul_ctrl_blk_type 	  *ctrl_blk,  
                                      rlc_ul_logchan_format_type  *chan_info, 
                                      l1_l2_data_type             *l1_ul_tx_buf_ptr,
                                      uint16                      src_desc_idx);
/*===========================================================================

FUNCTION RLC_ENH_BUILD_PARTIAL_PDU_START

DESCRIPTION

  This API updates the PDU building IOVEC, ciperhing related parameters for the Partial start PDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_build_partial_pdu_start(rlci_ul_ctrl_blk_type       *ctrl_blk,  
                                         rlc_ul_logchan_format_type  *chan_info, 
                                         l1_l2_data_type             *l1_ul_tx_buf_ptr,
                                         uint16                       src_desc_idx);

/*===========================================================================

FUNCTION RLC_ENH_BUILD_PARTIAL_PDU_START

DESCRIPTION

  This API builds the remaining partial PDU(PM or PE) and updates the IOVEC, ciphering parameters.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if PDU building succeds else FALSE.
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean  rlc_enh_build_partial_middle_end(rlc_ul_logchan_format_type  *chan_info,
                                         rlci_ul_ctrl_blk_type       *ctrl_blk,
                                         l1_l2_data_type            *l1_ul_tx_buf_ptr
                                         #ifdef FEATURE_RLC_QXDM_LOGGING
                                         , boolean                     log_allowed
                                         #endif
                                        );
#endif
/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_AM_PDUS

DESCRIPTION

  This prepares the new data, re-tx, ctrl pdu for a AM LC mapped to a
  E-DCH transport channel.

  PP.SW:
  RLC PDU payloads are provided as a set of IO vectors.  
  RLC HDR(+LIs) are stored in a header buffer as word format.
  
  7200A:
  RLC HDR(+LIs), RLC PDU payloads are all provided as set of IO vectors.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_build_am_pdus
(
  rlci_ul_ctrl_blk_type       *ctrl_blk,  /* Pointer to ctrl_blk           */
  rlc_ul_logchan_format_type  *chan_info, /* Pointer to channel information*/
  l1_l2_data_type            *l1_ul_tx_buf_ptr
#ifdef FEATURE_RLC_QXDM_LOGGING
  , boolean                    log_allowed
#endif  
                                          /* Place to store IO vectors     */
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NON_EDCH_AM_PDUS

DESCRIPTION

  This prepares the new data, re-tx, ctrl pdu for a AM LC.

  The set of io vectors formed will be copied into a pre-allocated PDU
  dsm item. This dsm is passed to the lower layers.
  
DEPENDENCIES
  None.
  
RETURN VALUE

  uint16 - Number of PDUs prepared.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_enh_build_non_edch_am_pdus
(
  rlci_ul_ctrl_blk_type       *ctrl_blk,  /* Pointer to ctrl_blk           */
  rlc_ul_logchan_format_type  *chan_info, /* Pointer to channel information*/
  rlc_ul_logchan_data_type    *data_ptr   /* Pointer to data information   */
);


/*===========================================================================

FUNCTION RLC_UL_AM_FLUSH_INTERNAL_Q

DESCRIPTION

  This flushes the internal Q and sends a SDU confimation.
  
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_flush_internal_q
(
  rlci_ul_am_ctrl_type          *am_ctrl_ptr, /* pointer to am_ctrl   */
  rlc_sdu_tx_fail_reason_e_type reason        /* SDU cnf reason       */
);

/*===========================================================================

FUNCTION RLC_UL_AM_SEND_SDU_CNF

DESCRIPTION

  This sends the SDU cnf to the upper layers with in the SN 
  old_vt_a to vt_a .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_send_sdu_cnf
(
  rlci_ul_am_ctrl_type          *am_ctrl_ptr, /* Pointer to am_ctrl */
  uint16                        prev_vt_a,    /* Start SN           */
  uint16                        new_vt_a,     /* End SN             */
  uecomdef_status_e_type        status,       /* success/failue     */
  rlc_sdu_tx_fail_reason_e_type fail_reason   /* SDU cnf reason     */
);

/*===========================================================================

FUNCTION RLCi_DUMP_PDU_DATA

DESCRIPTION

  Dumps the enitre PDU contents
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dump_pdu_data
(
  l1_l2_data_type  *l1_ul_tx_buf_ptr, 
  uint16            src_desc_idx
);

/*===========================================================================

FUNCTION RLCI_ENH_GET_DATA_NUM_BYTES

DESCRIPTION
  This function reports the total number of data bytes that need to be
  transmitted by RLC. This includes new data and retx data.
  If RLC_NO_DISCARD is configured, then there is a check to see if
  maxdat is going to be hit for VT_A or VT_S. If number of re-transmissions
  of VT_A is maxdat -1 and there is a NAK for VT_A sitting in the nak
  buffer,then initate a reset.However, if maxreset is set to 1, then post a
  reset failure to RRC. VT_S is re-transmitted when there is no other data
  to send and polling is triggered. If VT_S hits maxdat -1 and polling is
  triggered, initiate a reset if maxreset is greater than 1 otherwise
  post a RRC reset failure.This is to prevent an extra padding PDU from
  being sent since reset takes place in the task context and the reset PDU
  goes out in the next TTI. However, this does not address the problem
  yet for MRW procedure. If MRW is triggered on hitting maxdat,a padding
  PDU goes out in that TTI and MRW SUFI is sent in the next TTI.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Can cause a potential problem when MRW is configured since a padding
  PDU goes out on hitting maxdat before a MRW SUFI is sent in the next TTI.

===========================================================================*/
void rlci_enh_get_data_num_bytes
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_lc_buf_status_type *status_ptr,
  rlc_ul_bo_query_source query_from_bfi_flag,
  boolean                *rlc_wait_for_ack   
);

/*===========================================================================

FUNCTION RLCI_ENH_GET_STATUS_NUM_BYTES

DESCRIPTION
 This function gets the total number of bytes needed to be transmitted in
 AM Uplink.

 Priorities of status SUFIs:
 1. RESET/RESET ACK PDUs.
 2. MRW/MRW_ACK SUFIs.
 3. NAK SUFIs.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  rlci_enh_get_status_num_bytes
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  rlc_ul_lc_buf_status_type  *status_ptr,
  rlc_ul_bo_query_source     query_from_bfi_flag
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_PADDING_ACK_PDU

DESCRIPTION

  Builds ACK status PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean  rlc_ul_enh_build_padding_ack_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,    /* Pointer to thectrl_blk         */
  uint16                pdu_size,         /* PDU size in bits               */
  l1_l2_data_type      *l1_ul_tx_buf_ptr /* Place to store Io vectors      */
 #ifdef FEATURE_RLC_QXDM_LOGGING
 , boolean  log_allowed
 #endif  
 #ifdef FEATURE_MAC_I
  , mac_i_seg_status        mac_segment_needed
#endif
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NON_EDCH_PADDING_ACK_PDU

DESCRIPTION

  Builds ACK status PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_build_non_edch_padding_ack_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,    /* Pointer to thectrl_blk         */
  uint16                pdu_size,         /* PDU size in bits               */
  rlc_ul_pdu_data_type  *data_ptr         /* Valid if chnl_type = NON_E_DCH */  
);
#ifdef FEATURE_MAC_I
/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_POLL_SUFI

DESCRIPTION

 This API to update Poll sufi control PDU.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_build_poll_sufi(rlci_ul_ctrl_blk_type *ctrl_blk_ptr,
                                dsm_item_type         *pdu_ptr,
                                uint16                *bit_offset);
#endif
/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_CTRL_PDU

DESCRIPTION

  This prepares the ctrl pdu for a particular AM LC.

  The control PDU is prepared using a pre-allocated ctrl_pdu_dsm_item.
  ctrl_pdu_dsm_item is shared by all AM LC's who ever wants to prepare 
  control PDU in this TTI. If the 1st item is exhanused, control PDUs
  are built uisng the second item.

  E_DCH:
  The control PDU is provided as single IO vector.
  
  NON_E_DCH:
  The single io vector formed will be copied into a pre-allocated PDU
  dsm item. This dsm is passed to the lower layers.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_ctrl_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,    /* pointer to uplink control     */
  uint16                pdu_size,         /* pdu size in bits              */
  boolean               first_status_pdu, /* TRUE - 1st status PDU for this*/ 
                                          /* LC. FALSE - other wise        */
  l1_l2_data_type      *l1_ul_tx_buf_ptr
                                          /* Place to store IO vectors     */
 #ifdef FEATURE_RLC_QXDM_LOGGING
  , boolean log_allowed
 #endif
 #ifdef FEATURE_MAC_I
 , boolean copy_to_mac_i_ctrl_ptr
 #endif
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NON_EDCH_CTRL_PDU

DESCRIPTION

  This prepares the ctrl pdu for a particular AM LC.

  The control PDU is prepared using a pre-allocated pdu_dsm_pool.
  The prepared dsm_item is paased to the lower layers.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_non_edch_ctrl_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,    /* pointer to uplink control     */
  uint16                pdu_size,         /* pdu size in bits              */
  boolean               first_status_pdu, /* TRUE - 1st status PDU for this*/ 
                                          /* LC. FALSE - other wise        */
  rlc_ul_pdu_data_type  *data_ptr
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_RETX_DATA_PDU

DESCRIPTION

  This builds the re-tx PDU using the stored book-keeping information.

  PP.SW:
  RLC PDU payloads are provided as a set of IO vectors.  
  RLC HDR(+LIs) are stored in a header buffer as word format.
  
  7200A:
  RLC HDR(+LIs), RLC PDU payloads are all provided as set of IO vectors.

DEPENDENCIES
  None.
  
RETURN VALUE
  boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_retx_data_pdu
(
  rlci_ul_ctrl_blk_type     *ctrl_blk,  /* pointer to uplink control      */
  uint16                    retx_sn,    /* re-tx PDU SN */
  uint16                    pdu_size,   /* pdu size in bits               */
  l1_l2_data_type          *l1_ul_tx_buf_ptr /* Place to store IO vectors      */
#ifdef FEATURE_RLC_QXDM_LOGGING
  , boolean                 log_allowed
#endif   
#ifdef FEATURE_MAC_I
  , mac_i_seg_status   mac_segment_needed
#endif
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NON_EDCH_RETX_DATA_PDU

DESCRIPTION

  This builds the re-tx PDU using the stored book-keeping information.

  NON_E_DCH:
  The set of io vectors formed will be copied into a pre-allocated PDU
  dsm item. This dsm is passed to the lower layers.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_non_edch_retx_data_pdu
(
  rlci_ul_ctrl_blk_type     *ctrl_blk,  /* pointer to uplink control      */
  uint16                    retx_sn,    /* re-tx PDU SN */
  uint16                    pdu_size,   /* pdu size in bits               */
  rlc_ul_pdu_data_type      *data_ptr   /* Place to store prepared PDUS   */
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NEW_DATA_PDU

DESCRIPTION

  This builds the New PDU using the stored book-keeping information.

  PP.SW:
  RLC PDU payloads are provided as a set of IO vectors.  
  RLC HDR(+LIs) are stored in a header buffer as word format.
  
  7200A:
  RLC HDR(+LIs), RLC PDU payloads are all provided as set of IO vectors.

DEPENDENCIES
  None.
  
RETURN VALUE
  boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_new_data_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,        /* pointer to uplink control      */
  uint16                pdu_size,         /* PDU size in bits               */
  l1_l2_data_type      *l1_ul_tx_buf_ptr /* Valid if chnl_type = E_DCH     */ 
#ifdef FEATURE_RLC_QXDM_LOGGING
   , boolean log_allowed
#endif   
#ifdef FEATURE_MAC_I
  , mac_i_seg_status   mac_segment_needed
#endif
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_NON_EDCH_NEW_DATA_PDU

DESCRIPTION

  This builds the new PDU using the stored book-keeping information.

  NON_E_DCH:
  The set of io vectors formed will be copied into a pre-allocated PDU
  dsm item. This dsm is passed to the lower layers.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE/FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_enh_build_non_edch_new_data_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,        /* pointer to uplink control      */
  uint16                pdu_size,         /* PDU size in bits               */
  rlc_ul_pdu_data_type  *data_ptr         /* Place to store prepared PDUS   */  
);

/*===========================================================================

FUNCTION RLCI_RESET_STATISTICS_VARIABLES

DESCRIPTION
  The function resets the number of mrw acks sent and number of reset acks
  sent out. These variables are used for reporting events for logs.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Only no_mrw_ack_sent, no_reset_sent and no_reset_ack_sent are reset.

===========================================================================*/

void rlci_reset_statistics_variables
( void
);

/*===========================================================================

FUNCTION RLCI_GET_RETX_COUNT_C

DESCRIPTION
  Obtain the Count_c of ciphering for this SN.

  Use act_old_cfg and act_sn to determine which config to use. (old or
  current)

  If there is a wrap around, the real distance between the tx_sn and
  the VT_S will be greater than 2048.

  It assumes that tx_sn is always less than VT_S, since the tx_sn is
  the SN for a retx PDU.

DEPENDENCIES
  None.

RETURN VALUE
  Count_c for ciphering.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_get_retx_count_c
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  uint16 tx_sn,
  uint32                *key_index,
  boolean               *enabled,
  uecomdef_wcdma_cipher_algo_e_type *cipher_algo,
  uint32                *cnt_c  
);

/*===========================================================================

FUNCTION RLCI_DISCARD_SDU_IN_WM

DESCRIPTION
  Remove the unfinished SDU from WM.
  This function will be called when performing RESET or generating MRW.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_discard_sdu_in_wm
(
  rlci_ul_ctrl_blk_type *ctrl_blk
);

/*===========================================================================

FUNCTION RLCI_RESET_UL_AM_CTL_BLK

DESCRIPTION
  Reset the UL AM RLC control block.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_reset_ul_am_ctl_blk
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  boolean IsRelease                    /* TRUE for the reset being asked  */
                                       /* by the release procedure.       */
);

/*===========================================================================

FUNCTION BUILD_RESET_PDU

DESCRIPTION
  Builds RESET PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 build_reset_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  dsm_item_type *pdu_ptr,
  uint16 bit_offset,
  uint16 pdu_size                             /* PDU size in bits */
);

/*===========================================================================

FUNCTION BUILD_NAK_SUFI

DESCRIPTION
  Builds NAK PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 build_nak_sufi
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  dsm_item_type *pdu_ptr,
  uint16 bit_offset,
  uint16 pdu_size,                         /* PDU size in bits */
  boolean *nak_built, 
  uint16 *first_nak_sn,
  uint16 curr_pdu_bit_offset               /* PDU start offset inside the DSM item */
);

/*===========================================================================

FUNCTION BUILD_MRW_SUFI

DESCRIPTION
  Builds MRW SUFI in a PDU for AM in Uplink.
  We only send one MRW SUFI at a time and wait until rx'd the corresponding
  ACK of this MRW from the peer side.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 build_mrw_sufi
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  dsm_item_type *pdu_ptr,
  uint16 bit_offset,
  uint16 pdu_size,                    /* PDU size in bits */
  uint16 curr_pdu_bit_offset          /* PDU start offset inside the DSM item */
);

/*===========================================================================

FUNCTION BUILD_MRW_ACK_SUFI

DESCRIPTION
  Builds NAK PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 build_mrw_ack_sufi
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  dsm_item_type *pdu_ptr,
  uint16 bit_offset,
  uint16 pdu_size,                      /* PDU size in bits */
  uint16 curr_pdu_bit_offset            /* PDU start offset inside the DSM item */
);

/*===========================================================================

FUNCTION UPDATE_BITMAP

DESCRIPTION
  Set the first 0 bit to be 1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 update_bitmap(uint8 bitmap);

/*===========================================================================

FUNCTION RESET_RETX_SIZE

DESCRIPTION
  Reset retx_size if the rtx_q is empty.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void reset_retx_size
(
  rlci_ul_ctrl_blk_type *ctrl_blk
);

/*===========================================================================

FUNCTION UPDATE_RETX_Q

DESCRIPTION
  When received an ACK, look into the rtx_q to remove all the NAK requests
  that is ACKed now. Update the number of bytes for retransmission.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void update_retx_q
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  uint16  ack_sn
);

/*===========================================================================

FUNCTION GENERATE_MRW

DESCRIPTION
  Discard PDUs associated w/ an SDU. Generate MRW for this discarding.
  Enqueue this MRW into mrw_q for next TTI to tx.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void generate_mrw
(
  rlci_ul_ctrl_blk_type *ctrl_blk,
  uint16                rlc_sn,
  boolean               discard_first_sdu_only
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  ,boolean               task_context_flg
#endif
);


/*===========================================================================

FUNCTION UPDATE_MRW_Q

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void update_mrw_q
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  uint16  ack_sn
);

/*===========================================================================

FUNCTION REPORT_SDU_TX_FAILURE

DESCRIPTION
  Frees PDUs in txq from vt_a to mrw_sn. Markes the SDU that has been
  reported to the upper layer.
  Updates VT(A) and VT(MS) w/ mrw_sn.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void report_sdu_tx_failure
(
  rlci_ul_ctrl_blk_type *ctl_blk_ptr,
  uint16  mrw_sn,
  uint8   mrw_n
);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_UL_STORE_AM_PDU_LOG_DATA

DESCRIPTION

 Stores the information for AM PDU log packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_store_am_pdu_log_data
(
  rlci_ul_am_ctrl_type  *am_ctrl_ptr,
  uint32                log_data,
  uint16                pdu_size,
  uint32                count_c
);

/*===========================================================================

FUNCTION RLC_UL_STORE_AM_CIPHER_LOG

DESCRIPTION

 Stores the information for AM PDU cipher log packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_store_am_cipher_log
(
  rlci_ul_am_ctrl_type    *am_ctrl_ptr,
  uint32                  key_id,
  uint32                  cnt_c,
  uecomdef_wcdma_cipher_algo_e_type  cipher_algo,
 // rlc_cipher_algo_e_type  cipher_algo,
  uint16                  rlc_sn
);
#endif
/*===========================================================================

FUNCTION RLC_UL_ENH_AM_ALLOCATE_CTRL_DSM

DESCRIPTION

 This API allocates memory for building AM control PDU.

DEPENDENCIES
  None.

RETURN VALUE
  NULL - if memory not found otherwise a valid memory region.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ctrl_pdu_type *rlc_ul_enh_am_allocate_ctrl_dsm (uint16 pdu_size);

/*===========================================================================

FUNCTION RLC_UL_GET_CTRL_DSM_POOL

DESCRIPTION

  Get one DSM item from the control PDU DSM pool.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ctrl_pdu_type* rlc_ul_get_ctrl_dsm_pool(void);

/*===========================================================================

FUNCTION RLC_UL_AM_BUILD_ACK_SUFI_CTRL_PDU

DESCRIPTION

 This  API builds ACK control PDU.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_build_ack_sufi_ctrl_pdu(uint16        ack_sn,   
                                       uint16        *bit_offset,
                                       dsm_item_type *pdu_ptr);

/*===========================================================================

FUNCTION  RLC_BUILD_PADDING_ACK_PDU

DESCRIPTION

  Builds Padding ACK PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean: TRUE  - PDU prepared
           FALSE - PDU not prepared.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_build_padding_ack_pdu
(
  rlci_ul_am_ctrl_type  *am_ctrl_ptr,
  uint16                pdu_size,
  uint16                bit_offset,
  dsm_item_type         *pdu_ptr 
);

/*===========================================================================

FUNCTION  RLC_BUILD_CTRL_PDU

DESCRIPTION

  Builds Control PDU for AM in Uplink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_build_ctrl_pdu
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,    /* Poninter to control block     */
  uint16                pdu_size,         /* PDU size in bits              */
  dsm_item_type         *pdu_ptr,         /* Dsm where control PDU is built*/
  uint16                bit_offset,       /* Bit offset with in pdu_ptr    */
  boolean               first_status_pdu  /* TRUE - 1st status PDU for this*/ 
                                          /* LC. FALSE - other wise        */
);

/*===========================================================================

FUNCTION RLC_UL_AM_GET_RETX_PDU_CIPHER_PARAMS

DESCRIPTION

  This computes the ciphering parameters for the re-tx SN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_get_retx_pdu_cipher_params
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* Pointer to ctrl_blk        */
  uint16                retx_sn,        /* re-tx SN                   */
  uint32                *key_index,     /* Key id                     */
  boolean               *enabled,       /* TRUE - ciphering activated */
                                        /* FALSE - Otherwise          */
  uecomdef_wcdma_cipher_algo_e_type *cipher_algo,
  uint32                *cnt_c          /* COUNT_C                    */
);

/*===========================================================================

FUNCTION RLC_UL_AM_GET_NEW_PDU_CIPHER_PARAMS

DESCRIPTION

  This computes the ciphering parameters for the new DATA PDU.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
//uecomdef_wcdma_cipher_algo_e_type rlc_ul_am_get_new_pdu_cipher_params
uecomdef_wcdma_cipher_algo_e_type rlc_ul_am_get_new_pdu_cipher_params
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* Pointer to ctrl_blk        */
  uint32                *key_index,     /* Key id                     */
  boolean               *enabled,       /* TRUE - ciphering activated */
                                        /* FALSE - Otherwise          */
  uint32                *cnt_c          /* COUNT_C                    */
);

/*===========================================================================

FUNCTION RLC_UL_ENH_POLL_TIMER_TRIGGER

DESCRIPTION

 This API is used to check and reset the configured polling related timer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_poll_timer_trigger
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr , 
  uint16 sn
);

/*===========================================================================

FUNCTION RLC_UL_AM_COMPUTE_POLL

DESCRIPTION

  Compute whether polling needs to be set in this PDU or not.

DEPENDENCIES
  None.

RETURN VALUE
 boolean - TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_am_compute_poll
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* Pointer to ctrl_blk      */
  uint16                sn,             /* SN of the PDU            */
  rlc_am_pdu_e_type     pdu_type        /* re-tx, control, new PDU  */        
#ifdef FEATURE_MAC_I
 , mac_i_seg_status   mac_segment_needed
#endif
);


/*===========================================================================

FUNCTION RLC_UL_AM_CHECK_SDU_DISCARD

DESCRIPTION

  Validates if this PDU hits MAX_DAT and initiates MRW procedure.

DEPENDENCIES
  None.

RETURN VALUE

  boolean: TRUE  - PDU should be discarded
           FALSE - PDU is valid
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_am_check_sdu_discard
(
  rlci_ul_ctrl_blk_type   *ctrl_blk_ptr,
  uint16                  retx_sn
);

/*===========================================================================

FUNCTION RLC_UL_AM_UPD_RETX_INFO

DESCRIPTION

 This API is used to check the validity of the PDU to be retransmitted and if valid update
 the correponding PDU information..

DEPENDENCIES
  None.

RETURN VALUE
  Valid SN value if PDU is valid or returns RLC_INVALID_SN(0xFFFF)- 

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_am_upd_retx_info
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* pointer to uplink control  */
  uint16                  retx_sn
);

/*===========================================================================

FUNCTION RLC_UL_AM_GET_RETX_SN

DESCRIPTION

  This will compute the next re-tx SN to be transmitted.
  The total_retx_size is also updated accordingly.
  If this SN hits MAX_DAT, and discard mode = RLC_MAXDAT, 
  then MRW is generated.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  uint16 - re-tx SN
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_am_get_retx_sn
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,  /* pointer to uplink control  */
  uint16                pdu_size,        /* PDU size in bits           */
  rlci_missing_list_type  **retxq_next_ptr
);

/*===========================================================================

FUNCTION RLC_UL_AM_FORM_RE_TX_PAYLOAD

DESCRIPTION

  This builds the payload of re-tx PDU using the stored book-keeping
  information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_form_re_tx_payload
(
  rlci_ul_ctrl_blk_type *ctrl_blk,        /* Pointer to ctrl_blk          */
  uint16                retx_sn,          /* re-tx SN                     */
  rlc_buff_type         *hdr_list,        /* Local place to store headers */
  l1_l2_data_type      *l1_ul_tx_buf_ptr /* Place to store IO vectors    */
);

/*===========================================================================

FUNCTION RLC_UL_AM_UPDATE_SPECIAL_LI_FIELD

DESCRIPTION

API to update specila LI field in RLC header.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
static inline void rlc_ul_am_update_special_li_field(uint8                li_len,
                                              uint16               rem_pdu_size,
                                              boolean              *octet_less_flag,
                                              rlc_buff_type        *hdr_list,
                                              rlc_am_pdu_info_type *pdu_info);

boolean rlc_ul_am_form_new_pdu_payload
(
  rlci_ul_ctrl_blk_type *ctrl_blk,        /* Pointer to ctrl_blk          */
  uint16                 pdu_size_bytes,  /* PDU size in bytes            */  
  rlc_buff_type         *hdr_list,        /* Local place to store headers */
  l1_l2_data_type      *l1_ul_tx_buf_ptr /* Place to store IO vectors    */
);

void rlc_ul_am_update_li_len(rlci_ul_am_ctrl_type *am_ctl_blk_ptr , uint8 old_li_len);


void rlc_ul_enq_sdu_len
(
#ifdef FEATURE_DSM_WM_CB
  dsm_watermark_type* wm_ptr,
  void*               func_data
#else
  void
#endif
);
 
dsm_item_type *rlc_ul_am_dsm_dequeue(rlci_ul_ctrl_blk_type *ctl_blk_ptr);

 void rlc_ul_deq_sdu_len
 (
    dsm_watermark_type* wm_ptr,
    void*               func_data
 );


void rlc_ul_sdu_len_q_check_valid
(
  dsm_watermark_type* wm_ptr,
  rlc_ul_sdu_len_q_info *rlc_ul_sdu_q
);

uint16 rlc_ul_read_enq_sdu_len(rlc_ul_sdu_len_q_info *rlc_ul_sdu_q, uint16 *deq_idx, uint16 *num_sdu);
uint16 rlc_ul_am_get_deq_sdu_len(rlc_ul_sdu_len_q_info *rlc_ul_sdu_len_q_ptr, 
                                       uint8                 *num_sdu,
                                       boolean               *last_sdu_flg);

rlc_ul_sdu_len_q_info *rlc_ul_read_sdu_deq_idx(rlc_lc_id_type rlc_id);

void rlc_ul_am_update_li_len(rlci_ul_am_ctrl_type *am_ctl_blk_ptr , uint8 old_li_len);

#ifdef FEATURE_WCDMA_UL_COMPR
void rlc_ul_am_comp_dsm_alloc(void);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION  RLC_UL_COMP_SAVE_LOG_PARAMS

DESCRIPTION
  Populates the global associated with RLC ULC log packet

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_comp_save_log_params(
  uint8 comp_isr_late,
  uint32 num_miss_comp,
  uint8 num_sdu_comp,
  uint16 num_comp_bytes, 
  uint16 num_uncomp_bytes,
  uint32 num_ticks_taken,
  uint8 num_uncomp_pkts,
  uint8  limits_reached
);
#endif

#endif

#endif
