/*!
  @file
  lte_rrc_csp.h

  @brief
  External API for RRC CSP.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_csp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/2011 vatsac  Added forbidden_manual_ta_list in lte_rrc_csp_nas_req_info_s
04/13/11   sk      Added support for storing deactivate reason especially for
                   the case when the reason is LPM.
03/13/2011 sureshs Added API to get tdd/fdd mode
02/02/2010 vatsac  Removed support for irat_info in service req
01/13/2010 sureshs Support for lte_rrc_csp_cell_selection_is_in_progress
01/12/2010 sureshs Accessor functions for G and W band preferences
12/18/2009 sureshs Moved nas req info type to external header file
11/23/2009 sureshs Moved acq db type from external header file
11/17/2009 sureshs Added function for getting camped status
11/03/2009 sureshs Added GW supported functions
09/19/2009 sureshs Added CDMA band supported and preference functions
09/18/2009 sureshs Added CDMA supported functions
08/17/2009 sureshs Added lte_rrc_csp_get_camped_cell_cell_identity
01/28/2009 sureshs Added function to get band preference
01/19/2009 sureshs Removed RSSI set routine
01/03/2008 ask     Included lte_rrc_int_msg.h to resolve include dependencies
12/11/2008 sureshs Added function to get camped BW
12/01/2008 sureshs Added function to get camped TAC
10/28/2008 sureshs Added lte_rrc_csp_get_selected_plmn routine
10/08/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CSP_H
#define LTE_RRC_CSP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_variation.h"
#include <lte.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_osys_asn1.h>
#include <sys.h>
#include "__lte_rrc_csp.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! Maximum value of p_Max in db in SIB1 - 36331 Section 6 */
#define LTE_RRC_CSP_MAX_SIB1_PMAX_VAL 33

/*! Minimum value of p_Max in db in SIB1  - 36331 Section 6 */
#define LTE_RRC_CSP_MIN_SIB1_PMAX_VAL (-30)

/*! Cell quality threshold in dBm  - 36304 Section 5.1.2.2 */
#define LTE_RRC_CSP_HIGH_CELL_QUALITY_THRESHOLD (-110)

/*! Maximum number of entries in SIB1 PLMN list */
#define LTE_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE 6

/*! Index to split acq for X2L blind redireciton*/
#define LTE_RRC_CSP_SPLIT_ACQ_DB_INDEX 4

/*! Max ACQ DB EFS version supported */
#define LTE_RRC_ACQ_DB_EFS_MAX_VER 2

/* Returns the network select mode for the latest service_req */
extern sys_network_selection_mode_e_type lte_rrc_csp_get_network_select_mode(void);

/* Returns the current cell ID and DL freq, E_SUCCESS if camped, E_FAILURE
   otherwise */
extern lte_errno_e lte_rrc_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Cell info pointer */
);

/* Returns the PLMN selected for camping and its index (0 ..) in SIB1 list. For
   limited service, it returns the first PLMN in SIB1 list and index of 0.
   For use in Conn Setup Complete, increment the index by 1. Returns E_SUCCESS 
   if camped, E_FAILURE otherwise */
extern lte_errno_e lte_rrc_csp_get_selected_plmn
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

/* Returns the TAC of the camped cell's TAC, E_SUCCESS if camped, E_FAILURE
   otherwise */
extern lte_errno_e lte_rrc_csp_get_camped_cell_tac
(
  uint16 *tac_ptr /*!< Pointer to TAC to be filled in */
);

/* Returns the BW of the camped cell's TAC, E_SUCCESS if camped, E_FAILURE
   otherwise */
extern lte_errno_e lte_rrc_csp_get_camped_cell_bw
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
);
/* Returns the cell identity of the LAST camped cell's TAC, E_SUCCESS if dd_ptr is available, 
   E_FAILURE otherwise */
extern lte_errno_e lte_rrc_csp_get_last_camped_cell_identity_for_irat
(
  lte_rrc_global_cell_id_t *cell_identity_ptr /*!< Pointer to be filled in */
);
/* Returns the cell identity of the camped cell's TAC, E_SUCCESS if camped, 
   E_FAILURE otherwise */
extern lte_errno_e lte_rrc_csp_get_camped_cell_identity
(
  lte_rrc_global_cell_id_t *cell_identity_ptr /*!< Pointer to be filled in */
);

/* Returns the csg id of the camped cell, E_SUCCESS if camped, E_FAILURE
   otherwise */
