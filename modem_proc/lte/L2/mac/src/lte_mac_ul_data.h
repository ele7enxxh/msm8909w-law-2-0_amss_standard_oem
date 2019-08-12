/*!
  @file
  lte_mac_ul_data.h

  @brief
  This header file contains the variou defines, enums, and other data
  structure types, function APIs to process the up link data path


*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly 
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_ul_data.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/07/15   su      CR808724: Limit the number of built mac SDU under 200
03/26/15   el      CR813111: Fix side effect of the CR 805765
03/10/15   el      CR805765: Force second pass during packet build
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
09/04/14   el      CR 697620: Replace Mac Padding to PN31 for GPS 
09/03/14   el      CR687377: Move periodic logic from CTRL to UL task
08/03/14   mg      CR702710: QSH analysis and reset
07/07/14   el      CR 688341: SPS fix for no SR sending after reconnection
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
06/14/13   st      CR 367915: ePHR enhancement
03/01/13   st      CR 458002: Removed field to track no QoS_fb instances
12/18/12   st      CR 372149: Fixed a compiler warning
09/24/12   st      CR 367941: Support for extendedBSR sizes
04/03/12   bn      Enhance MAC stat and add logic to support for SPS release
12/16/11   ax      feature clean up FEATURE_LTE_MAC_FC_UL
10/07/11   bn      Replaced double buffer by circular queue for QoS feedback
05/20/11   bn      Moved LTE_MAC_UL_DATA_NUM_SW_HARQ to l2_common.h file
05/05/11   bn      Changed to support Nikel HW HARQ buffer reduction
09/02/11   st      Added a new statistics structure for global counters
07/13/11   bn      Added in MAC support for statistics
11/18/10   bn      Force padding when UL FC enters zero BSR state
10/01/10   bn      Removed the obsoleted define LTE_MAC_UL_DATA_SDU_MIN_SIZE
08/02/10   bn      Forced BSR when detecting zero FC buffer size
07/15/10   bn      added in error code for no A2 reserved task
04/15/10   bn      Added logic to fix the order of building MAC headers 
04/06/10   bn      Defined LTE_MAC_UL_DATA_SDU_MIN_SIZE
03/31/10   bn      Added in support MAC FC for Up Link
01/19/10   bn      Defined a macro for max MAC SDU fill header 
11/06/09   bn      Moved MAC UL MAC header size define to lte_l2_common.h
08/24/09   bn      Added in support for using previous QoS Prep Info
08/11/09   bn      Added in supports Mar09 Spec. CR 091655
07/30/09   bn      Supported store local copy of MSG3 for UL/DL RACH
05/29/09   bn      Make sure on include the CRNTI MCE in MSG3 RACH HO case
07/08/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_UL_DATA_H
#define LTE_MAC_UL_DATA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <timetick.h>
#include "lte_mac_ul.h"
#include "lte_mac_qos.h"
#include "lte_mac_log.h"
#include "qsh.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined maximum length of MCE payloads in bytes*/
#define LTE_MAC_UL_DATA_MCE_MAX_LENTH               32

/*! @brief defined long BSR size = 4 bytes*/
#define LTE_MAC_UL_DATA_LONG_BSR_MCE_SIZE           4

/*! @brief defined RESERVED FILL SDU header max size = 2 bytes*/
#define LTE_MAC_UL_RESERVED_FILL_SDU_HDR_MAX_SIZE   2

/*! @brief defined short BSR size = 4 bytes*/
#define LTE_MAC_UL_DATA_SHORT_BSR_MCE_SIZE          2

/*! @brief defined CRNTI MCE size = 3 bytes*/
#define LTE_MAC_UL_DATA_CRNTI_MCE_SIZE              3

/*! @brief defined POWER headroom MCE size = 2 bytes */
#define LTE_MAC_UL_DATA_POWER_HEADROOM_MCE_SIZE     2

/*! @brief defined POWER headroom MCE header size = 1 bytes */
#define LTE_MAC_UL_DATA_PHR_HDR_SIZE                1

/*! @brief defined ePHR MCE header size = 2 bytes */
#define LTE_MAC_UL_DATA_EPHR_HDR_SIZE               2

/*! @brief defined maximum BSR level index*/
#define LTE_MAC_UL_DATA_BSR_LEVEL_MAX_INDEX         64

/*! @brief defined short BSR MCE header */
#define LTE_MAC_UL_DATA_SHORT_BSR_MCE_HEADER        0x3D

/*! @brief defined long BSR MCE header */
#define LTE_MAC_UL_DATA_LONG_BSR_MCE_HEADER         0x3E

