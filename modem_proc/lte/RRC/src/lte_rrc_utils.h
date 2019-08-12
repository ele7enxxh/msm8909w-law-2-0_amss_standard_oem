/*!
  @file lte_rrc_utils.h

  @brief
  Miscellaneous utilities.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_utils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/12   kp      Moved definition of lte_rrc_get_3gpp_release_version to lte_rrc_ext_api.h
06/08/11   vatsac  Eliminate band scan on bands with overlapping freq ranges
04/21/11   sk      Moved lte_rrc_get_tdd_prune_required to external api header file
03/30/11   sureshs Updated TDD max earfcn
09/03/10   mm      Added fns to convert between RRC PLMN and Sys PLMN formats
06/23/2010 sureshs Added functions to convert B12 to B17 earfcns
05/14/10   mm      Changed lte_rrc_revoke_trigger_e to include Meas events
05/04/2010 amit    Added lte_rrc_revoke_trigger_e 
04/15/2010 sureshs Added revoke utility
04/14/2010 sureshs Added include of internal message file
03/23/10   mm      Added prototype for fn that determines if a cell is TDD or FDD
02/26/10   mm      Added prototype for lte rrc_get_wcdma_band_from_uarfcn
09/18/2009 sureshs Prototype change for lte_rrc_get_band_from_dl_earfcn
05/04/2009 vatsac  Added lte_rrc_get_sys_band_class()
04/03/2009 amit    Added lte_rrc_band_from_enum_to_int()
02/05/2009 sureshs Added UMID field to timer struct for event logging
01/19/2009 sureshs Removed invalid access class macro
12/04/2008 sureshs Added routine to get band from earfcn
11/26/2008 sureshs Added access class 11 and 15 macros
09/11/2008 sureshs Changed doxygen comment
07/22/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_UTILS_H
#define LTE_RRC_UTILS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr_umid.h>
#include <msgr_types.h>
#include <sys.h>
#include <lte_as.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_ext_api.h>
#include <lte_rrc_int_msg.h>
#include <geran_grr_api.h>
#include <stringl.h>
#include "rfm_lte_earfcn.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH  
#include <mcfg_refresh.h>
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RRC helper macro to suppress compiler and/or Lint warnings for
           unused variables.
*/
#define LTE_RRC_UNUSED(var)             ( var = var )

/*! @brief Enumeration for RRC timer mode
*/
typedef enum
{
  LTE_RRC_TMR_MODE_ONESHOT,
  LTE_RRC_TMR_MODE_PERIODIC
} lte_rrc_tmr_mode_e;

/*! @brief Macro for access class 11
*/
#define LTE_RRC_ACCESS_CLASS_11 11

/*! @brief Macro for access class 12
*/
#define LTE_RRC_ACCESS_CLASS_12 12

/*! @brief Macro for access class 13
*/
#define LTE_RRC_ACCESS_CLASS_13 13

/*! @brief Macro for access class 14
*/
#define LTE_RRC_ACCESS_CLASS_14 14
/*! @brief Macro for access class 15
*/
#define LTE_RRC_ACCESS_CLASS_15 15
/*! @brief Macro for access class 10
*/
#define LTE_RRC_ACCESS_CLASS_10 10

/*! @brief Macros for maximum FDD DL EARFN as per 36.101-b40
*/
#define LTE_RRC_MAX_FDD_DL_EARFN 9869

/*! @brief Macros for minimum TDD DL EARFN as per 36.101
*/
#define LTE_RRC_MIN_TDD_DL_EARFN 36000
/*! @brief Macros for maximum TDD DL EARFN as per 36.101
*/
#define LTE_RRC_MAX_TDD_DL_EARFN 45589

/*! @brief Macros for INVALID PLMN ID DIGIT
*/
#define LTE_RRC_INVALID_MCC_MNC_DIGIT 0xF 

/*! @brief default value for QXRLEVMIN is -118*/
#define LTE_RRC_Q_RXLEVMIN_CUSTOM_DEFAULT -118

/* Debug trace id value used for ML1 signal strength req for CMAPI */
#define LTE_RRC_CMAPI_CPHY_DEBUG_TRACE_ID -2

#define LTE_RRC_MAX_1_BYTE_VALUE 0xFF

#define LTE_RRC_MAX_2_BYTE_VALUE 0xFFFF

#define LTE_RRC_MAX_5_BIT_VALUE 0x1F

/* MAX bands number get from EFS/RF for band range limitation */
#define LTE_RRC_MAX_BANDS_TO_UPDATE      3

