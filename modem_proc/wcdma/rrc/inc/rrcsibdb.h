#ifndef RRCSIBDB_H
#define RRCSIBDB_H
/*===========================================================================
              R R C   S I B   D A T A B A S E   H E A D E R   M O D U L E 

DESCRIPTION

  This module is the header module for the SIB Database module. This
  contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2000-2004,2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibdb.h_v   1.6   31 May 2002 18:18:42   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcsibdb.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/16   sn      Changes for FR30881
12/07/12   sn      CSG feature changes
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
10/15/09   dm      Added preprocessor directive for pending update sibs timer
10/12/09   kp      Added support to re-read all the SIB's when UE come back from LPM.
05/08/09   ss      Updated Copyright Information
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
09/29/08   kp      Added function to print cell id,scr-code 7 plmn from sibdb for debugging
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE.SIB11BIS.SUPPORT 
03/07/08   vr\da   Added function to return SIB DB index based on Freq and PSC. Support 
                   added under FEATURE_MODEM_MBMS
12/14/07   kp      Added function to provide status of active cell SIB valid in SIBDB..
07/17/07   kp      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
09/26/06   kp      Moved macro MAX_CELL_INDEX_COUNT_SIXHR from FEATURE SIX HRS SIB RE READ 
                   to FEATURE SIX HRS SIB CLEAN UP, as both feature flag use it.
09/25/06   kp      Added support for six hours sib re-read & sib_change_ind interface change
06/07/06   kp      Added support for six hours sib update
03/22/06   kp      Added a function which returns mib value tag for serving cell
11/18/04   kc      Added interface fn. to clear serving cell status when we go from WtoG
                   or due to OOS
09/13/04   sgk     Added prototype for rrc_sibdb_adjust_age_index and
                   rrc_sibdb_invalidate_mib_sb_value_tags_for_interrupted_sib_update
                   as it was called in sibproc.c, to fix lint error.
08/19/04   sgk     Added prototype for rrc_sibdb_free_cells 
08/10/04   kc      Added prototypes to get sib_type from SibSB_type and Sib_type_and_tag
                   type
12/16/03   svk     Modified prototype rrc_sibdb_find_cell_index_for_freq_psc to 
                   rrc_sibdb_find_cell_index_for_freq_psc to find the db index based
                   on plmn_id, freq, psc.
12/03/03   ram     Added prototype rrc_sibdb_find_cell_index_for_freq_psc to find
                   the database index for a given freq and scr code.
02/13/03   ram     Added rrcdata.h needed for GSTK module function call.
02/10/02   ram     Added a prototype rrc_get_serving_cell_id for GSTK module
                   in order to support the USAT feature.
05/07/02   ram     Externalized functions to register and deregister for
                   SIB change notifications. Other RRC procedures can use
                   these to be notified of SIB changes.
02/05/02   ram     Updated copyright.
03/23/01   ram     Updated file to use the internal format for cell id. The
                   external (ASN.1) format has been changed due to the usage
                   of new LED ASN.1 header files. Removed unused structure
                   rrc_sibdb_entry_id_type.
01/26/01   ram     Removed function prototype rrc_sibdb_init since it's
                   not called by modules external to SIB processing. Updated comments.
01/04/01   ram     Updated function prototypes as per the changes in rrcsibdb.c.
                   Updated copyright.
12/20/00   ram     Externalized function prototypes for functions that are
                   called by modules external to the SIB procedure.
10/30/00   ram     Created file.


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
#include "rrctmr.h"
#include "rrcsibproci.h"
#include "rrcsibproc.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*The Index Value is set to 33(Multiplied by 10min) which is 5.30.Hrs that is less than 6hr
   Bcoz for serving cell we will update sibs once UE does Leaving Connected mode
   scan for going to IDLE.In case of UE is IDLE RRC connection may be established for doing 
   LAU or RAU, Since the periodicity at which these(LAU/RAU) are done is tunable by N/W, 
   So setting this timer half an hours(choosing half an hour doesnt have any logic) before its expiry*/
 #define MAX_CELL_INDEX_COUNT_SIXHR_CELL_SEL_RESEL 33

/*Max Six hours sib cell counter = six hours*/
 #define MAX_CELL_INDEX_COUNT_SIXHR 36

/*This is a 10min Timer, which is used to implemnt 6 hrs SIB refresh timer*/
 #define RRCSIB_SIX_HR_SIB_UPDATE_TIMER 600000


