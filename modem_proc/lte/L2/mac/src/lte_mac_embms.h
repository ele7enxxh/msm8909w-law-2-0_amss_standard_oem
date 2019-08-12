/*!
  @file
  lte_mac_embms.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE MAC eMBMS.


*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/01/15   mg      CR785052:Give up priority - Early termination - Enhanced
                   estimated loss calculation - Disable MSI
02/12/13   mg      CR 613938: eMBMS SS API crash 
12/20/13   mg      CR 592094: FR 19078: SS eMBMS API Requirement - L2 MAC 
07/24/13   st      CR 457620: Filter out the duplicate MSIs to ML1
08/21/12   st      CR 387720: Fix caching MSI logic across a cell selection/RLF
07/17/12   st      CR 373453,371140: LCID range & number of LCIDs in MSI fix
06/04/12   st      CR 361039: MAC saves the MSI even though RRC does not 
                   configure MAC yet for eMBMS
04/20/12   st      CR 351466,333829: Area id > 8 and MSI ordering fix
01/04/12   st      Updated interface changes with ML1 for the invalid stop 
                   subordinal subfn macro
12/09/11   st      Added the STOP_ORD_SUBFN_UNSCHED_LCID macro
12/08/11   st      Added MSI endianness fix and made LTE_CPHY_MSI_REQ static
12/07/11   st      Added eMBMS specific statistics 
11/20/11   st      Updated as part of a bug fix for inaccurate LCID release
10/06/11   st      Initial version  
===========================================================================*/

#ifndef LTE_MAC_EMBMS_H
#define LTE_MAC_EMBMS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte_l2_comdef.h>
#include <lte_mac_rrc.h>
#include <lte_as.h>

/* Start of MAC internal include list */
#include "lte_mac_int_msg.h"
#include "lte_mac_dl.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief To extract LCID field that is the least significant 5 bits of the
 * two byte msi_record in the MSI OTA       
 */
#define LTE_MAC_EMBMS_GET_LCID_FIELD(msi) (((msi) & 0xF800) >> 11)

/*! @brief To extract the ordinal stop frame number field that is the 
 *  most significant 11 bits of the two byte msi_record in the MSI OTA       
 */
#define LTE_MAC_EMBMS_GET_STOP_SUBFN_FIELD(msi) (msi & 0x7FF)

/*! @brief Macro to set the bitmask indicating MSIs corresponding to which PMCHs 
 *  are sent to ML1
 */
#define LTE_MAC_EMBMS_SET_PMCH_BM(bm, pmch)  (bm = (bm | (1 << pmch)))

/*! @brief Macro to reset the bitmask indicating MSIs corresponding to which PMCHs 
 *  are sent to ML1
 */
#define LTE_MAC_EMBMS_RESET_PMCH_BM(bm, pmch)  (bm = (bm & (~(1 << pmch))))

/*! @brief Macro to read the bitmask indicating MSIs corresponding to which PMCHs 
 *  are sent to ML1
 */
#define LTE_MAC_EMBMS_READ_PMCH_BM(bm, pmch)  ((bm & (1 << pmch)) >> pmch)

/*! @brief The max value that the area id can take */
#define LTE_MAC_EMBMS_MAX_AREA_ID_VAL             255

/*! @brief Max number of MSBSFN areas */
#define LTE_MAC_EMBMS_MAX_AREA_ID_NUM             8

/*! @brief Maximum value of PMCH id is 16; max number of PMCHs is 15 */
#define LTE_MAC_EMBMS_MAX_PMCH_ID_NUM             16

/*! @brief There are 32 LCIDs from 0 to 31. LCIDs 29, 30, 31 are for reserved, 
 *  MSI and padding respectively. Hence, the data is carried over LCIDs
 *  0 to 28. This macro is used to define the size of the RRC configuration and
 *  MSI data structures which are meant for the use of ONLY the data carrying
 *  LCIDs i.e. 0 to 28
 */
#define LTE_MAC_EMBMS_MAX_LCID_NUM                29

/*! @brief Invalid MSI database index */
#define LTE_MAC_EMBMS_INVALID_MSI_DB_IDX          0xFF

/*! @brief Invalid LCID value */
#define LTE_MAC_EMBMS_INVALID_LCID_VAL            LTE_L2_DLSCH_LCID_INVALID

