/*!
  @file
  lte_rrc_cspi.h

  @brief
  Private data for CSP SM.

*/

/*===========================================================================

  Copyright (c) 2008 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_cspi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/11   vatsac  Added support for iterative band scan
05/27/11   vatsac  Added support for ims emergency from Rel9 SIB1
04/13/11   sk      Added support for storing deactivate reason especially for
                   the case when the reason is LPM.
03/23/11   sureshs Added current and previous modes
12/08/10   vatsac  Comapct Acq Db usage
12/06/10   vatsac  EFS controlled system scan disabling
11/16/10   sureshs Added deact_during_reest flag
06/24/10   np      Fill SIB-8 Available in Service Indication to NAS
03/22/10   sureshs Split data into static and dynamic parts
02/24/10   sureshs Added new scan type for IRAT To LTE Full redirection
12/18/09   sureshs Moved nas req info type to external header file
11/23/09   sureshs Moved acq db type to external header file
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
09/18/09   sureshs Added IRAT info to NAS requested info
06/09/09   sureshs Added scan type LTE_RRC_CSP_SCAN_CELL_SEL_RLF
05/28/09   vatsac  Added LTE_RRC_CSP_SCAN_CELL_SEL_INTRA_LTE_REDIR scan type
05/27/09   sureshs Data structures to store system scan frequencies
04/15/09   sureshs Added CSG indication field to SIB1 info
04/06/09   sureshs Added fields for OOS handling
03/18/09   sureshs Added reselection_is_stopped flag
03/06/09   sureshs Changes for Dec spec compliance
02/18/09   sureshs Added num_tried_cells to acquision struct
01/23/09   sureshs Removed cell reservation extension field from SIB1 info
01/14/09   sureshs Added found plmn type and RSSI as needed
01/09/09   sureshs Added freq-is_barred field to sib1_info
01/05/09   sureshs Added serv_cell_is_weak_during_resel flag
12/08/08   sureshs Added reslection scan type
12/08/08   sureshs Removed EHPLMN valid flag
11/18/08   sureshs Made EHPLMN into a list
11/12/08   sureshs Added entry_is_occupied array in acq db
10/30/08   sureshs Support for System Update
10/29/08   sureshs Support for EHPLMN and EPLMN list
10/28/08   sureshs Support for selected PLMN
10/24/08   sureshs Added found PLMN structures
10/23/08   sureshs Changes for cell barring logic
10/23/08   sureshs Changes for cell barring logic
10/20/08   sureshs Changes from NAS interface meeting
10/16/08   sureshs Added field forbidden TA logic
10/16/08   sureshs Incorporated changes to NAS interface after meeting
10/14/08   sureshs Changes for using new LTE band enums
10/09/08   sureshs Removed MIB field
10/08/08   sureshs Cleanup after code review
10/07/08   sureshs Removed mib and cp info
10/06/08   sureshs Removed entry_valid field
10/06/08   sureshs Simplified mode information
10/04/08   sureshs Cleanup
10/03/08   sureshs Moved systems array from stack to private data
10/03/08   sureshs Changed num_entries from boolean to uint32 and last tried
                     entry to int32
7/29/08    vramacha  Initial Version
===========================================================================*/

#ifndef LTE_RRC_CSPI_H
#define LTE_RRC_CSPI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_int_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_cphy_msg.h"
#include "lte.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_csp.h"
#include "policyman.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_CSP_MAX_ACQ_CANDIDATE_LIST_ENTRIES  (LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS*LTE_CPHY_BANDSCAN_NUM_CANDIDATES)
// hst earfcn + acq db + interfrequency neighbors
#define LTE_RRC_CSP_MAX_SYSTEM_SCAN_EARFCN_ENTRIES (LTE_RRC_CSP_MAX_HST_EARFCN + LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES + LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES)

/*! @brief Structure to store the camping status
*/
typedef struct
{
    /*! Current camped status of the cell */
  lte_rrc_camped_status_e camped_status;

  /*! Current cell data */
    lte_rrc_csp_acq_entry_s current_cell;

  /*! Number of Tx antennas */
  uint8 num_tx_antennas;

  /* is registration possible on camped cell */
  boolean reg_not_allowed;
    
  /*! HeNB Name for CSG cells */
  sys_home_nodeb_name_type hnb_name;
}lte_rrc_csp_camp_info_s;