/*invalid Value_tag Value for SIB*/
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* data struct to convert sib_sb type to sib_type and also to get value tags*/
typedef struct
{
  rrc_SIB_type sib_type;
  uint32 sib_tag;
}rrcsib_sib_type_vtag;  

typedef enum
{
  RRCSIB_READ_SIBS_AFTER_VTAG_MATCH,
  RRCSIB_READ_ALL_SIBS
}rrcsib_vtag_match_type;  

 #define RRCSIB_SIB_CHANGE_GUARD_TIMER_VALUE 1000
 #define RRCSIB_PENDING_UPDATE_SIB_TIMER_VALUE 100

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
 #define RRCSIB_SIB_CHANGE_GUARD_TMR_FOR_DEFERRED_SIB 100
#endif

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
 
/*===========================================================================

FUNCTION RRC_SIBDB_RETURN_SIB_FOR_DB_INDEX

DESCRIPTION

  This function returns a pointer to the requested SIB for
  the requested cell index in the SIB database. If there is no cell
  for that particular index, or if this particular SIB has not been read 
  completely for that cell, this function returns
  NULL.
  
  The pointer is typecast to void since different sibs have
  different type definitions. The calling function is responsible
  for typecasting the pointer to the required SIB type.
   
  Caution: The calling function should read all the required
  data from the pointer as soon as the function returns since
  the SIB data could be overwritten if the serving cell changes
  or if a SIB change indication has been received.
  
DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read or if there
  is no cell stored at the requested index.

SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_sibdb_return_sib_for_db_index
(
  rrc_SIB_type  req_sib,          /* The SIB that is required */
  uint32        rrc_sibdb_index   /* Index into the SIB database for
                                  the cell from which the SIB is required.
                                  Must be between 0 and RRC_MAX_SIBDB_INDEX */
);




/*===========================================================================

FUNCTION RRC_SIBDB_RETURN_SIB_FOR_CELL

DESCRIPTION

  This function returns a pointer to the requested SIB for
  For a requested Cell id and PLMN Id. If there are no sibs stored
  for that particular PLMN and Cell id combination, or if this particular 
  SIB has not been read completely for that cell, this function returns
  NULL.
  
  The pointer is typecast to void since different sibs have
  different type definitions. The calling function is responsible
  for typecasting the pointer to the required SIB type.
   
  Caution: The calling function should read all the required
  data from the pointer as soon as the function returns since
  the SIB data could be overwritten if the serving cell changes
  or if a SIB change indication has been received.
  
DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read or if there
  is no cell stored at the requested index.

SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_sibdb_return_sib_for_cell
(
  rrc_SIB_type  req_sib,          /* The SIB that is required */
  rrc_plmn_identity_type plmn,    /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell       /* The cell id for the cell */
);


/*===========================================================================

FUNCTION RRC_SIBDB_RETURN_SIB_FOR_SRV_CELL

DESCRIPTION

  This function returns a pointer to the requested SIB for
  the current serving cell. If there is no serving cell
  selected, or if this particular SIB has not been read 
  completely for the serving cell, this function returns
  NULL.
  
  The pointer is typecast to void since different sibs have
  different type definitions. The calling function is responsible
  for typecasting the pointer to the required SIB type.
  
  Caution: The calling function should read all the required
  data from the pointer as soon as the function returns since
  the SIB data could be overwritten if the serving cell changes
  or if a SIB change indication has been received.

DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read or if there
  is no serving cell selected.

SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_sibdb_return_sib_for_srv_cell
(
  rrc_SIB_type  req_sib  /* The SIB that is required */
);

/*===========================================================================

FUNCTION rrc_sibdb_return_psuedo_sib19

DESCRIPTION


DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_sibdb_return_psuedo_sib19
(
  void
);
/*===========================================================================

FUNCTION rrc_sibdb_is_sib11bis_present_in_srv_cell

DESCRIPTION

  This function returns TRUE is SIB11bis is present in serving cell.
  other wise it returns FALSE
DEPENDENCIES
none
RETURN VALUE

TRUE is SIB11bis is present in serving cell.
FALSE: other wise 

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_sibdb_is_sib11bis_present_in_srv_cell
(
  void
);

/*===========================================================================

FUNCTION RRCSIB_REGISTER_FOR_SIB_CHANGE_NOTIFICATION

DESCRIPTION

  This function registers SIB change notification requests from 
  other procedures. When a procedure needs to be notified when a 
  particular SIB in the serving cell changes, that procedure must register
  using this function. 

  Only when the SIBs change in the serving cell the procedures
  will be notified. When the serving cell itself changes, the procedures
  are not notified - they must use a NEW_CELL_INDICATION notice for
  that.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_register_for_sib_change_notification
(
  rrc_proc_e_type  procedure,   /* Procedure that's doing this registration*/
  rrc_SIB_type sib              /* Sib for which notification is needed    */
);





