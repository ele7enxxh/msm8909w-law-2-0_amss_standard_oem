/*===========================================================================
             R R C    C S P    F R E Q U E N C Y    S C A N
DESCRIPTION

   This file contains the access routines for Frequency Scan.

   It implements the frequency scan algorithm support in RRC.
   The implemented blocks are marked with a "=======" boundary below.

              CSP PLMN SEARCH MECHANISM
              
       -----------   ------------    -------------    ---------
       |         |   | CSP      |    | Acq DB    |    | Acq   |
       |CSP      |   | Initial  |    | Access    |    | DB    |
       |State    |   | And .    |<-->| Functions |<-->|       |
       |Machine  |   | Stored   |    | -PLMN     |    |       |
       |Functions|<->|Selection |    | -Acq type |    |       |
       |         |   |Controller|    -------------    ---------
       |         |   |-PLMN     |    
       |         |   |-PLMN     |    ============
       -----------   | type     |<-->| CSP      |
            |        ------------    | Frequency|
            |           |            | Scan     |
            |           |            | Algorithm| 
       -----------      |            ============            
       |Internal |   ------------       
       |  Data   |   |Selection |
       |         |   |          |
       -----------   |Data      |
                     ------------

EXTERNALIZED FUNCTIONS   
   
  rrc_csp_init_freq_scan_data()
    This function initializes the frequency scan data in Current selection 
    data.

  rrc_csp_get_next_freq_scan_entry()
    This function is responsible for determining the next frequency scan type
    and returning the frequency.

  rrc_csp_update_freq_scan_entry()
    This function is responsible for updating frequency scan
    data in Current selection data. 


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

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccspfscan.c_v   1.3   10 Apr 2002 12:08:02   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccspfscan.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   vs      Changes to remove irrelevant f3s
09/15/14   bc      Changes to correct F3.
08/08/14   sr      Made changes to pass the bandclass by derving from UARFCN during acq db scan
08/04/14   bc      Made changes to fix compiler warnings.
05/20/14   rmn     Made changes to initialize CSG ID stored in internal data structure
05/16/14   vi      Made changes to support MOB framework
02/17/14   sr      Changes to pass correct EFS DB size and consider serving frequency for manual CSG search
01/03/14   bc      Made Changes to read SIBs across DRX cycles.
01/02/14   rmn     Reduction of unwanted F3's
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
12/23/13   bc      Made changes to try ACQ on the last camped frequency in idle mode once during every RAW scan.
12/13/13   sn      Manual PLMN search optimizations
11/18/13   bc      Checked in REL 10 spec crs changes
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
06/17/13   sr      Changes to fix compilation errors
05/09/13   sn      Reduce fine/raw scan skip range for manual CSG search
04/03/13   sn      Changes to skip frequencies in 4.4MHz for XtoW CSG searches
04/03/13   sr      Made changes to remove redundant MSG_HIGHs
03/22/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
12/07/12   sn      CSG feature changes
10/01/12   sn      Changes to avoid list_db array overflow which is leading to WRRC ACQ DB/ 
                   other global variables corruption 
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/15/12   vg      fixed compilation errors.
04/13/12   pm      removing FEATURE_BAND_PRIORITY_ORDER_THROUGH_NV feature flag and reverting some #ifdef
                   changes
04/10/12   pm      Added code to support the band priority nv
11/29/11   gv      Fixed compiler warnings
10/19/11   gv      Made changes to properly skip the overlapping frequencies
10/12/11   gv      Corrected the dl_offset set for BC19 normal channels
09/28/11   gv      Added code to support optimized band scan in WCDMA
05/19/11   sks     Made changes to use GSM detected cell list during LTOW BPLMN search.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/23/11   sk      added code for BC19 additonal channel scaning
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/20/10   sks     Made changes to perform acq retries on LTOW redirected freq.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/30/10   sks     Made changes to prioritize redirected freq over neighbour cells provided
                   by LTE-RRC during LTOW redirection.
07/06/10   rm      Fixed compiler errors for UOOS
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
04/12/10   rm      Added code to support BC11
03/19/10   rm      Added code to scan 850 band first during full scan when NV
                   NV_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I is set.
                   Changes are under FEATURE_PRIORITIZE_850_BAND_FOR_FULL_SCAN
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/26/09   sks     Fixed klocwork errors.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/22/09   rm      Added code to use last camped PLMN ID to derive the band class
                   of the UARFCNs in 850-800 bands overlap region when prioritizing
                   the bands for full search
07/20/09   rm      Removed usage of FEATURE_SURF_5200
07/16/09   rm      Ported FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE to COOS
07/10/09   rm      Added code to check if the band is supported when adding 800, 850 bands to 
                   prioritized_bands
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/16/09   rm      Made changes to not call l1_srchfs_list_scan_direct when WL1 is being stopped
02/02/09   rm      Ported List Frequency Scan (LFS) to conventional OOS
10/31/08   sk      Removed GCF specific feature to limit raw scan results to 5
06/17/08   pk      Limit the maximum number of target FDD entries that are tried for acquisition
                   to MAX_ACQ_DB_ENTRIES.
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/21/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
10/05/07   vm      For FEATURE_UMTS_ACQ_CHNL_INFO_SHARING, added check for 
                   RRC_CSP_OUT_OF_SERVICE_AREA_SCAN as well.                   
07/07/07   vm      Removed BSP defined constants for WCDMA Band Boundaries. These have been
                   replaced by WL1 defined constants.
06/25/07   vm      Added fix for CR 119540. Added support to correctly set/reset current_band 
                   so that the band indicator check by SIB proc is done against the correct
                   current_band value.
06/20/07   vm      Added fix for CR 121957. The GSM skip range has now been 
                   changed for W RAW and Fine Scans.
05/23/07   pk      Added fix for CR 117463. Now different timers are used for automatic and manual
                   searches under the feature flag FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER. 
                   Full frequency scans are not done for automatic and manual (both foreground 
                   and background) searches while the respective timers are running.
05/21/07   vm      Added support for the feature FEATURE_RRC_ADDL_CHNL_SCAN_OPTIMIZATION
03/27/07   vm      Added fix for CR 113241. When UOOS is enabled, # of raw scan
                   results are NOT limited to 5 when GCF flag is enabled.
03/09/07   vm      FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2: added the fix to NOT  
                   call L1 function to get list frequency scan results if acq
                   has already succeeded.
03/08/07   vm      FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2: added the fix to check 
                   if uoos_use_timer is TRUE before calling L1 function to get
                   list frequency scan results.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
11/17/06   vm      Added fix for CR 106389. The "skip range" for eliminating
                   UMTS channels around GSM ARFCNs is now set to 12 (earlier it was 
                   15 (for RAW) / 18 (for Fine)).
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
07/21/06   vm      Added support for the feature UOOSI.
06/08/06   vm      Added support for new feature FEATURE_MCC_AND_850_800_BAND_COMBINATION.
                   The function validate_dl_arfcn() used in the file now takes
                   two additional arguments when the feature is defined. If the feature
                   is not defined then it continues to take 1 argument only.
03/30/06   vm      Added support to start the PLMN list full freq scan timer 
                   for 3G to 3G Automatic/Manual PLMN searches also (both 
                   background and foreground). As long as this timer is active 
                   no full scans are done. 
02/21/06   vm      Added support for 1700_2100 AWS Band (IV) under feature name
                   FEATURE_WCDMA_BC4.
02/17/06   vm      Added support for 1700 band under feature name FEATURE_WCDMA_BC9.
02/09/06   vm      Added fix to not check whether full scan is needed when
                   current scan is RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN or
                   RRC_CSP_WTOW_BPLMN_MANUAL_SCAN.
01/25/06   bd      Lint clean-up
01/20/06   vm      Added support for getting GSM arfcns during Manual/Automatic
                   PLMN search so that acq attempt on some of the W channels can
                   be skipped to reduce freq scan times in PCS/850 bands. The 
                   changes are under feature flag FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
11/28/05   bu      Added initial support for 900 MHz band under FEATURE_WCDMA_900.
11/23/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
                   Before acquisition attempt is made on a frequency, it is checked if the
                   frequency can be skipped by using information from GSM ARFCN list.
11/22/05   vm      Added changes related to scanning ALL ACQ DB entries before
                   doing Full Scan. 
11/16/05   vm      Added more support related to BPLMN features.
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
09/26/05   vm      Added support for not adding the last camped frequency to the 
                   fine scan list when the scan type is Manual or Automatic PLMN 
                   List Scan, or Inter Frequency Redirection scan.
08/04/05   vm      Added support for scanning again the last camped frequency 
                   during every fine scan when UE goes OOS in Idle mode.
                   Added the last camped frequency as the last entry in the fine
                   scan list received from L1.
05/09/05   vm      Variable ret_freq now has a legitimate value when 
                   Additional Channel Scan is needed (addl_chls_scan_needed = TRUE)
05/05/05   vm      Removed a misleading MSG_HIGH from rrccsp_should_freq_be_skipped()
                   and corrected a MSG_HIGH in rrc_csp_update_freq_scan_entry() function.
04/15/05   vm      Due to R&S testbox returning spurious frequencies which
                   adversely affects the GCF test cases, a fix has been added
                   to limit the number of raw scan results to RRC_MAX_RAW_SCAN_FREQ. 
                   The changes are under GCF NV flag.
02/15/05   vm      Added support for deep sleep. In particular, when UE is in 
                   deep sleep then full scan should be done only on those bands
                   whose frequencies are not present in ACQ DB.
01/21/05   vm      For additional channel scan results, reset the variable
                   rrc_csp_curr_select_data.scan_info.fine_scan_completed to TRUE.
12/23/04   vm      Added a fix to handle full frequency scan when ue is in 
                   deep sleep and 2G->3G BPLMN scan is also in progress.
12/14/04   bu      Initiate an additional channels' scan even when no raw scan
                   results are returned by L1.
12/02/04   vm      Included file rrcscmgr.h to remove lint errors.
11/22/04   vm      Added code for handling new scan types RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN 
                   and RRC_CSP_GTOW_BPLMN_MANUAL_SCAN.
11/10/04   bu/vm   Added support to not skip the last camped frequency while UE
                   was in connected mode for DCH->FACH/PCH and OOS in connected
                   mode scans.
10/12/04   bu      Added support to scan additional channels for various WCDMA
                   bands if needed.
09/01/04   bu      Added support for 1800 MHz band.
08/31/04   bu      Use rrc_csp_band_class_type instead of rrc_csp_band_class_e_type.
                   In rrc_csp_init_freq_scan_data(), if the first_band_to_be_scanned
                   hasn't been scanned yet, set it to current band.
08/13/04   bu      Added WCDMA multiband support. Added range validity checks for
                   800 and 850 MHz bands in rrc_csp_update_freq_scan_entry().
07/08/04   bu      Added rrccsp_should_freq_be_skipped(), rrccsp_is_fine_scan_required
                   and rrccsp_is_acq_scan_required(). An acquisition or fine frequency
                   scan on a particular result is skipped if it falls in the skip range
                   (-18 <= freq <= +18) for fine scan result and (-15 <= freq <= +15)
                   for a raw scan result.
06/14/04   bu      Added support to not trigger a full frequency scan if the full
                   scan timer has not expired or if UE is not in deep sleep. Removed
                   the clipping of coarse scan results.
05/05/04   bu      Added support to trigger RAW SCAN only if the full scan timer
                   has expired or the scan type is PLMN_LIST_SCAN, DCH->FACH or
                   DCH->PCH scan.
03/12/04   bu      Break out of the loop first time a coarse scan result
                   below -98 is found. 
03/10/04   bu      Changed number of raw scan hypotheses to max of 5 and number
                   of raw scan results > -98 dBm.
01/21/04   vn      Relaxed condition for count of frequencies returned
                   in fine scan, also check for 100KHz offset channels.
09/10/03   vn      Fixed implementation under FEATURE_LIMITED_RAW_SCAN to 
                   not presume all RRC_MAX_RAW_SCAN_FREQ frequencies are 
                   present in Freq Scan Cnf. Also added more defensive checks.
06/24/03   bu      Removed lint errors.
01/31/03   vn      Removed linting errors.
12/16/02   bu      Changes to scan only RRC_MAX_RAW_SCAN_FREQ frequencies during
                   a raw scan in rrc_csp_update_freq_scan_entry(). Changes are
                   under FEATURE_LIMITED_RAW_SCAN.
04/10/02   bu      Changes to scan only one frequency if FEATURE_ONE_FREQ_SCAN
                   is defined.
01/30/02   vn      Include bsp.h in place of bsp_rf_jerry1.h
09/17/01   vn      Fixed compiler error for VST build. For non-surf builds
                   use DEFAULT_FREQ since hw file is not included.
08/23/01   vn      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "err.h"
#include "uecomdef.h"
#include "task.h"

#include "rrcmmif.h"
#include "rrcintif.h"
#include "rrccmd_v.h"
#include "l1rrcif.h"
#include "l1task_v.h"


#include "rrccspi.h"
#include "rrcscmgr.h"

#include "rrcccm.h"
#include "rrcmeasi.h"
#include "rrcsibdb.h"

#include "rrcmcm.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


extern rrc_csp_acq_entry_type  list_db[MAX_ACQ_DB_ENTRIES];

extern boolean prioritize_850_band;
/* This macro picks the next frequency from raw scan list in current
 * selection data and returns the frequency for fine scan. It also
 * initializes the fine scan list and sets the type of scan to Fine Scan.
 */
