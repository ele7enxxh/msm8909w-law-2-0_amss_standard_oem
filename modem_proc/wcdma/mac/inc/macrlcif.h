#ifndef MACRLCIF_H
#define MACRLCIF_H

/*==================================================================

   MAC - RLC INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the interface functions and data types defined
  in MAC and used by RLC

  Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2002, 2005-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
====================================================================*/

/*===================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macrlcif.h_v   1.10   09 May 2001 18:16:08   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/macrlcif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     -------------------------------------------------- 
04/29/15   scv     RLC_ENH: Avoid RESET due to MaxDAT while higher priority LC is receiving DL traffic
04/23/15   as      Added code change at RLC to report buffer occupancy for re-transmission independently for each flexible size 
                   configured RB instead of reporting maximum of 63 PDU's to be re-transmitted for all RB's.
01/30/15   as      Added code change at RLC to not report any BO for RLC-ID with flexible configuration whenever 
                   there is pending data for re-transmission and MAC-RLC flexible PDU re-transmission buffer is full.
01/14/15   aa      Changes for new ULC log packet(0x41C5)
01/12/15   kv      Replace __FILE__ with __FILENAME__ 
11/28/14   aa      Fix to set poll_next to TRUE whenever partials with poll bit set are cleared
10/21/14   ts      SW enhancement to avoid reporting of event 6D due to persistent higher ETFCIs selected on both the carriers
10/17/14   ts      Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
10/07/14   as      Fix to avoid RLC sending last segment of SDU without setting E-bit 
                   in ALT-e configuration.
09/09/14   kc      QXDM command for changing in STMR ticks per compression usage
06/19/14   kc      Mainline FEATURE_WCDMA_HS_RACH_DELAY_CU
04/04/14   scv     Code changes for UL Compression feature
04/02/14   scv     Code changes for UL Compression feature
01/09/14   scv     Code changes for UL Compression feature
01/06/14   kc      Data Indication to RRC during FACH state
12/09/13   as      Added code change not to disable RACH if there is any pending data in DL 
                   that needs to be received which in turn would not trigger EDRX full sleep.
11/27/13   scv     Code Changes for UL Compression feature
05/22/13   kc      Added DC-HSUPA changes
05/09/13   kc      Fix to clear status at RLC while moving to PCH & RACH abort is receive
05/03/13   as      Change to discard DL PDUs if PDU health passed by decoder is false
04/29/13   kv      Moved F3s under debug flag & added QXDM support for tvm_debug
02/19/13   kc      Fix to pend status only if trashed pdu is RLC AM CTRL pdu
01/30/13   as      Changing RLC_MAX_NUM_SDU_LEN_INFO_Q_SIZE value
01/15/13   as      MAC_I - Fix to avoid crash due to EUL ISR accessing a global which is being memset 
                   as part of R99 UL ISR context.
07/06/12  as       Fix to avoid concurency issue of L2 accessing WM during BO in ISR context and 
                   WM deregisteration happening in L2 conetxt.
09/21/12   as      Cleaned up code with proper identation and added missing function headers. 
10/12/12   kc      HS_RACH MAC F3's moved under debug variable and x4314 log packet issue
09/14/12   as      Cap the PDU size to 123bytes instead of 126bytes when LI-7 bit configured
09/14/12   mrg     HSRACH: RLC TM mode – Don’t add the same DSM item to rlc_free_mac_i_ctrl_q
10/12/12   ts      Reordering of structure elements for memory optimization
08/31/12   as      Fix to build exact number of control,re-tx & new data PDU 
                   at RLC that is requested by MAC.
10/01/12   pm      New enum RLC_SUSPEND/RLC_RESUME added in CMAC_UL_CONFIG
08/23/12   mrg     MAC-iis: Special HE functionality with LI 7 configured and 
                   RLC PDU size greater than 126 octets
08/06/12   mrg     HS-RACH - Tb timer semaphore added
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/25/12   as      New WM reg api() added for trigger base rach 
07/18/12   as      Fix to trigger RACH procedure whenever there is pending data in L2
                   watermark queue when L1 is in FACH state
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
08/13/12   mrg     HS-RACH & MAC-I Feature updated code check-in.
07/17/12  mrg      Moved enable/disable indication of Trigger based RACH from BFI to l1_query_mac_rach_data()
08/13/12   mrg     HS-RACH - Tb timer semaphore added
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/18/12   mrg     L2 - MAC-I and HS-RACH code changes
04/26/12   grk     Added new API file exposing L2 command structures
04/24/12   grk     Fixed compiler warnings
01/10/12   grk     Profiling support for RLC PDU building and MAC TFCI selection.
02/16/12   Ksr     Added support for Trigger Based Rach Procedure
01/02/12   grk     Feature cleanup
01/23/12   kc      Changes for free floating tasks
09/05/11   grk     Fix to consider SRBs as highest priority during TFCI selection.
03/18/11   grk     Fixed compiler warnings.
03/09/11   ssg     MAC-ehs support on Apollo.
12/16/10   mrg     In case of RACH abort and persistency failure MAC inform to RLC to re-send status report
12/15/10   vp      Fixed compiler warning
11/30/10   pj      L2 mutex cleanup
10/18/10   ssg     Added changes for new WCDMA-L2 interface.
11/03/10   grk     Added support to set RLC/MAC debug variables through QXDM command.
10/19/10   kvk     Added support to flush rlc_tsn_info_q when HS is stopped
06/28/10   mrg     MAC-R99 ZI memory reduction changes
04/13/10  grk      changes for ZI memory reduction.
07/14/09  mrg      Fix to drop TM SDUs with UL cfn greater than MAC_LB_MAX_TTI_DELAY
                   (10 TTIs) for LB2 test cases.
07/06/09   av      R99 & L2 Mutex cleanup
07/02/09  mrg      Coade added under feature FEATURE_TC_CONSTANT_DELAY_ENH
                   for the TS 34.121 TC 7.9.1A fix
                   with this fix we are maintaing a constant delay for the LB-2, 
                   if the delay corssed more than 10 TTIs we are dropingthe SDUs.
07/01/09   av      Fix mob compilation error
06/26/08   av      l1-l2 interface cleanup 
06/03/09   kvk     Fixed MOB compilation error
05/29/09   ssg     Added support for SNOW_3G algo.
05/27/09   grk     R99/EUL mutex changes to backup the mutex(R99/EUL)
                   being locked and unlock the same.
05/15/09   grk     Added changes for R99 mutex.
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
03/11/09   pj      Added code changes to have UL/DL-task & EUL-ISR concurrency 
                   protection under light-weight mutex
03/01/09   rm      Added new featurization for FEATURE_CMI.
01/22/09   grk     Merged L2 mutex changes from taxis to mainline.
05/27/05   ssg     Modified UM buffer status for VOIP optimisation
                   under FEATURE_RLC_ALTERNATE_E_BIT.
12/23/08   ms      Replaced dob_handle_crit_sect with dec_tsn_q_crit_sect.
12/19/08   grk     Added mutual exclusion check for FEATURE_Q6_MT and
                   FEATURE_L2_MUTEX features, only one of these two
                   features should be enabled at a time.
12/19/08   ksr     compilation warning
                   updated the definition of L2_ACQ_UL_DL_UNLOCK, when feature
                   FEATURE_Q6_MT is turned off
12/18/08   ksr     L2 Mutex changes to avoid concurrency issues
12/05/08   grk     compilation fix for 8200 (rel7).
10/17/08   grk     Merged taxis changes to mainline.
05/19/08   kvk     rlc_dl_decode_pdu_hdr function will now return boolean status flag
03/20/08   sk      added changed for the UL data path optimization under FEATURE_WCDMA_UL_DATA_PATH_OPT
09/05/07   sk      Defined new interfaces to get buffer status information from RLC
                   Merged the interfaces for EUL, R99
                   Used the rlc_ul_enh_buffer_status fn under FEATURE_RLC_ENH_BUFFER_STATUS
08/08/07   sk      Modified the comments
07/20/07   sk      Modified the RLC-MAC buffer occupancy interface
                   for R99 channels.
02/19/07   sk      Changes to include RLC headers in SI reporting,
                   include the RLC wm when the tx window is hit.
01/04/07   ssg     Added function rlc_dl_set_cipher_logging().
08/14/06   sk      Added HSUPA support
06/28/06   gsc     Made nbytes in rlc_ul_lc_buf_status_type as uint32
03/02/06   sk      Added prototype for function
                   RLC_PROCESS_STATUS_REPORT_REQ under feature
                   FEATURE_WCDMA_LAST_UL_SRB_DATA_REC
02/15/05   gsc     Added HSDPA support under FEATURE_HSDPA feature flag.
10/23/02   sk      cipher_key_id is moved from the rlc_ul_logchan_data_type
                   into rlc_ul_pdu_data_type
09/04/02   sk      Added pdu_cipher_enable to rlc_pdu_data_type structure.
01/30/01   ps      removed "rlc_id" from "rlc_ul_lc_buf_status_type"
                   removed "num_chan" from "rlc_ul_buf_status_type"
                   included "uecomdef.h" and modified the data types
                   to comply with this include file

01/12/01   ps      merged the contents of rlcmacif.h into this file

12/11/00   ps      changed the prototypes of the following functions:
                    - rlc_dl_pdu_data_ind ()
                    - rlc_dl_decode_pdu_hdr()
                    - rlc_ul_build_pdus()

11/17/00   ps      Initial release

====================================================================*/