/*===========================================================================

FUNCTION RRCSIB_DEREGISTER_SIB_CHANGE_NOTIFICATION

DESCRIPTION

  This function de-registers previously registered SIB change 
  notification requests. When a procedure no longer needs to be 
  notified of sib changes, it may de-register using this
  function.
   
  This is not mandatory but if the procedure does 
  not de-register, it will continue to receive sib change
  notifications.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_deregister_sib_change_notification
(
  rrc_proc_e_type  procedure,   /* Procedure that's de-registering         */
  rrc_SIB_type sib              /* Sib for which notification is not needed*/
);


/*===========================================================================

FUNCTION RRC_GET_SERVING_CELL_ID

DESCRIPTION

  This function gets the current serving cell's id in the
  format of an unsigned char array of 2 elements with the
  LSB 8 bits of the cell-id in the first element and the 
  2nd LSB 8 bits of the cell-id in the second element.
  Only the first 16 bits of the cell-id are used - the rest
  if any, are ignored.

  It will also fill in the length in bits of the cell id.

  At present this function is meant to be used only by
  GSTK module.

DEPENDENCIES

  The calling function is responsible for allocating 
  the appropriate amount of space to the 2 pointers that are
  input to this function.
  
RETURN VALUE
  
  TRUE if cell-id has been copied, FALSE if it's not.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_get_serving_cell_id
( 
  uint8* cell_id, /* Cell Id to be copied */
  uint8* length   /* Length of the Cell Id to be copied */
);


/*===========================================================================

FUNCTION RRC_GET_SERVING_CELL_MIB_VALUE_TAG

DESCRIPTION

  This function gets the current serving cell's 
  MIB Value tag to the calling function 

DEPENDENCIES
  None
  
RETURN VALUE
  
  Mib Value tag of serving cell index.

SIDE EFFECTS

  None.

===========================================================================*/
  uint32 rrc_get_serving_cell_mib_value_tag(void);


/*===========================================================================

FUNCTION RRC_SIBDB_FIND_CELL_INDEX_FOR_PLMN_FREQ_PSC

DESCRIPTION

  This function tries to find the index in the sib
  database for the given frequency and primary scrambling
  code combination.
  
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
uint32 rrc_sibdb_find_cell_index_for_plmn_freq_psc
(
  rrc_plmn_identity_type plmn,    /* The PLMN Id for the cell */ 
  uint32 freq,    /* Frequency of the cell */
  uint32 scr_code,  /* PSC of the cell       */
  sib_events_index_e_type  event_index /*Current sib event index*/
);

/*===========================================================================

FUNCTION rrcsib_get_sib_type_from_sib_type_and_tag

DESCRIPTION

  Helper function to get sib_type from sibsb type received in
  MIB
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_get_sib_type_from_sib_type_and_tag
(
  rrc_SIB_TypeAndTag * sib_type_and_tag,
  rrcsib_sib_type_vtag * sib_type_tag
);
/*===========================================================================

FUNCTION rrcsib_get_sib_type_from_sibsb_type_and_tag

DESCRIPTION

  Helper function to get sib_type from sibsb type received in
  SB1 and SB2 functions
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_get_sib_type_from_sibsb_type_and_tag
(
  rrc_SIBSb_TypeAndTag * sib_sb_type_tag,
  rrcsib_sib_type_vtag * sib_type_tag
);

/*===========================================================================

FUNCTION rrc_sibdb_free_cells

DESCRIPTION

DEPENDENCIES

RETURN VALUE

  boolean indicating if SIBDB was able to free up the space required to decode the
  newly arrived SIB's. 

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_sibdb_free_cells
(
  void
);

/*===========================================================================

FUNCTION RRC_SIBDB_ADJUST_AGE_INDEX

DESCRIPTION

  This function adjusts the "age_index" for the cells in the SIB
  database. The age_index is a number that indicates how long a cell
  has been stored in the database. The newest cell has the age
  index of 0, and each previous cell has an index one higher than
  the newest cell.
  
  If the database is full, the oldest cell has the age index of 
  SIBDB_MAX_INDEX.  
    
  The index of the newest cell is passed to this function as soon as
  it is created. NOTE: Only rrc_sibdb_create_cell_index can call this
  function.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_adjust_age_index
(
  uint32 new_cell_index  /* Index of the new cell that has just been created */
);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION RRC_SIBDB_DECREMENT_AGE_INDEX

