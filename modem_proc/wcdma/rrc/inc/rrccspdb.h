#ifndef RRCCSPDB_H
#define RRCCSPDB_H
/*===========================================================================
                        R R C    C S P   D B

DESCRIPTION
    This file contains type definitions and function declarations needed 
    for Acquisition.Database access.   
   
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None


    The acquisition database is a two-layered structure as shown below:

    [Most recent ptr]------->[Acq. entry]
    [Next recent ptr]------->[Acq. entry]
    [Next recent ptr]------->[Acq. entry]
    [Next recent ptr]------->[Acq. entry]
    [Next recent ptr]------->[Acq. entry]
    [Next recent ptr]------->[Acq. entry]


Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccspdb.h_v   1.10   12 Feb 2002 11:15:56   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrccspdb.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/15   sas     Update ACQ-DB when SIB reading is skipped for VPLMN.
12/29/14   bc      Made changes to control the range of frequencies to be skipped around a WCDMA cell by an nv.
03/17/14   sr      Made changes to not trigger PLMN LIST REQ when no FP entries are available
12/13/13   sn      Manual PLMN search optimizations
11/18/13   bc      Checked in REL 10 spec crs changes
17/05/13   sr      Hybrid CSG feature changes
05/09/13   sn      Reduce fine/raw scan skip range for manual CSG search
02/28/13   sn      New log packet for CSG finger printing information
01/03/13   ad      Made changes to reduce the redundant F3s
12/07/12   sn      CSG feature changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/13/12   pm      removing FEATURE_BAND_PRIORITY_ORDER_THROUGH_NV feature flag and reverting some #ifdef
                   changes
04/10/12   pm      Added code to support the band priority nv
02/24/12   sn      Changes to periodically save ACQ DB in NV
02/09/12   sn      Changes to send partial CGI info if MIB,SIB3 is read but not SIB1 
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/21/11   rl      Added code to support L2W CGI 
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
04/12/10   rm      Added code to support BC11
05/08/09   ss      Updated Copyright Information
02/02/09   rm      Ported List Frequency Scan (LFS) to conventional OOS
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/21/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
06/25/07   vm      Added fix for CR 119540. Added support to correctly set/reset current_band 
                   so that the band indicator check by SIB proc is done against the correct
                   current_band value.
06/20/07   vm      Added fix for CR 121957. The GSM skip range has now been 
                   changed for W RAW and Fine Scans.
06/19/07   vm      optimization: Based on systems recommendation 
                   (a)increased RRCCSP_FINE_SCAN_SKIP_RANGE to 22 from 18;
                   (b)increased RRCCSP_RAW_SCAN_SKIP_RANGE to 17 from 15 (when UOOS_P2
                      is NOT defined). When UOOS_P2 is defined we need to continue using the 
                      RRCCSP_RAW_SCAN_SKIP_RANGE as 15.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
11/17/06   vm      Added fix for CR 106389. Added new constant RRCCSP_GSM_CHANNEL_SKIP_RANGE.
02/21/06   vm      Added support for 1700_2100 AWS Band (IV) under feature name
                   FEATURE_WCDMA_BC4.
02/17/06   vm      Added support for 1700 band under feature name FEATURE_WCDMA_BC9.
11/28/05   bu      Added initial support for 900 MHz band under FEATURE_WCDMA_900.
04/14/05   vm      Since the feature FEATURE_BARRED_CELL_DURING_CELL_SELECTION is always
                   defined, un-featurized the code.
12/30/04   vm      Added prototype for rrc_csp_acq_db_freq_scanned_type.
10/08/04   bu      Removed prototype for rrc_csp_return_acq_list_band_class()
                   as it's no longer needed.
09/01/04   bu      Added mask for 1800 MHz band. Removed rrc_csp_band_class_e_type
                   as it's not used any more. Added prototype for
                   rrccsp_update_band_class_in_acq_db().
08/31/04   bu      Added masks for all the supported frequency bands.
                   Changed rrc_csp_band_class_e_type to rrc_csp_band_class_type
                   in rrc_csp_acq_entry_type.
08/20/04   sgk     Added prototype for rrc_csp_return_acq_list_band_class
                   included nv_items.h to suport return type 
                   nv_rrc_csp_band_class_e_type for the function 
                   rrc_csp_return_acq_list_band_class
                   removed  #define  MAX_ACQ_DB_ENTRIES  10 as it is 
                   already defined in nv_items.h
08/13/04   bu      Added WCDMA multiband support. Added enums for 800 and 850 MHz
                   bands in rrc_csp_band_class_e_type. Changed band_class to
                   rrc_csp_band_class_e_type in rrc_csp_acq_entry_type.
07/08/04   bu      Added prototype for rrccsp_should_freq_be_skipped(). Added
                   RRCCSP_RAW_SCAN_SKIP_RANGE & RRCCSP_FINE_SCAN_SKIP_RANGE.
11/07/03   bu      Added prototype for rrc_csp_should_freq_be_scanned().
06/04/03   bu      The boolean interrat_scr_code_present in rrc_csp_acq_entry_type
                   will be used only if FEATURE_BARRED_CELL_DURING_CELL_SELECTION
                   is not defined.
02/18/03   vn      Removed RRC_CSP_ANY_PLMN_CONN_BAR from rrc_csp_acq_e_type.
10/11/02   bu      Support for GSM to WCDMA cell reselection. Added a boolean
                   - interrat_scr_code_present to rrc_csp_acq_entry_type. Change
                   has been made under FEATURE_INTERRAT_CELL_RESELECTION_GTOW.
02/12/02   bu      Added prototypes for rrc_csp_read_nv_acq_info() and
                   rrc_csp_write_nv_acq_info().
12/21/01   vn      Changes to acquisition database structure so that it stores
                   acquisition list indices in place of pointers. Needed for
                   a relocatable acquisition database. Reduced memory requirement
                   for acquisition database.
05/16/01   vn      Added RRC_CSP_SAME_MCC and RRC_CSP_SAME_MNC to compare MCC
                   and MNC of two PLMNs based on the new array format in 
                   rrc_plmn_identity_type.
03/15/01   vn      Changes to acoomodate LED ASN1 header file.
02/08/01   vn      Moved compare_expected_cells to rrccspi.h, removed
                   rrc_csp_get_type_acq as it is not possible to correctly
                   calculate Acq type from PLMN and Acq status mask.
01/19/00   vn      Added new function prototypes for rrc_csp_get_type_acq() 
                   and rrc_csp_compare_expected_cell().
01/03/00   vn      Updates from code review.
12/29/00   vn      Includes rrcccm.h, new definitions for freq and scr code.
12/08/00   vn      Added last_entry_invalid field to acq. database type.
12/06/00   vn      Added Band Class to acq. entry structure.
12/06/00   vn      Added MAX_ACQ_DB_INDEX, updated rrc_csp_acq_db_type.
11/17/00   vn      Created file.

===========================================================================*/