#define  RRC_CSP_PICK_NEXT_RAW_FREQ_FOR_FINE_SCAN()    \
  {                                                    \
    rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index ++;   \
    curr_freq_index =                                                 \
      rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index;    \
    *ret_freq =                                                       \
      rrc_csp_curr_select_data.scan_info.raw_scan.freq_result[curr_freq_index].ctr_freq; \
    rrc_csp_curr_select_data.scan_info.fine_scan_completed = FALSE;   \
    rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index = 0; \
    rrc_csp_curr_select_data.scan_info.fine_scan.num_freq = 0;        \
    rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_FINE_SCAN;  \
  }

/* This macro picks the first frequency from raw scan list in current
 * selection data and returns the frequency for fine scan. It also
 * initializes the fine scan list and sets the type of scan to Fine Scan.
 */
#define  RRC_CSP_PICK_FIRST_RAW_FREQ_FOR_FINE_SCAN()  \
  {                                                   \
    rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index = -1;  \
    RRC_CSP_PICK_NEXT_RAW_FREQ_FOR_FINE_SCAN();                        \
  }

/* This macro picks the next frequency from fine scan list in current
 * selection data and returns the frequency for acquisition. It also
 * sets the type of scan to Acquisition scan.
 */
#define  RRC_CSP_PICK_NEXT_FINE_FREQ_FOR_ACQ()   \
  {                                              \
    rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index ++;  \
    *ret_freq = rrc_csp_curr_select_data.scan_info.fine_scan.         \
                freq_result[rrc_csp_curr_select_data.scan_info.         \
                            fine_scan.curr_freq_index].ctr_freq;        \
    rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_ACQ_SCAN; \
  }

/* This macro picks the first frequency from fine scan list in current
 * selection data and returns the frequency for acquisition. It also
 * sets the type of scan to Acquisition scan.
 */
#define  RRC_CSP_PICK_FIRST_FINE_FREQ_FOR_ACQ()   \
  {                                         \
    rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index = -1;  \
    RRC_CSP_PICK_NEXT_FINE_FREQ_FOR_ACQ();                              \
  }

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION          RRC_CSP_INIT_FREQ_SCAN_DATA

DESCRIPTION       This function initializes the frequency scan data in
                  Current selection data.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void  rrc_csp_init_freq_scan_data(void)
{

  /* Since we haven't started the scan, initialize it to NO_SCAN */
  rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_NO_SCAN;

  /* Set Raw and Fine scan completed booleans to FALSE */
  rrc_csp_curr_select_data.scan_info.raw_scan_completed = FALSE;
  rrc_csp_curr_select_data.scan_info.fine_scan_completed = FALSE;

  /* Current freq. indices in raw and fine scan lists are set to 0 */
  rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index = 0;
  rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index = 0;

  /* Number of freq in raw and fine scan list are set to 0 */
  rrc_csp_curr_select_data.scan_info.raw_scan.num_freq = 0;
  rrc_csp_curr_select_data.scan_info.fine_scan.num_freq = 0;
  /* If the first band to be scanned has not been scanned yet, 
   * assign it to current band so that a full scan is initiated
   * on it
   */

#ifdef FEATURE_WCDMA_MULTIBAND
  if(rrc_csp_curr_select_data.first_band_to_be_scanned & 
       rrc_csp_curr_select_data.next_band_mask)
  {
    rrc_csp_curr_select_data.current_band = 
      rrc_csp_curr_select_data.first_band_to_be_scanned;

#ifdef FEATURE_EOOS
    #error code not present
#endif
  }

#else /* FEATURE_WCDMA_MULTIBAND */

  rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;

#endif



} /* rrc_csp_init_freq_scan_data */
/*===========================================================================

FUNCTION          RRCCSP_SHOULD_OVERLAPPING_FREQ_BE_SKIPPED

DESCRIPTION       This function determines based on previous scanned frequencies,
                  whether a new frequency should be scanned or skipped.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if frequency should be skipped
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_should_overlapping_freq_be_skipped(rrc_freq_type freq)
{
   boolean ret_status = FALSE;
   boolean additional_channel;
   uint16 frequency;
   uint16 i;

   frequency = rrccsp_derive_freq_from_uarfcn(freq,rrc_csp_curr_select_data.current_band);


   /* Refer 3GPP TS 25.101 for frequency numbers */
  #ifdef FEATURE_WCDMA_MULTIBAND
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_IMT2K_BAND)
   {
     if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
     {
        /* IMT2K band is already scanned and current band being scanned is AWS. Hence skip all
         * all the AWS band frequencies except for the additional frequencies 
         */
        i = 0;
        additional_channel = FALSE;
        while(rrc_dl_band_h_addl_channels[i] != 0)
        {
          if(freq == rrc_dl_band_h_addl_channels[i])
          {
            additional_channel = TRUE;
            break;
          }
          i++;
        }
		 
        if(!additional_channel)
        {
          WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
          return(ret_status = TRUE);
        }
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_1700_2100_BAND)
   {
     if(rrc_csp_curr_select_data.current_band == RRC_CSP_IMT2K_BAND)
     {
       /* AWS band is already scanned and current band being scanned is IMT2K. Hence skip only
        * those frequencies which are already scanned as a part of AWS band. As AWS band is a 
        * complete subset of IMT2K band we will skip frequencies starting from 21124 (Fdl_Low_AWS)
        * till 21526 (Fdl_High_AWS)
        */
       if((frequency >= 21124) && (frequency <= 21526))
       {
         WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
         return(ret_status = TRUE);
       }
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_850_BAND)
   {
     if((rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
        || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
       )
     {
       /* 850 band is already scanned and current band being scanned is either 800 band or BC19.
        * In either case we will skip all the frequencies as 850 band is a superset of 800 band 
        * and BC19. It also covers the additional frequencies.
        */ 
       WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
       return(ret_status = TRUE);
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_800_BAND)
   {
     if((rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
        || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
       )
     {
       /* 800 band is already scanned and the current band being scanned is either 850 band or BC19.
        * In this case we can skip all the frequencies starting from 8774 (Fdl_Low_800) till 8826 (Fdl_High_800)
        * corresponding to 800 band as it is a complete subset of 850 band and BC19.
        */
       if((frequency >= 8774) && (frequency <= 8826))
       {
         WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
         return(ret_status = TRUE);
       }
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_BC19_BAND)
   {
     if((rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND) ||
        (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
       )
     {
       /* BC19 is already scanned and the current band being scanned is either 850 band or 800 band.
        * In this case we can skip all the frequencies starting from 8774 (Fdl_Low_BC19) till 8876 (Fdl_High_BC19)
        * 800 band is a subset of BC19 and 850 band is a superset of BC19.
        */
       if((frequency >= 8774) && (frequency <= 8876))
       {
         WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
         return(ret_status = TRUE);
       }
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_1800_BAND)
   {
     if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_BAND)
     {
       /* 1800 band is already scanned and current band being scanned is 1700 band
        * We will skip all the frequencies as 1800 band is a superset of 1700 band 
        */ 
       WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
       return(ret_status = TRUE);
     }
   }
   if(rrc_csp_curr_select_data.scanned_band_mask & RRC_CSP_WCDMA_1700_BAND)
   {
     if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1800_BAND)
     {
       /* 1700 band is already scanned and current band being scanned is 1800 band
        * We will skip all the frequencies starting from 18474 (Fdl_Low_1700) till 18774 (Fdl_Low_1700) 
        */
       if((frequency >= 18474) && (frequency <= 18774))
       {
         WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
         return(ret_status = TRUE);
       }
     }
   }
  #endif /* FEATURE_WCDMA_MULTIBAND */



  return ret_status;
}

/*===========================================================================

FUNCTION          RRCCSP_SHOULD_FREQ_BE_SCANNED_WITHOUT_SKIPPING

DESCRIPTION       This function checks whether the frequency should be allowed to scan
                  in the current band without being skipped.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if frequency should be scanned without being skipped
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_should_freq_be_scanned_without_skipping(rrc_freq_type freq)
{
   boolean ret_status = FALSE;
   uint16 i;
   uint16 frequency;

   frequency = rrccsp_derive_freq_from_uarfcn(freq,rrc_csp_curr_select_data.current_band);

   for(i=0;i < rrc_csp_curr_select_data.num_of_barred_freq; i++)
   {
     if(frequency == rrccsp_derive_freq_from_uarfcn(rrc_csp_curr_select_data.barred_freq[i],rrc_csp_curr_select_data.band_corr_barred_freq[i]))
     return TRUE;
   }
   
  return ret_status;
}   
/*===========================================================================

FUNCTION          RRCCSP_VALIDATE_CTR_FREQ

DESCRIPTION       This function checks if the UARFCN is valid in the current band.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_validate_ctr_freq(uint16 frequency, rrc_csp_band_class_type curr_band)
{
  
  boolean ret_status = FALSE;

  /* Check if the absolute frequency lies within the range
   * of the current band being scanned 
   */
  if((curr_band == RRC_CSP_IMT2K_BAND) &&
     (frequency >= 21124) && (frequency <= 21676))
    ret_status = TRUE;
  
  else if((curr_band == RRC_CSP_WCDMA_1700_2100_BAND) &&
         (frequency >= 21124) && (frequency <= 21526))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_1800_BAND) &&
          (frequency >= 18474) && (frequency <= 18774))
    ret_status = TRUE;
 
  else if((curr_band == RRC_CSP_WCDMA_1700_BAND) &&
         (frequency >= 18074) && (frequency <= 18776))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_850_BAND) &&
         (frequency >= 8714) && (frequency <= 8916))
    ret_status = TRUE;
  
  else if((curr_band == RRC_CSP_WCDMA_800_BAND) &&
          (frequency >= 8774) && (frequency <= 8826))
    ret_status = TRUE;
  
  else if((curr_band == RRC_CSP_WCDMA_BC19_BAND) &&
          (frequency >= 8774) && (frequency <= 8876))
    ret_status = TRUE;

  if(ret_status == FALSE)
    WRRC_MSG2_ERROR("WCDMA frequency %d invalid in band %d",frequency,rrc_csp_curr_select_data.current_band);
  
  return (ret_status);
}
/*===========================================================================

FUNCTION          RRCCSP_VALIDATE_CTR_FREQ_UL

DESCRIPTION       This function checks if the UL UARFCN is valid in the current band.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_validate_ctr_freq_ul(uint16 frequency, rrc_csp_band_class_type curr_band)
{
  
  boolean ret_status = FALSE;

  /* Check if the absolute frequency lies within the range
   * of the current band being scanned 
   */
  if((curr_band == RRC_CSP_IMT2K_BAND) &&
     (frequency >= 19224) && (frequency <= 19776))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_1700_2100_BAND) &&
         (frequency >= 17124) && (frequency <= 17526))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_1800_BAND) &&
          (frequency >= 17124) && (frequency <= 17826))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_1700_BAND) &&
         (frequency >= 17524) && (frequency <= 17824))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_850_BAND) &&
         (frequency >= 8264) && (frequency <= 8466))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_800_BAND) &&
          (frequency >= 8324) && (frequency <= 8376))
    ret_status = TRUE;

  else if((curr_band == RRC_CSP_WCDMA_BC19_BAND) &&
          (frequency >= 8324) && (frequency <= 8426))
    ret_status = TRUE;

  if(ret_status == FALSE)
  WRRC_MSG2_ERROR("WCDMA frequency %d invalid in band %d",frequency,rrc_csp_curr_select_data.current_band);
  
  return (ret_status);
}


