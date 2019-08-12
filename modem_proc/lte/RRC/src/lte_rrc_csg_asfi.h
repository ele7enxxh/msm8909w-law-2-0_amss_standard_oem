/*!
  @file
  lte_rrc_csg_asfi.h

  @brief
  Internal API for RRC CSG_ASF Message Handler.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_csg_asfi.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
05/13/11   vatsac   Reduced the min time required to search on LTE to 200ms 
11/18/09   vatsac   Initial version
===========================================================================*/

#ifndef LTE_RRC_CSG_ASFI_H
#define LTE_RRC_CSG_ASFI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_csg_asf.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>
#include "lte_rrc_utils.h"
#include "lte_rrc_csg_asf.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_CSG_ASF_TMRI_VAL 300000 /*!< ASF Timer in ms */

#define LTE_RRC_CSG_PROXIMITY_TMRI_VAL 5000 /*!< ASF Timer in ms */

#define LTE_RRC_CSG_ASF_INVALID_FP_IDX          0xFF

#define LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES       10
#define LTE_RRC_CSG_ASF_MAX_FP_MACROS_PER_CSG    10

#define LTE_RRC_CSG_ASF_MAX_MACRO_DB_ENTRIES    100
#define LTE_RRC_CSG_ASF_MAX_FP_CSGS_PER_MACRO    10

#define LTE_RRC_CSG_ASF_MAX_PCI_RANGE_DB_ENTRIES 10

#define LTE_RRC_CSG_ASF_24_HOURS                 (24*60*60*1000)

/*! @brief Data type for CSG_ASF_DB_RECORD
*/
typedef struct
{
    lte_earfcn_t              freq;          /*!< Frequency */
    lte_rrc_plmn_list_s       plmn_list;     /*!< PLMN list from SIB1 */
    sys_csg_id_type           csg_id;        /*!< CSG id */
    lte_phy_cell_id_t         phy_cell_id;   /*!< Physical cell id */
    lte_bandwidth_e           bandwidth;     /*!< System bandwidth */
    lte_rrc_global_cell_id_t  cell_identity; /*!< Global cell id (from SIB1) */
    uint8                     num_fp_macros; /*!< Num of macros fingerprinted */
    uint8                     fp_macro_idx[LTE_RRC_CSG_ASF_MAX_FP_MACROS_PER_CSG]; /*!< Fingerprinted macros indexes */
    boolean                   is_valid;      /*!< Record is valid */
} lte_rrc_csg_asf_db_record_s;


/*! @brief Data type for CSG_MACRO_DB_RECORD
*/
typedef struct
{
    sys_radio_access_tech_e_type rat;           /*!< RAT */
    lte_earfcn_t                 freq;          /*!< Frequency */
    sys_plmn_id_s_type           plmn;          /*!< PLMN list from SIB1 */
    lte_rrc_global_cell_id_t     cell_identity; /*!< Global cell id (from SIB1) */
    uint8                        num_fp_csgs;   /*!< Num of CSGs fingerprinted */
    uint8                        fp_csg_idx[LTE_RRC_CSG_ASF_MAX_FP_CSGS_PER_MACRO]; /*!< Fingerprinted CSG indexes */
    boolean                      is_valid;      /*!< Record is valid */
} lte_rrc_csg_macro_db_record_s;


/*! @brief Data type for CSG_ASF_DB
*/
typedef struct
{
    lte_rrc_csg_asf_db_record_s  record[LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES]; /*!< CSG Info */
    uint8                        mru_idx[LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES];  /*!< Most recently used entries in the list */
} lte_rrc_csg_asf_db_s;


/*! @brief Data type for CSG_MACRO_DB
*/
typedef struct
{
    lte_rrc_csg_macro_db_record_s  record[LTE_RRC_CSG_ASF_MAX_MACRO_DB_ENTRIES]; /*!< CSG Info */
    uint8                          mru_idx[LTE_RRC_CSG_ASF_MAX_MACRO_DB_ENTRIES];  /*!< Most recently used entries in the list */
} lte_rrc_csg_macro_db_s;

/*! @brief Data type for CSG_ASF_DB_RECORD
*/
typedef struct
{
    lte_efs_earfcn_t          freq;          /*!< Frequency */
    lte_rrc_plmn_list_s       plmn_list;     /*!< PLMN list from SIB1 */
    sys_csg_id_type           csg_id;        /*!< CSG id */
    lte_phy_cell_id_t         phy_cell_id;   /*!< Physical cell id */
    lte_bandwidth_e           bandwidth;     /*!< System bandwidth */
    lte_rrc_global_cell_id_t  cell_identity; /*!< Global cell id (from SIB1) */
    uint8                     num_fp_macros; /*!< Num of macros fingerprinted */
    uint8                     fp_macro_idx[LTE_RRC_CSG_ASF_MAX_FP_MACROS_PER_CSG]; /*!< Fingerprinted macros indexes */
    boolean                   is_valid;      /*!< Record is valid */
} lte_rrc_csg_asf_efs_db_record_s;