/*! @brief Enumeration for LTE RRC release version
*/
typedef enum
{
  LTE_RRC_REL_1_0,
  LTE_RRC_REL_1_5,
  LTE_RRC_REL_2_0,
  LTE_RRC_REL_3_0,
  LTE_RRC_REL_4_0,
  LTE_RRC_REL_5_0
} lte_rrc_release_e;
#ifdef FEATURE_LTE_ELS_ENABLED
/*! @brief Enumeration for field that indicates when to send ELS indication
*/
typedef enum
{
  LTE_RRC_SEND_ELS_IND_NONE,
  LTE_RRC_SEND_ELS_IND_CONN_SETUP_COMPLETE,
  LTE_RRC_SEND_ELS_IND_SM_COMPLETE,
  LTE_RRC_SEND_ELS_IND_RECFG_COMPLETE,
  LTE_RRC_SEND_ELS_IND_MAX
} lte_rrc_send_els_ind_e;
#endif

#define LTE_RRC_SUBFRAME_PATTERN_FDD_NUMBITS 40                         /*<! Subframe Pattern for FDD in Bits */
#define LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_1_5_NUMBITS 20     /*<! Subframe Pattern for TDD with Subframe Config 1-5 in Bits */
#define LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_0_NUMBITS 70       /*<! Subframe Pattern for TDD with Subframe Config 0 in Bits */
#define LTE_RRC_SUBFRAME_PATTERN_TDD_SUBFRAME_CONFIG_6_NUMBITS 60       /*<! Subframe Pattern for TDD with Subframe Config 6 in Bits */

#define LTE_BW_NRB_6_MASK 0x1  /*<! BW NRB6 Mask */
#define LTE_BW_NRB_15_MASK 0x2  /*<! BW NRB15 Mask */
#define LTE_BW_NRB_25_MASK 0x4  /*<! BW NRB25 Mask */
#define LTE_BW_NRB_50_MASK 0x8  /*<! BW NRB50 Mask */
#define LTE_BW_NRB_75_MASK 0x10  /*<! BW NRB75 Mask */
#define LTE_BW_NRB_100_MASK 0x20  /*<! BW NRB100 Mask */


#define LTE_RRC_BAND_UNDEFINED 0
#define LTE_RRC_BAND_INVALID_VALUE 0

#define LTE_RRC_ASCII_VALUE_SEMICOLON 0x3B
#define LTE_RRC_ASCII_VALUE_HYPHEN 0x2D
#define LTE_RRC_ASCII_VALUE_0 0x30
#define LTE_RRC_ASCII_VALUE_9 0x39
#define LTE_RRC_ASCII_VALUE_A 0x41
#define LTE_RRC_ASCII_VALUE_B 0x42
#define LTE_RRC_ASCII_VALUE_C 0x43
#define LTE_RRC_ASCII_VALUE_D 0x44
#define LTE_RRC_ASCII_VALUE_E 0x45
#define LTE_RRC_ASCII_VALUE_F 0x46

/*! @brief Enumeration for LTE RRC triggers for revoking the operation for which 
controller is suspended
*/
typedef enum
{
  LTE_RRC_REVOKE_TRIGGER_SIB_3HR_TMR,
  LTE_RRC_REVOKE_TRIGGER_SIB_PERIODIC_SIB8,
  LTE_RRC_REVOKE_TRIGGER_SIB_SI_UPDATE,
  LTE_RRC_REVOKE_TRIGGER_SIB_ETWS,
  LTE_RRC_REVOKE_TRIGGER_SIB_CMAS,
  LTE_RRC_REVOKE_TRIGGER_SIB_WT_TMR,
  LTE_RRC_REVOKE_TRIGGER_PAG_DRX_INFO_REQ,
  LTE_RRC_REVOKE_TRIGGER_PAG_SIM_UPDATE_REQ,
  LTE_RRC_REVOKE_TRIGGER_PLMN_SEARCH_MANUAL,
  LTE_RRC_REVOKE_TRIGGER_MEAS_T320_EXPIRY,
  LTE_RRC_REVOKE_TRIGGER_MEAS_W_RESEL_TMR_EXPIRY,
  LTE_RRC_REVOKE_TRIGGER_MEAS_TDS_RESEL_TMR_EXPIRY,
  LTE_RRC_REVOKE_TRIGGER_CEP,
  LTE_RRC_REVOKE_TRIGGER_IRAT_CGI,
  LTE_RRC_REVOKE_TRIGGER_EMBMS,
  LTE_RRC_REVOKE_TRIGGER_SIB_PERIODIC_SIB16,
  LTE_RRC_REVOKE_TRIGGER_SIB_RF_RECOVERY,
  LTE_RRC_REVOKE_TRIGGER_SIB_EAB,
  LTE_RRC_REVOKE_TRIGGER_MAX
} lte_rrc_revoke_trigger_e;

/*! @brief Enumeration for generic error fatal reason 
*/
typedef enum
{
  LTE_RRC_ERR_FATAL_CTLR_DEADLOCK_TMR_EXPIRY, /*!< 0 */
} lte_rrc_err_fatal_e ;

