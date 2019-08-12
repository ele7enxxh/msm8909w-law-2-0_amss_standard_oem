#ifndef RLCDL_H
#define RLCDL_H
/*===========================================================================
                  D O W N L I N K  R L C  I N T E R N A L

                         H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  downlink RLC module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlcdl.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/15  scv     Prevent taking DACC Mutex after EUL ISR Mutex in Task context
10/12/15  scv     Changes for DSM Multiple DeQ/EnQ
01/12/15  kv       Replace __FILE__ with __FILENAME__ 
10/17/14  ts       Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
08/21/14  ts       FR22375: Fixed Medium compiler warnings in WCDMA on Bolt 2.0/2.1
01/09/14  scv      Code changes for UL Compression Feature
05/08/13  scv      Fix L2 Compilation Errors
05/03/13  as       Change to discard DL PDUs if PDU health passed by decoder is false
01/21/13  as       Added code check to enqueue DL data to Watermark queue only 
                   if its length is less than 1503 bytes.
01/17/13  as       Removed unwanted F3's.
01/04/13  kc       Prototypes for ZI reduction function
05/24/12  kc       Added changes for QCHAT HFN mismatch error recovery
07/13/12  grk      MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/24/12  geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12  kc       Moving WM deregistration prototypes to rlcrrcif.h
02/01/12  grk      Feature cleanup
12/06/10  grk      Added periodic PS<->RLC DL WM traces at RLC
10/11/11  grk      Fix to move PDU discarding(when WM is not registered) for TM
                   PDUs from ISR to task context.
08/17/11  grk      Fix to trigger UE initiated RESET if HFN value in RESET/RESET_ACK
                   received from NW is lower than the one currently configured.
03/02/11  kvk      Added support for PDU/SDU print and also Invalid SDU detection >1500
09/30/10  pj       moved enum definitions to rlci.h from rlcdl.h to avoid mob error 
09/20/10  pj       Added support to log erroneous conditions
07/07/10  pj       Added changes to use a global base timer for status processing
06/28/10  pj       Added changes for ZI memory reduction.
05/12/10  grk      Reorganized part of code under FEATURE_INACTIVITY_HANDLING.
05/06/10  pj       Added function prototypes 
04/13/10  grk      changes for ZI memory reduction.
12/07/09  grk      Changes to return valid deciphering parameters for PDUs 
                   received before receiving RESET_ACK (in case DL is in 
                   RESET_PENDING due to UE initiated RESET) and drop PDUs received 
                   after receiving RESET_ACK till DL comes out of RESET_PENDING.
10/21/09  grk      1.Added new DL state variable to keep track of SN till which
                     DL status array is updated dl_vr_h_status. 
                   2.Fix to use vr_r as last NAK SN in case dl_vr_h_status is less 
                     than vr_r.
08/13/09   grk     Changes for bitmap based status tracking.
04/13/09   pj      Added support for CS-over-HS
06/10/09   grk     Changes for ZI memory reduction on DL.
06/12/09   av      Added feature to genrate RLC event during DL config
05/29/09   ssg     Added support for SNOW_3G algo.
05/08/09   av      Enhancement to detect spurios reset pdu in srb 
05/11/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
01/28/09   grk     Merged missed out fixes from taxis to mainline.
01/22/09   grk     Merged L2 mutex changes from taxis to mainline.
03/03/08   ssg     Added support to alternate e-bit interpretations under the
                   featue flag FEATURE_RLC_ALTERNATE_E_BIT.
12/19/08   ssg     Added prototype for rlc_enh_dl_data_reassembly_signal().
12/17/08   ksr     PDU boundary check while counting LI in PDU
12/05/08   grk     compilation fix for 8200 (rel7).
11/22/08   kvk     Now sharing Mutex with L1 for all L2 to avoid memory concurrency issues
11/21/08   kvk     rlci_get_li_count() will now take pdu_len to check PDU validity                  
11/13/08   pj      Moved prototype for rlci_chk_dl_reset_pending() to rlci.h
10/23/08   pj      Added prototype for rlci_chk_dl_reset_pending() function 
10/17/08   grk     Merged taxis changes to mainline.
09/02/08   cnp     Replaced intlock/free with wcdma_intlock/free.
           cnp     Replaced tasklock/free with wcdma_tasklock/free.
           cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.

05/20/08   kvk     Added dl_ctl_cnfg_local and am_common_config to store the
                   config locally
05/19/08   kvk     Moved RLCI_GET_CTRL_BLK_INDEX and RLCI_GET_BLK_PTR out of 
                   FEATURE_WCDMA_DL_DATA_PATH_OPT feature
05/16/08   ssg     Added prototype for crlc_dl_continue_handler().
04/25/08   ksr     app_field is already discounted while putting in PDU into SDU pointer. 
                   While discarding SDU, discard mechanism is updated to discard only 
                   when app_field is 0
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
           vg      1. Fixed issues during DL integration testing
           ar      2. a. Added macros for enabling/disabling UM user plane log packets
                         feature FEATURE_WCDMA_DL_DATA_PATH_OPT
                      b. Modified macro RLCI_GET_AM_LOGGING_CONFIG under feature
                         FEATURE_RLC_QXDM_LOGGING
02/01/08   pj      Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
11/30/07   ssg     Added mbms support under FEATURE_MBMS.
01/11/07   vg      added UM PDU logging structure in UM ctrl type
01/04/07   ssg     Added rlc_dl_log_cipher_pkt() function prototype.
11/10/06   vg      added prototype of WM clenup function
05/17/06   ssg     Added the variable cur_status_periodic in the struct
                   rlci_dl_am_state_type
08/15/05   mv      Added the variable report_vr_h in the struct
                   rlci_dl_am_state_type under the feature
                   FEATURE_RLC_REPORT_VR_H_ACK_SN.
05/31/05   mv      Defined RLC_DL_AM_USR_PLANE_START_LC_ID as the start for
                   user/data plane AM logical channels.
05/03/05   mv      Added the function definition for
                   crlc_dl_abort_cipher_config.
                   Added the parameter cipher_backup to the structure
                   rlci_dl_ctrl_blk_type.
5/01/05    mv      Fixed a bug in GET16 macro.
04/05/05   aw      Removed Log_on_demand feature.
03/07/05   mv/aw   Merged the HSDPA changes to mainline.
02/09/05   aw      Converted get16 function to GET16 macro.
1/27/05    mv      Removed the include file
06/08/04   mv      Increased the max receive window size from 768 to 2048.
06/02/04   mv      Fixed lint errors and warnings.
01/20/04   mv      Replaced the FEATURE_RLC_ENHANCED_CB with the
                   FEATURE_RLC_SDU_RCVD_ENHANCED_CB.
12/29/03   vsk     Replace FEATURE_MULTIPLE_PDP with FEATURE_RLC_ENHANCED_CB
12/06/03   ttl     Added data structure for PDU ciphering parameters logging.
10/14/03   mv      Added a variable rx_win to indicate the receiver's window
                   size on the downlink.
09/23/03   mv      Added a third parameter to the rlc_post_rx_func_ptr()
                   to support multiple PDP contexts under the feature
                   FEATURE_MULTIPLE_PDP.
09/04/03   ttl     Added fst_rx_pdu.
03/28/03   ttl     Changed UE_MAX_DL_RLC_ENTITY definition to be
                   UE_MAX_DL_LOGICAL_CHANNEL.
02/14/03   ttl     Removed duplicated declaration of rlc_dl_register_srvc_handler().
                   Removed rlc_dl_pdu_data_ind() definition due to the duplication
                   in macrlcif.h.
01/14/03   ttl     Added nak_list_q in rlci_dl_am_ctrl_type.
12/10/02   ttl     Added backup_timer_cnt for backup timer.
                   Added function declaration of rlc_dl_am_backup_time().
11/13/02   ttl     Added next_to_vr_r.
11/05/02   ttl     Added peer_reset_sn to rlci_dl_am_state_type.
10/25/02   ttl     Added FEATURE_DATA_MM to exclude the get16 if not defined.
                   and include dsbyte.h, otherwise.
09/16/02   ttl     Added last_sn_in_q in rlci_dl_am_state_type.
09/05/02   ttl     Added Ciphering feature.
07/31/02   ttl     Rewrote 10ms base timer, removed timer sig, using timer
                   callback function instead.
                   Removed rlci_dl_am_get_timer_sig(), rlci_dl_am_timer_sig_to_rlc_id(),
                   rlci_dl_am_release_timer_sig().
06/12/02   ttl     Restructure rlc_dl_chan_am_stat_type.
                   Added structure rlc_dl_chan_am_pdu_type.
05/13/02   ttl     Added DL_STOP to internal states.
05/08/02   ttl     Added 2nd parameter for rlc_post_rx_func_ptr().
04/24/02   ttl     Rewrote the statistic log code.
04/03/02   ttl     Defined a structure for Log_on_demand feature.
03/15/02   sk      Added structure rlc_dl_chan_am_stat_type for collecting the
                   statistics information for a single AM entity. Added the
                   field rlc_dl_chan_am_stat_type to rlci_dl_am_ctrl_type
                   structure.
02/13/02   ttl     Added service de-registration function.
11/09/01   sk      Added prev_vr_h ,status_periodic_cnt,status_prohibit_cnt,
                   status_periodic to rlci_dl_am_state_type structure.Added
                   timer_blk to rlci_dl_am_ctrl_blk_type.
11/04/01   ttl     Removed application/voice service callback function feature.
10/26/01   ttl     Added application/voice service callback function feature.
08/20/01   rc      Added support for QXDM logging.
06/18/01   rc      Added support for Processing Downlink TM PDUs in
                   both Task and Interrupt Context.
06/07/01   rc      Added prototype for crlc_dl_release_all_handler().
06/07/01   rc      Removed temporary fix for VST problem.
06/06/01   rc      Added fields discard_flag, nlen and change_wins to
                   rlci_dl_am_state_type.
                   Added a temporary fix for a VST problem.
05/21/01   rc      Added more comments.
05/02/01   ttl     Added definition for rlci_dl_proc_reset_msg().
03/27/01   ttl     Added rlc_register_am_sdu_discard_cb_req().
                   Changed the max of control block for RLC to
                   UE_MAX_DL_RLC_ENTITY.
02/28/01   ttl     Added support for Acknowledged Mode.
02/13/01   rc      Removed maccommdefs.h. Changed MAC_DL_MAX_RLC_PDUS to
                   UE_MAX_DL_RLC_PDUS.
01/08/01   rc      Modified Code to use data types defined in uecomdef.h
11/22/00   rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/


#include "wcdma_variation.h"
#include "uecomdef.h"
#include "queue.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "assert.h"
#include "rex.h"
#include "rlcrrcif_v.h"
#include "rlcdsapif.h"
#include "macrlcif.h"
#include "rlci.h"
#include "l2dltask.h"
#include "l2ultask.h"
#include "rlclog.h"
#include "l1macif.h"
#include "stringl.h"

#include "modem_mem.h"

extern boolean dl_pdu_decode_result;

#define MAX_RX_WINDOW_SIZE 2048

#define RLC_WM_TRACE_CTR 50

/*---------------------------------------------------------------------------
  The user/data plane logical channel ids start from the value that is
  calculated by RRC as MAX DL TM channels + MAX DL UM channels +
  MAX AM Signalling channels.
---------------------------------------------------------------------------*/