/*! @brief Enum to represent various scan types in CSP
*/
typedef enum
{
    /*! Cell Selection scan triggered by NAS request */
    LTE_RRC_CSP_SCAN_CELL_SEL_NAS,                

    /*! Cell Selection scan due to Connected to Idle transition */
    LTE_RRC_CSP_SCAN_CELL_SEL_CONN_TO_IDLE, 

  /*! Cell Selection scan due to Reselection */
    LTE_RRC_CSP_SCAN_CELL_SEL_RESELECTION,

  /*! Cell Selection scan due to intraLTE Redirection */
  LTE_RRC_CSP_SCAN_CELL_SEL_INTRA_LTE_REDIR,

  /*! Cell Selection scan due to RLF */
  LTE_RRC_CSP_SCAN_CELL_SEL_RLF,

  /*! Cell Selection scan due to Connection release during RLF */
  LTE_RRC_CSP_SCAN_CELL_SEL_CONN_TO_IDLE_CRE_FAILURE,

  /*! Cell Selection scan due to IRAT to LTE reselection */
  LTE_RRC_CSP_SCAN_CELL_SEL_IRAT_TO_LTE_RESEL,

  /*! Cell Selection scan due to IRAT to LTE List redirection */
  LTE_RRC_CSP_SCAN_CELL_SEL_IRAT_TO_LTE_LIST_REDIR,

  /*! Cell Selection scan due to IRAT to LTE Full redirection */
  LTE_RRC_CSP_SCAN_CELL_SEL_IRAT_TO_LTE_FULL_REDIR,

  /*! Cell Selection scan triggered internally by CSP */
  LTE_RRC_CSP_SCAN_CELL_SEL_CSP,

}lte_rrc_csp_scan_type_e;

typedef enum
{
  /*! ACQ list contains none of the below */
  LTE_RRC_CSP_ACQ_CANDIDATE_LIST_NONE = 0,

  /*! ACQ list contains systems scan entries */
  LTE_RRC_CSP_ACQ_CANDIDATE_LIST_SYSTEM_SCAN,

  /*! ACQ list contains band scan results */
  LTE_RRC_CSP_ACQ_CANDIDATE_LIST_BAND_SCAN,

} lte_rrc_csp_acq_candidate_list_type_e;

/*! @brief Enum  to represent various barring causes.
*/
typedef enum
{
  /*! Cell Barring due to evaluation of SIB1 params*/
  LTE_RRC_CSP_BAR_CAUSE_MANDATORY,                

  /*! Cell Barring due to SIB read issues */
  LTE_RRC_CSP_BAR_CAUSE_SIB_ERROR, 

  /*! Cell Barring for back off due to internal errors Eg: IM3 backoff */
  LTE_RRC_CSP_BAR_CAUSE_INTERNAL_ERROR,

  /*! Cell Barring due to higher layer requests */
  LTE_RRC_CSP_BAR_CAUSE_NAS,

  /*! Cell Barring due to CEP conn control */
  LTE_RRC_CSP_BAR_CAUSE_CEP_CONN_CTRL,

  /*! Cell Barring due to SIB error during reslectionsl */
  LTE_RRC_CSP_BAR_CAUSE_SHORT_BAR_TIME,  

  /*! Cell Barring due to Repeated RLF */
  LTE_RRC_CSP_BAR_CAUSE_RLF_SHORT_BAR_TIME, 

  /*! Cell Barring Max */
  LTE_RRC_CSP_BAR_CAUSE_MAX
}lte_rrc_csp_barring_cause_e;

/*! @brief Enum  to represent various prioritization basis
*/
typedef enum
{
  /*! ACQ list contains none of the below */
  LTE_RRC_CSP_SORT_BY_NONE = 0,

  /*! ACQ list contains systems scan entries */
  LTE_RRC_CSP_SORT_BY_PLMN_MATCH,

  /*! ACQ list contains band scan results */
  LTE_RRC_CSP_SORT_BY_BAND_PRIORITY,
}lte_rrc_csp_sort_by_reason_e;

