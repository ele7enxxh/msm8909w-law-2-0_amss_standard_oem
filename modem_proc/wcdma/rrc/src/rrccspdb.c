/*===========================================================================
                        R R C    C S P    D B
DESCRIPTION

   This file contains the access routines for Stored Acquisition Database 
   used for Cell Selection Procedure.

   The Stored Acquisition Database is part of the PLMN Search mechanism. 
   The implemented blocks are marked with a "=======" boundary below.

              CSP PLMN SEARCH MECHANISM
              
       -----------   ------------    =============    =========
       |         |   | CSP      |    | Acq DB    |    | Acq   |
       |CSP      |   | Initial  |    | Access    |    | DB    |
       |State    |   | And .    |<-->| Functions |<-->|       |
       |Machine  |   | Stored   |    | -PLMN     |    |       |
       |Functions|<->|Selection |    | -Acq type |    |       |
       |         |   |Controller|    =============    =========
       |         |   |-PLMN     |    
       |         |   |-PLMN     |    ------------
       -----------   | type     |<-->| CSP      |
            |        ------------    | Initial  |
            |           |            | Selection|
            |           |            | Algorithm| 
       -----------      |            ------------            
       |Internal |   ------------       
       |  Data   |   |Selection |
       |         |   |          |
       -----------   |Data      |
                     ------------

EXTERNALIZED FUNCTIONS   
   
    rrc_csp_init_acq_db()
        This function initializes the stored acquisition database. 

    rrc_rrc_csp_add_acq_entry()
        This function adds an acquisition entry to the acquisition 
        list. While doing so, it also updates the acquisition
        database pointers so that the topmost pointer points to this
        most recent acquisition entry.

    rrc_csp_get_first_match_acq_entry()
        This function gets the most recent acquisition entry from acq.
        database that matches the criteria given in the parameters. 

    rrc_csp_get_next_match_acq_entry()
        This function gets next acquisition entry from acq. database
        that matches the criteria given in the parameters. 


DEFINITIONS

    Acquisition Entry
        Data Structure that contains a mapping of PLMN and Frequency. In
        addition, it also contains the status of cell belonging to this 
        PLMN and found on this frequency. The status may be 
        barred/non-barred, forbidden / not forbidden reg. area.

    Stored Acquisition Database
        List of pointers that point to acquisition entries in the order 
        such that the most recent acquisition entry is pointed to by the 
        topmost pointer and the oldest acquisition entry is pointed to by
        the last pointer going in that order.

    Acquisition List
        An array for storing acquisition entries. The order of the entries
        is not important. All acquisition entries are accessed thru' pointers
        of the acquisition database and there is no direct access. 

  
MECHANISM

    The acquisition database is a two-layered structure as shown below:
   
          ACQ. DATABASE           ACQ. LIST
          -------------           ----------
        [Most recent ptr]------->[Acq. entry]
        [Next recent ptr]------->[Acq. entry]
        [Next recent ptr]------->[Acq. entry]
        [Next recent ptr]------->[Acq. entry]
        [Next recent ptr]------->[Acq. entry]
        [Next recent ptr]------->[Acq. entry]

    The pointers in Acquisition Database are ordered with the first one pointing
    to the most recent acquisition entry and so on. 

    The two-layered structure obviated the need to re-order acquisition list 
    every time a new acquisition entry is added to the list. A re-oder of 
    pointers in acquisition database is done but the acquisition list does not
    need to be ordered. This saves processing and is efficient.

    NOTE: Over time, the figure above will change - the arrows will no longer 
    remain horizontal. This is because acquisition list will not have any order
    but acquisition database pointers will always be ordered.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccspdb.c_v   1.13   22 Feb 2002 11:03:18   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccspdb.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/16   sas     Added api interface fun to provide acqdb cell list info to LTE.
06/23/15   sn      Changes to fill RAT information in Finger printing log packet 
06/16/16   sn      Changes to re-try ACQ on same frequency upon receiving DL 
                   weak indication with no lock
05/19/15   sp      Remove unwanted/frequent message highs
04/24/15   sas     Update ACQ-DB when SIB reading is skipped for VPLMN.
08/08/14   sn      Changes to allow scanning on frequency which is being retried
                   in next DRX
08/04/14   bc      Made changes to fix compiler warnings.
07/21/14   ac      NV reorganization fixes in rrc_csp_acq_list_convert_to_efs
07/15/14   ac      NV reorganization
05/14/14   vi      Created new NV to prioritize 850 band
05/06/14   geg     Use safer versions of memcpy() and memmove()
03/26/14   vg      made changes to scan all ACQ-DB entries as part of SERVICE_REQ of ACQ_DB_TYPE 
03/17/14   sr      Made changes to not trigger PLMN LIST REQ when no FP entries are available
03/13/14   vi      Stack reduction changes
01/07/14   sn      Reset invalid ACQ DB entries
01/02/14   rmn     Reduction of unwanted F3's
12/13/13   sn      Manual PLMN search optimizations
09/12/13   sn      Changes to restart service search correctly if WRM timer expires when CSP
                   is waiting for lock to try acquisition on detected PSC
09/02/13   vg      made changes to scan all ACQ-DB entries as part of SERVICE_REQ of ACQ_DB_TYPE 
08/21/13   sn      Changes to not skip non-white CSG ACD DB entry for auto PLMN searches
07/08/13   sn      Changes for LTE CSG feature
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
05/30/13   gv      Made changes to perform acq db only scan when requested so by higher layers
02/28/13   sn      New log packet for CSG finger printing information
01/31/13   gv      Made changes to stop scanning of the same band twice when the band
                   corresponding to the first matched ACQ DB entry and first band in
                   the band prioritization list are different
01/03/13   ad      Made changes to reduce the redundant F3s
12/07/12   sn      Changes to indicate absence of FP info in ASF srch resp
12/07/12   sn      CSG feature changes
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
08/13/12   gv      Added protection code to recover ACQ DB in case the same gets corrupted
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
02/24/12   sn      Changes to periodically save ACQ DB in NV
03/15/12   mn      Made changes to not abort LFS search if a LFS entry does not match any ACQ-DB entry.
09/09/11   gv      Added code to support NAS feature FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
07/08/11   gv      Fixed KW errors
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/14/10   as      Added changes for enabling Tuneaway using NV item
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/06/10   rm      Fixed compiler errors for UOOS
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/11/10   rm      Made changes to not use LFS for connected mode OOS
05/06/10   rm      Fixed compiler warnings
04/28/10   rm      Added code to prioritize Last Camped Frequency (LCF) from list scan results 
                   during connected mode OOS search
03/29/10   sks     Fixed lint errors.
03/29/10   as      Made changes to set acq_db_empty flag properly for LFS_COOS, so that 850/PCS/900
                   band optimization can be followed accordingly.
03/19/10   rm      Added code to scan 850 band first during full scan when NV
                   NV_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I is set.
                   Changes are under FEATURE_PRIORITIZE_850_BAND_FOR_FULL_SCAN
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/02/09   sks     During non-UOOS search (after UE found a signal during UOOS search), added
                   check to not skip scanning frequencies already scanned as part of LFS.
10/26/09   sks     Fixed klocwork errors.
09/16/09   sks     Replaced the code to update curr_band with forst bands to be scanned when
                   following LFS for COOS, if there are no ACQ-DB entries.
09/16/09   sks     Made changes to update the first_band_to_be_scanned with the first entry
                   in the LIST DB when FEATURE_WCDMA_BAND_PRIORITIZATION is not enabled.
09/16/09   sks     Added code to check for num_mnc_digits read from NV and limit
                   it to the MAX value of 3.
08/06/09   rm      Ported full frequency scan timer to FEATURE_LFS_COOS
07/16/09   rm      Ported FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE to COOS
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
02/02/09   rm      Ported List Frequency Scan (LFS) to conventional OOS
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/21/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/19/07   pk      Added support for scanning EPLMN(s) frequencies from ACQ DB for
                   FACH->FACH/PCH state transitions.   
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
06/25/07   vm      Added fix for CR 119540. Added support to correctly set/reset current_band 
                   so that the band indicator check by SIB proc is done against the correct
                   current_band value.
06/20/07   vm      Changed the order in which "if" check is done.
04/11/07   pk      Added fix for doing any plmn scan while freq scan timer is running
                   and acq db for spec plmn does not have matching entries for requested 
                   plmns in the automatic plmn search.
03/12/07   vm      added assignment to acq_db_plmn_type var in list_db structure
03/12/07   vm      Code review comment: when same freq is present multiple times in
                   the ACQ DB and the later entry belongs to a higher priority PLMN,
                   then after copying the complete acq_list structure to the
                   existing list_db entry, the list_db_plmn_type should be written
                   to list_db entry separately.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
09/20/06   vm      Added support for checking band information also in addition
                   to freq information to determine if an ACQ DB UARFCN shd be scanned or
                   not during full frequency scan. For details pls see CR 101351.
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
09/12/06   segu    Check for both the current requested PLMN Id and internal PLMN Id as 
                   part of the equivaent PLMN check
09/08/06   vm      For GtoW and WtoW BPLMN searches, RRC is incorrectly resetting the 
                   variable ACQ_DB_EMPTY to TRUE (inside the function 
                   rrc_csp_get_first_match_acq_entry()) on every ACQ DB access 
                   due to which even when 15mins No Full Scan timer is active, RRC is doing 
                   full frequency scan. This is because in addition to the full scan timer
                   flag, RRC looks at the acq_db_empty flag. If acq_db_empty flag is true
                   then even if full scan timer is active, RRC does full frequency scan.
08/28/06   vm      Added support for the feature FEATURE_DCH_TO_FACH_PCH_OUT_OF_SERVICE_ENHANCEMENT.
07/06/06   vm      Added support to not try acquisition on any acq db frequency
                   during full frequency scan irrespective of the type of request:
                   specific or any; because now ALL acq db entries are scanned before
                   full frequency scan and hence there is no need to re-try these
                   frequencies again during full frequency scan. 
06/08/06   vm      Added support for new feature FEATURE_MCC_AND_850_800_BAND_COMBINATION.
                   1. Now band class information for a particular ACQ DB entry is
                      also written and read from the NV.
                   2. For skipping frequencies present in ACQ DB during RAW scan, 
                      additional check has been added for the band_class also to 
                      which that frequency belongs to. 
03/30/06   vm      Added support to start the PLMN list full freq scan timer 
                   for 3G to 3G Automatic/Manual PLMN searches also (both 
                   background and foreground). As long as this timer is active 
                   no full scans are done. 
11/22/05   vm      Added support for searching RPLMN only during first ACQ DB scan
                   when the CSP scan type is cell selection scan; and scanning ALL
                   ACQ DB entries during the second ACQ DB scan when the next step 
                   is Full Frequency Scan.
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. 
10/11/05   vm      Added support for comparing BCCH and NAS PLMNs based on number
                   of MCC-MNC digits.
09/21/05   vm      Added support for not checking for EPLMN list if the network
                   select mode is MANUAL and current scan is Cell Selection.
08/31/05   vm      Added support for reading the downlink frequency from NV.
08/04/05   vm      Added support for scanning again the last camped frequency 
                   during every fine scan when UE goes OOS in Idle mode.
03/03/05   vm      Added support for inter-frequency redirection feature. When
                   inter frequency redirection is in progress then last camped 
                   frequency should not be scanned till the wait timer expires. 
                   Hence the function rrc_csp_has_acq_db_freq_been_scanned()
                   will now return true if the frequency passed to the function 
                   is the last camped frequency so that acquisition is not attempted
                   on this frequency.
02/15/05   vm      Added support for deep sleep. In particular, new band mask
                   rrc_csp_int_data.deep_sleep_acq_db_band_mask which maintains
                   a list of bands that are present in ACQ DB.
12/30/04   vm      Added code to avoid multiple scans on the same frequency during 
                   Acq DB scan.
12/23/04   vm      When 2G->3G BPLMN scan is in progress and ue is in deep sleep
                   then "any" plmn scan should not be done if "specific" plmn
                   scan is requested.
11/22/04   vm      Added definition of function rrccsp_bplmn_update_stored_acq_db_rd_index.
11/15/04   vm      When UE is in deep sleep and spec plmn scan is requested and matching
                   plmn is not found in acq db, then UE will do any plmn scan also.
11/10/04   vm      Added code to function rrc_csp_should_freq_be_scanned() so 
                   that the frequency is always scanned if UE is in connected mode.
10/13/04   vm      Included file "rrccsp.h" to fix lint errors.
10/08/04   bu      Removed rrc_csp_return_acq_list_band_class() as it's no longer
                   needed.
08/31/04   bu      Initialize the band_class read from NV appropriately based on
                   the frequency. Added definition for 
                   rrccsp_update_band_class_in_acq_db().
08/19/04   sgk     Added function rrc_csp_return_acq_list_band_class to return
                   the enum band class value to solve the lint error Type mismatch 
                   (assignment) (enum {...} = unsigned short) in line 
                   nv_acq_list1.acq_entry[i].band_class = acq_list[k].band_class;
                   in function rrc_csp_write_nv_acq_info
                   Type cast nv_acq_list1.acq_entry[i].band_class to uint16 to solve
                   lint warning Converting enum to int at line 
                   acq_list[k].band_class = nv_acq_list1.acq_entry[i].band_class;
                   in function rrc_csp_read_nv_acq_info
08/13/04   bu      Added WCDMA multiband support. Return an acquisition database entry
                   only if the frequency band is supported.
07/08/04   bu      If the PLMN in the acquisition database belongs to an equivalent
                   PLMN list, consider that frequency for acquisition if it has not
                   already been scanned.
06/30/04   bu      Removed updating of curr_rd_index if the acquisition entry was
                   already present in the ACQ DB.
04/27/04   bu      Increment the rd_index in rrc_csp_add_acq_entry(), if an already
                   existing acquisition entry is found and is being updated.
02/11/04   bu      Removed the status mask check in rrc_csp_get_match_acq_entry().
11/07/03   bu      Added a function rrc_csp_should_freq_be_scanned() to help confirm
                   if a particular frequency was scanned as part of the acquistion
                   database scan.
07/10/03   bu      While reading stored acquisition database from NV, check if the
                   curr_wr_index and curr_rd_index are valid. If not re-initialize
                   the stored acquisition database entries.
02/18/03   vn      Removed processing of RRC_CSP_ANY_PLMN_IDLE_BAR as that 
                   acquisition type is no longer needed.
01/28/03   bu      Made the required lint changes.
02/22/02   bu      Added NV_ACQ_LIST_4_I to further break down NV_ACQ_LIST_I
                   because of tools limitations.
02/20/02   bu      Added support to read/write from/to 3 NV_ACQ_LIST items.
                   Items are NV_ACQ_LIST_1_I, NV_ACQ_LIST_2_I and
                   NV_ACQ_LIST_3_I respectively. 
02/12/02   bu      Added new functions rrc_read_nv_acq_info() and
                   rrc_write_nv_acq_info(). These functions read/write
                   the acquisition database and acquisition list from/to the
                   NV respectively. 
12/21/01   vn      Process new acquisition database structure that stores
                   acquisition list indices in place of pointers. Needed for
                   a relocatable acquisition database.
10/31/01   vn      Changed from calling RRC_SAME_PLMN to rrc_bcch_plmns_match
                   at the time of updating the database and rrc_bcch_and_nas_
                   plmns_match when looking for a matching PLMN in database.
                   All PLMNs in the database are stored in BCCH PLMN format. 
05/16/01   vn      Changes to accomodate new rrc_plmn_id_type.
02/09/01   vn      Added/modified debug messages, Removed usage of
                   RRC_CSP_SPEC_PLMN_BIT as it is redundant.
02/08/01   vn      Moved compare_expected_cells to rrccsputil.c, removed
                   rrc_csp_get_type_acq as it is not possible to correctly
                   calculate Acq type from PLMN and Acq status mask.
01/24/01   vn      Added new messages.
01/19/01   vn      Added new functions rrc_csp_get_type_acq() and 
                   rrc_csp_compare_expected_cell().
01/06/01   vn      Took out memcpy, replaced with structure copy.
01/03/01   vn      Updates from code review.
12/21/00   vn      Shifted Macro RRCCSP_SAME_PLMNS to rrccspi.h. Usage of 
                   RRCCCSP_NOT_SAME_PLMNS.
12/08/00   vn      Added checking for last_entry_invalid flag to correctly
                   determine the number of valid entries.
12/05/00   vn      Added functionality to look for duplicate entries while 
                   adding new acq. entries. Added detailed comments.
11/17/00   vn      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "assert.h"
#include "uecomdef.h"

#include "rrccspdb.h"
#include "rrcnv.h"
#include "assert.h"
#include "rrccsp.h"
#include "rrccspi.h"
#include "rrcscmgr.h"


#include "mm_umts.h"

#include "sys_eplmn_list.h"

#ifdef FEATURE_FEMTO_CSG
#include "rrcccm.h"
#include "rrclogging.h"
#include "lte_irat_types.h"
#include "wcdma_rrc_api.h"
#endif

#include "fs_async_put.h"

#include "stringl.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

LOCAL nv_acq_db_type      nv_acq_db;

/* We need 4 NV ACQ lists as a NV item has a limitation of 128 bytes */
LOCAL nv_acq_list1_type    nv_acq_list1; /* Has 3 items */
LOCAL nv_acq_list1_type    nv_acq_list2; /* Has 3 items */
LOCAL nv_acq_list2_type    nv_acq_list3; /* Has 2 items */
LOCAL nv_acq_list2_type    nv_acq_list4; /* Has 2 items */
LOCAL nv_item_type         nv_wcdma_dl_freq; /* Only item */
LOCAL nv_item_type         nv_wcdma_dl_freq_enabled; /* Only item */

#ifdef FEATURE_FEMTO_CSG
LOCAL char wcdma_csg_efs_list_2[] = "/nv/item_files/conf/wcdma_csg_efs_2.conf";

LOCAL char wcdma_csg_efs_list_3[] = "/nv/item_files/conf/wcdma_csg_efs_3.conf";

LOCAL char wcdma_csg_efs_list_4[] = "/nv/item_files/conf/wcdma_csg_efs_4.conf";

LOCAL char wcdma_csg_efs_list_5[] = "/nv/item_files/conf/wcdma_csg_efs_5.conf";

/* Number of entries per EFS file */
#define MAX_ACQ_LIST_ENTRIES_EFS (MAX_ACQ_DB_SIZE/5)
#endif

LOCAL char wcdma_csg_efs_db[] = "/nv/item_files/conf/wcdma_csg_efs_db.conf";

LOCAL char wcdma_csg_efs_list_1[] = "/nv/item_files/conf/wcdma_csg_efs_1.conf";


/* Max entries for the NV_ACQ_LIST items. */
#define  MAX_NV_ACQ_DB_ENTRIES1  3
#define  MAX_NV_ACQ_DB_ENTRIES2  2

/* List of Acquisition entries: Note that the Acquisition entries are NOT
 * accessed directly. All access is through access routines that utilize
 * Stored Acquisition Database pointers.
 */
LOCAL  rrc_csp_acq_entry_type  acq_list[MAX_ACQ_DB_SIZE];

#ifdef FEATURE_FEMTO_CSG
/* List of Acquisition entries: To temporarily store CSG cells
   found during manual CSG search
 */
LOCAL  rrc_csp_acq_entry_type  acq_list_temp[MAX_ACQ_DB_ENTRIES_TEMP];
#endif
/* LIST DB in which ACQ DB entries are stored. This LIST DB is used to
 * to send freqs to l1 for doing list scan
 */
rrc_csp_acq_entry_type  list_db[MAX_ACQ_DB_ENTRIES];


#ifdef FEATURE_EOOS
#error code not present
#endif

/* Stored Acquisition Database: The pointers in the database point to entries
 * in the Acquisition List 
 */
LOCAL  rrc_csp_acq_db_type     stored_acq_db;

#ifdef FEATURE_FEMTO_CSG
/* Stored Acquisition Database: To temporarily store CSG cells
   found during manual CSG search
 */
LOCAL  rrc_csp_acq_db_type     stored_acq_db_temp;
#endif
/* Used for storing frequencies scanned during acquisition database scan
 */
LOCAL  rrc_csp_acq_db_freq_scanned_type acq_db_freq_scanned_list;

/* Last Requested PLMN: This stores the last PLMN requested thru' 
 * rrc_csp_get_acq_entry(). The responsibility is with the caller to remember
 * last sent PLMN and send the same again when calling 
 * rrc_csp_get_next_acq_entry(). 
 * This local var serves as a check to compare the passed value in 
 * rrc_csp_get_next_acq_entry() with the one stored by it. Both should match.
 */
LOCAL  rrc_plmn_identity_type   rrc_csp_last_req_plmn_id;

/* Last Requested Type of Acq: This stores the last acq. type requested thru'
 * rrc_csp_get_acq_entry(). The responsibility is with the caller to remember
 * last sent acq. type and send the same again when calling 
 * rrc_csp_get_next_acq_entry(). 
 * This local var serves as a check to compare the passed value in 
 * rrc_csp_get_next_acq_entry() with the one stored by it. Both should match.
 */
LOCAL  rrc_csp_acq_e_type       rrc_csp_last_req_type_acq;

boolean prioritize_850_band = WNV_GET_DEFAULT(WNV_ID_RRC_PRIORITIZE_850_BAND);
#ifdef FEATURE_DUAL_SIM
extern rrc_ds_tuneaway_status_change_ind_type ds_tuneaway_status_change_ind;
boolean rrc_temp_nv = FALSE;
#endif