/*! @brief Data structure for RRC timer
*/
typedef struct
{
  lte_rrc_timer_type timer; /*!< MCS timer */
  msgr_umid_type umid; /*!< Message ID to be posted in callback */
} lte_rrc_tmr_s;

/*! @brief Enumeration for LTE band type (DL/UL/Both)
*/
typedef enum
{
  LTE_RRC_DL_BAND = 0x1,
  LTE_RRC_UL_BAND = 0x2,
  LTE_RRC_DL_UL_BAND = 0x3,
  LTE_RRC_PARTIAL_DL_UL_BAND = 0x40, /*!< Band property to denote that UL and DL 
				       BW ranges are not the same. e.g. B66 */
}lte_rrc_band_type_e;

/*! @brief Data structure for EUTRA Band Info
*/
typedef struct
{
  uint16 band;                         /*!< band number in integer as signalled in OTAs */
  boolean is_tdd;                      /*!< boolean to indicate if band is TDD or not */
  lte_rrc_band_type_e dl_ul_mask;      /*!< Bit mask to indicate the band type (DL/UL/Both) */
  lte_earfcn_t n_offs_dl;              /*!< earfcn start for DL */
  lte_earfcn_t n_offs_ul;              /*!< earfcn start for UL */
  uint32 f_dl_low;                     /*!< phy freq start for DL in 100Khz */
  uint32 f_ul_low;                     /*!< phy freq start for UL in 100Khz */
  uint16 chnl_bw_mask;                 /*!< mask indicating supported channel BWs */
  uint16 bw_mhz;                       /*!< Bandwidth in MHz */
  lte_rrc_band_type_e scnd_bw_type;    /*!< Bit to indicate if secondary BW is mentioned for UL or DL */
  uint16 scnd_bw_mhz;                  /*!< Secondary Bandwidth in MHz */
}lte_rrc_band_info_s;

typedef struct
{
  boolean single_tx_info_valid; /*! Has the single Tx info been obtained from TRM?*/
  boolean single_tx_enabled; /*! Single Tx feature has been enabled for the UE*/ 
}lte_rrc_single_tx_s;

/*! @brief
  Band Updated info structure got from  
  EFS ("update_band_range")*/
typedef struct 
{
  /* Number of bands */
  uint16                          num_bands;
  /* Band freq info structure */
  lte_l1_band_freq_range_s  lte_band_range[LTE_RRC_MAX_BANDS_TO_UPDATE];
} lte_rrc_bands_updated_info_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  lte_rrc_release_e lte_rrc_release;  /*!< Global variable denoting the LTE RRC Release */
  lte_3gpp_release_ver_e lte_rrc_3gpp_release_ver;  /*!< Global variable denoting the LTE 3GPP Spec Release */
  boolean tdd_pruning; /*!< Global var for indicating if TDD prunning is required */
  boolean axgp; /*!< Global var for indicating that axgp band is supported*/
  boolean rohc_supported; /*!< Global var for indicating if ROHC is supported */
  boolean lte_deprio_supported; /*!< Global var for indicating if LTE deprioritization is supported */
  boolean asn1_profiling_enabled; /*!< Global var for indicating if asn1 profiling is enabled  */
  uint8 inst_id;  /*!< Instance ID  */
  lte_l1_extnd_bands_updated_info_s lte_rrc_update_band; /*!< Bands updated info via EFS ("update_band_range") */
  uint16 repeated_rlf_window_size_in_sec;
  int32 q_rxlevmin_custom;
  lte_l1_bs_bw_supported_s  lte_rrc_bs_bw_supported; /*!< BW supported info via EFS ("bs_bw_supported") */
  boolean disable_trm_sanity_check;/*!< If need to turn off TRM sanity checks*/
  lte_rrc_single_tx_s lte_rrc_single_tx; /*! Global var which contains single Tx feature info*/
  #ifdef FEATURE_LTE_ELS_ENABLED
  boolean els_signature_received_in_sib1; /*!< Boolean to indicate whether ELS Signature is received in SIB1*/
  lte_rrc_send_els_ind_e send_els_ind; /*!< Enum to indicate when to send ELS UE Indication*/
  boolean els_ind_sent; /*!< Indicates if UE ELS indication is sent or not */
  boolean lte_els_supported; /*!< Global var for indicating if LTE ELS is supported */
  boolean lte_udc_supported; /*!< Global var for indicating if LTE UDC is supported */
  boolean skip_els_indi; /*!< Global var for indicating if ELS UE Indication or ELS CAP Info needs to be sent*/
  uint16 els_nw_feature_grp_ind; /*!< ELS NW feature group indicators*/
  #endif
} lte_rrc_utils_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* initializes message header with default values for RRC */
extern void lte_rrc_init_default_hdr
(
  msgr_hdr_s *hdr_ptr, /*!< Header  to be filled */
  msgr_umid_type umid /*!< Message ID */
);

