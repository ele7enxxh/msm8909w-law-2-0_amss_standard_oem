/*===========================================================================
            R R C   M E A S U R E M E N T S

DESCRIPTION

   This module supports Inter Freq Measurement Control and Measurement Reporting
   for the RRC layer.


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcifreq.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   vs      f3 density reduction changes
05/12/16   vs      Adding log packet for measurement related logging
08/24/15   sas     Made Changes to implement 3Gpp CR3382 related to multiple PLMNs list in SIB18.
08/04/14   bc      Made changes to fix compiler warnings.
06/17/14   bc      Made changes to check if no of elements in mfbi list are greater 
                   than the element we are trying to access.
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/08/14   vi      Made changes to skip cells under unspported frequency
03/13/14   vi      Stack reduction changes
03/11/14   vi     Made changes to disable IRAT and IFREQ measurements in different states based on the NV rrc_disable_meas_nv
01/27/14   sr      Added NV control for MFBI support
01/03/14   sr      Added code changes for MFBI support
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
12/13/13   vi      Modified the code to ignore the invalid cell id configured in cellsforInterfreqmeaslist/cellsforinterRATmeaslist 
                    and sending MCF only when the whole list is invalid.
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   bc      Checked in REL 10 spec crs changes
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/28/13   sr      Added code for measurement identity extension
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
12/12/12   mp      Fixing KW errors
11/29/12   sn      Backed out changes to send hcs_used as FALSE to L1 if SIB19 has valid priority info.
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
08/15/12   vg      fixed merging error which was causing a crash.
08/13/12   sr      Fixed merging error caused while mainlining the FEATURE_SIB18
12/16/11   gv      Fixed compilation issue
12/12/11   gv      Added code to support free floating of tasks
11/8/11    vg      modified code to fill the meas result correctly.
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/16/11   sks     Fixed compilation error when FEATURE_WCDMA_ABSOLUTE_PRIORITY is not defined.
06/15/11   vg      Made changes to send hcs_used as FALSE to L1 if SIB19 has valid priority info.
05/03/11   rl      Fixed the lint error
04/26/11   rl      Support for transition from TMC heap to Modem Heap in WCDMA.
04/25/11   rl      Fixed the KW error
03/01/11   rl      Fixed porting issues on enabling 
                   FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/13/10   rm      Added code to process freqQualityEstimateQuantity-FDD only when
                   FDD mode is chosen
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   sks     Fixed KW errors.
10/29/10   su      Made changes to remove WCDMA_TASKLOCK() usage from RRC code base.
10/11/10   ss      ASN.1 migration to ASN1 9.3.0
10/11/10   ss      Upgraded SIB11Bis code to ASN 8.a and added code to init sibs lookup table 
                   after initialization of rrc_nv_rel_indicator.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
04/23/10   as      Fixed Klocwork issues
11/05/09   rm      Made changes to ignore the removal of cell id which is not present at UE and
                   continue processing the MCM 
10/26/09   rm      Fixed Klockwork errors
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
08/21/09   rm      Fixed Lint Warnings
07/20/09   rm      Added code to not call sys_eplmn_list_equivalent_plmn when n/w sel mode is
                   LIMITED
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
11/21/08   rm      Made changes to unblock confguring L1 with Inter Frequency measurements.
                   Changes are under FEATURE_WCDMA_REL7
10/13/08   rm      Merged changes for FEATURE MEAS TRID ENHNC for Rel7
10/08/08   rm      Made changes to skip sending InterFreq and Inter RAT measurements configured by SIB11, SIB12 and MCM to L1
                   Changes are under FEATURE_WCDMA_REL7
09/12/08   rm      Added code for REl7 under FEATURE_WCDMA_REL7
09/02/08   cnp     Replaced intlock/free with wcdma_intlock/free.
           cnp     Replaced tasklock/free with wcdma_tasklock/free.
           cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.
07/16/08   rm    Enhanced the handling of transaction id received in MCM message so that
                 UE accepts the MCM's with same transaction id and in case of failure UE
                 sends MCM failure for a transaction id only if it is latest among the same
                 transactions ids
06/30/08   ns    Made the necessary changes to enable the functionality of the feature 
                 FEATURE NETWORK SHARING only when the GCF NV is set
03/10/08   rm    Added support for SIB11bis. Code changes are under feature flag 
                 FEATURE_SIB11BIS_SUPPORT  
12/27/07   rm    Added code to free the memory allocated for the cell list in
                 inter frequency measurement indication
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
09/07/07   vm    Added support for feature FEATURE_RRC_CALLOC
07/17/07   rm/vg Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
07/02/07   vm    Added newline at EOF to fix a warning
06/28/07   rm    Fixed memory leak when the validation of a measurement report
                 for inter frequency fails
04/20/07   da    Made changes needed to update ASN.1 version to R6 sept R6 2006
03/13/07   vg    modified MCF error cause to unsupported confi from protocol
                 error when DL arfcn band is unsupported by UE.
02/02/07   vm    Stability Opt: Removed freeing of already allocated memory
                 before hitting EF. This is to get the "correct" snapshot
                 of the heap instead of a "stale" one.  
11/02/06   vg    removed FEATURE_WCDMA_PATHLOSS feature flag
10/06/06   vg    modified MCM modify additional meas handling
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases.
08/30/06   vg    added code for MCF error cause.
08/29/06   sm/da Made changes required for R6 ASN.1 migration.  Also merged
                 FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/22/06   sgk   Added support for the VASET feature and inter frequency
                 cells for measurement.
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under
                 FEATURE REL5 while retaining the HSDPA specifc processing
                 under FEATURE HSDPA.
04/28/06   sgk   Added code to fix error in rel5 extension processing in rel 5
                 MCM.
04/18/06   da    Modified code to support migration to Mar-06 Rel-5 ASN1 +
                 R6 Hacks
03/14/06   kp    Lint clean-up
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
01/25/06   bd    Lint clean-up
01/11/06   sgk   Made fix to correctly add the delta rscp values to the
                 measurement report when there are multiple frequencies.
11/10/05   sgk   Added rrc_nv_rel_indicator check before adding the extensions
                 for the measurement report.
                 Added check for rrc_nv_rel_indicator to be equal to
                 RRC_NV_REL_INDICATOR_REL5 before processing the rel 5 ies.
10/21/05   sgk   Added rel5 support for the processing of delta qrxlevmin,
                 delta qhcs, delta threshold used frequency that is used in
                 the evaluation of events 2b, 2d, 2f and delta threshold non
                 used frequency that is used in the evaluation of events
                 2a, 2b, 2c, 2e.
09/09/05   sgk   Rolled back change to reject MCM if it has more than 2 dl fq
                 at the request of ast.
09/08/05   sgk   Added code to reject MCM if it has more than 2 dl frequencies.
09/07/05   sgk   Added additional check cell_id < L1_MAX_CELL_MEAS before
                 accessing it in rrcifreq_build_composite_cell_list function
                 to satisfy Coverity tool.
09/02/05   sgk   Modified code to set the inter frequency rpt mode to
                 L1_INTER_FREQ_NO_CHANGE and send the MCM modify down to
                 L1 instead of rejecting it, in the case where the cell list
                 is included in the modify, the report criteria is set to
                 inter frequency report criteria and the event list is not
                 included or NULL.
08/26/05   sgk   Added code to cover scenario where the remove cell list IE
                 of sib12 is absent when building the composite list from
                 Sib11 and Sib12.
08/15/05   sgk   Added code to check if the PLMN from SIB18 is the selected
                 PLMN, and if so allow the cell for reselection, instead of
                 calling the nas function to check if it belongs to the
                 equivalent list.
                 Added code for sib11 hcs processing to copy the frequency
                 info from the previous cells if not present.
08/01/05   sgk   Made the correct bit mask check against
                 rrc_NewInterFreqCellSI_HCS_ECN0_frequencyInfo_present, for
                 checking if the freq info was present for hcs ECNO.
07/29/05   sgk   Brought back change to block cells with frequencies from
                 unsupported bands from going down to L1.
07/19/05   sgk   Modified function rrcmeas_fill_event_criteria to support
                 passing of sib11 and 12 non critical extension ptrs.
07/13/05   sgk   Added rel5 non critical extensions for rel 99 MCM ie the
                 transaction identifier extension.
06/27/05   sgk   Added basic rel 5 measurement ctrl msg support.
06/23/05   sgk   Mapped default QHCS values based on RCSP and ECN0.
                 Fixed lint erros
06/07/05   sgk   Rolled back the check to turn off sib18 if the selected
                 PLMN at rrc is not the same as the one in NAS. SIB18 will
                 be applied even in this case, to block cells for reselection
                 based on the NAS equivalent PLMN list.
05/03/05   vk    Used the new functional interface to set additional meas
                 in DB when FEATURE HSDPA is defined
05/10/05   sgk   Allowed cells with frequencies from unsupported bands to go
                 down to L1 temporarily to support testing until scripts are
                 updated.
04/21/05   sgk   Added HCS related fixes.
04/20/05   sgk   Added blocking of SIB18 processing when the registerd PLMN
                 != selected PLMN.
04/15/05   sgk   Added ASN1 Rel 5 support for HCS.
04/07/05   sgk   Added HCS support.
04/01/05   sgk   Added Code to block SIB18 processing in true limited service.
03/28/05   sgk   Added GCF fix to copy the SIB18 related measure_in_idle_pch_fach
                 flag from SIB11 to SIB12 when SIB12 is present but does not
                 have inter freq measurement information.
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/09/05   sgk   Lint fixes.
03/07/05   sgk   Added SIB18 support for inter Freq measurements in idle and
                 connected mode.
03/02/05   ttl   Support RRC ASN1 Release 5.
02/10/05   sgk   Added sib18 initial support during processing of inter freq
                 cells of sib11.
12/21/04   sgk   Lint fix removed  function parameter tmp_inter_freq_cell_list
                 from rrcifreq_build_composite_cell_list as it is now global
                 and modified the locations calling this function.
12/11/04   sgk   Made inter_freq_sib12_new_cell_list a global variable to fix
                 stack over flow problem.
12/08/04   sgk   Fixed compiler warnings.
12/03/04   vk    While sending Inter Freq Measurement Report, added check to
                 include path loss measured results
12/02/04   sgk   Update the cell list with dl freq information from the previous
                 cell instead of the first cell when the optional IE DL Freq
                 info is absent.
                 Lint Fixes for SIB12 code.
11/23/04   sgk   Added initial SIB12 Inter Frequency support.
10/15/04   vk    Put the change where RRC forcibly sets the reporting mode
                 to L1_INTER_FREQ_NO_CHANGE in case network signals VASET
                 from MCM under FEATURE_VASET to mask LINT warning
10/04/04   vk    Corrected non freq threshold parameter handling for Event 2E
08/17/04   vk    If reporting criteria is specified as intra freq, then forcibly
                 set it to L1_INTER_FREQ_NO_CHANGE so that L1 can act on new inter
                 freq cells
08/05/04   vk    Fixed previous Lint Error
07/30/04   vk    Filtered out Inter Freq neighbors that are outside the band
                 from SIB11 and in case network sends these neighbors in Meas
                 Ctrl Msg, send a Meas Ctrl Failure Message
07/27/04   vk    Made changes as per March 04 ASN1
05/17/04   vk    Initialized measurement validity to NOT_PRESENT instead of
                 CELL_DCH in Measurement Database. Added meas validity as
                 parameter to rrcifreq_config_inter_freq_db fn.
05/11/04   vk    Removed the setting of countC-SFN-Frame-difference parameter
                 from Meas Report as it is not required
05/04/04   vk    Corrected used freq parameter for Inter Freq when
                 sending Meas cmd to L1. Also used the freq info from the first
         inter freq cell when it is not present in the second cell
04/06/04   vk    Cleaned up Lint Warnings
03/26/04   vk    For event 2D and Event 2F, retrieve the value of reporting
                 cell status as it is valid
03/15/04   bu    Removed the use of FEATURE DUALMODE BASELINE. This feature is
                 always defined (even for singlemode builds).
01/12/04   vk    While processing Inter Freq Meas Report, now parse Inter freq
                 cells list ptr instead of the static cell list for a particular
                 freq
11/07/03   vk    Added check to verify that additional measurements corresponding
                 to a primary measurement can never be greater than the maximum
                 allowed
11/06/03   vk    Added support for Inter Freq Measurements
06/03/03   vk    Register for L2 ack status when sending report in Ack Mode
05/29/03   vk    Added code for Implementing Additional Measurements for
                 Inter Freq Measurements. Removed FEATURE_2D_2F_MEAS.
01/04/03   vk    Added code to read inter-freq measurement quantity. Previously
                 it was hard coded to Ec/Io
01/22/03   vk    Set elements in structure reporting_mode while sending setup
                 command to L1
01/21/03   vk    Merged changes related to Inter-Freq Meas Report. Changed the
                 name of feature FEATURE_INTRA_INTER_FREQ_MEAS to FEATURE_2D_2F_MEAS
01/08/03   vk    Added fixes done during integration.
                 1. Corrected checking of the presence of Inter Freq Meas Quantity
                 2. Set meas_quan_incl to TRUE on presence of measurement quantity
                 3. Added reporting mode information for inter-frequency measurement
12/03/02   vk    Created initial file. Supports inter-freq events 2d and 2f
                 without compressed mode being activated. Code has been placed
                 under FEATURE_INTRA_INTER_FREQ_MEAS

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
#include "mm_umts.h"
#include "sys_eplmn_list.h"
#include "rrcdata_v.h"
#include "rrccsp.h"
#include "rrcsibproc.h"
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif
#include "rrccspi.h"
#include "rrcasn1util.h"
#include "seq.h"
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#include "rrcfreefloating.h"
#endif
#include "stringl.h"
#include "rrclog.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Inter-Freq Event Type */
#define  RET_INTER_FREQ_EVENT_TYPE_R6(event_type)  \
(  \
 ((event_type == T_rrc_InterFreqEvent_r6_event2a) ? L1_INTER_FREQ_EVENT_2A : \
  ((event_type == T_rrc_InterFreqEvent_r6_event2b) ? L1_INTER_FREQ_EVENT_2B : \
  ((event_type == T_rrc_InterFreqEvent_r6_event2c) ? L1_INTER_FREQ_EVENT_2C : \
  ((event_type == T_rrc_InterFreqEvent_r6_event2d) ? L1_INTER_FREQ_EVENT_2D : \
  ((event_type == T_rrc_InterFreqEvent_r6_event2e) ? L1_INTER_FREQ_EVENT_2E : \
  L1_INTER_FREQ_EVENT_2F ))))) \
)

#define  RET_INTER_FREQ_EVENT_TYPE(event_type)  \
(  \
 ((event_type == T_rrc_InterFreqEvent_event2a) ? L1_INTER_FREQ_EVENT_2A : \
  ((event_type == T_rrc_InterFreqEvent_event2b) ? L1_INTER_FREQ_EVENT_2B : \
  ((event_type == T_rrc_InterFreqEvent_event2c) ? L1_INTER_FREQ_EVENT_2C : \
  ((event_type == T_rrc_InterFreqEvent_event2d) ? L1_INTER_FREQ_EVENT_2D : \
  ((event_type == T_rrc_InterFreqEvent_event2e) ? L1_INTER_FREQ_EVENT_2E : \
  L1_INTER_FREQ_EVENT_2F ))))) \
)


/* Converts time to trigger for FDD from ASN1 format to L1 format */
#define  RET_INTER_FREQ_TIME_TO_TRIGGER(time_to_trigger)  \
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


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Local Object Definition
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern rrcmeas_substate_e_type rrcmeas_current_substate;
extern uint8 rrcmeas_pending_rpt;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* Maximum No of Inter RAT measurements based on additional meas */
inter_freq_meas_id_list_struct_type inter_freq_meas_id_list[MAX_INTER_FREQ_MEAS];

inter_freq_cell_list_struct_type inter_freq_cell_list[L1_MAX_CELL_MEAS];

inter_freq_cell_list_struct_type inter_freq_cell_list_tmp[L1_MAX_CELL_MEAS];

inter_freq_cell_list_struct_type tmp_inter_freq_cell_list[L1_MAX_CELL_MEAS];


#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

/*Boolean added to track if interfrequency and interrat measurements are to be processed or not*/
extern boolean process_interfreq_interrat_meas_rel7;

/*Boolean to track if inter frequency measurements are allowed or not
Inter freq measurements are allowed if process_interfreq is TRUE 
(Even in case process_interfreq_interrat_meas_rel7 is FALSE)

Use the same variable to unblock UE internal measurements also
*/
extern boolean process_interfreq_uei_meas_rel7;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;

extern uint16 Fdd1_internal_band[];
extern uint16 Fdd2_internal_band[];

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
extern boolean rrc_mfbi_support_nv;
#endif

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM

DESCRIPTION

    This function processes the Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
,rrc_DL_DCCH_Message* dcch_msg_ptr
,uint8 meas_ctrl_tr_id
)
{

  rrc_InterFreqEventList* ifreq_event_list_ptr = NULL;
  /* Process the INTER-FREQ Measurement Control Message */
  uint8 count = 0, cnt = 0;

  uint8 transaction_id = INVALID_TRANSACTION_ID;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

rrcmeas_status_e_type result;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrc_MeasurementControl_v590ext_IEs *rel5_ext_ptr = NULL;
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;
  uint32 idx=0;
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

  transaction_id = meas_ctrl_tr_id;
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    if((dcch_msg_ptr->message.u.measurementControl.u.r3.m.v390nonCriticalExtensionsPresent)
      && (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.m.
      v3a0NonCriticalExtensionsPresent) && (dcch_msg_ptr->message.u.
      measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.m.
      laterNonCriticalExtensionsPresent) &&
      (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.m.
        laterNonCriticalExtensionsPresent) &&
      (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.
      laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)
      && (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.
      laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)
      && (dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.v3a0NonCriticalExtensions.
      laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent))
    {
      rel5_ext_ptr = &(dcch_msg_ptr->message.u.measurementControl.u.r3.v390nonCriticalExtensions.
       v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
       v590NonCriticalExtensions.measurementControl_v590ext);
    }
  }


  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = (uint16) transaction_id;
  l1_inter_freq_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;



  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_setup))
  {

    /* Double check that it is INTER-FREQ Meas */
    if (!RRC_CHECK_COMMON_MSG_TYPE_PTR(rrc_mcm_ptr->measurementCommand.u.setup,
        rrc_MeasurementType_interFrequencyMeasurement))
    {
      WRRC_MSG1_HIGH("IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }


    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria,rrc_InterFreqReportCriteria_intraFreqReportingCriteria))
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    

    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      & rrc_InterFrequencyMeasurement,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
        
        l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
        l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }

      else
      {
        /* Since L1 does not support this, reject the MCM */
        WRRC_MSG0_HIGH("IFreq:Set Update t invalid");
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }

    if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria,
       rrc_InterFreqReportCriteria_interFreqReportingCriteria)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
      u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
        reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n> idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_event2f))
          {
            WRRC_MSG0_ERROR("IFreq:Ifreq Set update not set and evt id != 2d or 2f");
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;
          }
          idx++;
        }
      }
    }

    /* Process Reporting Criteria which is mandatory in case meas obj is present */
    if (rrcifreq_process_reporting_criteria(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f, rel5_ext_ptr) == FAILURE)

    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
    {
      
     /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList,
        transaction_id, &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    /* Fill cell list, cell info */
    result = rrcifreq_fill_inter_freq_info(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
                        l1_inter_freq_meas_params);

    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */
      
      /* Send Meas Ctrl Failure Msg */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
        
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl, INTER_FREQ, (uint32)rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement,measurementValidity))
    {
      
      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
    {

      WRRC_MSG0_ERROR("IFreq:Unable to set MeasId in DB");
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("IFREQ:Modify:InterFreq:MType absent");

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)

        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl,  INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          
        }
      } /* End of if of Additional Meas Present */

    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }

      if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria,rrc_InterFreqReportCriteria_intraFreqReportingCriteria) )&&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
        & rrc_IntraFreqReportingCriteria,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
         rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
         rrc_InterFrequencyMeasurement,interFreqSetUpdate))
      {
        if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
        {
 
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }

        else
        {
          /* Since L1 does not support this, reject the MCM */
          WRRC_MSG0_HIGH("IFreq:Set Update t invalid");
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }

      /* Process Reporting Criteria */
      if (rrcifreq_process_reporting_criteria(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f, rel5_ext_ptr) == FAILURE)
      {

        if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
          reportCriteria,rrc_InterFreqReportCriteria_interFreqReportingCriteria)) &&
          !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
          reportCriteria.u.interFreqReportingCriteria,
          rrc_InterFreqReportingCriteria,interFreqEventList))) &&
          (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
          reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {

          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);


      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      {
 

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
               addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
               &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }

        }
      } /* End of if of Additional Meas Present */
      else
      {
        WRRC_MSG0_HIGH("IFREQ:additional meas list absent");
      }


      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info(
        rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        l1_inter_freq_meas_params);

      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
         rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
              /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement,measurementValidity))
      {
        
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
      {
        WRRC_MSG0_ERROR("IFreq:Unable to set MeasId in DB");
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count],
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
 
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending interFreq measurements from MCM to L1");
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_freq_meas_params);
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_EVENT_CRITERIA

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

uecomdef_status_e_type rrcifreq_process_event_criteria
(
  rrc_InterFreqReportingCriteria* rrc_event_criteria_ptr,
  l1_inter_freq_report_crit_struct_type* l1_event_criteria_ptr
 ,rrc_MeasurementControl_v590ext_IEs* rel5_ext_ptr
)
{
  /* Store the list in a local pointer */
  rrc_InterFreqEventList * ifreq_event_list_ptr = NULL;

  uint8 cnt = 0, count = 0;
  uint32 idx=0;
  /* To satisfy LINT */
  uint32 tmp = 0;

  rrc_NonUsedFreqParameterList * local_non_used_param_ptr = NULL;

  rrc_Inter_FreqEventCriteriaList_v590ext *inter_f_evt_ext_ptr = NULL;

  ifreq_event_list_ptr = &rrc_event_criteria_ptr->interFreqEventList;

  if (ifreq_event_list_ptr->n  == 0)
  {
    WRRC_MSG0_ERROR("IFreq:Event Criteria list empty");
    return FAILURE;
  }

  if((rel5_ext_ptr != NULL) && (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rel5_ext_ptr,
    measurementCommand_v590ext)) &&
  (rel5_ext_ptr->measurementCommand_v590ext.t == 
  T_rrc_MeasurementControl_v590ext_IEs_measurementCommand_v590ext_inter_frequency))
  {
    inter_f_evt_ext_ptr = &rel5_ext_ptr->measurementCommand_v590ext.u.inter_frequency;
  }

  while ((ifreq_event_list_ptr->n > idx) && (cnt < L1_MAX_MEAS_EVENT))
  {
    /* Get Event Id */
    l1_event_criteria_ptr->evt[cnt].evt_id =
      RET_INTER_FREQ_EVENT_TYPE(ifreq_event_list_ptr->elem[idx].t);

    l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;
    l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

    switch(l1_event_criteria_ptr->evt[cnt].evt_id)
    {

    case L1_INTER_FREQ_EVENT_2A:
      /* Change of best frequency. */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        (ifreq_event_list_ptr->elem[idx].u.event2a->usedFreqW);

      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2a->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2a->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2a,
        rrc_Event2a,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2a->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2a,
        rrc_Event2a,nonUsedFreqParameterList))
      {
        uint32 idy=0;

        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2a->nonUsedFreqParameterList;
        
        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh
            =  (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;
       /*  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh_included
           = FALSE;        */
          if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n >idx))
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_f_evt_ext_ptr,threholdNonUsedFrequency_deltaList))
            {
              if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.n > idy)
              {
                if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].m.deltaRSCPPresent)
                {
                  /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].
                    delta_non_used_freq_thresh_included = TRUE;
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh
                    = inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;*/
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh +=
                    (int16) inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;
                }
                idy++;
              }
            }
          }

          /* Non used f */
          tmp = local_non_used_param_ptr->elem[count].nonUsedFreqW;
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) tmp;

          count++;
        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;

      }
      break;

    case L1_INTER_FREQ_EVENT_2B:
    /* The estimated quality of the currently used frequency is below a certain
    threshold and the estimated quality of a non-used frequency is above a certain
      threshold. */

      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        (ifreq_event_list_ptr->elem[idx].u.event2b->usedFreqThreshold);
      /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = FALSE; */
      if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n > idx))
      {
        /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = TRUE;
        l1_event_criteria_ptr->evt[cnt].delta_freq_thresh = inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta; */
        l1_event_criteria_ptr->evt[cnt].freq_thresh += (int16) inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta;
      }
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        (ifreq_event_list_ptr->elem[idx].u.event2b->usedFreqW);

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2b->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2b->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2b,
        rrc_Event2b,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2b->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2b,
        rrc_Event2b,nonUsedFreqParameterList))
      {
        uint32 idy=0;
                
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2b->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;
         /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh_included
           = FALSE;        */
          if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n >idx))
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_f_evt_ext_ptr,threholdNonUsedFrequency_deltaList))
            {
              if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.n > idy)
              {
                if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].m.deltaRSCPPresent)
                {
                  /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].
                    delta_non_used_freq_thresh_included = TRUE;
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh
                    = inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;*/
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh +=
                    (int16) inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;
                }
                idy++;
              }
            }
          }
          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;
    case L1_INTER_FREQ_EVENT_2C:
      /* The estimated quality of a non-used frequency is above a certain threshold. */

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2c->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2c->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2c,
        rrc_Event2c,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2c->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2c,
        rrc_Event2c,nonUsedFreqParameterList))
      {
        uint32 idy =0;
        
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2c->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;
         /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh_included
           = FALSE;        */
          if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n >idx))
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_f_evt_ext_ptr,threholdNonUsedFrequency_deltaList))
            {
              if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.n > idy )
              {
                if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].m.
                  deltaRSCPPresent)
                {
                  /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].
                    delta_non_used_freq_thresh_included = TRUE;
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh
                    = inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;*/
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh +=
                    (int16) inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;
                }
                idy++;
              }
            }
          }
          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;



    case L1_INTER_FREQ_EVENT_2E:
      /* The estimated quality of a non-used frequency is below a certain threshold. */

      /* Vivek Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2e->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2e->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2e,
        rrc_Event2e,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2e->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2e,
        rrc_Event2e,nonUsedFreqParameterList))
      {
        uint32 idy=0;
        
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2e->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;
        /* l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh_included
           = FALSE;        */
          if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n > idx))
          {
            if(RRC_MSG_LIST_BITMASK_IE_TYPE2(inter_f_evt_ext_ptr,threholdNonUsedFrequency_deltaList))
            {
              if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.n > idy)
              {
                if(inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].m.
                  deltaRSCPPresent)
                {
                  /*l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].
                    delta_non_used_freq_thresh_included = TRUE;
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].delta_non_used_freq_thresh
                    = inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;*/
                  l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh +=
                    (int16) inter_f_evt_ext_ptr->elem[idx].threholdNonUsedFrequency_deltaList.elem[idy].deltaRSCP;
                }
                idy++;
              }
            }
          }
          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;

    case L1_INTER_FREQ_EVENT_2D:

      /* Get used freq threshold */
      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        ifreq_event_list_ptr->elem[idx].u.event2d->usedFreqThreshold;
      /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = FALSE; */
      if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n > idx))
      {
        /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = TRUE;
        l1_event_criteria_ptr->evt[cnt].delta_freq_thresh = inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta; */
        l1_event_criteria_ptr->evt[cnt].freq_thresh += (int16) inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta;
      }
      /* Get W */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2d->usedFreqW;

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2d->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2d->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2d,
        rrc_Event2d,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2d->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }

      /* Initialize no of non used freq params to 0 */
      l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

      break;

    case L1_INTER_FREQ_EVENT_2F:

      /* Get used freq threshold */
      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        ifreq_event_list_ptr->elem[idx].u.event2f->usedFreqThreshold;
      /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = FALSE; */
      if((inter_f_evt_ext_ptr != NULL) && (inter_f_evt_ext_ptr->n > idx))
      {
        /*l1_event_criteria_ptr->evt[cnt].delta_freq_thresh_included = TRUE;
        l1_event_criteria_ptr->evt[cnt].delta_freq_thresh = inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta; */
        l1_event_criteria_ptr->evt[cnt].freq_thresh += (int16) inter_f_evt_ext_ptr->elem[idx].
          threholdUsedFrequency_delta;
      }
      /* Get W */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2f->usedFreqW;

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2f->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2f->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2f,
        rrc_Event2d,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2f->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }

      /* Initialize no of non used freq params to 0 */
      l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

      break;

    default:
      WRRC_MSG1_ERROR("InterFreq Event %d not supported", l1_event_criteria_ptr->evt[cnt].evt_id);
      return FAILURE;

    } /* End of switch */

    if ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl == TRUE) &&
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat != L1_RPT_ASET) &&
      ((l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2D) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2F)))
    {
      WRRC_MSG1_ERROR("Event 2D/2F, Rpt cell stat!= ASET",l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat);
      return FAILURE;
    }
    else if ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl == TRUE) &&
      ((l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2A) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2B) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2C) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2E)) &&
      ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ASET) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ))
    {
      WRRC_MSG1_ERROR("Event 2A/B/C/D, rpt cell stat %d",
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat);

      return FAILURE;
    }
    else
    {
      MSG_LOW("Reporting Cell Status valid",0,0,0);
    }

    cnt++;

    /* Increment Pointer count */
   idx++;
  } /* End of while */

  /* Store the count */
  l1_event_criteria_ptr->num_event = cnt;

  return SUCCESS;

}

/*===========================================================================

FUNCTION   RRCIFREQ_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Inter Freq Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcifreq_initialize_meas_id_list
(
 void
)
{
  uint8 count = 0, cnt = 0;
  for ( count = 0; count < MAX_INTER_FREQ_MEAS; count++ )
  {
    /* Set all positions to vacant and make PSC as invalid */
    inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
    inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;
    inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
    inter_freq_meas_id_list[count].mcm_owned = FALSE;

    /* Also initialize additional meas */
    for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
      inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
  }
}



/*===========================================================================

FUNCTION  RRCIFREQ_SEND_MEAS_REPORT

DESCRIPTION

 This function sends the measurement report. First it validates the event
 and measured results depending upon the report type and it ignores the L1
 report if reported values are incorrect. The function also appends
 additional measurements if any and also updates the Inter freq meas
 id list if last periodic report is being sent

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcifreq_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;

  uint8 cnt = 0, count = 0, cell_number = 0;
  uint8 special_index =0, cell_cnt=0;

  rrc_InterFreqCellList * local_rrc_event_cell_list_ptr = NULL;

  rrc_InterFreqMeasuredResultsList * local_rrc_meas_results_ptr = NULL;

  rrc_InterFreqCellMeasuredResultsList * local_inter_freq_cell_list_ptr = NULL;

  rrc_InterFrequencyMeasuredResultsList_v590ext *inter_freq_meas_res_ext_ptr = NULL;

  rrc_CellMeasurementEventResults_fdd * local_scr_code_list_ptr = NULL;

  /* Stores status of send chain */
  rrcsend_status_e_type status;

  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode;

  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id;

  /* Radio bearer mode */
  rrc_RB_Identity rb_mode;

  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status = RRCSEND_L2ACK_REQUIRED;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_mrm_ota mrm_params;
  memset(&mrm_params,0,sizeof(rrc_log_rrc_mrm_ota));  
  mrm_params.meas_id = l1_meas_report_ptr->meas_id;
  mrm_params.meas_type = L1_INTER_FREQ;
  mrm_params.meas_rpt_type = l1_meas_report_ptr->rpt_crit;
  if(mrm_params.meas_rpt_type == L1_EVENT_TRIGGER_RPT)
  {
    mrm_params.event_id = l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id;
  }
  mrm_params.num_cells_rpt = l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq;
  rrc_qsh_log_meas_rpt_params(&mrm_params);
