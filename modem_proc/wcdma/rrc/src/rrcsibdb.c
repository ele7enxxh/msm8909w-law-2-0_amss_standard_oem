/*===========================================================================
              R R C   S I B   D A T A B A S E   M O D U L E 

DESCRIPTION

  This module is part of the SIB processing procedure. This module containts
  the SIB database and utility functions related to the SIB database.
  
  The SIB database stores the SIBs from the last few cells that the UE
  has camped on. The number of cells for which the SIBs are stored is
  configurable. The SIB database is used to improve the efficiency of
  the UE since the UE does not have to read SIBs in a cell for which SIBs
  are already stored in the database, although it has to check if the
  SIBs are current.
  
  The SIB database is indexed using the combination of the Cell Id and the
  PLMN Id. This combination is unique for every cell in the UMTS system.
  When the UE moves to a new cell, it reads the MIB from the BCCH and
  retrieves the Cell Id and the PLMN Id. It then searches the database
  to see if there is a match. If so, it only has to check the value
  tag of the SIBs with the new value tags received in the MIB and make sure
  they are current.
  
  SIBs without value tags are read each time the UE camps on a new cell.

  Access functions are provided to read stored SIBs from the database.
  One function is provided to return a pointer to the desired SIB for
  the current active (serving) cell. Another function is provided to
  return a pointer to the desired SIB for any cell (cell-id and PLMN-id
  to be provided by the calling function).

  When SIBs in a serving cell change, this module can notify other
  procedures of the SIB change. This is done only if the modules
  register to be notified of the sib change.

  
  NOTE about the Value Tag Implementation:
  
  Note that currently both "cell value tags" and "PLMN value tags" are
  treated similarly - Only when there is a cell-id and plmn-id match for
  a cell in the database, it's value tags are checked. This is done for
  simplicity. This is functionally correct since plmn value tags are valid
  over the entire PLMN nd we match both the PLMN and the cell ids. 
  
  In the future an optimization could be done so that for
  SIBs having PLMN value tags, the value tags could be compared with any
  other cell in the same PLMN, and the SIB could be used for this cell
  also. Since only SIB 1 and SIB 16 have PLMN value tags,there may not 
  be much incentive to do this.
  
EXTERNALIZED FUNCTIONS

  rrc_sibdb_init  Initialize the database.
  
  rrc_sibdb_return_sib_for_cell  Return a pointer to the requested SIB
                                 In the requested cell. 
  
  rrc_sibdb_return_sib_for_db_index  Return a pointer to the requested SIB
                                     for the requested database index. 
  
  rrc_sibdb_return_sib_for_srv_cell  Return a pointer to the requested
                                     SIB in the serving cell.

  rrc_sibdb_store_value_tags_for_new_cell  Store value_tags obtained for a new
                                     cell in the SIB database.

  rrc_sibdb_compare_value_tags_for_cell   Compare the value-tags stored in the
                                     SIB database with the newly received value-tags
                                     for a cell.
                                     
INITIALIZATION AND SEQUENCING REQUIREMENTS

  rrc_sibdb_init must be called once at power-up to initialize the
  SIB Database.
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc. 
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibdb.c_v   1.13   28 Feb 2002 15:43:42   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsibdb.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/16   vs      Changes to fix KW errors
01/12/16   sn      Changes for FR30881
05/11/16   vs      Changes to remove irrelevant f3s
09/16/14   rmn     Resolved compilation errors in WRRC for LTE removal flavor
08/28/14   bc      Made changes to check the individual Value Tags of non mandatory SIBs even when MIB value Tag
                   matches.
08/12/14   sn      Changes to update extension SIBs value tags only when they are decoded
07/21/14   sn      f3 reduction changes
05/26/14   bc      Made changes to update value tags for deferred sibs when value tag 
                   of MIB has matched.
05/16/14   vi      Made changes to support MOB framework
03/13/14   vi      Made changes to update valuetag for deferred sibs correctly
12/2/13    bc      Changes to To avoid reading SB1 during Update SIBs event if any of the deferred SIBs are 
                   scheduled in SB1 and if SB1 is already present in SIB DB
10/28/13   sn      Changes to continue with update SIBs event when CSFB call status
                   is true
08/27/13   sr      Made changes to access SB2 properly while updating value tags for SIB19
07/08/13   sn      Changes for LTE CSG feature
06/12/13   sn      Changes to not invalidate SIBDB when MIB value tag matches
05/23/13   sr      Added support for SIB18/SIB19 deferring
05/14/13   sr      Hybrid CSG changes
01/03/13   ad      Made changes to reduce the redundant F3s
01/03/13   sr      Made changes to remove the ASN functions used for encoding of SIBs and modified the
                   processing of CSFB system information container accordingly as part of memrory optimization
11/12/12   sn      To properly intialize CSG ID field to invalid for macro cells
12/07/12   sn      CSG feature changes
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
08/30/12   sr      Made changes to check the presence of extension SIBs properly 
                   in SB1 while comparing value tags
07/18/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/12/11   su      Added code changes to store the value-tag of SIB5bis 
                   as value-tag of SIB5.
07/06/11   gv      Fixed KW errors
05/03/11   rl      Fixed the lint error
04/25/11   rl      Fixed the KW error
03/10/11   rl      Fixed the compiler warning
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/25/11   ad      Added code to not delete the SIBDB entry when age_index has reached 
                   SIBDB_MAX_INDEX for Serving cell due to multiple cell reselection failures.
                   Instead we delete the entry for the index with next highest age_index.
02/24/11   rl      Fixed the KW warning
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/07/10   ss      Fixed compilation error
11/29/10   sks     Fixed bugs found during testing.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/03/10   kp      Fixed compilation issue under feature FEATURE_WCDMA_DEFERRED_SIB11_12_READING
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
05/13/10   as      Fixed KW errors
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
11/20/09   dm      Fixed compiler warning
10/23/09   dm      Fixed Klocwork warnings
10/12/09   kp      Added support to re-read all the SIB's when UE come back from LPM.
05/11/09   dm      Feature to dynamically allocate sib_event ptr
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
02/27/09   kp      Added support code to free anoter 46k from sib heap under feature 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP.
02/26/09   kp      Fixed compilation error for FEATURE_RRC_OPTIMIZED_SIB_HEAP
                   when FEATURE NETWORK SHARING is not defined.
02/23/09   kp      Fixed compiler warning for FEATURE_RRC_OPTIMIZED_SIB_HEAP
                   when FEATURE NETWORK SHARING is not defined.
02/13/09   kp      Added support get mib ptr by calling rrc_sibdb_return_sib_for_db_index 
                   in fucntion rrc_sibdb_check_if_sb_present.With feature F
                   EATURE_RRC_OPTIMIZED_SIB_HEAP, mib ptr may be null in that 
                   case we have to decode MIB.
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
09/29/08   kp      Added function to print cell id,scr-code 7 plmn from sibdb for debugging
06/30/08   ns      Made the necessary changes to enable the functionality of the feature 
                   FEATURE NETWORK SHARING only when the GCF NV is set
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE SIB11BIS SUPPORT 
03/07/08   vr\da   Added function to return SIB DB index based on Freq and PSC. Support 
                   added under FEATURE_MODEM_MBMS
12/14/07   kp      Added function to provide status of active cell SIB valid in SIBDB..
10/23/07   kp      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
07/17/07   kp      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
03/28/07   kp      Added support for implementation of BCCH Mod Timer
                   (Implementation is done in rrcpg1 module).Clear BCCH Mod DB 
                   when Serving cell SIB DB index is invalidated. 
01/22/07   vr      Fixed lint errors
09/25/06   kp      Added support for six hours sib re-read & sib_change_ind interface change
09/05/06   kp      Changed order for posting SIB_CHANGE_IND to over come a race 
                   condition at L1
06/07/06   kp      Added support for six hours sib update
04/27/06   vr      Fixed typo in SIB5bis support
04/26/06   vr      Support for SIB5bis
03/24/06   vr      Lint Fixes
03/22/06   kp      Added a function which returns mib value tag for serving cell
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1. 
                   rrc_SIB_Type_spare2 changed to rrc_SIB_Type_spare1 as per REL-6 ASN1
07/27/05   kc      Added support for sending CELL_ID/EST_CELL_ID in GSTK interface
03/09/05   kc      Added support to update connected mode timers from v3a0 extensions.
01/07/05   da      Made change to reverse the order in which sib change notification
                   is sent to procedures. sib 18 goes first, down to sib1.
11/18/04   kc      Added fn. to clear serving cell status when we go from WtoG
                   or due to OOS
09/13/04   sgk     Typecast count1 to rrc_SIB_Type in function
                   rrc_sibdb_notify_sib_change_to_procs to fix lint error:
                   Type mismatch (assignment) (int/enum).                          
                   Changed the typecast fo sibdb.cell[sibdb.active_cell_index].
                   sibs.sib1->bit_mask from int to uint32 in function
                   rrcscmgr_notify_sib_change_to_procs to fix lint error: Loss
                   of sign in promotion from int to unsigned int.
                   Removed value_tag from rrc_sibdb_compare_value_tags_for_cell
                   and index_found from rrc_sibdb_find_cell_index_for_plmn_freq_psc
                   to fix lint error: 'not referenced'.
                   Typecast MAX_SIB_EVENTS_INDEX  to uint8 in function
                   rrc_sibdb_free_cells to fix lint error: Converting enum to int.
                   Initialized Variable 'cell_index_for_current_active_event'
                   to 0 in function rrc_sibdb_free_cells. to fix lint error:
                   may not be initialized.
                   Initialized sib type to rrc_SIB_Type_spare2 (RRC_TOT_SIBS -1)
                   instead of RRC_TOT_SIBS + 1 in function
                   rrcsib_get_sib_type_from_sib_type_and_tag and
                   rrcsib_get_sib_type_from_sibsb_type_and_tag to fix lint error
                   mismatch (assignment) (int/enum).
                   Changed if( sib >= RRC_TOT_SIBS ) to 
                   if( sib > rrc_SIB_Type_spare2 ) In function
                   rrcsib_register_for_sib_change_notification to fix lint error
                   Constant out of range for operator '>=', Converting enum to int.
08/10/04   kc      Added helper functions to get SIB_TYPE from SIBSb type and 
                   Sib_type_and_tag type
07/15/04   kc      Function to invalidate MIB and SB value-tags when SIB-Update
                   is interrupted due to a higher-priority BCCH-event.
06/01/04   vn      Fixed rrc_sibdb_free_cells() to correctly determine the cell
                   for the current active event. Also loop till and including
                   SIBDB_MAX_INDEX for all cells in SIB DB. Added debug messages
                   to print SIBDB usage when allocating indices and freeing cells.
                   Keep track of the number of allocated cells in DB.
03/11/04   svk     Added rrc_sib_db_free_cells() to free up additional space
                   when malloc fails for the sib_db
12/16/03   svk     Modified function rrc_sibdb_find_cell_index_for_freq_psc to 
                   rrc_sibdb_find_cell_index_for_freq_psc to find the db index based
                   on plmn_id, freq, psc.
12/14/03   svk     in rrc_sibdb_init() : Reset all the state change notifications
                   only at the time of power-up. 
08/11/03   ram     Added function rrc_sibdb_find_cell_index_for_freq_psc to find
                   the database index for a given freq and scr code.
06/25/03   ram     Fixed lint errors in rrc_sibdb_store_value_tags_for_new_cell.
05/15/03   ram     After freeing SIB pointers, equated them to NULL to avoid
                   freeing same pointer twice.
05/14/03   ram     Fixed some potential problems with value-tag mismatches
                   when a SIB event is not completed succesfully. Now value
                   tags for all received SIBs are stored in the database only
                   after all SIBs have been received and decoded. Also fixed
                   a problem where SIBs may not be available for reading during
                   an update. Earlier when SIBs were being updated, the older
                   SIBs were deleted as soon as the event started, so they were
                   unavailable for a window of time. Now they are deleted just
                   before the new SIBs are received so they are available for
                   reading all the time.
04/30/03   ram     Linted file and fixed lint errors.
02/10/02   ram     Added a function rrc_get_serving_cell_id for GSTK module
                   in order to support the USAT feature.
11/21/02   ram     Fixed a problem in function rrc_sibdb_check_if_sb_present
                   where the rrc_SIB_Type enum value was being compared with
                   the #define value for SIBSb value tags. This is now fixed
                   by using the correct types for comparison.
09/05/02   ram     Earlier sib change notifications were being sent as
                   soon as each changed sib was decoded. This had a problem 
                   because the procedure receiving the notification could
                   try to access other change SIBs that are not yet decoded.
                   Now this has been fixed so that sib change notifications
                   are only sent when all changed SIBs have been decoded.
02/28/02   ram     Fixed a problem in rrc_sibdb_notify_sib_change_to_procs
                   where a SIB change notification was being sent without
                   checking if any procedure has registered to be notified
                   of that change.
02/19/02   ram     Added interface to update connected mode timers
                   and constants if required by the UTRAN Mobility Information
                   procedure. This is done if SIB1 gets modified.
02/05/02   ram     Implemented a registration mechanism where procedures
                   can register to be notified of sib changes in a cell.
                   When a sib changes in a cell this module sends a 
                   "RRC_SIB_CHANGE_IND" command to the registered procedures.
                   Procedures can also deregister if they don't need the
                   notification anymore.
01/03/02   ram     Added support for SIB 15.4, SIB 18 and Scheduling Blocks 1 & 2
                   which are introduced in June '01 version of the specs.
10/31/01   vn      Substitute a call to RRC_SAME_PLMN with rrc_bcch_plmns_match.
05/18/01   ram     The internal format for PLMN ID has been changed to use
                   arrays instead of integer to store MCC and MNC. Updated
                   this module to use the new format.
04/30/01   ram     Added a function to store value-tags for a cell when we
                   camp on it for the first time, bypassing any value-tag
                   comparisons. Changed the function name rrc_sibdb_process
                   _value_tags_for_cell to rrc_sibdb_compare_value_tags_for_cell
                   for better readability. Updated comments.
04/07/01   ram     Added support for value-tag comparisons. Value-tag comparisons
                   are used for a couple of reasons. When we re-visit a cell
                   that we already have sibs stored for, value-tags are compared
                   to see which sibs are current and can be re-used. They
                   are also used when we get an indication from UTRAN saying
                   that the MIB or Scheduling Blocks need to be read again
                   since some system information has changed. 
                   Added support to call the free-pdu function to free SIBs
                   whenever a SIB database index is being cleared to be used
                   for a different cell.
                   Removed special code for returning SIB5 since this is not
                   being used.
03/23/01   ram     Updated file to use the internal format for cell id. The
                   external (ASN.1) format has been changed due to the usage
                   of new LED ASN.1 header files.
02/08/01   ram     In function rrc_sibdb_create_cell_index, initialized the
                   cell header in the sib database whenever a nex index is
                   used or a index is re-used for a different cell.
01/26/01   ram     Updated comments.
01/09/01   ram     In function rrc_sibdb_create_cell_index, also copied the
                   PLMN and cell id for the cell.
01/04/01   ram     Created new function that returns a pointer to the requested
                   SIB for a particular PLMN and Cell Id. Changed the name
                   of the previous function rrc_sibdb_return_sib_for_cell to
                   rrc_sibdb_return_sib_for_db_index since this accepts an
                   index to the database as a parameter. Implemented algorithm
                   to keep track of the age of the cells in the database so
                   that the oldest cell can be removed to accomodate new cells.
                   Added utility functions to match new cells with existing
                   cells in the database and to create new indices as needed
                   to store new cells. Updated copyright. Added more comments.
12/20/00   ram     Included rrcdata_v.h which now contains the definition for
                   RRC_TOT_SIBS. Added some temporary code to return a pointer
                   to SIB5 to support VST testing. This change will be removed
                   later.
10/30/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "assert.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrcsibdb.h"
#include "rrcsibdbi.h"
#include "rrcumi.h"
#include "rrcpg1.h"
#include "rrccsp.h"
#include "rrcmcm.h"

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
#include "l1rrcif.h"
#include "bit.h"
#endif

#include "mm_umts.h"
#include "mm_as_types.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Total numer of SIB change notification registrations accepted.
It's 16 for now. If more than 16 registrations are needed, increase
this number. */
#define RRC_TOT_SIB_CHG_N 16

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Structure to hold each sib change notification that has been
registered with this module */
typedef struct
{
  boolean index_used;           /* Indicate if this index is used.       */
  rrc_proc_e_type  procedure;   /* Procedure to be notified              */
  rrc_SIB_type sib;             /* SIB for which notification is needed. */
  uint8 priority;                 /*priority for reveived sib type which will be
                                           used for posting sib change to procedures*/
}rrcsib_sib_change_notification_type;

/* Union used to copy Cell-id to a format used by GSTK. */
typedef union
{
  rrc_int_cell_id_type int_cell_id; /* Internal type uint32 */
  uint8 ext_cell_id[4]; /* External type array of 4 uint8s  */
}rrcsib_cell_id_convert_type;

typedef struct 
{
  uint32                 cell_id;
  rrc_plmn_identity_type plmn_id;
  rrc_SysInfoType19 * sib19_ptr;
} rrcsibdb_pseudo_sib19_type;


/* To note if power-up initialization has been done already.
This is used in initializing variables for cases other than power-up
init such as FTM and LPM */
extern boolean rrcsib_powerup_init_done;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

rrc_sibdb_type sibdb;   /* The SIB database */

/* Array to store all the registered state change notifications */
rrcsib_sib_change_notification_type rrcsib_cn[RRC_TOT_SIB_CHG_N];