/*==========================================================================
** Includes and Public Data Declarations
**=========================================================================*/


/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "uecomdef.h"
#include "rrcmmif.h"
#include "rrcdata_v.h"
#include "nv_items.h"
#include "rrcnv.h"

#ifdef FEATURE_FEMTO_CSG
#include "rrcintif.h"
#endif


/* =========================================================================
** Constant / Define Declarations
** =========================================================================*/
#ifdef FEATURE_FEMTO_CSG
#define  MAX_ACQ_DB_INDEX    (MAX_ACQ_DB_SIZE - 1)
#define  MAX_ACQ_DB_INDEX_TEMP    (MAX_ACQ_DB_ENTRIES_TEMP - 1)

/* Max No.of LTE cells for finger printing */
#define  NUM_CSG_LTE_MACRO         6
/* Max No.of W cells for finger printing */
#define  NUM_CSG_WCDMA_MACRO       8
#else
#define  MAX_ACQ_DB_INDEX    (MAX_ACQ_DB_SIZE - 1)
#endif

#ifdef FEATURE_EOOS
  #error code not present
#else
  #define RRCCSP_RAW_SCAN_SKIP_RANGE 17
  #define RRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE 7
#endif

#ifdef FEATURE_FEMTO_CSG
  #define RRCCSP_RAW_SCAN_SKIP_CSG_RANGE   5
  #define RRCCSP_FINE_SCAN_SKIP_CSG_RANGE 10