/*! @brief Enum  to represent various prioritization reasons
*/
typedef enum
{
  /*! prioritize by band priority EFS */
  LTE_RRC_CSP_PRIORITIZE_BAND_PRIO_EFS = 0,
  LTE_RRC_CSP_PRIORITIZE_BAND_PRIO_MRU = 1,
  /*! Prioritize Max */
  LTE_RRC_CSP_PRIORITIZE_CAUSE_MAX
}lte_rrc_csp_priority_scan_reason;

/*! @brief Structure to hold band scan results
*/
typedef struct
{
    /*! Number of entries in the list. 0 indicates band list in empty */
    int32 num_entries;

    /*! Index that was last tried. -1 indicates that this is a fresh list 
        and none of the entries have been tried. If this index is equal to 
            "num_entries-1" then all entries have been tried 
      There is a change in meaning with the feature of sending multiple bands
      in a band scan request. Now, this variable is the index of the last 
      band in the group of bands that has been tried. In other words, if it is
      4, then indices tried were 2,3,4 if 3 bands were sent in the band scan
      request. num_sent_bands below would then be 3. The bands sent would be
      (last_tried_index - (num_sent_bands - 1)) to last_tried_index.
      The condition for all entries having been tried is still index equal to 
      num_entries-1.
      */
  int32 last_tried_index;

  /*! Number of bands sent in last band scan request */
  int32 num_sent_bands;

  /*! Band information */
  sys_sband_lte_e_type bands[SYS_SBAND_LTE_EUTRAN_BAND_MAX];

  /*! the bandwidth masks for each band in the above list */
  lte_cphy_bw_mask_t bw_masks[SYS_SBAND_LTE_EUTRAN_BAND_MAX];

}lte_rrc_csp_band_scan_list_s;

/*! @brief This structure to hold the Acq list during cell selection process
*/
typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn;

    /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range: 
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;

  /*! System Bandwidth */
    lte_bandwidth_e bandwidth;

  /*! Indicates the number of cells in the blacklist Range: 0..4 */
  uint8 num_black_cells;

  /*! Indicates the number of cells tried for a particular frequency, need
      not be the same as num_black_cells because barred cells are not added
      to blacklist */
  uint8 num_tried_cells;

  /*! Energy, if known */
  int16 energy;

  /*! Cell blacklist; L1 must try to acquire a cell not in this list; used for
      e.g. barred cells, can be empty */
  lte_phy_cell_id_t blacklist[LTE_CPHY_MAX_ACQ_BLACKLISTED_CELLS];

  /*! If PCI is provided, camp on that cell only */
  boolean is_pci_valid; 

  uint8                 pci_count;
  /*! PCI */
  lte_phy_cell_id_t pci[LTE_CPHY_ACQ_MAX_PCI];

  /* Did acquisition succeed on this entry? */
  boolean acq_success;

}lte_rrc_csp_acq_candidate_list_entry_s;

/*! @brief This structure holds the Acq list.
*/
typedef struct
{
    /*! Number of entries in the list. 0 indicates Acq list is empty */
    int32 num_entries;

    /*! Index that was last tried. -1 indicates that this is a fresh list 
        and none of the entries have been tried. If this index is equal to 
            "num_entries-1" then all entries have been tried */
  int32 last_tried_index;

  /*! Acquisition list prepared wither from System scan results or Band scan 
        results */
  lte_rrc_csp_acq_candidate_list_entry_s 
    acq_list[LTE_RRC_CSP_MAX_ACQ_CANDIDATE_LIST_ENTRIES];
}lte_rrc_csp_acq_candidate_list_s;

/*! @brief This structure holds the candidate' cell's data
*/
typedef struct
{
    /*! Band to which cell belongs */
    sys_sband_lte_e_type band;

  /*! Cell identity */
    lte_rrc_cell_id_s cell_id;

    /*! Bandwidth from MIB from Acq Cnf */
  lte_bandwidth_e bandwidth;

  /*! Energy, if known */
  int16 energy;

  /*! Number of Tx antennas */
  uint8 num_tx_antennas;

  /*! Is the cell barred */
  boolean cell_barred;  
}lte_rrc_csp_candidate_cell_s;