#ifdef FEATURE_FEMTO_CSG
LOCAL rrc_csp_acq_entry_type   rrccsp_csg_temp_acq_db;
#endif

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION          RRC_CSP_INIT_ACQ_DB

DESCRIPTION       This function initializes the Stored acquisition database. 
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  At present, the acquisition database is initialized with
                  all pointers set to NULL. Once the NV task is 
                  added, the database can be stored in NV and read from it
                  on powerup.
===========================================================================*/
void  rrc_csp_init_acq_db(void)
{
  uint8  i;

  /* Initialize Current Read Index to -1 */
  stored_acq_db.curr_rd_index = -1;

  /* Initialize Current Number of Pointers to 0 */
  stored_acq_db.curr_wr_index = 0;

  stored_acq_db.last_entry_invalid = TRUE;

  /* Initialize all values in ordered acquisition list in acq database to 
   */

  for (i=0; i < MAX_ACQ_DB_SIZE; i++)
  {
    stored_acq_db.acq_list_indices[i] = i;
  }
  
#ifdef FEATURE_FEMTO_CSG
  /* Initialize Current Read Index to -1 */
  stored_acq_db_temp.curr_rd_index = -1;

  /* Initialize Current Number of Pointers to 0 */
  stored_acq_db_temp.curr_wr_index = 0;

  stored_acq_db_temp.last_entry_invalid = TRUE;

  /* Initialize all values in ordered acquisition list in acq database to 
   */
  for (i=0; i < MAX_ACQ_DB_ENTRIES_TEMP; i++)
  {
    stored_acq_db_temp.acq_list_indices[i] = i;
  }
#endif
}  /* rrc_csp_init_acq_db */

#ifdef FEATURE_FEMTO_CSG

/*===========================================================================

FUNCTION          RRC_CSP_FILL_ACQ_ENTRY

DESCRIPTION       Selectively fills ACQ DB entry

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
static void rrc_csp_fill_acq_entry(rrc_csp_acq_entry_type * acq_ptr_1, rrc_csp_acq_entry_type* acq_ptr_2)
{
  acq_ptr_1->plmn_id = acq_ptr_2->plmn_id;
  acq_ptr_1->freq = acq_ptr_2->freq;
  acq_ptr_1->status_mask = acq_ptr_2->status_mask;
  acq_ptr_1->cell_id = acq_ptr_2->cell_id;
  acq_ptr_1->scr_code = acq_ptr_2->scr_code;
  acq_ptr_1->lac = acq_ptr_2->lac;
#ifdef FEATURE_LTE_TO_WCDMA
  acq_ptr_1->rac_valid = acq_ptr_2->rac_valid;
  acq_ptr_1->lac_valid = acq_ptr_2->lac_valid;
  acq_ptr_1->rai = acq_ptr_2->rai;
#endif
#ifdef FEATURE_EOOS
  #error code not present
#endif
  acq_ptr_1->is_valid_nw_sharing = acq_ptr_2->is_valid_nw_sharing;
  acq_ptr_1->shared_list_of_plmns = acq_ptr_2->shared_list_of_plmns;
  acq_ptr_1->csg_id = acq_ptr_2->csg_id;
}

/*===========================================================================

FUNCTION          rrc_csp_efs_convert_to_acq_list

DESCRIPTION       This function fills the ACQ LIST with the data read from EFS
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
static void rrc_csp_efs_convert_to_acq_list(const rrc_csp_acq_entry_efs_type *efs_db,
                                          rrc_csp_acq_entry_type *list_db,
                                          uint8 index)
{
  uint8 i, j;

  for (j = 0,i = index; i < index+MAX_ACQ_LIST_ENTRIES_EFS; i++,j++)
  {
    WCDMA_MEMCPY(&list_db[i].plmn_id, 
                 sizeof(rrc_plmn_identity_type),
                 &efs_db[j].plmn_id, 
                 sizeof(rrc_plmn_identity_type));

    list_db[i].freq = efs_db[j].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    list_db[i].band_class = efs_db[j].band_class;
#else
    list_db[i].band_class = rrc_get_frequency_band(list_db[i].freq, list_db[i].plmn_id);
#endif
    list_db[i].status_mask = efs_db[j].status_mask;
    list_db[i].cell_id = efs_db[j].cell_id;
    list_db[i].scr_code = efs_db[j].scr_code;
    list_db[i].lac = efs_db[j].lac;
    list_db[i].csg_id = efs_db[j].csg_id;

    list_db[i].fp_w_macro_cnt = efs_db[j].fp_w_macro_cnt;
    list_db[i].w_macro_write_idx = efs_db[j].w_macro_write_idx;
    WCDMA_MEMCPY(&list_db[i].w_macro,
                 sizeof(list_db[i].w_macro),
                 &efs_db[j].w_macro,
                 sizeof(efs_db[j].w_macro));
 
    list_db[i].fp_lte_macro_cnt = efs_db[j].fp_lte_macro_cnt;
    list_db[i].lte_macro_write_idx = efs_db[j].lte_macro_write_idx;
    WCDMA_MEMCPY(&list_db[i].lte_macro,
                 sizeof(list_db[i].lte_macro),
                 &efs_db[j].lte_macro,
                 sizeof(efs_db[j].lte_macro));
     MSG_9(MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "CSG: Entry %d, FPW: %d, FPL: %d, CSG ID %d, cell ID %d, band class %d, freq %d",
         i,efs_db[j].fp_w_macro_cnt,efs_db[j].fp_lte_macro_cnt,efs_db[j].csg_id,efs_db[j].cell_id,
         efs_db[j].band_class,efs_db[j].freq,0,0);
  
  }

}

/*===========================================================================

FUNCTION          rrc_csp_acq_list_convert_to_efs

DESCRIPTION       This function filters the data from ACQ LIST that needs to be
                  written to EFS
           
DEPENDENCIES
                  None

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

NOTE
                  None.
===========================================================================*/
static void rrc_csp_acq_list_convert_to_efs(rrc_csp_acq_entry_efs_type *efs_db,
                                         const rrc_csp_acq_entry_type *list_db,
                                         uint8 index)
{
uint8 i, j;

  for (j = 0,i = index; i < index+MAX_ACQ_LIST_ENTRIES_EFS; i++,j++)
  {
    WCDMA_MEMCPY(&efs_db[j].plmn_id,
                 sizeof(rrc_plmn_identity_type),
                 &list_db[i].plmn_id,
                 sizeof(rrc_plmn_identity_type));

    efs_db[j].freq = list_db[i].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    efs_db[j].band_class = list_db[i].band_class;
#endif
    efs_db[j].status_mask = list_db[i].status_mask;
    efs_db[j].cell_id = list_db[i].cell_id;
    efs_db[j].scr_code = list_db[i].scr_code;
    efs_db[j].lac = list_db[i].lac;
    efs_db[j].csg_id = list_db[i].csg_id;

    efs_db[j].fp_w_macro_cnt = list_db[i].fp_w_macro_cnt;
    efs_db[j].w_macro_write_idx = list_db[i].w_macro_write_idx;
    WCDMA_MEMCPY(efs_db[j].w_macro,
                 sizeof(efs_db[j].w_macro),
                 list_db[i].w_macro,
                 sizeof(list_db[i].w_macro));
 
    efs_db[j].fp_lte_macro_cnt = list_db[i].fp_lte_macro_cnt;
    efs_db[j].lte_macro_write_idx = list_db[i].lte_macro_write_idx;
    WCDMA_MEMCPY(efs_db[j].lte_macro,
                 sizeof(efs_db[j].lte_macro),
                 list_db[i].lte_macro,
                 sizeof(list_db[i].lte_macro));
   MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "CSG: Entry %d, FPW: %d, FPL: %d, CSG ID %d, cell ID %d, band class %d, freq %d",
         i,efs_db[j].fp_w_macro_cnt,efs_db[j].fp_lte_macro_cnt,efs_db[j].csg_id,efs_db[j].cell_id,
         efs_db[j].band_class,efs_db[j].freq,0,0);
  
  }
}
#endif
/*===========================================================================

FUNCTION          RRC_CSP_RECOVER_ACQ_DB

DESCRIPTION       This function recovers the acquisition database pointers 
                  if any of its entries are corrupted.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrc_csp_recover_acq_db(void)
{
  uint8 i;
  uint8 k = 0;
  uint8 num_valid_idxs = 0;
  uint8 num_invalid_idxs = 0;
  uint8 acqdb_size = MAX_ACQ_DB_SIZE;
  uint8 valid_list_indices[MAX_ACQ_DB_SIZE] = {0};
  uint8 missed_list_indices[MAX_ACQ_DB_SIZE] = {0};

/* First we will find out the number of valid indices available */
  for( i=0; i< acqdb_size; i++ )
  {
    if(stored_acq_db.acq_list_indices[i] > MAX_ACQ_DB_INDEX)
    {
      WRRC_MSG1_ERROR("Stored Acq Db has a corrupted ACQ List index %d",stored_acq_db.acq_list_indices[i]);
    }
    else
    {
      valid_list_indices[num_valid_idxs] = stored_acq_db.acq_list_indices[i];
      num_valid_idxs++;
    }

    /*check all the valid indices*/
    if((i < (stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid))&&
       (RRC_CSP_BAND_MAX == rrc_get_frequency_band(acq_list[i].freq,acq_list[i].plmn_id))) 
    {
      MSG_ERROR("Detected invalid frequency in ACQ DB index %d, freq %d",i,acq_list[i].freq,0);
      memset(&acq_list[i],0,sizeof(rrc_csp_acq_entry_type));
    }
  }

/* If number of valid indices is same as MAX_ACQ_DB_ENTRIES it means no corruption has happened */
  if(acqdb_size == num_valid_idxs)
  {
    return;
  }

/* Here we will find the missed list indices which are not present in the array of valid list indices */
  for(i=0; i<acqdb_size; i++)
  {
    for(k=0; k < num_valid_idxs; k++)
    {
      if(i == valid_list_indices[k])
      break;
    }
    if(k >= num_valid_idxs)
    {
      missed_list_indices[num_invalid_idxs++] = i;
    }
  }

  num_invalid_idxs = 0;

/* Replace the corrupt values with the missed entries */
  for( i=0; i< acqdb_size; i++ )
  {
    if(stored_acq_db.acq_list_indices[i] > MAX_ACQ_DB_INDEX)
    {
      stored_acq_db.acq_list_indices[i]= missed_list_indices[num_invalid_idxs++];
      WRRC_MSG2_ERROR("Replacing the corrupted location %d of acq_list_indices with value %d",i,stored_acq_db.acq_list_indices[i]);
    }
  }
}  /* rrc_csp_recover_acq_db */
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
void  rrc_csp_read_nv_acq_info(void)
{
  int  i;
  int  k=0;

#ifdef FEATURE_FEMTO_CSG
  rrc_csp_acq_entry_efs_type *efs_db = rrc_malloc(MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type));
#else
  int  j;
#endif
  if ( rrc_prioritize_850_band_nv)
  {
    prioritize_850_band= TRUE;
    WRRC_MSG1_HIGH("Prioritize 850 band %d ",prioritize_850_band);
  }
#ifndef FEATURE_FEMTO_CSG
  WRRC_MSG0_HIGH("Reading ACQ DB header from EFS");
  if(!rrc_get_from_efs(wcdma_csg_efs_db,&stored_acq_db, sizeof(rrc_csp_acq_db_type)))
  {
    WRRC_MSG0_ERROR("CSG: Error reading ACQ DB header initialize now");
    rrc_csp_init_acq_db();
  }
    
  /* Do a Sanity check - check read and write index. */
  if ((stored_acq_db.curr_wr_index > MAX_ACQ_DB_INDEX) || 
      (stored_acq_db.curr_rd_index > MAX_ACQ_DB_INDEX) ||
      (stored_acq_db.curr_rd_index < -1) ||
      (stored_acq_db.curr_rd_index > stored_acq_db.curr_wr_index)||
      (stored_acq_db.curr_wr_index < 0))
  {
    WRRC_MSG2_ERROR("Acq db invalid, Wr index %d Rd index %d",
                stored_acq_db.curr_wr_index, stored_acq_db.curr_rd_index);

    /* Re-initialize the Acquisition database */
    rrc_csp_init_acq_db();
    return;
  }


  if( NV_DONE_S ==  (rrc_get_nv( NV_ACQ_LIST_1_I, (nv_item_type *) &nv_acq_list1 )))
  {
    /* Read the acq entries from the NV_ACQ_LIST_1 */
    for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
    {
      acq_list[k].plmn_id.mcc[0] = nv_acq_list1.acq_entry[i].plmn_id.mcc[0];
      acq_list[k].plmn_id.mcc[1] = nv_acq_list1.acq_entry[i].plmn_id.mcc[1];
      acq_list[k].plmn_id.mcc[2] = nv_acq_list1.acq_entry[i].plmn_id.mcc[2];
      acq_list[k].plmn_id.num_mnc_digits = nv_acq_list1.acq_entry[i].plmn_id.num_mnc_digits;
      if(acq_list[k].plmn_id.num_mnc_digits > RRC_MAX_MNC_DIGITS)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_1_I has invalid num_mnc_digits");
        acq_list[k].plmn_id.num_mnc_digits = RRC_MAX_MNC_DIGITS;
      }
      for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
      {
        acq_list[k].plmn_id.mnc[j] = nv_acq_list1.acq_entry[i].plmn_id.mnc[j];
      }

      acq_list[k].freq = (uint16)nv_acq_list1.acq_entry[i].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
      acq_list[k].band_class = (uint16)nv_acq_list1.acq_entry[i].band_class;
#else
      /* Get the band_class based on the frequency */
      acq_list[k].band_class = rrc_get_frequency_band(acq_list[k].freq, acq_list[k].plmn_id);
#endif
      acq_list[k].status_mask = (uint16)nv_acq_list1.acq_entry[i].status_mask;
      acq_list[k].cell_id = nv_acq_list1.acq_entry[i].cell_id;
      acq_list[k].scr_code = (uint16)nv_acq_list1.acq_entry[i].scr_code;
      acq_list[k++].lac = (uint8)nv_acq_list1.acq_entry[i].lac;
    }
  }
  else
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_1_I not read");
  }


  if( NV_DONE_S ==  (rrc_get_nv( NV_ACQ_LIST_2_I, (nv_item_type *) &nv_acq_list2 )))
  {
    for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
    {
      acq_list[k].plmn_id.mcc[0] = nv_acq_list2.acq_entry[i].plmn_id.mcc[0];
      acq_list[k].plmn_id.mcc[1] = nv_acq_list2.acq_entry[i].plmn_id.mcc[1];
      acq_list[k].plmn_id.mcc[2] = nv_acq_list2.acq_entry[i].plmn_id.mcc[2];
      acq_list[k].plmn_id.num_mnc_digits = nv_acq_list2.acq_entry[i].plmn_id.num_mnc_digits;
      if(acq_list[k].plmn_id.num_mnc_digits > RRC_MAX_MNC_DIGITS)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_2_I has invalid num_mnc_digits");
        acq_list[k].plmn_id.num_mnc_digits = RRC_MAX_MNC_DIGITS;
      }
      for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
      {
        acq_list[k].plmn_id.mnc[j] = nv_acq_list2.acq_entry[i].plmn_id.mnc[j];
      }

      acq_list[k].freq = (uint16)nv_acq_list2.acq_entry[i].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
      acq_list[k].band_class = (uint16)nv_acq_list2.acq_entry[i].band_class;
#else
      /* Get the band_class based on the frequency */
      acq_list[k].band_class = rrc_get_frequency_band(acq_list[k].freq, acq_list[k].plmn_id);
#endif
      acq_list[k].status_mask = (uint16)nv_acq_list2.acq_entry[i].status_mask;
      acq_list[k].cell_id = nv_acq_list2.acq_entry[i].cell_id;
      acq_list[k].scr_code = (uint16)nv_acq_list2.acq_entry[i].scr_code;
      acq_list[k++].lac = (uint8)nv_acq_list2.acq_entry[i].lac;
    }
  }
  else
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_2_I not read");
  }

  if( NV_DONE_S ==  (rrc_get_nv( NV_ACQ_LIST_3_I, (nv_item_type *) &nv_acq_list3 )))
  {
    for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
    {
      acq_list[k].plmn_id.mcc[0] = nv_acq_list3.acq_entry[i].plmn_id.mcc[0];
      acq_list[k].plmn_id.mcc[1] = nv_acq_list3.acq_entry[i].plmn_id.mcc[1];
      acq_list[k].plmn_id.mcc[2] = nv_acq_list3.acq_entry[i].plmn_id.mcc[2];
      acq_list[k].plmn_id.num_mnc_digits = nv_acq_list3.acq_entry[i].plmn_id.num_mnc_digits;
      if(acq_list[k].plmn_id.num_mnc_digits > RRC_MAX_MNC_DIGITS)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_3_I has invalid num_mnc_digits");
        acq_list[k].plmn_id.num_mnc_digits = RRC_MAX_MNC_DIGITS;
      }
      for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
      {
        acq_list[k].plmn_id.mnc[j] = nv_acq_list3.acq_entry[i].plmn_id.mnc[j];
      }

      acq_list[k].freq = (uint16)nv_acq_list3.acq_entry[i].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
      acq_list[k].band_class = (uint16)nv_acq_list3.acq_entry[i].band_class;
#else
      /* Get the band_class based on the frequency */
      acq_list[k].band_class = rrc_get_frequency_band(acq_list[k].freq, acq_list[k].plmn_id);
#endif
      acq_list[k].status_mask = (uint16)nv_acq_list3.acq_entry[i].status_mask;
      acq_list[k].cell_id = nv_acq_list3.acq_entry[i].cell_id;
      acq_list[k].scr_code = (uint16)nv_acq_list3.acq_entry[i].scr_code;
      acq_list[k++].lac = (uint8)nv_acq_list3.acq_entry[i].lac;
    }
  }
  else
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_3_I not read");
  }

  if( NV_DONE_S ==  (rrc_get_nv( NV_ACQ_LIST_4_I, (nv_item_type *) &nv_acq_list4 )))
  {
    for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
    {
      acq_list[k].plmn_id.mcc[0] = nv_acq_list4.acq_entry[i].plmn_id.mcc[0];
      acq_list[k].plmn_id.mcc[1] = nv_acq_list4.acq_entry[i].plmn_id.mcc[1];
      acq_list[k].plmn_id.mcc[2] = nv_acq_list4.acq_entry[i].plmn_id.mcc[2];
      acq_list[k].plmn_id.num_mnc_digits = nv_acq_list4.acq_entry[i].plmn_id.num_mnc_digits;
      if(acq_list[k].plmn_id.num_mnc_digits > RRC_MAX_MNC_DIGITS)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_4_I has invalid num_mnc_digits");
        acq_list[k].plmn_id.num_mnc_digits = RRC_MAX_MNC_DIGITS;
      }
      for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
      {
        acq_list[k].plmn_id.mnc[j] = nv_acq_list4.acq_entry[i].plmn_id.mnc[j];
      }

      acq_list[k].freq = (uint16)nv_acq_list4.acq_entry[i].freq;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
      acq_list[k].band_class = (uint16)nv_acq_list4.acq_entry[i].band_class;
#else
      /* Get the band_class based on the frequency */
      acq_list[k].band_class = rrc_get_frequency_band(acq_list[k].freq, acq_list[k].plmn_id);
#endif
      acq_list[k].status_mask = (uint16)nv_acq_list4.acq_entry[i].status_mask;
      acq_list[k].cell_id = nv_acq_list4.acq_entry[i].cell_id;
      acq_list[k].scr_code = (uint16)nv_acq_list4.acq_entry[i].scr_code;
      acq_list[k++].lac = (uint8)nv_acq_list4.acq_entry[i].lac;
    }
  }
  else
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_4_I not read");
  }

   /*read remaning acq db entries*/
  (void)rrc_get_from_efs(wcdma_csg_efs_list_1, &acq_list[10], 5*sizeof(rrc_csp_acq_entry_type));