/*! @brief PMCH ID of MSI when ML1 is not configured yet but receives MSI on DL */
#define LTE_MAC_EMBMS_ML1_NO_CFG_PMCH             0

/*! @brief Default value of the disable MSI flag*/	 	 
#define LTE_MAC_EMBMS_DEFAULT_DISABLE_MSI         0

/*! @brief Clipboard for storing MSIs of which area + pmch combos need to be 
    sent on the reception of RRC config */
typedef struct
{
  boolean valid
    [LTE_MAC_EMBMS_MAX_AREA_ID_VAL + 1][LTE_MAC_EMBMS_MAX_PMCH_ID_NUM];
} lte_mac_embms_msi_combo_tracker_s;

/*! @brief Information about a given LCID needed at MAC */
typedef struct
{
  uint8   lcid;
  uint16  stop_ord_subfn;
}lte_mac_lcid_info_s;

/*! @brief Type of internal MSI database entries */
typedef struct
{
  uint8          msi_cnt;
  uint8          num_mtchs;
  lte_l1_subfn_t subfn;         /*! Subfn of the MSI OTA element */
  uint16         area;  /* This is needed to map back to the mapping array from
                           the database */
  lte_mac_lcid_info_s   lcid_info[LTE_MAC_EMBMS_MAX_LCID_NUM];
  lte_l1_sfn_t   sysfn;         /*! SFN of the MSI OTA element */
} lte_mac_embms_monitor_s;

/*! @brief Type for the internal area id range maps for RRC configuration */
typedef struct
{
  /* This variable is used to keep track of the allotment of the internal
   * indices into the RRC config DB to the area ids. Since only 8 areas are
   * supported, this variable should never be more than 7
   */
  uint8 rrc_db_idx;

  /* This array has a mapping of the area id to the internal RRC config
   * database idx
   */
  uint8 area_to_rrc_db_idx[LTE_MAC_EMBMS_MAX_AREA_ID_VAL + 1];

  /* This variable is used to keep track of the allotment of the internal
   * indices into the MSI DB to the area ids. Since only 8 areas are
   * supported, this variable should never be more than 7 (for indices 0 to 7)
   */
  uint8 msi_db_idx;

  /* This array has the mapping of the area id to the internal MSI
   * database index
   */
  uint8 area_to_msi_db_idx[LTE_MAC_EMBMS_MAX_AREA_ID_VAL + 1];
} lte_mac_embms_area_range_map_s;

/*! @brief Type of internal copy of the MSI */
typedef struct
{
  /* Internal MSI copy's storage 2D array */
  lte_mac_embms_monitor_s  
    info_arr[LTE_MAC_EMBMS_MAX_AREA_ID_NUM][LTE_MAC_EMBMS_MAX_PMCH_ID_NUM];
} lte_mac_embms_msi_cfg_s;

/*! @brief Type of internal copy of the RRC configuration */
typedef struct
{
  /* Size of first dimension is one more than the area id num in order to
   * store the lookup dummies
   */
  boolean db[LTE_MAC_EMBMS_MAX_AREA_ID_NUM + 1][LTE_MAC_EMBMS_MAX_PMCH_ID_NUM]
    [LTE_MAC_EMBMS_MAX_LCID_NUM];
  mtch_info_s *embms_ptr[LTE_MAC_EMBMS_MAX_AREA_ID_NUM + 1][LTE_MAC_EMBMS_MAX_PMCH_ID_NUM]
    [LTE_MAC_EMBMS_MAX_LCID_NUM];
} lte_mac_embms_rrc_cfg_s;

/*! @brief Global data structure of LTE_MAC_EMBMS */
typedef struct
{
  lte_mac_embms_msi_cfg_s   msi_cfg;  /* MSI config element */
  /* Internal RRC config copy storage 3D array */
  lte_mac_embms_rrc_cfg_s   rrc_cfg;
  lte_mac_embms_msi_combo_tracker_s *msi_combo_tracker_ptr;
  boolean                   rrc_cfg_present;   /* Flag to indicate the
                                                  presence of RRC config */
  lte_mac_dl_data_s         *dl_data_ptr;
  lte_cphy_msi_req_s        cphy_msi_req;
  lte_mac_embms_area_range_map_s  map;
  uint32                    num_msi_before_rrc_cfg;
  boolean                   uniq_area_pmch_combo;
  /*Stores the MTCH TB bytes for each active embms channel*/
  lte_mac_active_embms_stats_s *active_embms_stats_ptr; 
  /*!< Lock to maintain consistency during lte_mac_embms_state_reset() 
  called from lte_mac_ctrl_config_int_action_process() and lte_mac_embms_config_req_handler()
  */
  pthread_mutex_t       state_reset_lock; 
  boolean                   disable_msi;/*Flag to indicate disabling sending the 
  MSI to ML1*/
} lte_mac_embms_s;