/*! @brief This structure holds the data that is needed during a particular
    scan.
*/
typedef struct
{
    /*! Holds the current scan type. This field identifies what type of scan CSP
         is currently performing */
  lte_rrc_csp_scan_type_e scan_type;

    /*! Last sent CPHY trans id */
    uint8 last_cphy_trans_id;

    /*! Band scan list */
    lte_rrc_csp_band_scan_list_s band_scan_list;

    /*! Acq candidate list type */
    lte_rrc_csp_acq_candidate_list_type_e acq_candidate_list_type;

    /*! Acq candidate list. This will be build either from System Scan results
         or band scan results */
    lte_rrc_csp_acq_candidate_list_s acq_candidate_list;

    /*! Candidate cell */
  lte_rrc_csp_candidate_cell_s candidate_cell;

  /*! List of acceptable PLMNs found so far */
  lte_rrc_found_plmn_list_s found_plmn_list;

  /*! attempt acquisition on a better cell if indicated by ML1*/
  boolean acq_on_better_cell;  

  /*! ML1 timing info to be included only for acq on conn rel*/
  uint64 ref_time;
  /*! priority scan in progress. Perform non priority scan at the end of priority scan*/
  boolean priority_scan;  
} lte_rrc_csp_current_scan_info_s;

/*! @brief This structure holds the data gathered from reading SIB1
*/
typedef struct
{
  /*! PLMN list from SIB1 - temporary variable*/
  lte_rrc_plmn_list_s sib1_plmn_list;
  /*! Reservation status for each entry in sib1_plmn_list - temporary variable*/
  lte_rrc_csp_plmn_res_status_e plmn_res_status[LTE_RRC_MAX_PLMN_LIST_SIZE];
  /*! Cell barring status from SIB1 */
  lte_rrc_osys_SystemInformationBlockType1_cellAccessRelatedInfo_cellBarred cell_barred;  
  /*! Tracking area code from SIB1 */
  uint16 tac;
  /*! Is reselection not allowed to another cell of the same frequency if this
      cell is barred? */
  boolean freq_is_barred;
  /*! CSG indication */
  boolean csg_indication;
  /*! CSG Id of the cell */
  sys_csg_id_type csg_id;
  /*! SIB 8 scheduled? */
  boolean sib8_scheduled;
  /*! IMS Emergency Support enabled? */
  boolean ims_emergency_support_enabled;
  /*! To save list of overlap frequencies recieved in SIB1*/
  lte_rrc_earfcn_list new_dl_freq_list;
} lte_rrc_sib1_info_s;

typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn;

  /*! PCI */
  lte_phy_cell_id_t pci;

}lte_rrc_csp_pci_lock_s;

typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn1;

  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn2;

}lte_rrc_csp_earfcn_lock_s;


typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  lte_efs_earfcn_t  dl_earfcn;

  /*! PCI */
  lte_phy_cell_id_t pci;

}lte_rrc_csp_efs_pci_lock_s;

typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  lte_efs_earfcn_t  dl_earfcn1;

  /*! 0..65535 (allowing for expansion) */
  lte_efs_earfcn_t  dl_earfcn2;

}lte_rrc_csp_efs_earfcn_lock_s;

/*! @brief This structure defines the contents of each entry 
           in the Acq DB as stored in EFS
*/
typedef struct
{
  /*! Numper of PLMNs from SIB1 */
  uint8 num_plmns;

  /*! PLMN List from SIB1 */
  sys_plmn_id_s_type plmn[LTE_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE];

  /*! DL Center frequency */
  lte_efs_earfcn_t dl_earfcn;          

  /*! Band */
  lte_efs_band_enum_t band;            

  /*! System Bandwidth */
  lte_bandwidth_e bandwidth;

  /*! Physical cell id of last camped cell */
  lte_phy_cell_id_t phy_cell_id;   

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;
}lte_rrc_csp_efs_acq_entry_s;