uint8 rrcsib_cn_max_priority = 0;/*Used to hold max number priority*/

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*Look up table for holding the value tag and segments of the deferred SIBs.*/
extern rrc_deferred_sib_info_type deferred_sib_info;
#endif

rrcsibdb_pseudo_sib19_type rrc_psuedo_sib19;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */



/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


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
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void * rrc_sibdb_get_sib_ptr_for_cell
(
  uint32 index,    /* Index for the requested cell */
  rrc_SIB_type sib /* The SIB that's requested */
)
{
  void *ptr=NULL; /* Local ptr */

  /* Switch on the requested sib and copy the 
  corrosponding SIB pointer */
  switch( sib )
  {
    case rrc_MIB :
      if(sibdb.cell[index].sibs.mib == NULL)
      {
        if(sibdb.cell[index].sibs.mib_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.mib =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.mib_encoded_ptr), /* Encoded SIB      */
          rrc_MasterInformationBlock_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.mib_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.mib == NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from MIB", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.mib;
        }
#ifdef FEATURE_3GPP_CSFB
        else if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->mib);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:mib_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.mib;
      }
      break;

    case rrc_SIB1 :
      if(sibdb.cell[index].sibs.sib1 == NULL)
      {
        if(sibdb.cell[index].sibs.sib1_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib1 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib1_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType1_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib1_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib1== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from SIB1", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib1;
        }
#ifdef FEATURE_3GPP_CSFB
        else if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType1);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib1_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib1;
      }
      break;

    case rrc_SIB2 :
      if(sibdb.cell[index].sibs.sib2 == NULL)
      {
        if(sibdb.cell[index].sibs.sib2_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib2 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib2_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType2_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib2_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib2== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib2", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib2;
        }
        else
        {
          ERR_FATAL("SIB_HEAP:sib2_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib2;
      }
      break;

    case rrc_SIB3 :
      if(sibdb.cell[index].sibs.sib3 == NULL)
      {
        if(sibdb.cell[index].sibs.sib3_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib3 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib3_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType3_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib3_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib3== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib3", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib3;
        }
#ifdef FEATURE_3GPP_CSFB
        else if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType3);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib3_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib3;
      }
      break;

    case rrc_SIB4 :

      if(sibdb.cell[index].sibs.sib4 == NULL)
      {
        if(sibdb.cell[index].sibs.sib4_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib4 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib4_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType4_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib4_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib4== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib4", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib4;
        }
        else
        {
          ERR_FATAL("SIB_HEAP:sib4_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib4;
      }
      break;

    case rrc_SIB5 :
      if(sibdb.cell[index].sibs.sib5 == NULL)
      {
        if(sibdb.cell[index].sibs.sib5_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib5 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib5_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType5bis_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib5_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib5 == NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib5", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib5;
        }
#ifdef FEATURE_3GPP_CSFB
        else if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType5);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib5_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib5;
      }
      break;

    case rrc_SIB6 :
      if(sibdb.cell[index].sibs.sib6 == NULL)
      {
        if(sibdb.cell[index].sibs.sib6_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib6 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib6_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType6_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib6_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib6== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib6", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib6;
        }
        else
        {
          ERR_FATAL("SIB_HEAP:sib6_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib6;
      }
      break;

    case rrc_SIB7 :
      if(sibdb.cell[index].sibs.sib7 == NULL)
      {
        if(sibdb.cell[index].sibs.sib7_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib7 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib7_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType7_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib7_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib7== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib7", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib7;
        }
#ifdef FEATURE_3GPP_CSFB
        else if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType7);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib7_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib7;
      }
      break;

    case rrc_SIB11 :
      if(sibdb.cell[index].sibs.sib11 == NULL)
      {
        if(sibdb.cell[index].sibs.sib11_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib11 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib11_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType11_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib11_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib11== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib11", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib11;
        }
#ifdef FEATURE_3GPP_CSFB
        else if((NULL != sibdb.rrc_decoded_sib_cont) && (sibdb.rrc_decoded_sib_cont->m.sysInfoType11Present))
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType11);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib11_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib11;
      }
      break;

    case rrc_SIB12 :
      if(sibdb.cell[index].sibs.sib12 == NULL)
      {
        if(sibdb.cell[index].sibs.sib12_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib12 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib12_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType12_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib12_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib12== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib12", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib12;
        }
#ifdef FEATURE_3GPP_CSFB
        else if((NULL != sibdb.rrc_decoded_sib_cont) && (sibdb.rrc_decoded_sib_cont->m.sysInfoType12Present))
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType12);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib12_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib12;
      }
      break;

    case rrc_SIB18 :
      if(sibdb.cell[index].sibs.sib18 == NULL)
      {
        if(sibdb.cell[index].sibs.sib18_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib18 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib18_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType18_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib18_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib18== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib18", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib18;
        }
        else
        {
          ERR_FATAL("SIB_HEAP:sib18_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib18;
      }
      break;
  
    case rrc_SB1 :
      if(sibdb.cell[index].sibs.sb1 == NULL)
      {
        if(sibdb.cell[index].sibs.sb1_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sb1 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sb1_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoTypeSB1_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sb1_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sb1== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sb1", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sb1;
        }
#ifdef FEATURE_3GPP_CSFB
        else if((NULL != sibdb.rrc_decoded_sib_cont) && (sibdb.rrc_decoded_sib_cont->m.sysInfoTypeSB1Present))
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoTypeSB1);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sb1_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sb1;
      }
      break;
  
    case rrc_SB2 :
      if(sibdb.cell[index].sibs.sb2 == NULL)
      {
        if(sibdb.cell[index].sibs.sb2_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sb2 =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sb2_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoTypeSB2_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sb2_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sb2== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sb2", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sb2;
        }
#ifdef FEATURE_3GPP_CSFB
        else if((NULL != sibdb.rrc_decoded_sib_cont) && (sibdb.rrc_decoded_sib_cont->m.sysInfoTypeSB2Present))
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoTypeSB2);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sb2_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sb2;
      }
      break;
  
    case rrc_SIB11bis :
      if(sibdb.cell[index].sibs.sib11bis == NULL)
      {
        if(sibdb.cell[index].sibs.sib11bis_encoded_pdu_length != INVALID_PDU_LENGTH)
        {
          /* ASN.1 decode the PDU */
          sibdb.cell[index].sibs.sib11bis =  rrcasn1_decode_pdu( 
          (void *)(sibdb.cell[index].sibs.sib11bis_encoded_ptr), /* Encoded SIB      */
          rrc_SysInfoType11bis_PDU,                                                       /* SIB's PDU number */
          sibdb.cell[index].sibs.sib11bis_encoded_pdu_length   /* SIB's Length in bytes */
          );
          if(sibdb.cell[index].sibs.sib11bis== NULL)
          {
            ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib11bis", 0,0, 0);
          }
          ptr = (void *) sibdb.cell[index].sibs.sib11bis;
        }
#ifdef FEATURE_3GPP_CSFB
        else if((NULL != sibdb.rrc_decoded_sib_cont) && (sibdb.rrc_decoded_sib_cont->m.sysInfoType11bisPresent))
        {
          ptr = (void *)(&sibdb.rrc_decoded_sib_cont->sysInfoType11bis);
        }
#endif
        else
        {
          ERR_FATAL("SIB_HEAP:sib11bis_encoded_ptr is NULL", 0,0, 0);
        }
      }
      else
      {
        ptr = (void *) sibdb.cell[index].sibs.sib11bis;
      }
      break;

#ifdef FEATURE_WCDMA_TO_LTE
        case rrc_SIB19 :
          if(sibdb.cell[index].sibs.sib19 == NULL)
          {
            if(sibdb.cell[index].sibs.sib19_encoded_pdu_length != INVALID_PDU_LENGTH)
            {
              /* ASN.1 decode the PDU */
              sibdb.cell[index].sibs.sib19 =  rrcasn1_decode_pdu( 
              (void *)(sibdb.cell[index].sibs.sib19_encoded_ptr), /* Encoded SIB      */
              rrc_SysInfoType19_PDU,                                                       /* SIB's PDU number */
              sibdb.cell[index].sibs.sib19_encoded_pdu_length   /* SIB's Length in bytes */
              );
              if(sibdb.cell[index].sibs.sib19== NULL)
              {
                ERR_FATAL("SIB_HEAP:Asn.1 decode failed from sib19", 0,0, 0);
              }
              ptr = (void *) sibdb.cell[index].sibs.sib19;
            }
            else
            {
              ERR_FATAL("SIB_HEAP:sib19_encoded_ptr is NULL", 0,0, 0);
            }
          }
          else
          {
            ptr = (void *) sibdb.cell[index].sibs.sib19;
          }
          break;
#endif

    default:
      WRRC_MSG1_ERROR("Unknown SIB%d", sib);
      break;
  }/* end switch */
  return(ptr);
}/* end function rrc_sibdb_get_sib_ptr_for_cell */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
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
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void * rrc_sibdb_return_sib_for_db_index
(
  rrc_SIB_type  req_sib,          /* The SIB that is required */
  uint32        rrc_sibdb_index   /* Index into the SIB database for
                                  the cell from which the SIB is required.
                                  Must be between 0 and RRC_MAX_SIBDB_INDEX */
)
{
  /* If the database is empty return NULL */
  if(sibdb.database_empty)
  {
    WRRC_MSG0_ERROR("Database empty");
    return( NULL );
  }

  /* If the given sib is not in the valid range return NULL */
  /*lint -save -e685 */ 
  if(!(SIBDB_VALID_SIB_RANGE(req_sib)))
  {
    WRRC_MSG1_ERROR("Invalid SIB %d", req_sib);
    return( NULL );
  }
  /*lint -restore */
  /* If the given index is not in the valid range return NULL */
  if(!(SIBDB_VALID_INDEX(rrc_sibdb_index)))
  {
    WRRC_MSG1_ERROR("Invalid sibdb index %d", rrc_sibdb_index);
    return( NULL );
  }

  /* Now check if the requested SIB has been received */
/*lint -e661 lint bug - incorrect out of bounds for ? operator */
  if(!(sibdb.cell[rrc_sibdb_index].sib_valid[req_sib]))
/*lint +e661 */
  {
    return( NULL );
  }

  /* Get the SIB pointer from the cell header and return the ptr */
  return( rrc_sibdb_get_sib_ptr_for_cell( rrc_sibdb_index, req_sib ));
} /* end function rrc_sbidb_return_sib_for_db_index */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

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
void rrcsibdb_free_pseudo_sib19()
{
   if (rrc_psuedo_sib19.sib19_ptr)
   {
      if (rrc_psuedo_sib19.sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem)
      {
        rrc_free(rrc_psuedo_sib19.sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem);
      }

      rrc_free(rrc_psuedo_sib19.sib19_ptr);

      rrc_psuedo_sib19.sib19_ptr = NULL;
   }
   rrc_psuedo_sib19.cell_id = WCDMA_INVALID_CELL_ID;
}

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
)
{
   uint8 i = 0;
   uint8  num_freq = 0;
   earfcn_nas_type earfcn_list;
   rrc_SysInfoType19 * pseudo_sib19_ptr = NULL;
   boolean is_psuedo_sib19_enabled = FALSE;
   rrc_lai_type lai;
   rrc_gsm_map_nas_sys_info_type   nas_common_info;
   rrc_SysInfoType1 *sib1_ptr = rrc_sibdb_return_sib_for_srv_cell(rrc_SIB1);
   unsigned char  *data_ptr;
   inter_task_cell_id_type cell_info;

  if(sib1_ptr == NULL)
  {
    ERR_FATAL("SIB1_ptr is NULL", 0, 0, 0);
  }
  nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
  data_ptr = sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.data;
  for( i = 0; i <sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts; i++)
  {
    nas_common_info.value[i] =  *data_ptr;
    data_ptr ++;
  }
  if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
  {
    ERR_FATAL("Could not extract LAI from NAS Common sys info", 0, 0, 0);
  }
  cell_info.lac = b_unpackw(lai.lac,0,16);
  cell_info.rat = SYS_RAT_UMTS_RADIO_ACCESS;
  cell_info.plmn = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id);
  cell_info.cell_id = cell_id;

   is_psuedo_sib19_enabled = get_lte_earfcn_list_from_nas(
                               &cell_info,
                               &earfcn_list
                              );


   if (!is_psuedo_sib19_enabled)
   {
     return NULL;
   }
   
   pseudo_sib19_ptr = (rrc_SysInfoType19 *) rrc_malloc (sizeof(rrc_SysInfoType19));

   pseudo_sib19_ptr->m.gsm_PriorityInfoListPresent = 0;
   pseudo_sib19_ptr->m.eutra_FrequencyAndPriorityInfoListPresent = 1;
   pseudo_sib19_ptr->m.v920NonCriticalExtensionsPresent = 0;

   pseudo_sib19_ptr->utra_PriorityInfoList.m.utran_FDD_FrequencyListPresent = 0;
   pseudo_sib19_ptr->utra_PriorityInfoList.m.utran_TDD_FrequencyListPresent = 0;

   pseudo_sib19_ptr->utra_PriorityInfoList.utra_ServingCell.m.s_PrioritySearch2Present = 0;
   
   pseudo_sib19_ptr->utra_PriorityInfoList.utra_ServingCell.priority = rrc_nv_psuedo_sib19_nv.utra_priority;
   pseudo_sib19_ptr->utra_PriorityInfoList.utra_ServingCell.s_PrioritySearch1 = rrc_nv_psuedo_sib19_nv.s_priority_search1;
   pseudo_sib19_ptr->utra_PriorityInfoList.utra_ServingCell.threshServingLow = rrc_nv_psuedo_sib19_nv.thresh_serving_low;

   pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.n = earfcn_list.count;

   pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem = 
      (rrc_EUTRA_FrequencyAndPriorityInfo *) rrc_malloc (sizeof(rrc_EUTRA_FrequencyAndPriorityInfo) * earfcn_list.count);
   

   for (i = 0; i < earfcn_list.count; i++)
   {
      WRRC_MSG1_HIGH("PS_SIB19: EARFCN %d", earfcn_list.earfcn_nas[i]);

      /* Provide hard-coded bandwidth */
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].m.measurementBandwidthPresent = 1;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].m.eutra_blackListedCellListPresent = 0;
      
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].earfcn = (uint16) earfcn_list.earfcn_nas[i];
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].measurementBandwidth = rrc_nv_psuedo_sib19_nv.eutra_meas_bandwidth;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].priority = rrc_nv_psuedo_sib19_nv.eutra_priority;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].threshXhigh = rrc_nv_psuedo_sib19_nv.eutra_thresh_high;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].threshXlow = rrc_nv_psuedo_sib19_nv.eutra_thresh_low;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].qRxLevMinEUTRA = rrc_nv_psuedo_sib19_nv.eutra_qrxlevmin;
      pseudo_sib19_ptr->eutra_FrequencyAndPriorityInfoList.elem[i].eutraDetection = FALSE;
   }

   return pseudo_sib19_ptr;
}
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
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void * rrc_sibdb_return_sib_for_srv_cell
(
  rrc_SIB_type  req_sib  /* The SIB that is required */
)
{
  /* First check if we have an active cell selected */
  if(!(sibdb.active_cell_sibs_valid))
  {
    WRRC_MSG0_HIGH("No active cell selected yet!");
    return(NULL);
  }
  /* Then use the generic function to return sibs for a cell */
  return( rrc_sibdb_return_sib_for_db_index
          ( req_sib, /* requested sib */
            sibdb.active_cell_index /* For the active cell */
          )
        );
} /* end function rrc_sibdb_return_sib_for_srv_cell */      
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/





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
)
{
    /* This is for validating the SIB DB for serving cell index */
    rrc_SysInfoType3 * sib3_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SIB3, sibdb.active_cell_index);

    if (sib3_ptr)
    {
      uint32 cell_id = rrc_translate_cell_id(&sib3_ptr->cellIdentity);

      if ((rrc_bcch_plmns_match(rrc_psuedo_sib19.plmn_id,sibdb.cell[sibdb.active_cell_index].plmn_id))
          && (rrc_psuedo_sib19.cell_id == cell_id)
          && (rrc_psuedo_sib19.sib19_ptr != NULL))
      {
         WRRC_MSG0_HIGH("PS_SIB19: No cell change, keep the same sib19");
      }
      else
      {
         rrcsibdb_free_pseudo_sib19();
         
         rrc_psuedo_sib19.plmn_id = sibdb.cell[sibdb.active_cell_index].plmn_id;
         rrc_psuedo_sib19.cell_id = cell_id;
         rrc_psuedo_sib19.sib19_ptr = rrcsibdb_get_pseudo_sib19(cell_id,sibdb.cell[sibdb.active_cell_index].plmn_id);

         WRRC_MSG2_HIGH("PS_SIB19: new sib19 for cell_id %d, sib19 0x%x", 
                     rrc_psuedo_sib19.cell_id,
                     rrc_psuedo_sib19.sib19_ptr);
      }

    }
    else
    {
      WRRC_MSG0_HIGH("PS_SIB19: SIB3 unavailable!");
    }
  return rrc_psuedo_sib19.sib19_ptr;
} /* end function rrc_sibdb_return_sib_for_srv_cell */      

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
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
 )
{ 
  //set the sib_type to invalid initially
  sib_type_tag->sib_type = rrc_MAX_SIB;
  sib_type_tag->sib_tag = INVALID_MIB_SB_VALUE_TAG;

  switch(RRCGET_SIB_TYPE_AND_TAG(sib_type_and_tag))
  {                                                  
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType1):   
      sib_type_tag->sib_type = rrc_SIB1;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType1;
      break;                                         
      
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType2):
      sib_type_tag->sib_type = rrc_SIB2;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType2;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType3):
      sib_type_tag->sib_type = rrc_SIB3;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType3;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType4):
      sib_type_tag->sib_type = rrc_SIB4;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType4;
      break;

    /*lint -save -e616  Fallthrough intended */
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType5bis):
      WRRC_MSG0_HIGH("SIB5bis schedule present in MIB/SB.Typecasting it to SIB5");
      sib_type_tag->sib_type = rrc_SIB5;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType5bis;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType5):
      sib_type_tag->sib_type = rrc_SIB5;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType5;
      break;
    /*lint -restore */

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType6):
      sib_type_tag->sib_type = rrc_SIB6;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType6;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType11):
      sib_type_tag->sib_type = rrc_SIB11;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType11;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType12):
      sib_type_tag->sib_type = rrc_SIB12;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType12;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType18):
      sib_type_tag->sib_type = rrc_SIB18;
      sib_type_tag->sib_tag = sib_type_and_tag->u.sysInfoType18;
      break;
    
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType7):
      sib_type_tag->sib_type = rrc_SIB7;
      break;
  
    default:                                           
      WRRC_MSG1_ERROR("Unknown/Unsup value tag %d", RRCGET_SIB_TYPE_AND_TAG(sib_type_and_tag));  
      break;                                         
  } 
}                                                       

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
 )   
{ 
  //set the sib_type to invalid initially
  sib_type_tag->sib_type = rrc_MAX_SIB;
  sib_type_tag->sib_tag = INVALID_MIB_SB_VALUE_TAG;

  //switch on the sibSb type and initialize sib_type_tag data-struct
  //to sib_type and get the sib_tag too
  switch(RRCGET_SIB_TYPE_AND_TAG(sib_sb_type_tag))
  {                                                  
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType1):     
      sib_type_tag->sib_type = rrc_SIB1;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType1;
      break;                                         
      
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType2):
      sib_type_tag->sib_type = rrc_SIB2;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType2;
    break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType3):
      sib_type_tag->sib_type = rrc_SIB3;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType3;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType4):
      sib_type_tag->sib_type = rrc_SIB4;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType4;
      break;

    /*lint -save -e616  Fallthrough intended */
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType5bis):
      WRRC_MSG0_HIGH("SIB5bis schedule present in MIB/SB.Typecasting it to SIB5");
      sib_type_tag->sib_type = rrc_SIB5;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType5bis;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType5):
      sib_type_tag->sib_type = rrc_SIB5;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType5;
      break;
    /*lint -restore */

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType6):
      sib_type_tag->sib_type = rrc_SIB6;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType6;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType11):
      sib_type_tag->sib_type = rrc_SIB11;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType11;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType12):
      sib_type_tag->sib_type = rrc_SIB12;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType12;
      break;

    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType18):
      sib_type_tag->sib_type = rrc_SIB18;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoType18;
      break;
  
    case RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB1):
      sib_type_tag->sib_type = rrc_SB1;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoTypeSB1;
      break;
  
    case RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB2):
      sib_type_tag->sib_type = rrc_SB2;
      sib_type_tag->sib_tag = sib_sb_type_tag->u.sysInfoTypeSB2;
      break;
  
    case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType7):
      sib_type_tag->sib_type = rrc_SIB7;
      break;
  
    default:                                           
      WRRC_MSG1_ERROR("Unknown/Unsup value tag type %d", RRCGET_SIB_TYPE_AND_TAG(sib_sb_type_tag));  
      break;                                         
  } 
}                                                       

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
void rrc_sibdb_clear_active_cell_status( void )
{
  /* Clear SIB database of active cell selected yet. */
  WRRC_MSG0_HIGH("Invalidating Serving cell Index");
  sibdb.active_cell_sibs_valid = FALSE;
  sibdb.active_cell_index = SIBDB_INVALID_INDEX;
  rrcpg1_check_and_abort_bcch_mod_info_processing();
}
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
void rrc_sibdb_init( void )
{
  uint32 index; /* Index to database */

  WRRC_MSG1_HIGH("Initializing the SIB-DB, power up init done %d",rrcsib_powerup_init_done);

  /* Indicate that the database is empty */
  sibdb.database_empty = TRUE;

  /* Indicate that the SIB database does not have any active
  cell selected yet. */
  sibdb.active_cell_sibs_valid = FALSE;

  /* Clear active cell index */
  sibdb.active_cell_index = SIBDB_INVALID_INDEX;

  sibdb.num_stored_cells = 0;

#ifdef FEATURE_3GPP_CSFB
  sibdb.sib_cont_sibdb_index = SIBDB_INVALID_INDEX;
#endif

#ifdef FEATURE_FEMTO_CSG
  sibdb.num_csg_cells = 0;
#endif
  /* Clear the Active Cells SIB Change indications.
  These are used when the SIBs change in the active
  cell to indicate the change to other RRC procedures. */
  for( index=0; index<RRC_TOT_SIBS; index++ )
  {
    sibdb.active_cell_sib_changed[index] = FALSE;
  }

  /* Make sure all indices are unused */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    sibdb.cell[index].index_used = FALSE;
    /*Initalize six hr timer for each index to Zero*/
    sibdb.cell[index].six_hr_count =0;
  }

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  deferred_sib_info.val_tag.sib11_val_tag =0;
  deferred_sib_info.val_tag.sib11bis_val_tag =0;
  deferred_sib_info.val_tag.sib12_val_tag =0;
  deferred_sib_info.val_tag.sib18_val_tag =0;