#else
  memset(efs_db,0,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type));
  WRRC_MSG0_HIGH("CSG: Reading ACQ DB from EFS");
  if(!rrc_get_from_efs(wcdma_csg_efs_db,&stored_acq_db, sizeof(rrc_csp_acq_db_type)))
  {
    WRRC_MSG0_ERROR("CSG: Error reading ACQ DB header initialize now");
    rrc_csp_init_acq_db();
  }

  /* Do a Sanity check - check read and write index. */
  if ((stored_acq_db.curr_wr_index > MAX_ACQ_DB_INDEX) || 
      (stored_acq_db.curr_rd_index > MAX_ACQ_DB_INDEX) ||
      (stored_acq_db.curr_rd_index < -1) ||
      (stored_acq_db.curr_rd_index > stored_acq_db.curr_wr_index)||
      (stored_acq_db.curr_wr_index < 0))
  {
    WRRC_MSG2_ERROR("Acq db invalid, Wr index %d Rd index %d",
                stored_acq_db.curr_wr_index, stored_acq_db.curr_rd_index);

    /* Re-initialize the Acquisition database */
    rrc_csp_init_acq_db();
    rrc_free(efs_db);
    return;
  }

  if(rrc_get_from_efs(wcdma_csg_efs_list_1, efs_db, MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
  {
    rrc_csp_efs_convert_to_acq_list(efs_db,acq_list,0);
  }
  else
  {
    WRRC_MSG0_ERROR("CSG: Reading failed_1");
  }

  if(rrc_get_from_efs(wcdma_csg_efs_list_2, efs_db, MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
  {
    rrc_csp_efs_convert_to_acq_list(efs_db,acq_list,MAX_ACQ_LIST_ENTRIES_EFS);
  }
  else
  {
    WRRC_MSG0_ERROR("CSG: Reading failed_2");
  }

  if(rrc_get_from_efs(wcdma_csg_efs_list_3, efs_db, MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
  {
    rrc_csp_efs_convert_to_acq_list(efs_db,acq_list,2*MAX_ACQ_LIST_ENTRIES_EFS);
  }
  else
  {
    WRRC_MSG0_ERROR("CSG: Reading failed_3");
  }

  if(rrc_get_from_efs(wcdma_csg_efs_list_4, efs_db, MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
  {
    rrc_csp_efs_convert_to_acq_list(efs_db,acq_list,3*MAX_ACQ_LIST_ENTRIES_EFS);
  }
  else
  {
    WRRC_MSG0_ERROR("CSG: Reading failed_4");
  }

  if(rrc_get_from_efs(wcdma_csg_efs_list_5, efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
  {
    rrc_csp_efs_convert_to_acq_list(efs_db,acq_list,4*MAX_ACQ_LIST_ENTRIES_EFS);
  }
  else
  {
    WRRC_MSG0_ERROR("CSG: Reading failed_5");
  }
#endif

  rrc_csp_recover_acq_db();
#ifdef FEATURE_EOOS
  #error code not present
#endif

  WRRC_MSG0_HIGH("Setting is_valid_nw_sharing to FALSE");
  for(i = 0; i<k; i++)
  {
    acq_list[i].is_valid_nw_sharing = FALSE;
  }

  WRRC_MSG0_HIGH("Reading W freq (if present) for single freq scan");
  if( WNV_SUCCESS ==  (wnv_get_nv( WNV_ID_WCDMA_DL_FREQ_ENABLED_I, &nv_wcdma_dl_freq_enabled, sizeof(nv_wcdma_dl_freq_enabled))))
  {
    rrc_csp_int_data.wcdma_dl_freq_enabled = nv_wcdma_dl_freq_enabled.wcdma_dl_freq_enabled;
    if(rrc_csp_int_data.wcdma_dl_freq_enabled)
    {
      if( WNV_SUCCESS == (wnv_get_nv( WNV_ID_WCDMA_DL_FREQ_I, &nv_wcdma_dl_freq, sizeof(nv_wcdma_dl_freq))))
      {
        rrc_csp_int_data.wcdma_dl_freq = (uint16)nv_wcdma_dl_freq.wcdma_dl_freq;
        WRRC_MSG1_HIGH("NV_WCDMA_DL_FREQ_I read. Freq = %d", rrc_csp_int_data.wcdma_dl_freq);
      }
      else
      {
        WRRC_MSG0_ERROR("NV_WCDMA_DL_FREQ_I not read");
        rrc_csp_int_data.wcdma_dl_freq_enabled = FALSE;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("wcdma_dl_freq_enabled set to FALSE");
    }
  }
  else
  {
    WRRC_MSG0_ERROR("NV_WCDMA_DL_FREQ_ENABLED_I not read");
    rrc_csp_int_data.wcdma_dl_freq_enabled = FALSE;
  } 

#ifdef FEATURE_DUAL_SIM
  if ( WNV_SUCCESS ==  (wnv_get_nv( WNV_ID_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I, 
                                    &rrc_temp_nv,
                                    sizeof(rrc_temp_nv))))
  {
    WRRC_MSG1_HIGH("RRC temporary NV : %d ",rrc_temp_nv);
  }
  else
  {
    rrc_temp_nv = FALSE; /* Default disabled*/
  }
#endif
#ifdef FEATURE_FEMTO_CSG
rrc_free(efs_db);
#endif

}  /* rrc_csp_read_nv_acq_info */


/*===========================================================================

FUNCTION          RRC_CSP_WRITE_NV_ACQ_INFO

DESCRIPTION       This function saves the stored acquisition database and the 
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
void  rrc_csp_write_nv_acq_info(void)
{

#ifdef FEATURE_FEMTO_CSG
  rrc_csp_acq_entry_efs_type *efs_db = rrc_malloc(MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type));
#else
  int  i;
  int  j;
  int  k=0;
#endif
  /* Do a Sanity check - check read and write index. */
  if ((stored_acq_db.curr_wr_index > MAX_ACQ_DB_INDEX) || 
      (stored_acq_db.curr_rd_index > MAX_ACQ_DB_INDEX) ||
      (stored_acq_db.curr_rd_index < -1) ||
      (stored_acq_db.curr_rd_index > stored_acq_db.curr_wr_index) ||
      (stored_acq_db.curr_wr_index < 0))
  {
    WRRC_MSG2_ERROR("Acq db invalid, Wr index %d Rd index %d",
                stored_acq_db.curr_wr_index, stored_acq_db.curr_rd_index);

    /* Re-initialize the Acquisition database */
    rrc_csp_init_acq_db();
  }

#ifdef FEATURE_FEMTO_CSG

WRRC_MSG0_HIGH("CSG: ACQ DB is being written to EFS");
if(!rrc_put_efs(wcdma_csg_efs_db,&stored_acq_db,sizeof(rrc_csp_acq_db_type)))
{
  WRRC_MSG0_ERROR("CSG: ACQ DB header write failed");
}

rrc_csp_acq_list_convert_to_efs(efs_db,acq_list, 0);
if(!rrc_put_efs(wcdma_csg_efs_list_1,efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
{
  WRRC_MSG0_ERROR("CSG:1 Failed");
}

rrc_csp_acq_list_convert_to_efs(efs_db,acq_list, MAX_ACQ_LIST_ENTRIES_EFS);
if(!rrc_put_efs(wcdma_csg_efs_list_2,efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
{
  WRRC_MSG0_ERROR("CSG: 2 write failed");
}

rrc_csp_acq_list_convert_to_efs(efs_db,acq_list, 2*MAX_ACQ_LIST_ENTRIES_EFS);
if(!rrc_put_efs(wcdma_csg_efs_list_3,efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
{
  WRRC_MSG0_ERROR("CSG: 3 write failed");
}

rrc_csp_acq_list_convert_to_efs(efs_db,acq_list, 3*MAX_ACQ_LIST_ENTRIES_EFS);
if(!rrc_put_efs(wcdma_csg_efs_list_4,efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
{
  WRRC_MSG0_ERROR("CSG:4 write failed");
}

rrc_csp_acq_list_convert_to_efs(efs_db,acq_list, 4*MAX_ACQ_LIST_ENTRIES_EFS);
if(!rrc_put_efs(wcdma_csg_efs_list_5,efs_db,MAX_ACQ_LIST_ENTRIES_EFS*sizeof(rrc_csp_acq_entry_efs_type)))
{
  WRRC_MSG0_ERROR("CSG: 5 write failed");
}
#else
  WRRC_MSG0_HIGH("Writing ACQ DB to NV");
  /* Write NV_ACQ_LIST_1_I to NV */
  for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
  {
    nv_acq_list1.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
    nv_acq_list1.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
    nv_acq_list1.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
    nv_acq_list1.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
    for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
    {
      nv_acq_list1.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
    }

    nv_acq_list1.acq_entry[i].freq = acq_list[k].freq;
    nv_acq_list1.acq_entry[i].status_mask = acq_list[k].status_mask;
    nv_acq_list1.acq_entry[i].cell_id = acq_list[k].cell_id;
    nv_acq_list1.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    nv_acq_list1.acq_entry[i].band_class = acq_list[k].band_class;
#endif
    nv_acq_list1.acq_entry[i].lac = acq_list[k++].lac;
  }

  if( NV_DONE_S !=  (rrc_put_nv( NV_ACQ_LIST_1_I, (nv_item_type *) &nv_acq_list1 )))
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_1_I not written");
  }

  /* Write NV_ACQ_LIST_2_I to NV */
  for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
  {
    nv_acq_list2.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
    nv_acq_list2.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
    nv_acq_list2.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
    nv_acq_list2.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
    for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
    {
      nv_acq_list2.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
    }

    nv_acq_list2.acq_entry[i].freq = acq_list[k].freq;
    nv_acq_list2.acq_entry[i].status_mask = acq_list[k].status_mask;
    nv_acq_list2.acq_entry[i].cell_id = acq_list[k].cell_id;
    nv_acq_list2.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    nv_acq_list2.acq_entry[i].band_class = acq_list[k].band_class;
#endif
    nv_acq_list2.acq_entry[i].lac = acq_list[k++].lac;
  }

  if( NV_DONE_S !=  (rrc_put_nv( NV_ACQ_LIST_2_I, (nv_item_type *) &nv_acq_list2 )))
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_2_I not written");
  }

  /* Write NV_ACQ_LIST_3_I to NV */
  for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
  {
    nv_acq_list3.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
    nv_acq_list3.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
    nv_acq_list3.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
    nv_acq_list3.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
    for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
    {
      nv_acq_list3.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
    }

    nv_acq_list3.acq_entry[i].freq = acq_list[k].freq;
    nv_acq_list3.acq_entry[i].status_mask = acq_list[k].status_mask;
    nv_acq_list3.acq_entry[i].cell_id = acq_list[k].cell_id;
    nv_acq_list3.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    nv_acq_list3.acq_entry[i].band_class = acq_list[k].band_class;
#endif
    nv_acq_list3.acq_entry[i].lac = acq_list[k++].lac;
  }

  if( NV_DONE_S !=  (rrc_put_nv( NV_ACQ_LIST_3_I, (nv_item_type *) &nv_acq_list3 )))
  {
   WRRC_MSG0_ERROR("NV_ACQ_LIST_3_I not written");
  }

  /* Write NV_ACQ_LIST_4_I to NV */
  for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
  {
    nv_acq_list4.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
    nv_acq_list4.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
    nv_acq_list4.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
    nv_acq_list4.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
    for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
    {
      nv_acq_list4.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
    }

    nv_acq_list4.acq_entry[i].freq = acq_list[k].freq;
    nv_acq_list4.acq_entry[i].status_mask = acq_list[k].status_mask;
    nv_acq_list4.acq_entry[i].cell_id = acq_list[k].cell_id;
    nv_acq_list4.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    nv_acq_list4.acq_entry[i].band_class = acq_list[k].band_class;
#endif
    nv_acq_list4.acq_entry[i].lac = acq_list[k++].lac;
  }


  if( NV_DONE_S !=  (rrc_put_nv( NV_ACQ_LIST_4_I, (nv_item_type *) &nv_acq_list4 )))
  {
    WRRC_MSG0_ERROR("NV_ACQ_LIST_4_I not written");
  }
  

  /*Write remaning entries into EFS file*/

  if(!rrc_put_efs(wcdma_csg_efs_list_1,&acq_list[10],5*sizeof(rrc_csp_acq_entry_type)))
  {
    WRRC_MSG0_ERROR("CSG:1 Failed");
  }
  WRRC_MSG0_HIGH("CSG: ACQ DB is being written to EFS");
  if(!rrc_put_efs(wcdma_csg_efs_db,&stored_acq_db,sizeof(rrc_csp_acq_db_type)))
  {
    WRRC_MSG0_ERROR("CSG: ACQ DB header write failed");
  }
#endif
#ifdef FEATURE_FEMTO_CSG
rrc_free(efs_db);
#endif
}  /* rrc_csp_write_nv_acq_info */

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
void  rrc_csp_write_nv_acq_list_info(int index)
{
  int  i;
  int  j;
  int  k=0;


  switch(index)
  {
    case 0:
    case 1:
    case 2:
      /* Write NV_ACQ_LIST_1_I to NV */
      for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
      {
        nv_acq_list1.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
        nv_acq_list1.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
        nv_acq_list1.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
        nv_acq_list1.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
        for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
        {
          nv_acq_list1.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
        }

        nv_acq_list1.acq_entry[i].freq = acq_list[k].freq;
        nv_acq_list1.acq_entry[i].status_mask = acq_list[k].status_mask;
        nv_acq_list1.acq_entry[i].cell_id = acq_list[k].cell_id;
        nv_acq_list1.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
        nv_acq_list1.acq_entry[i].band_class = acq_list[k].band_class;
#endif
        nv_acq_list1.acq_entry[i].lac = acq_list[k++].lac;
      }

      if((rrc_queue_nv_write( NV_ACQ_LIST_1_I, (nv_item_type *) &nv_acq_list1, sizeof(nv_acq_list1_type))) == -1)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_1_I not written");
      }
      break;

    case 3:
    case 4:
    case 5:
      k = MAX_NV_ACQ_DB_ENTRIES1;
      /* Write NV_ACQ_LIST_2_I to NV */
      for( i=0; i< MAX_NV_ACQ_DB_ENTRIES1; i++ )
      {
        nv_acq_list2.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
        nv_acq_list2.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
        nv_acq_list2.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
        nv_acq_list2.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
        for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
        {
          nv_acq_list2.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
        }

        nv_acq_list2.acq_entry[i].freq = acq_list[k].freq;
        nv_acq_list2.acq_entry[i].status_mask = acq_list[k].status_mask;
        nv_acq_list2.acq_entry[i].cell_id = acq_list[k].cell_id;
        nv_acq_list2.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
        nv_acq_list2.acq_entry[i].band_class = acq_list[k].band_class;
#endif
        nv_acq_list2.acq_entry[i].lac = acq_list[k++].lac;
      }

      if((rrc_queue_nv_write( NV_ACQ_LIST_2_I, (nv_item_type *) &nv_acq_list2, sizeof(nv_acq_list1_type))) == -1)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_2_I not written");
      }
      break;

    case 6:
    case 7:
      k = MAX_NV_ACQ_DB_ENTRIES1*2;
      /* Write NV_ACQ_LIST_3_I to NV */
      for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
      {
        nv_acq_list3.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
        nv_acq_list3.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
        nv_acq_list3.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
        nv_acq_list3.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
        for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
        {
          nv_acq_list3.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
        }

        nv_acq_list3.acq_entry[i].freq = acq_list[k].freq;
        nv_acq_list3.acq_entry[i].status_mask = acq_list[k].status_mask;
        nv_acq_list3.acq_entry[i].cell_id = acq_list[k].cell_id;
        nv_acq_list3.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
        nv_acq_list3.acq_entry[i].band_class = acq_list[k].band_class;
#endif
        nv_acq_list3.acq_entry[i].lac = acq_list[k++].lac;
      }

      if((rrc_queue_nv_write( NV_ACQ_LIST_3_I, (nv_item_type *) &nv_acq_list3, sizeof(nv_acq_list2_type))) == -1)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_3_I not written");
      }
      break;

    case 8:
    case 9:	
      k = (MAX_NV_ACQ_DB_ENTRIES1*2)+ MAX_NV_ACQ_DB_ENTRIES2;
      /* Write NV_ACQ_LIST_4_I to NV */
      for( i=0; i< MAX_NV_ACQ_DB_ENTRIES2; i++ )
      {
        nv_acq_list4.acq_entry[i].plmn_id.mcc[0] = acq_list[k].plmn_id.mcc[0];
        nv_acq_list4.acq_entry[i].plmn_id.mcc[1] = acq_list[k].plmn_id.mcc[1];
        nv_acq_list4.acq_entry[i].plmn_id.mcc[2] = acq_list[k].plmn_id.mcc[2];
        nv_acq_list4.acq_entry[i].plmn_id.num_mnc_digits = acq_list[k].plmn_id.num_mnc_digits;
        for(j=0; j<acq_list[k].plmn_id.num_mnc_digits; j++)
        {
          nv_acq_list4.acq_entry[i].plmn_id.mnc[j] = acq_list[k].plmn_id.mnc[j];
        }

        nv_acq_list4.acq_entry[i].freq = acq_list[k].freq;
        nv_acq_list4.acq_entry[i].status_mask = acq_list[k].status_mask;
        nv_acq_list4.acq_entry[i].cell_id = acq_list[k].cell_id;
        nv_acq_list4.acq_entry[i].scr_code = acq_list[k].scr_code;
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
        nv_acq_list4.acq_entry[i].band_class = acq_list[k].band_class;
#endif
        nv_acq_list4.acq_entry[i].lac = acq_list[k++].lac;
      }

      if((rrc_queue_nv_write( NV_ACQ_LIST_4_I, (nv_item_type *) &nv_acq_list4, sizeof(nv_acq_list2_type) )) == -1)
      {
        WRRC_MSG0_ERROR("NV_ACQ_LIST_4_I not written");
      }
      break;
  
    default:
      WRRC_MSG1_HIGH("Support will be added for this index later %d",index);
      break;
  }
  
}  /* rrc_csp_write_nv_acq_list_info */

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
void  rrc_csp_write_nv_acq_db_info(void)
{
  uint32  status;

  status = efs_async_put(wcdma_csg_efs_db,&stored_acq_db,sizeof(rrc_csp_acq_db_type),O_CREAT|O_AUTODIR, 0777);
  if(status == -1)
  {
    WRRC_MSG0_ERROR("ACQ DB header queue failed");
  }
  else
  {
    WRRC_MSG0_HIGH("ACQ DB header queue successful");
  }

  
}  /* rrc_csp_write_nv_acq_db_info */


/*===========================================================================

FUNCTION          RRC_CSP_ADD_ACQ_ENTRY

DESCRIPTION       This function adds an acquisition entry to the acquisition 
                  list. While doing so, it also updates the acquisition
                  database pointers so that the 1st pointer points to this
                  most recent acquisition entry in acquisition list.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  rrc_csp_add_acq_entry(rrc_csp_acq_entry_type* acq_entry_ptr)
{

  int   wr_acq_list_index = 0;
  int   i;
  int   ptr_index;
  int   num_valid_entries;
  rrc_csp_band_class_type band_class;
  
#ifdef FEATURE_FEMTO_CSG
  int   replaced_stored_acqdb_index = 0;
  boolean replace_csg_entry = FALSE;
  boolean replace_macro_entry = FALSE;
#endif

  /* Do a Sanity check - check read and write index. A serious error if one
   * of them is out of bounds 
   */
  if ((stored_acq_db.curr_wr_index > MAX_ACQ_DB_INDEX) || 
      (stored_acq_db.curr_rd_index > MAX_ACQ_DB_INDEX) ||
      (stored_acq_db.curr_rd_index < -1) ||
      (stored_acq_db.curr_rd_index > stored_acq_db.curr_wr_index))
  {
    WRRC_MSG2_ERROR("ERROR: Acq db invalid Wr index %d Rd index %d",
                stored_acq_db.curr_wr_index, stored_acq_db.curr_rd_index);
    return(FAILURE);
  }

  rrc_csp_recover_acq_db();

  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) && 
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db.curr_wr_index + 1;
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    num_valid_entries = stored_acq_db.curr_wr_index;
  }

  /* Get a matching acq. entry in acq. list that matches the PLMN and freq with 
   * the entry passed as a parameter.
   */
  for (i = 0; i < num_valid_entries; i++)
  {
    if ((rrc_bcch_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id, 
                              acq_entry_ptr->plmn_id)) &&
        (acq_list[stored_acq_db.acq_list_indices[i]].freq == acq_entry_ptr->freq)
#ifdef FEATURE_FEMTO_CSG
        && (acq_list[stored_acq_db.acq_list_indices[i]].csg_id == acq_entry_ptr->csg_id)
        && ((acq_entry_ptr->csg_id == SYS_CSG_ID_INVALID) ||
            (acq_list[stored_acq_db.acq_list_indices[i]].scr_code == acq_entry_ptr->scr_code))
#endif
        )
    {
      /* Here, we found an acq. entry with same PLMN and freq. This acq. entry 
       * is overwritten with the acq. data provided in the parameter. All acq.
       * database pointers, starting from the first (topmost) and going till the
       * acq. database pointer that pointed to this dup entry, are moved one 
       * downwards, freeing the topmost pointer which is made to point to this
       * previously-dup-but-now-updated acq. entry. Note that in this process, the
       * acq. database pointer pointing to dup entry is not moved downwards and is
       * overwritten by the pointer prior to it.
       * Since the total number of acq. entries has not changed, the write index
       * is unaffected.
       */
      wr_acq_list_index = stored_acq_db.acq_list_indices[i];

      for (ptr_index = i; ptr_index > 0; ptr_index--)
      {
        stored_acq_db.acq_list_indices[ptr_index] = 
          stored_acq_db.acq_list_indices[ptr_index-1];
      }

#ifndef FEATURE_FEMTO_CSG
      acq_list[wr_acq_list_index] = *acq_entry_ptr;
#else
      rrc_csp_fill_acq_entry(&acq_list[wr_acq_list_index],acq_entry_ptr);
#endif
      band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
      acq_list[wr_acq_list_index].band_class = band_class;

      /* Initialize the topmost pointer to the most recent acq. list entry */
      stored_acq_db.acq_list_indices[0] = (uint8)wr_acq_list_index;
      /*Save acq db info in NV. Only if WCDMA RRC Save ACQDB NV is set*/
#ifndef FEATURE_FEMTO_CSG
      if(rrc_save_acqdb_nv)
      {
        /* If first entry matches then no need to write ACQ DB
           header as none of its data members are modified */
        if(i!=0)
        {
          rrc_csp_write_nv_acq_db_info();
        }
        rrc_csp_write_nv_acq_list_info(wr_acq_list_index);
      }
#endif
      /* Return always a Success for the time being */
      MSG_HIGH("Updated: freq %d scr_code %d band %d in Acq DB", 
                              acq_entry_ptr->freq, acq_entry_ptr->scr_code, band_class);

#ifdef FEATURE_FEMTO_CSG
      MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "CSG: Updated: PLMN %d-%d, CSG ID %d; num_csg_entries:%d, curr_wr_index:%d", RRC_CSP_GET_INT_MCC(acq_entry_ptr->plmn_id),
                  RRC_CSP_GET_INT_MNC(acq_entry_ptr->plmn_id), acq_entry_ptr->csg_id,
                  stored_acq_db.num_of_csg_entries, stored_acq_db.curr_wr_index);
#endif

      return (SUCCESS);
    }
  }


  /* Initialize write pointer to the acquisition entry in acquisition list where
   * the new acq. data can be stored. If all entries in acq. list are already
   * filled, the oldest entry as pointed to by the "last" pointer in acq. database
   * is to be overwritten and to be made the most recent entry.
   */


#ifdef FEATURE_FEMTO_CSG
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) &&
      (stored_acq_db.last_entry_invalid == FALSE))
  {
  
      /* ACQ-DB is full */
        MSG_HIGH("CSG: num_of_csg_entries: %d, curr_wr_index %d, CSG ID to add: %d", 
        stored_acq_db.num_of_csg_entries, stored_acq_db.curr_wr_index, acq_entry_ptr->csg_id);
  
      /* Number of macro entries are less than 10*/
        if(acq_entry_ptr->csg_id == SYS_CSG_ID_INVALID)
        {
          replace_macro_entry = TRUE;
        }
        else
        {
          replace_csg_entry = TRUE;
        }
  
  }
  /* If new entry is a Macro and Macro limit is reached, replace the last entered Macro */

  else 
  {
    wr_acq_list_index = stored_acq_db.curr_wr_index;
    if(acq_entry_ptr->csg_id != SYS_CSG_ID_INVALID)
    {
      if(stored_acq_db.num_of_csg_entries == MAX_ACQ_DB_CSG_ENTRIES)
      {
        replace_csg_entry = TRUE;
      }
      else
      {
        stored_acq_db.num_of_csg_entries++;
      }
    }
    else
    {
      if((stored_acq_db.curr_wr_index - stored_acq_db.num_of_csg_entries) == MAX_ACQ_DB_MACRO_ENTRIES)
      {
        replace_macro_entry = TRUE;
      }
    }
  }

  if(replace_csg_entry || replace_macro_entry)
  {
    int8 idx = MAX_ACQ_DB_INDEX;

      MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "CSG: num_of_csg_entries:%d, curr_wr_index:%d, CSG ID to add %d, replace_csg_entry: %d, replace_macro_entry %d",
              stored_acq_db.num_of_csg_entries, stored_acq_db.curr_wr_index, acq_entry_ptr->csg_id,
              replace_csg_entry, replace_macro_entry);
  
    /* If the ACQ-DB is not full traverse DB till it got updated. */
    if(!((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) &&
      (stored_acq_db.last_entry_invalid == FALSE)))
    {
      idx = stored_acq_db.curr_wr_index-1;
    }

      for(; idx > 0; idx-- )
      {
        if((replace_csg_entry && (acq_list[stored_acq_db.acq_list_indices[idx]].csg_id != SYS_CSG_ID_INVALID))
          || ((replace_macro_entry) && (acq_list[stored_acq_db.acq_list_indices[idx]].csg_id == SYS_CSG_ID_INVALID)))
        {
          wr_acq_list_index = stored_acq_db.acq_list_indices[idx];
  
          MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "CSG: Replacing: PLMN %d-%d, CSG ID %d, freq %d scr_code %d", 
                  RRC_CSP_GET_INT_MCC(acq_list[stored_acq_db.acq_list_indices[idx]].plmn_id),
                  RRC_CSP_GET_INT_MNC(acq_list[stored_acq_db.acq_list_indices[idx]].plmn_id),
                  acq_list[stored_acq_db.acq_list_indices[idx]].csg_id, acq_list[stored_acq_db.acq_list_indices[idx]].freq,
                  acq_list[stored_acq_db.acq_list_indices[idx]].scr_code);
  
          break;
        }
      }

      replaced_stored_acqdb_index = idx;
  }
  
#else /* FEATURE_FEMTO_CSG */
  
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) &&
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    wr_acq_list_index = stored_acq_db.acq_list_indices[stored_acq_db.curr_wr_index];
  }
  else
  {
    wr_acq_list_index = stored_acq_db.curr_wr_index;
  }

#endif

  if((wr_acq_list_index > MAX_ACQ_DB_INDEX) || (wr_acq_list_index < 0))
  {
    ERR_FATAL("Invalid write index.", 0, 0, 0);
    return(FAILURE);
  }

  /* Copy the contents now */
#ifndef FEATURE_FEMTO_CSG
  acq_list[wr_acq_list_index] = *acq_entry_ptr;
#else
  rrc_csp_fill_acq_entry(&acq_list[wr_acq_list_index],acq_entry_ptr);

/*Invalidate FP info*/
  acq_list[wr_acq_list_index].w_macro_write_idx = 0;
  acq_list[wr_acq_list_index].fp_w_macro_cnt = 0;
  acq_list[wr_acq_list_index].lte_macro_write_idx = 0;
  acq_list[wr_acq_list_index].fp_lte_macro_cnt = 0;
#endif

  band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
  acq_list[wr_acq_list_index].band_class = band_class;

  /* Now push all pointers in acq. database downwards and copy the pointer to 
   * this acq. entry in the first (topmost) acq. database pointer.
   */
  i = stored_acq_db.curr_wr_index;
#ifdef FEATURE_FEMTO_CSG
  if(replace_csg_entry || replace_macro_entry)
  {
    i = replaced_stored_acqdb_index;
  }
#endif

  for (; i > 0; i--)
  {
    stored_acq_db.acq_list_indices[i] = 
      stored_acq_db.acq_list_indices[i-1];
  }
  /* Initialize the topmost pointer to the most recent acq. list entry */
  stored_acq_db.acq_list_indices[0] = (uint8)wr_acq_list_index;

  /* Increment current number of valid acquisition database pointers if 
   * not all acquisition db pointers have been initialized so far.
   */
  if (stored_acq_db.curr_wr_index < MAX_ACQ_DB_INDEX)
  {
#ifdef FEATURE_FEMTO_CSG
    if(!(replace_csg_entry || replace_macro_entry))
#endif
    {
    stored_acq_db.curr_wr_index++;
    stored_acq_db.last_entry_invalid = TRUE;
  }
  }
  else
  {
    stored_acq_db.last_entry_invalid = FALSE;
  }

  MSG_HIGH("Added: freq %d scr_code %d band %d in Acq DB", 
                              acq_entry_ptr->freq, acq_entry_ptr->scr_code, band_class);
#ifndef FEATURE_FEMTO_CSG
  /*Save acq db info in NV*/
  if(rrc_save_acqdb_nv)
  {
    rrc_csp_write_nv_acq_db_info();
    rrc_csp_write_nv_acq_list_info(wr_acq_list_index);
  }
#endif

#ifdef FEATURE_FEMTO_CSG
  if(stored_acq_db.num_of_csg_entries > MAX_ACQ_DB_CSG_ENTRIES)
  {
    WRRC_MSG0_ERROR("CSG: Invalid number of CSG entries.");
  }
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "CSG: Added: PLMN %d-%d, CSG ID %d; num_csg_entries: %d, curr_wr_index: %d",
          RRC_CSP_GET_INT_MCC(acq_entry_ptr->plmn_id), RRC_CSP_GET_INT_MNC(acq_entry_ptr->plmn_id),
          acq_entry_ptr->csg_id, stored_acq_db.num_of_csg_entries, stored_acq_db.curr_wr_index);

#endif
  /* Return always a Success for the time being */
  return (SUCCESS);

}  /* rrc_csp_add_acq_entry */


