/*===========================================================================
            R R C   M E A S U R E M E N T S

DESCRIPTION

   This module supports Measurement Control and Measurement Reporting
   for the RRC layer for UE Internal Measurements



EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

   RRC Task must have been started. All RRC command queues must
   have been initialized. All RRC signals must have been cleared
   at task startup.

Copyright (c) 2003-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

             EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcueimeas.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/14   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/28/13   sr      Added code for measurement identity extension
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp    Removed tmc.h include file
01/03/13   ad    Made changes to reduce the redundant F3s
10/23/12   pm    Made changes to assign pointer to NULL after de-allocation
05/03/11   rl    Fixed the lint error
02/07/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad    Added changes for RRC SW decoupling
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/07/09   gkg   Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss    Updated Copyright Information
11/21/08   rm    Made changes to unblock confguring L1 with Internal measurements.
                 Changes are under FEATURE_WCDMA_REL7
11/10/08   rm    Made changes to process Rel7 measurement control message
16/10/08   rm    Made changes to process internal measurements from MCM at RRC
                 and blocked sending meas req to L1, Changes are for FEATURE_WCDMA_REL7
09/12/08   ps    Added code for REl7 under FEATURE_WCDMA_REL7. ASN1 migration
                 to march 08 ver
07/16/08   rm    Enhanced the handling of transaction id received in MCM message so that
                 UE accepts the MCM's with same transaction id and in case of failure UE
                 sends MCM failure for a transaction id only if it is latest among the same
                 transactions ids
10/13/06   sgk   Fixed 7200 compiler warnings. 
10/09/06   vg    made changes for MCM modify additinal meas handling
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases. 
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under 
                 FEATURE REL5 while retaining the HSDPA specifc processing 
                 under FEATURE HSDPA. 
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
11/11/05   sgk   Added support for 6E measurements, removed blocking UTRA carrier
                 RSSI as a measurement quantity. 
07/13/05   sgk   Added rel5 non critical extensions for rel 99 MCM ie the 
                 transaction identifier extension.
07/12/05   sgk   Added additional check to see if both ue trx pwr included and
                 rx tx time diff included is false and if so not include the 
                 measured results in the report. 
06/27/05   sgk   Added basic rel 5 measurement ctrl msg support.
05/26/05   sgk   Added additional validation to reject event triggered 
                 measurement report if the event id is invalid.
05/03/05   vk    Used the new functional interface to set additional meas
                 in DB when FEATURE HSDPA is defined
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/02/05   ttl   Support RRC ASN1 Release 5.
01/27/05   sgk   Added check to see if internal measurement quantity
                 == rrc_utra_Carrier_RSSI and if so returned Measurement control 
                 failure with cause rrc_unsupportedMeasurement_chosen in
                 function rrcueim_process_mcm.
08/30/04   sgk   Replaced if (l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id
                 = L1_INTERNAL_MEAS_EVENT_6G) by (l1_meas_report_ptr->u.
                 ue_internal_meas_rpt.evt_id == L1_INTERNAL_MEAS_EVENT_6G)
                 in function rrcueim_send_meas_report to fix lint error
                 Bolean test of assignment, boolean within ifalways evaluates to
                 TRUE and Constant value boolean.
08/17/04   vk    Changed feature name FEATURE_EVENT_6F_6G_MEAS to
                 FEATURE_WCDMA_RX_TX
08/13/04   vk    Added code to support Event 6F/6G Measurements under
                 FEATURE_EVENT_6F_6G_MEAS
04/06/04   vk    Cleaned up Lint warnings
03/30/04   vk    Fixed merge issue
03/29/04   vk    Read periodic reporting criteria from modify object when
                 MCM with modify is specified
11/07/03   vk    Added check to verify that additional measurements corresponding
                 to a primary measurement can never be greater than the maximum
                 allowed
11/06/03   vk    Initial Release that handles UE Internal Measurements
                 under FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENTS

===========================================================================*/




/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

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
#include "rrcasn1util.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define UE_MIN_POWER 21
#define UE_MAX_POWER 104


#define MIN_RX_TX_TIME_DIFF_THRESHOLD 768
#define MAX_RX_TX_TIME_DIFF_THRESHOLD 1280



/* Internal Meas Event Type */
#define  RET_UEIM_EVENT_TYPE(event_type)  \
(  \
 ((event_type == T_rrc_UE_InternalEventParam_event6a) ? L1_INTERNAL_MEAS_EVENT_6A : \
  ((event_type == T_rrc_UE_InternalEventParam_event6b) ? L1_INTERNAL_MEAS_EVENT_6B : \
  ((event_type == T_rrc_UE_InternalEventParam_event6c) ? L1_INTERNAL_MEAS_EVENT_6C : \
  ((event_type == T_rrc_UE_InternalEventParam_event6d) ? L1_INTERNAL_MEAS_EVENT_6D : \
  ((event_type == T_rrc_UE_InternalEventParam_event6e) ? L1_INTERNAL_MEAS_EVENT_6E : \
  ((event_type == T_rrc_UE_InternalEventParam_event6f) ? L1_INTERNAL_MEAS_EVENT_6F : \
  L1_INTERNAL_MEAS_EVENT_6G )))))) \
)

/* UE Internal Measurement Quantity */
#define  RRCUEIM_RET_MEAS_QUAN(quantity)  \
(  \
 ((quantity == rrc_UE_MeasurementQuantity_ue_TransmittedPower) ? L1_UE_TX_PWR : \
  ((quantity == rrc_UE_MeasurementQuantity_utra_Carrier_RSSI) ? L1_UTRA_RSSI : \
  ((quantity == rrc_UE_MeasurementQuantity_ue_RX_TX_TimeDifference) ? L1_RX_TX_DIFF : \
  L1_MEAS_NO_MEAS ))) \
)