DESCRIPTION
rrc_sibdb_adjust_age_index doesnt update ages properly when a SIBDB entry is 
removed. This function correctly adjusts the age.
 
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_decrement_age_index
(
  uint32 new_cell_index  /* Index of the new cell that has just been created */
);
#endif

/*===========================================================================

FUNCTION rrc_sibdb_invalidate_mib_sb_value_tags_for_interrupted_sib_update

DESCRIPTION

  This function invalidates the MIB and SB's value-tags in case SIB_UPDATE_IND
  is interrupted by a higher priority n_bcch event.  Invalidation of the MIB
  and SB value tags, cause the SIB's in the reference-list and SIB's in SB's to
  be compared for value-tags afresh.  This is needed bcos, it could so happen that
  in the earlier run which was interrupted, SB and MIB were read but all the SIB's
  that were changed are not completely read.
  
DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_invalidate_mib_sb_value_tags_for_interrupted_sib_update
(
  uint32 dbindex     /* SIB Database Index for the cell for which the
                        new Value Tags have been received */
);

/*===========================================================================

FUNCTION rrc_sibdb_clear_active_cell_status

DESCRIPTION

  This function reset's the sib-db active cell flags.
  This is done whenever we go OOS or whenever we go from WtoG
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_clear_active_cell_status
( 
  void 
);


/*===========================================================================

FUNCTION rrc_sibdb_update_six_hr_timer

DESCRIPTION

  This function will Implement the required six hours 
  Timer for each of the valid sib index.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/

void  rrc_sibdb_update_six_hr_timer
(
void
);

/*===========================================================================

FUNCTION rrcsib_clear_a_cell_index

DESCRIPTION

  This function will clear all the SIB's/MIB/SB's for the given cell inedx
  for the SIB db.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrc_sibdb_clear_a_cell_index(
uint32 dbindex
);

/*===========================================================================

FUNCTION rrc_sibdb_find_valid_cell_index

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
uint32 rrc_sibdb_find_valid_cell_index
(
  rrc_plmn_identity_type plmn,    /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell,       /* The cell id for the cell */
  sib_events_index_e_type event_index /*Current avtice sib event*/
);
/*===========================================================================

FUNCTION rrcsib_encode_sib_change_mask

DESCRIPTION

  This function prepares sib change indication bit mask.
  which will be sent to all the procedures which have registered 
  with sib for a notification
  
DEPENDENCIES
  None.

RETURN VALUE
none

SIDE EFFECTS
  None.
     
===========================================================================*/
uint8 rrcsib_check_and_prepare_sib_change_mask
(
rrc_sib_change_mask_type *sib_change_mask,
rrc_proc_e_type procedure,
uint8 priority
);

/*===========================================================================

FUNCTION rrcsib_is_it_registered_procedure_for_sibchange

DESCRIPTION

  This function will loop through regestered procedure 
  data base for sib change to look whether the passed 
  procedure has regestered for sib change
  
DEPENDENCIES
  None.

RETURN VALUE
TRUE : If Sibs have changed for this registered procedure
FALSE: Otherwise

SIDE EFFECTS
  None.
     
===========================================================================*/
uint8 rrcsib_is_it_registered_procedure_for_sibchange
(
rrc_proc_e_type procedure
);
/*===========================================================================

FUNCTION rrcsib_set_priority

DESCRIPTION

  This function will set a priority for reveived sib type
  for registration which will be used for posting sib change ind
  to procedures when ever these sibs are changed.
DEPENDENCIES
  None.

RETURN VALUE
none
SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_set_priority
(
  uint32 index,
  rrc_SIB_type sib
);
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
/*===========================================================================

FUNCTION rrcsib_is_serving_cell_sib_vaild

DESCRIPTION

  This function will return the status of serving cell SIB information. 
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:If SIB db index for serving cell is valid
  FALSE:otherwise
SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_serving_cell_sib_vaild
(
  void
);

/*===========================================================================

FUNCTION RRC_SIBDB_GET_SIB_PTR_FOR_CELL

DESCRIPTION

  This function returns a pointer to the requested SIB 
  for the requested cell. The pointer is typecast
  to void since different sibs have different type
  definitions. The calling function is reponsible for
  typecasting the returned pointer to the right type.
  
  The calling function MUST verify that the SIB database
  index passed to this function is a valid index
  created by this module.
  
DEPENDENCIES

  SIB Database should be correctly initialized.

RETURN VALUE

  A void pointer that points to the requested SIB in the
  given SIB database index.
  
SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_sibdb_get_sib_ptr_for_cell
(
  uint32 index,    /* Index for the requested cell */
  rrc_SIB_type sib /* The SIB that's requested */
);