#endif

  if (rrcifreq_validate_meas_report(l1_meas_report_ptr) == FALSE)
  {
    WRRC_MSG0_HIGH("IFREQ: Ignore Meas Report");
  /* Deallocate the cell list if any allocated by L1 */
    rrcifreq_free_memory_allocated_for_reporting(l1_meas_report_ptr);
    return;
  }

  if ( l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE )
  {
    logical_channel_mode = UE_MODE_ACKNOWLEDGED_DATA;
    rb_mode = DCCH_AM_RADIO_BEARER_ID;
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
  /* Deallocate the cell list if any allocated by L1 */
    rrcifreq_free_memory_allocated_for_reporting(l1_meas_report_ptr);
    return;
  }

  /* Allocate a buffer for an uplink DCCH Message. This buffer
  * should be released by SEND_CHAIN using OSS compiler functions */

  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));

  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;


  /* First store the meas id depending whether extended measurement identity is supported or not*/
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,l1_meas_report_ptr->meas_id, INTER_FREQ);


  if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
  {
    WRRC_MSG0_MED("IFREQ: Event Triggered Report");

    /* First set the bit mask for Event Results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,eventResults);

    /* Set Event Results t */
    RRC_SET_COMMON_MSG_TYPE2(msg_ptr->message.u.measurementReport.eventResults,rrc_EventResults,
      interFreqEventResults);
    msg_ptr->message.u.measurementReport.eventResults.u.interFreqEventResults =
         rtxMemAllocTypeZ(&enc_ctxt,rrc_InterFreqEventResults);
    /* Initialize bit mask */
    RRC_RESET_MSG_IE_PRESENT_PTR(msg_ptr->message.u.measurementReport.eventResults.u.interFreqEventResults);

    rrcifreq_set_inter_freq_event_id(&l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id,
      &msg_ptr->message.u.measurementReport.eventResults.u.interFreqEventResults->eventID);

    if ((l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id == L1_INTER_FREQ_EVENT_2D) ||
      (l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id == L1_INTER_FREQ_EVENT_2F))
    {
      MSG_MED("Event 2D/2F. No cell list reqd",0,0,0);
    }
    else
    {
      /* For event results, inter freq cell list would be present */
      RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.eventResults.u.interFreqEventResults,
        rrc_InterFreqEventResults,interFreqCellList);

      
      /* Assign ptr to head */
      local_rrc_event_cell_list_ptr = &msg_ptr->message.u.measurementReport.eventResults.u.interFreqEventResults->interFreqCellList;
      local_rrc_event_cell_list_ptr->n =  l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq;

      ALLOC_ASN1ARRAY2(&enc_ctxt,local_rrc_event_cell_list_ptr,rrc_InterFreqCell);

      for (cnt = 0; cnt < L1_MAX_FREQ && cnt < l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq; cnt++)
      {
        local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.t =
          T_rrc_FrequencyInfo_modeSpecificInfo_fdd;
        local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd =
             rtxMemAllocTypeZ(&enc_ctxt,rrc_FrequencyInfoFDD);

        RRC_RESET_MSG_IE_PRESENT_PTR(local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd);
        
        if (l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].freq_info.uarfcn_ul_incl == TRUE)
        {
          /* Get UL UARFCN */
          local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL =
            l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].freq_info.uarfcn_ul;

          local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent =1;
        }
        /* Get DL UARFCN */
        local_rrc_event_cell_list_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL =
          l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].freq_info.uarfcn_dl;

        /* Set PSC */
        local_rrc_event_cell_list_ptr->elem[cnt].nonFreqRelatedEventResults.t = T_rrc_CellMeasurementEventResults_fdd;

        local_rrc_event_cell_list_ptr->elem[cnt].nonFreqRelatedEventResults.u.fdd=
            rtxMemAllocTypeZ(&enc_ctxt,rrc_CellMeasurementEventResults_fdd);

        local_scr_code_list_ptr = local_rrc_event_cell_list_ptr->elem[cnt].nonFreqRelatedEventResults.u.fdd;

        if(l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc < L1_MAX_FREQ)
            {
        local_scr_code_list_ptr->n =
            l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc;
            }
        else
          {
        local_scr_code_list_ptr->n =
            L1_MAX_FREQ;
            }  
        ALLOC_ASN1ARRAY2(&enc_ctxt,local_scr_code_list_ptr,rrc_PrimaryCPICH_Info);
          
        for (count = 0; count < L1_MAX_FREQ && count < l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc; count++)
        {
          /* Store psc */
          local_scr_code_list_ptr->elem[count].primaryScramblingCode =
            l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].prim_scr_code[count];

        } /* End of for of list of psc */
      } /* End of for of num_cells */
    } /* End of else */
  } /* End of if of Event triggered report */


  /* Now add Measured Results */
  if ((l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results != 0) &&
    (l1_meas_report_ptr->u.inter_f_rpt.meas_results_included == TRUE))
  {
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
        rrc_MeasurementReport,measuredResults);

    RRC_SET_COMMON_MSG_TYPE(msg_ptr->message.u.measurementReport.measuredResults,
      rrc_MeasuredResults_interFreqMeasuredResultsList);

    /* Allocate Memory */

    local_rrc_meas_results_ptr = rtxMemAllocTypeZ(&enc_ctxt,rrc_InterFreqMeasuredResultsList);

    msg_ptr->message.u.measurementReport.measuredResults.u.interFreqMeasuredResultsList =
      local_rrc_meas_results_ptr;

    if(L1_MAX_FREQ > l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results)
    {
      local_rrc_meas_results_ptr->n= l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results;
    }
    else
    {
      local_rrc_meas_results_ptr->n=L1_MAX_FREQ;
    }

    ALLOC_ASN1ARRAY2(&enc_ctxt,local_rrc_meas_results_ptr,rrc_InterFreqMeasuredResults);

    for (cnt = 0; (cnt < L1_MAX_FREQ) && (cnt < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results); cnt++)
    {
      cell_cnt += l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells;
    }
    
    for (cnt = 0; (cnt < L1_MAX_FREQ) && (cnt < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results); cnt++)
    {
      RRC_RESET_MSG_IE_PRESENT(local_rrc_meas_results_ptr->elem[cnt]);

      if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
        msmt_results[cnt].utra_carrier_rssi_incl == TRUE)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(local_rrc_meas_results_ptr->elem[cnt],
            utra_CarrierRSSI);

        local_rrc_meas_results_ptr->elem[cnt].utra_CarrierRSSI =
          (rrc_UTRA_CarrierRSSI)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
          msmt_results[cnt].utra_carrier_rssi;
      }

      /* Set bit maks for Freq Info */
      RRC_MSG_COMMON_SET_BITMASK_IE(local_rrc_meas_results_ptr->elem[cnt],
      rrc_InterFreqMeasuredResults,frequencyInfo);

      /* Set t to fdd */
      local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.t =
        T_rrc_FrequencyInfo_modeSpecificInfo_fdd;
      local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd =
        rtxMemAllocTypeZ(&enc_ctxt,rrc_FrequencyInfoFDD);
      
      /* Set UL ARFCN bit mask to 0 */
      RRC_RESET_MSG_IE_PRESENT_PTR(local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd);

      if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
        msmt_results[cnt].freq_info.uarfcn_ul_incl == TRUE)
      {
        local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent =1;

        local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL =
          l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].freq_info.uarfcn_ul;
      }

      local_rrc_meas_results_ptr->elem[cnt].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL =
        l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].freq_info.uarfcn_dl;


      /* Now get Inter Freq Cells */
      if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (cnt == 0) && (cell_cnt >0))
        {
          msg_ptr->message.u.measurementReport.m.v390nonCriticalExtensionsPresent =1;

          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.m.
            v4b0NonCriticalExtensionsPresent =1;

          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;

          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext.m.
            measuredResults_v590extPresent =1;
          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext.
            measuredResults_v590ext.t = T_rrc_MeasuredResults_v590ext_interFrequencyMeasuredResultsList;
          
          inter_freq_meas_res_ext_ptr = rtxMemAllocTypeZ(&enc_ctxt,rrc_InterFrequencyMeasuredResultsList_v590ext);
        inter_freq_meas_res_ext_ptr->n = cell_cnt;
          ALLOC_ASN1ARRAY2(&enc_ctxt,inter_freq_meas_res_ext_ptr,rrc_DeltaRSCPPerCell);

          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions.measurementReport_v590ext.
            measuredResults_v590ext.u.interFrequencyMeasuredResultsList = inter_freq_meas_res_ext_ptr;

        }

      if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells == 0)
      {
        WRRC_MSG1_HIGH("IFREQ:Num cells per freq is %d",
          l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells);
      }
      else
      {
        /* Assign memory for these cells */
        local_inter_freq_cell_list_ptr = &local_rrc_meas_results_ptr->elem[cnt].interFreqCellMeasuredResultsList;
        /* Set bit mask for Cell Meas Results List present */
        local_rrc_meas_results_ptr->elem[cnt].m.interFreqCellMeasuredResultsListPresent =1;

        /* Assign head */
         if(L1_MAX_CELL_MEAS > l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells)
         {
           local_inter_freq_cell_list_ptr->n = 
            l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells;
         }
         else
         {
           local_inter_freq_cell_list_ptr->n = L1_MAX_CELL_MEAS;
         }

        ALLOC_ASN1ARRAY2(&enc_ctxt,local_inter_freq_cell_list_ptr,rrc_CellMeasuredResults); 
       
        /* Now validate Measured Results */
        for ( cell_number = 0; ((cell_number < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells)
          && ( cell_number < L1_MAX_CELL_MEAS)); cell_number++ )
        {
          RRC_RESET_MSG_IE_PRESENT(local_inter_freq_cell_list_ptr->elem[cell_number]);
          if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_rpt_ind == L1_NO_REPORT)
          {
            MSG_LOW("SFN SFN observed time diff not set", 0,0,0);
          }
          else
          {
            /* Set bit mask to sfn_sfn present */
            local_inter_freq_cell_list_ptr->elem[cell_number].m.dummyPresent =1;

            /* Store sfn-sfn type indicator */

            if ( l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_rpt_ind == L1_REPORT_TYPE_1)
            {
              local_inter_freq_cell_list_ptr->elem[cell_number].dummy.t =  T_rrc_SFN_SFN_ObsTimeDifference_type1;

              MSG_LOW("Report type 1 selected", 0,0,0);
              /* Type 1 report */
              local_inter_freq_cell_list_ptr->elem[cell_number].dummy.u.type1 =
                l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff;

            }
            else
            {
              MSG_LOW("Report type 2 selected", 0,0,0);
              local_inter_freq_cell_list_ptr->elem[cell_number].dummy.t =  T_rrc_SFN_SFN_ObsTimeDifference_type2;
              /* Type 2 report */
              local_inter_freq_cell_list_ptr->elem[cell_number].dummy.u.type2 =
                (rrc_SFN_SFN_ObsTimeDifference2)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff;

            }

          } /* End of else of L1_NO_REPORT */

          /* Check if cell sync info present */

          if ( l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info_incl == TRUE )
          {
            /* Set cell sync info present to rrc_cellSynchronisationInfo) */
            local_inter_freq_cell_list_ptr->elem[cell_number].m.cellSynchronisationInfoPresent =1;

            /* Store Choice Mode info to fdd*/
            local_inter_freq_cell_list_ptr->elem[cell_number].cellSynchronisationInfo.
              modeSpecificInfo.t =
              T_rrc_CellSynchronisationInfo_modeSpecificInfo_fdd;

            local_inter_freq_cell_list_ptr->elem[cell_number].cellSynchronisationInfo.modeSpecificInfo.u.fdd
                = rtxMemAllocTypeZ(&enc_ctxt,rrc_CellSynchronisationInfo_fdd);
            local_inter_freq_cell_list_ptr->elem[cell_number].cellSynchronisationInfo.
              modeSpecificInfo.u.fdd->tm =
              (uint16)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info.tm;



            /* Set bit mask indicating rrc_CountC_SFN_Frame_difference present */
            RRC_RESET_MSG_IE_PRESENT_PTR(local_inter_freq_cell_list_ptr->elem[cell_number].cellSynchronisationInfo.
              modeSpecificInfo.u.fdd);

          } /* End of if of rrc_cellSynchronisationInfo) */

          /* Set in the FDD mode for rrc_CellMeasuredResults_modeSpecificInfo */
          local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.t =
            T_rrc_CellMeasuredResults_modeSpecificInfo_fdd;

          local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd =
            rtxMemAllocTypeZ(&enc_ctxt,rrc_CellMeasuredResults_fdd);

          /* Set primary scrambling code */
          local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->
            primaryCPICH_Info.primaryScramblingCode =
            l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pri_scr_code;

          RRC_RESET_MSG_IE_PRESENT_PTR(local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd);



          /* Convert meas value to UTRAN format */

          /* Check if RSCP included */
          if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->rscp_included == TRUE)
          {
            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->cpich_RSCP =
              (rrc_CPICH_RSCP)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->rscp_val;

            /* Set bit mask */
            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->m.cpich_RSCPPresent =1;
            if((inter_freq_meas_res_ext_ptr != NULL) &&
              (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].
              cell[cell_number]->delta_rscp_included == TRUE))
            {
              inter_freq_meas_res_ext_ptr->elem[special_index].m.deltaRSCPPresent =1;
              inter_freq_meas_res_ext_ptr->elem[special_index].deltaRSCP = 
                (rrc_DeltaRSCP)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->delta_rscp;
            }
          }
          /* Check if EC/Io included */
          if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->ec_no_included == TRUE)
          {

            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->cpich_Ec_N0 =
              (rrc_CPICH_Ec_N0)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->ec_no_val;

            /* Set bit mask */
            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->m.
            cpich_Ec_N0Present =1;

          }

          /* Check if Path Loss included */
          if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_included == TRUE)
          {

            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->pathloss =
              (rrc_Pathloss)l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_val;

            /* Set bit mask */
            local_inter_freq_cell_list_ptr->elem[cell_number].modeSpecificInfo.u.fdd->m.pathlossPresent =1;

          }
            special_index++;
          /* One cell completely filled here */
        } /* End of for loop */

      } /* No of cell != 0 */


    } /* End of for of meas results */
  }    /* num msmt results != 0 */

  msg_ptr->message.u.measurementReport.additionalMeasuredResults.n = 0;

  rrcmeas_process_additional_measured_results(
    &l1_meas_report_ptr->meas_id, INTER_FREQ, &msg_ptr->message.u.measurementReport);

  /* Now check if this is the last report. Meas Id needs to be deleted if true */
  if ((l1_meas_report_ptr->last_periodic_report == TRUE) &&
    (l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT))
  {
    /* Delete this Measurement */
    for ( cnt =0; cnt < MAX_INTER_FREQ_MEAS; cnt++)
    {
      if (inter_freq_meas_id_list[cnt].meas_id == l1_meas_report_ptr->meas_id)
      {
        WRRC_MSG1_MED("IFREQ:Delete Meas %d",l1_meas_report_ptr->meas_id);
        inter_freq_meas_id_list[cnt].meas_id = INVALID_MEAS_ID;

        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;

        inter_freq_meas_id_list[count].mcm_owned = FALSE;

        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( count = 0; count < MAX_ADDITIONAL_MEAS; count++ )
          inter_freq_meas_id_list[cnt].additional_meas[count] = INVALID_MEAS_ID;

        break;
      }
    }

    WRRC_MSG1_MED("IFreq:Delete %d from addtl meas DB", l1_meas_report_ptr->meas_id);
    rrcmeas_delete_addtl_meas(&l1_meas_report_ptr->meas_id);

  }

  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);

  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("Send chain failed to send Inter-RAT Meas Rpt");
  }
  else
  {
    
    if(l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE)
    {
      rrcmeas_pending_rpt++;
      if(rrcmeas_pending_rpt >= 5)
      {
        WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
      }
    }
  }

  /* Deallocate the cell list if any allocated by L1 */
  rrcifreq_free_memory_allocated_for_reporting(l1_meas_report_ptr);

  return;
}


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria
(
 rrc_InterFreqReportCriteria* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
 ,rrc_MeasurementControl_v590ext_IEs* rel5_ext_ptr
)
{
  switch(ifreq_rpt_criteria_ptr->t)
  {
  case T_rrc_InterFreqReportCriteria_interFreqReportingCriteria:

    WRRC_MSG0_MED("IFreq:Inter Freq Event Rpt Crit");

    l1_rpt_params_ptr->u.inter_rpt_crit.num_event = 0;

    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        rrc_InterFreqReportingCriteria,interFreqEventList)) &&
      (ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria->interFreqEventList.n != 0))
    {
      if (rrcifreq_process_event_criteria(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        &l1_rpt_params_ptr->u.inter_rpt_crit, rel5_ext_ptr) == FAILURE)
      {
        return FAILURE;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("InterFreq Event List Empty");
      return FAILURE;
    }

    /* Inter-Freq Events successfully processed */
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_EVT_TRIG;

    break;

  case T_rrc_InterFreqReportCriteria_intraFreqReportingCriteria:
    
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_CHANGE;
    break;

  case T_rrc_InterFreqReportCriteria_periodicalReportingCriteria:

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_PERIODIC;

    WRRC_MSG0_HIGH("IFreq:Perodic Rpt Crit");

    l1_rpt_params_ptr->rpt_cell_status_incl = FALSE;

    /* Set reporting cells */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
      else
      {
        return FAILURE;
      }
    }

    /* Check if reporting amount present */
  
      /* Store reporting amount */
      l1_rpt_params_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type)
        RRCMEAS_RET_REPORTING_AMOUNT(
        ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingAmount);
    
    /* The spec indicates that 0 means UE behaviour unspecified */
    if (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
     
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS;
    }
    else
    {
      /* Store reporting interval */
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL
        (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingInterval);
    }

    WRRC_MSG2_HIGH("IFreq:Rpting Amt is %d, Infin:0 Rpting Intvl is %d msec",l1_rpt_params_ptr->u.periodic_crit.rpt_amount, l1_rpt_params_ptr->u.periodic_crit.rpt_interval);
    break; /* Come out of switch */

  case T_rrc_InterFreqReportCriteria_noReporting:
    

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_REPORTING;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.noReporting,
        rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.noReporting->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        WRRC_MSG0_HIGH("IFREQ:Rpting Crit is Addtl");

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
    }
    break;



  default:
    WRRC_MSG1_ERROR("InterFreq Rpt Crit %d not supported",ifreq_rpt_criteria_ptr->t);
    return FAILURE;

  }
  return SUCCESS;

}


/*===========================================================================

FUNCTION  RRCIFREQ_VALIDATE_MEAS_REPORT

DESCRIPTION

 This function validates the measurement report.

DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcifreq_validate_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  uint8 cnt = 0, count = 0, cell_number = 0;

  if ((l1_meas_report_ptr->rpt_crit != L1_PERIODIC_RPT) &&
    (l1_meas_report_ptr->rpt_crit != L1_EVENT_TRIGGER_RPT))
  {
    WRRC_MSG1_ERROR("IFreq: Invalid Rpt Crit %d",l1_meas_report_ptr->rpt_crit);
    return FALSE;
  }


  if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
  {
    WRRC_MSG1_MED("IFreq:Validate Event Report.No of Freq is %d", l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq);

    if ((l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id == L1_INTER_FREQ_EVENT_2D) ||
      (l1_meas_report_ptr->u.inter_f_rpt.event_results.evt_id == L1_INTER_FREQ_EVENT_2F))
    {
      MSG_MED("Event 2D/2F reported",0,0,0);
    }
    else
    {
      if (l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq == 0)
      {
        WRRC_MSG1_ERROR("IFreq: Invalid no of freq %d", l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq);
        return FALSE;
      }

      for (cnt = 0; (cnt < L1_MAX_FREQ) && (cnt < l1_meas_report_ptr->u.inter_f_rpt.event_results.num_freq); cnt++)
      {
        if (rrcifreq_validate_freq(&l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].freq_info) == FALSE)
        {
          return FALSE;
        }

        WRRC_MSG1_HIGH("IFreq:No of scr is %d", l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc);

        if (l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc == 0)
        {
          WRRC_MSG1_ERROR("IFreq:Invalid no of scr codes %d", l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc);
          return FALSE;
        }
        for (count = 0; (count < L1_MAX_FREQ) && (count < l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].num_psc); count++)
        {
          if (l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].prim_scr_code[count] > MAX_PSC)
          {
            WRRC_MSG1_ERROR("IFreq: Invalid %d PSC", l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].prim_scr_code[count]);
            return FALSE;
          }
          else
            WRRC_MSG2_HIGH("IFreq:Scr Code cnt %d is %d", count,
            l1_meas_report_ptr->u.inter_f_rpt.event_results.freq[cnt].prim_scr_code[count]);
        }
      }
    } /* End of else */
  } /* End of if L1_EVENT_TRIGGER_RPT */

  if (l1_meas_report_ptr->u.inter_f_rpt.meas_results_included == FALSE)
  {
    WRRC_MSG0_HIGH("IFreq:Meas Results not included");
    return TRUE;
  }

  /* Check for Measured Results */
  if ((l1_meas_report_ptr->u.inter_f_rpt.meas_results_included == TRUE) || (l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT))
  {
    if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results != 0)
    {

      for (cnt = 0; (cnt < L1_MAX_FREQ) && (cnt < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results); cnt++)
      {
        if ((l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
          msmt_results[cnt].utra_carrier_rssi_incl == TRUE) &&
          (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
          msmt_results[cnt].utra_carrier_rssi > INTER_FREQ_RSSI))
        {
          WRRC_MSG1_ERROR("IFreq: UTRAN RSSI value %d invalid",
            l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
            msmt_results[cnt].utra_carrier_rssi);
          return FALSE;

        }

        if (rrcifreq_validate_freq(&l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
          msmt_results[cnt].freq_info) == FALSE)
        {
          MSG_HIGH("IFreq: Invalid DFreq or UFreq, RSSI is %d, DFreq is %d, Num Cells per Freq is %d", l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
            msmt_results[cnt].utra_carrier_rssi, l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
            msmt_results[cnt].freq_info.uarfcn_dl,l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells);
          return FALSE;
        }

        if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells != 0)
        {
          /* Now validate Measured Results */
          for ( cell_number = 0; ((cell_number < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].num_cells)
            && ( cell_number < L1_MAX_CELL_MEAS)); cell_number++ )
          {
            if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_rpt_ind == L1_NO_REPORT)
            {
              MSG_LOW("SFN SFN observed time diff not set", 0,0,0);
            }
            else
            {
              /* Store sfn-sfn type indicator */

              if ( l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_rpt_ind == L1_REPORT_TYPE_1)
              {
                if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff >
                  SFN_SFN_TYPE1_MAX_VALUE)
                {
                  MSG_LOW("IFreq:Incorrect SFN-SFN Type1 value %d",
                    l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff,0,0);

                  return FALSE;
                }
              }
              else
              {
                MSG_LOW("sfn-sfn type 2 selected",0,0,0);

                /* Type 2 report */
                if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff >
                  SFN_SFN_TYPE2_MAX_VALUE)
                {
                  MSG_LOW("IFreq:Incorrect SFN-SFN Type2 value %d",
                    l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->sfn_sfn_tm_diff,0,0);

                  return FALSE;
                }
              }
            } /* End of else of L1_NO_REPORT */

            /* Check if cell sync info present */

            if ( l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info_incl == TRUE )
            {
              /* Set off value */
              if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info.off > OFF_MAX)
              {
                MSG_LOW("IFReq:Off value %d not in range",
                  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info.off,0,0);
                return FALSE;

              }

              /* Store tm value */
              if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info.tm > TM_MAX)
              {
                MSG_LOW("IFreq:Tm value %d not in range",
                  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->cell_sync_info.tm,0,0);
                return FALSE;
              }

            } /* End of if of rrc_cellSynchronisationInfo) */

            /* Check primary scrambling code */
            if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pri_scr_code > MAX_PSC)
            {
              MSG_LOW("IFreq:Invalid PSC = %d",  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pri_scr_code,0,0);
              return FALSE;
            }

            /* Check if rscp included */
            if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->rscp_included == TRUE)
            {
              if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->rscp_val > RSCP_MAX_VALUE)
              {
                MSG_LOW("IFreq:Incorrect attached RSCP value %d received",
                  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->rscp_val,0,0);

                return FALSE;

              }
              if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
                && (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
                msmt_results[cnt].cell[cell_number]->delta_rscp_included == TRUE))
              {
                if((l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
                msmt_results[cnt].cell[cell_number]->delta_rscp > DELTA_RSCP_MAX)
                  || (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
                msmt_results[cnt].cell[cell_number]->delta_rscp < DELTA_RSCP_MIN))
                {
                   MSG_LOW("Delta RSCP not in range -5..-1 %d received",
                     l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.
                     msmt_results[cnt].cell[cell_number]->delta_rscp,0,0);
                   MSG_LOW("Trash Meas Report",0,0,0);
                   return FALSE;
                }
              }
            }

            /* Now add check to determine if ec_no_included too */
            if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->ec_no_included == TRUE)
            {
              if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->ec_no_val > ECN0_MAX_VALUE)
              {
                MSG_LOW("IFreq:Incorrect attached EcN0 value %d received",
                  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->ec_no_val,0,0);

                return  FALSE;

              }
            }

            /* Now add check to determine if path loss included too */
            if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_included == TRUE)
            {
              if ((l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_val < PATH_LOSS_MIN_VALUE) ||
                  (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_val > PATH_LOSS_MAX_VALUE))
              {
                MSG_LOW("IFreq:Incorrect attached Path Loss value %d received",
                  l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[cnt].cell[cell_number]->pathloss_val,0,0);

                return  FALSE;

              }
            }

          } /* End of for loop of num cells */

        } /* No of cell != 0 */

      } /* End of for loop of num_msmt_res */

    } /* End of if num_msmt_results != 0 */
    else
    {
      WRRC_MSG0_HIGH("IFreq: No of msmt res is 0 so nothing to validate");
    }
  } /* End of if measured results included  */
  return TRUE;
}


/*===========================================================================

FUNCTION  RRCIFREQ_VALIDATE_FREQ

DESCRIPTION

 This function validates the uplink and downlink carrier frequency.


DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcifreq_validate_freq
(
  l1_freq_info_struct_type* freq_info_ptr
)
{
  if (freq_info_ptr->uarfcn_ul_incl == TRUE)
  {
    if (freq_info_ptr->uarfcn_ul > FDD_ARFCN)
    {
      WRRC_MSG1_ERROR("IFreq: UL UARFCN is %d",
        freq_info_ptr->uarfcn_dl);
      return FALSE;
    }
  }
  if (freq_info_ptr->uarfcn_dl > FDD_ARFCN)
  {
    WRRC_MSG1_ERROR("IFreq:DL UARFCN is %d",
      freq_info_ptr->uarfcn_dl);
    return FALSE;
  }
  else
  {
    WRRC_MSG1_HIGH("IFreq:DL Freq is %d", freq_info_ptr->uarfcn_dl);
  }

  return TRUE;
}


/*===========================================================================

FUNCTION  RRCIFREQ_SET_INTER_FREQ_EVENT_ID

DESCRIPTION

 This function sets the event id as per ASN1 format from what L1 specified
 in the measurement report

DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

void rrcifreq_set_inter_freq_event_id
(
 l1_inter_freq_event_enum_type* event_id_ptr,
 rrc_EventIDInterFreq* inter_freq_event_id_ptr
)
{
  /* Only 1 event is sent to UTRAN. So if L1 sends more than 1 event, neglect it*/

  MSG_LOW("Value of event Id is : %d", *event_id_ptr,0,0);
  WRRC_MSG1_HIGH("Ifreq:Event triggered report, EVENT %d[0/1/2/3/4/5 : 2A/2B/2C/2D/2E/2F] reported",*event_id_ptr);

  /* Find type of event and set it */
  switch ( *event_id_ptr )
  {
  case L1_INTER_FREQ_EVENT_2A:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2a;
    break;

  case L1_INTER_FREQ_EVENT_2B:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2b;

    break;

  case L1_INTER_FREQ_EVENT_2C:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2c;

    break;

  case L1_INTER_FREQ_EVENT_2D:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2d;

    break;

  case L1_INTER_FREQ_EVENT_2E:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2e;

    break;
  case L1_INTER_FREQ_EVENT_2F:
    *inter_freq_event_id_ptr = rrc_EventIDInterFreq_e2f;

    break;
  default:
    /* Not possible as validation has already been done */
    
    break;

  }  /* End of switch */
}

/*===========================================================================

FUNCTION   copy_sib11_inter_freq_cell_list_to_sib12

DESCRIPTION

    This function copies the inter freq cell list from SIB 11 nto the SIB12
    inter freq cell list ptr

DEPENDENCIES

    None

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/

void rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
(
l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib11_ptr,
l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib12_ptr
)
{
  uint16 count;
  l1_inter_freq_sib12_ptr->num_cell_rmv = 0;
  l1_inter_freq_sib12_ptr->num_cell_add =  l1_inter_freq_sib11_ptr->num_cell_add;

  for(count = 0; count < l1_inter_freq_sib11_ptr->num_cell_add; count ++)
  {
    l1_inter_freq_sib12_ptr->add_cell[count].psc
     =  l1_inter_freq_sib11_ptr->add_cell[count].psc;
    l1_inter_freq_sib12_ptr->add_cell[count].measure_in_idle_pch_fach
     =  l1_inter_freq_sib11_ptr->add_cell[count].measure_in_idle_pch_fach;
    l1_inter_freq_sib12_ptr->add_cell[count].freq_info
     =  l1_inter_freq_sib11_ptr->add_cell[count].freq_info;
    l1_inter_freq_sib12_ptr->add_cell[count].cell_info
     =  l1_inter_freq_sib11_ptr->add_cell[count].cell_info;
  }
}

/*===========================================================================

FUNCTION   rrcifreq_process_remove_inter_freq_cells

DESCRIPTION

    This function processes the inter freq remove cell list of SIB12 and updates
    the inter freq cell list variable.

DEPENDENCIES

    This function assumes that SIB12 inter freq removel cell list IE is present

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/
static void rrcifreq_process_remove_inter_freq_cells
(
rrc_RemovedInterFreqCellList *removedInterFreqCellList,
uint32 *num_rmv_cells
)
{
  uint32 inter_freq_cell_id;
  uint32 idx=0;
  *num_rmv_cells = 0;
  WRRC_MSG1_HIGH("remove [1: All  2: Some  3: No]:- %d cells from inter freq cells list",removedInterFreqCellList->t);
  switch(removedInterFreqCellList->t)
  {
    case T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells:
         
         *num_rmv_cells = L1_MAX_CELL_MEAS;
         rrcifreq_initialize_cell_list();
      break;

    case T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells:
         if(removedInterFreqCellList->u.removeSomeInterFreqCells->n == 0)
         {
           MSG_LOW("Assign no of removed cells to be 0", 0,0,0);
         }
         else
         {
          
           do
           {
             inter_freq_cell_id = removedInterFreqCellList->u.removeSomeInterFreqCells->elem[idx];

             /* Remove cell at position given by the cell id from the inter freq cell info list */
             if(inter_freq_cell_id < L1_MAX_CELL_MEAS)
             {
               inter_freq_cell_list[inter_freq_cell_id].psc = INVALID_PSC;
               inter_freq_cell_list[inter_freq_cell_id].cell_position = VACANT;
               inter_freq_cell_list[inter_freq_cell_id].dl_freq = INVALID_INTER_FREQ;
               (*num_rmv_cells)++; /* Increment count */
             }

             /*  Get next element in the list */
             WRRC_MSG1_HIGH("Removing cell id %d from the inter freq cell list", inter_freq_cell_id);
             idx++;

           } /* Continue if cond satisfied */
           while ((removedInterFreqCellList->u.removeSomeInterFreqCells->n > idx) &&
            ((*num_rmv_cells) < L1_MAX_CELL_MEAS));
         }
      break;

    case T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells:
         /* no cells need to be removed */
         
      break;

    default:
      /* cannot come here */
      break;
  }
}
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
/*===========================================================================

FUNCTION   rrcifreq_check_if_multiplefrequency_bands_supported

DESCRIPTION

    This function checks if we support any of the multiple frequency bands passed.

DEPENDENCIES

    None

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/
static void rrcifreq_check_if_multiplefrequency_bands_supported(rrc_MultipleFrequencyBandIndicatorListFDD *inter_multifreq_ptr,
                                                                boolean *band_supported,
                                                                rrc_csp_band_class_type *band_class)
{
  uint8 i=0;
  *band_supported=FALSE;
  while((i< inter_multifreq_ptr->n) && !(*band_supported))
  {
    if(inter_multifreq_ptr->elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator1)
    {
      rrc_RadioFrequencyBandFDD FDD1 = inter_multifreq_ptr->elem[i].u.frequencyBandsIndicator1;
      if((Fdd1_internal_band[FDD1] != RRC_CSP_BAND_MAX_INDEX) &&
         (rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]])))
          {
            *band_supported = TRUE;
        *band_class = rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]];
      }
    }
    else if(inter_multifreq_ptr->elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator2)
    {
      rrc_RadioFrequencyBandFDD FDD2 = inter_multifreq_ptr->elem[i].u.frequencyBandsIndicator2; 
      if((Fdd2_internal_band[FDD2] != RRC_CSP_BAND_MAX_INDEX) &&
         (rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]])))
          {
            *band_supported = TRUE;
        *band_class=rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]];
      }
    }
    i++;
  }
}
/*===========================================================================

FUNCTION   rrcifreq_populate_sibs_if_band_found

DESCRIPTION

    This function populates the neighbour frequency info from SIB11,SIB12,SIB11bis.

DEPENDENCIES

    None

RETURN VALUE
   FALSE if unsuccesful
   TRUE if succesful

SIDE EFFECTS

    None

===========================================================================*/
static boolean rrcifreq_populate_sibs_if_band_found
(  
  rrc_csp_band_class_type tmp_band_class,
  rrc_FrequencyInfo_modeSpecificInfo mode_specific_info,
  l1_freq_info_struct_type *frequecny_info,
  rrc_plmn_identity_type plmn_id
)
{
  rrc_freq_type tmp_dl_uarfcn;
  rrc_freq_type tmp_ul_uarfcn;
  /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
   of the current band*/
  if(rrccsp_get_uarfcn(mode_specific_info.u.fdd->uarfcn_DL, rrc_get_frequency_band(mode_specific_info
    .u.fdd->uarfcn_DL,plmn_id),tmp_band_class, &tmp_dl_uarfcn) == FALSE)
  {
    WRRC_MSG1_HIGH("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",tmp_dl_uarfcn);
    return FALSE;
  }
  if(tmp_dl_uarfcn > FDD_ARFCN)
  {
    WRRC_MSG1_HIGH("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",tmp_dl_uarfcn);
    return FALSE;
  }
  WRRC_MSG2_HIGH("IFreq:unsupported uarfcn %d supported uarfcn %d",mode_specific_info.u.fdd->uarfcn_DL,tmp_dl_uarfcn);
  /* Store DL freq */
  frequecny_info->uarfcn_dl = tmp_dl_uarfcn;
  WRRC_MSG1_HIGH("IFreq:DL Freq is %d", tmp_dl_uarfcn);
  /* Calculate the UL Freq */
  /* Store the UL Freq */
  frequecny_info->uarfcn_ul_incl = FALSE;
  if (mode_specific_info.u.fdd->m.uarfcn_ULPresent)
  {
    
    if(rrccsp_get_uarfcn_ul(mode_specific_info.u.fdd->uarfcn_UL, rrc_get_frequency_band(mode_specific_info
      .u.fdd->uarfcn_DL,plmn_id),tmp_band_class, &tmp_ul_uarfcn) == FALSE)
    {
      WRRC_MSG1_HIGH("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",tmp_ul_uarfcn);
      return FALSE;
    }
    if(tmp_ul_uarfcn > FDD_ARFCN)
    {
      WRRC_MSG1_HIGH("IFreq:Invalid UL ARFCN %d.Ignore Inter Freq Cell",mode_specific_info.u.fdd->uarfcn_UL);
      return FALSE;
    }
    WRRC_MSG2_HIGH("IFreq:unsupported uarfcn %d supported uarfcn %d",mode_specific_info.u.fdd->uarfcn_UL,tmp_ul_uarfcn);
    /* Store the UL Freq */
    frequecny_info->uarfcn_ul_incl = TRUE;
    frequecny_info->uarfcn_ul = (uint16)tmp_ul_uarfcn;
  }
  return TRUE;
}
#endif
/*===========================================================================

FUNCTION   rrcifreq_build_composite_cell_list

DESCRIPTION

    This function builds the composite inter freq cell info list from SIB 11
    and SIB12.

DEPENDENCIES

    None

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_build_composite_cell_list
(
rrc_RemovedInterFreqCellList *removedInterFreqCellList,
l1_inter_freq_cell_list_struct_type* sib11_cell_list_ptr,
l1_inter_freq_cell_list_struct_type* sib12_cell_list_ptr,
uint32 *num_rmv_cells
)
{
/* Initialize all variables */

  /* Stores no of intra-freq cells stored in SIB11 */
  uint32 sib11_added_cells = 0;
  uint32 idx=0;
  /* Stores no of intra-freq cells stored in SIB12 */
  uint32 sib12_added_cells = 0;

  /* Counts no of cells in SIB12 */
  uint32 sib12_count_cells = 0;

  /* Counts removed cells matched against new intra freq cells of SIB11 */
  uint32 sib12_r_cell = 0;

  /* Counts no of new intra freq cells in SIB11 and helps in comparing
  * with no fo removed cells in SIB12 */
  uint32 sib11_a_cell = 0;

  /* If removed cell is found */
  uint32 match_found = 0;

  uint16 new_cell_list = 0;
  uint16 new_cell_list_from_sib11 = 0;
  uint16 count = 0;
  uint32 cell_id;
  l1_inter_freq_cell_list_struct_type *inter_freq_sib12_new_cell_list =  rrc_malloc(sizeof(l1_inter_freq_cell_list_struct_type));



  /* Stores ptr to SIB12 list */
  l1_inter_freq_cell_list_struct_type * new_cell_list_ptr = NULL;

  sib11_added_cells = sib11_cell_list_ptr->num_cell_add;

  if(removedInterFreqCellList->t == T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells)
  {
    /* No cells need to be picked up from SIB 11 */
    sib12_cell_list_ptr->num_cell_rmv = 0;
    rrc_free(inter_freq_sib12_new_cell_list);
    return;
  }

  /* Criteria: Match removed cell of SIB12 against all cells of SIB11
  * one at a time */

  for ( sib11_a_cell=0 ; sib11_a_cell<sib11_added_cells; sib11_a_cell++ )
  {
    
    for ( sib12_r_cell=0; sib12_r_cell < (*num_rmv_cells); sib12_r_cell++ )
    {
  
      if(removedInterFreqCellList->t == T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells)
      {
        idx=0;
        if(removedInterFreqCellList->u.removeSomeInterFreqCells->n> idx)
        {
          if(tmp_inter_freq_cell_list[removedInterFreqCellList->u.removeSomeInterFreqCells->elem[idx]].psc !=
            sib11_cell_list_ptr->add_cell[sib11_a_cell].psc)
          {
            idx++;
            continue; /* Read next value of removed cell*/
          }
        }
        match_found = 1;
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

      inter_freq_sib12_new_cell_list->add_cell[new_cell_list].measure_in_idle_pch_fach =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].measure_in_idle_pch_fach;

      inter_freq_sib12_new_cell_list->add_cell[new_cell_list].psc =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].psc;

      inter_freq_sib12_new_cell_list->add_cell[new_cell_list].cell_info =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].cell_info;

      inter_freq_sib12_new_cell_list->add_cell[new_cell_list].freq_info =
        sib11_cell_list_ptr->add_cell[sib11_a_cell].freq_info;

      /* Increment counter */
      new_cell_list++;

    } /* End of if */

    /* Reinitialize match_found variable */
    match_found = 0;

    /* Read next SIB 11 cell */

  } /* End of for sib11_a_cell */

  new_cell_list_ptr = inter_freq_sib12_new_cell_list;
  new_cell_list_from_sib11 = new_cell_list;

  /* Check if SIB12 new intra freq cells have id same as the one already present
   * from SIB 11 if so overwrite, if not append to the list */
  for ( sib12_added_cells =0; sib12_added_cells < sib12_cell_list_ptr->num_cell_add && new_cell_list < L1_MAX_CELL_MEAS;
  sib12_added_cells++ )
  {
    for(cell_id = 0; cell_id < L1_MAX_CELL_MEAS; cell_id++)
    {
      if(inter_freq_cell_list[cell_id].psc == sib12_cell_list_ptr->add_cell[sib12_added_cells].psc)
      {
        break;
      }
    }
    if((cell_id < L1_MAX_CELL_MEAS) && (tmp_inter_freq_cell_list[cell_id].psc != INVALID_PSC))
    {
      for(count = 0; count < new_cell_list_from_sib11; count++)
      {
        if(tmp_inter_freq_cell_list[cell_id].psc
          == new_cell_list_ptr->add_cell[count].psc)
        {
          new_cell_list_ptr->add_cell[count].psc =
            sib12_cell_list_ptr->add_cell[sib12_added_cells].psc;

          new_cell_list_ptr->add_cell[count].measure_in_idle_pch_fach =
            sib12_cell_list_ptr->add_cell[sib12_added_cells].measure_in_idle_pch_fach;
          new_cell_list_ptr->add_cell[count].cell_info =
            sib12_cell_list_ptr->add_cell[sib12_added_cells].cell_info;

          new_cell_list_ptr->add_cell[count].freq_info =
            sib12_cell_list_ptr->add_cell[sib12_added_cells].freq_info;

          break;
        }
      }
      if(count == new_cell_list_from_sib11)
      {
        new_cell_list_ptr->add_cell[new_cell_list].psc =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].psc;

         new_cell_list_ptr->add_cell[new_cell_list].measure_in_idle_pch_fach =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].measure_in_idle_pch_fach;
        new_cell_list_ptr->add_cell[new_cell_list].cell_info =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].cell_info;

        new_cell_list_ptr->add_cell[new_cell_list].freq_info =
            sib12_cell_list_ptr->add_cell[sib12_added_cells].freq_info;

        new_cell_list++;
      }
    }
    else
    {
      new_cell_list_ptr->add_cell[new_cell_list].measure_in_idle_pch_fach =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].measure_in_idle_pch_fach;
      new_cell_list_ptr->add_cell[new_cell_list].psc =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].psc;

      new_cell_list_ptr->add_cell[new_cell_list].cell_info =
        sib12_cell_list_ptr->add_cell[sib12_added_cells].cell_info;

      new_cell_list_ptr->add_cell[new_cell_list].freq_info =
          sib12_cell_list_ptr->add_cell[sib12_added_cells].freq_info;

      new_cell_list++;
    }
  } /* End of for */

  /* Store the no of cells */
  sib12_cell_list_ptr->num_cell_add = new_cell_list;

  MSG_LOW("There are %d cells in SIB12 cell list", new_cell_list, 0,0);

  /* Now copy the structure in SIB12 */
  for ( sib12_count_cells =0; sib12_count_cells<new_cell_list && sib12_count_cells < L1_MAX_CELL_MEAS;
  sib12_count_cells++ )
  {

    sib12_cell_list_ptr->add_cell[sib12_count_cells].psc =
      new_cell_list_ptr->add_cell[sib12_count_cells].psc;
    sib12_cell_list_ptr->add_cell[sib12_count_cells].measure_in_idle_pch_fach =
      new_cell_list_ptr->add_cell[sib12_count_cells].measure_in_idle_pch_fach;

    sib12_cell_list_ptr->add_cell[sib12_count_cells].cell_info =
      new_cell_list_ptr->add_cell[sib12_count_cells].cell_info;

     sib12_cell_list_ptr->add_cell[sib12_count_cells].freq_info =
      new_cell_list_ptr->add_cell[sib12_count_cells].freq_info;

  } /* End of for */

    /* Initialize no of removed cells to 0 as SIB12 intra-freq cell info list has
  * already taken them into account */
  sib12_cell_list_ptr->num_cell_rmv = 0;
  rrc_free(inter_freq_sib12_new_cell_list);
}