#include    "wcdma_variation.h"
#include    "dsm.h"
#include    "uecomdef.h"
#include    "l1macif.h"
#include    "l1macdata.h"
#include    "a2_ul_phy_hspa.h"
#include    "a2_ul_sec.h"

#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
#include "modem_mem.h"
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */


#ifdef  FEATURE_WCDMA_TRIGGER_BASED_RACH
/* enhanced logging mechanism for extended debugging */
extern uint32 mac_hs_rach_debug_trace_level;

#define MAC_HS_RACH_MSG_ERROR   0x01
#define MAC_HS_RACH_MSG_MED     0x02
#define MAC_HS_RACH_MSG_LOW     0x04

#define MSG_HIGH_HS_RACH  MSG_HIGH
#define MSG_ERROR_HS_RACH   (((mac_hs_rach_debug_trace_level & MAC_HS_RACH_MSG_ERROR) > 0) ? ERR_FATAL : MSG_ERROR);
#define MSG_MED_HS_RACH   (((mac_hs_rach_debug_trace_level & MAC_HS_RACH_MSG_MED) > 0) ? MSG_HIGH : MSG_MED);
#define MSG_LOW_HS_RACH   (((mac_hs_rach_debug_trace_level & MAC_HS_RACH_MSG_LOW) > 0) ? MSG_HIGH : MSG_LOW);
#endif

/* Public portion of the API exists in wcdmamvsif.h */
#include "wcdmamvsif.h"
#include "wl2api.h"

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
#define MAC_LB_MAX_TTI_DELAY 10
#define MAC_CFN_INCREMENT 10
#endif
/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */
extern boolean         mac_gcf_test_flag;
extern rex_crit_sect_type e_ul_isr_l2_mutex;

extern rex_crit_sect_type e_ul_isr_l2_datapath_mutex;
extern rex_crit_sect_type r99_ul_isr_l2_datapath_mutex;
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
extern rex_crit_sect_type rlc_reset_table_sem;
extern rex_crit_sect_type mac_cipher_param_mutex;
#endif

extern rex_crit_sect_type eul_rlc_wm_enq_mutex;

extern uint16 tvm_debug;

#ifdef FEATURE_WCDMA_HS_RACH
extern    rex_crit_sect_type mac_hsrach_tb_timer_sem;
#endif /* FEATURE_WCDMA_HS_RACH */

extern boolean mac_ul_rach_abrt_rcvd;

/* Take the UL and DL Mutex */
#define L2_ACQ_UL_DL_LOCK()           REX_ISR_LOCK(&e_ul_isr_l2_mutex); \
                                    REX_ISR_LOCK(&dec_tsn_q_crit_sect)

/* Free the acquired UL and DL Mutex */
#define L2_ACQ_UL_DL_UNLOCK()         REX_ISR_UNLOCK(&e_ul_isr_l2_mutex); \
                                    REX_ISR_UNLOCK(&dec_tsn_q_crit_sect)
/* Take the DL  Mutex */
#define L2_ACQ_DL_LOCK()            REX_ISR_LOCK(&dec_tsn_q_crit_sect)
/* Free the acquired DL Mutex */
#define L2_ACQ_DL_UNLOCK()          REX_ISR_UNLOCK(&dec_tsn_q_crit_sect)

/* Take the UL  Mutex */
#define L2_ACQ_UL_LOCK()            REX_ISR_LOCK(&e_ul_isr_l2_mutex)
/* Free the acquired UL Mutex */
#define L2_ACQ_UL_UNLOCK()          REX_ISR_UNLOCK(&e_ul_isr_l2_mutex)

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  #define RLC_ACQ_RESET_TABLE_LOCK()          REX_ISR_LOCK(&rlc_reset_table_sem)
  #define RLC_ACQ_RESET_TABLE_UNLOCK()        REX_ISR_UNLOCK(&rlc_reset_table_sem)

  /* Take Mutex that protects ciphering related info */
  #define L2_ACQ_CIPHER_PARAM_LOCK()    REX_ISR_LOCK(&mac_cipher_param_mutex)
  /* Free  Mutex that protects ciphering related info */
  #define L2_ACQ_CIPHER_PARAM_UNLOCK()  REX_ISR_UNLOCK(&mac_cipher_param_mutex)

#else
  #define RLC_ACQ_RESET_TABLE_LOCK()
  #define RLC_ACQ_RESET_TABLE_UNLOCK()
  #define L2_ACQ_CIPHER_PARAM_LOCK()    {}
  #define L2_ACQ_CIPHER_PARAM_UNLOCK()  {}