#endif

#define RRCCSP_FINE_SCAN_SKIP_RANGE  wcdma_freq_skip_range_nv
#define RRCCSP_GSM_CHANNEL_FINE_SCAN_SKIP_RANGE 12

#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
#define RRCCSP_LTE_CHANNEL_RAW_SCAN_SKIP_RANGE 9 /*(25-16), units 100KHZ*/
#define RRCCSP_LTE_CHANNEL_FINE_SCAN_SKIP_RANGE 19 /*(25-6), units 100KHZ*/
#endif

/* These are the bits that together define the status of the stored acq. entry */
#define  RRC_CSP_SPEC_PLMN_BIT         0x0001
/* The RAI or LAI is forbidden */
#define  RRC_CSP_FORBID_REG_AREA_BIT   0x0002
/* The cell is barred in Idle mode - Not Barred for Emergency Calls */ 
#define  RRC_CSP_IDLE_CELL_BAR_BIT     0x0004

/* The cell is barred from Access in Connected/Connecting mode. Not barred
 * for Emergency Calls 
 */
#define  RRC_CSP_CONN_CELL_BAR_BIT     0x0008


/* This macro compares MCC of two PLMNs */
#define   RRC_CSP_SAME_MCC(plmn1, plmn2)   \
( \
  (plmn1.mcc[0] == plmn2.mcc[0]) && \
  (plmn1.mcc[1] == plmn2.mcc[1]) && \
  (plmn1.mcc[2] == plmn2.mcc[2])  \
)

/* This macro compares MNC of the 2 PLMNs. If the number of MNC digits in a PLMN is 
 * 2, the last byte of MNC has to be a 0xf. Hence the comparison is always of 3 bytes
 * irrespective of the actual length of MNC.
 */
#define   RRC_CSP_SAME_MNC(plmn1, plmn2)   \
( \
  (plmn1.mnc[0] == plmn2.mnc[0]) && \
  (plmn1.mnc[1] == plmn2.mnc[1]) && \
  (plmn1.mnc[2] == plmn2.mnc[2])  \
)

#define  RRC_CSP_PCS_BAND_INDEX 0              
#define  RRC_CSP_IMT2K_BAND_INDEX 1            
#define  RRC_CSP_WCDMA_800_BAND_INDEX 2        
#define  RRC_CSP_WCDMA_850_BAND_INDEX 3       
#define  RRC_CSP_WCDMA_1800_BAND_INDEX 4       
#define  RRC_CSP_WCDMA_900_BAND_INDEX 5       
#define  RRC_CSP_WCDMA_1700_BAND_INDEX 6       
#define  RRC_CSP_WCDMA_1700_2100_BAND_INDEX 7  
#define  RRC_CSP_WCDMA_1500_BAND_INDEX 8       
#define  RRC_CSP_WCDMA_BC19_BAND_INDEX 9      
#define  RRC_CSP_BAND_MAX_INDEX 10

#define  RRC_CSP_PCS_BAND              (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_PCS_BAND_INDEX]
#define  RRC_CSP_IMT2K_BAND            (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_IMT2K_BAND_INDEX]
#define  RRC_CSP_WCDMA_800_BAND        (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_800_BAND_INDEX]
#define  RRC_CSP_WCDMA_850_BAND        (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_850_BAND_INDEX]
#define  RRC_CSP_WCDMA_1800_BAND       (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_1800_BAND_INDEX]
#define  RRC_CSP_WCDMA_900_BAND        (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_900_BAND_INDEX]
#define  RRC_CSP_WCDMA_1700_BAND       (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_1700_BAND_INDEX]
#define  RRC_CSP_WCDMA_1700_2100_BAND  (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_1700_2100_BAND_INDEX]
#define  RRC_CSP_WCDMA_1500_BAND       (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_1500_BAND_INDEX]
#define  RRC_CSP_WCDMA_BC19_BAND       (uint16)rrc_nv_band_priority_config.band_priority_list[RRC_CSP_WCDMA_BC19_BAND_INDEX]
#define  RRC_CSP_BAND_MAX              0x1000

  /* Note that Maximum number of shared PLMNs is 6 for a given Cell.
   * (1 primary PLMN Id of MIB + 5 PLMNs in multiple PLMN List of MIB)
   */