#define RLC_DL_AM_USR_PLANE_START_LC_ID (UE_MAX_DL_TM_CHANNEL + \
                     UE_MAX_DL_UM_CHANNEL + UE_MAX_SRB_AM_EMTITIES_3)

#define RLC_DL_AM_START_LC_ID (UE_MAX_DL_TM_CHANNEL + \
                     UE_MAX_DL_UM_CHANNEL)

#define RLC_RE_ASSEMBLY_Q_SIZE 2048

#define RLC_AM_HALF_MOD_MASK 0x07FF
/* Bitmask for enabling/disabling UM user plane log packets */
#define RLCI_UM_USR_PLANE_LOG_MASK 0x01

/* Bitmask for enabling/disabling AM user plane log packets */
#define RLCI_AM_USR_PLANE_LOG_MASK 0x02

/* Bitmask for enabling/disabling AM signalling plane log packets */
#define RLCI_AM_SIG_PLANE_LOG_MASK 0x04
 
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/* Bitmask for enabling/disabling AM log packets for R7 */
#define RLCI_AM_R7_LOG_MASK 0x08
#endif

#define RLCI_GET_RESEQ_Q_IDX(sn) ((sn) & RLC_AM_HALF_MOD_MASK)

#define RLCI_GET_PDU_DATA_PTR(pdu_ptr, offset) (pdu_ptr + offset)

#define RLCI_GET_DC_BIT(pdu_hdr)  (pdu_hdr & 0x8000)

#define RLCI_GET_AM_SN(pdu_hdr)   ((pdu_hdr & 0x7fff) >> 3)

#define RLCI_GET_POLL_BIT(pdu_hdr)  ((pdu_hdr & 0x0004) >> 2)

#define RLCI_GET_H_EXT_BIT(pdu_hdr)  (pdu_hdr & 0x0003)