#endif

#define L2_MUTEX_LOCK(x)    REX_ISR_LOCK(x)
#define L2_MUTEX_UNLOCK(x)  REX_ISR_UNLOCK(x)



#ifdef FEATURE_WCDMA_HS_RACH
  #define MAC_ACQ_TB_TIMER_LOCK()          REX_ISR_LOCK(&mac_hsrach_tb_timer_sem)
  #define MAC_ACQ_TB_TIMER_UNLOCK()        REX_ISR_UNLOCK(&mac_hsrach_tb_timer_sem)
#else
  #define MAC_ACQ_TB_TIMER_LOCK()          {}
  #define MAC_ACQ_TB_TIMER_UNLOCK()        {}
#endif /* FEATURE_WCDMA_HS_RACH */

#ifdef FEATURE_MAC_I

#define RLC_AM_MAX_7BIT_LI_SDU_LEN CONVERT_BYTES_TO_BITS(123)
#define RLC_UL_AM_DEFAULT_HDR_LEN_BITS (16)
#define RLC_UL_AM_7BIT_LI_E_LEN_BITS   (8)
#define RLC_UL_AM_DEFAULT_HDR_LEN_BYTES (2)
#define RLC_UL_AM_7BIT_LI_E_LEN_BYTES  (1)
#define RLC_UL_AM_15BIT_LI_E_LEN_BITS   (16)
#define RLC_UL_AM_15BIT_LI_E_LEN_BYTES  (2)
#define RLC_UL_AM_MIN_CTRL_PDU_LEN_BITS  (32)


#define MAC_HS_UL_MAC_I_PDU_SPECIAL_HE_POSSIBLE(li_len,max_pdu_size,special_he_flg) \
                      ((li_len == 1) && ((max_pdu_size) > RLC_AM_MAX_7BIT_LI_SDU_LEN) && (special_he_flg == TRUE))

#define MAC_UL_HS_UPD_FLEX_PARTIAL_PDU_INFO(flex_pdu_size_chosen_ptr,pdu_size,ab,partial_pdu_size) \
do\
{\
  flex_pdu_size_chosen_ptr->complete_pdu_size[flex_pdu_size_chosen_ptr->complete_pdu_idx] = pdu_size;\
  flex_pdu_size_chosen_ptr->num_complete_pdu[flex_pdu_size_chosen_ptr->complete_pdu_idx] = 1;\
  flex_pdu_size_chosen_ptr->partial_present = TRUE;\
  flex_pdu_size_chosen_ptr->partial_size = partial_pdu_size;\
  MAC_I_PER_LC_UPD_PDU_INFO(flex_pdu_size_chosen_ptr, partial_pdu_size);\
    ab -= partial_pdu_size;\
}while(0)

#define MAC_UL_HS_UPD_FLEX_PDU_SIZE(flex_pdu_size_chosen_ptr,pdu_size,ab) \
do\
{\
  flex_pdu_size_chosen_ptr->complete_pdu_size[flex_pdu_size_chosen_ptr->complete_pdu_idx] = pdu_size;\
  flex_pdu_size_chosen_ptr->num_complete_pdu[flex_pdu_size_chosen_ptr->complete_pdu_idx] = 1;\
  ab -= pdu_size;\
  MAC_I_PER_LC_UPD_PDU_INFO(flex_pdu_size_chosen_ptr, pdu_size);\
}while(0)

#endif

/*===================================================================

                       ENUMERATED TYPE DATA DEFINITIONS

====================================================================*/

//#define MAX_MAC_E_HDR_SIZE_BYTES  16
#define MAX_MAC_E_HDR_SIZE_BYTES  500

#define UM_PDU_HDR_SIZE_BITS 8

#define MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC               63

/*------------------------------------------------------------------*/
/* RLC Buffer occupancy information specification                   */
/*------------------------------------------------------------------*/

typedef enum {

    /* Buffer Occupancy specified as "PDU size & Number of PDUs" */
    FIXED_PDUS = 1,

    /* Buffer Occupancy specified as "Number of bytes" */
    FLEX_PDUS

}rlc_ul_buf_status_info_e_type;

#ifdef FEATURE_MAC_I
/*------------------------------------------------------------------*/
/*    MAC PDU type                                                  */
/*------------------------------------------------------------------*/
typedef enum {

   MAC_E_PDU = 0,
   MAC_I_PDU,
   MAC_R99_PDU,
   INVALID_MAC_PDU
} rlc_mac_ul_pdu_hdr_e_type;

typedef enum {
  MAC_I_COMPLETE_PDU = 0,
  MAC_I_PARTIAL_END,
  MAC_I_PARTIAL_START,
  MAC_I_PARTIAL_MIDDLE_OR_START_END,
  MAC_I_PARTIAL_INVALID
} mac_i_seg_status;

#endif /* FEATURE_MAC_I */

/*------------------------------------------------------------------*/
/* Enum for UL channel type R99(DCH) or EUL(EDCH)                   */
/*------------------------------------------------------------------*/

typedef enum {

    CHANNEL_DCH = 0,

    CHANNEL_EDCH,
    
    CHANNEL_MAX

}rlc_ul_channel_type;
/*===================================================================

                       STRUCTURE TYPE DATA DEFINITIONS

====================================================================*/
#ifdef FEATURE_MAC_I
 typedef struct 
 {
   boolean mac_is_sdu_seg_present;
 
   /* For MAC-i PDU, 1 byte should be added per LC for TSN/SS Hdr    */
   /* This will be added by MAC                                      */
 
   /* rlc_id on which partial is present */
   rlc_lc_id_type      rlc_id;
   uecomdef_logch_mode_e_type lc_mode;   /* RLC Mode                 */
 
   /* Remaining Partial PDU to be transmitted in Bytes */
   uint16              rem_partial_len_bytes;
   uint16              partial_offset;
 
 } rlc_ul_partial_info_type;

typedef struct
{
  uint16           pdu_size_bits;
  uint16           pdu_sn;
} rlc_retx_pdu_info_type;