#ifdef FEATURE_FEMTO_CSG
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
void rrccsp_add_macro_fp_info_in_acq_db(rrc_csp_acq_entry_type* femto_info)
{

  boolean is_match = FALSE;
  uint8 fp_wr_index;
  uint8 i,j;

  uint8 num_valid_entries = rrc_csp_assert_acq_db_vars();

  for (i = 0; i < num_valid_entries; i++)
  {
    if ((rrc_bcch_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id, femto_info->plmn_id)) 
        && (acq_list[stored_acq_db.acq_list_indices[i]].freq == femto_info->freq)
        && (acq_list[stored_acq_db.acq_list_indices[i]].csg_id == femto_info->csg_id)
        && ((acq_list[stored_acq_db.acq_list_indices[i]].scr_code == femto_info->scr_code)
            && (acq_list[stored_acq_db.acq_list_indices[i]].cell_id == femto_info->cell_id))

        )
    {
      if(rrc_csp_int_data.last_csg_rat == RRC_CSG_LTE)
      {
        WRRC_MSG1_HIGH("CSG: num_of_fp_entries: %d", acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt);

        fp_wr_index = acq_list[stored_acq_db.acq_list_indices[i]].lte_macro_write_idx;
        WRRC_MSG1_HIGH("CSG: LTE FP write index %d", fp_wr_index);
        if(fp_wr_index >= NUM_CSG_LTE_MACRO)
       {
         fp_wr_index = acq_list[stored_acq_db.acq_list_indices[i]].lte_macro_write_idx = 0;
       }
        for(j = 0; j < acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt; j++)
        {
          if ((rrc_bcch_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[j].plmn_id,rrc_csp_int_data.fp_macro.plmn)) 
              && (acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[j].freq == rrc_csp_int_data.fp_macro.freq)
              && (acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[j].pci == rrc_csp_int_data.fp_macro.pci)
              && (acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[j].cell_id == rrc_csp_int_data.fp_macro.cell_id)
             )
          {
            WRRC_MSG0_HIGH("CSG: LTE macro mapping already exists. Setting prune counter to zero");
            acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[j].acq_fail_counter = 0;
            is_match = TRUE;
            break;
          }
        }
        if(is_match)
        {
          break;
        }
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[fp_wr_index].freq = rrc_csp_int_data.fp_macro.freq;
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[fp_wr_index].plmn_id = rrc_csp_int_data.fp_macro.plmn;
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[fp_wr_index].pci= rrc_csp_int_data.fp_macro.pci;
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[fp_wr_index].cell_id = rrc_csp_int_data.fp_macro.cell_id;
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro[fp_wr_index].acq_fail_counter = 0;
        /* Recovery code */
        if(acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt >= NUM_CSG_LTE_MACRO)
        {
          acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt = NUM_CSG_LTE_MACRO;
        }
        if(acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt != NUM_CSG_LTE_MACRO)
        {
          acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt++;
        }
        acq_list[stored_acq_db.acq_list_indices[i]].lte_macro_write_idx++;


        MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "CSG: Added FP info: PLMN %d-%d, CELL ID %d; FREQ: %d, PSC: %d",
               RRC_CSP_GET_INT_MCC(rrc_csp_int_data.fp_macro.plmn), RRC_CSP_GET_INT_MNC(rrc_csp_int_data.fp_macro.plmn),
               rrc_csp_int_data.fp_macro.cell_id, rrc_csp_int_data.fp_macro.freq,rrc_csp_int_data.fp_macro.pci);
        break;
      
      }
      else if(rrc_csp_int_data.last_csg_rat == RRC_CSG_WCDMA)
      {
        WRRC_MSG1_HIGH("CSG: num_of_fp_entries: %d", acq_list[stored_acq_db.acq_list_indices[i]].fp_w_macro_cnt);
        
        fp_wr_index = acq_list[stored_acq_db.acq_list_indices[i]].w_macro_write_idx;
        WRRC_MSG1_HIGH("CSG: W FP write index %d", fp_wr_index);
        if(fp_wr_index >= NUM_CSG_WCDMA_MACRO)
       {
         fp_wr_index = acq_list[stored_acq_db.acq_list_indices[i]].w_macro_write_idx = 0;
       }
        for(j = 0; j < acq_list[stored_acq_db.acq_list_indices[i]].fp_w_macro_cnt; j++)
        {
          if ((rrc_bcch_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].w_macro[j].plmn_id,rrc_csp_int_data.fp_macro.plmn)) 
              && (acq_list[stored_acq_db.acq_list_indices[i]].w_macro[j].freq == rrc_csp_int_data.fp_macro.freq)
              && (acq_list[stored_acq_db.acq_list_indices[i]].w_macro[j].psc == rrc_csp_int_data.fp_macro.pci)
              && (acq_list[stored_acq_db.acq_list_indices[i]].w_macro[j].cell_id == rrc_csp_int_data.fp_macro.cell_id)
             )
          {
            WRRC_MSG0_HIGH("CSG: W macro mapping already exists. Setting acq counter to zero");
            acq_list[stored_acq_db.acq_list_indices[i]].w_macro[j].acq_fail_counter = 0;
            is_match = TRUE;
            break;
          }
        }
        if(is_match)
        {
          break;
        }
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro[fp_wr_index].freq = rrc_csp_int_data.fp_macro.freq;
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro[fp_wr_index].plmn_id = rrc_csp_int_data.fp_macro.plmn;
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro[fp_wr_index].psc= rrc_csp_int_data.fp_macro.pci;
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro[fp_wr_index].cell_id = rrc_csp_int_data.fp_macro.cell_id;
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro[fp_wr_index].acq_fail_counter = 0;
        /* Recovery code */
        if(acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt >= NUM_CSG_WCDMA_MACRO)
        {
          acq_list[stored_acq_db.acq_list_indices[i]].fp_lte_macro_cnt = NUM_CSG_WCDMA_MACRO;
        }
        if(acq_list[stored_acq_db.acq_list_indices[i]].fp_w_macro_cnt != NUM_CSG_WCDMA_MACRO)
        {
          acq_list[stored_acq_db.acq_list_indices[i]].fp_w_macro_cnt++;
        }
        acq_list[stored_acq_db.acq_list_indices[i]].w_macro_write_idx++;
  
        MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "CSG: Added FP info: PLMN %d-%d, CELL ID %d; FREQ: %d, PSC: %d",
               RRC_CSP_GET_INT_MCC(rrc_csp_int_data.fp_macro.plmn), RRC_CSP_GET_INT_MNC(rrc_csp_int_data.fp_macro.plmn),
               rrc_csp_int_data.fp_macro.cell_id, rrc_csp_int_data.fp_macro.freq,rrc_csp_int_data.fp_macro.pci);
        break;
      }
      else
      {
        WRRC_MSG0_HIGH("CSG: FP: Not adding FP info");
      }
    }
  }
}

#endif


/*===========================================================================

FUNCTION          RRC_CSP_SHOULD_FREQ_BE_SCANNED

DESCRIPTION       This function checks whether an acquisition has already been
                  tried on the indicated frequency as part of acquisition database
                  scan.

DEPENDENCIES
                  None.

RETURN VALUE
                  Boolean. False indicates the frequency has been scanned before
                           True otherwise

SIDE EFFECTS
                  None.

===========================================================================*/
boolean
rrc_csp_should_freq_be_scanned(rrc_requested_plmn_type req_plmn, rrc_freq_type freq)
{
  int i;
  int num_valid_entries;
  rrc_state_e_type rrc_current_state = rrc_get_state();

  /* When inter frequency redirection scan is in progress, 
   * UE should not attempt acquisition on the last camped frequency till
   * the expiry of the wait timer. Since this function has been called,
   * it implies that the wait timer has not expired. Hence, always return
   * FALSE, i.e., do not scan this frequency.
   */
  if((rrc_current_state == RRC_STATE_CONNECTING) 
      && (rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN) 
        && (rrc_csp_int_data.last_camped_freq == freq))
  {
    WRRC_MSG1_HIGH("freq Rdrct: Do not attempt acq on last camped freq %d during freq scan",
                                                          rrc_csp_int_data.last_camped_freq);
    return FALSE;
  }

  /*Added scan check for safety. Do not skip scanning frequency on which we
    suspended BPLMN upon DRX timer expiry*/
  if(((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))&&
     (rrc_csp_int_data.bplmn_acq_succ_freq == freq))
  {
    return TRUE;
  }

#ifdef FEATURE_EOOS
  #error code not present
#endif /* FEATURE_EOOS */


  /* If UE is in connected mode then always scan the frequency - performance;
   * Or if in idle mode the frequency found during full scan is the same as the last
   * camped frequency then scan this frequency again - performance.
   */
  if(((rrc_current_state != RRC_STATE_DISCONNECTED)
      || (freq == rrc_csp_int_data.last_camped_freq_in_idle_mode))
    && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
    && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
    && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOG_BPLMN_AUTOMATIC_SCAN)
    && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOG_BPLMN_MANUAL_SCAN))
  {
    return TRUE;
  }

  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) &&
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db.curr_wr_index + 1;
  }
  else
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    num_valid_entries = stored_acq_db.curr_wr_index;
  }  

  /* Now look for an acq. entry that has the PLMN to be matched */
  for (i = 0; i < num_valid_entries; i++)
  {
    /* For SPEC and ANY PLMN_REQ, all the acquisition entries are scanned */
    if(acq_list[stored_acq_db.acq_list_indices[i]].freq == freq)
    { 

#ifdef FEATURE_FEMTO_CSG
       /*Scan non-white ACQ DB CSG entries which are skipped earlier. If ACQ DB has two entries with same frequency 
         one with macro and other with non white CSG cell then we should skip, as macro entry would have been
         scanned already*/
       if((acq_list[stored_acq_db.acq_list_indices[i]].csg_id != SYS_CSG_ID_INVALID) && 
          !(rrccsp_csg_in_whitelist(acq_list[stored_acq_db.acq_list_indices[i]].csg_id,
                                       acq_list[stored_acq_db.acq_list_indices[i]].plmn_id)))
       {
         continue;
       }
#endif      /* Frequency found in the acquisition database. Now check if the band 
       * corresponding to this frequency is supported by the UE or not. If not, then
       * during ACQ DB scan this frequency was not scanned and hence needs to be scanned
       * during full frequency scan. Otherwise, this freq was scanned during ACQ DB scan
       * and hence there is no need to re-scan this frequency.
       * E.g., UE camped on 4387 with Japan MCC and 800 band. UE then moves to US and the
       * band UE is searching for is 850 and acq type is ANY. During ACQ DB scan, 4387
       * will be skipped as the corresponding band_class (800) is not supported; and during
       * full frequency scan if this freq is found then it will again be skipped incorrectly
       * if the additional check below is not added.
       */

      if(rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))
      {
        WRRC_MSG0_HIGH("Freq and band found in acq db, don't scan");
        return FALSE;
      }
    }
  }

  return TRUE;

} /* rrc_csp_should_freq_be_scanned */


/*===========================================================================

FUNCTION          RRC_CSP_HAS_ACQ_DB_FREQ_BEEN_SCANNED

DESCRIPTION       This function avoids multiple acquisition attempts on 
                  multiple entries of the same frequency in the acquisition 
                  database during any plmn scan.
                  
DEPENDENCIES
                  None.

RETURN VALUE
                  Boolean. True  indicates the frequency has been scanned before
                           False otherwise

SIDE EFFECTS
                  None.

===========================================================================*/
static boolean
rrc_csp_has_acq_db_freq_been_scanned(rrc_freq_type freq)
{
  uint8 i;

  for (i = 0; i < acq_db_freq_scanned_list.wr_index; i++)
  {
    if(freq == acq_db_freq_scanned_list.freq[i])
    {
      WRRC_MSG1_HIGH("Acq DB Freq %d already scanned. Don't scan",freq);
      return TRUE;
    }             
  } 

 /* When inter frequency redirection scan is in progress, 
  * UE should not attempt acquisition on the last camped frequency till
  * the expiry of the wait timer. Since this function has been called,
  * it implies that the wait timer has not expired. Hence, always return
  * FALSE, i.e., do not scan this frequency.
  */
  if((rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN) 
      && (rrc_csp_int_data.last_camped_freq == freq))
  {
    WRRC_MSG1_HIGH("Freq Redirect wt tmr has !expired for freq %d. Don't scan", freq);
    return TRUE;
  }

  WRRC_MSG1_HIGH("Acq DB Freq %d NOT scanned. Scan",freq);
  return FALSE;
} /* rrc_csp_has_acq_db_freq_been_scanned */


/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_ACQ_DB_FREQ_SCANNED_LIST

DESCRIPTION       This function adds the acq db frequency to the scanned list.
                  
DEPENDENCIES
                  None.

RETURN VALUE
                  Boolean. True  indicates the frequency has been scanned before
                           False otherwise

SIDE EFFECTS
                  None.

===========================================================================*/
static void
rrc_csp_update_acq_db_freq_scanned_list(rrc_freq_type freq)
{ 
  if(acq_db_freq_scanned_list.wr_index >= MAX_ACQ_DB_SIZE)
  {
    WRRC_MSG0_ERROR("Max # of available entries in freq scanned list exceeded!");
    acq_db_freq_scanned_list.wr_index = 0;
  }

  /* Add Frequency to the list */
  WRRC_MSG1_HIGH("Adding freq %d to the scanned list", freq);
  acq_db_freq_scanned_list.freq[acq_db_freq_scanned_list.wr_index++] = freq;

  return;
} /* rrc_csp_update_acq_db_freq_scanned_list */


