#ifndef RLCUL_H
#define RLCUL_H
/*===========================================================================
                  U P L I N K  R L C  I N T E R N A L
                      
                         H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  Uplink RLC module.
  
Copyright (c) 2001-03 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2004-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcul.h_v   1.29   09 Jul 2002 17:31:26   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlcul.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when      who     what, where, why
--------  ---     ---------------------------------------------------------- 
12/04/15  kc      Avoid taking lock on EUL ISR during ULC enable/disable
10/12/15  scv     Prevent taking DACC Mutex after EUL ISR Mutex in Task context
10/12/15  scv     Changes for DSM Multiple DeQ/EnQ
10/09/15  kv      To break out of DACC - ISR and release DACC mutex, if datapath flag is TRUE
10/08/15  kv      Changes to acquire datapath mutex before PTP msg processing in DL context
08/05/15  kc      Adding new DACC interface changes
05/15/15  aa      RLC optimization to trigger Status PDUs on all AM channels after LTA
01/14/15  aa      Changes for new ULC log packet(0x41C5)
12/10/13  kc      Send status PDU for 3 TTIs to optimize DCH to FACH transition
11/24/14  scv     Forgive BAD ACK/NAK for first 200ms after Establishment/Re-Establishment
08/21/14  ts      FR22375: Fixed Medium compiler warnings in WCDMA on Bolt 2.0/2.1
07/01/14  aa      CFCM changes for RLC
06/19/14  kc      Mainline FEATURE_WCDMA_HS_RACH_DELAY_CU
04/07/14  scv     Changes to ensure graceful handling of a PTP message with invalid contents.
04/04/14  scv     Code changes for UL Compression Feature
01/28/14  scv     Code changes for UL Compression Feature
01/09/14  scv     Code changes for UL Compression Feature
01/06/14  kc      Data Indication to RRC during FACH state
12/20/13  grk     Moved timer callback processing from timer task to L2 UL 
                  task for TM timer based SDU discard.
11/27/13  scv     Code changes for UL Compression feature
11/20/13  as      RLC_OOS: Changes to re-enqueue the SDU's that are waiting for ACK back to the RLC UL 
                  Watermark during RLC RESET/RE-establishment procedure.
07/22/13  kc      New MRAB API - change to save CS call where PS RAB release is colliding with RLC RESET.
05/17/13  as      Enhanced interface for PURGE_REQ between L2 & DS.
05/15/13  vp      Avoid FEATURE_WCDMA_AM_IGNORE_MAXRST when gcf flag is enabled
07/06/11  vp      Avoid RLC reset on PS RB during MRAB call
05/08/13  scv     Fix L2 compilation errors
05/03/13  as      Fix to block data RB transimission during ongoing PS call 
                  release procedure at upper layers
10/15/12  mrg     Before accesing the num_src_dscr in UM logging checkfor the data_bits size
09/27/12  as      MAC-i-is:  SDUs added after RLC-Buff-status computation needs to 
                  be de-queued  before BFI and en-queue them in same order back to WM
09/21/12  as      Cleaned up code with proper identation and added missing function headers.  
07/16/12  mrg     MAC_I & HS-RACH Compilation Warnings fixed
08/23/12  mrg     MAC-iis: Special HE functionality with LI 7 configured and 
                  RLC PDU size greater than 126 octets
07/25/12  as      New WM reg api() added for trigger base rach
07/18/12  as      Fix to trigger RACH procedure whenever there is pending data in L2
                  watermark queue when L1 is in FACH state
07/13/12  grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/18/12  mrg     L2 - MAC-I and HS-RACH code changes
04/12/12  grk     Fix to have separate PDU header and log buffers for EUL and R99.
04/24/12  kc      Moving WM registration prototypes to rlcrrcif.h
02/16/12  Ksr     Added support for Trigger Based Rach Procedure
02/01/12  grk     Feature cleanup
05/10/11  grk     Protecting internal Q access while UL RESET procedure.
03/18/11  grk     Fixed compiler warnings.
03/09/11  ssg     MAC-ehs support on Apollo.
12/07/10  vp      Added support for amr packet simulation in uplink
07/07/10  pj      Added changes to use a global base timer for status processing
06/28/10  pj      Added changes for ZI memory reduction.
05/06/10  pj      Added function prototypes 
04/13/10  grk     changes for ZI memory reduction.
02/05/10  grk     Reduced MAX_MUI value from 2048 to 256 since RRC can have
                  a maximum of only 256 MUIs in the transmission Q at a time.
02/02/09  av      Moving DSM & DUP FC to UL task  
11/30/09  grk     Added fix to use correct LSN in case NAK LIST is split across
                  TTIs
11/24/09  pj      MoB fix: Removed declarations for status prohibit timer functions 
11/24/09  pj      Added declarations for status prohibit timer functions
11/11/09  rmsd    Replaced assert.h with amssassert.h.
11/04/09  grk     Fixed compilation error for 8650.
10/23/09  av      Temperature FC   
10/07/09  av      Multi-client flow control algorithm 
08/13/09  grk     Changes for bitmap based status tracking.
04/13/09  pj      Added support for CS-over-HS
06/26/08  av      l1-l2 interface cleanup 
05/29/09  ssg     Added support for SNOW_3G algo.
05/25/09  ssg     Increased no. of pre-allocated Control PDU DSM items.
05/07/09  grk     Moved rlci_ul_dsm_fc_update_peer_rx_win() prototype under
                  FEATURE_RLC_FLOW_CONTROL_MANAGEMENT.
05/15/09  grk     Added changes for R99 mutex.
05/11/09  ssg     Updated Copyright Information
05/08/09  grk     Added changes for ZI memory reduction.
04/29/09  av      Fixed compiler warnings in L2 
04/26/09  ssg     FEATURE_POLL_OPTIMIZATION is checked conditionally with FEATURE_WCDMA_UL_DATA_PATH_OPT
04/09/09  kvk     Added support for backup and restore FC params for RLC reset params
03/11/09  pj      Added code changes to have UL/DL-task & EUL-ISR concurrency 
                  protection under light-weight mutex
12/29/08  pj      Added retx_size_reduced, to keep retx-bytes within Tx window,
                  when Tx window gets reduced
03/03/08  ssg     Added support to alternate e-bit interpretations under the
                  featue flag FEATURE_RLC_ALTERNATE_E_BIT.
12/08/08  ssg     Added fixes for UL-PP-HW.
10/30/08  grk     CR 159516 - Optimized processing for Poll_Window IE in RLC
10/17/08  grk     Merged taxis changes to mainline.
09/08/08  pj      Added seperate MAX_SDU_PER_TTI for edch & non-edch channels
                  with more MAX_SDUs for EDCH channel
          pj      Added prototype to get Active UL UM channels
08/08/08  ssg     Added fix to ignore duplicate Naks received.
05/26/08  pj      Added seperate queues for EDCH & Non-EDCH UM channels
05/20/08  kvk     Added ul_cnfg_var_local and am_common_config to store the
                   config locally
04/25/08  ar      Added new var win_sufi_config_wins for storing the latest 
                  win sufi configured window size
04/18/08  ssg     Added new bit definitions for ctrl_info.
03/28/08  pj      Added fix to handle HFNI value during duplicate resets
03/24/08  pj      Added support for data path optimizations under 
                  FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
03/17/08  ar      Added appropriate featurization:FEATURE_DL_CPU_BASED_FC
                  or FEATURE_UL_CPU_BASED_FC for flow control functions and 
                  variables, required for separating UL and DL fc functionality
02/01/08  pj      Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
12/14/07  ssg     Added flag in rlci_ul_am_state_type.
11/30/07  ssg     Added mbms support under FEATURE_MBMS.
09/18/07  vg      added rbs CONTINUE support.
09/06/07  ssg     Removed the function prototype rlci_ul_init_buffer_status().
08/07/07  ssg     Featurized  rlci_ul_init_buffer_status() under FEATURE_WCDMA_HSUPA.
07/26/07  ssg     Added function prototype to initialize buffer status pointer.
07/06/07  ssg     Added field to know loopback mode in rlci_ul_tm_ctrl_type
                  under FEATURE_TM_LB.
06/26/07  ssg     Added flag to indicate start segment of an SDU is ack'd 
                  and remaining pdus containing this SDU segments are yet to
                  be transmitted.
02/16/07  vg      added variable to check if cipher pdu logging is active 
01/11/07  vg      added UM PDU logging structure in cltrl block also added
                  function prototype for start_pdu_logging
11/23/06  ssg     Removed function prototypes for crlc_ul_suspend_handler(),
                  rlc_ul_suspend_cnf().
08/15/06  ttl     Put rlc_size under PERFORM_RLC_SIZE_CHECKING.
08/11/06  ttl     Put QoS support under FEATURE_CPU_BASED_FLOW_CONTROL_WITH_QOS. 
08/01/06  ttl     Add support to EUL.
07/03/06  ssg     Added rlci_ul_am_sdu_tx_cnf_type structure.
04/12/06  ttl     Give the failure reason of sdu_cnf_cb().
12/12/05  aw      Merged the structure rlc_ul_chan_am_retrans_stat_type with
                  rlc_ul_chan_am_stat_type.
9/12/05   aw      Added a new structure rlc_ul_chan_am_retrans_stat_type to
                  rlci_ul_am_ctrl_type to support duplication transmissions
                  in RLC statistics log packet.
06/06/05  aw      Added a new semaphore mrw_q_sema for accessing mrw queue.
05/20/05  aw      Added two variables timer_discard_handle and 
                  timer_discard_sequence_num to rlci_ul_am_ctrl_type
                  structure. Also added new function declaration for 
                  rlci_um_handle_discard_timer_cb and 
                  rlc_ul_sdu_discard_timeout_handler to support timer based
                  SDU discard.
05/31/05  mv      Defined RLC_UL_AM_USR_PLANE_START_LC_ID as the start for
                  user/data plane logical channels.
05/03/05  mv      Added the function definition for 
                  crlc_ul_abort_cipher_config.
                  Added the parameter cipher_backup to the structure
                  rlci_ul_ctrl_blk_type.
04/05/05  aw      Removed Log_on_demand feature.
02/11/05  mv      Added the boolean purge_wm to rlci_ul_ctrl_blk_type.
                  Added the function definition for rlc_ul_purge_uplink_wm.
07/09/04  mv      Added a reset_fail flag in the UL control block to 
                  indicate maxrst on a particular logical channel.
06/08/04  mv      Increased the max TX window size, MUI and tx queue size 
                  mask from 256 to 2048.
06/02/04  mv      Fixed lint errors and warnings.
02/09/04  ttl     Added rlci_am_handle_cellpch_timer_cb().
01/07/04  ttl     Added CELL_PCH support.
12/06/03  ttl     Added data structures for PDU ciphering parameters logging.
10/06/03  mv      Pulled the AM transmission queue out of the structure
                  rlci_ul_am_ctrl_type to save the memory utilized
                  by uplink_table.
09/04/03  ttl     Added fst_tx_pdu.
08/20/03  ttl     Merged branch code into tip.
07/29/03  ttl     Fixed a bug of allocating memory in interrupt context.
05/16/03  ttl     Added rlc_size to rlci_ul_am_state_type.
04/21/03  ttl     Added two stages flow control of DSM free items.
03/28/03  ttl     Changed UE_MAX_UL_RLC_ENTITY definition to be
                  UE_MAX_UL_LOGICAL_CHANNEL.
02/14/03  ttl     Added poll_delay.
01/14/03  ttl     Naming convention change for tx_nak_q.
12/10/02  ttl     Added backup_timer_cnt for backup timer.
11/20/02  ttl     Added Window SUFI retx for opening the peer side tx window.
11/13/02  ttl     Added ignore_dl_reset_msg to rlci_ul_am_state_type.
09/16/02  ttl     Changed from uint8 to uint16 for some variables in rlci_ul_am_state_type.
09/05/02  ttl     Added Ciphering feature.
08/16/02  ttl     Added RX_TX_CLOSE to rlci_close_rx_win_e_type.
                  Added RLC_TXQ_SIZE_MASK.
08/13/02  ttl     Added function type for rlci_um_handle_discard_timer_cb().
08/01/02  ttl     Added the definition of rlc_get_data_counters().
07/31/02  ttl     Rewrote 10ms base timer, removed timer sig, using timer
                  callback function instead.
                  Changed rlci_am_handle_10ms_timer() to rlci_am_handle_10ms_timer_cb().
                  Removed rlci_ul_am_get_timer_sig(), rlci_ul_am_timer_sig_to_rlc_id(),
                  rlci_ul_am_release_timer_sig().
                  Added SDU discard timer feature definitions for TM.
07/27/02  ttl     Added cur_sdu_ptr to rlci_ul_ctrl_blk_type.
07/09/02  ttl     Changed RLC_TXQ_SIZE to 256.
06/12/02  ttl     Restructuring the logging structures.
05/17/02  ttl     Added n_to_suspend to UM control table.
05/13/02  ttl     Added flow control mechanism for the flow control for use 
                  plane data application.
                  Added UL_STOP to internal states.
05/08/02  ttl     Added ctl_in_rach in Control table.
                  Moved function prototype of rlci_non_empty_func() to rlci.h.
04/24/02  ttl     Rewrote the statistic log code.
04/03/02  ttl     Changed incorrect naming of FACH to RACH.
                  Added Log_on_demand feature.
03/15/02  sk      Added structures rlc_ul_am_chan_stat_type, rlc_ul_am_chan_pdu_type
                  to collect statistics information for a single AM entity. Thes
                  fields are added in rlci_ul_am_ctrl_type to log info for all
                  AM entities.  
03/13/02  ttl     Added wm_in_fach to fix the bug of accessing WM before it got
                  registered in FACH.
02/13/02  ttl     Added service de-registration function.
11/30/01  ttl     Modified function declaration of rlci_post_dl_reset_msg().
11/17/01  ttl     Added function declaration for rlci_post_reset_to_rrc().
08/20/01  rc      Added support for QXDM logging.
08/14/01  ttl     Added Timer_poll mechanism.
08/08/01  ttl     Added reset_ack_sn.
06/29/01  ttl     Removed rlctmr_ul_callback();
                  Included task.h.
                  Added definition of rlci_ul_am_get_timer_sig().
                  Added definition of rlci_ul_am_timer_sig_to_rlc_id().
                  Added definition of rlci_ul_am_release_timer_sig().
06/26/01  ttl     Fixed a typo of rlc_ul_register_am_sdu_cnf_cb_req().
06/08/01  rc      Removed prototype for rlci_dl_release_all_cnf_handler().
06/07/01  rc      Added prototypes for rlci_dl_release_all_cnf_handler(),
                  and crlc_release_all_handler().
06/06/01  ttl     Added Poll_peri_cnt, Poll_proh_cnt.
                  Added declaration of rlctmr_ul_callback().
05/29/01  ttl     Added q_mrw_sn to ul_am_state.
05/23/01  rc      Fixed warning found while compiling with ARM.
05/23/01  ttl     Added rpt_cnt to rlci_am_sdu_pdu_map_type.
05/21/01  rc      Added more comments.
05/02/01  ttl     Added definition rlci_sdu_pdu_seg_e_type for PDU/SDU 
                  mapping information.
                  Added more definition for RESET, MRW, NAK, RETX topics.
                  Added rlci_am_handle_10ms_timer().
03/26/01  ttl     Changed typedef for (*sdu_cnf_cb)().
                  Added rlc_ul_register_am_sdu_cnf_cb_req() definition.
02/28/01  ttl     Added support for Acknowledged Mode.
02/13/01  rc      Removed maccommdefs.h
01/08/01  rc      Modified code to use the data types defined in uecomdef.h.
08/03/00  rc      Created file.

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
#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "rlcrrcif_v.h"
#include "rlcdsapif.h"
#include "stringl.h"

#ifdef FEATURE_WCDMA_HSUPA
  #include "l1macdata.h"
#endif //FEATURE_WCDMA_HSUPA

#include "rlci.h"
#include "l2dltask.h"
#include "l2ultask.h"
#include "rlclog.h"
#include "modem_mem.h"
#ifdef FEATURE_QXDM_DSM_TRACING
#include "fc_dsm_trace.h"
#endif

#ifdef FEATURE_WCDMA_AM_IGNORE_MAXRST
#error code not present
#endif

#ifdef FEATURE_WCDMA_UL_COMPR
#include "daccif.h"
#endif


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

#ifdef FEATURE_WCDMA_AM_IGNORE_MAXRST
  #error code not present
#else
  #define RLC_UL_AM_IGNORE_MAXRST(ctrl_blk) (FALSE)
#endif

#ifndef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3

#define UE_MAX_UL_RLC_ENTITY  UE_MAX_UL_LOGICAL_CHANNEL

#else

/*---------------------------------------------------------------------------
SRB's - 1 TM (srb 0) + 1 UM (srb 1) + 3 AM (srb 2,3,4)
AMR - 3 TM
PS - 2 AM or 2 UM
Video call - 1 TM

UE_MAX_UL_RLC_ENTITY   16  ( 5TM, 3UM, 5AM + 3 buffer)
---------------------------------------------------------------------------*/
#define UE_MAX_UL_RLC_ENTITY  16