#define RLCI_GET_LI_EXT_BIT(pdu_hdr) (pdu_hdr & 0x01)

#define RLCI_GET_CTRL_PDU_TYPE(pdu_hdr)  (pdu_hdr & 0x07)

#define RLCI_GET_ACCUM_LI(pdu_hdr)   (pdu_hdr >> 1)

#define RLCI_GET_INC_Q_IDX(idx)   ((idx+1) & RLC_AM_HALF_MOD_MASK)

#define RLCI_GET_RESET_SN(data)  ((data & 0x08) >> 3)

#define RLCI_GET_RESET_RESV(data)   (data & 0x07)

#define RLCI_GET_LI_VAL(li)    (li >> 1)

#define RLCI_INVALID_SEQ_NUM 0xFFFF
#define RLCI_MAX_LI_IN_LIST 15

#define RLC_GET_ABS_LI(walk_item_ptr, offset, li_len, accum_li) \
 (li_len == RLC_LI_SHORT) ? (accum_li = walk_item_ptr->dsm_ptr->data_ptr[offset]) \
                          : (accum_li = (((uint16) walk_item_ptr->dsm_ptr->data_ptr[offset] << 8) | ((uint16)walk_item_ptr->dsm_ptr->data_ptr[offset + 1])))


#define RLCI_GET_UM_SN(pdu_hdr)   ((pdu_hdr & 0xFF) >> 1)

#define RLC_DISCARD_DL_WRONG_LI_B4_RST_ACK  (1 << L2_PDU_DISCARD_HDR_MSB_POS)

#define rlc_discard_sdu_ptr(item_ptr) \
             rlci_discard_sdu_ptr(item_ptr, __FILENAME__, __LINE__)

#ifdef FEATURE_RLC_QXDM_LOGGING

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
#define RLCI_GET_AM_LOGGING_CONFIG(lc_id) \
      (((lc_id >= RLC_DL_AM_USR_PLANE_START_LC_ID) && \
        ((rlci_log_mask & RLCI_AM_USR_PLANE_LOG_MASK) || \
         (rlci_log_mask & RLCI_AM_R7_LOG_MASK))) || \
       ((lc_id < RLC_DL_AM_USR_PLANE_START_LC_ID) && \
         (rlci_log_mask & RLCI_AM_SIG_PLANE_LOG_MASK)))

#define RLCI_GET_AM_R7_LOG_ENABLED_THIS_LC(lc_id) \
        ((lc_id >= RLC_DL_AM_USR_PLANE_START_LC_ID) && \
         (rlci_log_mask & RLCI_AM_R7_LOG_MASK))

#define RLCI_GET_AM_R7_LOG_ENABLED() \
        (rlci_log_mask & RLCI_AM_R7_LOG_MASK)
#else
#define RLCI_GET_AM_LOGGING_CONFIG(lc_id) \
      (((lc_id >= RLC_DL_AM_USR_PLANE_START_LC_ID) && \
         (rlci_log_mask & RLCI_AM_USR_PLANE_LOG_MASK)) || \
        ((lc_id < RLC_DL_AM_USR_PLANE_START_LC_ID) && \
         (rlci_log_mask & RLCI_AM_SIG_PLANE_LOG_MASK)))
#endif

#define RLCI_GET_UM_LOGGING_CONFIG \
         (rlci_log_mask & RLCI_UM_USR_PLANE_LOG_MASK)

//DL_INTG
/*         
#define RLC_AM_STAT_INC_ERR_PDU(dl_chan_am_stat_ptr) \
            dl_chan_am_stat_ptr->tot_num_error_pdu_rxd++

#define RLC_AM_STAT_INC_CTRL_PDU(dl_chan_am_stat_ptr) \
            dl_chan_am_stat_ptr->tot_num_ctl_pdu_rxd++

#define RLC_AM_STAT_INC_DATA_PDU(dl_chan_am_stat_ptr) \ 
            dl_chan_am_stat_ptr->tot_num_data_pdu_rxd++

#define RLC_AM_STAT_UPD_NUM_PDU_BYTE_RCVD(dl_chan_am_stat_ptr, length) \
            dl_chan_am_stat_ptr->tot_num_pdu_byte_rxd += length

#define RLC_AM_STAT_UPD_NUM_SDU_BYTE_RCVD(dl_chan_am_stat_ptr, length) \
            dl_chan_am_stat_ptr->tot_num_sdu_byte_rxd += length
*/

#define RLC_AM_STAT_INC_ERR_PDU(macro_var)  macro_var++

#define RLC_AM_STAT_INC_CTRL_PDU(macro_var) macro_var++

#define RLC_AM_STAT_INC_DATA_PDU(macro_var) macro_var++

#define RLC_AM_STAT_UPD_NUM_PDU_BYTE_RCVD(macro_var, length) \
            macro_var += length

#define RLC_AM_STAT_UPD_NUM_SDU_BYTE_RCVD(macro_var, length) \
            macro_var += length

#endif

#define RLCI_GET_CTRL_INDEX(lc_id)   dl_ctl_blk_id_tbl[lc_id]
#define RLCI_GET_CTRL_BLK_PTR(ctrl_index)    &(downlink_table.ctrl_blk[ctrl_index])

#ifdef FEATURE_QCHAT_HFN_MISMATCH_RECOVERY
/* Threshold value for counter that tracks continuous PDU error */
#define RLC_PDU_CONT_ERR_CNT_TRSHLD1 1
#define RLC_PDU_CONT_ERR_CNT_TRSHLD2 10
#endif
/*===========================================================================

MACRO GET16

DESCRIPTION
  This macro return the word value from the 2 byte value

DEPENDENCIES
  None

RETURN VALUE
  A word representing the 2 bytes pointed at by the passed parameter

SIDE EFFECTS
  None
===========================================================================*/

#define GET16(cp) (                             \
                      ((( cp[0]) << 8) | cp[1])   \
                  )

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/
#ifndef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3

#define UE_MAX_DL_RLC_ENTITY    UE_MAX_DL_LOGICAL_CHANNEL

#else

/*---------------------------------------------------------------------------
SRB's - 2 UM (srb 0,1) + 3 AM (srb 2,3,4)
CTCH - 1 UM
BCCH - 1 TM
AMR - 3 TM
PS - 2 AM or 2 UM
Video call - 1 TM

UE_MAX_DL_RLC_ENTITY   18 (5TM, 5UM, 5AM + 3 buffer)
UE_MAX_DL_RLC_ENTITY   24 for MBMS with 2MCCH + 2MSCH + 2MTCH
---------------------------------------------------------------------------*/
#define UE_MAX_DL_RLC_ENTITY    18

#endif