/*===========================================================================

FUNCTION          RRC_CSP_SHOULD_ALL_ACQ_DB_BE_SCANNED

DESCRIPTION       This function determines if for a particular CSP scan type
                  the complete ACQ DB should be scanned or not. The decision is
                  based on the CSP scan type and whether full frequency scan
                  is required or not
           
DEPENDENCIES
                  None

RETURN VALUE
                  Boolean
                  TRUE: Complete ACQ DB should be scanned
                  FALSE: Otherwise.
                  
SIDE EFFECTS
                  None.

===========================================================================*/
static boolean rrc_csp_should_all_acq_db_be_scanned(void)
{
  boolean status = TRUE;

  switch(rrc_csp_int_data.curr_scan)
  {
    case RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN:
    case RRC_CSP_LEAVING_CONN_MODE_SCAN:
      status = FALSE;
      break;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
     #error code not present
#endif
    case RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN:
    case RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN:
    case RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN:
    case RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN:
    case RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN:
    case RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN:
      if(rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles > 1)
      {
        WRRC_MSG1_HIGH("Do NOT scan ALL ACQ DB. cycle# %d", 
                  rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles);
        status = FALSE;
      }
      break;

    case RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN:
#ifdef FEATURE_FEMTO_CSG
      if(rrc_csp_int_data.csg_asf_srch_in_progress)
      {
        status = FALSE;
        WRRC_MSG0_HIGH("CSG: Do not scan all ACQ DB during ASF srch.");
      }
      else
#endif
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
      /* Under this feature we always scan all ACQ DB entries as we are not sure 
         which PLMN NAS is interested in. NAS needs to be reported with all the PLMNs 
         available till specific PLMN is found */
         WRRC_MSG0_HIGH("Do not skip ACQ DB entry scan");
#else
      if(   (rrc_csp_int_data.bplmn_req_list_rd_index != (rrc_csp_int_data.bplmn_num_req - 1))
          || ((!rrc_csp_int_data.plmn_list_freq_scan_needed)
                && (!rrc_csp_int_data.acq_db_empty)))
      {
        MSG_HIGH("Do NOT scan ALL ACQ DB. rd_ix %d, num_req %d, fscan_needed %d", 
                                       rrc_csp_int_data.bplmn_req_list_rd_index, 
                                       rrc_csp_int_data.bplmn_num_req, 
                                       rrc_csp_int_data.plmn_list_freq_scan_needed);

        status = FALSE;
      }
#endif
      break;

    case RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN:
#ifdef FEATURE_FEMTO_CSG
      if(rrc_csp_int_data.csg_asf_srch_in_progress)
      {
        status = FALSE;
        WRRC_MSG0_HIGH("CSG: Do not scan all ACQ DB during ASF srch.");
      }
      else
#endif
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
      /* Under this feature we always scan all ACQ DB entries as we are not sure 
         which PLMN NAS is interested in. NAS needs to be reported with all the PLMNs 
         available till specific PLMN is found */
         WRRC_MSG0_HIGH("Do not skip ACQ DB entry scan");
#else
      if(   (rrc_csp_int_data.bplmn_req_list_rd_index != (rrc_csp_int_data.wtow_bplmn_num_req - 1))
          || ((!rrc_csp_int_data.plmn_list_freq_scan_needed)
                && (!rrc_csp_int_data.acq_db_empty)))
      {
        MSG_HIGH("Do NOT scan ALL ACQ DB. rd_ix %d, num_req %d, fscan_needed %d", 
                                       rrc_csp_int_data.bplmn_req_list_rd_index, 
                                       rrc_csp_int_data.wtow_bplmn_num_req, 
                                       rrc_csp_int_data.plmn_list_freq_scan_needed);

        status = FALSE;
      }
#endif
      break;


    case RRC_CSP_OUT_OF_SERVICE_AREA_SCAN:
      if(    (rrc_get_state() == RRC_STATE_CELL_DCH)
          && (rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles > 1))
      {
        WRRC_MSG1_HIGH("Do NOT scan ALL ACQ DB. cycle# %d", 
                  rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles);
        status = FALSE;
      }
      break;


    default:
      break;
  }

  WRRC_MSG2_HIGH("Scan Type %d. Scan ALL Acq DB Status = %d", rrc_csp_int_data.curr_scan, status);
  return (status);

} /* rrc_csp_should_all_acq_db_be_scanned */



/*===========================================================================

FUNCTION          RRC_CSP_GET_MATCH_ACQ_ENTRY

DESCRIPTION       This function gets the entry,from the acquisition list that 
                  matches the criteria given in the parameters. It starts 
                  looking for match starting from the acq. database pointer 
                  next to the one indexed by current read index.
           
DEPENDENCIES
                  This function is closely knit with rrc_csp_compare_expected_cell()
                  and rrc_csp_continue_stored_initial_select_ctrl(). 
                  Any change in this function's logic should be reflected in others.

RETURN VALUE
                  Pointer to acquisition entry. A NULL pointer means no 
                  acquisition entry was found that fulfilled selection criteria.
SIDE EFFECTS
                  None.

===========================================================================*/
static rrc_csp_acq_entry_type*  
rrc_csp_get_match_acq_entry(rrc_plmn_identity_type req_plmn_id, rrc_csp_acq_e_type req_type_acq)
{

  int i;
  int num_valid_entries;

  rrc_csp_acq_entry_type  *ret_acq_entry_ptr = NULL;
  boolean is_eplmn = FALSE;
  boolean is_network_shared = FALSE;
  boolean is_acq_db_plmn_matched = FALSE;

  /* Do a Sanity check - check read and write index. A serious error if one
   * of them is out of bounds  Also read index should never be higher than 
   * write index.
   */
  num_valid_entries = rrc_csp_assert_acq_db_vars();


  switch(req_type_acq)
  {
    case RRC_CSP_SPEC_PLMN:
      
      /* if specific plmn is not found when UE is in deep sleep then
       * automatically look for any plmn in acq db 
       */
      if(!rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop)
      {
        
        for (i = stored_acq_db.curr_rd_index + 1; i < num_valid_entries; i++)
        {
          is_network_shared = acq_list[stored_acq_db.acq_list_indices[i]].is_valid_nw_sharing;
          /* Resetting the is_eplmn variable at start of loop */
          is_eplmn = FALSE;

#ifdef FEATURE_FEMTO_CSG
          if(((acq_list[stored_acq_db.acq_list_indices[i]].csg_id != SYS_CSG_ID_INVALID) && 
             !(rrccsp_csg_in_whitelist(acq_list[stored_acq_db.acq_list_indices[i]].csg_id,
                                       acq_list[stored_acq_db.acq_list_indices[i]].plmn_id)))
              || ((rrccsp_asf_actively_in_prog() && (acq_list[stored_acq_db.acq_list_indices[i]].csg_id == SYS_CSG_ID_INVALID)))
              || ((rrccsp_asf_actively_in_prog() && !rrccsp_fp_macro_present_in_acq_db(acq_list[stored_acq_db.acq_list_indices[i]]))))
          {
            MSG_MED("CSG: Skip acq db entry %d as CSG ID %d is not in whitelist/ macro cell during ASF", stored_acq_db.acq_list_indices[i],acq_list[stored_acq_db.acq_list_indices[i]].csg_id,0);
            continue;
          }
#endif

          if(    (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
              /* 
               * we need it now bcoz for rl_failure the scan type is no longer dch->fach
               * and is always oos_area_scan. 
               */
              || (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
              || (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
            )
          {
            if((rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                      rrc_csp_int_data.curr_req_plmn.plmn_id))) == TRUE)
            {
              WRRC_MSG0_HIGH("Curr requested PLMN is in the NAS equivalent PLMN list");
              if((!is_network_shared) &&
                 (rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                      acq_list[stored_acq_db.acq_list_indices[i]].plmn_id)) == TRUE))
              {
                WRRC_MSG2_HIGH("PLMN %d-%d is in the eq PLMN list",
                           RRC_CSP_GET_INT_MCC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id),
                           RRC_CSP_GET_INT_MNC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id));
                is_eplmn = TRUE;
              }
              else if((is_network_shared) && 
                 ((rrccsp_nas_plmn_equivalent_to_any_of_shared_bcch_plmns(
                                                                          rrc_csp_int_data.curr_req_plmn.plmn_id,
                                                                          stored_acq_db.acq_list_indices[i])) == TRUE))
              {
                is_eplmn = TRUE;
              }
            }
          }
          
          if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
          {
            MSG_HIGH("ACQ DB PLMN being compared %d-%d. Freq %d",
                     RRC_CSP_GET_INT_MCC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id),
                     RRC_CSP_GET_INT_MNC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id),
                     acq_list[stored_acq_db.acq_list_indices[i]].freq);
          }
          
  #ifdef FEATURE_WCDMA_MULTIBAND
          /* Collect band information for frequencies present in ACQ DB */
          if(rrc_csp_int_data.ue_in_deep_sleep && 
                  (rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class)))
          { 
              rrc_csp_int_data.deep_sleep_acq_db_band_mask |= 
                                               acq_list[stored_acq_db.acq_list_indices[i]].band_class;
          }
  #endif
          is_acq_db_plmn_matched = FALSE;
          if((!is_network_shared) &&
             (rrc_bcch_and_nas_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id, 
                                           req_plmn_id)))
          {
            is_acq_db_plmn_matched = TRUE;
          }
          else if((is_network_shared) &&
                  (rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                       req_plmn_id,
                                                       stored_acq_db.acq_list_indices[i])))
          {
            is_acq_db_plmn_matched = TRUE;
          }

          if((is_acq_db_plmn_matched
              || (is_eplmn)
              ) && (!rrccsp_should_freq_be_skipped(
                acq_list[stored_acq_db.acq_list_indices[i]].freq, RRCCSP_FINE_SCAN_SKIP_RANGE))
                && (rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))
                && (!rrc_csp_has_acq_db_freq_been_scanned(
                          acq_list[stored_acq_db.acq_list_indices[i]].freq)))
          {
            /* A suitable entry is found. Set the Read index to this db pointer entry and 
             * return pointer to the acquisition list contained in this db pointer entry
             */
            stored_acq_db.curr_rd_index = (int8)i;
            ret_acq_entry_ptr = &acq_list[stored_acq_db.acq_list_indices[i]];
  
            /* Update scanned list with the frequency to avoid multiple acquisition
             * attempts on the same frequency
             */
            rrc_csp_update_acq_db_freq_scanned_list(acq_list[stored_acq_db.acq_list_indices[i]].freq);
            rrc_csp_int_data.acq_db_empty = FALSE;
            break;
          }
        }
      }
      else
      {
        /* If ue is in deep sleep then search for any plmn if
         * specific plmn is not found/could not acquire.
         */
        WRRC_MSG1_HIGH("Searching for Any PLMN; looping = TRUE(%d)",rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop);
      }
      break;

    case RRC_CSP_ANY_PLMN:
      i = stored_acq_db.curr_rd_index + 1;
      while(i < num_valid_entries)
      {
#ifdef FEATURE_WCDMA_MULTIBAND
        /* Collect band information for frequencies present in ACQ DB */
        if(rrc_csp_int_data.ue_in_deep_sleep && 
                (rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class)))
        { 
          rrc_csp_int_data.deep_sleep_acq_db_band_mask |= 
                                             acq_list[stored_acq_db.acq_list_indices[i]].band_class;
        }
#endif

        if((!(rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))) ||
           (rrccsp_should_freq_be_skipped(
             acq_list[stored_acq_db.acq_list_indices[i]].freq, RRCCSP_FINE_SCAN_SKIP_RANGE)) ||
           (rrc_csp_has_acq_db_freq_been_scanned(acq_list[stored_acq_db.acq_list_indices[i]].freq)))
        {
          i++;
        }
        else
        {
          stored_acq_db.curr_rd_index = (int8)i;
          ret_acq_entry_ptr = &acq_list[stored_acq_db.acq_list_indices[i]];

          /* Update scanned list with the frequency to avoid multiple acquisition
           * attempts on the same frequency
           */
          rrc_csp_update_acq_db_freq_scanned_list(acq_list[stored_acq_db.acq_list_indices[i]].freq);
          rrc_csp_int_data.acq_db_empty = FALSE;
          break;
        }
      }

      break;

    default:
      WRRC_MSG1_HIGH("Invalid req acq type %d", req_type_acq);
      break;
  }

  /* If ret_acq_entry_ptr is NULL and if UE is in deep sleep then try all PLMNs 
   * in the Acq DB before doing a full frequency scan.
   */
  if(    (!ret_acq_entry_ptr)
      && (req_type_acq == RRC_CSP_SPEC_PLMN)
      && (rrc_csp_should_all_acq_db_be_scanned())
    )
  {
    /* reset stored_acq_db.curr_rd_index to search the acq db again */
    if(!rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop)
    {
      rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop = TRUE;
      stored_acq_db.curr_rd_index                        = -1;
      WRRC_MSG0_HIGH("Scan ALL ACQ DB entries now. curr_rd_index=-1 and looping=1");
    }

    /* Look in the acq db for any PLMN. 
     * Code same as in the switch case RRC_CSP_ANY_PLMN above.
     */
    i = stored_acq_db.curr_rd_index + 1;
    while(i < num_valid_entries)
    {
      is_network_shared = acq_list[stored_acq_db.acq_list_indices[i]].is_valid_nw_sharing;

#ifdef FEATURE_WCDMA_MULTIBAND
      /* Collect band information for frequencies present in ACQ DB */
      if(rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))
      { 
        rrc_csp_int_data.deep_sleep_acq_db_band_mask |= 
                                           acq_list[stored_acq_db.acq_list_indices[i]].band_class;
      }
#endif

      if( (rrccsp_should_freq_be_skipped(
               acq_list[stored_acq_db.acq_list_indices[i]].freq, RRCCSP_FINE_SCAN_SKIP_RANGE)) 
          || (!rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))
          || ((!is_network_shared && 
               rrc_bcch_and_nas_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id, 
                                            req_plmn_id))
              || (is_network_shared &&
                  rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                       req_plmn_id,
                                                       stored_acq_db.acq_list_indices[i]))
              )
          || (rrc_csp_has_acq_db_freq_been_scanned(acq_list[stored_acq_db.acq_list_indices[i]].freq)))
      {
        i++;
      }
      else
      {
        stored_acq_db.curr_rd_index = (int8)i;
        ret_acq_entry_ptr = &acq_list[stored_acq_db.acq_list_indices[i]];

        /* Update scanned list with the frequency to avoid multiple acquisition
         * attempts on the same frequency
         */
        rrc_csp_update_acq_db_freq_scanned_list(acq_list[stored_acq_db.acq_list_indices[i]].freq);
        break;
      }
    }

    /* if no PLMN is found then reset deep_sleep_acq_db_plmn_searchloop 
     * for future searches in deep sleep
     */

    if(!ret_acq_entry_ptr)
    {
      rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop = FALSE;
      WRRC_MSG1_HIGH("Exhausted ALL ACQ DB entries scanning. Looping=%d",
                    rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop);
    }
                                                                                                                    
  } /* end ret_acq_entry_ptr is NULL and ue_in_deep_sleep */

  return (ret_acq_entry_ptr);

}  /* rrc_csp_get_match_acq_entry */

/*===========================================================================

FUNCTION          RRC_CSP_GET_FIRST_MATCH_ACQ_ENTRY

DESCRIPTION       This function gets the most recent entry from the acquisition 
                  list that matches the criteria given in the parameters. 
                  
                  The most recent acquisition entry is based on the order of 
                  pointers in acquisition database and NOT on the order of entries
                  in acquisition list. 
                  An acquisition entry pointed to by a pointer that occurs before
                  in the acquisition database pointer list is considered more 
                  recent than another entry which is pointed to by a pointer which
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
rrc_csp_get_first_match_acq_entry(rrc_plmn_identity_type req_plmn_id, rrc_csp_acq_e_type req_type_acq)
{
  rrc_csp_last_req_type_acq = req_type_acq;
  rrc_csp_last_req_plmn_id = req_plmn_id;

  /* Reset Read Index of Acq. database to -1 */
  stored_acq_db.curr_rd_index = -1;

  /* Reset number of unique acq db frequencies already scanned */

  /* If Automatic BPLMN search is in progress then determine if 
   * the variables need to be reset or not based on the read_index
   * of the Higher Priority PLMN list
   */
  if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
      || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
  {
    if(rrc_csp_int_data.bplmn_req_list_rd_index == 0)
    {
      acq_db_freq_scanned_list.wr_index = 0;

      /* Acq db empty variable is initialized to TRUE. The variable is updated if
       * any matching PLMNs are found in ACQ DB
       */
      rrc_csp_int_data.acq_db_empty = TRUE;
    } 
  }
  /* For all other scans, always reset the following variables */
  else
  {
    acq_db_freq_scanned_list.wr_index = 0;

    /* Acq db empty variable is initialized to TRUE. The variable is updated if
     * any matching PLMNs are found in ACQ DB
     */
    rrc_csp_int_data.acq_db_empty = TRUE;
  }

  /* Reset deep_sleep_acq_db_search_loop */
  rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop = FALSE;
  WRRC_MSG2_HIGH("Acq DB Search. Looping = %d and ue_in_deep_sleep = %d",
         rrc_csp_int_data.deep_sleep_acq_db_plmn_searchloop,rrc_csp_int_data.ue_in_deep_sleep); 

  return(rrc_csp_get_match_acq_entry( req_plmn_id, req_type_acq));

}  /* rrc_csp_get_first_match_acq_entry */