/*===========================================================================

FUNCTION   IFREQ_READ_SIB12

DESCRIPTION

    This function reads Inter Freq Cell List from SIB 12 and
    stores it in inter_freq_meas_id_list. This Inter Freq
    Cell List is used for cell reselection. When UTRAN sends a meas
    in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

    This function assumes that SIB12 is present and that the sib12_ptr
    != NULL

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_read_sib12
(
 rrc_SysInfoType12 *sib12_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib11_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib12_ptr
)
{

  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterFreqCellsList *tmp_rrc_PLMNsOfInterFreqCellsList_ptr = NULL;
  rrc_MultiplePLMNsOfInterFreqCellsList *tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = NULL;

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR   
  rrc_csp_band_class_type tmp_band_class = RRC_CSP_BAND_MAX;
  boolean band_found = FALSE;
  int mfbi_index = -1;
#endif
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  boolean sib18_present = FALSE;
  uint32 idx=0;
  /* Local variables for storing value from SIB11 */
  rrc_InterFreqMeasurementSysInfo_RSCP* inter_freq_meas_rscp_ptr = NULL;

  rrc_InterFreqMeasurementSysInfo_ECN0* inter_freq_meas_ecno_ptr = NULL;

  rrc_InterFreqMeasurementSysInfo_HCS_RSCP* hcs_inter_freq_meas_rscp_ptr = NULL;

  rrc_InterFreqMeasurementSysInfo_HCS_ECN0* hcs_inter_freq_meas_ecno_ptr = NULL;

  /* Ptrs to go thr linked list */
  rrc_NewInterFreqCellSI_List_RSCP * rscp_cell_list_ptr = NULL;

  rrc_NewInterFreqCellSI_List_ECN0 * ecno_cell_list_ptr = NULL;

  rrc_NewInterFreqCellSI_List_HCS_RSCP * hcs_rscp_cell_list_ptr = NULL;

  rrc_NewInterFreqCellSI_List_HCS_ECN0 * hcs_ecno_cell_list_ptr = NULL;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
  rrc_MultipleFrequencyBandIndicatorListFDD *inter_freq_multifreq_ptr = NULL;
#endif
  uint32 count = 0;

  uint8 cell_count = 0;

  uint32 cell_position = 0;

  uint32 num_rmv_cells = 0;
  rrc_SysInfoType12_v590ext_IEs *sib12_rel5_ext_ptr = NULL;
  rrc_SysInfoType12_v590ext_IEs_newInterFrequencyCellInfoList_v590ext * rel5_ext_ptr = NULL;

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

  /* Initialize L1 params */
  l1_inter_freq_sib12_ptr->num_cell_add = 0;
  l1_inter_freq_sib12_ptr->num_cell_meas = 0;
  l1_inter_freq_sib12_ptr->num_cell_rmv = 0;
  l1_inter_freq_sib12_ptr->cells_for_meas_incl = FALSE;
  l1_inter_freq_sib12_ptr->hcs_used = FALSE;
  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    
    return FAILURE;
  }

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
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
          connectedModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis,
          plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
          connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
      WRRC_MSG0_HIGH("SIB11bis:SIB12 processing SIB18 Conn Inter Freq from bis extension ");
      sib18_present = TRUE;
    }
    else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
            (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
              idleModePLMNIdentitiesSIB11bis)) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
              plmnsOfInterFreqCellsList)) &&
            (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
          idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
      WRRC_MSG0_HIGH("SIB11bis:SIB12 processing SIB18 idle Inter Freq from bis extension ");
      sib18_present = TRUE;
    }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB12 processing SIB18 Conn Multiple PLMNs Inter Freq");
      sib18_present = TRUE;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, connectedModePLMNIdentities)) &&
      (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities, plmnsOfInterFreqCellsList)) &&
      (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList;
      WRRC_MSG0_HIGH("SIB12 processing SIB18 Conn Inter Freq");
      sib18_present = TRUE;
    }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB12 processing SIB18 Idle Multiple PLMNs Inter Freq");
      sib18_present = TRUE;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18,idleModePLMNIdentities)) &&
      (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterFreqCellsList)) &&
      (sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
    {
      tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList;
      WRRC_MSG0_HIGH("SIB12 processing SIB18 Idle Inter Freq");
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

  for(count = 0; count < L1_MAX_CELL_MEAS; count++)
  {
    tmp_inter_freq_cell_list[count] = inter_freq_cell_list[count];
  }

  /* Read SIB 12 parameters */
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
      /* Check for REL 5 extension ptr */
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib12_ptr,rrc_SysInfoType12,v4b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE(sib12_ptr->v4b0NonCriticalExtensions,
        rrc_SysInfoType12_v4b0NonCriticalExtensions,v590NonCriticalExtension)))
      {
        sib12_rel5_ext_ptr = &(sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
          sysInfoType12_v590ext);
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(sib12_rel5_ext_ptr,
            rrc_SysInfoType12_v590ext_IEs,newInterFrequencyCellInfoList_v590ext))
        {
          rel5_ext_ptr = &sib12_rel5_ext_ptr->newInterFrequencyCellInfoList_v590ext;
        }
      }
  }

