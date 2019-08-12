#ifndef RRCSIBDBI_H
#define RRCSIBDBI_H
/*===========================================================================
       R R C   S I B   D A T A B A S E   I N T E R N A L   H E A D E R 

DESCRIPTION

  This module contains the internal definitions for the SIB database
  modules. This module is visible only to the SIB processing modules.
  It is not to be used by any other RRC module.
  
Copyright (c) 2000-2004, 2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibdbi.h_v   1.10   05 Feb 2002 17:15:20   krishnan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsibdbi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/12/13   sn      Changes to not invalidate SIBDB when MIB value tag matches
01/03/13   sr      Made changes to remove the ASN functions used for encoding of SIBs and modified the
                   processing of CSFB system information container accordingly as part of memrory optimization
12/07/12   sn      CSG feature changes
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
05/08/09   ss      Updated Copyright Information
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE.SIB11BIS.SUPPORT 
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
09/25/06   kp      Added support for six hours sib re-read & sib_change_ind interface change
06/07/06   kp      Added support for six hours sib update
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1. 
13/09/04   kc      Created a ENUM type for timer-based-sib reading during 
                   value-tag comparison.
09/13/04   sgk     Changed macro SIBDB_VALID_SIB_RANGE to check if sib type
                   <= rrc_SIB_Type_spare2 instead of RRC_TOT_SIBS to fix lint 
                   error Constant out of range for operator '<' and Converting
                   enum to int.
08/19/04   sgk     Moved prototype of rrc_sibdb_free_cells to rrcsibdb_v.h
06/01/04   vn      Added a field num_stored_cells in rrc_sibdb_type. Changed
                   SIBDB_MAX_CELLS when FEATURE_SIB_HEAP is defined.
04/02/04   svk     Increased the SIB DB size from 4 to 10.
03/11/04   svk     Added prototypes. When feature_RRC_SIB_HEAP is defined
                   the SIB_DB number of cells is set to 8.
05/14/03   ram     Externalized some functions that are now used by the rrcsibproc.c
                   module. Changed some prototypes corrosponding to the changes in
                   the functions.
04/30/03   ram     Fixed range checking in SIBDB_VALID_SIB_RANGE.
03/31/03   rj      Reduced SIBDB_MAX_CELLS to 4 from 8.
09/17/02   ram     Modified SIB Database structure to hold information about
                   SIB changes in the active cell.
02/05/02   ram     Externalized functions rrc_sibdb_find_cell_index and 
                   rrc_sibdb_check_if_sb_present which are used by rrcsibproc
                   module.
01/03/02   ram     Added support for SIB 15.4, SIB 18 and Scheduling Blocks 1 & 2
                   which are introduced in June '01 version of the specs.
04/30/01   ram     Externalized function rrc_sibdb_store_value_tags_for_new_cell
                   and function rrc_sibdb_compare_value_tags_for_cell.
04/07/01   ram     Updated SIB database variables to support value-tags for
                   SIBs.
03/23/01   ram     Updated file to use the internal format for cell id. The
                   external (ASN.1) format has been changed due to the usage
                   of new LED ASN.1 header files.
01/26/01   ram     Removed redundant page breaks. Updated comments.
01/25/01   ram     Externalized the sib database declaration. This is used
                   by the main SIB procedure to decode and store SIBs directly
                   into the space reserved for them.
01/09/01   ram     Externalized rrc_sibdb_create_cell_index which is called
                   by the main sib procedure.
01/05/01   ram     Updated SIB database structure to keep track of age of
                   the stored cells. Defined SIBDB_INVALID_INDEX used by
                   the interface functions when requested cells are not
                   found in the database. Updated copyright for 2001. 
12/20/00   ram     Included rrcdata_v.h which now contains the definition for
                   RRC_TOT_SIBS.
11/08/00   ram     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrcmmif.h"
#include "rrcsibdb.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_FEMTO_CSG
/* Maximum number of macro cells for which SIBs are maintained at a given time. */
#define SIBDB_MAX_MACRO_CELLS 10