/* Converts report deactivation threshold for FDD from ASN1 format to 
 * L1 format 
 */  
#define  RRCUEIM_RET_REPORTING_AMOUNT(reporting_amount)  \
(  \
 ((reporting_amount == rrc_ReportingAmount_ra_Infinity) ? 0 : \
 ((reporting_amount == rrc_ReportingAmount_ra1) ? 1 : \
  ((reporting_amount == rrc_ReportingAmount_ra2) ? 2 : \
  ((reporting_amount == rrc_ReportingAmount_ra4) ? 4 :\
  ((reporting_amount == rrc_ReportingAmount_ra8) ? 8 :\
  ((reporting_amount == rrc_ReportingAmount_ra16) ? 16 :\
  ((reporting_amount == rrc_ReportingAmount_ra32) ? 32 : 64 \
  ))))))) \
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
extern uint8 rrcmeas_pending_rpt;

/*Boolean added to track if interfrequency and interrat measurements are to be processed or not*/
/*Used the same variable to block sending UE internal measurements to L1*/
extern boolean process_interfreq_interrat_meas_rel7;

/*Boolean to track if inter frequency measurements are allowed or not
Inter freq measurements are allowed if process_interfreq is TRUE 
(Even in case process_interfreq_interrat_meas_rel7 is FALSE)

Use the same variable to unblock UE internal measurements also
*/
extern boolean process_interfreq_uei_meas_rel7;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* Maximum No of UE Internal measurements based on additional meas */
ueim_meas_id_list_struct_type ueim_meas_id_list[MAX_INTERNAL_MEAS];


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM

DESCRIPTION 

 This function processes the Measurement Control Message

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm
(
 rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
 ,uint8 meas_ctrl_tr_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  transaction_id = meas_ctrl_tr_id;

  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;
  l1_internal_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;
  
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,
    rrc_MeasurementControl_r3_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n > 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,
        rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    /* Set Meas Quantity to present */
    l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = TRUE;
    
    /* Set Meas Quantity */

    l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = TRUE;
    
    MSG_LOW("Filter Coefficient present", 0,0,0);
    l1_internal_meas_params->meas_object.u.ue_internal.filt_idx =
      RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.
      setup->u.ue_InternalMeasurement->ue_InternalMeasQuantity.filterCoefficient);
   
    
    l1_internal_meas_params->meas_object.u.ue_internal.meas_quan =
      RRCUEIM_RET_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.
      setup->u.ue_InternalMeasurement->ue_InternalMeasQuantity.measurementQuantity);
    
    /* Set Reporting Quantity */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
       rrc_UE_InternalMeasurement,ue_InternalReportingQuantity))
    {
      
      l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = TRUE;
      
      l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = 
        rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
        ue_InternalReportingQuantity.ue_TransmittedPower;
      /* Get the actual reporting quantity */
      if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
        ue_InternalReportingQuantity.modeSpecificInfo,
        rrc_UE_InternalReportingQuantity_modeSpecificInfo_fdd))
      {
        /* Tx Power required */
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt =
          rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
          ue_InternalReportingQuantity.modeSpecificInfo.u.fdd->ue_RX_TX_TimeDifference;
        
      }
      
    }
    
    
    /* Get Reporting Criteria */
    if (rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
      reportCriteria.t ==  T_rrc_UE_InternalReportCriteria_ue_InternalReportingCriteria)
    {
      /* Set criteria to event triggered */
      l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_EVENT_TRIGGER_RPT;
      
      mcm_rpt_crit = EVENT;
      
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
        reportCriteria.u.ue_InternalReportingCriteria,ue_InternalEventParamList))
      {
        WRRC_MSG0_HIGH("Event List present");
      }
      
      /* Event Triggered Reporting Chosen */
      if (rrcueim_process_event_criteria(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
        reportCriteria.u.ue_InternalReportingCriteria,
        &l1_internal_meas_params->meas_object.u.ue_internal.u.evt_trig_crit) == FAILURE)
      {
        
        
        
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
    }
    else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
      reportCriteria,rrc_UE_InternalReportCriteria_periodicalReportingCriteria))
    {
      /* Set periodic reporting */
      l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_PERIODIC_RPT;
      
      mcm_rpt_crit = PERIODIC;
      /* Periodic Reporting Chosen */
      /* Check if reporting amount present */
      
      /* Assign Default Value i.e. Infinity */
      l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_amount
        = L1_RPT_AMOUNT_INFINITY;
      
     /* Store reporting amount */
     l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_amount =
   (l1_amount_of_reporting_enum_type)
       RRCUEIM_RET_REPORTING_AMOUNT(
       rrc_mcm_ptr->measurementCommand.u.setup->u.
       ue_InternalMeasurement->reportCriteria.u.periodicalReportingCriteria->reportingAmount);

      
      /* Store reporting interval */
      l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_interval =
    (l1_reporting_interval_enum_type)
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL(
        rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement->
        reportCriteria.u.periodicalReportingCriteria->reportingInterval);
      
      
    } /* End of else of periodic reporting chosen */
    else
    {
      /* Set periodic reporting */
      l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_NO_RPT;
      
      mcm_rpt_crit = ADDTL;
    }
    
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,
        rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
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
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_modify))
  {
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType))
    {
      
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
            rrc_UE_InternalMeasurement,ue_InternalMeasQuantity))
      {
        /* Set Meas Quantity to present */
        l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = TRUE;
        
       
        l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = TRUE;
        
        MSG_LOW("Filter Coefficient present", 0,0,0);
        
        l1_internal_meas_params->meas_object.u.ue_internal.filt_idx =
          RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(rrc_mcm_ptr->measurementCommand.u.modify->
          measurementType.u.ue_InternalMeasurement->ue_InternalMeasQuantity.filterCoefficient);
      
        l1_internal_meas_params->meas_object.u.ue_internal.meas_quan =
          RRCUEIM_RET_MEAS_QUAN(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          ue_InternalMeasurement->ue_InternalMeasQuantity.measurementQuantity);
        
      } /* End of Meas Quantity Present */
      
      
      /* Set Reporting Quantity */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
         rrc_UE_InternalMeasurement,ue_InternalReportingQuantity))
      {
        
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = TRUE;
        
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = 
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
          ue_InternalReportingQuantity.ue_TransmittedPower;
        /* Get the actual reporting quantity */
        if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
          ue_InternalReportingQuantity.modeSpecificInfo,
          rrc_UE_InternalReportingQuantity_modeSpecificInfo_fdd))
        {
          /* Tx Power required */
          l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt =
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
            ue_InternalReportingQuantity.modeSpecificInfo.u.fdd->ue_RX_TX_TimeDifference;
          
        }
        
      }
      
      /* Get Reporting Criteria */
      if ( rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
        reportCriteria.t == T_rrc_UE_InternalReportCriteria_ue_InternalReportingCriteria)
      {
        /* Set criteria to event triggered */
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_EVENT_TRIGGER_RPT;
        
        mcm_rpt_crit = EVENT;
        
        if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
          reportCriteria.u.ue_InternalReportingCriteria,ue_InternalEventParamList))
        {
          WRRC_MSG0_HIGH("Event List present");
        }
        
        /* Event Triggered Reporting Chosen */
        if (rrcueim_process_event_criteria(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
          reportCriteria.u.ue_InternalReportingCriteria,
          &l1_internal_meas_params->meas_object.u.ue_internal.u.evt_trig_crit) == FAILURE)
        {
          
          WRRC_MSG0_HIGH("Invalid Event Criteria specified");
          
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
          
          /* Send MCF mhsg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;
        }
      }
      else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
        reportCriteria,rrc_UE_InternalReportCriteria_periodicalReportingCriteria))
      {
        /* Set periodic reporting */
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_PERIODIC_RPT;
        
        mcm_rpt_crit = PERIODIC;
        /* Periodic Reporting Chosen */
        /* Check if reporting amount present */
        
        /* Assign Default Value i.e. Infinity */
        l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_amount
          = L1_RPT_AMOUNT_INFINITY;
        
      
       /* Store reporting amount */
       l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_amount =
       (l1_amount_of_reporting_enum_type)
         RRCUEIM_RET_REPORTING_AMOUNT(
         rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
         reportCriteria.u.periodicalReportingCriteria->reportingAmount);
     
     
        /* Store reporting interval */
        l1_internal_meas_params->meas_object.u.ue_internal.u.periodic_crit.rpt_interval =
      (l1_reporting_interval_enum_type)
          RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL(
          rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement->
          reportCriteria.u.periodicalReportingCriteria->reportingInterval);
        
        
      } /* End of else of periodic reporting chosen */
      else
      {
        /* Set periodic reporting */
        l1_internal_meas_params->meas_object.u.ue_internal.rpt_mode = L1_NO_RPT;
        
        mcm_rpt_crit = ADDTL;
      }
      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
       &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;

        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
                &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;

        }

            /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }
    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;
  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  
  rrc_free(l1_internal_meas_params);
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_EVENT_CRITERIA