#ifdef FEATURE_WCDMA_TO_LTE
  deferred_sib_info.val_tag.sib19_val_tag =0;
#endif
#endif

  /* Only at powerup should the state change notifications be
  initialized. Check for this before initializing */
  if (rrcsib_powerup_init_done == FALSE)
  {

    /* Make sure all the SIB change registration indices are
    unused */
    for( index=0; index<RRC_TOT_SIB_CHG_N; index++ )
    {
      rrcsib_cn[index].index_used = FALSE;
    }
  }

  rrc_psuedo_sib19.sib19_ptr = NULL;
  rrc_psuedo_sib19.cell_id = WCDMA_INVALID_CELL_ID;

  /* Nothing else to do here. Individual cell headers can be initialized 
  before they need to be used */

} /* end function rrc_sibdb_init */


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
)
{
  /* Check if the given pointer is not NULL */
  if( sib_ptr != NULL )
  {
    /* Call the free pdu function. It returns non-zero if an error
    occured */
    (void)rrcasn1_free_pdu( pdu_num, sib_ptr );
  }
}




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
)
{
  uint32 count; /* Counter */

  for( count = 0; count < RRC_TOT_SIBS; count++ )
  {
    /* Initialize each SIB status for this cell index to invalid */
    sibdb.cell[index].sib_valid[count] = FALSE;
    /* Initialize each SIB's value tag to 0 which is not a valid
    value tag. This is to indicate that no value tag is stored
    in this index yet. */
    sibdb.cell[index].sib_value_tag[count] = 0; 
  }
  
  sibdb.cell[index].six_hr_count  = 0;

  /* Since the cells may have previously been used to store
  SIBs, we need to free the memory used by each SIB for this
  index. Since we have to use a different PDU type for each
  SIB, we have to free each SIB seperately. The rrc_sibdb_free
  _sib function is used to free them.  */

  /* Note that even though we don't support all these SIBs now,
  the code has been added here since additional SIBs will be supported
  in future releases */
  if( sibdb.cell[index].sibs.mib != NULL )
  {
    rrc_sibdb_free_sib(rrc_MasterInformationBlock_PDU,
                       (void *)sibdb.cell[index].sibs.mib);
    sibdb.cell[index].sibs.mib = NULL;
  }
  if( sibdb.cell[index].sibs.sib1 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,
                       (void *)sibdb.cell[index].sibs.sib1);
    sibdb.cell[index].sibs.sib1 = NULL;
  }
  if( sibdb.cell[index].sibs.sib2 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType2_PDU,
                       (void *)sibdb.cell[index].sibs.sib2);
    sibdb.cell[index].sibs.sib2 = NULL;
  }
  if( sibdb.cell[index].sibs.sib3 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType3_PDU,
                       (void *)sibdb.cell[index].sibs.sib3);
    sibdb.cell[index].sibs.sib3 = NULL;
  }
  if( sibdb.cell[index].sibs.sib4 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType4_PDU,
                       (void *)sibdb.cell[index].sibs.sib4);
    sibdb.cell[index].sibs.sib4 = NULL;
  }
  if( sibdb.cell[index].sibs.sib5 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType5bis_PDU,
                       (void *)sibdb.cell[index].sibs.sib5);
    sibdb.cell[index].sibs.sib5 = NULL;
  }
  if( sibdb.cell[index].sibs.sib6 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType6_PDU,
                       (void *)sibdb.cell[index].sibs.sib6);
    sibdb.cell[index].sibs.sib6 = NULL;
  }
  if( sibdb.cell[index].sibs.sib7 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType7_PDU,
                       (void *)sibdb.cell[index].sibs.sib7);
    sibdb.cell[index].sibs.sib7 = NULL;
  }
  if( sibdb.cell[index].sibs.sib11 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType11_PDU,
                       (void *)sibdb.cell[index].sibs.sib11);
    sibdb.cell[index].sibs.sib11 = NULL;
  }
  if( sibdb.cell[index].sibs.sib12 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType12_PDU,
                       (void *)sibdb.cell[index].sibs.sib12);
    sibdb.cell[index].sibs.sib12 = NULL;
  }
  if( sibdb.cell[index].sibs.sib18 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType18_PDU,
                       (void *)sibdb.cell[index].sibs.sib18);
    sibdb.cell[index].sibs.sib18 = NULL;
  }
  if( sibdb.cell[index].sibs.sb1 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoTypeSB1_PDU,
                       (void *)sibdb.cell[index].sibs.sb1);
    sibdb.cell[index].sibs.sb1 = NULL;
  }
  if( sibdb.cell[index].sibs.sb2 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoTypeSB2_PDU,
                       (void *)sibdb.cell[index].sibs.sb2);
    sibdb.cell[index].sibs.sb2 = NULL;
  }
  if( sibdb.cell[index].sibs.sib11bis != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType11bis_PDU,
                       (void *)sibdb.cell[index].sibs.sib11bis);
    sibdb.cell[index].sibs.sib11bis = NULL;
  }
#ifdef FEATURE_WCDMA_TO_LTE
  if( sibdb.cell[index].sibs.sib19 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType19_PDU,
                       (void *)sibdb.cell[index].sibs.sib19);
    sibdb.cell[index].sibs.sib19 = NULL;
  }
#endif
  memset(sibdb.cell[index].sibs.mib_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.mib_encoded_pdu_length = INVALID_PDU_LENGTH;
  
  memset(sibdb.cell[index].sibs.sib1_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib1_encoded_pdu_length = INVALID_PDU_LENGTH;
  
  memset(sibdb.cell[index].sibs.sib2_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib2_encoded_pdu_length = INVALID_PDU_LENGTH;
  
  memset(sibdb.cell[index].sibs.sib3_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib3_encoded_pdu_length = INVALID_PDU_LENGTH;  
  
   memset(sibdb.cell[index].sibs.sib4_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib4_encoded_pdu_length = INVALID_PDU_LENGTH;

  //CRC:nickle, remove if check
  memset(sibdb.cell[index].sibs.sib5_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib5_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib6_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib6_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib7_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib7_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib11_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib11_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib12_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib12_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib18_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib18_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sb1_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sb1_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sb2_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sb2_encoded_pdu_length = INVALID_PDU_LENGTH;

  memset(sibdb.cell[index].sibs.sib11bis_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib11bis_encoded_pdu_length = INVALID_PDU_LENGTH;

#ifdef FEATURE_WCDMA_TO_LTE
  memset(sibdb.cell[index].sibs.sib19_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[index].sibs.sib19_encoded_pdu_length = INVALID_PDU_LENGTH;
#endif


} /* end function rrc_sibdb_cell_header_init */




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
)
{    
  uint32 index;                /* To find the matching index */
  boolean index_found = FALSE; /* Index has'nt been found yet */

  /* Try to match the PLMN and Cell Ids to one of the cells
  stored in the database. */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    /* If this particular index is being used */
    if( sibdb.cell[index].index_used == TRUE )
    {
      /* Match the PLMN id and cell id for each index. */
      if( (rrc_bcch_plmns_match((plmn), (sibdb.cell[index].plmn_id))) &&
          (cell == sibdb.cell[index].cell_id)
        )
      {
        /* A Match has been found. Get out of the for loop. */
        index_found = TRUE;
        break; /* Break out of the for loop */
      }
    }
  }/* end for */

  if( index_found )
  {
    return( index );
  }
  else                                                       
  {
    return( SIBDB_INVALID_INDEX );
  }
} /* end function rrc_sibdb_find_cell_index */

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
)
{    
  uint32 index;                /* To find the matching index */
  boolean index_found = FALSE; /* Index has'nt been found yet */

  /* Try to match the PLMN and Cell Ids to one of the cells
  stored in the database. */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    /* If this particular index is being used */
    if( sibdb.cell[index].index_used == TRUE )
    {
      /* Match the PLMN id and cell id for each index. */
      if( (rrc_bcch_plmns_match((plmn), (sibdb.cell[index].plmn_id))) &&
          (cell == sibdb.cell[index].cell_id)
        )
      {
        if((rrcsib_is_need_to_cleanup_cell(event_index) == TRUE) &&
            (sibdb.cell[index].six_hr_count  >=  MAX_CELL_INDEX_COUNT_SIXHR_CELL_SEL_RESEL))
        {
          MSG_HIGH("Index=%d, SIBs valid %d, Active index %d, ",index,sibdb.active_cell_sibs_valid,sibdb.active_cell_index);
          if((sibdb.active_cell_sibs_valid == TRUE)&&
             (sibdb.active_cell_index == index))
          {
            /* We want to skip clearing serving cell, Just to ensure that other procedures accessing
             * SIB DB should not get blocked. 
             */
          }
          else
          {
            rrc_sibdb_clear_a_cell_index(index);
            index=SIBDB_INVALID_INDEX ;
          }
        }
        /* A Match has been found. Get out of the for loop. */
        index_found = TRUE;
        break; /* Break out of the for loop */
      }
    }
  }/* end for */

  if( index_found )
  {
    return( index );
  }
  else
  {
    return( SIBDB_INVALID_INDEX );
  }
} /* end function rrc_sibdb_find_cell_index */


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
)
{
  uint32 count;  /* Counter */

  /* If this cell is being used for the first time, all 
  other cells are older than this cell and their age
  index needs to be adjusted. 
  
  If this cell is being re-used, only those cells that
  were previously newer than this cell need to be adjusted. */ 

  if(new_cell_index >= SIBDB_MAX_CELLS)
  {
    WRRC_MSG1_HIGH("sib db idx = %d, cant adjust",new_cell_index);
    return;
  }

  /* First make sure the age index is not invalid */
  if(!(sibdb.cell[new_cell_index].age_index <= SIBDB_MAX_INDEX))
  {
    ERR_FATAL("Age index greater than MAX",0,0,0);
  }
  	

  if( sibdb.cell[new_cell_index].index_used == FALSE )
  {
    /* This is the first time this cell is being used. All
    other used cells have to be adjusted. Force the age index
    of this cell to be the maximum. */
    sibdb.cell[new_cell_index].age_index = SIBDB_MAX_INDEX;
  }

  for( count=0; count <=SIBDB_MAX_INDEX; count++ )
  {
    if( /* If this is not the new cell's index              */ 
        (count != new_cell_index) &&        
        /* And if this cell index is used                   */
        (sibdb.cell[count].index_used == TRUE) &&  
        /* And if this cell is newer than the selected cell */
        (sibdb.cell[count].age_index < sibdb.cell[new_cell_index].age_index)
      )
    {
      /* If the above conditions are satisfied, the age index needs
      to be incremented by one. */
      sibdb.cell[count].age_index++;
    }

  }

  /* Finally make the age index of the given cell to be 0 since this
  is now the newest cell. */
  sibdb.cell[new_cell_index].age_index = 0;

#ifdef FEATURE_FEMTO_CSG
  for( count=0; count <=SIBDB_MAX_INDEX; count++ )
  {
    MSG_MED("CSG: Index: %d, Age: %d, In use: %d", count, sibdb.cell[count].age_index,sibdb.cell[count].index_used);
  }
#endif
} /* end function rrc_sibdb_adjust_age_index */                                      

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
)
{
  uint32 count;  /* Counter */

  /* If this cell is being used for the first time, all 
  other cells are older than this cell and their age
  index needs to be adjusted. 
  
  If this cell is being re-used, only those cells that
  were previously newer than this cell need to be adjusted. */ 

  if(new_cell_index >= SIBDB_MAX_CELLS)
  {
    WRRC_MSG1_HIGH("sib db idx = %d, cant adjust",new_cell_index);
    return;
  }

  /* First make sure the age index is not invalid */
  if(!( sibdb.cell[new_cell_index].age_index <= SIBDB_MAX_INDEX ))
  {
    ERR_FATAL("Age index is greater than MAX",0,0,0);
  }

  for( count=0; count <=SIBDB_MAX_INDEX; count++ )
  {
    if( /* If this is not the new cell's index              */ 
        (count != new_cell_index) && 
        /* And if this cell is newer than the selected cell */
        (sibdb.cell[count].age_index > sibdb.cell[new_cell_index].age_index)
      )
    {
      /* If the above conditions are satisfied, the age index needs
      to be decremented by one. */
      sibdb.cell[count].age_index--;
    }

  }

  /* Finally make the age index of the given cell to be 0 since this
  is now the newest cell. */
  sibdb.cell[new_cell_index].age_index = 0;

#ifdef FEATURE_FEMTO_CSG
    for( count=0; count <=SIBDB_MAX_INDEX; count++ )
  {

     MSG_MED("CSG: Index: %d, Age: %d, In use: %d", count, sibdb.cell[count].age_index,sibdb.cell[count].index_used);
  }

#endif
} /* end function rrc_sibdb_adjust_age_index */                                      

#endif

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
  ,rrc_int_csg_id_type csg,
  boolean is_csg
#endif
)
{    
  uint32 index;      /* The index to be returned           */
#ifdef FEATURE_FEMTO_CSG
  uint32 macro_age=0;
  uint32 macro_index=0;
  uint32 csg_age=0;
  uint32 csg_index=0;
#else
  uint32 l_index=0; /* Used to find the largest age Index */
  uint32 l_age=0;    /* Used to find largest age index     */
#endif

  /* Always set to FALSE the variable that indicates if the returned
  index has SIBs for a matching PLMN and Cell id. This will be set
  to TRUE later in this function if needed. */
  sibdb.last_plmn_and_cell_id_matched = FALSE;

  /* If the database is empty, just return the first index. */
  if(sibdb.database_empty)
  {
    sibdb.database_empty = FALSE;    /* Not empty anymore */
    sibdb.cell[0].index_used = TRUE; /* Index 0 is now used */
    sibdb.cell[0].age_index = 0;     /* This is the newest cell in the
                                     database. */
    sibdb.cell[0].plmn_id = plmn;    /* Copy the PLMN id */
    sibdb.cell[0].cell_id = cell;    /* Copy the Cell id */
#ifdef FEATURE_FEMTO_CSG
    sibdb.cell[0].is_csg = is_csg;
    sibdb.cell[0].csg_id = csg;
    if(is_csg)
    {
      sibdb.num_csg_cells++;
    }
#endif
    rrc_sibdb_cell_header_init(0);   /* Initialize the cell header for this cell */
    sibdb.num_stored_cells++;   
#ifdef FEATURE_FEMTO_CSG
    MSG_HIGH("New SIBDB Index created - %d, %d num_cells, %d num_csg_cells", 0, sibdb.num_stored_cells, sibdb.num_csg_cells);
#else
    WRRC_MSG2_HIGH("New SIBDB Index created - %d, %d num_cells",0,sibdb.num_stored_cells);
#endif
    return(0); /* Return index 0 */
  }
  
  /* If the database is not empty, try to find a match
  for the given PLMN and Cell Id within the database. That
  means we are re-visiting a cell already stored in the
  database. */
  if( (index = rrc_sibdb_find_valid_cell_index(plmn, cell,event_index)) != SIBDB_INVALID_INDEX )
  {
    /* We found a match. Adjust the age index for the cells in the
    database and return the index. Note that "index_used" will
    already be set to TRUE and the PLMN and Cell ids don't have
    to be written again. */
    rrc_sibdb_adjust_age_index( index );
    /* Since we already stored SIBs for the same PLMN and Cell ids, indicate
    this by setting the corrosponding variable to TRUE. */
    sibdb.last_plmn_and_cell_id_matched = TRUE;
    WRRC_MSG2_HIGH("SIBDB Index found - %d, %d num_cells",index, sibdb.num_stored_cells);
    return( index );
  }

  /* If a match is not found then check for the next empty
  slot in the database */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    if(sibdb.cell[index].index_used == FALSE)
    {
      rrc_sibdb_adjust_age_index( index ); /* Adjust age index for cells */
      sibdb.cell[index].index_used = TRUE; /* This index is now used */
      sibdb.cell[index].plmn_id = plmn;    /* Copy the PLMN id */
      sibdb.cell[index].cell_id = cell;    /* Copy the Cell id */
#ifdef FEATURE_FEMTO_CSG
      sibdb.cell[index].is_csg = is_csg;
      sibdb.cell[index].csg_id = csg;
      if(is_csg)
      {
        sibdb.num_csg_cells++;
      }
#endif	 
      rrc_sibdb_cell_header_init(index);   /* Initialize the cell header */
      sibdb.num_stored_cells++;   
#ifdef FEATURE_FEMTO_CSG
      MSG_HIGH("New SIBDB Index created - %d, %d num_cells %d num_csg", index, sibdb.num_stored_cells, sibdb.num_csg_cells);
#else
      WRRC_MSG2_HIGH("New SIBDB Index created - %d, %d num_cells", index, sibdb.num_stored_cells);
#endif
      return( index );
    }
  }