/* initializes message header for DSM items with default values for RRC */
extern void lte_rrc_init_default_hdr_attach
(
  msgr_hdr_struct_type *hdr_ptr,/*!< Header  to be filled */
  msgr_tech_module_type from,
  msgr_umid_type umid,/*!< Message ID */
  msgr_priority_t pri,
  uint8 num /*!< Number of attachments */
);

/*Initializes lte_rrc_plmn_s with Invalid MCC/MNC digit 0xF*/
extern void lte_rrc_init_plmn_id
(
  lte_rrc_plmn_s* plmn_id_ptr  /*! RRC PLMN format */
);

/* Initialize a timer */
extern void lte_rrc_tmr_init
(
  lte_rrc_tmr_s *tmr_ptr,/*!< Pointer to timer */
  msgr_umid_type umid /*!< UMID of message to be posted from callback */
);

/*De-Initialize a timer*/
extern void lte_rrc_tmr_deinit
(
  lte_rrc_tmr_s *tmr_ptr /*!< Pointer to timer */
);


/* Initialize a timer */
extern void lte_rrc_tmr_loc_services_init
(
  lte_rrc_tmr_s *tmr_ptr,/*!< Pointer to timer */
  lte_rrc_loc_services_tmr_type_e tmr_type /*!< UMID of message to be posted from callback */
);

/* Start/restart a timer */
extern void lte_rrc_tmr_start
(
  lte_rrc_tmr_s *tmr_ptr, /*!< Pointer to timer */
  uint64 tmr_val, /*!< First expiry period in ms - or simply the expiry period
                       for oneshot timer */
  uint64 reload_val, /*!< Subsequent expiry period in ms for periodic timers
                          only, specify 0 for oneshot timers*/
  lte_rrc_tmr_mode_e mode /*!< Oneshot or Periodic */
);

/* Stop a timer */
extern void lte_rrc_tmr_stop
(
  lte_rrc_tmr_s *tmr_ptr /*!< Pointer to timer */
);

/* Get remaining time of a timer in ms */
extern uint64 lte_rrc_tmr_get_remaining
(
  lte_rrc_tmr_s *tmr_ptr /*!< Pointer to timer */
);

/* Is timer running */
extern boolean lte_rrc_tmr_is_running
(
  lte_rrc_tmr_s *tmr_ptr /*!< Pointer to timer */
);

/* Get B41 range from EFS */
extern boolean lte_rrc_get_band_range_from_efs
(
  sys_sband_lte_e_type band, /*!< Band*/
  uint16 * start_freq, /*!< Start frequency */
  uint16 * end_freq /*!< End frequency */
);


/* Get band from DL EARFCN, band_is_valid set to TRUE if valid, else FALSE */
extern sys_sband_lte_e_type lte_rrc_get_band_from_dl_earfcn
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  boolean *band_is_valid_ptr /*!< Is it valid? */
);

/* Get band from DL EARFCN (without considering update_band_range EFS), 
   band_is_valid set to TRUE if valid, else FALSE */
extern sys_sband_lte_e_type lte_rrc_get_band_from_dl_earfcn_wo_efs
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  boolean *band_is_valid_ptr /*!< Is it valid? */
);


/* Get band from UL EARFCN, band_is_valid set to TRUE if valid, else FALSE */
extern sys_sband_lte_e_type lte_rrc_get_band_from_ul_earfcn
(
  lte_earfcn_t ul_earfcn, /*!< UL EARFCN */
  boolean *band_is_valid_ptr /*!< Is it valid? */
);

/* Get band from DL EARFCN including CA bands, band_is_valid set to TRUE if valid, else FALSE */
sys_sband_lte_e_type lte_rrc_get_band_from_dl_earfcn_ca
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  boolean *band_is_valid_ptr /*!< Is it valid? */
);

extern sys_lte_band_mask_e_type lte_rrc_get_subset_band
(
 sys_lte_band_mask_e_type lte_band_pref, /*!< LTE band preference in */
 sys_sband_lte_e_type band
);

extern sys_sband_lte_e_type lte_rrc_get_band_based_on_bw
(
  sys_lte_band_mask_e_type lte_band_pref_subset_mask,/* Subset band that are supported by UE*/
  uint16 bw
);
extern sys_plmn_id_s_type lte_rrc_plmn_to_sys_plmn_id
( 
  lte_rrc_plmn_s plmn_id  /*! RRC PLMN format */
);

extern lte_rrc_plmn_s lte_rrc_sys_plmn_to_rrc_plmn_id
(
  sys_plmn_id_s_type plmn_id  /*! Sys PLMN format */
);

/* Get WCDMA band from DL UARFCN */
extern sys_sband_e_type lte_rrc_get_wcdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
);

/* Get TDSCDMA band from DL UARFCN */
extern sys_sband_tds_e_type lte_rrc_get_tdscdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
);

