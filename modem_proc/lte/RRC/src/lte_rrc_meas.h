/*!
  @file lte_rrc_meas.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_meas.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem
                   and sd_ptr for usage with statically allocated mem.
02/10/10   sureshs Added GSM defines for dedicated priorities
03/25/09   ask     Added code for connected mode EUTRA measurements
03/19/09   sk      Added support for rxLevMin and pMax for inter and intra 
                   freq meas
02/23/09   sk      Initial Version
===========================================================================*/

#ifndef LTE_RRC_MEAS_H
#define LTE_RRC_MEAS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_rrc_osys_asn1.h"
#include "lte_rrc_osys_asn1util.h"
#include <__lte_rrc_meas.h>
#include "lte_rrc_int_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*!< No. of bits in a byte */
#define LTE_RRC_BITS_IN_BYTE 8
/*!< Max Bitmap size of GERAN */  
#define LTE_RRC_ASN_GERAN_MAX_ARFCN_BITMAP_SIZE 16
/*!< Max number of ARFCN */
#define LTE_RRC_MAX_ARFCN 1024

#define LTE_RRC_ASN_MIN_PCI                 0   /*<! Minimum value of cell id */
#define LTE_RRC_ASN_MAX_PCI                 503 /*<! Maximum value of cell id */


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern stm_state_t lte_rrc_meas_get_state
(
  void
);

lte_errno_e lte_rrc_meas_validate_connected_config
(
  /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_rrc_osys_RRCConnectionReconfiguration *dlm_ptr,

  /*! TRUE if Reconfig DLM also includes MobilityControlInformation IE */
  boolean is_part_of_handover
);

boolean lte_rrc_meas_cdma_system_time_is_valid
(
  lte_rrc_osys_SystemInformationBlockType8 *sib8_ptr,  /*!< ptr to SIB8 */
  lte_cphy_irat_cdma_sib8_system_time_info_s *system_time_info_ptr /*!< Ptr to CDMA system time info */
);

#ifdef FEATURE_LTE_REL11
extern void lte_rrc_meas_update_loc_cfg
(
   lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *rrc_conn_rcfg_ptr
);

extern boolean lte_rrc_meas_obtain_loc_info_is_configured 
(
 void
);
#endif

uint64 lte_rrc_meas_get_dedicated_priority_info
(
  /*! Pointer to pointer to dedicated priority list */
  const lte_irat_dedicated_priority_list_s **freq_list_ptr_ptr
);

boolean lte_rrc_meas_start_loc_ser_is_sent
(
   void
);

sys_sband_e_type lte_rrc_meas_get_gsm_band_from_arfcn
(
  lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr, /*!< Ptr to GSM info */
  uint16 arfcn  /*!< GSM ARFCN */
);

lte_errno_e lte_rrc_meas_read_cdma_neigh_cell_list
(
  /*! Ptr to SIB8 */
  lte_rrc_osys_SystemInformationBlockType8  *sib8_ptr, 
  /*!< Ptr to CDMA neighbor cell info */
  lte_cphy_meas_irat_cdma_neigh_cell_info_s *neigh_cell_info_ptr, 
  /* Ptr to number of elements in neigh cell band channel info list */
  uint8 *num_neigh_cell_info_list_size,
  /*!< boolean to indicate if passed in params are for ehrpd */
  boolean ehrpd 
);

void lte_rrc_meas_dd_init
(
  void
);

void lte_rrc_meas_dd_deinit
(
  void
);

void lte_rrc_meas_autonomous_meas_id_removal
(
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs* conn_recfg_ptr,  // reconfig msg ptr
  boolean is_serving_freq_valid,//serving_freq will be valid only if  is_serving_freq_valid is TRUE
  lte_earfcn_t serving_freq //serving_freq(Pcell) 
);

extern sys_lte_band_mask_e_type lte_rrc_meas_get_lte_neigh_band_mask
(
  void
);

extern boolean lte_rrc_meas_earfcn_in_inter_freq_neighbors
(
  lte_earfcn_t freq
);

boolean lte_rrc_meas_resel_prio_lower_than_w_neighbors
(
  lte_l1_resel_priority_t   resel_priority,
  lte_cphy_meas_irat_wcdma_resel_info_s  *wcdma_resel_info
);

boolean lte_rrc_meas_serv_prio_lower_than_wcdma_neighbors
(
  lte_cphy_idle_meas_cfg_req_s  *idle_meas_cfg_req
);

extern lte_rrc_lte_ota_depri_info_s  * lte_rrc_meas_get_ota_depri_info
(
  void
);

extern boolean lte_rrc_meas_earfcn_is_deprioritized
(
  lte_earfcn_t freq,
  lte_rrc_depri_cause_e cause,
  lte_rrc_lte_depri_info_s *depri_info_ptr
);

extern lte_errno_e lte_rrc_meas_dlm_validate_meas_cycle_scell
(
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *conn_recfg_ptr /*!< reconfig msg ptr*/
);

extern lte_errno_e lte_rrc_meas_get_eutra_meas_obj_param
(
  lte_earfcn_t ref_earfcn,
  lte_cphy_meas_obj_eutra_s *eutra_meas_obj_ptr
);

extern boolean lte_rrc_meas_get_intra_freq_meas_subframe_pattern
(
  lte_cphy_meas_subfm_patter_neigh_s* meas_subfm_pattern_intra_neigh_ptr
);

extern boolean lte_rrc_meas_lte_rat_deprioritized
(
  void
);

extern lte_rrc_plmn_list_s* lte_rrc_meas_get_depri_plmn_list
(
  void
);

extern boolean lte_rrc_meas_auto_gap_is_in_progress
(
  void
);

#endif /* LTE_RRC_MEAS_H */