/*===========================================================================

FUNCTION          RRCCSP_GET_UARFCN

DESCRIPTION       This function translates barred UARFCN to the one corresponding to the
                  current band
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if the translated UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrccsp_get_uarfcn (rrc_freq_type barred_uarfcn, rrc_csp_band_class_type barred_band, rrc_csp_band_class_type current_band, rrc_freq_type *uarfcn)
{
   uint16 absol_freq, fdl_offset;
   uint16 i;
   boolean addl_channel = FALSE;
   
   fdl_offset = 0;
   absol_freq = rrccsp_derive_freq_from_uarfcn(barred_uarfcn,barred_band);

   /* If the barred UARFCN is an additional channel set the flag addl_channel to TRUE */
   if(barred_band == RRC_CSP_WCDMA_850_BAND)
   {
     i = 0;
     while(rrc_dl_band_c_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_dl_band_c_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }
   
   else if(barred_band == RRC_CSP_WCDMA_800_BAND)
   {
     i = 0;
     while(rrc_dl_band_d_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_dl_band_d_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }
   
   else if( barred_band == RRC_CSP_WCDMA_BC19_BAND)
   {
     i = 0;
     while(rrc_dl_band_j_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_dl_band_j_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }

   /* Find the offset corresponding to the current band */
   if(current_band == RRC_CSP_IMT2K_BAND)
   {
     fdl_offset = RRC_DEFAULT_FDL_OFFSET;
   }
   
   else if(current_band == RRC_CSP_WCDMA_1700_2100_BAND)
   {
     if(addl_channel)
     fdl_offset = RRC_FDL_OFFSET_BAND4_ADDL;
     else
     fdl_offset = RRC_FDL_OFFSET_BAND4;
   }
   
   else if(current_band == RRC_CSP_WCDMA_1800_BAND)
   {
     fdl_offset = RRC_FDL_OFFSET_BAND3;
   }

   else if(current_band == RRC_CSP_WCDMA_1700_BAND)
   {
     fdl_offset = RRC_DEFAULT_FDL_OFFSET;
   }
   
   else if((current_band == RRC_CSP_WCDMA_850_BAND) ||
           (current_band == RRC_CSP_WCDMA_800_BAND))
   {
     if(addl_channel)
     fdl_offset = RRC_FDL_OFFSET_BAND5_BAND6_ADDL;
     else
     fdl_offset = RRC_DEFAULT_FDL_OFFSET;
   }

   else if(current_band == RRC_CSP_WCDMA_BC19_BAND)
   {
     if(addl_channel)
     fdl_offset = RRC_FDL_OFFSET_BAND19_ADDL;
     else
     fdl_offset = RRC_FDL_OFFSET_BAND19;
   }
   
   else
     WRRC_MSG0_ERROR("It is not expected to come here");
   

     /* UARFCN*10 = 5*(FDL-FDLoffset)   FDL and FDLoffset are in integral multiples of 100KHZ
      * FDL = 2*UARFCN + FDLoffset
      */
   
   *uarfcn = ((absol_freq - fdl_offset) / 2);
   
   return (rrccsp_validate_ctr_freq(absol_freq, current_band));
  
}
/*===========================================================================

FUNCTION          RRCCSP_GET_UARFCN_UL

DESCRIPTION       This function translates barred UL UARFCN to the one corresponding to the
                  current band
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if the translated UL UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrccsp_get_uarfcn_ul(rrc_freq_type barred_uarfcn, rrc_csp_band_class_type barred_band, rrc_csp_band_class_type current_band, rrc_freq_type *uarfcn)
{
   uint16 absol_freq, ful_offset;
   uint16 i;
   boolean addl_channel = FALSE;
   
   ful_offset = 0;
   absol_freq = rrccsp_derive_freq_from_uarfcn_ul(barred_uarfcn,barred_band);

   /* If the barred UARFCN is an additional channel set the flag addl_channel to TRUE */
   if(barred_band == RRC_CSP_WCDMA_850_BAND)
   {
     i = 0;
     while(rrc_ul_band_c_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_ul_band_c_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }

   else if(barred_band == RRC_CSP_WCDMA_800_BAND)
   {
     i = 0;
     while(rrc_ul_band_d_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_ul_band_d_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }

   else if( barred_band == RRC_CSP_WCDMA_BC19_BAND)
   {
     i = 0;
     while(rrc_ul_band_j_addl_channels[i] != 0)
     {
       if(barred_uarfcn == rrc_ul_band_j_addl_channels[i])
       {
         addl_channel = TRUE;
         break;
       }
       i++;
     };
   }

   /* Find the offset corresponding to the current band */
   if(current_band == RRC_CSP_IMT2K_BAND)
   {
     ful_offset = RRC_DEFAULT_FUL_OFFSET;
   }

   else if(current_band == RRC_CSP_WCDMA_1700_2100_BAND)
   {
     if(addl_channel)
     {
       ful_offset = RRC_FUL_OFFSET_BAND4_ADDL;
     }
     else
     {
       ful_offset = RRC_FUL_OFFSET_BAND4;
     }
   }

   else if(current_band == RRC_CSP_WCDMA_1800_BAND)
   {
     ful_offset = RRC_FUL_OFFSET_BAND3;
   }

   else if(current_band == RRC_CSP_WCDMA_1700_BAND)
   {
     ful_offset = RRC_DEFAULT_FUL_OFFSET;
   }

   else if((current_band == RRC_CSP_WCDMA_850_BAND) ||
           (current_band == RRC_CSP_WCDMA_800_BAND))
   {
     if(addl_channel)
     {
       ful_offset = RRC_FUL_OFFSET_BAND5_BAND6_ADDL;
     }
     else
     {
       ful_offset = RRC_DEFAULT_FUL_OFFSET;
     }
   }

   else if(current_band == RRC_CSP_WCDMA_BC19_BAND)
   {
     if(addl_channel)
     {
       ful_offset = RRC_FUL_OFFSET_BAND19_ADDL;
     }
     else
     {
       ful_offset = RRC_FUL_OFFSET_BAND19;
     }
   }

   else
   {
     WRRC_MSG0_ERROR("It is not expected to come here");
   }      
   
   *uarfcn = ((absol_freq - ful_offset) / 2);
   
   return (rrccsp_validate_ctr_freq_ul(absol_freq, current_band));
  
}
/*===========================================================================

FUNCTION          RRCCSP_SHOULD_FREQ_BE_SKIPPED

DESCRIPTION       This function determines based on previous acquired frequencies,
                  whether a new frequency should be scanned or skipped.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if frequency should be skipped
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrccsp_should_freq_be_skipped(rrc_freq_type freq, uint16 resolution)
{
  boolean ret_status = FALSE;
  uint16  i;
  rrc_csp_band_class_type curr_band = 0;
  rrc_plmn_identity_type plmn_id;

  rrc_csp_band_class_type band_class = rrc_csp_curr_select_data.current_band;

  if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) && (rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG))
  {
    if(rrc_ccm_get_curr_camped_freq() == freq)
    {
      ret_status = TRUE;
      /*Derive current band. It might have got set to different band if serving frequency is 
        not first entry in ACQ DB*/
      rrc_csp_curr_select_data.current_band = rrc_get_frequency_band(freq, rrc_ccm_get_curr_camped_plmn());
      rrc_csp_add_freq_to_available_freq_list(freq);
      rrc_csp_curr_select_data.current_band = band_class;
      return ret_status;
    }
  }

  if(((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))&&
     (rrc_csp_int_data.bplmn_acq_succ_freq == freq))
  {
    return ret_status;
  }

#ifdef FEATURE_FEMTO_CSG
  /*For manual CSG searches change fine and raw scan resolutions*/
  if((rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)&&
     ((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)||
      (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN))&&
      (wcdma_csg_srch_carrier_space_nv == TRUE))
  {
    if(resolution == RRCCSP_RAW_SCAN_SKIP_RANGE)
    {
      resolution = RRCCSP_RAW_SCAN_SKIP_CSG_RANGE;
    }
    else
    {
      resolution = RRCCSP_FINE_SCAN_SKIP_CSG_RANGE;
    }
    
  }
#endif
  /* Dont skip Last camped frequency in idle mode once for every raw scan*/
  if((rrc_get_state() == RRC_STATE_DISCONNECTED) 
     && (freq == rrc_csp_int_data.last_camped_freq_in_idle_mode)
     && (rrc_csp_int_data.skip_last_camped_freq_in_idle_mode == FALSE))
  {
    rrc_csp_int_data.skip_last_camped_freq_in_idle_mode = TRUE;
    WRRC_MSG0_HIGH("Do not skip last camped freq in idle mode once for every raw scan ");
    return ret_status; 
  }
  /* Don't skip the frequency if the freq passed is the same as last
   * camped frequency and scan type is NOT WTOW BPLMN scan 
   */
  if(   (rrc_get_state() != RRC_STATE_DISCONNECTED) 
     && (freq == rrc_csp_int_data.last_camped_freq_in_connected_mode)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
  {
    return ret_status; 
  }

  for(i=0; i<rrc_csp_curr_select_data.num_available_freq; i++)
  {
    if((freq >= rrc_csp_curr_select_data.available_freq_list[i] - resolution) &&
       (freq <= rrc_csp_curr_select_data.available_freq_list[i] + resolution))
    {
      if((resolution == RRCCSP_RAW_SCAN_SKIP_RANGE)
#ifdef FEATURE_FEMTO_CSG
        || (resolution == RRCCSP_RAW_SCAN_SKIP_CSG_RANGE)
#endif
        )
      {
        MSG_HIGH("Raw Freq %d skipped, prev_scanned %d, resolution %d", 
                   freq, rrc_csp_curr_select_data.available_freq_list[i],resolution);
      }
      else
      {
        WRRC_MSG2_HIGH("Fine Freq %d skipped, prev_scanned %d", 
                   freq, rrc_csp_curr_select_data.available_freq_list[i]);
      }

      return(ret_status = TRUE);
    }
  }
  
   /* Do not skip the frequency if it is barred while scanning the other overlapping band */
   if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) &&
      (rrc_csp_curr_select_data.num_of_barred_freq != 0) && 
      (rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_ACQ_SCAN) && 
      (rrccsp_should_freq_be_scanned_without_skipping(freq))) 
   {
     WRRC_MSG1_HIGH("Do not skip WCDMA ARFCN %d",freq);
     return(ret_status = FALSE);
   }

   /* Skip the frequency if it is already scanned as a part of other overlapping band 
    * Atleast one band should have been scanned completely
    */
   if(((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)) &&
       (rrc_csp_curr_select_data.scanned_band_mask))
   {
     WRRC_MSG1_HIGH("Current Scan Type is %d",rrc_csp_int_data.curr_scan);
     if(rrccsp_should_overlapping_freq_be_skipped(freq))
     {
       return(ret_status = TRUE);
     }
   }