/* Convert band number to band mask (sys_lte_band_mask_e_type) */
sys_lte_band_mask_e_type lte_rrc_band_from_int_to_mask
(
  uint16 band  /*!< Band number */
);

/* Convert band number to band enum (sys_sband_lte_e_type) */
extern sys_sband_lte_e_type lte_rrc_band_from_int_to_enum
(
  uint16 band  /*!< Band enum */
);

/* Reverse uint16*/
extern uint16 lte_rrc_reverse_uint16
(
  uint16 val
);

/* Convert band enum (sys_sband_lte_e_type) to uint8 (freqBandIndicator) */
extern uint16 lte_rrc_band_from_enum_to_int
(
  sys_sband_lte_e_type band /*!< Band enum */
);

/* Maps lte_rrc_CDMA2000_Bandclass enumeration to sys_band_class_e_type */
extern sys_band_class_e_type lte_rrc_get_sys_band_class
(
 lte_rrc_osys_BandclassCDMA2000 band_class  /*!< Band enum */
);

extern uint16 lte_rrc_reverse_uint16
(
  uint16 source
);

/* Maps sys_band_class_e_type enumeration to lte_rrc_CDMA2000_Bandclass  */
extern lte_rrc_osys_BandclassCDMA2000 lte_rrc_get_cdma_asn_band_class
(
 sys_band_class_e_type band_class  /*!< Band enum */
);


/* Returns a formatted PLMN string */
extern char* lte_rrc_get_plmn_as_str
(
 const lte_rrc_plmn_s *plmn,  /*!< PLMN */
 char *buf,                   /*!< Buffer */
 int avail_buf_size           /*!< Buffer Size */
);

/* Returns a formatted EHPLMN string */
extern char* lte_rrc_get_ehplmn_list_as_str
(
 const lte_rrc_ehplmn_list_s *ehplmn_list,  /*!< EHPLMN List */
 char *buf,                             /*!< Buffer */
 int avail_buf_size                     /*!< Buffer Size */
);

/* Returns a formatted PLMN list string */
extern char* lte_rrc_get_plmn_list_as_str
(
 const lte_rrc_plmn_list_s *plmn_list,  /*!< PLMN List */
 char *buf,                             /*!< Buffer */
 int avail_buf_size                     /*!< Buffer Size */
);

/* Returns a formatted TA string */
extern char* lte_rrc_get_ta_as_str
(
 const lte_rrc_tracking_area_s *ta,  /*!< TA */
 char *buf,                          /*!< Buffer */
 int avail_buf_size                  /*!< Buffer Size */
);

/* Returns a formatted TA list string */
extern char* lte_rrc_get_ta_list_as_str
(
 const lte_rrc_tracking_area_list_s *ta_list,  /*!< TA List */
 char *buf,                                    /*!< Buffer */
 int avail_buf_size                            /*!< Buffer Size */
);

/* reads RRC NV items */
extern void lte_rrc_nv_item_read
(
  void
);

/* Returns the LTE RRC release version */
extern lte_rrc_release_e lte_rrc_get_release_version
(
  void
);

/* Returns the revoke priority given the trigger */
extern lte_rrc_revoke_priority_e lte_rrc_get_revoke_priority
(
  lte_rrc_revoke_trigger_e trigger
);

/* Sends DLM Processed Indi indication */
extern void lte_rrc_send_dlm_processed_indi(void);

/* Sends revoke reqi */
extern void lte_rrc_send_revoke_reqi(void);

/* Converts the sys band to LTE band */
extern sys_sband_lte_e_type lte_rrc_utils_convert_sys_band_to_lte_band
(
  sys_band_class_e_type sys_band
);

/* Converts B12 DL earfcn to B17 DL earfcn */
extern lte_earfcn_t lte_rrc_convert_b12_to_b17_dl_earfcn
(
lte_earfcn_t b12_dl_earfcn
);

/* Converts B12 UL earfcn to B17 UL earfcn */
extern lte_earfcn_t lte_rrc_convert_b12_to_b17_ul_earfcn
(
  lte_earfcn_t b12_ul_earfcn
);

/* Converts B17 DL earfcn to B12 DL earfcn */
extern lte_earfcn_t lte_rrc_convert_b17_to_b12_dl_earfcn
(
lte_earfcn_t b17_dl_earfcn
);

/* Converts B17 UL earfcn to B12 UL earfcn */
extern lte_earfcn_t lte_rrc_convert_b17_to_b12_ul_earfcn
(
  lte_earfcn_t b17_ul_earfcn
);

/*Is Rel9 supported ?*/
extern boolean lte_rrc_rel9_supported
(
  void
);

extern sys_sband_e_type lte_rrc_get_gsm_band_from_arfcn
(
  geran_grr_band_ind_e    band_ind,
  uint16                  arfcn,
  boolean                 *gsm_band_valid_ptr
);