#ifdef  FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
  #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
  /* Check if HCS is in use. */
  if ( sib12_ptr->measurementControlSysInfo.use_of_HCS.t ==
       T_rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used )
  {
    
    /* start processing HCS */
    if ( sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      cellSelectQualityMeasure.t ==
      T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_1_cpich_RSCP )
    {
      /* The Cell Select Quality Measure is RSCP */
      if (!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_RSCP->m.interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys RSCP info is absent */
        /* No need to initialize inter_freq_meas_info_incl as it is already set to FALSE */
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          WRRC_MSG0_HIGH("Inter freq meas sys info absent in SIB 12 pick it from SIB11 ");
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          l1_inter_freq_sib12_ptr->hcs_used = TRUE;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      /* Stores RSCP sys info in the following var */
      hcs_inter_freq_meas_rscp_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_RSCP,interFreqCellInfoSI_List)))
      {
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          WRRC_MSG0_HIGH("Inter freq cell info list absent in SIB 12 pick it from SIB11 ");
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          l1_inter_freq_sib12_ptr->hcs_used = TRUE;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      if((RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_RSCP ,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_HCS_RSCP,removedInterFreqCellList)))
      {
        rrcifreq_process_remove_inter_freq_cells
          (&hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.removedInterFreqCellList, &num_rmv_cells);
      }

      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_RSCP,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_HCS_RSCP,newInterFreqCellList)) &&
        (hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        


        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;

        /* Store the ptr in a local variable */
        hcs_rscp_cell_list_ptr = &hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList;

        while ( (hcs_rscp_cell_list_ptr->n > idx) &&
          (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */

          /* Get the Freq Info */
          if (RRC_MSG_LIST_BITMASK_IE(hcs_rscp_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_RSCP,frequencyInfo))
          {
            if (hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("IFreq:Ignore Cell. TDD cell",0,0,0);
              idx++;
              continue;
            }
            if (validate_frequency_info(&hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
              WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
              idx++;
              /*Ignore all the cells till we find next freq*/
              while((hcs_rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(hcs_rscp_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_RSCP,frequencyInfo)))
              {
                idx++;
              }
              continue;
            }

            if (hcs_rscp_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
            {
              WRRC_MSG1_HIGH("IFreq:Invalid DL ARFCN %d Ignore Inter Freq Cell",hcs_rscp_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
             idx++;
              continue;
            }

            /* Store DL freq */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl = (uint16) hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

            WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib12_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl);

            /* Calculate the UL Freq */
            /* Store the UL Freq */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
              uarfcn_ul_incl = FALSE;

            if (hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
                uarfcn_ULPresent)
            {
              

              if (hcs_rscp_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
              {
                MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",hcs_rscp_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                idx++;
                continue;
              }
              /* Store the UL Freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = TRUE;

              l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_ul = (uint16)
                hcs_rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
            }
          }
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
              idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib12_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
            }
          }
          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
            }
          }
          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_rscp_cell_info(&hcs_rscp_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(hcs_rscp_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_RSCP,interFreqCellID))
          {
            cell_position = hcs_rscp_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */


          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        } /* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib12_ptr->num_cell_add = cell_count;

        if(!(RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_HCS_RSCP,removedInterFreqCellList)))
        {
          hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.removedInterFreqCellList.t =
            T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells;
        }
        rrcifreq_build_composite_cell_list(&hcs_inter_freq_meas_rscp_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList,
          l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);

      } /* Inter freq cell info list is present */
      else
      {
        

        if((RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_rscp_ptr,
          rrc_InterFreqMeasurementSysInfo_HCS_RSCP,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_HCS_RSCP,removedInterFreqCellList)))
        {
          l1_inter_freq_sib12_ptr->num_cell_add = 0;
          rrcifreq_build_composite_cell_list(&hcs_inter_freq_meas_rscp_ptr->
            interFreqCellInfoSI_List.removedInterFreqCellList,
            l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);
        }
        else
        {
          if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
          {
            WRRC_MSG0_HIGH("IFreq:Inter freq meas sys info absent in SIB 12 pick it from SIB11 ");
            rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
              (l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          }
        }
      }
      for(cell_count = 0; cell_count < l1_inter_freq_sib12_ptr->num_cell_add; cell_count++)
      {
        if(sib18_present)
        {
          rrcmeas_process_sib18_filtering(
             (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
             (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
             &tmp_plmn_identity,
             cell_count,
             &(l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
             (cell_count > 0)?l1_inter_freq_sib12_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
        }
        else
        {
          /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
          MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib12_ptr->add_cell[cell_count].psc,0,0);
        }
      }
    } /* End of if of RSCP chosen */
    else /* rrc_hcs_used_cellSelectQualityMeasure_cpich_Ec_N0 */
    {
      /* Store first the cell selection and reselection quality parameter */
      //rrc_inter_freq_sib_ptr->cell_sel_resel_quan_type =  L1_EC_NO;

      if (!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->m.interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys ECN0 info is absent */
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          l1_inter_freq_sib12_ptr->hcs_used = TRUE;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      /* Stores EcNo sys info in the following var */
      hcs_inter_freq_meas_ecno_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;
      if(!(hcs_inter_freq_meas_ecno_ptr->m.interFreqCellInfoSI_ListPresent))
      {
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq cell info list absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          l1_inter_freq_sib12_ptr->hcs_used = TRUE;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }
      if((hcs_inter_freq_meas_ecno_ptr->m.interFreqCellInfoSI_ListPresent) &&
        (hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.m.removedInterFreqCellListPresent))
      {
        rrcifreq_process_remove_inter_freq_cells
          (&hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.removedInterFreqCellList, &num_rmv_cells);
      }

      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_ecno_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_ECN0,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_HCS_ECN0,newInterFreqCellList)) &&
        (hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        

        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;

        /* Store the ptr in a local variable */
        hcs_ecno_cell_list_ptr = &hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList;
        idx=0;
        while ( (hcs_ecno_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max that is allowed by specs */

          /* Get the Freq Info */
          if (RRC_MSG_LIST_BITMASK_IE(hcs_ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo))
          {
            if (hcs_ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent/TDD");
              idx++;
              continue;
            }
            if (validate_frequency_info(&hcs_ecno_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
              WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                hcs_ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
              idx++;
              /*Ignore all the cells till we find next freq*/
              while((hcs_ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(hcs_ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
              {
                idx++;
              }

              continue;
            }
            if (hcs_ecno_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
            {
              MSG_LOW("IFreq:Invalid DL ARFCN %d Ignore Inter Freq Cell",hcs_ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
              idx++;
              continue;
            }

            /* Store DL freq */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl = (uint16) hcs_ecno_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

            WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib12_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl);

            /* Calculate the UL Freq */
            /* Store the UL Freq */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
              uarfcn_ul_incl = FALSE;

            if (hcs_ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
            {
              

              if (hcs_ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
              {
                MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",hcs_ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                idx++;
                continue;
              }

              /* Store the UL Freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = TRUE;

              l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_ul = (uint16)
                hcs_ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
            }
          }
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
              idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib12_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
            }
          }

          /* Downlink Freq Present */
          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 24 * 2;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
              l1_inter_freq_sib12_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset2 =
                l1_inter_freq_sib12_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset2;
            }
          }
          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_ecn0_cell_info(&hcs_ecno_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(hcs_ecno_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_ECN0,interFreqCellID))
          {
            cell_position = hcs_ecno_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */

          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        } /* End of while */
         /* Set number of added cells */
        l1_inter_freq_sib12_ptr->num_cell_add = cell_count;

        if(!(RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_HCS_ECN0,removedInterFreqCellList)))
        {
          hcs_inter_freq_meas_ecno_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList.t = T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells;
        }
        rrcifreq_build_composite_cell_list(&hcs_inter_freq_meas_ecno_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList,
          l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);

      } /* Inter freq cell info list is present */
      else
      {
        
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(hcs_inter_freq_meas_ecno_ptr,
          rrc_InterFreqMeasurementSysInfo_HCS_ECN0,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(hcs_inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_HCS_ECN0,removedInterFreqCellList)))
        {
          l1_inter_freq_sib12_ptr->num_cell_add = 0;
          rrcifreq_build_composite_cell_list(&hcs_inter_freq_meas_ecno_ptr->
            interFreqCellInfoSI_List.removedInterFreqCellList,
            l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);
        }
        else
        {
          if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
          {
            MSG_LOW("IFreq:Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
            rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
              (l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          }
        }
      }
      for(cell_count = 0; cell_count < l1_inter_freq_sib12_ptr->num_cell_add; cell_count++)
      {
        if(sib18_present)
        {
          rrcmeas_process_sib18_filtering(
             (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
             (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
             &tmp_plmn_identity,
             cell_count,
             &(l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
             (cell_count > 0)?l1_inter_freq_sib12_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
        }
        else
        {
          /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
          MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib12_ptr->add_cell[cell_count].psc,0,0);
        }
      }
    } /* End of else of EcNo chosen */
    /* end processing HCS */
    l1_inter_freq_sib12_ptr->hcs_used = TRUE;
  }
  else
  { /* HCS is NOT in use */
    /* Set HCS flag to FALSE */

    if ( sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
      cellSelectQualityMeasure.t ==
      T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_1_cpich_RSCP )
    {
      /* The Cell Select Quality Measure is RSCP */
      if (!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        cellSelectQualityMeasure.u.cpich_RSCP->m.
        interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys RSCP info is absent */
        /* No need to initialize inter_freq_meas_info_incl as it is already set to FALSE */
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      /* Stores RSCP sys info in the following var */
      inter_freq_meas_rscp_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;
      if(!(inter_freq_meas_rscp_ptr->m.interFreqCellInfoSI_ListPresent))
      {
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq cell info list absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      if((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_RSCP,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_RSCP,removedInterFreqCellList)))
      {
        rrcifreq_process_remove_inter_freq_cells
          (&inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.removedInterFreqCellList, &num_rmv_cells);
      }

      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_RSCP,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_RSCP,newInterFreqCellList)) &&
        (inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        


        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;

        /* Store the ptr in a local variable */
        rscp_cell_list_ptr = &inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList;
        idx=0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        while ( (rscp_cell_list_ptr->n > idx) &&
          (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          /* Get the Freq Info */
          if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_RSCP,frequencyInfo))
          {
            if (rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("IFreq:Ignore Cell. TDD cell",0,0,0);
              idx++;
              continue;
            }
            if (validate_frequency_info(&rscp_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              
              /*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if( (rrc_mfbi_support_nv) && (sib12_ptr != NULL) && 
                 (sib12_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) && 
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v690NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v7b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions
                                          .v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].
                                           multipleFrequencyBandIndicatorListFDD);
                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);
               
                if(band_found != TRUE)
                {
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_RSCP,frequencyInfo)))
                  {
                     idx++;
                  }
                  continue;
                }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
                idx++;
                /*Ignore all the cells till we find next freq*/
                while((rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_RSCP,frequencyInfo)))
                {
                   idx++;
                }
                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB12");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
            }
            else
            {
#endif
              if (rscp_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                MSG_HIGH("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
                idx++;
                continue;
              }

              /* Store DL freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

              MSG_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl,0,0);

              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;

              if (rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                

                if (rscp_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                  idx++;
                  continue;
                }
                /* Store the UL Freq */
                l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;

                l1_inter_freq_sib12_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
              idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib12_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }

          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_rscp_cell_info(&rscp_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            MSG_LOW("IFreq:Ignore InterFreq Cell",0,0,0);
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,
            rrc_NewInterFreqCellSI_RSCP,interFreqCellID))
          {
            cell_position = rscp_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */


          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;

               /* set the measure in PCH IDLE and FACH flag to TRUE */
               l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        } /* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib12_ptr->num_cell_add = cell_count;

        if(!(RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_RSCP,removedInterFreqCellList)))
        {
          inter_freq_meas_rscp_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList.t = T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells;
        }
        rrcifreq_build_composite_cell_list(&inter_freq_meas_rscp_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList,
          l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);
      } /* Inter freq cell info list is present */
      else
      {
        

        if((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_rscp_ptr,
          rrc_InterFreqMeasurementSysInfo_RSCP,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_RSCP,removedInterFreqCellList)))
        {
          l1_inter_freq_sib12_ptr->num_cell_add = 0;
          rrcifreq_build_composite_cell_list(&inter_freq_meas_rscp_ptr->
            interFreqCellInfoSI_List.removedInterFreqCellList,
            l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);
        }
        else
        {
          if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
          {
            MSG_LOW("IFreq:Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
            rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
              (l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          }
        }
      }
      for(cell_count = 0; cell_count < l1_inter_freq_sib12_ptr->num_cell_add; cell_count++)
      {
        if(sib18_present)
        {
          rrcmeas_process_sib18_filtering(
             (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
             (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
             &tmp_plmn_identity,
             cell_count,
             &(l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
             (cell_count > 0)?l1_inter_freq_sib12_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
        }
        else
        {
          /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
          MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib12_ptr->add_cell[cell_count].psc,0,0);
        }
      }
    } /* End of if of RSCP chosen */
    else /* rrc_hcs_not_used_cellSelectQualityMeasure_cpich_Ec_N0 */
    {
      /* Store first the cell selection and reselection quality parameter */
      //rrc_inter_freq_sib_ptr->cell_sel_resel_quan_type =  L1_EC_NO;

      if (!(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->m.interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys ECN0 info is absent */
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }

      /* Stores EcNo sys info in the following var */
      inter_freq_meas_ecno_ptr = &sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;
      if(!(inter_freq_meas_ecno_ptr->m.interFreqCellInfoSI_ListPresent))
      {
        if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
        {
          MSG_LOW("Inter freq cell info list absent in SIB 12 pick it from SIB11 ",0,0,0);
          rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12(l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
          return SUCCESS;
        }
        else
        {
          return FAILURE;
        }
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_ecno_ptr,
        rrc_InterFreqMeasurementSysInfo_ECN0,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_ECN0,removedInterFreqCellList)))
      {
        rrcifreq_process_remove_inter_freq_cells
          (&inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.removedInterFreqCellList, &num_rmv_cells);
      }

      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_ecno_ptr,
        rrc_InterFreqMeasurementSysInfo_ECN0,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_ECN0,newInterFreqCellList)) &&
        (inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        

        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;
        idx=0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        /* Store the ptr in a local variable */
        ecno_cell_list_ptr = &inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList;

        while ( (ecno_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max that is allowed by specs */

          /* Get the Freq Info */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_ECN0,frequencyInfo))
          {
            if (ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("IFreq:Ignore Cell. DL Freq absent/TDD",0,0,0);
             idx++;
              continue;
            }
            if (validate_frequency_info(&ecno_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
 /*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if((rrc_mfbi_support_nv) && (sib12_ptr != NULL) && 
                 (sib12_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) && 
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v690NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v7b0NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext
                             .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib12_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions
                                          .v7b0NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType12_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].
                                           multipleFrequencyBandIndicatorListFDD);
                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);
               
                  if(band_found != TRUE)
                  {
                    idx++;
                    /*Ignore all the cells till we find next freq*/
                    while((ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                    {
                      idx++;
                    }
                    continue;
                  }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
                idx++;
                /*Ignore all the cells till we find next freq*/
                while((ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                {
                   idx++;
                }

                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB12");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
              of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                      ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                      &(l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info),
                                                      selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
            }
            else
            {
#endif
              if (ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                MSG_LOW("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
                idx++;
                continue;
              }

              /* Store DL freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

              WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib12_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl);

              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;

              if (ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                

                if (ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                  idx++;
                  continue;
                }

                /* Store the UL Freq */
                l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;

                l1_inter_freq_sib12_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
              idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib12_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib12_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }

          /* Downlink Freq Present */

          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_ecn0_cell_info(&ecno_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            MSG_LOW("IFreq:Ignore InterFreq Cell",0,0,0);
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,
            rrc_NewInterFreqCellSI_ECN0,interFreqCellID))
          {
            cell_position = ecno_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */

          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib12_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib12_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib12_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;

               /* set the measure in PCH IDLE and FACH flag to TRUE */
               l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        } /* End of while */
         /* Set number of added cells */
        l1_inter_freq_sib12_ptr->num_cell_add = cell_count;

        if(!(RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_ECN0,removedInterFreqCellList)))
        {
          inter_freq_meas_ecno_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList.t = T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells;
        }
        rrcifreq_build_composite_cell_list(&inter_freq_meas_ecno_ptr->
          interFreqCellInfoSI_List.removedInterFreqCellList,
          l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);

      } /* Inter freq cell info list is present */
      else
      {
        
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_ecno_ptr,
          rrc_InterFreqMeasurementSysInfo_ECN0,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_ECN0,removedInterFreqCellList)))
        {
          l1_inter_freq_sib12_ptr->num_cell_add = 0;
          rrcifreq_build_composite_cell_list(&inter_freq_meas_ecno_ptr->
            interFreqCellInfoSI_List.removedInterFreqCellList,
            l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr, &num_rmv_cells);
        }
        else
        {
          if(l1_inter_freq_sib11_ptr->num_cell_add != 0)
          {
            MSG_LOW("IFreq:Inter freq meas sys info absent in SIB 12 pick it from SIB11 ",0,0,0);
            rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
              (l1_inter_freq_sib11_ptr, l1_inter_freq_sib12_ptr);
          }
        }
      }
      for(cell_count = 0; cell_count < l1_inter_freq_sib12_ptr->num_cell_add; cell_count++)
      {
        if(sib18_present)
        {
          rrcmeas_process_sib18_filtering(
             (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
             (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
             &tmp_plmn_identity,
             cell_count,
             &(l1_inter_freq_sib12_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
             (cell_count > 0)?l1_inter_freq_sib12_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
        }
        else
        {
          /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
          MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib12_ptr->add_cell[cell_count].psc,0,0);
        }
      }
    } /* End of else of EcNo chosen */
  } /* Else of t is set to 'not hcs' */
  if(l1_inter_freq_sib12_ptr->num_cell_add != 0)
  {
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
}

/*===========================================================================

FUNCTION   IFREQ_READ_SIB11

DESCRIPTION

    This function reads Inter Freq Cell List from SIB 11 and
    stores it in inter_freq_meas_id_list. This Inter Freq
    Cell List is used for cell reselection. When UTRAN sends a meas
    in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

    -

RETURN VALUE



SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_read_sib11
(
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
)
{
  rrc_SysInfoType11 *sib11_ptr = NULL; /* Declared to store the value of SIB 11*/
  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterFreqCellsList *tmp_rrc_PLMNsOfInterFreqCellsList_ptr = NULL;
  rrc_MultiplePLMNsOfInterFreqCellsList *tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = NULL;

  boolean sib18_present = FALSE;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
  rrc_csp_band_class_type tmp_band_class = RRC_CSP_BAND_MAX;
  boolean band_found = FALSE;
  int mfbi_index = -1;
#endif
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;

  /* Local variables for storing value from SIB11 */
  rrc_InterFreqMeasurementSysInfo_RSCP* inter_freq_meas_rscp_ptr = NULL;

  rrc_InterFreqMeasurementSysInfo_ECN0* inter_freq_meas_ecno_ptr = NULL;

  /* Ptrs to go thr linked list */
  rrc_NewInterFreqCellSI_List_RSCP * rscp_cell_list_ptr = NULL;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR  
  rrc_MultipleFrequencyBandIndicatorListFDD *inter_freq_multifreq_ptr = NULL;
#endif
  rrc_NewInterFreqCellSI_List_ECN0 * ecno_cell_list_ptr = NULL;
  uint32 idx=0;
  uint8 cell_count = 0;

  uint32 cell_position = 0;

  rrc_SysInfoType11_v590ext_IEs *sib11_rel5_ext_ptr = NULL;
  rrc_SysInfoType11_v590ext_IEs_newInterFrequencyCellInfoList_v590ext * rel5_ext_ptr = NULL;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
  #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

  uint16 prev_cell_count =0;

  sib11_ptr = (rrc_SysInfoType11*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB11);

  /* Initialize L1 params */
  l1_inter_freq_sib_ptr->num_cell_add = 0;
  l1_inter_freq_sib_ptr->num_cell_meas = 0;
  l1_inter_freq_sib_ptr->num_cell_rmv = 0;
  l1_inter_freq_sib_ptr->cells_for_meas_incl = FALSE;
  l1_inter_freq_sib_ptr->hcs_used = FALSE;
  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip Processing interFreq SIB11 measurements");
    return FAILURE;
  }

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
          (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
            connectedModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis,
            plmnsOfInterFreqCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 Conn Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
              (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
                idleModePLMNIdentitiesSIB11bis)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
                plmnsOfInterFreqCellsList)) &&
              (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n= 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 idle Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Conn Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, connectedModePLMNIdentities)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities, plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Conn Inter Freq",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, idleModePLMNIdentities)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities, plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Idle Inter Freq",0,0,0);
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
          (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
            idleModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
            plmnsOfInterFreqCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 Idle Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, idleModePLMNIdentities)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterFreqCellsList)) &&
              (sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Idle Inter Freq ",0,0,0);
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
    MSG_LOW("UE in limited service ignoring SIB18",0,0,0);
  }

  /* Check if the value received is O.K. */
  

  if ( sib11_ptr == NULL )
  {
    WRRC_MSG0_ERROR("IFreq:SIB 11 from SIB DB is NULL");

    return FAILURE;
  }
  else
  {
    /* Read SIB 11 parameters */
    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
    {
      /* Check for REL 5 extension ptr */
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib11_ptr,rrc_SysInfoType11,v4b0NonCriticalExtensions)) &&
        (sib11_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent))
      {
        sib11_rel5_ext_ptr = &(sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
          sysInfoType11_v590ext);
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(sib11_rel5_ext_ptr,
            rrc_SysInfoType11_v590ext_IEs,newInterFrequencyCellInfoList_v590ext))
        {
          rel5_ext_ptr = &sib11_rel5_ext_ptr->newInterFrequencyCellInfoList_v590ext;
        }
      }
    }
#ifdef  FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
    /* Check if HCS is in use. */
    if ( sib11_ptr->measurementControlSysInfo.use_of_HCS.t ==
         T_rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used )
    {
      
      if ((rrcifreq_fill_hcs_info(l1_inter_freq_sib_ptr, 
        (rrc_SysInfoType12_v590ext_IEs_newInterFrequencyCellInfoList_v590ext *) rel5_ext_ptr) == FAILURE))
      {
        return FAILURE;
      }
      l1_inter_freq_sib_ptr->hcs_used = TRUE;
    }
    else
    { /* HCS is NOT in use */
      /* Set HCS flag to FALSE */

      if ( sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
        cellSelectQualityMeasure.t ==
        T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_1_cpich_RSCP )
      {
        /* The Cell Select Quality Measure is RSCP */
        if (!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
          cellSelectQualityMeasure.u.cpich_RSCP->m.interFreqMeasurementSysInfoPresent))
        {
          /* Optional param Inter freq meas sys RSCP info is absent */
          /* No need to initialize inter_freq_meas_info_incl as it is already set to FALSE */

          MSG_LOW("IFreq:Inter freq meas sys info absent",0,0,0);

          /*
            SIB11bis has the cell list, SIb11 doesnt have the cell list
          */
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
            return FAILURE;
          }
          return SUCCESS;
        }

        /* Stores RSCP sys info in the following var */
        inter_freq_meas_rscp_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
          cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;


        /* Check inter-freq cell info present */
        if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_rscp_ptr,
          rrc_InterFreqMeasurementSysInfo_RSCP,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_RSCP,newInterFreqCellList)) &&
          (inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

        {
          


          /* Always ignore the number of removed cells in System Info 11
          as there is nothing to delete initially */

          /*  Process the new cell list */

          cell_count = 0;

          /* Store the ptr in a local variable */
          rscp_cell_list_ptr = &inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList;
          idx=0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index = -1;
#endif
          while ( (rscp_cell_list_ptr->n > idx) &&
            (cell_count < L1_MAX_CELL_MEAS) )
          {
          /* Pointer to next cell is Non-NULL and cell count
            hasn't exceeded the max as per specs */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            mfbi_index++; 
            band_found = FALSE;
#endif
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;

            /* Get the Freq Info */
            if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,
                rrc_NewInterFreqCellSI_RSCP,frequencyInfo))
            {
              if (rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
                T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
              {
                MSG_LOW("IFreq:Ignore Cell. TDD cell",0,0,0);
                idx++;
                continue;
              }

              if (validate_frequency_info(&rscp_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
              {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
                /*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
                if((rrc_mfbi_support_nv) && (sib11_ptr != NULL) && 
                  (sib11_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) && 
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v690NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.m.v7b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .m.multipleFrequencyInfoListFDDPresent) &&
                  ((sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
                {
                  inter_freq_multifreq_ptr=&(sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.
                                             v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                             sysInfoType11_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);
                  rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                      &band_found,
                                                                      &tmp_band_class);
                  if(band_found != TRUE)
                  {
                    idx++;
                    /*Ignore all the cells till we find next freq*/
                    while((rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                    {
                       idx++;
                    }

                    continue;
                  }
                }
                else
                {
#endif
                  WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                    rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                     idx++;
                  }

                  continue;
                }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              }
              if(band_found)
              {
                WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11");
                /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
                if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
                {
                  idx++;
                  continue;
                }
              }
              else
              {
#endif
                if (rscp_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
                {
                  WRRC_MSG1_HIGH("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
                  idx++;
                  continue;
                }

                /* Store DL freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_dl = (uint16) rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

                WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_dl);

                /* Calculate the UL Freq */
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = FALSE;
 
                if (rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
                  uarfcn_ULPresent)
                {
                  

                  if (rscp_cell_list_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                  {
                    MSG_LOW("IFreq:Invalid UL ARFCN %d.Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                      frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                    idx++;
                    continue;
                  }
                  /* Store the UL Freq */
                  l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                    uarfcn_ul_incl = TRUE;

                  l1_inter_freq_sib_ptr->add_cell[cell_count].
                    freq_info.uarfcn_ul = (uint16)
                    rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
                }
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
#endif
            else
            {
              if (cell_count == 0)
              {
                WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
                idx++;
                continue;
              }
              else
              {
                /* This implies that there is already one DL and UL freq present. Copy from there */
                WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                             sizeof(l1_freq_info_struct_type),
                             &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                             sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
                mfbi_index--;
#endif
              }
            }

            /* First store cell info so that psc of this cell can be extracted */
            if (rrcmeas_fill_rscp_cell_info(&rscp_cell_list_ptr->elem[idx].
              cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
            {
              MSG_LOW("IFreq:Ignore InterFreq Cell",0,0,0);
              idx++;
              continue;
            }

            /* Check if inter-freq cell ID is present */
            if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,
              rrc_NewInterFreqCellSI_RSCP,interFreqCellID))
            {
              cell_position = rscp_cell_list_ptr->elem[idx].interFreqCellID;
            } /* End of if of intra-freq cell id present */
            else
            {
              cell_position = AVAILABLE_CELL_POSITION;
            } /* End of else of inter-freq cell id not present */


            if ( rrcifreq_update_inter_freq_cell_list(
              &cell_position,
              &l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code,
              &l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl) == SUCCESS)
            {
              /* PSC sucessfully stored in the cell info list variable */
              /* Now store psc in the sib11 in add cell for layer1 to use psc */
              l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                cell_info.pri_scr_code;

              /* set the measure in PCH IDLE and FACH flag to TRUE */
              l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
              if(sib18_present)
              {
                rrcmeas_process_sib18_filtering(
                   (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                   (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                   &tmp_plmn_identity,
                   cell_count,
                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                   (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
              }
              else
              {
                /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
                MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib_ptr->add_cell[cell_count].psc,0,0);
              }
              /* Increment cell count as add cell successfully stored */
              cell_count ++;
            }  /* End of if of rrcmeas_update_inter_freq_cell_list */

            /* Get the next cell */
            idx++;
          } /* End of while */
          /* Set number of added cells */
          l1_inter_freq_sib_ptr->num_cell_add = cell_count;

          if (l1_inter_freq_sib_ptr->num_cell_add == 0)
          {
            WRRC_MSG0_HIGH("IFreq:No new cell. SIB req not sent");
            if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
            {
              MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
              return FAILURE;
            }
            return SUCCESS;
          }

        } /* Inter freq cell info list is present */
        else
        {
          
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            WRRC_MSG0_HIGH("SIB11bis: Reading inter frequency failure");
            return FAILURE;
          }
          return SUCCESS;
        }
      } /* End of if of RSCP chosen */
      else /* rrc_hcs_not_used_cellSelectQualityMeasure_cpich_Ec_N0 */
      {
        /* Store first the cell selection and reselection quality parameter */
        //rrc_inter_freq_sib_ptr->cell_sel_resel_quan_type =  L1_EC_NO;
   
        if (!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
          cellSelectQualityMeasure.u.cpich_Ec_N0->m.interFreqMeasurementSysInfoPresent))
        {
          /* Optional param Inter freq meas sys ECN0 info is absent */
          
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
            return FAILURE;
          }
          return SUCCESS;
        }
   
        /* Stores EcNo sys info in the following var */
        inter_freq_meas_ecno_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
          cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;
   
   
        /* Check inter-freq cell info present */
        if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_ecno_ptr,
          rrc_InterFreqMeasurementSysInfo_ECN0,interFreqCellInfoSI_List)) &&
          (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
          rrc_InterFreqCellInfoSI_List_ECN0,newInterFreqCellList)) &&
          (inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))
   
        {
          
   
          /* Always ignore the number of removed cells in System Info 11
          as there is nothing to delete initially */
   
          /*  Process the new cell list */
   
          cell_count = 0;
   
          /* Store the ptr in a local variable */
          ecno_cell_list_ptr = &inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList;
          idx=0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index = -1;
#endif
          while ( (ecno_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS) )
          {
          /* Pointer to next cell is Non-NULL and cell count
            hasn't exceeded the max that is allowed by specs */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            mfbi_index++;
            band_found = FALSE;
#endif
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 24 * 2;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
   
            /* Get the Freq Info */
            if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_ECN0,frequencyInfo))
            {
              if (ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
                T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
              {
                MSG_LOW("IFreq:Ignore Cell. DL Freq absent/TDD",0,0,0);
                idx++;
                continue;
              }
   
              if (validate_frequency_info(&ecno_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
              {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
/*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
                if( (rrc_mfbi_support_nv) && (sib11_ptr != NULL) && 
                  (sib11_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent) && 
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v690NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.m.v7b0NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .m.multipleFrequencyInfoListFDDPresent) &&
                  ((sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                  (sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11_va80ext
                              .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
                {
                  inter_freq_multifreq_ptr=&(sib11_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v690NonCriticalExtensions.v6b0NonCriticalExtensions.
                                             v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                             sysInfoType11_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);
                  rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                      &band_found,
                                                                      &tmp_band_class);
                  if(band_found != TRUE)
                  {
                    idx++;
                    /*Ignore all the cells till we find next freq*/
                    while((ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                    {
                       idx++;
                    }
                    continue;
                  }
                }
                else
                {
#endif
                  WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                    ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                     idx++;
                  }

                  continue;
                }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              }
              if(band_found)
              {
                WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11");
                /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
                if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
                {
                  idx++;
                  continue;
                }
              }
              else
              {
#endif
                if (ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
                {
                  MSG_LOW("IFreq:Invalid DL ARFCN %d.Ignore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                     frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
                  idx++;
                  continue;
                }
   
                /* Store DL freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_dl = (uint16) ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
   
                WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_dl);
   
                /* Calculate the UL Freq */
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = FALSE;
   
                if (ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
                  uarfcn_ULPresent)
                {
                  
   
                  if (ecno_cell_list_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                  {
                    MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                      frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                    idx++;
                    continue;
                  }
   
                  /* Store the UL Freq */
                  l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                    uarfcn_ul_incl = TRUE;
   
                  l1_inter_freq_sib_ptr->add_cell[cell_count].
                    freq_info.uarfcn_ul = (uint16)
                    ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
                }
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
#endif
            else
            {
              if (cell_count == 0)
              {
                WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
                idx++;
                continue;
              }
              else
              {
                /* This implies that there is already one DL and UL freq present. Copy from there */
                WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                             sizeof(l1_freq_info_struct_type),
                             &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                             sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
                mfbi_index--;
#endif
              }
            }
   
            /* Downlink Freq Present */
   
            /* First store cell info so that psc of this cell can be extracted */
            if (rrcmeas_fill_ecn0_cell_info(&ecno_cell_list_ptr->elem[idx].
              cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info,((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");
              idx++;
              continue;
            }

            /* Check if inter-freq cell ID is present */
            if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,
              rrc_NewInterFreqCellSI_ECN0,interFreqCellID))
            {
              cell_position = ecno_cell_list_ptr->elem[idx].interFreqCellID;
            } /* End of if of intra-freq cell id present */
            else
            {
              cell_position = AVAILABLE_CELL_POSITION;
            } /* End of else of inter-freq cell id not present */
   
   
   
   
            if ( rrcifreq_update_inter_freq_cell_list(
              &cell_position,
              &l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code,
              &l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl) == SUCCESS)
            {
              /* PSC sucessfully stored in the cell info list variable */
              /* Now store psc in the sib11 in add cell for layer1 to use psc */
              l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                cell_info.pri_scr_code;
   
              /* set the measure in PCH IDLE and FACH flag to TRUE */
              l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
              if(sib18_present)
              {
                rrcmeas_process_sib18_filtering(
                   (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                   (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                   &tmp_plmn_identity,
                   cell_count,
                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                   (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
              }
              else
              {
                /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
                MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib_ptr->add_cell[cell_count].psc,0,0);
              }
              /* Increment cell count as add cell successfully stored */
              cell_count ++;
            }  /* End of if of rrcmeas_update_inter_freq_cell_list */
   
            /* Get the next cell */
            idx++;
          } /* End of while */
          /* Set number of added cells */
          l1_inter_freq_sib_ptr->num_cell_add = cell_count;
   
          if (l1_inter_freq_sib_ptr->num_cell_add == 0)
          {
            MSG_LOW("IFreq:No new cell. SIB req not sent",0,0,0);
            if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
            {
              MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
              return FAILURE;
            }
            return SUCCESS;
          }
   
        } /* Inter freq cell info list is present */
        else
        {
          
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
            return FAILURE;
          }
          return SUCCESS;
        }
   
      } /* End of else of EcNo chosen */
      prev_cell_count = l1_inter_freq_sib_ptr->num_cell_add;
      if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
      {
        MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
        l1_inter_freq_sib_ptr->num_cell_add = prev_cell_count;
      }
    } /* Else of t is set to 'not hcs' */
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
  } /* End of sib11_ptr is valid */

  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIFREQ_UPDATE_INTER_FREQ_CELL_LIST

DESCRIPTION

  Updates the inter freq cell list with new values of inter freq
  cell id. This is done on receving SIB 11 system info. The calling fn gives
  position_no_ptr indicating where to store the psc. If it
  points to AVAILABLE_CELL_POSITION, then store psc in the first
  available position in inter_freq_cell_list and and mark that
  position as occupied. If position_no_ptr points to some other
  value then store psc at the position given by that value

DEPENDENCIES

  None

RETURN VALUE

  FAILURE if inter freq cell list cannot be updated due to incorrect
  value of the inter freq cell id
  SUCCESS if inter freq cell list can be successfully updated

SIDE EFFECTS

  None
===========================================================================*/

uecomdef_status_e_type rrcifreq_update_inter_freq_cell_list
(
 uint32* position_no_ptr,
 uint16* psc_ptr,
 uint16* dl_freq_ptr
)
{
  uint16 count = 0;

  /* If UTRAN tells UE to update cell in a position in which there is
  * already a cell, then this variable stores the psc */

  /* First check if one has to add this in the first available position */
  if (*position_no_ptr == AVAILABLE_CELL_POSITION)
  {
    for (count = 0; count < L1_MAX_CELL_MEAS; count++)
    {
      if (inter_freq_cell_list[count].cell_position == VACANT)
      {
        /* Store this cell in the first available vacant position */
        inter_freq_cell_list[count].cell_position = OCCUPIED;
        inter_freq_cell_list[count].psc = *psc_ptr;

        inter_freq_cell_list[count].dl_freq = *dl_freq_ptr;

        MSG_HIGH("IFreq:Adding cell %d,psc=%d,dfreq=%d",count,*psc_ptr,*dl_freq_ptr);

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
    * If yes, then this cell has to be removed and L1
    * has to be informed about it. So store its psc
      */

      MSG_HIGH("IFreq:Adding cell %d,psc=%d,dfreq=%d",count,*psc_ptr,*dl_freq_ptr);

      /* To add support for SIB12. Store removed freq and scr code */
      /* Store the new psc at this position */
      inter_freq_cell_list[count].psc = *psc_ptr;

      inter_freq_cell_list[count].dl_freq = *dl_freq_ptr;

      /* Mark the position as occupied */
      inter_freq_cell_list[count].cell_position = OCCUPIED;
      return SUCCESS;
    }
  }
  WRRC_MSG1_ERROR("IFreq:Inter-freq cell ID = %d not found", *position_no_ptr);
  return FAILURE;

}

/*===========================================================================

FUNCTION   RRCIFREQ_INITIALIZE_CELL_LIST

DESCRIPTION

    This function initializes the inter freq cell list


DEPENDENCIES

    None

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrcifreq_initialize_cell_list
(
 void
)
{
  uint8 cnt = 0;
  for (cnt = 0; cnt <L1_MAX_CELL_MEAS; cnt++ )
  {
    inter_freq_cell_list[cnt].psc = INVALID_PSC;
    inter_freq_cell_list[cnt].cell_position = VACANT;
    inter_freq_cell_list[cnt].dl_freq = INVALID_INTER_FREQ;
    inter_freq_cell_list_tmp[cnt].psc = INVALID_PSC;
    inter_freq_cell_list_tmp[cnt].cell_position = VACANT;
    inter_freq_cell_list_tmp[cnt].dl_freq = INVALID_INTER_FREQ;
  }
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
  #error code not present
#endif
}


/*===========================================================================

FUNCTION  RRCIFREQ_FILL_INTER_FREQ_INFO

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_fill_inter_freq_info
(
 rrc_InterFrequencyMeasurement* inter_f_meas_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
)
{
  inter_freq_removed_cell_struct_type cascaded_rem_cell[L1_MAX_CELL_MEAS];

  uint32 inter_freq_cell_id = 0;

  uint16 dl_freq = 0;

  uint16 psc = 0;
  uint16 psc_to_measure = INVALID_PSC;
  uint16 freq_to_measure = INVALID_INTER_FREQ;
  uint8 meas_count = 0;
  uint32 cell_id = 0;

  uint32 cell_position = 0;
  uint32 idx=0;
  uint8 cell_count = 0, removed_cell_count = 0, psc_removed_count = 0;

  rrc_RemovedInterFreqCellList_removeSomeInterFreqCells * local_rm_inter_freq_cell_ptr = NULL;

  struct rrc_NewInterFreqCellList * local_network_new_cell_list_ptr = NULL;


struct rrc_CellsForInterFreqMeasList *cellsForInterFreqMeasList_ptr = NULL;
  boolean valid_cell_id_found = FALSE;


  /* Declare a local var to store the inter freq object */

  l1_inter_freq_meas_ctrl_struct_type* l1_inter_freq_meas_ptr = NULL;

  l1_inter_freq_meas_ptr = &l1_meas_ptr->meas_object.u.inter_f;

  /* Copy Inter Freq CELL_INFO_LIST to a local var. If failure in the processing of
  CELL_INFO_LIST, copy  inter_freq_cell_list_tmp back to inter_freq_cell_list*/
  for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
  {
    WCDMA_MEMCPY(&inter_freq_cell_list_tmp[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type),
                 &inter_freq_cell_list[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type));

    cascaded_rem_cell[cell_count].freq = INVALID_INTER_FREQ;
    cascaded_rem_cell[cell_count].psc = INVALID_PSC;
  }

  if (RRC_MSG_COMMON_BITMASK_IE(inter_f_meas_ptr->interFreqCellInfoList,
    rrc_InterFreqCellInfoList,removedInterFreqCellList))
  {
    WRRC_MSG1_HIGH("IFreq:Remove:Delete [1: ALL  2: Some  3: No]:- %d cells",inter_f_meas_ptr->interFreqCellInfoList.removedInterFreqCellList.t);
    switch (inter_f_meas_ptr->interFreqCellInfoList.removedInterFreqCellList.t)
    {
    case T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells:
      
      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv = 0xff;


      /* Update Cell_info_list variable */
      for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
      {
        inter_freq_cell_list[cell_count].cell_position = VACANT;
        inter_freq_cell_list[cell_count].psc = INVALID_PSC;
        inter_freq_cell_list[cell_count].psc = INVALID_INTER_FREQ;
      }

      break;

    case T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells:

      if ((inter_f_meas_ptr->interFreqCellInfoList.removedInterFreqCellList.u.removeSomeInterFreqCells == NULL )
        ||(inter_f_meas_ptr->interFreqCellInfoList.removedInterFreqCellList.u.removeSomeInterFreqCells->n == 0 ))
      {
        break;
      }
      
      /* Store head of Link list */
      local_rm_inter_freq_cell_ptr =  inter_f_meas_ptr->interFreqCellInfoList.
        removedInterFreqCellList.u.removeSomeInterFreqCells;
      
      do
      {
        inter_freq_cell_id = local_rm_inter_freq_cell_ptr->elem[idx];

        /* Find the psc, dl freq as per given cell id from CELL_INFO_LIST */
        if ( rrcifreq_find_psc_freq(
          inter_freq_cell_id, &psc, &dl_freq) == PSC_ABSENT)
        {
          WRRC_MSG1_ERROR("IFreq:Rem:Incorrect cell id %d",inter_freq_cell_id);
          idx++;
          continue;
        }
        else
        {
          /* First remove cell from inter freq cell list */
          rrcifreq_remove_cellid(inter_freq_cell_id);

          /* Here one needs to put psc, dl freq so that L1 can remove it */
          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc = psc;

          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].freq = dl_freq;

          removed_cell_count++; /* Increment count */
        }

        /*  Get next element in the list */

        idx++;

      } /* Continue if while cond satisfied */

      while ( (local_rm_inter_freq_cell_ptr->n > idx) &&
        (removed_cell_count < L1_MAX_CELL_MEAS) );


      /* Now store the number of actual cells sent by UTRAN */

      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv =  removed_cell_count;
      WRRC_MSG1_HIGH("IFreq:Removed cell cnt = %d cells", removed_cell_count);

      break;

    case T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells:
      /* Already set removed cells to 0 */
      
      break;

    default:
      
      break;
    } /* End of switch */

  } /* End of if of removed inter freq cells present */

  /* Reinitialize these variables */
  cell_count = 0;
  psc = 0;

  /* Check if new cells present */
  if (RRC_MSG_COMMON_BITMASK_IE(inter_f_meas_ptr->interFreqCellInfoList,
    rrc_InterFreqCellInfoList,newInterFreqCellList))
  {
    

    /* Do not check for cell order. If UTRAN messes up, its their problem */

    /* Save the Head pointer of linked List */
    local_network_new_cell_list_ptr = &inter_f_meas_ptr->interFreqCellInfoList.newInterFreqCellList;
    idx =0;
    while ( ( local_network_new_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {

      if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.t == T_rrc_CellInfo_modeSpecificInfo_fdd)
      {
        if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.u.fdd->readSFN_Indicator ==TRUE)
        {
          return SFN_FAIL;
        }
      }
    /* Pointer to next cell is Non-NULL and cell count
      * hasn't exceeded the max. L1 can handle */

      if ((local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.t ==
        T_rrc_CellInfo_modeSpecificInfo_tdd) ||
        (!(local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.u.fdd->m.primaryCPICH_InfoPresent)))
      {
        MSG_LOW("IFreq:Ignore TDD cell",0,0,0);

        idx++;
        continue;

      }

      /* Fill in Cell Info */
      if (rrcmeas_fill_mcm_cell_info(
        &local_network_new_cell_list_ptr->elem[idx].cellInfo,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info) == FAILURE)
      {
        MSG_LOW("IFreq:Ignoring Cell",0,0,0);
        idx++;
        continue;
      }

      /* Check if cell id present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
            rrc_NewInterFreqCell,interFreqCellID))
      {
        cell_position = local_network_new_cell_list_ptr->elem[idx].interFreqCellID;

        WRRC_MSG1_MED("IFreq:Add:Cell at pos %d to be added",cell_position);

      } /*  End of if of inter-freq cell id present */
      else
      {
        
        cell_position = AVAILABLE_CELL_POSITION;
      }

      /* Check if freq present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,rrc_NewInterFreqCell,frequencyInfo))
      {
        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
          T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
        {
          MSG_LOW("IFreq:Ignore Cell. TDD cell",0,0,0);
          idx++;
          continue;
        }

        if (validate_frequency_info(&local_network_new_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
        {
          WRRC_MSG1_HIGH("Cell with DL freq %d out of ",
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
          return RRCMEAS_UNSUPPORTED;
        }


        if (local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
        {
          MSG_LOW("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",dl_freq,0,0);
          idx++;
          continue;
        }

        /* Store DL freq */
        dl_freq = l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          freq_info.uarfcn_dl = (uint16) local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

        /* Calculate the UL Freq */
        /* Store the UL Freq */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
          uarfcn_ul_incl = FALSE;

        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
          uarfcn_ULPresent)
        {
          

          if (local_network_new_cell_list_ptr->elem[idx].
            frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
          {
            WRRC_MSG1_HIGH("IFreq:Invalid UL ARFCN %d.Ignore Inter Freq Cell",local_network_new_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL);
            idx++;
            continue;
          }
          /* Store the UL Freq */
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
            uarfcn_ul_incl = TRUE;

          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
            freq_info.uarfcn_ul = (uint16)
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
        } /* End of up freq present */
      } /*  End of if of inter-freq freq present */
      else
      {
        if (cell_count == 0)
        {
          MSG_LOW("IFreq:Ignore Cell. DL Freq absent",0,0,0);
          idx++;
          continue;
        }
        else
        {
          /* This implies that there is already one DL and UL freq present. Copy from there */
          WCDMA_MEMCPY(&l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info,
                       sizeof(l1_freq_info_struct_type),
                       &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count-1].freq_info,
                       sizeof(l1_freq_info_struct_type));
        }
      }

      if ( rrcifreq_update_inter_freq_cell_list_mcm(
        &cell_position,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info.pri_scr_code, &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.uarfcn_dl,
        &psc, &dl_freq) == RRCMEAS_SUCCESS)
      {
        /* PSC sucessfully stored in the cell info list variable */
        /* Now store psc in the mcm in add cell for layer1 to use psc */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].psc =
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          cell_info.pri_scr_code;

        /* This flag is always true for MCM */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

        if ((psc != INVALID_PSC) && (dl_freq != INVALID_INTER_FREQ) && (psc_removed_count < L1_MAX_CELL_MEAS))
        {
          /* UTRAN wants us to delete this psc/freq. Result of CASCADING effect */
          cascaded_rem_cell[psc_removed_count].psc = psc;

          cascaded_rem_cell[psc_removed_count++].freq = dl_freq;
        }

        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }  /* End of if of rrcmeas_update_inter_freq_cell_list */

      /* Read next element */
     idx++;

    } /* End of while newIntraFreqCellList != NULL */

    /* Initialize no of added cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_add = cell_count;

    for (cell_count = 0; cell_count < psc_removed_count; cell_count++)
    {
      if (removed_cell_count >= L1_MAX_CELL_MEAS)
      {
        /* Preventive check */
        WRRC_MSG0_HIGH("IFREQ:Unable to delete all cells");
        break;
      }
      /* Add this removed cell to the already built removed cell list for L1 */
      WRRC_MSG1_MED("IFREQ:Cascade:L1 removed cell list upd %d psc",
        cascaded_rem_cell[cell_count].psc);
      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc =
        cascaded_rem_cell[cell_count].psc;

      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count++].freq =
        cascaded_rem_cell[cell_count].freq;

    }
    /* Now update the number of removed cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_rmv += psc_removed_count;



  } /* End of if for new inter freq cells present */

  /* Initialize no fo cells to 0 */
  l1_inter_freq_meas_ptr->cell_list.num_cell_meas = 0;
  cell_count = 0;
  l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = FALSE;
  /* Check if cells for meas present */
  if (RRC_MSG_COMMON_BITMASK_IE(inter_f_meas_ptr->interFreqCellInfoList,
       rrc_InterFreqCellInfoList,cellsForInterFreqMeasList))
  {
    MSG_LOW("Cells for meas present",0,0,0);
    l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = TRUE;
    idx=0;
    cellsForInterFreqMeasList_ptr = &inter_f_meas_ptr->interFreqCellInfoList.cellsForInterFreqMeasList;
    meas_count = 0;
    while (cellsForInterFreqMeasList_ptr->n > idx)
    {

      cell_id = cellsForInterFreqMeasList_ptr->elem[idx];
      /* Check if the cell id is present in intra_freq_cell_list VARIABLE */
      if ( rrcifreq_find_psc_freq(cell_id, &psc_to_measure, &freq_to_measure) == PSC_ABSENT)
      {
       WRRC_MSG1_ERROR("Invalid cell to measure with id = %d. Ignoring cell id",cell_id);

      } /* End of if for psc not found */
      else
      {
        valid_cell_id_found = TRUE;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count].psc = psc_to_measure;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count++].dl_freq = freq_to_measure;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
      /*None of the cells configured are valid*/
        MSG_LOW("None of the cells are valid .. REL99:Retain previous cell list before MCM rcved",0,0,0);

        WCDMA_MEMCPY(inter_freq_cell_list, 
                     sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS,
                     inter_freq_cell_list_tmp,
                     sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS);

        return RRCMEAS_UNSUPPORTED;
    }
    l1_inter_freq_meas_ptr->cell_list.num_cell_meas =  meas_count;

  } /* End of if of rrc_cellsForInterFreqMeasList) */

  return RRCMEAS_SUCCESS;
}


/*===========================================================================

FUNCTION   RRCIFREQ_FIND_PSC_FREQ

DESCRIPTION

 Finds the primary scrambling code of the intra freq cell id. The primary
 scrambling code is returned back in psc_ptr provided. If not found,
 then psc_ptr contains INVALID_PSC

DEPENDENCIES

 None

RETURN VALUE

 PSC_ABSENT if psc absent
 PSC_PRESENT if psc present

SIDE EFFECTS

 None
===========================================================================*/

psc_e_type rrcifreq_find_psc_freq
(
 uint32 inter_freq_cell_id,
 uint16* psc_ptr,
 uint16* dl_freq_ptr
)
{
  /* Found the position for which psc needs to be returned */
  if (inter_freq_cell_list[inter_freq_cell_id].cell_position == OCCUPIED)
  {
    /* Found the good psc */
    *psc_ptr = inter_freq_cell_list[inter_freq_cell_id].psc;

    *dl_freq_ptr = inter_freq_cell_list[inter_freq_cell_id].dl_freq;

    return PSC_PRESENT;
  }
  else
  {
    /* PSC at this position not valid */

    *psc_ptr = INVALID_PSC;
    *dl_freq_ptr = INVALID_INTER_FREQ;

    return PSC_ABSENT;
  }
}


/*===========================================================================

FUNCTION   RRCIFREQ_REMOVE_CELLID

DESCRIPTION

 Removes the primary scrambling code at position given by intra freq cell id
 in inter_freq_cell_list. The psc at the removed position is given back so that
 L1 can be informed of removing the particular cell.

DEPENDENCIES

 None

RETURN VALUE

 PSC_ABSENT if psc absent
 PSC_PRESENT if psc present

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_remove_cellid
(
 uint32 inter_freq_cell_id
)
{
  inter_freq_cell_list[inter_freq_cell_id].cell_position = VACANT;

  inter_freq_cell_list[inter_freq_cell_id].psc =  INVALID_PSC;

  inter_freq_cell_list[inter_freq_cell_id].dl_freq = INVALID_INTER_FREQ;

}


/*===========================================================================

FUNCTION   RRCIFREQ_UPDATE_INTER_FREQ_CELL_LIST_MCM

DESCRIPTION

 Invoked from the cell list processing fn specified by MCM
 The calling fn specifies the position, scrambling code
 and the downlink freq. The fn checks if there is
 already a valid scr code and dl freq at that position.
 If yes, then it stores them in psc_to_remove_ptr and
 dl_freq_to_remove_ptr and updates the inter freq cell list
 with the new scr code and dl freq and does not change
 the position. If there was no cell at that position, then
 it stores the dl freq and scr code and markls that position
 in the inter freq cell list as occupied.

DEPENDENCIES

 None.

RETURN VALUE

 FAILURE: If unable to update the inter freq cell list.
 SUCCESS: If able to update the inter freq cell list.

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_update_inter_freq_cell_list_mcm
(
 uint32* position_no_ptr,
 uint16* psc_ptr,
 uint16* dl_freq_ptr,
 uint16* psc_to_remove_ptr,
 uint16* dl_freq_to_remove_ptr
)
{
  uint16 count = 0;

  /* First check if one has to add this in the first available position */
  if (*position_no_ptr == AVAILABLE_CELL_POSITION)
  {
    for (count = 0; count < MAX_NO_OF_CELLS; count++)
    {
      if (inter_freq_cell_list[count].cell_position == VACANT)
      {
        /* Store this cell in this first available vacant position */
        inter_freq_cell_list[count].cell_position = OCCUPIED;
        inter_freq_cell_list[count].psc = *psc_ptr;
        inter_freq_cell_list[count].dl_freq = *dl_freq_ptr;


        MSG_HIGH("IFreq:Add Cell at pos=%d,psc=%d,freq=%d",count,*psc_ptr,*dl_freq_ptr);


        /* Set psc_to_remove_ptr to INVALID_PSC so that calling fn
        * does not need to update this */
        *psc_to_remove_ptr = INVALID_PSC;

        *dl_freq_to_remove_ptr = INVALID_INTER_FREQ;
        return RRCMEAS_SUCCESS;
      }
    } /* End of for */
  } /* End of if of  position_no_ptr == AVAILABLE_CELL_POSITION */

  /* Now check which position the cells needs to be updated */
  for (count = 0; count < MAX_NO_OF_CELLS; count++)
  {
  /*  Check if there is already a cell in this position
  *  If yes, then this cell has to be removed and L1
    *  has to be informed about it. So store its psc  */

    if ( (count == *position_no_ptr) &&
      (inter_freq_cell_list[count].cell_position == OCCUPIED) )
    {

      MSG_HIGH("IFreq:Add:PSC %d at pos=%d removed[Cascading] by %d",
        inter_freq_cell_list[count].psc,count,*psc_ptr);


      /* Store the removed psc */
      *psc_to_remove_ptr = inter_freq_cell_list[count].psc;

      *dl_freq_to_remove_ptr = inter_freq_cell_list[count].dl_freq;

      /* Store the new psc, dl freq at this position */
      inter_freq_cell_list[count].psc = *psc_ptr;

      inter_freq_cell_list[count].dl_freq = *dl_freq_ptr;

      return RRCMEAS_SUCCESS;
    }
    else if ( (count == *position_no_ptr) &&
      (inter_freq_cell_list[count].cell_position == VACANT) )
    {

      WRRC_MSG2_HIGH("IFreq:Adding cell at pos %d with psc %d",
        count,*psc_ptr);


      /* Store the new psc at this position */
      inter_freq_cell_list[count].psc = *psc_ptr;

      inter_freq_cell_list[count].dl_freq = *dl_freq_ptr;

      /* Mark the position as occupied. NOT REQD */
      inter_freq_cell_list[count].cell_position = OCCUPIED;

      /* No psc to be removes as this place is vacant */
      *psc_to_remove_ptr = INVALID_PSC;

      *dl_freq_to_remove_ptr = INVALID_INTER_FREQ;

      return RRCMEAS_SUCCESS;

    } /* End of else if */
  } /* End of for */
  WRRC_MSG1_ERROR("Inter-freq cell ID = %d not valid", *position_no_ptr);
  return RRCMEAS_FAIL;

}


/*===========================================================================

FUNCTION   RRCIFREQ_SEND_L1_CMD

DESCRIPTION

 This function sends the L1 command from SIB's only

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_send_l1_cmd
(
 l1_meas_ctrl_enum_type*  meas_choice_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
)
{

   l1_req_cmd * rrc_meas_cmd_l1_ptr = NULL;
  /* Ptr which carries info about the command */
  rrc_cmd_type        *rrc_meas_out_cmd_ptr = NULL;;
  uint8 i = 0;


if((((rrcmeas_current_substate == RRCMEAS_INITIAL)
   || (rrcmeas_current_substate == RRCMEAS_IDLE)
   ||(rrcmeas_current_substate == RRCMEAS_PCH))
   &&(rrc_disable_meas_nv & RRC_DISABLE_WTOW_IDLE_MEAS))

   ||((rrcmeas_current_substate == RRCMEAS_CELL_FACH) 
       &&(rrc_disable_meas_nv & RRC_DISABLE_WTOW_FACH_MEAS))
  
   || ((rrcmeas_current_substate == RRCMEAS_CELL_DCH)
       && (rrc_disable_meas_nv & RRC_DISABLE_WTOW_CM_MEAS))
  )
{
   MSG_LOW("Skip IRAT/IFREQ meas  Disable IRAT/IFREQ NV %d  in meas state  %d",rrc_disable_meas_nv,rrcmeas_current_substate,0);
   rrcifreq_initialize_cell_list();
   return;
}
  /* Now allocate memory */
  rrc_meas_out_cmd_ptr = rrc_get_int_cmd_buf();

  RRC_GET_POINTER_L1_REQ_VALUE(rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l1_req,rrc_meas_cmd_l1_ptr);
  
  /* Initialize the LLC command header first */

  rrc_meas_out_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_MCMR;

  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

  /* Now fill the L1 cmd header */
  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_MEASUREMENT_REQ;

  /* act_time_type */
  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type =
    L1_ACTIVATION_TIME_NONE;
  
  /* Now allocate memory to the pointer inside the command */
  rrc_meas_cmd_l1_ptr->meas.meas_ptr =
       (l1_meas_parm_type *)rrc_malloc_interlayer_cmd(sizeof(l1_meas_parm_type));

  /* Now fill the values inside the ptr */
  if ( *meas_choice_ptr == L1_INTER_FREQ_SIB_PARMS)
  {
    rrc_meas_cmd_l1_ptr->meas.meas_ptr->u.inter_freq_sib =
    *l1_inter_freq_sib_ptr;
    WRRC_MSG1_MED(" %d-Interfreq cells will be sent to L1",l1_inter_freq_sib_ptr->num_cell_add);
    for(i=0;i<l1_inter_freq_sib_ptr->num_cell_add; i++)
    {
      WRRC_MSG3_MED("    Freq:%d  PSC:%d, measure_in_idle_pch_fach=%d",l1_inter_freq_sib_ptr->add_cell[i].freq_info.uarfcn_dl,l1_inter_freq_sib_ptr->add_cell[i].psc,l1_inter_freq_sib_ptr->add_cell[i].measure_in_idle_pch_fach);
    }
    rrc_log_meas_info(NULL,NULL,NULL,*meas_choice_ptr,l1_inter_freq_sib_ptr);
  }

  /* Fill in param t */
  rrc_meas_cmd_l1_ptr->meas.meas_ptr->parm_type =
    *meas_choice_ptr;
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
  #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/
  /* Now send command to L1 to start Inter Freq Measurements */
  WRRC_MSG0_HIGH("IFreq:Sending SIB cmd to L1");

  /* Send the Internal RRC Command - LLC should directly handle it */
  rrc_put_int_cmd( rrc_meas_out_cmd_ptr );
}


/*===========================================================================

FUNCTION   RRCIFREQ_FILL_RPT_QUAN

DESCRIPTION

 This function fills the reporting quantity in L1 structure from the ASN1 format

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_fill_rpt_quan
(
 rrc_InterFreqReportingQuantity* mcm_rpt_quan_ptr,
 l1_inter_freq_rpt_quan_struct_type* l1_rpt_quan_ptr)
{
  l1_rpt_quan_ptr->rpt_rssi = FALSE;
  l1_rpt_quan_ptr->freq_qual_est = FALSE;
  l1_rpt_quan_ptr->other_rpt_quan.cell_id_rpt = FALSE;
  l1_rpt_quan_ptr->other_rpt_quan.cfn_sfn_delta_rpt = FALSE;
  l1_rpt_quan_ptr->other_rpt_quan.ec_no_rpt = FALSE;
  l1_rpt_quan_ptr->other_rpt_quan.pathloss_rpt = FALSE;
  l1_rpt_quan_ptr->other_rpt_quan.rscp_rpt = FALSE;

  if (mcm_rpt_quan_ptr->utra_Carrier_RSSI == TRUE)
    l1_rpt_quan_ptr->rpt_rssi = TRUE;

  if (mcm_rpt_quan_ptr->frequencyQualityEstimate == TRUE)
    l1_rpt_quan_ptr->freq_qual_est = TRUE;

  if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.cellIdentity_reportingIndicator == TRUE)
    l1_rpt_quan_ptr->other_rpt_quan.cell_id_rpt = TRUE;

  l1_rpt_quan_ptr->other_rpt_quan.sfn_rpt =
    RRCMEAS_RET_INTRA_FREQ_SFN_DELTA(mcm_rpt_quan_ptr->nonFreqRelatedQuantities.dummy);

  if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.cellSynchronisationInfoReportingIndicator == TRUE)
    l1_rpt_quan_ptr->other_rpt_quan.cfn_sfn_delta_rpt = TRUE;


  if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.modeSpecificInfo.t ==
    T_rrc_CellReportingQuantities_modeSpecificInfo_fdd)
  {
    if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.modeSpecificInfo.u.fdd->cpich_Ec_N0_reportingIndicator == TRUE)
      l1_rpt_quan_ptr->other_rpt_quan.ec_no_rpt = TRUE;

    if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.modeSpecificInfo.u.fdd->cpich_RSCP_reportingIndicator == TRUE)
      l1_rpt_quan_ptr->other_rpt_quan.rscp_rpt = TRUE;

    if (mcm_rpt_quan_ptr->nonFreqRelatedQuantities.modeSpecificInfo.u.fdd->pathloss_reportingIndicator == TRUE)
      l1_rpt_quan_ptr->other_rpt_quan.pathloss_rpt = TRUE;
  }
}


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MEAS_QUANTITY

DESCRIPTION

 This function processes the measurement quantity and converst it into L1 format.

DEPENDENCIES

 None

RETURN VALUE

 Success if meas quantity is valid
 Failure if meas quantity is invalid

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_status_e_type rrcifreq_process_meas_quantity
(
 rrc_InterFreqMeasQuantity* meas_quan_ptr,
 l1_inter_freq_meas_quan_struct_type* l1_meas_quan_ptr
)
{
  rrcmeas_status_e_type meas_status = RRCMEAS_SUCCESS;

  l1_meas_quan_ptr->interf_meas_quan.filter_coef = 0;


  /* Get Measurement Quantity elements */
  if (meas_quan_ptr->reportingCriteria.t == T_rrc_InterFreqMeasQuantity_reportingCriteria_interFreqReportingCriteria)
  {
    /* Set criteria to event triggered */
    l1_meas_quan_ptr->rpt_choice = L1_INTER_FREQ;


    /* TBD Check filter coefficient value if not present which is the current scenario */
    l1_meas_quan_ptr->interf_meas_quan.filter_coef = 0;

    if(T_rrc_InterFreqMeasQuantity_modeSpecificInfo_fdd ==
          meas_quan_ptr->reportingCriteria.u.interFreqReportingCriteria->modeSpecificInfo.t)
    {
      if (meas_quan_ptr->reportingCriteria.u.interFreqReportingCriteria->modeSpecificInfo.u.fdd->
        freqQualityEstimateQuantity_FDD == rrc_FreqQualityEstimateQuantity_FDD_cpich_Ec_N0)
      {
        l1_meas_quan_ptr->interf_meas_quan.quan_type = L1_EC_NO;
      }
      else /* UTRAN can specify RSCP or EC/Io only */
      {
        l1_meas_quan_ptr->interf_meas_quan.quan_type = L1_RSCP;
      } 

      /* Store filter coefficient */
      l1_meas_quan_ptr->interf_meas_quan.filter_coef = (uint16)
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(
        meas_quan_ptr->reportingCriteria.u.interFreqReportingCriteria->filterCoefficient);
    }
    else
    {
      MSG_LOW("IFreq:Mode specific choice is TDD",0,0,0);
      meas_status = RRCMEAS_FAIL;
    }    
  }
  else
  {
    /* Set criteria to event triggered */
    l1_meas_quan_ptr->rpt_choice = L1_INTRA_FREQ;

    
      /* Store filter coefficient */
      l1_meas_quan_ptr->interf_meas_quan.filter_coef = (uint16)
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(
        meas_quan_ptr->reportingCriteria.u.intraFreqReportingCriteria->
        intraFreqMeasQuantity.filterCoefficient);
    

    if (meas_quan_ptr->reportingCriteria.u.intraFreqReportingCriteria->intraFreqMeasQuantity.modeSpecificInfo.t
      == T_rrc_IntraFreqMeasQuantity_modeSpecificInfo_fdd)
    {
      l1_meas_quan_ptr->interf_meas_quan.quan_type = RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(meas_quan_ptr->reportingCriteria.u.
        intraFreqReportingCriteria->intraFreqMeasQuantity.modeSpecificInfo.u.fdd->intraFreqMeasQuantity_FDD);

      if (l1_meas_quan_ptr->interf_meas_quan.quan_type == L1_PATHLOSS)
      {
        MSG_LOW("IFreq:PathLoss not supported as MeasQuan",0,0,0);
        meas_status = RRCMEAS_FAIL;
      }
    }
    else
    {
      MSG_LOW("IFreq:Mode specific t is TDD",0,0,0);
      meas_status = RRCMEAS_FAIL;
    }

  }
  return meas_status;
}


/*===========================================================================

FUNCTION   RRCIFREQ_CONFIG_INTRA_FREQ_DB

DESCRIPTION

  Sets Intra Freq Reporting Criteria in RRC Measurement Database
  from Measurement Control Message after the validation of
  the message has been completed

DEPENDENCIES

  None

RETURN VALUE

  Boolean

SIDE EFFECTS

  None
===========================================================================*/

boolean rrcifreq_config_inter_freq_db
(
 l1_meas_ctrl_parm_struct_type* l1_mcm_ptr,
 rpt_crit_enum_type* rpt_crit_ptr,
 meas_validity_enum_type m_validity
)
{
  uint16 count = 0;
  /* First check if the meas id already exists */

  /* SETUP existing & MODIFY scenario */
  for ( count = 0; count < MAX_INTER_FREQ_MEAS; count++ )
  {
    if (inter_freq_meas_id_list[count].meas_id == l1_mcm_ptr->meas_id)
    {
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Meas Id = %d found in RRC db, Before commiting Id %d, Val %d, Owner %d",
            l1_mcm_ptr->meas_id,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      /* If a setup, always overwrite validity */
      if (l1_mcm_ptr->meas_cmd == L1_MEAS_SETUP)
      {
        /* Set Validity */
        inter_freq_meas_id_list[count].meas_validity = m_validity;
      }
      /* For MODIFY scenario, when validity not specified */
      else if ((l1_mcm_ptr->meas_cmd == L1_MEAS_MODIFY) &&
        (l1_mcm_ptr->meas_object_info_included == TRUE) &&
        (m_validity == NOT_PRESENT))
      {
        WRRC_MSG1_HIGH("IFREQ:Retain Current Validity %d", inter_freq_meas_id_list[count].meas_validity);
      }
      /* UTRAN specified Meas Validity with a Modify */
      else if ((l1_mcm_ptr->meas_cmd == L1_MEAS_MODIFY) &&
        (l1_mcm_ptr->meas_object_info_included == TRUE) &&
        (m_validity != NOT_PRESENT))
      {
        WRRC_MSG2_HIGH("IFREQ:Overwrite Current Validity %d with %d",
          inter_freq_meas_id_list[count].meas_validity,m_validity);

        inter_freq_meas_id_list[count].meas_validity = m_validity;
      }
      else /* To satisfy LINT */
      {
        /* Normal processing */
      }

      /* MCM in the owner of this Meas */
      inter_freq_meas_id_list[count].mcm_owned = TRUE;

      /* Set Report Criteria */
      inter_freq_meas_id_list[count].rpt_crit = *rpt_crit_ptr;

      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Meas Crit %d saved in RRC, After commiting Id %d, Val %d, Owner %d",
            inter_freq_meas_id_list[count].rpt_crit,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      return TRUE;
    }

  } /* End of for loop */
  WRRC_MSG1_MED("IFreq:New Meas Id = %d configured",l1_mcm_ptr->meas_id);

  /* SETUP not existing scenario */
  for ( count = 0; count < MAX_INTER_FREQ_MEAS; count++ )
  {
    /* If Meas Id not found, then it is a new Meas. Set it in first available position */
    if (inter_freq_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Setting Meas Id = %d in RRC db, Before commiting Id %d, Val %d, Owner %d",
            l1_mcm_ptr->meas_id,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      inter_freq_meas_id_list[count].meas_id = l1_mcm_ptr->meas_id;

      inter_freq_meas_id_list[count].meas_validity = m_validity;

      inter_freq_meas_id_list[count].mcm_owned = TRUE;

      /* Set Report Criteria */
      inter_freq_meas_id_list[count].rpt_crit = *rpt_crit_ptr;

      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Meas Crit %d saved in RRC, After commiting Id %d, Val %d, Owner %d",
            inter_freq_meas_id_list[count].rpt_crit,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      return TRUE;
    }

  } /* End of for loop */

  MSG_LOW("IFreq:RRC MeasDB for InterFreq FULL!!",0,0,0);
  return FALSE;

}


/*===========================================================================

FUNCTION  RRCIFREQ_FILL_HCS_INFO

DESCRIPTION

 This function fills the overall HCS info for an Inter Freq Cell.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill cell info correctly
 SUCCESS: If Cell Info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcifreq_fill_hcs_info
(
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
  ,rrc_SysInfoType12_v590ext_IEs_newInterFrequencyCellInfoList_v590ext * rel5_ext_ptr
)
{
  rrc_SysInfoType11 *sib11_ptr = NULL; /* Declared to store the value of SIB 11*/

  /* Local variables for storing value from SIB11 */
  rrc_InterFreqMeasurementSysInfo_HCS_RSCP* inter_freq_meas_rscp_ptr = NULL;

  rrc_InterFreqMeasurementSysInfo_HCS_ECN0* inter_freq_meas_ecno_ptr = NULL;

  /* Ptrs to go thr linked list */
  rrc_NewInterFreqCellSI_List_HCS_RSCP * rscp_cell_list_ptr = NULL;

  rrc_NewInterFreqCellSI_List_HCS_ECN0 * ecno_cell_list_ptr = NULL;
  uint32 idx=0;
  uint8 cell_count = 0;

  uint32 cell_position = 0;

  rrc_SysInfoType18 *sib18_ptr = NULL; /* Declared to store the value of SIB 18*/
  rrc_PLMNsOfInterFreqCellsList *tmp_rrc_PLMNsOfInterFreqCellsList_ptr = NULL;
  rrc_MultiplePLMNsOfInterFreqCellsList *tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = NULL;
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;
  boolean sib18_present = FALSE;
  uint16 prev_cell_count = 0;
  sib11_ptr = (rrc_SysInfoType11*)  rrc_sibdb_return_sib_for_srv_cell
    (rrc_SIB11);

  /* Initialize L1 params */
  l1_inter_freq_sib_ptr->num_cell_add = 0;
  l1_inter_freq_sib_ptr->num_cell_meas = 0;
  l1_inter_freq_sib_ptr->num_cell_rmv = 0;
  l1_inter_freq_sib_ptr->cells_for_meas_incl = FALSE;

  tmp_plmn_identity.mcc[0] =  tmp_plmn_identity.mcc[1] = tmp_plmn_identity.mcc[2] = 0;
  tmp_plmn_identity.num_mnc_digits = 0;
  tmp_plmn_identity.mnc[0] =  tmp_plmn_identity.mnc[1] = 0;

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
          plmnsOfInterFreqCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            connectedModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 Conn Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((rrc_sibdb_is_sib11bis_present_in_srv_cell()) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
        idleModePLMNIdentitiesSIB11bis)) &&
          (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
          plmnsOfInterFreqCellsList)) &&
          (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 idle Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.connectedModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Conn Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, connectedModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->connectedModePLMNIdentities, plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->connectedModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Conn Inter Freq",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Idle Inter Freq",0,0,0);
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
        (RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext,
        idleModePLMNIdentitiesSIB11bis)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis,
        plmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.sysInfoType18_v6b0ext.
            idleModePLMNIdentitiesSIB11bis.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11bis:SIB11 processing SIB18 Idle Inter Freq from bis extension ",0,0,0);
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr,rrc_SysInfoType18,v6b0NonCriticalExtensions)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions,v860NonCriticalExtensions))&&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext,idleModePLMNIdentities)) &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities,multipleplmnsOfInterFreqCellsList)) &&
        (sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr = &sib18_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType18_v860ext.idleModePLMNIdentities.multipleplmnsOfInterFreqCellsList;
        WRRC_MSG0_HIGH("SIB11 processing SIB18 Idle Multiple PLMNs Inter Freq");
        sib18_present = TRUE;
      }
      else if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib18_ptr, rrc_SysInfoType18, idleModePLMNIdentities))
        &&
        (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib18_ptr->idleModePLMNIdentities,plmnsOfInterFreqCellsList))
      &&
        (sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList.n != 0))
      {
        tmp_rrc_PLMNsOfInterFreqCellsList_ptr =& sib18_ptr->idleModePLMNIdentities.plmnsOfInterFreqCellsList;
        MSG_LOW("SIB11 processing SIB18 Idle Inter Freq ",0,0,0);
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
    MSG_LOW("UE in limited service ignoring SIB18",0,0,0);
  }


  /* Check if the value received is O.K. */

  if ( sib11_ptr == NULL )
  {
    MSG_LOW("IFreq:SIB 11 from SIB DB is NULL",0,0,0);

    return FAILURE;
  }
  else
  {
    /* Read SIB 11 parameters */

    if ( sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
      cellSelectQualityMeasure.t ==
      T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_1_cpich_RSCP )
    {

      /* The Cell Select Quality Measure is RSCP */
      if (!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_RSCP->m.interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys RSCP info is absent */
        /* No need to initialize inter_freq_meas_info_incl as it is already set to FALSE */

        MSG_LOW("IFreq:Meas Sys Info ABS",0,0,0);
        if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
        {
          MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
          return FAILURE;
        }
        return SUCCESS;
      }

      /* Stores RSCP sys info in the following var */
      inter_freq_meas_rscp_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_RSCP->interFreqMeasurementSysInfo;


      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_rscp_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_RSCP,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_HCS_RSCP,newInterFreqCellList)) &&
        (inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        


        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;

        /* Store the ptr in a local variable */
        rscp_cell_list_ptr = &inter_freq_meas_rscp_ptr->interFreqCellInfoSI_List.newInterFreqCellList;

        while ((rscp_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */

          /* Get the Freq Info */
          if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_RSCP,frequencyInfo))
          {
            if(rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("IFreq:Ignore Cell tdd",0,0,0);
              idx++;
              continue;
            }

            if (validate_frequency_info(&rscp_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
              WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
              idx++;
              /*Ignore all the cells till we find next freq*/
              while((rscp_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
              {
                 idx++;
              }

              continue;
            }

            /* Downlink Freq Present */
            if (rscp_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
            {
              MSG_LOW("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
              idx++;
              continue;
            }

            /* Store DL freq */
            l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl = (uint16) rscp_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

            WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl);

            /* Calculate the UL Freq */
            /* Store the UL Freq */
            l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
              uarfcn_ul_incl = FALSE;

            if (rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
                uarfcn_ULPresent)
            {
              

              if (rscp_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
              {
                MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",rscp_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                idx++;
                continue;
              }
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = TRUE;

              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_ul = (uint16)
                rscp_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
            }
          }
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
              idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
            }
          }

          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
            }
          }

          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_rscp_cell_info(&rscp_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(rscp_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_RSCP,interFreqCellID))
          {
            cell_position = rscp_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */



          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }
            else
            {
              /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
              MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib_ptr->add_cell[cell_count].psc,0,0);
            }
            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        }/* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;

        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
            return FAILURE;
          }
          return SUCCESS;
        }

      } /* Inter freq cell info list is present */
      else
      {
        
        if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
        {
          MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
          return FAILURE;
        }
        return SUCCESS;
      }
    } /* End of if of RSCP chosen */
    else /* rrc_hcs_used_cellSelectQualityMeasure_cpich_Ec_N0 */
    {
      if (!(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->m.interFreqMeasurementSysInfoPresent))
      {
        /* Optional param Inter freq meas sys ECN0 info is absent */
        WRRC_MSG0_HIGH("IFreq:Meas Sys info absent");
        if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
        {
          MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
          return FAILURE;
        }
        return SUCCESS;
      }

      /* Stores EcNo sys info in the following var */
      inter_freq_meas_ecno_ptr = &sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
        cellSelectQualityMeasure.u.cpich_Ec_N0->interFreqMeasurementSysInfo;


      /* Check inter-freq cell info present */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(inter_freq_meas_ecno_ptr,
        rrc_InterFreqMeasurementSysInfo_HCS_ECN0,interFreqCellInfoSI_List)) &&
        (RRC_MSG_COMMON_BITMASK_IE(inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List,
        rrc_InterFreqCellInfoSI_List_HCS_ECN0,newInterFreqCellList)) &&
        (inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList.n != 0))

      {
        

        /* Always ignore the number of removed cells in System Info 11
        as there is nothing to delete initially */

        /*  Process the new cell list */

        cell_count = 0;

        /* Store the ptr in a local variable */
        ecno_cell_list_ptr = &inter_freq_meas_ecno_ptr->interFreqCellInfoSI_List.newInterFreqCellList;
        idx=0;
        while ((ecno_cell_list_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max that is allowed by specs */

          /* Get the Freq Info */
          if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo))
          {
            if(ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
            T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("IFreq:Ignore Cell.TDD",0,0,0);
              idx++;
              continue;
            }

            if (validate_frequency_info(&ecno_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
              WRRC_MSG1_HIGH("Ignored cell with dl freq %d",
                ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
              idx++;
              /*Ignore all the cells till we find next freq*/
              while((ecno_cell_list_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
              {
                 idx++;
              }

              continue;
            }

            /* Downlink Freq Present */
            if (ecno_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
            {
              MSG_LOW("IFreq:Invalid DL ARFCN %dIgnore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
              idx++;
              continue;
            }

            /* Store DL freq */
            l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl = (uint16) ecno_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

            WRRC_MSG1_HIGH("IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
              freq_info.uarfcn_dl);

            /* Calculate the UL Freq */
            /* Store the UL Freq */
            l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
              uarfcn_ul_incl = FALSE;

            if (ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
            {
              
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = TRUE;

              if (ecno_cell_list_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
              {
                MSG_LOW("IFreq:Invalid UL ARFCN %d.Ignore Inter Freq Cell",ecno_cell_list_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
                idx++;
                continue;
              }

              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = TRUE;

              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_ul = (uint16)
                ecno_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
            }
          }
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
             idx++;
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
            }
          }

          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 24 * 2;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
              l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset2 =
                l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset2;
            }
          }
          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_ecn0_cell_info(&ecno_cell_list_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");
            idx++;
            continue;
          }

          /* Check if inter-freq cell ID is present */
          if (RRC_MSG_LIST_BITMASK_IE(ecno_cell_list_ptr,
            rrc_NewInterFreqCellSI_HCS_ECN0,interFreqCellID))
          {
            cell_position = ecno_cell_list_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */



          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;

            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            /* Increment cell count as add cell successfully stored */
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }
            else
            {
              /* Add code to set flag meas_pch_fach_idle to TRUE for the cell */
              MSG_LOW("psc %d measured in IDLE,FACH,PCH,DCH",l1_inter_freq_sib_ptr->add_cell[cell_count].psc,0,0);
            }
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
        } /* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;

        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          MSG_LOW("IFreq:No new cell. SIB req not sent",0,0,0);
          if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
          {
            MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
            return FAILURE;
          }
          return SUCCESS;
        }

      } /* Inter freq cell info list is present */
      else
      {
        
        if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
        {
          MSG_LOW("SIB11bis: Reading inter frequency failure",0,0,0);
          return FAILURE;
        }
        return SUCCESS;
      }

    } /* End of else of EcNo chosen */

  } /* End of sib11_ptr is valid */
  
  prev_cell_count = l1_inter_freq_sib_ptr->num_cell_add;
  if(rrcifreq_read_sib11bis(l1_inter_freq_sib_ptr,sib18_present,tmp_rrc_PLMNsOfInterFreqCellsList_ptr, tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr) == FAILURE)
  {
    
    l1_inter_freq_sib_ptr->num_cell_add = prev_cell_count;
  }
  return SUCCESS;

}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R5

DESCRIPTION

    This function processes the release 5 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r5
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
)
{
  rrc_InterFreqEventList * ifreq_event_list_ptr = NULL;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  uint32 idx=0;
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_v590ext_IEs* mcm_590_ext_ptr = NULL;

  uint8 count = 0;

  uint8 cnt =  0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

    /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4;

  if ((dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.
    criticalExtensions.u.r4.m.v4d0NonCriticalExtensionsPresent) &&
    (dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.
    criticalExtensions.u.r4.v4d0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent))
    {
      mcm_590_ext_ptr = &dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.
      criticalExtensions.u.r4.v4d0NonCriticalExtensions.v590NonCriticalExtensions.measurementControl_v590ext;
    }


  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = transaction_id;
  l1_inter_freq_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("IFREQ:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_setup)
  {

    /* Double check that it is INTER-FREQ Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r4_interFrequencyMeasurement)
    {
      WRRC_MSG1_HIGH("IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r4,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }


    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r4,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.t == T_rrc_InterFreqReportCriteria_r4_intraFreqReportingCriteria)
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

  


    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      & rrc_InterFrequencyMeasurement_r4,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,
        rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
         
         l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
         l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }

      else
      {
        /* Since L1 does not support this, reject the MCM */
        MSG_LOW("IFreq:Set Update t invalid",0,0,0);
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.t
      == T_rrc_InterFreqReportCriteria_interFreqReportingCriteria) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
       u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n > idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_event2f))
          {
             MSG_LOW("IFreq:Ifreq Set update not set and evt id != 2d or 2f ",0,0,0);
             rrcmeas_mcf.rrc_transaction_id = transaction_id;

             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
             rrc_free(l1_inter_freq_meas_params);
             return FAILURE;
          }
          idx++;
        }
      }
    }


    /* Process Reporting Criteria which is mandatory in case meas obj is present */

    if (rrcifreq_process_reporting_criteria_r5(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f, mcm_590_ext_ptr) == FAILURE)
    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */

      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }



    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
    {
      
     /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */

    result = rrcifreq_fill_inter_freq_info_r5(&rrc_mcm_ptr->measurementCommand.u.setup->u.
      interFrequencyMeasurement->interFreqCellInfoList,
      l1_inter_freq_meas_params) ;
    /* Fill cell list, cell info */
    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Msg */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
         rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }

      return FAILURE;

    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement_r4,measurementValidity))
    {

      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
    {

      MSG_LOW("IFreq:Unable to set MeasId in DB",0,0,0);
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r4_modify,measurementType)))
    {
      

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("IFreq:Addtl Meas spec. for Meas %d", rrc_mcm_ptr->measurementIdentity);

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */

            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
        }
      } /* End of if of Additional Meas Present */

    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r4,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r4,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }
      if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.t  == T_rrc_InterFreqReportCriteria_r4_intraFreqReportingCriteria) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
        & rrc_IntraFreqReportingCriteria_r4,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        & rrc_InterFrequencyMeasurement_r4,interFreqSetUpdate))
      {
          if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
            {
           
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }
        else
        {
          /* Since L1 does not support this, reject the MCM */
          MSG_LOW("IFreq:Set Update t invalid",0,0,0);
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
           rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }


      /* Process Reporting Criteria */

      if (rrcifreq_process_reporting_criteria_r5(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f, mcm_590_ext_ptr) == FAILURE)
      {

        if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.t == T_rrc_InterFreqReportCriteria_r4_interFreqReportingCriteria) &&
         !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria,interFreqEventList)) &&
         (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0)))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);


      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
               addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
               &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */

            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }

        }
      } /* End of if of Additional Meas Present */
      else
      {
        WRRC_MSG0_HIGH("IFREQ:additional meas list absent");
      }


      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info_r5(
        &rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        interFreqCellInfoList, l1_inter_freq_meas_params);
      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
                /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement_r4,measurementValidity))
      {
        
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
      {
        MSG_LOW("IFreq:Unable to set MeasId in DB",0,0,0);
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_freq_meas_params);
  return SUCCESS;
}