#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) || 
     (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) || 
     (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) || 
     (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) || 
     (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN))
  {
    uint16 frequency; /*integral multiple of 100KHz*/
    uint16 idx;
    uint8 lte_resol;
    uint8 gsm_resol;

    /* During acq db scan, current band will not be updated in curr select data 
       Derive the absolute frequency by passing the band class from UARFCN */
    
    if(rrc_csp_curr_select_data.acq_db_in_use)
    {
      memset(plmn_id.mcc, 0, RRC_MAX_MCC_DIGITS * sizeof(byte));
      curr_band = rrc_get_frequency_band(freq,plmn_id);
    }
    else
    {
      curr_band = rrc_csp_curr_select_data.current_band;
    }

    frequency = rrccsp_derive_freq_from_uarfcn(freq,curr_band);
    if((resolution == RRCCSP_RAW_SCAN_SKIP_RANGE)
#ifdef FEATURE_FEMTO_CSG
      ||       (resolution == RRCCSP_RAW_SCAN_SKIP_CSG_RANGE)
#endif
      )
    {
      lte_resol = RRCCSP_LTE_CHANNEL_RAW_SCAN_SKIP_RANGE;
      gsm_resol = RRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE * 2;
    }
    else
    {
      lte_resol = RRCCSP_LTE_CHANNEL_FINE_SCAN_SKIP_RANGE;
      gsm_resol = RRCCSP_GSM_CHANNEL_FINE_SCAN_SKIP_RANGE * 2;
    }
    for(idx = 0;idx<rrc_csp_int_data.detected_frequency_list.num_freqs && idx < LTE_IRAT_MAX_DETECTED_FREQ;idx++)
    {
      if(rrc_csp_int_data.detected_frequency_list.freq[idx].rat == SYS_RAT_LTE_RADIO_ACCESS)
      {
        uint16 bw = rrc_csp_int_data.detected_frequency_list.freq[idx].bandwidth;
        if((frequency >= rrc_csp_int_data.detected_frequency_list.freq[idx].frequency - (bw/2+lte_resol))&&
           (frequency <= rrc_csp_int_data.detected_frequency_list.freq[idx].frequency + (bw/2+lte_resol)))
        {
          WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
          return (ret_status = TRUE);
        }
      }
      else if(rrc_csp_int_data.detected_frequency_list.freq[idx].rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        if((frequency >= rrc_csp_int_data.detected_frequency_list.freq[idx].frequency - gsm_resol)&&
         (frequency <= rrc_csp_int_data.detected_frequency_list.freq[idx].frequency + gsm_resol))
        {
          WRRC_MSG2_HIGH("Skip WCDMA ARFCN %d , frequency in 100KHz units %d",freq,frequency);
          return (ret_status = TRUE);
        }
      }
    }
  }
#endif

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION

  if(   (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
     #ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
     || (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
     #endif
    )
  {

    for(i=0; i<rrc_csp_int_data.umts_chls_from_gsm.size; i++)
    {

      if((resolution == RRCCSP_RAW_SCAN_SKIP_RANGE)
#ifdef FEATURE_FEMTO_CSG
        ||   (resolution == RRCCSP_RAW_SCAN_SKIP_CSG_RANGE)
#endif
        )
      {
        /* GSM channel skip range is smaller because the GSM channel bandwidth is different
         * from that of WCDMA.
         */
        if((freq >= rrc_csp_int_data.umts_chls_from_gsm.cells[i].num - RRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE) &&
           (freq <= rrc_csp_int_data.umts_chls_from_gsm.cells[i].num + RRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE))
        {
          MSG_HIGH("Freq %d skipped, GSM prev_scanned %d, resolution %d", 
                        freq, rrc_csp_int_data.umts_chls_from_gsm.cells[i].num,RRCCSP_GSM_CHANNEL_RAW_SCAN_SKIP_RANGE);

          ret_status = TRUE;
          break;
        }
      }
      else
      {
        /* GSM channel skip range is smaller because the GSM channel bandwidth is different
         * from that of WCDMA.
         */
        if((freq >= rrc_csp_int_data.umts_chls_from_gsm.cells[i].num - RRCCSP_GSM_CHANNEL_FINE_SCAN_SKIP_RANGE) &&
           (freq <= rrc_csp_int_data.umts_chls_from_gsm.cells[i].num + RRCCSP_GSM_CHANNEL_FINE_SCAN_SKIP_RANGE))
        {
          WRRC_MSG2_HIGH("Freq %d skipped, GSM prev_scanned %d", 
                        freq, rrc_csp_int_data.umts_chls_from_gsm.cells[i].num);

          ret_status = TRUE;
          break;
        }
      }
    }
  }
#endif

  return ret_status;

} /* rrccsp_should_freq_be_skipped */


/*===========================================================================

FUNCTION          RRCCSP_IS_FINE_SCAN_REQUIRED

DESCRIPTION       This function determines if a fine scan is required or not.
                  It loops through the fine scan results and skips the result,
                  if it falls within the RAW_SCAN_SKIP_RANGE. 
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if Fine scan is required,
                  FALSE otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_is_fine_scan_required(rrc_freq_type* ret_freq)
{
  boolean fine_scan_required = TRUE;
  int16  curr_freq_index;

  /* There are no more frequencies in fine scan or we've covered all of 
   * them for acquisition - pick the next freq. in raw scan list for fine scan
   */
  if((rrc_csp_curr_select_data.scan_info.raw_scan.num_freq > 0) &&
     (rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index <
        rrc_csp_curr_select_data.scan_info.raw_scan.num_freq -1))
  {
    /* There is atleast one more freq in raw scan list that can be tried for
     * fine scan.
     */
    RRC_CSP_PICK_NEXT_RAW_FREQ_FOR_FINE_SCAN();
  }
  else
  {
    return FALSE;
  }

  while(rrccsp_should_freq_be_skipped(*ret_freq, RRCCSP_RAW_SCAN_SKIP_RANGE))
  {
    if((rrc_csp_curr_select_data.scan_info.raw_scan.curr_freq_index <
       rrc_csp_curr_select_data.scan_info.raw_scan.num_freq -1))
    {
      RRC_CSP_PICK_NEXT_RAW_FREQ_FOR_FINE_SCAN();
    }
    else
    {
      fine_scan_required = FALSE;
      break;
    }
  }

  return fine_scan_required;

} /* rrccsp_is_fine_scan_required */


/*===========================================================================

FUNCTION          RRCCSP_IS_ACQ_SCAN_REQUIRED

DESCRIPTION       This function determines if an acquisition is required or not.
                  It loops through the fine scan results and skips the result,
                  if it falls within the FINE_SCAN_SKIP_RANGE. 
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if Acquisition is required,
                  FALSE otherwise

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_is_acq_scan_required(rrc_freq_type* ret_freq)
{
  boolean acq_scan_required = TRUE;

  while(rrccsp_should_freq_be_skipped(*ret_freq, RRCCSP_FINE_SCAN_SKIP_RANGE))
  {
    if((rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index <
       rrc_csp_curr_select_data.scan_info.fine_scan.num_freq -1))
    {
      RRC_CSP_PICK_NEXT_FINE_FREQ_FOR_ACQ();
    }
    else
    {
      acq_scan_required = FALSE;
      break;
    }
  }

  return acq_scan_required;

} /* rrccsp_is_acq_scan_required */

#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */


/*===========================================================================

FUNCTION          RRC_CSP_INIT_ADDL_CHNL_SCAN_DATA

DESCRIPTION       This function determines if additional channel scan is needed
                  or not for a particular band
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_init_addl_chnl_scan_data()
{
#ifdef FEATURE_WCDMA_MULTIBAND

  if((rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
  || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
  || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
  || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
  || (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
  )
  {
    WRRC_MSG1_HIGH("Addl chl scan needed for band %d", rrc_csp_curr_select_data.current_band);
    rrc_csp_curr_select_data.addl_chls_scan_needed = TRUE;
  }
  else
  {
    WRRC_MSG1_HIGH("Addl chl scan NOT needed for band %d",rrc_csp_curr_select_data.current_band);
    rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;
  }

#else

  rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;

#endif

} /* rrc_csp_init_addl_chnl_scan_data */