#ifdef FEATURE_FEMTO_CSG
  /* First findout oldest macro and femto entries */
  for(index=0; index<=SIBDB_MAX_INDEX; index++)
  {
    if((sibdb.active_cell_sibs_valid == TRUE)&&
	   (sibdb.active_cell_index == index))
    {
      WRRC_MSG1_HIGH("age_index is %d for active cell index, so continue to next entry"
               ,SIBDB_MAX_INDEX);
      continue;
    }
    if((sibdb.cell[index].is_csg)&&(sibdb.cell[index].age_index > csg_age))
    {
      csg_age = sibdb.cell[index].age_index;
      csg_index = index;
    }
    else if((!sibdb.cell[index].is_csg)&&(sibdb.cell[index].age_index > macro_age))
    {
      macro_age = sibdb.cell[index].age_index;
      macro_index = index;
    }
  }

  /* Check whether new entry to add is femto or macro */
  if(is_csg)
  {
    if(sibdb.num_csg_cells >= SIBDB_MAX_CSG_CELLS)
    {
      /*replace femto cell*/
      rrc_sibdb_adjust_age_index( csg_index );
      sibdb.cell[csg_index].plmn_id = plmn;
      sibdb.cell[csg_index].cell_id = cell;
      sibdb.cell[csg_index].csg_id = csg;
      rrc_sibdb_cell_header_init(csg_index);
      MSG_HIGH("SIBDB Index created - %d, %d num_cells, %d num_csg",csg_index, sibdb.num_stored_cells, sibdb.num_csg_cells);
      return csg_index;
    }
    else
    {
      /*replace macro cell*/
      rrc_sibdb_adjust_age_index( macro_index );
      sibdb.cell[macro_index].plmn_id = plmn;
      sibdb.cell[macro_index].cell_id = cell;
      sibdb.cell[macro_index].csg_id = csg;
      sibdb.cell[macro_index].is_csg = TRUE;
      rrc_sibdb_cell_header_init(macro_index);
      sibdb.num_csg_cells++;
      MSG_HIGH("SIBDB Index created - %d, %d num_cells, %d num_csg",macro_index, sibdb.num_stored_cells, sibdb.num_csg_cells);
      return macro_index;
    }
  }
  else
  {
    if(sibdb.num_csg_cells > SIBDB_MAX_CSG_CELLS)
    {
      /*replace femto cell*/
      rrc_sibdb_adjust_age_index( csg_index );
      sibdb.cell[csg_index].plmn_id = plmn;
      sibdb.cell[csg_index].cell_id = cell;
      sibdb.cell[csg_index].is_csg = FALSE;
      sibdb.cell[csg_index].csg_id = csg;
      rrc_sibdb_cell_header_init(csg_index);
      sibdb.num_csg_cells--;
      MSG_HIGH("SIBDB Index created - %d, %d num_cells, %d num_csg",csg_index, sibdb.num_stored_cells, sibdb.num_csg_cells);
      return csg_index;
    }
    else
    {
      /*replace macro cell*/
      rrc_sibdb_adjust_age_index( macro_index );
      sibdb.cell[macro_index].plmn_id = plmn;
      sibdb.cell[macro_index].cell_id = cell;
      sibdb.cell[macro_index].csg_id = csg;
      rrc_sibdb_cell_header_init(macro_index);
      MSG_HIGH("SIBDB Index created - %d, %d num_cells, %d num_csg",macro_index, sibdb.num_stored_cells, sibdb.num_csg_cells);
      return macro_index;
    }
  }

#else

  /* If we get here, then the database is full. The oldest
  cell has to be removed to accomodate the new cell. Note that
  the oldest cell will have the age_index of SIBDB_MAX_INDEX
  since we store only that many cells in the database.Also note 
  that "index_used" will already be set to TRUE. */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    if(sibdb.cell[index].age_index == SIBDB_MAX_INDEX)
    {
      if((sibdb.active_cell_sibs_valid == TRUE)&&
         (sibdb.active_cell_index == index))
      {
        WRRC_MSG1_HIGH("age_index is %d for active cell index, so continue to next entry"
                 ,SIBDB_MAX_INDEX);
        continue;
      }
      /* This is the oldest cell. Adjust the age index
      of the cells accordingly and return this index. */
      rrc_sibdb_adjust_age_index( index );
      sibdb.cell[index].plmn_id = plmn;    /* Copy the PLMN id */
      sibdb.cell[index].cell_id = cell;    /* Copy the Cell id */
      rrc_sibdb_cell_header_init(index);   /* Initialize the cell header */
      WRRC_MSG2_HIGH("SIBDB Index created - %d, %d num_cells",index, sibdb.num_stored_cells);
      return( index );
    }
  }

  /* We can come here if the index with highest age_index is active cell's index
   and hence we can't delete this entry or due to an error situation since one of the cells must
  have an age index of SIBDB_MAX_INDEX. Note that the error situation should never
  happen in the field. */
  WRRC_MSG1_ERROR("Highest age_index is %d and is active cell's index or Indexing problem in the SIB database!"
       ,SIBDB_MAX_INDEX);

  /* We don't have to err-fatal here. We can recover from this
  error by just picking the largest age index and using that
  index for the new cell */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    if(sibdb.cell[index].age_index > l_age)
    {
      if((sibdb.active_cell_sibs_valid == TRUE)&&
         (sibdb.active_cell_index == index))
      {
        WRRC_MSG1_HIGH("age_index is %d for active cell index, so continue to next entry"
                 ,SIBDB_MAX_INDEX);
        continue;
      }
       /* In order to get the next largest next time assign the 
           comparison parameter to present largest*/
      l_age = sibdb.cell[index].age_index;
      /* Replace current largest index with new largest index */
      l_index = index;
    }
  }
  /* Now, l_index is the index of the oldest cell */
  rrc_sibdb_adjust_age_index( l_index );
  sibdb.cell[l_index].plmn_id = plmn;  /* Copy the PLMN id */
  sibdb.cell[l_index].cell_id = cell;  /* Copy the Cell id */
  rrc_sibdb_cell_header_init(l_index); /* Initialize the cell header */
  WRRC_MSG1_HIGH("SIBDB Index created - %d",l_index);
  return( l_index );
#endif

} /* end function rrc_sibdb_create_cell_index */
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
)
{                           
  uint32 index; /* To find the matching index */

  if( (index = rrc_sibdb_find_cell_index(plmn, cell)) != SIBDB_INVALID_INDEX )
  {
    /* Use the generic function to return sibs for a index */
    return( rrc_sibdb_return_sib_for_db_index( req_sib, index ));
  }
  else
  {
    /* The requested SIB is not stored in the database. */
    WRRC_MSG0_HIGH("Match not found for given PLMN and cell id");
    return( NULL );
  }
} /* end function rrc_sibdb_return_sib_for_cell */

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
)
{  
  /* Make sure the index is a valid sib database index where
  SIBs are stored */
  if(!(SIBDB_VALID_INDEX(dbindex)))
  {
    WRRC_MSG0_ERROR("Invalid database index given");
    return;
  }
/*lint -e661 lint bug - incorrect out of bounds for ? operator */
  if( sibdb.cell[dbindex].index_used == FALSE )
/*lint +e661 */
  {
    WRRC_MSG0_ERROR("Unused database index given");
    return;
  }

  //invalidate value tags for mib,sb1 and sb2
  sibdb.cell[dbindex].sib_value_tag[rrc_MIB] = 
  sibdb.cell[dbindex].sib_value_tag[rrc_SB1] = 
  sibdb.cell[dbindex].sib_value_tag[rrc_SB2] = 
    INVALID_MIB_SB_VALUE_TAG;

}


/*===========================================================================

FUNCTION RRC_SIBDB_COMPARE_VALUE_TAGS_FOR_CELL

DESCRIPTION

  This function compares the stored value tags for a particular
  cell with the newly received value tags in the MIB or the
  Scheduling blocks.
  
  If the new value tags don't match the stored value tags, 
  this function indicates this in the result_ptr so that the
  new SIBs can be read and stored in their place.

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
  rrcsib_vtag_match_type  match_type,

  sib_events_index_e_type index

)
{
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */

  rrc_SIBSb_ReferenceList *sb_ptr; /* Local pointer to SIBSb ref list */
  rrc_SIB_ReferenceList *ptr;      /* Local pointer to SIB ref list   */
  uint32 count, n_index=0;             /* Local counter                   */
  void *sib_ptr;
  rrcsib_sib_type_vtag sib_type_tag;

  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;

#ifdef FEATURE_WCDMA_TO_LTE
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
#endif


  if( result_ptr == NULL )
  {
    WRRC_MSG0_ERROR("Un-initialized result ptr!");
    return;
  }

  /* Fill the results ptr with the default value. The
  results for the SIBs present in the reference list will
  be updated later. */
  for( count=0; count < RRC_TOT_SIBS; count++ )
  {
    result_ptr->result[count] = VALUE_TAG_NOT_PRESENT;
  }

  /* Make sure the index is a valid sib database index where
  SIBs are stored */
  if(!(SIBDB_VALID_INDEX(dbindex)))
  {
    WRRC_MSG0_ERROR("Invalid database index given");
    return;
  }
/*lint -e661 lint bug - incorrect out of bounds for ? operator */
  if( sibdb.cell[dbindex].index_used == FALSE )
/*lint +e661 */
  {
    WRRC_MSG0_ERROR("Unused database index given");
    return;
  }

  /* Now check whether the new value tags are from
  the MIB or one of the SBs. 
  
  If the new value tags are from the MIB, first we
  have to check the MIB value tag. If the MIB value
  tag does not match, save the new MIB value tag and
  free the space used by the old MIB for this cell. */
  
  if( sib == rrc_MIB )
  {
    MSG_LOW("Comparing value tags from MIB", 0, 0, 0);
    /* Typecast to MIB pointer */
    mib_ptr = (rrc_MasterInformationBlock *)block_ptr;

    /* Check the MIB value tag */
    if( (sibdb.cell[dbindex].sib_valid[rrc_MIB]) &&
        (mib_ptr->mib_ValueTag == sibdb.cell[dbindex]
           .sib_value_tag[rrc_MIB])
      )
    {
      if(match_type == RRCSIB_READ_ALL_SIBS)
      {
        WRRC_MSG0_HIGH("Forcibly re-read all sibs");
      }
      else
      {
        WRRC_MSG0_HIGH("MIB value tag matched");
        sib_events_ptr->event[index].is_mib_vtag_match = TRUE;
      }
      result_ptr->result[rrc_MIB] = VALUE_TAG_MATCHED;

      /* If the MIB value tag matched, the MIB has not changed so 
      we can assume all the SIBs stored for this cell are current.
      Update the result pointer accordingly. For loop starts from
      1 since we don't need to check the mib. */
      for( count=1; count < RRC_TOT_SIBS; count++ )
      {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
        if(((count == rrc_SIB11)||
           (count == rrc_SIB12)||
           (count == rrc_SIB11bis) ||
#ifdef FEATURE_WCDMA_TO_LTE
           (count == rrc_SIB19) ||
#endif
           (count == rrc_SIB18)
          )&&
           (sibdb.cell[dbindex].sib_value_tag[count] == INVALID_MIB_SB_VALUE_TAG)
           )
        {
          WRRC_MSG1_HIGH("Deferred SIB:sib %d value tag didnot match", count);
          result_ptr->result[count] = DEFERRED_SIB_RCVD;
        }
        else
#endif
        if( sibdb.cell[dbindex].sib_valid[count] )
        {
          if(match_type == RRCSIB_READ_ALL_SIBS)
          {
            result_ptr->result[count] = VALUE_TAG_DID_NOT_MATCH;
          }
          else
          {
            if(sibdb.cell[dbindex].sib_value_tag[count] != INVALID_MIB_SB_VALUE_TAG)
            {
              result_ptr->result[count] = VALUE_TAG_MATCHED;
              if(count == rrc_SB1 || count == rrc_SB2)
              {
                if((sib_ptr = rrc_sibdb_return_sib_for_db_index(count, dbindex)) != NULL)
                {
                  WRRC_MSG2_HIGH("Sched. Info DB-Idx %d for SIB %d", dbindex, count);
                  rrcsib_update_sched_info_from_mib_sb((uint32)count,sib_ptr, index);
                }
              }
            }
            else
            {
              result_ptr->result[count] = VALUE_TAG_DID_NOT_MATCH;
            }
          }
        }
        else
        {
          /* The sib is not present */
          result_ptr->result[count] = NO_SIB_FOR_THIS_VALUE_TAG;
        }
      }
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
      if((sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS)&&
         ((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] ==INVALID_MIB_SB_VALUE_TAG) ||
#ifdef FEATURE_WCDMA_TO_LTE
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] ==INVALID_MIB_SB_VALUE_TAG) ||
#endif
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] ==INVALID_MIB_SB_VALUE_TAG)
          )
        )
      {
        rrc_sibdb_copy_deferred_sibdb_to_tsibdb(dbindex,index, mib_ptr, rrc_MIB);
      }