/*===========================================================================

FUNCTION          RRC_CSP_GET_NEXT_MATCH_ACQ_ENTRY

DESCRIPTION       This function gets next entry from the acquisition list
                  that matches the criteria given in the parameters. 
                  To find the "next" entry, the acquisition database pointer list 
                  is read starting from the pointer next to the one pointed to by
                  curr read index.
                   
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
rrc_csp_get_next_match_acq_entry(rrc_plmn_identity_type req_plmn_id, rrc_csp_acq_e_type req_type_acq)
{
  /* Compare the stored acq type from last call to rrc_csp_get_acq_entry() 
   * with the one passed here. Return NULL if they don't match.
   */
  if( rrc_csp_last_req_type_acq != req_type_acq )
  {
    WRRC_MSG2_HIGH("Passed acq type %d does not match last acq type %d", 
                                        req_type_acq, rrc_csp_last_req_type_acq);
    return NULL;
  }

  /* If the acq. types match, compare the stored plmn ID passed in last call to 
   * rrc_csp_get_acq_entry() with the one passed here. Return NULL if they don't match.
   */
  if(rrc_csp_last_req_type_acq == RRC_CSP_SPEC_PLMN)
  {
    if(!RRC_CSP_SAME_MCC(req_plmn_id, rrc_csp_last_req_plmn_id))
    {
        WRRC_MSG2_HIGH("Passed plmn.MCC %d does not match last plmn.MCC %d", 
                                         RRC_CSP_GET_INT_MCC(req_plmn_id), 
                        RRC_CSP_GET_INT_MCC(rrc_csp_last_req_plmn_id));
        return NULL;
    }
    if(!RRC_CSP_SAME_MNC(req_plmn_id, rrc_csp_last_req_plmn_id))
    {
        WRRC_MSG2_HIGH("Passed plmn.MNC %d does not match last plmn.MNC %d", 
                                        RRC_CSP_GET_INT_MNC(req_plmn_id), 
                       RRC_CSP_GET_INT_MNC(rrc_csp_last_req_plmn_id));
        return NULL;
    }
  }

  /* The initial sanity check is okay - proceed further and get the matching entry */
  return(rrc_csp_get_match_acq_entry( req_plmn_id, req_type_acq));

}  /* rrc_csp_get_next_match_acq_entry */

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
void rrccsp_update_band_class_in_acq_db(void)
{
  int i;
  int num_valid_entries;

  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) &&
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db.curr_wr_index + 1;
  }
  else
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    num_valid_entries = stored_acq_db.curr_wr_index;
  }

  for (i = 0; i < num_valid_entries; i++)
  {
    if ((acq_list[stored_acq_db.acq_list_indices[i]].band_class & 
          RRC_CSP_WCDMA_850_BAND) &&
           (rrccsp_is_supported_band(RRC_CSP_WCDMA_850_BAND))
           /* If both bands 800 and 850 are supported in the UI/RF then 
            * we can't determine if the UARFCN belongs to 850 or 800.
            * Hence do not update the band class of the UARFCN and leave it
            * as 850 + 800.
            */
           && !(rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
       )
    {
      WRRC_MSG0_HIGH("Setting the band to 850MHHz");
      acq_list[stored_acq_db.acq_list_indices[i]].band_class =
        RRC_CSP_WCDMA_850_BAND;
    }
    else if((acq_list[stored_acq_db.acq_list_indices[i]].band_class & 
              RRC_CSP_WCDMA_800_BAND) &&
               (rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
               /* If both bands 800 and 850 are supported in the UI/RF then 
                * we can't determine if the UARFCN belongs to 850 or 800.
                * Hence do not update the band class of the UARFCN and leave it
                * as 850 + 800.
                */
               && !(rrccsp_is_supported_band(RRC_CSP_WCDMA_850_BAND))
           )
    {
      WRRC_MSG0_HIGH("Setting the band to 800MHHz");
      acq_list[stored_acq_db.acq_list_indices[i]].band_class =
        RRC_CSP_WCDMA_800_BAND;
    }
  }
}


/*===========================================================================

FUNCTION          RRCCSP_BPLMN_UPDATE_STORED_ACQ_DB_RD_INDEX

DESCRIPTION       This function is used by CSP to update 
                  stored_acq_db.curr_rd_index if BPLMN srch timer expires
                  before SIBs could be read.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_bplmn_update_stored_acq_db_rd_index ( )
{
  if(stored_acq_db.curr_rd_index >= 0)
  {
    --stored_acq_db.curr_rd_index;
  }
  --acq_db_freq_scanned_list.wr_index;
  WRRC_MSG2_HIGH("BPLMN: Setting curr_rd_ix=%d & acq_db_scanned_list_wr_ix=%d", 
                        stored_acq_db.curr_rd_index, acq_db_freq_scanned_list.wr_index);
}

/*===========================================================================

FUNCTION          RRCCSP_BPLMN_SET_STORED_ACQ_DB_RD_INDEX

DESCRIPTION       This function is used by CSP to set 
                  stored_acq_db.curr_rd_index if BPLMN srch timer expires
                  before SIBs could be read.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_bplmn_set_stored_acq_db_rd_index (int8 value )
{
  stored_acq_db.curr_rd_index = value;
}

/*===========================================================================

FUNCTION          RRC_CSP_ASSERT_ACQ_DB_VARS

DESCRIPTION       This function is used to check the sanity of acq db entries

DEPENDENCIES      NONE

RETURN VALUE      number of entries in ACQ DB
                  
SIDE EFFECTS      NONE

===========================================================================*/
int rrc_csp_assert_acq_db_vars(void)
{
  ASSERT(stored_acq_db.curr_wr_index <= MAX_ACQ_DB_INDEX);
  ASSERT(stored_acq_db.curr_rd_index <= MAX_ACQ_DB_INDEX);
  ASSERT(stored_acq_db.curr_rd_index >= -1);
  ASSERT(stored_acq_db.curr_rd_index <= stored_acq_db.curr_wr_index);

  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) && 
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    return (stored_acq_db.curr_wr_index + 1);
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    return (stored_acq_db.curr_wr_index);
  }
} /* rrc_csp_assert_acq_db_vars */

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
rrc_csp_acq_entry_type *rrc_csp_lfs_update_list_db_with_target_fdd_list(void)
{
  int i;
  int j;
  boolean freq_found_in_list_db = FALSE;

  WRRC_MSG1_HIGH("GtoW Redirection: Number of Target FDD frequencies: %d",
    rrc_csp_int_data.target_fdd_cell_list.num_of_frequencies);
  WRRC_MSG0_HIGH("LFS: Updating List DB with Target freqs");

#ifdef FEATURE_EOOS
  #error code not present
#else

  rrc_csp_int_data.num_freq_in_list_db =0;
  rrc_csp_int_data.curr_rd_idx =0;
  rrc_csp_curr_select_data.num_available_freq = 0;
  for(i = 0; i< rrc_csp_int_data.target_fdd_cell_list.num_of_frequencies; i++)
  {
    freq_found_in_list_db = FALSE;
    for(j = 0; j< rrc_csp_int_data.num_freq_in_list_db; j++)
    {
      if(list_db[j].freq == rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[i].uarfcn)
      {
        // Freq already present in List DB
        freq_found_in_list_db = TRUE;
        break;
      }
    }

    if(!freq_found_in_list_db)
    {
      list_db[rrc_csp_int_data.num_freq_in_list_db].freq =
       rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[i].uarfcn;
      rrc_csp_int_data.num_freq_in_list_db++;
    }
    
    if(rrc_csp_int_data.num_freq_in_list_db == MAX_ACQ_DB_ENTRIES)
    {
      WRRC_MSG0_HIGH("Reached the maximum limit for ACQ DB entries");
      break;
    }
  }
  WRRC_MSG1_HIGH("LFS: Added %d entries to List DB", rrc_csp_int_data.num_freq_in_list_db);
  return &list_db[0];
#endif
}

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
void rrccsp_add_acq_db_and_nw_db_entries(rrc_csp_acq_entry_type* acq_entry_ptr,
                                         rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr)
{
  rrc_plmn_identity_type chosen_plmn = acq_entry_ptr->plmn_id;

#ifdef FEATURE_FEMTO_CSG
  /* Do not add entries to ACQ-DB during ASF search */
  if(rrccsp_asf_actively_in_prog())
  {
    return;
  }
#endif

  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    WRRC_MSG0_HIGH("NWS: Current cell is shared, Adding shared plmns");
    acq_entry_ptr->plmn_id = nw_sharing_entry_ptr->common_plmn_id;
    acq_entry_ptr->shared_list_of_plmns = *nw_sharing_entry_ptr;
  }
  else
  {
    WRRC_MSG0_HIGH("NWS: Current cell is not shared, not adding shared plmns");
  }

  if(rrc_csp_add_acq_entry(acq_entry_ptr) == SUCCESS)
  {
    acq_entry_ptr->plmn_id = chosen_plmn;
  }
}

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
                                              boolean is_valid_nw_sharing)
{
  rrc_csp_acq_entry_type  acq_entry;
  acq_entry.status_mask  = 0;
  acq_entry.cell_id= 0;
  acq_entry.freq                 = rrc_csp_int_data.curr_acq_entry.freq;
  acq_entry.scr_code             = rrc_csp_int_data.curr_acq_entry.scr_code;
  acq_entry.is_valid_nw_sharing  = is_valid_nw_sharing;

#ifdef FEATURE_WCDMA_MULTIBAND
  acq_entry.band_class  = rrc_get_frequency_band(rrc_csp_int_data.curr_acq_entry.freq,
                                                 plmn_id);
  WRRC_MSG2_HIGH("band_class %d for freq %d", acq_entry.band_class, acq_entry.freq);
 
#else
  acq_entry.band_class = RRC_CSP_PCS_BAND;
  WRRC_MSG2_HIGH("band_class %d for freq %d", acq_entry.band_class, acq_entry.freq);
#endif
  
 
  if(acq_entry.is_valid_nw_sharing)
  {
    acq_entry.plmn_id = nw_sharing_entry_ptr->shared_plmns_access_info[0].plmn_id;
  }
  else 
  {
    acq_entry.plmn_id = plmn_id;
  }
  
#ifdef FEATURE_FEMTO_CSG
  /* Not yet read SIB3*/
  acq_entry.csg_id = SYS_CSG_ID_INVALID;
#endif
  
  rrccsp_add_acq_db_and_nw_db_entries(&acq_entry, nw_sharing_entry_ptr);
}

/*===========================================================================

FUNCTION          RRCCSP_NW_SHARING_PRINT_ENTRY

DESCRIPTION       Prints a given network sharing entry

DEPENDENCIES      NONE

RETURN VALUE      None
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_nw_sharing_print_entry(rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr)
{
  int i;

  WRRC_MSG2_HIGH("NWS: Common PLMN MCC:MNC - %d:%d",
                                         RRC_CSP_GET_INT_MCC(nw_sharing_entry_ptr->common_plmn_id),
                                         RRC_CSP_GET_INT_MNC(nw_sharing_entry_ptr->common_plmn_id));
  WRRC_MSG1_HIGH("NWS: Number of Shared PLMNs: %d",nw_sharing_entry_ptr->num_of_shared_plmns);

  WRRC_MSG0_HIGH("NWS: List of Shared PLMNs");
  WRRC_MSG0_HIGH("******************************************************************");
  for(i =0 ; i<nw_sharing_entry_ptr->num_of_shared_plmns; i++)
  {
    WRRC_MSG2_HIGH("MCC:MNC - %d:%d",
                                RRC_CSP_GET_INT_MCC(nw_sharing_entry_ptr->shared_plmns_access_info[i].plmn_id),
                                RRC_CSP_GET_INT_MNC(nw_sharing_entry_ptr->shared_plmns_access_info[i].plmn_id));
  }
  WRRC_MSG0_HIGH("******************************************************************");

}


/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_EQUIVALENT_TO_ANY_OF_SHARED_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are equivalent

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_nas_plmn_equivalent_to_any_of_shared_bcch_plmns(
                                                               rrc_plmn_identity_type nas_plmn, 
                                                               uint8 index_in_acq_or_shared_db)
{
  rrc_plmn_identity_type bcch_plmn;
  rrc_csp_acq_entry_type *acq_entry = &acq_list[index_in_acq_or_shared_db];
  int i;

  if(acq_entry->is_valid_nw_sharing)
  {
    for(i=0; i<acq_entry->shared_list_of_plmns.num_of_shared_plmns; i++)
    {
      bcch_plmn = acq_entry->shared_list_of_plmns.shared_plmns_access_info[i].plmn_id;
      if(rrccsp_check_for_eplmn(
          mmumts_convert_rrc_plmn_id_to_nas_plmn_id(bcch_plmn)) == TRUE)
      {
        WRRC_MSG2_HIGH("NWS: PLMN %d-%d is in the eq PLMN list",
                         RRC_CSP_GET_INT_MCC(bcch_plmn),
                         RRC_CSP_GET_INT_MNC(bcch_plmn));
        return TRUE;
      }
    }
  }
  WRRC_MSG0_HIGH("NWS: No BCCH PLMN is in the NAS equivalent PLMN list");
  return FALSE;
}

/*===========================================================================

FUNCTION          RRCCSP_NAS_PLMN_MATCH_ANY_OF_BCCH_PLMNS

DESCRIPTION       Finds out whether the NAS requested PLMN and any the of the shared bcch PLMNs are same.

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                rrc_plmn_identity_type nas_plmn, 
                                                uint8 index_in_acq_or_shared_db)
{
  rrc_plmn_identity_type bcch_plmn;
  rrc_csp_acq_entry_type acq_entry = acq_list[index_in_acq_or_shared_db];
  int i;

  if(acq_entry.is_valid_nw_sharing)
  {
    for(i=0; i<acq_entry.shared_list_of_plmns.num_of_shared_plmns; i++)
    {
      bcch_plmn = acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id;
      if(rrc_bcch_and_nas_plmns_match(bcch_plmn, nas_plmn))
      {
        return TRUE;
      }
    }
  }
  WRRC_MSG0_HIGH("NWS: No BCCH PLMN match the NAS requested PLMN");
  return FALSE;
}

/*===========================================================================

FUNCTION          RRCCSP_IS_ANY_OF_BCCH_PLMNS_IN_BPLMN_LIST

DESCRIPTION       Finds if any one of the shared PLMNs are in the list of NAS requested PLMNs. Note that this is 
                  used for early abort of SIB reading. If none of the shared bcch PLMNs are in the NAS requested 
                  PLMN list then SIB reading is avoided immediately after reading the MIB.

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_is_any_of_bcch_plmns_in_bplmn_list(rrc_csp_nw_sharing_entry_type bplmn_nw_db_entry)
{
uint32  i;
uint32  j;
  rrc_plmn_identity_type      plmn_id;

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    for(i = 0; i < rrc_csp_int_data.bplmn_num_req; ++i)
    {
      for(j=0; j<bplmn_nw_db_entry.num_of_shared_plmns; j++)
      {
        plmn_id = bplmn_nw_db_entry.shared_plmns_access_info[j].plmn_id;
        if(rrc_bcch_and_nas_plmns_match(plmn_id, rrc_csp_int_data.bplmn_req_list[i].plmn_id))
        {
          return TRUE;
        }
      }
    }   
  }
  else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
  {
    for(i = 0; i < rrc_csp_int_data.wtow_bplmn_num_req; ++i)
    {
      for(j=0; j<bplmn_nw_db_entry.num_of_shared_plmns; j++)
      {
        plmn_id = bplmn_nw_db_entry.shared_plmns_access_info[j].plmn_id;
        if(rrc_bcch_and_nas_plmns_match(plmn_id, rrc_csp_int_data.wtow_bplmn_req_list[i].plmn_id))
        {
          return TRUE;
        }
      }
    }   
  }
  return FALSE;
}

/*===========================================================================

FUNCTION           RRCCSP_ADD_FREQ_BANDS_FROM_ACQ_DB_TO_BAND_LIST

DESCRIPTION        Extracts frequency bands from ACQ DB in LIFO order and adds them
                  to the band_list provided starting from num_of_bands_already_present.

DEPENDENCIES       num_of_bands_already_present should be initialized properly before calling
                   this function.

RETURN VALUE       number of bands present in the band list at the end.

SIDE EFFECTS       None
===========================================================================*/
uint8 
rrccsp_add_freq_bands_from_acq_db_to_band_list
(
  rrc_csp_band_class_type*    band_list,
  uint8 num_of_bands_already_present
)
{
  int    i;
  int    num_valid_entries;

  /* Do a Sanity check - check read and write index. A serious error if one
   * of them is out of bounds 
   */
  if ((stored_acq_db.curr_wr_index > MAX_ACQ_DB_INDEX) || 
      (stored_acq_db.curr_rd_index > MAX_ACQ_DB_INDEX) ||
      (stored_acq_db.curr_rd_index < -1) ||
      (stored_acq_db.curr_rd_index > stored_acq_db.curr_wr_index))
  {
    WRRC_MSG2_ERROR("ERROR: Acq db invalid Wr index %d Rd index %d",
                stored_acq_db.curr_wr_index, stored_acq_db.curr_rd_index);
    return num_of_bands_already_present;
  }

  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) && 
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db.curr_wr_index + 1;
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    num_valid_entries = stored_acq_db.curr_wr_index;
  }

  WRRC_MSG0_HIGH("WBP: Adding frequency bands from ACQ DB");

  for (i = 0; ((i < num_valid_entries) && (num_of_bands_already_present < MAX_NUM_OF_WCDMA_BANDS)); i++)
  {
    if((rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class) &&
        !rrccsp_is_band_already_added_to_prioritized_list(acq_list[stored_acq_db.acq_list_indices[i]].band_class,
                                                          num_of_bands_already_present)))
    {
      WRRC_MSG1_HIGH("WBP: Adding ACQ DB band %d to prioritized band list",acq_list[stored_acq_db.acq_list_indices[i]].band_class);
      band_list[num_of_bands_already_present++] = acq_list[stored_acq_db.acq_list_indices[i]].band_class;
    }
  }

  return num_of_bands_already_present;
}

/*===========================================================================

FUNCTION          RRC_CSP_LIST_DB_EXHAUSTED

DESCRIPTION    This function resets lis db after scanning thro' all the entries in it.
DEPENDENCIES
                  None

RETURN VALUE
SIDE EFFECTS
                  None.

===========================================================================*/
static void rrc_csp_list_db_exhausted(void)
{
    /* Invalidate the number of frequencies in LFS table*/
    rrc_csp_curr_select_data.scan_info.list_scan.num_freq = 0;
    
    rrc_csp_curr_select_data.acq_db_in_use=FALSE;
    
    /* Invalidate the indices scanned_index and curr_rd_index*/
     rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index = -1;

    /* Freq Scan Initialization */
    WRRC_MSG0_HIGH("LFS_COOS: Exhausted list scan db. Initialization done for RAW/FINE frequency scan ");
    rrc_csp_init_freq_scan_data();
    rrc_csp_init_addl_chnl_scan_data();
    return;
}

/*===========================================================================

FUNCTION          RRC_CSP_GET_LIST_DB_MATCH_ACQ_ENTRY

DESCRIPTION    This function will match the frequency and PLMN id of the entry received from L1 as part of the 
                         frequency scan confirmation against the ACQ DB entries. If the match is successful  pointer to the acq list
                         entry will be returned to the called function otherwise NULL will be returned. GSM scan will be initiated if the 
                         List DB exhausts and the criteria for GSM Scan satisifies..

DEPENDENCIES
                  None

RETURN VALUE
                  Pointer to acquisition entry. A NULL pointer means no 
                  acquisition entry was found that fulfilled selection criteria.

SIDE EFFECTS
                  None.

===========================================================================*/
rrc_csp_acq_entry_type*  
rrc_csp_get_list_db_match_acq_entry(
  rrc_csp_freq_scan_e_type *ret_type_freq_scan
  )
{
  int i;
  uint16 freq;
  int16 curr_freq_index;
  rrc_plmn_identity_type plmn_id;
  /*Initialize both the scanned and curr_rd indices before finding a matching
     ACQ entry from the list db
  */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
  {
    if((rrc_csp_int_data.num_freq_in_list_db >0) &&
     (rrc_csp_int_data.curr_rd_idx < rrc_csp_int_data.num_freq_in_list_db))
    {
      WRRC_MSG2_HIGH("LFS: starting acq on freq %d, curr_rd_ix %d", 
                                        list_db[rrc_csp_int_data.curr_rd_idx].freq, 
                                        rrc_csp_int_data.curr_rd_idx);
      return(&list_db[rrc_csp_int_data.curr_rd_idx++]);
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    /*Recovery code. freq_index never expected to be less than -1*/
    if(rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index <= -1)
    {
       rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index = 0;
    }

    curr_freq_index = rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index; 

    while((curr_freq_index < rrc_csp_curr_select_data.scan_info.list_scan.num_freq)&&
          (curr_freq_index < L1_MAX_SCAN_FREQ))
    {
      plmn_id = rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[curr_freq_index];
      freq = rrc_csp_curr_select_data.scan_info.list_scan.freq_result[curr_freq_index].ctr_freq;

      /* Get the matching frequency from the current read index */
      WRRC_MSG2_HIGH("LFS_COOS: curr_freq_index = [%d]--->freq=[%d]",curr_freq_index,freq);
      /* increment the scanned index and current read index will point to the next frequency scan entry */
      rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index++;
  
      /*Get the matching acquisition entry from the Acq DB*/
#ifdef FEATURE_FEMTO_CSG
  if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
     (rrccsp_get_curr_nas_requested_csg_id() != SYS_CSG_ID_INVALID)&&
     (!rrccsp_is_csg_white(rrccsp_get_curr_nas_requested_csg_id(),rrccsp_get_curr_nas_requested_plmn())))
  {
      for(i=0;i<MAX_ACQ_DB_ENTRIES_TEMP;i++)
      {
        if( (freq  == acq_list_temp[i].freq) &&
             (RRC_CSP_SAME_MCC(plmn_id, acq_list_temp[i].plmn_id)) &&
             (RRC_CSP_SAME_MNC(plmn_id, acq_list_temp[i].plmn_id))
          )
        {
          /* update the acq_db_freq_scanned_list with the matching freq 
               from the acq entry
          */
          WRRC_MSG1_HIGH("LFS_COOS: Matching acq entry. update the freq=%d into the scanned list",freq);
          rrc_csp_update_acq_db_freq_scanned_list(freq);
          WRRC_MSG1_HIGH("LFS_COOS: new freq idx=%d ",
                   rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index);
          /* This is the acquisition entry with which CSP will try for acquisition */
          return &acq_list_temp[i];  
        }
      }
  }
  else
#endif
  {
      for(i=0;i<MAX_ACQ_DB_SIZE;i++)
      {
        if( (freq  == acq_list[i].freq) &&
             (RRC_CSP_SAME_MCC(plmn_id, acq_list[i].plmn_id)) &&
             (RRC_CSP_SAME_MNC(plmn_id, acq_list[i].plmn_id))
          )
        {
          /* update the acq_db_freq_scanned_list with the matching freq 
               from the acq entry
          */
          WRRC_MSG1_HIGH("LFS_COOS: Matching acq entry. update the freq=%d into the scanned list",freq);
          rrc_csp_update_acq_db_freq_scanned_list(freq);
          WRRC_MSG1_HIGH("LFS_COOS: new freq idx=%d ",
                   rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index);
          /* This is the acquisition entry with which CSP will try for acquisition */
          return &acq_list[i];  
        }
      }
  }
      WRRC_MSG0_HIGH("LFS_COOS: No matching acq-DB entry. Pick the next freq.");
      curr_freq_index++;
    }
    /*If it comes here, then all list db scan is over*/
    rrc_csp_list_db_exhausted();
  }

  return NULL;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_MATCH_ACQ_ENTRIES_UPDATE_LIST_DB

DESCRIPTION       This function is used by CSP to run through the acq DB for specific/any PLMN based on the current
                           acquisition type and if a suitable entry is found  then the frequency, PLMN Id will be stored in the 
                           current CSP database

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_update_match_acq_entries_into_list_db(void)
{
  rrc_plmn_identity_type req_plmn_id;
  int num_valid_entries;
  int i=0;
  int iterator = 0;
  boolean is_freq_add_into_lfs_list = TRUE;
  boolean is_eplmn = FALSE;
  rrc_csp_acq_entry_type  *ret_acq_entry_ptr = NULL;
  boolean is_network_shared = FALSE;
  boolean is_acq_db_plmn_matched = FALSE;
  boolean scan_all_acq_db = FALSE;
#ifdef FEATURE_FEMTO_CSG
  uint32 req_csg_id = rrc_csp_int_data.curr_req_plmn.csg_id;
#endif
 
  req_plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

  /*Invalidate the curr and scanned indices */
  rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index = -1;

  /* Initialize the number of frequencies in the list scan table */
  rrc_csp_curr_select_data.scan_info.list_scan.num_freq = 0;

  acq_db_freq_scanned_list.wr_index = 0;

  rrc_csp_int_data.acq_db_empty = TRUE;

  WRRC_MSG0_HIGH("LFS_COOS: LIST DB UPDATE ");

  /* Do a Sanity check - check read and write index. A serious error if one
    * of them is out of bounds  Also read index should never be higher than 
    * write index.
  */
  ASSERT(stored_acq_db.curr_wr_index <= MAX_ACQ_DB_INDEX);
  ASSERT(stored_acq_db.curr_rd_index <= MAX_ACQ_DB_INDEX);
  ASSERT(stored_acq_db.curr_rd_index >= -1);
  ASSERT(stored_acq_db.curr_rd_index <= stored_acq_db.curr_wr_index);

#ifdef FEATURE_FEMTO_CSG
if((req_csg_id != SYS_CSG_ID_INVALID)&&(!(rrccsp_is_csg_white(req_csg_id,req_plmn_id))))
{
  WRRC_MSG1_HIGH("CSG: Checking entry from temp db. CSG ID: %d",req_csg_id);
  ASSERT(stored_acq_db_temp.curr_wr_index <= MAX_ACQ_DB_INDEX_TEMP);
  ASSERT(stored_acq_db_temp.curr_rd_index <= MAX_ACQ_DB_INDEX_TEMP);
  ASSERT(stored_acq_db_temp.curr_rd_index >= -1);
  ASSERT(stored_acq_db_temp.curr_rd_index <= stored_acq_db_temp.curr_wr_index);

    /* Compute Number of Valid entries */
  if ((stored_acq_db_temp.curr_wr_index == MAX_ACQ_DB_INDEX_TEMP) && 
      (stored_acq_db_temp.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db_temp.curr_wr_index + 1;
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
    * to a pointer entry which is Invalid and still needs to be initialized.
    */
    num_valid_entries = stored_acq_db_temp.curr_wr_index;
  }

    /* Loop through the temp ACQ DB and store the matching entries into List DB */
  for (i = 0; i < num_valid_entries; i++)
  {
    if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq >= L1_MAX_SCAN_REQ)
    {
       WRRC_MSG0_HIGH("CSG: LIST DB is full. Do not add any more frequencies");
       break;
    }

#ifdef FEATURE_WCDMA_MULTIBAND
    /* Collect band information for frequencies present in ACQ DB */
    if((rrc_csp_int_data.ue_in_deep_sleep) && 
        (rrccsp_is_supported_band(acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].band_class)))
    {
      rrc_csp_int_data.deep_sleep_acq_db_band_mask |= 
                                             acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].band_class;
      WRRC_MSG1_HIGH("LFS_COOS:deep_sleep_acq_db_band_mask=%x",rrc_csp_int_data.deep_sleep_acq_db_band_mask );
    }
#endif
    is_network_shared = acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].is_valid_nw_sharing;

    if(rrc_csp_curr_select_data.acq_type == RRC_CSP_SPEC_PLMN)
    {
          /* Resetting the is_eplmn variable at start of loop */
          is_eplmn = FALSE;
  
          is_acq_db_plmn_matched = FALSE;
          if((!is_network_shared) &&
             (rrc_bcch_and_nas_plmns_match(acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].plmn_id, 
                                           req_plmn_id))&&
             (acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].csg_id == req_csg_id))
          {
            is_acq_db_plmn_matched = TRUE;
          }
          else if((is_network_shared) &&
                  (rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                       req_plmn_id,
                                                       stored_acq_db_temp.acq_list_indices[i]))&&
                  (acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].csg_id == req_csg_id))
          {
            is_acq_db_plmn_matched = TRUE;
          }
  
    }
    if((is_acq_db_plmn_matched
        ) && ((!rrccsp_should_freq_be_skipped(
            acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].freq, RRCCSP_FINE_SCAN_SKIP_RANGE))
            && (rrccsp_is_supported_band(acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].band_class))
            && (!rrc_csp_has_acq_db_freq_been_scanned(
                    acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].freq))))
    {
      /* This check is added to avoid the duplicate frequencies in to the lfs data*/
      for (iterator =0;iterator<rrc_csp_curr_select_data.scan_info.list_scan.num_freq;iterator++)
      {
        if (acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].freq == 
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[iterator].ctr_freq
           )
        {
          is_freq_add_into_lfs_list = FALSE;
        }
      }
      if(is_freq_add_into_lfs_list)
      {
        /* A suitable entry is found. Set the Read index to this db pointer entry and 
         * return pointer to the acquisition list contained in this db pointer entry
         */
        stored_acq_db_temp.curr_rd_index = (int8)i;
        ret_acq_entry_ptr = &acq_list_temp[stored_acq_db_temp.acq_list_indices[i]];

        /* Store the frequency*/
        rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].ctr_freq=
                                          ret_acq_entry_ptr->freq;

        /* Store plmn type based on the current acq type*/
        rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].result_plmn_type = (uint8) RRC_CSP_SPEC_PLMN;

        rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[rrc_csp_curr_select_data.scan_info.list_scan.num_freq] = ret_acq_entry_ptr->plmn_id;			
        MSG_HIGH("LFS_COOS: Added an entry in lfs table, freq=%d, plmn type = %d, num_freq=%d ",
               rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].ctr_freq,
               rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].result_plmn_type,
               rrc_csp_curr_select_data.scan_info.list_scan.num_freq + 1
                );
        