/*===========================================================================

FUNCTION rrc_sibdb_decode_all_sibs_for_a_cell

DESCRIPTION

  This function ASN.1 decode all sibs for a cell. 
  It uses ASN.1 encoded messages which are stored in sibdb 
  
DEPENDENCIES

 None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_decode_all_sibs_for_a_cell
(
  uint32 index /* Index of the cell header to be initialized */
);
/*===========================================================================

FUNCTION rrc_sibdb_free_all_encodedsibs_for_a_cell

DESCRIPTION

  This function will free all ASN.1 decode sibs for a cell. 
  
DEPENDENCIES

 None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sibdb_free_all_decodedsibs_for_a_cell
(
  uint32 index /* Index of the cell header to be initialized */
);

/*===========================================================================

FUNCTION RRCSIB_IS_SIB_SCHEDULED

DESCRIPTION

  This function checks the scheduling information present
  in the current cell to determine if the given SIB is
  scheduled by the MIB or one of the scheduling blocks.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is the SIB is scheduled, FALSE if not scheduled.

SIDE EFFECTS

  None.
   
===========================================================================*/
boolean rrcsib_is_sib_scheduled
(
  rrc_SIB_type sib, /* SIB to be checked */
  uint32 dbindex    /* Index to the database */
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , boolean for_deferred_sib  /* Whether this is called for checking 
                                 the scheduling info for Deferred SIBs*/
  , rrc_SIB_type *def_sib_stored_in_sb1_sb2 /* Scheduling block where the scheduling 
                                   info for deferred SIBs is present */
#endif
);

/*===========================================================================
FUNCTION rrcsib_print_cellid_psc_plmnid_from_sibdb

DESCRIPTION

  This function will prints cell id, plmn id & PSC from sib db
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_print_cellid_psc_plmnid_from_sibdb
(
  void
);
/*===========================================================================

FUNCTION rrc_sibdb_clear_cell_header

DESCRIPTION

  Clear all cell header in sibdb
  
DEPENDENCIES

 None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  This will make SIB to reread all sib's.

===========================================================================*/
void rrc_sibdb_clear_cell_header
(
  void
);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================
FUNCTION rrc_sibdb_return_intra_freq_meas_quan_for_deferred_sib

DESCRIPTION

  This function returns the Intra Freq Meas Quantity to be used during measurement reporting in RACH
  for Deferred SIB 11/11bis/12reading. This IE is fetched from SIB3.

DEPENDENCIES
none

RETURN VALUE

L1_EC_NO: if Meas Quantity indicated in SIB3 is Ec/N0.
L1_RSCP:   if Meas Quantity indicated in SIB3 is RSCP or IE intraFreqMeasQuantity_FDD is not present in SIB3.

SIDE EFFECTS
  None.

===========================================================================*/
l1_meas_quan_enum_type rrc_sibdb_return_intra_freq_meas_quan_for_deferred_sib
(
  void
);
#endif

/*===========================================================================

FUNCTION rrcsibdb_free_pseudo_sib19

DESCRIPTION

  This function frees pseudo SIB19 pointer
  
DEPENDENCIES

RETURN VALUE
  none.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsibdb_free_pseudo_sib19
(
  void
);

/*===========================================================================

FUNCTION rrcsibdb_get_pseudo_sib19

DESCRIPTION

  This function returns a pointer to the SIB19 generated
  from NV and NAS EARFCN DB
  
DEPENDENCIES

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read or if there
  is no serving cell selected.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_SysInfoType19 * rrcsibdb_get_pseudo_sib19
(
  uint32 cell_id,
  rrc_plmn_identity_type plmn_id
);
#endif /* RRCSIBDB_H */