/*===========================================================================

FUNCTION          RRC_CSP_GET_ADDL_CHANNEL_BAND

DESCRIPTION       This function returns the additional channel scan frequency
                  
DEPENDENCIES      None

RETURN VALUE      returns the additional channel frequency
                  
SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_get_addl_channel_band(rrc_freq_type *ret_freq)
{
  WRRC_MSG1_HIGH("starting Addl chl scan on band %d", rrc_csp_curr_select_data.current_band);

  rrc_csp_curr_select_data.add_barr_freq_to_fine_res = TRUE;
  
  rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_ADDL_CHLS_SCAN;

  rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;

  if(rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
  {
    *ret_freq = WCDMA_FS_RX_CHAN_PCS_ADDITIONAL_MIN;
  }
  else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
  {
    *ret_freq = WCDMA_FS_RX_CHAN_850_ADDITIONAL_MIN;
  }
  else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
  {
    *ret_freq = WCDMA_FS_RX_CHAN_800_ADDITIONAL_MIN;
  }
  else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
  {
    *ret_freq = WCDMA_FS_RX_CHAN_1700_2100_ADDITIONAL_MIN;
  }
  else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)  
  {
    *ret_freq = WCDMA_FS_RX_CHAN_EXTENDED_800_ADDITIONAL_MIN;
  }

  else
  {
    WRRC_MSG1_ERROR("No Addl chls for band %d", rrc_csp_curr_select_data.current_band);
  }

  rrc_csp_curr_select_data.addl_chnl_done_mask |= rrc_csp_curr_select_data.current_band;
  WRRC_MSG1_HIGH("Addl: addl_chnl_done_mask = %d", rrc_csp_curr_select_data.addl_chnl_done_mask);
  
} /* rrc_csp_get_addl_channel_band */



/*===========================================================================

FUNCTION          RRC_CSP_GET_NEXT_FREQ_SCAN_ENTRY

DESCRIPTION       This function is responsible for determining the next
                  frequency scan type and returning the frequency.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  SUCCESS: Able to find a frequency where a Raw or Fine scan
                           or an Acquisition can be tried.
                  FAILURE: All frequencies exhausted.

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_get_next_freq_scan_entry(rrc_freq_type *ret_freq)
{
  int16  curr_freq_index;
  uecomdef_status_e_type  return_value = FAILURE;

  if(rrc_csp_curr_select_data.acq_db_in_use)
  {
    WRRC_MSG0_ERROR("Frequency scan not active");
    return(FAILURE);
  }

#ifdef FEATURE_EOOS
  #error code not present
#endif


  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
    "addl: addl chl scan needed =  %d, curr_scan %d,plmn_list_freq_scan_needed %d,manual_plmn_list_freq_scan_needed %d",
     rrc_csp_curr_select_data.addl_chls_scan_needed, rrc_csp_int_data.curr_scan,  rrc_csp_int_data.plmn_list_freq_scan_needed,
                                                rrc_csp_int_data.manual_plmn_list_freq_scan_needed);

  switch(rrc_csp_curr_select_data.scan_info.type_freq_scan)
  {
    case RRC_CSP_NO_SCAN:
    {
      /* This is the begining of frequency scan. Start it with a Raw scan of the
       * entire band. Always start frequency scan for certain scan types even if
       * the frequency scan timer hasn't expired
       */
      if(((rrc_csp_int_data.ue_in_deep_sleep) && 
          (!rrc_csp_int_data.full_scan_needed) &&
           (!rrc_csp_int_data.acq_db_empty) &&
            (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
             (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
              (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) &&
               ((rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask) == 0) &&
                (((rrc_csp_int_data.deep_sleep_band_scanned_mask & rrc_csp_int_data.wcdma_band_mask)
                                                              ^ rrc_csp_int_data.wcdma_band_mask) == 0)
#ifdef FEATURE_EOOS
                 #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
                 && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                 && (rrc_csp_int_data.curr_scan != RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN))
#ifdef FEATURE_FEMTO_CSG
                 ||((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
                    &&(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
                    &&(rrc_csp_int_data.curr_req_plmn.csg_id != SYS_CSG_ID_INVALID))
#endif
        )
      {
        WRRC_MSG1_HIGH("Full scan not needed, Scan type %d",rrc_csp_int_data.curr_scan);
        return_value = FAILURE;
      }
      /* Different timers are used for Automatic & Manual searches so that customers can set the timer values
        * independently based on their needs 
        */
      else if ( ( (((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) ||
                    (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)) &&
                  (!rrc_csp_int_data.plmn_list_freq_scan_needed)) 
                  ||
                (((rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) ||
                   (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) ||
                   (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)) &&
                 (!rrc_csp_int_data.manual_plmn_list_freq_scan_needed)) ) &&
             (!rrc_csp_int_data.acq_db_empty)
#ifdef FEATURE_FEMTO_CSG
             && !((rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG) &&
                  ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) ||
                   (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)))
#endif
         )
      {
        WRRC_MSG1_HIGH("BPLMN/WTOW full FreqScan not needed, Scan type %d",rrc_csp_int_data.curr_scan);
        return_value = FAILURE;
      }   
      else
      {
        if(rrc_csp_curr_select_data.addl_chls_scan_needed)
        {
          rrc_csp_get_addl_channel_band(ret_freq);
        }
        else
        {
          rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_RAW_SCAN;

          /* Init Raw scan data */
          rrc_csp_curr_select_data.scan_info.raw_scan_completed = FALSE;
        


    #ifdef FEATURE_WCDMA_MULTIBAND
          if(rrc_csp_curr_select_data.current_band == RRC_CSP_IMT2K_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_PCS_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_850_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_800_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1800_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_1800_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_900_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_900_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_1700_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_1700_2100_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1500_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_1500_MIN;
          }
          else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
          {
            *ret_freq = WCDMA_FS_RX_CHAN_EXTENDED_800_MIN; 
          }
          else
          {
            WRRC_MSG1_ERROR("Invalid current band %d",rrc_csp_curr_select_data.current_band);
          }
  
    #else
          *ret_freq = WCDMA_FS_RX_CHAN_MIN;
    #endif /* FEATURE_WCDMA_MULTIBAND */
          
          

          WRRC_MSG1_HIGH("Starting raw scan on UARFCN %d", *ret_freq);
          rrc_csp_int_data.skip_last_camped_freq_in_idle_mode = FALSE;

          /* This will ensure that barred freq's corresponding to the current band
           * are added only to the first fine scan result
           */
          rrc_csp_curr_select_data.add_barr_freq_to_fine_res = TRUE;

        }
        
        return_value = SUCCESS;
      }
    }
    break;

    case RRC_CSP_RAW_SCAN:
    {
      if(rrc_csp_curr_select_data.scan_info.raw_scan_completed == FALSE)
      {
        WRRC_MSG0_ERROR("Wrong call to fn: Raw scan not finished");
        return(FAILURE);
      }
      
      /* Raw Scan has been completed. If there are non-zero entries in Raw list,
       * start the fine scan with the strongest raw freq. entry. If there are no 
       * entries in raw scan list, we are pretty much doomed - Return a failure.
       */
      if(rrc_csp_curr_select_data.scan_info.raw_scan.num_freq == 0)
      {
        
        WRRC_MSG0_HIGH("Freq scan - no more frequency");
        return(FAILURE);
      }
      else  /* Raw scan got some frequencies */
      {
        /* We got some good frequencies in raw scan - proceed to fine scan */
        RRC_CSP_PICK_FIRST_RAW_FREQ_FOR_FINE_SCAN();
        WRRC_MSG1_HIGH("Starting fine scan on UARFCN %d", *ret_freq);
        return_value = SUCCESS;
      }
    }   /* In Raw Scan */
    break;

    case RRC_CSP_FINE_SCAN:
    {
      if(rrc_csp_curr_select_data.scan_info.fine_scan_completed == FALSE)
      {
        WRRC_MSG0_ERROR("Wrong call to fn: Fine scan not finished");
        return(FAILURE);
      }
      /* If there are any frequencies in fine scan list that have not been
       * tried for acquisition, get the next fine scan frequency and return it
       * to be tried for acquisition. 
       */
      if(rrc_csp_curr_select_data.scan_info.fine_scan.num_freq > 0)
      {
#ifdef FEATURE_EOOS
#error code not present
#endif
        
        RRC_CSP_PICK_FIRST_FINE_FREQ_FOR_ACQ();

        if(rrccsp_is_acq_scan_required(ret_freq))
        {
          WRRC_MSG1_HIGH("Starting acq on UARFCN %d", *ret_freq);

#ifdef FEATURE_EOOS
          #error code not present
#endif
          
          return_value = SUCCESS;
        }
        else
        {
          if(rrccsp_is_fine_scan_required(ret_freq))
          {
            WRRC_MSG1_HIGH("Starting fine scan on UARFCN %d", *ret_freq);
            return_value = SUCCESS;
          }
          else
          {
            
            WRRC_MSG0_HIGH("Freq scan - no more frequency");
            return(FAILURE);
          }
        }
#ifdef FEATURE_EOOS
        #error code not present
#endif
      }
      else
      {
        /* There are no more frequencies in fine scan or we've covered all of 
         * them for acquisition - pick the next freq. in raw scan list for fine scan
         */
        if(rrccsp_is_fine_scan_required(ret_freq))
        {
          WRRC_MSG1_HIGH("Starting fine scan on UARFCN %d", *ret_freq);
          return_value = SUCCESS;
        }
        else
        {
          WRRC_MSG0_HIGH("Freq scan - no more frequency");
          return(FAILURE);
        }
      }   /* No more freq for acq in fine scan list */
    } /* In Fine Scan */
    break;

    case RRC_CSP_ACQ_SCAN:
    {
      /* Move on to the next frequency in fine scan list and try acquisition there. If 
       * there is no more frequency in fine scan list to be tried, pick the next one 
       * from raw scan list and try a fine scan there. If raw scan does not have a 
       * frequency either, return a failure.
       */
#ifdef FEATURE_EOOS
      #error code not present
#endif

      if((rrc_csp_curr_select_data.scan_info.fine_scan.num_freq > 0) &&
         (rrc_csp_curr_select_data.scan_info.fine_scan.curr_freq_index < 
          rrc_csp_curr_select_data.scan_info.fine_scan.num_freq -1))
      {
        /* Pick the next fine scan frequency and try acquisition there */
        RRC_CSP_PICK_NEXT_FINE_FREQ_FOR_ACQ();

        if(rrccsp_is_acq_scan_required(ret_freq))
        {
          WRRC_MSG1_HIGH("Starting acq on UARFCN %d", *ret_freq);

#ifdef FEATURE_EOOS
          #error code not present
#endif

          return_value = SUCCESS;
        }
        else
        {
          if(rrccsp_is_fine_scan_required(ret_freq))
          {
            WRRC_MSG1_HIGH("Starting fine scan on UARFCN %d", *ret_freq);
            return_value = SUCCESS;
          }
          else
          {
            WRRC_MSG0_HIGH("Freq scan - no more frequency");
            return(FAILURE);
          }
        }   /* No more freq for acq in fine scan list */
      }
      else
      {
        if(rrccsp_is_fine_scan_required(ret_freq))
        {
          WRRC_MSG1_HIGH("Starting fine scan on UARFCN %d", *ret_freq);
          return_value = SUCCESS;
        }
        else
        {
          WRRC_MSG0_HIGH("Freq scan - no more frequency");
          return(FAILURE);
          }
      }
#ifdef FEATURE_EOOS
      #error code not present
#endif

    } /* In Acq scan */
    break;

    default:
      WRRC_MSG1_ERROR("Incorrect scan type %d", rrc_csp_curr_select_data.scan_info.type_freq_scan);
      return(FAILURE);

  } /* switch type_freq_scan */

  return(return_value);

} /* rrc_csp_get_next_freq_scan_entry */