#endif
      /* Nothing else to do here, just return */
      return;
    }
    else
    {
      if(match_type == RRCSIB_READ_ALL_SIBS)
      {
        WRRC_MSG0_HIGH("Forcibly re-read all sibs");
      }
      else
      {
        WRRC_MSG0_HIGH("MIB value tag did not match");
      }
    }

    /* The MIB value tag did not match. So we have to go through the
    reference list and see which SIBs are current and which ones need
    to be read again. Compare all value-tags present in the MIB. */
    sb_ptr = &(mib_ptr->sibSb_ReferenceList);
    n_index = 0;
    while(sb_ptr->n > n_index)
    {
      /* Switch on the value tag's sib type and look
      at the corrosponding SIBs stored value tag for
      a match */
      rrcsib_get_sib_type_from_sibsb_type_and_tag(&sb_ptr->elem[n_index].sibSb_Type,
                                                  &sib_type_tag);

          
      switch( sib_type_tag.sib_type )
      {
      case rrc_SIB1:
      case rrc_SIB2:
      case rrc_SIB3:
      case rrc_SIB4:
      case rrc_SIB5:
      case rrc_SIB6:
      case rrc_SIB11:
      case rrc_SIB12:
      case rrc_SIB18:
      case rrc_SB1:
      case rrc_SB2:

        if(sibdb.cell[dbindex].sib_valid[sib_type_tag.sib_type])
        {
          if((sib_type_tag.sib_tag) == 
             (sibdb.cell[dbindex].sib_value_tag[sib_type_tag.sib_type]))
          {
            if(match_type == RRCSIB_READ_ALL_SIBS)
            {
              result_ptr->result[sib_type_tag.sib_type] = VALUE_TAG_DID_NOT_MATCH;
            }
            else
            {
              if(sib_type_tag.sib_type == rrc_SB1 || sib_type_tag.sib_type == rrc_SB2)
              {
                if((sib_ptr = rrc_sibdb_return_sib_for_db_index(sib_type_tag.sib_type, dbindex)) != NULL)
                {
                  WRRC_MSG2_HIGH("Sched. Info DB-Idx %d for SIB %d", dbindex, sib_type_tag.sib_type);
                  rrcsib_update_sched_info_from_mib_sb((uint32)sib_type_tag.sib_type,sib_ptr, index);
                }
              }
              /* SIB1 is present and the value tags match */
              MSG_LOW("V-Tag matched for MIB SIB-%d", sib_type_tag.sib_type, 0, 0);
              result_ptr->result[sib_type_tag.sib_type] = VALUE_TAG_MATCHED;
            }
          }
          else
          {
            /* Update result pointer */
            result_ptr->result[sib_type_tag.sib_type] = 
              VALUE_TAG_DID_NOT_MATCH;
          }
        }
        else
        {
          /* The SIB is not present in the database. Update result pointer. */
          result_ptr->result[sib_type_tag.sib_type] = 
            NO_SIB_FOR_THIS_VALUE_TAG;
        }

        break;

      case rrc_SIB7:
        break;

      default:
          WRRC_MSG0_ERROR("Unknown/Unsup value tag type");
          //value_tag = INVALID_MIB_SB_VALUE_TAG;
          break;
      }/* end switch */

      n_index++;
    }/* end while */

    if(EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr))
    {
       ext_siblist_ptr = &(mib_ptr->v690NonCriticalExtensions.
        v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
    }

#ifdef FEATURE_WCDMA_TO_LTE
    if((mib_ptr->m.v690NonCriticalExtensionsPresent) &&
       (mib_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
       (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
       (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.
          masterInformationBlock_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent))
    {
      ext_siblist2_ptr = &(mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.
        masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List);
    }
#endif
  }/* end if value tags from MIB */
  else 
  {
    /* The value tags must be from one of the scheduling blocks. Find out which one. */
    if( sib == rrc_SB1 )
    {
      /* Value tag is from Scheduling Block 1 
      Copy the pointer and compare the received
      value tags. */
      sb1_ptr = (rrc_SysInfoTypeSB1 *)block_ptr;
      ptr = &(sb1_ptr->sib_ReferenceList);
      if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
            sb1_ptr,rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
      {
        ext_siblist_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.
          sysInfoTypeSB1_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
      }
#ifdef FEATURE_WCDMA_TO_LTE
      if((sb1_ptr->m.v6b0NonCriticalExtensionsPresent) &&
         (sb1_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
         (sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.
            m.extSIBTypeInfoSchedulingInfo_ListPresent))
      {
        ext_siblist2_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
          sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List);
      }
#endif

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
      if((sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS)&&
         ((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] ==INVALID_MIB_SB_VALUE_TAG)
#ifdef FEATURE_WCDMA_TO_LTE
         || (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] ==INVALID_MIB_SB_VALUE_TAG)
#endif
         || (sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] ==INVALID_MIB_SB_VALUE_TAG)
        ))
      {
        rrc_sibdb_copy_deferred_sibdb_to_tsibdb(dbindex, index, sb1_ptr, rrc_SB1);
      }
#endif
    }
    else if( sib == rrc_SB2 ) 
    {
      /* Value tag is from Scheduling Block 2 
      Copy the pointer and compare the received
      value tags. */
      sb2_ptr = (rrc_SysInfoTypeSB2 *)block_ptr;
      ptr = &(sb2_ptr->sib_ReferenceList);
      
      if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
            sb2_ptr,rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
      {
           ext_siblist_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.
          sysInfoTypeSB2_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
      }
#ifdef FEATURE_WCDMA_TO_LTE
      if((sb2_ptr->m.v6b0NonCriticalExtensionsPresent) &&
         (sb2_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
         (sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoTypeSB2_v860ext.
            m.extSIBTypeInfoSchedulingInfo_ListPresent))
      {
        ext_siblist2_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
          sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List);
      }
#endif

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
      if((sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS)&&
         ((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] ==INVALID_MIB_SB_VALUE_TAG)||
          (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] ==INVALID_MIB_SB_VALUE_TAG)
#ifdef FEATURE_WCDMA_TO_LTE
          || (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] ==INVALID_MIB_SB_VALUE_TAG)
#endif
          || (sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] ==INVALID_MIB_SB_VALUE_TAG)
          ))
      {
        rrc_sibdb_copy_deferred_sibdb_to_tsibdb(dbindex, index, sb2_ptr, rrc_SB2);
      }
#endif
    }

    else
    {
      WRRC_MSG1_HIGH("Invalid scheduling block %d", sib);
      ptr=NULL;
    }

    /* Now compare all the value tags given by this block */
    n_index = 0;
    while((ptr != NULL) && (ptr->n > n_index))
    {
      /* Switch on the value tag's sib type and look
      at the corrosponding SIBs stored value tag for
      a match */
      rrcsib_get_sib_type_from_sib_type_and_tag(&ptr->elem[n_index].sib_Type,
                                                &sib_type_tag);

          
      switch( sib_type_tag.sib_type )
      {
      case rrc_SIB1:
      case rrc_SIB2:
      case rrc_SIB3:
      case rrc_SIB4:
      case rrc_SIB5:
      case rrc_SIB6:
      case rrc_SIB11:
      case rrc_SIB12:
      case rrc_SIB18:

        if(sibdb.cell[dbindex].sib_valid[sib_type_tag.sib_type])
        {
          if((sib_type_tag.sib_tag) == 
             (sibdb.cell[dbindex].sib_value_tag[sib_type_tag.sib_type]))
          {
            if(match_type == RRCSIB_READ_ALL_SIBS)
            {
              result_ptr->result[sib_type_tag.sib_type] = VALUE_TAG_DID_NOT_MATCH;
            }
            else
            {
              /* SIB1 is present and the value tags match */
              MSG_LOW("V-Tag matched for SB SIB-%d", sib_type_tag.sib_type, 0, 0);
              result_ptr->result[sib_type_tag.sib_type] = VALUE_TAG_MATCHED;
            }
          }
          else
          {
            /* Update result pointer */
            result_ptr->result[sib_type_tag.sib_type] = 
              VALUE_TAG_DID_NOT_MATCH;
          }
        }
        else
        {
          /* The SIB is not present in the database. Update result pointer. */
          result_ptr->result[sib_type_tag.sib_type] = 
            NO_SIB_FOR_THIS_VALUE_TAG;
        }

        break;

      case rrc_SIB7:
        break;

      default:
          WRRC_MSG0_ERROR("Unknown/Unsup value tag type");
          //value_tag = INVALID_MIB_SB_VALUE_TAG;
          break;

      }/* end switch */
      n_index++;
    }/* end while */
  }/* end if value tags from SB1 or SB2 */

  if(ext_siblist_ptr != NULL)
  {
    n_index = 0;
    while(ext_siblist_ptr->n > n_index)
    {
      if( RRCCHECK_SIB_TypeExt(
            (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
             systemInfoType11bis)
        )
      {
   
        if(sibdb.cell[dbindex].sib_valid[rrc_SIB11bis])
        {
          if((ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag) == 
               (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis]))
          {
            if(match_type == RRCSIB_READ_ALL_SIBS)
            {
              result_ptr->result[rrc_SIB11bis] = VALUE_TAG_DID_NOT_MATCH;
            }
            else
            {
              /* SIB1 is present and the value tags match */
              WRRC_MSG1_HIGH("sib11bis:V-Tag matched for SIB11bis, Vtag-%d",
                  ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
              result_ptr->result[rrc_SIB11bis] = VALUE_TAG_MATCHED;
            }
          }
          else
          {
            /* Update result pointer */
            result_ptr->result[rrc_SIB11bis] = 
              VALUE_TAG_DID_NOT_MATCH;
          }
        }
        else
        {
          /* The SIB is not present in the database. Update result pointer. */
          result_ptr->result[rrc_SIB11bis] = 
            NO_SIB_FOR_THIS_VALUE_TAG;
        }
      }
      else
      {
        WRRC_MSG1_ERROR("sib11bis:extensionSIB_Type =%x not supported",
        RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
      }
      n_index++;    
    }
  }
#ifdef FEATURE_WCDMA_TO_LTE
  if(ext_siblist2_ptr != NULL)
  {
    n_index = 0;
    while(ext_siblist2_ptr->n > n_index)
    {
      if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t ==
         T_rrc_SIB_TypeExt2_systemInfoType19)
      {
        if(sibdb.cell[dbindex].sib_valid[rrc_SIB19])
        {
          if((ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag) == 
             (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19]))
          {
            if(match_type == RRCSIB_READ_ALL_SIBS)
            {
              result_ptr->result[rrc_SIB19] = VALUE_TAG_DID_NOT_MATCH;
            }
            else
            {
              /* SIB1 is present and the value tags match */
              WRRC_MSG1_HIGH("sib19:V-Tag matched for SIB19, Vtag-%d",
                  ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
              result_ptr->result[rrc_SIB19] = VALUE_TAG_MATCHED;
            }
          }
          else
          {
            /* Update result pointer */
            result_ptr->result[rrc_SIB19] = 
              VALUE_TAG_DID_NOT_MATCH;
          }
        }
        else
        {
          /* The SIB is not present in the database. Update result pointer. */
          result_ptr->result[rrc_SIB19] = 
            NO_SIB_FOR_THIS_VALUE_TAG;
        }
      }
      else
      {
        WRRC_MSG1_ERROR("sib19:extensionSIB_Type =%x not supported",
             ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
      }
      n_index++; 
    }
  }
#endif

}/* end function rrc_sibdb_compare_value_tags_for_cell */


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

)
{
  rrc_SIBSb_ReferenceList *sb_ptr=NULL; /* Local pointer to SIBSb ref list   */
  rrc_SIB_ReferenceList *ptr1=NULL;     /* To store Scheduling block1 ref ptr*/
  rrc_SIB_ReferenceList *ptr2=NULL;     /* To store Scheduling block2 ref ptr*/
  rrc_SIB_ReferenceList *ptr=NULL;      /* To store Scheduling block2 ref ptr*/
  boolean sb1_present=FALSE;           /* Indicate if Sched block 1 present */
  boolean sb2_present=FALSE;           /* Indicate if Sched block 2 present */
  uint32 count=0;                      /* Count to loop through Sched Blocks*/
  uint32 n_index=0;
  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;

#ifdef FEATURE_WCDMA_TO_LTE
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
#endif

  rrc_MasterInformationBlock *mib_ptr = NULL; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr = NULL;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr = NULL;         /* Local pointer to SB2       */

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  boolean deferred_meas_read_supported = FALSE;
   if(rrcsib_can_event_support_deferred_sib(index) == TRUE)
   {
     deferred_meas_read_supported =
                 rrcsib_can_nw_support_deferred_sib(dbindex);
   }
#endif
  WRRC_MSG0_HIGH("Storing value tags from MIB");

  if( sibdb.cell[dbindex].sibs.mib != NULL )
  {
    /* Copy the MIB value tag into the database */
    sibdb.cell[dbindex].sib_value_tag[rrc_MIB] =
      sibdb.cell[dbindex].sibs.mib->mib_ValueTag;

    /* We have to go through the reference list and
    store all value-tags present in the MIB. */
    sb_ptr = &(sibdb.cell[dbindex].sibs.mib->sibSb_ReferenceList);
    n_index = 0;

    while( sb_ptr->n > n_index )
    {
      /* Switch on the value tag's sib type and look
      at the corrosponding SIBs stored value tag for
      a match */
      switch(RRCGET_SIB_TYPE_AND_TAG((&(sb_ptr->elem[n_index].sibSb_Type))))
      {
        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType1):
          
          if( sibdb.cell[dbindex].sibs.sib1 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB1]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType1;
            MSG_LOW("SIB1 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType2):

          if( sibdb.cell[dbindex].sibs.sib2 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB2]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType2;
            MSG_LOW("SIB2 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType3):
  
          if( sibdb.cell[dbindex].sibs.sib3 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB3]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType3;
            MSG_LOW("SIB3 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType4):
          
          if( sibdb.cell[dbindex].sibs.sib4 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB4]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType4;
            MSG_LOW("SIB4 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType5):
          
          if( sibdb.cell[dbindex].sibs.sib5 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB5]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType5;
            MSG_LOW("SIB5 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType5bis):
          
          if( sibdb.cell[dbindex].sibs.sib5 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB5]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType5bis;
            MSG_LOW("SIB5bis value tag stored as sib5", 0, 0, 0);
          }
          break;


        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType6):
          
          if( sibdb.cell[dbindex].sibs.sib6 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB6]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType6;
            MSG_LOW("SIB6 value tag stored", 0, 0, 0);
          }
          break;
         
        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType7):
          /* SIB7 does not have a value-tag, it only has an expiration timer */
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType11):

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)&&
             (deferred_meas_read_supported == TRUE) &&
             !(sib_events_ptr->event[index].tsib[rrc_SIB11].sib_decoded)
             )
          {
            deferred_sib_info.val_tag.sib11_val_tag = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType11;
            WRRC_MSG1_HIGH("Deferred SIB: SIB11 value tag stored %d", deferred_sib_info.val_tag.sib11_val_tag);
          }
          else
#endif
          if( sibdb.cell[dbindex].sibs.sib11 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB11]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType11;
            MSG_LOW("SIB11 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType12):
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] == INVALID_MIB_SB_VALUE_TAG)&&
             (deferred_meas_read_supported == TRUE) &&
             !(sib_events_ptr->event[index].tsib[rrc_SIB12].sib_decoded)
             )
          {
            deferred_sib_info.val_tag.sib12_val_tag = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType12;
            WRRC_MSG1_HIGH("Deferred SIB: SIB12 value tag stored %d", deferred_sib_info.val_tag.sib12_val_tag);
          }
          else
#endif
          if( sibdb.cell[dbindex].sibs.sib12 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB12]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType12;
            MSG_LOW("SIB12 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType18):
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] == INVALID_MIB_SB_VALUE_TAG)&&
             (deferred_meas_read_supported == TRUE) &&
             !(sib_events_ptr->event[index].tsib[rrc_SIB18].sib_decoded)
             )
          {
            deferred_sib_info.val_tag.sib18_val_tag = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType18;
            WRRC_MSG1_HIGH("Deferred SIB: SIB18 value tag stored %d", deferred_sib_info.val_tag.sib18_val_tag);
          }
          else
#endif          
          if( sibdb.cell[dbindex].sibs.sib18 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SIB18]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoType18;
            MSG_LOW("SIB18 value tag stored", 0, 0, 0);
          }
          break;

        case RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB1):
     
          if( sibdb.cell[dbindex].sibs.sb1 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SB1]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoTypeSB1;
            MSG_LOW("SB1 value tag stored", 0, 0, 0);
  
            /* Note that Sched Block 1 is present */
            sb1_present=TRUE;
          }
          break;

        case RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB2):

          if( sibdb.cell[dbindex].sibs.sb2 != NULL )
          {
            sibdb.cell[dbindex].sib_value_tag[rrc_SB2]
             = sb_ptr->elem[n_index].sibSb_Type.u.sysInfoTypeSB2;
            MSG_LOW("SB2 value tag stored", 0, 0, 0);
  
            /* Note that Sched Block 2 is present */
            sb2_present=TRUE;
          }
          break;

        default:
          WRRC_MSG0_ERROR("Unknown value tag type");
          break;
      }/* end switch */
      n_index++;
    }/* end while */

    mib_ptr = sibdb.cell[dbindex].sibs.mib;

    if(EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr))
    {
      ext_siblist_ptr = &(sibdb.cell[dbindex].sibs.mib->v690NonCriticalExtensions.
        v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
  
      n_index = 0;
      while( ext_siblist_ptr->n > n_index )
      {
        if( RRCCHECK_SIB_TypeExt(
              (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
               systemInfoType11bis)
        )
        {
          WRRC_MSG1_HIGH("sib11bis:Updating vtag =%d for sib11bis",
              ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG)&&
             (deferred_meas_read_supported == TRUE) &&
             !(sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_decoded)
             )
          {
            deferred_sib_info.val_tag.sib11bis_val_tag = ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
          }
          else
#endif
          {
            if(sibdb.cell[dbindex].sibs.sib11bis != NULL)
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] =
                 ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
            }
          }
        }
        else
        {
          WRRC_MSG1_ERROR("sib11bis:extensionSIB_Type =%x not supported",
               RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
        }
        n_index++;
      }
    }