/*! @brief Data type for CSG_ASF_DB
*/
typedef struct
{
    lte_rrc_csg_asf_efs_db_record_s  record[LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES]; /*!< CSG Info */
    uint8                            mru_idx[LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES];  /*!< Most recently used entries in the list */
} lte_rrc_csg_efs_asf_db_s;

/*! @brief Data type for CSG_MACRO_DB_RECORD
*/
typedef struct
{
    sys_radio_access_tech_e_type rat;           /*!< RAT */
    lte_efs_earfcn_t             freq;          /*!< Frequency */
    sys_plmn_id_s_type           plmn;          /*!< PLMN list from SIB1 */
    lte_rrc_global_cell_id_t     cell_identity; /*!< Global cell id (from SIB1) */
    uint8                        num_fp_csgs;   /*!< Num of CSGs fingerprinted */
    uint8                        fp_csg_idx[LTE_RRC_CSG_ASF_MAX_FP_CSGS_PER_MACRO]; /*!< Fingerprinted CSG indexes */
    boolean                      is_valid;      /*!< Record is valid */
} lte_rrc_csg_macro_esf_db_record_s;


/*! @brief Data type for CSG_MACRO_DB
*/
typedef struct
{
    lte_rrc_csg_macro_esf_db_record_s  record[LTE_RRC_CSG_ASF_MAX_MACRO_DB_ENTRIES]; /*!< CSG Info */
    uint8                          mru_idx[LTE_RRC_CSG_ASF_MAX_MACRO_DB_ENTRIES];  /*!< Most recently used entries in the list */
} lte_rrc_csg_efs_macro_db_s;

typedef enum
{
    ENTERING,
    LEAVING
} lte_rrc_csg_proximity_record_status_e;


/*! @brief Data type for PCI_RANGE_DB_RECORD
*/
typedef struct
{
    lte_rrc_plmn_s               plmn;          /*!< Primary PLMN */
    lte_earfcn_t                 earfcn;        /*!< Frequency */
    lte_phy_cell_id_and_range_s  pci_range;     /*!< PCI Range */
    uint64                       timestamp;     /*!< Timestamp */
} lte_rrc_csg_asf_pci_range_db_record_s;


/*! @brief Data type for CSG_PCI_RANGE_DB
*/
typedef struct
{
    lte_rrc_csg_asf_pci_range_db_record_s  record[LTE_RRC_CSG_ASF_MAX_PCI_RANGE_DB_ENTRIES]; /*!< PCI Range Info */
    uint8                                  mru_idx[LTE_RRC_CSG_ASF_MAX_PCI_RANGE_DB_ENTRIES];  /*!< Most recently used entries in the list */
} lte_rrc_csg_asf_pci_range_db_s;


/*! @brief Data type for CSG_CONN_PROXIMITY_RECORD
*/
typedef struct
{
    sys_radio_access_tech_e_type          rat;   /*!< RAT */
    lte_earfcn_t                          freq;  /*!< Frequency */
    lte_rrc_csg_proximity_record_status_e status;/*!< Status */
    boolean                               msg_sent; /*!< Message sent indicator */
} lte_rrc_csg_conn_proximity_record_s;


/*! @brief Data type for CSG_CONN_PROXIMITY_LIST
*/
typedef struct
{
    lte_rrc_csg_conn_proximity_record_s record[2*LTE_RRC_CSG_ASF_MAX_CSG_DB_ENTRIES]; /*!< CSG Info */
    uint8                               num_records; /*!< Number of records */
} lte_rrc_csg_conn_proximity_list_s;