#define  MAX_SHARED_PLMNS          6

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/* Enumerated types of acquisitions */
typedef enum
{
  /* Get a non-barred, non-forbidden reg area cell of a specific PLMN */
  RRC_CSP_SPEC_PLMN,

  /* Get a non-barred, non-forbidden reg area cell of any PLMN */
  RRC_CSP_ANY_PLMN,

  RRC_CSP_MAX_ACQ_ENTRY_TYPES
}rrc_csp_acq_e_type;


#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */


typedef enum
{
  RRC_CSP_CELL_BELOW_EXPECTED,
  RRC_CSP_CELL_AS_EXPECTED,
  RRC_CSP_CELL_BETTER_THAN_EXPECTED
}rrc_csp_cell_actual_vs_expected_e_type;


/* This enum lists the status of PLMN ID compared with the requested PLMN ID */
typedef enum
{
  /* PLMN ID is same as requested */
  RRC_CSP_PLMN_SAME_AS_REQ,
  /* PLMN ID is NOT same as requested */
  RRC_CSP_PLMN_DIFF_THAN_REQ
}rrc_csp_plmn_status_e_type;

/* This is the storage type for band class */
typedef   uint16  rrc_csp_band_class_type;

/* This is a mask to store acq. charateristics of a cell */
typedef   uint16  rrc_csp_acq_status_type;


typedef struct
{

  /* The PLMN ID */
  rrc_plmn_identity_type  plmn_id;
  /* Type of this stored acquisition entry */
  rrc_csp_acq_status_type  status_mask;

}rrc_csp_shared_plmns_access_info_type;

typedef struct
{
  /* Primary PLMN ID as given in MIB */
  rrc_plmn_identity_type  common_plmn_id;

  /* For each of the PLMNs indicated as shared in the MIB, 
   * contains PLMN Id and its barring and forbidden info 
   */
  rrc_csp_shared_plmns_access_info_type shared_plmns_access_info[MAX_SHARED_PLMNS];

  /* Number of PLMNs indicated as shared in the MIB */
  uint8 num_of_shared_plmns;
}rrc_csp_nw_sharing_entry_type;

#ifdef FEATURE_FEMTO_CSG

/* W macro cell for FP */
typedef struct
{
  uint16 psc;
  uint32 freq;
  rrc_plmn_identity_type  plmn_id;
  rrc_int_cell_id_type         cell_id;
  uint32 acq_fail_counter;
} rrc_fp_W_macro_info_type;

/* LTE macro cell for FP */
typedef struct
{
  uint16 pci;
  uint32 freq;
  rrc_plmn_identity_type  plmn_id;
  rrc_int_cell_id_type         cell_id;
  uint32 acq_fail_counter;
} rrc_fp_lte_macro_info_type;

/* Structure of ACQ DB entry that will be written into EFS */
typedef struct
{ 
  /* The PLMN ID */
  rrc_plmn_identity_type  plmn_id;
  /* Freq. where PLMN is present */
  rrc_freq_type  freq;
 
  /* band Class - just a place holder for the time being */
  rrc_csp_band_class_type  band_class;
 
  /* Type of this stored acquisition entry */
  rrc_csp_acq_status_type  status_mask;
 
  /* Current Cell ID selected - received in SIB Confirmation message */
  rrc_int_cell_id_type         cell_id;
 
  /* Scrambling Code of the Cell */
  rrc_scr_code_type   scr_code;
 
  /* Location Area Code of the cell */
  uint8  lac;

  rrc_int_csg_id_type   csg_id;
 
uint8 fp_w_macro_cnt;
uint8 w_macro_write_idx;
rrc_fp_W_macro_info_type w_macro[NUM_CSG_WCDMA_MACRO];
 
uint8 fp_lte_macro_cnt;
uint8 lte_macro_write_idx;
rrc_fp_lte_macro_info_type lte_macro[NUM_CSG_LTE_MACRO];
 
}rrc_csp_acq_entry_efs_type;