#ifdef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3
extern rlci_missing_list_type *rlc_dl_nak_pool;
#else
extern rlci_missing_list_type rlc_dl_nak_pool[NUM_NAK_BUF];
#endif
extern q_type rlc_dl_free_nak_q;

typedef enum {
  DL_NULL_STATE = 0,                    /* Logical channel is currently not */
                                        /* in use.                          */
  DL_DATA_TRANSFER_READY,               /* Logical Channel has been         */
                                        /* established, and data transfer   */
                                        /* is allowed.                      */
  DL_RESET_PENDING,                     /* RESET has been sent to the peer  */
                                        /* RLC entity,and no data transfer  */
                                        /* is allowed unitl a RESET ACK is  */
                                        /* received. Only applicable for    */
                                        /* AM channels.                     */
  DL_STOP = 0x04                        /* Stop to receive any transmission */
} rlci_dl_state_e_type;

/*---------------------------------------------------------------------------
  Typedef for storing parameters specific to downlink transparent mode.
---------------------------------------------------------------------------*/
typedef struct {
  boolean seg_ind;                      /* Segmentation Indication- TRUE if*/
                                        /* segmentation is on, FALSE       */
                                        /* otherwise.                      */
  boolean act_incl;                     /* Activation Time - True if       */
                                        /* included, False Otherwise       */
  uint8 act_time;                       /* Activation Time.                */
} rlci_dl_tm_ctrl_type;