/*! @brief CSP EFS Acq DB structure
*/
typedef struct
{
  /*! Acq DB Format Version */
  uint8 version;

  /*! Indicates the number of items stored in acq list order. If value is 
   *  LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is 
   *  full and an old entry needs to be cleared to make room for the new one 
   */
  uint8 acq_db_count;

  /*! Acquisition Database */
  lte_rrc_csp_efs_acq_entry_s efs_db[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint8 acq_db_index[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];
}lte_rrc_csp_efs_acq_db_s;

/*! @brief This structure defines the contents of each entry 
           in the Acq DB V2 as stored in V2 EFS
*/
typedef struct
{
  /*! Numper of PLMNs from SIB1 */
  uint8 num_plmns;

  /*! PLMN List from SIB1 */
  sys_plmn_id_s_type plmn[LTE_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE];

  /*! DL Center frequency */
  lte_earfcn_t dl_earfcn;

  /*! Band */
  lte_efs_extnd_band_enum_t band;

  /*! System Bandwidth */
  lte_bandwidth_e bandwidth;

  /*! Physical cell id of last camped cell */
  lte_phy_cell_id_t phy_cell_id;

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;
}lte_rrc_csp_efs_acq_entry_v2_s;

/*! @brief CSP EFS Acq DB V2 structure
*/
typedef struct
{
  /*! Acq DB Format Version */
  uint8 version;

  /*! Indicates the number of items stored in acq list order. If value is 
   *  LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is 
   *  full and an old entry needs to be cleared to make room for the new one 
   */
  uint8 acq_db_count;

  /*! Acquisition Database */
  lte_rrc_csp_efs_acq_entry_v2_s efs_db[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint8 acq_db_index[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[LTE_RRC_CSP_MAX_ACQ_DB_ENTRIES];
}lte_rrc_csp_efs_acq_db_v2_s;

/*! @briefEARFCN frequency along with blacklisted cells
 */
typedef struct
{
  /*! LTE frequency */
  lte_earfcn_t          earfcn;

  /*!Blacklisted cells for that frequency*/
  uint8                 blacklisted_cells_count;
  lte_phy_cell_id_t     blacklisted_cells[LTE_CPHY_MAX_ACQ_BLACKLISTED_CELLS];

    /*! (Optional) If PCI is provided, (applicable to GSM/HDR IRAT only)*/
  uint8                 pci_count;
  lte_phy_cell_id_t     pci_list[LTE_RRC_IRAT_MAX_X2L_REDIR_PCI_PER_FREQ];
} lte_rrc_csp_redir_earfcn_info_s;

/*! @brief Data structure for each frequency entry
*/
typedef struct
{
  /*! DL Center frequency */
  lte_earfcn_t dl_earfcn;
/*! System Bandwidth */
  lte_bandwidth_e bandwidth;
  uint64 timestamp_for_hst_entry;
} lte_hst_earfcn;

typedef struct
{
  uint8 num_frequencies;
  lte_hst_earfcn hst_freq_list[LTE_RRC_CSP_MAX_HST_DB_ENTRIES];
 // boolean  hstfreq_scan;
}lte_rrc_hstfreq_list;


/*! @brief The structure for cell lock
*/
typedef struct
{
  uint8 num_cell_id;

  lte_rrc_cell_id_s cell_id_list[LTE_RRC_MAX_NUM_CELL_LOCK];

} lte_rrc_csp_cell_lock_s;

/*! @brief Structure to store irat to lte redirected earfcn pci info
*/
typedef struct
{
  lte_rrc_redir_rat_type_e                  source_rat;
  
  /*! Num frequencies present */
  uint8                                     earfcn_count;
  
  /*!< Composite of LTE Frequency with PCI info etc */
  lte_rrc_csp_redir_earfcn_info_s       earfcn_info[LTE_RRC_IRAT_MAX_REDIR_FREQ];  
}lte_rrc_csp_irat_redir_info_s;


/*! @brief Static data type for CSP
*/
typedef struct
{
    /*! Info related to current scan */
    lte_rrc_csp_current_scan_info_s current_scan_info;

    /*! Holds NAS req contents */
    lte_rrc_csp_nas_req_info_s nas_req_info;

    /*! CSP Acquisition DB */
  lte_rrc_csp_acq_db_s acq_db;

  /*! BAND/EARFCN/PhyCellId for PCI LOCK feature and only valid if pci_lock_enabled is TRUE*/
  lte_rrc_csp_pci_lock_s pci_lock_efs_info;

  /*! Enable/Disable pci lock feature through EFS*/
  uint8 pci_lock_enabled;

  /*! Two EARFCNs for EARFCN_LOCK feature and only valid if earfcn_lock_enabled is TRUE*/
  lte_rrc_csp_earfcn_lock_s earfcn_lock_efs_info;

  /*! Enable/Disable earfcn lock feature through EFS*/
  uint8 earfcn_lock_enabled;

  /*! Enable/Disable LFS for HST feature through EFS*/
  uint8 lfs_for_hst_enabled;

  /*! Maximum number of acquisition retries on same frequency */
  uint8 max_num_acq_retries;

  /*! Maximum number of bands in a band scan request */
  uint8 max_bands_bs_req;

  /*! Enable/Disable Cell barring through EFS */
  uint32 cell_barring_is_enabled;

  /*! Short cell barring time configured through EFS */
  uint32 short_cell_barring_time;

  /*! Short cell barring time for RLF configured through EFS */
  uint32 rlf_short_cell_barring_time;

  /*! Reduced Barring time configured through EFS for causes such as SIB read errors*/
  uint32 reduced_barring_time;

  /*! barring time configured through EFS for internal errors such as IM3 backoff */
  uint32 backoff_barring_time; 

  /*!HST earfcn validity durationin conn mode*/
  uint32 hst_timer_duration;

    /*!HST earfcn validity duration in idle mode*/
  uint32 hst_timer_duration_idle;

  /*! Enable/Disable Intra-LTE Redir retries */
  uint32 intra_lte_redir_test_enabled;

  /*! Temporary print buffer - static because it is used to print system 
      update contents */
  char str_buf[1024];

  /* Indicates if the optimization for current freq cell selection completion
     on EOOS timer expiry is turned on */
  boolean eoos_complete_current_freq;

  /*! Saved Acq candidate list. This list is stored to resume scan from last saved 
      frequency during EOOS sans */
  lte_rrc_csp_acq_candidate_list_s saved_acq_candidate_list;

  /*! LTE BST band preference - need to check for EOOS scans across deactivation */
  sys_lte_band_mask_e_type lte_bst_band_preference;

  /*! OOS System Scan Timer */
  lte_rrc_tmr_s oos_system_scan_tmr;

  /*! OOS System Scan Timer */
  boolean use_oos_system_scan_tmr;

  /*! Systems scan disabled through EFS */
  boolean lfs_is_disabled;

  /*! BSR Qrxlev min enabled through EFS */
  boolean bsr_qrx_is_enabled;

  /*! EFS to use all three MNC digits for matching */
  boolean use_all_mnc_digits_enabled;

  /*! CSP Acquisition EFS DB to be read/written to EFS */
  lte_rrc_csp_efs_acq_db_s efs_acq_db;

  /*! CSP Acquisition EFS DB to be read/written to EFS */
  lte_rrc_csp_efs_acq_db_v2_s efs_acq_db_v2;

  /* Acq_db modified, needs writing to EFS */
  boolean acq_db_modified;

  boolean save_acq_db_enabled;

  boolean band_pri_list_modified;

  boolean pci_lock_info_modified;

  boolean earfcn_lock_info_modified;

  /*! Is IM3 backoff applied ?*/
  boolean im3_backoff_is_applied;

  /*! Current mode */
  lte_rrc_div_duplex_e curr_mode;

  /*! Previous mode */
  lte_rrc_div_duplex_e prev_mode;

  /*! Iteratively get more candidates on the same band */
  boolean iterative_band_scan_enabled;

  /*! Systems unsuitable for camping found during band scan */
  lte_cphy_band_scan_req_unusable_systems_s unusable_systems;

  /*! Num prioritized bands */
  uint32 num_prioritized_bands;

  /*! Prioritized band list */
  sys_sband_lte_e_type band_priority_list[SYS_SBAND_LTE_EUTRAN_BAND_MAX];

  /*! Num prioritized bands */
  uint32 num_pref_earfcns;
  
  /*! Pref earfcns list */
  lte_earfcn_t   pref_earfcn_lists[LTE_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES];
  /*! Prioritized band mask to perform two pass scan */
  sys_lte_band_mask_e_type priority_band_mask;

  /*! Variable to activate memory report */
  uint8 mem_report_valid;

  /*!RLF recovery system scan duration*/
  uint32 rlf_system_scan_duration;

  /*! RLF System Scan Timer */
  lte_rrc_tmr_s rlf_system_scan_tmr;

  /*! RLF System Scan Timer */
  lte_rrc_tmr_s rlf_sys_scan_interleave_tmr;

  /*UE supports IMS E911*/
  boolean ims_cap_ue;

  /*!Interval for interleaved sys scan for RLF recovery*/
  uint32 rlf_sys_scan_interleave_period;

  /* Index to split acq for X2L blind redireciton, efs configurable wih default set to LTE_RRC_CSP_SPLIT_ACQ_DB_INDEX*/
  uint8 split_acq_db_index_efs;  

  /*struct to the cell list for cell lock functionality */
  lte_rrc_csp_cell_lock_s cell_lock_list;

  /*struct to store hst neighbour*/
  lte_rrc_hstfreq_list hstfreq_list_update;

} lte_rrc_csp_static_data_s;

/*! @brief Dynamic data type for CSP
*/
typedef struct
{
    /*! Camped status */
    lte_rrc_csp_camp_info_s camp_info;

    /*! Holds the mode change request details, if there is one in progress */
    lte_rrc_mode_change_e mode;

    /*! Holds the Cfg req info */
    uint32 last_cfg_req_id;

  /*! System scan construction list - a temporary structure */
  lte_cphy_system_s systems[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

  /*! System scan construction list for operator  - a temporary structure */
  lte_cphy_system_s pref_freq_list_systems[LTE_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES];

  /*! Has system scan completed? */
  boolean system_scan_done;

  /*! Num elements in system scan cnf list */
  uint32 num_systems_cnf;

  /*! System scan cnf list - a temporary structure */
  lte_cphy_system_s systems_cnf[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

  /*! Energy metric from system scan cnf - a temporary structure */
  int16 energy[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

  /*! Array to help in sorting the system scan list - a temporary structure */
  boolean matched_systems[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

  /*! Is deactivation pending? */
  boolean deactivate_is_pending;

  /*! Info gathered from SIB1 */
  lte_rrc_sib1_info_s sib1_info;

  /*! Tracks reselection progress */
  boolean resel_is_in_progress;

  /*! Has SIB timeout or DL weak been received for serving cell? */
  boolean serv_cell_is_weak_during_resel;

  /*! Has many OOS events UE has seen since power up (reset on deactivate) */
  uint16 num_oos_events;

  /*! Has reselection been forbidden by a Proceed with Reselection(false)? */
  boolean reselection_is_stopped;

  /*! OOS Timer */
  lte_rrc_tmr_s oos_tmr; 

  /*! Has OOS scan finished once unsuccessfully? */
  boolean oos_search_done;

  /*! Variable to track self-deactivate due to IRAT redirection failure */
  boolean deact_reason_is_irat_redir_fail;

  /*! Is LTE supported? */
  boolean lte_is_supported;

  /*! Is 1X supported? */
  boolean cdma_1x_is_supported;

  /*! Is DO supported? */
  boolean cdma_do_is_supported;

  /*! Is GSM supported? */
  boolean gsm_is_supported;

  /*! Is WCDMA supported? */
  boolean wcdma_is_supported;

  /*! Is TDSCDMA supported? */
  boolean tdscdma_is_supported;

  /*! LTE band preference */
  sys_lte_band_mask_e_type lte_band_preference;

  /*! 1X band preference */
  sys_band_mask_e_type cdma_1x_band_preference;

  /*! DO band preference */
  sys_band_mask_e_type cdma_do_band_preference;

  /*! GSM band preference */
  sys_band_mask_e_type gsm_band_preference;

  /*! WCDMA band preference */
  sys_band_mask_e_type wcdma_band_preference;

  /*! TDSCDMA band preference */
  sys_band_mask_e_type tdscdma_band_preference;

  /*! Reselection target cell info */
  lte_cphy_resel_target_cell_info_s resel_target_cell_info;

  /*! Is cell select abort pending? */
  boolean cell_select_abort_is_pending;

  /*! Is deinit required? */
  boolean deinit_is_reqd;

  /*! Did we not add as found PLMN because of forbidden TA check? 
       or Cell is suitable but IMS not supported for emergency call */
  boolean cell_not_barred_not_suitable_for_camping;

  /* Pend svc_req while waiting for cfg_cnf in camped state */
  boolean camped_cfg_cnf_is_pending;

  /* Remeber to send svc_ind after receiving cfg_cnf in camped state  */
  boolean camped_svc_ind_is_pending;

  /* Remember to stop camping on the cell after receiving cfg_cnfi */
  boolean cell_camping_is_abandoned;

  /* Pend svc_req,dl_weak,oos,sib_read_error while waiting for cfg_cnf */
  boolean cfg_cnf_is_outstanding;

  /* OOS timer expired */
  boolean oos_tmri_expired;

  /* RLF system scan timer expired */
  boolean rlf_system_scan_tmri_expired;  

  /* RLF system scan timer expired */
  boolean sys_scan_prior_t311_expiry_tmri_expired; 

  /* did deactivate(ALT_SCAN) come during connection reestablishmsnt after
     camping adn was pended? */
  boolean deact_pended_during_reest;

  /* Deactivate reason */
  sys_stop_mode_reason_e_type deactivate_reason;

  /*!Enhanced requested PLMN for eHRPD to LTE Reselection */
  lte_rrc_plmn_list_s enh_req_plmn_list_info;

  /* Candidate PLMN list - to be used in PLMN match function only after cell barring check */
  lte_rrc_plmn_list_s candidate_plmn_list;

  /* Num of intra-LTE redirection acq attempts before giving up 
     (retries done incase cell is not up) */
  uint32 num_redir_acq_retries;

  /* type of access possible on camped cell */
  lte_rrc_cell_access_status_e cell_access_status;

  boolean irat_w2l_psho_in_progress;
  boolean irat_tds2l_psho_in_progress;

  /*struct to store interfrequency neighbors received in conn meas config*/
  lte_rrc_interfreq_list interfreq_list_update;

  /* Remember to send trans_id if scan type needs to change from CELL_SEL_NAS/RLF/
     CRE_FAILURE */
  boolean use_nas_trans_id;

  /* Remember to perform an additional system scan after all band scan 
     for Inital cell selection and RLF recovery*/
  boolean additional_sys_scan_done;

 /* To perform RLF acq Db scan only */
  boolean rlf_last_acq_db_scan;

 /*To perform HST scan*/
  boolean  hstfreq_scan;
 /*To perform operator scan */
  boolean pref_freq_list_scan;
   
/* List of earfcn sent in system scan during RLF
   This is filled when HST earfcn sent + acq DB + extension acq DB */
 lte_earfcn_t dl_earfcn_sent_in_sys_scan[LTE_RRC_CSP_MAX_SYSTEM_SCAN_EARFCN_ENTRIES]; //FEATURE_EXTENSION_ACQ_DB
   
/* Count of earfcn sent in  system scan*/
  uint32 num_earfcn_sent_sys_scan;

  /*! CSG reselection is in progress */
  boolean csg_resel;

  /* Saved Irat to LTE redirection info */
  lte_rrc_csp_irat_redir_info_s irat_redir_info;

  /*!To get RSRP for PLMN, send force cell_select_req*/
  boolean force_cell_select_req;

  /*To store total number of systems for system scan */
  uint8 num_systems;

  /*To store total number of systems for system scan */
  uint8 pref_freq_list_num_systems;

  /*Index to split acq_db system scan for two pass*/
  uint8 split_index;

  /*Index to split pref freq list system scan for batch when system number is largr than MAX number of system scan number */
  uint8 pref_freq_list_split_index;

  /*To store total number of systems for system scan */
  uint8 num_systems_in_sys_scan;

  /*To check if CSP is waiting for SIB1 after successfull HO*/
  boolean waiting_for_sib_after_ho;

  /*To remenber if service indication already send after reading SIB1 only*/
  boolean ho_service_ind_sent;
} lte_rrc_csp_dyn_data_s;

/*! @brief Private data type for CSP
*/
typedef struct
{
    lte_rrc_csp_static_data_s *sd_ptr; /*!< Static data pointer */
    lte_rrc_csp_dyn_data_s    *dd_ptr; /*!< Dynamic data pointer */
} lte_rrc_csp_s;

#endif /* LTE_RRC_CSPI_H */