#endif
/*---------------------------------------------------------------------------
  The user/data plane logical channel ids start from the value that is 
  calculated by RRC as MAX UL TM channels + MAX UL UM channels + 
  MAX AM Signalling channels.
---------------------------------------------------------------------------*/

#define RLC_UL_AM_USR_PLANE_START_LC_ID (UE_MAX_UL_TM_CHANNEL + \
                     UE_MAX_UL_UM_CHANNEL + UE_MAX_SRB_AM_EMTITIES_3)

#define RLC_UL_AM_START_LC_ID (UE_MAX_UL_TM_CHANNEL + \
                               UE_MAX_UL_UM_CHANNEL)

#define UM_CIPHER_OFFSET 8
#define AM_CIPHER_OFFSET 16

#define RLC_UM_CIPHER_OFFSET 8
#define RLC_AM_CIPHER_OFFSET 16

#ifdef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3
extern rlci_missing_list_type *rlc_naklist_pool;
#else
extern rlci_missing_list_type rlc_naklist_pool[NUM_NAK_BUF];
#endif

extern q_type rlc_ul_free_nak_q;

#define RLC_INVALID_COUNT_C            ~(0) /* Invalid value for count_c  */

/*---------------------------------------------------------------------------
  Length Indicator related definitions
---------------------------------------------------------------------------*/
#define RLC_UL_LI_OCTET_LESS             0xFFF6  /* = (0x7FFB << 1)  */

#define RLC_UL_LI_PADDING_7_BIT          0xFE    /* = (0x7F << 1)    */
#define RLC_UL_LI_PADDING_15_BIT         0xFFFE  /* = (0x7FFF << 1)  */

#define RLC_UL_LI_PIGGYBACK_7_BIT        0xFC    /* = (0x7E << 1)    */
#define RLC_UL_LI_PIGGYBACK_15_BIT       0xFFFC  /* = (0x7FFE << 1)  */

#define RLC_CHOOSE_LI(li_len,x,y)     ((li_len == RLC_8_BIT_HDR) ? x : y)

/*---------------------------------------------------------------------------
  PDU padding related.
---------------------------------------------------------------------------*/
#define   PDU_PADDING_MASK              0x20000000
#define   SET_PDU_PADDING(x)            ((x) |= PDU_PADDING_MASK)
#define   SET_PDU_PADDING_LEN(x, y)     ((x) |= (((uint32)(y)) << 16))

#define   IO_VEC_LEN_MASK               0x0000FFFF
#define   GET_IO_VEC_LEN(x)             ((x) & IO_VEC_LEN_MASK)

/*---------------------------------------------------------------------------
  Common definitions
---------------------------------------------------------------------------*/
#define   MAX_IO_VEC_PER_PDU             20
#define   MAX_HDR_BUFF_SIZE             404

/* The Max SDUs that are possible per TTI */
#define   MAX_SDU_PER_TTI                 100
#define   MAX_TM_SDU_PER_TTI_EDCH          15
#define   MAX_SDU_PER_TTI_EDCH            200
#define   MAX_SDU_PER_TTI_NON_EDCH        100

/*---------------------------------------------------------------------------
  Maximum no. of DSM items for building control PDUs
  among all AM logical channels in a particular TTI.
---------------------------------------------------------------------------*/
#define   MAX_CTRL_DSM_ITEMS              8
#define   MAX_MAC_I_CTRL_DSM_ITEMS              UE_MAX_UL_LOGICAL_CHANNEL

#define   MAX_UL_PDUS_PER_TTI            26  /* Maximum no. of UL PDUs per TTI in R99 path */
#define   RLC_SI_HDR_SIZE_BYTES           3
#define   RLC_TSN_HDR_SIZE_BYTES          4

#define RLC_UL_AM_MAX_TCP_ACK_MISMATCH_SDU (10)

/*---------------------------------------------------------------------------
  7200A specific:
  io.length(31)   : TTI_LAST_IO_VEC  
  io.length(30)   : PDU_LAST_IO_VEC
  io.length(29)   : PDU_PADDING       --Valid only if io.length(30) = 1
  io.length(28)   : Reserved
  io.length(27-16): padding length    --Valid only if io.length(29) = 1
  io.length(15-0): data length
---------------------------------------------------------------------------*/
#define   TTI_LAST_IO_VEC_MASK          0x80000000
#define   SET_TTI_LAST_IO_VEC(x)        ((x) |= TTI_LAST_IO_VEC_MASK)

#define   PDU_LAST_IO_VEC_MASK          0x40000000
#define   SET_PDU_LAST_IO_VEC(x)        ((x) |= PDU_LAST_IO_VEC_MASK)

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
#define RLC_UL_REG_TRIG_BASED_RACH_API(is_l1_in_fach,rlc_id,ul_wm_ptr) \
do\
{\
  if ((is_l1_in_fach) && (NULL != ul_wm_ptr))\
  {\
    MSG_HIGH_HS_RACH("TRIG_RACH: L1 State RACH - Register mac_ul_trigger_tx_req for non_empty_func_ptr - LC %d",\
                                                                                                  rlc_id, 0, 0);\
    ul_wm_ptr->non_empty_func_ptr = mac_ul_trigger_tx_req;\
  }\
}while(0)
#else
#define RLC_UL_REG_TRIG_BASED_RACH_API(is_l1_in_fach,rlc_id,ul_wm_ptr)
#endif
/* Enum for the RLC headers. includes UM, AM, 7/15 bit LI headers */
typedef enum
{
  RLC_8_BIT_HDR  = 0x1,
  RLC_16_BIT_HDR = 0x2
}rlc_hdr_e_type;

typedef enum
{
  RLC_AM_CONTROL_PDU   = 0x0,
  RLC_AM_NEW_DATA_PDU  = 0x1,
  RLC_AM_RETX_DATA_PDU = 0x2
}rlc_am_pdu_e_type;

typedef enum
{
  RLC_INVALID_CHNL    = 0x0,
  RLC_NON_E_DCH_CHNL  = 0x1,
  RLC_E_DCH_CHNL      = 0x2
}rlc_ul_chnl_e_type;

typedef enum
{
  NON_BFI_NON_TVM_QUERY  = 0x0,
  BFI_QUERY              = 0x1,
  TVM_QUERY              = 0x2,
}rlc_ul_bo_query_source;