/*---------------------------------------------------------------------------
  Typedef for storing UM PDU logging parameters
---------------------------------------------------------------------------*/
#ifdef FEATURE_UM_PDU_LOGGING
typedef struct{
  uint16 num_pdu;                 /* Number of PDUs to be built every TTI  */
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_DL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_dl_chan_um_pdu_type;
#endif

/*---------------------------------------------------------------------------
  Typedef for storing parameters specific to downlink unacknowledged mode.
---------------------------------------------------------------------------*/
typedef struct {
  boolean act_incl;                     /* Activation Time - True if       */
                                        /* included, False Otherwise       */
  uint8 act_time;                       /* Activation Time.                */
  uint8 li_length;                      /* Length of the length indicator  */
                                        /* - 7 or 15 bits                  */
  uint8 vr_ur;                          /* Sequence Number of the next Pdu */
                                        /* to be received. This value is   */
                                        /* set to RLC_SN + 1 upon reception*/
                                        /* of a PDU                        */
  dsm_item_type *pdu_ptr;               /* Pointer to chain of pdus being  */
                                        /* reassembled                     */
  boolean discard_flag;                 /* Indicates if the PDUs belonging */
                                        /* to a SDU should be discarded    */
  rlc_lc_id_type rlc_id;                /* Logic channel ID                */

  dsm_item_type* partial_sdu_ptr;
  dsm_item_type* last_dsm_ptr;

#ifdef FEATURE_RLC_QXDM_LOGGING
#ifdef FEATURE_UM_PDU_LOGGING
  rlc_dl_chan_um_pdu_type  dl_chan_um_pdu;  /* UM PDU logging structure*/
#endif
#endif

  boolean alternate_e_bit;          /* TRUE - Alternate E-bit interpretation */
                                    /* FALSE - Normal E-bit interpretation   */

#ifdef FEATURE_CS_VOICE_OVER_HSPA  
  boolean sn_delivery;              /* TRUE - if RLC needs to indicate the SN to PDCP*/
#endif /* FEATURE_CS_VOICE_OVER_HSPA */
#ifdef FEATURE_QCHAT_HFN_MISMATCH_RECOVERY
  uint32 pdu_err_cnt; /* Counter to track continuous error */
#endif

} rlci_dl_um_ctrl_type;

/*---------------------------------------------------------------------------
  Used to store the necessary down-link control state information for an
  RLC AM entity.
---------------------------------------------------------------------------*/
typedef struct {
  uint16 old_vr_r;
  uint16 dl_vr_h_status;                /*DL vr_h state indicating the SN */
                                        /*till which nak array has been   */
                                        /*updated. Alternatively this indicates*/
                                        /*the highest SN till which UL should*/
                                        /*form status PDUs.               */
  uint16 vr_r;                          /* VR(R) -- Rx state               */
  uint16 vr_h;                          /* VR(H) -- Expected Rx state      */
  uint16 prev_vr_h;                     /* PREV_VR_H - Store the old value */
                                        /* of VR(H)                        */
  uint16  report_vr_h;

  uint16 rx_win;                        /* Rx window size                  */
  uint16 vr_mr;                         /* VR(MR) -- V(R) + Rx_Window_size */
  uint16 first_err;                     /* First Error Sequence Number     */
                                        /* received in the status report.  */
                                        /* Valid only if nak_rvcd = TRUE   */
  boolean nak_rcvd;                     /* Received a NAK in the Status    */
                                        /* Report.                         */
  boolean rcvd_poll;                    /* Received a poll from the peer   */
                                        /* side in this TTI.               */
  boolean status_prohibit;              /* status report prohibit or not   */
  boolean missing_pdu_ind;              /* Missing PDU indicator           */
  uint8 li_length;                      /* Length of the length indicator  */
                                        /* value 1 means 7 bits            */
                                        /* Value 2 means 15 bits           */
  boolean discard_flag;                 /* Flag indicating whether the     */
                                        /* first N LIs and SDUs of the     */
                                        /* first PDU in the reseq Queue    */
                                        /* need to be discarded.           */
  byte nlen;                            /* Number of LIs to discard. Valid */
                                        /* only if discard_flag == TRUE    */
  boolean change_wins;                  /* Indicates if window size changed*/
  uint16  status_periodic_cnt;
  uint16  status_prohibit_cnt;
  uint16  last_sn_in_q;                 /* Last SN in re-assemble queue.   */
  boolean status_periodic;              /* Status periodic                 */
  uint8   peer_reset_sn;
  uint8   backup_timer_cnt;             /* backup timer                    */
  boolean fst_rx_pdu;                   /* TRUE, if the next rx PDU is the */
                                        /* 1st rx'd PDU.                   */

  uint16  cur_status_periodic;          /* Current Timer_Status_Periodic   */
                                        /* value                           */
  boolean lower_hfn_rcvd;               /*flag to track if HFN value received */
                                        /*in RESET/RESET_ACK is lower than the one configured*/
  uint16 last_sn_processed;
} rlci_dl_am_state_type;


#ifdef FEATURE_RLC_QXDM_LOGGING
/*---------------------------------------------------------------------------
  Data Structure for RLC DL states for Acknowledged Mode statistics logging
---------------------------------------------------------------------------*/
typedef struct{
  uint32 tot_num_pdu_byte_rxd;    /* Total number of bytes received        */
  uint32 tot_num_sdu_byte_rxd;    /* Total number of sdu bytes to be sent  */
                                  /* to upper layer                        */
  uint16 tot_num_error_pdu_rxd;
  uint16 tot_num_data_pdu_rxd;    /* Total number of AMD PDUs received     */
  uint16 tot_num_ctl_pdu_rxd;     /* Total number of Control PDUs received */
  uint16 tot_num_pdu_nacked;      /* Total number of pdus nacked by peer   */
}rlc_dl_chan_am_stat_type;

/*--------------------------------------------------------------------------
Data structures for logging the first 4 bytes of the PDU by QCAT.
---------------------------------------------------------------------------*/

typedef struct{
  uint16 num_pdu;                 /* Number of PDUs to be built every TTI  */
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  uint16 pdu_size[MAX_DL_RLC_LOG_PDUS];
#else
  uint16 pdu_size;                /* PDU size in octets of each PDU        */
#endif
  uint32 log_data[MAX_DL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_dl_chan_am_pdu_type;

#endif

typedef struct {
  dsm_item_type*   dsm_ptr;
  uint16           offset;
  uint16           length;
  /* AM - last 15 bits - SN(12bits) + P(1 bit) + HE(2 bits) */
  /* UM - last 8 bits - SN(7 bits) + E(1 bit) */
  uint16           ctrl_info; 
}rlc_pdu_info_type;

typedef struct {
  dsm_item_type*   dsm_ptr;
  uint16           offset;
  uint16           length;
}rlc_am_sdu_info_type;

typedef struct {
  boolean in_use;
  rlc_lc_id_type dl_data_id;
  rlc_pdu_info_type *pdu_info;
  dsm_item_type* partial_sdu_ptr;
  dsm_item_type* last_dsm_ptr;
  uint32 sdu_size;
}rlc_dl_am_reseq_q_type;  


typedef struct {
  uint16 pdu_cnt;

  /* starting address of big dsm poiner which may have many PDU payload */
  dsm_item_type   *dsm_ptr;

  /* offset from where this PDU payload starts */
  uint16 offset;

  /* length of PDU*/
  uint16 length;
}rlci_dl_temp_pdu_info_type;
/*---------------------------------------------------------------------------
  Used to store the necessary down-link control information for
  an RLC AM entity.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_am_common_config_type am_common_local; /* Local copy of common config variables.        */
  rlc_dl_am_config_type  dl_ctl_cnfg_local;   /* Local copy dl control config variables.    */
  rlc_am_common_config_type *am_common; /* pointer to common config variables.        */
  rlc_dl_am_config_type *dl_ctl_cnfg;   /* pointer to dl control config variables.    */
  rlci_dl_am_state_type  dl_state_var;  /* dl control state variables.     */
  rlc_opt_status_type  dl_status_opt_bitmap; /* bitmap array for DL status */
  uint16               num_sn_li; /* Number of SN_LI pairs for LIST SUFI*/
  rlc_dl_am_reseq_q_type *dl_reseq_q;   /* DL Resequence Queue holding 
                                           the received DL PDUs */
  void (*sdu_discard_cb) (rlc_lc_id_type data_lc_id, byte num_sdu);
                                        /* SDU discard callback function   */
  rlc_tmr_status_type dl_tmr_sts;       /* Holds status of DL AM timer     */
#ifdef FEATURE_RLC_QXDM_LOGGING
  rlc_dl_chan_am_stat_type dl_chan_am_stat; /* Stores the Am statistics    */
                                            /* information for one channel */
  rlc_dl_chan_am_pdu_type  dl_chan_am_pdu;   /* Stores the PDU information */
                                             /* for a single AM entity     */
#endif
} rlci_dl_am_ctrl_type;

/*---------------------------------------------------------------------------
  Union for storing parameters specific to a mode.
---------------------------------------------------------------------------*/
typedef union {
  rlci_dl_tm_ctrl_type tm_ctrl;         /* Transparent Mode parameters.    */
  rlci_dl_um_ctrl_type um_ctrl;         /* Unacknowledged Mode parameters. */
  rlci_dl_am_ctrl_type am_ctrl;         /* Acknowledged Mode parameters.   */
} rlci_dl_ctrl_u_type;

/*---------------------------------------------------------------------------
  Used to store information about a downlink logical channel.
---------------------------------------------------------------------------*/
typedef  struct {
  uecomdef_logchan_e_type lc_type;      /* Downlink logical channel type   */
  uecomdef_logch_mode_e_type lc_mode;   /* RLC Mode                        */
  rlci_dl_state_e_type lc_state;        /* Downlink Logical channel state  */
  rlci_ciphering_type cipher;           /* Ciphering Parameters            */

  rlci_ciphering_type cipher_backup;    /* Backup for ciphering parameters
                                           while applying the new one      */

  rlci_dl_ctrl_u_type ctrl;             /* Union of TM, UM and AM specific */
                                        /* parameters.                     */
  boolean context;                      /* Task(FALSE) or Interrupt (TRUE) */

  void *rlc_post_rx_func_ptr_para;      /* 3rd parameter for               */
                                        /* rlc_post_rx_func_ptr()          */
  uint16 n_sdu;                      
  void  (*rlc_post_rx_func_ptr)(uint8, uint8, void *);
                                        /* Callback function called when an*/
                                        /* SDU is placed on the downlink   */
                                        /* data queue                      */

  dsm_watermark_type *dl_wm_ptr;        /* Pointer to the associated RLC   */
                                        /* downlink data queue             */

} rlci_dl_ctrl_blk_type;

#ifdef FEATURE_RLC_QXDM_LOGGING
/*---------------------------------------------------------------------------
  The following two structures are used only for QXDM logging.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data Structure for RLC DL states for Transparent, Unacknowledged and
  Acknowledged Modes
---------------------------------------------------------------------------*/
typedef struct
{
  uint8    rlc_id;              /* DL logical channel id (0..18)           */
  uint8    lc_state;            /* State of the RLC State Machine          */
                                /* 0 - RLC_DL_NULL_STATE                   */
                                /* 1 - TM_DL_DATA_TRANSFER_READY           */
                                /* 2 - UM_DL_DATA_TRANSFER_READY           */
                                /* 3 - AM_DL_DATA_TRANSFER_READY           */
                                /* 4 - AM_DL_RESET_PENDING                 */

} rlc_dl_chan_state_type;

typedef struct
{
  uint8 nchan;                  /* Number of Downlink logical channels.    */
  rlc_dl_chan_state_type dl_state[UE_MAX_DL_LOGICAL_CHANNEL];
                                /* State Information for each Downlink     */
                                /* logical channel.                        */
}rlc_dl_log_state_type;

/*--------------------------------------------------------------------------
Data structures for logging the DL AM&UM PDU ciphering parameters
---------------------------------------------------------------------------*/

typedef struct{
  uint8  log_dl_rlc_id;           /* Data Logical Channel DL RLC ID(0..18) */
  uint32 cipher_key_idx;
  uecomdef_wcdma_cipher_algo_e_type cipher_algo;
  uint32 b4_cipher;               /* The 1st 4 bytes of the PDU before     */
                                  /* decipher.                             */
  uint32 count_c;                 /* PDU size in octets of each PDU        */
                                  /* Dump of the status PDU                */
} rlc_dl_cipher_log_data_type;

/*--------------------------------------------------------------------------
Data structures for logging the DL AM&UM PDUs ciphering parameters
---------------------------------------------------------------------------*/
typedef struct{
  boolean cipher_pdu_logging;     /* TRUE - cipher PDUs to be logged in    */
                                  /* Task Context, FALSE - otherwise       */
  uint8 num_pdu;                  /* Number of PDUs to be built every TTI  */
  rlc_dl_cipher_log_data_type cipher_log_data[MAX_CIPHER_PDU];
                                  /* The first four bytes of the pdu       */
} rlc_dl_pdu_cipher_info_type;


#endif


/*---------------------------------------------------------------------------
  Database that is used to store information about all the downlink logical
  channels. Maximum number of downlink logical channels is 21.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 active_ch;                      /* Num of active logical channels. */
  rlci_dl_ctrl_blk_type ctrl_blk[UE_MAX_DL_RLC_ENTITY];
                                        /* Data structure used to store all*/
                                        /* the information for the         */
                                        /* downlink logical channel.       */
} rlci_downlink_table_type;

typedef struct {
  uint8 rlc_id;
  uint8 npdus;
  uint16 pdu_size;
  dsm_item_type *dsm_ptr[UE_MAX_DL_RLC_PDUS];
}rlci_dl_chan_data_type;

typedef struct {
  uint8 nchan;
  rlci_dl_chan_data_type chan_info[UE_MAX_DL_LOGICAL_CHANNEL];
} rlci_dl_frame_data_type;


extern uint8 dl_ctl_blk_id_tbl[UE_MAX_DL_LOGICAL_CHANNEL];

/*===========================================================================

FUNCTION RLCI_DL_AM_HANDLE_10MS_TIMER_CB

DESCRIPTION
  Handle 10ms Base Timer timed out.
  In downlink, there is only one base timer. All timers related in the downlink
  are done in the form of counters. Every time the 10ms base timer expired,
  this function looks into the timer counters that needed to be increased
  by one and then compares w/ the real configured timer related to the
  individual counters for timer expiration. If the real timer expired, a
  related flag will be raised.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_am_base_timer_handler ( void );
/*===========================================================================

FUNCTION RLC_DL_INIT

DESCRIPTION
  Initializes the RLC Downlink Logical channel table.

DEPENDENCIES
  This function should be called at task start up.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_init
(
  void
);

/*===========================================================================

FUNCTION CRLC_DL_CIPHER_CONFIG_HANDLER

DESCRIPTION
  Configures ciphering on one or more Unacknowledged or Acknowledged
  Mode Channels.This function configures the ciphering parameters that are
  recevied from RRC. The ciphering parameters are updated only if the the
  channel is in DL_DATA_TRANSFER_READY STATE.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_cipher_config_handler
(
  rlc_dl_cipher_config_type *cipher_ptr   /* Ptr to Downlink Cipher Config */
);
/*===========================================================================

FUNCTION CRLC_DL_HFN_HANDLER

DESCRIPTION
 This function handles the request for the Hyperframe number by RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_hfn_handler
(
  rlc_dl_hfn_type *hfn_ptr                 /* Ptr to HFN info              */
);