/*===========================================================================

FUNCTION  RRCIFREQ_FILL_INTER_FREQ_INFO_R5

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 5

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r5
(
 rrc_InterFreqCellInfoList_r4* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
)
{
  inter_freq_removed_cell_struct_type cascaded_rem_cell[L1_MAX_CELL_MEAS];

  uint32 inter_freq_cell_id = 0;
  uint32 idx=0;
  uint16 dl_freq = 0;

  uint16 psc = 0;

  uint32 cell_position = 0;


  uint16 psc_to_measure = INVALID_PSC;
  uint16 freq_to_measure = INVALID_INTER_FREQ;
  uint8 meas_count = 0;
  uint32 cell_id = 0;


  uint8 cell_count = 0, removed_cell_count = 0, psc_removed_count = 0;

  rrc_RemovedInterFreqCellList_removeSomeInterFreqCells* local_rm_inter_freq_cell_ptr = NULL;


  rrc_CellsForInterFreqMeasList *cellsForInterFreqMeasList_ptr = NULL;
  boolean valid_cell_id_found = FALSE;


  rrc_NewInterFreqCellList_r4* local_network_new_cell_list_ptr = NULL;


  /* Declare a local var to store the inter freq object */

  l1_inter_freq_meas_ctrl_struct_type* l1_inter_freq_meas_ptr = NULL;

  l1_inter_freq_meas_ptr = &l1_meas_ptr->meas_object.u.inter_f;

  /* Copy Inter Freq CELL_INFO_LIST to a local var. If failure in the processing of
  CELL_INFO_LIST, copy  inter_freq_cell_list_tmp back to inter_freq_cell_list*/
  for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
  {
    WCDMA_MEMCPY(&inter_freq_cell_list_tmp[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type),
                 &inter_freq_cell_list[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type));

    cascaded_rem_cell[cell_count].freq = INVALID_INTER_FREQ;
    cascaded_rem_cell[cell_count].psc = INVALID_PSC;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    rrc_InterFreqCellInfoList_r4,removedInterFreqCellList))
  {
    WRRC_MSG1_HIGH("IFreq:Remove:Delete [1: ALL  2: Some  3: No] %d cells selected",interFreqCellInfoList_ptr->removedInterFreqCellList.t);
    switch (interFreqCellInfoList_ptr->removedInterFreqCellList.t)
    {
    case T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells:
      
      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv = 0xff;


      /* Update Cell_info_list variable */
      for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
      {
        inter_freq_cell_list[cell_count].cell_position = VACANT;
        inter_freq_cell_list[cell_count].psc = INVALID_PSC;
        inter_freq_cell_list[cell_count].psc = INVALID_INTER_FREQ;
      }

      break;

    case T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells:

      if (interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells == NULL )
      {
        break;
      }
      
      /* Store head of Link list */
      local_rm_inter_freq_cell_ptr =  interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells;
      do
      {
        inter_freq_cell_id = local_rm_inter_freq_cell_ptr->elem[idx];

        /* Find the psc, dl freq as per given cell id from CELL_INFO_LIST */
        if ( rrcifreq_find_psc_freq(
          inter_freq_cell_id, &psc, &dl_freq) == PSC_ABSENT)
        {
          MSG_LOW("IFreq:Rem:Incorrect cell id %d",inter_freq_cell_id,0,0);
          idx++;
          continue;
        }
        else
        {
          /* First remove cell from inter freq cell list */
          rrcifreq_remove_cellid(inter_freq_cell_id);

          /* Here one needs to put psc, dl freq so that L1 can remove it */
          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc = psc;

          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].freq = dl_freq;

          removed_cell_count++; /* Increment count */
        }

        /*  Get next element in the list */

        idx++;

      } /* Continue if while cond satisfied */

      while ( (local_rm_inter_freq_cell_ptr->n  > idx) &&
        (removed_cell_count < L1_MAX_CELL_MEAS) );


      /* Now store the number of actual cells sent by UTRAN */

      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv =  removed_cell_count;
      WRRC_MSG1_HIGH("IFreq:Removed cell cnt = %d cells", removed_cell_count);

      break;

    case T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells:
      /* Already set removed cells to 0 */
      
      break;

    default:
      
      break;
    } /* End of switch */

  } /* End of if of removed inter freq cells present */

  /* Reinitialize these variables */
  cell_count = 0;
  psc = 0;

  /* Check if new cells present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    rrc_InterFreqCellInfoList_r4,newInterFreqCellList))
  {
    

    /* Do not check for cell order. If UTRAN messes up, its their problem */
    idx=0;
    /* Save the Head pointer of linked List */
    local_network_new_cell_list_ptr = &interFreqCellInfoList_ptr->newInterFreqCellList;

    while ( ( local_network_new_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {
    /* if SFN-ind is presnt then return as failure*/
      if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.t == 
        T_rrc_CellInfo_modeSpecificInfo_fdd)
      {
        if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.u.fdd->readSFN_Indicator ==TRUE)
        {
          return SFN_FAIL;
        }
      }
    /* Pointer to next cell is Non-NULL and cell count
      * hasn't exceeded the max. L1 can handle */

      if ((local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.t ==
        T_rrc_CellInfo_r4_modeSpecificInfo_tdd) ||
        (!(local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.u.fdd->m.primaryCPICH_InfoPresent)))
      {
        MSG_LOW("IFreq:Ignore TDD cell",0,0,0);

        idx++;
        continue;

      }

      /* Fill in Cell Info */
      if (rrcmeas_fill_mcm_cell_info_r5(
        &local_network_new_cell_list_ptr->elem[idx].cellInfo,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info) == FAILURE)
      {
        MSG_LOW("IFreq:Ignoring Cell",0,0,0);
        idx++;
        continue;
      }

      /* Check if cell id present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
        rrc_NewInterFreqCell_r4,interFreqCellID))
      {
        cell_position = local_network_new_cell_list_ptr->elem[idx].interFreqCellID;

        WRRC_MSG1_MED("IFreq:Add:Cell at pos %d to be added",cell_position);

      } /*  End of if of inter-freq cell id present */
      else
      {
        MSG_LOW("IFREQ:Add: Cell Id not present",0,0,0);
        cell_position = AVAILABLE_CELL_POSITION;
      }

      /* Check if freq present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
          rrc_NewInterFreqCell_r4,frequencyInfo))
      {
        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
          T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
        {
          MSG_LOW("IFreq:Ignore Cell. TDD cell",0,0,0);
          idx++;
          continue;
        }

        if (validate_frequency_info(&local_network_new_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
        {
          WRRC_MSG1_HIGH("Cell with DL freq %d out of ",
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
          return RRCMEAS_UNSUPPORTED;
        }


        if (local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
        {
          MSG_LOW("IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",dl_freq,0,0);
          idx++;
          continue;
        }

        /* Store DL freq */
        dl_freq = l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          freq_info.uarfcn_dl = (uint16) local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

        /* Calculate the UL Freq */
        /* Store the UL Freq */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
          uarfcn_ul_incl = FALSE;

        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
          uarfcn_ULPresent)
        {
          

          if (local_network_new_cell_list_ptr->elem[idx].
            frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
          {
            MSG_LOW("IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",local_network_new_cell_list_ptr->elem[idx].
              frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);
            idx++;
            continue;
          }
          /* Store the UL Freq */
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
            uarfcn_ul_incl = TRUE;

          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
            freq_info.uarfcn_ul = (uint16)
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
        } /* End of up freq present */
      } /*  End of if of inter-freq freq present */
      else
      {
        if (cell_count == 0)
        {
          WRRC_MSG0_HIGH("IFreq:Ignore Cell. DL Freq absent");
          idx++;
          continue;
        }
        else
        {
          /* This implies that there is already one DL and UL freq present. Copy from there */
          WCDMA_MEMCPY(&l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info,
                       sizeof(l1_freq_info_struct_type),
                       &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count-1].freq_info,
                       sizeof(l1_freq_info_struct_type));
        }
      }

      if ( rrcifreq_update_inter_freq_cell_list_mcm(
        &cell_position,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info.pri_scr_code, &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.uarfcn_dl,
        &psc, &dl_freq) == RRCMEAS_SUCCESS)
      {
        /* PSC sucessfully stored in the cell info list variable */
        /* Now store psc in the mcm in add cell for layer1 to use psc */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].psc =
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          cell_info.pri_scr_code;

        /* This flag is always true for MCM */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

        if ((psc != INVALID_PSC) && (dl_freq != INVALID_INTER_FREQ) && (psc_removed_count < L1_MAX_CELL_MEAS))
        {
          /* UTRAN wants us to delete this psc/freq. Result of CASCADING effect */
          cascaded_rem_cell[psc_removed_count].psc = psc;

          cascaded_rem_cell[psc_removed_count++].freq = dl_freq;
        }

        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }  /* End of if of rrcmeas_update_inter_freq_cell_list */

      /* Read next element */
      idx++;

    } /* End of while newIntraFreqCellList != NULL */

    /* Initialize no of added cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_add = cell_count;

    for (cell_count = 0; cell_count < psc_removed_count; cell_count++)
    {
      if (removed_cell_count >= L1_MAX_CELL_MEAS)
      {
        /* Preventive check */
        WRRC_MSG0_HIGH("IFREQ:Unable to delete all cells");
        break;
      }
      /* Add this removed cell to the already built removed cell list for L1 */
      WRRC_MSG1_HIGH("IFREQ:Cascad:L1 removed cell list upd %d psc",
        cascaded_rem_cell[cell_count].psc);
      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc =
        cascaded_rem_cell[cell_count].psc;

      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count++].freq =
        cascaded_rem_cell[cell_count].freq;

    }
    /* Now update the number of removed cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_rmv += psc_removed_count;

  } /* End of if for new inter freq cells present */


  /* Initialize no fo cells to 0 */
  l1_inter_freq_meas_ptr->cell_list.num_cell_meas = 0;
  cell_count = 0;
  idx=0;
  l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = FALSE;
  /* Check if cells for meas present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    & rrc_InterFreqCellInfoList_r4,cellsForInterFreqMeasList))
  {
    
    l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = TRUE;

    cellsForInterFreqMeasList_ptr = &interFreqCellInfoList_ptr->cellsForInterFreqMeasList;
    meas_count = 0;
    while (cellsForInterFreqMeasList_ptr->n > idx)
    {

      cell_id = cellsForInterFreqMeasList_ptr->elem[idx];
      /* Check if the cell id is present in intra_freq_cell_list VARIABLE */
      if ( rrcifreq_find_psc_freq(cell_id, &psc_to_measure, &freq_to_measure) == PSC_ABSENT)
      {
       MSG_LOW("Invalid cell to measure with id = %d. Ignoring cell id",cell_id,0,0);

      } /* End of if for psc not found */
      else
      {
         valid_cell_id_found=TRUE;
         l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count].psc = psc_to_measure;
         l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count++].dl_freq = freq_to_measure;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
       WRRC_MSG0_HIGH("None of the cell id are valid");
       MSG_LOW("REL5:Retain previous cell list before MCM rcved",0,0,0);

       WCDMA_MEMCPY(inter_freq_cell_list, 
                    sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS,
                    inter_freq_cell_list_tmp,
                    sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS);

       return RRCMEAS_UNSUPPORTED;
    }
    l1_inter_freq_meas_ptr->cell_list.num_cell_meas =  meas_count;

  } /* End of if of rrc_cellsForInterFreqMeasList) */

  return RRCMEAS_SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R5

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r5
(
 rrc_InterFreqReportCriteria_r4* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr,
 rrc_MeasurementControl_v590ext_IEs* mcm_590_ext_ptr
)
{
  WRRC_MSG1_HIGH("IFreq:Inter Freq rpt criteria: [1: Intra event 2: Inter Event  3: Periodic  4: No]:-- %d",ifreq_rpt_criteria_ptr->t);

  switch(ifreq_rpt_criteria_ptr->t)
  {
  
  case T_rrc_InterFreqReportCriteria_r4_interFreqReportingCriteria:

    

    l1_rpt_params_ptr->u.inter_rpt_crit.num_event = 0;

    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria,interFreqEventList)) &&
      (ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria->interFreqEventList.n != 0))
    {
      if (rrcifreq_process_event_criteria(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        &l1_rpt_params_ptr->u.inter_rpt_crit, mcm_590_ext_ptr) == FAILURE)

      {
        return FAILURE;
      }

    }
    else
    {
      MSG_LOW("InterFreq Event List Empty",0,0,0);
      return FAILURE;
    }

    /* Inter-Freq Events successfully processed */
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_EVT_TRIG;

    break;

  case T_rrc_InterFreqReportCriteria_r4_intraFreqReportingCriteria:

    MSG_LOW("IFreq:Intra Freq Event Rpt Crit. Setting rpt crit to L1_INTER_FREQ_NO_CHANGE",0,0,0);
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_CHANGE;
    break;

  case T_rrc_InterFreqReportCriteria_r4_periodicalReportingCriteria:

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_PERIODIC;

    

    l1_rpt_params_ptr->rpt_cell_status_incl = FALSE;

    /* Set reporting cells */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          MSG_LOW("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat,0,0);
          return FAILURE;
        }
      }
      else
      {
        return FAILURE;
      }
    }

 
      /* Store reporting amount */
      l1_rpt_params_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type)
        RRCMEAS_RET_REPORTING_AMOUNT(
        ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingAmount);
    
    MSG_LOW("IFreq:Rpting Amt is %d, Infin:0", l1_rpt_params_ptr->u.periodic_crit.rpt_amount,0,0);

    /* The spec indicates that 0 means UE behaviour unspecified */
    if (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
      MSG_LOW("Periodic Rpting 0 specified. use 250",0,0,0);
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS;
    }
    else
    {
      /* Store reporting interval */
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL
        (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingInterval);
    }

    MSG_LOW("IFreq:Rpting Intvl is %d msec", l1_rpt_params_ptr->u.periodic_crit.rpt_interval,0,0);
    break; /* Come out of switch */

  case T_rrc_InterFreqReportCriteria_r4_noReporting:
    

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_REPORTING;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.noReporting,
        rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.noReporting->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          MSG_LOW("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat,0,0);
          return FAILURE;
        }
      }
    }
    break;



  default:
    
    return FAILURE;

  }
  return SUCCESS;

}


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R6