DESCRIPTION 

 This function processes the event criteria specified in Measurement
 Control Message

DEPENDENCIES

 None

RETURN VALUE

 Success if event parameters are valid
 Failure Otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_event_criteria
(
 rrc_UE_InternalReportingCriteria* rrc_event_criteria_ptr,
 l1_internal_meas_rpt_crit_struct_type* l1_event_criteria_ptr
)
{
  /* Store the list in a local pointer */
  rrc_UE_InternalEventParamList * rrc_local_list_ptr = NULL;
  
  uint8 cnt = 0;
  
  rrc_local_list_ptr = &rrc_event_criteria_ptr->ue_InternalEventParamList;
  
  while ((rrc_local_list_ptr->n > cnt) && (cnt < L1_MAX_MEAS_EVENT))
  {
    /* Get Event Id */
    l1_event_criteria_ptr->evt[cnt].event_id =
      RET_UEIM_EVENT_TYPE(rrc_local_list_ptr->elem[cnt].t);
    
    
    switch(l1_event_criteria_ptr->evt[cnt].event_id)
    {
    case L1_INTERNAL_MEAS_EVENT_6A:
      
      /* Txion Power threshold */
      l1_event_criteria_ptr->evt[cnt].ue_tx_pwr_thresh = (int16)
        rrc_local_list_ptr->elem[cnt].u.event6a->transmittedPowerThreshold;
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6a->timeToTrigger);
      
      break;
      
      
    case L1_INTERNAL_MEAS_EVENT_6B:
      /* Txion Power threshold */
      l1_event_criteria_ptr->evt[cnt].ue_tx_pwr_thresh = (int16)
        rrc_local_list_ptr->elem[cnt].u.event6b->transmittedPowerThreshold;
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6b->timeToTrigger);
      
      break;
      
    case L1_INTERNAL_MEAS_EVENT_6C:
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6c);
      
      
      break;
      
    case L1_INTERNAL_MEAS_EVENT_6D:
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6d);
      
      
      break;
    
    case  L1_INTERNAL_MEAS_EVENT_6E:
        
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6e);
      
      break;

    case L1_INTERNAL_MEAS_EVENT_6F:
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6f->timeToTrigger);
      
      if ((rrc_local_list_ptr->elem[cnt].u.event6f->ue_RX_TX_TimeDifferenceThreshold 
        < MIN_RX_TX_TIME_DIFF_THRESHOLD) ||
        (rrc_local_list_ptr->elem[cnt].u.event6f->ue_RX_TX_TimeDifferenceThreshold
        > MAX_RX_TX_TIME_DIFF_THRESHOLD))
      {
        WRRC_MSG1_ERROR("6F RX-TX-diff-thresold %d out of range",
          rrc_local_list_ptr->elem[cnt].u.event6f->ue_RX_TX_TimeDifferenceThreshold);
        return FAILURE;
      }
      
      l1_event_criteria_ptr->evt[cnt].rx_tx_diff_thresh = 
        (uint16) (rrc_local_list_ptr->elem[cnt].u.event6f->ue_RX_TX_TimeDifferenceThreshold);
      
      
      break;
      
    case L1_INTERNAL_MEAS_EVENT_6G:
      
      /* Time to trigger */
      l1_event_criteria_ptr->evt[cnt].ttt =
        RRCMEAS_RET_TIME_TO_TRIGGER(
        rrc_local_list_ptr->elem[cnt].u.event6g->timeToTrigger);
      
      if ((rrc_local_list_ptr->elem[cnt].u.event6g->ue_RX_TX_TimeDifferenceThreshold 
        < MIN_RX_TX_TIME_DIFF_THRESHOLD) ||
        (rrc_local_list_ptr->elem[cnt].u.event6g->ue_RX_TX_TimeDifferenceThreshold
        > MAX_RX_TX_TIME_DIFF_THRESHOLD))
      {
        WRRC_MSG1_ERROR("6G RX-TX-diff-thresold %d out of range",
          rrc_local_list_ptr->elem[cnt].u.event6g->ue_RX_TX_TimeDifferenceThreshold);
        return FAILURE;
      }
      
      l1_event_criteria_ptr->evt[cnt].rx_tx_diff_thresh = 
        (uint16) rrc_local_list_ptr->elem[cnt].u.event6g->ue_RX_TX_TimeDifferenceThreshold;
      
      
      break;
      
    default:
      break;  
    } /* End of switch */
    
    cnt++;
  } /* End of while */
  
  /* Store the count */
  l1_event_criteria_ptr->num_event = cnt;
  
  return SUCCESS;

}