typedef enum {
  NO_SDU  = 0x00,
  S_SDU   = 0x01,
  E_SDU   = 0x02,
  MID_SDU = 0x03,
  ENS_SDU = 0x04,
  FIT_SDU = 0x05
} rlci_sdu_pdu_seg_e_type;

typedef enum {
  NONE_CLOSE = 0x00,
  APP_CLOSE = 0x01,
  RX_TX_CLOSE = 0x02,
  HALF_CLOSE_WIN = 0x80
} rlci_close_rx_win_e_type;

typedef enum {
  PEER_TX_DONT_CARE = 0x00,
  PEER_TX_ONE_WIN,
  PEER_TX_HALF_WIN, 
  PEER_TX_FULL_WIN 
} rlci_peer_tx_win_size_e_type;

/*---------------------------------------------------------------------------
  Enumerated type that defines the RLC Uplink Logical Channel states.
---------------------------------------------------------------------------*/
typedef enum {
  UL_NULL_STATE = 0,                    /* Logical channel is currently not*/
                                        /* in use.                         */
  UL_DATA_TRANSFER_READY,               /* Logical channel has been        */
                                        /* established, and data transfer  */
                                        /* is allowed.                     */
  UL_WAITING_TO_SUSPEND,                /* RRC has sent a suspend request  */
                                        /* with N value greater than 0, and*/
                                        /* RLC has not yet transmitted     */
                                        /* SN + N PDUs. Only applicable to */
                                        /* UM and AM channels.             */
  UL_SUSPENDED = 0x04,                  /* Data transfer on the logical    */
                                        /* channel has been temporarily    */
                                        /* suspended. Only applicable to UM*/
                                        /* and AM channels.                */
  UL_RESET_PENDING = 0x08,              /* RESET has been sent to the peer */
                                        /* entity, and no data transfer is */
                                        /* allowed until a RESET_ACK is    */
                                        /* received. Only applicable to AM */
                                        /* channels.                       */
  UL_CONFIG_PENDING = 0x10,             /* Waiting for the DL side to      */
                                        /* complete its config             */
  UL_STOP = 0x20                        /* Stop all of the transmission    */ 
} rlci_ul_state_e_type;

/*--------------------------------------------------------------------------- 
  Typedef for storing parameters specific to transparent mode.
---------------------------------------------------------------------------*/
typedef struct {
  boolean seg_ind;                      /* Segmentation Indication - TRUE  */ 
                                        /* if segmentation is on, FALSE    */
                                        /* otherwise.                      */
  boolean act_incl;                     /* Activation Time - True if       */
                                        /* included, False Otherwise       */
  uint8   act_time;                     /* Activation Time.                */
  rlc_lc_id_type rlc_id;                /* RLC logical channel ID.         */
  uint16  timer_discard;                /* Timer Discard value in ms.      */
#ifdef FEATURE_TM_LB
  rlc_loop_back_mode_e_type lpm;
#endif /* FEATURE_TM_LB */
} rlci_ul_tm_ctrl_type;

/*--------------------------------------------------------------------------- 
  Typedef for storing info of UM PDU logging
---------------------------------------------------------------------------*/
#ifdef FEATURE_UM_PDU_LOGGING
typedef struct{
  uint8  carrier_indx;            /* Carrier index for logging the Pri/Sec carrier for 0x430F ver 3 */
  uint16 num_pdu;                 /* Number of PDUs to be built every TTI  */
  uint16 pdu_size[MAX_UL_RLC_LOG_PDUS];         /* PDU size in octets of each PDU        */
  uint32 count_c[MAX_UL_RLC_LOG_PDUS];          /* count_c for each UM PDU               */
#ifdef FEATURE_MAC_I
  uint8 num_sdu;
  uint32 buff_status;
#endif
  uint32 log_data[MAX_UL_RLC_LOG_PDUS];
                                  /* The first four bytes of the pdu       */
} rlc_ul_chan_um_pdu_type;
#endif

/*--------------------------------------------------------------------------- 
  Typedef for storing parameters specific to unacknowledged mode.
---------------------------------------------------------------------------*/
typedef struct {
  boolean act_incl;                     /* Activation Time - True if       */
                                        /* included, False Otherwise       */
  uint8   act_time;                     /* Activation Time.                */
  uint16  timer_discard;                /* Timer Discard value in ms       */
  //uint8   rlc_sn;                       /* RLC Sequence Number             */
  uint8   li_length;                    /* Length of the length indicator  */
                                        /* - 7 or 15 bits                  */
  uint8   vt_us;                        /* Sequence Number of the next in- */
                                        /* Sequence Pdu to be transmitted. */
  uint8   n_to_suspend;                 /* Number of PDUs can be sent in   */
                                        /* waiting to suspend state.       */
  boolean sn_wrap;                      /* SN wraparound?                  */
  rlc_lc_id_type rlc_id;                /* RLC logical channel ID.         */

  boolean alternate_e_bit;              /* TRUE - Alternate E-bit interpretation */
                                        /* FALSE- Normal E-bit interpretation    */
#ifdef FEATURE_MAC_I
  boolean ul_flex_pdu;
#endif
#ifdef FEATURE_CS_VOICE_OVER_HSPA  
  boolean segmentation_enabled;         /* TRUE - if RLC needs to only Segment 
                                                  SDUs but not concatenate
                                        */
#endif /* FEATURE_CS_VOICE_OVER_HSPA */

#ifdef FEATURE_RLC_QXDM_LOGGING
#ifdef FEATURE_UM_PDU_LOGGING
  rlc_ul_chan_um_pdu_type  ul_chan_um_pdu;   /* Stores the UM PDU information */
#endif
#endif
} rlci_ul_um_ctrl_type;

#define RLC_TXQ_SIZE        2048      /* Max. PDUs can be tx'd w/o ACK   */
#define RLC_TXQ_SIZE_MASK   0x7FF 

#define MAX_MUI   256          /* Message Unit ID                 */

typedef struct {
  uint16  rlc_fc_ws;
} rlc_fc_ws_type;
/*--------------------------------------------------------------------------- 
  Used to store the necessary up-link control state variables 
  for an RLC AM entity.
---------------------------------------------------------------------------*/
typedef struct {
  uint16  vt_s;                         /* VT(S) - Send State              */
  uint16  vt_a;                         /* VT(A) - Acked State             */
  uint16  vt_ms;                        /* VT(MS) - VT(A) + Tx_Window_size */
  uint16  vt_wins;                      /* Tx_Window_size                  */
  uint16  peer_wins;                    /* Tx_Window_Size to peer side     */
  boolean tx_wins;                      /* Send Tx_Window_Size SUFI?       */
  uint8   vt_pu;                        /* VT(PU) - Poll_PU                */
  uint8   vt_sdu;                       /* VT(SDU) - Poll_SDU              */
  uint8   vt_rst;                       /* VT(RST) - # of RESET sent       */
  uint8   reset_sn;                     /* Sequence number for RESET PDU.  */
                                        /* Initial to 0                    */
  uint8   reset_ack_sn;                 /* Sequence number for RESET_ACK   */
                                        /* PDU Initial to 0                */
  uint16  poll_proh_cnt;
  uint16  poll_peri_cnt;
  boolean poll_prohibit;                /* timer_poll_prohibit on or off   */
  boolean poll_next;                    /* Set polling bit in the next     */
                                        /* outgoing frame                  */
  uint16  poll_window_size;             /* poll window size used for       */
                                        /* tx poll window % calculation    */
  uint8   timer_poll_dat;               /* Max number of polling rerty     */
  boolean timer_poll;                   /* Truned on/off timer polling     */
  uint16  timer_poll_sn;                /* SN for timer polling            */
  uint16  timer_poll_cnt;               /* Counter for timer polling       */
  boolean owe_reset;                    /* Need to send a Reset PDU        */
  boolean need_reset_ack;               /* In need for RESET ACK?          */
  uint16  reset_timer_cnt;              /* Timer counter for RESET PDU retx*/
  boolean owe_reset_ack;                /* Need to send a Reset_ACK PDU    */
  uint8   status_rpt_cnt;               /* No of back to back status PDU Tx*/
  boolean status_rpt;                   /* Being allowed to report status  */  
  boolean ack_pdu;                      /* ACK received PDU in downlink.   */
  boolean tx_mrw;                       /* If TRUE, send the first MRW in  */
                                        /* mrw_q                           */
  uint16  mrw_timer_cnt;
  uint16  q_mrw_sn;                     /* Latest queued MRW's SN          */
  uint32  retx_size;                    /* number of bytes to be reTX.     */
  uint32  retx_size_reduced;            /* used when there is a TX window reduction
                                           indicates, number of bytes (within Tx window) 
                                           to be reTX */
  rlci_close_rx_win_e_type close_rx_win;/* APP_CLOSE_RX_WIN,               */
                                        /* RLC_CLOSE_RX_WIN                */
  uint8   li_length;                    /* Length of the length indicator  */
                                        /* - 7 or 15 bits */
  uint8   n_to_suspend;                 /* Number of PDUs can be sent in   */
                                        /* waiting to suspend state.       */
  boolean need_mrw_ack;                 /* In need for MRW ACK?            */
  boolean nw_only_init_reset;           /* TRUE - UE rxd RESET PDU.        */
                                        /* FALSE- When duplicate Reset PDU */
                                        /*        is received.             */
                                        /*      - Reset PDU is rxd while   */
                                        /*        onging UE initiated Reset*/
                                        /*        procedure.               */
  boolean duplicate_reset_pdu_rx;       /* TRUE - UE rxd duplicate RESET   */
  uint32  txd_reset_ack_hfn;            /* To store previous hfni value, used
                                           to handle duplicate resets      */
                                        
  uint16  retx_win_cnt;                 /* Timer count for retx win SUFI   */
  boolean retx_win_size;
  byte    n_retx_win;
  boolean retx_q_sema;                  /* retx q access semaphore         */
  boolean mrw_q_sema;                   /* mrw q access semaphore         */
  boolean internal_q_sema;                   /* mrw q access semaphore         */
  boolean mrw_ack_q_sema;               /* mrw ack q access semaphore      */  
  uint8   backup_timer_cnt;             /* backup timer                    */
  boolean poll_delay;                   /* delay the polling to next PDU   */
  boolean fst_tx_pdu;                   /* TRUE, if the next going out PDU */
                                        /* is the 1st tx PDU.              */
  boolean reset_fail;                   /* indicates reset failure on a 
                                           logical channel                 */
  rlc_fc_ws_type rlc_fc_ws;             /* Peer window size requests from
                                           all RLC flow controls.          */
  uint8   dl_cpu_base_fc_f;                /* Flag of indication of performing*/
                                        /* CPU base flow control.          */
  uint8   ul_cpu_base_fc_f;                /* Flag of indication of performing*/
                                        /* CPU base flow control.          */
  uint8    ul_count_after_re_est;         /* Delay after the RLC re-establishment - Either UL or both */
#ifdef FEATURE_FLOW_CONTROL_WITH_QOS
  uint8 fc_qos_flag; /*This is a general flag to say if the flow is a qos flow
                       for it to be flow controlled. Right now it will be used only
                       for temp fc. Ideally it would be better to migrate cpu fc also 
                       into this flag. fc acts only when fc_qos_flag = false for qos
                       sensitive flow controller.*/
#endif /*FEATURE_FLOW_CONTROL_WITH_QOS*/
#ifdef FEATURE_MAC_I
  boolean poll_sufi_reported;
#endif
} rlci_ul_am_state_type;