#ifdef FEATURE_WCDMA_TO_LTE
    if(EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(mib_ptr))
        {
          ext_siblist2_ptr = &(sibdb.cell[dbindex].sibs.mib->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
          v860NonCriticalExtensions.masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List);
      
          n_index = 0;
          while(ext_siblist2_ptr->n > n_index)
          {
            if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t & T_rrc_SIB_TypeExt2_systemInfoType19)
            {
              WRRC_MSG1_HIGH("sib19:Updating vtag =%d for sib19",
              ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
              if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] == INVALID_MIB_SB_VALUE_TAG)&&
                 (deferred_meas_read_supported == TRUE) &&
                 !(sib_events_ptr->event[index].tsib[rrc_SIB19].sib_decoded))
              {
                deferred_sib_info.val_tag.sib19_val_tag = ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
              }
              else
#endif  
              {
                if(sibdb.cell[dbindex].sibs.sib19 != NULL)
                {
                  sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] =
                    ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
                }
              }
            }
            else
            {
              WRRC_MSG1_ERROR("sib19:extensionSIB_Type =%x not supported",
              ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
            }
            n_index++;
          }
        }
#endif

    /* If scheduling blocks are present, save value tags from 
    them also. */
    if( (sb1_present == TRUE) && (sibdb.cell[dbindex].sibs.sb1!=NULL) )
    {
      count++;
      ptr1 = &(sibdb.cell[dbindex].sibs.sb1->sib_ReferenceList);
      sb1_ptr = sibdb.cell[dbindex].sibs.sb1;

      if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
            sb1_ptr,rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
      {
        ext_siblist_ptr = &(sibdb.cell[dbindex].sibs.sb1->v6b0NonCriticalExtensions.
            sysInfoTypeSB1_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
        
        n_index = 0;
        while( ext_siblist_ptr->n > n_index )
        {
          if(ext_siblist_ptr->elem[n_index].extensionSIB_Type.t == 
               T_rrc_SIB_TypeExt_systemInfoType11bis)
          {
            WRRC_MSG1_HIGH("sib11bis:Updating vtag =%d for sib11bis",
                ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG)&&
              (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_decoded)
               )
            {
              deferred_sib_info.val_tag.sib11bis_val_tag = ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
            }
            else
#endif
            {
              if( sibdb.cell[dbindex].sibs.sib11bis != NULL )
              {
                sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] =
                  ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
              }
            }
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:extensionSIB_Type =%d not supported",
                 RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
          }
          n_index++;
        }
      }

#ifdef FEATURE_WCDMA_TO_LTE
      if((EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(
            sb1_ptr,rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
          {
            ext_siblist2_ptr = &(sibdb.cell[dbindex].sibs.sb1->v6b0NonCriticalExtensions.
             v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List);
        
            n_index = 0;
            while(ext_siblist2_ptr->n > n_index)
            {
              if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t ==
                 T_rrc_SIB_TypeExt2_systemInfoType19)
              {
                WRRC_MSG1_HIGH("sib19:Updating vtag =%d for sib19",
                ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] == INVALID_MIB_SB_VALUE_TAG)&&
                   (deferred_meas_read_supported == TRUE) &&
                   !(sib_events_ptr->event[index].tsib[rrc_SIB19].sib_decoded))
                {
                  deferred_sib_info.val_tag.sib19_val_tag = ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
                }
                else
#endif
                {
                  if( sibdb.cell[dbindex].sibs.sib19 != NULL )
                  {
                    sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] =
                      ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
                  }
                }
              }
              else
              {
                WRRC_MSG1_ERROR("sib19:extensionSIB_Type =%d not supported",
                ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
              }
              n_index++;
            }
          }
#endif
    }
    if( (sb2_present == TRUE) && (sibdb.cell[dbindex].sibs.sb2!=NULL) )
    {
      count++;
        ptr2 = &(sibdb.cell[dbindex].sibs.sb2->sib_ReferenceList);

      sb2_ptr = sibdb.cell[dbindex].sibs.sb2;

      if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
            sb2_ptr,rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
      {
        ext_siblist_ptr = &(sibdb.cell[dbindex].sibs.sb2->v6b0NonCriticalExtensions.
          sysInfoTypeSB2_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
    
        n_index = 0;
        while( ext_siblist_ptr->n > n_index )
        {
          if(ext_siblist_ptr->elem[n_index].extensionSIB_Type.t ==
               T_rrc_SIB_TypeExt_systemInfoType11bis)
          {
            WRRC_MSG1_HIGH("sib11bis:Updating vtag =%d for sib11bis",
                ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG)&&
               (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_decoded)
               )
            {
              deferred_sib_info.val_tag.sib11bis_val_tag = ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
            }
            else
#endif
            {
              if( sibdb.cell[dbindex].sibs.sib11bis != NULL )
              {
                sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] =
                   ext_siblist_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
              }
            }
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:extensionSIB_Type =%d not supported",
                 RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
          }
          n_index++;
        }
      }


#ifdef FEATURE_WCDMA_TO_LTE
      if((EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(
            sb2_ptr,rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
      {
        ext_siblist2_ptr = &(sibdb.cell[dbindex].sibs.sb2->v6b0NonCriticalExtensions.
          v860NonCriticalExtensions.sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List);
    
        n_index = 0;
        while(ext_siblist2_ptr->n > n_index)
        {
          if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t &
             T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            WRRC_MSG1_HIGH("sib19:Updating vtag =%d for sib19",
                ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag);
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] == INVALID_MIB_SB_VALUE_TAG)&&
               (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB19].sib_decoded)
               )
            {
              deferred_sib_info.val_tag.sib19_val_tag = ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
            }
            else
#endif
            {
              if( sibdb.cell[dbindex].sibs.sib19 != NULL )
              {
                sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] =
                   ext_siblist2_ptr->elem[n_index].valueTagInfo.u.cellValueTag;
              }
            }
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:extensionSIB_Type =%d not supported",
                 ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
          }
          n_index++;
        }
      }
#endif
    }

    while( count != 0 )
    {
      /* Sequentialy copy value-tags from SB1 and SB2 */
      count--;
      if(sb1_present)
      {
        sb1_present=FALSE;
        ptr=ptr1;
      }
      else if(sb2_present)
      {
        sb2_present=FALSE;
        ptr=ptr2;
      }

      n_index = 0;;
      while((ptr!=NULL) && (ptr->n > n_index))
      {
        /* Switch on the value tag's sib type and store the value tag */
        switch(ptr->elem[n_index].sib_Type.t)
        {
          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType1):
            
            if( sibdb.cell[dbindex].sibs.sib1 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB1]
               = ptr->elem[n_index].sib_Type.u.sysInfoType1;
              MSG_LOW("SIB1 value tag stored", 0, 0, 0);
            }
            break;
  
          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType2):
    
            if( sibdb.cell[dbindex].sibs.sib2 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB2]
               = ptr->elem[n_index].sib_Type.u.sysInfoType2;
              MSG_LOW("SIB2 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType3):
    
            if( sibdb.cell[dbindex].sibs.sib3 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB3]
               = ptr->elem[n_index].sib_Type.u.sysInfoType3;
              MSG_LOW("SIB3 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType4):
            
            if( sibdb.cell[dbindex].sibs.sib4 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB4]
               = ptr->elem[n_index].sib_Type.u.sysInfoType4;
              MSG_LOW("SIB4 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType5):
            
            if( sibdb.cell[dbindex].sibs.sib5 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB5]
               = ptr->elem[n_index].sib_Type.u.sysInfoType5;
              MSG_LOW("SIB5 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType5bis):
            
            if( sibdb.cell[dbindex].sibs.sib5 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB5]
               = ptr->elem[n_index].sib_Type.u.sysInfoType5bis;
              MSG_LOW("SIB5bis value tag stored as sib5", 0, 0, 0);
            }
            break;


          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType6):
            
            if( sibdb.cell[dbindex].sibs.sib6 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB6]
               = ptr->elem[n_index].sib_Type.u.sysInfoType6;
              MSG_LOW("SIB6 value tag stored", 0, 0, 0);
            }
            break;
         
          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType7):
            
            /* SIB7 does not have a value-tag, it only has an expiration
            timer */
  
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType11):
            /*Missing code. Already taken care when scheduled in MIB*/
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)&&
               (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB11].sib_decoded)
              )
            {
              deferred_sib_info.val_tag.sib11_val_tag = ptr->elem[n_index].sib_Type.u.sysInfoType11;
              WRRC_MSG1_HIGH("Deferred SIB: SIB11 value tag stored %d", deferred_sib_info.val_tag.sib11_val_tag);
            }
            else
#endif
            if( sibdb.cell[dbindex].sibs.sib11 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB11]
               = ptr->elem[n_index].sib_Type.u.sysInfoType11;
              MSG_LOW("SIB11 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType12):
    
        #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] == INVALID_MIB_SB_VALUE_TAG)&&
               (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB12].sib_decoded)
               )
            {
              deferred_sib_info.val_tag.sib12_val_tag = ptr->elem[n_index].sib_Type.u.sysInfoType12;
              WRRC_MSG1_HIGH("Deferred SIB: SIB12 value tag stored %d", deferred_sib_info.val_tag.sib12_val_tag);
            }
            else
        #endif
            if( sibdb.cell[dbindex].sibs.sib12 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB12]
               = ptr->elem[n_index].sib_Type.u.sysInfoType12;
              MSG_LOW("SIB12 value tag stored", 0, 0, 0);
            }
            break;

          case RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType18):
            #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] == INVALID_MIB_SB_VALUE_TAG)&&
               (deferred_meas_read_supported == TRUE) &&
              !(sib_events_ptr->event[index].tsib[rrc_SIB18].sib_decoded)
               )
            {
              deferred_sib_info.val_tag.sib18_val_tag = ptr->elem[n_index].sib_Type.u.sysInfoType18;
              WRRC_MSG1_HIGH("Deferred SIB: SIB18 value tag stored %d", deferred_sib_info.val_tag.sib18_val_tag);
            }
            else
        #endif
            if( sibdb.cell[dbindex].sibs.sib18 != NULL )
            {
              sibdb.cell[dbindex].sib_value_tag[rrc_SIB18]
               = ptr->elem[n_index].sib_Type.u.sysInfoType18;
              MSG_LOW("SIB18 value tag stored", 0, 0, 0);
            }
            break;

          default:
            WRRC_MSG0_ERROR("Unknown value tag type");
            break;
        }/* end switch */
        n_index++;
      }/* end while ptr != NULL */
    }/* end while count != 0 */
  }/* end if MasterInformationblock is not null */
  else 
  {
    WRRC_MSG0_ERROR("MIB ptr is NULL!");
  }
} /* rrc_sibdb_store_value_tags_for_new_cell */



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
)
{
  uint32 count; /* Local counter */
  boolean free_index_found = FALSE; /* Indicate if a space has
                been found to save this registration. */

  /* Check if the given SIB is valid. */
  /*lint -save -e685 */ 
  if( sib >= rrc_ExtensionType )
  {
    WRRC_MSG1_HIGH("Invalid SIB registered %d", sib);
    return;
  }
  /*lint -restore */

  /* Register this in the first available space */
  for( count=0; count < RRC_TOT_SIB_CHG_N; count++ )
  {
    if( rrcsib_cn[count].index_used == FALSE )
    {
      free_index_found = TRUE;
      break; /* Break from for loop */
    }
  }

  if((free_index_found == TRUE ) && (count < RRC_TOT_SIB_CHG_N))
  {
    /* Ok to register this. */
    rrcsib_cn[count].procedure = procedure;   /* Copy procedure   */
    rrcsib_cn[count].sib = sib;         /* Copy sib type          */
    rrcsib_cn[count].index_used = TRUE; /* This index is now used */
    rrcsib_set_priority(count,sib);    /*Set priority for this sibtype,which will
                                          be used for posting sib_change_ind */
  }
  else
  {
    /* All indices are taken up - 
    We probably need to increase the count */
    WRRC_MSG0_ERROR("No space to register SIB CN");
  }
}



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
)
{
  uint32 count; /* Local counter */

  /* Find the previous registration and remove it */
  for( count=0; count < RRC_TOT_SIB_CHG_N; count++ )
  {
    if( rrcsib_cn[count].index_used == TRUE )
    {
      if( (rrcsib_cn[count].procedure == procedure) &&
          (rrcsib_cn[count].sib == sib) )
      {
        /* Match found. This index is no longer used. */
        rrcsib_cn[count].index_used = FALSE;
        break; /* Break from for loop */
      }
    }
  }
}




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
void rrc_sibdb_notify_sib_change_to_procs( void )
{
  rrc_cmd_type *cmd_ptr;
  rrc_sib_change_mask_type sib_change_mask=0;
  uint8 procedure;
  uint8 priority; 

  /* Check which SIB has changed by looking at the
  active cell SIB change indication */
  //  for( count1 =0; count1 < RRC_TOT_SIBS; count1++ )
  /* Processing SIB in the reverse order - from sib 18 to sib 1 */
  for(priority = 1;priority<=rrcsib_cn_max_priority;priority++)
  {
    for(procedure=(uint8)MAX_NUM_OF_PROCEDURES;procedure>(uint8)0;procedure--)
    {
      if(rrcsib_is_it_registered_procedure_for_sibchange((rrc_proc_e_type)procedure) == TRUE)
      {
        if(rrcsib_check_and_prepare_sib_change_mask(&sib_change_mask,(rrc_proc_e_type)procedure,priority) == TRUE)
        {
          WRRC_MSG2_HIGH("SIB change ind sent to proc %d with bit mask =%x", procedure, sib_change_mask);
          if( (cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
          {
            /* Fill up the command type, the sib, and the procedure
            name. */
            cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_CHANGE_IND;
            cmd_ptr->cmd.sib_change_ind.sib_change_mask= sib_change_mask;
            cmd_ptr->cmd.sib_change_ind.procedure = (rrc_proc_e_type)procedure;
            /* Now, send the command. */
            rrc_put_int_cmd( cmd_ptr );
          }
        }
      }
    }
  }
  /* Whenever we get a new SIB1 we need to check if the
  connected mode timers and constants need to be updated.
  */
  if( sibdb.active_cell_sib_changed[rrc_SIB1] == TRUE )
  {
    if( (rrcumi_check_ue_timer_constants_update_status(FALSE))
        == RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ )
    {

      (void)rrc_update_ue_conn_timers_and_constants
        (
          (RRCCHECK_SIB_MSG_IE_PRESENT(sibdb.cell[sibdb.active_cell_index].sibs.sib1,
            rrc_SysInfoType1_, 
            ue_ConnTimersAndConstants))? 
            &(sibdb.cell[sibdb.active_cell_index].sibs.sib1->ue_ConnTimersAndConstants):
            NULL,
          (RRCCHECK_SIB_MSG_IE_PRESENT(sibdb.cell[sibdb.active_cell_index].sibs.sib1,
             rrc_SysInfoType1_, 
             v3a0NonCriticalExtensions))?
            &(sibdb.cell[sibdb.active_cell_index].sibs.sib1->v3a0NonCriticalExtensions.
            sysInfoType1_v3a0ext.ue_ConnTimersAndConstants_v3a0ext):
            NULL,
           ((sibdb.cell[sibdb.active_cell_index].sibs.sib1->m.v3a0NonCriticalExtensionsPresent) &&
            (sibdb.cell[sibdb.active_cell_index].sibs.sib1->v3a0NonCriticalExtensions.m.v860NonCriticalExtentionsPresent))? 
            &(sibdb.cell[sibdb.active_cell_index].sibs.sib1->v3a0NonCriticalExtensions.v860NonCriticalExtentions.
            sysInfoType1_v860ext.ue_ConnTimersAndConstants):
            NULL
        );
    }
    else
    {
      (void)rrc_update_ue_conn_timers_and_constants(NULL, NULL,
        ((sibdb.cell[sibdb.active_cell_index].sibs.sib1->m.v3a0NonCriticalExtensionsPresent) &&
          (sibdb.cell[sibdb.active_cell_index].sibs.sib1->v3a0NonCriticalExtensions.m.v860NonCriticalExtentionsPresent))? 
          &(sibdb.cell[sibdb.active_cell_index].sibs.sib1->v3a0NonCriticalExtensions.v860NonCriticalExtentions.
          sysInfoType1_v860ext.ue_ConnTimersAndConstants):
          NULL
        );
    }
  }
}/* end function - rrcscmgr_notify_sib_change_to_procs */
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
)
{
  rrc_SIBSb_ReferenceList *sb_ptr; /* Local pointer to SIBSb ref list */
  boolean match_found = FALSE;    /* Indicate if match is found      */
  rrc_MasterInformationBlock *mib = NULL;
  uint32 n_value=0, index=0;

  if( sibdb.cell[dbindex].sib_valid[rrc_MIB] )
  {
    /* If the MIB is valid */
    mib = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);
    if(mib != NULL)
    {
      sb_ptr = &(mib->sibSb_ReferenceList);

      n_value = sb_ptr->n;
      index = 0;
      while( n_value != 0 )
      {
         /* Check if the given scheduling block matches
            this particular sib type */
        if( (RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[index].sibSb_Type),sysInfoTypeSB1)) &&
           (sib == rrc_SB1) )
        {
          /* We found it */
          match_found = TRUE;
          break; /* Out of the while loop */
        }
        else if( (RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[index].sibSb_Type),sysInfoTypeSB2)) &&
                 (sib == rrc_SB2) )
        {
          /* We found it */
          match_found = TRUE;
          break; /* Out of the while loop */
        }
        /* Now increment the index */
        n_value--;
        index++;
      }
    }                     
    else
    {
      /* If the MIB hasn't been read yet, we'll assume
          the scheduling block is present. */
      match_found = TRUE;
    }
  }
  return( match_found );
}
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
void rrc_sibdb_clear_active_cell_sib_change( void )
{
  uint32 index;
  
  /* Clear the Active Cells SIB Change indications */
  for( index=0; index<RRC_TOT_SIBS; index++ )
  {
    sibdb.active_cell_sib_changed[index] = FALSE;
  }
}


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
)
{

  /* If the given sib is not in the valid range indicate
  the error. */
  /*lint -save -e685 */ 
  if(!(SIBDB_VALID_SIB_RANGE(sib)))
  {
    WRRC_MSG1_ERROR("Invalid SIB %d", sib);
  }
  /*lint -restore */
  else
  {
    /* Note that this particular SIB has changed */
/*lint -e661 lint bug - incorrect out of bounds for ? operator */
    sibdb.active_cell_sib_changed[sib] = TRUE;
/*lint +e661 */
  }
}


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
)
{
  rrcsib_cell_id_convert_type temp;
  uint32 dbindex;

  if( (cell_id == NULL ) || (length == NULL) )
  {
    WRRC_MSG0_ERROR("Uninitialized pointers!");
    return(FALSE);
  }

  if( sibdb.active_cell_sibs_valid == FALSE )
  {
    WRRC_MSG0_HIGH("No active cell in sibdb!");
    return(FALSE);
  }

  /* Note the current active index */
  dbindex = sibdb.active_cell_index;

  /* Copy the cell-id length directly from SIB3 */
  if( sibdb.cell[dbindex].sib_valid[rrc_SIB3] )
  {
    /* Ok to typecast to uint8 since longest length is only 28 */
    *length = (uint8)sibdb.cell[dbindex].sibs.sib3->cellIdentity.numbits;
  }
  else
  {
    WRRC_MSG0_ERROR("SIB3 not present in serving cell!");
    return(FALSE);
  }

  /* Copy cell-id from database into union */
  temp.int_cell_id = sibdb.cell[dbindex].cell_id;

  /* Copy first 2 bytes into received pointer. */
  *cell_id = temp.ext_cell_id[0];
  *(cell_id+1) = temp.ext_cell_id[1];

  //purpously changging order here between 3 and 2, since GSTK want's it in the reverse order
  *(cell_id+2) = temp.ext_cell_id[3];
  *(cell_id+3) = temp.ext_cell_id[2];
  
  return(TRUE);
  
}

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
  uint32 rrc_get_serving_cell_mib_value_tag(void)
  {
    uint32 dbindex;
    if(!(sibdb.active_cell_sibs_valid))
    {
      WRRC_MSG0_HIGH("No active cell selected yet!");
      return(INVALID_MIB_SB_VALUE_TAG);
    }
    dbindex = sibdb.active_cell_index;
    WRRC_MSG1_HIGH("Stored MIB V-tag=%d", sibdb.cell[dbindex].sib_value_tag[rrc_MIB]);
    return (sibdb.cell[dbindex].sib_value_tag[rrc_MIB]);
  }