/*===========================================================================

FUNCTION   RRCUEIM_INITIALIZE_MEAS_ID_LIST

DESCRIPTION 

 This function initializes the measurement identity list of
 UE Internal Measurements

DEPENDENCIES

 None

RETURN VALUE

 None 

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_initialize_meas_id_list
(
 void
)
{
  uint8 count = 0, cnt = 0;
  /* Initialize Measurement Identity variable */
  MSG_LOW("Initializing Internal Meas Id List",0,0,0);
  for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
  {
    
    ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
    ueim_meas_id_list[count].meas_validity = STATE_DCH;
    ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
    
    /* Also initialize additional meas */
    for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
      ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
  }

}


/*===========================================================================

FUNCTION   RRCUEIM_SEND_MEAS_REPORT

DESCRIPTION 

 This function sends the Measurement Report after validation

DEPENDENCIES

 None

RETURN VALUE

 None 

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;
  
  /* Stores status of send chain */
  rrcsend_status_e_type status;
  
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
  
  rrc_UE_RX_TX_ReportEntryList *rx_tx_ptr = NULL;
  /* Sanity Tests before building Measurement Report */
  /* Check if reporting criteria is O.K. */
  
  if (l1_meas_report_ptr->rpt_crit == L1_NO_RPT ||
  l1_meas_report_ptr->rpt_crit == L1_NO_CHANGE )
  {
    WRRC_MSG1_ERROR("Rejecting InternalMeasRpt R.Crit=%d", l1_meas_report_ptr->rpt_crit);
    return;
  }
  
  
  if ((l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT) &&
    (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_results_included == TRUE))
  {
    /* Validate Measured Results */
    if (rrcueim_validate_measured_results(
      &l1_meas_report_ptr->u.ue_internal_meas_rpt) == FAILURE)
    {
      WRRC_MSG0_ERROR("Invalid Measured Results");
      return;
    }
    else
    {
      if((l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_tx_pwr_included == TRUE)
        || (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_rx_tx_time_diff_included == TRUE))
      {  
        measured_res_incl = TRUE;
      }
    }
  }
  else
  {
    if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
    {
      if(l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id > L1_INTERNAL_MEAS_EVENT_6G)                
      {
        WRRC_MSG1_ERROR("Ignore %d Event Report invalid evt id",l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id);
        return;
      }      
      
      if (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_results_included == FALSE)
      {
        WRRC_MSG0_HIGH("Meas Results not included");
      }
      else
      {
        /* Now check if measured results are present */
        if (rrcueim_validate_measured_results(
          &l1_meas_report_ptr->u.ue_internal_meas_rpt) == SUCCESS)
        {
          if((l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_tx_pwr_included == TRUE)
            || (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_rx_tx_time_diff_included == TRUE))
          {  
            measured_res_incl = TRUE;
          }
        }
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
  
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.measurementReport);
  
  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;
  
  
  /* First store the meas id */
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,l1_meas_report_ptr->meas_id, INTERNAL_MEAS);
  

  /* Append Event Results if present */
  if (l1_meas_report_ptr->rpt_crit == L1_EVENT_TRIGGER_RPT)
  {
    /* Set event results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
        rrc_MeasurementReport,eventResults);
    
    /* Set t to Inter RAT Events Results chosen */
    msg_ptr->message.u.measurementReport.eventResults.t = T_rrc_EventResults_ue_InternalEventResults;

     msg_ptr->message.u.measurementReport.eventResults.u.ue_InternalEventResults =
    rtxMemAllocTypeZ (&enc_ctxt, rrc_UE_InternalEventResults);
    
    /* Set Event ID */
    rrcueim_set_event_id(&l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id,
      msg_ptr->message.u.measurementReport.eventResults.u.ue_InternalEventResults);

    if ((l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id == L1_INTERNAL_MEAS_EVENT_6F) ||
    (l1_meas_report_ptr->u.ue_internal_meas_rpt.evt_id == L1_INTERNAL_MEAS_EVENT_6G))
    {
      if (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_psc > MAX_PSC)
      {
        WRRC_MSG1_ERROR("PSC %d out of range. Ignoring Meas Report", l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_psc);
      //  rrc_free(msg_ptr);
        rrcasn1_free_pdu(rrc_UL_DCCH_Message_PDU,msg_ptr);
        msg_ptr = NULL;
        return;
      }

      msg_ptr->message.u.measurementReport.eventResults.u.ue_InternalEventResults->u.event6f =
       rtxMemAllocTypeZ (&enc_ctxt, rrc_PrimaryCPICH_Info);
      /* Set PSC */
      WRRC_MSG1_HIGH("Event 6F on PSC %d", l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_psc);
      msg_ptr->message.u.measurementReport.eventResults.u.
        ue_InternalEventResults->u.event6f->primaryScramblingCode = l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_psc;
      
    }
    
    
  }
  /* Now add Measured Results */
  if (measured_res_incl == FALSE)
  {
    WRRC_MSG1_HIGH("UEIM: Meas Results absent,measured_res_incl is %d",measured_res_incl);
  }
  else
  {
    /* First set the bit mask for Measured Results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,measuredResults);
    
    /* Set Measured Results t */
    msg_ptr->message.u.measurementReport.measuredResults.t =
      T_rrc_MeasuredResults_ue_InternalMeasuredResults;
    
    msg_ptr->message.u.measurementReport.measuredResults.u.ue_InternalMeasuredResults =
    rtxMemAllocTypeZ (&enc_ctxt, rrc_UE_InternalMeasuredResults);
     
    /* Set t to fdd */
    msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_InternalMeasuredResults->modeSpecificInfo.t =
      T_rrc_UE_InternalMeasuredResults_modeSpecificInfo_fdd;

    msg_ptr->message.u.measurementReport.measuredResults.u.ue_InternalMeasuredResults->modeSpecificInfo.u.fdd =
    rtxMemAllocTypeZ (&enc_ctxt, rrc_UE_InternalMeasuredResults_fdd);
    /* Initialize bit mask */
    RRC_RESET_MSG_IE_PRESENT_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_InternalMeasuredResults->modeSpecificInfo.u.fdd);
    
    if (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_tx_pwr_included == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
        ue_InternalMeasuredResults->modeSpecificInfo.u.fdd,
        ue_TransmittedPowerFDD);
      
      msg_ptr->message.u.measurementReport.measuredResults.u.
        ue_InternalMeasuredResults->modeSpecificInfo.u.fdd->ue_TransmittedPowerFDD =
        (rrc_UE_TransmittedPower)l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_tx_pwr;
    }
    /* Check if RX-TX present */
    if (l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.ue_rx_tx_time_diff_included == TRUE) 
    {
      //rx_tx_ptr = rrc_malloc (sizeof(struct rrc_UE_RX_TX_ReportEntryList_));

      rx_tx_ptr = &msg_ptr->message.u.measurementReport.measuredResults.u.ue_InternalMeasuredResults->
        modeSpecificInfo.u.fdd->ue_RX_TX_ReportEntryList;
      rx_tx_ptr->n = l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.num_rl;

      ALLOC_ASN1ARRAY2(&enc_ctxt,rx_tx_ptr,rrc_UE_RX_TX_ReportEntry);

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
        ue_InternalMeasuredResults->modeSpecificInfo.u.fdd,
       ue_RX_TX_ReportEntryList);


      /* Get the no of RL links */
      for (cnt = 0; cnt < l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.num_rl; cnt++) 
      {
        /* Get the first scrambling code */
        rx_tx_ptr->elem[cnt].primaryCPICH_Info.primaryScramblingCode = 
          l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.rx_tx_rpt[cnt].pri_scr_code;

        rx_tx_ptr->elem[cnt].ue_RX_TX_TimeDifferenceType1 = 
          l1_meas_report_ptr->u.ue_internal_meas_rpt.cell_measured_result_info.rx_tx_rpt[cnt].rx_tx_diff_type_1; 

        
      }  /* End of for */
    }    /* rx tx diff included */
  } /* End of else of measured_res_incl */
  
  /* Addtl Meas List must be initialized */
  msg_ptr->message.u.measurementReport.additionalMeasuredResults.n = 0;
  
  /* Append Additional Measured Results if available */
  rrcmeas_process_additional_measured_results(
    &l1_meas_report_ptr->meas_id, INTERNAL_MEAS, &msg_ptr->message.u.measurementReport);
  
  if (l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT) 
  {
    
    /* Now check if this is the last report. Meas Id needs to be deleted if true */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      /* Delete this Measurement */
      for (meas_id_cnt =0; meas_id_cnt < MAX_INTERNAL_MEAS; meas_id_cnt++ )
      {
        if (ueim_meas_id_list[meas_id_cnt].meas_id == l1_meas_report_ptr->meas_id)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",l1_meas_report_ptr->meas_id);
          ueim_meas_id_list[meas_id_cnt].meas_id = INVALID_MEAS_ID;
          
          ueim_meas_id_list[meas_id_cnt].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            ueim_meas_id_list[meas_id_cnt].additional_meas[cnt] = INVALID_MEAS_ID;
          
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
  
  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);
  
  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("Send chain failed to send InternalMeas Rpt");
  }
  else
  {
    WRRC_MSG0_HIGH("InternalMeas Rpt sent from RRC=>RLC");
    if(l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE)
    {
      rrcmeas_pending_rpt++; 
      WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
    }
  }
  
  return;

}