typedef struct
{
  uint8            num_pdu;
  rlc_retx_pdu_info_type    rlc_retx_pdu_info[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
} rlc_flex_retx_pdu_info_type;

extern rlc_flex_retx_pdu_info_type *rlc_flex_retx_pdu_info[];

#endif /* FEATURE_MAC_I */

#define RLC_NUM_PROFILE_SAMPLES 30

typedef struct
{
  int32 frame_qty_start;
  int32 cx8_qty_start;
  int32 frame_qty_end;
  int32 cx8_qty_end;
  uint32 add_on1;
  uint32 add_on2;
  uint16 add_on3;
}l2_rlc_profile_data_param_type;

typedef struct
{
  uint32 dump_idx;
  l2_rlc_profile_data_param_type profile_array[RLC_NUM_PROFILE_SAMPLES];
}l2_rlc_profile_data_struct_type;

void rlc_profile_dump_start(l2_rlc_profile_data_struct_type *profile_dump);
void rlc_profile_dump_end(l2_rlc_profile_data_struct_type *profile_dump);

/*------------------------------------------------------------------*/
/* Buffer occupancy information needs to be reported only for the
channels that MAC is requesting for  */
/*------------------------------------------------------------------*/
typedef struct
{
#ifdef FEATURE_MAC_I
  /* MAC-E/MAC-r99 or MAC-I pdu type */
  rlc_mac_ul_pdu_hdr_e_type  mac_ul_pdu_type;
#endif /* FEATURE_MAC_I */

  /* Number of channels for which MAC need the buffer status */
  uint8                     nchan;

  /* LC ids for which MAC needs the buffer status */
  rlc_lc_id_type            rlc_id[UE_MAX_UL_LOGICAL_CHANNEL];

}rlc_ul_lc_info_type;


/*------------------------------------------------------------------*/
/* Loopback delay information of a single UL RLC logical channel  */
/*------------------------------------------------------------------*/

/*
possible TTI info values are :
{
  L1_TTI_10MS,-->1
  L1_TTI_20MS,-->2
  L1_TTI_40MS,-->4
  L1_TTI_80MS,-->8
  L1_NUM_TTI  -->
} 
*/

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
typedef struct
{
  boolean lb_configured_flag;
  uint16  loop_back_constant_delay;
  uint32  ul_cfn;
  uint16  lb_trch_tti_info;
}rlc_ul_lb_lc_info_type;
#endif /* FEATURE_TC_CONSTANT_DELAY_ENH */


/*------------------------------------------------------------------*/
/* Buffer occupancy information of a single UL RLC logical channel  */
/*------------------------------------------------------------------*/

typedef struct
{
    /* Buffer occupancy  (AM/UM-FLEX_PDUS, TM-FIXED_PDU)            */
    rlc_ul_buf_status_info_e_type   type;

    /* No. of PDUs. Only applicable to TM(FIXED_PDUs)               */
    uint16  npdus;

    /* PDU Size in bits - Valid for TM                              */
    uint16  pdu_size;

    uint16 num_sdu;

    uint16 rem_deq_sdu_size;
    /* This parameter is kept for backward compatibility. Used only */
    /* by MAC during TFCI selection algorithm. RLC will not fill    */
    /* this parameter.                                              */
    uint32  nbytes;

    /* No. of bytes in UL- WM + No. of LIs. Doesn't include RLC-hdr.*/
    /* This appilcable for both AM & UM.                            */
    /* No. of bytes in UL- WM + No. of LIs(SDUs) + Special LI's     */
    /* For MAC-e/MAC-r99, doesn't include RLC-Hdr per PDU           */
    /* For UM channel, if alt_e_bit = TRUE,doesn't include LI's(SDU)*/
    /* For MAC-i PDU, include new data + MAC-i Hdr's per RLC-PDU    */
    /* MAC-i Hdr's = (ceil(new_data_bytes/largetst_pdu_size) * 2)   */
    uint32   new_data_bytes;

    uint32   new_data_uncomp_bytes;
    
    /* The following parameters are applicable to only AM           */
    /* No. of bytes of retx PDUs. It includes LI's and payload.     */
    /* For MAC-e PDU/MAC-r99 PDU, does not include RLC-hdr.         */
    /* For MAC-i PDU,include RLC-Hdr's + MAC-i Hdr's per RLC-PDU    */
    uint32   retx_data_bytes;

    /* Total number of bytes of control/status SUFI.                */
    /* For MAC-e/MAC-r99 PDU, it is not rounded to the pdu sizes    */
    /* i.e. if the control PDU  has 4 bytes of data, then           */
    /* ctrl_data_bytes = 4 rather than  (pdu_size - 2))             */
    /* For MAC-i PDU, include control bytes + MAC-i Hdr's           */
    /* MAC-i Hdr's = (ceil(ctrl_data_bytes/largetst_pdu_size) * 2)  */
    uint32   ctrl_data_bytes;

    /* For MAC-e/MAC-R99 PDU,available RLC PDU window size for LC   */
    /* For MAC-i PDU, No. of new data bytes allowed to send         */
    /* (effect_win_size * largest_pdu_size) + MAC-I Hdr's           */
    /* MAC-i Hdr's = (ceil(ctrl_data_bytes/largetst_pdu_size) * 2)  */
    uint16   effect_win_size;

    /* Flag to block the PDU building as the RLC is in the internal */
    /* states transition. This is applicable to both AM & UM        */
    boolean suspend_pdu_build_f;

    boolean sp_li_flg;
#ifdef FEATURE_MAC_I
    /* 0 -Indicates, there is no  pending MAC-Is-SDU segment        */
    /* X -Indicates, size of the MAC-segment + MAC-i hdr(LCH+L+F)   */
    /* Priority should be given for the LC with MAC segment         */
    uint32 mac_is_sdu_seg_size;

    /* For MAC-i PDU, 1 byte should be added per LC for TSN/SS Hdr  */
    /* This will be added by MAC                                     */
    rlc_pdu_size_type  rlc_pdu_type;
#endif /* FEATURE_MAC_I */

    boolean li_start_sdu_set;              

}rlc_ul_lc_buf_status_type;

/*------------------------------------------------------------------*/
/* Buffer occupancy information of all uplink RLC logical channels  */
/*------------------------------------------------------------------*/

typedef struct

{
    /* Buffer occupancy specification (FLEX_PDUS, FIXED_PDU) */
    rlc_ul_lc_buf_status_type chan_info[UE_MAX_UL_LOGICAL_CHANNEL];

}rlc_ul_buf_status_type;


/*------------------------------------------------------------------*/
/* Format of a single uplink RLC logical channel                    */
/*------------------------------------------------------------------*/

typedef struct {

    /* Contains TSN for this LC.This has to be written to HW as is.
    This value if given to RLC from MAC*/
    uint8           mac_es_hdr[4];
    uint8           mac_es_hdr_size;  /* Indicates the TSN size in bits.    */

    /* RLC logical channel ID */
    rlc_lc_id_type  rlc_id;

    /* Number of PDUs */
    /* This is applicable only for Fixed PDU size(MAC-e OR Mac-i) */
    uint16          npdus;

    /* PDU Size in bits */
    /* This is applicable only for Fixed PDU size(MAC-e OR Mac-i) */
    uint16          pdu_size[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];

#ifdef FEATURE_MAC_I
    uint16          num_ctrl_pdu;
    uint16          num_retx_pdu;
    uint16          num_new_data_pdu;
    mac_i_seg_status   mac_segment_needed;
    uint16          ps_size_bits;
    uint16          ps_offset_bits;
    boolean         pe_pm_middle;
    uint16          pe_pm_size_bits;
    uint16          pe_pm_offset_bits;
    uint16          retx_sn[MAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC];
#endif /* FEATURE_MAC_I */

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
    rlc_ul_lb_lc_info_type   loop_back_lc_info;
#endif
    uint8    carr_indx_info;  
}rlc_ul_logchan_format_type;

/*------------------------------------------------------------------*/
/* Uplink frame format of all RLC logical channels                  */
/*------------------------------------------------------------------*/

typedef struct {
  uint8   nchan;              /* Number of logical channels */
  
  uint8  mac_e_hdr[MAX_MAC_E_HDR_SIZE_BYTES];
                              /* Contains (DDIi, Ni) sets for i = 1..8 LCs */
                              /* including optional DDI0. This is given in */
                              /* the word format written to the ERAM bank       */
                              /* This is valid only for MAC-E PDU          */

  uint16  mac_e_hdr_size;     /* Size of mac_e header in bits. RLC is      */
                              /* expected to compute the number of words   */
                              /* that has to be written from mac_e_hdr     */
                              /* array to eram bank                        */
                              /* This is valid only for MAC-E PDU          */

  uint16  si_size;            /* 0 – Indicates SI field is not present     */
                              /* X – Indicates size of SI field in bits    */

  uint32  si;                 /* Contains the optional SI information. RLC */
                              /* is expected to write this as it is to ERAM*/

  uint32  padding_size;       /* 0 – no padding in E-DCH transport block   */
                              /* X – Indicates padding is present.         */
                              /* Note:                                     */
                              /* 1. If padding is present, the last cipher */
                              /* control word is prepared for padding along*/
                              /* with indication for last_unit_indicator.  */
                              /* 2. If padding is not present the last     */
                              /* cipher control word is prepared only for  */
                              /* indicating last_unit_indicator.           */

  l1_l2_data_type *l1_ul_tx_buf;
                              /* Pointer to current UL buffer to store the */
                              /* IO vectors for the payloads.              */

#ifdef FEATURE_MAC_I
  rlc_mac_ul_pdu_hdr_e_type  mac_pdu_type;
                              /* MAC-e or MAC-i pdu type                   */
#endif /* FEATURE_MAC_I */

    /* Format of each logical channel */
    rlc_ul_logchan_format_type  chan_fmt[UE_MAX_UL_LOGICAL_CHANNEL];

}rlc_ul_frame_format_type;


/*------------------------------------------------------------------*/
/* Information of a single uplink RLC PDU data                      */
/*------------------------------------------------------------------*/

typedef struct {

    /* Pointer to RLC PDU data */
    dsm_item_type   *pdu_ptr;

    /* 32-bit COUNT-C value used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32          count_c;

    /* Ciphering key ID and is used by ciphering engine */
    /* Valid only  when "cipher_flag = TRUE" */
    uint32          cipher_key_id;

    /* The ciphering algorithm can be either UE_0,  UE_1 or UE_2*/
    uecomdef_wcdma_cipher_algo_e_type cipher_algo;

    /* TRUE indicates if this PDU needs to be ciphered. FALSE in the case
    of control pdu which need not be ciphered */
    boolean         pdu_cipher_enable;

}rlc_ul_pdu_data_type;

/*------------------------------------------------------------------*/
/* PDU data of a single uplink logical channel                      */
/*------------------------------------------------------------------*/

typedef struct {

    /* Information of each PDU */
    rlc_ul_pdu_data_type    pdu_info[UE_MAX_UL_RLC_PDUS];

    /* Radio Bearer ID */
    rb_id_type      rb_id;

    /* Number of RLC PDUs */
    uint16          npdus;

    /* RLC PDU Size in bits */
    uint16          pdu_size;

    /* RLC  ID */
    rlc_lc_id_type  rlc_id;

    /* If ciphering is to be done, this flag is set to TRUE */
    /* Otherwise, it is set to FALSE */
    /* For TM mode logical channels,MAC decides this value */
    /* Otherwise, it is decided by RLC */
    boolean         cipher_flag;


    /* Bit offset from the start of the RLC PDU */
    /* Valid only when "cipher_flag = TRUE" */
    uint8           cipher_offset;


}rlc_ul_logchan_data_type;

/*------------------------------------------------------------------*/
/* PDU data of the current uplink frame                             */
/*------------------------------------------------------------------*/

typedef struct {

    /* Number of RLC logical channels */
    uint8   nchan;

    /* PDU data of each logical channel */
    rlc_ul_logchan_data_type  chan_info[UE_MAX_UL_LOGICAL_CHANNEL];

}rlc_ul_frame_data_type;



/*------------------------------------------------------------------*/
/* Header information of a single downlink transport block          */
/*------------------------------------------------------------------*/

typedef struct {

    /* Flag to discard the DL PDUs received while the RESET ACK was being processed */
    boolean         discard_l2_pdu;
    
    /* Downlink RLC logical channel identity */
    rlc_lc_id_type  rlc_id;

    /* Bit offset of start of RLC PDU from start of transport block
    header */
    uint8           rlc_pdu_start_offset;

    /* Pointer to start of transport block header data */
    uint8           *hdr_ptr;

}rlc_dl_tb_hdr_info_type;

/*------------------------------------------------------------------*/
/* Ciphering information of a single downlink transport block       */
/*------------------------------------------------------------------*/

typedef struct {

    /* If ciphering is to be done, it is set to TRUE */
    /* Otherwise, it is set to FALSE */
    boolean     cipher_flag;

    /* Ciphering algorithm */
    uecomdef_wcdma_cipher_algo_e_type ciphering_algo;

    /* Bit offset of ciphering from start of transport block header */
    /* Valid only when "cipher_flag = TRUE" */
    uint8       cipher_offset;

    /* Ciphering key ID used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32      cipher_key_id;

    /* 32-bit COUNT-C value used by ciphering engine */
    /* Valid only when "cipher_flag = TRUE" */
    uint32      count_c;

    /* Uplink Radio bearer ID used by ciphering engine */
    rb_id_type  rb_id;

}rlc_dl_tb_cipher_info_type;


/*------------------------------------------------------------------*/
/* Downlink RLC PDUs                                                */
/*------------------------------------------------------------------*/
typedef struct {

    /* RLC logical channel identity */
    rlc_lc_id_type  rlc_id;

    /* RLC PDU size in bits */
    uint16          pdusize;

    /* Pointers to downlink RLC PDUs */
    dsm_item_type   *pdu_ptr;

}rlc_dl_pdu_data_type;

/*------------------------------------------------------------------*/
/* Downlink RLC PDUs of entire frame                                */
/*------------------------------------------------------------------*/
typedef struct {

    /* Number of RLC PDUs */
    uint8   npdus;

    /* RLC PDUs of each valid downlink RLC logical channel */
    rlc_dl_pdu_data_type  pdu_info[UE_MAX_DL_RLC_PDUS];

}rlc_dl_frame_data_type;

typedef struct{

//To make sure that either one of ul-task or ISR can used this DS at a time
  boolean in_use;

  /* Number of channels for which status report has been requested for */
  uint8  nchan;
  /* Channels ids of the logical channels for which status has to be
  reported */
  rlc_lc_id_type rlc_id_list[UE_MAX_UL_LOGICAL_CHANNEL];

} rlc_status_report_req_type;

#define UE_OPT_MAX_DL_RLC_PDUS 32

typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 num_complete_pdus;
  boolean decode_result;
#ifndef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3
  complete_pdu_type complete_pdu[UE_MAX_DL_RLC_PDUS];
#else
  complete_pdu_type complete_pdu[UE_OPT_MAX_DL_RLC_PDUS];
#endif
} rlc_enh_data_ind_type;

