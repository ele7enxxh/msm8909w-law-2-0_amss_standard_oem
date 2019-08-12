#ifndef TDSRRCCSPDB_H
#define TDSRRCCSPDB_H
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


Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrccspdb.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/11   zz     Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53

===========================================================================*/

/*==========================================================================
** Includes and Public Data Declarations
**=========================================================================*/


/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsuecomdef.h"
#include "tdsrrcmm_v.h"
#include "tdsrrcdata.h"
#include "tdsrrccommon.h"
#include "tdsiratif.h"
#include "nv_items.h"


/* =========================================================================
** Constant / Define Declarations
** =========================================================================*/
#define  TDS_MAX_ACQ_DB_NV_ENTRIES   32  /* num of entries in NV element, need in-sync with QXDM */
#define  TDS_MAX_ACQ_DB_ENTRIES      MAX_ACQ_DB_ENTRIES  /* num of effective entries in use */
#define  TDSMAX_ACQ_DB_INDEX    (TDS_MAX_ACQ_DB_ENTRIES - 1)
#define  TDS_MAX_PRIM_FREQ_NBR       32
#define  TDS_MAX_FREQ_SCANNED_ENTRIES (TDS_MAX_ACQ_DB_ENTRIES + TDS_MAX_PRIM_FREQ_NBR)

/* Don't skip raw scan range */
#define TDSRRCCSP_RAW_SCAN_SKIP_RANGE     0
/* Set skip range as 7*200KHz for fine scan */
#define TDSRRCCSP_FINE_SCAN_SKIP_RANGE    7
#define TDSRRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE 7
#define TDSRRCCSP_GSM_CHANNEL_FINE_SCAN_SKIP_RANGE 12

#if defined(FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
#define TDSRRCCSP_LTE_CHANNEL_RAW_SCAN_SKIP_RANGE 9 /*(25-16), units 100KHZ*/
#define TDSRRCCSP_LTE_CHANNEL_FINE_SCAN_SKIP_RANGE 19 /*(25-6), units 100KHZ*/
#endif

/* These are the bits that together define the status of the stored acq. entry */
#define  TDSRRC_CSP_SPEC_PLMN_BIT         0x0001
/* The RAI or LAI is forbidden */
#define  TDSRRC_CSP_FORBID_REG_AREA_BIT   0x0002
/* The cell is barred in Idle mode - Not Barred for Emergency Calls */ 
#define  TDSRRC_CSP_IDLE_CELL_BAR_BIT     0x0004

/* The cell is barred from Access in Connected/Connecting mode. Not barred
 * for Emergency Calls 
 */
#define  TDSRRC_CSP_CONN_CELL_BAR_BIT     0x0008


/* This macro compares MCC of two PLMNs */
#define   TDSRRC_CSP_SAME_MCC(plmn1, plmn2)   \
( \
  (plmn1.mcc[0] == plmn2.mcc[0]) && \
  (plmn1.mcc[1] == plmn2.mcc[1]) && \
  (plmn1.mcc[2] == plmn2.mcc[2])  \
)

/* This macro compares MNC of the 2 PLMNs. If the number of MNC digits in a PLMN is 
 * 2, the last byte of MNC has to be a 0xf. Hence the comparison is always of 3 bytes
 * irrespective of the actual length of MNC.
 */
#define   TDSRRC_CSP_SAME_MNC(plmn1, plmn2)   \
( \
  (plmn1.mnc[0] == plmn2.mnc[0]) && \
  (plmn1.mnc[1] == plmn2.mnc[1]) && \
  (plmn1.mnc[2] == plmn2.mnc[2])  \
)

/* TDD bands in the order of band priority */
#define  RRC_CSP_TDS_A_BAND               0x1000

#define  RRC_CSP_TDS_F_UPPER_BAND         0x0800
#define  RRC_CSP_TDS_F_LOWER_BAND         0x0400

#define  RRC_CSP_TDS_F_BAND               (RRC_CSP_TDS_F_UPPER_BAND|RRC_CSP_TDS_F_LOWER_BAND)

#define  RRC_CSP_TDS_E_BAND               0x0200

#define  TDSRRC_CSP_BAND_MAX              0x8000

/* the size of sub band to be divided within the whole band. Unit 200KHz */
#define  TDSRRC_CSP_SUB_BAND_GAP          71

  /* Note that Maximum number of shared PLMNs is 6 for a given Cell.
   * (1 primary PLMN Id of MIB + 5 PLMNs in multiple PLMN List of MIB)
   */
#define  TDSMAX_SHARED_PLMNS          6

#define  TDSRRC_CSP_SUPPORT_LOWER_BAND_F  \
  (tdsrrccsp_band_f_pattern.lower_band_shallow || tdsrrccsp_band_f_pattern.lower_band_deep)

#define  TDSRRC_CSP_SUPPORT_UPPER_BAND_F  \
  (tdsrrccsp_band_f_pattern.upper_band_shallow || tdsrrccsp_band_f_pattern.upper_band_deep)


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/* Enumerated types of acquisitions */
typedef enum
{
  /* Get a non-barred, non-forbidden reg area cell of a specific PLMN */
  TDSRRC_CSP_SPEC_PLMN,

  /* Get a non-barred, non-forbidden reg area cell of any PLMN */
  TDSRRC_CSP_ANY_PLMN,

  TDSRRC_CSP_MAX_ACQ_ENTRY_TYPES
}tdsrrc_csp_acq_e_type;