extern lte_errno_e lte_rrc_csp_get_camped_cell_csg_id
(
  sys_csg_id_type *csg_id_ptr /*!< CSG Id Ptr */
);

/* Is reselection in progress? */
extern boolean lte_rrc_csp_reselection_is_in_progress(void);

/* Returns the band preference */
extern sys_lte_band_mask_e_type lte_rrc_csp_get_lte_band_preference(void);

/* This function compares if two plmns are same. Returns True if the PLMNs 
   match, False if they dont */
extern boolean lte_rrc_csp_compare_plmn_ids
(
  const lte_rrc_plmn_s *plmn1_ptr,     /*!< PLMN ID 1 */
  const lte_rrc_plmn_s *plmn2_ptr      /*!< PLMN ID 2 */
);

/* This function searches if the PLMN in the EHPLMN list provided. 
   Returns True if present, FALSE otherwise. If TRUE, index is filled in */
extern boolean lte_rrc_csp_check_for_plmn_in_ehplmn_list
(
  const lte_rrc_plmn_s *plmn_ptr,   /*!< PLMN to be searched in PLMN list */
  const lte_rrc_ehplmn_list_s *ehplmn_list_ptr,    /*!< EHPLMN list */
  uint32 *index_ptr /*!< Index of found PLMN in list */
);

/* This function searches if the PLMN in the PLMN list provided. 
   Returns True if present, FALSE otherwise. If TRUE, index is filled in */
extern boolean lte_rrc_csp_check_for_plmn_in_list
(
  const lte_rrc_plmn_s *plmn_ptr,   /*!< PLMN to be searched in PLMN list */
  const lte_rrc_plmn_list_s *plmn_list_ptr,    /*!< PLMN list */
  uint32 *index_ptr /*!< Index of found PLMN in list */
);

/* This function checks if any EHPLMN in first list matches one in the second.
   Returns TRUE if so, FALSE otherwise. Also returns index in second list 
   which matched. */
extern boolean lte_rrc_csp_find_match_in_ehplmn_lists
(
  const lte_rrc_ehplmn_list_s *ehplmn_list1_ptr,  /*!< EHPLMN list 1 */
  const lte_rrc_plmn_list_s *plmn_list2_ptr,  /*!< PLMN list 2 */
  uint32 *index_ptr /*!< index in second list if match */
);

/* This function checks if any PLMN in first list matches one in the second.
   Returns TRUE if so, FALSE otherwise. Also returns index in second list 
   which matched. */
extern boolean lte_rrc_csp_find_match_in_plmn_lists
(
  const lte_rrc_plmn_list_s *plmn_list1_ptr,  /*!< PLMN list 1 */
  const lte_rrc_plmn_list_s *plmn_list2_ptr,  /*!< PLMN list 2 */
  uint32 *index_ptr /*!< index in second list if match */
);

/* This function returns TRUE if the LTE band is supported */
extern boolean lte_rrc_csp_lte_band_is_supported
(
  sys_sband_lte_e_type band /*!< CPHY band */
);

/* This function returns TRUE if 1X is supported */
extern boolean lte_rrc_csp_cdma_1x_is_supported(void);

/* This function returns TRUE if DO is supported */
extern boolean lte_rrc_csp_cdma_do_is_supported(void);

/* This function returns TRUE if PCI_LOCK feature enabled */
extern boolean lte_rrc_csp_pci_lock_enabled(void);

/* This function returns TRUE if EARFCN_LOCK feature enabled */
extern boolean lte_rrc_csp_earfcn_lock_enabled(void);

/*This function returns TRUE if dl_earfcn is one of the locked earfcn*/
extern boolean lte_rrc_csp_earfcn_is_locked(lte_earfcn_t dl_earfrcn);

/*This function returns TRUE if two EARFCNs present in locked_earfcn efs are same.*/
extern boolean lte_rrc_csp_locked_earfcn_same(void);

/* This function returns TRUE if the 1X band is supported */
extern boolean lte_rrc_csp_cdma_1x_band_is_supported
(
  sys_band_class_e_type band /*!< Band */
);

/* This function returns TRUE if the GSM band is supported */
extern boolean lte_rrc_csp_gsm_band_is_supported
(
  sys_sband_e_type band /*!< Band */
);

/* This function returns TRUE if the WCDMA band is supported */
extern boolean lte_rrc_csp_wcdma_band_is_supported
(
  sys_sband_e_type band /*!< Band */
);