/*===========================================================================

FUNCTION   RRCUEIM_SET_EVENT_ID

DESCRIPTION 

 This function sets the event ID in ASN1 format

DEPENDENCIES

 None

RETURN VALUE

 None 

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_set_event_id
(
 l1_internal_meas_event_enum_type* l1_event_type_ptr,
 rrc_UE_InternalEventResults* rrc_event_id_ptr
)
{
  WRRC_MSG1_HIGH("UEIM:EVENT %d[0/1/2/3/4/5/6:6A/6B/6C/6D/6E/6F/6G] reported",*l1_event_type_ptr );
  /* Find type of event and set it */
  switch ( *l1_event_type_ptr )
  {
  case L1_INTERNAL_MEAS_EVENT_6A:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6a;
    break;
    
  case L1_INTERNAL_MEAS_EVENT_6B:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6b;
    
    break;
    
  case L1_INTERNAL_MEAS_EVENT_6C:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6c;
    
    break;
    
  case L1_INTERNAL_MEAS_EVENT_6D:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6d;
    
    break;
    
  case L1_INTERNAL_MEAS_EVENT_6E:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6e;
    break;

  case L1_INTERNAL_MEAS_EVENT_6F:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6f;

    break;
    
  case L1_INTERNAL_MEAS_EVENT_6G:
    rrc_event_id_ptr->t = T_rrc_UE_InternalEventResults_event6g;
    break;
   

  default:
    WRRC_MSG0_ERROR("Invalid t");
    break;
  }  /* End of switch */
  
  return;
}