/*--------------------------------------------------------------------------- 
  Used to store the mapping information between the SDUs and PDUs. When the 
  ACK for a PDU is received ( and the SDUs being put in this PDU are required 
  to report to the upper layer), the RLC uses this table to obtain the MUIs 
  for these SDUs and the information about other segments of the same SDU in 
  another PDUs gotten ACKed as well. The individual SDU information needed to 
  be reported to the upper layer is stored in another table. 
  The validity of the SDU confirmation callback function is checked every time 
  to determine whether to look at this mapping information when an ACK for a 
  PDU is received from the peer entity.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 mui;                     /* starting SDU sequence number           */
  uint8 distance;                /* number of consecutive SDUs in this PDU */
  rlci_sdu_pdu_seg_e_type sdu_seg;
                                 /* SDU segmentation infor in this PDU.    */
                                 /* The value of this variable stands for: */
                                 /* 0: NO SDU. or only padding exist       */
                                 /* 1: Contains only start of an SDU.      */
                                 /* 2: Contains only end of an SDU.        */
                                 /* 3: Contains only the middle of an SDU. */
                                 /* 4: Contains the end of an SDU and the  */
                                 /*    start of another SDU.               */
                                 /* 5: All SDUs in this PDU are fit in.    */
  
  uint8 num_ack;/* number of the ACKs received.                            */
                /* 0 : not receiving Ack yet.                              */
                /* 1 : either the ACK being received for this PDU or the   */
                /*     previous PDU containing the part of the same SDU    */
                /*     in this PDU got ACKed.                              */
                /* 2 : both this PDU and previous PDU got acked.           */
                /* When the ACK for this PDU is received, the num_ack is   */
                /* incrementing by one. For an SDU being segmented into    */
                /* several segments, and each segment being fit into one   */
                /* PDU, except the first segment, each segment needs two   */
                /* acks to show this PDU/SDU segment and the previous      */
                /* PDU/SDU segment being ACKed. When the num_ack reaches   */
                /* the required value for this PDU (1 for the first        */
                /* segment and 2 for the reset), it continues to look at   */
                /* the next PDU to update the num_ack. (i.e. Adds one to   */
                /* it. When the last segment of the SDU reaches, and the   */
                /* num_ack is 2, the callback function to report this      */
                /* SDU to the upper layer is called, if necessary.         */
  boolean exact_fit;              /* One byte less or exactly fit w/o LI   */
  uint8   rpt_cnt;    /* number of SDUs being reported to the upper layer */
} rlci_am_sdu_pdu_map_type;

#ifdef FEATURE_RLC_QXDM_LOGGING
/*---------------------------------------------------------------------------
  Data Structure for logging RLC UL statistics for acknowledged mode.
---------------------------------------------------------------------------*/

typedef struct
{
  uint32 tot_num_pdu_byte_txd;    /* Total number of bytes transmitted     */
                                  /* Includes teh ctrl & data pdus         */
  uint32 tot_num_sdu_byte_txd;    /* Total number of sdu bytes rxd from    */
                                  /* upper layer                           */
  uint16 tot_num_new_data_pdu_txd;/* total number of data PDUs transmitted */
  uint16 tot_num_data_pdu_retxd;  /* Total number of PDUs retransmitted    */
  uint16 tot_num_pdu_nacked;      /* Total number of pdus naked            */
  uint16 tot_num_ctrl_pdu_txd;    /* Total number of control PDUs          */
  uint16 tot_num_rlc_resets;      /* Total number of rlc resets            */
  uint16 ul_am_buf_status;        /* Stores the uplink buffer status value */
  uint16 tot_num_pdus_with_two_or_more_retrans; /* Total number of dat   */
  uint16 tot_num_pdus_with_three_or_more_retrans; /* count for pdus which  */
  uint16 tot_num_pdus_with_four_or_more_retrans;  /* have more than 2 dat  */
  uint16 tot_num_pdus_with_five_or_more_retrans;  /* counts. Number of PDUs*/
  uint16 tot_num_pdus_with_six_or_more_retrans;   /* with >2, >3, >4, >5   */
} rlc_ul_chan_am_stat_type;                       /* or >=6 dat counts     */

/*--------------------------------------------------------------------------
Data structures for logging the first 4 bytes of the PDU by QCAT.
---------------------------------------------------------------------------*/

typedef struct{
  uint8  carrier_indx;            /* Carrier index for logging the Pri/Sec carrier for 0x430F ver 3 */
  uint8  num_sdu;
  uint8  special_he;
  uint16 num_pdu;                 /* Number of PDUs to be built every TTI  */
  uint32 new_data_bytes;
  uint32 retx_data_bytes;  
  uint16 ctrl_data_bytes;  
  uint16 pdu_size[MAX_UL_RLC_LOG_PDUS];                /* PDU size in octets of each PDU        */
  uint32 log_data[MAX_UL_RLC_LOG_PDUS];                /* The first four bytes of the pdu       */
  uint32 count_c[MAX_UL_RLC_LOG_PDUS];                 /* count_c for each PDU                  */

} rlc_ul_chan_am_pdu_type;

/*--------------------------------------------------------------------------
Data structures for logging the UL AM&UM PDU ciphering parameters
---------------------------------------------------------------------------*/

typedef struct{
  uint8  log_ul_rlc_id;           /* Data Logical Channel UL RLC ID(0..18) */
  uint32 cipher_key_idx;
  uecomdef_wcdma_cipher_algo_e_type  cipher_algo;
  uint32 count_c;                 /* PDU size in octets of each PDU        */
                                  /* Dump of the status PDU                */
} rlc_ul_cipher_log_data_type;

/*--------------------------------------------------------------------------
Data structures for logging the UL AM&UM PDUs ciphering parameters
---------------------------------------------------------------------------*/
typedef struct{
  uint16 num_pdu;                  /* Number of PDUs to be built every TTI  */
  boolean ul_cipher_logging ; /* TRUE if cipher logging in progress in TaskC */
  rlc_ul_cipher_log_data_type cipher_log_data[MAX_CIPHER_PDU];
                                  /* The first four bytes of the pdu       */
} rlc_ul_pdu_cipher_info_type;

#ifdef FEATURE_WCDMA_UL_COMPR
/*--------------------------------------------------------------------------
Data structure for logging RLC UL Compression Information
---------------------------------------------------------------------------*/
typedef struct{
  uint8 cfn;                 /* CFN value when ISR fired*/
  uint8 comp_isr_late;       /* No of BPGs compression ISR arrived late */
  uint16 bytes_comp;         /* No of bytes compressed during current ISR */
  uint16 bytes_uncomp;       /* No of uncompressed bytes used */
  uint8 num_sdu_comp;        /* No of higher layer SDUs compressed in current ISR(3 bits) */
  uint8 num_uncomp_pkts;     /* No of uncompressed SDUs pumped in current ISR(5 bits) */
  uint8 ticks_taken;         /* Ticks usage per compression ISR(6 bits) */                      
  uint8 bo_multiplier;       /* Cumulative compression Rate (7 bits)*/
  uint8 limits_reached;      /* ISR exited due to limits 0-No limits, 1-COMP/UNCOMP limit, 2-MAX BPG limit
                                3-DSM limit, 4-FC limit, 5-SDU limit, 6-Q limit, 
                                7-MTX limit(datapath mutex acquired)(3 bits)*/
  uint32 wm_byte_cnt;        /* No of bytes of outstanding data in WM Q*/
  uint16 comp_q_byte_cnt;    /* No of bytes of outstanding data in UL COMP Q*/   
} rlc_ul_comp_log_sample_type;
 typedef struct{
   rlc_lc_id_type rlc_id;
   uint8 num_samples;
   uint32 tot_isr_misses;   
   rlc_ul_comp_log_sample_type rlci_ulc_log_params[MAX_ULC_LOG_SAMPLES];
 } rlc_ul_compression_log_info_type;

 extern rlc_ul_compression_log_info_type rlci_comp_log_info;
 #endif
 
#endif //FEATURE_RLC_QXDM_LOGGING
/*--------------------------------------------------------------------------- 
  Used as an entry in the transmit queue to store the transmitted RLC frame 
  for the later retransmit requirement by the peer side. The DSM item 
  contains the completely formatted RLC frame, excluded the piggybacked 
  status PDUs.
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------- 
  Contains all the headers & LI's for a particular PDU.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   *buff;
  uint16   num_bytes;
}rlc_buff_type;

/*--------------------------------------------------------------------------- 
  Contains IO vectors corresponding to a particular PDU.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8* ptr;
  uint32 len;
}rlc_io_vec_type;

typedef struct
{
  rlc_io_vec_type io_vec[MAX_IO_VEC_PER_PDU];
  uint8           num_io_vecs;
}rlc_io_vec_list_type;

/*--------------------------------------------------------------------------- 
  The headers i.e. MAC-e, MAC-es, RLC, LIs are stored in the Header Buffer
  while building the PDUs. All these headers are copied into the ERAM bank
  header section by L1. Each header is considered as a separate data unit.
  MAC-e, MAC-es data units are byte aligned. RLC headers, LIs need be word
  aligned.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 num_valid_words;         /* No. of words taken for headers.       */
  
  uint32 buff[MAX_HDR_BUFF_SIZE]; /* Store MAC-e, MAC-es, RLC headers, LIs */
}rlc_ul_hdr_buff_type;

typedef dsm_item_type * free_sdu_buffer_type;

typedef struct
{

  free_sdu_buffer_type *sdu_buffer_ptr;    /* Pointer to the SDU that needs*/
                                           /* to be freed.                 */
  uint8 num_sdu;                           /* No. of SDUs to be freed.     */

} rlc_sdu_q_type;

/*--------------------------------------------------------------------------- 
  Definition for ctrl pdu dsm pool
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type   *item_ptr;                /* Statically allocated DSM item */
                                          /* which is used for building the*/
                                          /* contrl PDUs in this TTI       */
  uint16           num_ctrl_bits;            /* No. of ctrl_pdu bits built    */
  uint16           pdu_size;                 /* Size in bits */
  rlc_lc_id_type   rlc_id;
  boolean          in_use;
}rlc_ctrl_pdu_type;

typedef rlc_sdu_q_type rlc_um_sdu_q_type;
typedef rlc_sdu_q_type rlc_tm_sdu_q_type;

typedef struct
{
  uint8   num_dsm_item;
  rlc_ctrl_pdu_type *   mac_i_ctrl_dsm_idx[MAX_MAC_I_CTRL_DSM_ITEMS];
}rlc_mac_i_ctrl_dsm_free_type;


typedef struct
{
  dsm_item_type *sdu_ptr;     /* Contains pointer to the 1st SDU whose     */
                              /* data goes as payload with this PDU        */
                              
  uint16 offset;              /* Offset with in the 1st SDU                */
                              
  uint16 payload_length;      /* Length of PDU payload excluding padding   */
                              /* and RLC Header - 2bytes AM                */
  
  uint16 ctrl_info;           /* bit15 - bit11 --> RESERVED                */

                              /* bit10 --> 1 - HE is set in this PDU       */
                              
                              /* bit9 --> 1 - LI=7ffb present in next PDU. */
                              /*            - This is used while SDU ackcnf*/
  
                              /* bit8 --> 1 - Last octet of SDU is last the*/
                              /*              octet of PDU and LI fits in  */
                              /*              this PDU itself              */
                              
                              /* bit7 --> 1 - LI=0 present in next PDU.This*/
                              /*            - is used during SDU ack cnf.  */
                              
                              /* bit6 --> 1 - LI=0 present in this PDU     */
                              
                              /* bit5 --> 1 - LI=0x7ffb present in this PDU*/

                              /* bit4 --> 1 - PDU contains padding         */

                              /* bit3-bit0--> No. of SDUs ended in this PDU*/
                              /* Note: This information will be used when  */
                              /* L2-Ack is received from NW.This will tell */
                              /* how many SDUs need to be freed. If LI=0/  */
                              /* 0x7ffb needs to be sent in next PDU then  */
                              /* num_sdus need to be incremented in the    */
                              /* current PDU information.                  */
  uint8 num_sdu;
}rlc_am_pdu_info_type;