#endif


/* Acquisition entry type */
typedef struct
{ 
  /* The PLMN ID */
  rrc_plmn_identity_type  plmn_id;
  /* Freq. where PLMN is present */
  rrc_freq_type  freq;

  /* band Class - just a place holder for the time being */
  rrc_csp_band_class_type  band_class;

  /* Type of this stored acquisition entry */
  rrc_csp_acq_status_type  status_mask;

  /* Current Cell ID selected - received in SIB Confirmation message */
  rrc_int_cell_id_type         cell_id;

  /* Scrambling Code of the Cell */
  rrc_scr_code_type   scr_code;

  /* Location Area Code of the cell */
  uint8  lac;
#ifdef FEATURE_LTE_TO_WCDMA
  boolean rac_valid;
  boolean lac_valid;
  rrc_rai_type rai;
#endif
#ifdef FEATURE_EOOS
  #error code not present
#endif

  /* Is the current cell network shared */
  boolean            is_valid_nw_sharing;

  /* If the current cell is shared, then the list of shared PLMNs 
   * and their barring/forbidden info 
   */
  rrc_csp_nw_sharing_entry_type  shared_list_of_plmns;

#ifdef FEATURE_FEMTO_CSG
  /* Indicates if the cell is hybrid or not */
  boolean cell_is_hybrid;

/* Current CSG ID selected - received in SIB confirmation message */
  rrc_int_csg_id_type   csg_id;

/* Stores W macro finger printing info*/
  uint8 fp_w_macro_cnt;
  uint8 w_macro_write_idx;
  rrc_fp_W_macro_info_type w_macro[NUM_CSG_WCDMA_MACRO];
  
 /* Stores LTE macro finger printing info*/
  uint8 fp_lte_macro_cnt;
  uint8 lte_macro_write_idx;
  rrc_fp_lte_macro_info_type lte_macro[NUM_CSG_LTE_MACRO];
#endif
}rrc_csp_acq_entry_type;


typedef struct
{
  /* Next Acquisition pointer to be read */
  int8  curr_rd_index;
  /* This is the current write index pointing to the pointer entry in ptr_list
   * that can be written into next.
   */
  int8  curr_wr_index;

  /* Indicates if the last entry in database pointer list is Invalid. 
   */
  boolean last_entry_invalid;

#ifdef FEATURE_FEMTO_CSG
  /* Indicates the number of CSG entries in ACQ-DB */
  uint8  num_of_csg_entries;
#endif
  /* List of ordered indices to acquisition entries */
  uint8  acq_list_indices[MAX_ACQ_DB_SIZE];

}rrc_csp_acq_db_type;


typedef struct
{
  /* frequencies scanned during acquisition database scan */
  rrc_freq_type freq[MAX_ACQ_DB_SIZE];

  /* array index where next frequency should be stored */
  uint8         wr_index;
} rrc_csp_acq_db_freq_scanned_type;


/*===========================================================================
**                  FUNCTION DECLARATIONS
**=========================================================================*/

/*===========================================================================

FUNCTION          RRC_CSP_INIT_ACQ_DB

DESCRIPTION       This function initializes the Stored acquisition database. 
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

===========================================================================*/
void  rrc_csp_init_acq_db(void);