/*===========================================================================

FUNCTION   RRCUEIM_VALIDATE_MEASURED_RESULTS

DESCRIPTION 

 This function validates the measured results

DEPENDENCIES

 None

RETURN VALUE

 Success if validation successful
 Failure Otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_validate_measured_results
(
  l1_ue_internal_meas_rpt_struct_type* meas_results_ptr
)
{
  uint8 cnt = 0;
  
  
  if (meas_results_ptr->cell_measured_result_info.ue_tx_pwr_included == TRUE)
  {
    if ((meas_results_ptr->cell_measured_result_info.ue_tx_pwr >= UE_MIN_POWER) &&
      (meas_results_ptr->cell_measured_result_info.ue_tx_pwr <= UE_MAX_POWER))
    {
      WRRC_MSG1_HIGH("UEIM:UE TX Power is %d", meas_results_ptr->cell_measured_result_info.ue_tx_pwr);
    }
    else
    {
      WRRC_MSG1_ERROR("UEIM:UE TX Power invalid %d", meas_results_ptr->cell_measured_result_info.ue_tx_pwr);
      return FAILURE;
    }
  }
  /* Check if RX-TX time difference included */
  if (meas_results_ptr->cell_measured_result_info.ue_rx_tx_time_diff_included == TRUE)
  {
    if ((meas_results_ptr->cell_measured_result_info.num_rl > L1_MAX_RL) || 
    (meas_results_ptr->cell_measured_result_info.num_rl == 0))
    {
      WRRC_MSG1_ERROR("Ignore Report. No RL Links %d",meas_results_ptr->cell_measured_result_info.num_rl); 
      return FAILURE;
    }
    /* Check RLs */
    for (cnt = 0; cnt < meas_results_ptr->cell_measured_result_info.num_rl; cnt++)
    {
      /* Check SCR code value & RX-TX value */
      if (meas_results_ptr->cell_measured_result_info.rx_tx_rpt[cnt].pri_scr_code > MAX_PSC)
      {
        WRRC_MSG1_ERROR("Invalid PSC %d", meas_results_ptr->cell_measured_result_info.rx_tx_rpt[cnt].pri_scr_code);
        return FAILURE;
      }
      if ((meas_results_ptr->cell_measured_result_info.rx_tx_rpt[cnt].rx_tx_diff_type_1 < MIN_RX_TX_TIME_DIFF_THRESHOLD) ||
        (meas_results_ptr->cell_measured_result_info.rx_tx_rpt[cnt].rx_tx_diff_type_1 > MAX_RX_TX_TIME_DIFF_THRESHOLD))
      {
        WRRC_MSG1_ERROR("Invalid MIN_RX_TX_TIME_DIFF_THRESHOLD %d", meas_results_ptr->cell_measured_result_info.rx_tx_rpt[cnt].rx_tx_diff_type_1);
        return FAILURE;
      }
    }
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCUEIM_FILL_MCM_INFO_R5

DESCRIPTION 

 This function fills the Measurement Control information into the L1 structure

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_fill_mcm_info_r5
(
  rrc_UE_InternalMeasurement_r4 *ue_InternalMeasurement_ptr,
  l1_ue_internal_meas_ctrl_struct_type *ue_internal_ptr,
  uint32 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
)
{
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ue_InternalMeasurement_ptr, rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity))
  {
    /* Set Meas Quantity to present */
    ue_internal_ptr->meas_quan_included = TRUE;

    /* Check if Filter Coefficient is present */

      ue_internal_ptr->filter_coeff_incl = TRUE;

      MSG_LOW("Filter Coefficient present", 0,0,0);

      ue_internal_ptr->filt_idx =
        RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(ue_InternalMeasurement_ptr->ue_InternalMeasQuantity.filterCoefficient);
    

    ue_internal_ptr->meas_quan =
      RRCUEIM_RET_MEAS_QUAN(ue_InternalMeasurement_ptr->ue_InternalMeasQuantity.measurementQuantity);

  } /* End of Meas Quantity Present */


  /* Set Reporting Quantity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ue_InternalMeasurement_ptr,
     rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity))
  {
    
    ue_internal_ptr->rpt_quan_included = TRUE;

    ue_internal_ptr->rpt_quan.ue_tx_pwr_rpt = 
      ue_InternalMeasurement_ptr->
      ue_InternalReportingQuantity.ue_TransmittedPower;
    /* Get the actual reporting quantity */
    if( RRC_CHECK_COMMON_MSG_TYPE(ue_InternalMeasurement_ptr->
      ue_InternalReportingQuantity.modeSpecificInfo,
      rrc_UE_InternalReportingQuantity_r4_modeSpecificInfo_fdd))
    {
      /* Tx Power required */
      ue_internal_ptr->rpt_quan.ue_rx_tx_diff_rpt =
        ue_InternalMeasurement_ptr->
        ue_InternalReportingQuantity.modeSpecificInfo.u.fdd->ue_RX_TX_TimeDifference;

    }

  }

  /* Get Reporting Criteria */
  if (ue_InternalMeasurement_ptr->reportCriteria.t == T_rrc_UE_InternalReportCriteria_ue_InternalReportingCriteria)
  {
    /* Set criteria to event triggered */
    ue_internal_ptr->rpt_mode = L1_EVENT_TRIGGER_RPT;

    *mcm_rpt_crit_ptr = EVENT;

    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ue_InternalMeasurement_ptr->reportCriteria.u.ue_InternalReportingCriteria,
        ue_InternalEventParamList))
    {
      WRRC_MSG0_HIGH("Event List present");
    }

    /* Event Triggered Reporting Chosen */
    if (rrcueim_process_event_criteria(ue_InternalMeasurement_ptr->reportCriteria.u.ue_InternalReportingCriteria,
      &ue_internal_ptr->u.evt_trig_crit) == FAILURE)
    {

      WRRC_MSG0_HIGH("Invalid Event Criteria specified");

      rrcmeas_mcf.rrc_transaction_id = transaction_id;

      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;

      /* Send MCF mhsg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

      return FAILURE;
    }
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(
    ue_InternalMeasurement_ptr->reportCriteria,rrc_UE_InternalReportCriteria_periodicalReportingCriteria))
  {
    /* Set periodic reporting */
    ue_internal_ptr->rpt_mode = L1_PERIODIC_RPT;

    *mcm_rpt_crit_ptr = PERIODIC;
    /* Periodic Reporting Chosen */
    /* Check if reporting amount present */

    /* Assign Default Value i.e. Infinity */
    ue_internal_ptr->u.periodic_crit.rpt_amount
      = L1_RPT_AMOUNT_INFINITY;

 
    /* Store reporting amount */
    ue_internal_ptr->u.periodic_crit.rpt_amount =
      (l1_amount_of_reporting_enum_type)
      RRCUEIM_RET_REPORTING_AMOUNT(ue_InternalMeasurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingAmount);
  

    /* Store reporting interval */
    ue_internal_ptr->u.periodic_crit.rpt_interval =
      (l1_reporting_interval_enum_type)
      RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL(
      ue_InternalMeasurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingInterval);


  } /* End of else of periodic reporting chosen */
  else
  {
    /* Set periodic reporting */
    ue_internal_ptr->rpt_mode = L1_NO_RPT;

    *mcm_rpt_crit_ptr = ADDTL;
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R5

DESCRIPTION 

 This function processes the Measurement Control Message rel 5

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r5
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4; 

  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;
  l1_internal_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r4_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
   
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */
    
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
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
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r4_modify))
  {
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r4_modify,measurementType))
    {
      if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
      {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      
      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
                &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
         rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;

        }
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */

    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;
  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_internal_meas_params);
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R6