DESCRIPTION

    This function processes the release 6 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r6
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
)
{

  rrc_InterFreqEventList_r6 * ifreq_event_list_ptr = NULL;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  uint32 idx=0;
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 count = 0;

  uint8 cnt =  0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

    /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6;

  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = transaction_id;
  l1_inter_freq_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("IFREQ:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r6_setup)
  {

    /* Double check that it is INTER-FREQ Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r6_interFrequencyMeasurement)
    {
      WRRC_MSG1_HIGH("IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r6,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r6,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.t == T_rrc_InterFreqReportCriteria_r6_intraFreqReportingCriteria)
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

  

    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
         rrc_InterFrequencyMeasurement_r6,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,
        rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
         
         l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
         l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }

      else
      {
        /* Since L1 does not support this, reject the MCM */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }
    if((RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria,
      rrc_InterFreqReportCriteria_r6_interFreqReportingCriteria) )&&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
       u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria_r6,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n > idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2f))
          {
             MSG_LOW("IFreq:Ifreq Set update not set and evt id != 2d or 2f ",0,0,0);
             rrcmeas_mcf.rrc_transaction_id = transaction_id;

             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
             rrc_free(l1_inter_freq_meas_params);
             return FAILURE;
          }
          idx++;
        }
      }
    }


    /* Process Reporting Criteria which is mandatory in case meas obj is present */

    if (rrcifreq_process_reporting_criteria_r6(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */

      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }



    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
    {
      
     /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    result = rrcifreq_fill_inter_freq_info_r5(&rrc_mcm_ptr->measurementCommand.u.setup->u.
      interFrequencyMeasurement->interFreqCellInfoList,
      l1_inter_freq_meas_params) ;
    /* Fill cell list, cell info */
    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */


      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */

  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement_r6,measurementValidity))
    {
      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
    {

      MSG_LOW("IFreq:Unable to set MeasId in DB",0,0,0);
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r6_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r6_modify,measurementType)))
    {
      

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,
        additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
        }
      } /* End of if of Additional Meas Present */

    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r6,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r6,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }
      if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.t  == T_rrc_InterFreqReportCriteria_r6_intraFreqReportingCriteria) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
        & rrc_IntraFreqReportingCriteria_r6,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        & rrc_InterFrequencyMeasurement_r6,interFreqSetUpdate))
      {
        if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
        {
           
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }

        else
        {
          /* Since L1 does not support this, reject the MCM */
          MSG_LOW("IFreq:Set Update t invalid",0,0,0);
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }


      /* Process Reporting Criteria */

      if (rrcifreq_process_reporting_criteria_r6(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
      {

        if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.t == T_rrc_InterFreqReportCriteria_r6_interFreqReportingCriteria) &&
         !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
         (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0)))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);


      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
               addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
               &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }

        }
      } /* End of if of Additional Meas Present */
      else
      {
        WRRC_MSG0_HIGH("IFREQ:additional meas list absent");
      }

      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info_r5(
        &rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        interFreqCellInfoList, l1_inter_freq_meas_params);
      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
                /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement_r6,measurementValidity))
      {
        
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
      {
        MSG_LOW("IFreq:Unable to set MeasId in DB",0,0,0);
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("IFREQ:Commit Addtl Meas to RRC DB");
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_freq_meas_params);
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R6

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r6
(
 rrc_InterFreqReportCriteria_r6* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
)
{
  WRRC_MSG1_HIGH("IFreq:Inter Freq rpt criteria: [1: Intra event 2: Inter Event  3: Periodic  4: No]:-- %d",ifreq_rpt_criteria_ptr->t);
  switch(ifreq_rpt_criteria_ptr->t)
  {
  case T_rrc_InterFreqReportCriteria_r6_interFreqReportingCriteria:

    

    l1_rpt_params_ptr->u.inter_rpt_crit.num_event = 0;

    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
      & rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
      (ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria->interFreqEventList.n != 0))
    {
      if (rrcifreq_process_event_criteria_r6(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        &l1_rpt_params_ptr->u.inter_rpt_crit) == FAILURE)
      {
        return FAILURE;
      }

    }
    else
    {
      WRRC_MSG0_HIGH("InterFreq Event List Empty");
      return FAILURE;
    }

    /* Inter-Freq Events successfully processed */
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_EVT_TRIG;

    break;

  case T_rrc_InterFreqReportCriteria_r6_intraFreqReportingCriteria:
    MSG_LOW("IFreq:Intra Freq Event Rpt Crit. Setting rpt crit to L1_INTER_FREQ_NO_CHANGE",0,0,0);
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_CHANGE;
    break;

  case T_rrc_InterFreqReportCriteria_r6_periodicalReportingCriteria:

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_PERIODIC;

    

    l1_rpt_params_ptr->rpt_cell_status_incl = FALSE;

    /* Set reporting cells */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
      else
      {
        return FAILURE;
      }
    }


      /* Store reporting amount */
      l1_rpt_params_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type)
        RRCMEAS_RET_REPORTING_AMOUNT(
        ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingAmount);
    
    /* The spec indicates that 0 means UE behaviour unspecified */
    if (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
      WRRC_MSG0_ERROR("Periodic Rpting 0 specified. use 250");
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS;
    }
    else
    {
      /* Store reporting interval */
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL
        (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingInterval);
    }

    WRRC_MSG2_HIGH("IFreq:Rpting Amt is %d, Infin:0. Rpting Intvl is %d msec", l1_rpt_params_ptr->u.periodic_crit.rpt_amount,l1_rpt_params_ptr->u.periodic_crit.rpt_interval);
    break; /* Come out of switch */

  case T_rrc_InterFreqReportCriteria_r6_noReporting:
    

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_REPORTING;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.noReporting,
        rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.noReporting->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        WRRC_MSG0_HIGH("IFREQ:Rpting Crit is Addtl");

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
    }
    break;



  default:
    
    return FAILURE;

  }
  return SUCCESS;

}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_EVENT_CRITERIA_R6

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

uecomdef_status_e_type rrcifreq_process_event_criteria_r6
(
  rrc_InterFreqReportingCriteria_r6* rrc_event_criteria_ptr,
  l1_inter_freq_report_crit_struct_type* l1_event_criteria_ptr
)
{
  /* Store the list in a local pointer */
  struct rrc_InterFreqEventList_r6* ifreq_event_list_ptr = NULL;

  uint8 cnt = 0, count = 0;

  /* To satisfy LINT */
  uint32 tmp = 0,idx=0;

  struct rrc_NonUsedFreqWList_r6 * local_2a_non_used_param_ptr = NULL;
  struct rrc_NonUsedFreqParameterList_r6 * local_non_used_param_ptr = NULL;

  ifreq_event_list_ptr = &rrc_event_criteria_ptr->interFreqEventList;

  if (ifreq_event_list_ptr == NULL)
  {
    WRRC_MSG0_ERROR("IFreq:Event Criteria list empty");
    return FAILURE;
  }

  while ((ifreq_event_list_ptr->n > idx) && (cnt < L1_MAX_MEAS_EVENT))
  {
    /* Get Event Id */
    l1_event_criteria_ptr->evt[cnt].evt_id =
      RET_INTER_FREQ_EVENT_TYPE_R6(ifreq_event_list_ptr->elem[idx].t);

    l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;
    l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

    switch(l1_event_criteria_ptr->evt[cnt].evt_id)
    {

    case L1_INTER_FREQ_EVENT_2A:
      /* Change of best frequency. */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        (ifreq_event_list_ptr->elem[idx].u.event2a->usedFreqW);

      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2a->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2a->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2a,
        rrc_Event2a_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2a->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2a,
        rrc_Event2a_r6,nonUsedFreqParameterList))
      {
        local_2a_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2a->nonUsedFreqParameterList;

        while ((local_2a_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh
            =  0;
          /* Non used f */
          tmp = local_2a_non_used_param_ptr->elem[count];
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) tmp;

          count++;
        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;

      }
      break;

    case L1_INTER_FREQ_EVENT_2B:
    /* The estimated quality of the currently used frequency is below a certain
    threshold and the estimated quality of a non-used frequency is above a certain
      threshold. */

      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        (ifreq_event_list_ptr->elem[idx].u.event2b->usedFreqThreshold);

      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        (ifreq_event_list_ptr->elem[idx].u.event2b->usedFreqW);

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2b->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2b->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2b,
        rrc_Event2b_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2b->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2b,
        rrc_Event2b_r6,nonUsedFreqParameterList))
      {
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2b->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;
          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;

    case L1_INTER_FREQ_EVENT_2C:
      /* The estimated quality of a non-used frequency is above a certain threshold. */

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2c->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2c->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2c,
        rrc_Event2c_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2c->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(
        ifreq_event_list_ptr->elem[idx].u.event2c,
        rrc_Event2c_r6,nonUsedFreqParameterList))
      {
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2c->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;

          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;

    case L1_INTER_FREQ_EVENT_2E:
      /* The estimated quality of a non-used frequency is below a certain threshold. */

      /* Vivek Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2e->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2e->timeToTrigger);

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2e,
        rrc_Event2e_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2e->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }
      /* Get Threshold Non-Used Freq */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_event_list_ptr->elem[idx].u.event2e,
          rrc_Event2e_r6,nonUsedFreqParameterList))
      {
        local_non_used_param_ptr =
          &ifreq_event_list_ptr->elem[idx].u.event2e->nonUsedFreqParameterList;

        while ((local_non_used_param_ptr->n > count) && (count < L1_MAX_NON_USED_FREQ))
        {
          /* Get non used threshold freq */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].non_used_f_thresh =
            (int16) local_non_used_param_ptr->elem[count].nonUsedFreqThreshold;

          /* Non used f */
          l1_event_criteria_ptr->evt[cnt].non_used_f_parm_array[count].w_non_used_f =
            (uint16) (local_non_used_param_ptr->elem[count].nonUsedFreqW);

          count++;

        } /* End of while */

        l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = count;
        count = 0;
      }
      break;

    case L1_INTER_FREQ_EVENT_2D:

      /* Get used freq threshold */
      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        ifreq_event_list_ptr->elem[idx].u.event2d->usedFreqThreshold;

      /* Get W */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2d->usedFreqW;

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2d->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2d->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(
        ifreq_event_list_ptr->elem[idx].u.event2d,
        rrc_Event2d_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2d->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }

      /* Initialize no of non used freq params to 0 */
      l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

      break;

    case L1_INTER_FREQ_EVENT_2F:

      /* Get used freq threshold */
      l1_event_criteria_ptr->evt[cnt].freq_thresh = (int16)
        ifreq_event_list_ptr->elem[idx].u.event2f->usedFreqThreshold;
      /* Get W */
      l1_event_criteria_ptr->evt[cnt].w_used = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2f->usedFreqW;

      /* Actual Value is IE*0.5. Hysterisis */
      l1_event_criteria_ptr->evt[cnt].hyst = (uint16)
        ifreq_event_list_ptr->elem[idx].u.event2f->hysteresis;

      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].time_to_trig =
        RET_INTER_FREQ_TIME_TO_TRIGGER(
        ifreq_event_list_ptr->elem[idx].u.event2f->timeToTrigger);

      l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = FALSE;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(
        ifreq_event_list_ptr->elem[idx].u.event2f,
        rrc_Event2f_r6,reportingCellStatus))
      {
        if (rrcmeas_fill_no_of_reporting_cells(
          &ifreq_event_list_ptr->elem[idx].u.event2f->reportingCellStatus,
          &l1_event_criteria_ptr->evt[cnt].rpt_cell_status) == SUCCESS)
        {
          l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl = TRUE;
        }
      }

      /* Initialize no of non used freq params to 0 */
      l1_event_criteria_ptr->evt[cnt].num_non_used_f_parm = 0;

      break;

    default:
      WRRC_MSG1_ERROR("InterFreq Event %d not supported", l1_event_criteria_ptr->evt[cnt].evt_id);
      return FAILURE;

    } /* End of switch */

    if ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl == TRUE) &&
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat != L1_RPT_ASET) &&
      ((l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2D) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2F)))
    {
      WRRC_MSG1_ERROR("Event 2D/2F, Rpt cell stat!= ASET",l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat);
      return FAILURE;
    }
    else if ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status_incl == TRUE) &&
      ((l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2A) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2B) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2C) ||
      (l1_event_criteria_ptr->evt[cnt].evt_id == L1_INTER_FREQ_EVENT_2E)) &&
      ((l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ASET) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
      (l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ))
    {
      WRRC_MSG1_ERROR("Event 2A/B/C/D, rpt cell stat %d",
        l1_event_criteria_ptr->evt[cnt].rpt_cell_status.rpt_cell_stat);

      return FAILURE;
    }
    else
    {
      MSG_LOW("Reporting Cell Status valid",0,0,0);
    }

    cnt++;

    /* Increment Pointer count */
    idx++;
  } /* End of while */

  /* Store the count */
  l1_event_criteria_ptr->num_event = cnt;

  return SUCCESS;

}