/*===========================================================================

FUNCTION CRLC_DL_HFN_CNF

DESCRIPTION
 This function sends the hyperframe number of the requested channels to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_hfn_cnf
(
  rlc_dl_hfn_cnf_type *hfn_ptr             /* Ptr to HFN CNF info.         */
);


/*===========================================================================

FUNCTION RLC_DL_PDU_DATA_HANDLER

DESCRIPTION
 This function processes PDUs received on unacknowledged mode and
 acknowledged mode channels. If the channel for which the PDU is intended is
 in DL_DATA_TRANSFER_READY State, a function is called to process the PDU.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_pdu_data_handler(void);

#ifdef RLC_DEBUGGING
#error code not present
#endif //RLC_DEBUGGING

/*===========================================================================

FUNCTION RLC_REGISTER_AM_SDU_DISCARD_CB_REQ

DESCRIPTION
  Register AM SDU discard callback function.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_register_am_sdu_discard_cb_req
(
  rlc_register_am_sdu_discard_callback_type *dl_sdu_discard_cb
);

/*===========================================================================

FUNCTION CRLC_DL_TM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of Transparent Mode Logical
  channels. This function is called when RRC sends a CRLC_DL_TM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DL_DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the channel is set to the NULL
  state and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the downlink table entry for the
  channel is updated with the configuration received from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_tm_config_handler
(
  rlc_dl_tm_config_type *tm_ptr /* Pointer to TM config info               */
);


/*===========================================================================

FUNCTION CRLC_DL_UM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of Unacknowledged Mode Logical
  channels. This function is called when RRC sends a CRLC_DL_UM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DL_DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the channel is set to the NULL
  state and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the downlink table entry for the
  channel is updated with the configuration received from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_um_config_handler
(
  rlc_dl_um_config_type *um_ptr /* Pointer to UM config info               */
);

/*===========================================================================

FUNCTION CRLC_DL_AM_CONFIG_HANDLER
  This function handles the configuration of an Acknowledged Mode entity.
  it is called when RRC sends a CRLC_DL_AM_CONFIG_REQ to RLC. If a new
  entity is to be established, the function checks if the channels exists.
  If it already exists, an ERR message is dispalyed. If the entity does not
  exist, an uplink table entry for this entity is updated and the state is
  set to DATA_TRANSFER_READY.
  If an entity is to be released,the function checks if the channel is in the
  DATA_TRANSFER_READY state. If it is, the channel is set to the NULL state
  and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DATA_TRANSFER_READY state. If it is, the downlink table entry for the channel
  is updated with the configuration received from RRC.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void crlc_dl_am_config_handler
(
  rlci_dl_am_config_req_type *dl_am_config_req_ptr
);

/*===========================================================================

FUNCTION GET_AM_CTL_RLK

DESCRIPTION
  This function return the pointer of control block for an AM.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the AM control block.

SIDE EFFECTS
  None
===========================================================================*/

