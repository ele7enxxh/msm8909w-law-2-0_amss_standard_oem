/*===========================================================================
            R R C   M E A S U R E M E N T S

DESCRIPTION

   This module supports Quality Measurement Control and Measurement Reporting
   for the RRC layer.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

   RRC Task must have been started. All RRC command queues must
   have been initialized. All RRC signals must have been cleared
   at task startup.

Copyright (c) 2002, 2003, 2005-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

             EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcqm.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/15   ad      Made changes to log important parms from OTAs
11/25/14   vi      Made changes to free msg_ptr if no transport channels and no additional measured results are present in quality measurement report.
08/04/14   vi      Made changes to avoid compiler warnings
03/24/14   sr      Incorrect size of data type resulting a crash while freeing
03/13/14   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/28/13   sr      Added code for measurement identity extension
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp      Removed tmc.h include file
01/19/13   sr      Made changes to send dummy measurement report if additional measurements are configured with it
01/03/13   ad      Made changes to reduce the redundant F3s
05/03/11   rl      Fixed the lint error
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
11/10/08   rm      Made changes to process Rel7 measurement control message
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
07/16/08   rm      Enhanced the handling of transaction id received in MCM message so that
                   UE accepts the MCM's with same transaction id and in case of failure UE
                   sends MCM failure for a transaction id only if it is latest among the same
                   transactions ids
07/05/07   rm      Initialize the bit mask in measurement report
11/22/06   sgk     Added fix to set the report quantity to true for set up with 
                   periodic reporting criteria.  
09/11/06   sgk     Added rel6 measurement support and code optimization across
                   releases. 
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA. 
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
07/13/05   sgk     Added rel5 non critical extensions for rel 99 MCM ie the 
                   transaction identifier extension.
06/27/05   sgk     Added basic rel 5 measurement ctrl msg support.
05/03/05   vk      Used the new functional interface to set additional meas
                   in DB when FEATURE HSDPA is defined
04/14/05   sgk     Added range checks and additional validations for quality 
                   measurement parameters.                       
03/15/05   sgk     Added measurement optimization to restrict max pending ack
                   mode measurement rpts to 12.
03/09/05   sgk     Added RRC ASN1 Release 5 support for Qulaity measurements.
                   Lint fixes.
                   Set reporting mode in L1 structure to L1_NO_RPT instead of 
                   blocking it when the reporting mode is no reporting. 
03/04/05   sgk     Created Quality Measurements File.
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
#include "rrcmcm.h"
#include "rrcasn1util.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_BLER 63
#define MAX_CRC 512
#define MIN_CRC 1
#define MAX_PENDING_AFTER_TRIGGER 512
#define MIN_PENDING_AFTER_TRIGGER 1

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

quality_meas_id_list_struct_type quality_meas_id_list[MAX_QUALITY_MEAS];


/* -----------------------------------------------------------------------
** Local Object Definition
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern uint8 rrcmeas_pending_rpt;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*==========================================================================
 FUNCTION   RRCQM_FILL_MCM_INFO

DESCRIPTION

  This function fills the L1 structure for Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_fill_mcm_info
(
  rrc_QualityMeasurement *qualityMeasurement_ptr,
  l1_qual_meas_ctrl_struct_type  *qual_ptr,
  uint8 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
)
{
  uint8 count = 0, cnt = 0, count1 = 0;
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* local ptr to the trch list for BLER measurement */
  rrc_BLER_TransChIdList *bler_dl_TransChIdList_ptr = NULL;
                       
  /* local ptr to the event criteria list */
  rrc_QualityReportingCriteria *qualityReportingCriteria_ptr = NULL;

  qual_ptr->bler_rpt_requested = FALSE;
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(qualityMeasurement_ptr,qualityReportingQuantity))
  {
    if(qualityMeasurement_ptr->qualityReportingQuantity.dl_TransChBLER == TRUE)
    {
      
      qual_ptr->bler_rpt_requested = TRUE; 
      qual_ptr->rpt_all_transport_ch = TRUE;
      if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(qualityMeasurement_ptr->qualityReportingQuantity,
         bler_dl_TransChIdList)) && 
        (qualityMeasurement_ptr->qualityReportingQuantity.
        bler_dl_TransChIdList.n != 0))
      {
        qual_ptr->rpt_all_transport_ch = FALSE;
        bler_dl_TransChIdList_ptr = &qualityMeasurement_ptr->
          qualityReportingQuantity.bler_dl_TransChIdList;
        
        while((bler_dl_TransChIdList_ptr->n > cnt) && (cnt < L1_MAX_TRANSPORT_CHANNELS))
        {
          if((bler_dl_TransChIdList_ptr->elem[cnt] > L1_MAX_TRANSPORT_CHANNELS)
            || (bler_dl_TransChIdList_ptr->elem[cnt] < 1))
          {
            WRRC_MSG0_ERROR("Modify:Invalid Tr ch id");
            /* Set faliure choice */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            return FAILURE; 
          }         
          qual_ptr->reported_transport_channels[cnt]
            =  (uint8) bler_dl_TransChIdList_ptr->elem[cnt];
          cnt++;
        }     
        qual_ptr->no_of_tr_ch = cnt;   
      }      
    }
  }


  /* processing the report criteria IE */ 
  switch(qualityMeasurement_ptr->reportCriteria.t)
  {
  case T_rrc_QualityReportCriteria_qualityReportingCriteria:
    WRRC_MSG0_HIGH("Quality meas: processing event criteria");   
    cnt = 0;   
    *mcm_rpt_crit_ptr =  EVENT; 
    qual_ptr->rpt_mode = L1_EVENT_TRIGGER_RPT;
    qualityReportingCriteria_ptr = qualityMeasurement_ptr->reportCriteria.u.qualityReportingCriteria;
    
    while((qualityReportingCriteria_ptr->n > cnt ) && (cnt < L1_MAX_TRANSPORT_CHANNELS))
    {
      if((qualityReportingCriteria_ptr->elem[cnt].transportChannelIdentity > L1_MAX_TRANSPORT_CHANNELS)
        ||(qualityReportingCriteria_ptr->elem[cnt].transportChannelIdentity < 1))
      {
        WRRC_MSG0_ERROR("Modify:Invalid Trch id");
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        return FAILURE; 
      }                         
      qual_ptr->u.evt_trig_crit.tr_ch[cnt].tr_ch_id =
        (uint8) qualityReportingCriteria_ptr->elem[cnt].transportChannelIdentity;
      
      if((qualityReportingCriteria_ptr->elem[cnt].totalCRC > MAX_CRC) ||
        (qualityReportingCriteria_ptr->elem[cnt].totalCRC < MIN_CRC) ||
        (qualityReportingCriteria_ptr->elem[cnt].badCRC > MAX_CRC) ||
        (qualityReportingCriteria_ptr->elem[cnt].badCRC < MIN_CRC))       
      {
        WRRC_MSG0_ERROR("Modify:Invalid CRC value");
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        return FAILURE; 
      }
      
      if((qualityReportingCriteria_ptr->elem[cnt].pendingAfterTrigger > MAX_PENDING_AFTER_TRIGGER) ||
        (qualityReportingCriteria_ptr->elem[cnt].pendingAfterTrigger < MIN_PENDING_AFTER_TRIGGER))
      {
        WRRC_MSG0_ERROR("Modify:Invalid Pending After Trigger value");
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        return FAILURE; 
      }
      
      if(qualityReportingCriteria_ptr->elem[cnt].totalCRC < qualityReportingCriteria_ptr->elem[cnt].badCRC)
      {
        WRRC_MSG0_ERROR("MODIFY:total CRC < BAD CRC");
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        return FAILURE; 
      }
      qual_ptr->u.evt_trig_crit.tr_ch[cnt].total_crc =
        (uint16) qualityReportingCriteria_ptr->elem[cnt].totalCRC;
      qual_ptr->u.evt_trig_crit.tr_ch[cnt].bad_crc =
        (uint16) qualityReportingCriteria_ptr->elem[cnt].badCRC;
      qual_ptr->u.evt_trig_crit.tr_ch[cnt].pending_time_after_trigger =
        (uint16) qualityReportingCriteria_ptr->elem[cnt].pendingAfterTrigger;
      cnt++;
    }
    
    qual_ptr->u.evt_trig_crit.no_of_transport_channels = cnt;
    for(count = 0; count < cnt-1; count++)
    {
      for(count1 = count+1; count1 < cnt; count1++)
      {
        if(qual_ptr->u.evt_trig_crit.tr_ch[count].tr_ch_id
          == qual_ptr->u.evt_trig_crit.tr_ch[count1].tr_ch_id)
        {
          WRRC_MSG0_ERROR("Modify:trch id repeated");
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          return FAILURE; 
        }
      }   
    }  
    break;
    
  case T_rrc_QualityReportCriteria_periodicalReportingCriteria:
    WRRC_MSG0_HIGH("Quality meas: processing periodic criteria");               
    *mcm_rpt_crit_ptr =  PERIODIC; 
    qual_ptr->rpt_mode = L1_PERIODIC_RPT;
    
    qual_ptr->u.periodic_crit.rpt_amount = L1_RPT_AMOUNT_INFINITY;
    
 
    qual_ptr->u.periodic_crit.rpt_amount = (l1_amount_of_reporting_enum_type) 
      RRCMEAS_RET_REPORTING_AMOUNT(qualityMeasurement_ptr->reportCriteria.u.
      periodicalReportingCriteria->reportingAmount);
    
    
    if(qualityMeasurement_ptr->
      reportCriteria.u.periodicalReportingCriteria->reportingInterval == rrc_ReportingIntervalLong_ril0)
    {
      WRRC_MSG0_ERROR("rpt interval 0 using 250");
      qual_ptr->u.periodic_crit.rpt_interval = L1_RPT_INTERVAL_250MS; 
    }
    else
    {
      qual_ptr->u.periodic_crit.rpt_interval = (l1_reporting_interval_enum_type) 
        RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL(qualityMeasurement_ptr->reportCriteria.
        u.periodicalReportingCriteria->reportingInterval);
    }    
    
    break;
    
  case T_rrc_QualityReportCriteria_noReporting:
    qual_ptr->rpt_mode = L1_NO_RPT;
    break; 
    
  default: /* Invalid t value */
    break;
  }
  return SUCCESS;
}