/*===========================================================================

FUNCTION          RRC_CSP_WRITE_NV_ACQ_DB_INFO

DESCRIPTION       This function saves the stored acquisition database into the NV.
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
void  rrc_csp_write_nv_acq_db_info(void);

/*===========================================================================

FUNCTION          RRC_CSP_WRITE_NV_ACQ_LIST_INFO

DESCRIPTION       This function saves the stored acquisition list info into the NV.
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
void  rrc_csp_write_nv_acq_list_info(int index);

/*===========================================================================

FUNCTION          RRC_CSP_ADD_ACQ_ENTRY

DESCRIPTION       This function adds an acquisition entry to the acquisition 
                  list. While doing so, it also updates the acquisition
                  database pointers so that the 1st pointer points to this
                  most recent acquisition entry.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  rrc_csp_add_acq_entry(rrc_csp_acq_entry_type* acq_entry_ptr);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          RRC_CSP_ADD_ACQ_ENTRY_TEMP

DESCRIPTION       Temporary ACQ DB to store CSG cells which are not in 
                  whitelist found during manual CSG search
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  rrc_csp_add_acq_entry_temp(rrc_csp_acq_entry_type* acq_entry_ptr);

/*===========================================================================

FUNCTION          RRCCSP_FP_MACRO_PRESENT_IN_ACQ_DB

DESCRIPTION       Checks whether passed W/L macro information is present in 
                  finger printing data of anyone of CSG cells ACQ DB entries
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if entry is found otherwise FALSE

SIDE EFFECTS
                  None.

===========================================================================*/
boolean rrccsp_fp_macro_present_in_acq_db(rrc_csp_acq_entry_type acq_db_entry);

/*===========================================================================

FUNCTION          RRCCSP_ADD_MACRO_FP_INFO_IN_ACQ_DB

DESCRIPTION       Adds finger printing information to passed ACQ DB entry
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrccsp_add_macro_fp_info_in_acq_db(rrc_csp_acq_entry_type* femto_info);

/*===========================================================================

FUNCTION          RRCCSP_PRUNE_ACQDB_AFTER_ACQ

DESCRIPTION      This function scans the acq db for any CSG cells that are not detected in acq cnf(success/failure)
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                 None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrccsp_prune_acqdb_after_acq(boolean acq_status);

/*===========================================================================

FUNCTION          RRCCSP_PRUNE_ACQDB_AFTER_SIB

DESCRIPTION      This function scans the acq db for any CSG cells that are not detected in acq cnf(success/failure)
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                 None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrccsp_prune_acqdb_after_SIB(rrc_int_csg_id_type csg_id, boolean is_white);
/*===========================================================================

FUNCTION          RRCCSP_REMOVE_FP_INFO

DESCRIPTION      Removes CSG <-> macro FP assocaition
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                 None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrccsp_remove_fp_info(uint8 acq_index, uint8 fp_index, boolean is_w_macro);
#endif

/*===========================================================================

FUNCTION          RRC_CSP_SHOULD_FREQ_BE_SCANNED

DESCRIPTION       This function checks whether an acquisition has already been
                  tried on the indicated frequency as part of acquisition database
                  scan.

DEPENDENCIES
                  None.

RETURN VALUE
                  Boolean. FALSE indicates the frequency has been scanned before
                           TRUE otherwise

SIDE EFFECTS
                  None.

===========================================================================*/
boolean
rrc_csp_should_freq_be_scanned(rrc_requested_plmn_type req_plmn, rrc_freq_type freq);


/*===========================================================================

FUNCTION          RRC_CSP_GET_FIRST_MATCH_ACQ_ENTRY

DESCRIPTION       This function gets the most recent entry from the acquisition 
                  list that matches the criteria given in the parameters. 
                  
                  The most recent acquisition entry is based on the order of 
                  pointers in acquisition database and NOT on the order of entries
                  in acquisition list. 
                  An acquisition entry pointed to by a pointer that occurs before
                  in the acquisition database pointer list is considered more 
                  recent than another entry which is pointed to by a pointer that 
                  occurs later in the acquisition database pointer list.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Pointer to acquisition entry. A NULL pointer means no 
                  acquisition entry was found that fulfilled selection criteria.
SIDE EFFECTS
                  None.

===========================================================================*/
rrc_csp_acq_entry_type*
rrc_csp_get_first_match_acq_entry(rrc_plmn_identity_type req_plmn_id, rrc_csp_acq_e_type req_type_acq);


