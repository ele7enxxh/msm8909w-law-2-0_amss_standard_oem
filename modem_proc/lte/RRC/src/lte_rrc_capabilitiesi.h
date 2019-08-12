/*!
  @file
  lte_rrc_capabilitiesi.h

  @brief
  Internal header file of RRC Capabilities containing private data.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_capabilitiesi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/12   kp      Added EFS support to disable FGI-25 related optimization
02/22/12   kp      Added EFS support to disable e_CSFB_1XRTT
11/01/11   kp      Moved code for comparing jtag-id's from HW to compute UE cat 
                   to lte\common, use common API to get UE cat.
10/25/11   kp      Added code to indicate UE cat as 2 by comparing 
                   jtag-id's from HW.
10/01/11   kp      Added code to include e_RedirectionUTRA_r9 in a Rel-8 UE 
                   when camped on PLMN 450-08.
07/28/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CAPABILITIESI_H
#define LTE_RRC_CAPABILITIESI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_rrc_ext_msg.h"
#include "sys.h"  
#include <lte_rrc_utils.h>
#include <lte_irat_types.h>
#include <lte_pdcp_ext_api.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! Maximum number of RATs that can be reported in the Information message*/
#define LTE_RRC_CAPABILITIES_MAX_NUM_REPORTED_RATS 5

#define LTE_RRC_CAP_MAX_NUM_CDMA_BANDS 18
#define LTE_RRC_CAP_MAX_NUM_UTRA_FDD_BANDS 16
#define LTE_RRC_CAP_MAX_NUM_GERAN_BANDS 11
#define LTE_RRC_CAP_MAX_NUM_TDSCDMA_TDD128_BANDS 6


#define LTE_RRC_CAP_PLMN1_MCC_DIGIT_1 4
#define LTE_RRC_CAP_PLMN1_MCC_DIGIT_2 5
#define LTE_RRC_CAP_PLMN1_MCC_DIGIT_3 0
#define LTE_RRC_CAP_PLMN1_NUM_MNC_DIGITS 2
#define LTE_RRC_CAP_PLMN1_MNC_DIGIT_1 0
#define LTE_RRC_CAP_PLMN1_MNC_DIGIT_2 5


#define LTE_RRC_CAP_PLMN2_MCC_DIGIT_1 4
#define LTE_RRC_CAP_PLMN2_MCC_DIGIT_2 5
#define LTE_RRC_CAP_PLMN2_MCC_DIGIT_3 0
#define LTE_RRC_CAP_PLMN2_NUM_MNC_DIGITS 2
#define LTE_RRC_CAP_PLMN2_MNC_DIGIT_1 0
#define LTE_RRC_CAP_PLMN2_MNC_DIGIT_2 8
#ifdef FEATURE_LTE_ELS_ENABLED
#define LTE_RRC_CAP_TAG_MAX          15
#define LTE_RRC_CAP_TAG_MIN           0
#endif
/*CA bandwidth class*/
#define LTE_RRC_CAP_CA_NUM_BANDWIDTH_CLASS_MAX 0x06

#define LTE_RRC_DISABLE_SRVCC_BITMASK 0xFF7FFFDF /*Set bits 9, 27 to 0*/
#define LTE_RRC_ENABLE_SRVCC_BITMASK  0x00800020 /*Set bits 9, 27 to 1*/

/*! @brief Enumeration of RATs that can be requested for
*/
typedef enum
{
  LTE_RRC_CAPABILITIES_RAT_EUTRA, /*!< 0 */
  LTE_RRC_CAPABILITIES_RAT_GERAN_CS, /*!< 1 */
  LTE_RRC_CAPABILITIES_RAT_GERAN_PS, /*!< 2 */
  LTE_RRC_CAPABILITIES_RAT_UTRA_FDD, /*!< 3 */
  LTE_RRC_CAPABILITIES_RAT_1X, /*!< 4 */
  LTE_RRC_CAPABILITIES_RAT_UTRA_TDD /*!< 5 */
} lte_rrc_cap_rats_e;