/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
  ,uint8 meas_ctrl_tr_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  uint8 transaction_id = INVALID_TRANSACTION_ID; 
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  transaction_id = meas_ctrl_tr_id;

  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = (uint16)rrc_mcm_ptr->measurementIdentity;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
  
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
        qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:Quality Rpt Quan not present");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("SETUP:meas Rpt mode mandatory");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
         rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
        rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("SETUP:qual rept quantity for tdd chosen");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }

    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
      rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 

    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    } 

    /* validate the additional measurements*/
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error choice */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
      }
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          quality_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
           qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
           rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure choice */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,
           rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("Modify:qual rept quantity for tdd chosen");
            /* Set faliure choice */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
          measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
          transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        } 
      }
      else
      {
        WRRC_MSG0_HIGH("Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
        && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
      {
        WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
        /* Call fn which makes check for additional measurements */
        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            
            /* Set error choice */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE;
          }
        }
      }
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == rrc_mcm_ptr->measurementIdentity)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",rrc_mcm_ptr->measurementIdentity);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n  > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
      rrc_mcm_ptr->measurementIdentity);
    WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
  }

  

  /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}

/*==========================================================================
 FUNCTION   RRCQM_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  This function initializes the measurement identity list for
  Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    None
    
SIDE EFFECTS

    None

===========================================================================*/