/*===========================================================================

FUNCTION          RRC_CSP_GET_NEXT_MATCH_ACQ_ENTRY

DESCRIPTION       This function gets next entry from the acquisition list
                  that matches the criteria given in the parameters. 
                  To find the "next" entry, the acquisition database pointer list 
                  is utilized from curr read index onwards.
                   
                  The most recent acquisition entry is based on the order of 
                  pointers in acquisition database and NOT on the order of entries
                  in acquisition list. 
                  An acquisition entry pointed to by a pointer that occurs before
                  in the acquisition database pointer list is considered more 
                  recent than another entry which is pointed to by a pointer that 
                  occurs later in the acquisition database pointer list.
           
DEPENDENCIES
                  None

RETURN VALUE
                  Pointer to acquisition entry. A NULL pointer means no acquisition
                  entry was found that fulfilled selection criteria.
SIDE EFFECTS
                  None.

===========================================================================*/
rrc_csp_acq_entry_type*
rrc_csp_get_next_match_acq_entry(rrc_plmn_identity_type req_plmn_id, rrc_csp_acq_e_type req_type_acq);


/*===========================================================================

FUNCTION          RRC_CSP_READ_NV_ACQ_INFO

DESCRIPTION       This function reads the stored acquisition databse and the
                  acquisition list from the NV.

DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
void  rrc_csp_read_nv_acq_info(void);


/*===========================================================================

FUNCTION          RRC_CSP_WRITE_NV_ACQ_INFO

DESCRIPTION       This function saves the stored acquisition databse and the
                  acquisition list into the NV.

DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
void  rrc_csp_write_nv_acq_info(void);

/*===========================================================================

FUNCTION          RRCCSP_SHOULD_FREQ_BE_SKIPPED

DESCRIPTION       This function determines based on previous acquired frequencies,
                  whether a new frequency should be scanned or skipped.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrccsp_should_freq_be_skipped(rrc_freq_type freq, uint16 resolution);


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_BAND_CLASS_IN_ACQ_DB

DESCRIPTION       This function updates the band class for all acquisition
                  entries in the acquisition database based on the latest
                  UE band preference.
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
void rrccsp_update_band_class_in_acq_db(void);

/*===========================================================================

FUNCTION          RRC_CSP_ASSERT_ACQ_DB_VARS

DESCRIPTION       This function is used to check the sanity of acq db entries

DEPENDENCIES      NONE

RETURN VALUE      number of entries in ACQ DB
                  
SIDE EFFECTS      NONE

===========================================================================*/
int rrc_csp_assert_acq_db_vars(void);


#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */


/*===========================================================================

FUNCTION           RRCCSP_ADD_ACQ_DB_AND_NW_DB_ENTRIES

DESCRIPTION        Adds an entry into the NW sharing DB at the given index Note that one-one mapping
                   is maintained between ACQ DB and NW sharing DB.

DEPENDENCIES       None

RETURN VALUE       Success/Failure. Failure will be an extra-ordinary event and
                   could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS       None
===========================================================================*/
void rrccsp_add_acq_db_and_nw_db_entries(
                                                           rrc_csp_acq_entry_type* acq_entry_ptr,
                                                           rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr);

/*===========================================================================

FUNCTION           RRCCSP_ADD_PLMN_ACQ_DB_AND_NW_DB_ENTRIES

DESCRIPTION        Adds an entry into the NW sharing DB at the given index Note that one-one mapping
                   is maintained between ACQ DB and NW sharing DB.

DEPENDENCIES       None

RETURN VALUE       Success/Failure. Failure will be an extra-ordinary event and
                   could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS       None
===========================================================================*/
void rrccsp_add_plmn_acq_db_and_nw_db_entries(rrc_plmn_identity_type plmn_id,
                                              rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr, 
                                              boolean is_valid_nw_sharing);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION           RRCCSP_ADD_ACQ_DB_AND_NW_DB_ENTRIES_TEMP

DESCRIPTION        Temporary ACQ DB to store CSG cells which are not in 
                   whitelist found during manual CSG search