/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_FREQ_SCAN_ENTRY

DESCRIPTION       This function is responsible for updating frequency scan
                  data in current selection data and list frequency scan
                  data. Note that only the raw frequency scan list, fine frequency 
                  scan list and list frequency scan list need to be updated.

DEPENDENCIES
                  The L1 command data has to be validated before calling
                  this function.

RETURN VALUE
                  SUCCESS: If received scan type matches with the expected
                           scan type.
                  FAILURE: Discrepency between received and expected scan 
                           types.
SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_update_freq_scan_entry(l1_freq_scan_cnf_type *freq_scan_cnf)
{
  uint32 i;
  rrc_state_e_type rrc_state = rrc_get_state();

  uint32    p,q,r;
  uint32    list_db_ix = 0;

  uint16        freq_rd_idx;
  rrc_freq_type uarfcn;

#ifdef FEATURE_EOOS
  #error code not present
#endif

  uint16  j,k;
  uint8 temp_plmn;
  uint16 temp_freq;
  rrc_plmn_identity_type temp_plmn_id;
  int spec_plmn_count;


  if( (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
      (freq_scan_cnf->step == L1_FREQ_LIST_SCAN) )
  {
    rrc_csp_curr_select_data.scan_info.list_scan.num_freq  = freq_scan_cnf->num_freq;
    WRRC_MSG1_HIGH("LFS: LFS cnf rcvd from L1 with #freqs = %d",
                       rrc_csp_curr_select_data.scan_info.list_scan.num_freq);

    /*           --------------------
     *           PRINT SORTED LIST DB 
     *           --------------------
     */

    WRRC_MSG0_HIGH("____________________________");
    WRRC_MSG0_HIGH("LFS: Printing LFS results");
    WRRC_MSG0_HIGH("____________________________");
    for(i = 0; i < freq_scan_cnf->num_freq; ++i)
    {
      WRRC_MSG1_HIGH("LFS: Freq = %d", freq_scan_cnf->freq_result[i].ctr_freq);
    }

    for(p = 0; (p < rrc_csp_curr_select_data.scan_info.list_scan.num_freq) && (list_db_ix < MAX_ACQ_DB_ENTRIES); ++p)
    {
      for(q = 0; (q < rrc_csp_int_data.target_fdd_cell_list.num_of_frequencies) && (list_db_ix < MAX_ACQ_DB_ENTRIES); q++)
      {
        if(freq_scan_cnf->freq_result[p].ctr_freq == rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].uarfcn)
        {
          if(rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].num_of_cells < 1)
          {
            /* Only frequency is given. Add it with scr_code set to invalid value */
            list_db[list_db_ix].freq = (uint16) rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].uarfcn;
            list_db[list_db_ix].scr_code = DEFAULT_SCR_CODE;
            list_db_ix++;
            if(list_db_ix >= MAX_ACQ_DB_ENTRIES)
            {
              WRRC_MSG0_HIGH("GtoW Redirection: Reached MAX_ACQ_DB_ENTRIES, Ignore the rest");
              break;
            }
          }
          else
          {
            for(r = 0; r < rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].num_of_cells; r++)
            {
              list_db[list_db_ix].freq = (uint16) rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].uarfcn;
              list_db[list_db_ix].scr_code = (uint16) rrc_csp_int_data.target_fdd_cell_list.fdd_cell_list[q].cell_list[r].scrambling_code;
              list_db_ix++;
              if(list_db_ix >= MAX_ACQ_DB_ENTRIES)
              {
                WRRC_MSG0_HIGH("GtoW Redirection: Reached MAX_ACQ_DB_ENTRIES, Ignore the rest");
                break;
              }
            }
          }
        }
      }
    }
#ifdef FEATURE_EOOS
    #error code not present
#else
    rrc_csp_int_data.num_freq_in_list_db = (uint8) list_db_ix;
    rrc_csp_int_data.curr_rd_idx = 0;
#endif
    return (SUCCESS);
  }

#ifdef FEATURE_EOOS
  #error code not present
#endif /* FEATURE_EOOS */

  if((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_RAW_SCAN) && 
     (freq_scan_cnf->step == L1_FREQ_RAW_SCAN))
  {
    /* Update the Raw Scan list */
    rrc_csp_curr_select_data.scan_info.raw_scan_completed = TRUE;
    WRRC_MSG1_HIGH("Num freq in RScanCnf %d", freq_scan_cnf->num_freq);

    /*lint -e506*/
    if(freq_scan_cnf->num_freq > L1_MAX_RAW_SCAN_FREQ)
    /*lint +e506*/
    {
      WRRC_MSG1_ERROR("Invalid num_freq %d in RScan Cnf", freq_scan_cnf->num_freq); 
      rrc_csp_curr_select_data.scan_info.raw_scan.num_freq = 0;
    }
    else
    {
      /*lint -e506*/
      rrc_csp_curr_select_data.scan_info.raw_scan.num_freq = 
        MIN(freq_scan_cnf->num_freq, L1_MAX_RAW_SCAN_FREQ);
      /*lint +e506*/

      for(i=0; i < rrc_csp_curr_select_data.scan_info.raw_scan.num_freq; i++)        
      {
        rrc_csp_curr_select_data.scan_info.raw_scan.freq_result[i] = freq_scan_cnf->freq_result[i];
      } /* for (...) */
    }
    return(SUCCESS);
  }

  if((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_FINE_SCAN) && 
     /*lint -e506*/
     (freq_scan_cnf->step == L1_FREQ_FINE_SCAN))
     /*lint +e506*/
  {
    WRRC_MSG1_HIGH("Num freq in FnScanCnf %d", freq_scan_cnf->num_freq);
    /* Update the Fine Scan list */
    rrc_csp_curr_select_data.scan_info.fine_scan_completed = TRUE;
    
    WRRC_MSG1_HIGH("Num_freq %d in FnScan Cnf", freq_scan_cnf->num_freq); 

    /*lint -e506*/
    rrc_csp_curr_select_data.scan_info.fine_scan.num_freq = 
      MIN(freq_scan_cnf->num_freq, L1_MAX_FINE_SCAN_FREQ);

    /* In Connected mode, UE needs to do additional acquisitions on
     * the frequency it was camped on last, so we are adding it at 
     * the end of the fine scan list.
     */
    if(    (rrc_state != RRC_STATE_DISCONNECTED)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      if(  (rrc_csp_int_data.last_camped_freq_in_connected_mode  != 0)
        && (SUCCESS == validate_dl_arfcn(rrc_csp_int_data.last_camped_freq_in_connected_mode, 
                                     rrc_csp_curr_select_data.req_plmn.plmn_id,
                                     W_UARFCN_NOT_FROM_GSM_NBR_LIST
                                     , FALSE
                                     ))
       )
     
      {
        MSG_HIGH("Add freq %d, rssi -146 to fine scan list. Curr_scan %d, RRC state %d",
                   rrc_csp_int_data.last_camped_freq_in_connected_mode,rrc_csp_int_data.curr_scan,rrc_state);
        freq_scan_cnf->num_freq++;
        rrc_csp_curr_select_data.scan_info.fine_scan.num_freq =
            freq_scan_cnf->num_freq;
        freq_scan_cnf->freq_result[freq_scan_cnf->num_freq - 1].ctr_freq =
          rrc_csp_int_data.last_camped_freq_in_connected_mode;
        freq_scan_cnf->freq_result[freq_scan_cnf->num_freq - 1].rssi =  -146;
      }
      else
      {
        WRRC_MSG1_HIGH("Skipping LCF: %d in connected mode",rrc_csp_int_data.last_camped_freq_in_connected_mode);
      }
    }
    /* Add last camped frequency to every fine scan list if RAW scan
     * is NOT due to Manual or Automatic PLMN List Search; and also
     * NOT due to Inter Frequency Redirection */
    else if(   (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN)
            && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
            && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
            && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
            && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
            && (rrc_csp_int_data.curr_scan != RRC_CSP_INTER_FREQ_REDIRECTION_SCAN)
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
            && (SUCCESS == validate_dl_arfcn(rrc_csp_int_data.last_camped_freq_in_idle_mode, 
                                             rrc_csp_curr_select_data.req_plmn.plmn_id,
                                             W_UARFCN_NOT_FROM_GSM_NBR_LIST
                                             , FALSE
                                            )
               )
#else
            && (SUCCESS == validate_dl_arfcn(rrc_csp_int_data.last_camped_freq_in_idle_mode
                                             , FALSE
                                            )
               )
#endif
           )
    {
      MSG_HIGH("Idle mode: Add freq %d, rssi -146 to fine scan list. Curr_scan %d, RRC state %d",
                 rrc_csp_int_data.last_camped_freq_in_idle_mode,rrc_csp_int_data.curr_scan,rrc_state);
      freq_scan_cnf->num_freq++;
      rrc_csp_curr_select_data.scan_info.fine_scan.num_freq = freq_scan_cnf->num_freq;
      freq_scan_cnf->freq_result[freq_scan_cnf->num_freq - 1].ctr_freq =
                                       rrc_csp_int_data.last_camped_freq_in_idle_mode;
      freq_scan_cnf->freq_result[freq_scan_cnf->num_freq - 1].rssi = -146;
    }

    /*lint +e506*/

#ifdef FEATURE_WCDMA_MULTIBAND
   if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) && (rrc_csp_curr_select_data.num_of_barred_freq != 0) &&
      (rrc_csp_curr_select_data.add_barr_freq_to_fine_res)
        )
    {
      WRRC_MSG0_HIGH("Adding overlapped freq's corresponding to the current band (if any) to the top of fine scan list");
      /* To ensure that the barred freq is added only to the top of first fine scan result */
      rrc_csp_curr_select_data.add_barr_freq_to_fine_res = FALSE;

      freq_rd_idx = 0;
              
      /* If current band being scanned is a part of dont skip band mask find UARFCN 
       * corresponding to the current band from the barred UARFCN
       */
      for(i=0; i < rrc_csp_curr_select_data.num_of_barred_freq; i++)
      {
        if(rrc_csp_curr_select_data.dont_skip_band_mask & rrc_csp_curr_select_data.current_band)
        {
          if(rrccsp_get_uarfcn(rrc_csp_curr_select_data.barred_freq[i], rrc_csp_curr_select_data.band_corr_barred_freq[i], 
                rrc_csp_curr_select_data.current_band, &uarfcn))
          {
            rrc_csp_curr_select_data.scan_info.fine_scan.freq_result[freq_rd_idx].ctr_freq = uarfcn;
            WRRC_MSG2_HIGH("Fine [%d], freq = %d, ",freq_rd_idx,uarfcn);
            freq_rd_idx++;
          }
        }
      }
      
      rrc_csp_curr_select_data.scan_info.fine_scan.num_freq += freq_rd_idx;

      for(i = freq_rd_idx; i < rrc_csp_curr_select_data.scan_info.fine_scan.num_freq; i++)      
      {
         rrc_csp_curr_select_data.scan_info.fine_scan.freq_result[i] = 
           freq_scan_cnf->freq_result[i - freq_rd_idx];
      } /* for (...) */
    }
    else