extern lte_rrc_div_duplex_e lte_rrc_get_mode_for_earfcn
(
  lte_earfcn_t dl_earfcn
);
#ifdef TEST_FRAMEWORK
#error code not present
#endif
extern uint16 lte_rrc_get_rlf_window_size
(
  void
);

extern sys_lte_band_mask_e_type lte_rrc_remove_complete_band_overlaps
(
 const sys_lte_band_mask_e_type lte_band_pref /*!< LTE band preference in */
);

extern sys_lte_band_mask_e_type lte_rrc_get_band_from_update_range
(
  sys_lte_band_mask_e_type lte_band_pref 
);
extern boolean lte_rrc_band_is_superset
(
 sys_sband_lte_e_type band1,    /*!< Superset freq Band */
 sys_sband_lte_e_type band2    /*!< Subset freq Band */
);

extern boolean lte_rrc_get_axgp_supported
(
  void
);

extern uint8 lte_rrc_get_inst_id
(
  void
);

extern boolean lte_rrc_rohc_is_supported
(
  void
);

extern boolean lte_rrc_lte_deprio_is_supported
(
  void
);

/* Gets N_dl_offset for each band as defined in 36.101 */
extern lte_earfcn_t lte_rrc_get_earfcn_dl_offset
(
  sys_sband_lte_e_type band  /*!< Band enum */
);

/* Gets F_dl_low (in 100KHz) for each band as defined in 36.101 */
extern uint16 lte_rrc_get_freq_dl_low_10
(
  sys_sband_lte_e_type band  /*!< Band enum */
);

/* Gets allowed bandwidths mask for the band as defined in 36.101 */
extern uint16 lte_rrc_get_bw_mask
(
  sys_sband_lte_e_type band  /*!< Band enum */
);

/* Converts bandwidth enum to bandwidth mask repesentaiton. */
extern uint16 lte_rrc_get_bandwidth_as_mask
(
 lte_bandwidth_e bw /*!< Bandwith enum */
);

/* This API Returns TRUE if the input band is TDD, FALSE otherwise. */
extern boolean lte_rrc_lte_band_is_tdd 
(
  sys_sband_lte_e_type band  /*!< Band enum */
);

/* This API Returns TRUE if asn profiling is enabled, FALSE otherwise. */
extern boolean lte_rrc_is_asn1_profiling_enabled
(
  void
);

/* This function converts DL EARFCN to freq in 100 MHz */
extern uint16 lte_rrc_get_freq_from_earfcn
(
 lte_earfcn_t earfcn /*!< Earfcn */
);

/* This function converts DL EARFCN to freq in 100 MHz, Should be used only for CA */
extern uint16 lte_rrc_get_freq_from_earfcn_ca
(
 lte_earfcn_t earfcn /*!< Earfcn */
);
/* This API does LTE_ASSERT. F3 in invoking function will dictate the cause */
extern void lte_rrc_throw_err_fatal
(
  lte_rrc_err_fatal_e reason
);

/* This API Returns E_SUCCESS if conversion of given dl earfcn was successful between bands provided */
extern errno_enum_type lte_rrc_convert_dl_earfcn_by_band
(
  lte_earfcn_t *dl_earfcn,
  sys_sband_lte_e_type from_band,
  sys_sband_lte_e_type to_band  
);

/* This API Returns E_SUCCESS if conversion of given ul earfcn was successful between bands provided */
extern errno_enum_type lte_rrc_convert_ul_earfcn_by_band
(
  lte_earfcn_t *ul_earfcn,
  sys_sband_lte_e_type from_band,
  sys_sband_lte_e_type to_band  
);

/* This API Returns time difference between two timestamps provided in ms, taking care of wrap-around cases */
extern uint64 lte_rrc_get_time_diff
(
  uint64 cur_time, 
  uint64 ref_time
);

/* This API Returns E_SUCCESS if the passed osys plmn info is validated and updated in the provided DS */
extern lte_errno_e lte_rrc_process_plmn_info
(
  lte_rrc_plmn_s *plmn_info_ptr,
  lte_rrc_osys_PLMN_Identity *osys_plmn_info_ptr/*!< ASN.1 decoded OTA msg */
);

/* This API Returns E_SUCCESS if meas_subframe_pattern gets populated */
lte_errno_e lte_rrc_populate_meas_SubframePattern
( 
  lte_l1_meas_subfm_pattern_param_s *subfm_pattern_cfg_ptr,
  lte_rrc_osys_MeasSubframePattern_r10 *measSubframePatternPtr,
  boolean *enable_ptr,
  lte_rrc_div_duplex_e cell_mode
);

/* This API  Creates a config file for all RRC EFS files */
extern void lte_rrc_create_nv_item_config_file 
( 
  void 
);

/* This API  free redirection info (cell_info_list).  */
extern void lte_rrc_util_free_redir_info
(
  lte_rrc_redir_info_s *
);