/*! @brief Static Private context of CSG_ASF SM
*/
typedef struct
{
    struct
    {
        /* Control variables */
        boolean                       l_csg_camping_disabled;  /*!< Enable intraLTE searches */
        boolean                       l_asf_disabled;  /*!< Enable LTE to W searches */
        boolean                       l_to_w_asf_disabled;  /*!< Enable LTE to W searches */
        boolean                       w_to_l_asf_disabled;  /*!< Enable LTE to G searches */
        boolean                       l_tdd_csg_disabled; /*!< Enable TDD CSG camping and searches */
        boolean                       l_hybrid_disabled; /*!< Enable Hybrid cell camping */
        uint64                        asf_tmri_val; /*!< Guard Timer Value */
    } control;

    /* Session variables */
    lte_rrc_csg_list_s                csg_whitelist; /*!< CSG whitelist */

    boolean                           rplmn_info_is_valid; /*!< Is registered PLMN info available? */

    lte_rrc_plmn_s                    rplmn; /*!< Registered PLMN */

    boolean                           hplmn_info_is_valid; /*!< Is HPLMN info available?  */

    lte_rrc_plmn_s                    hplmn; /*!< HPLMN */

    lte_rrc_ehplmn_list_s               ehplmn_list; /*!< EHPLMN list */

    lte_rrc_plmn_list_s               eplmn_list; /*!< EPLMN list  */

    lte_rrc_tracking_area_list_s      forbidden_ta_list; /*!< Forbidden TA list */

    lte_rrc_csg_asf_db_s              db; /*!< CSG DB */

    lte_rrc_csg_macro_db_s            macro_db; /* Macro DB */

    lte_rrc_csg_asf_db_record_s       source_cell_info; /*!< Source cell info on cell change */

    sys_radio_access_tech_e_type      source_cell_rat; /*!< Source cell RAT */

    lte_rrc_csg_asf_pci_range_db_s    pci_range_db; /*!< PLMN CSG PCI Range DB */

    /* State variables. Initialize in SM entry. */
    lte_rrc_tmr_s                     csg_asf_tmr; /*!< CSG ASF Timer */

    /* Temporary variables. Initialize before use. */
    char                              str_buf[1024];

} lte_rrc_csg_asf_static_data_s;


/*! @brief Dynamic Private context of CSG_ASF SM
*/
typedef struct
{
    /* Session variables */
    sys_rat_pri_list_info_s_type      rat_pri_list; /*!< RAT priority list */

    sys_plmn_rat_list_s_type          plmn_rat_list; /*!< PLMN RAT List */

    lte_rrc_plmn_search_status_e      search_status; /*!< Search status */

    sys_detailed_plmn_list_s_type     found_plmn_rat_list; /*!< found PLMN RAT list returned to NAS */

    lte_rrc_plmn_s                    camped_plmn; /*!< Camped PLMN */

    uint16                            camped_tac; /*!< Camped TAC */

    lte_rrc_global_cell_id_t          camped_cell_id; /*!< Global Cell Id */

    lte_phy_cell_id_t                 camped_pci;  /*!< Physical cell id */

    lte_earfcn_t                      camped_earfcn;  /*!< DL Frequency */

    sys_csg_id_type                   camped_csg_id;

    boolean                           asf_timer_expired; /*!< ASF timer expired flag */
    
    boolean                           no_fp_on_l; /*!< No FP info on LTE */

    boolean                           no_fp_on_w; /*!< No FP info on WCDMA */
    
    boolean                           no_fp_on_t; /*!< No FP info on TDS */

    lte_rrc_csg_conn_proximity_list_s conn_proximity_list; /*!< Connected mode proximity list */

    source_cell_info_type             csg_cells[LTE_IRAT_MAX_MACRO_CELLS]; /*!< CSG cells list */

    boolean                           report_proximity_eutra; /*!< EUTRA Prox Configured */

    boolean                           report_proximity_utra; /*!< UTRA Prox Configured */

    /* Temporary variables. Initialize before use. */
    sys_priority_list_info_s_type     temp_rat_info[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    boolean                           matched_rat_index[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    uint8                             num_black_freqs;
    lte_cphy_bplmn_freq_s             black_freqs[LTE_CPHY_BPLMN_MAX_FREQS];
    lte_rrc_plmn_list_s               temp_plmn_list;
    lte_cphy_meas_csg_info_s          w_csg_freq_cell_list; /*! W CSGs */
    lte_cphy_meas_lte_csg_info_s      l_csg_freq_cell_list; /*! LTE CSGs */
    lte_cphy_meas_eutra_freq_pci_range_s csg_pci_range[LTE_RRC_CSG_ASF_MAX_PCI_RANGE_DB_ENTRIES];

} lte_rrc_csg_asf_dynamic_data_s;


/*! @brief Private data for CSG ASF
*/
typedef struct
{
    lte_rrc_csg_asf_static_data_s  *s_ptr; /*!< Static data pointer */
    lte_rrc_csg_asf_dynamic_data_s *d_ptr; /*!< Dynamic data pointer */
} lte_rrc_csg_asf_data_s;


#endif /* LTE_RRC_CSG_ASFI_H */