typedef enum
{
    TDSRRC_CSP_NO_LOCK,
    TDSRRC_CSP_LOCK_FREQ,
    TDSRRC_CSP_LOCK_CELL
}tdsrrc_csp_lock_mode;

typedef struct{ 
  tdsrrc_csp_lock_mode                             lock_mode;
  uint16                                           lock_freq;
  uint8                                            lock_cpid;
} tdsrrc_csp_lock_freq_cell_type;

extern tdsrrc_csp_lock_freq_cell_type tdsrrccsp_lock_freq_cell;

typedef enum
{
  TDSRRC_CSP_CELL_BELOW_EXPECTED,
  TDSRRC_CSP_CELL_AS_EXPECTED,
  TDSRRC_CSP_CELL_BETTER_THAN_EXPECTED
}tdsrrc_csp_cell_actual_vs_expected_e_type;


/* This enum lists the status of PLMN ID compared with the requested PLMN ID */
typedef enum
{
  /* PLMN ID is same as requested */
  TDSRRC_CSP_PLMN_SAME_AS_REQ,
  /* PLMN ID is NOT same as requested */
  TDSRRC_CSP_PLMN_DIFF_THAN_REQ
}tdsrrc_csp_plmn_status_e_type;

/* This is the storage type for band class */
typedef   uint16  tdsrrc_csp_band_class_type;

/* This is a mask to store acq. charateristics of a cell */
typedef   uint16  tdsrrc_csp_acq_status_type;


typedef struct
{

  /* The PLMN ID */
  rrc_plmn_identity_type  plmn_id;
  /* Type of this stored acquisition entry */
  tdsrrc_csp_acq_status_type  status_mask;

}tdsrrc_csp_shared_plmns_access_info_type;

typedef struct
{
  /* Primary PLMN ID as given in MIB */
  rrc_plmn_identity_type  common_plmn_id;

  /* For each of the PLMNs indicated as shared in the MIB, 
   * contains PLMN Id and its barring and forbidden info 
   */
  tdsrrc_csp_shared_plmns_access_info_type shared_plmns_access_info[TDSMAX_SHARED_PLMNS];

  /* Number of PLMNs indicated as shared in the MIB */
  uint8 num_of_shared_plmns;
}tdsrrc_csp_nw_sharing_entry_type;



/* Acquisition entry type */
typedef struct
{ 
  /* The PLMN ID */
  rrc_plmn_identity_type  plmn_id;
  /* Freq. where PLMN is present */
  tdsrrc_freq_type  freq;

  /* band Class - just a place holder for the time being */
  tdsrrc_csp_band_class_type  band_class;

  /* Type of this stored acquisition entry */
  tdsrrc_csp_acq_status_type  status_mask;

  /* Current Cell ID selected - received in SIB Confirmation message */
  tdsrrc_int_cell_id_type         cell_id;

  /* Scrambling Code of the Cell */
  /* For TDD it means Cell Parameters ID. Keep the old FDD name for now */
  tdsrrc_cpid_type   cpid;

  /* HO target cell rscp, for L1 to skip step 0 and do reAcq */
  int16              rscp;

#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
  /* timing info passed to L1 for synchronized re-acq */
  uint8       lna_state; /* current LNA state */
  uint16      ts0_position; /* ts0 start position from step0 result */
  uint8       num_of_jds_cells; /* number of JDS cells for interference cancellation */
  uint8       cell_list[TDSIRAT_MAX_TDS_CELLS_PER_FREQ]; /* cell parameter id */
#endif

  /* Location Area Code of the cell */
  uint8  lac;
  boolean rac_valid;
  boolean lac_valid;
  rrc_rai_type rai;

  /* Is the current cell network shared */
  boolean            is_valid_nw_sharing;

  /* If the current cell is shared, then the list of shared PLMNs 
   * and their barring/forbidden info 
   */
  tdsrrc_csp_nw_sharing_entry_type  shared_list_of_plmns;

}tdsrrc_csp_acq_entry_type;


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

  /* List of ordered indices to acquisition entries */
  uint8  acq_list_indices[TDS_MAX_ACQ_DB_ENTRIES];
}tdsrrc_csp_acq_db_type;


typedef struct
{
  /* frequencies scanned during acquisition database and primary freq scan */
  tdsrrc_freq_type freq[TDS_MAX_FREQ_SCANNED_ENTRIES];

  /* array index where next frequency should be stored */
  uint8         wr_index;
} tdsrrc_csp_acq_db_freq_scanned_type;


typedef struct
{
  /* require lower band F shallow search */
  boolean     lower_band_shallow;
  /* require lower band F deep search */
  boolean     lower_band_deep;
  /* require upper band F shallow search */
  boolean     upper_band_shallow;
  /* require upper band F deep search */
  boolean     upper_band_deep;
} tdsrrc_csp_band_f_pattern_type;