/*! @brief Dynamic Data type for Capabilities SM
*/
typedef struct
{
  uint8 mu_id; /*!< MU ID of Capability Information ULM */

  /*! array of reported RATs */
  lte_rrc_cap_rats_e rep_rats[LTE_RRC_CAPABILITIES_MAX_NUM_REPORTED_RATS];
  uint8 num_rep_rats; /*!< Number of reported RATs */
  uint8 num_outstanding_rats;
  /*!< Number of outstanding RATs from which we are expecting a capabilities response */
  uint32 hrpd_1x_cap_length;/*!< The length of 1x/HRPD capabilities string in bytes*/
  uint8 *hrpd_1x_cap_container_ptr;/*!< Ptr to the 1x HRPD Capabilities string*/

  uint32 w_cap_length;/*!< The length of W Capabilities string in bytes*/
  uint8 *w_cap_container_ptr;/*!< Ptr to the W(UTRA) Capabilities string*/

  uint32 tds_cap_length;/*!< The length of TDS Capabilities string in bytes*/
  uint8 *tds_cap_container_ptr;/*!< Ptr to the TDS(UTRA) Capabilities string*/

  uint32 g_cs_cap_length;/*!< The length of GERAN CS Capabilities string in bytes*/
  uint8 *g_cs_cap_container_ptr;/*!< Ptr to the GERAN CS Capabilities string*/

  uint32 g_ps_cap_length;/*!< The length of GERAN PS Capabilities string in bytes*/
  uint8 *g_ps_cap_container_ptr;/*!< Ptr to the GERAN PS Capabilities string*/

  uint32 trans_id; /*!< Transaction ID of the Capabilities Enquiry received*/

  uint32 num_lte_bands;
  uint32 lte_bands[LTE_BAND_NUM];/*!< EUTRA bands*/

  uint32 num_1x_bands;
  lte_rrc_osys_BandclassCDMA2000 cdma_1x_bands[LTE_RRC_CAP_MAX_NUM_CDMA_BANDS]; 
  /*!< CDMA bands*/
  uint32 num_do_bands;
  lte_rrc_osys_BandclassCDMA2000 cdma_do_bands[LTE_RRC_CAP_MAX_NUM_CDMA_BANDS]; 
  /*!< CDMA bands*/

  uint32 num_utra_fdd_bands;
  lte_rrc_osys_SupportedBandUTRA_FDD utra_fdd_bands[LTE_RRC_CAP_MAX_NUM_UTRA_FDD_BANDS]; 
  /*!< UTRA bands*/
  
  uint32 num_tdscdma_tdd128_bands;
  lte_rrc_osys_SupportedBandUTRA_TDD128 tds_cdma_tdd128_bands[LTE_RRC_CAP_MAX_NUM_TDSCDMA_TDD128_BANDS]; 

  uint32 num_geran_bands;
  lte_rrc_osys_SupportedBandGERAN geran_bands[LTE_RRC_CAP_MAX_NUM_GERAN_BANDS] ;
  /*!< GERAN bands*/
  /* Pointer to store Cap container address */
  lte_rrc_osys_UE_EUTRA_Capability *cap_container;
  #ifdef FEATURE_LTE_ELS_ENABLED
  /*!< ELS capabity ULM requested
            Bit 0 (LSB) corresponds to whether Std Capability was requested
            Bit 1 corresponds to whether ELS Capability was requested*/
  uint8 send_els_cap_ulm:2;
  #endif
} lte_rrc_cap_dyn_data_s;


/*Structure to hold CA bandwidth combos per CC*/
typedef struct
{ 
  uint8 band; /* Secondary cell */
  uint8 bw_mask;
  uint8 bw_class_mask;
  boolean ul_supported; /* Tell if UL is supported on this band or not */
}lte_rrc_cap_ca_cc_per_combo_s;

/*Structure to hold CA bandwidth combos */
typedef struct
{  
  uint8 num_cc_per_combo; /* Number of CC per band combo */
  /* Supported Band/BW for gievn pcell+scell combo*/
  lte_rrc_cap_ca_cc_per_combo_s *cc_per_combo_ptr;
} lte_rrc_cap_ca_combo_per_band_s;