#ifdef FEATURE_WCDMA_MULTIBAND
        if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq == 0)
        {
          if(rrc_csp_curr_select_data.num_of_prioritized_bands == 0)
          {
            rrc_csp_curr_select_data.first_band_to_be_scanned = ret_acq_entry_ptr->band_class;
            WRRC_MSG2_HIGH("LFS_COOS: Setting 1st_band to %d acq_db_empty %d",
                                      ret_acq_entry_ptr->band_class,rrc_csp_int_data.acq_db_empty);
          }
        }
#endif /* FEATURE_WCDMA_MULTIBAND */

        /* Increment the number of frequencies */
        rrc_csp_curr_select_data.scan_info.list_scan.num_freq++;
        if(is_acq_db_plmn_matched)
        {
          rrc_csp_int_data.acq_db_empty = FALSE;
        }
      }
      else
      {
        WRRC_MSG1_HIGH("LFS_COOS: frequency =%d can be skipped",
                 acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].freq);
        is_freq_add_into_lfs_list = TRUE;
      }
    }
  }
  WRRC_MSG1_HIGH("LFS_COOS: number of freqs added into lfs data = %d",
           rrc_csp_curr_select_data.scan_info.list_scan.num_freq);

}
else
#endif
{
  /* Compute Number of Valid entries */
  if ((stored_acq_db.curr_wr_index == MAX_ACQ_DB_INDEX) && 
      (stored_acq_db.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db.curr_wr_index + 1;
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
    * to a pointer entry which is Invalid and still needs to be initialized.
    */
    num_valid_entries = stored_acq_db.curr_wr_index;
  }
  /* MM has requested to look for specific PLMN Cell Selection Procedure 
      needs this information while finding matching acq entry from the ACQ
      DB */
  req_plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

  WRRC_MSG2_HIGH("LFS_COOS: no of valid entries =%d, curr_rd_index=%d",
           num_valid_entries,stored_acq_db.curr_rd_index);

  if((rrc_csp_curr_select_data.acq_type == RRC_CSP_ANY_PLMN)
      || (rrc_csp_should_all_acq_db_be_scanned()))
  {
#ifdef FEATURE_FEMTO_CSG
    if(req_csg_id == SYS_CSG_ID_INVALID)
#endif
    scan_all_acq_db = TRUE;
  }


  /* Loop through the ACQ DB and store the matching entries into List DB */
  for (i = 0; i < num_valid_entries; i++)
  {
#ifdef FEATURE_FEMTO_CSG
    if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq >= L1_MAX_SCAN_REQ)
    {
       WRRC_MSG0_HIGH("CSG: LIST DB is full. Do not add any more frequencies");
       break;
    }
#endif

#ifdef FEATURE_WCDMA_MULTIBAND
    /* Collect band information for frequencies present in ACQ DB */
    if((rrc_csp_int_data.ue_in_deep_sleep) && 
        (rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class)))
    {
      rrc_csp_int_data.deep_sleep_acq_db_band_mask |= 
                                             acq_list[stored_acq_db.acq_list_indices[i]].band_class;
      WRRC_MSG1_HIGH("LFS_COOS:deep_sleep_acq_db_band_mask=%x",rrc_csp_int_data.deep_sleep_acq_db_band_mask );
    }
#endif
    is_network_shared = acq_list[stored_acq_db.acq_list_indices[i]].is_valid_nw_sharing;

    if(rrc_csp_curr_select_data.acq_type == RRC_CSP_SPEC_PLMN)
    {
          /* Resetting the is_eplmn variable at start of loop */
          is_eplmn = FALSE;
          if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
          {
            if((rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                      rrc_csp_int_data.curr_req_plmn.plmn_id))) == TRUE)
            {
              WRRC_MSG0_HIGH("Curr requested PLMN is in the NAS equivalent PLMN list");
              if((!is_network_shared) &&
                 (rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                      acq_list[stored_acq_db.acq_list_indices[i]].plmn_id)) == TRUE))
              {
                WRRC_MSG2_HIGH("PLMN %d-%d is in the eq PLMN list",
                           RRC_CSP_GET_INT_MCC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id),
                           RRC_CSP_GET_INT_MNC(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id));
                is_eplmn = TRUE;
              }
              else if((is_network_shared) && 
                 ((rrccsp_nas_plmn_equivalent_to_any_of_shared_bcch_plmns(
                                                                          rrc_csp_int_data.curr_req_plmn.plmn_id,
                                                                          stored_acq_db.acq_list_indices[i])) == TRUE))
              {
                is_eplmn = TRUE;
              }
            }
          }
  
          is_acq_db_plmn_matched = FALSE;
          if((!is_network_shared) &&
             (rrc_bcch_and_nas_plmns_match(acq_list[stored_acq_db.acq_list_indices[i]].plmn_id, 
                                           req_plmn_id)))
          {
            is_acq_db_plmn_matched = TRUE;
#ifdef FEATURE_FEMTO_CSG
            if((req_csg_id != SYS_CSG_ID_INVALID)&&(!(acq_list[stored_acq_db.acq_list_indices[i]].csg_id == req_csg_id)))
            {
              is_acq_db_plmn_matched = FALSE;
            }
#endif
          }
          else if((is_network_shared) &&
                  (rrccsp_nas_plmn_match_any_of_bcch_plmns(
                                                       req_plmn_id,
                                                       stored_acq_db.acq_list_indices[i])))
          {
            is_acq_db_plmn_matched = TRUE;
#ifdef FEATURE_FEMTO_CSG
            if((req_csg_id != SYS_CSG_ID_INVALID)&&(!(acq_list[stored_acq_db.acq_list_indices[i]].csg_id == req_csg_id)))
            {
              is_acq_db_plmn_matched = FALSE;
            }
#endif
          }
  
    }
    if((scan_all_acq_db || is_acq_db_plmn_matched
              || (is_eplmn)
        ) && ((!rrccsp_should_freq_be_skipped(
            acq_list[stored_acq_db.acq_list_indices[i]].freq, RRCCSP_FINE_SCAN_SKIP_RANGE))
            && (rrccsp_is_supported_band(acq_list[stored_acq_db.acq_list_indices[i]].band_class))
            && (!rrc_csp_has_acq_db_freq_been_scanned(
                    acq_list[stored_acq_db.acq_list_indices[i]].freq))))
    {
      /* This check is added to avoid the duplicate frequencies in to the lfs data*/
      for (iterator =0;iterator<rrc_csp_curr_select_data.scan_info.list_scan.num_freq;iterator++)
      {
        if (acq_list[stored_acq_db.acq_list_indices[i]].freq == 
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[iterator].ctr_freq
           )
        {
          is_freq_add_into_lfs_list = FALSE;
        }
      }
      if(is_freq_add_into_lfs_list)
      {
        /* A suitable entry is found. Set the Read index to this db pointer entry and 
         * return pointer to the acquisition list contained in this db pointer entry
         */
        stored_acq_db.curr_rd_index = (int8)i;
        ret_acq_entry_ptr = &acq_list[stored_acq_db.acq_list_indices[i]];

        /* Store the frequency*/
        rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].ctr_freq=
                                          ret_acq_entry_ptr->freq;

        /* Store plmn type based on the current acq type*/
        if((is_acq_db_plmn_matched)
                      || is_eplmn
                   )
        {
          rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].result_plmn_type = (uint8) RRC_CSP_SPEC_PLMN;
        }
        else
        {
          rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].result_plmn_type = (uint8) RRC_CSP_ANY_PLMN;
        }

        rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[rrc_csp_curr_select_data.scan_info.list_scan.num_freq] = ret_acq_entry_ptr->plmn_id;			
        MSG_HIGH("LFS_COOS: Added an entry in lfs table, freq=%d, plmn type = %d, num_freq=%d ",
               rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].ctr_freq,
               rrc_csp_curr_select_data.scan_info.list_scan.freq_result[rrc_csp_curr_select_data.scan_info.list_scan.num_freq].result_plmn_type,
               rrc_csp_curr_select_data.scan_info.list_scan.num_freq + 1
                );
        
#ifdef FEATURE_WCDMA_MULTIBAND
        if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq == 0)
        {
          if(rrc_csp_curr_select_data.num_of_prioritized_bands == 0)
          {
            rrc_csp_curr_select_data.first_band_to_be_scanned = ret_acq_entry_ptr->band_class;
            WRRC_MSG2_HIGH("LFS_COOS: Setting 1st_band to %d acq_db_empty %d",
                                      ret_acq_entry_ptr->band_class,rrc_csp_int_data.acq_db_empty);
          }
        }
#endif /* FEATURE_WCDMA_MULTIBAND */

        /* Increment the number of frequencies */
        rrc_csp_curr_select_data.scan_info.list_scan.num_freq++;
        if(is_acq_db_plmn_matched
             || (is_eplmn)
             || (rrc_csp_curr_select_data.acq_type == RRC_CSP_ANY_PLMN))
        {
          rrc_csp_int_data.acq_db_empty = FALSE;
        }
      }
      else
      {
        WRRC_MSG1_HIGH("LFS_COOS: frequency =%d can be skipped",
                 acq_list[stored_acq_db.acq_list_indices[i]].freq);
        is_freq_add_into_lfs_list = TRUE;
      }
    }
  }
}
  WRRC_MSG1_HIGH("LFS_COOS: number of freqs added into lfs data = %d",
           rrc_csp_curr_select_data.scan_info.list_scan.num_freq);

#ifdef FEATURE_WCDMA_MULTIBAND
  if(ret_acq_entry_ptr == NULL && rrc_csp_curr_select_data.scan_info.list_scan.num_freq == 0)
  {
    WRRC_MSG0_HIGH("LFS_COOS: No freq is found in the ACQ DB search for list scan");
    rrc_csp_curr_select_data.current_band = 
            rrc_csp_curr_select_data.first_band_to_be_scanned;
  }
#endif /* FEATURE_WCDMA_MULTIBAND */
}

/*===========================================================================

FUNCTION           WCDMA_RRC_GET_ACQDB_CELL_LIST

DESCRIPTION        Fills the passed LTE ptr with valid macro cell entries stored in W AcqDB.

DEPENDENCIES       None

RETURN VALUE       void

SIDE EFFECTS       None
===========================================================================*/
void wcdma_rrc_get_acqdb_cell_list(wcdma_acqdb_cell_list  *wcdma_acqdb_cell_list_ptr)
{
  int i;
  wcdma_acqdb_cell_list_ptr->num_valid_entries = 0;

  for(i=0; (i<(stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid)) && (wcdma_acqdb_cell_list_ptr->num_valid_entries < MAX_ACQ_DB_MACRO_ENTRIES); i++)
  {
#ifdef FEATURE_FEMTO_CSG
    /* Skip CSG cells */
    if(acq_list[stored_acq_db.acq_list_indices[i]].csg_id == SYS_CSG_ID_INVALID)
#endif
    {
       wcdma_acqdb_cell_list_ptr->cellInfoList[wcdma_acqdb_cell_list_ptr->num_valid_entries].freq= acq_list[stored_acq_db.acq_list_indices[i]].freq;
       wcdma_acqdb_cell_list_ptr->cellInfoList[wcdma_acqdb_cell_list_ptr->num_valid_entries].nas_plmn_id =   mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                                                               acq_list[stored_acq_db.acq_list_indices[i]].plmn_id);
       wcdma_acqdb_cell_list_ptr->num_valid_entries++;
    }
  }
}

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION           RRCCSP_ADD_ACQ_DB_AND_NW_DB_ENTRIES_TEMP

DESCRIPTION        Adds an entry into the NW sharing DB at the given index Note that one-one mapping
                   is maintained between ACQ DB and NW sharing DB.

DEPENDENCIES       None

RETURN VALUE       Success/Failure. Failure will be an extra-ordinary event and
                   could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS       None
===========================================================================*/
uecomdef_status_e_type rrccsp_add_acq_db_and_nw_db_entries_temp(
                                                           rrc_csp_acq_entry_type* acq_entry_ptr,
                                                           rrc_csp_nw_sharing_entry_type* nw_sharing_entry_ptr)
{
  uint8 index = MAX_ACQ_DB_INDEX_TEMP;
  uecomdef_status_e_type status;
  rrc_plmn_identity_type chosen_plmn = acq_entry_ptr->plmn_id;

  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    WRRC_MSG0_HIGH("NWS: Current cell is shared, Adding shared plmns");
    acq_entry_ptr->plmn_id = nw_sharing_entry_ptr->common_plmn_id;
    acq_entry_ptr->shared_list_of_plmns = *nw_sharing_entry_ptr;
  }
  else
  {
    WRRC_MSG0_HIGH("NWS: Current cell is not shared, not adding shared plmns");
  }

  if(rrc_csp_add_acq_entry_temp(acq_entry_ptr) == SUCCESS)
  {
    status = SUCCESS;
    acq_entry_ptr->plmn_id = chosen_plmn;
  }
  else
  {
    status = FAILURE;
    WRRC_MSG2_HIGH("NWS: Index in ACQ DB: %d, status: %d",index,status);
  }

  return (status);
}