#ifndef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3
#define RLC_MAX_TTI_DATA_DL 30
#else
#define RLC_MAX_TTI_DATA_DL 10
#endif
/* Q size to store sdu len. NOTE: Should be power of 2 */
#define RLC_MAX_DSM_ITEM_PER_TTI_VAL 60
#define RLC_MAX_DSM_ITEM_PER_PDU_VAL 30

#define RLC_MAX_NUM_SDU_LEN_INFO_Q_SIZE   (RLC_MAX_DSM_ITEM_PER_TTI_VAL)

typedef struct
{
  uint16 sdu_len_q[RLC_MAX_NUM_SDU_LEN_INFO_Q_SIZE];
  uint8 num_sdu;
}rlc_ul_sdu_len_q_info;

/*===================================================================

                            FUNCTION DEFINITIONS

====================================================================*/

/*-------------------------------------------------------------------
FUNCTION rlc_ul_enh_buffer_status

DESCRIPTION
  This function exists in RLC. MAC will call always this function
  for the BO, irrespective of UE capability and LC type i.e R99
  LC, E-DCH mapped LC.

--------------------------------------------------------------------*/
extern void rlc_ul_enh_buffer_status
(
  rlc_ul_lc_info_type    *lc_info_ptr,  /* Report buffer status for */
                                        /* only following channels  */
  rlc_ul_buf_status_type *status_ptr,    /* Pointer to UL Buf.Status */
  
  boolean           query_from_bfi_flag,  
  boolean           *rlc_wait_for_ack   
);
/*===========================================================================
FUNCTION RLC_CHECK_PENDING_DL_ACTIVE_RB_DATA

DESCRIPTION
  Checks if there are any PDUs yet to be received or processed
  for all active DL RB's.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if more PDUs are expected for a SDU
  FALSE - otherwise 
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rlc_check_pending_dl_active_rb_data(void);

/*-------------------------------------------------------------------
FUNCTION rlc_ul_build_pdus

DESCRIPTION
  This function exists in RLC. MAC uses it to get RLC PDUs for a
  given frame

  RLC builds RLC PDUs as per the MAC-specified format (*fmtptr)
  and returns the frame data information

--------------------------------------------------------------------*/
extern    void  rlc_ul_build_pdus
(
    /* Pointer to frame format data */
    rlc_ul_frame_format_type  *fmtptr,

    /* Pointer to uplink frame data */
    rlc_ul_frame_data_type      *ul_data_ptr
);