/*! @brief defined truncated BSR MCE header */
#define LTE_MAC_UL_DATA_TRUNCATED_BSR_MCE_HEADER    0x3C 

/*! @brief defined CRNTI MCE header */
#define LTE_MAC_UL_DATA_CRNTI_MCE_HEADER            0x3B

/*! @brief defined Power Headroom MCE header */
#define LTE_MAC_UL_DATA_POWER_HEADROOM_MCE_HEADER   0x3A

/*! @brief defined Power Headroom MCE header */
#define LTE_MAC_UL_DATA_E_PHR_MCE_HEADER            0x39

/*! @brief defined FILL SDU HEADER header */
#define LTE_MAC_UL_DATA_FILL_SDU_HEADER             0x3F 

/*! @brief defined PADDING SDU HEADER header */
#define LTE_MAC_UL_DATA_PADDING_SDU_HEADER          0x1F    

/*! @brief defined MAX number of system frame number multiply subframe number */
#define LTE_MAC_UL_DATA_SFN_SUBFN_MODULUS           10240

/*! @brief defined MAX GPS Padding buffer length in bytes */
#define LTE_MAC_UL_DATA_GPS_PADDING_BUFF_LEN        256  
  
/*! @brief defined MAX number of GPS padding buffers */
#define LTE_MAC_UL_DATA_NUM_GPS_PADDING_BUFF_MAX    2    

/*! @brief defined MAX number of GPS padding IOV 6250/256 */
#define LTE_MAC_UL_DATA_NUM_GPS_PADDING_IOV        40    

/*! @brief MAC UL data internal statistics structure */
typedef struct 
{
  uint32 num_lcg_bsr[LTE_MAC_BSR_LCGROUP_MAX]; /*Number of times BSR is sent for LCG 0 to 3*/
  uint32 total_bsr_sent_lcg[LTE_MAC_BSR_LCGROUP_MAX]; /*Total accummulated BSR value for LCG 0 to 3*/
} lte_mac_ul_data_bsr_stats_s;

/*! @brief MAC UL data internal statistics structure */
typedef struct 
{
  /*! Counter to indicate number of times grant size is altered 
  during RACH */
  uint32 grant_size_change_rach_cnt;
  uint32 no_qos_prep_cnt;
  uint32 no_grant_for_crnti_cnt;
  uint32 no_a2_reserved_task_cnt;
  uint32 total_pkt_build_ind_cnt; /*Number of new MAC PDUs built*/
  uint32 total_retx_pkt_build_ind_cnt; /*Number of retx MAC PDUs built*/
  uint64 total_grant_received_bytes; /*Total grant received in bytes*/
  uint64 total_padding_bytes; /*Total padding sent in bytes including the header*/
  lte_mac_ul_data_bsr_stats_s bsr_stats; /*Stats used to calculcate avg BSR*/
  uint32 no_phr_sent; /*Number of PHR sent on the uplink*/
  uint32 total_phr_sent; /*Accumulated value of the PHR sent so far*/
  uint32 final_harq_retx_attempt_cnt[LTE_MAC_UL_DATA_NUM_SW_HARQ];
  uint32 gps_padding_error_cnt;
} lte_mac_ul_data_stats_s;

/*! @brief SPS related data structure 
*/
typedef struct
{
  /*!< number of emptied MAC SDU to implicit SPS release  */
  uint16  sps_num_tx_release;
  /*!< current counter of SPS release */
  uint16  sps_num_tx_release_cnt;
  /*!< sps enable / disable flag */
  boolean is_sps_enabled;
} lte_mac_ul_data_sps_s;