#endif /* FEATURE_WCDMA_MULTIBAND */
    {
      for(i=0; i < rrc_csp_curr_select_data.scan_info.fine_scan.num_freq; i++)      
      {
        rrc_csp_curr_select_data.scan_info.fine_scan.freq_result[i] = 
          freq_scan_cnf->freq_result[i];
      } /* for (...) */
    }


    return(SUCCESS);
  }

  if((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_LIST_SCAN) && 
     (freq_scan_cnf->step == L1_FREQ_LIST_SCAN))
  {
    WRRC_MSG1_HIGH("LFS_COOS: LFS cnf rcvd from L1 with #freqs = %d",
                       freq_scan_cnf->num_freq);
    /* Store the resultant (Sorted on RSSI)  */
    /* Separate out the frequencies based on the PLMN type
       This code will arrange the frequencies of SPECIFIC PMN 
       and ANY PLMN in the order from top to bottom
    */
    
    spec_plmn_count = 0;
    for(i=0;(i<freq_scan_cnf->num_freq);i++)
    {
      if((rrc_csp_acq_e_type)freq_scan_cnf->freq_result[i].result_plmn_type == RRC_CSP_SPEC_PLMN)
      {
        for(j=spec_plmn_count;j<rrc_csp_curr_select_data.scan_info.list_scan.num_freq;j++)
        {
          if(freq_scan_cnf->freq_result[i].ctr_freq == 
                     rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].ctr_freq)
          {
            temp_freq  = rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].ctr_freq;
            temp_plmn = rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].result_plmn_type;
            temp_plmn_id = rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[j];
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j] = 
                   rrc_csp_curr_select_data.scan_info.list_scan.freq_result[spec_plmn_count];
            rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[j] = 
                   rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[spec_plmn_count];
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[spec_plmn_count].ctr_freq = temp_freq;
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[spec_plmn_count].result_plmn_type = temp_plmn;
            rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[spec_plmn_count] = temp_plmn_id;
            spec_plmn_count++;
            break;
          }
        }
      }
    }
    k = spec_plmn_count;
    for(i=0;(i<freq_scan_cnf->num_freq);i++)
    {
      if((rrc_csp_acq_e_type)freq_scan_cnf->freq_result[i].result_plmn_type == RRC_CSP_ANY_PLMN)
      {
        for(j=k;j<rrc_csp_curr_select_data.scan_info.list_scan.num_freq;j++)
        {
          if(freq_scan_cnf->freq_result[i].ctr_freq ==  
                   rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].ctr_freq)
          {
            temp_freq  = rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].ctr_freq;
            temp_plmn = rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j].result_plmn_type;
            temp_plmn_id = rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[j];
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[j] = 
                 rrc_csp_curr_select_data.scan_info.list_scan.freq_result[k];
            rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[j] = 
                 rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[k];
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[k].ctr_freq = temp_freq;
            rrc_csp_curr_select_data.scan_info.list_scan.freq_result[k].result_plmn_type = temp_plmn;
            rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[k] = temp_plmn_id;              
            k++;
            break;
          }
        }
      }
    }  
    rrc_csp_curr_select_data.scan_info.list_scan.num_freq = freq_scan_cnf->num_freq;
    WRRC_MSG0_HIGH("____________________________");
    WRRC_MSG0_HIGH("LFS_COOS: Printing SORTED LIST DB");
    WRRC_MSG0_HIGH("____________________________");
    for(i = 0;  i < rrc_csp_curr_select_data.scan_info.list_scan.num_freq; ++i)
    {
      WRRC_MSG1_HIGH("LFS_COOS: Freq = %d", rrc_csp_curr_select_data.scan_info.list_scan.freq_result[i].ctr_freq);
      MSG_HIGH("LFS_COOS: PLMN-ID (%d - %d), PLMN-TYPE (%d)", 
                                    RRC_CSP_GET_INT_MCC(rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[i]), 
                                    RRC_CSP_GET_INT_MNC(rrc_csp_curr_select_data.scan_info.list_scan.plmn_id[i]), 
                                    rrc_csp_curr_select_data.scan_info.list_scan.freq_result[i].result_plmn_type);
      WRRC_MSG1_HIGH("LFS_COOS: From L1 Freq = %d", freq_scan_cnf->freq_result[i].ctr_freq);                                   
    }
    return(SUCCESS);
  }

#ifdef FEATURE_WCDMA_MULTIBAND
  if((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_ADDL_CHLS_SCAN) && 
     /*lint -e506*/
     (freq_scan_cnf->step == L1_FREQ_ADDITIONAL_CHANNEL_SCAN))
     /*lint +e506*/
  {
    rrc_csp_curr_select_data.scan_info.fine_scan_completed = TRUE;

    /* Reset the scan type to Fine Scan */
    rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_FINE_SCAN;

    WRRC_MSG1_HIGH("Num freq in FnScanCnf %d", freq_scan_cnf->num_freq);

    /*lint -e506*/
    rrc_csp_curr_select_data.scan_info.fine_scan.num_freq = freq_scan_cnf->num_freq;
    /*lint +e506*/

    for(i=0; i < rrc_csp_curr_select_data.scan_info.fine_scan.num_freq; i++)      
    {
      rrc_csp_curr_select_data.scan_info.fine_scan.freq_result[i] = 
        freq_scan_cnf->freq_result[i];
    } /* for (...) */

    return(SUCCESS);
  }
#endif /* FEATURE_WCDMA_MULTIBAND */
  else
  {
    WRRC_MSG2_ERROR("rrc_csp_update_freq_scan_entry returning failure. Invalid type curr scan %d or L1 scan %d",
      rrc_csp_curr_select_data.scan_info.type_freq_scan, freq_scan_cnf->step); 
    return(FAILURE);
  }
}   /* rrc_csp_update_freq_scan_entry */

