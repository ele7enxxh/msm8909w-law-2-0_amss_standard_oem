/*!
  @file
  lte_rrc_plmn_searchi.h

  @brief
  Internal API for RRC PLMN_SEARCH Message Handler.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_plmn_searchi.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
05/13/11   vatsac   Reduced the min time required to search on LTE to 200ms 
11/18/09   vatsac   Initial version
===========================================================================*/

#ifndef LTE_RRC_PLMN_SEARCHI_H
#define LTE_RRC_PLMN_SEARCHI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_plmn_search.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>
#include "lte_rrc_csp.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_pendq.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_PLMN_SEARCH_MAX_START_STOP_TIME       20 /* ms */
#define LTE_RRC_PLMN_SEARCH_MAX_SUSPEND_RESUME_TIME   35 /* ms */

#define LTE_RRC_PLMN_SEARCH_LTE_MIN_GUARD_TIME       180 /* ms */
#define LTE_RRC_PLMN_SEARCH_GSM_MIN_GUARD_TIME       195 /* ms */
#define LTE_RRC_PLMN_SEARCH_WCDMA_MIN_GUARD_TIME     300 /* ms */
#define LTE_RRC_PLMN_SEARCH_WCDMA_MANUAL_MIN_GUARD_TIME     220 /* ms */
#define LTE_RRC_PLMN_SEARCH_TDSCDMA_MIN_GUARD_TIME   233 /* ms */

#define LTE_RRC_PLMN_SEARCH_MAX_CONTINOUS_IRAT_SKIP_COUNT 20

/*! @brief Data type for PLMN_SEARCH SM
*/
typedef struct
{
    lte_phy_cell_id_t         phy_cell_id;           /*!< Physical cell id */
    lte_earfcn_t              freq;                  /*!< Frequency */
    lte_rrc_plmn_list_s       plmn_list;             /*!< PLMN list from SIB1 */
    boolean                   plmns_are_reserved;           /*!< Reserved PLMNs in list */
    boolean                   reserved_plmn_cell_is_barred; /*!< Cell barred due to reserved PLMNs  */
    sys_csg_id_type           csg_id;                /*!< CSG id */
    sys_home_nodeb_name_type  henb_name;             /*!< HeNB Name */
    boolean                   hybrid_cell;           /*!< Hybrid_cell */
    boolean                   plmn_match_has_failed; /*!< No PLMNs Matched request */
    uint32                    plmn_index;            /*!< Matching PLMn index */
    uint16                    tac;                   /*!< Tracking area code */
    boolean                   scrit_has_failed;      /*!< S-criteria check */
    sys_signal_quality_e_type signal_quality;        /*!< Signal Quality */
    sys_signal_strength_type  signal_strength;       /*!< RSRP */
    boolean                   bw_is_invalid;         /*!< Invalid Bandwidth */
    lte_rrc_global_cell_id_t  cell_identity;         /*!< Global cell id (from SIB1) */
    lte_bandwidth_e           bandwidth;             /*!< System bandwidth */
    sys_pci_scan_signal_info_s_type pci_sig_info;    /*!< RSRP/RSRQ */
} lte_rrc_plmn_search_record_s;

#define LTE_RRC_PLMN_SEARCH_GUARD_TMRI_VAL 180000 /*!< Guard Timer in ms */

#define LTE_RRC_PLMN_SEARCH_ACQ_DB_TMRI_VAL 300000 /*!< ACQ DB Timer in ms */

#define LTE_RRC_PLMN_SEARCH_MANUAL_SEARCH_BAND_SCAN_SEP_TMRI_VAL 300000 /*!< Small ACQ DB Timer in ms */

#define LTE_RRC_PCI_PLMN_SEARCH_GUARD_TMRI_VAL 300000 /*!< Guard Timer in ms */

typedef struct
{
    lte_rrc_plmn_search_record_s found_plmn[SYS_PLMN_LIST_MAX_LENGTH]; /*!< Found LTE PLMNs List */
    uint32                       next_index;  /*!< Next free index in the list */
    uint32                       hplmn_index; /*!< HPLMN index in the list */
} lte_rrc_plmn_search_table_s;