/*! @brief Global structure for MAC UL Data processing
*/
typedef struct
{
  /*! pointer points to the QoS preprocessing double buffer */
  lte_mac_common_dbuf_s  *qos_prep_dbuff_ptr;
  /*! pointer points to the QoS feedback circular queue */
  lte_circ_q_s  *qos_feedback_circq_ptr;
  /*! current leftover grant*/
  uint16 leftover_grant;
  /*! indicate the last sent BSR instance number*/
  uint16  sent_bsr_instance_number;
  /*!< current BSR triggered */
  lte_mac_qos_bsr_triggered_e bsr_triggered;
  /*! estimated BSR size in including the header: 2 or 4 bytes */
  uint8   bsr_size;
  /*! buffer to hold the MCE payload */
  uint8   mce_buf[LTE_MAC_UL_DATA_MCE_MAX_LENTH];
  /*! index to the next avaible space of mce_buf */
  uint32   mce_buf_index;
  /*! index to the next avaible space of buf */
  uint32   buf_index;
  /* index into the buf to locate the BSR header */
  uint32   bsr_header_index;
  /*! pointer to buffer to hold the MAC PDU header and MCE payload */
  uint8   *buf_ptr;
  /*! head pointer of buffer to use for storing MAC PDU header and MCE payload */
  uint8   *head_buf_ptr;
  /*! reserved bytes for fill SDU headers */
  uint8   reserved_fill_sdu_hdr;
  /*! last built LC ID */
  lte_lc_id_t  last_built_lcid;
  /*! last SDU header size */
  uint8        last_sdu_header_size;
  /*! last SDU size */
  uint16       last_sdu_size;
  /*! LC packet build info from RLC  */
  lte_mac_ul_data_info_s  lc_pkt_info[LTE_MAX_LC];
  /*! total of RLC PDU built per LC in first pass */
  uint8  fp_num_rlc_sdu[LTE_MAX_LC];
  /*! Flag to indicate of the second pass PDU is performed */
  boolean build_second_pass;
  /*! QoS feedback pointer */
  lte_mac_qos_feedback_info_s  *qos_fb_ptr;
  /*! Pointer to global MAC Log structure */
  lte_mac_log_s*      log;
  /*! current SFN, this is just pass on to RLC */
  lte_sfn_s           current_sfn;
  /*! BSR Event */
  lte_mac_qos_bsr_event_e  bsr_event; 
  /*! last bsr sent sys frame for periodical bsr handling in mac ul task */
  uint16          last_bsr_sent_sys_fn;
  /*! last bsr sent sub frame for periodical bsr handling in mac ul task */
  uint8           last_bsr_sent_sub_fn;
  /*! BSR periodicity timer config in ms */
  uint16          bsr_timer_cfg;
  /*! BSR periodicity timer in ms */
  uint16          bsr_timer_cur_use_cfg;
  /*! last none zero LCG*/
  lte_mac_bsr_lcgroup_e    last_none_zero_lcg;
  /*! current QoS Prep Info using in packet build*/
  lte_mac_qos_prep_info_s  curr_qos_prep_info;
  /*! flag to indicate if UL need to force a BSR */
  boolean                  forced_bsr;
  /*! current HARQ ID */
  uint8 harq_id;
  /*! total used size for UL Flow Control*/
  uint32  fc_total_used_size; 
  /*! indicate the UL FC BSR is zero now so 
 all the packet should be padding now */
  boolean ul_fc_zero_bsr_enable;
  /*! last sent power headroom */
  uint8 sent_power_headroom;
  /*! Statistics structure */
  lte_mac_ul_data_stats_s stats;
  /*! BSR level lookup table */
  const uint32 *bsr_lookup_table;
  /*! total number of sdu built per TB */
  uint16 total_built_sdu_num_per_tb;
  /*! Store the EFS configured overwritten BSR index */
  uint8         overwt_bsr_index;
  /*! SPS related data */
  lte_mac_ul_data_sps_s sps_data;
  /*! linear Feedback Shif register to generate GPS padding data */
  uint32 gps_padding_lfsr;
  /*! GPS padding buffers in 64 words or 256 bytes each */
  uint32 gps_pad_buf[LTE_MAC_UL_DATA_NUM_GPS_PADDING_BUFF_MAX][LTE_MAC_UL_DATA_GPS_PADDING_BUFF_LEN/4];
  /*! number of GPS padding IOV */
  uint32 num_gps_pad_iov;
  /*! GPS Padding IOV table */
  a2_iovec_t gps_pad_iov[2 * LTE_MAC_UL_DATA_NUM_GPS_PADDING_IOV];
  /*! current index in the GPS Padding buffer*/
  uint8 gps_pad_buf_index[LTE_MAC_UL_DATA_NUM_GPS_PADDING_BUFF_MAX];
  /*! Record the last use buf index for gps padding */
  uint8 last_use_buf_index;
  /*! flag to indicate whether gps padding is required or not */
  boolean padding_for_gps_is_required;
  /*! Time at which QSH reset is called*/
  timetick_type  last_reset; 
} lte_mac_ul_data_int_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_mac_ul_data_init

===========================================================================*/
/*!
    @brief
    This function initializes the UL Data Path variables during power up.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_init
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_main_handler

===========================================================================*/
/*!
    @brief
    This function is the main handler of the UL Data Path Processing.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_main_handler
(
  lte_mac_ul_msg_u* msg  /*!< input message pointer */ 
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_build_mac_pdu