rlci_dl_ctrl_blk_type *get_am_ctl_blk
(
  byte lc_id
);

/*===========================================================================

FUNCTION RLCI_DL_PROC_RESET_MSG

DESCRIPTION
  Process the RLCI_UL_RESET_DONE and RLCI_DL_RESET_REQ messages from
  UL AM RLC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_proc_reset_msg
(
  wcdma_l2_dl_cmd_enum_type cmd_id,
  rlc_lc_id_type dl_rlc_data_id
);

/*===========================================================================

FUNCTION CRLC_DL_RELEASE_ALL_HANDLER

DESCRIPTION
  Releases all downlink Channels and sends a confirmation to the uplink
  after release.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_release_all_handler( void );

/*===========================================================================

FUNCTION RLCI_DL_GET_CTL_BLK_PTR

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlci_dl_ctrl_blk_type *rlci_dl_get_ctl_blk_ptr(uint8 ch_id) ;


/*===========================================================================

FUNCTION RLCI_DL_AM_HANDLE_10MS_TIMER_CB

DESCRIPTION
  Handle 10ms Base Timer timed out.
  In downlink, there is only one base timer. All timers related in the downlink
  are done in the form of counters. Every time the 10ms base timer expired,
  this function looks into the timer counters that needed to be increased
  by one and then compares w/ the real configured timer related to the
  individual counters for timer expiration. If the real timer expired, a
  related flag will be raised.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_am_handle_10ms_timer_cb(uint32 timer_sig);


#ifdef FEATURE_RLC_QXDM_LOGGING

/*===========================================================================

FUNCTION RLCI_DL_AM_HANDLE_QXDM_LOG_TIMER

DESCRIPTION

This function is called when the qxdm log timer expires. This functionthen
prepares to send the packet to qxdm for logging purposes. RLC_AM_LOG_TIMER_BASE
is currently set to 3seconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rlci_dl_am_handle_qxdm_log_timer(uint32 dl_log_timer_sig);

#endif  /* FEATURE_RLC_QXDM_LOGGING */

/*===========================================================================

FUNCTION  RLC_DL_AM_BACKUP_TIMER

DESCRIPTION
  This function check the backup timer count. If it is greater than the
  hard-coded value (5). Start the 10ms timer for the AM channels.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_am_backup_timer(void);

/*==========================================================================

FUNCTION RLC_PROCESS_MAC_HS_RESET_REQ

DESCRIPTION
  This function handles the MAC HS Reset command from MAC. RLC sends
  a status report on receiving this command even if any status prohibit
  timer is on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================*/
void rlc_process_mac_hs_reset_req
(
  mac_hs_dl_reset_req_type  *mac_hs_reset_req
);


/*===========================================================================

FUNCTION CRLC_DL_ABORT_CIPHER_CONFIG

DESCRIPTION
  This function reverts back to the old ciphering configuration if an abort
  is received from RRC while applying the new configuration due to triggering
  of cell update in RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void crlc_dl_abort_cipher_config
(
  rlc_abort_cipher_config_type *abort_cipher_info
);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_DL_LOG_CIPHER_PKT

DESCRIPTION
This function logs the dowlink PDU ciphering parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_log_cipher_pkt
(
  void
);
#endif /* FEATURE_RLC_QXDM_LOGGING */

/*===========================================================================

FUNCTION CRLC_DL_WM_CLEANUP_CMD_HANDLER

DESCRIPTION
   this function cleanup the PDCP-RLC WM by a call back function.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/

void crlc_dl_wm_clean_up_cmd_handler
(
  rlci_cleanup_wm_cmd_type *cleanup_wm_cmd
);

/*===========================================================================

FUNCTION CRLC_DL_CONTINUE_HANDLER

DESCRIPTION
  Continue transmission on one or more downlink unacknowledged or acknowledged
  mode channels.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_continue_handler
(
  rlc_dl_continue_type *continue_ptr      /* Ptr to continue Info                */
);

/*===========================================================================

FUNCTION rlci_enh_dl_check_pdu_validity

DESCRIPTION
   Validate the PDU Contents.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/

boolean rlci_enh_dl_check_pdu_validity
(
  rlci_dl_ctrl_blk_type    *ctrl_blk,
  complete_pdu_type        *complete_pdu_ptr,
  boolean                  *rxd_piggy_back, 
  mcalwcdma_dec_iovec_type *piggy_back_pdu
  , boolean alt_e_bit
);

/*===========================================================================

FUNCTION rlci_get_li_value

DESCRIPTION
   Get the LI Value.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_get_li_value(complete_pdu_type *complete_pdu_ptr, uint16 offset,
                         uint16 li_len);

/*===========================================================================

FUNCTION rlci_get_li_count

DESCRIPTION
   Get number of LI present.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_get_li_count(dsm_item_type* dsm_ptr, uint16 offset, uint16 li_len, 
                         uint16 pdu_len);


/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler

DESCRIPTION
    - Process the R99 data, if received
       - Get the RLC Info from the rlc_enh_data_queue. Protect this operation with 
         WCDMA_INTLOCK, as it this queue is accessed by MAC-HS also
       - After processing all the entries(PDUs) in the received rlc_enh_data_ptr,
         rlc_enh_data_ptr will be added to free queue
    - Process the HS data, if received
       - Get the TSN from the rlc_tsn_info_q. Protect this operation with WCDMA_INTLOCK
         as it this queue is accessed by MAC-HS also
       - After processing all the entries(PDUs) in the received tsn_info_buf_ptr,
         tsn_info_buf_ptr will be added to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_handler(void);

#if 0
//#ifdef FEATURE_Q6_MT
/*===========================================================================

FUNCTION rlc_enh_dl_data_reassembly_signal

DESCRIPTION  
  Go through all the AM Channels and if there is any old_vr_r which is different
  from vr_r, then there is some data left over and RLC can reassemble them.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Need to optimize the function such that, only AM channels will be used 
  in FOR loop, than all the logical channels
  Data in the first AM LC will be completed before moving to next AM LC,
  as Signal doesnt know about the LC which triggered this action

===========================================================================*/
void rlc_enh_dl_data_reassembly_signal(void);
#endif