/* This API Returns E_SUCCESS if conversion of given dl earfcn was successful between bands provided */
extern errno_enum_type lte_rrc_convert_dl_earfcn_by_band_wo_mfbi
(
  lte_earfcn_t *dl_earfcn,
  sys_sband_lte_e_type from_band,
  sys_sband_lte_e_type to_band  
);

extern boolean lte_rrc_utils_cm_deinit_msgr
(
  void
);

#ifdef FEATURE_LTE_RRC_MEM_REPORT
/* This API prints the size of various structures for memory analysis study */
extern void lte_rrc_disp_mem_report
(
  void
);
#endif



/*! This macro is a wrapper for the memscpy function */

#define LTE_RRC_MEMCPY(dst_ptr, dst_size, src_ptr, src_size) \
{ \
  size_t ret_size = 0; \
  if (dst_size < src_size) \
  { \
    LTE_RRC_MSG_2_FATAL ( "UTILS: memscpy, dst_size %d bytes < src_size %d, " \
                         "bytes", dst_size, src_size ); \
  } \
  if (src_size > (ret_size = memscpy(dst_ptr, dst_size, src_ptr, src_size))) \
  { \
    LTE_RRC_MSG_2_FATAL ( "UTILS: Truncation in memscpy, bytes copied to dest %d, " \
                         "src size %d bytes", ret_size, src_size ); \
  } \
} 


/*! This macro is a wrapper for the memsmove function */

#define LTE_RRC_MEMMOVE(dst_ptr, dst_size, src_ptr, src_size) \
{ \
  size_t ret_size = 0; \
  if (dst_size < src_size) \
  { \
    LTE_RRC_MSG_2_FATAL ( "UTILS: memsmove, dst_size %d bytes < src_size %d, " \
                         "bytes", dst_size, src_size ); \
  } \
  if (src_size > (ret_size = memsmove(dst_ptr, dst_size, src_ptr, src_size))) \
  { \
    LTE_RRC_MSG_2_FATAL ( "UTILS: Truncation in memsmove, bytes copied to dest %d, " \
                         "src size %d bytes", ret_size, src_size ); \
  } \
} 

/* This API updates the passed InstnaceID with the one in DB */
extern void lte_rrc_update_inst_id
(
  uint8 inst_id_new /*!< Instance ID */
);
/* This API validates the passed InstanceID with the one in DB */
extern void lte_rrc_validate_inst_id
(
  uint8 inst_id_new /*!< Instance ID */
);
/* This API returns AS_ID */
extern sys_modem_as_id_e_type lte_rrc_get_as_id
(
  void
);
/* This API validates BW against Band */
extern lte_errno_e lte_rrc_validate_bw_against_band
(
  sys_sband_lte_e_type band, /*!< Band */
  lte_bandwidth_e bandwidth /*!< BandWidth */
);
extern int32 lte_rrc_get_q_rxlevmin_custom(void);

/* Convert uint8 to WCDMA sys band enum (sys_sband_e_type) */
extern sys_sband_e_type lte_rrc_get_wcdma_sys_band_from_uint8
(
  uint8 band  /*!< band number */
);

extern boolean lte_rrc_band_is_supported 
(
  sys_sband_lte_e_type lte_band, /*!< DL EARFCN */
  sys_lte_band_mask_e_type *lte_band_preference /*!< LTE band preference */
);

extern lte_rrc_band_info_s* lte_rrc_get_band_info
(
  sys_sband_lte_e_type band_enum,
  lte_rrc_band_info_s *band_info
);

extern uint16 lte_rrc_get_extnd_fbi_from_sib1
(
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr
);

extern lte_earfcn_t lte_rrc_get_extnd_earfcn_from_meas_obj
(
  lte_rrc_osys_MeasConfig *meas_cfg_ptr,
  uint8 idx
);

extern sys_sband_lte_e_type lte_rrc_get_extnd_band_from_mfbi_v9e0
(
  lte_rrc_osys_MultiBandInfoList_v9e0 *mfbi_info_extn_ptr,
  uint8 idx
);

/* Send change in priority to controller */
extern void lte_rrc_send_trm_priority_change_indi
(
  lte_trm_priority_e        trm_pri, /*trm priority*/
  lte_rrc_trm_client_id_e   trm_client,
  boolean                   send_pri_to_ml1
);

extern void lte_rrc_utils_nv_multi_read(int8 sub_id);

extern boolean lte_rrc_compare_eutra_earfcn_in_hz
(
  lte_earfcn_t src_earfcn, //earfcn signalled in OTA, so band validation is necessary
  lte_earfcn_t ref_earfcn,  //earfcn obtained from DB for comparision
  boolean print_only
);

extern lte_earfcn_t lte_rrc_get_extnd_earfcn_from_mobility_ctrl_info
(
  lte_rrc_osys_MobilityControlInfo *mob_ctrl_info_ptr
);