typedef struct 
{
  rlc_am_pdu_info_type pdu_info; /* New Book Keeping Information           */
  uint8  vt_dat_cur;                      /*bit(6..0)-No.of times a PDU txd*/
                                          /* bit(7) = 1 - PDU NAK'd        */
                                          /* bit(7) = 0 - Not NAK'd        */

#if  defined(FEATURE_MAC_I)
  uint16 tx_sn;
  uint16 pdu_size_bits;
#endif
}rlci_txq_element_type;

/*--------------------------------------------------------------------------
  Used to store the AM transmission queue and a boolean to indicate if 
  the AM transmission queue is in use for a particular AM channel
--------------------------------------------------------------------------*/

typedef struct
{
  boolean in_use;                         /* indicates if the transmission
                                             queue for a particular AM 
                                             channel is in use             */
  rlci_txq_element_type *am_txq;
                                          /* AM transmission queue has RLC
                                             TXQ_SIZE as 2047 right now, 
                                             Memory allocation for this Q is 
                                             done during LC establishment.  */
} rlci_am_txq_type;

/*--------------------------------------------------------------------------- 
 Definition for R99 PDU dsm pool.
---------------------------------------------------------------------------*/
typedef struct
{
  /* DSM item used to build data PDU in this TTI.  */
  dsm_item_type *item_ptr[MAX_UL_PDUS_PER_TTI];

  /* No. of DSM items used. */
  uint8         num_item;
}rlc_pdu_dsm_type;



typedef struct
{
  rlc_ctrl_pdu_type ctrl_pdu[MAX_CTRL_DSM_ITEMS];
  
  /* No. of DSM items used. */
  uint8         num_item;
}rlc_am_ctrl_dsm_type;

typedef struct
{
  rlc_ctrl_pdu_type ctrl_pdu[MAX_MAC_I_CTRL_DSM_ITEMS];
  
  /* No. of DSM items used. */
  uint8         num_item;
}rlc_am_mac_i_dsm_type;
/*--------------------------------------------------------------------------- 
  Used to store the necessary up-link control information for an RLC AM 
  entity.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_am_common_config_type  am_common_local;   /* Local copy common config variables.      */ 
  rlc_ul_am_config_type  ul_cnfg_var_local;              /* config information from RRC.  */
  rlc_am_common_config_type *am_common;   /* ptr to common config variables.      */ 
  rlc_ul_am_config_type *ul_cnfg_var;     /* pointer to config information from RRC.  */
  rlci_ul_am_state_type ul_state;         /* Control state variables.      */

  rlc_tmr_status_type ul_tmr_sts;         /* holds status of RLC AM timer */

  rex_timer_type tmr_discard_handle;      /* RLC timer for timer based SDU */
                                          /* discard                       */
  uint16 tmr_discard_sequence_num;        /* Sequence number for which     */
                                          /* discard timer is running      */

  rex_timer_type* tmr_cellpch_ptr;        /* Pointer Rex timer Structure   */

  rlc_opt_status_type    ul_status_opt_bitmap; /*UL rtx status bitmap array   */
  uint16                 ul_num_sn_li;    /*number of SN-LI pairs to be formed*/
                                          /*on UL, basically indicates status BO*/
                                          /*on UL in terms of number of SN-LI pairs*/
  q_type mrw_q;                           /* MRW queue                     */
  q_type mrw_ack_q;                       /* MRW ACK queue                 */
                                               
  q_type internal_q;                      /* Contains SDUs that are tx and */
                                          /* their corresponding PDUs are  */
                                          /* yet to be acknowledged.       */
  boolean  sdu_cnf_pending;               /* TRUE - if sdu cnf is delayed  */
                                          /* because LI_exact/LI_octet_less*/
                                          /* present in next pdu, but ack'd*/
                                          /* till this pdu. FALSE-otherwise*/
  void (*sdu_cnf_cb) (rlc_lc_id_type ul_data_lc_id, 
                      uecomdef_status_e_type status,
                      uint8 mui,
                      rlc_sdu_tx_fail_reason_e_type failure_reason
                      );                  /* SDU confirm callback          */
                                          /* function.                     */

  rlci_am_txq_type *am_txq_ptr;           /* AM transmission queue pointer */

boolean sdu_start_seg_ack;  /* 1 - pdu containing start segment of this SDU*/
                            /* is ack'd, but remaining pdus containing this*/
                            /* SDU segments are yet to be transmitted.     */
                            /* 0 - other wise                              */
#ifdef FEATURE_RLC_QXDM_LOGGING
          
  rlc_ul_chan_am_stat_type ul_chan_am_stat; /* Stores the Am statistics    */
                                            /* info.                       */  
  rlc_ul_chan_am_pdu_type  ul_chan_am_pdu;   /* Stores the PDU information */
                                             /* for a single AM entity     */
#endif //FEATURE_RLC_QXDM_LOGGING

  multiclient_fc_arbitarator_type multiclient_dl_fc_arbitarator;
  multiclient_fc_arbitarator_type multiclient_ul_fc_arbitarator;
#ifdef FEATURE_MAC_I
  uint8 num_sdu_processed;
  rlc_ul_sdu_len_q_info *rlc_ul_sdu_len_q;
  int8 num_sdu_temp_q;
  dsm_item_type *temp_sdu_mismatch_q[RLC_UL_AM_MAX_TCP_ACK_MISMATCH_SDU];
#endif

#ifdef FEATURE_WCDMA_UL_COMPR
  q_type ul_comp_dsm_q;                   /* DSM items to be used for compression */
  q_type ul_orig_q;                       /* Contains the uncompressed SDUs that
                                             are pending transmission      */
  q_type ul_comp_q;                       /* Contains the compressed SDUs that
                                             are pending transmission      */
  
  uint16 ul_comp_sdu_bytes;                                               
  
  rlc_ptp_ul_comp_config_type           rlc_ptp_ul_comp_config;
  rlci_ptp_sufi_type                    ul_comp_rsp_pending;
  rlci_ptp_sufi_rsp_reason_type         ul_comp_rsp_reason;
  boolean                               ul_comp_rsp_pending_to_go_now;
  uint32                                bo_multiplier; 
  dacc_handle_type                      dacc_handle;
#endif                                          

} rlci_ul_am_ctrl_type;



/*--------------------------------------------------------------------------- 
  Typedef for storing parameters specific to transparent mode.
---------------------------------------------------------------------------*/
typedef union {
  rlci_ul_tm_ctrl_type tm_ctrl;        /* Transparent Mode parameters.     */
  rlci_ul_um_ctrl_type um_ctrl;        /* Unacknowledged Mode parameters.  */
  rlci_ul_am_ctrl_type am_ctrl;        /* Acknowledged Mode parameters.    */
} rlci_ul_ctrl_u_type;


/*---------------------------------------------------------------------------
  Used to store information about a uplink logical channel. 
---------------------------------------------------------------------------*/
typedef  struct {
  uecomdef_logchan_e_type lc_type;      /* Uplink logical channel type     */
  uecomdef_logch_mode_e_type lc_mode;   /* RLC Mode                        */
  rlci_ul_state_e_type lc_state;        /* Uplink Logical channel state    */
  rlci_ciphering_type cipher;           /* Ciphering Parameters            */
  rlci_ciphering_type cipher_backup;    /* Backup for ciphering parameters
                                           while applying the new one      */
  rlci_ul_ctrl_u_type ctrl;             /* Union of TM, UM and AM specific */
                                        /* parameters.                     */
  rex_timer_type* tmr_discard_ptr;      /* Pointer Rex timer Structure     */
                                        /* for Timer discard using.        */
  dsm_watermark_type *ul_wm_ptr;        /* Pointer to the associated RLC   */
                                        /* uplink data queue               */
  dsm_item_type *dequeue_sdu_head_ptr;  /* Pointer to head of dequeued SDUs*/
  dsm_item_type *dequeue_last_ack_ptr;  /* Pointer to the last TCP ACK that 
                                           was dequeued but not transmitted*/
  uint16  num_sdus_dequeued;            /* Number of SDUs in dequeue chain */
  uint32  length_sdus_dequeued;         /* Total length of SDUs dequeued   */                         
  dsm_item_type *cur_sdu_ptr;           /* Point to the starting of data   */
                                        /* on current SDU DSM chain        */ 
  uint16  cur_sdu_len;                  /* Size of the current SDU         */
  uint16  cur_sdu_tx_len;               /* Bytes txd from the current SDU  */
  boolean special_ind;                  /* Boolean to indicate if          */
                                        /* the first length indicator value*/
                                        /* of the next PDU should be a     */
                                        /* special indicator (value 0x00). */
  boolean octet_less;                   /* Boolean to indicate if the last */
                                        /* PDU was one octet short of      */
                                        /* filling in the LI.              */
  boolean padding;                      /* Boolean to indicate if the rest */
                                        /* of the PDU(s) in the TTI should */
                                        /* have padding bits.              */
  boolean skip_pdu_build;               /*Boolean to indicate if PDU building
                                          is to be skipped for curr TTI
                                          if TRUE, zero BO is 
                                          reported for that logical channel*/
#ifdef FEATURE_RLC_QXDM_LOGGING
  boolean ul_pdu_logging;     /* boolean to indicate if set to TRUE  then 
                                                    dont fill the logging structure in ctr block*/
#endif
#ifdef FEATURE_MAC_I
  uint16 mac_i_ps_sn;
  boolean mac_i_ps_sn_poll;
  uint32 mac_i_ps_sn_pdu_size;
  boolean mac_i_ciph_enabled;
  uint32 mac_i_count_c;
  uint8  mac_i_key_idx;
  uint8  mac_i_algo;
#endif  
} rlci_ul_ctrl_blk_type;

 
/*---------------------------------------------------------------------------
  Database that is used to store information about all the uplink 
  logical channels. This database should be initialized when the uplink 
  RLC task is started. The maximum number of entries in the table is 21.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 active_ch;                      /* Number of active uplink logical */
                                        /* channels.                       */
  rlci_ul_ctrl_blk_type ctrl_blk[UE_MAX_UL_RLC_ENTITY];
                                        /* Data structure used to store all*/ 
                                        /* the information for the uplink  */
                                        /* logical channel.                */
  wm_cb_type data_ind_in_fach_func_ptr; /* CB to indicate RRC of data presence
                                           in FACH state                   */ 
} rlci_uplink_table_type;

#ifdef FEATURE_RLC_QXDM_LOGGING
/*---------------------------------------------------------------------------
  The following two structures are used only for QXDM logging.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data Structure for RLC UL states for Transparent, Unacknowledged and 
  Acknowledged Modes.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8    rlc_id;              /* UL logical channel id (0..18)           */
  uint8    lc_state;            /* State of the RLC State Machine          */
                                /*   0 - RLC_UL_NULL_STATE                 */
                                /*   1 - TM_UL_DATA_TRANSFER_READY         */
                                /*   2 - UM_UL_DATA_TRANSFER_READY         */
                                /*   3 - UM_UL_WAITING_TO_SUSPEND          */
                                /*   4 - UM_UL_SUSPENDED                   */
                                /*   5 - AM_UL_DATA_TRANSFER_READY         */
                                /*   6 - AM_UL_WAITING_TO_SUSPEND          */
                                /*   7 - AM_UL_SUSPENDED                   */
                                /*   8 - AM_UL_RESET_PENDING               */
                                /*   9 - AM_UL_CONFIG_PENDING              */
} rlc_ul_chan_state_type;