DEPENDENCIES       None

RETURN VALUE       Success/Failure. Failure will be an extra-ordinary event and
                   could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS       None
===========================================================================*/

uecomdef_status_e_type rrccsp_add_acq_db_and_nw_db_entries_temp(
                                                           rrc_csp_acq_entry_type* acq_entry_ptr,
                                                           rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr);


/*===========================================================================

FUNCTION          RRCCSP_CHECK_WCDMA_FP_INFO

DESCRIPTION       Checks if WCDMA FP info is available or not

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_check_wcdma_fp_info(void);


#endif

/*===========================================================================

FUNCTION          RRCCSP_NW_SHARING_PRINT_ENTRY

DESCRIPTION       Prints a given network sharing entry

DEPENDENCIES      NONE

RETURN VALUE      None
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_nw_sharing_print_entry(rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr);

/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_EQUIVALENT_TO_ANY_OF_SHARED_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are equivalent

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_nas_plmn_equivalent_to_any_of_shared_bcch_plmns(
                                                               rrc_plmn_identity_type nas_plmn, 
                                                               uint8 index_in_acq_or_shared_db);

/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_MATCH_ANY_OF_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are same.

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                rrc_plmn_identity_type nas_plmn, 
                                                uint8 index_in_acq_or_shared_db);

/*===========================================================================

FUNCTION          RRCCSP_IS_ANY_OF_BCCH_PLMNS_IN_BPLMN_LIST

DESCRIPTION       Finds if any one of the shared PLMNs are in the list of NAS requested PLMNs. Note that this is 
                  used for early abort of SIB reading. If none of the shared bcch PLMNs are in the NAS requested 
                  PLMN list then SIB reading is avoided immediately after reading the MIB.

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_is_any_of_bcch_plmns_in_bplmn_list(rrc_csp_nw_sharing_entry_type bplmn_nw_db_entry);

/*===========================================================================

FUNCTION           RRCCSP_ADD_FREQ_BANDS_FROM_ACQ_DB_TO_BAND_LIST

DESCRIPTION        Extracts frequency bands from ACQ DB in LIFO order and adds them
                  to the band_list provided starting from num_of_bands_already_present.

DEPENDENCIES       num_of_bands_already_present should be initialized properly before calling
                   this function.

RETURN VALUE       number of bands present in the band list at the end..

SIDE EFFECTS       None
===========================================================================*/
uint8 
rrccsp_add_freq_bands_from_acq_db_to_band_list
(
  rrc_csp_band_class_type*    band_list,
  uint8 num_of_bands_already_present
);

/*===========================================================================

FUNCTION          RRC_CSP_LFS_UPDATE_LIST_DB_WITH_TARGET_FDD_LIST

DESCRIPTION       This function populates LIST DB with target FDD list given by GSM-RR and also
                  returns pointer to the first entry in LIST DB. If LIST DB is empty then NULL is returned

DEPENDENCIES      This function is called only for GtoW redirection search and also only after initiating
                  target FDD cell list from GSM.

RETURN VALUE      Pointer to first entry of LIST DB
                  Otherwise, NULL if LIST DB is empty
                  
SIDE EFFECTS      NONE

===========================================================================*/
rrc_csp_acq_entry_type *rrc_csp_lfs_update_list_db_with_target_fdd_list(void);

/*===========================================================================

FUNCTION          RRCCSP_UPDATE_MATCH_ACQ_ENTRIES_INTO_LIST_DB

DESCRIPTION       This function is used by CSP to run through the acq DB for specific
                  (or) any PLMN based on the current acquisition type and if a suitable 
                  entry is found  then the frequency, PLMN Id will be stored in the
                           current CSP list scan database

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_update_match_acq_entries_into_list_db(void);

/*===========================================================================

FUNCTION          RRCCSP_LOG_MACRO_FP_INFO

DESCRIPTION       Logs CSG cells finger printed to current camped macro.
                  CSP uses this function at the start of ASF

DEPENDENCIES      NONE

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_log_macro_fp_info(uint8);
#endif