/*==========================================================================
 FUNCTION   rrcifreq_free_memory_allocated_for_reporting

DESCRIPTION

  This function takes care of deallocating the cell list  which was allocated by L1 when it is 
  sending a inter frequency measurement indication to RRC
DEPENDENCIES

    None
RETURN VALUE

    None
SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_free_memory_allocated_for_reporting
(
  l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  int freq_idx=0;
  int cell_count=0;
  WRRC_MSG0_MED("Deallocating the Cell list allocated by L1");
  if(l1_meas_report_ptr == NULL) 
  {
    WRRC_MSG0_ERROR("Measurement Pointer is NULL");
    return ;
  }
  if(l1_meas_report_ptr->u.inter_f_rpt.meas_results_included == FALSE)
  {
    MSG_LOW("Measurement Results not Included",0,0,0);
    return;
  }
  for (freq_idx = 0;(freq_idx < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.num_msmt_results)&&(freq_idx<L1_MAX_FREQ);freq_idx++)
  {
    for ( cell_count = 0; ((cell_count < l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[freq_idx].num_cells)
      && ( cell_count < L1_MAX_CELL_MEAS)); cell_count++ )
    {
      if (l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[freq_idx].cell[cell_count] != NULL)
      {
        rrc_free_interlayer_cmd(l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[freq_idx].cell[cell_count]);
        l1_meas_report_ptr->u.inter_f_rpt.inter_freq_measurements.msmt_results[freq_idx].cell[cell_count] = NULL;
      }
    }
  }
}


#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif
/*===========================================================================

FUNCTION rrcifreq_read_sib11bis

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

uecomdef_status_e_type rrcifreq_read_sib11bis
(
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr,
  boolean sib18_present,
  rrc_PLMNsOfInterFreqCellsList *tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
  rrc_MultiplePLMNsOfInterFreqCellsList* tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr
)
{
  rrc_SysInfoType11bis *sib11bis_ptr = NULL;
  rrc_MeasurementControlSysInfoExtensionAddon_r5 *sib11bis_rel5_ptr = NULL;
  rrc_NewInterFreqCellSI_List_HCS_RSCP * inter_freq_meas_hcs_rscp_ptr = NULL;
  rrc_NewInterFreqCellSI_List_HCS_ECN0 * inter_freq_meas_hcs_ecn0_ptr = NULL;
  rrc_NewInterFreqCellSI_List_RSCP * inter_freq_meas_rscp_ptr = NULL;
  rrc_NewInterFreqCellSI_List_ECN0 * inter_freq_meas_ecn0_ptr = NULL;
  rrc_MeasurementControlSysInfoExtensionAddon_r5_newInterFrequencyCellInfoListAddon_r5 *rel5_ext_ptr = NULL; 
  uint16 cell_count = 0;
  uint32 cell_position = 0, idx = 0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
  rrc_csp_band_class_type tmp_band_class = RRC_CSP_BAND_MAX;
  boolean band_found = FALSE;
  int mfbi_index = -1;
#endif
  rrc_plmn_identity_type tmp_plmn_identity;
  rrc_plmn_identity_type selected_plmn_identity;
  rrc_int_cell_id_type  tmp_cell_id;

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR  
  rrc_MultipleFrequencyBandIndicatorListFDD *inter_freq_multifreq_ptr = NULL;
#endif
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
  if(!(sib11bis_ptr->m.measurementControlSysInfoPresent))
  {
    WRRC_MSG0_HIGH("SIB11bis: Mesaurement Control SysInfo is not Present in SIB11bis");
    return FAILURE;
  }
  if(sib11bis_ptr->m.measurementControlSysInfoExtensionAddon_r5Present)
  {
    MSG_LOW("SIB11bis: Rel5 Measurement Control info present in SIB11bis",0,0,0);
    sib11bis_rel5_ptr = &sib11bis_ptr->measurementControlSysInfoExtensionAddon_r5;
    if(sib11bis_rel5_ptr->m.newInterFrequencyCellInfoListAddon_r5Present)
    {
      MSG_LOW("SIB11bis: Rel5 Cell Selection Reselection info present for Inter frequency",0,0,0);
      rel5_ext_ptr = &sib11bis_rel5_ptr->newInterFrequencyCellInfoListAddon_r5;
    }
  }

  if(sib18_present == FALSE)
  {
    MSG_LOW("SIB11bis: SIB18 is Absent/Ignored",0,0,0);
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
    MSG_LOW("SIB11bis: HCS Used in SIB11bis",0,0,0);
    l1_inter_freq_sib_ptr->hcs_used = TRUE;
    switch(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->cellSelectQualityMeasure.t)
    {
      /*RSCP is choosen and HCS is used*/
      case T_rrc_MeasurementControlSysInfoExtension_cellSelectQualityMeasure_1_cpich_RSCP:
        MSG_LOW("SIB11bis: cellSelectQualityMeasure is RSCP ",0,0,0);
        if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->cellSelectQualityMeasure.u.cpich_RSCP->m.newInterFreqCellListPresent))
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter Frequency Cell list is absent");
          return FAILURE;
        }
        inter_freq_meas_hcs_rscp_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
            cellSelectQualityMeasure.u.cpich_RSCP->newInterFreqCellList;
        cell_count = l1_inter_freq_sib_ptr->num_cell_add;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        while ((inter_freq_meas_hcs_rscp_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          /* Get the Freq Info */
          if (inter_freq_meas_hcs_rscp_ptr->elem[idx].m.frequencyInfoPresent)
          {
            if(inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              WRRC_MSG0_HIGH("SIB11bis: IFreq:Ignore Cell tdd");

              idx++;              
              continue;
            }

            if (validate_frequency_info(&inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              /*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if((rrc_mfbi_support_nv) && (sib11bis_ptr != NULL) && 
                 (sib11bis_ptr->m.v7b0NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) && 
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                           sysInfoType11bis_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);

                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);

                if(band_found != TRUE)
                {
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((inter_freq_meas_hcs_rscp_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_hcs_rscp_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                    idx++;
                  }

                  continue;
                }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("SIB11bis: Ignored cell with dl freq %d",
                inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);

                idx++;              
                /*Ignore all the cells till we find next freq*/
                while((inter_freq_meas_hcs_rscp_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_hcs_rscp_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                {
                  idx++;
                }

                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11bis");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
            }
            else
            {
#endif
              /* Downlink Freq Present */
              if (inter_freq_meas_hcs_rscp_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                WRRC_MSG1_HIGH("SIB11bis: IFreq:Invalid DL ARFCN %d.Ignore Inter Freq Cell",inter_freq_meas_hcs_rscp_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);
              
                idx++;
                continue;
              }

              /* Store DL freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) inter_freq_meas_hcs_rscp_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

              WRRC_MSG1_HIGH("SIB11bis: IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl);

              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;

              if (inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                

                if (inter_freq_meas_hcs_rscp_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  WRRC_MSG1_HIGH("SIB11bis: IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_hcs_rscp_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL);

                  idx++;                
                  continue;
                }
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;

                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  inter_freq_meas_hcs_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("SIB11bis: IFreq:Ignore Cell. DL Freq absent");

              idx++;              
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }

          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
            }
          }

          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_rscp_cell_info(&inter_freq_meas_hcs_rscp_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("IFreq:Ignore InterFreq Cell");

            idx++;           
            continue;
          }
          /* Check if inter-freq cell ID is present */
          if ( inter_freq_meas_hcs_rscp_ptr->elem[idx].m.interFreqCellIDPresent)
          {
            cell_position = inter_freq_meas_hcs_rscp_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */



          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;
  
          if((cell_count == L1_MAX_CELL_MEAS) &&
            (inter_freq_meas_hcs_rscp_ptr->n > idx))
          {
            /* If there are more new cells than can be accomodated in L1
            Cmd, print a message and hope L1 will change their capability
            sometime. Just pass the number that L1 can handle.
              */
            MSG_LOW("SIB11bis: All Inter freq cells not included",0,0,0);
            break;
          }
        }/* End of while */
        
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;

        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          WRRC_MSG0_HIGH("IFreq:No new cell. SIB req not sent");
          return FAILURE;
        }
        
        break;

      /*ECNO is choosen and HCS is used*/
      case T_rrc_MeasurementControlSysInfoExtension_cellSelectQualityMeasure_1_cpich_Ec_N0:
        MSG_LOW("SIB11bis: cellSelectQualityMeasure is ECNO ",0,0,0);
        if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->cellSelectQualityMeasure.u.cpich_Ec_N0->m.newInterFreqCellListPresent))
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter Frequency Cell list is absent");
          return FAILURE;
        }
        inter_freq_meas_hcs_ecn0_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
            cellSelectQualityMeasure.u.cpich_Ec_N0->newInterFreqCellList;
        cell_count = l1_inter_freq_sib_ptr->num_cell_add;
        idx = 0;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        while ((inter_freq_meas_hcs_ecn0_ptr->n > idx) && (cell_count < L1_MAX_CELL_MEAS))
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */

          /* Get the Freq Info */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          if (inter_freq_meas_hcs_ecn0_ptr->elem[idx].m.frequencyInfoPresent)
          {
            if(inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("SIB11bis: IFreq:Ignore Cell tdd",0,0,0);

              idx++;              
              continue;
            }

            if (validate_frequency_info(&inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
 /*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if((rrc_mfbi_support_nv) && (sib11bis_ptr != NULL) && 
                 (sib11bis_ptr->m.v7b0NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) && 
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                           sysInfoType11bis_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);

                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);

                if(band_found != TRUE)
                {
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((inter_freq_meas_hcs_ecn0_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_hcs_ecn0_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                    idx++;
                  }

                  continue;
                }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("SIB11bis: Ignored cell with dl freq %d",
                  inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);

                idx++;              
                /*Ignore all the cells till we find next freq*/
                while((inter_freq_meas_hcs_ecn0_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_hcs_ecn0_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                {
                  idx++;
                }

                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11bis");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
            }
            else
            {
#endif
              /* Downlink Freq Present */
              if (inter_freq_meas_hcs_ecn0_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                MSG_LOW("SIB11bis: IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_hcs_ecn0_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);
              
                idx++;
                continue;
              }

              /* Store DL freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) inter_freq_meas_hcs_ecn0_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

              WRRC_MSG1_HIGH("SIB11bis: IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl);

              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;

              if (inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                

                if (inter_freq_meas_hcs_ecn0_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  MSG_LOW("SIB11bis: IFreq:Invalid UL ARFCN %d.Ignore Inter Freq Cell",inter_freq_meas_hcs_ecn0_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);

                  idx++;                
                  continue;
                }
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;

                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  inter_freq_meas_hcs_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              MSG_LOW("SIB11bis: IFreq:Ignore Cell. DL Freq absent",0,0,0);

              idx++;              
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }

          /* Set default values for the HCS neighboring cell info if it is first cell
          * else use the info from the previous cell */
          if(cell_count == 0)
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
          }
          else
          {
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.hcs_prio;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.q_hcs;
            l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time =
              l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time;

            if(l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.penalty_time
              != 0)
            {
              l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.tmp_offset1 =
                l1_inter_freq_sib_ptr->add_cell[cell_count - 1].cell_info.hcs_info.tmp_offset1;
            }
          }

          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_hcs_ecn0_cell_info(&inter_freq_meas_hcs_ecn0_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            MSG_LOW("IFreq:Ignore InterFreq Cell",0,0,0);

            idx++;            
            continue;
          }
          /* Check if inter-freq cell ID is present */
          if ( inter_freq_meas_hcs_ecn0_ptr->elem[idx].m.interFreqCellIDPresent)
          {
            cell_position = inter_freq_meas_hcs_ecn0_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */



          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */

          /* Get the next cell */
          idx++;

          if((cell_count == L1_MAX_CELL_MEAS) &&
            (inter_freq_meas_hcs_ecn0_ptr->n > idx))
          {
            /* If there are more new cells than can be accomodated in L1
            Cmd, print a message and hope L1 will change their capability
            sometime. Just pass the number that L1 can handle.
              */
            MSG_LOW("SIB11bis: All Inter freq cells not included",0,0,0);
            break;
          }
        }/* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;

        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          WRRC_MSG0_HIGH("IFreq:No new cell. SIB req not sent");
          return FAILURE;
        }
        break;
        
      default:
        WRRC_MSG1_ERROR("SIB11bis: Invalid cellSelectQualityMeasure :%d",sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->cellSelectQualityMeasure.t);
        return FAILURE;
    }
  }
  else
  {
    l1_inter_freq_sib_ptr->hcs_used = FALSE;
    MSG_LOW("SIB11bis: HCS is not Used in SIB11bis",0,0,0);
    switch(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.t)
    {
      case T_rrc_MeasurementControlSysInfoExtension_cellSelectQualityMeasure_1_cpich_RSCP:
        MSG_LOW("SIB11bis: cellSelectQualityMeasure is RSCP ",0,0,0);
        if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.u.cpich_RSCP->m.newInterFreqCellListPresent))
        {
          WRRC_MSG0_HIGH("SIB11bis: Inter Frequency Cell list is absent");
          return FAILURE;
        }
        inter_freq_meas_rscp_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
            cellSelectQualityMeasure.u.cpich_RSCP->newInterFreqCellList;
        cell_count = l1_inter_freq_sib_ptr->num_cell_add;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        while ( (inter_freq_meas_rscp_ptr->n > idx) &&
          (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
  
          /* Get the Freq Info */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          if (inter_freq_meas_rscp_ptr->elem[idx].m.frequencyInfoPresent)
          {
            if (inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("SIB11bis: IFreq:Ignore Cell. TDD cell",0,0,0);

              idx++;              
              continue;
            }
  
            if (validate_frequency_info(&inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
/*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if((rrc_mfbi_support_nv) && (sib11bis_ptr != NULL) && 
                 (sib11bis_ptr->m.v7b0NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) && 
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                           sysInfoType11bis_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);

                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);

                if(band_found != TRUE)
                {
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((inter_freq_meas_rscp_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_rscp_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                    idx++;
                  }

                  continue;
                }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("SIB11bis: Ignored cell with dl freq %d",
                  inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);

                idx++;              
                /*Ignore all the cells till we find next freq*/
                while((inter_freq_meas_rscp_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_rscp_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                {
                  idx++;
                }

                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11bis");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
            }
            else
            {
#endif
              if (inter_freq_meas_rscp_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                MSG_LOW("SIB11bis: IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_rscp_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);

                idx++;              
                continue;
              }
              /* Store DL freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
  
              WRRC_MSG1_HIGH("SIB11bis: IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl);
  
              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;
  
              if (inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                
  
                if (inter_freq_meas_rscp_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  MSG_LOW("SIB11bis: IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_rscp_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);

                  idx++;                
                  continue;
                }
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;
  
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  inter_freq_meas_rscp_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              WRRC_MSG0_HIGH("SIB11bis: IFreq:Ignore Cell. DL Freq absent");

              idx++;              
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }
  
          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_rscp_cell_info(&inter_freq_meas_rscp_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            WRRC_MSG0_HIGH("SIB11bis: IFreq:Ignore InterFreq Cell");

            idx++;            
            continue;
          }
          /* Check if inter-freq cell ID is present */
          if ( inter_freq_meas_rscp_ptr->elem[idx].m.interFreqCellIDPresent)
          {
            cell_position = inter_freq_meas_rscp_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */
  
  
          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
  
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }

            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */
  
          /* Get the next cell */
          idx++;
        } /* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;
        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          MSG_LOW("IFreq:No new cell. SIB req not sent",0,0,0);
          return FAILURE;
        }
        break;

        
      case T_rrc_MeasurementControlSysInfoExtension_cellSelectQualityMeasure_1_cpich_Ec_N0:
        
        if(!(sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->cellSelectQualityMeasure.u.cpich_Ec_N0->m.newInterFreqCellListPresent))
        {
          MSG_LOW("SIB11bis: Inter Frequency Cell list is absent",0,0,0);
          return FAILURE;
        }
        inter_freq_meas_ecn0_ptr = &sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
            cellSelectQualityMeasure.u.cpich_Ec_N0->newInterFreqCellList;
        cell_count = l1_inter_freq_sib_ptr->num_cell_add;
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        mfbi_index = -1;
#endif
        while ( (inter_freq_meas_ecn0_ptr->n > idx) &&
          (cell_count < L1_MAX_CELL_MEAS) )
        {
        /* Pointer to next cell is Non-NULL and cell count
          hasn't exceeded the max as per specs */
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          mfbi_index++;
          band_found = FALSE;
#endif
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.hcs_prio = 0;
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.q_hcs = 115;
          l1_inter_freq_sib_ptr->add_cell[cell_count].cell_info.hcs_info.penalty_time = 0;
  
          /* Get the Freq Info */
          if (inter_freq_meas_ecn0_ptr->elem[idx].m.frequencyInfoPresent)
          {
            if (inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
              T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
            {
              MSG_LOW("SIB11bis: IFreq:Ignore Cell. TDD cell",0,0,0);

              idx++;              
              continue;
            }
  
            if (validate_frequency_info(&inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo) == FAILURE)
            {
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
/*As the frequency validation failed,we look into the MFBI list present for this UARFCN*/
              if((rrc_mfbi_support_nv) && (sib11bis_ptr != NULL) && 
                 (sib11bis_ptr->m.v7b0NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) && 
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.m.va80NonCriticalExtensionsPresent) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .m.multipleFrequencyInfoListFDDPresent) &&
                 ((sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.n) > mfbi_index) &&
                 (sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType11bis_va80ext
                   .multipleFrequencyInfoListFDD.elem[mfbi_index].m.multipleFrequencyBandIndicatorListFDDPresent))
              {
                inter_freq_multifreq_ptr=&(sib11bis_ptr->v7b0NonCriticalExtensions.v860NonCriticalExtensions.va80NonCriticalExtensions.
                                           sysInfoType11bis_va80ext.multipleFrequencyInfoListFDD.elem[mfbi_index].multipleFrequencyBandIndicatorListFDD);

                rrcifreq_check_if_multiplefrequency_bands_supported(inter_freq_multifreq_ptr,
                                                                    &band_found,
                                                                    &tmp_band_class);

                if(band_found != TRUE)
                {
                  idx++;
                  /*Ignore all the cells till we find next freq*/
                  while((inter_freq_meas_ecn0_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_ecn0_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                  {
                     idx++;
                  }

                  continue;
                }
              }
              else
              {
#endif
                WRRC_MSG1_HIGH("SIB11bis: Ignored cell with dl freq %d",
                  inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL);

                idx++;             
                /*Ignore all the cells till we find next freq*/
                while((inter_freq_meas_ecn0_ptr->n > idx) && !(RRC_MSG_LIST_BITMASK_IE(inter_freq_meas_ecn0_ptr,rrc_NewInterFreqCellSI_HCS_ECN0,frequencyInfo)))
                {
                  idx++;
                }

                continue;
              }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
            }
            if(band_found)
            {
              WRRC_MSG0_HIGH("a supported band is found in the MFBI list of SIB11bis");
              /*If we find a supported band in the MFBI list,we convert the UARFCN of previous band into the UARFCN
                of the current band*/
              if(rrcifreq_populate_sibs_if_band_found(tmp_band_class,
                                                   inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo,
                                                   &(l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info),
                                                   selected_plmn_identity) == FALSE)
              {
                idx++;
                continue;
              }
              
            }
            else
            {
#endif
              if (inter_freq_meas_ecn0_ptr->elem[idx].
                frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
              {
                MSG_LOW("SIB11bis: IFreq:Invalid DL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_ecn0_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL,0,0);

                idx++;              
                continue;
              }
              /* Store DL freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl = (uint16) inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
  
              WRRC_MSG1_HIGH("SIB11bis: IFreq:DL Freq is %d", l1_inter_freq_sib_ptr->add_cell[cell_count].
                freq_info.uarfcn_dl);
  
              /* Calculate the UL Freq */
              /* Store the UL Freq */
              l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                uarfcn_ul_incl = FALSE;
  
              if (inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent)
              {
                
  
                if (inter_freq_meas_ecn0_ptr->elem[idx].
                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
                {
                  MSG_LOW("SIB11bis: IFreq:Invalid UL ARFCN %d. Ignore Inter Freq Cell",inter_freq_meas_ecn0_ptr->elem[idx].
                    frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL,0,0);

                  idx++;                
                  continue;
                }
                /* Store the UL Freq */
                l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info.
                  uarfcn_ul_incl = TRUE;
  
                l1_inter_freq_sib_ptr->add_cell[cell_count].
                  freq_info.uarfcn_ul = (uint16)
                  inter_freq_meas_ecn0_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
              }
            }
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
          }
#endif
          else
          {
            if (cell_count == 0)
            {
              MSG_LOW("SIB11bis: IFreq:Ignore Cell. DL Freq absent",0,0,0);

              idx++;              
              continue;
            }
            else
            {
              /* This implies that there is already one DL and UL freq present. Copy from there */
              WCDMA_MEMCPY(&l1_inter_freq_sib_ptr->add_cell[cell_count].freq_info,
                           sizeof(l1_freq_info_struct_type),
                           &l1_inter_freq_sib_ptr->add_cell[cell_count-1].freq_info,
                           sizeof(l1_freq_info_struct_type));
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
              mfbi_index--;
#endif
            }
          }
  
          /* First store cell info so that psc of this cell can be extracted */
          if (rrcmeas_fill_ecn0_cell_info(&inter_freq_meas_ecn0_ptr->elem[idx].
            cellInfo, &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info, ((rel5_ext_ptr != NULL) ? ((rel5_ext_ptr->n > idx) ? (&rel5_ext_ptr->elem[idx]) : NULL) : NULL)) == FAILURE)
          {
            MSG_LOW("SIB11bis: IFreq:Ignore InterFreq Cell",0,0,0);

            idx++;            
            continue;
          }
          /* Check if inter-freq cell ID is present */
          if ( inter_freq_meas_ecn0_ptr->elem[idx].m.interFreqCellIDPresent)
          {
            cell_position = inter_freq_meas_ecn0_ptr->elem[idx].interFreqCellID;
          } /* End of if of intra-freq cell id present */
          else
          {
            cell_position = AVAILABLE_CELL_POSITION;
          } /* End of else of inter-freq cell id not present */
  
  
          if ( rrcifreq_update_inter_freq_cell_list(
            &cell_position,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            cell_info.pri_scr_code,
            &l1_inter_freq_sib_ptr->add_cell[cell_count].
            freq_info.uarfcn_dl) == SUCCESS)
          {
            /* PSC sucessfully stored in the cell info list variable */
            /* Now store psc in the sib11 in add cell for layer1 to use psc */
            l1_inter_freq_sib_ptr->add_cell[cell_count].psc =
              l1_inter_freq_sib_ptr->add_cell[cell_count].
              cell_info.pri_scr_code;
  
            /* set the measure in PCH IDLE and FACH flag to TRUE */
            l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach = TRUE;
            if(sib18_present)
            {
              rrcmeas_process_sib18_filtering(
                 (rrc_MultiplePLMNsOfIntraFreqCellsList *)tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr,
                 (rrc_PLMNsOfIntraFreqCellsList *)tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
                 &tmp_plmn_identity,
                 cell_count,
                 &(l1_inter_freq_sib_ptr->add_cell[cell_count].measure_in_idle_pch_fach),
                 (cell_count > 0)?l1_inter_freq_sib_ptr->add_cell[cell_count -1].measure_in_idle_pch_fach: TRUE);
            }
            /* Increment cell count as add cell successfully stored */
            cell_count ++;
          }  /* End of if of rrcmeas_update_inter_freq_cell_list */
  
          /* Get the next cell */
          idx++;          
        } /* End of while */
        /* Set number of added cells */
        l1_inter_freq_sib_ptr->num_cell_add = cell_count;
        if (l1_inter_freq_sib_ptr->num_cell_add == 0)
        {
          WRRC_MSG0_HIGH("IFreq:No new cell. SIB req not sent");
          return FAILURE;
        }
        break;

      default:
        WRRC_MSG1_ERROR("SIB11bis: Invalid cellSelectQualityMeasure :%d",sib11bis_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->cellSelectQualityMeasure.t);
        return FAILURE;
    }
  }
  return SUCCESS;
}
/*==========================================================================
 FUNCTION   rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind

DESCRIPTION

  This function takes care of deallocating the cell list  which was allocated by L1 when it is 
  sending a inter frequency measurement indication to RRC
DEPENDENCIES

    None
RETURN VALUE

    None
SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind
(
  rrc_cmd_type* cmd_ptr
)
{
  if (cmd_ptr->cmd.meas_ind.meas_rpt->meas_type == L1_INTER_FREQ)
  {
    rrcifreq_free_memory_allocated_for_reporting(cmd_ptr->cmd.meas_ind.meas_rpt);
  }
}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R7

DESCRIPTION

    This function processes the release 7 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r7
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
)
{
  rrc_InterFreqEventList_r6 * ifreq_event_list_ptr = NULL;

  uint32 idx=0;
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 count = 0;

  uint8 cnt =  0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

    /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;

  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = transaction_id;
  l1_inter_freq_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("IFREQ:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r7_setup)
  {

    /* Double check that it is INTER-FREQ Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r7_interFrequencyMeasurement)
    {
      WRRC_MSG1_HIGH("IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r7,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r7,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.t == T_rrc_InterFreqReportCriteria_r7_intraFreqReportingCriteria)
    {
      WRRC_MSG0_ERROR("IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);



    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
       rrc_InterFrequencyMeasurement_r7,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,
        rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
         
         l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
         l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }

      else
      {
        /* Since L1 does not support this, reject the MCM */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.t
      == T_rrc_InterFreqReportCriteria_r7_interFreqReportingCriteria) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
       u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria_r6,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n > idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2f))
          {
             WRRC_MSG0_ERROR("IFreq:Ifreq Set update not set and evt id != 2d or 2f ");
             rrcmeas_mcf.rrc_transaction_id = transaction_id;

             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
             rrc_free(l1_inter_freq_meas_params);
             return FAILURE;
          }
          idx++;
        }
      }
    }

    /* Process Reporting Criteria which is mandatory in case meas obj is present */

    if (rrcifreq_process_reporting_criteria_r7(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */

      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }



    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
    {
      
     /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    result = rrcifreq_fill_inter_freq_info_r5(&rrc_mcm_ptr->measurementCommand.u.setup->u.
      interFrequencyMeasurement->interFreqCellInfoList,
      l1_inter_freq_meas_params) ;
    /* Fill cell list, cell info */
    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement_r7,measurementValidity))
    {
     
      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
    {

      WRRC_MSG0_ERROR("IFreq:Unable to set MeasId in DB");
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r7_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r7_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("IFREQ:Modify:InterFreq:MType absent");

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("IFreq:Addtl Meas spec. for Meas %d", rrc_mcm_ptr->measurementIdentity);

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
        }
      } /* End of if of Additional Meas Present */

    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r7,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r7,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }
      if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.t  == T_rrc_InterFreqReportCriteria_r7_intraFreqReportingCriteria) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
        rrc_IntraFreqReportingCriteria_r7,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
         rrc_InterFrequencyMeasurement_r7,interFreqSetUpdate))
      {
        if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
        {
           
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }

        else
        {
          /* Since L1 does not support this, reject the MCM */
          
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }


      /* Process Reporting Criteria */

      if (rrcifreq_process_reporting_criteria_r7(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
      {

        if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.t == T_rrc_InterFreqReportCriteria_r7_interFreqReportingCriteria) &&
         !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
         (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0)))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);

      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
               addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
               &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }

        }
      } /* End of if of Additional Meas Present */
      else
      {
        WRRC_MSG0_HIGH("IFREQ:additional meas list absent");
      }

      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info_r5(
        &rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        interFreqCellInfoList, l1_inter_freq_meas_params);
      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
                /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement_r7,measurementValidity))
      {
        
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
      {
        WRRC_MSG0_ERROR("IFreq:Unable to set MeasId in DB");
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        MSG_LOW("IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
   
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_freq_meas_params);
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R7

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r7
(
 rrc_InterFreqReportCriteria_r7* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
)
{
  WRRC_MSG1_HIGH("IFreq:Inter Freq rpt criteria: [1: Intra event 2: Inter Event  3: Periodic	4: No]:-- %d",ifreq_rpt_criteria_ptr->t);

  switch(ifreq_rpt_criteria_ptr->t)
  {
  case T_rrc_InterFreqReportCriteria_r7_interFreqReportingCriteria:

    

    l1_rpt_params_ptr->u.inter_rpt_crit.num_event = 0;

    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
      & rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
      (ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria->interFreqEventList.n != 0))
    {
      if (rrcifreq_process_event_criteria_r6(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        &l1_rpt_params_ptr->u.inter_rpt_crit) == FAILURE)
      {
        return FAILURE;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("InterFreq Event List Empty");
      return FAILURE;
    }

    /* Inter-Freq Events successfully processed */
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_EVT_TRIG;

    break;

  case T_rrc_InterFreqReportCriteria_r7_intraFreqReportingCriteria:
    MSG_LOW("IFreq: Intra Freq Event Rpt Crit. Setting rpt crit to L1_INTER_FREQ_NO_CHANGE",0,0,0);
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_CHANGE;
    break;

  case T_rrc_InterFreqReportCriteria_r7_periodicalReportingCriteria:

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_PERIODIC;

    

    l1_rpt_params_ptr->rpt_cell_status_incl = FALSE;

    /* Set reporting cells */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
      else
      {
        return FAILURE;
      }
    }

    /* Check if reporting amount present */
 
      /* Store reporting amount */
      l1_rpt_params_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type)
        RRCMEAS_RET_REPORTING_AMOUNT(
        ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingAmount);
    
    /* The spec indicates that 0 means UE behaviour unspecified */
    if (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
      WRRC_MSG0_ERROR("Periodic Rpting 0 specified. use 250");
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS;
    }
    else
    {
      /* Store reporting interval */
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL
        (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingInterval);
    }

    WRRC_MSG2_HIGH("IFreq:Rpting Amt is %d, Infin:0 Rpting Intvl is %d msec", l1_rpt_params_ptr->u.periodic_crit.rpt_amount, 
             l1_rpt_params_ptr->u.periodic_crit.rpt_interval);
    break; /* Come out of switch */

  case T_rrc_InterFreqReportCriteria_r7_noReporting:
    

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_REPORTING;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.noReporting,
        rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.noReporting->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        WRRC_MSG0_HIGH("IFREQ:Rpting Crit is Addtl");

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
    }
    break;



  default:
    
    return FAILURE;

  }
  return SUCCESS;

}


/*===========================================================================
FUNCTION   RRCIFREQ_PROCESS_MCM_R8

DESCRIPTION
    This function processes the release 7 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES
    None.

RETURN VALUE
    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS
    None
==========================================================================*/
uecomdef_status_e_type rrcifreq_process_mcm_r8
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
)
{
  struct rrc_InterFreqEventList_r6 * ifreq_event_list_ptr = NULL;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  uint32 idx=0;
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 count = 0;

  uint8 cnt =  0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

    /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;

  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = transaction_id;
  l1_inter_freq_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("REL8:IFREQ:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_setup)
  {

    /* Double check that it is INTER-FREQ Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r8_interFrequencyMeasurement)
    {
      WRRC_MSG1_HIGH("REL8:IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r8,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("REL8:SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r8,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("REL8:IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.t == T_rrc_InterFreqReportCriteria_r7_intraFreqReportingCriteria)
    {
      WRRC_MSG0_ERROR("REL8:IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);


    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
       rrc_InterFrequencyMeasurement_r8,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
         
         l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
         l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }
      else
      {
        /* Since L1 does not support this, reject the MCM */
       
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.t
      == T_rrc_InterFreqReportCriteria_r7_interFreqReportingCriteria) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
       u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria_r6,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n > idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2f))
          {
             WRRC_MSG0_ERROR("REL8:IFreq:Ifreq Set update not set and evt id != 2d or 2f ");
             rrcmeas_mcf.rrc_transaction_id = transaction_id;

             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
             rrc_free(l1_inter_freq_meas_params);
             return FAILURE;
          }
          idx++;
        }
      }
    }


    /* Process Reporting Criteria which is mandatory in case meas obj is present */

    if (rrcifreq_process_reporting_criteria_r7(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */

      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }



    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
    {
      
     /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    result = rrcifreq_fill_inter_freq_info_r8(&rrc_mcm_ptr->measurementCommand.u.setup->u.
      interFrequencyMeasurement->interFreqCellInfoList,
      l1_inter_freq_meas_params) ;
    /* Fill cell list, cell info */
    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
     rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("REL8:IFreq:SFN-ind set as True");
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
       rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement_r8,measurementValidity))
    {
      
      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
    {

      WRRC_MSG0_ERROR("REL8:IFreq:Unable to set MeasId in DB");
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      MSG_LOW("REL8:IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r8_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("REL8:IFREQ:Modify:InterFreq:MType absent");

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }
          rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          MSG_LOW("REL8:IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
        }
      } /* End of if of Additional Meas Present */

    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r8,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r8,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }
      if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.t  == T_rrc_InterFreqReportCriteria_r7_intraFreqReportingCriteria) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
         rrc_IntraFreqReportingCriteria_r7,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("REL8:IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
         rrc_InterFrequencyMeasurement_r8,interFreqSetUpdate))
      {
        if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
        {
           
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }

        else
        {
          /* Since L1 does not support this, reject the MCM */
          
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }


      /* Process Reporting Criteria */

      if (rrcifreq_process_reporting_criteria_r7(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
      {

        if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.t == T_rrc_InterFreqReportCriteria_r7_interFreqReportingCriteria) &&
         !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
         (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0)))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);

      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      {
        

        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
               addtl_meas_incl, INTER_FREQ, rrc_mcm_ptr->measurementIdentity,
               &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;

          }

        }
      } /* End of if of Additional Meas Present */
      else
      {
        WRRC_MSG0_HIGH("REL8:IFREQ:additional meas list absent");
      }

      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info_r8(
        &rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        interFreqCellInfoList, l1_inter_freq_meas_params);
      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
                /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("REL8:IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement_r8,measurementValidity))
      {
       
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity) == FALSE)
      {
        WRRC_MSG0_ERROR("REL8:IFreq:Unable to set MeasId in DB");
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        MSG_LOW("REL8:IFREQ:Commit Addtl Meas to RRC DB",0,0,0);
      }

    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
   
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  rrc_free(l1_inter_freq_meas_params);
  return SUCCESS;
}

/*===========================================================================
FUNCTION  rrcifreq_fill_inter_freq_info_r8

DESCRIPTION
 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 8

DEPENDENCIES
 None

RETURN VALUE
 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS
 None
===========================================================================*/
rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r8
(
 rrc_InterFreqCellInfoList_r8* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
)
{
  inter_freq_removed_cell_struct_type cascaded_rem_cell[L1_MAX_CELL_MEAS];

  uint32 inter_freq_cell_id = 0;

  uint16 dl_freq = 0;

  uint16 psc = 0;

  uint32 cell_position = 0;

  uint16 psc_to_measure = INVALID_PSC;
  uint16 freq_to_measure = INVALID_INTER_FREQ;
  uint8 meas_count = 0;
  uint32 cell_id = 0,idx=0;


  uint8 cell_count = 0, removed_cell_count = 0, psc_removed_count = 0;

  rrc_RemovedInterFreqCellList_removeSomeInterFreqCells* local_rm_inter_freq_cell_ptr = NULL;

  struct rrc_CellsForInterFreqMeasList *cellsForInterFreqMeasList_ptr = NULL;
  boolean valid_cell_id_found = FALSE;


  rrc_NewInterFreqCellList_r8* local_network_new_cell_list_ptr = NULL;


  /* Declare a local var to store the inter freq object */

  l1_inter_freq_meas_ctrl_struct_type* l1_inter_freq_meas_ptr = NULL;

  l1_inter_freq_meas_ptr = &l1_meas_ptr->meas_object.u.inter_f;

  /* Copy Inter Freq CELL_INFO_LIST to a local var. If failure in the processing of
  CELL_INFO_LIST, copy  inter_freq_cell_list_tmp back to inter_freq_cell_list*/
  for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
  {
    WCDMA_MEMCPY(&inter_freq_cell_list_tmp[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type),
                 &inter_freq_cell_list[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type));

    cascaded_rem_cell[cell_count].freq = INVALID_INTER_FREQ;
    cascaded_rem_cell[cell_count].psc = INVALID_PSC;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,rrc_InterFreqCellInfoList_r8,
    removedInterFreqCellList))
  {
    WRRC_MSG1_HIGH("REL8:IFreq:Remove:Delete [1: ALL  2: SOME  3:NO ]:- %d cells selected",interFreqCellInfoList_ptr->removedInterFreqCellList.t);
    switch (interFreqCellInfoList_ptr->removedInterFreqCellList.t)
    {
    case T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells:
      
      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv = 0xff;


      /* Update Cell_info_list variable */
      for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
      {
        inter_freq_cell_list[cell_count].cell_position = VACANT;
        inter_freq_cell_list[cell_count].psc = INVALID_PSC;
        inter_freq_cell_list[cell_count].psc = INVALID_INTER_FREQ;
      }

      break;

    case T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells:

      if ((interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells == NULL )||
        (interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells->n == 0 ))
      {
        break;
      }
      
      /* Store head of Link list */
      local_rm_inter_freq_cell_ptr =  interFreqCellInfoList_ptr->
        removedInterFreqCellList.u.removeSomeInterFreqCells;
      do
      {
        inter_freq_cell_id = local_rm_inter_freq_cell_ptr->elem[idx];

        /* Find the psc, dl freq as per given cell id from CELL_INFO_LIST */
        if ( rrcifreq_find_psc_freq(
          inter_freq_cell_id, &psc, &dl_freq) == PSC_ABSENT)
        {
          WRRC_MSG0_HIGH("REL8:IFreq:Retain previous cell list");

          for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
          {
            /* Retain the old CELL LIST */
            WCDMA_MEMCPY(&inter_freq_cell_list[cell_count], 
                         sizeof(inter_freq_cell_list_struct_type),
                         &inter_freq_cell_list_tmp[cell_count],
                         sizeof(inter_freq_cell_list_struct_type));
          }

          return RRCMEAS_FAIL;
        }
        else
        {
          /* First remove cell from inter freq cell list */
          rrcifreq_remove_cellid(inter_freq_cell_id);

          /* Here one needs to put psc, dl freq so that L1 can remove it */
          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc = psc;

          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].freq = dl_freq;

          removed_cell_count++; /* Increment count */
        }

        /*  Get next element in the list */

        idx++;

      } /* Continue if while cond satisfied */

      while ( (local_rm_inter_freq_cell_ptr->n > idx) &&
        (removed_cell_count < L1_MAX_CELL_MEAS) );


      /* Now store the number of actual cells sent by UTRAN */

      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv =  removed_cell_count;
      WRRC_MSG1_HIGH("REL8:IFreq:Removed cell cnt = %d cells", removed_cell_count);

      break;

    case T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells:
      /* Already set removed cells to 0 */
      
      break;

    default:
      
      break;
    } /* End of switch */

  } /* End of if of removed inter freq cells present */

  /* Reinitialize these variables */
  cell_count = 0;
  psc = 0;

  /* Check if new cells present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    rrc_InterFreqCellInfoList_r8,newInterFreqCellList))
  {
    

    /* Do not check for cell order. If UTRAN messes up, its their problem */
    idx=0;
    /* Save the Head pointer of linked List */
    local_network_new_cell_list_ptr = &interFreqCellInfoList_ptr->newInterFreqCellList;

    while ( ( local_network_new_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {
    /* if SFN-ind is presnt then return as failure*/
      if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.t == T_rrc_CellInfo_r4_modeSpecificInfo_fdd)
      {
        if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.u.fdd->readSFN_Indicator ==TRUE)
        {
          return SFN_FAIL;
        }
      }
    /* Pointer to next cell is Non-NULL and cell count
      * hasn't exceeded the max. L1 can handle */

      if ((local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.t ==
        T_rrc_CellInfo_r4_modeSpecificInfo_tdd) ||
        (!(local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.u.fdd->m.primaryCPICH_InfoPresent)))
      {
        MSG_LOW("REL8:IFreq:Ignore TDD cell",0,0,0);

        idx++;
        continue;

      }

      /* Fill in Cell Info */
      if (rrcmeas_fill_mcm_cell_info_r5(
        &local_network_new_cell_list_ptr->elem[idx].cellInfo,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info) == FAILURE)
      {
        MSG_LOW("REL8:IFreq:Ignoring Cell",0,0,0);
        idx++;
        continue;
      }

      /* Check if cell id present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
        rrc_NewInterFreqCell_r8,interFreqCellID))
      {
        cell_position = local_network_new_cell_list_ptr->elem[idx].interFreqCellID;

        WRRC_MSG1_MED("REL8:IFreq:Add:Cell at pos %d to be added",cell_position);

      } /*  End of if of inter-freq cell id present */
      else
      {
        WRRC_MSG0_HIGH("REL8:IFREQ:Add: Cell Id not present");
        cell_position = AVAILABLE_CELL_POSITION;
      }

      /* Check if freq present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
        rrc_NewInterFreqCell_r8,frequencyInfo))
      {
        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
          T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
        {
          MSG_LOW("REL8:IFreq:Ignore Cell. TDD cell",0,0,0);
          idx++;
          continue;
        }

        if (validate_frequency_info(&local_network_new_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
        {
          return RRCMEAS_UNSUPPORTED;
        }


        if (local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
        {
          idx++;
          continue;
        }

        /* Store DL freq */
        dl_freq = l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          freq_info.uarfcn_dl = (uint16) local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

        /* Calculate the UL Freq */
        /* Store the UL Freq */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
          uarfcn_ul_incl = FALSE;

        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
          uarfcn_ULPresent)
        {
          

          if (local_network_new_cell_list_ptr->elem[idx].
            frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
          {
            idx++;
            continue;
          }
          /* Store the UL Freq */
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
            uarfcn_ul_incl = TRUE;

          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
            freq_info.uarfcn_ul = (uint16)
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
        } /* End of up freq present */
      } /*  End of if of inter-freq freq present */
      else
      {
        if (cell_count == 0)
        {
          WRRC_MSG0_HIGH("REL8:IFreq:Ignore Cell. DL Freq absent");
          idx++;
          continue;
        }
        else
        {
          /* This implies that there is already one DL and UL freq present. Copy from there */
          WCDMA_MEMCPY(&l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info,
                       sizeof(l1_freq_info_struct_type),
                       &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count-1].freq_info,
                       sizeof(l1_freq_info_struct_type));
        }
      }

      if ( rrcifreq_update_inter_freq_cell_list_mcm(
        &cell_position,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info.pri_scr_code, &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.uarfcn_dl,
        &psc, &dl_freq) == RRCMEAS_SUCCESS)
      {
        /* PSC sucessfully stored in the cell info list variable */
        /* Now store psc in the mcm in add cell for layer1 to use psc */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].psc =
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          cell_info.pri_scr_code;

        /* This flag is always true for MCM */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

        if ((psc_removed_count < L1_MAX_CELL_MEAS) && (psc != INVALID_PSC) && (dl_freq != INVALID_INTER_FREQ))
        {
          /* UTRAN wants us to delete this psc/freq. Result of CASCADING effect */
          cascaded_rem_cell[psc_removed_count].psc = psc;

          cascaded_rem_cell[psc_removed_count++].freq = dl_freq;
        }

        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }  /* End of if of rrcmeas_update_inter_freq_cell_list */

      /* Read next element */
     idx++;

    } /* End of while newIntraFreqCellList != NULL */

    /* Initialize no of added cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_add = cell_count;

    for (cell_count = 0; cell_count < psc_removed_count; cell_count++)
    {
      if (removed_cell_count >= L1_MAX_CELL_MEAS)
      {
        /* Preventive check */
        WRRC_MSG0_HIGH("REL8:IFREQ:Unable to delete all cells");
        break;
      }
      /* Add this removed cell to the already built removed cell list for L1 */
      WRRC_MSG1_HIGH("REL8:IFREQ:Cascad:L1 removed cell list upd %d psc",
        cascaded_rem_cell[cell_count].psc);
      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc =
        cascaded_rem_cell[cell_count].psc;

      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count++].freq =
        cascaded_rem_cell[cell_count].freq;

    }
    /* Now update the number of removed cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_rmv += psc_removed_count;

  } /* End of if for new inter freq cells present */

  /* Initialize no fo cells to 0 */
  l1_inter_freq_meas_ptr->cell_list.num_cell_meas = 0;
  cell_count = 0;
  l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = FALSE;
  /* Check if cells for meas present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    & rrc_InterFreqCellInfoList_r8,cellsForInterFreqMeasList))
  {
    
    l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = TRUE;
    idx=0;
   
    cellsForInterFreqMeasList_ptr = &interFreqCellInfoList_ptr->cellsForInterFreqMeasList;
    meas_count = 0;
    while (cellsForInterFreqMeasList_ptr->n > idx)
    {

      cell_id = cellsForInterFreqMeasList_ptr->elem[idx];
      /* Check if the cell id is present in intra_freq_cell_list VARIABLE */
      if ( rrcifreq_find_psc_freq(cell_id, &psc_to_measure, &freq_to_measure) == PSC_ABSENT)
      {
         WRRC_MSG1_ERROR("Invalid cell to measure with id = %d. Ignoring cell id",cell_id);

      } /* End of if for psc not found */
      else
      {
        valid_cell_id_found = TRUE;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count].psc = psc_to_measure;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count++].dl_freq = freq_to_measure;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {

       WRRC_MSG0_HIGH("None of the cell id is valid .. REL8:Retain previous cell list before MCM rcved");

       WCDMA_MEMCPY(inter_freq_cell_list, 
                    sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS,
                    inter_freq_cell_list_tmp,
                    sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS);
       return RRCMEAS_UNSUPPORTED;
    }
    l1_inter_freq_meas_ptr->cell_list.num_cell_meas =  meas_count;

  } /* End of if of rrc_cellsForInterFreqMeasList) */

  return RRCMEAS_SUCCESS;
}

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================

FUNCTION   RRCIFREQ_CONFIG_INTER_FREQ_DB_R9

DESCRIPTION

  Sets Inter Freq Reporting Criteria in RRC Measurement Database
  from Measurement Control Message for Release 9 after the validation of
  the message has been completed

DEPENDENCIES

  None

RETURN VALUE

  Boolean

SIDE EFFECTS

  None
===========================================================================*/