/*Sttructure to hold CA band/bandwidth combos */
typedef struct
{ 

  /* Tells number of band combos supported for this pcell*/
  uint8 num_combos;
  uint8 mac_cc_in_db;     /* Max number of CC supportedo */  
  /* This entry is populated bansed on the number of bits set in above mask*/
  lte_rrc_cap_ca_combo_per_band_s *combo_per_band_ptr;

} lte_rrc_cap_ca_band_combos_s;

typedef struct
{
   /*! Indicates if DTM is enabled*/
   boolean               geran_dtm_is_supported;

   boolean               geran_dtm_is_valid;
} lte_rrc_cap_dtm_info_s;

typedef struct lte_rrc_cap_band_combo_node
{
  /* Pointer to combo from lte_rrc_cap_ca_band_combos_s */
  lte_rrc_cap_ca_combo_per_band_s *bc_ptr;
  /* Pointer to next BC node */
  struct lte_rrc_cap_band_combo_node *next_bc_ptr;
}lte_rrc_cap_band_combo_node_s;

typedef struct
{
  /* Band combo map */
  lte_rrc_cap_band_combo_node_s *bc_node;
    /* Band combo map */
  sys_lte_band_mask_e_type ca_bc_2dl;

} lte_rrc_cap_band_info_s;

#ifdef FEATURE_LTE_ELS_ENABLED
typedef struct
{
  /*!< UDC Capability*/
  lte_pdcpul_udc_cap_s udc_cap;
} lte_rrc_cap_els_info_s;
#endif