void rrcqm_initialize_meas_id_list()
{
  uint8 count, cnt;
   
  for(count = 0; count < MAX_QUALITY_MEAS; count ++)
  {
    quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
    quality_meas_id_list[count].meas_validity = NOT_PRESENT;
    quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;

    /* Also initialize additional meas */
    for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
      quality_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
   }
}

/*===========================================================================

FUNCTION   RRCQM_SEND_MEAS_REPORT

DESCRIPTION 

 This function sends the Quality Measurement Report after validation

DEPENDENCIES

 None

RETURN VALUE

 None 

SIDE EFFECTS

 None
===========================================================================*/

void rrcqm_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;
  
  /* Stores status of send chain */
  rrcsend_status_e_type status;
  
  uint8 meas_id_cnt = 0, cnt = 0;
  
  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode;
  
  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id;
  
  /* Radio bearer mode */
  rrc_RB_Identity rb_mode;
  
  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status;  
  
  rrc_BLER_MeasurementResultsList *blerMeasurementResultsList_ptr = NULL;
  
  rrc_QualityEventResults *qualityEventResults_ptr = NULL;
  
  /* Sanity Tests before building Measurement Report */
  /* Check if reporting criteria is O.K. */
  
#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_mrm_ota mrm_params;
  memset(&mrm_params,0,sizeof(rrc_log_rrc_mrm_ota));    
  mrm_params.meas_id = l1_meas_report_ptr->meas_id;
  mrm_params.meas_type = L1_QUALITY;
  mrm_params.meas_rpt_type = l1_meas_report_ptr->rpt_crit;
  rrc_qsh_log_meas_rpt_params(&mrm_params);