/*===========================================================================

FUNCTION RLC_UL_ENH_BUILD_PDUS

DESCRIPTION
  This function is executed in RLC. This is called to build pdus of
  logical channels that are mapped E-DCH transport channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void  rlc_ul_enh_build_pdus
(
  rlc_ul_frame_format_type *ul_fmt_ptr /* Pointer to frame format data */
);

#define rlc_discard_dsm_item(item_ptr) \
             rlci_discard_dsm_item(item_ptr, __FILENAME__, __LINE__)
/*===========================================================================

FUNCTION rlci_discard_dsm_item

DESCRIPTION
    Discard the DSM Item as follows
    If the app_field is greater than zero, decrement it.
    If the resulting app_field is zero, 
       - DISCARD the DSM item by calling dsm_free_packet
    Otherwise
       - Nothing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_discard_dsm_item
(
 dsm_item_type **dsm_ptr,
 const char * file,
 uint32 line
);

/*-------------------------------------------------------------------
FUNCTION rlc_dl_decode_pdu_hdr

DESCRIPTION
  This function exists in RLC. MAC uses it to get RLC ciphering
  parameters of a downlink transport block

  Depending on RLC header of the transport block, RLC returns ciphering
  information.

  This function is called in interrupt context of Physical Layer.

--------------------------------------------------------------------*/
extern  boolean  rlc_dl_decode_pdu_hdr
(
    /* Pointer to transport block header information */
    rlc_dl_tb_hdr_info_type     *hdrptr,

    /* Pointer to the ciphering info */
    rlc_dl_tb_cipher_info_type  *cipherptr
);

/*-------------------------------------------------------------------
FUNCTION rlc_dl_pdu_data

DESCRIPTION
  This function exists in RLC and is used by MAC to indicate downlink
  PDUs

--------------------------------------------------------------------*/
extern  void    rlc_dl_pdu_data_ind
(
    /* Pointer to DL Transport Block Data */
    rlc_dl_frame_data_type  *dataptr
);

/*-------------------------------------------------------------------
FUNCTION mac_ul_tx_trigger_req

DESCRIPTION
  This function exists in MAC. RLC uses it to inform MAC that there is
  (are) SDU(s) in its uplink queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
extern void  mac_ul_trigger_tx_req
(
#ifdef FEATURE_DSM_WM_CB
  dsm_watermark_type* wm_ptr,
  void*               func_data
#else
  void
#endif
);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_DL_SET_CIPHER_LOGGING

DESCRIPTION
This function sets signal to log the cipher PDU packets in task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void rlc_dl_set_cipher_logging
(
  void
);
#endif

/*===========================================================================

FUNCTION rlc_enh_get_free_data_q_entry

DESCRIPTION
    - Returns entry from rlc_enh_free_data_q.
      If Quue is empty, NULL will be returned

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

rlc_enh_data_ind_type *rlc_enh_get_free_data_q_entry(void);

/*===========================================================================

FUNCTION rlc_enh_add_to_free_data_q_entry

DESCRIPTION
    - Adds the rlc_enh_data entry into Free Data Queue rlc_enh_free_data_q

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_add_to_free_data_q_entry(rlc_enh_data_ind_type *rlc_enh_data_ind_ptr);

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_ind

DESCRIPTION
    - Process TM data, if present and configured to process in interrupt context
    - If some data present,
        - Enqueue the RLC entry into rlc_enh_data_queue
        - Post a signal to RLC DL Task to process in DL Task context
      else
        - Return the entry to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_ind(rlc_enh_data_ind_type *rlc_enh_data_ptr);


/*===========================================================================

FUNCTION rlc_enh_calc_rlc_hdr_length

DESCRIPTION
    - Return the RLC header length, which is
        2 for AM Data PDU, 0 for AM Ctrl PDU, 1 for UM and 0 for TM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 rlc_enh_calc_rlc_hdr_length(rlc_lc_id_type rlc_id, uint8 first_byte,
  boolean *is_tm_mode);

/*===========================================================================
FUNCTION: mac_hs_enqueue_free_tsn

DESCRIPTION:
  Enqueue the tsn_info_buf to Free queue

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_enqueue_free_tsn(l1_dec_hs_tsn_info_type *tsn_info_buf_ptr);

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_ind_hs

DESCRIPTION
    - Enqueue the tsn entry into rlc_tsn_info_q
    - Post a signal to RLC DL Task to process in DL Task context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_ind_hs(l1_dec_hs_tsn_info_type *tsn_info_buf_ptr);

/*===========================================================================

FUNCTION rlc_enh_dl_tsn_flush_hs

DESCRIPTION
    - Flush the TSN from TSN q
    - 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  There might be slight data loss dueing HS STOP

===========================================================================*/
 void rlc_enh_dl_tsn_flush_hs(void);

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*===========================================================================
FUNCTION: mac_ehs_query_hs_type