/*===========================================================================

  FUNCTION:  lte_mac_embms_state_reset

===========================================================================*/
/*!
  @brief
  This function initializes the internal state the very first time the DL
  task comes up
 
  @detail
  
 
  @return
  none

  @note
  

  @see lte_mac_dl_process_mac_pdu_embms()

*/
/*=========================================================================*/
extern void lte_mac_embms_state_reset
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_process_data

===========================================================================*/
/*!
  @brief
  This function processes an eMBMS data packet and passes it onto RLC.
 
  @detail
  An eMBMS packet is stamped with data that uniquely identifies it,
  basically the trio of area id, pmch id and lcid, and enqueues this
  packet into the MAC RLC eMBMS exclusive circular buffer. It then
  sends a message to RLC indicating that data is available.
 
  @return
  none

  @note
  There is no filtering of packets being sent to upper layers on the DL

  @see lte_mac_dl_process_mac_pdu_embms()

*/
/*=========================================================================*/
extern void lte_mac_embms_process_data
(
  lte_mac_dl_pdu_sub_payload_s* current_sub_payload,
  a2_dl_phy_tb_info_t* tb_meta_info,
  lte_mac_dl_s* lte_mac_dl
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_process_msi

===========================================================================*/
/*!
  @brief
  This function processes MSI obtained OTA and stores it internally.
 
  @detail
  MSI is MTCH schedule information that MAC receives OTA. Each time
  this is received OTA, MAC needs to send it to ML1. Not all the
  MTCHs identified in the MSI are relevant though; only those that are
  asked to be monitored as per the RRC configuration. This function
  parses the MSI and stores it internally. Additionally, if the RRC
  configuration is present already, the ML1 message is triggered.
 
  The MSI information is assumed to be per area id in a given PMCH
  scheduling interval. This means that in a given PMCH scheduling
  period, we do not see variation in the area_id and hence we do not
  index MSI information per area id.
 
  @return
  none

  @note

  @see  

*/
/*=========================================================================*/
extern void lte_mac_embms_process_msi
(
  lte_mac_dl_pdu_sub_payload_s* current_sub_payload,
  a2_dl_phy_tb_info_t* tb_meta_info
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_config_req_handler

===========================================================================*/
/*!
  @brief
  This function is called by lte_mac_dl to handle an incoming RRC
  configuration request for eMBMS. 
 
  @detail

 
  @return
  none

  @note

  @see
  lte_mac_dl_cb_handler()

*/
/*=========================================================================*/
extern void lte_mac_embms_config_req_handler
(
  lte_mac_embms_cfg_s *embms_cfg,
  lte_l2_cfg_reason_e cfg_type
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_free_dynamic_mem

===========================================================================*/
/*!
  @brief
  This function is called to free the dynamic memory allocated to 
  lte_mac_embms.active_embms_stats_ptr 
 
  @detail

 
  @return
  none

  @note

  @see

*/
/*=========================================================================*/
extern void lte_mac_embms_free_dynamic_mem
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_create_mutexes

===========================================================================*/
/*!
  @brief
  This function is called to create all the mutexes used with in the mac embms context 
 
  @detail

 
  @return
  none

  @note

  @see

*/
/*=========================================================================*/
extern void lte_mac_embms_create_mutexes
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_destroy_mutexes

===========================================================================*/
/*!
  @brief
  This function is called to destroy all the mutexes used with in the mac embms context 
 
  @detail

 
  @return
  none

  @note

  @see

*/
/*=========================================================================*/
extern void lte_mac_embms_destroy_mutexes
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_embms_set_disable_msi_flag

===========================================================================*/
/*!
  @brief
  This function sets the disable msi flag through diag interface
 
  @detail
  
 
  @return
  none

  @note
  

  @see lte_mac_dl_diag_cb_handle()

*/
/*=========================================================================*/
extern void lte_mac_embms_set_disable_msi_flag
(
  uint32 cmd_data_2
);

#endif /* LTE_MAC_EMBMS_H */