DESCRIPTION 

 This function processes the Measurement Control Message rel 6

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r6
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6; 

  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;
  l1_internal_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r6_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }

    
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
       &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */
    
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
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
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r6_modify,measurementType))
    {
      
      if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
      {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }

      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
                &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;

        }
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }

    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;
  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_internal_meas_params);
  return SUCCESS;
}


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R7

DESCRIPTION 

 This function processes the Measurement Control Message rel 7

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r7
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;

  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;
  l1_internal_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;
  
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r7_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }

    
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
       &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */
    
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
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
    rrc_MeasurementCommand_r7_modify))
  {
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r7_modify,measurementType))
    {
      
      if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
      {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }

      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
                &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;

        }
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */
  
    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;

  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_internal_meas_params);
  return SUCCESS;
}


/*===========================================================================
FUNCTION   RRCUEIM_PROCESS_MCM_R8

DESCRIPTION 
 This function processes the Measurement Control Message rel 8

DEPENDENCIES
 None

RETURN VALUE
 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS
 None
===========================================================================*/
uecomdef_status_e_type rrcueim_process_mcm_r8
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;

  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;
  l1_internal_meas_params->meas_id =  (uint16) rrc_mcm_ptr->measurementIdentity;
  
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r8_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }

    
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
       &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */
    
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
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
    rrc_MeasurementCommand_r8_modify))
  {
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r8_modify,measurementType))
    {
      
      if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
      {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }

      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  INTERNAL_MEAS, rrc_mcm_ptr->measurementIdentity,
                &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */

          rrcmeas_mcf.rrc_transaction_id = transaction_id;

          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

          /* Send MCF msg to UTRAN */
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_internal_meas_params);
          return FAILURE;

        }
        /* Can't set additional meas in DB now as meas id has not yet been set */
      }
    } /* End of if of Additional Meas Present */


    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */
  
    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;

  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_internal_meas_params);
  return SUCCESS;
}

