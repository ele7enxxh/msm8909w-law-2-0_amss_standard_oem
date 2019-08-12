/*!
  @file
  lte_mac_rrc.h

  @brief
  This file contains all the common define structures/function prototypes 
  interface between MAC and RRC.


*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_mac_rrc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/07/16   al      CR942705: FR33651 Fast SR enhancement to counter no scheduling 
                   post CDRX in ZTE drops.
03/06/12   mm      Added change to pass TDD UL\DL config to MAC
07/18/11   mm      Added interface changes for eMBMS
07/13/11   bn      Added in MAC support for statistics
04/21/10   bn      Added in start/stop request structures
04/20/10   bn      Added in support for START/STOP MAC dynamic mem allocation
01/18/10   bn      Renamed bsr timer max to bsr timer infinity
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
08/06/09   mm      Added mesgPowerOffsetGroupB values for March spec migration
06/03/09   bn      Added in comment for connection cancellation
12/18/08   dineshd Added struct for CancelConnReq (lte_mac_cancel_conn_req_msg_s) 
05/15/08   bn      Initial version  
===========================================================================*/

#ifndef LTE_MAC_RRC_H
#define LTE_MAC_RRC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_cphy_msg.h>
/* Start of L2 internal including */
#include "lte_l2_comdef.h"
#include "lte_mac.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined PBR infinity value
*/
#define LTE_MAC_PBR_INFINITY          0xFFFF

/*! @brief defined BSR timer infinity value
*/
#define LTE_MAC_BSR_TIMER_INFINITY    0xFFFF

/*! @brief defined TA infinity value
*/
#define LTE_MAC_TA_INFINITY           LTE_OTA_IE_INFINITY_UINT16

/*! @brief defined bit mask to indicate DL configuration is present
*/
#define LTE_MAC_DL_CFG_BM       0x01

/*! @brief defined bit mask to indicate RACH configuration is present
*/
#define LTE_MAC_RACH_CFG_BM     0x02

/*! @brief defined bit mask to indicate UL configuration is present
*/
#define LTE_MAC_UL_CFG_BM       0x04

/*! @brief defined bit mask to indicate LC Info configuration is present
*/
#define LTE_MAC_LC_INFO_CFG_BM  0x08

/*! @brief defined bit mask to indicate C-RNTI configuration is present
*/
#define LTE_MAC_C_RNTI_CFG_BM   0x10

/*! @brief defined bit mask to indicate eMBMS configuration is present
*/
#define LTE_MAC_EMBMS_CFG_BM    0x20

/*! @brief defined bit mask to indicate CDRX length configuration is present
*/
#define LTE_MAC_CDRX_LENGTH_CFG_BM   0x40

/*! @brief defined bit mask to indicate all bitmasks are present
*/
#define LTE_MAC_ALL_CFG_BM                    \
               (LTE_MAC_DL_CFG_BM         |   \
                LTE_MAC_RACH_CFG_BM       |   \
                LTE_MAC_UL_CFG_BM         |   \
                LTE_MAC_LC_INFO_CFG_BM    |   \
                LTE_MAC_C_RNTI_CFG_BM     |   \
                LTE_MAC_EMBMS_CFG_BM      |   \
                LTE_MAC_CDRX_LENGTH_CFG_BM)


/*! @brief defined total number of RA Preamble Index
*/
#define LTE_MAC_MAX_RA_PREAMBLE_INDEX   64

/*! @brief defined size of the connection request SDU = 6 bytes
*/
#define LTE_MAC_MAX_CONN_REQ_SDU_SIZE   6

/*! @brief message power offset group B enumeration type
*/
typedef enum
{
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_INFINITY,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_DB0,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB5,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB8,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB10,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB12,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB15,
  LTE_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB18
} lte_mac_msg_pwr_offset_group_b_e;


/*! @brief TDD UL\DL Config enum; This is same as 
    TDD subframe assignment config received in SIB1
    or in RadioResourceConfigCommon during Mobility
    IMPORTANT: Do not change the enum value, since it
    has to match the TDD UL\DL enum values of
    lte_l1_tdd_ul_dl_cfg_index_e in ML1 interface
*/
typedef enum
{
  LTE_MAC_TDD_UL_DL_CFG_0,
  LTE_MAC_TDD_UL_DL_CFG_1,
  LTE_MAC_TDD_UL_DL_CFG_2,
  LTE_MAC_TDD_UL_DL_CFG_3,
  LTE_MAC_TDD_UL_DL_CFG_4,
  LTE_MAC_TDD_UL_DL_CFG_5,
  LTE_MAC_TDD_UL_DL_CFG_6,
  LTE_MAC_TDD_UL_DL_CFG_NONE, /* Used for FDD */
  LTE_MAC_TDD_UL_DL_CFG_MAX   /* Not Used, Invalid value */

} lte_mac_tdd_ul_dl_cfg_e;