/*===========================================================================

FUNCTION rlci_discard_sdu_ptr

DESCRIPTION
    For every DSM Buffer/item in the packet chain, discard the buffer/item as 
    per conditions below
    If the DSM Buffer/item app_field is zero, 
       - DISCARD the DSM Buffer/item by calling dsmi_free_buffer
    Otherwise
       - Nothing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_discard_sdu_ptr
(
 dsm_item_type **dsm_ptr,
 const char * file,
 uint32 line
);

/*===========================================================================

FUNCTION rlc_discard_pdu

DESCRIPTION
    Discard the PDU if not NULL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_discard_pdu
(
  complete_pdu_type       *complete_pdu_ptr
);
#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION rlc_enh_logging_data_handler

DESCRIPTION
    - Logging functionality called for all the PDUs which belongs to given LC Id     
      If first_pdu_on_this_lch is TRUE, initialise the num_pdu to zero

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_logging_data_handler(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                  complete_pdu_type *complete_pdu_ptr,
                                  boolean first_pdu_on_this_lch);

/*===========================================================================

FUNCTION rlc_enh_logging_post_data_handler

DESCRIPTION
    - Logging functionality called for all the PDUs which belongs to given LC Id     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_logging_post_data_handler(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                       uint8 lc_id, boolean hs_path);

#endif

/*===========================================================================

FUNCTION rlci_enh_update_nak_list

DESCRIPTION
    - As part of Post-Processing, Status message will be prepared as below
      - Prepare the LIST SUFI in which, 
        SN is the sequence number in which first hole is existing and 
        L is the number of consecutive holes present      

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_update_nak_list(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);

/*===========================================================================

FUNCTION rlci_enh_status_generation

DESCRIPTION
    - As part of Post-Processing, Status message will be prepared
      - Copy NAK item from NAK_LIST_Q to TX_NAK_Q
      - Let the UL know that it has to send a status report to the peer entity
      - Clear the rcvd poll bit if set
      - Turn the status prohibit flag to TRUE to prohibit the sending further status

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_enh_status_generation(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);

/*===========================================================================

FUNCTION rlci_enh_dl_reassembly_post_pdu

DESCRIPTION
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality which will work on the received PDUs in this call
       - If needed NAK list will be prepared
       - If needed, reassembly functionality is called to prepare SDUs out of
         received in-seq PDUs.
       - If needed and allowed, Status message will be prepared

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_reassembly_post_pdu(rlci_dl_ctrl_blk_type *ctrl_blk_ptr);

/*===========================================================================

FUNCTION rlc_enh_rcvd_data_post_processing

DESCRIPTION
    - As part of the post_processing functionality
      - 1. If configured, Post the logging data
      - 2. For all the previously received PDUs on prev_lc_id, 
             For AM - Call the Post procesing function
             For UM - Reassembly is taken care during the PDU handling itself
                      If there is anything left over in dl_pdu_info_ptr 
                      for this LgCh,
                        - Update to partial_sdu_ptr
      - 3. If any SDU is enqueued into Water mark and Call back is registerd, 
           Call the callback function to post SDU data to Upper Layers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_rcvd_data_post_processing(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                                       uint8 prev_rlc_id, 
                                       boolean hs_path);

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler_hs

DESCRIPTION
    - For every log Channel
       - Process each PDU one after another
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality (Reassembly, Status building, Status preperation)
      which will work on the received PDUs in this call
    - For UM and TM, during the PDU processing itself Post processing is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_dl_pdu_data_handler_hs(l1_dec_hs_tsn_info_type *tsn_info_buf_ptr);

/*===========================================================================

FUNCTION rlc_enh_handle_tm_data

DESCRIPTION
    - Process TM data, if present and configured to process in interrupt context
    - If some data present in TM with interrupt context TRUE, 
        - Process the TM PDUs
    - If some more PDUs are left in the rcvd, copy them into local buffer
      and back into rlc_enh_data_ptr
    - If nothing, return the entry to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_handle_tm_data(rlc_enh_data_ind_type *rlc_enh_data_ptr);

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler_non_hs

DESCRIPTION
    - For every log Channel
       - Process each PDU one after another
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality (Reassembly, Status building, Status preperation)
      which will work on the received PDUs in this call
    - For UM and TM, during the PDU processing itself Post processing is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_dl_pdu_data_handler_non_hs(rlc_enh_data_ind_type  *rlc_enh_data_ptr);


/*===========================================================================

FUNCTION RLCI_DEBUG_TRACE_PRINT_DATA

DESCRIPTION
  To print the contents of the data in the item_ptr.
  P.S: Needed trace length needs to be passed in number of words
  
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_debug_trace_print_data(dsm_item_type *walk_item_ptr, 
                                 uint16         needed_trace_length);

/*===========================================================================

FUNCTION DL_ASSIGN_CTL_BLK

DESCRIPTION
  Assign Downlink control block for an RLC entity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dl_assign_ctl_blk(void);
/*===========================================================================

FUNCTION RLC_UL_ENQ_DSM_ITEM_TO_WM

DESCRIPTION
  API to check for SDU length boundary check and enqueue to DL WM queue.

DEPENDENCIES
  None.

RETURN VALUE
  Returns SDU length that is enqueued to WM
  Returns 0 if SDU is not within boundary.

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlc_ul_enq_dsm_item_to_wm 
(
  rlc_lc_id_type         rlc_id,
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
  dsm_item_type         *sdu_ptr
);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLCI_GENERATE_DL_CONFIG_EVENT

DESCRIPTION
  To generate an event whenever a DTCH logical channel is established, modified
  or restablished, to aid in automation. The argument rlc_size is used only
  in the case the UM RLC entity to pass the size to this function, as this
  information is not available in the control block.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_generate_dl_config_event(rlci_dl_ctrl_blk_type *ctrl_blk,
                                   uint32 rlc_size);
#endif /*FEATURE_QXDM_LOGGING*/

/*===========================================================================

FUNCTION RLC_UL_AM_CHAIN_DSM_ITEM

DESCRIPTION
  Function to check for SDU length boundary check and chain the DSM item to 
  chain_tail_ptr if SDU length is valid.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns SDU Length if valid, else returns 0.
  
SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlci_ul_am_chain_dsm_item
(
  rlci_dl_ctrl_blk_type  *ctrl_blk_ptr,
  dsm_item_type          *sdu_ptr,
  dsm_item_type         **chain_head_ptr,
  dsm_item_type         **chain_tail_ptr
);

/*===========================================================================
FUNCTION: RLC_DL_ALLOCATE_MEM_ZI_OPT

DESCRIPTION:
  This function dynamically allocates memory to RLC-DL data structures
===========================================================================*/
void rlc_dl_allocate_mem_ZI_opt(void);

/*===========================================================================
FUNCTION: RLC_DL_DEALLOCATE_MEM_ZI_OPT

DESCRIPTION:
  This function frees the RLC-DL dynamic allocated data structures
===========================================================================*/
void rlc_dl_deallocate_mem_ZI_opt(void);

rlci_dl_ctrl_blk_type *rlci_get_dl_ctrl_blk_ptr(uint8 ctrl_index);

#endif