/* Maximum number of CSG cells for which SIBs are maintained at a given time */
#define SIBDB_MAX_CSG_CELLS 15

/* Total number of cells */
#define SIBDB_MAX_CELLS SIBDB_MAX_MACRO_CELLS+SIBDB_MAX_CSG_CELLS
#else
#define SIBDB_MAX_CELLS 10
#endif

/* Largest index for which SIBs will be stored in the SIB database. */
#define SIBDB_MAX_INDEX (SIBDB_MAX_CELLS - 1)

/* Invalid index - used to indicate an incorrect index to the
database. Must be higher than the maximum valid index. 1 is chosen
arbitrarily. */
#define SIBDB_INVALID_INDEX (SIBDB_MAX_INDEX + 1)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* Structure of all the SIB pointers. These pointers will be passed
to the ASN.1 function which writes the decoded SIBs to these pointers.
The memory for those SIBs is also allocated by the ASN.1 pointer */

typedef struct
{
  rrc_MasterInformationBlock *mib;
  rrc_SysInfoType1           *sib1;
  rrc_SysInfoType2           *sib2;
  rrc_SysInfoType3           *sib3;
  rrc_SysInfoType4           *sib4;
  rrc_SysInfoType5           *sib5;
  rrc_SysInfoType6           *sib6;
  rrc_SysInfoType7           *sib7;
  rrc_SysInfoType11          *sib11;
  rrc_SysInfoType12          *sib12;
  rrc_SysInfoType18          *sib18;
  rrc_SysInfoTypeSB1         *sb1;
  rrc_SysInfoTypeSB2         *sb2;
  rrc_SysInfoType11bis       *sib11bis;
#ifdef FEATURE_WCDMA_TO_LTE
  rrc_SysInfoType19          *sib19;
#endif

/* To hold the encoded SIB.Maximum possible length of a encoded
     SIB is MAX_SIB_LENGTH_BYTES bytes. */
  byte mib_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 mib_encoded_pdu_length;
  byte sib1_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib1_encoded_pdu_length;
  byte sib2_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib2_encoded_pdu_length;
  byte sib3_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib3_encoded_pdu_length;
  byte sib4_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib4_encoded_pdu_length;
  byte sib5_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib5_encoded_pdu_length;
  byte sib6_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib6_encoded_pdu_length;
  byte sib7_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib7_encoded_pdu_length;
  byte sib11_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib11_encoded_pdu_length;
  byte sib12_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib12_encoded_pdu_length;
  byte sib18_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib18_encoded_pdu_length;
  byte sb1_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sb1_encoded_pdu_length;
  byte sb2_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sb2_encoded_pdu_length;
  byte sib11bis_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib11bis_encoded_pdu_length;
#ifdef FEATURE_WCDMA_TO_LTE
  byte sib19_encoded_ptr[MAX_SIB_LENGTH_BYTES];
  uint32 sib19_encoded_pdu_length;
#endif

}rrc_sib_ptr_type;


/* Structure of Database Cell Header. The cell header contains 
all the SIB pointers for a particular cell and other relevant
information about the cell */
typedef struct
{
  boolean index_used;              /* Indicates if this index is used
                                   or not */
#ifdef FEATURE_FEMTO_CSG  
  boolean is_csg; /* set to true if its a CSG cell */
  rrc_int_csg_id_type  csg_id; /* CSG Id of this cell  - internal format */
#endif
  rrc_plmn_identity_type plmn_id;  /* PLMN Id of this cell - internal format */
  rrc_int_cell_id_type cell_id;    /* Cell Id of this cell - internal format */
  uint32 scr_code;                 /* Scrambling code of this cell */
  uint32 freq;                     /* Frequency of this cell */
  uint32 age_index;                /* A number showing how long this
                                   cell has been stored in the database.
                                   This varies from 0 to SIBDB_MAX_INDEX.
                                   This is used to remove the oldest cell
                                   when the database is full. */
  boolean sib_valid[RRC_TOT_SIBS]; /* Indicates validity of each SIB */
  uint32 sib_value_tag[RRC_TOT_SIBS]; /* Holds the value tag for each
                                      SIB. Note that uint32 is used to
                                      store all value tag types to 
                                      improve speed efficiency in the ARM.
                                      The following are the ranges of the
                                      different value tags -
                                        
                                        Cell Value Tag - 1 to 4
                                        PLMN Value Tag - 1 to 256
                                        MIB Value Tag -  1 to 8 */
                                        
  rrc_sib_ptr_type sibs;           /* Points to all SIBs for this cell */

  uint8  six_hr_count ;

  uint8 ten_min_count;

}rrc_sibdb_cell_header_type;