typedef struct
{
  uint8 nchan;                  /* Number of Uplink logical channels.      */
  rlc_ul_chan_state_type ul_state[UE_MAX_UL_LOGICAL_CHANNEL];                                                 
                                /* State Information for each Uplink       */
                                /* logical channel.                        */

}rlc_ul_log_state_type;

#endif //FEATURE_RLC_QXDM_LOGGING

/*---------------------------------------------------------------------------
  Table to store Uplink SDU tx conformation.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;         /* AM UL logical channel id                */
  uint8 mui;                    /* SDU discarded whose failure             */
                                /* confirmation is pending                 */
}rlci_ul_am_sdu_tx_cnf_type ;

/*-------------------------------------------------------------------------
  These queues hold the UM SDUs transmitted in this TTI for non-EDCH and 
  EDCH channels respectively. SDUs from these queues are freed during next 
  TTI build request for the corresponding channel.
-------------------------------------------------------------------------*/   
extern rlc_um_sdu_q_type rlc_free_sdu_q_non_edch;
extern rlc_um_sdu_q_type rlc_free_sdu_q_edch;

extern free_sdu_buffer_type free_sdu_buffer_edch [MAX_SDU_PER_TTI_EDCH];
extern free_sdu_buffer_type free_sdu_buffer_non_edch [MAX_SDU_PER_TTI_NON_EDCH];

/* rlc_id to ul control block index table */
extern uint8 ul_ctl_blk_id_tbl[UE_MAX_UL_LOGICAL_CHANNEL];
/* UL control block */
extern rlci_uplink_table_type uplink_table;


#define RLCI_GET_UL_CTRL_INDEX(lc_id)   ul_ctl_blk_id_tbl[lc_id]
#define RLCI_GET_UL_CTRL_BLK_PTR(ctrl_index)    &(uplink_table.ctrl_blk[ctrl_index])

/*===========================================================================

FUNCTION RLCI_UL_AM_BASE_TIMER_HANDLER

DESCRIPTION
  Handle 10ms Base Timer timed out.
  In Uplink, there is only one base timer. All timers related in the Uplink
  are done in the form of counters. Every time the 10ms base timer expired,
  this function looks into which timer counters are needed to be increased
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
void rlci_ul_am_base_timer_handler (void);

typedef enum
{
  RLC_FC_CMD_NULL_ENTITY,
  RLC_FC_CMD_LARGE_DSM_ENTITY,
  RLC_FC_CMD_SMALL_DSM_ENTITY,
  RLC_FC_CMD_DUP_ENTITY
} rlc_fc_cmd_entity_type;

typedef struct 
{
  q_link_type               link;
  rlc_fc_cmd_entity_type    entity;
  uint16                    cmd;
}rlc_fc_cmd_type;

/*===========================================================================

FUNCTION RLC_UL_INIT

DESCRIPTION
  Initializes the RLC Uplink Logical channel table.
  
DEPENDENCIES
  This function should be called at task start up.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlc_ul_init
(
  void
);

/*===========================================================================

FUNCTION CRLC_UL_CIPHER_CONFIG_HANDLER

DESCRIPTION
  Configures ciphering on one or more Unacknowledged or Acknowledged 
  Mode Channels.  This function configures the ciphering parameters that are 
  recevied from RRC. The ciphering parameters are updated only if the the 
  channel is in UL_DATA_TRANSFER_READY STATE.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_cipher_config_handler
( 
  rlc_ul_cipher_config_type *cipher_ptr  /* Pointer to ciphering config.   */
);

/*===========================================================================

FUNCTION CRLC_UL_RESUME_HANDLER

DESCRIPTION
  Resume transmission on one or more uplink unacknowledged or acknowledged 
  mode channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_resume_handler
( 
  rlc_ul_resume_type *resume_ptr         /* Pointer to Resume Information  */
);

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
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
void rlc_ul_register_trigger_based_rach_api_active_rbs(void);

/*===========================================================================

FUNCTION RLC_UL_REGISTER_TRIGGER_BASED_API_PER_LC

DESCRIPTION
  Register trigger based RACH API or trigger RACH if data is already present 
  for the given RLC ID.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_register_trigger_based_api_per_lc(rlc_lc_id_type rlc_id,  rlci_ul_ctrl_blk_type *ctrl_blk_ptr);
#endif /* FEATURE_WCDMA_TRIGGER_BASED_RACH */
/*===========================================================================

FUNCTION CRLC_DL_HFN_HANDLER

DESCRIPTION
  Gets the hfn value for one or more uplink unacknowledged or acknowledged 
  mode logical channels  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_hfn_cnf
(
  rlc_ul_hfn_cnf_type *hfn_ptr           /* Pointer to HFn information     */
);

/*===========================================================================

FUNCTION CRLC_UL_HFN_HANDLER

DESCRIPTION
  Gets the hfn value for one or more uplink unacknowledged or acknowledged 
  mode logical channels
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_hfn_handler 
(
  rlc_ul_hfn_type *hfn_ptr               /* Pointer to HFN Cnf Info.       */
);

/*===========================================================================

FUNCTION CLRLC_UL_REG_FOR_DATA_IND_HANDLER

DESCRIPTION
  Register Data Indication callback function to registered
  WM's non_empty_func_ptr.  
  This function is for Cell_PCH.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void clrlc_ul_reg_for_data_ind_handler
(
  rlc_ul_register_for_data_ind_type *ul_reg_data_ind_ptr
);

/*===========================================================================

FUNCTION CLRLC_UL_UNREG_FOR_DATA_IND_HANDLER

DESCRIPTION
  De-Register Data Indication callback function from registered
  WM's non_empty_func_ptr.  
  This function is for Cell_PCH.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void clrlc_ul_unreg_for_data_ind_handler
(
  rlc_ul_unregister_for_data_ind_type *ul_unreg_data_ind_ptr
);

/*===========================================================================

FUNCTION CRLC_UL_TM_CONFIG_HANDLER

DESCRIPTION

  This function handles the configuration of Transparent Mode Logical 
  channels. This function is called when RRC sends a CRLC_TM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the channel is set to the NULL state
  and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the downlink table entry for the 
  channel is updated with the configuration received from RRC.

  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void crlc_ul_tm_config_handler 
(
  rlc_ul_tm_config_type *tm_ptr         /* Pointer to TM config Info.      */
);

/*===========================================================================

FUNCTION CRLC_UL_UM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of Unacknowledged Mode Logical 
  channels. This function is called when RRC sends a CRLC_UM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the channel is set to the NULL state
  and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the downlink table entry for the 
  channel is updated with the configuration received from RRC.


DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void crlc_ul_um_config_handler 
(
  rlc_ul_um_config_type *um_ptr           /* Pointer to UM config Info     */
);