boolean rrcifreq_config_inter_freq_db_r9
(
 l1_meas_ctrl_parm_struct_type* l1_mcm_ptr,
 rpt_crit_enum_type* rpt_crit_ptr,
 meas_validity_enum_type m_validity,
 rrc_MeasurementControl_r9 *mcm_r9
)
{
  uint16 count = 0;
  uint8 meas_id_val = 0;
  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;
  
  /* First check if the meas id already exists */

  /* SETUP existing & MODIFY scenario */
  for ( count = 0; count < MAX_INTER_FREQ_MEAS; count++ )
  {
    if (inter_freq_meas_id_list[count].meas_id == l1_mcm_ptr->meas_id)
    {
      WRRC_MSG1_HIGH("IFreq:Meas Id = %d found in RRC db",l1_mcm_ptr->meas_id);

      MSG_LOW("IFREQ:Before commiting ..IFREQ:Id %d, Val %d, Owner %d", inter_freq_meas_id_list[count].meas_id,
        inter_freq_meas_id_list[count].meas_validity,
        inter_freq_meas_id_list[count].mcm_owned);

      WRRC_MSG1_HIGH("IFREQ:Meas Id = %d found in RRC db",l1_mcm_ptr->meas_id);

      /* If a setup, always overwrite validity */
      if (l1_mcm_ptr->meas_cmd == L1_MEAS_SETUP)
      {
        /* Set Validity */
        inter_freq_meas_id_list[count].meas_validity = m_validity;
      }
      /* For MODIFY scenario, when validity not specified */
      else if ((l1_mcm_ptr->meas_cmd == L1_MEAS_MODIFY) &&
        (l1_mcm_ptr->meas_object_info_included == TRUE) &&
        (m_validity == NOT_PRESENT))
      {
        WRRC_MSG1_HIGH("IFREQ:Retain Current Validity %d", inter_freq_meas_id_list[count].meas_validity);
      }
      /* UTRAN specified Meas Validity with a Modify */
      else if ((l1_mcm_ptr->meas_cmd == L1_MEAS_MODIFY) &&
        (l1_mcm_ptr->meas_object_info_included == TRUE) &&
        (m_validity != NOT_PRESENT))
      {
        WRRC_MSG2_HIGH("IFREQ:Overwrite Current Validity %d with %d",
          inter_freq_meas_id_list[count].meas_validity,m_validity);

        inter_freq_meas_id_list[count].meas_validity = m_validity;
      }
      else /* To satisfy LINT */
      {
        /* Normal processing */
      }

      /* MCM in the owner of this Meas */
      inter_freq_meas_id_list[count].mcm_owned = TRUE;

      /* Set Report Criteria */
      inter_freq_meas_id_list[count].rpt_crit = *rpt_crit_ptr;
      WRRC_MSG1_HIGH("IFreq:Meas Crit %d saved in RRC", inter_freq_meas_id_list[count].rpt_crit);


      MSG_HIGH("IFREQ:After commiting .. IFREQ:Id %d, Val %d, Owner %d", inter_freq_meas_id_list[count].meas_id,
        inter_freq_meas_id_list[count].meas_validity,
        inter_freq_meas_id_list[count].mcm_owned);

      return TRUE;
    }

  } /* End of for loop */
  WRRC_MSG1_HIGH("IFreq:New Meas Id = %d configured",l1_mcm_ptr->meas_id);

  /* SETUP not existing scenario */
  for ( count = 0; count < MAX_INTER_FREQ_MEAS; count++ )
  {
    /* If Meas Id not found, then it is a new Meas. Set it in first available position */
    if (inter_freq_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Setting Meas Id = %d in RRC db, Before commiting Id %d, Val %d, Owner %d",
            l1_mcm_ptr->meas_id,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
      {
        inter_freq_meas_id_list[count].meas_id = meas_id_val;

        /* Set this only if extended measurements is supported */
        /* This needs to be set while sending measurement report */
        inter_freq_meas_id_list[count].meas_id_legacy = rrc_mcm_ptr->measurementIdentity;
      }
      else
      {
        inter_freq_meas_id_list[count].meas_id = rrc_mcm_ptr->measurementIdentity;
      }


      inter_freq_meas_id_list[count].meas_validity = m_validity;

      inter_freq_meas_id_list[count].mcm_owned = TRUE;

      /* Set Report Criteria */
      inter_freq_meas_id_list[count].rpt_crit = *rpt_crit_ptr;

      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "IFreq:Meas Crit %d saved in RRC, After commiting Id %d, Val %d, Owner %d",
            inter_freq_meas_id_list[count].rpt_crit,inter_freq_meas_id_list[count].meas_id,
            inter_freq_meas_id_list[count].meas_validity,
            inter_freq_meas_id_list[count].mcm_owned);

      return TRUE;
    }

  } /* End of for loop */

  
  return FALSE;

}

/*===========================================================================
FUNCTION  rrcifreq_fill_inter_freq_info_r9

DESCRIPTION
 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 8

DEPENDENCIES
 None

RETURN VALUE
 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS
 None
===========================================================================*/
rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r9
(
 rrc_InterFreqCellInfoList_r9* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
)
{
  inter_freq_removed_cell_struct_type cascaded_rem_cell[L1_MAX_CELL_MEAS];

  uint32 inter_freq_cell_id = 0;

  uint16 dl_freq = 0;

  uint16 psc = 0;

  uint32 cell_position = 0;

  uint16 psc_to_measure = INVALID_PSC;
  uint16 freq_to_measure = INVALID_INTER_FREQ;
  uint8 meas_count = 0;
  uint32 cell_id = 0,idx=0;


  uint8 cell_count = 0, removed_cell_count = 0, psc_removed_count = 0;

  rrc_RemovedInterFreqCellList_removeSomeInterFreqCells* local_rm_inter_freq_cell_ptr = NULL;

  struct rrc_CellsForInterFreqMeasList *cellsForInterFreqMeasList_ptr = NULL;
  boolean valid_cell_id_found = FALSE;
 


  rrc_NewInterFreqCellList_r9* local_network_new_cell_list_ptr = NULL;


  /* Declare a local var to store the inter freq object */

  l1_inter_freq_meas_ctrl_struct_type* l1_inter_freq_meas_ptr = NULL;

  l1_inter_freq_meas_ptr = &l1_meas_ptr->meas_object.u.inter_f;

  /* Copy Inter Freq CELL_INFO_LIST to a local var. If failure in the processing of
  CELL_INFO_LIST, copy  inter_freq_cell_list_tmp back to inter_freq_cell_list*/
  for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
  {
    WCDMA_MEMCPY(&inter_freq_cell_list_tmp[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type),
                 &inter_freq_cell_list[cell_count], 
                 sizeof(inter_freq_cell_list_struct_type));

    cascaded_rem_cell[cell_count].freq = INVALID_INTER_FREQ;
    cascaded_rem_cell[cell_count].psc = INVALID_PSC;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,rrc_InterFreqCellInfoList_r9,
    removedInterFreqCellList))
  {
    WRRC_MSG1_HIGH("REL8:IFreq:Remove:Delete [1: ALL  2: SOME  3:NO ]:- %d cells selected",interFreqCellInfoList_ptr->removedInterFreqCellList.t);
    switch (interFreqCellInfoList_ptr->removedInterFreqCellList.t)
    {
    case T_rrc_RemovedInterFreqCellList_removeAllInterFreqCells:
      
      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv = 0xff;


      /* Update Cell_info_list variable */
      for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
      {
        inter_freq_cell_list[cell_count].cell_position = VACANT;
        inter_freq_cell_list[cell_count].psc = INVALID_PSC;
        inter_freq_cell_list[cell_count].psc = INVALID_INTER_FREQ;
      }

      break;

    case T_rrc_RemovedInterFreqCellList_removeSomeInterFreqCells:

      if ((interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells == NULL )||
        (interFreqCellInfoList_ptr->removedInterFreqCellList.u.removeSomeInterFreqCells->n == 0 ))
      {
        break;
      }
      
      /* Store head of Link list */
      local_rm_inter_freq_cell_ptr =  interFreqCellInfoList_ptr->
        removedInterFreqCellList.u.removeSomeInterFreqCells;
      do
      {
        inter_freq_cell_id = local_rm_inter_freq_cell_ptr->elem[idx];

        /* Find the psc, dl freq as per given cell id from CELL_INFO_LIST */
        if ( rrcifreq_find_psc_freq(
          inter_freq_cell_id, &psc, &dl_freq) == PSC_ABSENT)
        {
          WRRC_MSG0_HIGH("REL9:IFreq:Retain previous cell list");

          for (cell_count = 0; cell_count < L1_MAX_CELL_MEAS; cell_count++)
          {
            /* Retain the old CELL LIST */
            WCDMA_MEMCPY(&inter_freq_cell_list[cell_count], 
                         sizeof(inter_freq_cell_list_struct_type),
                         &inter_freq_cell_list_tmp[cell_count],
                         sizeof(inter_freq_cell_list_struct_type));
          }

          return RRCMEAS_FAIL;
        }
        else
        {
          /* First remove cell from inter freq cell list */
          rrcifreq_remove_cellid(inter_freq_cell_id);

          /* Here one needs to put psc, dl freq so that L1 can remove it */
          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc = psc;

          l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].freq = dl_freq;

          removed_cell_count++; /* Increment count */
        }

        /*  Get next element in the list */

        idx++;

      } /* Continue if while cond satisfied */

      while ( (local_rm_inter_freq_cell_ptr->n > idx) &&
        (removed_cell_count < L1_MAX_CELL_MEAS) );


      /* Now store the number of actual cells sent by UTRAN */

      l1_inter_freq_meas_ptr->cell_list.num_cell_rmv =  removed_cell_count;
      WRRC_MSG1_HIGH("REL9:IFreq:Removed cell cnt = %d cells", removed_cell_count);

      break;

    case T_rrc_RemovedInterFreqCellList_removeNoInterFreqCells:
      /* Already set removed cells to 0 */
      
      break;

    default:
      
      break;
    } /* End of switch */

  } /* End of if of removed inter freq cells present */

  /* Reinitialize these variables */
  cell_count = 0;
  psc = 0;

  /* Check if new cells present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    rrc_InterFreqCellInfoList_r9,newInterFreqCellList))
  {
    

    /* Do not check for cell order. If UTRAN messes up, its their problem */
    idx=0;
    /* Save the Head pointer of linked List */
    local_network_new_cell_list_ptr = &interFreqCellInfoList_ptr->newInterFreqCellList;

    while ( ( local_network_new_cell_list_ptr->n > idx) &&
      (cell_count < L1_MAX_CELL_MEAS) )
    {
    /* if SFN-ind is presnt then return as failure*/
      if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.t == T_rrc_CellInfo_r9_modeSpecificInfo_fdd)
      {
        if(local_network_new_cell_list_ptr->elem[idx].cellInfo.modeSpecificInfo.u.fdd->readSFN_Indicator ==TRUE)
        {
          return SFN_FAIL;
        }
      }
    /* Pointer to next cell is Non-NULL and cell count
      * hasn't exceeded the max. L1 can handle */

      if ((local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.t ==
        T_rrc_CellInfo_r9_modeSpecificInfo_tdd) ||
        (!(local_network_new_cell_list_ptr->elem[idx].
        cellInfo.modeSpecificInfo.u.fdd->m.primaryCPICH_InfoPresent)))
      {
        MSG_LOW("REL9:IFreq:Ignore TDD cell",0,0,0);

        idx++;
        continue;

      }

      /* Fill in Cell Info */
      if (rrcmeas_fill_mcm_cell_info_r9(
        &local_network_new_cell_list_ptr->elem[idx].cellInfo,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info) == FAILURE)
      {
        MSG_LOW("REL9:IFreq:Ignoring Cell",0,0,0);
        idx++;
        continue;
      }

      /* Check if cell id present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
        rrc_NewInterFreqCell_r9,interFreqCellID))
      {
        cell_position = local_network_new_cell_list_ptr->elem[idx].interFreqCellID;

        WRRC_MSG1_MED("REL9:IFreq:Add:Cell at pos %d to be added",cell_position);

      } /*  End of if of inter-freq cell id present */
      else
      {
        WRRC_MSG0_HIGH("REL9:IFREQ:Add: Cell Id not present");
        cell_position = AVAILABLE_CELL_POSITION;
      }

      /* Check if freq present */
      if (RRC_MSG_LIST_BITMASK_IE(local_network_new_cell_list_ptr,
        rrc_NewInterFreqCell_r9,frequencyInfo))
      {
        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.t ==
          T_rrc_FrequencyInfo_modeSpecificInfo_tdd)
        {
          MSG_LOW("REL9:IFreq:Ignore Cell. TDD cell",0,0,0);
          idx++;
          continue;
        }

        if (validate_frequency_info(&local_network_new_cell_list_ptr->elem[idx].frequencyInfo) == FAILURE)
        {
          return RRCMEAS_UNSUPPORTED;
        }


        if (local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL > FDD_ARFCN)
        {
          idx++;
          continue;
        }

        /* Store DL freq */
        dl_freq = l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          freq_info.uarfcn_dl = (uint16) local_network_new_cell_list_ptr->elem[idx].
          frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;

        /* Calculate the UL Freq */
        /* Store the UL Freq */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
          uarfcn_ul_incl = FALSE;

        if (local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->m.
          uarfcn_ULPresent)
        {
          

          if (local_network_new_cell_list_ptr->elem[idx].
            frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL > FDD_ARFCN)
          {
            idx++;
            continue;
          }
          /* Store the UL Freq */
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.
            uarfcn_ul_incl = TRUE;

          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
            freq_info.uarfcn_ul = (uint16)
            local_network_new_cell_list_ptr->elem[idx].frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_UL;
        } /* End of up freq present */
      } /*  End of if of inter-freq freq present */
      else
      {
        if (cell_count == 0)
        {
          WRRC_MSG0_HIGH("REL9:IFreq:Ignore Cell. DL Freq absent");
          idx++;
          continue;
        }
        else
        {
          /* This implies that there is already one DL and UL freq present. Copy from there */
          WCDMA_MEMCPY(&l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info,
                       sizeof(l1_freq_info_struct_type),
                       &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count-1].freq_info,
                       sizeof(l1_freq_info_struct_type));
        }
      }

      if ( rrcifreq_update_inter_freq_cell_list_mcm(
        &cell_position,
        &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
        cell_info.pri_scr_code, &l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].freq_info.uarfcn_dl,
        &psc, &dl_freq) == RRCMEAS_SUCCESS)
      {
        /* PSC sucessfully stored in the cell info list variable */
        /* Now store psc in the mcm in add cell for layer1 to use psc */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].psc =
          l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].
          cell_info.pri_scr_code;

        /* This flag is always true for MCM */
        l1_inter_freq_meas_ptr->cell_list.add_cell[cell_count].measure_in_idle_pch_fach = TRUE;

        if ((psc_removed_count < L1_MAX_CELL_MEAS) && (psc != INVALID_PSC) && (dl_freq != INVALID_INTER_FREQ))
        {
          /* UTRAN wants us to delete this psc/freq. Result of CASCADING effect */
          cascaded_rem_cell[psc_removed_count].psc = psc;

          cascaded_rem_cell[psc_removed_count++].freq = dl_freq;
        }

        /* Increment cell count as add cell successfully stored */
        cell_count ++;
      }  /* End of if of rrcmeas_update_inter_freq_cell_list */

      /* Read next element */
     idx++;

    } /* End of while newIntraFreqCellList != NULL */

    /* Initialize no of added cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_add = cell_count;

    for (cell_count = 0; cell_count < psc_removed_count; cell_count++)
    {
      if (removed_cell_count >= L1_MAX_CELL_MEAS)
      {
        /* Preventive check */
        WRRC_MSG0_HIGH("REL9:IFREQ:Unable to delete all cells");
        break;
      }
      /* Add this removed cell to the already built removed cell list for L1 */
      WRRC_MSG1_HIGH("REL9:IFREQ:Cascad:L1 removed cell list upd %d psc",
        cascaded_rem_cell[cell_count].psc);
      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count].psc =
        cascaded_rem_cell[cell_count].psc;

      l1_inter_freq_meas_ptr->cell_list.rmv_cell[removed_cell_count++].freq =
        cascaded_rem_cell[cell_count].freq;

    }
    /* Now update the number of removed cells */
    l1_inter_freq_meas_ptr->cell_list.num_cell_rmv += psc_removed_count;

  } /* End of if for new inter freq cells present */

  /* Initialize no fo cells to 0 */
  l1_inter_freq_meas_ptr->cell_list.num_cell_meas = 0;
  cell_count = 0;
  l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = FALSE;
  /* Check if cells for meas present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(interFreqCellInfoList_ptr,
    & rrc_InterFreqCellInfoList_r9,cellsForInterFreqMeasList))
  {
    
    l1_inter_freq_meas_ptr->cell_list.cells_for_meas_incl = TRUE;
    idx=0;
   
    cellsForInterFreqMeasList_ptr = &interFreqCellInfoList_ptr->cellsForInterFreqMeasList;
    meas_count = 0;
    while (cellsForInterFreqMeasList_ptr->n > idx)
    {

      cell_id = cellsForInterFreqMeasList_ptr->elem[idx];
      /* Check if the cell id is present in intra_freq_cell_list VARIABLE */
      if ( rrcifreq_find_psc_freq(cell_id, &psc_to_measure, &freq_to_measure) == PSC_ABSENT)
      {
        WRRC_MSG1_ERROR("REL9:Invalid cell to measure with id = %d",
          cell_id);

      } /* End of if for psc not found */
      else
      {
        valid_cell_id_found=TRUE;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count].psc = psc_to_measure;
        l1_inter_freq_meas_ptr->cell_list.meas_cells_list[meas_count++].dl_freq = freq_to_measure;
      }

      /* Increment Ptr */
      idx++;
    } /* End of while */
    if(!valid_cell_id_found)
    {
      WRRC_MSG0_HIGH("None of the cell id is valid .. REL9:Retain previous cell list before MCM rcved");

      WCDMA_MEMCPY(inter_freq_cell_list,
                   sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS, 
                   inter_freq_cell_list_tmp,
                   sizeof(inter_freq_cell_list_struct_type) * L1_MAX_CELL_MEAS);
      return RRCMEAS_UNSUPPORTED;
    }

    l1_inter_freq_meas_ptr->cell_list.num_cell_meas =  meas_count;

  } /* End of if of rrc_cellsForInterFreqMeasList) */

  return RRCMEAS_SUCCESS;
}



/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R9

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r9
(
 rrc_InterFreqReportCriteria_r9* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
)
{
  switch(ifreq_rpt_criteria_ptr->t)
  {
  case T_rrc_InterFreqReportCriteria_r9_interFreqReportingCriteria:


    l1_rpt_params_ptr->u.inter_rpt_crit.num_event = 0;

    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
      & rrc_InterFreqReportingCriteria_r6,interFreqEventList)) &&
      (ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria->interFreqEventList.n != 0))
    {
      if (rrcifreq_process_event_criteria_r6(ifreq_rpt_criteria_ptr->u.interFreqReportingCriteria,
        &l1_rpt_params_ptr->u.inter_rpt_crit) == FAILURE)
      {
        return FAILURE;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("InterFreq Event List Empty");
      return FAILURE;
    }

    /* Inter-Freq Events successfully processed */
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_EVT_TRIG;

    break;

  case T_rrc_InterFreqReportCriteria_r9_intraFreqReportingCriteria :

    WRRC_MSG0_HIGH("IFreq: Intra Freq Event Rpt Crit. Setting rpt crit to L1_INTER_FREQ_NO_CHANGE");
    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_CHANGE;
    break;

  case T_rrc_InterFreqReportCriteria_r9_periodicalReportingCriteria :

    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_PERIODIC;


    l1_rpt_params_ptr->rpt_cell_status_incl = FALSE;

    /* Set reporting cells */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria,
      rrc_PeriodicalWithReportingCellStatus,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.
        periodicalReportingCriteria->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
      else
      {
        return FAILURE;
      }
    }

    /* Check if reporting amount present */
 
      /* Store reporting amount */
      l1_rpt_params_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type)
        RRCMEAS_RET_REPORTING_AMOUNT(
        ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingAmount);
    
    /* The spec indicates that 0 means UE behaviour unspecified */
    if (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->
      periodicalReportingCriteria.reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
      WRRC_MSG0_ERROR("Periodic Rpting 0 specified. use 250");
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS;
    }
    else
    {
      /* Store reporting interval */
      l1_rpt_params_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL
        (ifreq_rpt_criteria_ptr->u.periodicalReportingCriteria->periodicalReportingCriteria.reportingInterval);
    }

    WRRC_MSG2_HIGH("IFreq:Rpting Amt is %d, Infin:0 Rpting Intvl is %d msec", l1_rpt_params_ptr->u.periodic_crit.rpt_amount, 
             l1_rpt_params_ptr->u.periodic_crit.rpt_interval);
    break; /* Come out of switch */

  case T_rrc_InterFreqReportCriteria_r9_noReporting :


    l1_rpt_params_ptr->rpt_mode = L1_INTER_FREQ_NO_REPORTING;

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ifreq_rpt_criteria_ptr->u.noReporting,
        rrc_ReportingCellStatusOpt,reportingCellStatus))
    {
      if (rrcmeas_fill_no_of_reporting_cells(&ifreq_rpt_criteria_ptr->u.noReporting->reportingCellStatus,
        &l1_rpt_params_ptr->rpt_cell_status) == SUCCESS)
      {
        l1_rpt_params_ptr->rpt_cell_status_incl = TRUE;

        WRRC_MSG0_HIGH("IFREQ:Rpting Crit is Addtl");

        /* Check if reporting cell status t is valid */
        if ((l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ) ||
          (l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat == L1_RPT_ASET))
        {
          WRRC_MSG1_ERROR("IFreq:Invalid cell status %d", l1_rpt_params_ptr->rpt_cell_status.rpt_cell_stat);
          return FAILURE;
        }
      }
    }
    break;



  default:
    
    return FAILURE;

  }
  return SUCCESS;

}




/*===========================================================================
FUNCTION   RRCIFREQ_PROCESS_MCM_R9

DESCRIPTION
    This function processes the release 9 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES
    None.

RETURN VALUE
    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS
    None
==========================================================================*/
uecomdef_status_e_type rrcifreq_process_mcm_r9
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
)
{
  struct rrc_InterFreqEventList_r6 * ifreq_event_list_ptr = NULL;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_inter_freq_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  uint32 idx=0;
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  rrcmeas_status_e_type result;

  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;

  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTER_FREQ;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 count = 0;

  uint8 cnt =  0;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;

  /* Assign default values to l1_inter_freq_meas_params */
  l1_inter_freq_meas_params->dpch_cm_status_incl = FALSE;

  l1_inter_freq_meas_params->meas_object_info_included = FALSE;
  l1_inter_freq_meas_params->num_add_meas = 0;
  l1_inter_freq_meas_params->rpt_mode_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.meas_type = L1_INTER_FREQ;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_add = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_meas = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.num_cell_rmv = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list.cells_for_meas_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_cell_status_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = FALSE;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_add_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.num_rmv_radio_link = 0;
  l1_inter_freq_meas_params->meas_object.u.inter_f.update.radio_link_info_incl = 0;

  l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = FALSE;

    /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  mcm_r9 = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.later_r8_criticalExtensions.u.r9;


  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;

  /* Set Transaction Id and Meas Id */
  l1_inter_freq_meas_params->trans_id = transaction_id;

  /* Check if extended measurement is supported. If meas id extension is given then
     override the meas id given in legacy IE */
  if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
  {
    l1_inter_freq_meas_params->meas_id = meas_id_val;
  }
  else
  {
    l1_inter_freq_meas_params->meas_id  = (uint16) rrc_mcm_ptr->measurementIdentity;;
  }


  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {

    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_inter_freq_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("REL9:IFREQ:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    /* Set compressed Mode as TRUE */
    l1_inter_freq_meas_params->dpch_cm_status_incl = TRUE;

  } /* End of if of compressed mode processing */


  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r9_setup)
  {

    /* Double check that it is INTER-FREQ Meas */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t !=
        T_rrc_MeasurementType_r9_interFrequencyMeasurement)
    {
      WRRC_MSG1_HIGH("REL9:IFREQ:UnSupported Meas %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    /* Check Measurement Quantity present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r9,interFreqMeasQuantity)))
    {
      WRRC_MSG0_ERROR("REL9:SETUP:MeasQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    l1_inter_freq_meas_params->meas_object_info_included = TRUE;

    if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.setup->
      u.interFrequencyMeasurement->interFreqMeasQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
    {
      /* Set Meas Quantity to TRUE */
      l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
    }
    else
    {
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Check Reporting Quantity */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r9,interFreqReportingQuantity)))
    {
      WRRC_MSG0_ERROR("REL9:IFreq:SETUP:RptQuan absent");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }
    if(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.t == T_rrc_InterFreqReportCriteria_r9_intraFreqReportingCriteria)
    {
      WRRC_MSG0_ERROR("REL9:IFreq:SETUP:inter freq rpt criteria set to Intra freq rpt criteria");

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    /* Set Reporting quantity */
    rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqReportingQuantity,
      &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


    l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;

    /* Store reporting mode to TRUE */
    l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

    l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);

    /* Check if update present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
       rrc_InterFrequencyMeasurement_r9,interFreqSetUpdate))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
      {
         
         l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
         l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
      }
      else
      {
        /* Since L1 does not support this, reject the MCM */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
    }
    if((rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.t
      == T_rrc_InterFreqReportCriteria_r9_interFreqReportingCriteria) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->reportCriteria.
       u.interFreqReportingCriteria,rrc_InterFreqReportingCriteria_r6,interFreqEventList)))
    {
      ifreq_event_list_ptr = &rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria.u.interFreqReportingCriteria->interFreqEventList;

      if(!(l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl))
      {
        while(ifreq_event_list_ptr->n > idx)
        {
          if((ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2d )
            && (ifreq_event_list_ptr->elem[idx].t != T_rrc_InterFreqEvent_r6_event2f))
          {
             WRRC_MSG0_ERROR("REL9:IFreq:Ifreq Set update not set and evt id != 2d or 2f ");
             rrcmeas_mcf.rrc_transaction_id = transaction_id;

             /* Set faliure t */
             rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

             rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
             rrc_free(l1_inter_freq_meas_params);
             return FAILURE;
          }
          idx++;
        }
      }
    }


    /* Process Reporting Criteria which is mandatory in case meas obj is present */

    if (rrcifreq_process_reporting_criteria_r9(&rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement->
      reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
    {

      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set faliure t */

      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

      mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);
    }


    result = rrcifreq_fill_inter_freq_info_r9(&rrc_mcm_ptr->measurementCommand.u.setup->u.
      interFrequencyMeasurement->interFreqCellInfoList,
      l1_inter_freq_meas_params) ;
    /* Fill cell list, cell info */
    if (result == RRCMEAS_FAIL)
    {
      /* Send Meas Ctrl Failure Message */

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;

      /* Set failure cause  which is IE value incorrect */
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }
    else
    {
      if(result == SFN_FAIL)
      {
              /* Send Meas Ctrl Failure Message */
        WRRC_MSG0_ERROR("REL9:IFreq:SFN-ind set as True");
      /* Send Meas Ctrl Failure Msg */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

      /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      if (result == RRCMEAS_UNSUPPORTED)
      {
        /* Send Meas Ctrl Failure Message */
  
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
  
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;

    }

    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,
       primary_meas_type,l1_inter_freq_meas_params->meas_id, &meas_validity) == FAILURE)
    {
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;
    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.interFrequencyMeasurement,
      rrc_InterFrequencyMeasurement_r9,measurementValidity))
    {
      
      /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
      then assume it to be CELL_DCH. If network specifies validity to be other than CELL_DCH,
      the UE behaviour is unspecified. So we assume it to be CELL_DCH */
      meas_validity = STATE_DCH;
    }

    /* Now commit new meas params to RRC db.*/
    if (rrcifreq_config_inter_freq_db_r9(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity,mcm_r9) == FALSE)
    {

      WRRC_MSG0_ERROR("REL9:IFreq:Unable to set MeasId in DB");
      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      /* Make the old CELL_INFO_LIST valid */

      for (count = 0; count < L1_MAX_CELL_MEAS; count++)
      {
        WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                     sizeof(inter_freq_cell_list_struct_type),
                     &inter_freq_cell_list_tmp[count], 
                     sizeof(inter_freq_cell_list_struct_type));
      }
      rrc_free(l1_inter_freq_meas_params);
      return FAILURE;

    }

    /* Now commit addtl meas db */
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
        l1_inter_freq_meas_params->meas_id);
      WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
    }

    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_SETUP;

    /* Now send command to L1 */
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r9_modify)
  {
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_inter_freq_meas_params->rpt_mode_incl = TRUE;

      l1_inter_freq_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    /* Set type to MODIFY */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_MODIFY;


    /* Check if Meas Object Present */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r9_modify,measurementType)))
    {
      WRRC_MSG0_HIGH("REL9:IFREQ:Modify:InterFreq:MType absent");

      if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,
         primary_meas_type,l1_inter_freq_meas_params->meas_id,&meas_validity) == FAILURE)
      {
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }


      if (addtl_meas_incl== TRUE)
      {
        rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
          l1_inter_freq_meas_params->meas_id);
        WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
      }
    }
    else
    {
      /* Meas Type has already been verified */
      l1_inter_freq_meas_params->meas_object_info_included = TRUE;

      /* Check Measurement Quantity present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r9,interFreqMeasQuantity))
      {

        if (rrcifreq_process_meas_quantity(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.
          u.interFrequencyMeasurement->interFreqMeasQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan) == RRCMEAS_SUCCESS)
        {
          /* Set Meas Quantity to TRUE */
          l1_inter_freq_meas_params->meas_object.u.inter_f.meas_quan_incl = TRUE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;

        }
      } /* End of meas quantity present */

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
          rrc_InterFrequencyMeasurement_r9,interFreqReportingQuantity))
      {

        /* Set Reporting quantity */
        rrcifreq_fill_rpt_quan(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqReportingQuantity,
          &l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan);


        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_quan_incl = TRUE;
      }
      if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.t  == T_rrc_InterFreqReportCriteria_r9_intraFreqReportingCriteria) &&
        (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria.u.intraFreqReportingCriteria,
         rrc_IntraFreqReportingCriteria_r9,eventCriteriaList)))
      {
        WRRC_MSG0_ERROR("REL9:IFreq:Modify:rpt crit set to Intraf rpt crit and evnt list non null");

        /* Tr Id */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      /* Check if update present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
         rrc_InterFrequencyMeasurement_r9,interFreqSetUpdate))
      {
        if(RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->interFreqSetUpdate,rrc_UE_AutonomousUpdateMode,
        onWithNoReporting))
        {
           
           l1_inter_freq_meas_params->meas_object.u.inter_f.inter_freq_set_update_incl = TRUE;
           l1_inter_freq_meas_params->meas_object.u.inter_f.update.auto_update_mode = L1_SET_UPDATE_ON_NO_RPT;
        }

        else
        {
          /* Since L1 does not support this, reject the MCM */
         
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }


      /* Process Reporting Criteria */

      if (rrcifreq_process_reporting_criteria_r9(&rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        reportCriteria, &l1_inter_freq_meas_params->meas_object.u.inter_f) == FAILURE)
      {

        if((rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.t == T_rrc_InterFreqReportCriteria_r9_interFreqReportingCriteria) &&
         !((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria,
         rrc_InterFreqReportingCriteria_r9,interFreqEventList)) &&
         (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
         reportCriteria.u.interFreqReportingCriteria->interFreqEventList.n != 0)))
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;
          l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode = L1_INTER_FREQ_NO_CHANGE;
        }
        else
        {
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
      }
      else
      {
        l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode_incl = TRUE;

        mcm_rpt_crit = RET_INTER_FREQ_RPT_CRIT(l1_inter_freq_meas_params->meas_object.u.inter_f.rpt_mode);

      }

      /* Now check Additional Measurements */
      /* Process Additional Meas */
      if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,l1_inter_freq_meas_params->meas_id,&meas_validity) == FAILURE)
      {
            rrc_free(l1_inter_freq_meas_params);
            return FAILURE;
      }

      /* Fill cell list, cell info */
      result = rrcifreq_fill_inter_freq_info_r9(
        &rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement->
        interFreqCellInfoList, l1_inter_freq_meas_params);
      if (result == RRCMEAS_FAIL)
      {
        /* Send Meas Ctrl Failure Message */

        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }
      else
      {
        if(result == SFN_FAIL)
        {
                /* Send Meas Ctrl Failure Message */
          WRRC_MSG0_ERROR("REL9:IFreq:SFN-ind set as True");
        /* Send Meas Ctrl Failure Msg */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_inter_freq_meas_params);
          return FAILURE;
        }
        else
        {
          l1_inter_freq_meas_params->meas_object.u.inter_f.cell_list_incl = TRUE;
        }
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.interFrequencyMeasurement,
        rrc_InterFrequencyMeasurement_r9,measurementValidity))
      {
        
        /* No need to check value as by default validity is CELL_DCH and if it is not CELL_DCH,
        then assume it to be CELL_DCH. */
        meas_validity = STATE_DCH;
      }

      /* Now commit new meas params to RRC db.*/
      if (rrcifreq_config_inter_freq_db_r9(l1_inter_freq_meas_params, &mcm_rpt_crit, meas_validity,mcm_r9) == FALSE)
      {
        WRRC_MSG0_ERROR("REL9:IFreq:Unable to set MeasId in DB");
        rrcmeas_mcf.rrc_transaction_id = transaction_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

        /* Make the old CELL_INFO_LIST valid */

        for (count = 0; count < L1_MAX_CELL_MEAS; count++)
        {
          WCDMA_MEMCPY(&inter_freq_cell_list[count], 
                       sizeof(inter_freq_cell_list_struct_type),
                       &inter_freq_cell_list_tmp[count], 
                       sizeof(inter_freq_cell_list_struct_type));
        }
        rrc_free(l1_inter_freq_meas_params);
        return FAILURE;
      }

      /* Now commit addtl meas db */
      if (addtl_meas_incl == TRUE)
      {
        rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
          l1_inter_freq_meas_params->meas_id);
        MSG_LOW("REL9:Additional Meas set in RRC DB",0,0,0);
      }
    } /* End of else of meas type present */

  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    that this meas exists. So let us delete it from inter_freq_meas_id_list
    Meas Identity, Transaction Id already set */
    l1_inter_freq_meas_params->meas_cmd = L1_MEAS_RELEASE;

    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_inter_freq_meas_params->meas_id);

    /* Clear Meas Id from inter_freq_meas_id_list List */
    for (count=0; count < MAX_INTER_FREQ_MEAS; count++)
    {
      if (inter_freq_meas_id_list[count].meas_id == l1_inter_freq_meas_params->meas_id)
      {
        inter_freq_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        inter_freq_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        inter_freq_meas_id_list[count].mcm_owned = FALSE;
        inter_freq_meas_id_list[count].meas_validity = NOT_PRESENT;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          inter_freq_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    
    rrc_free(l1_inter_freq_meas_params);
    return SUCCESS;
  }

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_inter_freq_meas_params->trans_id = alloc_idx;

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  rrcmeas_send_l1_cmd(NULL, l1_inter_freq_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  rrc_free(l1_inter_freq_meas_params);

  return SUCCESS;
}

#endif

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