DESCRIPTION:
  returns HS Call type - HS or EHS

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
mac_hs_e_type mac_ehs_query_hs_type(void);

#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

/*===========================================================================
FUNCTION: MAC_UL_GET_CHANNEL_TYPE

DESCRIPTION:
  API to map UL RLC LC ID to channel type

DEPENDENCIES:
  None

RETURN VALUE:
  channel type

SIDE EFFECTS:
  None
===========================================================================*/
rlc_ul_channel_type mac_ul_get_channel_type(rlc_lc_id_type rlc_id);
                      
/*===========================================================================
FUNCTION: L2_ACQ_UL_LW_LOCK

DESCRIPTION:
  API to acquire UL data path lock based on RLC LC id passed.

DEPENDENCIES:
  None

RETURN VALUE:
  Mutex locked R99/EUL.

SIDE EFFECTS:
  None
===========================================================================*/
rex_crit_sect_type* L2_ACQ_UL_LW_LOCK(rlc_lc_id_type rlc_id);

/*===========================================================================
FUNCTION: L2_ACQ_UL_LW_UNLOCK

DESCRIPTION:
  API to release UL data path lock passed.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void L2_ACQ_UL_LW_UNLOCK(rex_crit_sect_type* datapath_mutex);

typedef enum
{
  RLC_DEBUG_NONE,
  RLC_DEBUG_VAR,
  RLC_DEBUG_FLAG,
  RLC_DEBUG_LIMIT_NUM_SDU_PER_TTI,
  RLC_DEBUG_ENB_BUF_STATUS
 ,RLC_CHANGE_UL_COMPR_PARAMETERS = 5
 ,RLC_SEND_COMPRESSED_UNCOMPRESSED_PACKETS
 ,RLC_ENABLE_DISABLE_UL_COMPRESSION
 ,RLC_CHANGE_UL_COMP_BO_THRESHOLDS
 ,RLC_CHANGE_DACC_CONFIG_PARAMS
 ,RLC_ENABLE_UL_COMPR_RESOURCE_CRUNCH = 10
 ,RLC_UL_COMP_INDUCE_PTP_NAK
 ,RLC_INDUCE_BAD_RLC_NAK
 ,RLC_DEBUG_SDU_PDU_DUMP
 ,RLC_CHANGE_STMR_TICKS_PER_COMP_USAGE 
 ,RLC_UL_COMP_TEMP_2 = 15
 ,RLC_UL_COMP_TEMP_3
 ,RLC_UL_COMP_TEMP_4 
 ,RLC_ULC_LOGGING_DEBUG
}wcdma_rlc_debug_var_type;


/*---------------------------------------------------------------------------
 *   Enum Flag used to determine the type of logging in HS path

 *   MAC_HS_DBG_LOGGING_ONLY: Extra logging in HS path; No extra check for validity of TSNs
 *   MAC_HS_DBG_VLDT_TSN: Extra logging and check for validity of TSNs in HS path
 *-------------------------------------------------------------------------*/

typedef enum
{
  MAC_HS_DBG_NO_LOGGING,
  MAC_HS_DBG_LOGGING_ONLY,
  MAC_HS_DBG_VLDT_TSN
}mac_hs_dbg_enum_type;

extern uint8 mac_ul_debug_print_interval;

extern boolean mac_enable_tfci_debug_dump;
extern boolean mac_enable_low_medium_f3s;

extern mac_hs_dbg_enum_type mac_hs_logging;

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
extern uint32 mac_ehs_logging;
extern uint32 mac_ehs_p_to_c_logging;
#endif

typedef enum
{

  MAC_DEBUG_NONE = 0,
  MAC_ENABLE_TFCI_DEBUG_DUMP = 1, 
  MAC_ENABLE_LOW_MEDIUM_F3S = 2,

  MAC_DL_DEBUG_PRINT_INTERVAL = 3, 
  MAC_UL_DEBUG_PRINT_INTERVAL = 4, 

  MAC_HS_LOGGING = 5, 

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  MAC_EHS_LOGGING = 6, 
  MAC_EHS_P_TO_C_LOGGING = 7,
#endif

#ifdef FEATURE_WCDMA_HS_RACH
  MAC_HS_RACH_LOGGING = 8,
#endif

  MAC_TVM_LOGGING = 9

#if ((defined FEATURE_WCDMA_EVT6D_ENH) && (defined FEATURE_WCDMA_DC_HSUPA))
 ,MAC_EUL_EVENT_6D_SUP_THRSHLD   = 10,
  MAC_EUL_EVENT_6D_CONSEC_TTI    = 11,
  MAC_EUL_EVENT_6D_SUP_REDUCTION = 12,
  MAC_EUL_EVENT_6D_ENABLE        = 13
#endif
}wcdma_mac_debug_var_type;

void wcdma_mac_set_dbg_var_through_qxdm
(
  uint8 debug_var_type,
  uint32 debug_var_val
);
void wcdma_rlc_set_dbg_var_through_qxdm
(
  uint8 debug_var_type,
  uint32 debug_var_val
);

/*===========================================================================
FUNCTION: L2_ENH_ACQ_UL_LW_UNLOCK

DESCRIPTION:
  API to release UL data path lock based on RLC LC id passed.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
rex_crit_sect_type* L2_ENH_ACQ_UL_LW_UNLOCK(rlc_lc_id_type rlc_id);

#ifdef FEATURE_MAC_I
/*===========================================================================

FUNCTION MAC_HS_UL_GET_PDU_TYPE

DESCRIPTION
  Wrapper API to read the value of mac_hs_ul_pdu_type global

DEPENDENCIES
  None.

RETURN VALUE
MAC_E_PDU_HDR/MAC_I_PDU_HDR/INVALID_MAC_PDU_HDR

SIDE EFFECTS
  None.

===========================================================================*/
mac_ul_pdu_hdr_e_type mac_hs_ul_get_pdu_type();