/* Actual SIB database structure */
typedef struct
{
  boolean database_empty;          /* Indicates that the database is
                                   currently empty. Set to false when
                                   there's atleast one cell entry in
                                   the database. */
  boolean active_cell_sibs_valid;  /* Indicates that the current active
                                   cell has valid SIBs. If atleast one
                                   SIB is valid for the active cell, this
                                   is set to TRUE. */
  uint32 active_cell_index;        /* Indicates the index of the current
                                   active cell in the database */

  uint32 num_stored_cells;         /* Keeps track of total number of stored
                                    * cells in the DB at any time. */

  boolean active_cell_sib_changed[RRC_TOT_SIBS]; /* Used to indicate if
                                   any of the active cell SIBs have 
                                   changed */
  boolean last_plmn_and_cell_id_matched; /* Indicates if the last (latest)
                                   index selected for storing cells already
                                   has stored SIBs for a matching PLMN and Cell
                                   id. This indicates to the sib event controller
                                   whether value-tag matching is needed or not */
#ifdef FEATURE_FEMTO_CSG
  uint32 num_csg_cells; /* Keeps track of number of stored CSG cells */
#endif
  rrc_sibdb_cell_header_type cell[SIBDB_MAX_CELLS];
                                   /* Cell Headers - for SIBDB_MAX_CELLS
                                   number of cells. */
#ifdef FEATURE_3GPP_CSFB
  uint8                            sib_cont_sibdb_index;
                                    /* SIB DB Index holding the stored SIB container info */
    
  rrc_System_Information_Container* rrc_decoded_sib_cont;
                                     /* Pointer used to store the decoded system info container */
#endif
}rrc_sibdb_type;   



/* Externalize the sib database declaration. This is shared only by the
other SIB related procedures. */
extern rrc_sibdb_type sibdb;

/* Enumerated type to indicate the results of a value tag matching
process */
typedef enum
{
  VALUE_TAG_MATCHED = 1,           /* The new value tag matched the stored value tag      */
  VALUE_TAG_DID_NOT_MATCH,         /* The new value tag didn't match the stored value tag */
  NO_SIB_FOR_THIS_VALUE_TAG,       /* There was no sib in the database for this value tag */
  TIMER_BASED_SIB_RCVD,            /* This is a timer based SIB, SIB not already recvd    */
  VALUE_TAG_NOT_PRESENT            /* the new value tag was not present in the ref list   */
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , DEFERRED_SIB_RCVD               /* The SIB was deferred. Need to read it again. */
  #endif
}rrc_sibdb_value_tag_result_e_type;

/* Structure containing a list of value tag results for each SIB, to be
filled in by the procedure doing value tag comparisons */
typedef struct
{
  rrc_sibdb_value_tag_result_e_type result[RRC_TOT_SIBS]; /* Result for each SIB */
}rrc_sibdb_value_tag_results_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/*========================================================================
MACRO SIBDB_VALID_INDEX

ARGS
  index - a uint32 value.

DESCRIPTION:
  This macro returns TRUE if the index is a valid SIB database index.
  Else it returns FALSE.
========================================================================== */
#define SIBDB_VALID_INDEX( index ) \
   (((index) < SIBDB_INVALID_INDEX ) ? (TRUE) : (FALSE)) 