/* This function returns TRUE if the TDSCDMA band is supported */
extern boolean lte_rrc_csp_tdscdma_band_is_supported
(
  sys_sband_tds_e_type band /*!< Band */
);

/* This function returns TRUE if the DO band is supported */
extern boolean lte_rrc_csp_cdma_do_band_is_supported
(
  sys_band_class_e_type band /*!< Band */
);

/* This function returns DO band preference */
extern sys_band_mask_type lte_rrc_csp_get_do_band_preference(void);

/* This function returns 1X band preference */
extern sys_band_mask_type lte_rrc_csp_get_1x_band_preference(void);

/* This function returns GSM band preference */
extern sys_band_mask_type lte_rrc_csp_get_gsm_band_preference(void);

/* This function returns WCDMA band preference */
extern sys_band_mask_type lte_rrc_csp_get_wcdma_band_preference(void);

/* This function returns TDSCDMA band preference */
extern sys_band_mask_type lte_rrc_csp_get_tdscdma_band_preference(void);

/* This function returns TRUE if GSM is supported */
extern boolean lte_rrc_csp_gsm_is_supported(void);

/* This function returns TRUE if WCDMA is supported */
extern boolean lte_rrc_csp_wcdma_is_supported(void);

/* This function returns TRUE if TDSCDMA is supported */
extern boolean lte_rrc_csp_tdscdma_is_supported(void);

/* This function returns TRUE if WCDMA is supported */
extern boolean lte_rrc_csp_cap_wcdma_is_supported(void);

/* This function returns TRUE if TDSCDMA is supported */
extern boolean lte_rrc_csp_cap_tdscdma_is_supported(void);

/* Returns the camped status */
extern lte_rrc_camped_status_e lte_rrc_csp_get_camped_status(void);

/* Returns TRUE if band conversion is applicable else FALSE */
extern boolean lte_rrc_csp_is_band_change_applicable
(
 sys_sband_lte_e_type ,
 sys_sband_lte_e_type 
);


/*! @brief Enum to represent reservation status of PLMN
*/
typedef enum
{
  /*! Reserved */
  LTE_RRC_CSP_PLMN_STATUS_RESERVED,                
    
  /*! Not reserved */
  LTE_RRC_CSP_PLMN_STATUS_NOT_RESERVED,    
}lte_rrc_csp_plmn_res_status_e;

#define LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES    10

// Hst earfcn entries
#define LTE_RRC_CSP_MAX_HST_EARFCN               2

//MAX entries for preference frequencies list
#define LTE_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES   32

//MAX entries for HST DB
#define LTE_RRC_CSP_MAX_HST_DB_ENTRIES           2


/*! @brief This structure defines the contents of each entry in the Acq DB
*/
typedef struct
{
	/*! PLMN List from SIB1 */
  lte_rrc_plmn_list_s plmn_list;            

  /*! DL Center frequency */
  lte_earfcn_t dl_earfcn;          

	/*! Band */
  sys_sband_lte_e_type band;            

	/*! System Bandwidth */
	lte_bandwidth_e bandwidth;

	/*! Physical cell id of last camped cell */
	lte_phy_cell_id_t phy_cell_id;   

  /*! Global cell id (from SIB1) of last camped cell */
  lte_rrc_global_cell_id_t cell_identity;

  /*! Tracking area code of last camped cell */
  uint16 tac;

  /*! SIB db index of last camped cell */
  uint8 sib_db_key;

  /*! CSG Id of the cell */
  sys_csg_id_type csg_id;

  /*! PLMN selected for camping. For limited service, it is the first PLMN in
      the SIB1 list */
  lte_rrc_plmn_s selected_plmn;

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;
}lte_rrc_csp_acq_entry_s;

/*! @brief This structure defines the contents of acq band list */