/*! @brief Static Data type for Capabilities SM
*/
typedef struct
{
  uint32 efs_fgi; /*!< FGI bits*/
  uint32 efs_fgi_r9; /*!< FGI bits*/
  uint32 efs_fgi_tdd; /*!< TDD FGI bits*/
  uint32 efs_fgi_tdd_r9; /*!< TDD FGI bits*/
  boolean fgi_is_set_via_efs; /*!< Is FGI set via EFS file */
  boolean fdd_cap_r11_is_set_via_efs; /*! Is R11 FDD CAP set via EFS file */
  boolean tdd_cap_r11_is_set_via_efs; /*! Is R11 TDD CAP set via EFS file */
  boolean fgi_r9_is_set_via_efs; /*!< Is Rel9 FGI set via EFS file */
  boolean tdd_fgi_is_set_via_efs; /*!< Is FGI set via EFS file */
  boolean tdd_fgi_r9_is_set_via_efs; /*!< Is Rel9 TDD FGI set via EFS file */
  uint32 fgi_mask;/*!< Mask for FGI defaults*/
  uint32 efs_fdd_cap_r11; /*! Rel 11 FDD cap bits*/
  uint32 efs_tdd_cap_r11; /*! Rel 11 TDD cap bits*/
  uint32 fgi_r9_mask;/*!< Mask for FGI defaults*/
  uint32 r11_cap_mask; /*! Mask for R11 CAP bits defaults */
  uint32 efs_fgi_r10; /*!< FGI bits*/
  uint32 efs_fgi_tdd_r10; /*!< TDD FGI bits*/
  boolean fgi_r10_is_set_via_efs; /*!< Is FGI set via EFS file */
  boolean tdd_fgi_r10_is_set_via_efs; /*!< Is Rel10 TDD FGI set via EFS file */
  uint32 fgi_r10_mask;/*!< Mask for FGI defaults*/

  uint8 dual_rx_1x_csfb;/*!< Dual Rx support for 1x CSFB*/
  lte_irat_capabilities_s irat_capabilities;/*IRAT capabilities*/
  /*this will store UE cat*/
  uint8 lte_rrc_ue_cat;
  /*when value>1 diable e_CSFB_1XRTT, set through EFS*/
  uint8 e_CSFB_1XRTT_disable;

  /*when value>1 enable FDD/TDD capabilities/FGI to be signalled differently, set through EFS*/
  uint8 diff_fdd_tdd_fgi_enable;

  /*when value=1 enable FDD only intra frequency si acquition for handover,
       when value=2 enable TDD only intra frequency si acquition for handover,
       when value=3 enable FDD & TDD intra frequency si acquition for handover, set through EFS*/
  uint8 intra_freq_si_acq_for_ho_r9_enable;

  /*when value=1 enable FDD only inter frequency si acquition for handover,
       when value=2 enable TDD only inter frequency si acquition for handover,
       when value=3 enable FDD & TDD inter frequency si acquition for handover, set through EFS*/
  uint8 inter_freq_si_acq_for_ho_r9_enable;

  /*when value=1 enable FDD only utran si acquition for handover,
       when value=2 enable TDD only utran si acquition for handover,
       when value=3 enable FDD & TDD utran si acquition for handover, set through EFS*/
  uint8 utran_si_acq_for_ho_r9_enable;

  /*TO disable r9 redirection for Geran and UTRA*/
  uint8 lte_rrc_r9_redirection_disable;

  /* TO turn on L2G SRVCC over SGLTE */
  uint8 srvcc_over_sglte_support;

/*when value>1 enable Rel-10 CA feature, set through EFS*/
  uint8 rel10_ca_enabled;

/*rel-10 CA band combination list EFS NV will containing valid band combination 
*/
  lte_rrc_cap_ca_band_combos_s *ca_band_combo_ptr;
  sys_lte_band_mask_e_type last_used_ca_band_pref;
  
  uint8 is_bandwidth_combo_check_daisabled;
  uint8 is_intra_band_ca_validations_daisabled;
/*Boolean to track the window for FGI change, its from 
  1. SIM  invalid to 1st scapability req from network and 
  2. LTE deact(LPM) to  1st capability req from network*/
  boolean can_fgi_change_onthe_fly;
  /*enable Rel-10 MDT feature, set through EFS*/
  uint8 rel10_mdt_disabled;
  /*Disable Cap IEs*/
  uint64 disable_ies_efs;
  /*Enable Cap IEs*/
  uint64 enable_ies_efs;  
  /*enable non ca band support in UE cap, set through EFS*/
  uint8 non_ca_band_support_enabled;
  /*Disable TxAntennaSelection*/
  uint8 disable_tx_antenna_selection; 
  /* PDU to stored pre-built UL cap container */
  lte_rrc_pdu_buf_s prebuilt_pdu_buf;
  /* Mode on which last Capabity is requested */
  lte_rrc_div_duplex_e last_cap_built_mode;
  lte_irat_capabilities_s last_irat_capabilities;/*IRAT capabilities*/	
  /* Max CC supported... */
  uint8 max_cc_supported;
  /*Disable RACH report */
  uint8 disable_rach_rpt;       
/*! Indicates if DTM is enabled*/
  lte_rrc_cap_dtm_info_s dtm_info;
  /*Control of CA band combos via EFS*/  
  lte_rrc_cap_ca_rf_band_combos_s ca_bands_via_efs;
  #ifdef FEATURE_LTE_ELS_ENABLED
  /*!< ELS capability tag (0 to 15)*/
  int els_capability_tag;
  /*!< Standard capability tag (0 to 15)*/
  uint8 std_capability_tag;
  /*!< ELS Capability Info*/
  lte_rrc_cap_els_info_s els_cap_info;
  /*!< ELS capability bit mask requested in CAP Enquiry DLM*/
  uint16 els_cap_mask;
  #endif
} lte_rrc_cap_static_data_s;

/*! @brief Private data type for CAP
*/

typedef struct
{
  lte_rrc_cap_static_data_s *sd_ptr;/*!< Static data pointer*/ 
  lte_rrc_cap_dyn_data_s *dd_ptr; /*!< Dynamic data pointer */
} lte_rrc_cap_s;

extern void lte_rrc_cap_read_fgi_r10_efs_file(int8 sub_id);
extern void lte_rrc_cap_read_fgi_r10_tdd_efs_file(int8 sub_id);

#endif /* LTE_RRC_CAPABILITIESI_H */