/*! @brief RACH configuration parameters structure. This contains both
RACH parameters for MAC and L1
*/
typedef struct
{
  int16   preamble_initial_power;/*!< Initial Power Level */
  uint8   power_ramping_step;   /*!< Step power */
  /*! ra_index1 = sizeOfRA-PreamblesGroupA. GroupA is from 1 to ra_index1*/
  uint8   ra_index1;
  /*! ra_index2 = numberOfRA-Preambles. GroupB = (ra_index1 + 1) to
  ra_index2. Note that in case there is not GroupB, then 
  ra_index1 = ra_index2 */
  uint8   ra_index2;       
  uint16  preamble_trans_max;          /*!< MAX preamble attempts */
  uint16  contention_resolution_timer; /*!< Contention timer in ms */
  uint16  message_size_qroup_a;        /*!< size in bytes */

   /* start: these following fields will be removed */
  boolean partition_pathloss_threshold_present; 
  uint16  partition_pathloss_threshold; 
  /* end: of remove */

  /*! group B power offset */
  lte_mac_msg_pwr_offset_group_b_e pwr_offset_group_b;
  /*! Pmax to be used in the target cell. If absent the UE applies 
  the maximum power according to the UE capability */
  int16     p_max; 
  int16     delta_preamble_msg3;         /*!< delta preable message 3 */
  lte_cphy_prach_setup_param_s  l1_rach; /*!< L1 RACH configuration */
} lte_mac_rach_cfg_s;

/*! @brief QoS Logical channel configuration parameters structure
*/
typedef struct
{
  /*!< Priority bite rate in byte per TTI or Kbytes/s*/
  uint16              pbr;    
  /*! Priority Level    */
  uint8               priority;
  /*!< indicates lc group info present*/
  boolean             lcgroup_present;
  /*! LC group*/
  uint8               lcgroup; 
  /*! token bucket max size in bytes = token_bucket_duration * pbr*/
  uint32              token_bucket_size; 
  /*! SR-Mask, Configured if set to TRUE, should be released if set to FALSE */
  boolean             sr_mask_configured;

} lte_mac_lcqos_cfg_s;

/*! @brief LC info configuration parameters structure
*/
typedef struct
{
  /*! number of LC to be deleted */
  uint8                   num_deleted_lc;   
  uint8                   deleted_lc_list[LTE_MAX_LC];
  /*! number of added or modified logical channel*/
  uint8                   num_addmod_lc;       
  uint8                   addmod_lc_list[LTE_MAX_LC];
  /*! base pointer to LC info database of LTE_MAX_LC */
  lte_mac_lcqos_cfg_s*    lc_list; 
} lte_mac_lc_info_cfg_s;

/*! @brief Up Link configuration parameters structure
*/
typedef struct
{
  boolean  sr_cfg_included;     /*!< indicated of SR cfg is included or not*/
  boolean  sr_resource_present; /*!< indicated if SR recourse is available or not*/
  uint16   sr_periodicity;      /*!< Periodicity of triggering SR in ms*/
  boolean  bsr_cfg_included;    /*!< BSR configuration is included */
  uint16   bsr_timer;           /*!< BSR periodicity timer in ms */
  boolean  sps_release_cfg_included;/*!< SPS configuration is included */
  uint16   sps_num_tx_release;      /*!< Number of empty txed before release timer SPS
                                         0 value means that SPS is released */
  boolean  retxed_bsr_cfg_included; /*!< retxed BSR configuration is included */
  uint16   retxed_bsr_timer;        /*!< retxed BSR timer in ms */
  boolean  tdd_ul_dl_cfg_included; /*!< TDD UL\DL config is included; while on TDD, this field
                                        is TRUE when SIB1 contains the TDD UL\DL config;
                                        when moving from TDD to FDD, this is set to TRUE and 
                                        tdd_ul_dl_cfg is set to LTE_MAC_TDD_UL_DL_CFG_NONE */
  lte_mac_tdd_ul_dl_cfg_e  tdd_ul_dl_cfg;  /*!< TDD UL\DL Cfg */
  boolean extended_bsr_size_cfg_included; /*!< extendedBSR-Sizes If value TRUE is configured, the BSR index indicates 
              extended BSR size levels as defined in TS 36.321 [6, Table 6.1.3.1-2].*/
} lte_mac_ul_cfg_s;
//source/qcom/qct/modem/lte/api/main/latest/lte_mac_rrc.h
/*! @brief Down Link configuration parameters structure
*/
typedef struct
{
  boolean ta_timer_cfg_included;  /*!< flag to indicate if TA timer value is present*/
  uint16  ta_timer;               /*!< TA timer value */
} lte_mac_dl_cfg_s;

/*! @brief eMBMS LC info structure
*/
typedef struct
{
  lte_mbsfn_area_id_t  area_id;  /*!< Area id */
  lte_pmch_id_t        pmch_id;  /*!< PMCH id */
  lte_lc_id_t          lc_id;    /*!< Logical Channel id */
} lte_embms_lc_info_s;

/*! @brief eMBMS configuration parameters structure
*/
typedef struct
{
  /*! number of active eMBMS LCs */
  uint8                    num_active_lc;  

  /*! List of active eMBMS LCs*/
  lte_embms_lc_info_s      active_lc_list[LTE_MAX_EMBMS_LC];

} lte_mac_embms_cfg_s;