#endif
  
  if (l1_meas_report_ptr->rpt_crit == L1_NO_RPT ||
    l1_meas_report_ptr->rpt_crit == L1_NO_CHANGE || l1_meas_report_ptr->rpt_crit == L1_INVALID_RPT_MODE)
  {
    WRRC_MSG1_ERROR("Rejecting QualityMeasRpt R.Crit=%d", l1_meas_report_ptr->rpt_crit);
    return;
  }
  
  if ((l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch == 0) && (l1_meas_report_ptr->rpt_crit != L1_PERIODIC_RPT))
  {
    WRRC_MSG0_ERROR("Reject QualityMeasRpt 0 tr ch");
    return;
  }
  
  if ((l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch > L1_MAX_TRANSPORT_CHANNELS))
  {
    WRRC_MSG1_ERROR("Reject QualityMeasRpt num tr ch %d > max", l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch);
    return;
  }
  
  for(cnt = 0; cnt < l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch; cnt++)
  {
    if((l1_meas_report_ptr->u.qual_meas_rpt.l1_trch_bler[cnt].transport_ch_identity > L1_MAX_TRANSPORT_CHANNELS)
      || (l1_meas_report_ptr->u.qual_meas_rpt.l1_trch_bler[cnt].transport_ch_identity < 1))       
    {
      WRRC_MSG1_ERROR("Reject QualityMeasRpt invalid tr ch id %d", l1_meas_report_ptr->u.qual_meas_rpt.
        l1_trch_bler[cnt].transport_ch_identity);
      return;
    }
    if((l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT) &&
      (l1_meas_report_ptr->u.qual_meas_rpt.l1_trch_bler[cnt].bler_present == TRUE) &&
      (l1_meas_report_ptr->u.qual_meas_rpt.l1_trch_bler[cnt].bler_value > MAX_BLER))
    {
      WRRC_MSG1_ERROR("Reject QualityMeasRpt invalid BLER value %d", l1_meas_report_ptr->u.qual_meas_rpt.
        l1_trch_bler[cnt].bler_value);
      return;
    }
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
  
  /* Allocate a buffer for an uplink DCCH Message. This buffer
  * should be released by SEND_CHAIN using OSS compiler functions */
  
  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));
  
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.measurementReport);
  
  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;
  
  
  /* First store the meas id */
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,l1_meas_report_ptr->meas_id, QUALITY_MEAS);

  
  switch(l1_meas_report_ptr->rpt_crit)
  {
  case L1_EVENT_TRIGGER_RPT:
    WRRC_MSG0_HIGH("Quality meas: event 5A");       
    /* Set event results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,eventResults);
    
    /* Set choice to Inter RAT Events Results chosen */
    RRC_SET_COMMON_MSG_TYPE2(msg_ptr->message.u.measurementReport.eventResults,
    rrc_EventResults,qualityEventResults);
    msg_ptr->message.u.measurementReport.eventResults.u.qualityEventResults = 
     rtxMemAllocTypeZ(&enc_ctxt, rrc_QualityEventResults );

    msg_ptr->message.u.measurementReport.eventResults.u.qualityEventResults->n = 
      l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch;

    ALLOC_ASN1ARRAY2(&enc_ctxt,
        msg_ptr->message.u.measurementReport.eventResults.u.qualityEventResults,
        rrc_TransportChannelIdentity);
    qualityEventResults_ptr = msg_ptr->message.u.measurementReport.eventResults.u.qualityEventResults;
    for(cnt = 0; cnt < l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch; cnt++)
    {
      
      qualityEventResults_ptr->elem[cnt] = 
         l1_meas_report_ptr->u.qual_meas_rpt.
        l1_trch_bler[cnt].transport_ch_identity;
    
    } 
    
    break;
    
  case L1_PERIODIC_RPT:

    if(l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch == 0)
    {
      WRRC_MSG0_HIGH("Quality meas: no_of_tr_ch is 0, send empty meas report if addl meas present");	
    }
    else
    {
      /* set periodic measured results to TRUE */
      WRRC_MSG0_HIGH("Quality meas: periodic BLER rpt");
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
          rrc_MeasurementReport,measuredResults);
    
      RRC_SET_COMMON_MSG_TYPE(msg_ptr->message.u.measurementReport.measuredResults,
        rrc_MeasuredResults_qualityMeasuredResults);

       msg_ptr->message.u.measurementReport.measuredResults.u.qualityMeasuredResults = 
         rtxMemAllocTypeZ(&enc_ctxt, rrc_QualityMeasuredResults );
    
       RRC_RESET_MSG_IE_PRESENT_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
         qualityMeasuredResults);
    
       RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
        qualityMeasuredResults,blerMeasurementResultsList);
    
       msg_ptr->message.u.measurementReport.measuredResults.u.
         qualityMeasuredResults->blerMeasurementResultsList.n =
         l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch;
    
       blerMeasurementResultsList_ptr= &msg_ptr->message.u.measurementReport.measuredResults.u.
         qualityMeasuredResults->blerMeasurementResultsList;
    
       ALLOC_ASN1ARRAY2(&enc_ctxt,
         blerMeasurementResultsList_ptr,
         rrc_BLER_MeasurementResults);

       for(cnt = 0; cnt < l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch; cnt++)
       {
         blerMeasurementResultsList_ptr->elem[cnt].transportChannelIdentity
          =  l1_meas_report_ptr->u.qual_meas_rpt.
          l1_trch_bler[cnt].transport_ch_identity;
     
         RRC_RESET_MSG_IE_PRESENT(blerMeasurementResultsList_ptr->elem[cnt]);
      
        if(l1_meas_report_ptr->u.qual_meas_rpt.
          l1_trch_bler[cnt].bler_present == TRUE)
        {   

          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(blerMeasurementResultsList_ptr->elem[cnt],
            dl_TransportChannelBLER);
        
          blerMeasurementResultsList_ptr->elem[cnt].dl_TransportChannelBLER
            =  l1_meas_report_ptr->u.qual_meas_rpt.l1_trch_bler[cnt].bler_value;       
        }
      }
      RRC_SET_COMMON_MSG_TYPE(msg_ptr->message.u.measurementReport.measuredResults.u.qualityMeasuredResults->modeSpecificInfo,
        rrc_QualityMeasuredResults_modeSpecificInfo_fdd);
     }
     break;
    
     default: WRRC_MSG0_ERROR("Invalid meas rpt criteria");
       break;

  } 
  
  
  /* Addtl Meas List must be initialized */
  msg_ptr->message.u.measurementReport.additionalMeasuredResults.n = 0;
  
  /* Append Additional Measured Results if available */
  rrcmeas_process_additional_measured_results(
    &l1_meas_report_ptr->meas_id, QUALITY_MEAS, &msg_ptr->message.u.measurementReport);
  
  if (l1_meas_report_ptr->rpt_crit == L1_PERIODIC_RPT) 
  {
    /* Now check if this is the last report. Meas Id needs to be deleted if true */
    if (l1_meas_report_ptr->last_periodic_report == TRUE)
    {
      /* Delete this Measurement */
      for (meas_id_cnt =0; meas_id_cnt < MAX_QUALITY_MEAS; meas_id_cnt++ )
      {
        if (quality_meas_id_list[meas_id_cnt].meas_id
          == l1_meas_report_ptr->meas_id)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",l1_meas_report_ptr->meas_id);
          quality_meas_id_list[meas_id_cnt].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[meas_id_cnt].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[meas_id_cnt].additional_meas[cnt]
            = INVALID_MEAS_ID;
          
          break;
        }
      }
    }
  }
  
  if((l1_meas_report_ptr->u.qual_meas_rpt.no_of_tr_ch == 0) && (msg_ptr->message.u.measurementReport.additionalMeasuredResults.n == 0))
  {
    WRRC_MSG0_HIGH("Quality meas: no_of_tr_ch is 0 and addl_meas_present is FALSE, do not send empty meas report"); 
    rrc_free(msg_ptr );
    return;
  }

  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);
  
  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("Send chain failed to send QualityMeas Rpt");
  }
  else
  {
    WRRC_MSG0_HIGH("QualityMeas Rpt sent from RRC=>RLC");
    if(l1_meas_report_ptr->meas_transfer_mode == ACKNOWLEDGED_MODE)
    {
      rrcmeas_pending_rpt++; 
      WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
    } 
  }
  
  return;
}

