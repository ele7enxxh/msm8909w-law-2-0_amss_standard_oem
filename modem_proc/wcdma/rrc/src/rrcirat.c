/*===========================================================================
            R R C   M E A S U R E M E N T S

DESCRIPTION

   This module supports Measurement Control and Measurement Reporting
   for the RRC layer. At present, it supports the following features
   for in Idle mode and DCH state
   1. Read SIB 11 and tell L1 to setup inter-RAT meas
   based on them.
   2. Sends Meas Report to UTRAN in DCH state.
   3. Supports Measurement Control Message received from UTRAN



EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

   RRC Task must have been started. All RRC command queues must
   have been initialized. All RRC signals must have been cleared
   at task startup.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

             EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcirat.c#1 $


when       who   what, where, why
--------   ---   ------------------------------------------------------------
06/10/16   ad      Made changes to log important parms from OTAs
06/08/16   vs      f3 density reduction changes
05/11/16   vs      Changes to remove irrelevant f3s
02/23/15   vi    Fixing compilation warnings
02/17/15   aa    Made changes to include feature flag for fixing compilation error
08/04/14   vi      Made changes to avoid compiler warnings
07/11/14   vi    Made changes to convert earfcn type from uint16 to unti32
05/06/14   geg   Use safer versions of memcpy() and memmove()
03/13/14   vi    Made changes to correctly return the status after successfully processing cell info list in function rrcirat_process_cell_list_mcm_r6
03/11/13   vi     Made changes to disable IRAT and IFREQ measurements in different states based on the NV rrc_disable_meas_nv
01/06/14   sr    Made changes to send MCF cause as "Unsupported measurement" if GSM RAT is not supported
12/13/13   vi    Modified the code to ignore the invalid cell id configured in cellsforInterfreqmeaslist/cellsforinterRATmeaslist 
                 and sending MCF only when the whole list is invalid.
09/16/13   mp    Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/29/13   sr    Made changes to filter duplicate entries in MCM and Rel 9 MCM while sending meas request to L1
08/14/13   vg    Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
07/10/13   vi    Added code to send MCF when UE doesnt support TDD frequencies and Network configures TDD cells in MCM.
06/19/13   vi    Fixing KW error
06/07/13   vi    Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/28/13   sr    Added code for measurement identity extension
04/25/13   sg    Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp    Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
10/14/12   gv    Added code to support exchange of AS ID between WRRC and GRR commands
10/03/12   sr    Added changes to check the presence of IRAT cell info indication in Rel 8 MCM
09/20/12   sr    Added changes to send MCM failure when reporting cell status is invalid for inter RAT measurements
08/15/12   vg    corrected the check in the R8 MCM modify handling.
07/24/12   geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/03/12   sks     Added support for LTE TDD CM measurements.
24/05/11   rd    Made changes to add gaurd check for modespecific tag in cell selectioninfo
03/11/12   as    Made changes to correct the conditon check for irat meas
01/12/12   md    Reverting back to the code change required to eliminate reference to
                 GRR sub system private function.
01/03/12   pm    Reverting CL:2068347 changes to fix the compilation warnings
01/02/12   pm    Added rrc_wtol_cm_support_nv to put FEATURE_WCDMA_CM_LTE_SEARCH feature under nv check 
12/09/11   ad    Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
11/30/11   md    Included API 'geran_grr_api' to access fun rr_get_maximum_power_output_for_frequency.
07/08/11   gv    Fixed KW errors
05/23/11   sks   Made changes to prune LTE-TDD neighbours after calling lte_rrc_get_tdd_prune_required().
05/02/11   sks   Made changes to access IE pointer interRATMeasurement in a modify MCM only
                 when measurementType IE is present
04/27/11   vg    fixed compilation error by removing static
04/25/11   vg    made changes to allow empty report to go eventhough no eutra freq present
04/25/11   vg    made changes to handle R3 MCM-relese of type EUTRA.
04/25/11   rl    Fixed the KW error
03/17/11   rl    Fixed the compiler errors/warning after enabling 
                 FEATURE_WCDMA_CM_LTE_SEARCH
03/17/11   sks   Made changes to ignore LTE-TDD cells, when sending measurement req to L1.
02/07/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad    Added changes for RRC SW decoupling
01/04/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/17/10   rm    Added code to fail non GSM MCM
12/17/10   rm    Added missed validation when FEATURE_WCDMA_CM_LTE_SEARCH is not defined
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   sks   Fixed bugs found during testing.
11/09/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/29/10   su    Made changes to remove WCDMA_TASKLOCK() usage from RRC code base.
10/11/10   ss    ASN.1 migration to ASN1 9.3.0
10/11/10   ss    Upgraded SIB11Bis code to ASN 8.a and added code to init sibs lookup table 
                 after initialization of rrc_nv_rel_indicator.
11/03/10   rl    Fixed compiler warnings
10/30/10   rl    Objective systems migration changes check in 
                 on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm    Added code to support LTE<->WCDMA Inter RAT
06/29/10   rm    Made changes for ZI memory optimization
02/01/10   sks   Fixed klocwork error.
11/05/09   rm    Made changes to ignore the removal of cell id which is not present at UE and
                 continue processing the MCM 
10/26/09   rm    Fixed Klockwork errors
10/07/09   gkg   Changes for REL8 under FEATURE_WCDMA_REL8
07/20/09   rm    Added code to not call sys_eplmn_list_equivalent_plmn when n/w sel mode is
                 LIMITED
05/08/09   ss    Updated Copyright Information
04/29/09   ss    Made changes to compiler warnings on gcc compilers.
01/19/08   ps    Made changes for ASn1 migration 25.331v7.9  
                 i.e.May 2008 version  (2008-05) 
12/29/08   rm    Added support for 3G<->GAN Handover
11/10/08   rm    Made changes to process Rel7 measurement control message
10/08/08   rm    Made changes to skip sending InterFreq and Inter RAT measurements configured by SIB11, SIB12 and MCM to L1
                 Changes are under FEATURE_WCDMA_REL7
09/12/08   ps    Added code for REl7 under FEATURE_WCDMA_REL7.Changes for ASn1
                 migration March 08 version
07/16/08   rm    Enhanced the handling of transaction id received in MCM message so that
                 UE accepts the MCM's with same transaction id and in case of failure UE
                 sends MCM failure for a transaction id only if it is latest among the same
                 transactions ids
06/30/08   ns    Made the necessary changes to enable the functionality of the feature 
                 FEATURE NETWORK SHARING only when the GCF NV is set
03/10/08   rm    Added support for SIB11bis. Code changes are under feature flag 
                 FEATURE SIB11BIS SUPPORT  
03/05/08   pk    Removed unnecessary inclusion of rr_general.h
10/23/07   pk    Added support for network sharing feature.Changes are under feature flag 
                 FEATURE NETWORK SHARING.
08/07/07   rm    Made changes to initialize the intra frequency measurement quantity
                 while sending the measurement request to L1
06/13/07   rm    Made changes to increment the cell count when the IRAT cell list
                 for measurement is present in Measurement Control Message(R3).
04/20/07   da    Made changes needed to update ASN.1 version to R6 sept R6 2006
04/17/07   vg    added check to choose gsm,fdd,tdd structure inside GSM 
                 cellselection reselection info IE. also get the max_tx_pwr from
                 rr function call insted hardcoding.
02/20/07   vm    Fixed compilation error.
02/02/07   vg    fixed issue by removing check where rrc was resetting IRAT
                 cell ind variable to FALSE when cell list is not present in MCM.
11/22/06   sgk   modifed earlier fix for event list processing. 
11/15/06   sgk   Added fix to reject the MCM if event list is empty only if it
                 is a MCM setup. 
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases. 
08/30/06   sgk   Added fix to initialize the local inter rat cell list structure
                 only on receiving cell information in the MCM. Also added fix
                 to copy the cell information from the the local list to the 
                 global cell info list only if the MCM had cell information.
                 Fixed lint errors. 
08/29/06   sm/da Made changes required for R6 ASN.1 migration.  Also merged
                 FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   vg    added R5 IE "IRAT cell info ind" support in MCM and meas report.
             under the feature FEATURE IRAT CELL INFO IND.
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under
                 FEATURE REL5 while retaining the HSDPA specifc processing
                 under FEATURE HSDPA.
06/01/06   sgk   Added change to ignore cells when technology specific info is
                 absent.
05/25/06   sgk   Made changes to include rel5 non HSDPA functionality under
                 FEATURE REL5 while retaining the HSDPA specifc processing
                 under FEATURE HSDPA.
04/28/06   sgk   Added code to include the cell id of invalid cells in cell
                 list to remain in sync with the n/w when the cells are from
                 SIB11 and 12.
                 Added code to reject the MCM if it has invalid cells.
04/18/06   da    Modified code to support migration to Mar-06 Rel-5 ASN1 +
                 R6 Hacks
03/15/06   vg    fixed lint warnings
03/08/06   sgk   Added code to initialize the filter coefficient to a default
                 value 0 in MCM modify.
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
01/25/06   bd    Lint clean-up
11/10/05   sgk   Added check for rrc_nv_rel_indicator to be equal to RRC_NV_REL_INDICATOR_REL5
                 before processing the rel 5 ies.
10/21/05   sgk   Added rel5 support for the processing of delta qrxlevmin.
10/04/05   vk    Corrected the default value & calculated value of qhcs
                 for HCS Inter-RAT cells
09/29/05   vk    Corrected bit mask used for storing the HCS related information.
                 Also corrected the number of cells for Measurement that are sent to L1
09/08/05   sgk   Corrected code to pick up bsic verification required from the
                 MCM modify instead of setup when processing the MCM modify.
08/15/05   sgk   Added code to check if the PLMN from SIB18 is the selected
                 PLMN, and if so allow the cell for reselection, instead of
                 calling the nas function to check if it belongs to the
                 equivalent list.
07/15/05   sgk   Added change to call rr_rrc_get_arfcn_band inside the function
                 rrc_get_gsm_band to get the arfcn band.
07/13/05   sgk   Added rel5 non critical extensions for rel 99 MCM ie the
                 transaction identifier extension.
07/08/05   sgk   Made fix to return success in SIB11 if the inter rat measurement
                 is not present so that it can be picked up from SIB12 if not
                 present.
                 Made fix to set the inter sys info flag to true when picking up
                 inter rat info from SIB11 if not in SIB12.
07/05/05   sgk   Made changes to pick up Fach measurement occasion from SIB11
                 if not present in SIB12.
06/27/05   sgk   Added basic rel 5 measurement ctrl msg support.
06/23/05   sgk   Mapped default QHCS values based on RCSP and ECN0.
                 Fixed lint errors
06/07/05   sgk   Rolled back the check to turn off sib18 if the selected
                 PLMN at rrc is not the same as the one in NAS. SIB18 will
                 be applied even in this case, to block cells for reselection
                 based on the NAS equivalent PLMN list.
05/03/05   vk    Used the new functional interface to set additional meas
                 in DB when FEATURE HSDPA is defined
04/29/05   sgk   Added fix to check if the inter RAT measurement system info
                 is present in the bit mask prior to the check for the cell
                 info list and return FAILURE if absent in SIB11 processing.
                 In SIB12 if the inter RAT measurement system info
                 is absent in the bit mask, the cell info list will be picked
                 up from SIB11, if present.
04/22/05   sgk   Removed intialization of hcs flag as it is done already.
04/21/05   sgk   Added HCS related fixes.
                 Modified code to allow an IRAT MCM with empty cell list to
                 be sent down to L1 for processing instead of blocking it.
04/20/05   sgk   Added blocking of SIB18 processing when the registerd PLMN
                 != selected PLMN.
04/14/05   sgk   Added fix to check for rrc_tdd_q_RxlevMin_present instead of
                 rrc_fdd_q_RxlevMin_present for gsm cells in SIB11 and 12.
04/07/05   sgk   Added HCS support.
04/01/05   sgk   Added Code to block SIB18 processing in true limited service.
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/09/05   sgk   Lint fixes.
03/07/05   sgk   Added sib18 support for inter RAT measurements in idle and
                 connected mode.
03/02/05   ttl   Support RRC ASN1 Release 5.
12/16/04   sgk   Removed return FAILURE if event criteria list is not present
                 as it is optional in mcm modify.
12/11/04   sgk   Made inter_sys_sib12_new_cell_list a global variable to fix
                 stack over flow problem.
12/08/04   sgk   Moved setting meas obj included flag to TRUE outside the check
                 for cell info list present in mcm modify handling.
12/08/04   sgk   Fixed compiler warnings.
12/02/04   sgk   Lint fixes for SIB12 support.
                 Added check for interRATCellInfoList present before calling
                 rrcirat_process_cell_list_mcm during mcm_modify in function
                 rrcirat_process_mcm.
11/23/04   sgk   Added initial SIB12 Inter RAT support.
10/22/04   sgk   Advanced the inter rat cell list ptr to to the next cell in
                 case function rrc_get_gsm_band reported an INVALID BAND.
09/20/04   vk    Filtered out GSM neighbors if the band is invalid
08/19/04   sgk   Assigned gsm_band to the right enum values in function
                 rrc_get_gsm_band to fix enum mismatch lint error
07/30/04   vk    Defined a new function rrc_is_gsm_cell_present_in_sibs() that is
                 called by L1 to check if SIB11 has GSM cells
06/02/04   vk    Reformatted some MSG_HIGH's and fixed previous Lint Warnings/Errors
05/14/04   vk    Implemented fn rrcirat_delete_dup_non_verified_cells() to
                 purge duplicate non verified ARFCN in a Meas Report
03/20/04   vk    Added SIB4 support
03/15/04   bu    Removed the use of FEATURE DUALMODE BASELINE. This feature is
                 always defined (even for singlemode builds).
03/09/04   jh    Added initialization for Additional Measurements List in
                 rrcirat_send_meas_report().
12/18/03   vk    Made sure that reporting cell status is processed from periodic
                 reporting when a modify with periodic report is specified
12/16/03   bu    Added GSM quadband support.
12/10/03   vk    Added a check while processing Meas Ctrl Msg to reject the message
                 in case UTRAN specifies 2 GSM cells with same inter rat cell identity
                 Also added processing of reporting cell status in case of no reporting
                 and reject the message in case the reporting cell status choice is not
                 equal to within active set or within virtual active set of other RAT
12/10/03   vk    Added code to support INTER RAT cell reselection in CELL_FACH under
                 FEATURE INTER RAT CELL FACH RE SELECTION.
11/07/03   vk    Added check to verify that additional measurements corresponding
                 to a primary measurement can never be greater than the maximum
                 allowed
10/17/03   vk    Corrected a message printed when BSIC has been verified
07/21/03   vk    While validating Measurement Report, only send no of GSM cells
                 as allowed per specs and trash anything on top of the maximum
                 allowed
06/23/03   vk    Added support to read inter rat cells in case HCS is set to TRUE
06/03/03   vk    On receiving a delete indication from L1, reinitialize
                 measurement criteria and addtitional measurements for
                 the deleted measurement
05/31/03   vk    Delete primary measurement from additional measurement database
                 when all the periodic reports configured for that measurement
                 are sent. Corrected date of last checkin
05/29/03   vk    Added code for Implementing Additional Measurements for
                 Inter RAT Measurements
04/17/03   vk    Remove the validation when number of GSM cells are set to 0
                 for an Inter-RAT Periodic Measurement Report. This would allow
                 a blank Measurement Report to get encoded to meet the spec
                 requirement of sending a measurement report every measurement
                 interval
04/15/03   vk    Set measured_res_incl to TRUE after validation of periodic
                 report is successfully done.
02/18/03   vk    Ignored Observed time difference signalled in Measurement
                 Control Message as it is no longer supported in Rel 99
02/14/03   vk    Fixed most of lint errors
02/05/03   sk    March2002Rel specific changes
                   Changed  rrc_MeasurementQuantityGSM_pathloss
                   to  rrc_MeasurementQuantityGSM_dummy
01/21/03   vk    Merged fixes related to Inter-RAT Event triggered Meas Report
01/17/03   vk    Set Measurement Quantity to TRUE in case of modify
12/20/02   vk    Initializes cell reselection values to default values
                 if not present in SIB11. Also, cell_sel_info_incl flag now
                 is always set to TRUE
12/16/02   vk    Added the default value of maximum allowed uplink txion power
                 from radio access capability information. Also corrected the
                 default initialisation of Rxlevmin
12/13/02   bu    Corrected the value of Qrxlevmin sent to L1 in rrcirat_read_sib11().
12/03/02   vk    Removed the addition of cells in the removed cell list on
                 occurrence of cascading as it is not required
11/26/02   vk    Checked if Dual Mode is enabled before sending Inter-RAT SIB11
                 cells to L1
10/30/02   vk    Merged more Inter-RAT Meas fixes found during integration
10/29/02   vk    Merged file from the branch irat_gap into the mainline.
                 Defined function rrcirat_convert_rssi_value(...) to convert
                 rssi value received from L1 into UTRAN format. Also, added
                 fixes for issues found during integration while testing
                 compressed mode with Measurements
09/26/02   vk    Included meas object while sending Inter-RAT Meas Request
09/19/02   vk    Added void in definition of fns rrcirat_initialize_cell_list(..)
                 and rrcirat_initialize_local_neighbor_list(..) to reflect the
                 changes done in the header file
09/17/02   vk    While receiving a Modify for Inter-RAT Measurements, added
                 the check to see if Measurement Type is present or not when
                 the Message specifies a Modify
09/06/02   vk    Added code to handle Inter-RAT Measurements under FEATURE INTER RAT MEAS

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rrcmeas.h"
#include "rrcmeasi.h"
#include "rrcsibdb.h"
#include "msg.h"
#include "rrccmd_v.h"
#include "l1task_v.h"
#include "rrclcm.h"
#include "rrcsend.h"
#include "uecomdef.h"
#include "rrcscmgr.h"
#include "rrcmcm.h"
#include "rrccsp.h"
#include "rrcasn1util.h"

#ifdef FEATURE_GSM_QUADBAND
#include "rr_rrc_if.h"
#endif /* FEATURE_GSM_QUADBAND */

#ifdef FEATURE_GAN_3G
#error code not present
#endif
#include "rrccspi.h"
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
#include <lte_rrc_ext_api.h>
#endif
#include "rrcnv.h"
#include "rrclogging.h"
#include "geran_grr_api.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define RET_INTER_RAT_BAND(band) \
( \
(band == rrc_Frequency_Band_dcs1800BandUsed) ? L1_DCS_1800_BAND : L1_PCS_1900_BAND \
)

#define RET_BSIC_VERIFICATION_REQD(verification) \
( \
(verification == rrc_BSIC_VerificationRequired_required) ? TRUE : FALSE \
)

/* Network Color Code */
#define NCC_MAX   7
/* Base Station Color Code */
#define BCC_MAX   7

/* Inter-RAT Cell Id */
#define MAX_INTER_RAT_CELL_ID 31

/* ARFCN */
#define MAX_ARFCN 1024

/* MAX RSSI */
#define MAX_RSSI 63

/* Min RSSI */
#define MIN_RSSI 0

/* Max RSSI Length */
#define RSSI_MAX_LENGTH 1

/* Actual RSSI Length in bits */
#define RSSI_ACTUAL_LENGTH 6

/* Invalid ARFCN */
#define RRC_INVALID_ARFCN_FLAG 10000


/* Inter-RAT Reporting Amount */
#define  RET_INTER_RAT_REPORTING_AMOUNT(reporting_amount)  \
(  \
 ((reporting_amount == rrc_ReportingAmount_ra1) ? L1_RPT_AMOUNT_1 : \
  ((reporting_amount == rrc_ReportingAmount_ra2) ? L1_RPT_AMOUNT_2 : \
  ((reporting_amount == rrc_ReportingAmount_ra4) ? L1_RPT_AMOUNT_4 :\
  ((reporting_amount == rrc_ReportingAmount_ra8) ? L1_RPT_AMOUNT_8 :\
  ((reporting_amount == rrc_ReportingAmount_ra16) ? L1_RPT_AMOUNT_16 :\
  ((reporting_amount == rrc_ReportingAmount_ra32) ? L1_RPT_AMOUNT_32 :\
  (reporting_amount == rrc_ReportingAmount_ra64) ? L1_RPT_AMOUNT_64 : L1_RPT_AMOUNT_INFINITY \
  )))))) \
)

/*
#define RET_INTER_RAT_PERIODIC_REPORTING_INTERVAL(ri)  \
(  \
 (ri == rrc_ril0) ? 0 : \
  ((ri == rrc_ril0_25) ? L1_RPT_INTERVAL_250MS : \
  ((ri == rrc_ril0_5) ? L1_RPT_INTERVAL_500MS :\
  ((ri == rrc_ril1) ? L1_RPT_INTERVAL_1000MS :\
  ((ri == rrc_ril2) ? L1_RPT_INTERVAL_2000MS :\
  ((ri == rrc_ril3) ? L1_RPT_INTERVAL_3000MS :\
  ((ri == rrc_ril4) ? L1_RPT_INTERVAL_4000MS :\
  ((ri == rrc_ril6) ? L1_RPT_INTERVAL_6000MS :\
  ((ri == rrc_ril8) ? L1_RPT_INTERVAL_8000MS :\
  ((ri == rrc_ril12) ? L1_RPT_INTERVAL_12000MS :\
  ((ri == rrc_ril16) ? L1_RPT_INTERVAL_16000MS :\
  ((ri == rrc_ril20) ? L1_RPT_INTERVAL_20000MS :\
  ((ri == rrc_ril24) ? L1_RPT_INTERVAL_24000MS :\
  ((ri == rrc_ril28) ? L1_RPT_INTERVAL_28000MS :\
  (ri == rrc_ril32) ? L1_RPT_INTERVAL_32000MS : L1_RPT_INTERVAL_64000MS \
  ))))))))))))) \
)
*/


/* Inter-RAT Event Type */
#define  RET_INTER_RAT_EVENT_TYPE(event_type)  \
(  \
 ((event_type == T_rrc_InterRATEvent_event3a) ? L1_INTER_RAT_EVENT_3A : \
  ((event_type == T_rrc_InterRATEvent_event3b) ? L1_INTER_RAT_EVENT_3B : \
  ((event_type == T_rrc_InterRATEvent_event3c) ? L1_INTER_RAT_EVENT_3C : \
  L1_INTER_RAT_EVENT_3D ))) \
)

/* Converts time to trigger for FDD from ASN1 format to L1 format */
#define  RET_INTER_RAT_TIME_TO_TRIGGER(time_to_trigger)  \
    (  \
     (time_to_trigger == rrc_TimeToTrigger_ttt0) ? 0 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt10) ? 10 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt20) ? 20 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt40) ? 40 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt60) ? 60 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt80) ? 80 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt100) ? 100 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt120) ? 120 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt160) ? 160 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt200) ? 200 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt240) ? 240 : \
      ((time_to_trigger == rrc_TimeToTrigger_tt320) ? 320 : \
      ((time_to_trigger == rrc_TimeToTrigger_ttt640) ? 640 : \
     ((time_to_trigger == rrc_TimeToTrigger_ttt1280) ? 1280 : \
    ((time_to_trigger == rrc_TimeToTrigger_ttt2560) ? 2560 : 5000 \
      )))))))))))))) \
    )

/* GSM Filter Coefficient */
#define  RET_GSM_FILTER_COEFFICIENT(fc)  \
(  \
 (fc == rrc_FilterCoefficient_fc0 ) ? L1_GSM_FILTER_COEF_0 : \
  ((fc == rrc_FilterCoefficient_fc1 ) ? L1_GSM_FILTER_COEF_1 :\
  ((fc == rrc_FilterCoefficient_fc2 ) ? L1_GSM_FILTER_COEF_2 :\
  ((fc == rrc_FilterCoefficient_fc3 ) ? L1_GSM_FILTER_COEF_3 :\
  ((fc == rrc_FilterCoefficient_fc4 ) ? L1_GSM_FILTER_COEF_4 :\
  ((fc == rrc_FilterCoefficient_fc5 ) ? L1_GSM_FILTER_COEF_5 :\
  ((fc == rrc_FilterCoefficient_fc6 ) ? L1_GSM_FILTER_COEF_6 :\
  ((fc == rrc_FilterCoefficient_fc7 ) ? L1_GSM_FILTER_COEF_7 :\
  ((fc == rrc_FilterCoefficient_fc8 ) ? L1_GSM_FILTER_COEF_8 :\
  ((fc == rrc_FilterCoefficient_fc9 ) ? L1_GSM_FILTER_COEF_9 :\
  ((fc == rrc_FilterCoefficient_fc11 ) ? L1_GSM_FILTER_COEF_11 :\
  ((fc == rrc_FilterCoefficient_fc13 ) ? L1_GSM_FILTER_COEF_13 :\
  ((fc == rrc_FilterCoefficient_fc15 ) ? L1_GSM_FILTER_COEF_15 :\
  ((fc == rrc_FilterCoefficient_fc17 ) ? L1_GSM_FILTER_COEF_17 :\
  (fc == rrc_FilterCoefficient_fc19 ) ? L1_GSM_FILTER_COEF_19 : L1_GSM_FILTER_COEF_0 \
  ))))))))))))) \
)

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
#define RET_EUTRA_FILTER_COEFFICIENT(fc)\
(  \
 (fc == rrc_FilterCoefficient_fc0 ) ? L1_FILTER_COEF_0 : \
  ((fc == rrc_FilterCoefficient_fc1 ) ? L1_FILTER_COEF_1 :\
  ((fc == rrc_FilterCoefficient_fc2 ) ? L1_FILTER_COEF_2 :\
  ((fc == rrc_FilterCoefficient_fc3 ) ? L1_FILTER_COEF_3 :\
  ((fc == rrc_FilterCoefficient_fc4 ) ? L1_FILTER_COEF_4 :\
  ((fc == rrc_FilterCoefficient_fc5 ) ? L1_FILTER_COEF_5 :\
  ((fc == rrc_FilterCoefficient_fc6 ) ? L1_FILTER_COEF_6 :\
  ((fc == rrc_FilterCoefficient_fc7 ) ? L1_FILTER_COEF_7 :\
  ((fc == rrc_FilterCoefficient_fc8 ) ? L1_FILTER_COEF_8 :\
  ((fc == rrc_FilterCoefficient_fc9 ) ? L1_FILTER_COEF_9 :\
  ((fc == rrc_FilterCoefficient_fc11 ) ? L1_FILTER_COEF_11 :\
  ((fc == rrc_FilterCoefficient_fc13 ) ? L1_FILTER_COEF_13 :\
  ((fc == rrc_FilterCoefficient_fc15 ) ? L1_FILTER_COEF_15 :\
  ((fc == rrc_FilterCoefficient_fc17 ) ? L1_FILTER_COEF_17 :\
  (fc == rrc_FilterCoefficient_fc19 ) ? L1_FILTER_COEF_19 : L1_FILTER_COEF_0 \
  ))))))))))))) \
)

#define RET_EUTRA_MEAS_QUAN(meas_quan)\
( \
  (meas_quan == rrc_MeasurementQuantityEUTRA_rrsp) ? L1_EUTRA_QUAN_RSRP : L1_EUTRA_QUAN_RSRQ \
)
#endif
/* RSSI high and Low values */
#define RSSI_VAL_HIGH -48
#define RSSI_VAL_LOW -110

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
extern rrcmeas_substate_e_type rrcmeas_current_substate;
extern l1_meas_sib_parm_struct_type *rrcmeas_sib11;
extern uint8 rrcmeas_pending_rpt;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Local Object Definition
** ----------------------------------------------------------------------- */


/*-------------------------------------------------------------------
STRUCTURE: inter_rat_cell_list_struct_type
Stores the inter rat cell info list [CELL_INFO_LIST variable]
--------------------------------------------------------------------*/
inter_rat_cell_list_struct_type  inter_rat_cell_list[L1_MAX_CELL_MEAS];

/* Used for manipulation of Local Neighbours given in Meas Ctrl Msg */
inter_rat_cell_list_struct_type local_neighbor_list[L1_MAX_CELL_MEAS];

/* For storing temporary meas results with cells marked for deletion */
l1_measured_cells_struct_type tmp_measured_cells[L1_MAX_REPORTED_GSM_CELLS];

/* Declared to store new intra freq cell list that is being built */
LOCAL l1_inter_sys_cell_list_struct_type inter_sys_sib12_new_cell_list;

/* for storing interRAT cell info ind present in MCM */
interRAT_cell_info_status_type interRAT_cell_info_status;


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* Maximum No of Inter RAT measurements based on additional meas */
inter_rat_meas_id_list_struct_type inter_rat_meas_id_list[MAX_INTER_RAT_MEAS];

/*Boolean added to track if interfrequency and interrat measurements are to be processed or not*/
extern boolean process_interfreq_interrat_meas_rel7;

#ifdef FEATURE_GAN_3G
  #error code not present
#endif

#ifdef FEATURE_WCDMA_TO_LTE
  /*Frequencyl list*/
  #define RRC_INVALID_EUTRA_FREQ 0xFFFFFFFF
  rrcirat_eutra_freq_type rrcirat_eutra_freq_list[L1_MAX_EUTRA_FREQ];
  rrcirat_eutra_freq_type *rrcirat_eutra_freq_list_ptr = rrcirat_eutra_freq_list;
#endif

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#ifdef FEATURE_GSM_QUADBAND

sys_band_T converted_gsm_band;

static sys_band_T rrc_get_gsm_band(l1_gsm_cell_info_type gsm_info)
{
  if (gsm_info.band_indicator == L1_DCS_1800_BAND)
  {
    return(rr_rrc_get_arfcn_band_internal(gsm_info.bcch_arfcn.num, RRC_RR_DCS_1800));
  }
  else
  {
    return(rr_rrc_get_arfcn_band_internal(gsm_info.bcch_arfcn.num, RRC_RR_PCS_1900));
  }
}
#endif /* FEATURE_GSM_QUADBAND */

/*===========================================================================

FUNCTION   IRAT_READ_SIB11

DESCRIPTION

  This function reads Inter RAT Cell List from SIB 11 and
  stores it in inter_rat_meas_id_list. This Inter RAT
  Cell List is used for cell reselection. When UTRAN sends a meas
  in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

  Pointer to rrc_InterRATCellInfoList_B must be valid.

RETURN VALUE

  FAILURE: If cell list is not in order
  SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib11
(
  l1_meas_sib_parm_struct_type* l1_inter_rat_sib_ptr
)
{
  rrc_SysInfoType11 *sib11_ptr = NULL; /* Declared to store the value of SIB 11*/

  rrc_SysInfoType3  *sib3_ptr = NULL;

  rrc_SysInfoType4  *sib4_ptr = NULL;

  rrc_NewInterRATCellList_B* inter_rat_cell_list_ptr = NULL;

  uint8 cell_count = 0;

  uint16 cell_position;

  boolean sib4_present = FALSE;

  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterRATCellsList *tmp_rrc_PLMNsOfInterRATCellsList_ptr = NULL;
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  boolean sib18_present = FALSE;

  rrc_FACH_MeasurementOccasionInfo_inter_RAT_meas_ind * local_sib_inter_rat_ptr = NULL;

  rrc_SysInfoType11_v590ext_IEs *sib11_rel5_ext_ptr = NULL;
  rrc_SysInfoType11_v590ext_IEs_newInterRATCellInfoList_v590ext * rel5_ext_ptr = NULL;

  uint16 prev_cell_count =0;
  uint32 idx=0,idy=0;
  uint16 arfcn;

  /* First check if dual mode is enabled */
  if (rrcmcm_is_dualmode_enabled() == FALSE)
  {
    WRRC_MSG0_HIGH("IRAT:Dual Mode not enabled");
    return FAILURE;
  }

if((((rrcmeas_current_substate == RRCMEAS_INITIAL)
   ||(rrcmeas_current_substate == RRCMEAS_IDLE)
   ||(rrcmeas_current_substate == RRCMEAS_PCH)
   )
   &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_IDLE_MEAS))

   || ((rrcmeas_current_substate == RRCMEAS_CELL_FACH)
       &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_FACH_MEAS))

   || ((rrcmeas_current_substate == RRCMEAS_CELL_DCH)
       &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_CM_MEAS))
  )
{
  WRRC_MSG2_HIGH("Skip IRAT/IFREQ meas  Disable IRAT/IFREQ NV %d  in meas state  %d",rrc_disable_meas_nv,rrcmeas_current_substate);
  l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = FALSE;
  return FAILURE;
}
  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip Processing interRAT SIB11 measurements");
    l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = FALSE;
    return FAILURE;
  }

  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
  if( sib3_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB3 ptr is NULL");
    return( FAILURE );
  }

  if (((rrcmeas_current_substate == RRCMEAS_CELL_FACH) || (rrcmeas_current_substate == RRCMEAS_PCH)) &&
    ((sib3_ptr->sib4indicator == TRUE)))
  {
    sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB4);

    if (sib4_ptr == NULL)
    {
      WRRC_MSG0_ERROR("SIB4 ptr NULL.Use SIB3");
    }
    else
    {
      WRRC_MSG0_HIGH("Use default values from SIB4");
      sib4_present = TRUE;
    }
  }

  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;
   /* SIB18 */
  if((sib18_ptr = (rrc_SysInfoType18*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB18)) != NULL)
  {
    if(((rrcmeas_current_substate == RRCMEAS_CELL_DCH)
      || (rrcmeas_current_substate == RRCMEAS_PCH)
      || (rrcmeas_current_substate == RRCMEAS_CELL_FACH)))
    {
      /*
       1>if System Information Block type 11bis is scheduled on BCH:
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities for SIB 11bis"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Idle mode PLMN identities for SIB11bis";
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Connected mode PLMN identities"; 
       2>otherwise, 
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
       
       1> else
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
      */
      if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
          connectedModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis,
            plmnsOfInterRATCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 Conn Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,idleModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
            plmnsOfInterRATCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 idle Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities,plmnsOfInterRATCellsList)) &&
        (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Conn Inter RAT");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterRATCellsList)) &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Inter RAT");
        sib18_present = TRUE;
      }
    }
    else
    {
   
      /*
       If SIB11bis is present and SIB18 bis extenstions are present, use SIB18 bis extension
       If SIB11bis is present and SIb18 bis extensions are absent, use SIB18
       If SIB11bis is absent, use SIB18
      */
      if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,idleModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
          plmnsOfInterRATCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 Idle Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities))
        &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterRATCellsList))
        &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Inter RAT ");
        sib18_present = TRUE;
      }
    }
  }
  if(rrc_ccm_get_serv_plmn_cellid(&tmp_cell_id, &selected_plmn_identity) == RRC_CELL_LOSS_NO_NEW_CELL)
  {
    sib18_present = FALSE;
  }
  else
  {
    WCDMA_MEMCPY(&tmp_plmn_identity, 
                 sizeof(rrc_plmn_identity_type),
                 &selected_plmn_identity, 
                 sizeof(rrc_plmn_identity_type));
  }

  if(rrccsp_is_ue_in_true_limited_service() == TRUE)
  {
    sib18_present = FALSE;
    WRRC_MSG0_HIGH("UE in limited service ignoring SIB18");
  }
  {
    rrc_MasterInformationBlock *mib_ptr = NULL;/* Declared to store the value of MIB*/
    if((mib_ptr = (rrc_MasterInformationBlock*)  rrc_sibdb_return_sib_for_srv_cell
      (rrc_MIB))!= NULL)
    {
      if(rrc_is_nw_shared(mib_ptr))
      {
        sib18_present = FALSE;
        WRRC_MSG0_HIGH("NWS: Ignore SIB18");
      }
      else
      {
        WRRC_MSG0_HIGH("NWS: N/W not shader so consider SIB18");
      }
    }
  }




  sib11_ptr = (rrc_SysInfoType11*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB11);

  /* Check if the value received is O.K. */

  if ( sib11_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB 11 from SIB DB is NULL");
    return FAILURE;
  }
  else
  {

    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
    {
       /* Check for REL 5 extension ptr */
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib11_ptr,rrc_SysInfoType11,v4b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE(sib11_ptr->v4b0NonCriticalExtensions,
        rrc_SysInfoType11_v4b0NonCriticalExtensions,v590NonCriticalExtension)))
      {
        sib11_rel5_ext_ptr = &(sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
          sysInfoType11_v590ext);
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(sib11_rel5_ext_ptr,rrc_SysInfoType11_v590ext_IEs,newInterRATCellInfoList_v590ext))
        {
          rel5_ext_ptr = &sib11_rel5_ext_ptr->newInterRATCellInfoList_v590ext;
        }
      }
    }
    /* Check if the FACH Measurement Occassion Info if present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(sib11_ptr,
        rrc_SysInfoType11,fach_MeasurementOccasionInfo))
    {
      l1_inter_rat_sib_ptr->fach_meas_incl = TRUE;

      /* Initialize the following value to Default values */
      l1_inter_rat_sib_ptr->fach.k_ind = FALSE;
      l1_inter_rat_sib_ptr->fach.inter_f_ind = FALSE;
      l1_inter_rat_sib_ptr->fach.num_inter_sys = 0;

      /* Check if fACH_meas_occasion_coeff present */
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib11_ptr->fach_MeasurementOccasionInfo,
        fACH_meas_occasion_coeff))
      {
        /* Set the flag */
        l1_inter_rat_sib_ptr->fach.k_ind = TRUE;
        l1_inter_rat_sib_ptr->fach.k =
          sib11_ptr->fach_MeasurementOccasionInfo.fACH_meas_occasion_coeff;
      }

      /* Check if Inter Freq indicator present */
      if (sib11_ptr->fach_MeasurementOccasionInfo.inter_freq_FDD_meas_ind == TRUE)
      {
        l1_inter_rat_sib_ptr->fach.inter_f_ind = TRUE;
      }

      /* Check if Inter RAT indicator present */
      local_sib_inter_rat_ptr = &sib11_ptr->fach_MeasurementOccasionInfo.inter_RAT_meas_ind;
      idx=0;
      while (local_sib_inter_rat_ptr->n > idx)
      {
        if (local_sib_inter_rat_ptr->elem[idx] == rrc_RAT_Type_gsm)
        {
          l1_inter_rat_sib_ptr->fach.num_inter_sys = 1;
          l1_inter_rat_sib_ptr->fach.inter_sys[0] = L1_GSM_SYSTEM;
          break;
        }
        idx++;
      }
    }
    /* Check if HCS is in use. */
    if (RRC_CHECK_COMMON_MSG_TYPE(sib11_ptr->measurementControlSysInfo.use_of_HCS,
         rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used))
    {
      WRRC_MSG0_HIGH("HCS=>TRUE");
      if(!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.
        interRATMeasurementSysInfoPresent))
      {
        WRRC_MSG0_HIGH("Inter-RAT Sys info absent in SIB11");
        if(rrcirat_read_sib11bis(l1_inter_rat_sib_ptr, sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter RAT in SIB11bis Ignored");
          l1_inter_rat_sib_ptr->inter_s.num_cell_add = 0;
          l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        }
        return SUCCESS;
      }
      if (rrcirat_fill_hcs_info(&sib11_ptr->measurementControlSysInfo.use_of_HCS.u.
        hcs_used->interRATMeasurementSysInfo, l1_inter_rat_sib_ptr, rel5_ext_ptr) == SUCCESS)
      {
        l1_inter_rat_sib_ptr->hcs_used = TRUE;
        return SUCCESS;
      }
      else
        return FAILURE;
    }
    else
    { /* HCS is NOT in use */
      if(!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.
         interRATMeasurementSysInfoPresent))
      {
        WRRC_MSG0_HIGH("Inter-RAT Sys info absent in SIB11");
        if(rrcirat_read_sib11bis(l1_inter_rat_sib_ptr, sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter RAT in SIB11bis Ignored");
          l1_inter_rat_sib_ptr->inter_s.num_cell_add = 0;
          l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        }
        return SUCCESS;
      }

      if (!(RRC_MSG_COMMON_BITMASK_IE(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo_B,interRATCellInfoList)))
      {
        WRRC_MSG0_HIGH("Inter-RAT list absent in SIB11");
        if(rrcirat_read_sib11bis(l1_inter_rat_sib_ptr,sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter RAT in SIB11bis Ignored");
          l1_inter_rat_sib_ptr->inter_s.num_cell_add = 0;
          l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        }
        return SUCCESS;
      }

      /* Store ptr given by UTRAN */
      inter_rat_cell_list_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList;

      if (inter_rat_cell_list_ptr == NULL)
      {
        WRRC_MSG0_HIGH("inter_rat_cell_list_ptr is NULL");
        if(rrcirat_read_sib11bis(l1_inter_rat_sib_ptr,sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter RAT in  SIB11bis Ignored");
          l1_inter_rat_sib_ptr->inter_s.num_cell_add = 0;
          l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        }
        return SUCCESS;
      }
      idx=idy=0;
      while ( (inter_rat_cell_list_ptr->n > idx) &&
        (cell_count < L1_MAX_CELL_MEAS) )
      {
      /* Let first check the type of the cell to find if we
        * really need to give this cell to L1 */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
          T_rrc_NewInterRATCell_B_technologySpecificInfo_gsm)
        {
          WRRC_MSG0_HIGH("Ignoring Non GSM cell");
          arfcn = MAX_ARFCN;
          if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
          {
            /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
            cell_position = (uint16)
              inter_rat_cell_list_ptr->elem[idx].interRATCellID;
          }
          else
          {
            /* Store the cell in first order position */
            cell_position = AVAILABLE_CELL_POSITION;
          }
          /* Tell to get the first available cell position and
          * that would be the cell id. The called function would store
          * the cell id back in passed cell_position variable */
          if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
          {
            WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
            return FAILURE;
          }
          idx++;
          continue;
        }
        else
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

          /* Cell reselection info is always set to TRUE. If not present in SIB 11, just
          set it to default values */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info_incl = TRUE;

          /* Initializw q-offset1-2 Defaults as per specs */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset1_s_n = QOFFSET1;

          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset2_s_n = QOFFSET2;

          /* -24. Got this value from Radio Access Capability */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.max_tx_pwr = -24;

            /* Not valid for Inter-RAT but initialize to default
          * as they are present */
          if ((sib4_present == TRUE) && (sib4_ptr != NULL))
          {
            if(RRC_CHECK_COMMON_MSG_TYPE(sib4_ptr->cellSelectReselectInfo.modeSpecificInfo,
                     rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
            {
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.qual_min = (int16)
                sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;

              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.rxlev_min = (int16)
                (sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            }
            else
            {
              WRRC_MSG1_ERROR("ERR: SIB4 mode_choice NOT FDD %d", sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
            }
          }
          else
          {
            if(RRC_CHECK_COMMON_MSG_TYPE(sib3_ptr->cellSelectReselectInfo.modeSpecificInfo,
                     rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
            {
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.qual_min = (int16)
                sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.rxlev_min = (int16)
                (sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            }
            else
            {
              WRRC_MSG1_ERROR("ERR: SIB3 mode_choice NOT FDD %d", sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
            }
          }
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.hcs_info.hcs_prio = 0;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.q_hcs = 110;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.hcs_info.penalty_time = 0;

          /* Get GSM CELL Info */
          /* Check if rrc_gsm_cellSelectionReselectionInfo_present */
          if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->m.
            cellSelectionReselectionInfoPresent)
          {
            MSG_LOW("Cell Reselection info present",0,0,0);

            /* q-Offset1S-N */
          
              /* Range is -50...50 */
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.qoffset1_s_n = (int16)
                inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.q_Offset1S_N;
            

            /* q_Offset2S_N  -50...50 */
            if (RRC_MSG_COMMON_BITMASK_IE(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo,
              rrc_CellSelectReselectInfoSIB_11_12,q_Offset2S_N))
            {
              /* Range is -50...50 */
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.qoffset2_s_n =  (int16)
                inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.q_Offset2S_N;
            }

            /* Max Txit Power */
            if (RRC_MSG_COMMON_BITMASK_IE(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo,
              rrc_CellSelectReselectInfoSIB_11_12,maxAllowedUL_TX_Power))
            {
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.max_tx_pwr = (int16)
                inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.maxAllowedUL_TX_Power;
            }

            /* Check if gsm mode present  */
            if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.
                 modeSpecificInfo.t == T_rrc_CellSelectReselectInfoSIB_11_12_modeSpecificInfo_gsm)
            {
              /* Check if rxlev_min present */

              if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.modeSpecificInfo.u
                .gsm->m.q_RxlevMinPresent)
              {
                /* Store rxlev_min in L1 */
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min = (int16)
                  ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.modeSpecificInfo.u.gsm->q_RxlevMin * 2) + 1);
              }
            }
            else
            {
              WRRC_MSG0_ERROR("Incorrect mode specific info for GSM entry in SIB11");
            }
            /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
              delta_qrxlevmin_included = FALSE;  */
            if(rel5_ext_ptr != NULL)
            {
              if(RRC_MSG_LIST_BITMASK_IE_TYPE2(rel5_ext_ptr,deltaQrxlevmin))
              {
                /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                  delta_qrxlevmin_included = TRUE;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                  delta_qrxlevmin = rel5_ext_ptr->value.deltaQrxlevmin * 2; */
                MSG_HIGH("Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                  (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2),
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                  CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));

                /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                     (rel5_ext_ptr->value.deltaQrxlevmin * 2)) < -115)
                {*/
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +=
                     (int16) (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2);
                /*}*/
              }
            }

          } /* rrc_gsm_cellSelectionReselectionInfo_present */

          /* Cell Individual Offset */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->interRATCellIndividualOffset;

          /* Get BAND 1800 DCS or 1900 PCS */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
            RET_INTER_RAT_BAND(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->frequency_band);

          /* Get bcch_ARFCN */
          CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) =
            (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->bcch_ARFCN;

          arfcn =(uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bcch_ARFCN;

#ifdef FEATURE_GSM_QUADBAND

          if ((converted_gsm_band = rrc_get_gsm_band(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info)) !=
            INVALID_BAND)
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
              converted_gsm_band;
          }
          else
          {
            WRRC_MSG1_HIGH("ARFCN %d ignored bec of invalid band",inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
              u.gsm->bcch_ARFCN);
            arfcn = MAX_ARFCN;
            /* Increment the pointer */
            if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
            {
              /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
              cell_position = (uint16)
                inter_rat_cell_list_ptr->elem[idx].interRATCellID;
            }
            else
            {
              /* Store the cell in first order position */
              cell_position = AVAILABLE_CELL_POSITION;
            }
            /* Tell to get the first available cell position and
            * that would be the cell id. The called function would store
            * the cell id back in passed cell_position variable */
            if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
            {
              WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
              return FAILURE;
            }
            idx++;
            continue;
          }
#endif /* FEATURE_GSM_QUADBAND */

          /* To add BSIC (Base Station Identification Code) */
          if ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
            > NCC_MAX) ||
            (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
            > BCC_MAX))
          {
            WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

            return FAILURE;

          } /* End of if of checking NCC and BCC validity */

          /* Store good values of bcc and ncc */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc =
            (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->bsic.ncc;

          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc =
            (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->bsic.bcc;
        } /* End of gsm cell  */

        /* Time to get its cell id*/
        /* Cell Order has already been verified */

        if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
        {
          /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
          cell_position = (uint16)
            inter_rat_cell_list_ptr->elem[idx].interRATCellID;

        }
        else
        {
          /* Store the cell in first order position */
          cell_position = AVAILABLE_CELL_POSITION;
        }
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
        if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
        {
          WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
          return FAILURE;
        }
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id
          = cell_position;
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].
          measure_in_idle_pch_fach = TRUE;

        if(sib18_present)
        {
          if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->n > idy)
          {
            if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[idy].m.plmn_IdentityPresent)
            {
              rrc_translate_gsm_map_plmn_id(&tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[idy].plmn_Identity,
                &tmp_plmn_identity);
              if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
              {
                if((rrccsp_check_for_eplmn(
                  mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                  tmp_plmn_identity))) == FALSE)
                {
                  WRRC_MSG1_HIGH("cell id %d Non eq PLMN measure only in DCH",
                    l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
                  /* set the flag to FALSE */
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = FALSE;
                }
              }
            }
            else
            {
              /* use the previous value of the flag if PLMN id is not selected PLMN */
              if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
              {
                /* Add code to set the flag meas_pch_fach_idle to the flag value of the previous cell */
                if(cell_count > 0)
                {
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach =
                    l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].measure_in_idle_pch_fach;
                }
              }
            }

            idy++;
          }
          else
          {
            /* PLMN list is shorter than the cell info list set flag to true for the remaining cells */
            WRRC_MSG1_HIGH("PLMN list < Cell list. cell id %d measured in IDLE,FACH,PCH,DCH",
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
          }
        }
        /* Increment cell count */
        cell_count++;

        /* Increment the pointer */
        idx++;
        if ((inter_rat_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
        {
          WRRC_MSG0_HIGH("Inter-RAT Cell List full");
          break;
        }

      } /* End of while inter_rat_cell_list_ptr */

      /* Finally Set the cell count */
      l1_inter_rat_sib_ptr->inter_s.num_cell_add = cell_count;

      /* Now set inter sys info incl info to TRUE */
      l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
      l1_inter_rat_sib_ptr->hcs_used = FALSE;

      prev_cell_count = l1_inter_rat_sib_ptr->inter_s.num_cell_add;
      if(rrcirat_read_sib11bis(l1_inter_rat_sib_ptr, sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr) == FAILURE)
      {
        WRRC_MSG0_HIGH("SIB11bis: Reading Sib11bis FAILURE");
        l1_inter_rat_sib_ptr->inter_s.num_cell_add = prev_cell_count;
      }


      WRRC_MSG0_HIGH("IRAT CELL INFO = FALSE");
      interRAT_cell_info_status.interRAT_cell_info_present = FALSE;
      interRAT_cell_info_status.interRAT_cell_info_ind = 0;

      if (RRC_CHECK_COMMON_MSG_TYPE(sib11_ptr->measurementControlSysInfo.use_of_HCS,
           rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used ))
      {
        WRRC_MSG0_HIGH("HCS=>TRUE");
        if(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.
            interRATMeasurementSysInfoPresent)
        {
          if (RRC_MSG_COMMON_BITMASK_IE(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->interRATMeasurementSysInfo,
           rrc_InterRATMeasurementSysInfo,interRATCellInfoList))
          {
            WRRC_MSG0_HIGH("IRAT CELL INFO = TRUE");
            interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
            interRAT_cell_info_status.interRAT_cell_info_ind = 0;
            return SUCCESS;
          }
        }
      }
      else
      {
        if(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.
          interRATMeasurementSysInfoPresent)
        {
          if (RRC_MSG_COMMON_BITMASK_IE(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
            interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo_B,interRATCellInfoList))
          {
            WRRC_MSG0_HIGH("IRAT CELL INFO = TRUE");
            interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
            interRAT_cell_info_status.interRAT_cell_info_ind = 0;
            return SUCCESS;
          }
        }
      }



      return SUCCESS;

    } /* End of else of HCS not in use */


  } /* End of else of sib11_ptr = NULL */

}

/*===========================================================================

FUNCTION rrcirat_process_remove_cell_list

DESCRIPTION

 This function processes the remove cell list of SIB12 and updates the inter
 rat cell list

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE
 None

SIDE EFFECTS

 None

===========================================================================*/
void rrcirat_process_remove_cell_list
(
rrc_RemovedInterRATCellList *removedInterRATCellList,
uint16 *num_removed_cells
)
{
  uint32 inter_rat_cell_id;

  uint32 idx=0;

  *num_removed_cells = 0;
  switch(removedInterRATCellList->t)
  {
    case T_rrc_RemovedInterRATCellList_removeAllInterRATCells:
         WRRC_MSG0_HIGH("remove all cells from inter rat cells list");
         *num_removed_cells = L1_MAX_CELL_MEAS;
         rrcirat_initialize_cell_list();
      break;

    case T_rrc_RemovedInterRATCellList_removeSomeInterRATCells:
         if((removedInterRATCellList->u.removeSomeInterRATCells == NULL)
            ||(removedInterRATCellList->u.removeSomeInterRATCells->n == 0))
         {
           MSG_LOW("Assign no of removed cells to be 0", 0,0,0);
         }
         else
         {
          
           do
           {
             inter_rat_cell_id = removedInterRATCellList->u.removeSomeInterRATCells->elem[idx];

             /* Remove cell at position given by the cell id from the inter rat cell info list */
             if(inter_rat_cell_id < L1_MAX_CELL_MEAS)
             {
               inter_rat_cell_list[inter_rat_cell_id].cell_position = VACANT;
               (*num_removed_cells)++; /* Increment count */
             }

             /*  Get next element in the list */
            WRRC_MSG1_HIGH("removed  cell id %d from inter rat cell list", inter_rat_cell_id);
            idx++;

           } /* Continue if cond satisfied */
           while ((removedInterRATCellList->u.removeSomeInterRATCells->n > idx) &&
            ((*num_removed_cells) < L1_MAX_CELL_MEAS));
         }
      break;

    case T_rrc_RemovedInterRATCellList_removeNoInterRATCells:
         /* no cells need to be removed */
         MSG_LOW("Assign no of removed cells to be 0", 0,0,0);
      break;

    default:
      /* cannot come here */
      break;
  }
}

/*===========================================================================

FUNCTION rrcirat_build_composite_cell_list_from_SIB11_SIB12

DESCRIPTION

 This function builds a composite cell info list from SIB11 and SIB12 that is
 then sent down to L1

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcirat_build_composite_cell_list_from_SIB11_SIB12
(
rrc_RemovedInterRATCellList *removedInterRATCellList,
l1_inter_sys_cell_list_struct_type* sib11_cell_list_ptr,
l1_inter_sys_cell_list_struct_type* sib12_cell_list_ptr,
uint16 num_removed_cells
)
{
/* Initialize all variables */

  /* Stores no of intra-freq cells stored in SIB11 */
  uint32 sib11_added_cells = 0;

  /* Stores no of intra-freq cells stored in SIB12 */
  uint32 sib12_added_cells = 0;

  /* Counts no of cells in SIB12 */
  uint32 sib12_count_cells = 0;

  /* Counts removed cells matched against new inter rat cells of SIB11 */
  uint32 sib12_r_cell = 0;

  /* Counts no of new intra freq cells in SIB11 and helps in comparing
  * with no fo removed cells in SIB12 */
  uint32 sib11_a_cell = 0;

  /* If removed cell is found */
  uint32 match_found = 0;

  uint16 new_cell_list = 0;
  uint16 new_cell_list_from_sib11 = 0;
  uint16 count = 0;



  /* Stores ptr to SIB12 list */
  l1_inter_sys_cell_list_struct_type* new_cell_list_ptr = NULL;

  sib11_added_cells = sib11_cell_list_ptr->num_cell_add;
  /* initialize tmp cell list to satisfy lint */

  if(removedInterRATCellList->t == T_rrc_RemovedInterRATCellList_removeAllInterRATCells)
  {
    /* No cells need to be picked up from SIB 11 */
    sib12_cell_list_ptr->num_cell_rmv = 0;
    return;
  }
  /* First remove no of removed cells in SIB12 from SIB11 and store result
  * in a temporary list i.e. inter_sys_sib12_new_cell_list */


  /* Criteria: Match removed cell of SIB12 against all cells of SIB11
  * one at a time */

  for ( sib11_a_cell=0 ; sib11_a_cell<sib11_added_cells; sib11_a_cell++ )
  {
    
    count =0;
    for ( sib12_r_cell=0; sib12_r_cell <num_removed_cells; sib12_r_cell++ )
    {
      if(removedInterRATCellList->t == T_rrc_RemovedInterRATCellList_removeSomeInterRATCells)
      {
        if((removedInterRATCellList->u.removeSomeInterRATCells != NULL)
            && (removedInterRATCellList->u.removeSomeInterRATCells->n > count))
            
        {
          if(removedInterRATCellList->u.removeSomeInterRATCells->elem[count] !=
            sib11_cell_list_ptr->add_cell[sib11_a_cell].intersys_cell_id)
          {
            count++;
            continue; /* Read next value of removed cell*/
          }
        }
        match_found = 1;
        break;
      }
      if(match_found == 1)
      {
         /* Do not add this cell to the composite list */
         break;
      }
    }  /* End of for for sib12_r_cell */

    if ( match_found == 0 )
    {
      MSG_LOW("Copying SIB 11 cell back in new list", 0,0,0);

      inter_sys_sib12_new_cell_list.add_cell[new_cell_list].intersys_cell_id =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].intersys_cell_id;

      inter_sys_sib12_new_cell_list.add_cell[new_cell_list].measure_in_idle_pch_fach =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].measure_in_idle_pch_fach;
      inter_sys_sib12_new_cell_list.add_cell[new_cell_list].cell_info_type =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].cell_info_type;

      inter_sys_sib12_new_cell_list.add_cell[new_cell_list].u.gsm_cell =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].u.gsm_cell;

      /* Increment counter */
      new_cell_list++;

    } /* End of if */

    /* Reinitialize match_found variable */
    match_found = 0;
    /* Read next SIB 11 cell */
  } /* End of for sib11_a_cell */

  new_cell_list_ptr = &inter_sys_sib12_new_cell_list;
  new_cell_list_from_sib11 = new_cell_list;

  /* Check if SIB12 new intra freq cells have id same as the one already present
   * from SIB 11 if so overwrite, if not append to the list */
  for ( sib12_added_cells =0; sib12_added_cells < sib12_cell_list_ptr->num_cell_add && new_cell_list < L1_MAX_CELL_MEAS;
  sib12_added_cells++ )
  {
    for(count = 0; count < new_cell_list_from_sib11; count++)
    {
      if(sib12_cell_list_ptr->add_cell[sib12_added_cells].intersys_cell_id
         == new_cell_list_ptr->add_cell[count].intersys_cell_id)
      {
        new_cell_list_ptr->add_cell[count].measure_in_idle_pch_fach =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].measure_in_idle_pch_fach;
        new_cell_list_ptr->add_cell[count].cell_info_type =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].cell_info_type;

        new_cell_list_ptr->add_cell[count].u.gsm_cell =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].u.gsm_cell;
        break;
      }
    }
    if(count == new_cell_list_from_sib11)
    {
      new_cell_list_ptr->add_cell[new_cell_list].intersys_cell_id =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].intersys_cell_id;

      new_cell_list_ptr->add_cell[new_cell_list].measure_in_idle_pch_fach =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].measure_in_idle_pch_fach;
      new_cell_list_ptr->add_cell[new_cell_list].cell_info_type =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].cell_info_type;

      new_cell_list_ptr->add_cell[new_cell_list].u.gsm_cell =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].u.gsm_cell;

      new_cell_list++;
    }
  } /* End of for */

  /* Store the no of cells */
  sib12_cell_list_ptr->num_cell_add = new_cell_list;

  MSG_LOW("There are %d cells in SIB12 cell list", new_cell_list, 0,0);

  /* Now copy the structure in SIB12 */
  for (sib12_count_cells =0; sib12_count_cells<new_cell_list && sib12_count_cells < L1_MAX_CELL_MEAS;
   sib12_count_cells++)
  {

    sib12_cell_list_ptr->add_cell[sib12_count_cells].intersys_cell_id =
      new_cell_list_ptr->add_cell[sib12_count_cells].intersys_cell_id;

    sib12_cell_list_ptr->add_cell[sib12_count_cells].measure_in_idle_pch_fach =
      new_cell_list_ptr->add_cell[sib12_count_cells].measure_in_idle_pch_fach;
    sib12_cell_list_ptr->add_cell[sib12_count_cells].cell_info_type =
      new_cell_list_ptr->add_cell[sib12_count_cells].cell_info_type;

    sib12_cell_list_ptr->add_cell[sib12_count_cells].u.gsm_cell =
      new_cell_list_ptr->add_cell[sib12_count_cells].u.gsm_cell;
  } /* End of for */

    /* Initialize no of removed cells to 0 as SIB12 intra-freq cell info list has
  * already taken them into account */
  sib12_cell_list_ptr->num_cell_rmv = 0;
}

/*===========================================================================

FUNCTION rrcirat_read_sib12

DESCRIPTION

 Caller must check the sib12_present before calling this fn.
 This function processes SIB12 IEs and build the information
 to be sent down to L1 and updates the cell info list for
 inter rat measurements.

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE

 SUCCESS/FAIILURE

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib12
(
rrc_SysInfoType12 *sib12_ptr,
l1_meas_sib_parm_struct_type* l1_inter_rat_sib_ptr
)
{
  rrc_SysInfoType3  *sib3_ptr = NULL;

  rrc_SysInfoType4  *sib4_ptr = NULL;

  rrc_NewInterRATCellList_B* inter_rat_cell_list_ptr = NULL;

  rrc_NewInterRATCellList* hcs_inter_rat_cell_list_ptr = NULL;

  uint8 cell_count = 0;

  uint16 cell_position;

  boolean sib4_present = FALSE;

  uint16 num_removed_cells;
  uint32 idx=0;
  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterRATCellsList *tmp_rrc_PLMNsOfInterRATCellsList_ptr = NULL;
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  boolean sib18_present = FALSE;


  rrc_FACH_MeasurementOccasionInfo_inter_RAT_meas_ind* local_sib_inter_rat_ptr = NULL;

  rrc_SysInfoType12_v590ext_IEs *sib12_rel5_ext_ptr = NULL;
  rrc_SysInfoType12_v590ext_IEs_newInterRATCellInfoList_v590ext * rel5_ext_ptr = NULL;

  uint16 arfcn;

  /* First check if dual mode is enabled */
  if (rrcmcm_is_dualmode_enabled() == FALSE)
  {
    WRRC_MSG0_HIGH("IRAT:Dual Mode not enabled");
    return FAILURE;
  }

if((((rrcmeas_current_substate == RRCMEAS_INITIAL)
   ||(rrcmeas_current_substate == RRCMEAS_IDLE)
   ||(rrcmeas_current_substate == RRCMEAS_PCH)
  )
  &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_IDLE_MEAS))

  ||((rrcmeas_current_substate == RRCMEAS_CELL_FACH)
      &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_FACH_MEAS))

  || ((rrcmeas_current_substate == RRCMEAS_CELL_DCH)
       &&(rrc_disable_meas_nv & RRC_DISABLE_WTOG_CM_MEAS))
 )
{
  WRRC_MSG2_HIGH("Skip IRAT/IFREQ meas  Disable IRAT/IFREQ NV %d  in meas state  %d",rrc_disable_meas_nv,rrcmeas_current_substate);
  l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = FALSE;
  return FAILURE;
}
  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip Processing interRAT SIB12 measurements");
    l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = FALSE;
    return FAILURE;
  }

  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;

  if((sib18_ptr = (rrc_SysInfoType18*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB18)) != NULL)
  {
      /*
       1>if System Information Block type 11bis is scheduled on BCH:
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities for SIB 11bis"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Idle mode PLMN identities for SIB11bis";
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Connected mode PLMN identities"; 
       2>otherwise, 
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
       
       1> else
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
      */
    if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,connectedModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis,
          plmnsOfInterRATCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
          connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
      WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 Conn Inter RAT from bis extension ");
      sib18_present = TRUE;
    }
    else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,idleModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
          plmnsOfInterRATCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
          idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
      WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 idle Inter RAT from bis extension ");
      sib18_present = TRUE;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,connectedModePLMNIdentities)) &&
      (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities,plmnsOfInterRATCellsList)) &&
      (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList;
      WRRC_MSG0_HIGH("SIB11 processing SIB18 Conn Inter RAT");
      sib18_present = TRUE;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities)) &&
      (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities, plmnsOfInterRATCellsList)) &&
      (sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList;
      WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Inter RAT");
      sib18_present = TRUE;
    }
  }

  if(rrc_ccm_get_serv_plmn_cellid(&tmp_cell_id, &selected_plmn_identity) == RRC_CELL_LOSS_NO_NEW_CELL)
  {
    sib18_present = FALSE;
  }
  else
  {
    WCDMA_MEMCPY(&tmp_plmn_identity, 
                 sizeof(rrc_plmn_identity_type),
                 &selected_plmn_identity, 
                 sizeof(rrc_plmn_identity_type));
  }

  if(rrccsp_is_ue_in_true_limited_service() == TRUE)
  {
    sib18_present = FALSE;
    WRRC_MSG0_HIGH("UE in limited service ignoring SIB18");
  }
  {
    rrc_MasterInformationBlock *mib_ptr = NULL;/* Declared to store the value of MIB*/
    if((mib_ptr = (rrc_MasterInformationBlock*)  rrc_sibdb_return_sib_for_srv_cell
      (rrc_MIB))!= NULL)
    {
      if(rrc_is_nw_shared(mib_ptr))
      {
        sib18_present = FALSE;
        WRRC_MSG0_HIGH("NWS: Ignore SIB18");
      }
      else
      {
        WRRC_MSG0_HIGH("NWS: N/W not shader so consider SIB18");
      }
    }
  }



  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
  if( sib3_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB3 ptr is NULL");
    return( FAILURE );
  }

  if (((rrcmeas_current_substate == RRCMEAS_CELL_FACH) || (rrcmeas_current_substate == RRCMEAS_PCH)) &&
    ((sib3_ptr->sib4indicator == TRUE)))
  {
    sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB4);

    if (sib4_ptr == NULL)
    {
      WRRC_MSG0_ERROR("SIB4 ptr NULL.Use SIB3");
    }
    else
    {
      WRRC_MSG0_HIGH("Use default values from SIB4");
      sib4_present = TRUE;
    }
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    /* Check for REL 5 extension ptr */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib12_ptr,rrc_SysInfoType12,v4b0NonCriticalExtensions)) &&
      (RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->v4b0NonCriticalExtensions,
      rrc_SysInfoType12_v4b0NonCriticalExtensions,v590NonCriticalExtension)))
    {
      sib12_rel5_ext_ptr = &(sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
        sysInfoType12_v590ext);
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(sib12_rel5_ext_ptr,rrc_SysInfoType12_v590ext_IEs,newInterRATCellInfoList_v590ext))
      {
        rel5_ext_ptr = &sib12_rel5_ext_ptr->newInterRATCellInfoList_v590ext;
      }
    }
  }

  /* Check if the FACH Measurement Occassion Info if present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(sib12_ptr,rrc_SysInfoType12,fach_MeasurementOccasionInfo))
  {
    l1_inter_rat_sib_ptr->fach_meas_incl = TRUE;

    /* Initialize the following value to Default values */
    l1_inter_rat_sib_ptr->fach.k_ind = FALSE;
    l1_inter_rat_sib_ptr->fach.inter_f_ind = FALSE;
    l1_inter_rat_sib_ptr->fach.num_inter_sys = 0;

    /* Check if fACH_meas_occasion_coeff present */
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib12_ptr->fach_MeasurementOccasionInfo,fACH_meas_occasion_coeff))
    {
      /* Set the flag */
      l1_inter_rat_sib_ptr->fach.k_ind = TRUE;
      l1_inter_rat_sib_ptr->fach.k =
        sib12_ptr->fach_MeasurementOccasionInfo.fACH_meas_occasion_coeff;
    }

    /* Check if Inter Freq indicator present */
    if (sib12_ptr->fach_MeasurementOccasionInfo.inter_freq_FDD_meas_ind == TRUE)
    {
      l1_inter_rat_sib_ptr->fach.inter_f_ind = TRUE;
    }

    /* Check if Inter RAT indicator present */
    local_sib_inter_rat_ptr = &sib12_ptr->fach_MeasurementOccasionInfo.inter_RAT_meas_ind;
    idx=0;
    while (local_sib_inter_rat_ptr->n > idx)
    {
      if (local_sib_inter_rat_ptr->elem[idx] == rrc_RAT_Type_gsm)
      {
        l1_inter_rat_sib_ptr->fach.num_inter_sys = 1;
        l1_inter_rat_sib_ptr->fach.inter_sys[0] = L1_GSM_SYSTEM;
        break;
      }
      idx++;
    }
  }
  else if(rrcmeas_sib11->fach_meas_incl == TRUE)
  {
    l1_inter_rat_sib_ptr->fach_meas_incl = TRUE;

    /* Initialize the following value to Default values */
    l1_inter_rat_sib_ptr->fach.k_ind = rrcmeas_sib11->fach.k_ind;
    l1_inter_rat_sib_ptr->fach.inter_f_ind = rrcmeas_sib11->fach.inter_f_ind;
    l1_inter_rat_sib_ptr->fach.k =  rrcmeas_sib11->fach.k;
    l1_inter_rat_sib_ptr->fach.num_inter_sys = rrcmeas_sib11->fach.num_inter_sys;
    l1_inter_rat_sib_ptr->fach.inter_sys[0] = rrcmeas_sib11->fach.inter_sys[0];
  }

  /* Check if HCS is in use. */
  if(RRC_CHECK_COMMON_MSG_TYPE(sib12_ptr->measurementControlSysInfo.use_of_HCS,
     rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used ))
  {
    /* start HCS processing */
    WRRC_MSG0_HIGH("HCS Processing");
    if(!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.
        interRATMeasurementSysInfoPresent))
    {
      WRRC_MSG0_HIGH("Inter-RAT Sys info absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
        l1_inter_rat_sib_ptr->hcs_used = TRUE;
      }
      return SUCCESS;
    }
    if (!(RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo,interRATCellInfoList)))
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
        l1_inter_rat_sib_ptr->hcs_used = TRUE;
      }
      return SUCCESS;
    }

    /* Store ptr given by UTRAN */
    hcs_inter_rat_cell_list_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList;

    if (hcs_inter_rat_cell_list_ptr == NULL)
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
        l1_inter_rat_sib_ptr->hcs_used = TRUE;
      }
      return SUCCESS;
    }

    /* Process the remove cell list of SIB12 */
    rrcirat_process_remove_cell_list(&sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo.interRATCellInfoList.removedInterRATCellList, &num_removed_cells);
    idx=0;
    while ( (hcs_inter_rat_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {
      /* Let first check the type of the cell to find if we
      * really need to give this cell to L1 */
      if (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("Ignoring Non GSM cell");
        arfcn = MAX_ARFCN;
        if (RRC_MSG_LIST_BITMASK_IE_TYPE2(hcs_inter_rat_cell_list_ptr,interRATCellID))
        {
          /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
          cell_position = (uint16)
            hcs_inter_rat_cell_list_ptr->elem[idx].interRATCellID;
        }
        else
        {
          /* Store the cell in first order position */
          cell_position = AVAILABLE_CELL_POSITION;
        }
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
        if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
        {
          WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
          return FAILURE;
        }

        idx++;
        continue;
      }
      else
      {
       /* l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.delta_qrxlevmin_included = FALSE; */

        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

        /* Cell reselection info is always set to TRUE. If not present in SIB 11, just
        set it to default values */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info_incl = TRUE;

        /* Initializw q-offset1-2 Defaults as per specs */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.qoffset1_s_n = QOFFSET1;

        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.qoffset2_s_n = QOFFSET2;

        /* -24. Got this value from Radio Access Capability */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.max_tx_pwr = -24;

          /* Not valid for Inter-RAT but initialize to default
        * as they are present */
        if ((sib4_present == TRUE) && (sib4_ptr != NULL))
        {
          if(RRC_CHECK_COMMON_MSG_TYPE(sib4_ptr->cellSelectReselectInfo.modeSpecificInfo,
             rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qual_min = (int16)
              sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.rxlev_min = (int16)
              (sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            /* Check for REL 5 extension ptr */
            if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
              (sib4_ptr->m.v4b0NonCriticalExtensionsPresent) &&
              (sib4_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) &&
              (sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType4_v590ext.m.
              cellSelectReselectInfo_v590extPresent) && (sib4_ptr->v4b0NonCriticalExtensions.
              v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.m.
              deltaQrxlevminPresent))
            {
  
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
               gsm_cell.cell_sel_info.delta_qrxlevmin = sib4_ptr->v4b0NonCriticalExtensions.
               v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2; */
  
              MSG_HIGH("SIB4 Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (sib4_ptr->v4b0NonCriticalExtensions.
                 v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2),
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));
  
              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                   (sib4_ptr->v4b0NonCriticalExtensions.
                    v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.rxlev_min += (int16) (sib4_ptr->v4b0NonCriticalExtensions.
                 v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2);
              /*}*/
            }
          }
          else
          {
            WRRC_MSG1_ERROR("ERR: SIB4 mode_choice NOT FDD %d", sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
          }
        }
        else
        {
          if(RRC_CHECK_COMMON_MSG_TYPE(sib3_ptr->cellSelectReselectInfo.modeSpecificInfo,
             rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qual_min = (int16)
              sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.rxlev_min = (int16)
              (sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            /* Check for REL 5 extension ptr */
            if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
              (sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType3_v590ext.m.
              cellSelectReselectInfo_v590extPresent) && (sib3_ptr->v4b0NonCriticalExtensions.
              v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.m.
              deltaQrxlevminPresent))
            {
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin = sib3_ptr->v4b0NonCriticalExtensions.
                v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2; */
              MSG_HIGH("SIB3 Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (sib3_ptr->v4b0NonCriticalExtensions.
                  v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2),
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));

              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                   (sib3_ptr->v4b0NonCriticalExtensions.
                    v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.rxlev_min += (int16) (sib3_ptr->v4b0NonCriticalExtensions.
                  v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2);
              /*}*/
            }
          }
          else
          {
            WRRC_MSG1_ERROR("ERR: SIB3 mode_choice NOT FDD %d", sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
          }
        }

        /* Set default values for the HCS neighboring cell info if it is first cell
        * else use the info from the previous cell */
        if(cell_count == 0)
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.hcs_prio = 0;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.q_hcs = 110;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.penalty_time = 0;
        }
        else
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.hcs_prio =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.hcs_prio;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.q_hcs =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.q_hcs;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.penalty_time =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.penalty_time;

          if(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.penalty_time != 0)
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.hcs_info.tmp_offset1 =
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
              gsm_cell.hcs_info.tmp_offset1;
          }
        }

        /* Get GSM CELL Info */
        /* Check if rrc_gsm_cellSelectionReselectionInfo_present */
        if (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->m.cellSelectionReselectionInfoPresent)
        {
          MSG_LOW("Cell Reselection info present",0,0,0);

          /* q-Offset1S-N */

            /* Range is -50...50 */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qoffset1_s_n = (int16)
              hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.q_Offset1S_N;
          
          /* q_Offset2S_N  -50...50 */
          if (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.m.
              q_Offset2S_NPresent)
          {
            /* Range is -50...50 */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qoffset2_s_n =  (int16)
              hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.q_Offset2S_N;
          }

          /* Max Txit Power */
          if (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.m.
            maxAllowedUL_TX_PowerPresent)
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.max_tx_pwr = (int16)
              hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.maxAllowedUL_TX_Power;
          }

          /* Check if gsm mode present  */
          if(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.
               modeSpecificInfo.t == T_rrc_CellSelectReselectInfoSIB_11_12_modeSpecificInfo_gsm)
          {
            /* Check if rxlev_min present */

            if (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.modeSpecificInfo.u
              .gsm->m.q_RxlevMinPresent)
            {
              /* Store rxlev_min in L1 */
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min = (int16)
                ((hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.modeSpecificInfo.u.gsm->q_RxlevMin * 2) + 1);
            }
          }
          else
          {
            WRRC_MSG0_ERROR("Incorrect mode specific info for GSM entry in SIB12");
          }
          if(rel5_ext_ptr != NULL)
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2(rel5_ext_ptr,deltaQrxlevmin))
            {
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                delta_qrxlevmin = rel5_ext_ptr->value.deltaQrxlevmin * 2;        */
              MSG_HIGH("Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2),
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));

              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                   (rel5_ext_ptr->value.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +=
                  (int16) (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2);
              /*}*/
            }
          }
          /* HCS inforamtion */
          if(RRC_MSG_COMMON_BITMASK_IE(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo,
            rrc_CellSelectReselectInfoSIB_11_12,hcs_NeighbouringCellInformation_RSCP))
          {
            /* Default HCS params */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.hcs_info.hcs_prio = 0;
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.hcs_info.q_hcs = 110;
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 0;

            /* HCS PRIO*/
           
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.hcs_info.hcs_prio = (uint16)
                hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_PRIO;
            

            /*Q HCS */
            
              if(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS < 90)
              {
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.hcs_info.q_hcs = (uint16) (110 -
                  hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS);
              }
              else
              {
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.hcs_info.q_hcs = 37;
              }
            
            /* Delta Qhcs is not needed if it is not mapped from CPICH-RSCP or PCCPCH-RSCP.
             * For inter rat cells, Qhcs is always mapped from RSSI
             */

            /* PENALTY TIME*/
            if(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
              .penaltyTime.t != T_rrc_PenaltyTime_RSCP_notUsed)
            {
              switch(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.t)
              {
              case T_rrc_PenaltyTime_RSCP_pt10:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 10;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt10);
                break;

              case T_rrc_PenaltyTime_RSCP_pt20:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 20;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt20);
                break;

              case T_rrc_PenaltyTime_RSCP_pt30:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 30;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt30);
                break;

              case T_rrc_PenaltyTime_RSCP_pt40:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 40;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt40);
                break;

              case T_rrc_PenaltyTime_RSCP_pt50:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 50;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt50);
                break;

              case T_rrc_PenaltyTime_RSCP_pt60:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 60;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt60);
                break;

              default:  /* should not happen */
                break;
              }
            }
          }

        } /* rrc_gsm_cellSelectionReselectionInfo_present */

        /* Cell Individual Offset */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
          hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->interRATCellIndividualOffset;

        /* Get BAND 1800 DCS or 1900 PCS */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
          RET_INTER_RAT_BAND(hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->frequency_band);

        /* Get bcch_ARFCN */
        CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) =
          (uint16) hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN;

        arfcn = (uint16) hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bcch_ARFCN;

#ifdef FEATURE_GSM_QUADBAND

        if ((converted_gsm_band = rrc_get_gsm_band(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info)) !=
          INVALID_BAND)
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
            converted_gsm_band;
        }
        else
        {
          WRRC_MSG1_HIGH("ARFCN %d ignored bec of invalid band",hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->bcch_ARFCN);
          arfcn = MAX_ARFCN;
          /* Increment the pointer */
          if (RRC_MSG_LIST_BITMASK_IE_TYPE2(hcs_inter_rat_cell_list_ptr,interRATCellID))
          {
            /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
            cell_position = (uint16)
              hcs_inter_rat_cell_list_ptr->elem[idx].interRATCellID;
          }
          else
          {
            /* Store the cell in first order position */
            cell_position = AVAILABLE_CELL_POSITION;
          }
          /* Tell to get the first available cell position and
          * that would be the cell id. The called function would store
          * the cell id back in passed cell_position variable */
          if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
          {
            WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
            return FAILURE;
          }
          idx++;
          continue;
        }
#endif /* FEATURE_GSM_QUADBAND */

        /* To add BSIC (Base Station Identification Code) */
        if ((hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
          > NCC_MAX) ||
          (hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
          > BCC_MAX))
        {
          WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
            hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
            hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

          return FAILURE;

        } /* End of if of checking NCC and BCC validity */

        /* Store good values of bcc and ncc */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc =
          (uint8) hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.ncc;

        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc =
          (uint8) hcs_inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.bcc;
      } /* End of gsm cell  */

      /* Time to get its cell id*/
      /* Cell Order has already been verified */

      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(hcs_inter_rat_cell_list_ptr,interRATCellID))
      {
        /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
        cell_position = (uint16)
          hcs_inter_rat_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
      }
      /* Tell to get the first available cell position and
      * that would be the cell id. The called function would store
      * the cell id back in passed cell_position variable */
      if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
        return FAILURE;
      }
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id
        = cell_position;
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
      /* Increment cell count */
      cell_count++;

      /* Increment the pointer */
      idx++;
      if ((hcs_inter_rat_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List full");
        break;
      }
    } /* End of while hcs_inter_rat_cell_list_ptr */

    /* Finally Set the cell count */
    l1_inter_rat_sib_ptr->inter_s.num_cell_add = cell_count;

    /* build composite cell info list from SIB 11 and SIB 12 */
    rrcirat_build_composite_cell_list_from_SIB11_SIB12(
      &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo.interRATCellInfoList.removedInterRATCellList,
      &rrcmeas_sib11->inter_s, &l1_inter_rat_sib_ptr->inter_s, num_removed_cells);
    for(cell_count = 0; cell_count < l1_inter_rat_sib_ptr->inter_s.num_cell_add; cell_count++)
    {
       if(sib18_present)
       {
         if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->n > cell_count)
         {
           if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].m.
            plmn_IdentityPresent)
           {
             rrc_translate_gsm_map_plmn_id(&tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].plmn_Identity,
               &tmp_plmn_identity);
             if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
             {
               if((rrccsp_check_for_eplmn(
                  mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                    tmp_plmn_identity))) == FALSE)
               {
                 WRRC_MSG1_HIGH("cell id %d Non eq PLMN measure only in DCH",
                   l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
                 /* set the flag to FALSE */
                 l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = FALSE;
               }
             }
           }
           else
           {
             /* use the previous value of the flag if PLMN id is not selected PLMN */
             if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
             {
               /* Add code to set the flag meas_pch_fach_idle to the flag value of the previous cell */
               if(cell_count > 0) /* to satisfy lint */
               {
                 l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach =
                   l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].measure_in_idle_pch_fach;
               }
             }
           }

           
         }
         else
         {
           /* PLMN list is shorter than the cell info list set Flag to true for the remaining cells */
           WRRC_MSG1_HIGH("PLMN list < Cell list. cell id %d measured in IDLE,FACH,PCH,DCH",
             l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
         }
       }
       else
       {
         WRRC_MSG1_HIGH("cell id %d measured in IDLE,FACH,PCH,DCH",
           l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
       }
    }
    /* Now set inter sys info incl info to TRUE */
    l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
    l1_inter_rat_sib_ptr->hcs_used = TRUE;
    return SUCCESS;
    /*  end HCS processing */
  }
  else
  {
    /* HCS is NOT in use */
    l1_inter_rat_sib_ptr->hcs_used = FALSE;
    if(!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.
        interRATMeasurementSysInfoPresent))
    {
      WRRC_MSG0_HIGH("Inter-RAT Sys info absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
      }
      return SUCCESS;
    }
    if (!(RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo_B,interRATCellInfoList)))
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
      }
      return SUCCESS;
    }

    /* Store ptr given by UTRAN */
    inter_rat_cell_list_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList;

    if (inter_rat_cell_list_ptr == NULL)
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB12 checking SIB11");
      if(rrcmeas_sib11->inter_sys_meas_info_incl == TRUE)
      {
        WRRC_MSG0_HIGH("Copying inter rat cell info from SIB 11");
        l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
        l1_inter_rat_sib_ptr->inter_s = rrcmeas_sib11->inter_s;
      }
      return SUCCESS;
    }

    /* Process the remove cell list of SIB12 */
    rrcirat_process_remove_cell_list(&sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo.interRATCellInfoList.removedInterRATCellList, &num_removed_cells);
    
    idx=0;
    while ( (inter_rat_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {
    /* Let first check the type of the cell to find if we
      * really need to give this cell to L1 */
      if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("Ignoring Non GSM cell");
        arfcn = MAX_ARFCN;
        if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,
            interRATCellID))
        {
          /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
          cell_position = (uint16)
            inter_rat_cell_list_ptr->elem[idx].interRATCellID;
        }
        else
        {
          /* Store the cell in first order position */
          cell_position = AVAILABLE_CELL_POSITION;
        }
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
        if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
        {
          WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
          return FAILURE;
        }
        idx++;
        continue;
      }
      else
      {
      /*  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.delta_qrxlevmin_included = FALSE; */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

        /* Cell reselection info is always set to TRUE. If not present in SIB 11, just
        set it to default values */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info_incl = TRUE;

        /* Initializw q-offset1-2 Defaults as per specs */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.qoffset1_s_n = QOFFSET1;

        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.qoffset2_s_n = QOFFSET2;

        /* -24. Got this value from Radio Access Capability */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.cell_sel_info.max_tx_pwr = -24;

          /* Not valid for Inter-RAT but initialize to default
        * as they are present */
        if ((sib4_present == TRUE) && (sib4_ptr != NULL))
        {
          if(RRC_CHECK_COMMON_MSG_TYPE(sib4_ptr->cellSelectReselectInfo.modeSpecificInfo,
             rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qual_min = (int16)
              sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;

            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.rxlev_min = (int16)
              (sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            /* Check for REL 5 extension ptr */
            if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
              (sib4_ptr->m.v4b0NonCriticalExtensionsPresent) &&
              (sib4_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) &&
              (sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType4_v590ext.m.
              cellSelectReselectInfo_v590extPresent) && (sib4_ptr->v4b0NonCriticalExtensions.
              v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.m.
              deltaQrxlevminPresent))
            {
  
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin = sib4_ptr->v4b0NonCriticalExtensions.
                v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2; */
              MSG_HIGH("SIB4 Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (sib4_ptr->v4b0NonCriticalExtensions.
                 v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2),
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));
  
              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                   (sib4_ptr->v4b0NonCriticalExtensions.
                    v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.rxlev_min += (int16) (sib4_ptr->v4b0NonCriticalExtensions.
                  v590NonCriticalExtension.sysInfoType4_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2);
              /*}*/
            }
          }
          else
          {
            WRRC_MSG1_ERROR("ERR: SIB4 mode_choice NOT FDD %d", sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
          }
        }
        else
        {
          if(RRC_CHECK_COMMON_MSG_TYPE(sib3_ptr->cellSelectReselectInfo.modeSpecificInfo,
             rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qual_min = (int16)
              sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.rxlev_min = (int16)
              (sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
            /* Check for REL 5 extension ptr */
            if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
              (sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType3_v590ext.m.
              cellSelectReselectInfo_v590extPresent) && (sib3_ptr->v4b0NonCriticalExtensions.
              v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.m.
              deltaQrxlevminPresent))
            {
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.cell_sel_info.delta_qrxlevmin = sib3_ptr->v4b0NonCriticalExtensions.
                v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2;*/
              MSG_HIGH("SIB3 Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (sib3_ptr->v4b0NonCriticalExtensions.
                 v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2),
                 l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                 CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));
  
              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                 (sib3_ptr->v4b0NonCriticalExtensions.
                  v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.cell_sel_info.rxlev_min += (int16) (sib3_ptr->v4b0NonCriticalExtensions.
                  v590NonCriticalExtension.sysInfoType3_v590ext.cellSelectReselectInfo_v590ext.deltaQrxlevmin * 2);
              /*}*/
            }
          }
          else
          {
            WRRC_MSG1_ERROR("ERR: SIB3 mode_choice NOT FDD %d", sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
          }
        }

        /* Get GSM CELL Info */
        /* Check if rrc_gsm_cellSelectionReselectionInfo_present */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->m.cellSelectionReselectionInfoPresent)
        {
          MSG_LOW("Cell Reselection info present",0,0,0);

          /* q-Offset1S-N */

            /* Range is -50...50 */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qoffset1_s_n = (int16)
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.q_Offset1S_N;
          

          /* q_Offset2S_N  -50...50 */
          if (RRC_MSG_COMMON_BITMASK_IE(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo,
            rrc_CellSelectReselectInfoSIB_11_12,q_Offset2S_N))
          {
            /* Range is -50...50 */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.qoffset2_s_n =  (int16)
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.q_Offset2S_N;
          }

          /* Max Txit Power */
          if (RRC_MSG_COMMON_BITMASK_IE(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo,
            rrc_CellSelectReselectInfoSIB_11_12,maxAllowedUL_TX_Power))
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.cell_sel_info.max_tx_pwr = (int16)
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.maxAllowedUL_TX_Power;
          }

          /* Check if gsm mode present  */
          if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.
               modeSpecificInfo.t == T_rrc_CellSelectReselectInfoSIB_11_12_modeSpecificInfo_gsm)
          {
            /* Check if rxlev_min present */

            if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.modeSpecificInfo.u
              .gsm->m.q_RxlevMinPresent)
            {
              /* Store rxlev_min in L1 */
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min = (int16)
                ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.modeSpecificInfo.u.gsm->q_RxlevMin * 2) + 1);
            }
          }
          else
          {
            WRRC_MSG0_ERROR("Incorrect mode specific info for GSM entry in SIB12");
          }

          /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
            delta_qrxlevmin_included = FALSE;  */
          if(rel5_ext_ptr != NULL)
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2( rel5_ext_ptr,deltaQrxlevmin))
            {
              /*l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                delta_qrxlevmin_included = TRUE;
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
                delta_qrxlevmin = rel5_ext_ptr->value.deltaQrxlevmin * 2;        */
              MSG_HIGH("Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
                (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2),
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
                CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));

              /*if ((l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                 (rel5_ext_ptr->value.deltaQrxlevmin * 2)) < -115)
              {*/
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +=
                  (int16) (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2);
              /*}*/
            }
          }
        } /* rrc_gsm_cellSelectionReselectionInfo_present */

        /* Cell Individual Offset */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
          inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->interRATCellIndividualOffset;

        /* Get BAND 1800 DCS or 1900 PCS */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
          RET_INTER_RAT_BAND(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->frequency_band);

        /* Get bcch_ARFCN */
        CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) =
          (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN;

        arfcn = (uint16)inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bcch_ARFCN;

#ifdef FEATURE_GSM_QUADBAND

        if ((converted_gsm_band = rrc_get_gsm_band(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info)) !=
          INVALID_BAND)
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
            converted_gsm_band;
        }
        else
        {
          WRRC_MSG1_HIGH("ARFCN %d ignored bec of invalid band",inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
            u.gsm->bcch_ARFCN);
          arfcn = MAX_ARFCN;
          if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
          {
            /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
            cell_position = (uint16)
              inter_rat_cell_list_ptr->elem[idx].interRATCellID;
          }
          else
          {
            /* Store the cell in first order position */
            cell_position = AVAILABLE_CELL_POSITION;
          }          /* Tell to get the first available cell position and
          * that would be the cell id. The called function would store
          * the cell id back in passed cell_position variable */
          if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
          {
            WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
            return FAILURE;
          }
          /* Increment the pointer */
          idx++;
          continue;
        }
#endif /* FEATURE_GSM_QUADBAND */

        /* To add BSIC (Base Station Identification Code) */
        if ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
          > NCC_MAX) ||
          (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
          > BCC_MAX))
        {
          WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

          return FAILURE;

        } /* End of if of checking NCC and BCC validity */

        /* Store good values of bcc and ncc */
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc =
          (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.ncc;

        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc =
          (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.bcc;
      } /* End of gsm cell  */

      /* Time to get its cell id*/
      /* Cell Order has already been verified */

      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
      {
        /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
        cell_position = (uint16)
          inter_rat_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
      }
      /* Tell to get the first available cell position and
      * that would be the cell id. The called function would store
      * the cell id back in passed cell_position variable */
      if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
        return FAILURE;
      }
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id
        = cell_position;
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].
        measure_in_idle_pch_fach = TRUE;

      /* Increment cell count */
      cell_count++;

      /* Increment the pointer */
      idx++;
      if ((inter_rat_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List full");
        break;
      }
    } /* End of while inter_rat_cell_list_ptr */

    /* Finally Set the cell count */
    l1_inter_rat_sib_ptr->inter_s.num_cell_add = cell_count;

    /* build composite cell info list from SIB 11 and SIB 12 */
    rrcirat_build_composite_cell_list_from_SIB11_SIB12(
      &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo.interRATCellInfoList.removedInterRATCellList,
      &rrcmeas_sib11->inter_s, &l1_inter_rat_sib_ptr->inter_s, num_removed_cells);

    for(cell_count = 0; cell_count < l1_inter_rat_sib_ptr->inter_s.num_cell_add; cell_count++)
    {
      if(sib18_present)
      {
        if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->n > cell_count)
        {
          if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].m.plmn_IdentityPresent)
          {
            rrc_translate_gsm_map_plmn_id(&tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].plmn_Identity,
              &tmp_plmn_identity);
            if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
            {
              if((rrccsp_check_for_eplmn(
                mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                tmp_plmn_identity))) == FALSE)
              {
                WRRC_MSG1_HIGH("cell id %d Non eq PLMN measure only in DCH",
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
                /* set the flag to FALSE */
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = FALSE;
              }
            }
          }
          else
          {
            /* use the previous value of the flag if PLMN id is not selected PLMN */
            if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
            {
              /* Add code to set the flag meas_pch_fach_idle to the flag value of the previous cell */
              if(cell_count > 0) /* to satisfy lint */
              {
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach =
                  l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].measure_in_idle_pch_fach;
              }
            }
          }

        }
        else
        {
          /* PLMN list is shorter than the cell info list set Flag to true for the remaining cells */
          WRRC_MSG1_HIGH("PLMN list < Cell list. cell id %d measured in IDLE,FACH,PCH,DCH",
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
        }
      }
      else
      {
        WRRC_MSG1_HIGH("cell id %d measured in IDLE,FACH,PCH,DCH",
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
      }
    }

    /* Now set inter sys info incl info to TRUE */
    l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;




     WRRC_MSG0_HIGH("IRAT CELL INFO = FALSE");
     interRAT_cell_info_status.interRAT_cell_info_present = FALSE;
     interRAT_cell_info_status.interRAT_cell_info_ind = 0;

    if(RRC_CHECK_COMMON_MSG_TYPE(sib12_ptr->measurementControlSysInfo.use_of_HCS,
     rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used ))
    {
      if((sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.
        interRATMeasurementSysInfoPresent))
      {
        if ((RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
           interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo,interRATCellInfoList)) ||
           (rrcmeas_sib11->inter_sys_meas_info_incl == TRUE))
        {
          WRRC_MSG0_HIGH("IRAT CELL INFO = TRUE");
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = 0;
          return SUCCESS;
        }
      }
    }
    else
    {
      if((sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.
        interRATMeasurementSysInfoPresent))
      {
        if ((RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
          interRATMeasurementSysInfo,rrc_InterRATMeasurementSysInfo_B,interRATCellInfoList)) ||
          (rrcmeas_sib11->inter_sys_meas_info_incl == TRUE))
        {
          WRRC_MSG0_HIGH("IRAT CELL INFO = TRUE");
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = 0;
          return SUCCESS;
        }
      }
    }




    return SUCCESS;

  } /* End of else of HCS not in use */
}

/*===========================================================================

  FUNCTION   RRCIRAT_RET_PERIODIC_RPT_INTVL

DESCRIPTION

     This function converts the reporting interval specified by network into the
     format required by L1


DEPENDENCIES

  None

RETURN VALUE

  Rpting Interval required by L1

SIDE EFFECTS

  None

===========================================================================*/

static l1_reporting_interval_enum_type rrcirat_ret_periodic_rpt_intvl
(
  rrc_ReportingIntervalLong periodic_intvl
)
{
  l1_reporting_interval_enum_type l1_rpt_intvl = L1_RPT_INTERVAL_250MS;
  switch(periodic_intvl)
  {
  /* Since L1 did not define 0 ms, just to satisfy LINT use L1_RPT_INTERVAL_250MS.
  L1 interface change required.  rrc_ril0 scenario is taken care of before this fn is called
  by specifying that no reporting is required. Also for Reporting Interval 0, spec says
    UE behavior is unspecified. */
  case rrc_ReportingIntervalLong_ril0:
  case rrc_ReportingIntervalLong_ril0_25:
    l1_rpt_intvl = L1_RPT_INTERVAL_250MS;
    break;

  case rrc_ReportingIntervalLong_ril0_5:
    l1_rpt_intvl = L1_RPT_INTERVAL_500MS;
    break;

  case rrc_ReportingIntervalLong_ril1:
    l1_rpt_intvl = L1_RPT_INTERVAL_1000MS;
    break;

  case rrc_ReportingIntervalLong_ril2:
    l1_rpt_intvl = L1_RPT_INTERVAL_2000MS;
    break;

  case rrc_ReportingIntervalLong_ril3:
    l1_rpt_intvl = L1_RPT_INTERVAL_3000MS;
    break;

  case rrc_ReportingIntervalLong_ril4:
    l1_rpt_intvl = L1_RPT_INTERVAL_4000MS;
    break;

  case rrc_ReportingIntervalLong_ril6:
    l1_rpt_intvl = L1_RPT_INTERVAL_6000MS;
    break;

  case rrc_ReportingIntervalLong_ril8:
    l1_rpt_intvl = L1_RPT_INTERVAL_8000MS;
    break;

  case rrc_ReportingIntervalLong_ril12:
    l1_rpt_intvl = L1_RPT_INTERVAL_12000MS;
    break;

  case rrc_ReportingIntervalLong_ril16:
    l1_rpt_intvl = L1_RPT_INTERVAL_16000MS;
    break;

  case rrc_ReportingIntervalLong_ril20:
    l1_rpt_intvl = L1_RPT_INTERVAL_20000MS;
    break;

  case rrc_ReportingIntervalLong_ril24:
    l1_rpt_intvl = L1_RPT_INTERVAL_24000MS;
    break;

  case rrc_ReportingIntervalLong_ril28:
    l1_rpt_intvl = L1_RPT_INTERVAL_28000MS;
    break;

  case rrc_ReportingIntervalLong_ril32:
    l1_rpt_intvl = L1_RPT_INTERVAL_32000MS;
    break;

  case rrc_ReportingIntervalLong_ril64:
    l1_rpt_intvl = L1_RPT_INTERVAL_64000MS;
    break;
  }
  return l1_rpt_intvl;
}

/*===========================================================================

FUNCTION   RRCIRAT_CHECK_INTER_RAT_CELL_ORDER_MCM

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell list is in order
  SUCCESS: If cell list is not in order

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_inter_rat_cell_order
(
  rrc_InterRATCellInfoList_B* rrc_cell_info_list_ptr
)
{
  /* Find the cell order */

  uint32 cell_id_list[L1_MAX_CELL_MEAS];
  uint16 count=0, list_count1 =0, list_count2 = 0;

  rrc_NewInterRATCellList_B   *local_inter_rat_cell_list_ptr = NULL;

  local_inter_rat_cell_list_ptr = &rrc_cell_info_list_ptr->newInterRATCellList;

  for (count = 0; count <L1_MAX_CELL_MEAS; count++ )
    cell_id_list[count] = 0;

  count = 0;

  /* Store cell id's in a local list and calculate total no of cells */
  while ((local_inter_rat_cell_list_ptr->n > list_count1) && (count < L1_MAX_CELL_MEAS))
  {
    if (local_inter_rat_cell_list_ptr->elem[list_count1].m.interRATCellIDPresent)

    {
      /* Store cell id */
      cell_id_list[count] = local_inter_rat_cell_list_ptr->elem[list_count1].interRATCellID;
      count++;
    }
    /* Cell id was never present which is very likely */
    list_count1++;
  }

  if (count == 0 )
  {
    WRRC_MSG0_HIGH("Inter-RAT Cell list: No cell Id's");
    /* Return successfully as cells would now be stored
    in first available position */
    return SUCCESS;
  }
  if (count == 1)
  {
    WRRC_MSG0_HIGH("Inter-RAT Cell list: 1 cell with id");
    /* So no comparision has to be done. */
    return SUCCESS;
  }
  /* This means that there are multiple cells in the cell list and there
  order needs to be verified */

  /* Compare 1 st with 2,3,4..... Then 2nd with 3,4,5... and so on */
  for (list_count1 = 0; list_count1 < (count-1); list_count1++)
  {
    for (list_count2 = list_count1+1; list_count2 < count; list_count2++)
    {
      if (cell_id_list[list_count1] < cell_id_list[list_count2])
      {
        /* Good. It is in order */
        continue;
      }
      WRRC_MSG2_ERROR("Inter-RAT Cell List elements %d & %d not in order",
        cell_id_list[list_count1],cell_id_list[list_count2]);
      return FAILURE;   /* So ignore this list in the calling fn */
    } /* End of for for list_count2 */
  }  /* End of for for list_count1 */
#ifdef DEBUG_MSG
  WRRC_MSG1_HIGH("Inter-RAT Cell list has %d cells",count);
#endif
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIRAT_UPDATE_INTER_RAT_CELL_LIST

DESCRIPTION

  This function updates the inter rat cell list received from SIB11

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If cell list is not in order
  SUCCESS: If cell list is in order.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_update_inter_rat_cell_list
(
 uint16* position_no_ptr,
 uint16 arfcn
)
{
  uint8 count = 0;

  /* First check if one has to add this in the first available position */
  if (*position_no_ptr == AVAILABLE_CELL_POSITION)
  {
    for (count = 0; count < L1_MAX_CELL_MEAS; count++)
    {
      if (inter_rat_cell_list[count].cell_position == VACANT)
      {
        /* Store this cell in this first available vacant position */
        inter_rat_cell_list[count].cell_position = OCCUPIED;
        inter_rat_cell_list[count].cell_id = count;
        inter_rat_cell_list[count].arfcn = arfcn;
        /* Set the position_no_ptr to count */
        *position_no_ptr = count;
        return SUCCESS;
      }
    } /* End of for */
  } /* End of if of  position_no_ptr == AVAILABLE_CELL_POSITION */


  /* Now check which position the cells needs to be updated */
  for (count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    if (count == *position_no_ptr)
    {
    /* We have reached the position in the array which is given by id
     * Check if there is already a cell in this position
     * If yes, then overwrite on this cell */


      /* Store the new cell id given by *position_no_ptr at this position */
      inter_rat_cell_list[count].cell_id = *position_no_ptr;

      /* Mark the position as occupied */
      inter_rat_cell_list[count].cell_position = OCCUPIED;

      inter_rat_cell_list[count].arfcn = arfcn;

      return SUCCESS;
    }
  }
  WRRC_MSG1_ERROR("InterRAT cell ID = %d not found", *position_no_ptr);
  return FAILURE;

}

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_REPORT_CRITERIA

DESCRIPTION

  This function processes the report criteria in the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_report_criteria
(
  rrc_InterRATReportCriteria* reportCriteria_ptr,
  l1_inter_sys_meas_ctrl_struct_type* inter_s_ptr,
  rpt_crit_enum_type* mcm_rpt_crit_ptr,
  uint8 transaction_id
)
{
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Get Reporting Criteria */
  if (reportCriteria_ptr->t == T_rrc_InterRATReportCriteria_interRATReportingCriteria)
  {
    /* Set criteria to event triggered */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(reportCriteria_ptr->u.interRATReportingCriteria,interRATEventList))
      || (reportCriteria_ptr->u.interRATReportingCriteria->interRATEventList.n == 0))
    {
      WRRC_MSG0_HIGH("Event Criteria empty in modify set L1_NO_CHANGE");
      inter_s_ptr->rpt_criteria.rpt_mode = L1_NO_CHANGE;
      *mcm_rpt_crit_ptr = NOT_AVAILABLE;
      return SUCCESS;
    }      
    inter_s_ptr->rpt_criteria.rpt_mode = L1_EVENT_TRIGGER_RPT;

    *mcm_rpt_crit_ptr = EVENT;

    /* Event Triggered Reporting Chosen */
    if (rrcirat_process_event_criteria(reportCriteria_ptr->u.interRATReportingCriteria,
      &inter_s_ptr->rpt_criteria.u.evt_rpt_crit) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Event Criteria specified");

      /* Send Meas Ctrl Failure Message */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      return FAILURE;
    }
  }
  else if (reportCriteria_ptr->t == T_rrc_InterRATReportCriteria_periodicalReportingCriteria)
  {
    /* Set periodic reporting */
    inter_s_ptr->rpt_criteria.rpt_mode = L1_PERIODIC_RPT;

    *mcm_rpt_crit_ptr = PERIODIC;
    /* Periodic Reporting Chosen */
    /* Set reporting cells */
    /* Check if reporting amount present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(reportCriteria_ptr->u.
      periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {

      if ((rrcmeas_fill_no_of_reporting_cells(
        &reportCriteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &inter_s_ptr->rpt_cell_status) == SUCCESS) &&
        (reportCriteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus.t ==
        T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells))
      {
        inter_s_ptr->rpt_cell_status_incl = TRUE;
      }
      else
      {
        WRRC_MSG1_ERROR("Invalid Rpt Cell Status %d", reportCriteria_ptr->u.periodicalReportingCriteria->
          reportingCellStatus.t);
        /* Send Meas Ctrl Failure Message */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        return FAILURE;
      }

    }
    /* Assign Default Value i.e. Infinity */
    inter_s_ptr->rpt_criteria.u.periodic_crit.rpt_amount
      = L1_RPT_AMOUNT_INFINITY;

   
      /* Store reporting amount */
      inter_s_ptr->rpt_criteria.u.periodic_crit.rpt_amount =
        RET_INTER_RAT_REPORTING_AMOUNT(
        reportCriteria_ptr->u.periodicalReportingCriteria->
        periodicalReportingCriteria.reportingAmount);
    

    if (reportCriteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
    /* Since L1 does not have 0ms defined in interface, change rpt mode to no rpt
      so that L1 does not get confused. Also spec in this scenario says UE behaviour unspec */
      WRRC_MSG0_HIGH("Rpt Intvl 0, No PeriodicRpting");
      inter_s_ptr->rpt_criteria.rpt_mode = L1_NO_RPT;
    }
    else
    {
      /* Store reporting interval */
      inter_s_ptr->rpt_criteria.u.periodic_crit.rpt_interval =
        rrcirat_ret_periodic_rpt_intvl(
        reportCriteria_ptr->u.periodicalReportingCriteria->
        periodicalReportingCriteria.reportingInterval);
    }

  } /* End of else of periodic reporting chosen */
  else
  {
    /* Set No reporting */
    inter_s_ptr->rpt_criteria.rpt_mode = L1_NO_RPT;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(reportCriteria_ptr->u.noReporting,
      rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if ((rrcmeas_fill_no_of_reporting_cells(
        &reportCriteria_ptr->u.noReporting->reportingCellStatus,
        &inter_s_ptr->rpt_cell_status) == SUCCESS) &&
        (reportCriteria_ptr->u.noReporting->reportingCellStatus.t ==
        T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells))
      {
        inter_s_ptr->rpt_cell_status_incl = TRUE;
      }
      else
      {
        WRRC_MSG1_ERROR("Invalid Rpt Cell Status %d", reportCriteria_ptr->u.periodicalReportingCriteria->
          reportingCellStatus.t);

        /* Send Meas Ctrl Failure Message */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        return FAILURE;

      }

    }

    *mcm_rpt_crit_ptr = ADDTL;
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
,uint8 meas_ctrl_tr_id
,   rrc_DL_DCCH_Message* dcch_msg_ptr
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrcmeas_status_e_type result;
  uint8 transaction_id = INVALID_TRANSACTION_ID;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  rrcmeas_interrat_type interrat_type = RRCMEAS_IRAT_GSM;
#endif


  boolean cell_info_ind_present = FALSE;
  uint8 cell_info_ind = 0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  rrc_InterRATMeasurement *irat_meas_cmd_ptr = NULL;

  if(!(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_release) )
  {

    if((dcch_msg_ptr->message.u.measurementControl.u.r3.m.v390nonCriticalExtensionsPresent) &&
        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.m.
        v3a0NonCriticalExtensionsPresent) &&
        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.m.
         laterNonCriticalExtensionsPresent) &&
        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.
        v4b0NonCriticalExtensionsPresent) &&
        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.
        v590NonCriticalExtensionsPresent) &&
        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
        v5b0NonCriticalExtensionsPresent) &&

        (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.
        v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementControl_v5b0ext.m.interRATCellInfoIndicationPresent))
    {
      WRRC_MSG0_HIGH("irat cell info ind present");
      cell_info_ind_present=TRUE;
      cell_info_ind = dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementControl_v5b0ext.interRATCellInfoIndication;
    }
    else
    {
      WRRC_MSG0_HIGH("irat cell info ind not present");
      cell_info_ind_present=FALSE;
    }
  }

  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  if(rrcirat_is_meas_id_eutra_meas((uint16) rrc_mcm_ptr->measurementIdentity))
  {
    WRRC_MSG0_HIGH("MCM-R3 type EUTRA");
    interrat_type = RRCMEAS_IRAT_EUTRA;
  }    
  if((rrc_wtol_cm_support_nv == FALSE) && (interrat_type == RRCMEAS_IRAT_EUTRA))
  {
     WRRC_MSG0_HIGH("wtol cm support nv is not enabled");
    /* Send Meas Ctrl Failure Message */

    rrcmeas_mcf.rrc_transaction_id = transaction_id;

    /* Set error choice */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    rrc_free(l1_inter_rat_meas_params);
    return FAILURE;
  }
  if(((RRCMEAS_IRAT_EUTRA == interrat_type) && (rrcmcm_is_lte_mode_enabled() == FALSE)))
  {
    WRRC_MSG1_HIGH("REL8:IRAT:Mode not enabled, IRAT_Type %d",interrat_type);
    /* Send Meas Ctrl Failure Message */
  
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
    /* Set error choice */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
  
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    rrc_free(l1_inter_rat_meas_params);
    return FAILURE;
  }     
  if(RRCMEAS_IRAT_EUTRA == interrat_type)
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS_EUTRA;
    memset(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra,0,sizeof(l1_inter_sys_eutra_meas_ctrl_struct_type));
  }
  else
#endif    
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;

    /* The following is not supported in Rel 99 anymore. So set it to FALSE */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;
  }

  transaction_id = meas_ctrl_tr_id;


  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = (uint16) transaction_id;
  l1_inter_rat_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_setup)
  {
    /* Double check that it is INTER-RAT Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_interRATMeasurement)
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */

    if (!((rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->m.
      interRATMeasQuantityPresent) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->m.
      interRATReportingQuantityPresent) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.t == T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
      rrc_MeasurementQuantityGSM_dummy) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo.t ==
      T_rrc_InterRATReportingQuantity_ratSpecificInfo_gsm) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.t
      == T_rrc_InterRATReportCriteria_interRATReportingCriteria) &&
      (!(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->m.interRATEventListPresent) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }       

    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Set Meas Identity */
    l1_inter_rat_meas_params->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;

    /* Set Meas Quantity as Inter-sys Meas Quan present */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

    /* Initialize Default Value to intra_freq_meas_quan_incl */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
        modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Get Filter Coefficient too */
    
        /* Store filter coefficient */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
          interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
      
    } /* End of quality estimate present */

    /* Set System Choice to GSM */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

    /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

    /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
    So set it to RSSI */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
      L1_GSM_CARRIER_RSSI;

 
      /* GSM Filter Coefficient present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);
    

    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
      RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

    /* Store Reporting Quantity */
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

    /* Sys Choice */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

    /* L1 wants this to be FALSE for the first version as it does not support it */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

    /* Ignore reading Observed time difference as no longer supported in Rel 99
    l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
    rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
    ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

    /* Check if gsm_carrier_rssi wanted in Meas Report */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

    if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
    {
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    /* For L1 sake, copy reporting criteria */
    l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

    /* Now set other values */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;

    l1_inter_rat_meas_params->meas_object_info_included = TRUE;


    /* Time to process the cell list */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement,interRATCellInfoList))
    {
      result = rrcirat_process_cell_list_mcm(&rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATCellInfoList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
      if(result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else if(result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("REL5:Unable to fill InterRAT MCM");
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      /* Set Cell list is present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)

      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;

        }
      }
    } /* End of if of Additional Meas Present */


      /* At this point everything has been validated and now is the time
    to update the inter rat CELL_INFO_LIST */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement,interRATCellInfoList))
    {
      for (count=0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_rat_cell_list[count],
                     sizeof(inter_rat_cell_list_struct_type),
                     &local_neighbor_list[count],
                     sizeof(inter_rat_cell_list_struct_type));
      }
    }

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_modify)
  {
    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

      l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      {
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)

        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }
    }
    else
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement ;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            ((irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) ||
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
        interRATMeasurement,rrc_InterRATMeasurement,interRATMeasQuantity)))
      {
        WRRC_MSG0_HIGH("Meas Quan absent");
      }
      else
      {
        /* Set Meas Quantity to TRUE */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
        if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
          rrc_MeasurementQuantityGSM_dummy)
        {

          WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");

          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }

        /* Meas Quantity Parameter 1 i.e. Quality Estimate */
        if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity,rrc_InterRATMeasQuantity, measQuantityUTRAN_QualityEstimate))
        {
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

          /* Meas Quan i.e ECI0/RSCP/RSSI... */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
            u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
            modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

          /* Get Filter Coefficient too */
         
            /* Store filter coefficient */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
              .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              measQuantityUTRAN_QualityEstimate.filterCoefficient);
          
        }

        /* Set System Choice to GSM */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

        /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

        /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
        So set it to RSSI */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
          L1_GSM_CARRIER_RSSI;


          /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
            RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
            ratSpecificInfo.u.gsm->filterCoefficient);
        

        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
          RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      } /* End of else of Meas Quan present */

      /* Check if reporting quan present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
           rrc_InterRATMeasurement,interRATReportingQuantity))
      {
        /* Store Reporting Quantity */
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

        /* Sys Choice */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

        /* L1 wants this to be FALSE for the first version as it does not support it */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

        /* Observed time difference no longer supported in Rel 99
        l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

          /* Check if gsm_carrier_rssi wanted in Meas Report */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
      }

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.interRATMeasurement->reportCriteria,
        &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
      
      l1_inter_rat_meas_params->meas_object_info_included = TRUE;
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,rrc_MeasurementCommand_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
          rrc_InterRATMeasurement,interRATCellInfoList)) )
      {
        /* Time to process the cell list */
        result = rrcirat_process_cell_list_mcm(&rrc_mcm_ptr->measurementCommand.
          u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
        if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
         else if(result == RRCMEAS_UNSUPPORTED)
        {
          /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("REL5:Unable to fill InterRAT MCM");
          /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }

      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      {
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement,interRATCellInfoList) ))
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;

#ifdef FEATURE_GAN_3G
#error code not present
#endif

        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */



  if(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_setup)
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
       rrc_InterRATMeasurement,interRATCellInfoList)))
    {
      if(cell_info_ind_present==TRUE)
      {
        interRAT_cell_info_status.interRAT_cell_info_present=TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind=cell_info_ind;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_modify)
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
       rrc_InterRATMeasurement,interRATCellInfoList)))
    {
      if(cell_info_ind_present==TRUE)
      {
        interRAT_cell_info_status.interRAT_cell_info_present=TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind=cell_info_ind;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }

  }


  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;
  WRRC_MSG0_MED("MCM:Sending cmd to L1");
  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_CELL_LIST_MCM

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm
(
  rrc_InterRATCellInfoList* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
)
{
/* RRC new cell info list and l1 new cell info list would have the new cell info list
through local neighbour list. This has to be done to make sure that if the Meas Ctrl
  Msg contains bad cells, our good inter_rat_cell_list does not get disturbed */

  uint8 cnt = 0;

  uint8 cell_count = 0;

  uint32 cell_position = 0;

  uint16 cell_id = INVALID_CELL_ID;

  /* For storing the new cell list */
  rrc_NewInterRATCellList* local_rrc_new_cell_list_ptr = NULL;

  rrc_CellsForInterRATMeasList* local_cells_for_meas_ptr = NULL;
  boolean valid_cell_id_found =FALSE;
  uint32 idx=0;
#ifdef FEATURE_GAN_3G
  #error code not present
#endif

  /* Initializes the local neighbor list to default values */
  rrcirat_initialize_local_neighbor_list();
  /* Copy Inter-RAT CELL_INFO_LIST list to local neighbor list. This is necessary
  so as to make sure that in case UTRAN messes up, our old cell list is intact */

  for (cnt=0; cnt < L1_MAX_CELL_MEAS; cnt++)
  {
    WCDMA_MEMCPY(&local_neighbor_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type),
                 &inter_rat_cell_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type));
  }

  /* In Inter-RAT meas, remove is not optional */
  /* For sanity, initialize to 0 */
  l1_cell_list_ptr->num_cell_rmv = 0;

  if (rrcirat_fill_removed_inter_rat_cells(&mcm_cell_list_ptr->removedInterRATCellList,
    l1_cell_list_ptr) == FAILURE)
  {
    WRRC_MSG0_HIGH("Invalid Removed Inter-RAT cells");
    return RRCMEAS_FAIL;
  }

  /* UTRAN can ask us to remove all cells when it is possible that for that
  * measurement (First time SETUP) we may not have any cell list.
  * To avoid L1 getting confused, let us make number of cells to be
  * removed to 0 in case the cell list does not exist previously.
  * So check in the CELL_INFO_LIST */

  if ((mcm_cell_list_ptr->removedInterRATCellList.t ==
    T_rrc_RemovedInterRATCellList_removeAllInterRATCells) &&
    (rrcirat_check_cell_absence_in_cell_list() == SUCCESS))
  {
    /* Set No of Cells removed to 0 from oxff */
    WRRC_MSG0_HIGH("Setting No of cells removed to 0");
    l1_cell_list_ptr->num_cell_rmv = 0;
  }


  /* At this time, local_neighbor_list is completely updated and has
  * the latest and the greatest information
  * Get New cells to be added. UTRAN can just set it to NULL */
  if (mcm_cell_list_ptr->newInterRATCellList.n == 0)
  {
    WRRC_MSG0_HIGH("MCM:No new cells to be added");
    /* Put this info in L1 list */
    l1_cell_list_ptr->num_cell_add = 0;
  }
  else
  {
    if (rrcirat_check_inter_rat_cell_order_mcm(&mcm_cell_list_ptr->newInterRATCellList) == FAILURE)
    {
      WRRC_MSG0_HIGH("Sending Meas Ctrl Failure");
      return RRCMEAS_FAIL;
    }
    /* New cells need to be added */
    local_rrc_new_cell_list_ptr =  &mcm_cell_list_ptr->newInterRATCellList;

    /* Before verifying the cell id, make sure that cell params are correct */

    while ((local_rrc_new_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
    {
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
    /* Let first check the type of the cell to find if we
      really need to give this cell to L1 */
      if (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("MCM:Ignoring Non GSM cell");
        idx++;
        continue;
      }
      else
      {
        l1_cell_list_ptr->add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

        /* Get GSM CELL Info */

        /* No need to read cell selec/reselect info as that is for SIB11/12 */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.cell_sel_info_incl = FALSE;

        /* Cell Individual Offset */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->interRATCellIndividualOffset;

        /* Get BAND 1800 DCS or 1900 PCS */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
          RET_INTER_RAT_BAND(local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->frequency_band);

        /* Get bcch_ARFCN */
        CONVERT_ARFCN_TO_NUM(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN;

#ifdef FEATURE_GAN_3G
        #error code not present
#endif

#ifdef FEATURE_GSM_QUADBAND
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        {
          if ((converted_gsm_band = rrc_get_gsm_band(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info)) !=
            INVALID_BAND)
          {
            l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
              converted_gsm_band;
          }
          else
          {
            WRRC_MSG1_HIGH("ARFCN %d rejected bec of invalid band",local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
              u.gsm->bcch_ARFCN);
            idx++;
            return RRCMEAS_FAIL;
          }
        }

#endif /* FEATURE_GSM_QUADBAND */

        /* To add BSIC (Base Station Identification Code) */
        if ((local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
          > NCC_MAX) ||
          (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
          > BCC_MAX))
        {
          WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

          return RRCMEAS_FAIL;

        } /* End of if of checking NCC and BCC validity */

        /* Store good values of bcc and ncc */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.ncc;

        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.bcc;
      } /* End of gsm cell  */

      /* Time to get its cell id*/
      /* Cell Order has already been verified */

      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(local_rrc_new_cell_list_ptr,interRATCellID))
      {
        WRRC_MSG0_MED("MCM:Inter-RAT Cell Id present");
        cell_position = local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
        l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
      }


      /* Now store the cell id in local neighbour rat cell list */
      if (rrcirat_update_inter_rat_cell_list_mcm(&cell_position,
        &l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id, &cell_id)
        == SUCCESS)
      {
        /* set this flag to TRUE always for MCM */
        l1_cell_list_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }
      else
      {
        WRRC_MSG0_ERROR("MCM:Invalid Inter-RAT cell list");
        return RRCMEAS_FAIL;
      }

      /* Increment the pointer */
      /* Read Next One */
      idx++;


      if ((local_rrc_new_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List full");
        break;
      }

    } /* End of while inter_rat_cell_list_ptr */

    /* Finally Set the cell count */
    l1_cell_list_ptr->num_cell_add = cell_count;

  } /* End of else of new cells added */

  cell_count = 0;

  /* Check for cells for meas */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,
    rrc_InterRATCellInfoList,cellsForInterRATMeasList))
  {
    WRRC_MSG0_HIGH("Cells for meas present");
    l1_cell_list_ptr->cells_for_meas_incl = TRUE;

    local_cells_for_meas_ptr = &mcm_cell_list_ptr->cellsForInterRATMeasList;
    idx=0;
    while (local_cells_for_meas_ptr->n > idx)
    {
      cell_id = (uint16) local_cells_for_meas_ptr->elem[idx];

      /* Check if the cell id is present */
      if( rrcirat_find_cell_id(&cell_id) == FAILURE)
      {
        WRRC_MSG1_ERROR("cell to measure with id = %d not found",
          cell_id);

      } /* End of if for cell id not found */
      else
      {
      /* Store cell id */
        valid_cell_id_found=TRUE;
        l1_cell_list_ptr->meas_cells_list[cell_count++] = cell_id;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
       WRRC_MSG0_HIGH("None of the cell is valid");
       return RRCMEAS_UNSUPPORTED;
    }
    l1_cell_list_ptr->num_cell_meas =  cell_count;

  } /* End of if of rrc_cellsForIntraFreqMeasList_present */

  return RRCMEAS_SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_INIT_SIB11_PARAMS

DESCRIPTION

  This function initializes SIB11 params for Inter-RAT Meas

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If measured results are valid
  SUCCESS: If measured results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_init_sib11_params
(
  l1_meas_sib_parm_struct_type* l1_sib11_inter_rat_ptr
)
{
  l1_sib11_inter_rat_ptr->fach_meas_incl = FALSE;
  l1_sib11_inter_rat_ptr->hcs_used = FALSE;
  l1_sib11_inter_rat_ptr->inter_sys_meas_info_incl = FALSE;
  l1_sib11_inter_rat_ptr->inter_s.num_cell_rmv = 0;
  l1_sib11_inter_rat_ptr->inter_s.num_cell_add = 0;
  l1_sib11_inter_rat_ptr->inter_s.num_cell_rmv = 0;
  l1_sib11_inter_rat_ptr->inter_s.cells_for_meas_incl = FALSE;
}


/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_EVENT_CRITERIA

DESCRIPTION

  This function processes the event criteria received in a Measurement Control Message
    and stores in the format that is sent to L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If event criteria is invalid
  SUCCESS: If event criteria is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_event_criteria
(
  rrc_InterRATReportingCriteria* rrc_event_criteria_ptr,
  l1_inter_sys_report_crit_struct_type* l1_event_criteria_ptr
)
{
  /* Store the list in a local pointer */
  rrc_InterRATEventList* rrc_local_list_ptr = NULL;
  uint32 idx=0;
  uint8 cnt = 0;

  rrc_local_list_ptr = &rrc_event_criteria_ptr->interRATEventList;

  while ((rrc_local_list_ptr->n > idx) && (cnt < L1_MAX_MEAS_EVENT))
  {
    /* Get Event Id */
    l1_event_criteria_ptr->evt[cnt].evt_id =
      RET_INTER_RAT_EVENT_TYPE(rrc_local_list_ptr->elem[idx].t);

    switch(l1_event_criteria_ptr->evt[cnt].evt_id)
    {
    case L1_INTER_RAT_EVENT_3A:
    /* If The estimated quality of the currently used UTRAN frequency
    is below a certain threshold and the estimated quality of the
      other system is above a certain threshold.*/

      /* Get own threshold */
      l1_event_criteria_ptr->evt[cnt].own_thresh = (int16)
        rrc_local_list_ptr->elem[idx].u.event3a->thresholdOwnSystem;

      /* Get W */
      l1_event_criteria_ptr->evt[cnt].w = (uint16)
        rrc_local_list_ptr->elem[idx].u.event3a->w;

      /* Get Threshold other system */
      l1_event_criteria_ptr->evt[cnt].other_thresh = (int16)
        rrc_local_list_ptr->elem[idx].u.event3a->thresholdOtherSystem;

      /* Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        rrc_local_list_ptr->elem[idx].u.event3a->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_RAT_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[idx].u.event3a->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      /* Check if reporting cell status present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_local_list_ptr->elem[idx].u.event3a,
        rrc_Event3a,reportingCellStatus))
      {
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;

          if ((T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells != 
             rrc_local_list_ptr->elem[idx].u.event3a->reportingCellStatus.t) ||
            (rrcmeas_fill_no_of_reporting_cells(
            &rrc_local_list_ptr->elem[idx].u.event3a->reportingCellStatus,
            &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == FAILURE))
        {
          return FAILURE;
        }
      }
      break;


    case L1_INTER_RAT_EVENT_3B:
      /* The estimated quality of other system is below a certain threshold. */

      /* Get Threshold other system */
      l1_event_criteria_ptr->evt[cnt].other_thresh = (int16)
        rrc_local_list_ptr->elem[idx].u.event3b->thresholdOtherSystem;

      /* Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        rrc_local_list_ptr->elem[idx].u.event3b->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_RAT_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[idx].u.event3b->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      /* Check if reporting cell status present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_local_list_ptr->elem[idx].u.event3b,
        rrc_Event3b,reportingCellStatus))
      {
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;

        if ((T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells != 
           rrc_local_list_ptr->elem[idx].u.event3b->reportingCellStatus.t) ||
           (rrcmeas_fill_no_of_reporting_cells(
          &rrc_local_list_ptr->elem[idx].u.event3b->reportingCellStatus,
           &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == FAILURE))
        {
          return FAILURE;
        }
      }
      break;

    case L1_INTER_RAT_EVENT_3C:
      /* Event 3c: The estimated quality of other system is above a certain threshold. */

      /* Get Threshold other system */
      l1_event_criteria_ptr->evt[cnt].other_thresh = (int16)
        rrc_local_list_ptr->elem[idx].u.event3c->thresholdOtherSystem;

      /* Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        rrc_local_list_ptr->elem[idx].u.event3c->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_RAT_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[idx].u.event3c->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      /* Check if reporting cell status present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_local_list_ptr->elem[idx].u.event3c,
        rrc_Event3b,reportingCellStatus))
      {
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;

        if ((T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells != 
           rrc_local_list_ptr->elem[idx].u.event3c->reportingCellStatus.t) ||
           (rrcmeas_fill_no_of_reporting_cells(
          &rrc_local_list_ptr->elem[idx].u.event3c->reportingCellStatus,
           &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == FAILURE))
        {
          return FAILURE;
        }
      }
      break;

    case L1_INTER_RAT_EVENT_3D:
      /* Event 3d: Change of best cell in other system */
      /* Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        rrc_local_list_ptr->elem[idx].u.event3d->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_RAT_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[idx].u.event3d->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      /* Check if reporting cell status present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_local_list_ptr->elem[idx].u.event3d,
         rrc_Event3d,reportingCellStatus))
      {
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;

        if ((T_rrc_ReportingCellStatus_withinActSetOrVirtualActSet_InterRATcells != 
            rrc_local_list_ptr->elem[idx].u.event3d->reportingCellStatus.t) ||
           (rrcmeas_fill_no_of_reporting_cells(
          &rrc_local_list_ptr->elem[idx].u.event3d->reportingCellStatus,
            &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == FAILURE))
        {
          return FAILURE;
        }
      }
      break;

    } /* End of switch */

    cnt++;

    /* Increment Pointer count */
    idx++;

  } /* End of while */

  /* Store the count */
  l1_event_criteria_ptr->num_event = cnt;

  return SUCCESS;

}


/*===========================================================================

FUNCTION   RRCIRAT_CHECK_INTER_RAT_CELL_ORDER_MCM

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message
  and checks if there are cells with same cell identity.


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If 2 cells have the same cell identity
  SUCCESS: If no 2 cells have the same cell identity

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_inter_rat_cell_order_mcm
(
  rrc_NewInterRATCellList   *local_inter_rat_cell_list_ptr
)
{
  /* Find the cell order */

  uint32 cell_id_list[L1_MAX_CELL_MEAS];
  uint16 count=0, list_count1 =0, list_count2 = 0;

  /* Initialize addl meas list to satisfy lint */
  for (count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    cell_id_list[count] = 0;
  }

  count = 0;

  /* Store cell id's in a local list and calculate total no of cells */
  while ((local_inter_rat_cell_list_ptr->n > list_count1) &&(count < L1_MAX_CELL_MEAS))
  {

    if (local_inter_rat_cell_list_ptr->elem[list_count1].m.interRATCellIDPresent)

    {
      /* Store cell id */
      cell_id_list[count] = local_inter_rat_cell_list_ptr->elem[list_count1].interRATCellID;
      count++;
    }
    /* Cell id was never present which is very likely */
    list_count1++;
  }

  if (count == 0 )
  {
    WRRC_MSG0_HIGH("Inter-RAT Cell list: No cell Id's");
    /* Return successfully as cells would now be stored
    in first available position */
    return SUCCESS;
  }
  if (count == 1)
  {
    WRRC_MSG0_HIGH("Inter-RAT Cell list: 1 cell with id");
    /* So no comparision has to be done. */
    return SUCCESS;
  }
  /* This means that there are multiple cells in the cell list and there
  order needs to be verified */

  /* Compare 1 st with 2,3,4..... Then 2nd with 3,4,5... and so on */
  for (list_count1 = 0; list_count1 < (count-1); list_count1++)
  {
    for (list_count2 = list_count1+1; list_count2 < count; list_count2++)
    {
      if (cell_id_list[list_count1] == cell_id_list[list_count2])
      {
        WRRC_MSG0_ERROR("2 GSM cells=> Same CellId");
        /* Cell Id's match */
        return FAILURE;

      }

    } /* End of for for list_count2 */
  }  /* End of for for list_count1 */
#ifdef DEBUG_MSG
  WRRC_MSG1_HIGH("Inter-RAT Cell list has %d cells",count);
#endif
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_FILL_REMOVED_INTER_RAT_CELLS

DESCRIPTION

  This function processes the removed cell list received in a Measurement Control Message


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If removed cell list is valid
  SUCCESS: If removed cell list is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_fill_removed_inter_rat_cells
(
  rrc_RemovedInterRATCellList* rrc_remove_cells_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
)
{
  /* To store the temp pointer which reads element from the rrc__seqof3 list */
  uint8 removed_cell_count = 0;

  /* Intra Freq Cell Id */
  uint16 inter_rat_cell_id = 0;

  rrc_RemovedInterRATCellList_removeSomeInterRATCells* local_rrc_removed_ptr = NULL;
  uint32 idy =0;
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  switch ( rrc_remove_cells_ptr->t )
  {
  case T_rrc_RemovedInterRATCellList_removeAllInterRATCells:

    WRRC_MSG0_HIGH("MCM:Remove all Inter-RAT cells");
    l1_cell_list_ptr->num_cell_rmv = L1_REMOVE_ALL_CELLS;
    /* Remove all cells in local_neighbor_list for inter-RAT */
    for (removed_cell_count=0; removed_cell_count<L1_MAX_CELL_MEAS; removed_cell_count++ )
    {
      local_neighbor_list[removed_cell_count].cell_id = INVALID_CELL_ID;
      local_neighbor_list[removed_cell_count].cell_position = VACANT;
    }
#ifdef FEATURE_GAN_3G
    #error code not present
#endif
    break;

  case T_rrc_RemovedInterRATCellList_removeSomeInterRATCells:
    WRRC_MSG0_HIGH("MCM:Remove some Inter-RAT cells");

    /* Local ptr to store removed cells */
    local_rrc_removed_ptr = (rrc_remove_cells_ptr->u.removeSomeInterRATCells);


    if ( local_rrc_removed_ptr == NULL )
    {
      WRRC_MSG0_HIGH("MCM:Inter-RAT removed cell list ptr NULL");
      return FAILURE;
    }

    do
    {
      /* Get cell id */
      inter_rat_cell_id =  (uint16) local_rrc_removed_ptr->elem[idy];
      /* Find psc of the cell id */
      if (rrcirat_find_cell_id(&inter_rat_cell_id) == FAILURE)
      {
        WRRC_MSG0_ERROR("MCM:Inter RAT Cell to be removed not found");
        /*  Get next element in the list */
        idy++;
        continue;
      }
      else
      {
        /* Removes cell from local neighbor list */
        rrcirat_remove_cell_id(&inter_rat_cell_id);

        /* Store this removed cell id for L1 */
        l1_cell_list_ptr->rmv_cell[removed_cell_count].intersys_cell_id =
          inter_rat_cell_id;
      }
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
      removed_cell_count++; /* Increment count */

      /*  Get next element in the list */
      idy++;


    } /* Continue if cond satisfied */

    while ( (local_rrc_removed_ptr->n > idy) &&
      (removed_cell_count < L1_MAX_CELL_MEAS) );

    /* Now store the number of actual cells sent by UTRAN */

    l1_cell_list_ptr->num_cell_rmv =  removed_cell_count;

    break;

  case T_rrc_RemovedInterRATCellList_removeNoInterRATCells:

    WRRC_MSG0_HIGH("MCM:Remove 0 Inter-RAT cells");

    l1_cell_list_ptr->num_cell_rmv = 0;
    break;

  default:
    WRRC_MSG0_ERROR("Invalid Choice");
    return FAILURE;

  } /* End of switch */

  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_FIND_CELL_ID

DESCRIPTION

  This function processes the inter rat cell id


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell id is valid
  SUCCESS: If cell id is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_find_cell_id
(
 uint16* intra_freq_cell_id_ptr
)
{
  uint8 count = 0; /* Count variable */

  for (count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    /* Found if cell id exists */
    if ((local_neighbor_list[count].cell_position == OCCUPIED) &&
      (local_neighbor_list[count].cell_id == *intra_freq_cell_id_ptr))
    {
      return SUCCESS;
    }
  }

  return FAILURE;
}


/*===========================================================================

FUNCTION   RRCIRAT_REMOVE_CELL_ID

DESCRIPTION

  This function removes the inter rat cell id


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_remove_cell_id
(
 uint16* inter_rat_cell_id_ptr
)
{
  uint8 count = 0; /* Count variable */
  boolean cell_found = FALSE;

  for (count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    if ((count == (*inter_rat_cell_id_ptr)) &&
      (local_neighbor_list[count].cell_position == OCCUPIED))
    {
      WRRC_MSG1_HIGH("MCM:Removing cell id with id %d",
        *inter_rat_cell_id_ptr);

      local_neighbor_list[count].cell_position = VACANT;

      local_neighbor_list[count].cell_id =  INVALID_CELL_ID;

      cell_found = TRUE;

    }
  } /* End of for loop */

  if (cell_found == FALSE)
  {
    WRRC_MSG0_ERROR("Rem:Inter-RAT cell absent in local neighbor list");
  }

  return;
}


/*===========================================================================

FUNCTION   RRCIRAT_UPDATE_INTER_RAT_CELL_LIST_MCM

DESCRIPTION

  This function updates the inter rat cell list received in a Meas Ctrl Msg


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell list is valid
  SUCCESS: If cell list is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_update_inter_rat_cell_list_mcm
(
 uint32* position_no_ptr, /* First availabble position or cell id */
 uint16* cell_id_ptr,  /* L1 value */
 uint16* cell_id_to_remove_ptr  /* Determines if a cell has to be removed */
)
{
  uint16 count = 0;
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif

  /* First check if one has to add this in the first available position */
  if (*position_no_ptr == AVAILABLE_CELL_POSITION)
  {
    for (count = 0; count < L1_MAX_CELL_MEAS; count++)
    {
      if (local_neighbor_list[count].cell_position == VACANT)
      {
        /* Store this cell in this first available vacant position */
        local_neighbor_list[count].cell_position = OCCUPIED;
        local_neighbor_list[count].cell_id = count;

        /* This is the new cell id. So store it back in l1 struct */
        *cell_id_ptr = count;

        *cell_id_to_remove_ptr = INVALID_CELL_ID;

        WRRC_MSG2_HIGH("Add:Cell added at pos %d with id = %d",count,*cell_id_ptr);

        return SUCCESS;
      }
    } /* End of for */
  } /* End of if of  position_no_ptr == AVAILABLE_CELL_POSITION */

    /* position_no_ptr and cell_id_ptr has same value for cell
       to be stored when cell id given */
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  /* Now check which position the cells needs to be updated */
  for (count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    /* Check if there is already a cell in this position
       If yes, then this cell has to be removed and L1
       has to be informed about it. So store its cell id  */

    if ( (count == *position_no_ptr) &&
      (local_neighbor_list[count].cell_position == OCCUPIED) )
    {

      MSG_HIGH("Add:Cell Id %d at pos. %d removed[Cascading] by %d",
        local_neighbor_list[count].cell_id,count,*cell_id_ptr);


      /* Store the removed cell id */
      *cell_id_to_remove_ptr = local_neighbor_list[count].cell_id;

      /* Store the new cell id at this position */
      local_neighbor_list[count].cell_id = *cell_id_ptr;
      
#ifdef FEATURE_GAN_3G
      #error code not present
#endif

      return SUCCESS;
    }
    else if ( (count == *position_no_ptr) &&
      (local_neighbor_list[count].cell_position == VACANT) )
    {

      WRRC_MSG2_MED("Add:Adding cell at pos %d with id %d",
        count,*cell_id_ptr);


      /* Store the new psc at this position */
      local_neighbor_list[count].cell_id = *cell_id_ptr;

      /* Mark the position as occupied. NOT REQD */
      local_neighbor_list[count].cell_position = OCCUPIED;

      /* No cell id to be removes as this place is vacant */
      *cell_id_to_remove_ptr = INVALID_CELL_ID;

      return SUCCESS;

    } /* End of else if */
  } /* End of for */
  WRRC_MSG1_ERROR("Inter-RAT cell ID = %d not valid", *cell_id_ptr);
  return FAILURE;
}


/*===========================================================================

FUNCTION   RRCIRAT_INITIALIZE_CELL_LIST

DESCRIPTION

  This function initializes the inter rat cell list


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_initialize_cell_list(void)
{
  uint8 cnt = 0;
  rrcirat_initialize_local_neighbor_list();
  for (cnt =0; cnt <L1_MAX_CELL_MEAS; cnt++ )
  {
    inter_rat_cell_list[cnt].cell_id = INVALID_CELL_ID;
    inter_rat_cell_list[cnt].cell_position = VACANT;
  }

  interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
  interRAT_cell_info_status.interRAT_cell_info_ind=0;

#ifdef FEATURE_GAN_3G
  #error code not present
#endif
#ifdef FEATURE_WCDMA_TO_LTE
  rrcirat_initialize_eutra_freq_list();
#endif
}


/*===========================================================================

FUNCTION   RRCIRAT_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Iner RAT Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcirat_initialize_meas_id_list
(
 void
)
{
  uint8 count = 0, cnt = 0;
  /* Initialize Measurement Identity variable */
  MSG_LOW("Initializing Inter-RAT Meas Id List",0,0,0);
  for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
  {
    /* Set all positions to vacant and make PSC as invalid */
    inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;
    inter_rat_meas_id_list[count].meas_validity = STATE_DCH;
    inter_rat_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
#ifdef FEATURE_GAN_3G
    #error code not present
#endif
    inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
    /* Also initialize additional meas */
    for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
      inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
  }

#ifdef FEATURE_GAN_3G
  #error code not present
#endif
}


/*===========================================================================

FUNCTION   rrcirat_initialize_local_neighbor_list

DESCRIPTION

  This function initializes the local neighbor list


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_initialize_local_neighbor_list(void)
{
  uint8 cnt = 0;
  for (cnt =0; cnt <L1_MAX_CELL_MEAS; cnt++ )
  {
    local_neighbor_list[cnt].cell_id = INVALID_CELL_ID;
    local_neighbor_list[cnt].cell_position = VACANT;
  }
}

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION   rrcirat_is_meas_id_eutra_meas

DESCRIPTION
  This function returns TRUE if the meas id is EUTRA meas id

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the meas id is found in DB and is EUTRA measurement
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcirat_is_meas_id_eutra_meas
(
  uint16 meas_id
)
{
  uint8 cnt=0;
  boolean status=FALSE;
  for(cnt = 0;cnt < MAX_INTER_RAT_MEAS ; cnt++)
  {
    if((meas_id == inter_rat_meas_id_list[cnt].meas_id) && (inter_rat_meas_id_list[cnt].interrat_type == RRCMEAS_IRAT_EUTRA))
    {
      status = TRUE;
      break;
    }
  }
  return status;
}

/*===========================================================================

FUNCTION   rrcirat_send_eutra_meas_report

DESCRIPTION
  This function populates EUTRA measurement report based on WL1 reports and submits the report to RLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcirat_send_eutra_meas_report
(
  l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{

  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;
  /* Indicates if measured results would be appended to event results */
  boolean measured_res_incl = FALSE;

  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode;

  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id;

  /* Radio bearer mode */
  rrc_RB_Identity rb_mode;

  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status;

  uint8 meas_id_cnt,cnt;

  /* Stores status of send chain */
  rrcsend_status_e_type status;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_mrm_ota mrm_params;
#endif

  /*Validate the measurement results from WL1, TBD: Do we really need this, If WL1 takes care of this stuff, then not needed*/
  if(l1_meas_report_ptr->meas_type != L1_INTER_SYS_EUTRA)
  {
    WRRC_MSG2_ERROR("RRC has the meas_id %d as EUTRA where as L1 indicated meas_type as %d",
        l1_meas_report_ptr->meas_id,l1_meas_report_ptr->meas_type);
    return;
  }

#ifdef FEATURE_QSH_EVENT_METRIC
  memset(&mrm_params,0,sizeof(rrc_log_rrc_mrm_ota));    
  mrm_params.meas_id = l1_meas_report_ptr->meas_id;
  mrm_params.meas_type = L1_INTER_SYS_EUTRA;
  mrm_params.meas_rpt_type = l1_meas_report_ptr->rpt_crit;
  if(mrm_params.meas_rpt_type == L1_EVENT_TRIGGER_RPT)
  {
    mrm_params.event_id = l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.event_id;
  }	  
  mrm_params.num_cells_rpt = l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.num_freq;
  rrc_qsh_log_meas_rpt_params(&mrm_params);
#endif

  if(0 == l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.measured_eutra_freqs)
  {
    if(l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT)
    {
      WRRC_MSG0_ERROR("Periodic Measurement results absent");
      /* allowed empty MCR  to go eventhough no eutra freq present, done as part of cr fix */
      //return;
    }
    else
    {
      WRRC_MSG0_HIGH("Measurement results absent");
    }
  }
  else
  {
    measured_res_incl = TRUE;
  }


  if ( l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE )
  {
    logical_channel_mode = UE_MODE_ACKNOWLEDGED_DATA;
    rb_mode = DCCH_AM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_REQUIRED;
  }
  else
  {
    logical_channel_mode = UE_MODE_UNACKNOWLEDGED;
    rb_mode = DCCH_UM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_NOT_REQUIRED;
  }

  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH, rb_mode,
    logical_channel_mode);


  if ( ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND )
  {
    WRRC_MSG0_ERROR("RLC Logical channel not found");
    return;
  }

  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));

  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;


  /* First store the meas id */
  msg_ptr->message.u.measurementReport.measurementIdentity =
   (rrc_MeasurementIdentity) l1_meas_report_ptr->meas_id;

  /* Initialize Meas Report bit mask */
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.measurementReport);

  /* First store the meas id */
  msg_ptr->message.u.measurementReport.measurementIdentity =
    l1_meas_report_ptr->meas_id;

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport,v390nonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.measurementReport_v390ext);
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions,laterNonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions,v4b0NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions,
    v590NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.measurementReport_v590ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions);


  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions,v5b0NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions,v690NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.measurementReport_v690ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions,v770NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
    measurementReport_v770ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions);

  RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions,v860NonCriticalExtensions);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
    v860NonCriticalExtensions.measurementReport_v860ext);
  RRC_RESET_MSG_IE_PRESENT(
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
    v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions);

  if((l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT) && 
    (0 != l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.num_freq))
  {
    rrc_Eutra_EventResultList *local_event_list_ptr = NULL;
    uint8 freq_count, cell_count,num_valid_freq;

    num_valid_freq = 0;

    for(freq_count = 0;freq_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.num_freq && 
          freq_count <L1_MAX_REP_EUTRA_FREQ ;freq_count++)
    {
      if( 0 != l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].num_cell)
      {
        num_valid_freq++;
      }
    }
    if(num_valid_freq !=0)
    {
      uint8 freq_idx =0;
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        v860NonCriticalExtensions.measurementReport_v860ext,eutra_EventResults);

      rrcirat_set_inter_sys_event_id(&l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.event_id,
         &msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
             v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
             v860NonCriticalExtensions.measurementReport_v860ext.eutra_EventResults.eventID
          );
      local_event_list_ptr = & msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
                       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                       v860NonCriticalExtensions.measurementReport_v860ext.eutra_EventResults.eutra_EventResultsList;
      local_event_list_ptr->n = num_valid_freq;
      ALLOC_ASN1ARRAY2(&enc_ctxt,local_event_list_ptr,rrc_Eutra_EventResult);

      for(freq_count = 0;freq_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.num_freq && 
            freq_count <L1_MAX_REP_EUTRA_FREQ && freq_idx < num_valid_freq && freq_idx < L1_MAX_REP_EUTRA_FREQ ;freq_count++)
      {
        rrc_Eutra_EventResult_reportedCells *cell_list_ptr = NULL;
        if( 0 == l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].num_cell)
        {
          continue;
        }
        local_event_list_ptr->elem[freq_idx].earfcn = (uint16)l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].earfcn;
        cell_list_ptr = &local_event_list_ptr->elem[freq_idx].reportedCells;
        cell_list_ptr->n = 
          (l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].num_cell > L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY)
          ? L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY :l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].num_cell ;
        ALLOC_ASN1ARRAY2(&enc_ctxt,cell_list_ptr,rrc_EUTRA_PhysicalCellIdentity);

        for(cell_count = 0; cell_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].num_cell &&
              cell_count < L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY ; cell_count++)
        {
          cell_list_ptr->elem[cell_count] = l1_meas_report_ptr->u.inter_sys_eutra_rpt.evt_res.freqs[freq_count].phy_cell_id[cell_count];
        }
        freq_idx++;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("W2L: No LTE cells present in Event report from WL1");
    }
  }


  if(measured_res_incl)
  {
    rrc_Eutra_MeasuredResultList *local_meas_list_ptr = NULL;
    uint8 freq_count, cell_count,num_valid_freq;

    num_valid_freq = 0;
    for(freq_count = 0;freq_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.measured_eutra_freqs && 
          freq_count <L1_MAX_REP_EUTRA_FREQ ;freq_count++)
    {
      if( 0 != l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].num_cell)
      {
        num_valid_freq++;
      }
    }
    if(num_valid_freq != 0)
    {
      uint8 freq_idx =0;
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        v860NonCriticalExtensions.measurementReport_v860ext,eutra_MeasuredResults);
      local_meas_list_ptr = & msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
                       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                       v860NonCriticalExtensions.measurementReport_v860ext.eutra_MeasuredResults.eutraMeasuredResultList;
      local_meas_list_ptr->n = num_valid_freq;
      ALLOC_ASN1ARRAY2(&enc_ctxt,local_meas_list_ptr,rrc_Eutra_MeasuredResult);

      for(freq_count = 0;freq_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.measured_eutra_freqs && 
            freq_count <L1_MAX_REP_EUTRA_FREQ && freq_idx < num_valid_freq && freq_idx < L1_MAX_REP_EUTRA_FREQ;freq_count++)
      {
        rrc_Eutra_MeasuredResult_measuredEUTRACells * cell_list_ptr = NULL;
        if( 0 == l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].num_cell)
        {
          continue;
        }
        local_meas_list_ptr->elem[freq_idx].earfcn = (uint16)l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].earfcn;
        cell_list_ptr = & local_meas_list_ptr->elem[freq_idx].measuredEUTRACells;
        cell_list_ptr->n = 
          (l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].num_cell > L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY) ?
          L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY : l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].num_cell;
        ALLOC_ASN1ARRAY2(&enc_ctxt,cell_list_ptr,rrc_EUTRA_MeasuredCells);

        for(cell_count = 0; cell_count < l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].num_cell &&
              cell_count < L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY ; cell_count++)
        {
          cell_list_ptr->elem[cell_count].physicalCellIdentity = l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].cells[cell_count].phy_cell_id;
          RRC_RESET_MSG_IE_PRESENT(cell_list_ptr->elem[cell_count]);
          if(l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].cells[cell_count].rsrq_incl)
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(cell_list_ptr->elem[cell_count],rSRQ);
            cell_list_ptr->elem[cell_count].rSRQ =  l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].cells[cell_count].rsrq;
          }
          if(l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].cells[cell_count].rsrp_incl)
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(cell_list_ptr->elem[cell_count],rSRP);
            cell_list_ptr->elem[cell_count].rSRP =  l1_meas_report_ptr->u.inter_sys_eutra_rpt.meas_res.freqs[freq_count].cells[cell_count].rsrp;
          }
        }
        freq_idx ++;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("W2L: No LTE cells present in measured results from WL1");
    }

    /* Now check if this is the last report. Meas Id needs to be deleted if true */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      /* Delete this Measurement */
      for (meas_id_cnt =0; meas_id_cnt < MAX_INTER_RAT_MEAS; meas_id_cnt++ )
      {
        if (inter_rat_meas_id_list[meas_id_cnt].meas_id == l1_meas_report_ptr->meas_id)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",l1_meas_report_ptr->meas_id);
          inter_rat_meas_id_list[meas_id_cnt].meas_id = INVALID_MEAS_ID;
  
          inter_rat_meas_id_list[meas_id_cnt].rpt_crit = NOT_AVAILABLE;

#ifdef FEATURE_GAN_3G
          #error code not present
#endif

          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            inter_rat_meas_id_list[meas_id_cnt].additional_meas[cnt] = INVALID_MEAS_ID;

          break;
        }
      }
    }

    /* Now delete this meas from addtl meas DB */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      WRRC_MSG1_HIGH("Delete %d from addtl meas DB", l1_meas_report_ptr->meas_id);
      rrcmeas_delete_addtl_meas(&l1_meas_report_ptr->meas_id);
    }
  }

  /* Append Additional Measured Results if available */
  rrcmeas_process_additional_measured_results(
    &l1_meas_report_ptr->meas_id, INTER_SYS, &msg_ptr->message.u.measurementReport);
  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);
  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("Send chain failed to send Inter-RAT Meas Rpt");
  }
  else
  {
    WRRC_MSG0_HIGH("EUTRA Inter-RAT Meas Rpt sent from RRC=>RLC");
    if(l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE)
    {
      rrcmeas_pending_rpt++;
      if(rrcmeas_pending_rpt >= 5)
      {
        WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
      }
    }
  }

}
#endif

/*===========================================================================

FUNCTION   RRCIRAT_SEND_MEAS_REPORT

DESCRIPTION

  This function sends the Measurement Report to UTRAN in CELL_DCH only

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;

  /* Used as a counter to store cell number counted */
  uint8 cell_number = 0;

  /* GSM Cell Count */
  uint8 gsm_cell_count = 0;

  /* Event Cell Count */
  uint8 event_cell_count = 0;

  /* Stores status of send chain */
  rrcsend_status_e_type status;

  /* Stores RSSI value */
  uint8 rssi_value;

  uint8 meas_id_cnt = 0, cnt = 0;

  /* Indicates if measured results would be appended to event results */
  boolean measured_res_incl = FALSE;

  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode;

  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id;

  /* Radio bearer mode */
  rrc_RB_Identity rb_mode;

  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status;

  /* Contains ptr to the head of results of Inter-RAT Measured Results */
  rrc_InterRATMeasuredResultsList* local_rrc_measured_results_ptr = NULL;

  /* Contains Ptr to the head of GSM cells in one Inter-RAT system */
  rrc_GSM_MeasuredResultsList* local_rrc_gsm_list_ptr = NULL;

  rrc_CellToReportList* local_cell_list_ptr = NULL;

#ifdef FEATURE_GAN_3G
  #error code not present
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_mrm_ota mrm_params;
  memset(&mrm_params,0,sizeof(rrc_log_rrc_mrm_ota));  

  mrm_params.meas_id = l1_meas_report_ptr->meas_id;
  mrm_params.meas_type = L1_INTER_SYS;
  mrm_params.meas_rpt_type = l1_meas_report_ptr->rpt_crit;
  if(mrm_params.meas_rpt_type == L1_EVENT_TRIGGER_RPT)
  {
    mrm_params.event_id = l1_meas_report_ptr->u.inter_sys_rpt.event_results.event_id;
  }	  
  mrm_params.num_cells_rpt = l1_meas_report_ptr->u.inter_sys_rpt.event_results.num_cells;
  rrc_qsh_log_meas_rpt_params(&mrm_params);
#endif

  /* Sanity Tests before building Measurement Report */
  /* Check if reporting criteria is O.K. */
  if (l1_meas_report_ptr->rpt_crit == L1_NO_RPT ||
    l1_meas_report_ptr->rpt_crit == L1_NO_CHANGE )
  {
    WRRC_MSG1_ERROR("Rejecting Inter-RAT Report. R.Crit=%d", l1_meas_report_ptr->rpt_crit);
    return;
  }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  if(rrcirat_is_meas_id_eutra_meas(l1_meas_report_ptr->meas_id))
  {
    /*Call the EUTRA function to report EUTRA measurements*/
    rrcirat_send_eutra_meas_report(l1_meas_report_ptr);
    return;
  }
#endif

  if (l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT)
  {
    /* Validate Measured Results */
    if (rrcirat_validate_measured_results(
      &l1_meas_report_ptr->u.inter_sys_rpt.measurements) == FAILURE)
    {
      WRRC_MSG0_ERROR("Invalid Measured Results");
      return;
    }
    else
    {
      measured_res_incl = TRUE;
    }
  }
  else
  {
    if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
    {
      /* Validate event results */
      if (rrcirat_validate_event_results(&l1_meas_report_ptr->u.inter_sys_rpt.event_results) ==
        FAILURE)
      {
        WRRC_MSG0_HIGH("Inter-RAT Meas Report not sent");
        return;
      }

      /* Now check if measured results are present */
      if (rrcirat_validate_measured_results(
        &l1_meas_report_ptr->u.inter_sys_rpt.measurements) == FAILURE)
      {
        WRRC_MSG0_HIGH("Ignore Measured Results");
        /* Nevertheless send event report only */
      }
      else
      {
        measured_res_incl = TRUE;
      }
    }
  } /* End of else of event triggered report */

  if ( l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE )
  {
    logical_channel_mode = UE_MODE_ACKNOWLEDGED_DATA;
    rb_mode = DCCH_AM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_REQUIRED;
  }
  else
  {
    logical_channel_mode = UE_MODE_UNACKNOWLEDGED;
    rb_mode = DCCH_UM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_NOT_REQUIRED;
  }

  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH, rb_mode,
    logical_channel_mode);


  if ( ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND )
  {
    WRRC_MSG0_ERROR("RLC Logical channel not found");
    return;
  }

  /* Allocate a buffer for an uplink DCCH Message. This buffer
  * should be released by SEND_CHAIN using OSS compiler functions */

  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));

  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;


  /* First store the meas id */
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,l1_meas_report_ptr->meas_id, INTER_SYS);



  /* Append Event Results if present */
  if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
  {
    /* Set event results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,eventResults);

    /* Set t to Inter RAT Events Results chosen */
    RRC_SET_COMMON_MSG_TYPE2(msg_ptr->message.u.measurementReport.eventResults,
    rrc_EventResults,interRATEventResults);

    /* Allocate Memory for the cell */
    msg_ptr->message.u.measurementReport.eventResults.u.interRATEventResults =
         rtxMemAllocTypeZ(&enc_ctxt,rrc_InterRATEventResults);

    /* Set Event ID */
    rrcirat_set_inter_sys_event_id(&l1_meas_report_ptr->u.inter_sys_rpt.event_results.event_id,
      &msg_ptr->message.u.measurementReport.eventResults.u.
      interRATEventResults->eventID);

    local_cell_list_ptr =  &msg_ptr->message.u.measurementReport.eventResults.u.
        interRATEventResults->cellToReportList;

    local_cell_list_ptr->n = 
        l1_meas_report_ptr->u.inter_sys_rpt.event_results.num_cells;
    ALLOC_ASN1ARRAY2(&enc_ctxt,local_cell_list_ptr,
        rrc_CellToReport);

    /* Set Cell on which Event occurred */
    for (event_cell_count = 0;
    event_cell_count < l1_meas_report_ptr->u.inter_sys_rpt.event_results.num_cells;
    event_cell_count++)
    {
      /* If BSIC Verified */
      if (l1_meas_report_ptr->u.inter_sys_rpt.event_results.cell[event_cell_count].bsic_choice == L1_BSIC_VERIFIED)
      {
        local_cell_list_ptr->elem[event_cell_count].bsicReported.t = T_rrc_BSICReported_verifiedBSIC;

        WRRC_MSG1_HIGH("Verified BSIC is %d",
          l1_meas_report_ptr->u.inter_sys_rpt.event_results.cell[event_cell_count].u.
          bsic_verified.intersys_cell_id);

        /* Set cell id to Verified BSIC */
        local_cell_list_ptr->elem[event_cell_count].bsicReported.u.verifiedBSIC =
         (uint8) l1_meas_report_ptr->u.inter_sys_rpt.event_results.cell[event_cell_count].u.
          bsic_verified.intersys_cell_id;
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
      }
      else
      {
        local_cell_list_ptr->elem[event_cell_count].bsicReported.t = T_rrc_BSICReported_nonVerifiedBSIC;

        /* Set cell id to Non Verified BSIC */
        local_cell_list_ptr->elem[event_cell_count].bsicReported.u.nonVerifiedBSIC =
          CONVERT_ARFCN_TO_NUM(l1_meas_report_ptr->u.inter_sys_rpt.event_results.cell[event_cell_count].u.
          bsic_nonverified.bcch_arfcn);


        WRRC_MSG1_HIGH("Non Verified BSIC is %d",
          CONVERT_ARFCN_TO_NUM(l1_meas_report_ptr->u.inter_sys_rpt.event_results.cell[event_cell_count].u.
          bsic_nonverified.bcch_arfcn));
        
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
      }
      

    } /* End of for */
  } /* End of if of Event Triggered Events */

  /* Now add Measured Results */
  if ((measured_res_incl == FALSE) ||
    (l1_meas_report_ptr->u.inter_sys_rpt.measurements.
    inter_sys_msmt_results[0].u.gsm.measured_gsm_cells == 0))
  {
    WRRC_MSG1_HIGH("MeasResult: measured_res_incl =",measured_res_incl);
  }
  else
  {

    /* First set the bit mask for Measured Results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,measuredResults);

    /* Set Measured Results t */
    RRC_SET_COMMON_MSG_TYPE2(msg_ptr->message.u.measurementReport.measuredResults,
      rrc_MeasuredResults,interRATMeasuredResultsList);

    /* Allocate Memory */
    local_rrc_measured_results_ptr =
      rtxMemAllocTypeZ(&enc_ctxt,rrc_InterRATMeasuredResultsList);

    /* Store the head ptr */
    msg_ptr->message.u.measurementReport.measuredResults.u.interRATMeasuredResultsList
      = local_rrc_measured_results_ptr;

    cell_number = 0;

    local_rrc_measured_results_ptr->n =1;

    ALLOC_ASN1ARRAY2(&enc_ctxt,local_rrc_measured_results_ptr,rrc_InterRATMeasuredResults);
    /* Set System to GSM */
    local_rrc_measured_results_ptr->elem[0].t =
      T_rrc_InterRATMeasuredResults_gsm;

    local_rrc_gsm_list_ptr = rtxMemAllocTypeZ(&enc_ctxt,rrc_GSM_MeasuredResultsList);
    

    /* Assign Head Ptr */
    local_rrc_measured_results_ptr->elem[0].u.gsm = local_rrc_gsm_list_ptr;
   
    WRRC_MSG1_HIGH("No of GSM cells are %d", l1_meas_report_ptr->u.inter_sys_rpt.measurements.
      inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells);
    local_rrc_gsm_list_ptr->n =
        l1_meas_report_ptr->u.inter_sys_rpt.measurements.
      inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells;

    ALLOC_ASN1ARRAY2(&enc_ctxt,local_rrc_gsm_list_ptr,rrc_GSM_MeasuredResults);
    
    /* For First RAT, read the GSM cell */
    for (gsm_cell_count = 0;
    gsm_cell_count < l1_meas_report_ptr->u.inter_sys_rpt.measurements.
      inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells;
    gsm_cell_count++)
    {

      RRC_RESET_MSG_IE_PRESENT(local_rrc_gsm_list_ptr->elem[gsm_cell_count]);

      /* If BSIC Verified */
      if (l1_meas_report_ptr->u.inter_sys_rpt.measurements.inter_sys_msmt_results[cell_number].
        u.gsm.measured_cells[gsm_cell_count].bsic_choice == L1_BSIC_VERIFIED)
      {
        local_rrc_gsm_list_ptr->elem[gsm_cell_count].bsicReported.t = T_rrc_BSICReported_verifiedBSIC;

        WRRC_MSG1_HIGH("Verified cell index is %d", l1_meas_report_ptr->u.inter_sys_rpt.measurements.inter_sys_msmt_results[cell_number].
          u.gsm.measured_cells[gsm_cell_count].u.bsic_verified.inter_sys_cell_id);

        /* Set cell id to Verified BSIC */
        local_rrc_gsm_list_ptr->elem[gsm_cell_count].bsicReported.u.verifiedBSIC =
          (uint8)l1_meas_report_ptr->u.inter_sys_rpt.measurements.inter_sys_msmt_results[cell_number].
          u.gsm.measured_cells[gsm_cell_count].u.bsic_verified.inter_sys_cell_id;
      }
      else
      {
        local_rrc_gsm_list_ptr->elem[gsm_cell_count].bsicReported.t = T_rrc_BSICReported_nonVerifiedBSIC;

        /* Set cell id to Non Verified BSIC */
        local_rrc_gsm_list_ptr->elem[gsm_cell_count].bsicReported.u.nonVerifiedBSIC =
          CONVERT_ARFCN_TO_NUM(l1_meas_report_ptr->u.inter_sys_rpt.measurements.inter_sys_msmt_results[cell_number].
          u.gsm.measured_cells[gsm_cell_count].u.bsic_not_verified.bcch_arfcn);

        WRRC_MSG1_HIGH("Non Verified BSIC is %d",
          local_rrc_gsm_list_ptr->elem[gsm_cell_count].bsicReported.u.nonVerifiedBSIC);

      }

      /* Set RSSI value */
      if (l1_meas_report_ptr->u.inter_sys_rpt.measurements.inter_sys_msmt_results[cell_number].
        u.gsm.measured_cells[gsm_cell_count].gsm_carrier_rssi_incl == TRUE)
      {
        /* Convert value from uint8 to oss bit string */
        rrcirat_convert_rssi_value(
          &l1_meas_report_ptr->u.inter_sys_rpt.measurements.
          inter_sys_msmt_results[cell_number].u.gsm.measured_cells[gsm_cell_count].
          gsm_carrier_rssi, &rssi_value);

        /* Double check if value is O.K */
        if (rssi_value <= MAX_RSSI)
        {
         

          /* As per specs, only right most 6 bits should be sent. Lint e734 */
          rssi_value = rssi_value << 2;     /*lint !e734 */

          /* Put Actual Value to be 6 bits */
          local_rrc_gsm_list_ptr->elem[gsm_cell_count].gsm_CarrierRSSI.numbits= RSSI_ACTUAL_LENGTH;

          *(local_rrc_gsm_list_ptr->elem[gsm_cell_count].gsm_CarrierRSSI.data) = (unsigned char ) (rssi_value);


          /* Set RSSI value present in Meas Report */
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(local_rrc_gsm_list_ptr->elem[gsm_cell_count],
            gsm_CarrierRSSI);

        }  /* End of rssi value less than MAX_RSSI */

      } /* End of RSSI included */
      if (gsm_cell_count == (l1_meas_report_ptr->u.inter_sys_rpt.measurements.
        inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells-1))
      {
        WRRC_MSG1_HIGH("Thus no of GSM cells set are %d",l1_meas_report_ptr->u.inter_sys_rpt.measurements.
          inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells);

        /* Reached the end of the gsm cell list */

        break; /* Come out of first for loop */

      }
     

    } /* End of for loop of gsm_cell_count */

    /* Since only 1 GSM RAT, initialize the next RAT to 0 */
    


    /* Now check if this is the last report. Meas Id needs to be deleted if true */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      /* Delete this Measurement */
      for (meas_id_cnt =0; meas_id_cnt < MAX_INTER_RAT_MEAS; meas_id_cnt++ )
      {
        if (inter_rat_meas_id_list[meas_id_cnt].meas_id == l1_meas_report_ptr->meas_id)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",l1_meas_report_ptr->meas_id);
          inter_rat_meas_id_list[meas_id_cnt].meas_id = INVALID_MEAS_ID;

          inter_rat_meas_id_list[meas_id_cnt].rpt_crit = NOT_AVAILABLE;

#ifdef FEATURE_GAN_3G
          #error code not present
#endif

          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            inter_rat_meas_id_list[meas_id_cnt].additional_meas[cnt] = INVALID_MEAS_ID;

          break;
        }
      }
    }

    /* Now delete this meas from addtl meas DB */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      WRRC_MSG1_HIGH("Delete %d from addtl meas DB", l1_meas_report_ptr->meas_id);
      rrcmeas_delete_addtl_meas(&l1_meas_report_ptr->meas_id);
    }

  } /* End of else of measured_res_incl */



  if(((msg_ptr->message.u.measurementReport.m.measuredResultsPresent) &&
       (msg_ptr->message.u.measurementReport.measuredResults.t == T_rrc_MeasuredResults_interRATMeasuredResultsList))
         || ((msg_ptr->message.u.measurementReport.m.eventResultsPresent) &&
             (msg_ptr->message.u.measurementReport.eventResults.t == T_rrc_EventResults_interRATEventResults)))
  {
    WRRC_MSG0_HIGH("measured result list present");
    if(interRAT_cell_info_status.interRAT_cell_info_present==TRUE)
    {
      msg_ptr->message.u.measurementReport.m.v390nonCriticalExtensionsPresent =1;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.m.laterNonCriticalExtensionsPresent =1;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent=1 ;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1 ;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5b0NonCriticalExtensionsPresent=1 ;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.
        measurementReport_v5b0ext.m.interRATCellInfoIndicationPresent =1 ;

      msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
          interRAT_cell_info_status.interRAT_cell_info_ind ;
      WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);
    }
  }


  /* Addtl Meas List must be initialized */
  msg_ptr->message.u.measurementReport.additionalMeasuredResults.n = 0;

  /* Append Additional Measured Results if available */
  rrcmeas_process_additional_measured_results(
    &l1_meas_report_ptr->meas_id, INTER_SYS, &msg_ptr->message.u.measurementReport);

  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);

  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("Send chain failed to send Inter-RAT Meas Rpt");
  }
  else
  {
    WRRC_MSG0_HIGH("Inter-RAT Meas Rpt sent from RRC=>RLC");
    if(l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE)
    {
      rrcmeas_pending_rpt++;
      if(rrcmeas_pending_rpt >= 5)
      {
        WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
      }
    }
  }

#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  return;

}

/*===========================================================================

FUNCTION    RRCMEAS_SET_INTRA_FREQ_EVENT_ID

DESCRIPTION

  Sets the event id in measurement report from L1 format to ASN1 format

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE if event no is incorrect
  SUCCESS if event no is correct

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_set_inter_sys_event_id
(
  l1_inter_sys_event_enum_type* l1_event_type_ptr,
  rrc_EventIDInterRAT* rrc_event_id_ptr
)
{
  WRRC_MSG1_HIGH("Inter-RAT Event Triggered Report %d[0/1/2/3 : 3A/3B/3C/3D]",*l1_event_type_ptr);

  /* Find type of event and set it */
  switch ( *l1_event_type_ptr )
  {
    case L1_INTER_RAT_EVENT_3A:
      *rrc_event_id_ptr = rrc_EventIDInterRAT_e3a;
      break;

    case L1_INTER_RAT_EVENT_3B:
      *rrc_event_id_ptr = rrc_EventIDInterRAT_e3b;

      break;

    case L1_INTER_RAT_EVENT_3C:
      *rrc_event_id_ptr = rrc_EventIDInterRAT_e3c;

      break;

    case L1_INTER_RAT_EVENT_3D:
      *rrc_event_id_ptr = rrc_EventIDInterRAT_e3d;

      break;

  default:
    /* Not Possible */
      WRRC_MSG0_ERROR("L1 sent a wrong event ID.");

    break;
  }  /* End of switch */

  return;
}


/*===========================================================================

FUNCTION   RRCIRAT_VALIDATE_EVENT_RESULTS

DESCRIPTION

  This function validates the event results sent by L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If event resuulst are valid
  SUCCESS: If event results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_validate_event_results
(
  l1_inter_sys_event_results* l1_event_results_ptr
)
{
  uint8 cell_number = 0;

  /* Validate the results */
  if (l1_event_results_ptr->num_cells == 0)
  {
    WRRC_MSG0_ERROR("L1 gave 0 cells for Inter-RAT Event Report");
    return FAILURE;
  }
  if (l1_event_results_ptr->num_cells > L1_MAX_CELL_MEAS)
  {
    WRRC_MSG2_HIGH("Set Inter-RAT cells to %d, L1 sent %d",L1_MAX_CELL_MEAS,l1_event_results_ptr->num_cells);
    l1_event_results_ptr->num_cells = L1_MAX_CELL_MEAS;
  }

  for (cell_number = 0; cell_number <l1_event_results_ptr->num_cells; cell_number++)
  {
    if (l1_event_results_ptr->
      cell[cell_number].bsic_choice == L1_BSIC_VERIFIED)
    {
      if (l1_event_results_ptr->
        cell[cell_number].u.bsic_verified.intersys_cell_id > MAX_INTER_RAT_CELL_ID)
      {
        WRRC_MSG1_ERROR("Invalid Inter RAT cell id %d",
          l1_event_results_ptr->cell[cell_number].u.
          bsic_verified.intersys_cell_id);

        return FAILURE;
      }
    }
    else
    {

    /* Set bcch ARFCN i.e. Absolute Radio Freq Channel No. GSM carrier frequency is
      designated by ARFCN */
      if (CONVERT_ARFCN_TO_NUM(l1_event_results_ptr->cell[cell_number].u.
        bsic_nonverified.bcch_arfcn) > MAX_ARFCN)
      {
        WRRC_MSG1_ERROR("Invalid ARFCN %d",
          CONVERT_ARFCN_TO_NUM(l1_event_results_ptr->cell[cell_number].u.
          bsic_nonverified.bcch_arfcn));

        return FAILURE;
      }
    }

  } /* End of for */
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_CHECK_CELL_ABSENCE_IN_CELL_LIST

DESCRIPTION

  This function checks if the cells are absent in CELL_INFO_LIST

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell is absent in CELL_INFO_LIST
  SUCCESS: If cell is present in CELL_INFO_LIST

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_cell_absence_in_cell_list
(
  void
)
{
  uint8 local_neighbor_cnt = 0;

  for (local_neighbor_cnt=0; local_neighbor_cnt < L1_MAX_CELL_MEAS; local_neighbor_cnt++)
  {
    if (inter_rat_cell_list[local_neighbor_cnt].cell_id != INVALID_CELL_ID)
    {
      return FAILURE;
    }
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_VALIDATE_MEASURED_RESULTS

DESCRIPTION

  This function validates the measured results sent by L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If measured results are valid
  SUCCESS: If measured results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_validate_measured_results
(
  l1_inter_sys_meas_result_struct_type* meas_results_ptr
)
{
  uint8 cell_number=0, gsm_cell_count=0;
  WRRC_MSG0_HIGH("Validate Inter-RAT Measured Results");
  if ((meas_results_ptr->num_msmt_results == 0) || ((meas_results_ptr->num_msmt_results > 1)))
  {
    WRRC_MSG1_ERROR("RAT Meas Results (Valid is 1) = %d",meas_results_ptr->num_msmt_results);
    return FAILURE;
  }


  if (meas_results_ptr->inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells >
    L1_MAX_REPORTED_GSM_CELLS)
  {
    WRRC_MSG2_HIGH("Setting no of GSM cells to %d, L1 sent %d",L1_MAX_REPORTED_GSM_CELLS,
      meas_results_ptr->inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells);
    meas_results_ptr->inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells =
      L1_MAX_REPORTED_GSM_CELLS;
  }

  /* Delete duplicate non verified cells if any */
  rrcirat_delete_dup_non_verified_cells(meas_results_ptr);


  for (cell_number = 0;
  cell_number < meas_results_ptr->num_msmt_results;
  cell_number++)
  {
    if (meas_results_ptr->inter_sys_msmt_results[cell_number].system != L1_GSM)
    {
      WRRC_MSG1_ERROR("Invalid Inter-RAT system = %d",
        meas_results_ptr->inter_sys_msmt_results[cell_number].system);

      return FAILURE;
    }
    /* For First RAT, read the GSM cell */
    for (gsm_cell_count = 0;
    gsm_cell_count < meas_results_ptr->inter_sys_msmt_results[cell_number].u.gsm.measured_gsm_cells;
    gsm_cell_count++)
    {
      /* Validate Cell Id */
      if (meas_results_ptr->inter_sys_msmt_results[cell_number].
        u.gsm.measured_cells[gsm_cell_count].bsic_choice == L1_BSIC_VERIFIED)
      {
        if (meas_results_ptr->inter_sys_msmt_results[cell_number].
          u.gsm.measured_cells[gsm_cell_count].u.bsic_verified.inter_sys_cell_id > MAX_INTER_RAT_CELL_ID)
        {
          WRRC_MSG1_ERROR("Invalid Cell Id[0..31] = %d",
            meas_results_ptr->inter_sys_msmt_results[cell_number].
            u.gsm.measured_cells[gsm_cell_count].u.bsic_verified.inter_sys_cell_id);
          return FAILURE;
        }
      }
      else
      {
        /* Validate ARFCN */
        if (CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[cell_number].
          u.gsm.measured_cells[gsm_cell_count].u.bsic_not_verified.bcch_arfcn) >= MAX_ARFCN)
        {
          WRRC_MSG1_ERROR("Invalid ARFCN[0..1024] = %d",
            CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[cell_number].
            u.gsm.measured_cells[gsm_cell_count].u.bsic_not_verified.bcch_arfcn));
          return FAILURE;
        }
      }

    } /* End of for loop of gsm_cell_count */

  } /* End of for of system results */

  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_CONVERT_RSSI_VALUE

DESCRIPTION

  This function converst the RSSI value sent by L1 into a format expected by UTRAN.
    Maximum converted value is 63 and minimum converted value is 0

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_convert_rssi_value
(
  int16* gsm_carrier_rssi_ptr,
  uint8* rssi_converted_value_ptr
)
{
  /* -48 .. -110 */
  int16 rssi_val_range = RSSI_VAL_LOW; /* -110 */

  uint8 utran_rssi_value = 0;

  /* To Satify Lint */
  boolean value = TRUE;

  while (value)
  {
    if ((*gsm_carrier_rssi_ptr >= rssi_val_range) &&
      (*gsm_carrier_rssi_ptr < (rssi_val_range + 1)))
    {
      /* > 110, <= 109,  rssi = 1 .... */
      *rssi_converted_value_ptr = ++utran_rssi_value;
      break;
    }

    /* Increment rssi value */
    utran_rssi_value++;

    /* Increase range by 1 */
    rssi_val_range++;


    if (rssi_val_range == (RSSI_VAL_HIGH+1)) /* -47 */
    {
      /* Now find if the value is beyond RSSI_VAL_LOW */
      if (*gsm_carrier_rssi_ptr < RSSI_VAL_LOW)
      {
        *rssi_converted_value_ptr = MIN_RSSI;
      }
      else
      {
        *rssi_converted_value_ptr = MAX_RSSI;
      }
      break;
    }

  } /* End of while */

  WRRC_MSG2_HIGH("RSSI Recvd =%d, Converted : %d", *gsm_carrier_rssi_ptr,
    *rssi_converted_value_ptr);
  return;
}


/*===========================================================================

FUNCTION  RRCIRAT_FILL_HCS_INFO

DESCRIPTION

 This function fills the overall HCS info for an Inter RAT Cell.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill overall HCS info correctly
 SUCCESS: If overall HCS info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcirat_fill_hcs_info
(
  rrc_InterRATMeasurementSysInfo* sib_inter_rat_ptr,
  l1_meas_sib_parm_struct_type* l1_sib_ptr
  ,rrc_SysInfoType11_v590ext_IEs_newInterRATCellInfoList_v590ext * rel5_ext_ptr
)
{
  rrc_SysInfoType3  *sib3_ptr = NULL;

  rrc_SysInfoType4  *sib4_ptr = NULL;

  rrc_NewInterRATCellList* inter_rat_cell_list_ptr = NULL;

  uint8 cell_count = 0;

  boolean sib4_present = FALSE;

  uint16 cell_position;
  uint32 idx=0,idy=0;

  uint16 arfcn;

  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterRATCellsList *tmp_rrc_PLMNsOfInterRATCellsList_ptr = NULL;
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  boolean sib18_present = FALSE;
uint16 prev_cell_count=0;
  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
  if( sib3_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB3 ptr is NULL");
    return( FAILURE );
  }

  if (((rrcmeas_current_substate == RRCMEAS_CELL_FACH) || (rrcmeas_current_substate == RRCMEAS_PCH)) &&
    ((sib3_ptr->sib4indicator == TRUE)))
  {
    sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB4);

    if (sib4_ptr == NULL)
    {
      WRRC_MSG0_ERROR("SIB4 ptr NULL.Use SIB3");
    }
    else
    {
      WRRC_MSG0_HIGH("Use default values from SIB4");
      sib4_present = TRUE;
    }
  }
  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;
   /* SIB18 */
  if((sib18_ptr = (rrc_SysInfoType18*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB18)) != NULL)
  {
    if(((rrcmeas_current_substate == RRCMEAS_CELL_DCH)
      || (rrcmeas_current_substate == RRCMEAS_PCH)
      || (rrcmeas_current_substate == RRCMEAS_CELL_FACH)))
    {
      /*
       1>if System Information Block type 11bis is scheduled on BCH:
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities for SIB 11bis"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Idle mode PLMN identities for SIB11bis";
       2>otherwise,
                 the UE shall use, in place of any missing list, when present, the corresponding one in the IE "Connected mode PLMN identities"; 
       2>otherwise, 
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
       
       1> else
       2>the PLMN lists to be considered are the ones included, when present, in the IE "Connected mode PLMN identities"; 
       2>otherwise,
                 the UE shall use, in place of any missing list, the corresponding one in the IE "Idle mode PLMN identities";
      */
      if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,connectedModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis,
            plmnsOfInterRATCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            connectedModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 Conn Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,idleModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
            plmnsOfInterRATCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 idle Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities,plmnsOfInterRATCellsList)) &&
        (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList.n !=0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &(sib18_ptr->connectedModePLMNIdentities.plmnsOfInterRATCellsList);
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Conn Inter RAT");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterRATCellsList)) &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Inter RAT");
        sib18_present = TRUE;
      }
    }
    else
    {
      /*
       If SIB11bis is present and SIB18 bis extenstions are present, use SIB18 bis extension
       If SIB11bis is present and SIb18 bis extensions are absent, use SIB18
       If SIB11bis is absent, use SIB18
      */
      if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,idleModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
          plmnsOfInterRATCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11bis:SIB11 processing SIB18 Idle Inter RAT from bis extension ");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities))
        &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,
        plmnsOfInterRATCellsList))
        &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterRATCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterRATCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Inter RAT ");
        sib18_present = TRUE;
      }
    }
  }
  if(rrc_ccm_get_serv_plmn_cellid(&tmp_cell_id, &selected_plmn_identity) == RRC_CELL_LOSS_NO_NEW_CELL)
  {
    sib18_present = FALSE;
  }
  else
  {
    WCDMA_MEMCPY(&tmp_plmn_identity, 
                 sizeof(rrc_plmn_identity_type),
                 &selected_plmn_identity, 
                 sizeof(rrc_plmn_identity_type));
  }
  if(rrccsp_is_ue_in_true_limited_service() == TRUE)
  {
    sib18_present = FALSE;
    WRRC_MSG0_HIGH("UE in limited service ignoring SIB18");
  }
  {
    rrc_MasterInformationBlock *mib_ptr = NULL;/* Declared to store the value of MIB*/
    if((mib_ptr = (rrc_MasterInformationBlock*)  rrc_sibdb_return_sib_for_srv_cell
      (rrc_MIB))!= NULL)
    {
      if(rrc_is_nw_shared(mib_ptr))
      {
        sib18_present = FALSE;
        WRRC_MSG0_HIGH("NWS: Ignore SIB18");
      }
      else
      {
        WRRC_MSG0_HIGH("NWS: N/W not shader so consider SIB18");
      }
    }
  }



  if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(sib_inter_rat_ptr,
    rrc_InterRATMeasurementSysInfo,interRATCellInfoList)))
  {
    WRRC_MSG0_HIGH("Inter-RAT Cell List absent in SIB11");
    if(rrcirat_read_sib11bis(l1_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter RAT in SIB11bis Ignored");
      l1_sib_ptr->inter_s.num_cell_add = 0;
      l1_sib_ptr->inter_sys_meas_info_incl = TRUE;
    }
    return SUCCESS;
  }

  /* Store ptr given by UTRAN */
  inter_rat_cell_list_ptr = &sib_inter_rat_ptr->interRATCellInfoList.newInterRATCellList;

  if ((inter_rat_cell_list_ptr == NULL)||(inter_rat_cell_list_ptr->n == 0))
  {
    WRRC_MSG0_HIGH("inter_rat_cell_list_ptr is NULL");
    if(rrcirat_read_sib11bis(l1_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterRATCellsList_ptr)== FAILURE)
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter RAT in SIB11bis Ignored");
      l1_sib_ptr->inter_s.num_cell_add = 0;
      l1_sib_ptr->inter_sys_meas_info_incl = TRUE;
    }
    return SUCCESS;
  }

  while ( (inter_rat_cell_list_ptr->n > idx) &&
    (cell_count < L1_MAX_CELL_MEAS) )
  {
    /* Let first check the type of the cell to find if we
    * really need to give this cell to L1 */
    if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
      T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
    {
      WRRC_MSG0_HIGH("Ignoring Non GSM cell");
       arfcn = MAX_ARFCN;
      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
      {
        /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
        cell_position = (uint16)
          inter_rat_cell_list_ptr->elem[idx].interRATCellID;

      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
      }
      /* Tell to get the first available cell position and

      * that would be the cell id. The called function would store
      * the cell id back in passed cell_position variable */
      if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
        return FAILURE;
      }
      idx++;
      continue;
    }
    else
    {
      l1_sib_ptr->inter_s.add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

      /* Cell reselection info is always set to TRUE. If not present in SIB 11, just
      set it to default values */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info_incl = TRUE;

      /* Initializw q-offset1-2 Defaults as per specs */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.qoffset1_s_n = QOFFSET1;

      l1_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.qoffset2_s_n = QOFFSET2;

      /* -24. Got this value from Radio Access Capability */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.max_tx_pwr = -24;

      /* Set default values for the HCS neighboring cell info if it is first cell
       * else use the info from the previous cell */
      if(cell_count == 0)
      {
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
           gsm_cell.hcs_info.hcs_prio = 0;
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.q_hcs = 110;
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
         gsm_cell.hcs_info.penalty_time = 0;
      }
      else
      {
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.hcs_prio =
          l1_sib_ptr->inter_s.add_cell[cell_count - 1].u.
          gsm_cell.hcs_info.hcs_prio;
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.q_hcs =
          l1_sib_ptr->inter_s.add_cell[cell_count - 1].u.
          gsm_cell.hcs_info.q_hcs;
        l1_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.penalty_time =
          l1_sib_ptr->inter_s.add_cell[cell_count - 1].u.
          gsm_cell.hcs_info.penalty_time;
        
        if(l1_sib_ptr->inter_s.add_cell[cell_count - 1].u.
          gsm_cell.hcs_info.penalty_time != 0)
        {
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.tmp_offset1 =
            l1_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.tmp_offset1;
        }
      }

      if ((sib4_present == TRUE) && (sib4_ptr != NULL))
      {
      /* Not valid for Inter-RAT but initialize to default
        * as they are present */
        if(RRC_CHECK_COMMON_MSG_TYPE(sib4_ptr->cellSelectReselectInfo.modeSpecificInfo,
           rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
        {
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qual_min = (int16)
            sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;

          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.rxlev_min = (int16)
            (sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
        }
        else
        {
          WRRC_MSG1_ERROR("ERR: SIB4 mode_choice NOT FDD %d", sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
        }
      }
      else
      {
        if(RRC_CHECK_COMMON_MSG_TYPE(sib3_ptr->cellSelectReselectInfo.modeSpecificInfo,
           rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
        {
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qual_min = (int16)
            sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.rxlev_min = (int16)
            (sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
        }
        else
        {
          WRRC_MSG1_ERROR("ERR: SIB3 mode_choice NOT FDD %d", sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
        }
      }

      /* Get GSM CELL Info */
      /* Check if rrc_gsm_cellSelectionReselectionInfo_present */
      if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->m.
        cellSelectionReselectionInfoPresent)
      {
        MSG_LOW("Cell Reselection info present",0,0,0);

        /* q-Offset1S-N */
     
          /* Range is -50...50 */
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset1_s_n = (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.q_Offset1S_N;
        

        /* q_Offset2S_N  -50...50 */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.m.
            q_Offset2S_NPresent)
        {
          /* Range is -50...50 */
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset2_s_n =  (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.q_Offset2S_N;
        }

        /* Max Txit Power */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.m.
          maxAllowedUL_TX_PowerPresent)
        {
          l1_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.max_tx_pwr = (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.maxAllowedUL_TX_Power;
        }

        /* Check if gsm mode present  */

        /* Check if rxlev_min present */

        if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
           cellSelectionReselectionInfo.modeSpecificInfo.t == 
             T_rrc_CellSelectReselectInfoSIB_11_12_modeSpecificInfo_gsm)
        {
          if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.modeSpecificInfo.u
            .gsm->m.q_RxlevMinPresent)
          {
            /* Store rxlev_min in L1 */
            l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min = (int16)
              ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.modeSpecificInfo.u.gsm->q_RxlevMin * 2) + 1);
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Incorrect mode specific info for GSM entry in SIB11");
        }

          /*l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
            delta_qrxlevmin_included = FALSE;  */
        if(rel5_ext_ptr != NULL)
        {
          if(RRC_MSG_LIST_BITMASK_IE_TYPE2(rel5_ext_ptr,deltaQrxlevmin))
          {
            /*l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
              delta_qrxlevmin_included = TRUE;
            l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.
              delta_qrxlevmin = rel5_ext_ptr->value.deltaQrxlevmin * 2; */
            MSG_HIGH("Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
              (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2),
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
              CONVERT_ARFCN_TO_NUM(l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));

              /*if (l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +
                  (rel5_ext_ptr->value.deltaQrxlevmin * 2) < -115)
              {*/
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +=
                (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2);
            /*}*/
          }
        }
        /* HCS inforamtion */
        if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
          cellSelectionReselectionInfo.m.hcs_NeighbouringCellInformation_RSCPPresent)
        {
          /* HCS PRIO*/
         
            l1_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.hcs_info.hcs_prio = (uint16)
              inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_PRIO;
          
          /*Q HCS */
         
            if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS < 90)
            {
              l1_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.hcs_info.q_hcs = (uint16) (110 -
                inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS);
            }
            else
            {
              l1_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.hcs_info.q_hcs = 37;
            }
          
          /* Delta Qhcs is not needed if it is not mapped from CPICH-RSCP or PCCPCH-RSCP.
          * For inter rat cells, Qhcs is always mapped from RSSI
          */
          
          /* PENALTY TIME*/
          if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
            .penaltyTime.t != T_rrc_PenaltyTime_RSCP_notUsed)
          {
            
            switch(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
              .penaltyTime.t)
            {
            case T_rrc_PenaltyTime_RSCP_pt10:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 10;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt10);
              break;
              
            case T_rrc_PenaltyTime_RSCP_pt20:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 20;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt20);
              break;
              
            case T_rrc_PenaltyTime_RSCP_pt30:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 30;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt30);
              break;
              
            case T_rrc_PenaltyTime_RSCP_pt40:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 40;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt40);
              break;
              
            case T_rrc_PenaltyTime_RSCP_pt50:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 50;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt50);
              break;
              
            case T_rrc_PenaltyTime_RSCP_pt60:
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 60;
              l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.u.pt60);
              break;
              
            default:  /* should not happen */
              break;
            }
          }
        }
      } /* rrc_gsm_cellSelectionReselectionInfo_present */

      /* Cell Individual Offset */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
        inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->interRATCellIndividualOffset;

      /* Get BAND 1800 DCS or 1900 PCS */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
        RET_INTER_RAT_BAND(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->frequency_band);

      /* Get bcch_ARFCN */
      CONVERT_ARFCN_TO_NUM(l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) =
        (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bcch_ARFCN;
      l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.max_tx_pwr = 
        (int16)rr_get_maximum_power_output_for_frequency(l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn);
      WRRC_MSG1_HIGH("MAX_TX_POWR = %d",l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.max_tx_pwr);

      arfcn = (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bcch_ARFCN;

#ifdef FEATURE_GSM_QUADBAND

      if ((converted_gsm_band = rrc_get_gsm_band(l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info)) !=
        INVALID_BAND)
      {
        l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
          converted_gsm_band;
      }
      else
      {
        WRRC_MSG1_HIGH("ARFCN %d ignored bec of invalid band",inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN);
        arfcn = MAX_ARFCN;
        if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
        {
          /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
          cell_position = (uint16)
            inter_rat_cell_list_ptr->elem[idx].interRATCellID;

        }
        else
        {
          /* Store the cell in first order position */
          cell_position = AVAILABLE_CELL_POSITION;
        }
        /* Tell to get the first available cell position and

        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
        if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
        {
          WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
          return FAILURE;
        }
        idx++;
        continue;
      }

#endif /* FEATURE_GSM_QUADBAND */

      /* To add BSIC (Base Station Identification Code) */
      if ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
        > NCC_MAX) ||
        (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
        > BCC_MAX))
      {
        WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
          inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
          inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

        return FAILURE;

      } /* End of if of checking NCC and BCC validity */

      /* Store good values of bcc and ncc */
      l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc =
        (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bsic.ncc;

      l1_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc =
        (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bsic.bcc;
    } /* End of gsm cell  */

    /* Time to get its cell id*/
    /* Cell Order has already been verified */

    if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
    {
      /* WRRC_MSG0_HIGH("Inter-RAT Cell Id present"); */
      cell_position = (uint16)
        inter_rat_cell_list_ptr->elem[idx].interRATCellID;

    }
    else
    {
      /* Store the cell in first order position */
      cell_position = AVAILABLE_CELL_POSITION;
    }
    /* Tell to get the first available cell position and

    * that would be the cell id. The called function would store
    * the cell id back in passed cell_position variable */
    if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
    {
      WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
      return FAILURE;
    }
    l1_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id
      = cell_position;

    /* set the measure in PCH IDLE and FACH flag to TRUE */
    l1_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
    /* Increment cell count */

       if(sib18_present)
       {
         if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->n > idy)
         {
           if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[idy].m.plmn_IdentityPresent)
           {
             rrc_translate_gsm_map_plmn_id(&tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[idy].plmn_Identity,
               &tmp_plmn_identity);
             if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
             {
               if((rrccsp_check_for_eplmn(
                  mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                    tmp_plmn_identity))) == FALSE)
               {
                 WRRC_MSG1_HIGH("cell id %d Non eq PLMN measure only in DCH",
                   l1_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
                 /* set the flag to FALSE */
                 l1_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = FALSE;
               }
             }
           }
           else
           {
             /* use the previous value of the flag if PLMN id is not selected PLMN */
             if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
             {
               /* Add code to set the flag meas_pch_fach_idle to the flag value of the previous cell */
               if(cell_count > 0) /* to satisfy lint */
               {
                 l1_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach =
                   l1_sib_ptr->inter_s.add_cell[cell_count - 1].measure_in_idle_pch_fach;
               }
             }
           }

           idy++;
         }
         else
         {
           /* PLMN list is shorter than the cell info list set flag to true for the remaining cells */
           WRRC_MSG1_HIGH("PLMN list < Cell list. cell id %d measured in IDLE,FACH,PCH,DCH",
           l1_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
         }
       }
       else
       {
         WRRC_MSG1_HIGH("cell id %d measured in IDLE,FACH,PCH,DCH",
           l1_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
       }
    cell_count++;

    /* Increment the pointer */
    idx++;
    if ((inter_rat_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
    {
      WRRC_MSG0_HIGH("Inter-RAT Cell List full");
      break;
    }

  } /* End of while inter_rat_cell_list_ptr */

  /* Finally Set the cell count */
  l1_sib_ptr->inter_s.num_cell_add = cell_count;

  /* Now set inter sys info incl info to TRUE */
  l1_sib_ptr->inter_sys_meas_info_incl = TRUE;
  
 prev_cell_count = l1_sib_ptr->inter_s.num_cell_add;
 if(rrcirat_read_sib11bis(l1_sib_ptr, sib18_present, tmp_rrc_PLMNsOfInterRATCellsList_ptr) == FAILURE)
 {
   WRRC_MSG0_HIGH("SIB11bis: Reading Sib11bis FAILURE");
   l1_sib_ptr->inter_s.num_cell_add = prev_cell_count;
 }

  return SUCCESS;
}



/*===========================================================================

FUNCTION  RRCIRAT_DELETE_DUP_NON_VERIFIED_CELLS

DESCRIPTION

 This function deletes duplicate non verified cells from Meas Report.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcirat_delete_dup_non_verified_cells
(
 l1_inter_sys_meas_result_struct_type* meas_results_ptr
)
{
  uint8 count = 0, tmp_list_count = 0; /* Count variable */

  if ((meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells == 0) ||
    (meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells == 1))
  {
    WRRC_MSG0_HIGH("Dupl non-verf BSIC purging not done");
    return;
  }

  for (count = 0; count < meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells; count++)
  {
    WCDMA_MEMCPY(&tmp_measured_cells[count],
                 sizeof(l1_measured_cells_struct_type),
                 &meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count],
                 sizeof(l1_measured_cells_struct_type));
  }

  for (count = 0; count < meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells -1; count++)
  {
    for (tmp_list_count = count+1; tmp_list_count < meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells &&
            tmp_list_count < L1_MAX_REPORTED_GSM_CELLS; tmp_list_count++)
    {
      /* Now compare the cells */
      if (meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count].bsic_choice == L1_BSIC_VERIFIED)
      {
        /* Come out of for loop for tmp_list_count. Read the next cell */
        break;
      }
      else /* BSIC NOT verified case */
      {
        if (CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count].u.bsic_not_verified.bcch_arfcn) == RRC_INVALID_ARFCN_FLAG)
        {
          /* Come out of for loop for tmp_list_count. Read the next cell */
          break;
        }
        /* Check this cell with all celss in tmp_measured_cells */
        if (tmp_measured_cells[tmp_list_count].bsic_choice == L1_BSIC_VERIFIED)
        {
          /* Check with next cell. It could be BSIC non verified */
          continue;
        }
        else /* BSIC NOT verified case */
        {
          if (CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count].u.bsic_not_verified.bcch_arfcn)
            == RRC_INVALID_ARFCN_FLAG)
          {
            /* Check with next cell. It could be BSIC non verified */
            continue;
          }
          if (CONVERT_ARFCN_TO_NUM(tmp_measured_cells[tmp_list_count].u.bsic_not_verified.bcch_arfcn) ==
            CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count].u.bsic_not_verified.bcch_arfcn))
          {
            MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
                  "CellCnt %d == CellCnt %d ARF %d. Ignoring cell %d ARGFCN %d",
                  count, tmp_list_count,
                  CONVERT_ARFCN_TO_NUM(meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count].u.bsic_not_verified.bcch_arfcn),
                  tmp_list_count, CONVERT_ARFCN_TO_NUM(tmp_measured_cells[tmp_list_count].u.bsic_not_verified.bcch_arfcn));

            /* Mark ARFCN as invalid so that it could be deleted later */
            CONVERT_ARFCN_TO_NUM(tmp_measured_cells[tmp_list_count].u.bsic_not_verified.bcch_arfcn) = RRC_INVALID_ARFCN_FLAG;

            /* Go to end of inner for loop */
          }
          else
          {
            MSG_HIGH("CellCnt %d != CellCnt %d ARF %d", count, tmp_list_count,
              CONVERT_ARFCN_TO_NUM(tmp_measured_cells[tmp_list_count].u.bsic_not_verified.bcch_arfcn));

            /* Good this cell does not match. Go to end of inner for loop */
          }
        } /* Else of BSIC not verified case for inner loop */

      } /* Else of BSIC not verified case for outer loop */
    } /* End of for of tmp_list_count */

  } /* End of for of count */

  /* Now list has already been updated. tmp_measured_cells contains the deleted cells */
  /* Now copy update the original list */
  count = 0;
  for (tmp_list_count = 0;
  tmp_list_count < meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells; tmp_list_count++)
  {
    if (tmp_measured_cells[tmp_list_count].bsic_choice == L1_BSIC_VERIFIED)
    {
      /* No problem Just copy back */
      WCDMA_MEMCPY(&meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count++],
                   sizeof(l1_measured_cells_struct_type),
                   &tmp_measured_cells[tmp_list_count],
                   sizeof(l1_measured_cells_struct_type));
    }
    else
    {
      if (CONVERT_ARFCN_TO_NUM(tmp_measured_cells[tmp_list_count].u.bsic_not_verified.bcch_arfcn) == RRC_INVALID_ARFCN_FLAG)
      {
        /* Do not copy this ARFCN */
      }
      else
      {
        WCDMA_MEMCPY(&meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_cells[count++],
                     sizeof(l1_measured_cells_struct_type),
                     &tmp_measured_cells[tmp_list_count],
                     sizeof(l1_measured_cells_struct_type));
      }
    }

  }

  MSG_HIGH("L1 specifed %d cells. After purge dups, RRC has %d cells. New RRC cnt is %d", meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells,count,count);

  /* Set the new cell cnt */
  meas_results_ptr->inter_sys_msmt_results[0].u.gsm.measured_gsm_cells = count;
}


/*===========================================================================
FUNCTION     rrc_is_gsm_cell_present_in_sibs


DESCRIPTION
  This function returns TRUE if GSM cell are present in SIB11 else
  return FALSE. Called only by L1 in its context.


DEPENDENCIES
  None


RETURN VALUE
  True/False


SIDE EFFECTS

===========================================================================*/
boolean rrc_is_gsm_cell_present_in_sibs(void)
{
  rrc_SysInfoType11 *sib11_ptr = NULL; /* Declared to store the value of SIB 11*/

  rrc_NewInterRATCellList *inter_rat_cell_list_ptr;

  rrc_NewInterRATCellList_B* inter_rat_non_hcs_cell_list_ptr = NULL;
  uint32 idx=0;
  if (rrcmcm_is_dualmode_enabled() == FALSE)
  {
    WRRC_MSG0_HIGH("IRAT:Dual Mode not enabled");
    return FALSE;
  }

  sib11_ptr = (rrc_SysInfoType11*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB11);

  if ( sib11_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB 11 from SIB DB is NULL");
    return FALSE;
  }

  /* Check if HCS is in use. */
  if ( sib11_ptr->measurementControlSysInfo.use_of_HCS.t ==
       T_rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used )
  {
    if (!((sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.
        interRATMeasurementSysInfoPresent) &&
      (sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->interRATMeasurementSysInfo.m.
        interRATCellInfoListPresent) &&
      (sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList.n != 0)))
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB11");
      return FALSE;
    }

    /* Store ptr given by UTRAN */
    inter_rat_cell_list_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList;


    while (inter_rat_cell_list_ptr->n > idx)
    {
    /* Let first check the type of the cell to find if we
      * really need to give this cell to L1 */
      if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t ==
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("GSM cell present");
        return TRUE;
      }
      idx++;
    }
    return FALSE;

  }
  else
  {
    if (!((sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.
        interRATMeasurementSysInfoPresent) &&
      (sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->interRATMeasurementSysInfo.m.
       interRATCellInfoListPresent) &&
      (sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList.n != 0)))
    {
      WRRC_MSG0_HIGH("Inter-RAT list absent in SIB11");
      return FALSE;
    }

    /* Store ptr given by UTRAN */
    inter_rat_non_hcs_cell_list_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      interRATMeasurementSysInfo.interRATCellInfoList.newInterRATCellList;

    if (inter_rat_non_hcs_cell_list_ptr == NULL)
    {
      WRRC_MSG0_HIGH("inter_rat_cell_list_ptr is NULL");
      return FALSE;
    }
    idx=0;
    while (inter_rat_non_hcs_cell_list_ptr->n > idx)
    {
    /* Let first check the type of the cell to find if we
      * really need to give this cell to L1 */
      if (inter_rat_non_hcs_cell_list_ptr->elem[idx].technologySpecificInfo.t ==
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("GSM cell present");
        return TRUE;
      }
      idx++;
    }
    return FALSE;
  }
}

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R5

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r5
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  boolean cell_info_ind_present = FALSE;
  uint8 cell_info_ind = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrcmeas_status_e_type result;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  rrc_InterRATMeasurement_r4 *irat_meas_cmd_ptr = NULL;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4;



  if (!(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_release))
  {
    if((dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.u.r4.m.
        v4d0NonCriticalExtensionsPresent) &&
          (dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.m.
          v590NonCriticalExtensionsPresent) &&
          (dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.v590NonCriticalExtensions.m.
          v5b0NonCriticalExtensionsPresent) &&

          (dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementControl_v5b0ext.m.
          interRATCellInfoIndicationPresent))
    {
      WRRC_MSG0_HIGH("irat cell info ind present");
      cell_info_ind_present=TRUE;
      cell_info_ind = dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementControl_v5b0ext.interRATCellInfoIndication;
    }

    else
    {
      WRRC_MSG0_HIGH("irat cell info ind not present");
      cell_info_ind_present=FALSE;
    }

  }

  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;

  /* The following is not supported in Rel 99 anymore. So set it to FALSE */
  l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;

 

  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = transaction_id;
  l1_inter_rat_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_setup)
  {
    /* Double check that it is INTER-RAT Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r4_interRATMeasurement)
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */

    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r4,interRATMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r4,interRATReportingQuantity)) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.t == T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
      rrc_MeasurementQuantityGSM_dummy) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo.t ==
      T_rrc_InterRATReportingQuantity_ratSpecificInfo_gsm) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.t
      == T_rrc_InterRATReportCriteria_interRATReportingCriteria) &&
      (!(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->m.interRATEventListPresent) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    
    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
	  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Set Meas Identity */
    l1_inter_rat_meas_params->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;

    /* Set Meas Quantity as Inter-sys Meas Quan present */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

    /* Initialize Default Value to intra_freq_meas_quan_incl */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if(RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
        modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Get Filter Coefficient too */
      /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
     
        /* Store filter coefficient */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
          interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
      
    } /* End of quality estimate present */

    /* Set System Choice to GSM */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

    /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

    /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
    So set it to RSSI */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
      L1_GSM_CARRIER_RSSI;

    
      /* GSM Filter Coefficient present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);
    

    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
      RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

    /* Store Reporting Quantity */
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

    /* Sys Choice */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

    /* L1 wants this to be FALSE for the first version as it does not support it */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

    /* Ignore reading Observed time difference as no longer supported in Rel 99
    l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
    rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
    ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

    /* Check if gsm_carrier_rssi wanted in Meas Report */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

    if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
    {
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }


    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    /* For L1 sake, copy reporting criteria */
    l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

    /* Now set other values */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;

    l1_inter_rat_meas_params->meas_object_info_included = TRUE;


    /* Time to process the cell list */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r4,interRATCellInfoList))
    {
      result = rrcirat_process_cell_list_mcm_r5(&rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATCellInfoList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
      if(result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
       rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else if(result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("Unable to fill InterRAT MCM");
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
         rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      /* Set Cell list is present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;

        }
      }
    } /* End of if of Additional Meas Present */

    if( RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r4,interRATCellInfoList))
    {
      /* At this point everything has been validated and now is the time
      to update the inter rat CELL_INFO_LIST */

      for (count=0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_rat_cell_list[count],
                     sizeof(inter_rat_cell_list_struct_type),
                     &local_neighbor_list[count],
                     sizeof(inter_rat_cell_list_struct_type));
      }
    }

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_modify)
  {
    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

      l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r4_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      {
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }
    }
    else
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_r4_interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            ((irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) ||
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
        interRATMeasurement,rrc_InterRATMeasurement_r4,interRATMeasQuantity)))
      {
        WRRC_MSG0_HIGH("Meas Quan absent");
      }
      else
      {
        /* Set Meas Quantity to TRUE */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
        if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
          rrc_MeasurementQuantityGSM_dummy)
        {

          WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");

          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }

        /* Meas Quantity Parameter 1 i.e. Quality Estimate */
        if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity,
          rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
        {
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

          /* Meas Quan i.e ECI0/RSCP/RSSI... */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
            u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
            modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

          /* Get Filter Coefficient too */
          
            /* Store filter coefficient */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
              .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              measQuantityUTRAN_QualityEstimate.filterCoefficient);
          
        }

        /* Set System Choice to GSM */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

        /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

        /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
        So set it to RSSI */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
          L1_GSM_CARRIER_RSSI;

        
          /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
            RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
            ratSpecificInfo.u.gsm->filterCoefficient);
        

        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
          RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      } /* End of else of Meas Quan present */

      /* Check if reporting quan present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r4,interRATReportingQuantity))
      {
        /* Store Reporting Quantity */
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

        /* Sys Choice */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

        /* L1 wants this to be FALSE for the first version as it does not support it */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

        /* Observed time difference no longer supported in Rel 99
        l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

          /* Check if gsm_carrier_rssi wanted in Meas Report */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
      }

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.interRATMeasurement->reportCriteria,
        &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
      
      l1_inter_rat_meas_params->meas_object_info_included = TRUE;
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r4_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r4,interRATCellInfoList)) )
      {
        /* Time to process the cell list */
        result = rrcirat_process_cell_list_mcm_r5(&rrc_mcm_ptr->measurementCommand.
          u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);

          if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
          rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                  (uint8) RRC_PROCEDURE_MCMR,
                                  (uint8) rrcmeas_mcf.error_choice,
                                  (uint8) rrcmeas_mcf.error_cause
                                );
          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else if(result == RRCMEAS_UNSUPPORTED)
        {
          /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("Unable to fill InterRAT MCM");
          /* Send Meas Ctrl Failure Msg */
  
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }

      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      {
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type,&rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r4_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r4,interRATCellInfoList)) )
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;

#ifdef FEATURE_GAN_3G
        #error code not present
#endif

        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */



  if(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_setup)
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
       rrc_InterRATMeasurement_r4,interRATCellInfoList)))
    {
      if(cell_info_ind_present==TRUE)
      {
        interRAT_cell_info_status.interRAT_cell_info_present=TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind=cell_info_ind;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_modify)
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r4_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
       rrc_InterRATMeasurement_r4,interRATCellInfoList)))
    {
      if(cell_info_ind_present==TRUE)
      {
        interRAT_cell_info_status.interRAT_cell_info_present=TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind=cell_info_ind;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }


  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;
  WRRC_MSG0_MED("MCM:Sending cmd to L1");
  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_CELL_LIST_MCM_R5

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm_r5
(
  rrc_InterRATCellInfoList_r4* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
)
{
/* RRC new cell info list and l1 new cell info list would have the new cell info list
through local neighbour list. This has to be done to make sure that if the Meas Ctrl
  Msg contains bad cells, our good inter_rat_cell_list does not get disturbed */

  uint8 cnt = 0;

  uint8 cell_count = 0;

  uint32 cell_position = 0,idx=0;

  uint16 cell_id = INVALID_CELL_ID;

  /* For storing the new cell list */
  rrc_NewInterRATCellList* local_rrc_new_cell_list_ptr = NULL;

  rrc_CellsForInterRATMeasList* local_cells_for_meas_ptr = NULL;
  boolean valid_cell_id_found =FALSE;
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif

  /* Initializes the local neighbor list to default values */
  rrcirat_initialize_local_neighbor_list();
  
  /* Copy Inter-RAT CELL_INFO_LIST list to local neighbor list. This is necessary
  so as to make sure that in case UTRAN messes up, our old cell list is intact */

  for (cnt=0; cnt < L1_MAX_CELL_MEAS; cnt++)
  {
    WCDMA_MEMCPY(&local_neighbor_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type),
                 &inter_rat_cell_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type));
  }

  /* In Inter-RAT meas, remove is not optional */
  /* For sanity, initialize to 0 */
  l1_cell_list_ptr->num_cell_rmv = 0;

  if (rrcirat_fill_removed_inter_rat_cells(&mcm_cell_list_ptr->removedInterRATCellList,
    l1_cell_list_ptr) == FAILURE)
  {
    WRRC_MSG0_HIGH("Invalid Removed Inter-RAT cells");
    return RRCMEAS_FAIL;
  }

  /* UTRAN can ask us to remove all cells when it is possible that for that
  * measurement (First time SETUP) we may not have any cell list.
  * To avoid L1 getting confused, let us make number of cells to be
  * removed to 0 in case the cell list does not exist previously.
  * So check in the CELL_INFO_LIST */

  if ((mcm_cell_list_ptr->removedInterRATCellList.t ==
    T_rrc_RemovedInterRATCellList_removeAllInterRATCells) &&
    (rrcirat_check_cell_absence_in_cell_list() == SUCCESS))
  {
    /* Set No of Cells removed to 0 from oxff */
    WRRC_MSG0_HIGH("Setting No of cells removed to 0");
    l1_cell_list_ptr->num_cell_rmv = 0;
  }


  /* At this time, local_neighbor_list is completely updated and has
  * the latest and the greatest information
  * Get New cells to be added. UTRAN can just set it to NULL */
  if ( !(RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,
    rrc_InterRATCellInfoList_r4,newInterRATCellList))
    || ((RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,rrc_InterRATCellInfoList_r4,
    newInterRATCellList))
    && (mcm_cell_list_ptr->newInterRATCellList.n == 0)))
  {
    WRRC_MSG0_HIGH("MCM:No new cells to be added");
    /* Put this info in L1 list */
    l1_cell_list_ptr->num_cell_add = 0;
  }
  else
  {
    if (rrcirat_check_inter_rat_cell_order_mcm(&mcm_cell_list_ptr->newInterRATCellList) == FAILURE)
    {
      WRRC_MSG0_HIGH("Sending Meas Ctrl Failure");
      return RRCMEAS_FAIL;
    }
    /* New cells need to be added */
    local_rrc_new_cell_list_ptr =  &mcm_cell_list_ptr->newInterRATCellList;

    /* Before verifying the cell id, make sure that cell params are correct */
    idx=0;
    while ((local_rrc_new_cell_list_ptr ->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
    {
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
    /* Let first check the type of the cell to find if we
      really need to give this cell to L1 */
      if (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("MCM:Ignoring Non GSM cell");
        idx++;
        continue;
      }
      else
      {
        l1_cell_list_ptr->add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

        /* Get GSM CELL Info */

        /* No need to read cell selec/reselect info as that is for SIB11/12 */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.cell_sel_info_incl = FALSE;

        /* Cell Individual Offset */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->interRATCellIndividualOffset;

        /* Get BAND 1800 DCS or 1900 PCS */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
          RET_INTER_RAT_BAND(local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->frequency_band);

        /* Get bcch_ARFCN */
        CONVERT_ARFCN_TO_NUM(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN;

#ifdef FEATURE_GAN_3G
        #error code not present
#endif

#ifdef FEATURE_GSM_QUADBAND
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        {
          if ((converted_gsm_band = rrc_get_gsm_band(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info)) !=
            INVALID_BAND)
          {
            l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
              converted_gsm_band;
          }
          else
          {
            WRRC_MSG1_HIGH("ARFCN %d rejected bec of invalid band",local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
              u.gsm->bcch_ARFCN);
            idx++;
            return RRCMEAS_FAIL;
          }
        }

#endif /* FEATURE_GSM_QUADBAND */

        /* To add BSIC (Base Station Identification Code) */
        if ((local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
          > NCC_MAX) ||
          (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
          > BCC_MAX))
        {
          WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

          return RRCMEAS_FAIL;

        } /* End of if of checking NCC and BCC validity */

        /* Store good values of bcc and ncc */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.ncc;

        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.bcc;
      } /* End of gsm cell  */

      /* Time to get its cell id*/
      /* Cell Order has already been verified */

      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(local_rrc_new_cell_list_ptr,interRATCellID))
      {
        WRRC_MSG0_MED("MCM:Inter-RAT Cell Id present");
        cell_position = local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
        l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
      }


      /* Now store the cell id in local neighbour rat cell list */
      if (rrcirat_update_inter_rat_cell_list_mcm(&cell_position,
        &l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id, &cell_id)
        == SUCCESS)
      {
        /* set this flag to TRUE always for MCM */
        l1_cell_list_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }
      else
      {
        WRRC_MSG0_ERROR("MCM:Invalid Inter-RAT cell list");
        return RRCMEAS_FAIL;
      }

      /* Increment the pointer */
      /* Read Next One */
      idx++;


      if ((local_rrc_new_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List full");
        break;
      }

    } /* End of while inter_rat_cell_list_ptr */

    /* Finally Set the cell count */
    l1_cell_list_ptr->num_cell_add = cell_count;

  } /* End of else of new cells added */

  cell_count = 0;

  /* Check for cells for meas */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,
    rrc_InterRATCellInfoList_r4,cellsForInterRATMeasList))
  {
    WRRC_MSG0_HIGH("Cells for meas present");
    l1_cell_list_ptr->cells_for_meas_incl = TRUE;

    local_cells_for_meas_ptr = &mcm_cell_list_ptr->cellsForInterRATMeasList;
    idx=0;
    while (local_cells_for_meas_ptr->n > idx)
    {
      cell_id = (uint16) local_cells_for_meas_ptr->elem[idx];

      /* Check if the cell id is present */
      if( rrcirat_find_cell_id(&cell_id) == FAILURE)
      {
        WRRC_MSG1_ERROR("cell to measure with id = %d not found",
          cell_id);
      } /* End of if for cell id not found */
      else
      {
      /* Store cell id */
        valid_cell_id_found = TRUE;
      l1_cell_list_ptr->meas_cells_list[cell_count++] = cell_id;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
      /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");

        return RRCMEAS_UNSUPPORTED;
    }
    l1_cell_list_ptr->num_cell_meas =  cell_count;

  } /* End of if of rrc_cellsForIntraFreqMeasList_present */

  return RRCMEAS_SUCCESS;
}


/*===========================================================================

FUNCTION   rrcirat_add_irat_cell_info_ind

DESCRIPTION

  This function set the irat cell info ind in meas report.


DEPENDENCIES

  None

RETURN VALUE

  none

SIDE EFFECTS

  None

===========================================================================*/


void rrcirat_add_irat_cell_info_ind(rrc_MeasurementReport* rpt_ptr)
{
  if(interRAT_cell_info_status.interRAT_cell_info_present==TRUE)
  {
    if(rpt_ptr->m.v390nonCriticalExtensionsPresent)
    {
      if(rpt_ptr->v390nonCriticalExtensions.m.laterNonCriticalExtensionsPresent)
      {
        if(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)
        {
          if(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent)
          {
            if(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
               v5b0NonCriticalExtensionsPresent)
            {
              if(!(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.m.
               v690NonCriticalExtensionsPresent))
              {
                RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);
              }
              RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
              rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.
                interRATCellInfoIndicationPresent =1;
              rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
                  interRAT_cell_info_status.interRAT_cell_info_ind ;
            }
            else
            {
              RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
              rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
                v5b0NonCriticalExtensionsPresent =1;

              RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

              RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
              rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.
               interRATCellInfoIndicationPresent = 1;

              rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
                interRAT_cell_info_status.interRAT_cell_info_ind ;
            }
            WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);			
          }
          else
          {
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;
           // rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext.bit_mask = 0;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
              v5b0NonCriticalExtensionsPresent = 1;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext,
               interRATCellInfoIndication) ;

            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
              interRAT_cell_info_status.interRAT_cell_info_ind ;
            WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);
          }
        }
        else
        {
          if(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.measurementReport_r3_add_extPresent)
          {
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent=1 ;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent=1 ;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5b0NonCriticalExtensionsPresent=1 ;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.interRATCellInfoIndicationPresent=1 ;

            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
              interRAT_cell_info_status.interRAT_cell_info_ind ;
          }
          else
          {
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent=1 ;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1 ;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5b0NonCriticalExtensionsPresent =1;
            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.
               interRATCellInfoIndicationPresent =1;

            rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
              interRAT_cell_info_status.interRAT_cell_info_ind ;
          }
          WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);		  
        }
      }
      else
      {
        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions);
        rpt_ptr->v390nonCriticalExtensions.m.laterNonCriticalExtensionsPresent=1;
       // rpt_ptr->v390nonCriticalExtensions.measurementReport_v390ext.bit_mask = 0;
        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions);
        rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent=1 ;

        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);
        rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent=1 ;
        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext);

        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
        rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5b0NonCriticalExtensionsPresent=1 ;
        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext);

        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
        rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.
          interRATCellInfoIndicationPresent =1;

        rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
          interRAT_cell_info_status.interRAT_cell_info_ind ;
        WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);
      }

    }
    else
    {
      rpt_ptr->m.v390nonCriticalExtensionsPresent =1;

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions);
      rpt_ptr->v390nonCriticalExtensions.m.laterNonCriticalExtensionsPresent =1;
      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.measurementReport_v390ext);

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions);
      rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent =1;

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions);
      rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent=1 ;
      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.measurementReport_v4b0ext);

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);
      rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5b0NonCriticalExtensionsPresent =1;
      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext);

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext);
      rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.m.
          interRATCellInfoIndicationPresent =1;

      rpt_ptr->v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5b0NonCriticalExtensions.measurementReport_v5b0ext.interRATCellInfoIndication =
          interRAT_cell_info_status.interRAT_cell_info_ind ;
      WRRC_MSG1_HIGH("irat cell info ind added in meas report %d", interRAT_cell_info_status.interRAT_cell_info_ind);
    }

  }
}


/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R6

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r6
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  rrc_InterRATMeasurement_r6* irat_meas_cmd_ptr = NULL;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6;

  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;

  /* The following is not supported in Rel 99 anymore. So set it to FALSE */
  l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;

  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = transaction_id;
  l1_inter_rat_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r6_setup))
  {
    /* Double check that it is INTER-RAT Meas */
    if (!RRC_CHECK_COMMON_MSG_TYPE_PTR(rrc_mcm_ptr->measurementCommand.u.setup,
        rrc_MeasurementType_r6_interRATMeasurement))
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */

    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATReportingQuantity)) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.t == T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
      rrc_MeasurementQuantityGSM_dummy) &&
      (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo,
      rrc_InterRATReportingQuantity_ratSpecificInfo_gsm)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria,
      rrc_InterRATReportCriteria,interRATReportingCriteria)) &&
      (!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria,interRATEventList)) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Set Meas Identity */
    l1_inter_rat_meas_params->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;

    /* Set Meas Quantity as Inter-sys Meas Quan present */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

    /* Initialize Default Value to intra_freq_meas_quan_incl */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
        modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Get Filter Coefficient too */
      /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
     
        /* Store filter coefficient */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.
          interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
     
    } /* End of quality estimate present */

    /* Set System Choice to GSM */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

    /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

    /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
    So set it to RSSI */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
      L1_GSM_CARRIER_RSSI;

  
      /* GSM Filter Coefficient present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);
    

    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
      RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

    /* Store Reporting Quantity */
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

    /* Sys Choice */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

    /* L1 wants this to be FALSE for the first version as it does not support it */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

    /* Ignore reading Observed time difference as no longer supported in Rel 99
    l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
    rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
    ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

    /* Check if gsm_carrier_rssi wanted in Meas Report */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

    if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interRATMeasurement->reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s,
      &mcm_rpt_crit, transaction_id))
    {
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    /* For L1 sake, copy reporting criteria */
    l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

    /* Now set other values */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;

    l1_inter_rat_meas_params->meas_object_info_included = TRUE;


    /* Time to process the cell list */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATCellInfoList))
    {
      result = rrcirat_process_cell_list_mcm_r6(&rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATCellInfoList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
      if(result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
       rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else if(result == RRCMEAS_UNSUPPORTED)
      {
         WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
         return FAILURE;
      }
      /* Set Cell list is present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;

        }
      }
    } /* End of if of Additional Meas Present */

    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATCellInfoList))
    {
      /* At this point everything has been validated and now is the time
      to update the inter rat CELL_INFO_LIST */

      for (count=0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_rat_cell_list[count],
                     sizeof(inter_rat_cell_list_struct_type),
                     &local_neighbor_list[count],
                     sizeof(inter_rat_cell_list_struct_type));
      }
    }

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r6_modify))
  {
    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

      l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r6_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
      {
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }
    }
    else
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_r6_interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      
      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            ((irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) ||
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
        interRATMeasurement,rrc_InterRATMeasurement_r6,interRATMeasQuantity)))
      {
        WRRC_MSG0_HIGH("Meas Quan absent");
      }
      else
      {
        /* Set Meas Quantity to TRUE */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
        if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
          rrc_MeasurementQuantityGSM_dummy)
        {

          WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");

          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }

        /* Meas Quantity Parameter 1 i.e. Quality Estimate */
        if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
           interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
        {
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

          /* Meas Quan i.e ECI0/RSCP/RSSI... */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
            u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
            modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

          /* Get Filter Coefficient too */
          /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
         
            /* Store filter coefficient */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
              .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              measQuantityUTRAN_QualityEstimate.filterCoefficient);
         
        }

        /* Set System Choice to GSM */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

        /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

        /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
        So set it to RSSI */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
          L1_GSM_CARRIER_RSSI;

      
          /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
            RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
            ratSpecificInfo.u.gsm->filterCoefficient);
        

        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
          RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      } /* End of else of Meas Quan present */

      /* Check if reporting quan present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATReportingQuantity))
      {
        /* Store Reporting Quantity */
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

        /* Sys Choice */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

        /* L1 wants this to be FALSE for the first version as it does not support it */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

        /* Observed time difference no longer supported in Rel 99
        l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

          /* Check if gsm_carrier_rssi wanted in Meas Report */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
      }

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.interRATMeasurement->reportCriteria,
        &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
      
      l1_inter_rat_meas_params->meas_object_info_included = TRUE;
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r6_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATCellInfoList)) )
      {
        /* Time to process the cell list */
        result = rrcirat_process_cell_list_mcm_r6(&rrc_mcm_ptr->measurementCommand.
          u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
        if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else if(result == RRCMEAS_UNSUPPORTED)
        {
           WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
           /* Send Meas Ctrl Failure Msg */
  
           rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
           /* Set faliure t */
           rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

           rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_rat_meas_params);
           return FAILURE;
       }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }

      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
      {
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type,&rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r6_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATCellInfoList)) )
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r6_setup))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
       rrc_InterRATMeasurement_r6,interRATCellInfoList)))
    {
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATCellInfoList,
          interRATCellInfoIndication_r6))
      {
        interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.
                                 u.setup->u.interRATMeasurement->interRATCellInfoList.interRATCellInfoIndication_r6;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r6_modify))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r6_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATCellInfoList)))
    {
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
        interRATCellInfoIndication_r6))
      {
        interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.
           u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList.interRATCellInfoIndication_r6;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;

  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif

  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_CELL_LIST_MCM_R6

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm_r6
(
  rrc_InterRATCellInfoList_r6* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
)
{
/* RRC new cell info list and l1 new cell info list would have the new cell info list
through local neighbour list. This has to be done to make sure that if the Meas Ctrl
  Msg contains bad cells, our good inter_rat_cell_list does not get disturbed */

  uint8 cnt = 0;

  uint8 cell_count = 0;

  uint32 cell_position = 0;

  uint16 cell_id = INVALID_CELL_ID;

  /* For storing the new cell list */
  rrc_NewInterRATCellList* local_rrc_new_cell_list_ptr = NULL;
  uint32 idx=0;
  rrc_CellsForInterRATMeasList* local_cells_for_meas_ptr = NULL;
  boolean valid_cell_id_found =FALSE;
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  /* Initializes the local neighbor list to default values */
  rrcirat_initialize_local_neighbor_list();
  
  /* Copy Inter-RAT CELL_INFO_LIST list to local neighbor list. This is necessary
  so as to make sure that in case UTRAN messes up, our old cell list is intact */

  for (cnt=0; cnt < L1_MAX_CELL_MEAS; cnt++)
  {
    WCDMA_MEMCPY(&local_neighbor_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type),
                 &inter_rat_cell_list[cnt],
                 sizeof(inter_rat_cell_list_struct_type));
  }

  /* In Inter-RAT meas, remove is not optional */
  /* For sanity, initialize to 0 */
  l1_cell_list_ptr->num_cell_rmv = 0;

  if (rrcirat_fill_removed_inter_rat_cells(&mcm_cell_list_ptr->removedInterRATCellList,
    l1_cell_list_ptr) == FAILURE)
  {
    WRRC_MSG0_HIGH("Invalid Removed Inter-RAT cells");
    return RRCMEAS_FAIL;
  }

  /* UTRAN can ask us to remove all cells when it is possible that for that
  * measurement (First time SETUP) we may not have any cell list.
  * To avoid L1 getting confused, let us make number of cells to be
  * removed to 0 in case the cell list does not exist previously.
  * So check in the CELL_INFO_LIST */

  if ((RRC_CHECK_COMMON_MSG_TYPE2(mcm_cell_list_ptr->removedInterRATCellList,
    rrc_RemovedInterRATCellList, removeAllInterRATCells)) &&
    (rrcirat_check_cell_absence_in_cell_list() == SUCCESS))
  {
    /* Set No of Cells removed to 0 from oxff */
    WRRC_MSG0_HIGH("Setting No of cells removed to 0");
    l1_cell_list_ptr->num_cell_rmv = 0;
  }


  /* At this time, local_neighbor_list is completely updated and has
  * the latest and the greatest information
  * Get New cells to be added. UTRAN can just set it to NULL */
  if ( !(RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,rrc_InterRATCellInfoList_r6,newInterRATCellList))
    || ((RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,rrc_InterRATCellInfoList_r6,newInterRATCellList))
    && (mcm_cell_list_ptr->newInterRATCellList.n == 0)))
  {
    WRRC_MSG0_HIGH("MCM:No new cells to be added");
    /* Put this info in L1 list */
    l1_cell_list_ptr->num_cell_add = 0;
  }
  else
  {
    if (rrcirat_check_inter_rat_cell_order_mcm(&mcm_cell_list_ptr->newInterRATCellList) == FAILURE)
    {
      WRRC_MSG0_HIGH("Sending Meas Ctrl Failure");
      return RRCMEAS_FAIL;
    }
    /* New cells need to be added */
    local_rrc_new_cell_list_ptr =  &mcm_cell_list_ptr->newInterRATCellList;

    /* Before verifying the cell id, make sure that cell params are correct */

    while ((local_rrc_new_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
    {
    /* Let first check the type of the cell to find if we
      really need to give this cell to L1 */
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
      if (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
        T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
      {
        WRRC_MSG0_HIGH("MCM:Ignoring Non GSM cell");
        idx++;
        continue;
      }
      else
      {
        l1_cell_list_ptr->add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;

        /* Get GSM CELL Info */

        /* No need to read cell selec/reselect info as that is for SIB11/12 */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.cell_sel_info_incl = FALSE;

        /* Cell Individual Offset */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->interRATCellIndividualOffset;

        /* Get BAND 1800 DCS or 1900 PCS */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
          RET_INTER_RAT_BAND(local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->frequency_band);

        /* Get bcch_ARFCN */
        CONVERT_ARFCN_TO_NUM(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN;

#ifdef FEATURE_GAN_3G
        #error code not present
#endif

#ifdef FEATURE_GSM_QUADBAND
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        {
          if ((converted_gsm_band = rrc_get_gsm_band(l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info)) !=
            INVALID_BAND)
          {
            l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
              converted_gsm_band;
          }
          else
          {
            WRRC_MSG1_HIGH("ARFCN %d rejected bec of invalid band",local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
              u.gsm->bcch_ARFCN);
            idx++;
            return RRCMEAS_FAIL;
          }
        }

#endif /* FEATURE_GSM_QUADBAND */

        /* To add BSIC (Base Station Identification Code) */
        if ((local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
          > NCC_MAX) ||
          (local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
          > BCC_MAX))
        {
          WRRC_MSG2_ERROR("BCC %d or NCC %d invalid[0..7]",
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
            local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);

          return RRCMEAS_FAIL;

        } /* End of if of checking NCC and BCC validity */

        /* Store good values of bcc and ncc */
        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.ncc;

        l1_cell_list_ptr->add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc = (uint8)
          local_rrc_new_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bsic.bcc;
      } /* End of gsm cell  */

      /* Time to get its cell id*/
      /* Cell Order has already been verified */

      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(local_rrc_new_cell_list_ptr,interRATCellID))
      {
        WRRC_MSG0_MED("MCM:Inter-RAT Cell Id present");
        cell_position = local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
        l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id = (uint16)
          local_rrc_new_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        /* Store the cell in first order position */
        cell_position = AVAILABLE_CELL_POSITION;
        /* Tell to get the first available cell position and
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
      }


      /* Now store the cell id in local neighbour rat cell list */
      if (rrcirat_update_inter_rat_cell_list_mcm(&cell_position,
        &l1_cell_list_ptr->add_cell[cell_count].intersys_cell_id, &cell_id)
        == SUCCESS)
      {
        /* set this flag to TRUE always for MCM */
        l1_cell_list_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
#ifdef FEATURE_GAN_3G
        #error code not present
#endif
        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }
      else
      {
        WRRC_MSG0_ERROR("MCM:Invalid Inter-RAT cell list");
        return RRCMEAS_FAIL;
      }

      /* Increment the pointer */
      /* Read Next One */
      idx++;


      if ((local_rrc_new_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List full");
        break;
      }

    } /* End of while inter_rat_cell_list_ptr */

    /* Finally Set the cell count */
    l1_cell_list_ptr->num_cell_add = cell_count;

  } /* End of else of new cells added */

  cell_count = 0;

  /* Check for cells for meas */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(mcm_cell_list_ptr,
    rrc_InterRATCellInfoList_r6,cellsForInterRATMeasList))
  {
    WRRC_MSG0_HIGH("Cells for meas present");
    l1_cell_list_ptr->cells_for_meas_incl = TRUE;

    local_cells_for_meas_ptr = &mcm_cell_list_ptr->cellsForInterRATMeasList;
    idx=0;
    while (local_cells_for_meas_ptr->n > idx)
    {
      cell_id = (uint16) local_cells_for_meas_ptr->elem[idx];

      /* Check if the cell id is present */
      if( rrcirat_find_cell_id(&cell_id) == FAILURE)
      {
        WRRC_MSG1_ERROR("cell to measure with id = %d not found",
          cell_id);
      } /* End of if for cell id not found */
      else
      {
      /* Store cell id */
        valid_cell_id_found = TRUE;
      l1_cell_list_ptr->meas_cells_list[cell_count++] = cell_id;
      }
      

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
        WRRC_MSG0_HIGH("None of the cell id is valid");
        /* Send Meas Ctrl Failure Message */
  
        return RRCMEAS_UNSUPPORTED;
    }

    l1_cell_list_ptr->num_cell_meas =  cell_count;

  } /* End of if of rrc_cellsForIntraFreqMeasList_present */


  return RRCMEAS_SUCCESS;
}


/*===========================================================================

FUNCTION rrcirat_read_sib11bis

DESCRIPTION

  Reads SIB11bis from the current active serving cell for all the measurements.
  If SIB11bis information is incorrect, meas setup command is not send to L1/MAC
  and UE does not do any measurements..

DEPENDENCIES

  SIB must have been stored by the SIB procedure

RETURN VALUE

  Success if SIB11bis has been correctly read
  Failure if SIB11bis has not been correctly read

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib11bis
(
  l1_meas_sib_parm_struct_type* l1_inter_rat_sib_ptr,
  boolean sib18_present,
  rrc_PLMNsOfInterRATCellsList *tmp_rrc_PLMNsOfInterRATCellsList_ptr
)
{
  struct rrc_SysInfoType11bis *sib11bis_ptr = NULL;
  struct rrc_MeasurementControlSysInfoExtensionAddon_r5 *sib11bis_rel5_ptr = NULL;
  rrc_NewInterRATCellList *inter_rat_cell_list_ptr = NULL;
  rrc_SysInfoType3  *sib3_ptr = NULL;
  rrc_SysInfoType4  *sib4_ptr = NULL;
  rrc_MeasurementControlSysInfoExtensionAddon_r5_newInterRATCellInfoListAddon_r5 *rel5_ext_ptr = NULL;
  uint16 cell_count =0;
  boolean sib4_present = FALSE;
  uint16 cell_position;
  boolean hcs_used = FALSE;
  uint32 idx=0;
  uint16 arfcn;

  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  
  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;

  selected_plmn_identity.mcc[0] =  selected_plmn_identity.mcc[1] = selected_plmn_identity.mcc[2] = 0;
  selected_plmn_identity.num_mnc_digits = 0;
  selected_plmn_identity.mnc[0] =  selected_plmn_identity.mnc[1] = 0;
  
  if(!rrc_sibdb_is_sib11bis_present_in_srv_cell())
  {
    WRRC_MSG0_HIGH("SIB11bis: SIB11bis is not present in Serving Cell");
    return FAILURE;
  }
  sib11bis_ptr = (rrc_SysInfoType11bis *)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB11bis);
  if(sib11bis_ptr == NULL)
  {
    WRRC_MSG0_HIGH("SIB11bis: SIB11bis ptr is NULL");
    return FAILURE;
  }
  if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib11bis_ptr,measurementControlSysInfo)))
  {
    WRRC_MSG0_HIGH("SIB11bis: Mesaurement Control SysInfo is not Present in SIB11bis");
    return FAILURE;
  }
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib11bis_ptr,measurementControlSysInfoExtensionAddon_r5))
  {
    WRRC_MSG0_HIGH("SIB11bis: Rel5 Measurement Control info present in SIB11bis");
    sib11bis_rel5_ptr = &sib11bis_ptr->measurementControlSysInfoExtensionAddon_r5;
    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib11bis_rel5_ptr,newInterRATCellInfoListAddon_r5))
    {
      WRRC_MSG0_HIGH("SIB11bis: Rel5 Cell Selection Reselection info present for Inter RAT");
      rel5_ext_ptr = &sib11bis_rel5_ptr->newInterRATCellInfoListAddon_r5;
    }
  }
  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
  if( sib3_ptr == NULL )
  {
    WRRC_MSG0_ERROR("SIB11bis: SIB3 ptr is NULL");
    return( FAILURE );
  }
  
  if (((rrcmeas_current_substate == RRCMEAS_CELL_FACH) || (rrcmeas_current_substate == RRCMEAS_PCH)) &&
    ((sib3_ptr->sib4indicator == TRUE)))
  {
    sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB4);

    if (sib4_ptr == NULL)
    {
      WRRC_MSG0_ERROR("SIB11bis: SIB4 ptr NULL.Use SIB3");
    }
    else
    {
      WRRC_MSG0_HIGH("SIB11bis: Use default values from SIB4");
      sib4_present = TRUE;
    }
  }

  if(sib18_present == FALSE)
  {
    WRRC_MSG0_HIGH("SIB11bis: SIB18 is Absent/Ignored");
  }
  else if(rrc_ccm_get_serv_plmn_cellid(&tmp_cell_id, &selected_plmn_identity) != RRC_CELL_LOSS_NO_NEW_CELL)
  {
    WCDMA_MEMCPY(&tmp_plmn_identity, 
                 sizeof(rrc_plmn_identity_type),
                 &selected_plmn_identity, 
                 sizeof(rrc_plmn_identity_type));
  }
  else
  {
    sib18_present = FALSE;
  }
  if(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.t == 
      T_rrc_MeasurementControlSysInfoExtension_use_of_HCS_hcs_used)
  {
    WRRC_MSG0_HIGH("SIB11bis: HCS Used in SIB11bis");
    hcs_used = TRUE;
    if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->m.newInterRATCellListPresent))
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter RAT cell list is not present in SIB11bis");
      return FAILURE;
    }
    inter_rat_cell_list_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->newInterRATCellList;
  }
  else
  {
    WRRC_MSG0_HIGH("SIB11bis: HCS not Used in SIB11bis");
    hcs_used = FALSE;
    if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->m.newInterRATCellListPresent))
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter RAT cell list is not present in SIB11bis");
      return FAILURE;
    }
    inter_rat_cell_list_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->newInterRATCellList;
  }
  cell_count = l1_inter_rat_sib_ptr->inter_s.num_cell_add;
  while ( (inter_rat_cell_list_ptr->n > idx) &&
    (cell_count < L1_MAX_CELL_MEAS) )
  {
    /* Let first check the type of the cell to find if we
    * really need to give this cell to L1 */
    if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.t !=
      T_rrc_NewInterRATCell_technologySpecificInfo_gsm)
    {
      WRRC_MSG0_HIGH("SIB11bis: Ignoring Non GSM cell");
      arfcn = MAX_ARFCN;
      if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
      {
        cell_position = (uint16)
          inter_rat_cell_list_ptr->elem[idx].interRATCellID;
      }
      else
      {
        cell_position = AVAILABLE_CELL_POSITION;
      }
      /* Tell to get the first available cell position and
  
      * that would be the cell id. The called function would store
      * the cell id back in passed cell_position variable */
      if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
      {
        WRRC_MSG0_HIGH("Inter-RAT Cell List invalid");
        return FAILURE;
      }
      idx++;
      continue;
    }
    else
    {
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].cell_info_type = L1_GSM_SYSTEM;
  
      /* Cell reselection info is always set to TRUE. If not present in SIB 11, just
      set it to default values */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info_incl = TRUE;
  
      /* Initializw q-offset1-2 Defaults as per specs */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.qoffset1_s_n = QOFFSET1;
  
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.qoffset2_s_n = QOFFSET2;
  
      /* -24. Got this value from Radio Access Capability */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
        gsm_cell.cell_sel_info.max_tx_pwr = -24;
  
      /* Set default values for the HCS neighboring cell info if it is first cell
       * else use the info from the previous cell */
      if(hcs_used == TRUE)
      {
        if(cell_count == 0)
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
             gsm_cell.hcs_info.hcs_prio = 0;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.q_hcs = 110;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
           gsm_cell.hcs_info.penalty_time = 0;
        }
        else
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.hcs_prio =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.hcs_prio;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.q_hcs =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.q_hcs;
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.hcs_info.penalty_time =
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.penalty_time;
          
          if(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
            gsm_cell.hcs_info.penalty_time != 0)
          {
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
              gsm_cell.hcs_info.tmp_offset1 =
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].u.
              gsm_cell.hcs_info.tmp_offset1;
          }
        }
      }
      if ((sib4_present == TRUE) && (sib4_ptr != NULL))
      {
      /* Not valid for Inter-RAT but initialize to default
        * as they are present */
        if(RRC_CHECK_COMMON_MSG_TYPE(sib4_ptr->cellSelectReselectInfo.modeSpecificInfo,
           rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qual_min = (int16)
            sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.rxlev_min = (int16)
            (sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
        }
        else
        {
          WRRC_MSG1_ERROR("ERR: SIB4 mode_choice NOT FDD %d", sib4_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
        }
      }
      else
      {
        if(RRC_CHECK_COMMON_MSG_TYPE(sib3_ptr->cellSelectReselectInfo.modeSpecificInfo,
           rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qual_min = (int16)
            sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_QualMin;
  
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.rxlev_min = (int16)
            (sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1;
        }
        else
        {
          WRRC_MSG1_ERROR("ERR: SIB3 mode_choice NOT FDD %d", sib3_ptr->cellSelectReselectInfo.modeSpecificInfo.t);
        }
      }
  
      if(hcs_used == FALSE)
      {
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.hcs_prio = 0;
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.q_hcs = 110;
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
          gsm_cell.hcs_info.penalty_time = 0;
      }
      /* Get GSM CELL Info */
      /* Check if rrc_gsm_cellSelectionReselectionInfo_present */
      if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->m.cellSelectionReselectionInfoPresent)
      {
        MSG_LOW("Cell Reselection info present",0,0,0);
  
        /* q-Offset1S-N */
     
          /* Range is -50...50 */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset1_s_n = (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.q_Offset1S_N;
        
  
        /* q_Offset2S_N  -50...50 */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->cellSelectionReselectionInfo.m.q_Offset2S_NPresent)
        {
          /* Range is -50...50 */
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.qoffset2_s_n =  (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.q_Offset2S_N;
        }
  
        /* Max Txit Power */
        if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
          cellSelectionReselectionInfo.m.maxAllowedUL_TX_PowerPresent)
        {
          l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
            gsm_cell.cell_sel_info.max_tx_pwr = (int16)
            inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.maxAllowedUL_TX_Power;
        }
  
        /* Check if gsm mode present  */
  
        /* Check if rxlev_min present */
  
        if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
           cellSelectionReselectionInfo.modeSpecificInfo.t == 
             T_rrc_CellSelectReselectInfoSIB_11_12_modeSpecificInfo_gsm)
        {
          if (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.modeSpecificInfo.u
            .gsm->m.q_RxlevMinPresent)
          {
            /* Store rxlev_min in L1 */
            l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min = (int16)
              ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.modeSpecificInfo.u.gsm->q_RxlevMin * 2) + 1);
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Incorrect mode specific info for GSM entry in SIB11bis");
        }
        if(rel5_ext_ptr != NULL)
        {
          if(RRC_MSG_LIST_BITMASK_IE_TYPE2(rel5_ext_ptr,deltaQrxlevmin))
          {
            MSG_HIGH("SIB11bis: Delta-Qrxlev=%d, Qrxlev=%d, ARFCN=%d",
              (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2),
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min,
              CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn));
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.rxlev_min +=
                (rel5_ext_ptr->elem[idx].deltaQrxlevmin * 2);
          }
        }
        if(hcs_used == TRUE)
        {
          /* HCS inforamtion */
          if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
            cellSelectionReselectionInfo.m.hcs_NeighbouringCellInformation_RSCPPresent)
          {
            /* HCS PRIO*/
    
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                gsm_cell.hcs_info.hcs_prio = (uint16)
                inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_PRIO;
            
            
            /*Q HCS */
          
              if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS < 90)
              {
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.hcs_info.q_hcs = (uint16) (110 -
                  inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.q_HCS);
              }
              else
              {
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.
                  gsm_cell.hcs_info.q_hcs = 37;
              }
            
            /* Delta Qhcs is not needed if it is not mapped from CPICH-RSCP or PCCPCH-RSCP.
            * For inter rat cells, Qhcs is always mapped from RSSI
            */
            
            /* PENALTY TIME*/
            if(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
              cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
              .penaltyTime.t != T_rrc_PenaltyTime_RSCP_notUsed)
            {
              
              switch(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                .penaltyTime.t)
              {
              case T_rrc_PenaltyTime_RSCP_pt10:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 10;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt10);
                break;
                
              case T_rrc_PenaltyTime_RSCP_pt20:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 20;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt20);
                break;
                
              case T_rrc_PenaltyTime_RSCP_pt30:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 30;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt30);
                break;
                
              case T_rrc_PenaltyTime_RSCP_pt40:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 40;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt40);
                break;
                
              case T_rrc_PenaltyTime_RSCP_pt50:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 50;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt50);
                break;
                
              case T_rrc_PenaltyTime_RSCP_pt60:
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.penalty_time = 60;
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.hcs_info.tmp_offset1 = (uint16)
                  RRCMEAS_RET_HCS_TMP_OFFSET1(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->
                  cellSelectionReselectionInfo.hcs_NeighbouringCellInformation_RSCP.hcs_CellReselectInformation
                  .penaltyTime.u.pt60);
                break;
                
              default:  /* should not happen */
                break;
              }
            }
          }
        }
      } /* rrc_gsm_cellSelectionReselectionInfo_present */
  
      /* Cell Individual Offset */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.cio = (int16)
        inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->interRATCellIndividualOffset;
  
      /* Get BAND 1800 DCS or 1900 PCS */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.band_indicator =
        RET_INTER_RAT_BAND(inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->frequency_band);
  
      /* Get bcch_ARFCN */
      CONVERT_ARFCN_TO_NUM(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn) =
        (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bcch_ARFCN;
      if(hcs_used)
      {
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.max_tx_pwr = 
          (int16)rr_get_maximum_power_output_for_frequency(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn);
        WRRC_MSG1_HIGH("SIB11bis: MAX_TX_POWR = %d",l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.cell_sel_info.max_tx_pwr);
      }

      arfcn = (uint16) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bcch_ARFCN;

#ifdef FEATURE_GSM_QUADBAND
  
      if ((converted_gsm_band = rrc_get_gsm_band(l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info)) !=
        INVALID_BAND)
      {
        l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bcch_arfcn.band =
          converted_gsm_band;
      }
      else
      {
        WRRC_MSG1_HIGH("ARFCN %d ignored bec of invalid band",inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
          u.gsm->bcch_ARFCN);
        arfcn = MAX_ARFCN;
        if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr, interRATCellID))
        {
          cell_position = (uint16)
            inter_rat_cell_list_ptr->elem[idx].interRATCellID;
        }
        else
        {
          cell_position = AVAILABLE_CELL_POSITION;
        }
        /* Tell to get the first available cell position and
  
        * that would be the cell id. The called function would store
        * the cell id back in passed cell_position variable */
        if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter-RAT Cell List invalid");
          return FAILURE;
        }
        idx++;
        continue;
      }
  
#endif /* FEATURE_GSM_QUADBAND */
  
      /* To add BSIC (Base Station Identification Code) */
      if ((inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc
        > NCC_MAX) ||
        (inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc
        > BCC_MAX))
      {
        WRRC_MSG2_ERROR("SIB11bis: BCC %d or NCC %d invalid[0..7]",
          inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.bcc,
          inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.u.gsm->bsic.ncc);
  
        return FAILURE;
  
      } /* End of if of checking NCC and BCC validity */
  
      /* Store good values of bcc and ncc */
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.ncc =
        (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bsic.ncc;
  
      l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].u.gsm_cell.gsm_info.bsic.bcc =
        (uint8) inter_rat_cell_list_ptr->elem[idx].technologySpecificInfo.
        u.gsm->bsic.bcc;
    } /* End of gsm cell  */
  
    /* Time to get its cell id*/
    /* Cell Order has already been verified */
  
    if (RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_rat_cell_list_ptr,interRATCellID))
    {
      cell_position = (uint16)
        inter_rat_cell_list_ptr->elem[idx].interRATCellID;
    }
    else
    {
      cell_position = AVAILABLE_CELL_POSITION;
    }
    /* Tell to get the first available cell position and
  
    * that would be the cell id. The called function would store
    * the cell id back in passed cell_position variable */
    if (rrcirat_update_inter_rat_cell_list(&cell_position,arfcn) == FAILURE)
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter-RAT Cell List invalid");
      return FAILURE;
    }
    l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id
      = cell_position;
  
    /* set the measure in PCH IDLE and FACH flag to TRUE */
    l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
    /* Increment cell count */

    if(sib18_present)
    {
      if((tmp_rrc_PLMNsOfInterRATCellsList_ptr != NULL) && (tmp_rrc_PLMNsOfInterRATCellsList_ptr->n > cell_count))
      {
        if(tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].m.plmn_IdentityPresent)
        {
          rrc_translate_gsm_map_plmn_id(&tmp_rrc_PLMNsOfInterRATCellsList_ptr->elem[cell_count].plmn_Identity,
            &tmp_plmn_identity);
          if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
          {
            if((rrccsp_check_for_eplmn(
              mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
              tmp_plmn_identity))) == FALSE)
            {
              WRRC_MSG1_HIGH("cell id %d Non eq PLMN measure only in DCH",
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].intersys_cell_id);
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach = FALSE;
            }
          }
        }
        else
        {
          /* use the previous value of the flag if PLMN id is not selected PLMN */
          if(rrc_bcch_and_nas_plmns_match(tmp_plmn_identity, selected_plmn_identity) == FALSE)
          {
            /* Add code to set the flag meas_pch_fach_idle to the flag value of the previous cell */
            if(cell_count > 0)
            {
              l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count].measure_in_idle_pch_fach =
                l1_inter_rat_sib_ptr->inter_s.add_cell[cell_count - 1].measure_in_idle_pch_fach;
            }
          }
        }        
      }
      else
      {
        /* PLMN list is shorter than the cell info list set flag to true for the remaining cells */
        WRRC_MSG0_HIGH("PLMN list < Cell list");
      }
    }

    cell_count++;
    /* Increment the pointer */
    idx++;
    if ((inter_rat_cell_list_ptr->n > idx) && (cell_count == L1_MAX_CELL_MEAS))
    {
      WRRC_MSG0_HIGH("SIB11bis: Inter-RAT Cell List full");
      break;
    }
  }
  l1_inter_rat_sib_ptr->inter_s.num_cell_add = cell_count;
  WRRC_MSG1_HIGH("SIB11bis: Number of Inter RAT cells added from SIB11 and SIB11bis is %d",l1_inter_rat_sib_ptr->inter_s.num_cell_add);
  if(cell_count == 0)
  {
    WRRC_MSG0_HIGH("SIB11bis: Inter RAT, number of cells added is zero");
    l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = FALSE;
    return FAILURE;
  }
  l1_inter_rat_sib_ptr->inter_s.num_cell_add = cell_count;
  l1_inter_rat_sib_ptr->inter_sys_meas_info_incl = TRUE;
  l1_inter_rat_sib_ptr->hcs_used = hcs_used;
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R7

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r7
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL;
  rrcmeas_status_e_type result;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  rrc_InterRATMeasurement_r6 *irat_meas_cmd_ptr = NULL;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;
  
  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
  l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;

  /* The following is not supported in Rel 99 anymore. So set it to FALSE */
  l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;

  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = transaction_id;
  l1_inter_rat_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r7_setup))
  {
    /* Double check that it is INTER-RAT Meas */
    if (!(RRC_CHECK_COMMON_MSG_TYPE_PTR(rrc_mcm_ptr->measurementCommand.u.setup,
        rrc_MeasurementType_r7_interRATMeasurement)))
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */

    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATReportingQuantity)) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.t == T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
      rrc_MeasurementQuantityGSM_dummy) &&
      (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo,
      rrc_InterRATReportingQuantity_ratSpecificInfo_gsm)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria,
      rrc_InterRATReportCriteria,interRATReportingCriteria)) &&
      ((!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria,interRATEventList))) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Set Meas Identity */
    l1_inter_rat_meas_params->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;

    /* Set Meas Quantity as Inter-sys Meas Quan present */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

    /* Initialize Default Value to intra_freq_meas_quan_incl */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
        modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Get Filter Coefficient too */
      /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
      
        /* Store filter coefficient */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.
          interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
      
    } /* End of quality estimate present */

    /* Set System Choice to GSM */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

    /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

    /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
    So set it to RSSI */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
      L1_GSM_CARRIER_RSSI;

      /* GSM Filter Coefficient present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);


    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
      RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

    /* Store Reporting Quantity */
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

    /* Sys Choice */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

    /* L1 wants this to be FALSE for the first version as it does not support it */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

    /* Ignore reading Observed time difference as no longer supported in Rel 99
    l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
    rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
    ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

    /* Check if gsm_carrier_rssi wanted in Meas Report */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

    if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interRATMeasurement->reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s,
      &mcm_rpt_crit, transaction_id))
    {
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    /* For L1 sake, copy reporting criteria */
    l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

    /* Now set other values */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;

    l1_inter_rat_meas_params->meas_object_info_included = TRUE;


    /* Time to process the cell list */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATCellInfoList))
    {
      result = rrcirat_process_cell_list_mcm_r6(&rrc_mcm_ptr->measurementCommand.
        u.setup->u.interRATMeasurement->interRATCellInfoList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
	  if(result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
       rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else if(result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
         WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      /* Set Cell list is present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;

        }
      }
    } /* End of if of Additional Meas Present */

    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r6,interRATCellInfoList))
    {
      /* At this point everything has been validated and now is the time
      to update the inter rat CELL_INFO_LIST */

      for (count=0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_rat_cell_list[count],
                     sizeof(inter_rat_cell_list_struct_type),
                     &local_neighbor_list[count],
                     sizeof(inter_rat_cell_list_struct_type));
      }
    }

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r7_modify)
  {
    /* Initialize Filter Coefficient to Default Value i.e. 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

    /* Initialize GSM Filter Coefficient to 0 */
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
    L1_GSM_FILTER_COEF_0;

    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_rat_meas_params->rpt_mode_incl = TRUE;

      l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r7_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      {
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }
    }
    else
    {
      if(!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_r7_interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
     
      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            ((irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_ratSpecificInfo_gsm) ||
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
        interRATMeasurement,rrc_InterRATMeasurement_r6,interRATMeasQuantity)))
      {
        WRRC_MSG0_HIGH("Meas Quan absent");
      }
      else
      {
        /* Set Meas Quantity to TRUE */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
        if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
          rrc_MeasurementQuantityGSM_dummy)
        {

          WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");

          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }

        /* Meas Quantity Parameter 1 i.e. Quality Estimate */
        if(RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
           interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
        {
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

          /* Meas Quan i.e ECI0/RSCP/RSSI... */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
            u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
            modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

          /* Get Filter Coefficient too */
          /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
          
            /* Store filter coefficient */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
              .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              measQuantityUTRAN_QualityEstimate.filterCoefficient);
          
        }

        /* Set System Choice to GSM */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

        /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

        /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
        So set it to RSSI */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
          L1_GSM_CARRIER_RSSI;

       
          /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
            RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
            ratSpecificInfo.u.gsm->filterCoefficient);
       

        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
          RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
            modify->measurementType.u.interRATMeasurement->
          interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      } /* End of else of Meas Quan present */

      /* Check if reporting quan present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATReportingQuantity))
      {
        /* Store Reporting Quantity */
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

        /* Sys Choice */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;

        /* L1 wants this to be FALSE for the first version as it does not support it */
        l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

        /* Observed time difference no longer supported in Rel 99
        l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

          /* Check if gsm_carrier_rssi wanted in Meas Report */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
          interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
      }

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.interRATMeasurement->reportCriteria,
        &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
      
      l1_inter_rat_meas_params->meas_object_info_included = TRUE;
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r7_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATCellInfoList)) )
      {
        /* Time to process the cell list */
        result = rrcirat_process_cell_list_mcm_r6(&rrc_mcm_ptr->measurementCommand.
          u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
        if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

          /* Set Error Cause */
          rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else if(result == RRCMEAS_UNSUPPORTED)
        {
          /* Send Meas Ctrl Failure Message */
           WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
          /* Send Meas Ctrl Failure Msg */
  
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }

      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      {
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type,&rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = RRCMEAS_IRAT_GSM;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r7_modify,measurementType))
        && (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
        & rrc_InterRATMeasurement_r6,interRATCellInfoList) ))
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;

        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r7_setup))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
       rrc_InterRATMeasurement_r6,interRATCellInfoList)))
    {
      if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATCellInfoList,
          rrc_InterRATCellInfoList_r6,interRATCellInfoIndication_r6))
      {
        interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.
                                 u.setup->u.interRATMeasurement->interRATCellInfoList.interRATCellInfoIndication_r6;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r7_modify))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r7_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
         rrc_InterRATMeasurement_r6,interRATCellInfoList)))
    {
      if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList,
                   rrc_InterRATCellInfoList_r6,interRATCellInfoIndication_r6))
      {
        interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
        interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.
           u.modify->measurementType.u.interRATMeasurement->interRATCellInfoList.interRATCellInfoIndication_r6;
      }
      else
      {
        interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
        interRAT_cell_info_status.interRAT_cell_info_ind=0;
      }
    }
  }

  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;

  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}



#ifdef FEATURE_GAN_3G
#error code not present
#endif

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION   rrcirat_initialize_eutra_freq_list

DESCRIPTION 
  This function initializes EUTRA frequency list stored

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcirat_initialize_eutra_freq_list
(
  void
)
{
  uint32 cnt;
  for(cnt = 0; cnt < L1_MAX_EUTRA_FREQ;cnt++)
  {
    rrcirat_eutra_freq_list_ptr[cnt].earfcn = RRC_INVALID_EUTRA_FREQ;
  }
}

/*===========================================================================

FUNCTION   rrcirat_update_eutra_freq_list

DESCRIPTION 
  This function updates stored EUTRA frequency with EUTRA freq received in the argument

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency is updated in the DB
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_update_eutra_freq_list
(
  uint32 earfcn
)
{
  uint32 cnt;
  for(cnt = 0; cnt < L1_MAX_EUTRA_FREQ;cnt++)
  {
    if((rrcirat_eutra_freq_list_ptr[cnt].earfcn == RRC_INVALID_EUTRA_FREQ) ||
       (earfcn == rrcirat_eutra_freq_list_ptr[cnt].earfcn))
    {
      break;
    }
  }
  if(cnt !=L1_MAX_EUTRA_FREQ)
  {
    WRRC_MSG2_HIGH("WTOL: Updated Freq %d @ idx %d",earfcn,cnt);
    rrcirat_eutra_freq_list_ptr[cnt].earfcn = earfcn;
    return SUCCESS;
  }
  else
  {
    WRRC_MSG1_ERROR("Couldn't store earfcn %d, MAX frequencies reached",earfcn);
    return FAILURE;
  }
}
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION   rrcirat_remove_eutra_freq

DESCRIPTION 
  This function removes EUTRA freq from Stored DB

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency is removed from the DB
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcirat_remove_eutra_freq
(
  uint32 earfcn
)
{
  uint32 cnt;
  for(cnt=0;cnt<L1_MAX_EUTRA_FREQ;cnt++)
  {
    if(earfcn == rrcirat_eutra_freq_list_ptr[cnt].earfcn)
    {
      rrcirat_eutra_freq_list_ptr[cnt].earfcn = RRC_INVALID_EUTRA_FREQ;
      break;
    }
  }
  if(L1_MAX_EUTRA_FREQ != cnt)
  {
    WRRC_MSG2_HIGH("Removed freq %d from idx ",earfcn,cnt);
    return SUCCESS;
  }
  else
  {
    WRRC_MSG1_HIGH("Freq %d not present in the list",earfcn);
    return FAILURE;
  }
}

/*===========================================================================

FUNCTION   rrcirat_process_eutra_freq_list_mcm

DESCRIPTION 
  This function processes EUTRA freq list from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency list is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcirat_process_eutra_freq_list_mcm
(
  rrc_EUTRA_FrequencyList *eutra_freq_list,
  l1_inter_sys_eutra_freq_list_struct_type *l1_freq_list
)
{
  if(eutra_freq_list !=NULL)
  {
    struct rrc_EUTRA_FrequencyRemoval_removeSomeFrequencies *rmv_freq_ptr = NULL;
    uint8 freq_count =0;
    uint32 earfcn;
    memset(l1_freq_list,0,sizeof(l1_inter_sys_eutra_freq_list_struct_type));
    switch(RRC_GET_MSG_TYPE(eutra_freq_list->eutraFrequencyRemoval))
    {
      case T_rrc_EUTRA_FrequencyRemoval_removeAllFrequencies:
        WRRC_MSG0_HIGH("Remove all freqs");
        l1_freq_list->num_freq_rmv = L1_REMOVE_ALL_EUTRA_FREQ;
        rrcirat_initialize_eutra_freq_list();
        break;
      case T_rrc_EUTRA_FrequencyRemoval_removeNoFrequencies:
        WRRC_MSG0_HIGH("Remove None");
        l1_freq_list->num_freq_rmv = 0;
        break;
      case T_rrc_EUTRA_FrequencyRemoval_removeSomeFrequencies:
        rmv_freq_ptr = eutra_freq_list->eutraFrequencyRemoval.u.removeSomeFrequencies;
        freq_count = 0;
        if(rmv_freq_ptr != NULL)
        {
          uint8 idx = 0;
          do
          {
            earfcn = rmv_freq_ptr->elem[idx];
            if(FAILURE == rrcirat_remove_eutra_freq(earfcn))
            {
              WRRC_MSG1_ERROR("EARFCN %d not found",earfcn);
              idx++;
              continue;
            }
            l1_freq_list->rmv_freq[freq_count] = earfcn;
            freq_count++;
            idx++;
          } while((freq_count < L1_MAX_EUTRA_FREQ) && (rmv_freq_ptr->n > idx));
        }
        l1_freq_list->num_freq_rmv = freq_count;
        break;
      default :
        /*shouldn't happen*/
        WRRC_MSG0_HIGH("Invalid Choice");
        break;
    }
    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(eutra_freq_list,eutraNewFrequencies))
    {
      rrc_EUTRA_FrequencyInfoList *freq_list_ptr = NULL;
      uint8 blc_idx=0;
      uint32 cnt = 0;
      boolean duplicate_found =FALSE;
      WRRC_MSG0_HIGH("New Frequency list present");
      freq_list_ptr = &eutra_freq_list->eutraNewFrequencies;
      memset(&l1_freq_list->add_freq_list,0,sizeof(l1_meas_eutra_frequency_list_type));
      freq_count = 0;
      if(freq_list_ptr->n != 0)
      {
        uint8 idx = 0;
        do
        {
          earfcn = freq_list_ptr->elem[idx].earfcn;

          /* Filtering the duplicate entries */
          for(cnt = 0; (cnt < L1_MAX_EUTRA_FREQ) && (cnt < freq_count);cnt++)
          {
            if(l1_freq_list->add_freq_list.eutra_frequency[cnt].EARFCN == earfcn)
            {
              duplicate_found = TRUE;
              break;
            }
          }

          if(duplicate_found == TRUE)
          {
            duplicate_found = FALSE;
            idx++;
            continue;
          }

          if(!lte_rrc_earfcn_is_supported(earfcn,rrccsp_get_lte_band_pref()))
          {
            WRRC_MSG1_HIGH("EARFCN %d not supported by UE",earfcn);
            idx++;
            continue;
          }

#ifndef FEATURE_WTOL_TDD_CM_SUPPORT
        else if(lte_rrc_cell_is_tdd(earfcn))
          {
          WRRC_MSG1_HIGH("EARFCN %d belongs to TDD band. Rejecting MCM !!",earfcn);
          return FAILURE;
          
          }
#endif
          l1_freq_list->add_freq_list.eutra_frequency[freq_count].EARFCN = earfcn;
          l1_freq_list->add_freq_list.eutra_frequency[freq_count].meas_band_width = l1_mbw6;
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(freq_list_ptr->elem[idx],measurementBandwidth))
          {
            l1_freq_list->add_freq_list.eutra_frequency[freq_count].meas_band_width = 
                 RRCMEAS_GET_EUTRA_MEAS_BANDWIDTH(freq_list_ptr->elem[idx].measurementBandwidth);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(freq_list_ptr->elem[idx],eutra_blackListedCellList))
          {
            uint8 cell_count =0;
            struct rrc_EUTRA_BlacklistedCellPerFreqList *cell_list_ptr = &freq_list_ptr->elem[idx].eutra_blackListedCellList;
            if(cell_list_ptr != NULL)
            {
              do
              {
                l1_freq_list->add_freq_list.eutra_frequency[freq_count].blacklisted_cells[cell_count] = 
                  cell_list_ptr->elem[cell_count].physicalCellIdentity;
                cell_count++;
              }while((cell_count < L1_MAX_EUTRA_CELLS_PER_FREQUENCY) && (cell_list_ptr->n > cell_count));
              l1_freq_list->add_freq_list.eutra_frequency[freq_count].num_blacklisted_cells = cell_count;
            }
          }
          if(FAILURE == rrcirat_update_eutra_freq_list(earfcn))
          {
            WRRC_MSG1_ERROR("'EARFCN %d cannot be updated in freq list",earfcn);
            return FAILURE;
          }
          for(blc_idx =0;blc_idx< l1_freq_list->add_freq_list.eutra_frequency[freq_count].num_blacklisted_cells && 
            blc_idx < L1_MAX_EUTRA_CELLS_PER_FREQUENCY;blc_idx++)
          {
            WRRC_MSG1_HIGH("Blacklisted PCI %d ", l1_freq_list->add_freq_list.eutra_frequency[freq_count].blacklisted_cells[blc_idx]);
          }
          idx++;
          freq_count++;
        }while((freq_count < L1_MAX_EUTRA_FREQ) && (freq_list_ptr->n > idx));
        l1_freq_list->add_freq_list.num_eutra_frequencies = freq_count;
      }
    }
    return SUCCESS;
  }
  else
  {
    WRRC_MSG0_ERROR("Eutra Frequency List is NULL");
    return FAILURE;
  }
  
}

/*===========================================================================

FUNCTION   rrcirat_process_mcm_setup_modify_eutra

DESCRIPTION 
  This function processes setup or modify of EUTRA IRAT measurement IE from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA measurement IE from MCM is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcirat_process_mcm_setup_modify_eutra
(
  rrc_InterRATMeasurement_r8 *mcm_irat_ptr,
  l1_meas_ctrl_parm_struct_type *l1_inter_rat_meas_params,
  uint32 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
)
{
  l1_inter_sys_meas_ctrl_struct_type *temp_inter_s_meas_ctrl = rrc_malloc(sizeof(l1_inter_sys_meas_ctrl_struct_type));

  memset(temp_inter_s_meas_ctrl,0,sizeof(l1_inter_sys_meas_ctrl_struct_type));

  l1_inter_rat_meas_params->meas_object_info_included = TRUE;
  l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS_EUTRA;

  /*Fill the measurement quantity*/
  if(!RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATMeasQuantity))
  {
    WRRC_MSG0_HIGH("Measurement Object Absent");
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_meas_quantity_incl = FALSE;
  }
  else
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_meas_quantity_incl = TRUE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.system_choice = L1_EUTRA_SYSTEM;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(mcm_irat_ptr->interRATMeasQuantity,
          measQuantityUTRAN_QualityEstimate)) &&
         (RRC_CHECK_COMMON_MSG_TYPE(mcm_irat_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo,
         rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd)))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan_incl = TRUE;
      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(mcm_irat_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Store filter coefficient */
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan.filter_coef = (uint16)
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(mcm_irat_ptr->interRATMeasQuantity.
          measQuantityUTRAN_QualityEstimate.filterCoefficient);
    }

    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef = L1_FILTER_COEF_0;
/*    if(RRC_MSG_COMMON_BITMASK_IE(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA,))*/
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef =
          RET_EUTRA_FILTER_COEFFICIENT(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA->filterCoefficient);
    }
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan =
      RET_EUTRA_MEAS_QUAN(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA->measurementQuantity);
    WRRC_MSG2_HIGH("EUTRA meas quan %d, filter_coeff %d",
        l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan,
        l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef);
  }

  /*Reporting Quantity*/
  if(!RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATReportingQuantity))
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_rpt_quantity_incl = FALSE;
  }
  else
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_rpt_quantity_incl = TRUE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.utran_est_qual = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.sys_choice = L1_EUTRA_SYSTEM;
  
    if(rrc_InterRATReportingQuantity_r8_reportingQuantity_measurementQuantity == mcm_irat_ptr->interRATReportingQuantity.ratSpecificInfo.u.eutra->reportingQuantity)
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan = 
          l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan;
    }
    else
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan = L1_EUTRA_QUAN_BOTH;
    }
    WRRC_MSG1_HIGH("EUTRA reporting Quan %d",l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan);
  }

  /*Fill Reporting Criteria*/
  if(FAILURE == rrcirat_process_mcm_report_criteria(&mcm_irat_ptr->reportCriteria,temp_inter_s_meas_ctrl,
    mcm_rpt_crit_ptr, transaction_id))
  {
    rrc_free(temp_inter_s_meas_ctrl);
    return FAILURE;
  }

  l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_cell_status_incl =
    temp_inter_s_meas_ctrl->rpt_cell_status_incl;
    WCDMA_MEMCPY(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_cell_status,
                 sizeof(l1_meas_rpt_cell_status_struct_type),
                 &temp_inter_s_meas_ctrl->rpt_cell_status,
                 sizeof(l1_meas_rpt_cell_status_struct_type));

    WCDMA_MEMCPY(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_criteria,
                 sizeof(l1_inter_sys_reporting_criteria_struct_type),
                 &temp_inter_s_meas_ctrl->rpt_criteria,
                 sizeof(l1_inter_sys_reporting_criteria_struct_type));

  l1_inter_rat_meas_params->reporting_mode.rpt_mode = 
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_criteria.rpt_mode;

  /*Measurement Objects*/
  if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATMeasurementObjects)) &&
    (RRC_CHECK_COMMON_MSG_TYPE(mcm_irat_ptr->interRATMeasurementObjects,
      rrc_InterRATMeasurement_r8_interRATMeasurementObjects_eutra_FrequencyList)))
  {
    if(FAILURE == rrcirat_process_eutra_freq_list_mcm(mcm_irat_ptr->interRATMeasurementObjects.u.eutra_FrequencyList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_freq_list))
    {
      rrcmeas_mcf_struct_type rrcmeas_mcf;
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set error choice */
     rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(temp_inter_s_meas_ctrl);
      return FAILURE;
    }
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_freq_list_incl = TRUE;
  }
  rrc_free(temp_inter_s_meas_ctrl);
  return SUCCESS;
}
#endif
#endif

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R8

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/
/*RRC_GKG: This function needs to be reviewed because there're some typos here.*/
uecomdef_status_e_type rrcirat_process_mcm_r8
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;
  rrcmeas_status_e_type result;

  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  rrc_InterRATMeasurement_r8 *irat_meas_cmd_ptr = NULL;

  rrcmeas_interrat_type interrat_type = RRCMEAS_IRAT_GSM;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  rrcirat_eutra_freq_type temp_freq_list[L1_MAX_EUTRA_FREQ];
#endif

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;


  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r8_setup))
  {
    /* Double check that it is INTER-RAT Meas */
    if (!(RRC_CHECK_COMMON_MSG_TYPE_PTR(rrc_mcm_ptr->measurementCommand.u.setup,
        rrc_MeasurementType_r8_interRATMeasurement)))
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r8,interRATMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r8,interRATReportingQuantity)) &&
      (
      ((RRC_CHECK_COMMON_MSG_TYPE
           (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
             rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm)) &&
           (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
             interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
             rrc_MeasurementQuantityGSM_dummy)) 
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
           ||
           (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
             rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA))
#endif
       )&&
#ifndef FEATURE_WCDMA_CM_LTE_SEARCH
      (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo,
      rrc_InterRATReportingQuantity_ratSpecificInfo_gsm) 
      ) &&
#endif
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria,
      rrc_InterRATReportCriteria,interRATReportingCriteria)) &&
      (!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria,interRATEventList)) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }


#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    /*Check if one element is LTE IRAT and other are GSM, then send a failure with cause invalid configuration*/
    if(RRC_CHECK_COMMON_MSG_TYPE(
         rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
         rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA))
    {
      interrat_type = RRCMEAS_IRAT_EUTRA;
    }
    if((rrc_wtol_cm_support_nv == FALSE) && (interrat_type == RRCMEAS_IRAT_EUTRA))
    {
      WRRC_MSG0_HIGH("wtol cm support nv is not enabled");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if(
        ((RRCMEAS_IRAT_EUTRA == interrat_type) &&
          (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
              interRATMeasurementObjects)) && 
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
               rrc_InterRATMeasurement_r8_interRATMeasurementObjects_eutra_FrequencyList))) ||
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
               ratSpecificInfo, rrc_InterRATReportingQuantity_r8_ratSpecificInfo_eutra)))) ||
        ((RRCMEAS_IRAT_GSM == interrat_type) &&
        (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
             interRATMeasurementObjects)) && 
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
               rrc_InterRATMeasurement_r8_interRATMeasurementObjects_interRATCellInfoList))) ||
           (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
               ratSpecificInfo, rrc_InterRATReportingQuantity_r8_ratSpecificInfo_gsm))))
       )
    {
      WRRC_MSG0_ERROR("LTOW: Incompatible choice of RAT choice in objects, meas quantity, reporting quantity");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
#endif
  }
  else
  {
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    /*Derive if the measurement type is of LTE IRAT based on the stored measurement ID list*/
    /* Store meas in MEASUREMENT_IDENTITY List */
    if(rrcirat_is_meas_id_eutra_meas((uint16) rrc_mcm_ptr->measurementIdentity))
    {
      interrat_type = RRCMEAS_IRAT_EUTRA;
    }
    if((rrc_wtol_cm_support_nv == FALSE) && (interrat_type == RRCMEAS_IRAT_EUTRA))
    {
      WRRC_MSG0_HIGH("wtol cm support nv is not enabled");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /*Validations for measurement modify*/
    if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r8_modify)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify,measurementType)))
    {
      /*
        As of now If interrat rat type is EUTRA, then, no other type should be GSM
        TBD: Can N/W switch EUTRA<->GSM with modify ?
      */
      if(
          ((RRCMEAS_IRAT_EUTRA == interrat_type) &&
           (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasurementObjects)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
              rrc_InterRATMeasurement_r8_interRATMeasurementObjects_eutra_FrequencyList))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
              rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA ))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATReportingQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATReportingQuantity.ratSpecificInfo,
              rrc_InterRATReportingQuantity_r8_ratSpecificInfo_eutra ))))) ||
          ((RRCMEAS_IRAT_GSM == interrat_type) &&
           (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasurementObjects)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
              rrc_InterRATMeasurement_r8_interRATMeasurementObjects_interRATCellInfoList))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
              rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm ))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATReportingQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATReportingQuantity.ratSpecificInfo,
              rrc_InterRATReportingQuantity_r8_ratSpecificInfo_gsm )))))
        )
      {
        WRRC_MSG0_ERROR("LTOW: Incompatible choice of RAT choice in objects, meas quantity, reporting quantity");
  
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
    }
#endif

    if((rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_modify) &&
        (rrc_mcm_ptr->measurementCommand.u.modify->m.measurementTypePresent))
    {
      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            (
#ifndef FEATURE_WCDMA_CM_LTE_SEARCH
              (irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm) ||
#endif
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
    }
  }


  if (((RRCMEAS_IRAT_GSM == interrat_type) && (rrcmcm_is_dualmode_enabled() == FALSE)) 
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
       ||((RRCMEAS_IRAT_EUTRA == interrat_type) && (rrcmcm_is_lte_mode_enabled() == FALSE))
#endif
       )
  {
    WRRC_MSG1_HIGH("REL8:IRAT:Mode not enabled, IRAT_Type %d",interrat_type);
    /* Send Meas Ctrl Failure Message */
  
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
    /* Set error choice */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
  
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
     rrc_free(l1_inter_rat_meas_params);
    return FAILURE;
  }

  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = transaction_id;
  l1_inter_rat_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  if(RRCMEAS_IRAT_EUTRA == interrat_type)
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS_EUTRA;
    memset(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra,0,sizeof(l1_inter_sys_eutra_meas_ctrl_struct_type));
  }
  else
#endif
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;
    /* The following is not supported in Rel 99 anymore. So set it to FALSE */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;
  }


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,
      measurementReportingMode))
  {
    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;
  
    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
  }

  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r8_setup))
  {
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    if(RRCMEAS_IRAT_EUTRA == interrat_type)
    {
      uecomdef_status_e_type status;
      /*Take a back up to temp list*/
      WCDMA_MEMCPY(temp_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                   rrcirat_eutra_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
      rrcirat_eutra_freq_list_ptr = temp_freq_list;

      /*Call the function passing the MCM setup structure that updates LTE info to the l1_inter_rat_meas_params*/
      status = rrcirat_process_mcm_setup_modify_eutra(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
                             l1_inter_rat_meas_params,transaction_id,&mcm_rpt_crit);
      rrcirat_eutra_freq_list_ptr = rrcirat_eutra_freq_list;

      if(FAILURE == status)
      {
        rrc_free(l1_inter_rat_meas_params);
        return status;
      }
    }
    else
#endif
    {
      /* Set Meas Identity */
      l1_inter_rat_meas_params->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;

      /* Set Meas Quantity as Inter-sys Meas Quan present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

      /* Initialize Default Value to intra_freq_meas_quan_incl */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

      /* Initialize Filter Coefficient to Default Value i.e. 0 */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

      /* Initialize GSM Filter Coefficient to 0 */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
      L1_GSM_FILTER_COEF_0;

      /* Meas Quantity Parameter 1 i.e. Quality Estimate */
      if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
        interRATMeasQuantity,rrc_InterRATMeasQuantity_r8,measQuantityUTRAN_QualityEstimate))
      {
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

        /* Meas Quan i.e ECI0/RSCP/RSSI... */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
          u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
          modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

        /* Get Filter Coefficient too */
        /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
        
          /* Store filter coefficient */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.
            interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
        
      } /* End of quality estimate present */

      /* Set System Choice to GSM */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

      /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

      /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
      So set it to RSSI */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
        L1_GSM_CARRIER_RSSI;

        /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
          RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
          u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);

      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
        RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
        interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      /* Store Reporting Quantity */
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

      /* Sys Choice */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;
      /* L1 wants this to be FALSE for the first version as it does not support it */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

      /* Ignore reading Observed time difference as no longer supported in Rel 99
      l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

      /* Check if gsm_carrier_rssi wanted in Meas Report */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
        rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
        ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s,
        &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      /* For L1 sake, copy reporting criteria */
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

      l1_inter_rat_meas_params->meas_object_info_included = TRUE;

      /* Time to process the cell list */
      if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
        interRATMeasurementObjects)) && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
          rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList)))
      {
        result = rrcirat_process_cell_list_mcm_r6(rrc_mcm_ptr->measurementCommand.
          u.setup->u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
        if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
         rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else if(result == RRCMEAS_UNSUPPORTED)
        {
           /* Send Meas Ctrl Failure Message */
            WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
           /* Send Meas Ctrl Failure Msg */
  
           rrcmeas_mcf.rrc_transaction_id = transaction_id;
   
           /* Set faliure t */
           rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
           rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_rat_meas_params);
           return FAILURE;
       }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }
    }
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_rat_meas_params);
          return FAILURE;

        }
      }
    } /* End of if of Additional Meas Present */

    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      interRATMeasurementObjects))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
        rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList))
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
  
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }
    }
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    if(RRCMEAS_IRAT_EUTRA == interrat_type)
    {
      /*Copy back the temp freq list to orig freq list*/
      WCDMA_MEMCPY(rrcirat_eutra_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                   temp_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
    }
#endif
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = interrat_type;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          inter_rat_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = interrat_type;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_modify)
  {
    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;

    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r8_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      {
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params); 
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }
    }
    else
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_r8,interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
      if(RRCMEAS_IRAT_EUTRA == interrat_type)
      {
        uecomdef_status_e_type status;
        /*Take a back up to temp list*/
        WCDMA_MEMCPY(temp_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                     rrcirat_eutra_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
        rrcirat_eutra_freq_list_ptr = temp_freq_list;

        /*Call the function passing the MCM modify structure that updates LTE info to the l1_inter_rat_meas_params*/
        status = rrcirat_process_mcm_setup_modify_eutra(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
                               l1_inter_rat_meas_params,transaction_id,&mcm_rpt_crit);
        rrcirat_eutra_freq_list_ptr = rrcirat_eutra_freq_list;
        if(FAILURE == status)
        {
          rrc_free(l1_inter_rat_meas_params);
          return status;
        }
      }
      else
#endif
      {
        /* Initialize Filter Coefficient to Default Value i.e. 0 */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

        /* Initialize GSM Filter Coefficient to 0 */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        L1_GSM_FILTER_COEF_0;

        if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          interRATMeasurement,rrc_InterRATMeasurement_r6,interRATMeasQuantity)))
        {
          WRRC_MSG0_HIGH("Meas Quan absent");
        }
        else
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
          if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
            rrc_MeasurementQuantityGSM_dummy)
          {
  
            WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");
  
            /* Tr Id */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
  
            /* Set Error Cause */
            rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;
          }
  
          /* Meas Quantity Parameter 1 i.e. Quality Estimate */
          if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
          {
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;
  
            /* Meas Quan i.e ECI0/RSCP/RSSI... */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
              u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
              modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);
  
            /* Get Filter Coefficient too */
            /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
          
              /* Store filter coefficient */
              l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
                RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
                .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
                measQuantityUTRAN_QualityEstimate.filterCoefficient);
            
          }
  
          /* Set System Choice to GSM */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;
  
          /* Get GSM Meas Quan, Filter Coefficient, BSIC required */
  
          /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
          So set it to RSSI */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
            L1_GSM_CARRIER_RSSI;
  
            /* GSM Filter Coefficient present */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
              RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
              modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              ratSpecificInfo.u.gsm->filterCoefficient);
          
  
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
            RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
              modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);
  
        } /* End of else of Meas Quan present */
  
        /* Check if reporting quan present */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
           rrc_InterRATMeasurement_r6,interRATReportingQuantity))
        {
          /* Store Reporting Quantity */
          l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;
  
          /* Sys Choice */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;
  
          /* L1 wants this to be FALSE for the first version as it does not support it */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;
  
          /* Observed time difference no longer supported in Rel 99
          l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */
  
            /* Check if gsm_carrier_rssi wanted in Meas Report */
            l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
        }
  
        if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
          measurementType.u.interRATMeasurement->reportCriteria,
          &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
        {
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        l1_inter_rat_meas_params->reporting_mode.rpt_mode =
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
        
        l1_inter_rat_meas_params->meas_object_info_included = TRUE;
        if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
          rrc_MeasurementCommand_r8_modify,measurementType))
          && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
           rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList)) )
        {
          /* Time to process the cell list */
          result = rrcirat_process_cell_list_mcm_r6(rrc_mcm_ptr->measurementCommand.
            u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
          if(result == RRCMEAS_FAIL)
          {
            /* Send Meas Ctrl Failure Msg */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
  
            /* Set Error Cause */
            rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
  
            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;
          }
          else if(result == RRCMEAS_UNSUPPORTED)
          {
             /* Send Meas Ctrl Failure Message */
              WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
             /* Send Meas Ctrl Failure Msg */
  
             rrcmeas_mcf.rrc_transaction_id = transaction_id;
   
             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
              rrc_free(l1_inter_rat_meas_params);
             return FAILURE;
        }
          /* Set Cell list is present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
        }
      }
      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      {
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            TRUE, INTER_SYS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type,&rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
        }
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = interrat_type;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r8_modify,measurementType))
        && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
         rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList)) )
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
      if(RRCMEAS_IRAT_EUTRA == interrat_type)
      {
        /*Copy the updated frequency list*/
        WCDMA_MEMCPY(rrcirat_eutra_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                     temp_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
      }
#endif

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;

        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r8_setup))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
        interRATMeasurementObjects)))
    {
      if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
          interRATMeasurementObjects,rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList))
      {
        if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            interRATCellInfoIndication_r6))
        {
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
            interRATMeasurementObjects.u.interRATCellInfoList->interRATCellInfoIndication_r6;
        }
        else
        {
          interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
          interRAT_cell_info_status.interRAT_cell_info_ind=0;
        }
      }
    }
  }

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r8_modify))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r8_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
        interRATMeasurementObjects)))
    {
      if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
        interRATMeasurementObjects,rrc_InterRATMeasurement_r8_interRATMeasurementObjects,interRATCellInfoList))
      {
        if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            interRATCellInfoIndication_r6))
        {
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList->interRATCellInfoIndication_r6;
        }
        else
        {
          interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
          interRAT_cell_info_status.interRAT_cell_info_ind=0;
        }
      }
    }
  }

  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;

  WRRC_MSG0_MED("MCM:Sending cmd to L1");
  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}


#ifdef FEATURE_WCDMA_REL9
#ifdef FEATURE_WCDMA_TO_LTE
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH

/*===========================================================================

FUNCTION   rrcirat_process_eutra_freq_list_mcm_R9

DESCRIPTION 
  This function processes EUTRA freq list from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency list is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_eutra_freq_list_mcm_r9
(
  rrc_EUTRA_FrequencyList_r9 *eutra_freq_list,
  l1_inter_sys_eutra_freq_list_struct_type *l1_freq_list
)
{
  if(eutra_freq_list !=NULL)
  {
    struct rrc_EUTRA_FrequencyRemoval_removeSomeFrequencies *rmv_freq_ptr = NULL;
    uint8 freq_count =0;
    uint32 earfcn;
    memset(l1_freq_list,0,sizeof(l1_inter_sys_eutra_freq_list_struct_type));
    switch(RRC_GET_MSG_TYPE(eutra_freq_list->eutraFrequencyRemoval))
    {
      case T_rrc_EUTRA_FrequencyRemoval_removeAllFrequencies:
        WRRC_MSG0_HIGH("Remove all freqs");
        l1_freq_list->num_freq_rmv = L1_REMOVE_ALL_EUTRA_FREQ;
        rrcirat_initialize_eutra_freq_list();
        break;
      case T_rrc_EUTRA_FrequencyRemoval_removeNoFrequencies:
        WRRC_MSG0_HIGH("Remove None");
        l1_freq_list->num_freq_rmv = 0;
        break;
      case T_rrc_EUTRA_FrequencyRemoval_removeSomeFrequencies:
        rmv_freq_ptr = eutra_freq_list->eutraFrequencyRemoval.u.removeSomeFrequencies;
        freq_count = 0;
        if(rmv_freq_ptr != NULL)
        {
          uint8 idx = 0;
          do
          {
            earfcn = rmv_freq_ptr->elem[idx];
            if(FAILURE == rrcirat_remove_eutra_freq(earfcn))
            {
              WRRC_MSG1_ERROR("EARFCN %d not found",earfcn);
              idx++;
              continue;
            }
            l1_freq_list->rmv_freq[freq_count] = earfcn;
            freq_count++;
            idx++;
          } while((freq_count < L1_MAX_EUTRA_FREQ) && (rmv_freq_ptr->n > idx));
        }
        l1_freq_list->num_freq_rmv = freq_count;
        break;
      default :
        /*shouldn't happen*/
        WRRC_MSG0_HIGH("Invalid Choice");
        break;
    }
    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(eutra_freq_list,eutraNewFrequencies))
    {
      rrc_EUTRA_FrequencyInfoList *freq_list_ptr = NULL;
      uint8 blc_idx=0;
      uint32 cnt = 0;
      boolean duplicate_found =FALSE;

      WRRC_MSG0_HIGH("New Frequency list present");
      freq_list_ptr = &eutra_freq_list->eutraNewFrequencies;
      memset(&l1_freq_list->add_freq_list,0,sizeof(l1_meas_eutra_frequency_list_type));

      freq_count = 0;
      if(freq_list_ptr->n != 0)
      {
        uint8 idx = 0;
        do
        {
          earfcn = freq_list_ptr->elem[idx].earfcn;

          /* Filtering the duplicate entries */  
          for(cnt = 0; (cnt < L1_MAX_EUTRA_FREQ) && (cnt < freq_count);cnt++)
          {
            if(l1_freq_list->add_freq_list.eutra_frequency[cnt].EARFCN == earfcn)
            {
              duplicate_found = TRUE;
              break;
            }
          }

          if(duplicate_found == TRUE)
          {
            duplicate_found = FALSE;
            idx++;
            continue;
          }

          if(!lte_rrc_earfcn_is_supported(earfcn,rrccsp_get_lte_band_pref()))
          {
            WRRC_MSG1_HIGH("EARFCN %d not supported by UE",earfcn);
            idx++;
            continue;
          }
#ifndef FEATURE_WTOL_TDD_CM_SUPPORT
        else if(lte_rrc_cell_is_tdd(earfcn))
          {
          WRRC_MSG1_HIGH("EARFCN %d belongs to TDD band. Rejecting MCM !!",earfcn);
          return FAILURE;  

          }
#endif
          l1_freq_list->add_freq_list.eutra_frequency[freq_count].EARFCN = earfcn;
          l1_freq_list->add_freq_list.eutra_frequency[freq_count].meas_band_width = l1_mbw6;
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(freq_list_ptr->elem[idx],measurementBandwidth))
          {
            l1_freq_list->add_freq_list.eutra_frequency[freq_count].meas_band_width = 
                 RRCMEAS_GET_EUTRA_MEAS_BANDWIDTH(freq_list_ptr->elem[idx].measurementBandwidth);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(freq_list_ptr->elem[idx],eutra_blackListedCellList))
          {
            uint8 cell_count =0;
            struct rrc_EUTRA_BlacklistedCellPerFreqList *cell_list_ptr = &freq_list_ptr->elem[idx].eutra_blackListedCellList;
            if(cell_list_ptr != NULL)
            {
              do
              {
                l1_freq_list->add_freq_list.eutra_frequency[freq_count].blacklisted_cells[cell_count] = 
                  cell_list_ptr->elem[cell_count].physicalCellIdentity;
                cell_count++;
              }while((cell_count < L1_MAX_EUTRA_CELLS_PER_FREQUENCY) && (cell_list_ptr->n > cell_count));
              l1_freq_list->add_freq_list.eutra_frequency[freq_count].num_blacklisted_cells = cell_count;
            }
          }
          if(FAILURE == rrcirat_update_eutra_freq_list(earfcn))
          {
            WRRC_MSG1_ERROR("'EARFCN %d cannot be updated in freq list",earfcn);
            return FAILURE;
          }
          for(blc_idx =0;blc_idx< l1_freq_list->add_freq_list.eutra_frequency[freq_count].num_blacklisted_cells && 
            blc_idx < L1_MAX_EUTRA_CELLS_PER_FREQUENCY;blc_idx++)
          {
            WRRC_MSG1_HIGH("Blacklisted PCI %d ", l1_freq_list->add_freq_list.eutra_frequency[freq_count].blacklisted_cells[blc_idx]);
          }
          idx++;
          freq_count++;
        }while((freq_count < L1_MAX_EUTRA_FREQ) && (freq_list_ptr->n > idx));
        l1_freq_list->add_freq_list.num_eutra_frequencies = freq_count;
      }
    }
    return SUCCESS;
  }
  else
  {
    WRRC_MSG0_ERROR("'Eutra freq list is NULL");
    return FAILURE;
  }
}


/*===========================================================================

FUNCTION   rrcirat_process_mcm_setup_modify_eutra_R9

DESCRIPTION 
  This function processes setup or modify of EUTRA IRAT measurement IE from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA measurement IE from MCM is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_mcm_setup_modify_eutra_r9
(
  rrc_InterRATMeasurement_r9 *mcm_irat_ptr,
  l1_meas_ctrl_parm_struct_type *l1_inter_rat_meas_params,
  uint32 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
)
{
  l1_inter_sys_meas_ctrl_struct_type *temp_inter_s_meas_ctrl =  rrc_malloc(sizeof(l1_inter_sys_meas_ctrl_struct_type));

  memset(temp_inter_s_meas_ctrl,0,sizeof(l1_inter_sys_meas_ctrl_struct_type));

  l1_inter_rat_meas_params->meas_object_info_included = TRUE;
  l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS_EUTRA;

  /*Fill the measurement quantity*/
  if(!RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATMeasQuantity))
  {
    WRRC_MSG0_HIGH("Measurement Object Absent");
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_meas_quantity_incl = FALSE;
  }
  else
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_meas_quantity_incl = TRUE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.system_choice = L1_EUTRA_SYSTEM;

    /* Meas Quantity Parameter 1 i.e. Quality Estimate */
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(mcm_irat_ptr->interRATMeasQuantity,
          measQuantityUTRAN_QualityEstimate)) &&
         (RRC_CHECK_COMMON_MSG_TYPE(mcm_irat_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo,
         rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd)))
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan_incl = TRUE;
      /* Meas Quan i.e ECI0/RSCP/RSSI... */
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan.quan_type =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(mcm_irat_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      /* Store filter coefficient */
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.intra_meas_quan.filter_coef = (uint16)
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(mcm_irat_ptr->interRATMeasQuantity.
          measQuantityUTRAN_QualityEstimate.filterCoefficient);
    }

    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef = L1_FILTER_COEF_0;
/*    if(RRC_MSG_COMMON_BITMASK_IE(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA,))*/
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef =
          RET_EUTRA_FILTER_COEFFICIENT(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA->filterCoefficient);
    }
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan =
      RET_EUTRA_MEAS_QUAN(mcm_irat_ptr->interRATMeasQuantity.ratSpecificInfo.u.e_UTRA->measurementQuantity);
    WRRC_MSG2_HIGH("EUTRA meas quan %d, filter_coeff %d",
        l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan,
        l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.filter_coef);
  }

  /*Reporting Quantity*/
  if(!RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATReportingQuantity))
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_rpt_quantity_incl = FALSE;
  }
  else
  {
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_rpt_quantity_incl = TRUE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.utran_est_qual = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.sys_choice = L1_EUTRA_SYSTEM;
  
    if(rrc_InterRATReportingQuantity_r8_reportingQuantity_measurementQuantity == mcm_irat_ptr->interRATReportingQuantity.ratSpecificInfo.u.eutra->reportingQuantity)
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan = 
          l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.meas_quan.u.eutra_meas_quan.meas_quan;
    }
    else
    {
      l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan = L1_EUTRA_QUAN_BOTH;
    }
    WRRC_MSG1_HIGH("EUTRA reporting Quan %d",l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_quan.u.eutra_rpt_quan);
  }

  /*Fill Reporting Criteria*/
  if(FAILURE == rrcirat_process_mcm_report_criteria(&mcm_irat_ptr->reportCriteria,temp_inter_s_meas_ctrl,
    mcm_rpt_crit_ptr, transaction_id))
  {
    rrc_free(temp_inter_s_meas_ctrl);
    return FAILURE;
  }

  l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_cell_status_incl =
    temp_inter_s_meas_ctrl->rpt_cell_status_incl;
    WCDMA_MEMCPY(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_cell_status,
                 sizeof(l1_meas_rpt_cell_status_struct_type),
                 &temp_inter_s_meas_ctrl->rpt_cell_status,
                 sizeof(l1_meas_rpt_cell_status_struct_type));

    WCDMA_MEMCPY(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_criteria,
                 sizeof(l1_inter_sys_reporting_criteria_struct_type),
                 &temp_inter_s_meas_ctrl->rpt_criteria,
                 sizeof(l1_inter_sys_reporting_criteria_struct_type));

  l1_inter_rat_meas_params->reporting_mode.rpt_mode = 
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.rpt_criteria.rpt_mode;

  /*Measurement Objects*/
  if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mcm_irat_ptr,interRATMeasurementObjects)) &&
    (RRC_CHECK_COMMON_MSG_TYPE(mcm_irat_ptr->interRATMeasurementObjects,
      rrc_InterRATMeasurement_r9_interRATMeasurementObjects_eutra_FrequencyList)))
  {
    if(FAILURE == rrcirat_process_eutra_freq_list_mcm_r9(mcm_irat_ptr->interRATMeasurementObjects.u.eutra_FrequencyList,
        &l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_freq_list))
    {
      rrcmeas_mcf_struct_type rrcmeas_mcf;
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set error choice */
     rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      return FAILURE;
    }
    l1_inter_rat_meas_params->meas_object.u.inter_s_eutra.eutra_freq_list_incl = TRUE;
  }
  rrc_free(temp_inter_s_meas_ctrl);
  return SUCCESS;
}

#endif
#endif
/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R9

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_mcm_r9
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the INTER-RAT Measurement Control Message */
  uint8 count = 0, cnt = 0;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_rat_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_SYS;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;
  rrcmeas_status_e_type result;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  rrc_InterRATMeasurement_r9 *irat_meas_cmd_ptr = NULL;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;


  rrcmeas_interrat_type interrat_type = RRCMEAS_IRAT_GSM;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  rrcirat_eutra_freq_type temp_freq_list[L1_MAX_EUTRA_FREQ];
#endif

  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;
  uint16 meas_id = 0;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  mcm_r9 = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.later_r8_criticalExtensions.u.r9;


  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;


    /* Check if extended measurement is supported. If meas id extension is given then
     override the meas id given in legacy IE */
  if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
  {
    meas_id = meas_id_val;
  }
  else
  {
    meas_id  = (uint16) rrc_mcm_ptr->measurementIdentity;
  }


  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r9_setup))
  {
    /* Double check that it is INTER-RAT Meas */
    if (!(RRC_CHECK_COMMON_MSG_TYPE_PTR(rrc_mcm_ptr->measurementCommand.u.setup,
        rrc_MeasurementType_r9_interRATMeasurement)))
    {
      WRRC_MSG1_HIGH("UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r9,interRATMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      rrc_InterRATMeasurement_r9,interRATReportingQuantity)) &&
      (
      ((RRC_CHECK_COMMON_MSG_TYPE
           (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
             rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm)) &&
           (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
             interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity !=
             rrc_MeasurementQuantityGSM_dummy)) 
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
           ||
           (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
             rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA))
#endif
       )&&
#ifndef FEATURE_WCDMA_CM_LTE_SEARCH
      (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
      interRATReportingQuantity.ratSpecificInfo,
      rrc_InterRATReportingQuantity_ratSpecificInfo_gsm) 
      ) &&
#endif
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/ReportCriteria/R.Mode");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria,
      rrc_InterRATReportCriteria,interRATReportingCriteria)) &&
      (!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria,interRATEventList)) ||
      (rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->reportCriteria.
      u.interRATReportingCriteria->interRATEventList.n == 0)))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid, event criteria list absent ");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement ;
    if((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
       (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
        T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))
    {
      WRRC_MSG0_ERROR("Quality Estimate is TDD, which is not supported");
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }


#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    /*Check if one element is LTE IRAT and other are GSM, then send a failure with cause invalid configuration*/
    if(RRC_CHECK_COMMON_MSG_TYPE(
         rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
         rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA))
    {
      interrat_type = RRCMEAS_IRAT_EUTRA;
    }
    if((rrc_wtol_cm_support_nv == FALSE) && (interrat_type == RRCMEAS_IRAT_EUTRA))
    {
      WRRC_MSG0_HIGH("wtol cm support nv is not enabled");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
    if(
        ((RRCMEAS_IRAT_EUTRA == interrat_type) &&
          (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
              interRATMeasurementObjects)) && 
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
               rrc_InterRATMeasurement_r9_interRATMeasurementObjects_eutra_FrequencyList))) ||
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
               ratSpecificInfo, rrc_InterRATReportingQuantity_r8_ratSpecificInfo_eutra)))) ||
        ((RRCMEAS_IRAT_GSM == interrat_type) &&
        (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
             interRATMeasurementObjects)) && 
             (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
               rrc_InterRATMeasurement_r9_interRATMeasurementObjects_interRATCellInfoList))) ||
           (!RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
               ratSpecificInfo, rrc_InterRATReportingQuantity_r8_ratSpecificInfo_gsm))))
       )
    {
      WRRC_MSG0_ERROR("LTOW: Incompatible choice of RAT choice in objects, meas quantity, reporting quantity");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }
#endif
  }
  else
  {
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    /*Derive if the measurement type is of LTE IRAT based on the stored measurement ID list*/
    /* Store meas in MEASUREMENT_IDENTITY List */
    if(rrcirat_is_meas_id_eutra_meas((uint16) meas_id))
    {
      interrat_type = RRCMEAS_IRAT_EUTRA;
    }
    if((rrc_wtol_cm_support_nv == FALSE) && (interrat_type == RRCMEAS_IRAT_EUTRA))
    {
      WRRC_MSG0_HIGH("wtol cm support nv is not enabled");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

    /*Validations for measurement modify*/
    if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r9_modify)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify,measurementType)))
    {
      /*
        As of now If interrat rat type is EUTRA, then, no other type should be GSM
        TBD: Can N/W switch EUTRA<->GSM with modify ?
      */
      if(
          ((RRCMEAS_IRAT_EUTRA == interrat_type) &&
           (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasurementObjects)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
              rrc_InterRATMeasurement_r9_interRATMeasurementObjects_eutra_FrequencyList))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
              rrc_InterRATMeasQuantity_r8_ratSpecificInfo_e_UTRA ))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATReportingQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATReportingQuantity.ratSpecificInfo,
              rrc_InterRATReportingQuantity_r8_ratSpecificInfo_eutra ))))) ||
          ((RRCMEAS_IRAT_GSM == interrat_type) &&
           (((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasurementObjects)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
              rrc_InterRATMeasurement_r9_interRATMeasurementObjects_interRATCellInfoList))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATMeasQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo,
              rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm ))) ||
           ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR
                (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,interRATReportingQuantity)) &&
            (!RRC_CHECK_COMMON_MSG_TYPE(
              rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATReportingQuantity.ratSpecificInfo,
              rrc_InterRATReportingQuantity_r8_ratSpecificInfo_gsm )))))
        )
      {
        WRRC_MSG0_ERROR("LTOW: Incompatible choice of RAT choice in objects, meas quantity, reporting quantity");
  
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
    }
#endif

    if((rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r9_modify) &&
        (rrc_mcm_ptr->measurementCommand.u.modify->m.measurementTypePresent))
    {
      irat_meas_cmd_ptr = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement;
      if(
          ((irat_meas_cmd_ptr->m.interRATMeasQuantityPresent) &&
            (
#ifndef FEATURE_WCDMA_CM_LTE_SEARCH
              (irat_meas_cmd_ptr->interRATMeasQuantity.ratSpecificInfo.t != T_rrc_InterRATMeasQuantity_r8_ratSpecificInfo_gsm) ||
#endif
              ((irat_meas_cmd_ptr->interRATMeasQuantity.m.measQuantityUTRAN_QualityEstimatePresent) &&
                (irat_meas_cmd_ptr->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.modeSpecificInfo.t !=
                 T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd))))
        )
      {
        WRRC_MSG0_ERROR("Configuration not supported");
        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        /* Set faliure choice */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }
    }
  }


  if (((RRCMEAS_IRAT_GSM == interrat_type) && (rrcmcm_is_dualmode_enabled() == FALSE)) 
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
       ||((RRCMEAS_IRAT_EUTRA == interrat_type) && (rrcmcm_is_lte_mode_enabled() == FALSE))
#endif
       )
  {
    WRRC_MSG1_HIGH("REL9:IRAT:Mode not enabled, IRAT_Type %d",interrat_type);

    /* Send Meas Ctrl Failure Message */
  
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
    /* Set error choice */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
  
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    rrc_free(l1_inter_rat_meas_params);
    return FAILURE;
  }

  /* Set Transaction Id and Meas Id */
  l1_inter_rat_meas_params->trans_id = transaction_id;
  l1_inter_rat_meas_params->meas_id =   meas_id;


  /* Assign default values to inter_rat_meas_params */
  l1_inter_rat_meas_params->dpch_cm_status_incl = FALSE;
  l1_inter_rat_meas_params->meas_object_info_included = FALSE;
  l1_inter_rat_meas_params->num_add_meas = 0;
  l1_inter_rat_meas_params->rpt_mode_incl = FALSE;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  if(RRCMEAS_IRAT_EUTRA == interrat_type)
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS_EUTRA;
    memset(&l1_inter_rat_meas_params->meas_object.u.inter_s_eutra,0,sizeof(l1_inter_sys_eutra_meas_ctrl_struct_type));
  }
  else
#endif
  {
    l1_inter_rat_meas_params->meas_object.meas_type = L1_INTER_SYS;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_add = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_meas = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.num_cell_rmv = 0;
    l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list.cells_for_meas_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_cell_status_incl = FALSE;
    l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = FALSE;
    /* The following is not supported in Rel 99 anymore. So set it to FALSE */
    l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff = FALSE;
  }


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r9_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_rat_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_rat_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,
      measurementReportingMode))
  {
    /* Store reporting mode to TRUE */
    l1_inter_rat_meas_params->rpt_mode_incl = TRUE;
  
    l1_inter_rat_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
  }

  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r9_setup))
  {
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_SETUP;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    if(RRCMEAS_IRAT_EUTRA == interrat_type)
    {
      uecomdef_status_e_type status;
      /*Take a back up to temp list*/
      WCDMA_MEMCPY(temp_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                   rrcirat_eutra_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
      rrcirat_eutra_freq_list_ptr = temp_freq_list;

      /*Call the function passing the MCM setup structure that updates LTE info to the l1_inter_rat_meas_params*/
      status = rrcirat_process_mcm_setup_modify_eutra_r9(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
                             l1_inter_rat_meas_params,transaction_id,&mcm_rpt_crit);
      rrcirat_eutra_freq_list_ptr = rrcirat_eutra_freq_list;

      if(FAILURE == status)
      {
        rrc_free(l1_inter_rat_meas_params);
        return status;
      }
    }
    else
#endif
    {
      /* Set Meas Identity */
      l1_inter_rat_meas_params->meas_id = meas_id;

      /* Set Meas Quantity as Inter-sys Meas Quan present */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;

      /* Initialize Default Value to intra_freq_meas_quan_incl */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl =  FALSE;

      /* Initialize Filter Coefficient to Default Value i.e. 0 */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

      /* Initialize GSM Filter Coefficient to 0 */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
      L1_GSM_FILTER_COEF_0;

      /* Meas Quantity Parameter 1 i.e. Quality Estimate */
      if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
        interRATMeasQuantity,rrc_InterRATMeasQuantity_r8,measQuantityUTRAN_QualityEstimate))
      {
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;

        /* Meas Quan i.e ECI0/RSCP/RSSI... */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.setup->
          u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
          modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

        /* Get Filter Coefficient too */
        /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
        
          /* Store filter coefficient */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
            RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.setup->u.
            interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.filterCoefficient);
        
      } /* End of quality estimate present */

      /* Set System Choice to GSM */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;

      /* Get GSM Meas Quan, Filter Coefficient, BSIC required */

      /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
      So set it to RSSI */
      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
        L1_GSM_CARRIER_RSSI;

        /* GSM Filter Coefficient present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
          RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.
          u.setup->u.interRATMeasurement->interRATMeasQuantity.ratSpecificInfo.u.gsm->filterCoefficient);

      l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
        RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
        interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);

      /* Store Reporting Quantity */
      l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;

      /* Sys Choice */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;
      /* L1 wants this to be FALSE for the first version as it does not support it */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;

      /* Ignore reading Observed time difference as no longer supported in Rel 99
      l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
      rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
      ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */

      /* Check if gsm_carrier_rssi wanted in Meas Report */
      l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
        rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATReportingQuantity.
        ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;

      if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->
        u.interRATMeasurement->reportCriteria, &l1_inter_rat_meas_params->meas_object.u.inter_s,
        &mcm_rpt_crit, transaction_id))
      {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

      /* For L1 sake, copy reporting criteria */
      l1_inter_rat_meas_params->reporting_mode.rpt_mode =
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;

      l1_inter_rat_meas_params->meas_object_info_included = TRUE;

      /* Time to process the cell list */
      if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
        interRATMeasurementObjects)) && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
          rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList)))
      {
        result = rrcirat_process_cell_list_mcm_r6(rrc_mcm_ptr->measurementCommand.
          u.setup->u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
          &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
        if(result == RRCMEAS_FAIL)
        {
          /* Send Meas Ctrl Failure Msg */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
         rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        else if(result == RRCMEAS_UNSUPPORTED)
        {
           /* Send Meas Ctrl Failure Message */
            WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
           /* Send Meas Ctrl Failure Msg */
  
           rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
           /* Set faliure t */
           rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
           rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_rat_meas_params);
           return FAILURE;
       }
        /* Set Cell list is present */
        l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
      }
    }
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,
       &addtl_meas_incl,primary_meas_type,meas_id,&meas_validity) == FAILURE)
    {
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
    }

    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
      interRATMeasurementObjects))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects,
        rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList))
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
  
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }
    }
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
    if(RRCMEAS_IRAT_EUTRA == interrat_type)
    {
      /*Copy back the temp freq list to orig freq list*/
      WCDMA_MEMCPY(rrcirat_eutra_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                   temp_freq_list,
                   L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
    }
#endif
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id =  meas_id;
        inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        inter_rat_meas_id_list[count].interrat_type = interrat_type;
        meas_exists_for_setup = TRUE;
        break;
      }
    }

    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTER_RAT_MEAS; count++ )
      {
        /* First check if this meas already exists */
        if (inter_rat_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          if(meas_id_val != 0)
          {
            inter_rat_meas_id_list[count].meas_id = meas_id_val;
            inter_rat_meas_id_list[count].meas_id_legacy = rrc_mcm_ptr->measurementIdentity;
          }
          else
          {
            inter_rat_meas_id_list[count].meas_id = rrc_mcm_ptr->measurementIdentity;
          }
          inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          inter_rat_meas_id_list[count].interrat_type = interrat_type;
          break;
        }
      }

    } /* End of if */

    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
        meas_id);
      WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
    }

    /* Now one needs to send command to L1 */

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_modify)
  {
    /* Set type to MODIFY */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_MODIFY;

    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r9_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("MCM:I.RAT Modify: MType absent");

      /* Check for additional measurements */
     if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id, &meas_validity) == FAILURE)
    {
      rrc_free(l1_inter_rat_meas_params);
      return FAILURE;
    }

     if (addtl_meas_incl == TRUE)
     {
       rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
         meas_id);
       WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
      }
    }
    else
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
          rrc_MeasurementType_r9,interRATMeasurement)))
      {
        WRRC_MSG0_ERROR("Modify:Meas Id is not Inter-RAT");
        /* Send Meas Ctrl Failure  */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_rat_meas_params);
        return FAILURE;
      }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
      if(RRCMEAS_IRAT_EUTRA == interrat_type)
      {
        uecomdef_status_e_type status;
        /*Take a back up to temp list*/
        WCDMA_MEMCPY(temp_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                     rrcirat_eutra_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
        rrcirat_eutra_freq_list_ptr = temp_freq_list;

        /*Call the function passing the MCM modify structure that updates LTE info to the l1_inter_rat_meas_params*/
        status = rrcirat_process_mcm_setup_modify_eutra_r9(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
                               l1_inter_rat_meas_params,transaction_id,&mcm_rpt_crit);
        rrcirat_eutra_freq_list_ptr = rrcirat_eutra_freq_list;
        if(FAILURE == status)
        {
          rrc_free(l1_inter_rat_meas_params);
          return status;
        }
      }
      else
#endif
      {
        /* Initialize Filter Coefficient to Default Value i.e. 0 */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = 0;

        /* Initialize GSM Filter Coefficient to 0 */
        l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
        L1_GSM_FILTER_COEF_0;

        if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          interRATMeasurement,rrc_InterRATMeasurement_r9,interRATMeasQuantity)))
        {
          WRRC_MSG0_HIGH("Meas Quan absent");
        }
        else
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan_incl = TRUE;
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = FALSE;
          if (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity.ratSpecificInfo.u.gsm->measurementQuantity ==
            rrc_MeasurementQuantityGSM_dummy)
          {
  
            WRRC_MSG0_ERROR("MCM:MeasQuantity PathLoss");
  
            /* Tr Id */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
  
            /* Set Error Cause */
            rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);
            return FAILURE;
          }
  
          /* Meas Quantity Parameter 1 i.e. Quality Estimate */
          if (RRC_MSG_COMMON_BITMASK_IE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity,rrc_InterRATMeasQuantity,measQuantityUTRAN_QualityEstimate))
          {
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan_incl = TRUE;
  
            /* Meas Quan i.e ECI0/RSCP/RSSI... */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.quan_type =
              RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
              u.interRATMeasurement->interRATMeasQuantity.measQuantityUTRAN_QualityEstimate.
              modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);
  
            /* Get Filter Coefficient too */
            /* Check if rrc_IntraFreqMeasQuantity_filterCoefficient_present */
          
              /* Store filter coefficient */
              l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.intra_meas_quan.filter_coef = (uint16)
                RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u
                .modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
                measQuantityUTRAN_QualityEstimate.filterCoefficient);
            
          }
  
          /* Set System Choice to GSM */
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.system_choice = L1_GSM_SYSTEM;
  
          /* Get GSM Meas Quan, Filter Coefficient, BSIC required */
  
          /* Acc to specs, this quan can never be GSM_PATHLOSS(Error already taken care of.
          So set it to RSSI */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.meas_quan =
            L1_GSM_CARRIER_RSSI;
  
            /* GSM Filter Coefficient present */
            l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.filter_coef =
              RET_GSM_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
              modify->measurementType.u.interRATMeasurement->interRATMeasQuantity.
              ratSpecificInfo.u.gsm->filterCoefficient);
          
  
          l1_inter_rat_meas_params->meas_object.u.inter_s.meas_quan.u.gsm_meas_quan.bsic_ver_req =
            RET_BSIC_VERIFICATION_REQD(rrc_mcm_ptr->measurementCommand.u.
              modify->measurementType.u.interRATMeasurement->
            interRATMeasQuantity.ratSpecificInfo.u.gsm->bsic_VerificationRequired);
  
        } /* End of else of Meas Quan present */
  
        /* Check if reporting quan present */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
           rrc_InterRATMeasurement_r9,interRATReportingQuantity))
        {
          /* Store Reporting Quantity */
          l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_quan_incl = TRUE;
  
          /* Sys Choice */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.sys_choice = L1_GSM_SYSTEM;
  
          /* L1 wants this to be FALSE for the first version as it does not support it */
          l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.utran_est_qual = FALSE;
  
          /* Observed time difference no longer supported in Rel 99
          l1_inter_rat_meas_params.meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.time_diff =
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATReportingQuantity.ratSpecificInfo.u.gsm->observedTimeDifferenceGSM; */
  
            /* Check if gsm_carrier_rssi wanted in Meas Report */
            l1_inter_rat_meas_params->meas_object.u.inter_s.inter_sys_quan.gsm_rpt_quan.rssi_rpt_ind =
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
            interRATReportingQuantity.ratSpecificInfo.u.gsm->gsm_Carrier_RSSI;
        }
  
        if(FAILURE == rrcirat_process_mcm_report_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->
          measurementType.u.interRATMeasurement->reportCriteria,
          &l1_inter_rat_meas_params->meas_object.u.inter_s, &mcm_rpt_crit, transaction_id))
        {
          rrc_free(l1_inter_rat_meas_params);
          return FAILURE;
        }
        l1_inter_rat_meas_params->reporting_mode.rpt_mode =
        l1_inter_rat_meas_params->meas_object.u.inter_s.rpt_criteria.rpt_mode;
        
        l1_inter_rat_meas_params->meas_object_info_included = TRUE;
        if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
          rrc_MeasurementCommand_r9_modify,measurementType))
          && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
           rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList)) )
        {
          /* Time to process the cell list */
          result = rrcirat_process_cell_list_mcm_r6(rrc_mcm_ptr->measurementCommand.
            u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            &l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list);
            if(result == RRCMEAS_FAIL)
          {
            /* Send Meas Ctrl Failure Msg */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
  
            /* Set Error Cause */
            rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
  
            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_rat_meas_params);  
            return FAILURE;
          }
           else if(result == RRCMEAS_UNSUPPORTED)
           {
              /* Send Meas Ctrl Failure Message */
               WRRC_MSG0_ERROR("REL9:IFreq:Unable to fill Inter Freq");
              /* Send Meas Ctrl Failure Msg */
   
              rrcmeas_mcf.rrc_transaction_id = transaction_id;
   
              /* Set faliure t */
              rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
              rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
              rrc_free(l1_inter_rat_meas_params);  
              return FAILURE;
          }
  
          /* Set Cell list is present */
          l1_inter_rat_meas_params->meas_object.u.inter_s.cell_list_incl = TRUE;
        }
      }
      /* Check for additional measurements */
      /* Call fn which makes check for additional measurements */
     if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id,&meas_validity) == FAILURE)
     {
       rrc_free(l1_inter_rat_meas_params);
       return FAILURE;
     }


     if (addtl_meas_incl == TRUE)
     {
       rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
         meas_id);
       WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
      }


      /* Set reporting criteria in RRC DB */
      for (count=0; count < MAX_INTER_RAT_MEAS; count++)
      {
        if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
        {
          if(mcm_rpt_crit != NOT_AVAILABLE)
          {      
            inter_rat_meas_id_list[count].rpt_crit = mcm_rpt_crit;
            inter_rat_meas_id_list[count].interrat_type = interrat_type;
            WRRC_MSG0_HIGH("Reportinf Crit set in RRC DB");
          }
          break;
        }
      }
      
      if( (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r9_modify,measurementType))
        && (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects,
         rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList)) )
      {
        /* At this point everything has been validated and now is the time
        to update the inter rat CELL_INFO_LIST */
        for (count=0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_rat_cell_list[count],
                       sizeof(inter_rat_cell_list_struct_type),
                       &local_neighbor_list[count],
                       sizeof(inter_rat_cell_list_struct_type));
        }
      }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
      if(RRCMEAS_IRAT_EUTRA == interrat_type)
      {
        /*Copy the updated frequency list*/
        WCDMA_MEMCPY(rrcirat_eutra_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type),
                     temp_freq_list,
                     L1_MAX_EUTRA_FREQ * sizeof(rrcirat_eutra_freq_type));
      }
#endif

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
  * that this meas exists. So let us delete it from inter_rat_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_inter_rat_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_rat_meas_params->meas_id);

    /* Clear Meas Id from inter_rat_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (inter_rat_meas_id_list[count].meas_id == l1_inter_rat_meas_params->meas_id)
      {
        inter_rat_meas_id_list[count].meas_id = INVALID_MEAS_ID;

        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          inter_rat_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r9_setup))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement,
        interRATMeasurementObjects)))
    {
      if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
          interRATMeasurementObjects,rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList))
      {
        if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            interRATCellInfoIndication_r6))
        {
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.u.setup->u.interRATMeasurement->
            interRATMeasurementObjects.u.interRATCellInfoList->interRATCellInfoIndication_r6;
        }
        else
        {
          interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
          interRAT_cell_info_status.interRAT_cell_info_ind=0;
        }
      }
    }
  }

  if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r9_modify))
  {
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r9_modify,measurementType)) &&
       (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement,
        interRATMeasurementObjects)))
    {
      if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->
        interRATMeasurementObjects,rrc_InterRATMeasurement_r9_interRATMeasurementObjects,interRATCellInfoList))
      {
         if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList,
            interRATCellInfoIndication_r6))
        {
          interRAT_cell_info_status.interRAT_cell_info_present = TRUE;
          interRAT_cell_info_status.interRAT_cell_info_ind = rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            interRATMeasurement->interRATMeasurementObjects.u.interRATCellInfoList->interRATCellInfoIndication_r6;
        }
        else
        {
          interRAT_cell_info_status.interRAT_cell_info_present=FALSE;
          interRAT_cell_info_status.interRAT_cell_info_ind=0;
        }
      }
    }
  }

  if(!process_interfreq_interrat_meas_rel7)
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interRAT measurements from MCM to L1");
    rrc_free(l1_inter_rat_meas_params);  
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_rat_meas_params->trans_id = alloc_idx;

  WRRC_MSG0_MED("MCM:Sending cmd to L1");
  rrcmeas_send_l1_cmd(NULL, l1_inter_rat_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  rrc_free(l1_inter_rat_meas_params);
  return SUCCESS;
}

#endif
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /*WCDMA_REL10*/