/*===========================================================================

FUNCTION          RRC_CSP_ADD_ACQ_ENTRY_TEMP

DESCRIPTION       This function adds an acquisition entry to the acquisition 
                  list. While doing so, it also updates the acquisition
                  database pointers so that the 1st pointer points to this
                  most recent acquisition entry in acquisition list.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  rrc_csp_add_acq_entry_temp(rrc_csp_acq_entry_type* acq_entry_ptr)
{

  int   wr_acq_list_index;
  int   i;
  int   ptr_index;
  int   num_valid_entries;
  rrc_csp_band_class_type band_class;
  

  /* Do a Sanity check - check read and write index. A serious error if one
   * of them is out of bounds 
   */
  if ((stored_acq_db_temp.curr_wr_index > MAX_ACQ_DB_INDEX_TEMP) || 
      (stored_acq_db_temp.curr_rd_index > MAX_ACQ_DB_INDEX_TEMP) ||
      (stored_acq_db_temp.curr_rd_index < -1) ||
      (stored_acq_db_temp.curr_rd_index > stored_acq_db_temp.curr_wr_index))
  {
    WRRC_MSG2_ERROR("ERROR: Acq db invalid Wr index %d Rd index %d",
                stored_acq_db_temp.curr_wr_index, stored_acq_db_temp.curr_rd_index);
    return(FAILURE);
  }

  /* Compute Number of Valid entries */
  if ((stored_acq_db_temp.curr_wr_index == MAX_ACQ_DB_INDEX_TEMP) && 
      (stored_acq_db_temp.last_entry_invalid == FALSE))
  {
    /* If all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is the last one and is Valid.
     */
    num_valid_entries = stored_acq_db_temp.curr_wr_index + 1;
  }
  else 
  {
    /* If not all pointers in acq. dbase are filled up, the write index points
     * to a pointer entry which is Invalid and still needs to be initialized.
     */
    num_valid_entries = stored_acq_db_temp.curr_wr_index;
  }

  /* Get a matching acq. entry in acq. list that matches the PLMN and freq with 
   * the entry passed as a parameter.
   */
  for (i = 0; i < num_valid_entries; i++)
  {
    if ((rrc_bcch_plmns_match(acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].plmn_id, 
                              acq_entry_ptr->plmn_id)) &&
        (acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].freq == acq_entry_ptr->freq)&&
        (acq_list_temp[stored_acq_db_temp.acq_list_indices[i]].csg_id == acq_entry_ptr->csg_id))
    {
      /* Here, we found an acq. entry with same PLMN and freq. This acq. entry 
       * is overwritten with the acq. data provided in the parameter. All acq.
       * database pointers, starting from the first (topmost) and going till the
       * acq. database pointer that pointed to this dup entry, are moved one 
       * downwards, freeing the topmost pointer which is made to point to this
       * previously-dup-but-now-updated acq. entry. Note that in this process, the
       * acq. database pointer pointing to dup entry is not moved downwards and is
       * overwritten by the pointer prior to it.
       * Since the total number of acq. entries has not changed, the write index
       * is unaffected.
       */
      wr_acq_list_index = stored_acq_db_temp.acq_list_indices[i];

      for (ptr_index = i; ptr_index > 0; ptr_index--)
      {
        stored_acq_db_temp.acq_list_indices[ptr_index] = 
          stored_acq_db_temp.acq_list_indices[ptr_index-1];
      }

      acq_list_temp[wr_acq_list_index] = *acq_entry_ptr;
      band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
      acq_list_temp[wr_acq_list_index].band_class = band_class;

      /* Initialize the topmost pointer to the most recent acq. list entry */
      stored_acq_db_temp.acq_list_indices[0] = (uint8)wr_acq_list_index;
      /* Return always a Success for the time being */
      MSG_HIGH("Updated: freq %d scr_code %d band %d in Acq DB", 
                              acq_entry_ptr->freq, acq_entry_ptr->scr_code, band_class);

      return (SUCCESS);
    }
  }


  /* Initialize write pointer to the acquisition entry in acquisition list where
   * the new acq. data can be stored. If all entries in acq. list are already
   * filled, the oldest entry as pointed to by the "last" pointer in acq. database
   * is to be overwritten and to be made the most recent entry.
   */

  if ((stored_acq_db_temp.curr_wr_index == MAX_ACQ_DB_INDEX_TEMP) &&
      (stored_acq_db_temp.last_entry_invalid == FALSE))
  {
    wr_acq_list_index = stored_acq_db_temp.acq_list_indices[stored_acq_db.curr_wr_index];
  }
  else
  {
    wr_acq_list_index = stored_acq_db_temp.curr_wr_index;
  }

  if((wr_acq_list_index > MAX_ACQ_DB_INDEX_TEMP) || (wr_acq_list_index < 0))
  {
    WRRC_MSG0_ERROR("Invalid write index.");
    return(FAILURE);
  }

  /* Copy the contents now */
  acq_list_temp[wr_acq_list_index] = *acq_entry_ptr;

  band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
  acq_list_temp[wr_acq_list_index].band_class = band_class;

  /* Now push all pointers in acq. database downwards and copy the pointer to 
   * this acq. entry in the first (topmost) acq. database pointer.
   */
  for (i = stored_acq_db_temp.curr_wr_index; i > 0; i--)
  {
    stored_acq_db_temp.acq_list_indices[i] = 
      stored_acq_db_temp.acq_list_indices[i-1];
  }
  /* Initialize the topmost pointer to the most recent acq. list entry */
  stored_acq_db_temp.acq_list_indices[0] = (uint8)wr_acq_list_index;

  /* Increment current number of valid acquisition database pointers if 
   * not all acquisition db pointers have been initialized so far.
   */
  if (stored_acq_db_temp.curr_wr_index < MAX_ACQ_DB_INDEX_TEMP)
  {
    stored_acq_db_temp.curr_wr_index++;
    stored_acq_db_temp.last_entry_invalid = TRUE;
  }
  else
  {
    stored_acq_db_temp.last_entry_invalid = FALSE;
  }

  MSG_HIGH("Added: freq %d scr_code %d band %d in Acq DB", 
                              acq_entry_ptr->freq, acq_entry_ptr->scr_code, band_class);
  /* Return always a Success for the time being */
  return (SUCCESS);

}  /* rrc_csp_add_acq_entry_temp */


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
boolean rrccsp_fp_macro_present_in_acq_db(rrc_csp_acq_entry_type acq_db_entry)
{

  uint8 j;
  rrc_plmn_identity_type macro_plmn;
  uint32 macro_freq;
  uint16 macro_psc;
  uint32 macro_cell_id;
  boolean is_lte = FALSE;

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    macro_plmn = rrc_csp_int_data.lte_macro_asf.plmn;
    macro_freq = rrc_csp_int_data.lte_macro_asf.freq;
    macro_psc = rrc_csp_int_data.lte_macro_asf.pci;
    macro_cell_id = rrc_csp_int_data.lte_macro_asf.cell_id;
    is_lte = TRUE;
  }
  else
  {
    macro_plmn=rrc_ccm_get_curr_camped_plmn();
    macro_freq = rrc_ccm_get_curr_camped_freq();
    macro_psc= rrc_ccm_get_curr_camped_psc();
    macro_cell_id= rrc_ccm_get_curr_camped_cell_id();
  }

  if(is_lte)
  {
    for(j= 0; j< acq_db_entry.fp_lte_macro_cnt ;j++)
    {
      if(rrc_bcch_plmns_match(acq_db_entry.lte_macro[j].plmn_id,macro_plmn)
     && (acq_db_entry.lte_macro[j].freq == macro_freq)
     && (acq_db_entry.lte_macro[j].pci == macro_psc)
     && (acq_db_entry.lte_macro[j].cell_id == macro_cell_id))
      {
        WRRC_MSG0_HIGH("CSG: FP: Found a matching LTE FP macro info");
        return TRUE;
      }
    }
  }
  else
  { 
    for(j= 0; j< acq_db_entry.fp_w_macro_cnt ;j++)
    {
      if(rrc_bcch_plmns_match(acq_db_entry.w_macro[j].plmn_id,macro_plmn)
         && (acq_db_entry.w_macro[j].freq == macro_freq)
         && (acq_db_entry.w_macro[j].psc == macro_psc)
         && (acq_db_entry.w_macro[j].cell_id == macro_cell_id))
      {
        WRRC_MSG0_HIGH("CSG: FP: Found a matching WCDMA FP macro info");
        return TRUE; 
      }
    }
  }
  WRRC_MSG0_HIGH("CSG: FP: Didnt find any FP info");
  return FALSE;

}

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
void rrccsp_prune_acqdb_after_acq(boolean acq_status)
{
  uint8 i,j,k;
  rrc_plmn_identity_type macro_plmn;
  uint32 macro_freq;
  uint16 macro_psc;
  uint32 macro_cell_id;
  boolean is_lte = FALSE;
  boolean match_found = FALSE;

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    macro_plmn = rrc_csp_int_data.lte_macro_asf.plmn;
    macro_freq = rrc_csp_int_data.lte_macro_asf.freq;
    macro_psc = rrc_csp_int_data.lte_macro_asf.pci;
    macro_cell_id = rrc_csp_int_data.lte_macro_asf.cell_id;
    is_lte = TRUE;
  }
  else
  {
    macro_plmn = rrc_ccm_get_curr_camped_plmn();
    macro_freq = rrc_ccm_get_curr_camped_freq();
    macro_psc = rrc_ccm_get_curr_camped_psc();
    macro_cell_id = rrc_ccm_get_curr_camped_cell_id();
  }

  for(i=0; i< (stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid); i++)
  {
    if((acq_list[i].freq != rrc_csp_int_data.curr_acq_entry.freq)||
       (acq_list[i].csg_id == SYS_CSG_ID_INVALID))
    {
      continue;
    }
    if(!is_lte)
    {
      for(j=0; j<acq_list[i].fp_w_macro_cnt; j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].w_macro[j].plmn_id,macro_plmn)
           &&(acq_list[i].w_macro[j].freq == macro_freq)
           &&(acq_list[i].w_macro[j].psc == macro_psc)
           &&(acq_list[i].w_macro[j].cell_id == macro_cell_id))
        {
          /* acq failure cases */
          if(!acq_status)
          {
            if(acq_list[i].w_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
            {
              acq_list[i].w_macro[j].acq_fail_counter = 0;
              rrccsp_remove_fp_info(i,j,TRUE);
              WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
            }
            else
            {
              acq_list[i].w_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].w_macro[j].acq_fail_counter);
            }
          }
          else
          {
            match_found = FALSE;
            if(acq_list[i].scr_code == rrc_csp_int_data.curr_acq_entry.scr_code)
            {
              match_found = TRUE;
              WRRC_MSG2_HIGH("CSG: Match is found do not touch counter, entry %d, FP entry %d",i,j);
            }
            for(k=0; (k<rrc_csp_int_data.num_scr_code_det)&&(!match_found); k++)
            {
              if(acq_list[i].scr_code == rrc_csp_int_data.scr_code_det[k].scr_code)
              {
                match_found = TRUE;
                WRRC_MSG2_HIGH("CSG: Match is found do not touch counter, entry %d, FP entry %d",i,j);
                break;
              }
            }
            if(!match_found)
            {
              if(acq_list[i].w_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
              {
                acq_list[i].w_macro[j].acq_fail_counter = 0;
                rrccsp_remove_fp_info(i,j,TRUE);
                WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
              }
              else
              {
                acq_list[i].w_macro[j].acq_fail_counter++;
                MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].w_macro[j].acq_fail_counter);
              }
            }
          }
        }
      }
    }
    else
    {
      for(j=0; j<acq_list[i].fp_lte_macro_cnt; j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].lte_macro[j].plmn_id,macro_plmn)
           &&(acq_list[i].lte_macro[j].freq == macro_freq)
           &&(acq_list[i].lte_macro[j].pci == macro_psc)
           &&(acq_list[i].lte_macro[j].cell_id == macro_cell_id))
        {
          /* acq failure cases */
          if(!acq_status)
          {
            if(acq_list[i].lte_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
            {
              acq_list[i].lte_macro[j].acq_fail_counter = 0;
              rrccsp_remove_fp_info(i,j,FALSE);
              WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
            }
            else
            {
              acq_list[i].lte_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].lte_macro[j].acq_fail_counter);
            }
          }
          else
          {
            match_found = FALSE;
            if(acq_list[i].scr_code == rrc_csp_int_data.curr_acq_entry.scr_code)
            {
              match_found = TRUE;
              WRRC_MSG2_HIGH("CSG: Match is found do not touch counter, entry %d, FP entry %d",i,j);
            }
            for(k=0; (k<rrc_csp_int_data.num_scr_code_det)&&(!match_found); k++)
            {
              if(acq_list[i].scr_code == rrc_csp_int_data.scr_code_det[k].scr_code)
              {
                match_found = TRUE;
                WRRC_MSG2_HIGH("CSG: Match is found do not touch counter, entry %d, FP entry %d",i,j);
                break;
              }
            }
            if(!match_found)
            {
              if(acq_list[i].lte_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
              {
                acq_list[i].lte_macro[j].acq_fail_counter = 0;
                rrccsp_remove_fp_info(i,j,FALSE);
                WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
              }
              else
              {
                acq_list[i].lte_macro[j].acq_fail_counter++;
                MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].lte_macro[j].acq_fail_counter);
              }
            }
          }
        }
      }
    }
  }
}

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
void rrccsp_prune_acqdb_after_SIB(rrc_int_csg_id_type csg_id, boolean is_white)
{
  uint8 i,j;
  rrc_plmn_identity_type macro_plmn;
  uint32 macro_freq;
  uint16 macro_psc;
  uint32 macro_cell_id;
  boolean is_lte = FALSE;

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    macro_plmn = rrc_csp_int_data.lte_macro_asf.plmn;
    macro_freq = rrc_csp_int_data.lte_macro_asf.freq;
    macro_psc = rrc_csp_int_data.lte_macro_asf.pci;
    macro_cell_id = rrc_csp_int_data.lte_macro_asf.cell_id;
    is_lte = TRUE;
  }
  else
  {
    macro_plmn = rrc_ccm_get_curr_camped_plmn();
    macro_freq = rrc_ccm_get_curr_camped_freq();
    macro_psc = rrc_ccm_get_curr_camped_psc();
    macro_cell_id = rrc_ccm_get_curr_camped_cell_id();
  }

  for(i=0; i< (stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid); i++)
  {
    if((acq_list[i].freq != rrc_csp_int_data.curr_acq_entry.freq)||
       (acq_list[i].scr_code != rrc_csp_int_data.curr_acq_entry.scr_code)||
       (acq_list[i].csg_id == SYS_CSG_ID_INVALID))
    {
      continue;
    }
    if(!is_lte)
    {
      for(j=0; j<acq_list[i].fp_w_macro_cnt; j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].w_macro[j].plmn_id,macro_plmn)
           &&(acq_list[i].w_macro[j].freq == macro_freq)
           &&(acq_list[i].w_macro[j].psc == macro_psc)
           &&(acq_list[i].w_macro[j].cell_id == macro_cell_id))
        {
          if(csg_id != acq_list[i].csg_id)
          {
            if(acq_list[i].w_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
            {
              acq_list[i].w_macro[j].acq_fail_counter = 0;
              rrccsp_remove_fp_info(i,j,TRUE);
              WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
            }
            else
            {
              acq_list[i].w_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].w_macro[j].acq_fail_counter);
            }            
          }
          else
          {
            if(is_white)
            {
              acq_list[i].w_macro[j].acq_fail_counter = 0;
              WRRC_MSG2_HIGH("CSG: Prune counter set to zero, entry %d, FP entry %d",i,j);
            }
            else
            {
              acq_list[i].w_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].w_macro[j].acq_fail_counter);
            }
          }
        }
      }
    }
    else
    {
      for(j=0; j<acq_list[i].fp_lte_macro_cnt; j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].lte_macro[j].plmn_id,macro_plmn)
           &&(acq_list[i].lte_macro[j].freq == macro_freq)
           &&(acq_list[i].lte_macro[j].pci == macro_psc)
           &&(acq_list[i].lte_macro[j].cell_id == macro_cell_id))
        {
          if(csg_id != acq_list[i].csg_id)
          {
            if(acq_list[i].lte_macro[j].acq_fail_counter >= wcdma_rrc_csg_max_prune_count_nv)
            {
              acq_list[i].lte_macro[j].acq_fail_counter = 0;
              rrccsp_remove_fp_info(i,j,TRUE);
              WRRC_MSG2_HIGH("CSG: Removed FP assocation for entry %d FP entry %d",i,j);
            }
            else
            {
              acq_list[i].lte_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].lte_macro[j].acq_fail_counter);
            }            
          }
          else
          {
            if(is_white)
            {
              acq_list[i].lte_macro[j].acq_fail_counter = 0;
              WRRC_MSG2_HIGH("CSG: Prune counter set to zero, entry %d, FP entry %d",i,j);
            }
            else
            {
              acq_list[i].lte_macro[j].acq_fail_counter++;
              MSG_HIGH("CSG: Acq fail counter for entry %d FP entry %d : %d",i,j,acq_list[i].lte_macro[j].acq_fail_counter);
            }
          }
        }
      }
    }
  }
}

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
void rrccsp_remove_fp_info(uint8 acq_index, uint8 fp_index, boolean is_w_macro)
{
  uint8 i=0;

  if(is_w_macro)
  {
    for(i = fp_index; i < acq_list[acq_index].fp_w_macro_cnt - 1; i++)
    {
      acq_list[acq_index].w_macro[i] = acq_list[acq_index].w_macro[i+1];
    }
    acq_list[acq_index].fp_w_macro_cnt--;
  }
  else
  {
    for(i = fp_index; i < acq_list[acq_index].fp_lte_macro_cnt - 1; i++)
    {
      acq_list[acq_index].lte_macro[i] = acq_list[acq_index].lte_macro[i+1];
    }
    acq_list[acq_index].fp_lte_macro_cnt--;
  }
}

/*===========================================================================

FUNCTION          RRCCSP_LOG_MACRO_FP_INFO

DESCRIPTION       Logs CSG cells finger printed to current camped macro.
                  CSP uses this function at the start of ASF

DEPENDENCIES      NONE

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_log_macro_fp_info(uint8 rat )
{
  uint32 i,j,num_valid_entries;  
  WCDMA_FP_INFO_type* log_record_ptr;
  uint32 freq, cell_id;
  uint16 psc;
  rrc_plmn_identity_type plmn;
  
  log_record_ptr = (WCDMA_FP_INFO_type *)log_alloc( WCDMA_FP_INFO, sizeof(WCDMA_FP_INFO_type));

  if(log_record_ptr == NULL)
  {
    return;
  }

  log_record_ptr->num_csg_entries = 0;
  if(rat == W_ASF_RAT_WCDMA)
  {
    plmn = rrc_ccm_get_curr_camped_plmn();
    freq = rrc_ccm_get_curr_camped_freq();
    psc = rrc_ccm_get_curr_camped_psc();
    cell_id = rrc_ccm_get_curr_camped_cell_id();
  }
  else
  {
    plmn = rrc_csp_int_data.lte_macro_asf.plmn;
    freq = rrc_csp_int_data.lte_macro_asf.freq;
    psc = rrc_csp_int_data.lte_macro_asf.pci;
    cell_id = rrc_csp_int_data.lte_macro_asf.cell_id;
  }

  log_record_ptr->version = 0;
  log_record_ptr->rat = rat;
  log_record_ptr->uarfcn = freq;
  log_record_ptr->psc = psc;
  log_record_ptr->cell_id = cell_id;
  log_record_ptr->mcc[0] = plmn.mcc[0];
  log_record_ptr->mcc[1] = plmn.mcc[1];
  log_record_ptr->mcc[2] = plmn.mcc[2];
  log_record_ptr->mnc[0] = plmn.mnc[0];
  log_record_ptr->mnc[1] = plmn.mnc[1];
  log_record_ptr->mnc[2] = plmn.mnc[2];
  if (plmn.num_mnc_digits == 2) 
  {
    log_record_ptr->mnc[2] = 0xF;
  }

  num_valid_entries = stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid;
  for(i = 0; i < num_valid_entries; i++)
  {
    if(rat == W_ASF_RAT_WCDMA)
    {
      for(j = 0;((j < acq_list[i].fp_w_macro_cnt)&&(log_record_ptr->num_csg_entries < MAX_ACQ_DB_CSG_ENTRIES)); j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].w_macro[j].plmn_id,plmn)
           &&(acq_list[i].w_macro[j].freq == freq)
           &&(acq_list[i].w_macro[j].psc == psc)
           &&(acq_list[i].w_macro[j].cell_id == cell_id)
           &&(rrccsp_is_csg_white(acq_list[i].csg_id,acq_list[i].plmn_id)))
        {
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].uarfcn = acq_list[i].freq;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].psc = acq_list[i].scr_code/16;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].cell_id = acq_list[i].cell_id;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].csg_id = acq_list[i].csg_id;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[0] = acq_list[i].plmn_id.mcc[0];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[1] = acq_list[i].plmn_id.mcc[1];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[2] = acq_list[i].plmn_id.mcc[2];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[0] = acq_list[i].plmn_id.mnc[0];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[1] = acq_list[i].plmn_id.mnc[1];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[2] = acq_list[i].plmn_id.mnc[2];
          if (plmn.num_mnc_digits == 2) 
          {
            log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[2] = 0xF;
          }
          log_record_ptr->num_csg_entries++;
          break;
        }
      }
    }
    else
    {
      for(j = 0;((j < acq_list[i].fp_lte_macro_cnt)&&(log_record_ptr->num_csg_entries < MAX_ACQ_DB_CSG_ENTRIES)); j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].lte_macro[j].plmn_id,plmn)
           &&(acq_list[i].lte_macro[j].freq == freq)
           &&(acq_list[i].lte_macro[j].pci == psc)
           &&(acq_list[i].lte_macro[j].cell_id == cell_id)
           &&(rrccsp_is_csg_white(acq_list[i].csg_id,acq_list[i].plmn_id)))
        {
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].uarfcn = acq_list[i].freq;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].psc = acq_list[i].scr_code/16;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].cell_id = acq_list[i].cell_id;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].csg_id = acq_list[i].csg_id;
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[0] = acq_list[i].plmn_id.mcc[0];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[1] = acq_list[i].plmn_id.mcc[1];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mcc[2] = acq_list[i].plmn_id.mcc[2];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[0] = acq_list[i].plmn_id.mnc[0];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[1] = acq_list[i].plmn_id.mnc[1];
          log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[2] = acq_list[i].plmn_id.mnc[2];
          if (plmn.num_mnc_digits == 2) 
          {
            log_record_ptr->csg_cells[log_record_ptr->num_csg_entries].mnc[2] = 0xF;
          }
          log_record_ptr->num_csg_entries++;
          break;
        }
      }
       
    }
  }
  log_commit(log_record_ptr);
}

/*===========================================================================

FUNCTION          WCDMA_RRC_GET_WCSG_LFP_INFO

DESCRIPTION       Gives the list of FPed W CSG cells to passed L macro

DEPENDENCIES      NONE

RETURN VALUE      Number of CSG cells FPed
                  
SIDE EFFECTS      NONE

===========================================================================*/
uint8 wcdma_rrc_get_wcsg_lfp_info(const source_cell_info_type *l_macro_ptr, source_cell_info_type *w_csg_cells_ptr )
{
  uint8 i,j,num_valid_entries,k=0;  
  uint32 freq, cell_id;
  uint16 psc;
  rrc_plmn_identity_type plmn;
  

  if((w_csg_cells_ptr == NULL) ||(l_macro_ptr == NULL))
  {
    return 0;
  }

  plmn    = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(l_macro_ptr->plmn);
  freq    = l_macro_ptr->frequency;
  psc     = l_macro_ptr->phy_cell_id;
  cell_id = l_macro_ptr->cell_identity;

  num_valid_entries = stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid;
  
  for(i = 0; i < num_valid_entries; i++)
  {
      for(j = 0;((j < acq_list[i].fp_lte_macro_cnt)&&(k < LTE_IRAT_MAX_MACRO_CELLS)); j++)
      {
        if(rrc_bcch_plmns_match(acq_list[i].lte_macro[j].plmn_id,plmn)
           &&(acq_list[i].lte_macro[j].freq == freq)
           &&(acq_list[i].lte_macro[j].pci == psc)
           &&(acq_list[i].lte_macro[j].cell_id == cell_id)
           &&(rrccsp_is_csg_white(acq_list[i].csg_id,acq_list[i].plmn_id)))
        {
          w_csg_cells_ptr[k].frequency     = acq_list[i].freq;
          w_csg_cells_ptr[k].phy_cell_id   = acq_list[i].scr_code/16;
          w_csg_cells_ptr[k].cell_identity = acq_list[i].cell_id;
          w_csg_cells_ptr[k].csg_id        = acq_list[i].csg_id;
          w_csg_cells_ptr[k].plmn          = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(acq_list[i].plmn_id);
          k++;
        }
      }
       
  }
  return k;
}

/*===========================================================================

FUNCTION          RRCCSP_CHECK_WCDMA_FP_INFO

DESCRIPTION       Checks if WCDMA FP info is available or not

DEPENDENCIES      NONE

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean rrccsp_check_wcdma_fp_info(void)
{
  uint8 i,num_valid_entries = 0;
  boolean fp_entry_found = FALSE;
  num_valid_entries = stored_acq_db.curr_wr_index + !stored_acq_db.last_entry_invalid;

  for(i = 0; (i < num_valid_entries) && (fp_entry_found == FALSE); i++)
  { 
    fp_entry_found = rrccsp_fp_macro_present_in_acq_db(acq_list[i]);
  }
  return fp_entry_found;
}
#endif