/*! @brief RRC-MAC configuration parameters structure. Depend of the 
    cfg_type, The specific action will be taken by MAC as following:
  - LTE_L2_CFG_RELEASE : MAC will delete all the existing
    configuration by itself and go back to idle state.
  - LTE_L2_CFG_HO: Go to HO state and depend on the cfg_bitmask,
    MAC should take in the corresponding new configurations.
  - LTE_L2_CFG_RADIO_FAILURE: Go to radio failure state and depend on the 
    cfg_bitmask, MAC should take in the corresponding new configurations.
  - LTE_L2_CFG_RECFG : No state change and depend on the 
    cfg_bitmask, MAC should take in the corresponding new configurations.
  - LTE_L2_CFG_EMBMS : MAC will process the MSI based on the LC info sent by RRC
  - LTE_L2_CFG_RELEASE_EMBMS : MAC will delete all existing multicast LCs
  - LTE_L2_CFG_RELEASE_ALL : MAC will delete all the existing unicast and 
    multicast configuration
  Note that: RRC can still indicate MAC to delete a given configure by
  setting the corresponding bitmask to 1 and set the pointer to NULL.
*/
typedef struct
{
  /*! type of configuration */
  lte_l2_cfg_reason_e     cfg_type; 
  /*! bit masks to indicate which configurations are present*/
  uint8  cfg_bitmask;
  /*! cfg_bitmask will be applied to the following pointers */
  /*! LC Info configuration structure */
  lte_mac_lc_info_cfg_s   lc_info_cfg;
  /*! RACH configuration structure */
  lte_mac_rach_cfg_s*     rach_cfg;
  /*! DL configuration structure */
  lte_mac_dl_cfg_s*       dl_cfg;
  /*! UL configuration structure */
  lte_mac_ul_cfg_s*       ul_cfg;
  /*! new CRNTI configured */
  uint16                  crnti_cfg;
  /*! eMBMS configuration */
  lte_mac_embms_cfg_s     embms_cfg;
  /*! CDRX cycle length field(unit per sub-frame) */
  uint32                  cdrx_length;
} lte_mac_cfg_s;

/*! @brief RRC-MAC Access Request structure
*/
typedef struct
{
  lte_mac_access_reason_e  access_reason; /*!< Access reason */
  uint8    raid;            /*!< RA index */
  uint8    raid_mask;       /*!< Explicitly signalled PRACH resource PRACH Mask Index */ 
  uint64   ueid;            /*!< this is the TMSI */
  uint16   msg_length;      /*!< length of RRC MSG3 */
  uint8    *msg_ptr;        /*!< pointer to MSG3 */
} lte_mac_access_request_s;

/*-----------------------------------------------------------------------*/
/*     LTE MAC/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/

/*! @brief Access Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_mac_access_request_s            access_info; /*!< access info */
} lte_mac_access_req_msg_s;

/*! @brief Access Confirmation from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;            /*!< message header */
  /* Message payload */
  boolean                  crnti_included; /*!< if C-RNTI is present */
  uint16                   crnti;          /*!< C-RNTI value */
} lte_mac_access_cnf_msg_s;

/*! @brief configuration Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_mac_cfg_s                       cfg_data;
} lte_mac_cfg_req_msg_s;

/*! @brief configuration confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */
  errno_enum_type                     status; /*!< status of configuration */
} lte_mac_cfg_cnf_msg_s;

/*! @brief Access Abort Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */
  
} lte_mac_access_abort_req_msg_s;

/*! @brief Cancel Conn Request from RRC to MAC. Note that MAC
    must keep all the current configurations.
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */
} lte_mac_cancel_conn_req_msg_s;


/*! @brief Access Abort confirmation from MAC to RRC  
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of Abort */
} lte_mac_access_abort_cnf_msg_s;

/*! @brief MAC start request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */

} lte_mac_start_req_msg_s;

/*! @brief MAC stop request from RRC  to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */

} lte_mac_stop_req_msg_s;

/*! @brief MAC start confirmation from MAC to RRC  
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of mac start req */
} lte_mac_start_cnf_msg_s;

/*! @brief MAC stop confirmation from MAC to RRC  
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of mac stop req */
} lte_mac_stop_cnf_msg_s;

/*! @brief MAC RACH report request from RRC to MAC
*/
typedef struct 
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */  
  /*!Indicates if RACH RPT REQ is for failed RACH (T300 expiry) 
    or last successful RACH
    if TRUE, REQ is for failed RACH
    else REQ is for last successful RACH*/
  boolean failed_rach_rpt_is_req;
} lte_mac_rach_rpt_req_msg_s;

/*! @brief MAC RACH report confirmation from MAC to RRC  
*/
typedef struct 
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
  uint8 num_preambles_sent; /*!< Preambles sent in last successfull RACH */  
  uint8 num_contention_detected; /*!< Contentions detected in last successfull RACH */
} lte_mac_rach_rpt_cnf_msg_s;

#endif /* LTE_MAC_RRC_H */