===========================================================================*/
/*!
    @brief
    In Connected state, upon receiving LTE_CPHY_UL_PKT_BUILD_IND
    from L1, This function will be called to build UL MAC PDU. 

    @detail
    QoS functionalitis should run in MAC Control Task in order to
    support packet build process.

    @return
    None.  

    @see related_function()
    lte_mac_ul_data_pkt_build

*/
/*=========================================================================*/
extern void lte_mac_ul_data_build_mac_pdu
(
  lte_cphy_ul_pkt_build_ind_s  *ind_ptr  /*!< packet build indication ptr*/
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_build_msg3_ho_ul_dl_rach

===========================================================================*/
/*!
    @brief
    This function builds MSG3 for HO, UL/DL data arrival RACH cases. 
    Upon receiving LTE_CPHY_UL_PKT_BUILD_INDIt will make sure all the grant 
    will be used for C-RNTI MCE,BSR MCE and then for all the active LC. For 
    HO, it is assumed that LCID 1 will be the first entry in the active LCID
    list. The whole MSG3 will be build only for the first time, then copy 
    into the local copy so that it will be used later for subsequence
    transimission of MSG3

    @detail
    QoS functionalitis should run in MAC Control Task in order to
    support MSG3 HO RACH process.

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_build_msg3_ho_ul_dl_rach
(
  lte_cphy_ul_pkt_build_ind_s  *ind_ptr,     /*!< packet build indication ptr */
  boolean                      *dup_msg3,    /*!< need to duplicate MSG3 */
  uint8                        *dup_msg3_ptr,/*!< ptr store msg3 */
  uint16                       *msg_size,    /*!< msg3 size  */
  uint16                        crnti_mce    /*!< C-RNTI to build C-RNTI MCE*/
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_reset

===========================================================================*/
/*!
    @brief
    This function resets the UL Data Path variables during HO, connection
    release or radio failure

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_reset
(
  lte_mac_inform_action_e   inform_action  
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_deinit

===========================================================================*/
/*!
    @brief
    This function de-initializes the UL Data Path variables during shutdown.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_deinit
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_get_access_ptr

===========================================================================*/
/*!
    @brief
    This function return a pointer to the data structure of lte_mac_ul_data
    module.

    @detail

    @return
    None.  

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_ul_data_int_s* lte_mac_ul_data_get_access_ptr
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_bs_to_bsr_level_index

===========================================================================*/
/*!
    @brief
    This function converts buffer size to BSR level index

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern uint8 lte_mac_ul_data_bs_to_bsr_level_index
(
  uint32  buffer_size /*!< buffer size in byte*/
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_set_bsr_lookup_table

===========================================================================*/
/*!
    @brief
    This function sets the appropriate BSR size look up table 

    @detail

    @return
    None

    @see lte_mac_qos_ul_config_process()

*/
/*=========================================================================*/
extern void lte_mac_ul_data_set_bsr_lookup_table
(
  boolean extendedBSRSizes
);
/*===========================================================================

  FUNCTION:  lte_mac_ul_data_sps_deactive_cnf_handler

===========================================================================*/
/*!
    @brief
    This function set the sps related data when receivd sps deactive cnf from Ml1

    @detail
dir
    @return
    None

    @see 

*/
/*=========================================================================*/
extern void lte_mac_ul_data_sps_deactive_cnf_handler
(
  void
);
/*===========================================================================

  FUNCTION:  lte_mac_ul_data_get_sps_enabled_flag

===========================================================================*/
/*!
    @brief
    This function gets is_sps_enabled flag

    @detail

    @return
    None

    @see

*/
/*=========================================================================*/
extern boolean lte_mac_ul_data_get_sps_enabled_flag
(
  void
);
/*===========================================================================

  FUNCTION:  lte_mac_ul_data_set_ul_cfg

===========================================================================*/
/*!
    @brief
    This function sets ul cfg

    @detail

    @return
    None

    @see

*/
/*=========================================================================*/
extern void lte_mac_ul_data_set_ul_cfg
(
  lte_mac_ul_cfg_s *ul_cfg,  /*!< ul_cfg to config parameterS in UL data */
  uint8 ul_cfg_bitmask   /*!< bitmask to indicate the config type */
);

/*==============================================================================

  FUNCTION:  lte_mac_ul_data_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC UL DATA QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC UL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_ul_data_qsh_analysis_routine 
(
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_ul_data_qsh_reset_stats

==============================================================================*/
/*!
    @brief
    This API resets MAC UL DATA stats

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC UL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_ul_data_qsh_reset_stats
(
  void
);

#endif /* LTE_MAC_UL_DATA_H */