/*===========================================================================

FUNCTION           RRCCSP_ADD_NBR_FREQ_BANDS_FROM_SIB11_TO_BAND_LIST

DESCRIPTION        Extracts inter frequency bands from SIB11/12 and adds them
                  to the band_list provided starting from num_of_bands_already_present.

DEPENDENCIES       num_of_bands_already_present should be initialized properly before calling
                   this function.

RETURN VALUE       number of bands present in the band list at the end.

SIDE EFFECTS       None
===========================================================================*/
static uint8 
rrccsp_add_nbr_freq_bands_from_sib11_to_band_list
(
  rrc_csp_band_class_type*    band_list,
  uint8 num_of_bands_already_present
)
{
  rrc_SysInfoType11*    sib11_ptr;
  rrc_SysInfoType12*    sib12_ptr;
  rrc_MeasurementControlSysInfo*    measurementControlSysInfo_ptr;
  boolean    is_hcs_used = FALSE;
  boolean    is_rscp_used = FALSE;
  rrc_freq_type    inter_freq_list[MAX_NO_OF_CELLS] = {0};
  uint8    num_of_inter_freqs = 0;
  uint8    i = 0;
  uint32 idx=0;
  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_plmn_identity_type plmn_id = rrc_ccm_get_curr_camped_plmn();
  rrc_int_cell_id_type cell_id = rrc_ccm_get_curr_camped_cell_id();

  union interFreqMeasurementSysInfo
  {
    rrc_InterFreqMeasurementSysInfo_RSCP interFreqMeasurementSysInfo_rscp;
    rrc_InterFreqMeasurementSysInfo_ECN0 interFreqMeasurementSysInfo_ecno;
    rrc_InterFreqMeasurementSysInfo_HCS_RSCP interFreqMeasurementSysInfo_hcs_rscp;
    rrc_InterFreqMeasurementSysInfo_HCS_ECN0 interFreqMeasurementSysInfo_hcs_ecno;
  }measurementSysInfo;

  (void)rrc_ccm_get_curr_camped_common_plmn(&plmn_id);

  WRRC_MSG0_HIGH("WBP: Adding neighbor frequencies to the prioritized list");

  sib11_ptr = (rrc_SysInfoType11 *) rrc_sibdb_return_sib_for_cell(
                                            rrc_SIB11,
                                            plmn_id, 
                                            cell_id);

  if( sib11_ptr == NULL )
  {
    /* SIB3 for this PLMN, CEll ID combination not present */
    ERR("WBP: ERR: SIB11 for MCC %d, MNC %d, Cell ID %d not present",
          RRC_CSP_GET_INT_MCC(plmn_id),
          RRC_CSP_GET_INT_MNC(plmn_id),
          cell_id);

    return num_of_bands_already_present;
  } /* SIB3 not present */

  if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
  {
    if(sib11_ptr->sib12indicator == TRUE)
    {
      /* Read SIB4. If it's not there mark and error and continue with SIB3 */
      sib12_ptr = (rrc_SysInfoType12 *) rrc_sibdb_return_sib_for_cell(
                                rrc_SIB12, 
                                plmn_id,
                                cell_id);
      if( sib12_ptr == NULL )
      {
        /* SIB12 not present */
        WRRC_MSG0_HIGH("WBP: SIB12 indicated but not present, using SIB11");
        measurementControlSysInfo_ptr = &(sib11_ptr->measurementControlSysInfo);
      }
      else
      {
        measurementControlSysInfo_ptr = &(sib12_ptr->measurementControlSysInfo);
      }
    }
    else
    {
      measurementControlSysInfo_ptr = &(sib11_ptr->measurementControlSysInfo);
    }
  }
  else
  {
    measurementControlSysInfo_ptr = &(sib11_ptr->measurementControlSysInfo);
  }

  if(RRC_CHECK_COMMON_MSG_TYPE(measurementControlSysInfo_ptr->use_of_HCS,
     rrc_MeasurementControlSysInfo_use_of_HCS_hcs_not_used))
  {
    is_hcs_used = FALSE;
    if(measurementControlSysInfo_ptr->use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.t ==
        T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_RSCP)
    {
      is_rscp_used = TRUE;
      measurementSysInfo.interFreqMeasurementSysInfo_rscp = measurementControlSysInfo_ptr->use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;
    }
    else
    {
      is_rscp_used = FALSE;
      measurementSysInfo.interFreqMeasurementSysInfo_ecno = measurementControlSysInfo_ptr->use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;
    }
  }
  else
  {
    is_hcs_used = TRUE;
    if(measurementControlSysInfo_ptr->use_of_HCS.u.hcs_used->cellSelectQualityMeasure.t ==  
       (OSS_UINT32) T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_1_cpich_RSCP)
    {
      is_rscp_used = TRUE;
      measurementSysInfo.interFreqMeasurementSysInfo_hcs_rscp = measurementControlSysInfo_ptr->use_of_HCS.u.hcs_used->cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;
    }
    else
    {
      is_rscp_used = FALSE;
      measurementSysInfo.interFreqMeasurementSysInfo_hcs_ecno = measurementControlSysInfo_ptr->use_of_HCS.u.hcs_used->cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;
    }
  }

  if(is_hcs_used)
  {
    if(is_rscp_used)
    {
      if((RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_hcs_rscp,
        rrc_InterFreqMeasurementSysInfo_HCS_RSCP,interFreqCellInfoSI_List)) &&
         (RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_hcs_rscp.interFreqCellInfoSI_List,
           rrc_InterFreqCellInfoSI_List_HCS_RSCP,newInterFreqCellList)))
      {
        rrc_NewInterFreqCellSI_List_HCS_RSCP *newInterFreqCellList;
        idx=0;
        newInterFreqCellList = &measurementSysInfo.interFreqMeasurementSysInfo_hcs_rscp.interFreqCellInfoSI_List.newInterFreqCellList;
        while((newInterFreqCellList->n > idx) && (num_of_inter_freqs < MAX_NO_OF_CELLS))
        {
          if((RRC_MSG_LIST_BITMASK_IE(newInterFreqCellList,
            rrc_NewInterFreqCellSI_HCS_RSCP,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(newInterFreqCellList->elem[i].frequencyInfo.modeSpecificInfo,
             rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            inter_freq_list[num_of_inter_freqs++] = newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          idx++;
        }
      }
    }
    else
    {
      if((RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_hcs_ecno,
        rrc_InterFreqMeasurementSysInfo_HCS_ECN0,interFreqCellInfoSI_List)) &&
         (RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_hcs_ecno.interFreqCellInfoSI_List,
          (OSS_UINT32) rrc_InterFreqCellInfoSI_List_HCS_ECN0,newInterFreqCellList)))
      {
        rrc_NewInterFreqCellSI_List_HCS_ECN0 *newInterFreqCellList;
        idx=0;
        newInterFreqCellList = &measurementSysInfo.interFreqMeasurementSysInfo_hcs_ecno.interFreqCellInfoSI_List.newInterFreqCellList;
        while((newInterFreqCellList->n > idx) && (num_of_inter_freqs < MAX_NO_OF_CELLS))
        {
          if((RRC_MSG_LIST_BITMASK_IE(newInterFreqCellList,
            rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo,
             rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            inter_freq_list[num_of_inter_freqs++] = newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          idx++;
        }
      }
    }
  }
  else
  {
    if(is_rscp_used)
    {
      if((RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_rscp,
        rrc_InterFreqMeasurementSysInfo_RSCP,interFreqCellInfoSI_List)) &&
         (RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_rscp.interFreqCellInfoSI_List,
         rrc_InterFreqCellInfoSI_List_RSCP,newInterFreqCellList)))
      {
         rrc_NewInterFreqCellSI_List_RSCP *newInterFreqCellList;
         idx=0;
        newInterFreqCellList = &measurementSysInfo.interFreqMeasurementSysInfo_rscp.interFreqCellInfoSI_List.newInterFreqCellList;
        while((newInterFreqCellList->n > idx) && (num_of_inter_freqs < MAX_NO_OF_CELLS))
        {
          if((RRC_MSG_LIST_BITMASK_IE(newInterFreqCellList,
            rrc_NewInterFreqCellSI_RSCP,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo,
             rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            inter_freq_list[num_of_inter_freqs++] = newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          idx++;
        }
      }
    }
    else
    {
      if((RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_ecno,
        rrc_InterFreqMeasurementSysInfo_ECN0,interFreqCellInfoSI_List)) &&
         (RRC_MSG_COMMON_BITMASK_IE(measurementSysInfo.interFreqMeasurementSysInfo_ecno.interFreqCellInfoSI_List,
         rrc_InterFreqCellInfoSI_List_ECN0,newInterFreqCellList)))
      {
         rrc_NewInterFreqCellSI_List_ECN0 *newInterFreqCellList;
        idx=0;
        newInterFreqCellList = &measurementSysInfo.interFreqMeasurementSysInfo_ecno.interFreqCellInfoSI_List.newInterFreqCellList;
        while((newInterFreqCellList->n > idx) && (num_of_inter_freqs < MAX_NO_OF_CELLS))
        {
          if((RRC_MSG_LIST_BITMASK_IE(newInterFreqCellList,
            rrc_NewInterFreqCellSI_ECN0,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo,
             rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            inter_freq_list[num_of_inter_freqs++] = newInterFreqCellList->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          idx++;
        }
      }
    }
  }

  WRRC_MSG2_HIGH("WBP: is_hcs_used: %d, is_rscp_used: %d",is_hcs_used,is_rscp_used);

  for(i = 0; ((i < num_of_inter_freqs) && (num_of_bands_already_present < MAX_NUM_OF_WCDMA_BANDS)); i++)
  {
    rrc_csp_band_class_type nbr_freq_band;
    nbr_freq_band = rrc_get_frequency_band(inter_freq_list[i],
                                              plmn_id);
    if((RRC_CSP_BAND_MAX != nbr_freq_band) && 
       (rrccsp_is_supported_band(nbr_freq_band)))
    {
      if(!rrccsp_is_band_already_added_to_prioritized_list(nbr_freq_band, num_of_bands_already_present))
      {
        WRRC_MSG1_HIGH("WBP: Adding neighbor band %d to prioritized band list",nbr_freq_band);
        band_list[num_of_bands_already_present++] = nbr_freq_band;
      }
    }
  }

  return num_of_bands_already_present;
}

/*===========================================================================

FUNCTION           RRCCSP_PRIORITIZE_WCDMA_BANDS_FOR_FULL_SCAN

DESCRIPTION       Constructs the order in which the WCDMA bands need to be searched while doing
                  full frequency scan.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void 
rrccsp_prioritize_wcdma_bands_for_full_scan(void)
{
  /*
   * The priority order of scanning the WCDMA bands by the UE is: 
   * (i) Band on which UE last had service, followed by 
   * (ii) Band(s) to which the neighbor WCDMA frequencies belong to (extracted from System Information Blocks (SIB) 11 & 12), followed by 
   * (iii) Band(s) to which the frequencies stored in Acquisition Database belong to (LIFO), followed by 
   * (iv) Default band priority order specified by the carrier/OEM for any remaining bands. (this is the band mask values defined in rrccspdb.h).
   */

  rrc_csp_band_class_type    last_camped_band = RRC_CSP_BAND_MAX;
  rrc_freq_type    last_camped_freq = 0;
  rrc_state_e_type rrc_state = rrc_get_state();
  int i = 0;

  rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
  rrc_csp_curr_select_data.num_of_prioritized_bands = 0;

  if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN))
  {
    WRRC_MSG1_HIGH("WBP: Prioritizing the WCDMA bands used for full frequency scan, curr_scan is: %d.",rrc_csp_int_data.curr_scan);
  }
  else
  {
    WRRC_MSG1_HIGH("WBP: Not Prioritizing the WCDMA bands , curr_scan is: %d.",rrc_csp_int_data.curr_scan);
    return;
  }
  if(prioritize_850_band && rrccsp_is_supported_band(RRC_CSP_WCDMA_850_BAND))
  {
    rrc_csp_curr_select_data.prioritized_bands[rrc_csp_curr_select_data.num_of_prioritized_bands++] = RRC_CSP_WCDMA_850_BAND;
    WRRC_MSG0_HIGH("Set first band to be scanned as 850 band");
  }
  /*
    Step (i) Band on which UE last had service
  */

  if(rrc_state != RRC_STATE_DISCONNECTED)
  {
    WRRC_MSG1_HIGH("LCF in Connected Mode = %d", rrc_csp_int_data.last_camped_freq_in_connected_mode);
    last_camped_freq = rrc_csp_int_data.last_camped_freq_in_connected_mode;
  }
  else
  {
    WRRC_MSG1_HIGH("LCF in Idle Mode = %d", rrc_csp_int_data.last_camped_freq_in_idle_mode);
    last_camped_freq = rrc_csp_int_data.last_camped_freq_in_idle_mode;
  }

  last_camped_band = rrc_get_frequency_band(last_camped_freq,
                                            rrc_ccm_get_curr_camped_plmn());

  if((RRC_CSP_BAND_MAX != last_camped_band) &&
     (rrccsp_is_supported_band(last_camped_band)) &&
      !rrccsp_is_band_already_added_to_prioritized_list(last_camped_band, rrc_csp_curr_select_data.num_of_prioritized_bands))
  {
    WRRC_MSG1_HIGH("WBP: Adding last camped band %d to prioritized band list",last_camped_band);
    rrc_csp_curr_select_data.prioritized_bands[rrc_csp_curr_select_data.num_of_prioritized_bands++] = last_camped_band;
  }

  /*
    Step (ii) Band(s) to which the neighbor WCDMA frequencies belong to (extracted from System Information Blocks (SIB) 11 & 12)
  */
  rrc_csp_curr_select_data.num_of_prioritized_bands = rrccsp_add_nbr_freq_bands_from_sib11_to_band_list(
                                                            rrc_csp_curr_select_data.prioritized_bands,
                                                            rrc_csp_curr_select_data.num_of_prioritized_bands);

  /*
    Step (iii) Band(s) to which the frequencies stored in Acquisition Database belong to (LIFO)
  */
  rrc_csp_curr_select_data.num_of_prioritized_bands = rrccsp_add_freq_bands_from_acq_db_to_band_list(
                                                            rrc_csp_curr_select_data.prioritized_bands,
                                                            rrc_csp_curr_select_data.num_of_prioritized_bands);

  for(i = 0; ((i< rrc_csp_curr_select_data.num_of_prioritized_bands) && (i < MAX_NUM_OF_WCDMA_BANDS)); i++)
  {
    WRRC_MSG2_HIGH("WBP: Prioritized Band[%d] is %d ",i, rrc_csp_curr_select_data.prioritized_bands[i]);
  }

}