typedef struct
{
  /* require band A shallow search */
  boolean     band_shallow;
  /* require band A deep search */
  boolean     band_deep;
}tdsrrc_csp_band_a_pattern_type;

typedef struct
{
  /* require band E shallow search */
  boolean     band_shallow;
  /* require band E deep search */
  boolean     band_deep;
}tdsrrc_csp_band_e_pattern_type;


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
void  tdsrrccsp_init_acq_db(void);


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
uecomdef_status_e_type  tdsrrccsp_add_acq_entry(tdsrrc_csp_acq_entry_type* acq_entry_ptr);


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
tdsrrccsp_should_freq_be_scanned(rrc_requested_plmn_type req_plmn, tdsrrc_freq_type freq);


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
tdsrrc_csp_acq_entry_type*
tdsrrccsp_get_first_match_acq_entry(rrc_plmn_identity_type req_plmn_id, tdsrrc_csp_acq_e_type req_type_acq);


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
tdsrrc_csp_acq_entry_type*
tdsrrccsp_get_next_match_acq_entry(rrc_plmn_identity_type req_plmn_id, tdsrrc_csp_acq_e_type req_type_acq);


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
void  tdsrrccsp_read_nv_acq_info(void);


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
void  tdsrrccsp_write_nv_acq_info(void);

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
boolean tdsrrccsp_should_freq_be_skipped(tdsrrc_freq_type freq, uint16 resolution);
/*===========================================================================

FUNCTION          TDSRRCCSP_PRIMIARY_FREQ

DESCRIPTION       This function determines whether the frequency is in 
                  primiary frequency list
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - frequency in primiary frequency list
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
boolean tdsrrccsp_is_primiary_freq(tdsrrc_freq_type freq);

/*===========================================================================

FUNCTION          RRC_CSP_ASSERT_ACQ_DB_VARS

DESCRIPTION       This function is used to check the sanity of acq db entries

DEPENDENCIES      NONE

RETURN VALUE      number of entries in ACQ DB
                  
SIDE EFFECTS      NONE

===========================================================================*/
int tdsrrccsp_assert_acq_db_vars(void);


/*===========================================================================

FUNCTION           RRCCSP_ADD_ACQ_DB_AND_NW_DB_ENTRIES

DESCRIPTION        Adds an entry into the NW sharing DB at the given index Note that one-one mapping
                   is maintained between ACQ DB and NW sharing DB.

DEPENDENCIES       None

RETURN VALUE       Success/Failure. Failure will be an extra-ordinary event and
                   could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS       None
===========================================================================*/
uecomdef_status_e_type tdsrrccsp_add_acq_db_and_nw_db_entries(
                                                           tdsrrc_csp_acq_entry_type* acq_entry_ptr,
                                                           tdsrrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr);

/*===========================================================================

FUNCTION          RRCCSP_NW_SHARING_PRINT_ENTRY

DESCRIPTION       Prints a given network sharing entry

DEPENDENCIES      NONE

RETURN VALUE      None
                  
SIDE EFFECTS      NONE

===========================================================================*/
void tdsrrccsp_nw_sharing_print_entry(tdsrrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr);

/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_EQUIVALENT_TO_ANY_OF_SHARED_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are equivalent

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean tdsrrccsp_nas_plmn_equivalent_to_any_of_shared_bcch_plmns(
                                                               rrc_plmn_identity_type nas_plmn, 
                                                               uint8 index_in_acq_or_shared_db);

/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_MATCH_ANY_OF_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are same.

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean tdsrrccsp_nas_plmn_match_any_of_bcch_plmns(
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
boolean tdsrrccsp_is_any_of_bcch_plmns_in_bplmn_list(tdsrrc_csp_nw_sharing_entry_type bplmn_nw_db_entry);


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
tdsrrccsp_add_freq_bands_from_acq_db_to_band_list
(
  tdsrrc_csp_band_class_type*    band_list,
  uint8 num_of_bands_already_present
);


/*===========================================================================

FUNCTION          RRC_CSP_LFS_UPDATE_LIST_DB_WITH_TARGET_TDD_LIST

DESCRIPTION       This function populates LIST DB with target TDD list given 
                  by GSM-RR and also returns pointer to the first entry in LIST 
                  DB. If LIST DB is empty then NULL is returned.

DEPENDENCIES      This function is called only for G2TD redirection search and 
                  also only after initiating target TDD cell list from GSM.

RETURN VALUE      Pointer to first entry of LIST DB.
                  Otherwise, NULL if LIST DB is empty
                  
SIDE EFFECTS      NONE

===========================================================================*/
tdsrrc_csp_acq_entry_type *tdsrrccsp_lfs_update_list_db_with_target_tdd_list(void);

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
void tdsrrccsp_update_match_acq_entries_into_list_db(void);
/*===========================================================================

FUNCTION          TDSRRCCSP_ADD_PRIM_FREQ_INTO_LIST_DB

DESCRIPTION       This function is used by CSP to add primiary frequency to
                  CSP list scan database

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/

void tdsrrccsp_add_prim_freq_into_list_db(void);
#endif