#ifdef FEATURE_WCDMA_REL9
/*===========================================================================
FUNCTION   RRCUEIM_PROCESS_MCM_R8

DESCRIPTION 
 This function processes the Measurement Control Message rel 8

DEPENDENCIES
 None

RETURN VALUE
 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS
 None
===========================================================================*/
uecomdef_status_e_type rrcueim_process_mcm_r9
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
)
{
  /* Process the UE Internal Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_internal_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = INTERNAL_MEAS;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;
  uint8 meas_id = 0;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  mcm_r9 = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.later_r8_criticalExtensions.u.r9;


  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;
  /* Assign default values to inter_rat_meas_params */
  
  l1_internal_meas_params->meas_object_info_included = FALSE;
  l1_internal_meas_params->num_add_meas = 0;
  l1_internal_meas_params->rpt_mode_incl = FALSE;
  l1_internal_meas_params->dpch_cm_status_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.filter_coeff_incl = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.meas_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan_included = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_tx_pwr_rpt = FALSE;
  l1_internal_meas_params->meas_object.u.ue_internal.rpt_quan.ue_rx_tx_diff_rpt = FALSE;
  l1_internal_meas_params->meas_object.meas_type = L1_UE_INTERNAL;
  
  /* Set Transaction Id and Meas Id */
  l1_internal_meas_params->trans_id = (uint16) transaction_id;

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
  
  l1_internal_meas_params->meas_id =  meas_id;
  
  
  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r9_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_internal_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("Invalid Compressed Mode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_internal_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r9_setup))
  {
  /* Check if meas quantity,reporting quantity & Reporting criteria is received.
    If either of them is not received, then configuration incomplete ERROR */
    
    if (!((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      rrc_UE_InternalMeasurement_r4,ue_InternalReportingQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r9_IEs,measurementReportingMode))))
    {
      WRRC_MSG0_ERROR("SETUP:Invalid MeasQuan/ReportQuan/Rpt Mode");
      
      /* Tr Id */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set failure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }
    
    /* Check if Reporting mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r9_IEs,measurementReportingMode))
    {
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode =
        RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
    }
    
    if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.setup->u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }

    
    /* Store reporting mode to TRUE */
    l1_internal_meas_params->rpt_mode_incl = TRUE;
    
    l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
      rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    
    
    /* Now set other values */
    l1_internal_meas_params->meas_cmd = L1_MEAS_SETUP;
    
    l1_internal_meas_params->meas_object_info_included = TRUE;
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id, &meas_validity) == FAILURE)
    {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
    }
    
    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
      {
        /* Find first empty space */
        if (ueim_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          if(meas_id_val != 0)
          {
            ueim_meas_id_list[count].meas_id = meas_id_val;
            ueim_meas_id_list[count].meas_id_legacy = (uint16) rrc_mcm_ptr->measurementIdentity;
          }
          else
          {
            ueim_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          }
          ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          break;
        }
      }
    } /* End of if */
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
        l1_internal_meas_params->meas_id);
      WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
    }
    
    /* Now one needs to send command to L1 */
    
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r9_modify))
  {
    l1_internal_meas_params->meas_cmd = L1_MEAS_MODIFY;
    
    /* Check if reporting Mode present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r9_IEs,measurementReportingMode))
    {
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->reporting_mode.rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if Meas Type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      rrc_MeasurementCommand_r9_modify,measurementType))
    {
      
      if(FAILURE == rrcueim_fill_mcm_info_r5(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.ue_InternalMeasurement,
      &l1_internal_meas_params->meas_object.u.ue_internal, transaction_id, &mcm_rpt_crit))
      {
        rrc_free(l1_internal_meas_params);
        return FAILURE;
      }

      
      /* Store reporting mode to TRUE */
      l1_internal_meas_params->rpt_mode_incl = TRUE;
      
      l1_internal_meas_params->rpt_trans_mode = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(
        rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
      
      l1_internal_meas_params->meas_object_info_included = TRUE;
      
    }
    
    /* Now check Additional Measurements */
    /* Process Additional Meas */
    if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id,&meas_validity) == FAILURE)
    {
      rrc_free(l1_internal_meas_params);
      return FAILURE;
    }


    /* Store meas in MEASUREMENT_IDENTITY List */
    for ( count = 0; count < MAX_INTERNAL_MEAS; count++ )
    {
      /* This meas already exists */
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        break;
      }
    }
    
    if (addtl_meas_incl == TRUE)
    {
      rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
        meas_id);
     WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
    }
  } /* End of else if of modify */
  else
  {
    /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from ueim_meas_id_list
    * Meas Identity, Transaction Id already set */
    l1_internal_meas_params->meas_cmd = L1_MEAS_RELEASE;
    
    /* Delete addtl meas if any from additional DB */
    rrcmeas_delete_addtl_meas(&l1_internal_meas_params->meas_id);
    
    /* Clear Meas Id from ueim_meas_id_list List */
    for (count=0; count < MAX_INTER_RAT_MEAS; count++)
    {
      if (ueim_meas_id_list[count].meas_id == l1_internal_meas_params->meas_id)
      {
        ueim_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        ueim_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++)
        {
          ueim_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        break; /* Come out of for */
      }
    }
  } /* End of else of release */
  
    /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */

  if((!process_interfreq_interrat_meas_rel7) && (!process_interfreq_uei_meas_rel7))
  {
    WRRC_MSG0_HIGH("Rel7: Skip sending internal measurements from MCM to L1");
    rrc_free(l1_internal_meas_params);
    return SUCCESS;
  }
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_internal_meas_params->trans_id = alloc_idx;

  MSG_LOW("MCM:Sending Internal Meas cmd->L1",0,0,0);
  rrcmeas_send_l1_cmd(NULL, l1_internal_meas_params, NULL, L1_MEAS_CTRL_PARMS);
    rrc_free(l1_internal_meas_params);
  return SUCCESS;
}
#endif


#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