/*===========================================================================

FUNCTION CRLC_UL_AM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of an Acknowledged Mode entity.
  it is called when RRC sends a CRLC_UL_AM_CONFIG_REQ to RLC. If a new 
  entity is to be established, the function checks if the channels exists. 
  If it already exists, an ERR message is dispalyed. If the entity does not 
  exist, an uplink table entry for this entity is updated and the state is
  set to DATA_TRANSFER_READY. 
  If an entity is to be released,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the channel is set to the NULL state
  and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the 
  DATA_TRANSFER_READY state. If it is, the downlink table entry for the 
  channel is updated with the configuration received from RRC.


DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_am_config_handler 
(
  rlc_am_config_req_type *am_ptr
);

/*===========================================================================

FUNCTION RLC_UL_REGISTER_AM_SDU_CNF_CB_REQ

DESCRIPTION
  Register the SDU confirm callback function as request by RRC.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_register_am_sdu_cnf_cb_req
(
  rlc_register_am_sdu_confirm_callback_type *sdu_cnf_cb_req
);
#ifdef FEATURE_WCDMA_IGNORE_DATA_RB_DURING_CALL_REL
/*===========================================================================

FUNCTION RLC_UL_CHECK_FOR_RB_DATA_ALLOWED

DESCRIPTION
 API to check if PDU building is allowed for a particular RB or not.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if PDU building is allowed or else returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
inline boolean rlc_ul_check_for_rb_data_allowed
(
  boolean                 rrc_release_rab_active_flag,
  uecomdef_logchan_e_type lc_type,
  rlc_lc_id_type          rlc_id
);
#endif
/*===========================================================================

FUNCTION RLCI_GET_CTL_BLK_PTR

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI. 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
rlci_ul_ctrl_blk_type *rlci_get_ctl_blk_ptr 
(
  byte ch_id
);
/*===========================================================================
FUNCTION RLCI_SET_UL_NUM_SN_LI

DESCRIPTION
  Updates ul_num_sn_li, which indicates the number of SN-Li
  pairs which will formed on UL. This API is used to copy number
  SN-LI pairs calculated in DL to UL. 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_set_ul_num_sn_li 
(
  uint8   lc_id,
  uint16  num_sn_li
);
/*===========================================================================

FUNCTION RLCI_DL_CONFIG_CNF_HANDLER

DESCRIPTION
  Handle the config confirmation from the Downlink side. This is for AM.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_config_cnf_handler
(
  rlci_dl_config_cnf_type *cnf_ptr
);

/*===========================================================================

FUNCTION CRLC_UL_AM_RESET_HANDLER

DESCRIPTION
  Reset all AM entities that are configured.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_am_reset_handler(void);

/*===========================================================================

FUNCTION RLCI_UL_PROC_RESET_MSG

DESCRIPTION
  Process the RLCI_DL_RESET_DONE and RLCI_UL_RESET_REQ messages from 
  DL AM RLC. 
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_proc_reset_msg
(
  wcdma_l2_ul_cmd_enum_type cmd_id, 
  rlc_lc_id_type ul_rlc_data_id
);

/*===========================================================================

FUNCTION RLCI_AM_HANDLE_10MS_TIMER_CB

DESCRIPTION
  Handle 10ms Base Timer timed out. 
  In Uplink, there is only one base timer. All timers related in the Uplink
  are done in the form of counters. Every time the 10ms base timer expired,
  this function looks into which timer counters are needed to be increased
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
void rlci_am_handle_10ms_timer_cb(uint32 cb_param);

/*===========================================================================

FUNCTION  RLCI_TM_HANDLE_DISCARD_TIMER_CB

DESCRIPTION
  This callback function is called when the SDU discard timer expired.
  This function scan the WM to see whether there are SDUs needed to be 
  removed since they stay in the WM for too long. Also, it put the SDU
  discard time stamp on each new incoming SDU.
  
  The SDU discard time stamp is put in the 18th bit of the app_field, 
  since the lower 16 bits are used as the bit count of the DSM packet 
  chain.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_tm_handle_discard_timer_cb (uint32 cb_param);


/*===========================================================================

FUNCTION  RLCI_UM_HANDLE_DISCARD_TIMER_CB

DESCRIPTION
  This callback function is called when the SDU discard timer expired.
  This function scan the WM to see whether there are SDUs needed to be 
  removed since they stay in the WM for too long. Also, it put the SDU
  discard time stamp on each new incoming SDU.
  
  The SDU discard time stamp is put in the 3rd bit of the app_field, 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_um_handle_discard_timer_cb(uint32 cb_param);

/*===========================================================================

FUNCTION RLCI_AM_HANDLE_CELLPCH_TIMER_CB

DESCRIPTION
  Process CELLPCH_TIMER.
  
  This timer is set because of finding some retx PDUs remaining when RRC 
  registers data_ind CB to go to CELL_PCH. Since the RRC has to finish 
  its procedure, we set the timer and come back later to move UE out of
  CELL_PCH to send those remaining retx PDUs.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_am_handle_cellpch_timer_cb(uint32 cb_param);

/*===========================================================================

FUNCTION CRLC_RELEASE_ALL_HANDLER

DESCRIPTION
  Releases all Uplink Channels and sends a command to the Downlink to 
  release all downlink channels.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void crlc_release_all_handler( void );


/*===========================================================================

FUNCTION RLCI_POST_DL_RESET_MSG

DESCRIPTION
  Post RESET messages to the DL AM RLC. The RESET messages could be 
  RLCI_DL_RESET_REQ or RLCI_UL_RESET_DONE.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_dl_reset_msg
(
  void
);

/*===========================================================================

FUNCTION RLCI_POST_RESET_TO_RRC

DESCRIPTION
  Post the RESET result message to RRC
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_reset_to_rrc
(
  void
);

/*===========================================================================

FUNCTION RLC_GET_DATA_COUNTERS

DESCRIPTION
  Obtain the number of bytes of user data sent/received to/from the network.

DEPENDENCIES
  None.
  
RETURN VALUE
  FALSE: Channel is not existed.
  TRUE: Obtains data successfully.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_get_data_counters
( 
  rlc_lc_id_type lc_id, 
  uint32 *rx_from_network_byte_cnt, 
  uint32 *tx_to_network_byte_cnt 
);

/*===========================================================================

FUNCTION RLCI_POST_RLC_UL_RESET_DONE_SIG

DESCRIPTION
  This function post RLC_AM_UL_RESET_DONE_SIG to L2 UL task. When L2 UL task 
  sees this message in the task context, it posts RLCI_UL_RESET_DONE to RLC
  DL.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_rlc_ul_reset_done_sig
(
  rlc_lc_id_type  rlc_id
);

/*===========================================================================

FUNCTION RLCI_POST_RLC_UL_RESET_DONE_CMD

DESCRIPTION
  Posts RLCI_UL_RESET_DONE to RLC DL. This function is excuted in the task
  context.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_rlc_ul_reset_done_cmd
(
  void
);

#ifdef FEATURE_RLC_QXDM_LOGGING
uint32 rlc_ul_enh_update_log_data(uint32           num_src_desc,
                                       uint32           initial_io_vec_len,
                                       uint32           pdu_size_bytes,
                                       rlc_buff_type    *hdr_list, 
                                       l1_l2_data_type  *l1_ul_tx_buf_ptr,
                                       uint32            data_length_bits);

/*===========================================================================

FUNCTION RLCI_UL_AM_HANDLE_QXDM_LOG_TIMER

DESCRIPTION
  Collect the AM statistics data and send the statistics log packet out.
    
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_ul_am_handle_qxdm_log_timer(uint32 timer_sig);

/*===========================================================================

FUNCTION RLCI_GET_UL_PDU_CIPHER_LOG_ON

DESCRIPTION
  This function will be call when each PDU is building. This function is to 
  avoid to check the logging bit from the DIAG every time when a PDU is 
  building, variable rlci_ul_pdu_cipher_log_on will be updated at the 
  beginning of building PDUs each TTI. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PDU ciphering parameters logging is on.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_get_ul_pdu_cipher_log_on (void);

/*===========================================================================

FUNCTION RLCI_GET_UL_PDU_CIPHER_LOG_ON

DESCRIPTION
  Provide the access pointer of rlci_ul_pdu_cipher_log data structure.  

DEPENDENCIES
  None.

RETURN VALUE
  pointer of rlci_ul_pdu_cipher_log.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ul_pdu_cipher_info_type *rlci_get_ul_pdu_cipher_log_ptr(rlc_lc_id_type ul_rlc_id);


#endif //FEATURE_RLC_QXDM_LOGGING

/*===========================================================================

FUNCTION RLC_UL_PURGE_UPLINK_WM

DESCRIPTION
  Purges the SDUs from the uplink WM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_purge_uplink_wm
(
  rlc_ul_purge_wm_ind_type *ul_purge_wm_ind
);

/*===========================================================================

FUNCTION CRLC_UL_ABORT_CIPHER_CONFIG

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
void crlc_ul_abort_cipher_config
(
  rlc_abort_cipher_config_type *abort_cipher_info
);

/*===========================================================================

FUNCTION  RLCI_AM_HANDLE_DISCARD_TIMER_CB

DESCRIPTION
  This callback function is called when the SDU discard timer expires.
  This function posts a command for RLC to process the expired timer. 
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_am_handle_discard_timer_cb(uint32 cb_param);

/*===========================================================================

FUNCTION RLC_UL_APP_NEW_WINSUFI

DESCRIPTION
  Assigns new window size required by APP_CLOSE and transmits new window sufi.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_app_new_winsufi
(
  rlc_lc_id_type rlc_id,
  rlci_peer_tx_win_size_e_type peer_win
);

/*===========================================================================

FUNCTION RLCI_UL_NUM_ACTI_NON_SRB_AM_CH

DESCRIPTION
  Returns number of non SRB AM channels established.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_ul_num_acti_non_srb_am_ch(void);

/*===========================================================================

FUNCTION RLCI_POST_SDU_TX_CNF

DESCRIPTION
  Calls the sdu_cnf_cb function.The SDU tx confirmation delay information
  is stored in am_ul_sdu_tx_cnf.This function is called in L2-UL context.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_post_sdu_tx_cnf(void);

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_START_UL_LOGGING

DESCRIPTION
   This function will start logging AM/UM PDUs.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_start_ul_logging (void);
/*===========================================================================

FUNCTION RLC_START_UL_LOGGING

DESCRIPTION
   This function will start logging AM/UM PDUs.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_start_eul_logging (void);

#endif

/*===========================================================================

FUNCTION CRLC_UL_CONTINUE_HANDLER

DESCRIPTION
  Continue transmission on one or more uplink unacknowledged or acknowledged
  mode channels.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_ul_continue_handler
(
  rlc_ul_continue_type *continue_ptr      /* Ptr to continue Info                */
);

/*===========================================================================

FUNCTION RLC_UL_FORM_IO_VEC_PER_SDU

DESCRIPTION

  Parses thru the SDU dsm chain to identify the offset with in it, and 
  forms IO vectors for the dsm items corresponding to the PDU payload

DEPENDENCIES
  None.

RETURN VALUE
  Amount of data from this SDU goes as payload to PDU.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_form_io_vec_per_sdu
(
  dsm_item_type       *dsm_ptr,          /* Start dsm_ptr of the SDU chain */
  uint16              offset,            /* Offset with the SDU chain      */
  uint16              payload_len,       /* Amount of data for the PDU     */
  l1_l2_data_type    *l1_ul_tx_buf_ptr  
);

/*===========================================================================

FUNCTION RLC_UL_FORM_NON_EDCH_IO_VEC_PER_SDU

DESCRIPTION

  Parses thru the SDU dsm chain to identify the offset with in it, and 
  forms IO vectors for the dsm items corresponding to the PDU payload

DEPENDENCIES
  None.

RETURN VALUE
  Amount of data from this SDU goes as payload to PDU.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_ul_form_non_edch_io_vec_per_sdu
(
  dsm_item_type       *dsm_ptr,          /* Start dsm_ptr of the SDU chain */
  uint16              offset,            /* Offset with the SDU chain      */
  uint16              payload_len        /* Amount of data for the PDU     */
);

/*===========================================================================

FUNCTION RLC_UL_UPDATE_HDR

DESCRIPTION

  Stores the RLC PDU(AM/UM) header, LIs into the local buffer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_update_hdr
(
  uint8         hdr_len,
  uint16        hdr_val,
  rlc_buff_type *hdr_list 
);

/*===========================================================================

FUNCTION RLC_UL_FILL_IO_VEC

DESCRIPTION

  Stores the IO vector into the L1 tx buffer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_fill_io_vec
( 
  uint8             *data_ptr,        /* Source location                */
  uint32            len,              /* No. of bytes                   */
  l1_l2_data_type  *l1_ul_tx_buf_ptr, /* Place this IO vector is stored */
  boolean           last_io_vec  
);

/*===========================================================================

FUNCTION RLC_UL_GET_UL_HDR

DESCRIPTION
  
  Returns the pointer to the gloabal header buffer ul_hdr.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
rlc_ul_hdr_buff_type* rlc_get_ul_hdr(boolean isEul);

/*===========================================================================

FUNCTION RLC_UL_GET_PRE_ALLOC_PDU_DSM

DESCRIPTION

  Provides the next available DSM item for the pre allocated pool for
  building the R99 PDUs.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the available DSM item form the pool other wise NULL pointer.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* rlc_ul_get_pre_alloc_pdu_dsm(void);

/*===========================================================================

FUNCTION RLC_UL_INIT_IO_VEC_LIST

DESCRIPTION

  Intialising global io_vec_list before storing the set IO vector for 
  while building R99 AM new/re-tx, UM PDUs.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_init_io_vec_list(void);

/*===========================================================================

FUNCTION RLC_UL_BUILD_NON_EDCH_PDU

DESCRIPTION

  Builds the R99 PDU using new io vector method of building PDUs

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE  - R99 pdu successfully build
           FASLE - R99 pdu building failed

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_build_non_edch_pdu
(
  rlc_buff_type             *hdr_list,   /* Pointer to the headers         */
  rlc_ul_pdu_data_type      *pdu_info,   /* valid if chnl_type = NON_E_DCH */
  uint16                    padding_len  /* PDU padding length             */
  ,uint32                   key_index,   /* Ciphering key_id               */
  boolean                   enabled,     /* TRUE - cipheing applicable     */
                                         /* FALSE - otherwise              */
  uecomdef_wcdma_cipher_algo_e_type cipher_algo,
  uint32                    cnt_c        /* COUNT_C parameter              */
);

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
void rlc_ul_init_ctrl_dsm_pool(void);

/*===========================================================================

FUNCTION RLC_DSM_PACK_DATA

DESCRIPTION

  This will pack the amount of bits from the specified byte & bit position
  in the data_ptr of the PDu

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dsm_pack_data
(
  dsm_item_type *pdu_ptr,   /* Pointer to the PDU dsm item    */
  uint32        pack_data,  /* Data to pack                   */
  uint16        bit_pos,    /* Bit position from the 1st byte */
  uint8         data_len    /* No. of bits to pack            */
);

/*===========================================================================

FUNCTION RLCI_GET_ACTIVE_UL_UM_CHNLS

DESCRIPTION
  This function returns the number of active UM channels 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlci_get_active_ul_um_chnls (void);

/*=========================================================================

FUNCTION RLCI_CHANGE_PEER_TX_WIN_FC

DESCRIPTION

    This function changes the peer side's transmission window size in case of
    RESET procedure is finished and RLC  wants to change the peer window size to
    the last FC window size

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

uint16 rlci_change_peer_tx_win_fc
(
   rlci_ul_ctrl_blk_type *ctl_blk_ptr
); 


/*===========================================================================

FUNCTION RLCI_UL_DSM_FC_UPDATE_PEER_RX_WIN

DESCRIPTION
  Assigns new window size required by RX_TX_CLOSE and transmits new window sufi.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_dsm_fc_update_peer_rx_win
(
  rlc_lc_id_type rlc_id,
  rlci_peer_tx_win_size_e_type peer_win
);
/*===========================================================================

FUNCTION UL_ASSIGN_CTL_BLK

DESCRIPTION
  Assign Uplink control block for an RLC entity.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint8 ul_assign_ctl_blk(void);

/*===========================================================================

FUNCTION CRLC_UL_RESUME_CNF

DESCRIPTION
  Confirms suspension of one or more logical channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_resume_cnf
( 
  rlc_ul_resume_cnf_type *resume_ptr    /* Ptr to resume Cnf Info       */
);