/*===========================================================================

FUNCTION RRC_SIBDB_FIND_CELL_INDEX_FOR_PLMN_FREQ_PSC

DESCRIPTION

  This function tries to find the index in the sib
  database for the given plmn-id, frequency and primary scrambling
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
  uint32 scr_code, /* PSC of the cell       */
  sib_events_index_e_type  event_index /*Current sib event index*/
)
{    
  uint32 index=SIBDB_INVALID_INDEX ;  /* To find the matching index */

  /* Try to match the PLMN and Cell Ids to one of the cells
  stored in the database. */
  for( index=0; index<=SIBDB_MAX_INDEX; index++ )
  {
    /* If this particular index is being used */
    if( sibdb.cell[index].index_used == TRUE )
    {
      /* Match the freq and scr_code. */
      if( (freq == sibdb.cell[index].freq) &&
          (scr_code == sibdb.cell[index].scr_code) &&
          (rrc_bcch_plmns_match((plmn), (sibdb.cell[index].plmn_id))))
      {
        if((rrcsib_is_need_to_cleanup_cell(event_index) == TRUE) &&
            (sibdb.cell[index].six_hr_count  >=  MAX_CELL_INDEX_COUNT_SIXHR_CELL_SEL_RESEL))
        {
            WRRC_MSG2_HIGH("6hr sib:Clean sib db Index =%d,six hr count =%d ",index,sibdb.cell[index].six_hr_count);
            rrc_sibdb_clear_a_cell_index(index);
            index=SIBDB_INVALID_INDEX ;
        }
        /* A Match has been found. Get out of the for loop. */
        break; /* Break out of the for loop */
      }
    }
  }/* end for */

  return( index );
 
} /* end function rrc_sibdb_find_cell_index_for_freq_psc */


/*===========================================================================

FUNCTION rrc_sibdb_free_cells

DESCRIPTION

  
  
DEPENDENCIES

  

RETURN VALUE

  boolean indicating if SIBDB was able to free up the space required to decode the
  newly arrived SIB's. 

SIDE EFFECTS

  will need to modify function when reading SIBs is supported in dedicated mode of opera\tion.

===========================================================================*/
boolean rrc_sibdb_free_cells
(
  void
)
{    
  boolean   status = FALSE;
  /* Index of the serving cell in SIBDB */
  uint32    serving_cell_index; 
  uint8     count;

  /* Index of cell in SIBDB for current active event */
  uint32    cell_index_for_current_active_event = SIBDB_MAX_CELLS;
    
  /* If the database is empty, just return false. */
  if(sibdb.database_empty)
  {
    /* There is no space to free from the SIB-DB. */
    return(status); /* Return FALSE */
  }

  // Get the serving cell index ie the cell that we last camped on successfully
  serving_cell_index = sibdb.active_cell_index;
  WRRC_MSG1_HIGH("SIB Heap: %d serving Cell Idx", serving_cell_index);
  
  // Go through the events list 
  for (count=0; count<(uint8)MAX_SIB_EVENTS_INDEX;count++)
  {
    // Get the event which is currently active.
    if (sib_events_ptr->event[count].event_active == TRUE)
    {
      if(sib_events_ptr->event[count].sib_db_index_found == TRUE)
      {
        //Get the cell index for the event that is currently active
        cell_index_for_current_active_event = sib_events_ptr->event[count].sib_db_index;
      }
      MSG_HIGH("SIB Heap: %d Ev_actv, %d SIBDB Cell Idx,DB idx found=%d", count, cell_index_for_current_active_event,
        sib_events_ptr->event[count].sib_db_index_found); 
      break;
    }
  }
  if (count == (uint8)MAX_SIB_EVENTS_INDEX) 
  {

    /*For sib heap opt feature, SIB's are decoded on the need basis.
    *Hence event may not be active 
    */
    for( count=0; count<=SIBDB_MAX_INDEX; count++ )
    {
      if( /* If this is not the serving cell's index */ 
          (count != serving_cell_index) &&   
          /* And if this cell index is valid */
          (sibdb.cell[count].index_used == TRUE)   
          )
      {
        // Free the sibs associated with this cell
        WRRC_MSG1_HIGH("SIB Heap: Freeing %d SIB CellIdx freecells", count);
        rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
        status = TRUE;
      }
    }

    return status;
  }
  for( count=0; count<=SIBDB_MAX_INDEX; count++ )
  {
    if( /* If this is not the serving cell's index */ 
        (count != serving_cell_index) &&   
    /* If this cell index is not being used by current active event */
        (count != cell_index_for_current_active_event) &&
        /* And if this cell index is valid */
        (sibdb.cell[count].index_used == TRUE)   
        )
    {
      sibdb.cell[count].index_used = FALSE;
      rrc_sibdb_cell_header_init(count);
      
      if(sibdb.num_stored_cells)
      {
        sibdb.num_stored_cells--;
#ifdef FEATURE_FEMTO_CSG
      if(sibdb.cell[count].is_csg)
          sibdb.num_csg_cells--;
#endif
      }
      status = TRUE;
    }
    
  }
  // Mark the age of the active_age_index as 0
  if(status)
  {
    rrc_sibdb_adjust_age_index(cell_index_for_current_active_event);
  }

  return( status );

} /* end function rrc_sibdb_free_cells */


/*===========================================================================

FUNCTION rrc_sibdb_update_six_hr_timer

DESCRIPTION

  This function will Implement the required six hours 
  Timer for each of the valid sib db index.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/

void  rrc_sibdb_update_six_hr_timer(void)
{
  uint8     count;

  for( count=0; count<=SIBDB_MAX_INDEX; count++ )
  {
    if(sibdb.cell[count].index_used == TRUE)
    {
      if(sibdb.cell[count].six_hr_count !=MAX_CELL_INDEX_COUNT_SIXHR)
      {
        sibdb.cell[count].six_hr_count ++;
      }
      WRRC_MSG2_HIGH("6hr sib:Count value for index = %d is %d",count,sibdb.cell[count].six_hr_count);
    }
  }
  //Start 10min timer for getting six hours timer
  rrctmr_start_timer( RRCTMR_SIXHR_SIB_UPDATE_TIMER,
                                   RRCSIB_SIX_HR_SIB_UPDATE_TIMER );
}

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
void rrc_sibdb_clear_a_cell_index(uint32 dbindex)
{
  if (dbindex != SIBDB_INVALID_INDEX)
  {
    sibdb.cell[dbindex].index_used = FALSE; /* This index is now used */
    if(sibdb.num_stored_cells)
    {
      sibdb.num_stored_cells--;
#ifdef FEATURE_FEMTO_CSG
      if(sibdb.cell[dbindex].is_csg)
        sibdb.num_csg_cells--;
#endif
    }
          
#ifdef FEATURE_FEMTO_CSG
    rrc_sibdb_decrement_age_index(dbindex);
#else
    rrc_sibdb_adjust_age_index( dbindex ); /* Why do I need to do this??Adjust age index for cells */
#endif
    rrc_sibdb_cell_header_init(dbindex);

  
    WRRC_MSG1_HIGH("Clearing the sibdb_var's associated with sibdb_index %d", dbindex);
  }
  else
  {
    WRRC_MSG1_HIGH("InValid SIB-DB Index=%d",dbindex);
  }
}

/*===========================================================================

FUNCTION rrcsib_encode_sib_change_mask

DESCRIPTION

  This function prepares sib change indication bit mask.
  which will be sent to all the procedures which have registered 
  with sib for a notification
  
DEPENDENCIES
  None.

RETURN VALUE
TRUE : If Sibs have changed for this registered procedure
FALSE: Otherwise

SIDE EFFECTS
  None.
     
===========================================================================*/
uint8 rrcsib_check_and_prepare_sib_change_mask
(
rrc_sib_change_mask_type *sib_mask,
rrc_proc_e_type procedure,
uint8 priority
)
{
  uint8 i,count;
  uint8 sib_change_status = FALSE;
  rrc_sib_change_mask_type  gen_mask, sib_change_mask;
  *sib_mask = 0;
  sib_change_mask = MIB_BIT;
  for( count=0; count < RRC_TOT_SIB_CHG_N; count++ )
  {
    if((rrcsib_cn[count].procedure == procedure)&&
        (rrcsib_cn[count].index_used == TRUE)&&
        (rrcsib_cn[count].priority== priority)
      )
    {
      sib_change_mask |= (MIB_BIT<<(rrcsib_cn[count].sib));
      /* Indicate which SIBs are changed */
      gen_mask = MIB_BIT;
      for(i = 0;i < RRC_TOT_SIBS;i++, gen_mask <<= 1)
      {
        if((sibdb.active_cell_sib_changed[i] == TRUE)&&
           (rrcsib_cn[count].sib ==(rrc_SIB_type)i)
          )
        {
          *sib_mask |= gen_mask;
          MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
                "SIBType = %d is changed,Procedure intrested in it = %d prepare_sib_change_mask = %x, gen mask=%x, sibType=%d", 
                (rrc_SIB_type)i,procedure,*sib_mask,gen_mask,i);
          if(sib_change_status == FALSE)
          {
            sib_change_status = TRUE;
          }
          break;
        }
      }
    }
  }
  WRRC_MSG2_HIGH("sib_change_mask for procedure=%d is %d ",procedure,sib_change_mask);

  return sib_change_status;
}
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
uint8 rrcsib_is_it_registered_procedure_for_sibchange(rrc_proc_e_type procedure)
{
  uint8 count;
  uint8 status=FALSE;
  
  for( count=0; count < RRC_TOT_SIB_CHG_N; count++ )
  {
    if((rrcsib_cn[count].procedure == procedure)&&
        (rrcsib_cn[count].index_used == TRUE)
      )
    {
      status = TRUE;
      break;
    }
  }
  return status;
}
/*===========================================================================

FUNCTION rrcsib_set_priority

DESCRIPTION

  This function will set a priority for reveived sib type
  for registration which will be used for posting sib change ind
  to procedures when ever these sibs are changed.
DEPENDENCIES
  Boundary check of the index should be done by the caller of this function

RETURN VALUE
none
SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_set_priority(uint32 index,rrc_SIB_type sib)
{
  /*These case hard codes priorities for the sib type which are being registered
    by procedures. If  a sib type change needs to be intimated to procedures ahead
    of other, then the priorities needs to be changed. Current sequence should not 
    be altered.*/

  switch(sib)
  {
    case rrc_SIB5:
    case rrc_SIB6:
      rrcsib_cn[index].priority = 1;
      break;
      
    case rrc_SIB11:
    case rrc_SIB12:
    case rrc_SIB18:
    case rrc_SIB11bis:

#ifdef FEATURE_WCDMA_TO_LTE
    case rrc_SIB19:
#endif

      rrcsib_cn[index].priority = 2;
      break;
     
    case rrc_SIB1:
    case rrc_SIB3:
    case rrc_SIB4:
      rrcsib_cn[index].priority = 3;
      break;
      
    case rrc_SIB2:
      rrcsib_cn[index].priority = 4;
      break;
      
    default:
      rrcsib_cn[index].priority = 5;
      break;
  }
  
  if(rrcsib_cn_max_priority < rrcsib_cn[index].priority)
  {
    rrcsib_cn_max_priority = rrcsib_cn[index].priority; 
  }
}
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
)
{
  return(sibdb.active_cell_sibs_valid);
}
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
)
{
  uint32 count;  /* Counter */

  for( count=0; count <=SIBDB_MAX_INDEX; count++ )
  {
    if( sibdb.cell[count].index_used == TRUE)
    {
      MSG_HIGH("Cell I.d =%d, fre=%d,scr_code=%d",sibdb.cell[count].cell_id,
                                                    sibdb.cell[count].freq,
                                                    sibdb.cell[count].scr_code);
      PRINT_PLMN_ID(sibdb.cell[count].plmn_id)
    }
  }
}
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
)
{
  boolean status = FALSE;
  if(!sibdb.active_cell_sibs_valid)
  {
    return FALSE;
  }
  /*Check if SIB11bis present for serving cell*/
  if(sibdb.cell[sibdb.active_cell_index].sib_valid[rrc_SIB11bis] &&
     (rrcsib_check_if_extension_sib_present(sibdb.active_cell_index) == TRUE)
    )
  {
    WRRC_MSG0_HIGH("sib11bis:SIB11bis present for serving cell");
    status = TRUE;
  }
  return status;
}



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
)
{
  if(index >= SIBDB_INVALID_INDEX)
  {
    WRRC_MSG1_HIGH("SIB_HEAP:Bad sibbd idx=%d", index );
    return;
  }
  /* Note that even though we don't support all these SIBs now,
  the code has been added here since additional SIBs will be supported
  in future releases */
  if( sibdb.cell[index].sibs.mib != NULL )
  {
    rrc_sibdb_free_sib(rrc_MasterInformationBlock_PDU,
                       (void *)sibdb.cell[index].sibs.mib);
    sibdb.cell[index].sibs.mib = NULL;
  }
  if( sibdb.cell[index].sibs.sib1 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,
                       (void *)sibdb.cell[index].sibs.sib1);
    sibdb.cell[index].sibs.sib1 = NULL;
  }
  if( sibdb.cell[index].sibs.sib2 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType2_PDU,
                       (void *)sibdb.cell[index].sibs.sib2);
    sibdb.cell[index].sibs.sib2 = NULL;
  }
  if( sibdb.cell[index].sibs.sib3 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType3_PDU,
                       (void *)sibdb.cell[index].sibs.sib3);
    sibdb.cell[index].sibs.sib3 = NULL;
  }
  if( sibdb.cell[index].sibs.sib4 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType4_PDU,
                       (void *)sibdb.cell[index].sibs.sib4);
    sibdb.cell[index].sibs.sib4 = NULL;
  }
  if( sibdb.cell[index].sibs.sib5 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType5bis_PDU,
                       (void *)sibdb.cell[index].sibs.sib5);
    sibdb.cell[index].sibs.sib5 = NULL;
  }
  if( sibdb.cell[index].sibs.sib6 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType6_PDU,
                       (void *)sibdb.cell[index].sibs.sib6);
    sibdb.cell[index].sibs.sib6 = NULL;
  }
  if( sibdb.cell[index].sibs.sib7 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType7_PDU,
                       (void *)sibdb.cell[index].sibs.sib7);
    sibdb.cell[index].sibs.sib7 = NULL;
  }
  if( sibdb.cell[index].sibs.sib11 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType11_PDU,
                       (void *)sibdb.cell[index].sibs.sib11);
    sibdb.cell[index].sibs.sib11 = NULL;
  }
  if( sibdb.cell[index].sibs.sib12 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType12_PDU,
                       (void *)sibdb.cell[index].sibs.sib12);
    sibdb.cell[index].sibs.sib12 = NULL;
  }
  if( sibdb.cell[index].sibs.sib18 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType18_PDU,
                       (void *)sibdb.cell[index].sibs.sib18);
    sibdb.cell[index].sibs.sib18 = NULL;
  }
  if( sibdb.cell[index].sibs.sb1 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoTypeSB1_PDU,
                       (void *)sibdb.cell[index].sibs.sb1);
    sibdb.cell[index].sibs.sb1 = NULL;
  }
  if( sibdb.cell[index].sibs.sb2 != NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoTypeSB2_PDU,
                       (void *)sibdb.cell[index].sibs.sb2);
    sibdb.cell[index].sibs.sb2 = NULL;
  }
  if( sibdb.cell[index].sibs.sib11bis!= NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType11bis_PDU,
                       (void *)sibdb.cell[index].sibs.sib11bis);
    sibdb.cell[index].sibs.sib11bis= NULL;
  }