extern void lte_rrc_send_cphy_nv_update_req
(
  void
);

extern boolean lte_rrc_band_in_mfbi_is_valid
(
  sys_sband_lte_e_type band_in_fbi,
  sys_sband_lte_e_type band_in_mfbi
);

extern void lte_rrc_check_trm_clients(void);

extern boolean lte_rrc_compare_eutra_earfcn_in_hz
(
  lte_earfcn_t src_earfcn, //earfcn signalled in OTA, so band validation is necessary
  lte_earfcn_t ref_earfcn,  //earfcn obtained from DB for comparision
  boolean print_only
);

extern void lte_rrc_send_common_msim_crat_feature_req(void);

extern sys_lte_band_mask_e_type lte_rrc_get_lte_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_ptr
);

extern sys_lte_band_mask_e_type lte_rrc_get_DL_only_band_mask 
(
  void
);

extern void lte_rrc_check_sanity_trm_hw_capability
(
  lte_cphy_ue_mode_type_e            cphy_ue_mode, 
  sys_modem_device_mode_e_type       device_mode,
  sys_modem_dual_standby_pref_e_type standby_mode
);

extern uint8 lte_rrc_utils_read_geran_carrier_freq_info
(
   /*! OTA GERAN Freq to extract */
  lte_rrc_osys_CarrierFreqsGERAN   *geran_freqs,
  /*! Ouput of this utility function to populate */
  lte_l1_gsm_arfcn_t               *ouput_gsm_freqs,
  /*! Num of frequencies that have been added already*/
  uint8                             num_freq,
  /*! The max number of geran freq to populate in output*/
  uint8                            max_num_freq
);
#ifdef FEATURE_LTE_ELS_ENABLED
extern void lte_rrc_update_els_sig_received_in_sib1_flag 
(
  boolean els_sig_received_in_sib1_val
);

extern void lte_rrc_update_els_nw_feature_grp_ind 
(
  uint16 els_nw_feature_grp_ind
);

extern void lte_rrc_update_send_els_ind 
(
  lte_rrc_send_els_ind_e send_els_ind_val
);

extern lte_rrc_send_els_ind_e lte_rrc_get_send_els_ind 
(
  void
);

extern boolean lte_rrc_els_ind_sent
(
  void
);

extern void lte_rrc_update_els_ind_sent_status
(
  boolean els_ind_sent
);

extern boolean lte_rrc_skip_els_indi
(
  void
);

extern void lte_rrc_update_skip_els_indi
(
  boolean send_els_ind_or_cap
);

extern boolean lte_rrc_get_els_sig_received_in_sib1_flag 
(
  void
);

extern uint16 lte_rrc_get_els_nw_feature_grp_ind 
(
  void
);

extern boolean lte_rrc_lte_els_is_supported
(
  void
);

extern boolean lte_rrc_lte_udc_is_supported
(
  void
);
#endif

extern lte_errno_e lte_rrc_get_ul_earfcn_range_for_band_in_efs
(
  sys_sband_lte_e_type band,
  lte_earfcn_t *ul_earfcn_start,
  lte_earfcn_t *ul_earfcn_end
);

extern boolean lte_rrc_lte_band_is_DL_only 
(
  sys_sband_lte_e_type band  /*!< Band enum */
);

extern void lte_rrc_log_band_mask
(
  sys_lte_band_mask_e_type *band_mask_ptr
);

extern sys_lte_band_mask_e_type lte_rrc_get_XOR_band_mask
(
  sys_lte_band_mask_e_type band_mask1,
  sys_lte_band_mask_e_type band_mask2
);

extern sys_lte_band_mask_e_type lte_rrc_get_OR_band_mask
(
  sys_lte_band_mask_e_type band_mask1,
  sys_lte_band_mask_e_type band_mask2
);

extern sys_lte_band_mask_e_type lte_rrc_get_AND_band_mask
(
  sys_lte_band_mask_e_type band_mask1,
  sys_lte_band_mask_e_type band_mask2
);

extern sys_lte_band_mask_e_type lte_rrc_get_NOT_band_mask
(
  sys_lte_band_mask_e_type band_mask
);

extern boolean lte_rrc_check_if_band_mask_is_valid
(
  sys_lte_band_mask_e_type band_mask
);

extern boolean lte_rrc_compare_rat_band_cap
(
  sys_band_cap_u_type band_cap1,
  sys_sys_mode_e_type acq_mode1,
  sys_band_cap_u_type band_cap2,
  sys_sys_mode_e_type acq_mode2
);

extern lte_rrc_osys_FreqBandIndicator lte_rrc_get_sib1_freq_band_indicator
(
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg_ptr  /*!< Pointer to SIB1 */
);

#endif /* LTE_RRC_UTILS_H */