/*========================================================================
MACRO SIBDB_VALID_SIB

ARGS
  sib - a uint32 value indicating the sib number.

DESCRIPTION:
  This macro returns TRUE if the number is a valid sib.
  Else it returns FALSE.
========================================================================== */
#define SIBDB_VALID_SIB_RANGE( sib ) \
   (((sib) < rrc_ExtensionType ) ? (TRUE) : (FALSE)) 


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION RRC_SIBDB_INIT

DESCRIPTION

  This function initializes the SIB database. This is called
  once during power-up. Note that individual cell headers are
  not initialized at this time. They are initialized each time
  a new cell is selected and SIBs need to be stored in the cell
  headers.
  
DEPENDENCIES

 None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_init( void );




/*===========================================================================

FUNCTION RRC_SIBDB_CREATE_CELL_INDEX

DESCRIPTION

  This function creates a new cell index for the given
  PLMN and CELL id if needed. The function first checks
  if the index for this PLMN and Cell id combination is
  already present in the database. If so, it adjusts the
  "age_index" of the cells accordingly and returns the
  existing index. If a match is not found a new cell index
  is created and returned to the calling function.
  If the database is full, the oldest cell is removed and
  that index is used for the new cell.
    
DEPENDENCIES

  SIB Database should be correctly initialized. The PLMN
  and Cell Ids given by the function should be in the
  database.

RETURN VALUE

  The newly created index for the given cell. This ranges
  from 0 to SIBDB_MAX_INDEX.

SIDE EFFECTS

  None.

===========================================================================*/
uint32 rrc_sibdb_create_cell_index
(
  rrc_plmn_identity_type plmn,    /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell,       /* The cell id for the cell */
  sib_events_index_e_type event_index /*Current avtice sib event*/
#ifdef FEATURE_FEMTO_CSG
  ,rrc_int_csg_id_type csg
  ,boolean is_csg
#endif
);


/*===========================================================================

FUNCTION RRC_SIBDB_COMPARE_VALUE_TAGS_FOR_CELL

DESCRIPTION

  This function compares the stored value tags for a particular
  cell with the newly received value tags in the MIB or the
  Scheduling blocks.
  
  If the new value tags don't match the stored value tags, 
  this function frees the space occupied by the SIBs so that
  the new SIBs can be stored in their place.

  NOTE: THE FOLLOWING SIBS DO NOT HAVE VALUE-TAGS
  
    SIB7, SIB9, SIB10, SIB14, SIB17
    
  These SIBs only have expiration timers.

DEPENDENCIES

  SIB Database should be correctly initialized.

  The result_ptr should be initialized by the calling function.
  The results of the value tag comparisons will be stored in
  the result_ptr.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_compare_value_tags_for_cell
(
  uint32 dbindex,     /* SIB Database Index for the cell for which the
                      new Value Tags have been received */
  rrc_SIB_type sib,   /* Indicate which SIB the value-tags are from
                      This can be the MIB or SB1 or SB2. */
  void *block_ptr,    /* Pointer to the MIB, SB1 or SB2, typecast
                      to void */
  rrc_sibdb_value_tag_results_type *result_ptr, /* Pointer to store
                      the results of value tag comparisons */
/*Need a flag to indicate forcibly re-read all sibs*/
  rrcsib_vtag_match_type  vtag_match_type,

  sib_events_index_e_type event_index

);

/*===========================================================================

FUNCTION RRC_SIBDB_STORE_VALUE_TAGS_FOR_NEW_CELL

DESCRIPTION

  This function stores value tags for a new cell for which
  a MIB or a Scheduling Block was received. When a cell is 
  acquired, and the PLMN and Cell id are known, the sib procedure
  will try to find a match in the sib database. If a match is
  found rrc_sibdb_process_value_tags_for_cell is called to
  compare value_tags. If a match is not found, this function
  should be called so that the value_tags can be copied into
  the database.
    
  NOTE: THE FOLLOWING SIBS DO NOT HAVE VALUE-TAGS
  
    SIB7, SIB9, SIB10, SIB14, SIB17
    
  These SIBs only have expiration timers.

DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_store_value_tags_for_new_cell
(
  uint32 dbindex      /* SIB Database Index for the cell for which the
                      new Value Tags have been received */
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , sib_events_index_e_type index
  #endif
);    