/*! @brief Static Private context of PLMN_SEARCH SM
*/
typedef struct
{
    struct
    {
        /* Control variables */
        boolean                       l_to_l_disabled;  /*!< Enable intraLTE searches */
        boolean                       l_to_w_disabled;  /*!< Enable LTE to W searches */
        boolean                       l_to_tds_disabled;  /*!< Enable LTE to W searches */
        boolean                       l_to_g_disabled;  /*!< Enable LTE to G searches */
        boolean                       gw_to_l_disabled;  /*!< Enable IRAT to LTE searches */
        uint64                        guard_tmri_val; /*!< Guard Timer Value */
        uint64                        acq_db_tmri_val; /*!< ACQ DB Timer Scan Value */
        uint64                        msearch_bscan_sep_tmri_val; /*!< Manual Search Band Scan Seperator Timer Value */
    } control;

    /* Session variables */
    sys_rat_pri_list_info_s_type      ordered_rat_search_list; /*!< RAT search order */
  
    sys_network_selection_mode_e_type network_select_mode; /*!< network select mode */

    sys_service_search_e_type         search_type; /*!< service search */

    sys_scan_scope_e_type             scan_scope;

    sys_plmn_rat_list_s_type          plmn_rat_list; /*!< PLMN RAT List */

    sys_manual_search_timer_value_type partial_search_tmrs;/*!< Stored partial search tmrs given from NAS*/

    boolean                           rplmn_info_is_valid; /*!< Is registered PLMN info available? */

    lte_rrc_plmn_s                    rplmn; /*!< Registered PLMN */

    boolean                           hplmn_info_is_valid; /*!< Is HPLMN info available?  */

    lte_rrc_plmn_s                    hplmn; /*!< HPLMN */

    lte_rrc_ehplmn_list_s             ehplmn_list; /*!< EHPLMN list */

    lte_rrc_plmn_list_s               eplmn_list; /*!< EPLMN list  */

    lte_rrc_tracking_area_list_s      forbidden_ta_list; /*!< Forbidden TA list */

    lte_rrc_plmn_search_table_s       found_plmn_table; /*!< Table to keep track of LTE PLMN scan results */

    detected_frequency_list_type      detected_frequency_list; /*!< Detected Frequency List */

    detected_csg_cell_list_type       detected_csg_cell_list; /*!< Detected Frequency List */

    detected_macro_cell_list_type     detected_macro_cell_list; /*!< Detected macro cell list */

    boolean                           irat_to_lte_search_partial_done; /*!< IRAT To LTE search pending */

    sys_sys_mode_e_type               active_rat; /*!< Active RAT */

    /* State variables. Initialize in SM entry. */
    lte_rrc_tmr_s                     guard_tmr; /*!< Guard Timer */

    lte_rrc_tmr_s                     acq_db_tmr; /*!< ACQ DB Timer */

    boolean                           acq_db_scan_only; /*!< ACQ DB Scan only */

    lte_rrc_tmr_s                     msearch_bscan_sep_tmr; /*!< Manual Search Band Scan Seperator Timer */

    lte_rrc_tmr_s                     first_plmn_tmr;/*!< First Partial Search Timer*/
  
    lte_rrc_tmr_s                     more_plmn_tmr;/*!< Perodic Partial Search Timer*/

    lte_rrc_csp_acq_db_s             *acq_db_ptr;  /*!< Ptr to ACQ DB from CSP */

    /* Temporary variables. Initialize before use. */
    char                              str_buf[1024];
    
    sys_lte_band_mask_e_type              previous_seach_lte_band_cap; /*LTE Band capability of last search*/
} lte_rrc_plmn_search_static_data_s;