/*===========================================================================

FUNCTION MAC_RLC_QUERY_PARTIAL_SN_PDU_ACKED_STATUS

DESCRIPTION

  API to find out whether pending partial Re-tx PDU is valid or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Pending Partial PDU is valid.
  FALSE - Pending Partial PDU is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mac_rlc_query_partial_sn_pdu_acked_status(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION RLC_UL_CLEAR_ALL_MAC_I_CTRL_DSM_PTR

DESCRIPTION

  API to clear all the Control PDU DSM Pointer..

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_clear_all_mac_i_ctrl_dsm_ptr(void);

/*===========================================================================
FUNCTION: RLC_MAC_GET_LI_SIZE_SPECIAL_HE_FLG

DESCRIPTION:
  API used to get the LI length, special HE flag in case of AM and ALT E flag in case UM entity for the 
  given RLC ID.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void rlc_mac_get_li_size_special_he_flg(rlc_lc_id_type rlc_id, 
                                        uint8 *li_len, boolean *special_he_flg, boolean *alt_e_cfg);
/*===========================================================================

FUNCTION MAC_RLC_QUERY_RE_TX_QUEUE_STATUS

DESCRIPTION

  API to find out whether pending partial PDU is valid or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Pending Partial PDU is valid.
  FALSE - Pending Partial PDU is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mac_rlc_query_re_tx_queue_status(rlc_lc_id_type rlc_id);
/*===========================================================================

FUNCTION RLC_UL_CLEAR_MAC_I_CTRL_DSM_POOL_PER_LC

DESCRIPTION

  Clears the control PDU DSM pool for the passed RLC-ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_clear_mac_i_ctrl_dsm_pool_per_lc(rlc_lc_id_type rlc_id);

void mac_hs_ul_clear_mac_i_partial_info_per_lc(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION RLC_UL_AM_READ_SDU_Q_PTR

DESCRIPTION

  This API RLC SDU queue pointer which contains the length of the SDU which are 
  in UL watermark queue.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns RLC SDU Q pointer.
 
SIDE EFFECTS
  None.

===========================================================================*/
rlc_ul_sdu_len_q_info *rlc_ul_am_read_sdu_q_ptr(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION RLC_UL_UPD_SDU_LEN_Q_BASED_ON_PDU_SIZE

DESCRIPTION

  This API updates the RLC SDU length queue based upon the PDU Size selected and 
  returns the SDU length of next data in UL WM to be built.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns length of the next SDU in UL WM queue to be built.
 
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_upd_sdu_len_q_based_on_pdu_size(uint8                *num_sdu,
                                              uint32               *new_data_bytes,
                                              boolean                *sp_li_flg,
                                              uint16                 rlc_pdu_size,
                                              uint16                 cur_sdu_size,
                                              rlc_ul_sdu_len_q_info *rlc_ul_sdu_len_q_ptr);

/*===========================================================================

FUNCTION RLC_UL_AM_DUMP_SDU_LEN_Q

DESCRIPTION

  This API used dump the conten of SDU lenght queue.
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_dump_sdu_len_q(rlc_ul_sdu_len_q_info *rlc_ul_sdu_len_q_ptr);

/*===========================================================================

FUNCTION RLC_UL_UPDATE_CONFIG_PENDIG

DESCRIPTION

  This API used to update the RLC UL AM configuration status.
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void   rlc_ul_update_config_pending(boolean status);

/*===========================================================================

FUNCTION RLC_UL_INIT_CTRL_DSM_POOL

DESCRIPTION

  Initializes the control PDU DSM pool.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_init_mac_i_ctrl_dsm_pool(void);

/*===========================================================================

FUNCTION RLC_UL_AM_POLL_NEXT_PDU

DESCRIPTION
  This function sets poll_next to TRUE and poll_prohibit to FALSE, if PS had poll bit set to TRUE
  
DEPENDENCIES
  Should only be called for AM channels.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_am_poll_next_pdu(rlc_lc_id_type rlc_id);

#endif /* FEATURE_MAC_I */

#ifdef FEATURE_WCDMA_UL_COMPR
void rlc_ptp_ul_comp_sdu_in_advance(uint8 rlc_stmr_event_id);
#define mcalwcdma_evt_rlc_ptp_ul_comp_sdu_in_advance(a) rlc_ptp_ul_comp_sdu_in_advance(a)
#else
#define mcalwcdma_evt_rlc_ptp_ul_comp_sdu_in_advance(a) 
#endif

/*remapping function definitions in l1_l2 to mac_rlc
The original functions are actually defined in l1
*/
#define mac_rlc_build_cipher_engine_ctrl_info l1_l2_build_cipher_engine_ctrl_info
#define mac_rlc_build_copy_engine_ctrl_info l1_l2_build_copy_engine_ctrl_info
#define mac_rlc_build_copy_engine_ctrl_info_with_index l1_l2_build_copy_engine_ctrl_info_with_index
#define mac_rlc_update_ciph_engine_start_keystream_offset l1_l2_update_cipher_key_stream_offset_in_bytes
#define mac_rlc_get_ciph_engine_start_keystream_offset l1_l2_get_cipher_key_stream_offset_in_bytes

#define mac_rlc_write_data_section l1_l2_write_data_section
#define mac_rlc_write_hdr_section l1_l2_write_hdr_section

extern uint32 eulenc_pdu_hdr_len_bits_fn(uint16 idx);

extern uint32 eulenc_pdu_data_len_bits_fn(uint16 idx);

extern uint32 eulenc_pdu_ofst_len_bits_fn(uint16 idx);

void eulenc_update_ciphering_key_offset(uint16 offset_in_bytes);
extern uint16 rlc_ul_update_temp_deq_idx_num_sdu(rlc_ul_sdu_len_q_info *rlc_ul_sdu_q, 
                                                 uint16 *deq_idx, uint16 *num_sdu,
                                                 uint32 rlc_pdu_size,uint8 li_len, 
                                                 uint32 *new_data_bytes);

#ifdef  FEATURE_WCDMA_TRIGGER_BASED_RACH
/*==============================================================================================
 FUNCTION NAME  mac_ul_trigger_tx_req
 DESCRIPTION  This function is called by RLC whenever there is data queued in RLC buffers
        RLC notifies MAC to trigger L1 for PRACH enable
===============================================================================================*/
void  mac_rach_ul_trigger_tx_req(l1_ul_phychan_ctrl_type ul_req_type);
void  mac_check_for_bfi_disable_trigger(void);
void rlci_enh_ul_hsrach_flush_tm_pdus(rlc_lc_id_type  rlc_id);

/*===========================================================================

FUNCTION RLC_UL_REGISTER_TRIGGER_BASED_RACH_API_ACTIVE_RBS

DESCRIPTION
  Register trigger based RACH API for all the active RB's.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void rlc_ul_register_trigger_based_rach_api_active_rbs(void);
#endif //* FEATURE_WCDMA_TRIGGER_BASED_RACH *//

void rlc_ul_call_data_ind_in_fach_cb(void);


extern e_tti_enum_type mac_eul_tti_is_2ms(void);

#ifdef FEATURE_WCDMA_UL_COMPR
extern boolean rlc_ptp_debug_var;
#endif
#endif