/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R5

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 5

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r5
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;
          
  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;  

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4;
  
  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = (uint16)rrc_mcm_ptr->measurementIdentity;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
  
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_r4_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:Quality Rpt Quan not present");
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("SETUP:meas Rpt mode mandatory");
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
      rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
        rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("SETUP:qual rept quantity for tdd chosen");
      /* Set faliure choice */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    } 
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
      rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 
        
    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }

    /* validate the additional measurements*/
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
      }
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          quality_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_r4_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r4_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
          qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
            rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure t */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("Modify:qual rept quantity for tdd chosen");
            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
        transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }        
      }
      else
      {
        WRRC_MSG0_HIGH("Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
        && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
      {
        WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);   
        /* Call fn which makes check for additional measurements */

        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
                
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE;
          }
        }
      }
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_r4_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == rrc_mcm_ptr->measurementIdentity)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",rrc_mcm_ptr->measurementIdentity);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
      rrc_mcm_ptr->measurementIdentity);

    WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
  }

  

  /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}


/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R6

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 6

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r6
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;
          
  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;  

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6;
  
  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = (uint16)rrc_mcm_ptr->measurementIdentity;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_r6_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
        qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:Quality Rpt Quan not present");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("SETUP:meas Rpt mode mandatory");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
       rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
      rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("SETUP:qual rept quantity for tdd chosen");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    } 

    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
       rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 
        
    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }

    /* validate the additional measurements*/
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
      && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
      }
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          quality_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_r6_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r6_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
          qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
             rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure t */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("Modify:qual rept quantity for tdd chosen");
            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
        transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }        
      }
      else
      {
        WRRC_MSG0_HIGH("Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
        && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
      {
        WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);   
        /* Call fn which makes check for additional measurements */

        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
                
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl,  QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE;
          }
        }
      }
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_r6_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == rrc_mcm_ptr->measurementIdentity)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",rrc_mcm_ptr->measurementIdentity);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
      rrc_mcm_ptr->measurementIdentity);

    WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
  }

  

  /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}