/*! @brief Dynamic Private context of PLMN_SEARCH SM
*/
typedef struct
{
    /* Session variables */
    uint32                            trans_id; /*!< Transaction ID */

    uint8                             num_lte_bands; /*!< Num LTE bands */

    sys_sband_lte_e_type              lte_bands[LTE_CPHY_BPLMN_MAX_BANDS]; /*!< LTE bands */

    lte_cphy_bw_mask_t                lte_bw_masks[LTE_CPHY_BPLMN_MAX_BANDS]; /*!< Bandwidth mask for the above band */

    uint8                             num_chans; /*!< number of frequency included in this request */

    lte_cphy_bplmn_chan_s             chans[LTE_CPHY_BPLMN_MAX_FREQS]; /*!< the list of channels to be searched for PLMN */

    lte_rrc_plmn_search_status_e      search_status; /*!< Search status */

    sys_detailed_plmn_list_s_type     found_plmn_rat_list; /*!< found PLMN RAT list returned to NAS */

    boolean                           continue_search_after_cphy_bplmn_stop; /*!< continue search beyond stop */

    boolean                           irat_hplmn_found; /*!< IRAT HPLMN found */

    uint32                            old_trans_id; /*!< Aborted Search Transaction ID */

    sys_network_selection_mode_e_type old_network_select_mode; /*!< Aborted Search network select mode */

    sys_service_search_e_type         old_search_type; /*!< Aborted Search service search */

    lte_rrc_plmn_search_status_e      old_search_status; /*!< Aborted Search status */

    boolean                           pend_search_abort_till_mode_change_cnfi; /*!< pending flag */

    boolean                           ml1_revoke_cnfi_received; /*!< pending flag */

    boolean                           ml1_revoked; /*!< ML1 Revoked by other RRC module */

    boolean                           ml1_scheduled_search; /*!< ML1 scheduling PLMN search time slots */

    uint32                            irat_skip_count; /*!< No of times skipped due to insufficient time from ML1 */

    boolean                           rats_reordered; /*!< RATs are reordered */

    uint32                            num_irat_aborts; /*!< Num IRAT aborts sent */

    boolean                           nas_search_aborted; /*!< NAS search aborted */

    uint64                            expected_slot_end_time; /*!< Expected IRAT search slot end time */

    boolean                           stop_event_logged; /*!< Stop event logged earlier */

    lte_rrc_pendq_queue_item_s       *pended_cphy_abort_cnf_ptr; /*!< Pointer to cphy_abort_cnf on the PendQ */

    lte_rrc_mode_change_e             requested_mode; /*!< Holds the mode change request details */

    uint8                             num_no_fp_rat_list; /*!< Number of RATs with no FP info */

    sys_sys_mode_e_type               no_fp_rat_list[SYS_SYS_MODE_MAX]; /*!< List of RATs with no FP info */

    boolean                           csg_search; /* CSG Search */

    source_cell_info_type             serving_cell_info; /* Serving Cell Info for ASF search */

    boolean                           skip_searches_in_same_slot; /* Flag to skip subsequent L2X search attempts in same time slot */

    /* Temporary variables. Initialize before use. */
    sys_priority_list_info_s_type     temp_rat_info[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    boolean                           matched_rat_index[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    uint8                             num_black_freqs;
    lte_cphy_bplmn_freq_s             black_freqs[LTE_CPHY_BPLMN_MAX_FREQS];
    lte_rrc_plmn_list_s               candidate_plmn_list;
    sys_detailed_plmn_list_s_type     temp_found_plmn_rat_list;
    sys_band_mask_type                geran_band_pref;/*Will save geran band preference only when bst_bands_filter 
                                                        set for GERAN*/
    boolean                           bg_to_fg_cvrsn_done;

    boolean                           pci_scan; /* Manual pci Search */

    sys_pci_scan_plmn_list_s_type     found_pci_plmn_list;

} lte_rrc_plmn_search_dynamic_data_s;


/*! @brief Private data for PLMN Search
*/
typedef struct
{
    lte_rrc_plmn_search_static_data_s  *s_ptr; /*!< Static data pointer */
    lte_rrc_plmn_search_dynamic_data_s *d_ptr; /*!< Dynamic data pointer */
} lte_rrc_plmn_search_data_s;


#endif /* LTE_RRC_PLMN_SEARCHI_H */