#ifdef FEATURE_WCDMA_TO_LTE
  if( sibdb.cell[index].sibs.sib19!= NULL )
  {
    rrc_sibdb_free_sib(rrc_SysInfoType19_PDU,
                       (void *)sibdb.cell[index].sibs.sib19);
    sibdb.cell[index].sibs.sib19= NULL;
  }
#endif

}



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
)
{
  uint32 count,index; /* Counter */
  WRRC_MSG0_HIGH("clear all cell header in sibdb");

  for(index = 0; index < SIBDB_MAX_CELLS; index++)
  {
    for( count = 0; count < RRC_TOT_SIBS; count++ )
    {
      /* Initialize each SIB's value tag to 0 which is not a valid
      value tag. This is to indicate that no value tag is stored
      in this index yet. */
      sibdb.cell[index].sib_value_tag[count] = 0; 
    }
    sibdb.cell[index].six_hr_count  = 0;
  }
}
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
)
{
  rrc_SysInfoType3 *sib3_ptr = NULL;         /* Local pointer to SIB3       */
  l1_meas_quan_enum_type meas_quan = L1_MEAS_NONE;

  sib3_ptr = rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
    
  if(sib3_ptr!=NULL)
  {
    if(INTRA_FREQ_MEAS_QUAN_FOR_DEFERRED_SIB(sib3_ptr))
    {
      if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD == rrc_IntraFreqMeasQuantity_FDD_sib3_cpich_Ec_N0)
      {
        meas_quan = L1_EC_NO;
      }
      else if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD == rrc_IntraFreqMeasQuantity_FDD_sib3_cpich_RSCP)
      {
        meas_quan = L1_RSCP;
      }
      WRRC_MSG1_HIGH("Deferred SIB:Meas Quanity included  in SIB3 is %d",meas_quan);
    }
    else
    {
       meas_quan = L1_RSCP;
       WRRC_MSG0_HIGH("Deferred SIB:Meas Quanity not indicated in SIB3.Use RSCP");
    }
  }
  return meas_quan;
}

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
  sib_events_index_e_type index,
  void *block_ptr,    /* Pointer to the MIB, SB1 or SB2, typecast
                      to void */
  rrc_SIB_type sib   /* Indicate which SIB the value-tags are from
                      This can be the MIB or SB1 or SB2. */
)
{

  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */

  rrc_SIBSb_ReferenceList *sb_ptr = NULL; /* Local pointer to SIBSb ref list */
  rrc_SIB_ReferenceList *ptr = NULL;      /* Local pointer to SIB ref list   */
  uint32 idx;
  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;
#ifdef FEATURE_WCDMA_TO_LTE
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;  
#endif
  uint8 i;                   /* Local counter                   */
  rrcsib_sib_type_vtag sib_type_tag;
  memset((void *)&sib_type_tag, 0, sizeof(rrcsib_sib_type_vtag));

  if((deferred_sib_info.cell_id != sibdb.cell[dbindex].cell_id) ||
      !(rrc_bcch_plmns_match(deferred_sib_info.plmn_id, sibdb.cell[dbindex].plmn_id))
     )
  {
    WRRC_MSG0_HIGH("Deferred SIB:Cell id and PLMN id dont match");
  }
  else
  {
    WRRC_MSG0_HIGH("Deferred SIB: Copying Deferred SIB DB to TSIB");
    if( sib == rrc_MIB )
    {
      /* Typecast to MIB pointer */
      mib_ptr = (rrc_MasterInformationBlock *)block_ptr;
      sb_ptr = &(mib_ptr->sibSb_ReferenceList);

      if(EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr))
      {
        ext_siblist_ptr = &(mib_ptr->v690NonCriticalExtensions.
          v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
      }
#ifdef FEATURE_WCDMA_TO_LTE
      if((EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(mib_ptr)))
      {
        ext_siblist2_ptr = &(mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.
          masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List);
      }
#endif
      idx = 0;
      while( sb_ptr->n > idx )
      {
          /* Switch on the value tag's sib type and look
          at the corrosponding SIBs stored value tag for
          a match */
        rrcsib_get_sib_type_from_sibsb_type_and_tag(&sb_ptr->elem[idx].sibSb_Type,
                                                    &sib_type_tag);
        if(((sib_type_tag.sib_type == rrc_SIB11) &&
            (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib11_val_tag))
            ||
            ((sib_type_tag.sib_type == rrc_SIB12) &&
            (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib12_val_tag))
            || ((sib_type_tag.sib_type == rrc_SIB18) &&
                 (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib18_val_tag))
          )
        {
          if((sib_type_tag.sib_type == rrc_SIB11)&&
             (sib_events_ptr->event[index].tsib[rrc_SIB11].seg_index < 
               deferred_sib_info.dsib.sib11.seg_index)
             )
          {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB11 from DSIB", deferred_sib_info.dsib.sib11.seg_index);
            sib_events_ptr->event[index].tsib[rrc_SIB11].seg_index = 
              deferred_sib_info.dsib.sib11.seg_index;
  
            sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length = 
              deferred_sib_info.dsib.sib11.sib_tot_length;

            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB11].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib11.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }
          else if((sib_type_tag.sib_type == rrc_SIB12)&&
                  (sib_events_ptr->event[index].tsib[rrc_SIB12].seg_index < 
                    deferred_sib_info.dsib.sib12.seg_index)
                 )
          {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB12 from DSIB", deferred_sib_info.dsib.sib12.seg_index);
            sib_events_ptr->event[index].tsib[rrc_SIB12].seg_index = 
              deferred_sib_info.dsib.sib12.seg_index;
  
            sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length = 
              deferred_sib_info.dsib.sib12.sib_tot_length;
  
            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB12].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib12.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }
          else if((sib_type_tag.sib_type == rrc_SIB18)&&
                  (sib_events_ptr->event[index].tsib[rrc_SIB18].seg_index < 
                   deferred_sib_info.dsib.sib18.seg_index)
             )
          {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB18 from DSIB", deferred_sib_info.dsib.sib18.seg_index);
            sib_events_ptr->event[index].tsib[rrc_SIB18].seg_index = 
            deferred_sib_info.dsib.sib18.seg_index;
          
            sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length = 
            deferred_sib_info.dsib.sib18.sib_tot_length;
          
            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB18].sib_data_ptr,  /* Source Pointer */
              0,/* Source Offset is always 0 since we copy all the received bits */
              deferred_sib_info.dsib.sib18.sib_data_ptr, /* Destination Pointer */
              sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length,  /* Destination Offset */
              FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
            );
          }

          for( i = 0 ;i<MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD;i ++)
          {
            if(deferred_sib_info.deferred_sib_seg_db[i].sib_type == sib_type_tag.sib_type)
            {
              (void) rrcsib_store_sib_seg_in_tsib_optim_db(deferred_sib_info.deferred_sib_seg_db[i].sib_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sfn_prime,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_index,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sib_seg_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_length,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_ptr,
                                                   index);
  
            }
          }
          (void) rrcsib_process_sib_seg_in_tsib_optim_db(sib_type_tag.sib_type,index);
        }
        idx++;
      }
    }
    else if( sib == rrc_SB1 || sib == rrc_SB2 )
    {
      if(sib == rrc_SB1)
      {
        /* Value tag is from Scheduling Block 1 
        Copy the pointer and compare the received
        value tags. */
        sb1_ptr = (rrc_SysInfoTypeSB1 *)block_ptr;
        ptr = &(sb1_ptr->sib_ReferenceList);
        if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
              sb1_ptr,rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
        {
          ext_siblist_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB1_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
        }
#ifdef FEATURE_WCDMA_TO_LTE
       if((EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb1_ptr,rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
       {
         ext_siblist2_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List);
       }
#endif       
      }

      else if( sib == rrc_SB2 ) 
      {
        /* Value tag is from Scheduling Block 2 
        Copy the pointer and compare the received
        value tags. */
        sb2_ptr = (rrc_SysInfoTypeSB2 *)block_ptr;
        ptr = &(sb2_ptr->sib_ReferenceList);
        if((EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(
              sb2_ptr,rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
        {
          ext_siblist_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB2_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
        }
#ifdef FEATURE_WCDMA_TO_LTE
       if((EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb2_ptr,rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
      {
        ext_siblist2_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
        sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List);
      }
#endif       
      }
      idx = 0;
      while( ptr->n > idx)
      {
          /* Switch on the value tag's sib type and look
          at the corrosponding SIBs stored value tag for
          a match */
        rrcsib_get_sib_type_from_sib_type_and_tag(&ptr->elem[idx].sib_Type,
                                                    &sib_type_tag);
        if(((sib_type_tag.sib_type == rrc_SIB11) &&
            (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib11_val_tag))
            ||
            ((sib_type_tag.sib_type == rrc_SIB12) &&
            (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib12_val_tag))
            || ((sib_type_tag.sib_type == rrc_SIB18) &&
                 (sib_type_tag.sib_tag == deferred_sib_info.val_tag.sib18_val_tag))
          )
        {
        
        if((sib_type_tag.sib_type == rrc_SIB11)&&
           (sib_events_ptr->event[index].tsib[rrc_SIB11].seg_index < 
             deferred_sib_info.dsib.sib11.seg_index)
           )
        {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB11 from DSIB", deferred_sib_info.dsib.sib11.seg_index);

            sib_events_ptr->event[index].tsib[rrc_SIB11].seg_index = 
              deferred_sib_info.dsib.sib11.seg_index;
          
            sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length = 
              deferred_sib_info.dsib.sib11.sib_tot_length;

            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB11].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib11.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }
          else if((sib_type_tag.sib_type == rrc_SIB12)&&
                  (sib_events_ptr->event[index].tsib[rrc_SIB12].seg_index < 
                    deferred_sib_info.dsib.sib12.seg_index)
                 )
          {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB12 from DSIB", deferred_sib_info.dsib.sib12.seg_index);
            sib_events_ptr->event[index].tsib[rrc_SIB12].seg_index = 
              deferred_sib_info.dsib.sib12.seg_index;
          
            sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length = 
              deferred_sib_info.dsib.sib12.sib_tot_length;
          
            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB12].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib12.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }
          else if((sib_type_tag.sib_type == rrc_SIB18)&&
                  (sib_events_ptr->event[index].tsib[rrc_SIB18].seg_index < 
                    deferred_sib_info.dsib.sib18.seg_index)
                 )
          {
            WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB18 from DSIB", deferred_sib_info.dsib.sib18.seg_index);
            sib_events_ptr->event[index].tsib[rrc_SIB18].seg_index = 
              deferred_sib_info.dsib.sib18.seg_index;
          
            sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length = 
              deferred_sib_info.dsib.sib18.sib_tot_length;
          
            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB18].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib18.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }

          for( i = 0 ;i<MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD;i ++)
          {
            if(deferred_sib_info.deferred_sib_seg_db[i].sib_type == sib_type_tag.sib_type)
            {
              (void) rrcsib_store_sib_seg_in_tsib_optim_db(deferred_sib_info.deferred_sib_seg_db[i].sib_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sfn_prime,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_index,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sib_seg_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_length,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_ptr,
                                                   index);
            }
          }
          (void) rrcsib_process_sib_seg_in_tsib_optim_db(sib_type_tag.sib_type,index);
        }
        else
        {
          WRRC_MSG2_HIGH("Deferred SIB: Deferred SIB val tag %d, SIB val tag %d", deferred_sib_info.val_tag.sib11_val_tag ,sib_type_tag.sib_tag);
        }
        idx++;
      }
    }

    idx = 0;
    while((ext_siblist_ptr != NULL) && (ext_siblist_ptr->n > idx))
    {
      if(ext_siblist_ptr->elem[idx].extensionSIB_Type.t == T_rrc_SIB_TypeExt_systemInfoType11bis)
      {
        WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB11bis from DSIB", deferred_sib_info.dsib.sib11bis.seg_index);

        if(ext_siblist_ptr->elem[idx].valueTagInfo.u.cellValueTag == 
            deferred_sib_info.val_tag.sib11bis_val_tag)
        {
          if(sib_events_ptr->event[index].tsib[rrc_SIB11bis].seg_index < 
               deferred_sib_info.dsib.sib11bis.seg_index)
          {
            sib_events_ptr->event[index].tsib[rrc_SIB11bis].seg_index = 
              deferred_sib_info.dsib.sib11bis.seg_index;
          
            sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_tot_length = 
              deferred_sib_info.dsib.sib11bis.sib_tot_length;

            b_copy( sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib11bis.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
          }
          for( i = 0 ;i<MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD;i ++)
          {
            if(deferred_sib_info.deferred_sib_seg_db[i].sib_type == sib_type_tag.sib_type)
            {
              (void) rrcsib_store_sib_seg_in_tsib_optim_db(deferred_sib_info.deferred_sib_seg_db[i].sib_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sfn_prime,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_index,
                                                   deferred_sib_info.deferred_sib_seg_db[i].sib_seg_type,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_length,
                                                   deferred_sib_info.deferred_sib_seg_db[i].seg_data_ptr,
                                                   index);
          
            }
          }
          (void) rrcsib_process_sib_seg_in_tsib_optim_db(sib_type_tag.sib_type,index);
        }
      }
      idx++;
    }

#ifdef FEATURE_WCDMA_TO_LTE
  idx = 0;
  while((ext_siblist2_ptr != NULL) && (ext_siblist2_ptr->n > idx))
  {
    if(ext_siblist2_ptr->elem[idx].extensionSIB_Type2.t == T_rrc_SIB_TypeExt2_systemInfoType19)
    {
      WRRC_MSG1_HIGH("Deferred SIB: Copying %d segments of SIB19 from DSIB", deferred_sib_info.dsib.sib19.seg_index);
  
      if(ext_siblist2_ptr->elem[idx].valueTagInfo.u.cellValueTag == 
         deferred_sib_info.val_tag.sib19_val_tag)
      {
        if(sib_events_ptr->event[index].tsib[rrc_SIB19].seg_index < 
           deferred_sib_info.dsib.sib19.seg_index)
        {
          sib_events_ptr->event[index].tsib[rrc_SIB19].seg_index = 
            deferred_sib_info.dsib.sib19.seg_index;
         
          sib_events_ptr->event[index].tsib[rrc_SIB19].sib_tot_length = 
            deferred_sib_info.dsib.sib19.sib_tot_length;
  
          b_copy( sib_events_ptr->event[index].tsib[rrc_SIB19].sib_data_ptr,  /* Source Pointer */
             0,	/* Source Offset is always 0 since we copy all the received bits */
             deferred_sib_info.dsib.sib19.sib_data_ptr, /* Destination Pointer */
             sib_events_ptr->event[index].tsib[rrc_SIB19].sib_tot_length,  /* Destination Offset */
             FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
            );
        }
        for( i = 0 ;i<MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD;i ++)
        {
          if(deferred_sib_info.deferred_sib_seg_db[i].sib_type == sib_type_tag.sib_type)
          {
           (void) rrcsib_store_sib_seg_in_tsib_optim_db(deferred_sib_info.deferred_sib_seg_db[i].sib_type,
              deferred_sib_info.deferred_sib_seg_db[i].sfn_prime,
              deferred_sib_info.deferred_sib_seg_db[i].seg_index,
              deferred_sib_info.deferred_sib_seg_db[i].sib_seg_type,
              deferred_sib_info.deferred_sib_seg_db[i].seg_data_length,
              deferred_sib_info.deferred_sib_seg_db[i].seg_data_ptr,
              index);
          }
        }
       (void) rrcsib_process_sib_seg_in_tsib_optim_db(sib_type_tag.sib_type,index);
      }
    }
    idx++;
  }
#endif

  }
return;
}
#endif