/*===========================================================================

FUNCTION  RLCI_SET_SDU_DISCARD_TIMER 

DESCRIPTION
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_set_sdu_discard_timer
(
  uint8 index, 
  rlc_lc_id_type rlc_id
);

/*===========================================================================

FUNCTION RLC_UL_REMOVE_SDUS_FROM_WM

DESCRIPTION
  This function removes the SDUs from the UL WM. The number of SDUs that 
  are purged is the minimum of number specified by higher layers and the
  number of SDUs present in the WM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ul_wm_purge_rsp_status rlc_ul_remove_sdus_from_wm
(
  rlci_ul_ctrl_blk_type *ctrl_blk_ptr,
  uint8 num_sdus
);

/*===========================================================================

FUNCTION RLC_UL_COPY_IO_VEC_DATA

DESCRIPTION

  Copies the data from the set of IO vectors into the pre allocated DSM item
  while building R99 pdus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_copy_io_vec_data
(
  dsm_item_type *item_ptr,  /* Pointer to the pre allocated DSM item       */
  uint16         offset      /* Offset with in the DSM item, from where the */
                            /* data copy should begin                      */
);

/*===========================================================================

FUNCTION RLC_PROCESS_TA_IND

DESCRIPTION
  Processes the QTA/LTA Start/Stop received from RRC

===========================================================================*/
void rlci_process_ta_ind(rlc_ta_info_type *ta_info_ptr);

/*===========================================================================

FUNCTION RLC_PROC_LTA_START_IND

DESCRIPTION
  Processes LTA Start Indication received from RRC

===========================================================================*/
void rlc_proc_lta_start_ind(rlc_ta_success_e_type ta_succ_ind);

/*===========================================================================

FUNCTION RLC_PROC_LTA_STOP_IND

DESCRIPTION
  Processes LTA Stop Indication received from RRC

===========================================================================*/
void rlc_proc_lta_stop_ind(rlc_ta_success_e_type ta_succ_ind);

/*===========================================================================

FUNCTION RLC_PROC_QTA_START_IND

DESCRIPTION
  Processes QTA Start Indication received from RRC

===========================================================================*/
void rlc_proc_qta_start_ind(rlc_ta_success_e_type ta_succ_ind);

/*===========================================================================

FUNCTION RLC_PROC_QTA_START_IND

DESCRIPTION
  Processes QTA Stop Indication received from RRC

===========================================================================*/
void rlc_proc_qta_stop_ind(rlc_ta_success_e_type ta_succ_ind);

/*===========================================================================

FUNCTION RLC_TRIGGER_STATUS_ON_AM_CHNLS

DESCRIPTION  
  RLC will generate status PDU for all active AM channels
  
===========================================================================*/
void rlc_trigger_status_on_am_chnls();

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_UL_INIT_AM_LOG_PARAMS

DESCRIPTION

 Initalizes the logging_params for the AM PDUs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_init_am_log_params(void);

/*===========================================================================

FUNCTION RLC_UL_CHECK_AM_LOG_STATUS

DESCRIPTION

  Checks if the user plane/signalling plane PDU logging is turned on or not.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE  if PDU needs to be logged
            FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_ul_check_am_log_status
(
  rlci_ul_am_ctrl_type    *am_ctrl_ptr, /* Pointer to ul_chan_am_pdu   */
  rlc_lc_id_type          rlc_id,       /* RLC ID                      */
  uint16                  pdu_size      /* RLC PDU SIZE                */
);

#endif

/*===========================================================================

FUNCTION RLCI_UPDATE_TX_WIN_SIZE

DESCRIPTION
  Received Tx_Window_Size from the peer side. Update VT(MS) according the
  newly received tx_window_size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_update_tx_win_size
(
  rlci_ul_am_ctrl_type *ctrl_blk,
  uint16          tx_win_size           /* tx_window_size                 */
);
#ifdef FEATURE_QXDM_DSM_TRACING
/*===========================================================================

FUNCTION RLC_FC_GET_DSM_TRACE_CB

DESCRIPTION
  This function returns the number of active UM channels 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_fc_get_dsm_trace_cb
(
    uint32 cb_data, 
    fc_dsm_trace_info_type *wm_stat_ptr
);
#endif

#ifdef FEATURE_WCDMA_SIMULATE_AMR_PACKETS
/*===========================================================================

FUNCTION rlci_ul_tm_enq_data_sig_handler

DESCRIPTION
  Function will enqueue one SDU into UL-WM queue for TM channels
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_tm_enq_data_sig_handler(void);

#endif /*FEATURE_WCDMA_SIMULATE_AMR_PACKETS*/
/*===========================================================================

FUNCTION RLC_DUMP_UL_CTL_BLK_ID_TBL

DESCRIPTION
  dumps uplink_table index values of LCs within ul_ctl_blk_id_tbl

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dump_ul_ctl_blk_id_tbl(void);
/*===========================================================================

FUNCTION RLC_SANITIZE_UL_CTL_BLK_ID_TBL

DESCRIPTION
  Checks for any duplicate uplink_table index values of two different LCs within ul_ctl_blk_id_tbl

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_sanitize_ul_ctl_blk_id_tbl(void);


#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
/*===========================================================================

FUNCTION RLC_ENH_HS_RACH_CHECK_RACH_TRIGGER

DESCRIPTION
  Query WL1 whether to send ext command to enable Build frame indication- If TRUE and 
  If RLC had any new CTRL PDU pending for transmission, Put the CPHY_UL_TX_CONTROL_REQ 
  command in L1 External Command Queue
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_hs_rach_check_rach_trigger(uint8 rlc_id);
#endif  // *FEATURE_WCDMA_TRIGGER_BASED_RACH *//

/*===========================================================================

FUNCTION     l1m_state_is_fach

DESCRIPTION
  This function is called to check whether the UE is in L1M_FACH state

DEPENDENCIES
  None

RETURN VALUE
  TRUE if l1m_state is L1M_FACH and FALSE otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean l1m_state_is_fach(void);

/*===========================================================================

FUNCTION RLC_ENH_UL_PDU_PENDING

DESCRIPTION
 This function is to check whether there is any new CTRL PDU pending for trasmission
 or not. 

DEPENDENCIES
  None.

RETURN VALUE
  boolean - If there is any new CTRL PDU pending for transmission- will return TRUE.
             Else return FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean rlc_enh_ul_pdu_pending (rlci_ul_am_ctrl_type    *am_ctrl_ptr);

#ifdef FEATURE_MAC_I

/*===========================================================================
FUNCTION: RLC_UL_UPDATE_MAC_I_CTRL_DSM_ITEM

DESCRIPTION:
  API to free the memory allocated for partial PDU's.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void rlc_ul_update_mac_i_ctrl_dsm_item (rlc_ctrl_pdu_type *   mac_i_ctrl_dsm);


/*===========================================================================
FUNCTION: RLC_UL_FREE_PREV_TTI_MAC_I_CTRL_DSM_ITEM

DESCRIPTION:
  API to release the DSM memory for the UM/TM data that are transmitted in 
  previous TTI.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void rlc_ul_free_prev_tti_mac_i_ctrl_dsm_item (void);

/*===========================================================================

FUNCTION RLC_UL_GET_MAC_I_RLC_ID_CTRL_DSM_PTR

DESCRIPTION

  Get one DSM item from the MAC-i control PDU DSM pool, 
  which is assigned for the requested rlc_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ctrl_pdu_type* rlc_ul_get_mac_i_rlc_id_ctrl_dsm_ptr(rlc_lc_id_type rlc_id);
/*===========================================================================
FUNCTION: RLCI_ENH_CALC_UL_LI_LENGTH

DESCRIPTION:
 API used to calculate LI length for AM entity based upon RRC configuration.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
uint8 rlci_enh_calc_ul_li_length(rlc_ul_am_config_type *ul_cnfg_var_ptr, uint8 old_li_length);

/*===========================================================================

FUNCTION RLC_UL_GET_MAC_I_CTRL_DSM_POOL

DESCRIPTION

  Get one DSM item from the MAC-i control PDU DSM pool.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_ctrl_pdu_type* rlc_ul_alloc_mac_i_ctrl_dsm_pool(void);

/*===========================================================================

FUNCTION RLC_UL_ENH_PRINT_SRC_DESC

DESCRIPTION

  This prints the updated IOVEC information.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_print_src_desc(l1_l2_data_type  *l1_ul_tx_buf_ptr);

/*===========================================================================

FUNCTION RLC_UL_ENH_STORE_MAC_I_CTRL_PDU

DESCRIPTION

 This API stores the information related to AM control PDU in case of partial PDU 
 transmission.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_enh_store_mac_i_ctrl_pdu(mac_i_seg_status mac_segment_needed, rlci_ul_ctrl_blk_type *ctrl_blk_ptr, uint16 pdu_size_bytes, uint8 *pdu_ptr);

/*===========================================================================

FUNCTION RLC_UL_UPD_SDU_LEN_Q

DESCRIPTION

  This API updates the RLC SDU length queue based upon the data queued in UL WM.
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
uint32 rlc_ul_upd_sdu_len_q(rlci_ul_ctrl_blk_type *ctl_blk_ptr);

void rlc_ul_am_re_enq_sdu_temp_q(rlci_ul_ctrl_blk_type   *ctrl_blk_ptr, 
                                 rlci_ul_am_ctrl_type    *am_ctrl_ptr);
#endif /* FEATURE_MAC_I */

#define RLC_GET_DSM_LENGTH(sdu_ptr)           dsm_length_packet(sdu_ptr)
#define RLC_SET_DSM_LENGTH(sdu_ptr, pkt_len)

/*===========================================================================
FUNCTION: RLC_UL_FREE_PREV_TTI_SDU_DSM_ITEM

DESCRIPTION:
  API to release the DSM memory for the UM/TM data that are transmitted in 
  previous TTI.

DEPENDENCIES:
  None

RETURN VALUE:
  NONE

SIDE EFFECTS:
  None
===========================================================================*/ 
void rlc_ul_free_prev_tti_sdu_dsm_item (rlc_sdu_q_type * sdu_free_q_ptr, uint32  max_free_sdu_buff);


/*===========================================================================

FUNCTION  RLCI_TM_PROCESS_DISCARD_TIMER_CB

DESCRIPTION
  This callback function is called when the SDU discard timer expired.
  This function scan the WM to see whether there are SDUs needed to be 
  removed since they stay in the WM for too long. Also, it put the SDU
  discard time stamp on each new incoming SDU.
  
  The SDU discard time stamp is put in the 18th bit of the app_field, 
  since the lower 16 bits are used as the bit count of the DSM packet 
  chain.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_tm_process_discard_timer_cb(rlc_lc_id_type rlc_id);

#ifdef FEATURE_WCDMA_UL_COMPR

void rlc_ul_am_requeue_dequeued_sdus_to_wm (rlci_ul_ctrl_blk_type *ul_ctrl_blk_ptr );
void rlc_ul_am_enq_orig_sdu_to_wm(rlci_ul_ctrl_blk_type *ctl_blk_ptr);
void rlc_ul_am_comp_init_var(rlci_ul_am_ctrl_type  *am_ctl_blk_ptr);
rlc_ctrl_pdu_type *rlc_dl_ptp_func_mgmt_response(rlci_ptp_sufi_type             ret_response,
                                                 rlci_ptp_sufi_rsp_reason_type  ul_comp_rsp_reason);
void rlc_dl_ul_comp_config_current_dump(rlci_ul_am_ctrl_type  *ul_am_ctrl_ptr);
boolean rlc_config_deconfig_ul_compressor(rlci_ul_am_ctrl_type  *am_ctrl_blk_ptr, boolean compression_enable);
void rlc_ul_comp_disable_config_internal(rlci_ul_ctrl_blk_type *ctl_blk_ptr);
boolean rlc_ul_am_comp_resp_pending(rlc_lc_id_type ul_data_id);

extern boolean rlc_ptp_ul_comp_resource_crunch;
extern ul_comp_perf_metrics_type ul_comp_per_metrics_info;

#endif                                                

#endif