/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R7

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 7

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r7
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;
          
  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;  

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;
  
  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = (uint16)rrc_mcm_ptr->measurementIdentity;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_r7_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("SETUP:Quality Rpt Quan not present");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("SETUP:meas Rpt mode mandatory");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
       rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.
        qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
        rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("SETUP:qual rept quantity for tdd chosen");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    } 

    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
       rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 
        
    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }

    /* validate the additional measurements*/
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
    {
      WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl,  QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
      }
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          quality_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_r7_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r7_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
          qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
             rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure t */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,
            rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("Modify:qual rept quantity for tdd chosen");
            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
        transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }        
      }
      else
      {
        WRRC_MSG0_HIGH("Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
        && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
      {
        WRRC_MSG1_HIGH("MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);   
        /* Call fn which makes check for additional measurements */

        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
                
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE;
          }
        }
      }
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_r7_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == rrc_mcm_ptr->measurementIdentity)
        {
          WRRC_MSG1_HIGH("Delete Meas %d",rrc_mcm_ptr->measurementIdentity);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
      rrc_mcm_ptr->measurementIdentity);

    WRRC_MSG0_HIGH("Additional Meas set in RRC DB");
  }

  

  /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);

  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}


/*==========================================================================
 FUNCTION   rrcqm_process_mcm_r8

DESCRIPTION
  This function processes the Measurement Control Message for
  Quality Measurements for rel 8

DEPENDENCIES
   None.

RETURN VALUE
    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS
    None
===========================================================================*/
uecomdef_status_e_type rrcqm_process_mcm_r8
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;
          
  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;  

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;
  
  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = (uint16)rrc_mcm_ptr->measurementIdentity;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("REL8:QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_r8_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("REL8:SETUP:Quality Rpt Quan not present");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("REL8:SETUP:meas Rpt mode mandatory");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
      rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("REL8:SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
       rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("REL8:SETUP:qual rept quantity for tdd chosen");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    } 

    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
        rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 
        
    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }

    /* validate the additional measurements*/
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
    {
      WRRC_MSG1_HIGH("REL8:MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);    
      /* Call fn which makes check for additional measurements */
      if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
        &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
      else
      {
        addtl_meas_incl = TRUE;
        if (rrcmeas_check_mcm_for_setup_modify_additional(
          addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
          &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
        {
          /* As UTRAN tried to configure more than 4 Addtl Meas */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
      }
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          quality_meas_id_list[count].meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_r8_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r8_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
          qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
            rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("REL8:Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure t */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,
             rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("REL8:Modify:qual rept quantity for tdd chosen");
            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
        transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }        
      }
      else
      {
        WRRC_MSG0_HIGH("REL8:Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
        && (rrc_mcm_ptr->additionalMeasurementList.n != 0)) 
      {
        WRRC_MSG1_HIGH("REL8:MCM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);   
        /* Call fn which makes check for additional measurements */

        if (rrcmeas_validate_additional_meas(&rrc_mcm_ptr->additionalMeasurementList, transaction_id,
          &primary_meas_type, rrc_mcm_ptr->measurementIdentity) == FALSE)
                
        {
          /* Called function has already sent Meas Ctrl Failure */
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }
        else
        {
          addtl_meas_incl = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_incl, QUALITY_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE;
          }
        }
      }
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_r8_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == rrc_mcm_ptr->measurementIdentity)
        {
          WRRC_MSG1_HIGH("REL8:Delete Meas %d",rrc_mcm_ptr->measurementIdentity);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_set_additional_meas_in_db(&primary_meas_type, &rrc_mcm_ptr->additionalMeasurementList,
      rrc_mcm_ptr->measurementIdentity);

    WRRC_MSG0_HIGH("REL8:Additional Meas set in RRC DB");
  }

   /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}

#ifdef FEATURE_WCDMA_REL9
/*==========================================================================
 FUNCTION   rrcqm_process_mcm_r8

DESCRIPTION
  This function processes the Measurement Control Message for
  Quality Measurements for rel 8

DEPENDENCIES
   None.

RETURN VALUE
    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS
    None
===========================================================================*/
uecomdef_status_e_type rrcqm_process_mcm_r9
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
)
{
  /* Process the Quality Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  /* Declare MCM config structure that goes to L1 */
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  /* This var is set to TRUE in case addtl meas is successfully validated */
  boolean addtl_meas_incl = FALSE;
  
  /* Declared to store rpt criteria in RRC DB */
  rpt_crit_enum_type mcm_rpt_crit = NOT_AVAILABLE;
  
  /* Declared to store meas type */
  rrcmeas_layer_e_type primary_meas_type = QUALITY_MEAS;

  /* Declare MCM config structure that goes to L1 */
  l1_meas_ctrl_parm_struct_type  *l1_qual_meas_params = rrc_malloc(sizeof(l1_meas_ctrl_parm_struct_type));

  /* This is used if for SETUP, meas id does not exist in DB */
  boolean meas_exists_for_setup = FALSE;

  /* local ptr to the additional measurements list */
  rrc_AdditionalMeasurementID_List *additionalMeasurementList_ptr = NULL;
          
  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;  

  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  /* Meas Validity */
  meas_validity_enum_type meas_validity = NOT_PRESENT;


  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;
  uint8 meas_id = 0;

  /* Store the message */
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
  
  /* Initialize the l1 structure */
  l1_qual_meas_params->meas_object_info_included = FALSE;
  l1_qual_meas_params->num_add_meas = 0;
  l1_qual_meas_params->rpt_mode_incl = FALSE;   
  l1_qual_meas_params->meas_object.meas_type = L1_QUALITY;
  l1_qual_meas_params->trans_id = transaction_id;
  l1_qual_meas_params->meas_id = meas_id;
  l1_qual_meas_params->dpch_cm_status_incl = FALSE;
  l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode))
  {
    l1_qual_meas_params->rpt_mode_incl = TRUE;   
    l1_qual_meas_params->reporting_mode.rpt_trans_mode
      = RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode); 
  }

  /* Set Compressed Mode Information */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,dpch_CompressedModeStatusInfo)) &&
    (rrc_mcm_ptr->dpch_CompressedModeStatusInfo.tgp_SequenceShortList.n != 0))
  {
    
    if (rrcmeas_process_compressed_mode_status(&rrc_mcm_ptr->dpch_CompressedModeStatusInfo,
      &l1_qual_meas_params->cm_status_info) == FAILURE)
    {
      WRRC_MSG0_HIGH("REL9:QUALITY:Invalid CompMode Status Info");
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;
      
    }
    /* Set compressed Mode as TRUE */
    l1_qual_meas_params->dpch_cm_status_incl = TRUE;
    
  } /* End of if of compressed mode processing */
                  
  /* set the Transaction Id and the Meas Id */
  switch(rrc_mcm_ptr->measurementCommand.t)
  {
  case T_rrc_MeasurementCommand_r9_setup:
    
    /* check if the quality reporting quantity is present if not return failure */
    if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      qualityReportingQuantity)))
    {
      WRRC_MSG0_ERROR("REL9:SETUP:Quality Rpt Quan not present");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }    
    
    if(l1_qual_meas_params->rpt_mode_incl == FALSE)
    {
      WRRC_MSG0_ERROR("REL9:SETUP:meas Rpt mode mandatory");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE;    
    }
    
    l1_qual_meas_params->meas_cmd = L1_MEAS_SETUP;
    l1_qual_meas_params->meas_object_info_included = TRUE;
    
    /*process the reporting quantity IE */
    
    if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
      rrc_QualityReportCriteria,qualityReportingCriteria))
      &&
      (rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.dl_TransChBLER == TRUE))
    {
      WRRC_MSG0_ERROR("REL9:SETUP:BLER rpt set to true for evnt trigg meas");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    }  
    
    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->qualityReportingQuantity.modeSpecificInfo,
       rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
    {
      WRRC_MSG0_ERROR("REL9:SETUP:qual rept quantity for tdd chosen");
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      rrc_free(l1_qual_meas_params);
      return FAILURE; 
    } 

    if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement->reportCriteria,
        rrc_QualityReportCriteria_periodicalReportingCriteria))
    {
      l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE; 
    } 
        
    if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.setup->u.qualityMeasurement,
      &l1_qual_meas_params->meas_object.u.qual, transaction_id, &mcm_rpt_crit))
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }

    /* validate the additional measurements*/
    if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id,&meas_validity) == FAILURE)
    {
      rrc_free(l1_qual_meas_params);
      return FAILURE;
    }
    
    /* fill up the measurement identity, validity and rpt criteria in the db */
    /* Store meas in MEASUREMENT_IDENTITY List */
    
    for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
    {
      /* First check if this meas already exists */
      if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
      {
        quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
        quality_meas_id_list[count].meas_validity = STATE_DCH;
        meas_exists_for_setup = TRUE;
        break;
      }
    }
    
    /* Check if meas already exists. If not set the crit */
    if (meas_exists_for_setup == FALSE)
    {
      /* Set this meas in Meas Id List */
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* Find first empty space */
        if (quality_meas_id_list[count].meas_id == INVALID_MEAS_ID)
        {
          if(meas_id_val != 0)
          {
            quality_meas_id_list[count].meas_id = meas_id;
            quality_meas_id_list[count].meas_id_legacy = (uint16) rrc_mcm_ptr->measurementIdentity;
          }
          else
          {
            quality_meas_id_list[count].meas_id = meas_id;
          }
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      }
    } /* End of if */
    break;
        
    case T_rrc_MeasurementCommand_r9_modify:
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_MODIFY;
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        & rrc_MeasurementCommand_r9_modify,measurementType))
      {
        l1_qual_meas_params->meas_object_info_included = TRUE;
        
        l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = FALSE;
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement,
          qualityReportingQuantity))
        {
          l1_qual_meas_params->meas_object.u.qual.rpt_quan_included = TRUE;
          
          if((RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->reportCriteria,
            rrc_QualityReportCriteria,qualityReportingCriteria))
            &&
            (rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->qualityReportingQuantity.
            dl_TransChBLER == TRUE))
          {
            WRRC_MSG0_ERROR("REL9:Modify:BLER rpt set to true for evnt trigg meas");
            /* Set faliure t */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }  
          
          if(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.qualityMeasurement->
            qualityReportingQuantity.modeSpecificInfo,
             rrc_QualityReportingQuantity_modeSpecificInfo_tdd))
          {
            WRRC_MSG0_ERROR("REL9:Modify:qual rept quantity for tdd chosen");
            /* Set faliure t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationUnsupported;
            rrcmeas_mcf.rrc_transaction_id = transaction_id;
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
            rrc_free(l1_qual_meas_params);
            return FAILURE; 
          }
        }
        if(FAILURE == rrcqm_fill_mcm_info(rrc_mcm_ptr->measurementCommand.u.modify->
        measurementType.u.qualityMeasurement,&l1_qual_meas_params->meas_object.u.qual,
        transaction_id, &mcm_rpt_crit))
        {
          rrc_free(l1_qual_meas_params);
          return FAILURE;
        }        
      }
      else
      {
        WRRC_MSG0_HIGH("REL9:Quality:Modify:MType absent");
      }
      /* validate the additional measurements*/
      if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_id,&addtl_meas_incl,primary_meas_type,meas_id, &meas_validity) == FAILURE)
      {
        rrc_free(l1_qual_meas_params);
        return FAILURE;
      }
            
      
      for ( count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        /* this meas already exists */
        if (quality_meas_id_list[count].meas_id == l1_qual_meas_params->meas_id)
        {
          quality_meas_id_list[count].rpt_crit = mcm_rpt_crit;
          quality_meas_id_list[count].meas_validity = STATE_DCH;
          break;
        }
      } 
      break;
      
    case T_rrc_MeasurementCommand_r9_release:
      
      
      l1_qual_meas_params->meas_cmd = L1_MEAS_RELEASE;
      /* Delete this Measurement */
      for (count = 0; count < MAX_QUALITY_MEAS; count++ )
      {
        if (quality_meas_id_list[count].meas_id
          == meas_id)
        {
          WRRC_MSG1_HIGH("REL9:Delete Meas %d",meas_id);
          quality_meas_id_list[count].meas_id = INVALID_MEAS_ID;
          
          quality_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
          
          /* Also initialize additional meas */
          for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
            quality_meas_id_list[count].additional_meas[cnt]
            = INVALID_MEAS_ID;
          break;
        }
      }                
      
      alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
      l1_qual_meas_params->trans_id = alloc_idx;
      rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
      rrc_free(l1_qual_meas_params);
      return SUCCESS;    
      
    default:
      break;
  }
  
  cnt = 0; 
 
  additionalMeasurementList_ptr = &rrc_mcm_ptr->additionalMeasurementList;
  while((additionalMeasurementList_ptr->n > cnt) && (cnt < MAX_ADDITIONAL_MEAS))
  {
    l1_qual_meas_params->add_meas[cnt] =  (uint16) additionalMeasurementList_ptr->elem[cnt];
    cnt++;
  }
  l1_qual_meas_params->num_add_meas = cnt;
  if (addtl_meas_incl == TRUE)
  {
    rrcmeas_check_addtl_meas_in_db(&primary_meas_type, mcm_r9,
      meas_id);
    WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
  }

   /* Transaction Id would be stored in transaction list by the calling function
  if success is returned. Measurement Id has already been stored */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  l1_qual_meas_params->trans_id = alloc_idx;
  rrcmeas_send_l1_cmd(NULL, l1_qual_meas_params, NULL, L1_MEAS_CTRL_PARMS);
  
  
  rrc_free(l1_qual_meas_params);
  return SUCCESS;
}

#endif

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /*WCDMA_REL10*/