/*===========================================================================

FUNCTION RRC_SIBDB_NOTIFY_SIB_CHANGE_TO_PROCS

DESCRIPTION

  This function looks through the stored sib change registrations
  to see if the sib changes should be notified to any of
  the registered procedures. This is called by the sib procedure
  when the RRCSIB_UPDATE_SIBS event is active and all changed
  sibs haave been decoded.

  If it finds procedures that need to be notified, it sends
  an internal command RRC_SIB_CHANGE_IND to the internal command
  queue.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_notify_sib_change_to_procs( void );



/*===========================================================================

FUNCTION RRC_SIBDB_FIND_CELL_INDEX

DESCRIPTION

  This function tries to find the index in the sib
  database for the given cell id and PLMN id combination.
  If a match is found, the correct index is returned.
  
DEPENDENCIES

  SIB Database should be correctly initialized. The PLMN
  and Cell Ids given by the function should be in the
  database.

RETURN VALUE

  Index of the cell. If this cell is not present
  in the SIB database, SIBDB_INVALID_INDEX is returned.

SIDE EFFECTS

  None.

===========================================================================*/
uint32 rrc_sibdb_find_cell_index
(
  rrc_plmn_identity_type plmn,    /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell       /* The cell id for the cell */
);


/*===========================================================================

FUNCTION RRC_SIBDB_CHECK_IF_SB_PRESENT

DESCRIPTION

  This function checks if a Scheduling Block (1 or 2) is
  present in a particular cell. This is done by looking 
  through the MIB's scheduling information.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  TRUE if the SB is present. FALSE otherwise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_sibdb_check_if_sb_present
(
  rrc_SIB_type sib, /* Either SB1 or SB2 */ 
  uint32 dbindex    /* Database index    */
);


/*===========================================================================

FUNCTION RRC_SIBDB_CLEAR_ACTIVE_CELL_SIB_CHANGE

DESCRIPTION

  This function clears the variable that saves
  the active cell SIB change indications.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  TRUE if the SB is present. FALSE otherwise.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_clear_active_cell_sib_change( void );



/*===========================================================================

FUNCTION RRC_SIBDB_SAVE_ACTIVE_CELL_SIB_CHANGE

DESCRIPTION

  If a SIB in the active cell is changed by the UTRAN this function
  is called to save the information about the SIB that has changed.
  Later when all the changed SIBs have been read, the change information
  is sent to other RRC procedures if needed.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_save_active_cell_sib_change
( 
  rrc_SIB_type sib   /* The sib that has changed.*/
);


/*===========================================================================

FUNCTION RRC_SIBDB_FREE_SIB

DESCRIPTION

  This function frees the memory used by a System Information
  Block stored in the SIB database using the ASN.1 free pdu function.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_free_sib
(
  int pdu_num,    /* PDU number defined by ASN.1 for this SIB */
  void *sib_ptr   /* Pointer to the SIB typecast to void.     */
);



/*===========================================================================

FUNCTION RRC_SIBDB_CELL_HEADER_INIT

DESCRIPTION

  This function initializes a cell header within the SIB database.
  This function is called before storing SIBs for a new cell in the
  SIB database.
  
DEPENDENCIES

 None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_cell_header_init
(
  uint32 index /* Index of the cell header to be initialized */
);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================

FUNCTION rrc_sibdb_copy_deferred_sibdb_to_tsibdb

DESCRIPTION

  This function copies SIB segments from Deferred SIB optimization DB to TSIB.

DEPENDENCIES
none

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void  rrc_sibdb_copy_deferred_sibdb_to_tsibdb
(
  uint32 dbindex,     /* SIB Database Index for the cell for which the
                      new Value Tags have been received */
  sib_events_index_e_type event_index,
  void *block_ptr,    /* Pointer to the MIB, SB1 or SB2, typecast
                      to void */
  rrc_SIB_type sib   /* Indicate which SIB the value-tags are from
                      This can be the MIB or SB1 or SB2. */
);
#endif
#endif /* RRCSIBDBI_H */