typedef struct
{
  /*! Band */
  sys_sband_lte_e_type band[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  uint16 band_count;
  
}lte_rrc_csp_acq_band_list_s;

/*! @brief CSP Acq DB structure
*/
typedef struct
{
	/*! Acquisition Database */
  lte_rrc_csp_acq_entry_s db[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint32 acq_db_index[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

	/*! Indicates the number of items stored in acq list order. If value is 
	 *   LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is 
	 *	 full and an old entry needs to be cleared to make room for the new one 
	 */
  uint8 acq_db_count;

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];
}lte_rrc_csp_acq_db_s;

/*! @brief CSP Interfreq neighbors list
*/
typedef struct
{
  uint8 num_frequencies;
  lte_earfcn_bw intfreq_list[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];
  uint8 next_index;/*track next index location to update neighbors*/
  boolean  interfreq_scan;
}lte_rrc_interfreq_list;

typedef struct
{
  uint8 num_frequencies;
  lte_earfcn_bw hst_earfcn_list[LTE_RRC_CSP_MAX_HST_DB_ENTRIES];
}lte_rrc_hst_earfcn_list_s;

extern lte_rrc_hst_earfcn_list_s lte_rrc_csp_get_hst_earfcn_list(void);

/* Returns a pointer to acq db */
extern lte_rrc_csp_acq_db_s *lte_rrc_csp_get_acq_db(void);

/* Returns a pointer to inter frequency neighbor list */
extern lte_rrc_interfreq_list *lte_rrc_csp_get_inter_freq_neighbors(void);

extern void lte_rrc_csp_add_entry_to_acq_db
(
    lte_rrc_csp_acq_db_s *acq_db_ptr, /*!< Acq DB */
    const lte_rrc_csp_acq_entry_s *acq_entry_ptr
    /*!< Entry to be added to Acq DB */
);

extern lte_errno_e lte_rrc_csp_get_sib1_plmn_list(lte_rrc_plmn_list_s *plmn_list_ptr);


extern sys_lte_band_mask_e_type lte_rrc_csp_get_band_mask_from_acq_db(void);
/*! Return the earfcns num for PFL */
extern uint32 lte_rrc_csp_get_pref_freq_list
(
    lte_earfcn_t  **pref_earfcns_list_ptr  /*!< Pref earfcns List Ptr */
);

/*! @brief This structure holds the NAS request contents
*/
typedef struct
{
  /*! Transaction id from NAS */
	uint32 trans_id;

  /*! network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  /*! request type*/
  lte_rrc_service_req_type_e req_type;

  /*! scan type*/
  sys_scan_scope_e_type scan_scope;

	/*! Requested PLMN */
	lte_rrc_plmn_s req_plmn;

  /*! Is requested PLMN valid */
  boolean req_plmn_info_is_valid;

  /*! Is registered PLMN info available?  */
  boolean rplmn_info_is_valid; 

  /*! Registered PLMN */
  lte_rrc_plmn_s rplmn; 

  /*! Is HPLMN info available? */
  boolean hplmn_info_is_valid; 

  /*! HPLMN */
  lte_rrc_plmn_s hplmn; 

  /*! EHPLMN list */
  lte_rrc_ehplmn_list_s ehplmn_list; 

  /*! Forbidden TA list */
  lte_rrc_tracking_area_list_s forbidden_ta_list;

  /*!< Forbidden TA list in Manual mode */
  lte_rrc_tracking_area_list_s forbidden_manual_ta_list;

  sys_rat_pri_list_info_s_type rat_pri_list; /*!< RAT priority list */

  /*!< EPLMN list - list of PLMNS equivalent to RPLMN if rplmn_info_is_valid
       is TRUE */
  lte_rrc_plmn_list_s eplmn_list; 

  sys_csg_id_type csg_id; /*!< CSG Id */

  /*! Is E911 call pending */
  boolean emc_srv_pending;  

  /*! service request timeout duration */
  uint32 t_timeout;  

  /*!Boolean to give priority to EHPLMN/HPLMN to RPLMN*/
  boolean ehplmn_camping_allowed;

}lte_rrc_csp_nas_req_info_s;

/* Returns a pointer to NAS info */
extern const lte_rrc_csp_nas_req_info_s *lte_rrc_csp_get_nas_req_info(void);

/* Returns if cell selection/reselection/conn to idle - or any other form of 
   cell selection is in progress */
extern boolean lte_rrc_csp_cell_selection_is_in_progress(void);

/* Returns if Connected to Idle camping is in progress */
boolean lte_rrc_csp_connected_to_idle_camping_in_progress(void);

/* Returns number of Tx antennas obtained from MIB in acquisition cnf */
extern uint8 lte_rrc_csp_get_num_tx_antennas(void);

/* Allocates and initializes dynamic memory */
extern void lte_rrc_csp_dyn_mem_init(void);

/* Deallocates dynamic memory */
extern void lte_rrc_csp_dyn_mem_deinit(void);

/* Returns the sib_db_key of the camped cell */
extern lte_errno_e lte_rrc_csp_get_camped_cell_sib_db_key
(
  uint8 *sib_db_key_ptr /*!< Ptr to sib db key */
);

/* Returns the cell_access_status of the camped cell */
extern lte_errno_e lte_rrc_csp_get_camped_cell_access_status
(
 lte_rrc_cell_access_status_e *status_ptr /*!< Ptr to cell_access_status */
);

/* Returns the on_off IM3 backoff state */
extern boolean lte_rrc_csp_get_im3_backoff(void);

/* Returns the current mode - TDD or FDD - if camped
   or the previous camped mode if not camped. Returns 
   LTE_RRC_NONE_DIV_DUPLEX if never camped after power up */
extern lte_rrc_div_duplex_e lte_rrc_csp_get_last_camped_mode(void);

/* Returns deactivate reason */
extern sys_stop_mode_reason_e_type lte_rrc_csp_get_deactivate_reason(void);
/*Extract PLMN list from SIB1*/
extern void lte_rrc_csp_get_plmn_list_from_sib1
(
  const lte_rrc_osys_PLMN_IdentityList *sib1_plmn_list_ptr, 
                                     /*!< plmn list pointer from SIB1 */
  lte_rrc_plmn_list_s *plmn_list_ptr, /*!< pointer to extracted plmn list */
  lte_rrc_csp_plmn_res_status_e *plmn_res_status /* reservation status of 
                                                    each PLMN to be filled in */
);

/*Do MCCs of 2 PLMN's match*/
extern boolean lte_rrc_csp_mcc_is_same
(
  const lte_rrc_plmn_s *plmn1_ptr,     /*!< PLMN ID 1 */
  const lte_rrc_plmn_s *plmn2_ptr      /*!< PLMN ID 2 */
);

/* Gets band_priority_list */
extern uint32 lte_rrc_csp_get_band_priority_list
(
 sys_sband_lte_e_type **band_priority_list_ptr  /*!< Band Priority List Ptr */
);

/* Get the CSP state */
extern stm_state_t lte_rrc_csp_get_state
(
  void
);

/* This function checks the cell bar restrictions from SIB1. Returns TRUE if 
   barred, FALSE otherwise. Implements 36.304 Section 5.3.1. */
extern boolean lte_rrc_csp_cell_is_barred
(
  /*!< Cell barring status */
  lte_rrc_osys_SystemInformationBlockType1_cellAccessRelatedInfo_cellBarred cell_barred,  
  const lte_rrc_plmn_list_s *plmn_list_ptr, /*!< PLMN list from SIB1 */
  const lte_rrc_csp_plmn_res_status_e *plmn_res_status, /*!< Reservation status 
                                                            of PLMN list from 
                                                            SIB1 */
  boolean hplmn_info_is_valid, /*!< Is HPLMN info available?  */
  lte_rrc_plmn_s *hplmn_ptr, /*!< HPLMN */
  lte_rrc_ehplmn_list_s *ehplmn_list_ptr, /*!< EHPLMN list */
  boolean rplmn_info_is_valid, /*!< Is registered RPLMN info available? */
  lte_rrc_plmn_s *rplmn_ptr, /*!< Registered PLMN */
  boolean req_plmn_is_valid, /*!< Is registered Req_PLMN info available? */
  lte_rrc_plmn_s *req_plmn_ptr, /*!< Req PLMN */
  unsigned int num_enh_req_plmns, /*!< Num of Enh Req PLMNs */
  lte_rrc_plmn_s *enh_req_plmn_list_ptr,/*!< Enhanced Req PLMN list */
  sys_network_selection_mode_e_type network_select_mode,
  lte_rrc_plmn_list_s *candidate_plmn_list_ptr /*!< Subset PLMN list from SIB1
                                                 that will be used for Camping
                                                 if cell is not barred*/
);

/* Reset duplex mode */
extern void lte_rrc_csp_reset_duplex_mode
(
  void
);

extern uint32 lte_rrc_csp_get_last_cfg_req_id(void);


#ifdef TEST_FRAMEWORK
#error code not present
#endif

extern boolean lte_rrc_get_primary_plmn
(
  lte_rrc_plmn_s *primary_plmn_ptr
);

extern boolean lte_rrc_csp_multi_cell_lock_enabled
(
 void
);

extern boolean lte_rrc_csp_cell_is_locked
(
  lte_earfcn_t earfcn,
  boolean pci_valid,
  lte_phy_cell_id_t pci
);

#endif /* LTE_RRC_CSP_H */

