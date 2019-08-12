/*===========================================================================
                   RRC TRAFFIC VOLUME MEASUREMENTS  

DESCRIPTION

      This module supports traffic Volume Measurements from the UE to UTRAN.
      
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrctvm.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/09/16   nr    Made changes to remove irrelevant F3s
01/21/15   sa    Made changes for QTF with CRM
05/21/14   vi    Made changes to check for DCH->FACH and FACH->DCH transition and measurement validity before configuring measurement to MAC
05/16/14   vi    Made changes to support MOB framework
02/01/14   sp    Made changes to avoid crash while setting TVI in CU when Mac is waiting for L1 signal.  
12/03/13   mn    Moved all declarations to top of the code.
12/03/13   mp    Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
06/07/13   vi    Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/28/13   sr    Added code for measurement identity extension
04/25/13   sg    Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp    Removed tmc.h include file
03/21/13   mp    Fixing HSRACH featurization error
01/10/13   sa    Made changes to update the event criteria list based on the ul trch type
01/03/13   ad    Made changes to reduce the redundant F3s
12/12/12   mp    Fixing KW errors
10/16/12   db    Missing changes for QCHAT on Dime 
08/17/12   sa    Made the changes to delete the SIB TVM on reselections
07/24/12   geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad    Changes for HS-RACH feature
03/13/12   mp    added code to save, stop and resume TVM in state transition 
                 DCH->PCH->FACH/DCH.
05/03/11   rl    Fixed the lint error
04/26/11   rl    Support for transition from TMC heap to Modem Heap in WCDMA.
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad    Made changes in RRC related to L2 SW Decoupling
01/12/11   ad    Added changes for RRC SW decoupling
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/29/10   su    Made changes to remove WCDMA_TASKLOCK() usage from RRC code base.
11/03/10   rl    Fixed compiler warnings
10/30/10   rl    Objective systems migration changes check in 
                 on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm    Added code to support LTE<->WCDMA Inter RAT
01/22/10   rm    Modified the usage of qchatupk_wcdma.h with rrc_qchatupk.h
                 (at the request of QCHAT team)
11/27/09   gkg   RRC changes required to integrate QChat Porting Kit Optimizations 
                 into AMSS. The changes are under feature FEATURE_QCHAT.
10/07/09   gkg   Changes for REL8 under FEATURE_WCDMA_REL8
09/11/09   ps    Made changes for reducing the internal RRC command size 
                 under FEATURE_RRC_REDUCE_CMD_SIZE
05/08/09   ss    Updated Copyright Information
04/29/09   ss    Made changes to compiler warnings on gcc compilers.
01/19/08   ps    Made changes for ASn1 migration 25.331v7.9  
                 i.e.May 2008 version  (2008-05) 
11/10/08   rm    Made changes to process Rel7 measurement control message
09/12/08   ps    Added code for REl7 under FEATURE_WCDMA_REL7. ASN1 migration
                 to march 08 ver
09/02/08   cnp   Replaced intlock/free with wcdma_intlock/free.
           cnp   Replaced tasklock/free with wcdma_tasklock/free.
           cnp   Replaced intlock/free_sav with wcdma_intlock/free_sav.
07/16/08   rm    Enhanced the handling of transaction id received in MCM message so that
                 UE accepts the MCM's with same transaction id and in case of failure UE
                 sends MCM failure for a transaction id only if it is latest among the same
                 transactions ids
02/13/08   rm    Made changes to check whether the traffic volume indicator can be set in
                 cell update message under FEATURE_TVM_IND_IN_CU
09/07/07   vm    Added support for feature FEATURE_RRC_CALLOC
07/17/07   rm    Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
04/20/07   da    Made changes needed to update ASN.1 version to R6 sept R6 2006
02/05/07   sgk   Moved the rrc_event_crit_ptr null check to the beginning of
                 the function to prevent posible null pointer dereference. 
02/02/07   vm    Stability Opt: Removed freeing of already allocated memory
                 before hitting EF. This is to get the "correct" snapshot
                 of the heap instead of a "stale" one.  
10/09/06   vg    made changes for MCM modify additinal meas handling
09/25/06   kp      Added support for sib_change_ind interface change
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases. 
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under 
                 FEATURE REL5 while retaining the HSDPA specifc processing 
                 under FEATURE HSDPA. 
04/18/06   da    Modified code to support migration to Mar-06 Rel-5 ASN1 +
                 R6 Hacks
04/06/06   sgk   Added code to copy measurement validity to the measurement
                 id db always for setup.
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
11/10/05   sgk   Made code changes to satisfy lint.
08/05/05   sgk   Modified code to set flag exclude_modified_cell_results to 
                 false, when calling func rrcmeas_append_meas_rpt_on_rach.  This
                 is to allow adding of monitored cell results if present.
07/13/05   sgk   Added rel5 non critical extensions for rel 99 MCM ie the 
                 transaction identifier extension.
06/27/05   sgk   Added basic rel 5 measurement ctrl msg support.
05/26/05   sgk   Removed the check to send MCF if measurement validity is set
                 to all states but the tvm object list in absent in the MCM
                 modify. The check is retained for setup.                 
05/12/05   sgk   Added check to send a measurement control failure with cause 
                 configuration incomplete if the measurement validity is set 
                 to all states but the traffic volume measurement object list 
                 is not provided in both rel 99 and rel 5.  
05/03/05   vk    Added code that processes Release 5 Traffic Volume Measurement
                 Control Message
04/14/05   sgk   Added check to reject quality measurement as an additional 
                 measurement to tvm. 
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/02/05  ttl    Support RRC ASN1 Release 5.
01/10/05   sgk   Added procedure id as second parameter to function call
                 rrcmeas_append_meas_rpt_on_rach, in function
                 rrctvm_send_traffic_vol_meas_report.
09/24/04   vk    ReAdded multiplication factor when meas quantity is average or
                 variance
09/16/04   vk    Set Block RACH ind in measurement identity variable correctly
                 for SIB11/12. Removed the muiltiplication factor when
                 Average or Variance meas quantity is specified. Also fixed previous merge problem
09/13/04   vk    Indicate MAC to block txion on RACH only if report specified 
                 is Event 4A report. Also in case network sets txion interruption
                 time after trigger for DCH object, then ignore it
08/24/04   vk    Added fix for Interruption time after trigger issue. Now RRC correctly
                 indicates MAC to block txions on RACH when it receives event 4A. Also
                 added change to not overwrite MCM owned measurement in case SIB11/12
                 has the same measurement on DCH<-->FACH transition.
05/19/04   vk    Implemented fn rrctvm_process_sib_change_notification() to process
                 SIB11/12 change notification. Also, send cmd to MAC if validity 
                 is ALL to resume a meas so that MAC could switch the Meas Objects.
                 Reformatted MSG_HIGH's 
04/06/04   vk    Removed redundant macro DEFAULT_TRAFFIC_VOL_MEAS_IDENITY,
                 QUANTITY_NOT_PRESENT (Lint warning) as it has already been 
                 declared in rrcmeasi.h
03/25/03   vk    Allow a blank measurement report to go in case no of RB's are reported
                 to be 0
03/10/04   vk    Send blank Measurement Report in case measured results are unavailable
02/05/03   vk    Indicate to MAC through a boolean if measurements need to
                 be started right away with a start cmd
12/18/03   vk    Lock the task before calling MAC fn to append measurements
12/18/03   vk    Corrected appending of more than one additional measurements to
                 a measurement report. Lock the task before calling MAC fn to 
                 append measurements
12/16/03   vk    Initialize bit mask while appending Traffic Volume Meas Results
11/13/03   vk    Added support to allow Measured Results on RACH to be appended
                 in case a Traffic Volume Measurement Report is sent in CELL_FACH
                 Also allowed processing for multiple measurements on transitioning
                 from FACH->DCH
11/07/03   vk    Added check to verify that additional measurements corresponding
                 to a primary measurement can never be greater than the maximum
                 allowed
11/06/03   vk    Added checking of UE Internal Measurements as additional meas
                 under FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENTS
10/22/03   vk    Moved version before the last checked in version back to tip
07/21/03   vk    Store Interruption time after trigger if present for RACH
                 and then indicate MAC when to block DTCH transmissions
                 on RACH
06/03/03   vk    Register for L2 ack while sending a traffic volume meas rpt
                 in CELL_DCH state
05/30/03   vk    While deleting primary measurement, reinitialize reporting
                 criteria, measurement validity to default values. In case of
                 multiple traffic volume measurements, make sure that on
                 DCH-->FACH transitions they are deleted from addtl meas
                 DB when the primary traffic volume meas is deleted
05/30/03   vk    Saved reporting criteria in RRC when receiving a setup that
                 includes additional measurements
05/29/03   vk    Added code for Implementing Additional Measurements for
                 Traffic Volume Measurements
04/14/03   vk    1. Compared correctly if Variance payload is present in a Meas Report
                 2. While processing a Meas Ctrl Message to Release the Measurement,
                    set ack flag to TRUE so that accepted transaction table is cleared
                    of the transaction identifier once MAC sends the ack
02/25/03   ram   Even if SIB11 indicates that SIB12 is present, it is not gauranteed
                 to be present in the cell. Hence is SIB12 is not present in the
                 SIB database we should assume it was not present in the cell and use
                 SIB11 instead. Changed the code to reflect this.
02/20/02   vk    Set individual reporing quantities to FALSE before processing
                 the traffic volume reporting quantity
02/14/02   vk    1. Fixed most of Lint Errors 
                 2. Defined INVALID_PAYLOAD to make sure that average and variance
                    are not appended in the measurement report if MAC does not
                    send them 
02/05/03   sk    March2002Rel specific changes.
                 Changed    rrc_UL_TrCH_Identity_rach_chosen to  rrc_rachorcpch_chosen
12/16/02   vk    Changed rrc_malloc to mem_malloc while sending cmd to MAC since
                 MAC uses mem_free for the embedded buffer
10/28/02   vk    Removed feature define FEATURE_DCH_FACH_MEAS
10/24/02   rj    Updated to use rrc_malloc and rrc_free to use dual heaps 
                 in place of mem_malloc and mem_free to avoid memory
                 fragmentation
10/11/02   vk    Turned some MSG_HIGH's to MSG_LOW's for DCH<-->FACH transitions
10/09/02   vk    Merged code from branch in main line to handle intra-freq 
                 measuremenmts and traffic volume measurements on DCH<-->FACH 
                 transitions under feature FEATURE_DCH_FACH_MEAS
10/08/02   vk    Added code to handle Measurement Identity List
09/23/02   vk    Declared functions rrctvm_process_sibs_on_dch_to_fach(..)
                 and rrctvm_process_sibs_on_dch_to_fach(...) to process
                 traffic volume measurements on state transition. 
08/19/02   vk    Added processing of DCH-->FACH transitions under
                 FEATURE INTER RAT MEAS
07/29/02   vk    Added additional checking while processing event criteria
07/09/02   vk    Added code for processing Traffic Volume Measurements
                 in CELL_FACH state of RRC. Also added changes done 
                 during integration with MAC
06/27/02   vk    Reinitialized event criteria list on getting Meas
                 Control Message 
06/14/02   vk    Changed event criteria pointer to store params while
                 processing measurement control message
06/12/02   vk    Corrected printing of comments in previous checkin
05/29/02   vk    Corrected printing of Average RLC payload  
                 while processing Periodic Results  
05/17/02   vk    Created File for handling Traffic Volume Measurements. 
                 Implements Traffic Volume Measurements


===========================================================================*/



/*==========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "customer.h"
#include "rrcsend.h"
#include "rrclcm.h"
#include "uecomdef.h"
#include "rrcscmgr.h"
#include "msg.h"
#include "rrccmd_v.h"
#include "rrcmeasi.h"
#include "rrcsibdb.h"
#include "rrcmeas.h"
#include "rrcasn1util.h"
#include "mactraffic.h"
#include "rrcllc.h"
#include "macrrcif_v.h"

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /*FEATURE_QCHAT*/

#ifdef FEATURE_WCDMA_HS_RACH
#include "rrcllcoc.h"
#endif
#ifdef TEST_FRAMEWORK
#error code not present
#endif


/* -----------------------------------------------------------------------
** Extern Declarations
** ----------------------------------------------------------------------- */

extern sib12_present_e_type sib12_present;
extern uint8 rrcmeas_pending_rpt;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Default Traffic Vol Meas Identity  */

#define INVALID_PAYLOAD 0xffffffff



/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Object Definition
** ----------------------------------------------------------------------- */
// Stores SIB params
mac_ul_traffic_meas_config_type  mac_sib_params;


/*-------------------------------------------------------------------
STRUCTURE: tvm_meas_id_list_struct_type
Stores the traffic volume Meas Id's
--------------------------------------------------------------------*/
tvm_meas_id_list_struct_type     tvm_meas_id_list[MAX_TRAFFIC_VOLUME_MEASUREMENTS]; 


/*-------------------------------------------------------------------
ENUM: rrcmeas_current_substate
Stores substate of RRC Measurement procedure
--------------------------------------------------------------------*/
extern rrcmeas_substate_e_type rrcmeas_current_substate; 

extern boolean dch_to_pch_in_progress;

#ifdef FEATURE_QSH_EVENT_METRIC
  extern boolean rrc_qsh_dl_committed;
#endif

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================

FUNCTION   rrctvm_check_if_edch_object_exists

DESCRIPTION 

Checks if EDCH object and TrCh ID exists in the objects list or not
 
DEPENDENCIES

  None

RETURN VALUE

Boolean

SIDE EFFECTS


===========================================================================*/

boolean rrctvm_check_if_edch_object_exists
(
mac_ul_traffic_meas_config_type*  mac_config_ptr,
tr_ch_id_type trch_id
)
{
  uint8 cnt = 0;

  if(mac_config_ptr->meas_obj_incl == TRUE)
  {
    for(cnt=0;cnt<mac_config_ptr->meas_obj_list.num_trch;cnt++)
    {
      if((mac_config_ptr->meas_obj_list.trch_info[cnt].trch_type == EDCH) && (mac_config_ptr->meas_obj_list.trch_info[cnt].trch_id== trch_id))
      {
        WRRC_MSG1_HIGH("TVM:EDCH trch found for trch_id %d", trch_id);
        return TRUE;
      }
    }
  }
  WRRC_MSG1_HIGH("TVM:EDCH trch not found for trch_id %d", trch_id);
  return FALSE;
}
#endif
/*===========================================================================

FUNCTION     RRCTVM_PROCESS_SIB_INFO

DESCRIPTION  

    Process traffic volume information from SIB11/12 
                    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_sib_info
(
rrc_MeasurementControlSysInfo* sib_meas_ptr
)
{
  /* SIB params stored in MAC structure  mac_sib_params */  
  rrc_TrafficVolumeMeasurementObjectList * mac_obj_ptr = NULL;
  
  /* No of transport channels */
  uint8 transport_channel_count = 0;
  
  uint8 count = 0;
  
  uint8 cnt = 0;
  boolean block_set = FALSE;
  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib_meas_ptr,trafficVolumeMeasSysInfo))
  {
      MSG_LOW("Meas Identity present", 0,0,0);
      
      /* Set Meas Identity in traffic volume meas identity variable */
      mac_sib_params.meas_id = (uint16) 
        sib_meas_ptr->trafficVolumeMeasSysInfo.trafficVolumeMeasurementID;
    
    
    WRRC_MSG1_HIGH("TVM:Sys Info present. Meas Id is %d", mac_sib_params.meas_id);
    /* Set Meas Id and Validity */
    rrctvm_set_meas_identity(&mac_sib_params.meas_id, &sib_meas_ptr->trafficVolumeMeasSysInfo); 
    
    if (RRC_MSG_COMMON_BITMASK_IE(sib_meas_ptr->trafficVolumeMeasSysInfo,
        rrc_TrafficVolumeMeasSysInfo,measurementValidity))
    {
      
      if (((sib_meas_ptr->trafficVolumeMeasSysInfo.measurementValidity.ue_State == rrc_MeasurementValidity_ue_State_cell_DCH) 
           && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
        ((sib_meas_ptr->trafficVolumeMeasSysInfo.measurementValidity.ue_State == rrc_MeasurementValidity_ue_State_all_But_Cell_DCH) 
         && (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
      {
        WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
        mac_sib_params.meas_start_incl = FALSE;
      }
    }
    
    
  } /* End of if of sib12 for traffic vol info present */
  
  else
  {
    WRRC_MSG0_HIGH("TVM:No Sys Info present");
    return TVM_FAIL;
  }   
  
  if (RRC_MSG_COMMON_BITMASK_IE(sib_meas_ptr->trafficVolumeMeasSysInfo,rrc_TrafficVolumeMeasSysInfo,trafficVolumeMeasurementObjectList))
  {
    
    
    mac_obj_ptr = 
      &sib_meas_ptr->trafficVolumeMeasSysInfo.trafficVolumeMeasurementObjectList;
    
    mac_sib_params.meas_obj_incl = TRUE;
    
    while ((mac_obj_ptr->n > cnt) && (transport_channel_count < UE_MAX_TRCH))
    {
      if (RRC_CHECK_COMMON_MSG_TYPE(mac_obj_ptr->elem[cnt],rrc_UL_TrCH_Identity_usch))
      {
        MSG_LOW("USCH object given. Ignoring",0,0,0); 
      }
      else if (mac_obj_ptr->elem[cnt].t ==  T_rrc_UL_TrCH_Identity_rachorcpch)
      {
        WRRC_MSG0_HIGH("TVM:RACH object specified");
        /* Set channel type */
        
        mac_sib_params.meas_obj_list.trch_info[transport_channel_count].trch_type = RACH;
        
        /* Increment count */
        transport_channel_count++;
        
      }
      else
      {
        WRRC_MSG1_HIGH("TVM:DCH object %d specified",mac_obj_ptr->elem[cnt].u.dch);
        
        /* Set channel type */
        mac_sib_params.meas_obj_list.trch_info[transport_channel_count].
          trch_type = DCH;
#ifdef FEATURE_WCDMA_HS_RACH
          if (RRC_MSG_COMMON_BITMASK_IE(sib_meas_ptr->trafficVolumeMeasSysInfo,
          rrc_TrafficVolumeMeasSysInfo,measurementValidity))
        {
        
          if(sib_meas_ptr->trafficVolumeMeasSysInfo.measurementValidity.ue_State == rrc_MeasurementValidity_ue_State_all_But_Cell_DCH)
          {
                    /* Set channel type */
            WRRC_MSG0_HIGH("TVM:HSRACH OBJ Set as EDCH");
            mac_sib_params.meas_obj_list.trch_info[transport_channel_count].
            trch_type = EDCH;
          }
        }
#endif
        /* Get Channel ID too */
        mac_sib_params.meas_obj_list.trch_info[transport_channel_count].trch_id = (uint16)
          mac_obj_ptr->elem[cnt].u.dch; 
        
        transport_channel_count++;
      }
      cnt++;
      if ((mac_obj_ptr != NULL) && (transport_channel_count == UE_MAX_TRCH))
      {
        WRRC_MSG0_HIGH("TVM:All transport Ch not included");
        break;
      }
      continue;
      
    }   /* End of while */
    
    mac_sib_params.meas_obj_list.num_trch = transport_channel_count;  
    
    WRRC_MSG1_HIGH("TVM:Meas Obj No of TransCh %d", transport_channel_count);
    
  } /* End of meas object present */
  
  /* Check if Meas Quantity present */
  if (RRC_MSG_COMMON_BITMASK_IE(sib_meas_ptr->trafficVolumeMeasSysInfo,
    rrc_TrafficVolumeMeasSysInfo,trafficVolumeMeasQuantity))
  {
    WRRC_MSG1_HIGH("TVM:Meas Quan %d RLC/A/V:0/1/2", sib_meas_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeMeasQuantity.t);
    
    mac_sib_params.meas_qty_incl = TRUE;
    
    if (RRC_CHECK_COMMON_MSG_TYPE2(sib_meas_ptr->trafficVolumeMeasSysInfo.trafficVolumeMeasQuantity,
        rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
    {
      mac_sib_params.meas_qty.meas_qty_enum = RLC_BUF_PAYLOAD;
      mac_sib_params.meas_qty.meas_interval = QUANTITY_NOT_PRESENT;
    }
    else if (RRC_CHECK_COMMON_MSG_TYPE2(sib_meas_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,averageRLC_BufferPayload))
    {
      mac_sib_params.meas_qty.meas_qty_enum = AVERAGE_RLC_BUF_PAYLOAD;
      /* Put time interval too */
      mac_sib_params.meas_qty.meas_interval = 
        sib_meas_ptr->trafficVolumeMeasSysInfo.trafficVolumeMeasQuantity.
        u.averageRLC_BufferPayload;
      mac_sib_params.meas_qty.meas_interval = mac_sib_params.meas_qty.meas_interval * MEAS_QUANTITY_INTERVAL_OFFSET;
    }
    else
    {
      mac_sib_params.meas_qty.meas_qty_enum = VARIANCE_RLC_BUF_PAYLOAD;
      /* Put time interval too */
      mac_sib_params.meas_qty.meas_interval = sib_meas_ptr->trafficVolumeMeasSysInfo.
        trafficVolumeMeasQuantity.u.varianceOfRLC_BufferPayload;
      mac_sib_params.meas_qty.meas_interval = mac_sib_params.meas_qty.meas_interval * MEAS_QUANTITY_INTERVAL_OFFSET;
    } 
    
    
  } /* End of if of Meas Quantity present */
  else
  {
    WRRC_MSG0_ERROR("TVM:Meas Quantity absent");
    return TVM_FAIL;
    /* Do not send command to MAC */
  }
  
  /* Check if Reporting Quantity present */
  if (RRC_MSG_COMMON_BITMASK_IE(sib_meas_ptr->trafficVolumeMeasSysInfo,
    rrc_TrafficVolumeMeasSysInfo,trafficVolumeReportingQuantity))
  {
    mac_sib_params.meas_report_incl = TRUE;
    
    if (sib_meas_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeReportingQuantity.rlc_RB_BufferPayload == TRUE)
    {
      mac_sib_params.meas_report_qty.rlc_payload = TRUE;
    }
    if (sib_meas_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE)
    {
      mac_sib_params.meas_report_qty.avg_payload = TRUE;
    }
    if (sib_meas_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE)
    {
      mac_sib_params.meas_report_qty.var_payload = TRUE;
    }
    MSG_HIGH("TVM:Rpting Quan RLC%d A%d V%d", mac_sib_params.meas_report_qty.rlc_payload,
      mac_sib_params.meas_report_qty.avg_payload, 
      mac_sib_params.meas_report_qty.var_payload);
  }   /* End of reporting quantity present */
  
  /* To read traffic volume reporting mode */
  mac_sib_params.meas_report_mode.transfer_mode = 
    TVM_RET_REPORTING_MODE(sib_meas_ptr->trafficVolumeMeasSysInfo.
    measurementReportingMode.measurementReportTransferMode);

  WRRC_MSG1_HIGH("TVM:Rpting Mode %d ACK:0", mac_sib_params.meas_report_mode.transfer_mode);
  
  
  /* ReportingCriteria */
  if (RRC_CHECK_COMMON_MSG_TYPE(sib_meas_ptr->trafficVolumeMeasSysInfo.
    reportCriteriaSysInf,
    rrc_TrafficVolumeReportCriteriaSysInfo_trafficVolumeReportingCriteria))
  {
    if ( (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib_meas_ptr->trafficVolumeMeasSysInfo.reportCriteriaSysInf.u.trafficVolumeReportingCriteria,
      transChCriteriaList)) &&
      (sib_meas_ptr->trafficVolumeMeasSysInfo.
      reportCriteriaSysInf.u.trafficVolumeReportingCriteria->transChCriteriaList.n == 0))
    {
      WRRC_MSG0_ERROR("TVM:Empty event crit structure");
      return TVM_FAIL;
    }
    
    
    
    
    if ((rrctvm_process_event_criteria(&sib_meas_ptr->trafficVolumeMeasSysInfo.reportCriteriaSysInf.u.
        trafficVolumeReportingCriteria->transChCriteriaList,
        &mac_sib_params.meas_report_criteria.event_cfg
#ifdef FEATURE_WCDMA_HS_RACH
    ,&mac_sib_params 
#endif
      )) == TVM_FAIL) 
    {
      WRRC_MSG0_HIGH("TVM:Meas Req to MAC not sent");
      return TVM_FAIL;
    }
    
    /* Now set to Event Triggered Mode */
    mac_sib_params.meas_report_mode.report_mode = EVENT_TRIGGERED_MODE;
    
    /* Set meas_criteria to TRUE */
    mac_sib_params.meas_criteria_incl = TRUE;
    
    /* Check if Block intption on RACH present */
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        mac_sib_params.meas_id)
      {
        if (mac_sib_params.meas_report_mode.report_mode == 
          EVENT_TRIGGERED_MODE) 
          
        {
          for (transport_channel_count = 0; transport_channel_count < mac_sib_params.meas_report_criteria.event_cfg.num_trch; transport_channel_count++)
          {
            for (cnt = 0; cnt < mac_sib_params.meas_report_criteria.event_cfg.trch_event_cfg[transport_channel_count].num_events; cnt++)
            {
              if ((mac_sib_params.meas_report_criteria.event_cfg.trch_event_cfg[transport_channel_count].event_param[cnt].event_id == EVENT_4A) &&
                (mac_sib_params.meas_report_criteria.event_cfg.trch_event_cfg[transport_channel_count].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
              {
                tvm_meas_id_list[count].block_rach_incl = TRUE;
                WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
                
                block_set = TRUE;
                break; 
              } /* End of if of event_id and tx_interrupt_tmr check */
            } /* End of for */
            if (block_set == TRUE)
            {
              WRRC_MSG0_HIGH("Block RACH config finished");
              break;
            }
          } /* end of 2nd for */
          break;
        } /* End of if for Event Triggered Mode */
        break;
      } /* End of if */
      
    } /* End of for loop */ 
    
  }
  else
  {
    
    
    /* Set meas_criteria to TRUE */
    mac_sib_params.meas_criteria_incl = TRUE;
    
    mac_sib_params.meas_report_mode.report_mode = PERIODIC_MODE;
    
    /* Set default no of reports. TRUE indicates infinity */
    mac_sib_params.meas_report_criteria.periodic_cfg.report_infinity = TRUE;
    
    /* Get periodic information */
   
    mac_sib_params.meas_report_criteria.periodic_cfg.num_reports = 
      TVM_RET_REPORTING_AMOUNT(sib_meas_ptr->
      trafficVolumeMeasSysInfo.reportCriteriaSysInf.
      u.periodicalReportingCriteria->reportingAmount);
  
  WRRC_MSG1_HIGH("TVM:Rpting Amt is %d", 
       mac_sib_params.meas_report_criteria.periodic_cfg.num_reports);
    
    if (sib_meas_ptr->trafficVolumeMeasSysInfo.reportCriteriaSysInf.
      u.periodicalReportingCriteria->reportingAmount != rrc_ReportingAmount_ra_Infinity)
    {
      mac_sib_params.meas_report_criteria.periodic_cfg.report_infinity = FALSE;
    }
    
    
  
    
    /* Get Reporting Interval */
    mac_sib_params.meas_report_criteria.periodic_cfg.report_interval = 
      TVM_RET_PERIODIC_REPORTING_INTERVAL(sib_meas_ptr->
      trafficVolumeMeasSysInfo.reportCriteriaSysInf.u.
      periodicalReportingCriteria->reportingInterval);

  WRRC_MSG1_HIGH("TVM:Rpting Intvl is %d msec", 
       mac_sib_params.meas_report_criteria.periodic_cfg.report_interval);
    
      /* Check for periodic report, reporting quantity must be present
    * If not present, then do not send meas command to MAC */
    if (mac_sib_params.meas_report_incl != TRUE)
    {
      WRRC_MSG0_ERROR("TVM:Periodic Report: No Rep Quantity");
      return TVM_FAIL;
    }
  } /* End of else of periodic reporting  */
  
  /* Store Reporting Criteria */
  rrctvm_set_reporting_criteria(&mac_sib_params.meas_id, &sib_meas_ptr->
    trafficVolumeMeasSysInfo.reportCriteriaSysInf);
  
  return TVM_SUCCESS; 
}

/*===========================================================================

FUNCTION     rrctvm_check_if_meas_is_valid_for_current_state

DESCRIPTION  

This function checks for the meas validity in the current state.
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    FALSE if meas is not valid in current state, otherwise TRUE

SIDE EFFECTS

    None

===========================================================================*/

static boolean rrctvm_check_if_meas_is_valid_for_current_state(meas_validity_enum_type prim_meas_validity )
{
    rrcllc_oc_set_status_e_type oc_status;
       /*Get OC status*/
    oc_status = rrcllc_get_ordered_config_status();

   /* Check for meas validity and current state/transition mismatch*/

    if (((prim_meas_validity == STATE_DCH) && ((rrcmeas_current_substate == RRCMEAS_CELL_FACH)|| (oc_status == OC_SET_FOR_DCH_FACH_TRANS) || (oc_status == OC_SET_FOR_DCH_CELL_PCH_TRANS) || (oc_status == OC_SET_FOR_DCH_URA_PCH_TRANS)))
        ||
        ((prim_meas_validity == ALL_STATES_BUT_DCH) && ((rrcmeas_current_substate == RRCMEAS_CELL_DCH) || ((RRC_STATE_CELL_FACH == rrc_get_state()) && (oc_status == OC_SET_FOR_CELL_DCH)) )) 
      )
    {
      return FALSE;
    }
   return TRUE;
}


/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info
(
rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
, uint32 meas_ctrl_tr_id
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;
  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  
  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  
  boolean block_set = FALSE;

  uint32 transaction_id = INVALID_TRANSACTION_ID;
  /* Assign some default values */
  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  mac_mcm_params.meas_start_incl = TRUE;
  

  transaction_id = meas_ctrl_tr_id;
  
  
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_setup)
  {
  /* Although we came here bec it is TVM, still make check to ensure UTRAN
    * has not messed up */
    if (rrc_mcm_ptr->measurementCommand.u.setup->t != 
      T_rrc_MeasurementType_trafficVolumeMeasurement)
    {
      WRRC_MSG1_HIGH("TVM:Incorrect Meas params for id %d",rrc_mcm_ptr->measurementIdentity);
      /* Send Meas Ctrl Failure Message */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement, 
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }
    
    /* Process Additional Meas */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */

      if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList, 
                                          transaction_id, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        return TVM_FAIL;
      }
      
      /* This means additional meas has been successfully validated */
      addtl_meas_validation = TRUE;
      
    } /* End of if of Additional Meas Present */
    
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_id;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);
    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
      
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
    }
    
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_modify)
  {
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_id;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
            rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }
        
        /* Process Additional Meas */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
        {
          WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
          
          if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
            transaction_id, rrc_mcm_ptr->measurementIdentity) == FALSE)
          {
            /* Called function has already sent Meas Ctrl Failure */
            return TVM_FAIL;
          }
          
          /* This means additional meas has been successfully validated */
          addtl_meas_validation = TRUE;

         /* End of if of Additional Meas Present */
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_id;

            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

            /* Send MCF msg to UTRAN */
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);

            return TVM_FAIL;

          }
        }
        
        rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
        
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }
        
        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
      
      
      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r3_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
        

        if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
          transaction_id, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          return TVM_FAIL;
        }
        
        /* This means additional meas has been successfully validated */
        addtl_meas_validation = TRUE;
        
      } /* End of if of Additional Meas Present */
      if (rrcmeas_check_mcm_for_setup_modify_additional(
        addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
      {
        /* As UTRAN tried to configure more than 4 Addtl Meas */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
        
      }
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }
      
      
      
      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
      }
      
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_id;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        rrc_mcm_ptr->measurementIdentity)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      rrc_mcm_ptr->measurementIdentity)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */
  
  
  /* Now send config command to MAC */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_id);
  mac_mcm_params.config_ack_num = alloc_idx;
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}




/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R5

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R5. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r5
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;

  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint32 transaction_identifier = 0;

  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  
  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  boolean block_set = FALSE;
  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  /* Assign some default values */
  mac_mcm_params.meas_start_incl = TRUE;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4;

  /* Store the Transaction Identifier */
  transaction_identifier = tid;
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_setup)
  {
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement, 
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }

    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
 
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
        transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        return TVM_FAIL;
      }
      
      /* This means additional meas has been successfully validated */
      addtl_meas_validation = TRUE;
      
    } /* End of if of Additional Meas Present */
    
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);

    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
      
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
    }
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r4_modify)
  {
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify
        ,rrc_MeasurementCommand_r4_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_r4_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
           rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }

        /* Process Additional Meas */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
        {
          WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

          if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
            transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
          {
            /* Called function has already sent Meas Ctrl Failure */
            return TVM_FAIL;
          }
          
          /* This means additional meas has been successfully validated */
          addtl_meas_validation = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            
          
            rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
            return TVM_FAIL;
          
          }
          
        } /* End of if of Additional Meas Present */
        
        
        rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
        
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }

        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);

      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

        if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
          transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          return TVM_FAIL;
        }
        
        /* This means additional meas has been successfully validated */
        addtl_meas_validation = TRUE;
        
      } /* End of if of Additional Meas Present */
      
      if (rrcmeas_check_mcm_for_setup_modify_additional(
        addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
      {
        /* As UTRAN tried to configure more than 4 Addtl Meas */
        
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
        
      }
      
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }

      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
      }
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        rrc_mcm_ptr->measurementIdentity)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      rrc_mcm_ptr->measurementIdentity)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */
  
  
  /* Now send config command to MAC */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
  mac_mcm_params.config_ack_num = alloc_idx;
  WRRC_MSG0_HIGH("TVM:Sending rrctvm_send_mac_command");
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}




/*===========================================================================

FUNCTION     RRCTVM_READ_TRAFFIC_VOL_PARAMS

DESCRIPTION  

    Read traffic volume information in a Meas
    Ctrl Message. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    Sucess/Failure.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_read_traffic_vol_params
(
rrc_TrafficVolumeMeasurement* rrc_mcm_ptr,
mac_ul_traffic_meas_config_type*  mac_config_ptr
)
{
  rrc_TrafficVolumeMeasurementObjectList * mac_obj_ptr = NULL;
  uint32 idx=0;
  /* No of transport channels */
  uint8 transport_channel_count = 0;

  /* Initialize Measured Interval */
  mac_config_ptr->meas_qty.meas_interval = QUANTITY_NOT_PRESENT;

  /* Initialize individual reporting quantities */
  mac_config_ptr->meas_report_qty.rlc_payload = FALSE;
  mac_config_ptr->meas_report_qty.avg_payload = FALSE;
  mac_config_ptr->meas_report_qty.var_payload = FALSE;
  
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, 
    rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))
  {
    
    
    mac_obj_ptr = 
      &rrc_mcm_ptr->trafficVolumeMeasurementObjectList;
    
    mac_config_ptr->meas_obj_incl = TRUE;
    
    while ((mac_obj_ptr->n > idx) && (transport_channel_count < UE_MAX_TRCH))
    {
      if (mac_obj_ptr->elem[idx].t == T_rrc_UL_TrCH_Identity_usch)
      {
        WRRC_MSG0_HIGH("TVM:USCH object given. Ignoring"); 
      }
      else if (mac_obj_ptr->elem[idx].t ==  T_rrc_UL_TrCH_Identity_rachorcpch)
      {
        WRRC_MSG0_HIGH("TVM:RACH object specified");
        /* Set channel type */
        mac_config_ptr->meas_obj_list.trch_info[transport_channel_count].
          trch_type = RACH;
        
        /* Increment count */
        transport_channel_count++;
        
      }
      else
      {
        WRRC_MSG1_HIGH("TVM:DCH object %d specified",mac_obj_ptr->elem[idx].u.dch);
        
        /* Set channel type */
        mac_config_ptr->meas_obj_list.trch_info[transport_channel_count].
          trch_type = DCH;
#ifdef FEATURE_WCDMA_HS_RACH
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, 
            rrc_TrafficVolumeMeasurement,measurementValidity))
        {
        
          if(rrc_mcm_ptr->measurementValidity.ue_State == rrc_MeasurementValidity_ue_State_all_But_Cell_DCH)
          {
                    /* Set channel type */
            WRRC_MSG0_HIGH("TVM:HSRACH Obj set as EDCH");
            mac_config_ptr->meas_obj_list.trch_info[transport_channel_count].
            trch_type = EDCH;
          }
        }
#endif
        /* Get Channel ID too */
        mac_config_ptr->meas_obj_list.trch_info[transport_channel_count].trch_id = 
          (uint16) mac_obj_ptr->elem[idx].u.dch; 
        
        transport_channel_count++;
      }
      idx++;
      if ((mac_obj_ptr != NULL) && (transport_channel_count == UE_MAX_TRCH))
      {
        WRRC_MSG0_HIGH("TVM:All transport Ch not included");
        break;
      }
      continue;
      
    }   /* End of while */
    
    mac_config_ptr->meas_obj_list.num_trch = transport_channel_count;  
    
  } /* End of meas object present */
  
  if (mac_config_ptr->meas_obj_list.num_trch == 0)
  {
    WRRC_MSG0_HIGH("TVM:No Of TrCh 0, RepCrit ALL");
  }
  
  
  /* Check if Meas Quantity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, 
    rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity))
  {
    WRRC_MSG1_HIGH("TVM:Meas Quan %d RlC:A:V/1:2:3", rrc_mcm_ptr->
      trafficVolumeMeasQuantity.t);

    mac_config_ptr->meas_qty_incl = TRUE;
    
    if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->
      trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
    {
      mac_config_ptr->meas_qty.meas_qty_enum = RLC_BUF_PAYLOAD;
    }
    else if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->
      trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,averageRLC_BufferPayload))
    {
      mac_config_ptr->meas_qty.meas_qty_enum = AVERAGE_RLC_BUF_PAYLOAD;
      /* Put time interval too */
      mac_config_ptr->meas_qty.meas_interval = 
        rrc_mcm_ptr->trafficVolumeMeasQuantity.
        u.averageRLC_BufferPayload;

      mac_config_ptr->meas_qty.meas_interval = mac_config_ptr->meas_qty.meas_interval * MEAS_QUANTITY_INTERVAL_OFFSET;
    }
    else
    {
      mac_config_ptr->meas_qty.meas_qty_enum = VARIANCE_RLC_BUF_PAYLOAD;
      /* Put time interval too */
      mac_config_ptr->meas_qty.meas_interval = rrc_mcm_ptr->
        trafficVolumeMeasQuantity.u.varianceOfRLC_BufferPayload;

      mac_config_ptr->meas_qty.meas_interval = mac_config_ptr->meas_qty.meas_interval * MEAS_QUANTITY_INTERVAL_OFFSET;
    }
    
    
  } /* End of if of Meas Quantity present */

  /* Meas Quantity absent has already been taken care off for setup by
   * sending a Meas Ctrl Msg */
  
  /* Check if Reporting Quantity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, 
    rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity))
  {
    mac_config_ptr->meas_report_incl = TRUE;
    
    if (rrc_mcm_ptr->
      trafficVolumeReportingQuantity.rlc_RB_BufferPayload == TRUE)
    {
      mac_config_ptr->meas_report_qty.rlc_payload = TRUE;
    }
    if (rrc_mcm_ptr->
      trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE)
    {
      mac_config_ptr->meas_report_qty.avg_payload = TRUE;
    }
    if (rrc_mcm_ptr->
      trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE)
    {
      mac_config_ptr->meas_report_qty.var_payload = TRUE;
    }
    MSG_HIGH("TVM:Rpting Quan RLC%d A%d V%d", mac_config_ptr->meas_report_qty.rlc_payload,
      mac_config_ptr->meas_report_qty.avg_payload, 
      mac_config_ptr->meas_report_qty.var_payload);

  }   /* End of reporting quantity present */
  
  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->reportCriteria, 
    rrc_TrafficVolumeReportCriteria_trafficVolumeReportingCriteria))
  {
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_mcm_ptr->reportCriteria.u.trafficVolumeReportingCriteria,
      transChCriteriaList)) &&
      (rrc_mcm_ptr->reportCriteria.u.trafficVolumeReportingCriteria->
      transChCriteriaList.n != 0))
    {
      WRRC_MSG0_HIGH("TVM:Event triggered Mode selected");
    }
    else
    {
      WRRC_MSG0_ERROR("TVM:Empty event crit structure");
      return TVM_FAIL;
    }
    mac_config_ptr->meas_report_mode.report_mode = EVENT_TRIGGERED_MODE;
    if (rrctvm_process_event_criteria(&rrc_mcm_ptr->reportCriteria.
      u.trafficVolumeReportingCriteria->transChCriteriaList, 
      &mac_config_ptr->meas_report_criteria.event_cfg
#ifdef FEATURE_WCDMA_HS_RACH
      ,mac_config_ptr
#endif
      ) == TVM_FAIL)
    {
      WRRC_MSG0_HIGH("TVM:Meas Req to MAC not to be sent");
      return TVM_FAIL;
    }
    /* Set meas_criteria_incl to TRUE */
    mac_config_ptr->meas_criteria_incl = TRUE;
    
  } /* End of if of Event Triggered Mode */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->reportCriteria,
    rrc_TrafficVolumeReportCriteria_periodicalReportingCriteria))
  {
    WRRC_MSG0_HIGH("TVM:Periodic Mode selected");
    mac_config_ptr->meas_report_mode.report_mode = PERIODIC_MODE;
    
    /* Set default no of reports */
    mac_config_ptr->meas_report_criteria.periodic_cfg.report_infinity = TRUE;
    
    /* Set meas_criteria_incl to TRUE */
    mac_config_ptr->meas_criteria_incl = TRUE;
    
    
    /* Get periodic information */
   
    mac_config_ptr->meas_report_criteria.periodic_cfg.num_reports = 
      TVM_RET_REPORTING_AMOUNT(rrc_mcm_ptr->reportCriteria.
      u.periodicalReportingCriteria->reportingAmount);
  
  WRRC_MSG1_HIGH("TVM:Reporting Amt is %d",
       mac_config_ptr->meas_report_criteria.periodic_cfg.num_reports);
    
    if (rrc_mcm_ptr->reportCriteria.u.periodicalReportingCriteria->
      reportingAmount != rrc_ReportingAmount_ra_Infinity)
    {
      mac_config_ptr->meas_report_criteria.periodic_cfg.report_infinity = FALSE;
      
    }
      
    
    
    /* Get Reporting Interval */
    mac_config_ptr->meas_report_criteria.periodic_cfg.report_interval = 
      TVM_RET_PERIODIC_REPORTING_INTERVAL(
      rrc_mcm_ptr->reportCriteria.u.periodicalReportingCriteria->reportingInterval);
    
    MSG_LOW("Reporting Interval is %d",
      mac_config_ptr->meas_report_criteria.
      periodic_cfg.report_interval,0,0);
    
  } /* End of else if of periodic reporting  */
  else
  {
   WRRC_MSG0_HIGH("TVM:Addtl Meas Mode selected");

   /* Set meas_criteria_incl to TRUE */
   mac_config_ptr->meas_criteria_incl = TRUE;
   mac_config_ptr->meas_report_mode.report_mode = NO_REPORTING;
  }

  return TVM_SUCCESS; 
}


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_EVENT_CRITERIA

DESCRIPTION 

    Sets the traffic volume event criteria for MAC. Event criteria here can be 
    specified for a transport channel and one transport channel can have at the
    maximum of 2 events (4A/4B).

DEPENDENCIES

    None

RETURN VALUE

    Sucess/Failure  

SIDE EFFECTS

    None
===========================================================================*/
rrctvm_status_e_type rrctvm_process_event_criteria
(
rrc_TransChCriteriaList * event_crit_ptr,
mac_ul_traffic_event_trigger_cfg_type* mac_event_crit_ptr
#ifdef FEATURE_WCDMA_HS_RACH
,mac_ul_traffic_meas_config_type * mac_config_ptr
#endif
)
{
  /* Store event criteria parameters */
  rrc_TransChCriteriaList * rrc_event_crit_ptr = NULL;
  
  /* To count the no of transport channels */
  uint8 transport_channel_cnt = 0;
  
  boolean dch_object_specified = FALSE;
  
  boolean rach_object_specified = FALSE;
  
  uint16 no_of_events = 0;
  uint32 idx=0;
  rrc_TransChCriteria_eventSpecificParameters*  event_param_ptr;
  
  rrctvm_status_e_type status = TVM_SUCCESS;
  
  uint8 event_tr_ch_flag = 0; /* If 0, then event criteria is for all uplink channels */
  
  rrc_event_crit_ptr = event_crit_ptr;
 
  if(rrc_event_crit_ptr == NULL)
  {
    WRRC_MSG0_HIGH("TVM:Event criteria pointer is null");
    return TVM_FAIL;
  } 
  
  /* Store the head ptr of event parameters */
  event_param_ptr = &rrc_event_crit_ptr->elem[0].eventSpecificParameters;
  
  /* Get the events */
  while ((rrc_event_crit_ptr->n > idx) && (transport_channel_cnt < UE_MAX_TRCH))
  { 
    
    
    /* If transport channel id is not present, then that means that this
    * criteria is valid for all the transport channels. So ignore rest of the channels */
    if (RRC_MSG_LIST_BITMASK_IE_TYPE2(rrc_event_crit_ptr,ul_transportChannelID))
    {
      WRRC_MSG0_HIGH("TVM:Transport Channel Id present");
      /* Set Id */
      if ( rrc_event_crit_ptr->elem[idx].ul_transportChannelID.t == 
        T_rrc_UL_TrCH_Identity_usch)
      {
        WRRC_MSG0_HIGH("TVM:Ignoring USCH TrCh object");
        idx++;
        continue;
      }
      if (rrc_event_crit_ptr->elem[idx].ul_transportChannelID.t ==
         T_rrc_UL_TrCH_Identity_rachorcpch)
      {
        WRRC_MSG0_HIGH("TVM:RACH object given in R.Crit"); 
        /* Tr Ch type */
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          trch_info.trch_type = RACH;
        rach_object_specified = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("TVM:DCH object given");
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          trch_info.trch_type = DCH; 
        
        dch_object_specified = TRUE;
        /* Tr Id */
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].trch_info.trch_id = 
          (uint16) rrc_event_crit_ptr->elem[idx].ul_transportChannelID.u.dch;
#ifdef FEATURE_WCDMA_HS_RACH
        if(rrctvm_check_if_edch_object_exists(mac_config_ptr,mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].trch_info.trch_id))
        {
          mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
            trch_info.trch_type = EDCH; 
        }
#endif          
      }
      /* Increment flag */
      event_tr_ch_flag++;
      
    }   /* End of if of rrc_ul_transportChannelID_present present */
    
        /* We are following the policy that if UTRAN does not specify the meas
        * object here for the firts scenario, then the event criteria is common 
        * for all uplink channels. So read the tr ch id the first time. If not 
        * present, then read the event criteria and exit. UTRAN can mess up by 
        * not giving the event criteria too. In that scenario, don't send command to MAC */
    
    /* Now for each TrChId, there can be 2 events specified */
    if (RRC_MSG_LIST_BITMASK_IE_TYPE2(rrc_event_crit_ptr,eventSpecificParameters))
    {
      uint32 idy =0;
      WRRC_MSG0_HIGH("TVM:Event Params present");
      
      while (event_param_ptr->n > idy)
      {
        /* Set Event ID */
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].event_id = 
          TVM_RET_EVENT_ID(event_param_ptr->elem[idy].eventID);
        
        WRRC_MSG1_HIGH("TVM:Event Id %d 4A:1/4B:2", mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].event_id);
        
        /* Set threshold bytes */
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].report_threshold = 
          TVM_RET_THRESHOLD_BYTES(event_param_ptr->elem[idy].reportingThreshold);
        
        WRRC_MSG1_HIGH("TVM:Threshold %d bytes", mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].report_threshold);
        
        
        /* Set these params to Default values */
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].time_to_trigger = QUANTITY_NOT_PRESENT;
        
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].trigger_prohibit_tmr = QUANTITY_NOT_PRESENT;
        
        mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
          event_param[no_of_events].tx_interrupt_tmr = QUANTITY_NOT_PRESENT;
        
        
        /* Now browse the event parameters */
        if ( event_param_ptr->elem[idy].m.timeToTriggerPresent)
        {
          mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
            event_param[no_of_events].time_to_trigger = 
            TVM_RET_TIME_TO_TRIGGER(event_param_ptr->elem[idy].timeToTrigger);
          WRRC_MSG1_HIGH("TVM:Time to trigger %d msec",mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
            event_param[no_of_events].time_to_trigger);
        }
        
        /* Pending time after trigger */
        if (event_param_ptr->elem[idy].m.pendingTimeAfterTriggerPresent)
        {
          mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
            event_param[no_of_events].trigger_prohibit_tmr = 
            TVM_RET_PENDING_TIME_AFTER_TRIGGER(
            event_param_ptr->elem[idy].pendingTimeAfterTrigger);
          
          WRRC_MSG1_HIGH("TVM:Pending time after trigger %d msec",mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].
            event_param[no_of_events].trigger_prohibit_tmr);
        }
        
        
        /* Interruption time after trigger */
        if (event_param_ptr->elem[idy].m.tx_InterruptionAfterTriggerPresent)
        {
          if (((dch_object_specified == TRUE) && (rach_object_specified == TRUE)) ||
            (rach_object_specified == TRUE))
          {
           
            WRRC_MSG2_HIGH("TVM:I.Time after trigger present .. DCH object %d, RACH object %d", dch_object_specified, rach_object_specified);
            
            mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].event_param[no_of_events].tx_interrupt_tmr = 
              TVM_RET_INTPT_TIME_AFTER_TRIGGER(
              event_param_ptr->elem[idy].tx_InterruptionAfterTrigger);
          }
          else
          {
            if (event_tr_ch_flag == 0)
            {
              WRRC_MSG0_HIGH("No of TrCh = 0, I.Time value specified");
              mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].event_param[no_of_events].tx_interrupt_tmr = 
                TVM_RET_INTPT_TIME_AFTER_TRIGGER(
                event_param_ptr->elem[idy].tx_InterruptionAfterTrigger);
            }
          }
        }
        
        /* Increase no of events */
        no_of_events++;
        
        /* Get next value */
        idy++;
        
        
      } /* End of while of no of events */
      
      /* Set no of events */
      mac_event_crit_ptr->trch_event_cfg[transport_channel_cnt].num_events
        = no_of_events;
      
      /* Make sure event struct was not NULL */
      if (no_of_events == 0)
      {
        WRRC_MSG0_ERROR("TVM:Event Parameters NULL");
        return TVM_FAIL;
      }
      
      if (event_tr_ch_flag == 0)
      {
      /* This means that meas object was not specified but event params were present
        * So just skip reading the rest of the event channels */
        
        /* Set it to FALSE==>Apply to all uplink channels */
        mac_event_crit_ptr->trch_info_incl = FALSE;
        
        WRRC_MSG0_HIGH("TVM:EventCrit to ULTransCh");
        
        return TVM_SUCCESS;
      }
      
      
      
    } /* End of if of event params present */
    else
    {
      WRRC_MSG0_ERROR("TVM:Event params not specified:TrChId");
      return TVM_FAIL;
    } /* End of else of event params present */
    
    /* Increment Tr Ch count */
    transport_channel_cnt++;
    
    /* Get next one */
    idx++;
    
    if (rrc_event_crit_ptr->n  == idx)
    {
      /* End of event params */
      break;
    }
    
    event_param_ptr = &rrc_event_crit_ptr->elem[idx].eventSpecificParameters;
    /* Reinitialize no of events */
    no_of_events = 0;    
    
  } /* End of while of rrc_event_crit_ptr not NULL */
  
  mac_event_crit_ptr->num_trch = transport_channel_cnt;
  
  /* Means individual tr Ch criteria present */
  mac_event_crit_ptr->trch_info_incl = TRUE;
  
  return status;
}


/*===========================================================================

FUNCTION   RRCTVM_SEND_TRAFFIC_VOL_MEAS_REPORT

DESCRIPTION 

    Sends the traffic volume Measurement Report

DEPENDENCIES

    None

RETURN VALUE

    None  

SIDE EFFECTS

    None
===========================================================================*/

void rrctvm_send_traffic_vol_meas_report
(
rrc_cmd_type *cmd_ptr
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL; 
  
  /* Stores status of send chain */
  rrcsend_status_e_type status; 
  
  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode;
  
  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id;  
  
  /* Radio bearer mode */
  rrc_RB_Identity rb_mode;
  
  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status;
  
  uint8 count = 0, cnt = 0;

  uint16 tvm_to_be_deleted;
  
  rrc_TrafficVolumeMeasuredResultsList * measured_results_ptr = NULL;
  
  mac_ul_traffic_meas_ind_type* meas_report_ptr = NULL;
  
  /* Ptr which carries info about the command */
  rrc_cmd_type  *rrc_meas_out_cmd_ptr = NULL;
#ifdef FEATURE_QSH_EVENT_METRIC
	rrc_log_rrc_mrm_ota mrm_params; 
#endif

  meas_report_ptr = &(cmd_ptr->cmd.cmac_ul_traffic_ind);
  
  /* First find if it is an event triggered or periodic report */
  if (meas_report_ptr->report_mode == NO_REPORTING)
  {
    WRRC_MSG0_ERROR("TVM:MAC sent incorrect report type");
    return;
  }
  
#ifdef FEATURE_QSH_EVENT_METRIC
  memset(&mrm_params,0,sizeof(rrc_log_rrc_mrm_ota));  
  mrm_params.meas_id = meas_report_ptr->meas_id;
  mrm_params.meas_type = L1_TRAF_VOL;
  mrm_params.meas_rpt_type = meas_report_ptr->report_mode;
  mrm_params.event_id = meas_report_ptr->event_result.event_id;
  rrc_qsh_log_meas_rpt_params(&mrm_params);
#endif

  
  if (meas_report_ptr->num_reports_ind == TRUE)
  {    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == meas_report_ptr->meas_id)
      {
        WRRC_MSG1_HIGH("TVM:Removing Meas %d from ID List", meas_report_ptr->meas_id);
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;

        tvm_meas_id_list[count].block_rach_incl = FALSE;
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;

        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;

        break;
      }      
    } /* End of for loop */     
  }

  /* Now delete it from Addtl Meas DB */
  if (meas_report_ptr->num_reports_ind == TRUE)
  {
    tvm_to_be_deleted = meas_report_ptr->meas_id;
    WRRC_MSG1_HIGH("TVM:Delete %d from addtl meas DB", tvm_to_be_deleted);
    rrcmeas_delete_addtl_meas(&tvm_to_be_deleted);
  }

  
  if ( meas_report_ptr->report_transfer_mode == ACK_MODE )
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
    WRRC_MSG0_ERROR("TVM:RLC Logical channel not found");
    return;
  }
  
  /* Allocate a buffer for an uplink DCCH Message. This buffer 
   * should be released by SEND_CHAIN using OSS compiler functions */
  
  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));
  
  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport; 
  
  /* First store the meas id */
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,meas_report_ptr->meas_id, MAC_MEAS);
  
  /* It is event triggered report */
  if (meas_report_ptr->report_mode == EVENT_TRIGGERED_MODE)
  {
    /* Set event results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
      rrc_MeasurementReport,eventResults); 
    
    /* Set traffic vol event results */
    RRC_SET_COMMON_MSG_TYPE2(msg_ptr->message.u.measurementReport.eventResults,
      rrc_EventResults,trafficVolumeEventResults);

    msg_ptr->message.u.measurementReport.eventResults.u.
        trafficVolumeEventResults =
        rtxMemAllocTypeZ (&enc_ctxt, rrc_TrafficVolumeEventResults);
    /* Set event name */
    if (meas_report_ptr->event_result.event_id == EVENT_4A)
    {
      WRRC_MSG0_HIGH("TVM:EVENT 4A Reported");
      msg_ptr->message.u.measurementReport.eventResults.u.
        trafficVolumeEventResults->trafficVolumeEventIdentity = rrc_TrafficVolumeEventType_e4a;
    }
    else
    {
      WRRC_MSG0_HIGH("TVM:EVENT 4B Reported");
      msg_ptr->message.u.measurementReport.eventResults.u.
        trafficVolumeEventResults->trafficVolumeEventIdentity = rrc_TrafficVolumeEventType_e4b;
    }
    
    /* Set transport channel on which event happens */
    msg_ptr->message.u.measurementReport.eventResults.u.
      trafficVolumeEventResults->ul_transportChannelCausingEvent.t
      = TVM_RET_TRCH_TYPE(meas_report_ptr->event_result.trch_info.trch_type);
    
    /* Now set the channel id corresponding to channel type
     * This is doen only for DCH */
    if (msg_ptr->message.u.measurementReport.eventResults.u.
      trafficVolumeEventResults->ul_transportChannelCausingEvent.t == 
      T_rrc_UL_TrCH_Identity_dch)
    {
      WRRC_MSG1_HIGH("TVM:Event occurred on DCH TrChId = %d", 
        meas_report_ptr->event_result.trch_info.trch_id);
      
      msg_ptr->message.u.measurementReport.eventResults.u.
        trafficVolumeEventResults->ul_transportChannelCausingEvent.u.dch
        = (rrc_TransportChannelIdentity)meas_report_ptr->event_result.trch_info.trch_id;
    }
    else
    {
      WRRC_MSG0_HIGH("TVM:Event occurred on RACH");
    }
    
    
  } /* End of event triggered report */
  
  if ((meas_report_ptr->report_mode == PERIODIC_MODE) &&
    (meas_report_ptr->num_rbs == 0))
  {
  WRRC_MSG0_HIGH("TVM:Sending Blank Meas Rpt");
  }
  else
  {
  if ((meas_report_ptr->report_mode == PERIODIC_MODE)  ||
    (meas_report_ptr->num_rbs != 0))
  {
    WRRC_MSG0_HIGH("TVM:Measured Results received");
    
    /* Set Measured Results present */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
      rrc_MeasurementReport,measuredResults);
    
    /* Set Choice to Traffic Volume Results */
    RRC_SET_COMMON_MSG_TYPE(msg_ptr->message.u.measurementReport.measuredResults,
      rrc_MeasuredResults_trafficVolumeMeasuredResultsList);
    
    measured_results_ptr = rtxMemAllocTypeZ (&enc_ctxt, rrc_TrafficVolumeMeasuredResultsList);

    /* Update the linked list head ptr */
    msg_ptr->message.u.measurementReport.measuredResults.u.
      trafficVolumeMeasuredResultsList = measured_results_ptr;

    measured_results_ptr->n = meas_report_ptr->num_rbs;

    ALLOC_ASN1ARRAY2(&enc_ctxt,measured_results_ptr,rrc_TrafficVolumeMeasuredResults);

    WRRC_MSG1_HIGH("TVM:Measured Results is for %d RB's",
      meas_report_ptr->num_rbs);
    
    for (count=0; count < meas_report_ptr->num_rbs; count++)
    {
     /* Initialize bit mask */
      RRC_RESET_MSG_IE_PRESENT(measured_results_ptr->elem[count]);
      /* Get radio bearer Id */
      measured_results_ptr->elem[count].rb_Identity = 
       meas_report_ptr->rb_meas[count].rb_id;
      
     
      /* Now check the payload in each radio bearer */
      if (meas_report_ptr->rb_meas[count].raw_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("Raw RLC payload not present",0,0,0);
      }
      else
      {
        measured_results_ptr->elem[count].m.rlc_BuffersPayloadPresent =1; 
        
        /* Convert the size */        
        measured_results_ptr->elem[count].rlc_BuffersPayload = 
          TVM_RET_RLC_BUFFER_PAYLOAD(meas_report_ptr->rb_meas[count].raw_rlc_payload);
        
        if(meas_report_ptr->rb_meas[count].raw_rlc_payload != 0)
        { 
          WRRC_MSG2_HIGH("TVM:RB = %d, Raw RLC payload sent = %d", meas_report_ptr->rb_meas[count].rb_id,
          meas_report_ptr->rb_meas[count].raw_rlc_payload);
        }
        
      }
      if (meas_report_ptr->rb_meas[count].average_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("TVM:Avge RLC payload not present",0,0,0);
      }
      else
      {
        measured_results_ptr->elem[count].m.averageRLC_BufferPayloadPresent =1; 
        
        /* Convert the size in bytes */
        measured_results_ptr->elem[count].averageRLC_BufferPayload = 
          TVM_RET_AVG_BUFFER_PAYLOAD(meas_report_ptr->rb_meas[count].average_rlc_payload);
        
        WRRC_MSG2_HIGH("TVM:RB = %d, Avge RLC payload sent = %d", 
          meas_report_ptr->rb_meas[count].rb_id,
          meas_report_ptr->rb_meas[count].average_rlc_payload);
        
      }
      if (meas_report_ptr->rb_meas[count].variance_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("Var RLC payload not present",0,0,0);
      }
      else
      {
        measured_results_ptr->elem[count].m.varianceOfRLC_BufferPayloadPresent=1;
        
        /* Convert the size in bytes */
        measured_results_ptr->elem[count].varianceOfRLC_BufferPayload = 
          TVM_RET_VAR_BUFFER_PAYLOAD(meas_report_ptr->rb_meas[count].variance_rlc_payload);
        
        WRRC_MSG2_HIGH("TVM:RB = %d,Var Payload = %ul", 
          meas_report_ptr->rb_meas[count].rb_id,
          meas_report_ptr->rb_meas[count].variance_rlc_payload);
        
      }
      
         /* End of else */
    } /* End of for loop */
  } /* End of if of measured results */
  } /* End of else */

  /* Append Additional Measured Results if available */

  /* Initialize Additional Measured Results list */
  msg_ptr->message.u.measurementReport.additionalMeasuredResults.n = 0;

  rrcmeas_process_additional_measured_results(
   &meas_report_ptr->meas_id, MAC_MEAS, &msg_ptr->message.u.measurementReport);
  
  if (rrcmeas_current_substate == RRCMEAS_CELL_FACH)
  {
    /* Append RACH report if RACH criteria satisfied and report available */
    if(rrcmeas_append_meas_rpt_on_rach(
     &msg_ptr->message.u.measurementReport.measuredResultsOnRACH, RRC_PROCEDURE_MCMR, FALSE) == RRCMEAS_RACH_SUCCESSFUL)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
        rrc_MeasurementReport,measuredResultsOnRACH);
    }
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif
  }
  
  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);
  
  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("TVM:Send chain failed to send MeasReport");
  }
  else
  {
    WRRC_MSG0_HIGH("TVM:Meas Report sent from RRC to RLC");       
    
    if(meas_report_ptr->report_transfer_mode == ACK_MODE)
    {
      rrcmeas_pending_rpt++; 
      WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
    } 
    
    /* Now check if we need to send block command to MAC 
     * RRC sends to MAC this command only if meas_id sent by
     * MAC has block_rach_incl set to TRUE in tvm_meas_id_list
     * and RRC is in CELL_FACH state. If either of this condition is
     * not true, RRC does not send this command to MAC */    
    
    if ((rrcmeas_current_substate == RRCMEAS_CELL_FACH) &&
    (meas_report_ptr->report_mode == EVENT_TRIGGERED_MODE) &&
    (meas_report_ptr->event_result.event_id == EVENT_4A)
#ifdef FEATURE_WCDMA_HS_RACH
    && (FALSE == rrchsrach_get_ready_for_common_edch(current_config_ptr))
#endif
    )    
    { 
      
      for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
      {
        if ((tvm_meas_id_list[count].meas_id == meas_report_ptr->meas_id) &&
          (tvm_meas_id_list[count].block_rach_incl == TRUE))
        {
          l2_ul_req_cmd * rrc_cmd_l2_req = NULL;
          WRRC_MSG1_HIGH("Block RACH for Meas Id = %d",meas_report_ptr->meas_id);

          /* Send command to MAC. Allocate memory */
            rrc_meas_out_cmd_ptr = rrc_get_int_cmd_buf();
            RRC_GET_POINTER_L2_REQ_VALUE(rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l2_ul_req,rrc_cmd_l2_req);
          
            /* Initialize the LLC command header first */
          
            rrc_meas_out_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
            rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_MCMR;
          
            rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER2_MAC_UL;
          
            /* Now fill the L2 cmd header */
            rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l2_ul_cmd.cmd_id = 
              CMAC_UL_BLOCK_RACH_TRAFFIC_REQ;
          
            /* Set Block RACH indication */
            rrc_cmd_l2_req->block_rach = TRUE;
          /* Send the Internal RRC Command - LLC should directly handle it */
            rrc_put_int_cmd( rrc_meas_out_cmd_ptr );
          
          break;
        }  /* End of if for meas id found and block_rach is TRUE */
      } /* End of for */
      
    } /* End of if rrcmeas_current_substate == CELL_FACH */
    
  } /* End of else */
  
  return; 

}


/*===========================================================================

FUNCTION          RRCTVM_SEND_MAC_CMD

DESCRIPTION 

    This function sends measurement requests to L1. 
    Since all MAC cmds go via LLC, the LLC command 
    RRC_LLC_CMD_REQ actaully encapsules the real L1 
    command. This is the LLC command funneling mechanism.
    It frees allocated command and embedded buffers when failure 
    is detected before sending the command.

DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None.

===========================================================================*/

void rrctvm_send_mac_cmd
(
mac_ul_traffic_meas_config_type* traffic_vol_params_ptr
)
{

  /* Ptr which carries info about the command */
  rrc_cmd_type        *rrc_meas_out_cmd_ptr = NULL;
  l2_ul_req_cmd * rrc_cmd_l2_req = NULL;
  /* Now allocate memory */
  rrc_meas_out_cmd_ptr = rrc_get_int_cmd_buf();
  WRRC_MSG0_HIGH("TVM:Sending rrctvm_send_mac_command"); 
  RRC_GET_POINTER_L2_REQ_VALUE(rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l2_ul_req,rrc_cmd_l2_req);

  /* Initialize the LLC command header first */

  rrc_meas_out_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_MCMR;

  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER2_MAC_UL;

  /* Now fill the L1 cmd header */
  rrc_meas_out_cmd_ptr->cmd.rrc_llc_req.l2_ul_cmd.cmd_id = 
    CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ;

  /* Now allocate memory to the pointer inside the command */
  rrc_cmd_l2_req->mac_ul_traffic_vol_config = 
      (mac_ul_traffic_meas_config_type *)rrc_malloc_interlayer_cmd(sizeof(mac_ul_traffic_meas_config_type));
  
  /* Set Traffic Vol params */
  *(rrc_cmd_l2_req->mac_ul_traffic_vol_config) 
     = *traffic_vol_params_ptr;

#ifdef FEATURE_QSH_EVENT_METRIC
  /* This flag set to FALSE means that a DL OTA is received
     Since this code path is common and will be executed for state change and SIB info
     also, added rrc_qsh_dl_committed to identify the MCM reception */
  if(rrc_qsh_dl_committed == FALSE)
  {
    rrc_log_rrc_mcm_ota tvm_params;
    memset(&tvm_params,0,sizeof(rrc_log_rrc_mcm_ota));  
    tvm_params.meas_id = rrc_cmd_l2_req->mac_ul_traffic_vol_config->meas_id;
    tvm_params.meas_type = L1_TRAF_VOL;

    tvm_params.meas_rpt_type = rrc_cmd_l2_req->mac_ul_traffic_vol_config->meas_report_mode.report_mode;
  
    rrc_qsh_log_meas_ctrl_params(&tvm_params);
  }	
#endif

/* Send the Internal RRC Command - LLC should directly handle it */
  rrc_put_int_cmd( rrc_meas_out_cmd_ptr );

  /* Now send command to MAC to start measurements */
  MSG_LOW("Sent Meas command to MAC", 0,0,0);
}


/*===========================================================================

FUNCTION   RRCTVM_SET_MEAS_IDENTITY

DESCRIPTION 

    Set traffic volume meas identity variable from SIB information.

DEPENDENCIES

    None

RETURN VALUE

    None  

SIDE EFFECTS

    None
===========================================================================*/

void rrctvm_set_meas_identity
(
uint16* meas_id_ptr,
rrc_TrafficVolumeMeasSysInfo* sib_meas_ptr
)
{
  uint16 count = 0;
  /* First check if the meas id already exists */
  
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == *meas_id_ptr)
    {
      WRRC_MSG1_HIGH("TVM:Meas Id = %d already existing",*meas_id_ptr);
      
      tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
      /* Check if validity is present */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(sib_meas_ptr,
        rrc_TrafficVolumeMeasSysInfo,measurementValidity))
      {
        tvm_meas_id_list[count].meas_validity = 
          TVM_RET_MEAS_VALIDITY(sib_meas_ptr->measurementValidity.ue_State);
        
        WRRC_MSG1_HIGH("TVM:SIB12 Meas Validity is",tvm_meas_id_list[count].meas_validity);
        
      }

      return;
    }
    
  } /* End of for loop */ 
  
  WRRC_MSG0_HIGH("TVM:Appending TVM meas identity");
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      WRRC_MSG1_HIGH("TVM:Setting Meas Id = %d in tvm_meas_id_list",
        *meas_id_ptr);        
      tvm_meas_id_list[count].meas_id =  (uint8) *meas_id_ptr;
      
      tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
      
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(sib_meas_ptr,
        rrc_TrafficVolumeMeasSysInfo,measurementValidity))
      {
        tvm_meas_id_list[count].meas_validity = TVM_RET_MEAS_VALIDITY(sib_meas_ptr->measurementValidity.ue_State);
        
        WRRC_MSG1_HIGH("TVM:SIB12 Meas Validity is %d",tvm_meas_id_list[count].meas_validity);
      }
      
      return;
    }
  }
  /* The following should never happen */
  WRRC_MSG0_ERROR("TVM:Unable to set TVM Measurement Identity");
  return;

}


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_DCH_TO_FACH

DESCRIPTION 

  The main handler function that handles the processing of
  traffic volume measurements when RRC transitions from 
  DCH-->FACH. Check the MEASUREMENT_IDENTITY variable and
  determine for each measurement what has to be done as per
  its validity. The function accordingly updates the
  MEASUREMENT_IDENTITY variable and sends a command to
  MAC to reconfigure the traffic volume measurements.
  It also processes the scenario when no traffic volume 
  measurement has been assigned to the UE with a MEASUREMENT 
  CONTROL message that is valid in CELL_FACH state (stored 
  in the variable tvm_meas_id_list), that has the same identity 
  as the one indicated in the "Traffic volume measurement 
  system information
  

DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_dch_to_fach
(
 void
)
{
  uint8 count = 0, cnt = 0, addtl_deleted_cnt = 0;
   
  
  mac_ul_traffic_meas_config_type tvm_cmd;
  
  uint16 tvm_meas_id;
  
  uint8 prim_meas_to_be_deleted_from_addtl_db[MAX_TRAFFIC_VOLUME_MEASUREMENTS];
  
  /* Initialize it */
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
    prim_meas_to_be_deleted_from_addtl_db[count] = INVALID_MEAS_ID;
  
  WRRC_MSG0_HIGH("TVM:Processing Traffic Vol");
  
  /* Check the Validity in Measurement Identity variable for Traffic Vol Meas */  
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID)
    {
      switch(tvm_meas_id_list[count].meas_validity)
      {
      case NOT_PRESENT:
        
        WRRC_MSG1_HIGH("TVM:Validity absent. Delete Meas = %d",
          tvm_meas_id_list[count].meas_id);
        
          /* Build Delete Command and also got to read the 
        new meas from SIB11/12 */
        
        /* Cmd type */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
        
        /* Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Cnf not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        WRRC_MSG1_HIGH("TVM:Tell MAC to delete meas = %d ", tvm_cmd.meas_id);
        
        /* Set Meas Id in Meas Identity var to Invalid */
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        /* Just delete all the addtl meas associated with this identity */
        for (cnt = 0; cnt <MAX_ADDITIONAL_MEAS; cnt++ ) 
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        
          /* Store the deleted prim meas in prim_meas_to_be_deleted_from_addtl_db
        so that it could be deleted from addtl meas db */
        for (addtl_deleted_cnt = 0; addtl_deleted_cnt <MAX_TRAFFIC_VOLUME_MEASUREMENTS; 
        addtl_deleted_cnt++) 
        {
          if (prim_meas_to_be_deleted_from_addtl_db[addtl_deleted_cnt] == INVALID_MEAS_ID)
          {
          /* Found the first position. Store it so that addtl meas fn could be called
            to delete it */
            prim_meas_to_be_deleted_from_addtl_db[addtl_deleted_cnt] = (uint8) tvm_cmd.meas_id;
            break; /* Come out of for */
          }
        }
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        break;  /* Come out of switch and find if any other meas needs to be deleted */
        
      case ALL_STATES:
        
        WRRC_MSG1_HIGH("TVM:Validity ALL. Continue meas = %d",
          tvm_meas_id_list[count].meas_id);

     /* Since there are scenarios when MAC does not know about DCH to FACH
       trans and a meas could have both DCH and FACH object, so send MAC
       a explicit resume. In case MAC has stopped the meas, it resumes the meas
       with the new object. In case it is already doing, it ignores the cmd */
    
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Resume the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        /* No Need to send cmd to MAC */
        break;
        
      case STATE_DCH:
        
        WRRC_MSG1_HIGH("TVM:Validity DCH only. Stop,Save meas= %d",
          tvm_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Stop the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_SAVE;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        break;
        
      case ALL_STATES_BUT_DCH:
        WRRC_MSG1_HIGH("TVM:Validity all except DCH.Resume meas=%d",
          tvm_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Resume the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        break; 
        
      default:
        /* Not possible */
        WRRC_MSG0_ERROR("Invalid meas validity in tvm_meas_id_list");
        return;
        
      } /* End of switch */
    } /* end of if */
    
  } /* End of for */
  
  /* Now check if any primary meas which was deleted earlier needs to be deleted 
  from addtl meas db */
  
  for (addtl_deleted_cnt = 0; addtl_deleted_cnt <MAX_TRAFFIC_VOLUME_MEASUREMENTS; 
  addtl_deleted_cnt++) 
  {
    if (prim_meas_to_be_deleted_from_addtl_db[addtl_deleted_cnt] != INVALID_MEAS_ID)
    {
      MSG_LOW("Found prim meas %d to be deleted from addtl db",
        prim_meas_to_be_deleted_from_addtl_db[addtl_deleted_cnt],0,0);
      
      /* Store in tvm_meas_id as fn expects uint16 */
      tvm_meas_id = prim_meas_to_be_deleted_from_addtl_db[addtl_deleted_cnt];
      
      /* Remove this meas from any additional meas DB*/
      rrcmeas_delete_addtl_meas(&tvm_meas_id);
    }
  }
  
  if ((tvm_meas_id = rrctvm_get_meas_id_from_sibs()) == INVALID_MEAS_ID)
  {
    WRRC_MSG0_HIGH("TVM: SIB 11/12 not used");
  }
  else 
  {
  /* Find if any MCM is present in DB and has same id as one in tvm_meas_id  
    If yes, then cannot overwrite that with the one from SIB's. */
    for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
    {
      if ((tvm_meas_id_list[count].meas_id == tvm_meas_id) &&
        (tvm_meas_id_list[count].meas_ctrl_msg == TRUE))
      {
        WRRC_MSG1_HIGH("TVM:New SIB Meas Id %d == MCM Meas Id", tvm_meas_id);
        return;
      }
    } /* End of for of count of MAX_TRAFFIC_VOLUME_MEASUREMENTS */
    
    
    if (rrctvm_get_sibs_in_connected_state() == TVM_SUCCESS)
    {
      WRRC_MSG0_HIGH("TVM:Send SETUP (SIB cmd) to MAC"); 
      
      tvm_cmd = mac_sib_params;
      tvm_cmd.config_conf_ind = FALSE;
      tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_START;
      /* in PCH state send START with meas_start_incl FALSE otherwise MAC will reject it
        followed by RESUME otherwise MAC will not resume this TVM in FACH/DCH*/
      if(dch_to_pch_in_progress == TRUE)
      {
        tvm_cmd.meas_start_incl = FALSE;
      }
      WRRC_MSG0_HIGH("RRC_TVM: sending START with flag false in DCH->PCH");
      rrctvm_send_mac_cmd(&tvm_cmd);  
      if(dch_to_pch_in_progress == TRUE)
      {
        tvm_cmd.meas_id = tvm_meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Resume the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
      rrctvm_send_mac_cmd(&tvm_cmd);  
      }    
    }    
    else
    {
      WRRC_MSG0_ERROR( "Unbale to read SIB for TVM");
      return;
    }
  } /* End of else of SIB11/12 Meas Id being valid */
}

/*===========================================================================

FUNCTION   RRCTVM_GET_SIBS_IN_CONNECTED_STATE

DESCRIPTION 

  Processes the scenario when the transition from DCH-->FACH 
  transition determines that traffic volume measurement for the
  default measurement has to be overwritten by new SIB's from
  the new cell. So read the new SIB's for traffic volume measurements
  

DEPENDENCIES

  None

RETURN VALUE

  TVM_SUCCESS if SIB 11/12 is successfully read for Traffic Volume
  TVM_FAIL if SIB 11/12 is successfully read for Traffic Volume 

SIDE EFFECTS

  If SIB11 is not valid, then  traffic volume measurements may not be done.

===========================================================================*/

rrctvm_status_e_type rrctvm_get_sibs_in_connected_state
(
  void
)
{
  /* Declared pointer to SIB & SIB 12 */
  rrc_SysInfoType11 *sib11_ptr = NULL; 
  
  rrc_SysInfoType12 *sib12_ptr = NULL;
  
  /* Make SIB12 as absent */
    
  /* Reinitialize some key mac params */
  rrctvm_initialize();
  
  /* Get traffic volume params from SIB11 */
  sib11_ptr = (rrc_SysInfoType11*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB11);
  
  /* Call this function which reads SIB11 */ 
  if ( sib11_ptr == NULL )
  {
     WRRC_MSG0_ERROR("SIB 11 empty.");
    return TVM_FAIL;
  }

  sib12_ptr = (rrc_SysInfoType12*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB12);

  /* SIB 12 present */
  if ((sib11_ptr->sib12indicator == TRUE) && (sib12_ptr != NULL))
  {
    sib12_present = SIB12_PRESENT;
    
    /* Read traffic volume info from SIB12 and send meas req to MAC  */
    
    if (rrctvm_process_sib_info(&sib12_ptr->measurementControlSysInfo)
      == TVM_FAIL )
    {
      WRRC_MSG0_HIGH("TVM: Meas not done as per SIB 12");
      rrctvm_initialize();
      return TVM_FAIL;
    }
    
  } /* End of if of sib12 PRESENT */
  else
  {
    /* Get traffic volume params from SIB11 */
    
    /* Read traffic volume info from SIB12 and send meas req to MAC */
    if (rrctvm_process_sib_info(&sib11_ptr->measurementControlSysInfo) 
      == TVM_FAIL)
    {
      WRRC_MSG0_HIGH("TVM:Not done as per SIB 11");
      rrctvm_initialize();
      return TVM_FAIL;
    }
  }
  return TVM_SUCCESS; 

}


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_TVM_SIBS_IN_CONNECTED_STATE

DESCRIPTION 

  Invoked when cell reselection is done in CELL_FACH state
  and traffic volume measurements need to be updated as
  per the new SIB's. It reads the new traffic volume
  measurememt parameters from SIB11/12 and stores it in an
  internal data structure
 
DEPENDENCIES

  None

RETURN VALUE

  Returns TVM_SUCCESS if SIB11/12 successfully read. 
  Returns TVM_FAIL if SIB11/12 successfully read.

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

rrctvm_status_e_type rrctvm_process_tvm_sibs_in_connected_state
(
  void
)
{
  /* Declared pointer to SIB & SIB 12 */
  rrc_SysInfoType11 *sib11_ptr = NULL; 
  
  rrc_SysInfoType12 *sib12_ptr = NULL;
  
  rrc_state_e_type curr_state = rrc_get_state();

  /* Make SIB12 as absent */
   
  sib12_present = SIB12_ABSENT;
  
  /* Get traffic volume params from SIB11 */
  sib11_ptr = (rrc_SysInfoType11*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB11);
  
  /* Call this function which reads SIB11 */ 
  if ( sib11_ptr == NULL )
  {
     WRRC_MSG0_ERROR("SIB 11 empty.");
    return TVM_FAIL;
  }

  sib12_ptr = (rrc_SysInfoType12*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB12);

  /* SIB 12 present */
  if ((sib11_ptr->sib12indicator == TRUE) && (sib12_ptr != NULL))
  {
    sib12_present = SIB12_PRESENT;
    
    /* Read traffic volume info from SIB12 and send meas req to MAC  */
    
    /* Reinitialize some key mac params */
    rrctvm_initialize();
    
    
    if (rrctvm_process_sib_info(&sib12_ptr->measurementControlSysInfo)
      == TVM_SUCCESS )
    {
      WRRC_MSG0_HIGH("TVM:Successfully read from SIB12. Send cmd to MAC to Setup SIB Meas");
    }
    else
    {
      WRRC_MSG0_HIGH("TVM:Meas not done as per SIB 12");
      rrctvm_initialize(); 
      return TVM_FAIL;
    }
    
  } /* End of if of sib12 PRESENT */
  else
  {
    /* Get traffic volume params from SIB11 */
    
    rrctvm_initialize();
    
    /* Read traffic volume info from SIB12 and send meas req to MAC */
    if (rrctvm_process_sib_info(&sib11_ptr->measurementControlSysInfo) 
      == TVM_FAIL)
    {
      WRRC_MSG0_HIGH("TVM:Not done as per SIB 11");
      rrctvm_initialize();
      return TVM_FAIL;
    }
  }
   /* in PCH state send START with meas_start_incl FALSE otherwise MAC will reject it
        followed by RESUME otherwise MAC will not resume this TVM in FACH/DCH*/
      
  
  if((curr_state == RRC_STATE_CELL_PCH) || (curr_state == RRC_STATE_URA_PCH) )
  {
    mac_sib_params.meas_start_incl = FALSE;
  }
        
  rrctvm_send_mac_cmd(&mac_sib_params);

  if((curr_state == RRC_STATE_CELL_PCH) || (curr_state == RRC_STATE_URA_PCH) )
  {  
    WRRC_MSG0_HIGH("RRC_TVM: sending RESUME for meas on reselection/oos");
          
    /* Ack not reqd */
    mac_sib_params.config_conf_ind = FALSE;
      
    /* Resume the Measurement */
    mac_sib_params.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
    
    rrctvm_send_mac_cmd(&mac_sib_params);  
  }
      
  return TVM_SUCCESS; 

}


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_TVM_ON_CELL_RESELECTION

DESCRIPTION 

  Main handler that processes traffic volume measurements on cell
  reselection in connected mode. Checks the MEASUREMENT_IDENTITY
  variable for any measurement that is owned by UTRAN.
  It saves that measurement and tells MAC to store it. If it does
  not find any measurement in the database that corresponds to the
  default measurement, it reads SIB11/12 again to get the default
  measurement.
 
DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_tvm_on_cell_reselection(void)
{
  uint8 count = 0;
  uint16 tvm_meas_id = INVALID_MEAS_ID;
  
  /* For sending command to MAC */
  mac_ul_traffic_meas_config_type traffic_vol_params;
  /*
  1. Delete old cell's SIB related TVM.
  1. If SIB TVM not present then no need to anything.
  1. If SIB TVM is already part of MCM meas-id, then no need to do anything.
  1. If a different MCM TVM is present, then dont initialize meas-id list.
  1. If non MCM TVM, then initialize everything.
Later found that there is no need to initialize everything since if we delete first anyway.
So now reducing to 
  1. Delete old cell's SIB related TVM.
  1. If SIB TVM not present then no need to anything.
  1. If SIB TVM is already part of MCM meas-id, then no need to do anything.
   1. Configure MAC of new SIB TVM
  */
  
  WRRC_MSG0_HIGH("TVM:Processing on cell reselection");
   /* delete old cell's SIB configured TVM*/
  for (count=0; count <MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if ((tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID) &&
      (tvm_meas_id_list[count].meas_ctrl_msg == FALSE))
    {
      /* Construct a cmd to tell MAC to delete all Measurements */
      traffic_vol_params.config_conf_ind = FALSE;
      
      traffic_vol_params.meas_id = tvm_meas_id_list[count].meas_id;
      traffic_vol_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
      
      WRRC_MSG0_HIGH( "TVM:Sending cmd to MAC to delete SIB Meas");
      rrctvm_send_mac_cmd(&traffic_vol_params); 

        /* Set Meas Id in Meas Identity var to Invalid */
      tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
      tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
      tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
      tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
      tvm_meas_id_list[count].block_rach_incl = FALSE;
  
    }
  }

  /* Check MEASUREMENT_IDENTITY variable for TVM */
  if ((tvm_meas_id = rrctvm_get_meas_id_from_sibs()) == INVALID_MEAS_ID)
  {
    WRRC_MSG0_HIGH("TVM: SIB 11/12 not used");
  }
  else
  {
    for (count=0; count <MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
    {
      if ((tvm_meas_id_list[count].meas_id == tvm_meas_id) &&
      (tvm_meas_id_list[count].meas_ctrl_msg == TRUE))
      {
        WRRC_MSG1_HIGH("TVM:Found TVM = %d owned by MCM and it wil lremain with UE",
        tvm_meas_id_list[count].meas_id);
      
          return;
      
      }
    } /* End of for */ 

    
    /* Read SIB's again. The fn sends cmd to MAC */
    if (rrctvm_process_tvm_sibs_in_connected_state() == TVM_SUCCESS)
    {
      WRRC_MSG0_HIGH("TVM:Meas for new cell O.K");
    }
          
  }
  
}

/*===========================================================================

FUNCTION   RRCTVM_INITIALIZE_MEASUREMENT_IDENTITY

DESCRIPTION 

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Traffic Volume Measurements.
 
DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_initialize_measurement_identity
(
  void
)
{
  uint8 count = 0, cnt = 0;
  /* Initialize traffic volume Meas Id list */
  for (count=0; count <MAX_TRAFFIC_VOLUME_MEASUREMENTS ; count++)
  {
    tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
    tvm_meas_id_list[count].block_rach_incl = FALSE;
    tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
    tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
    tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
    
    /* Also initialize additional meas */
    for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
      tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
    
  }
}


/*===========================================================================

FUNCTION   RRCTVM_GET_MEAS_ID_FROM_SIBS

DESCRIPTION 

  Gets the default traffic volume measurement identity by peeking
  into SIB11/12 of SIB database.
 
DEPENDENCIES

  None

RETURN VALUE

  Returns DEFAULT_TRAFFIC_VOL_MEAS_IDENITY if SIB11/12 does not specify
  a measurement identity or return the measurememt identity specified
  in SIB11/12

  Returns INVALID_MEAS_ID if traffic volume system information is not present 

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

uint16 rrctvm_get_meas_id_from_sibs
(
  void
)
{
  /* Declared pointer to SIB & SIB 12 */
  rrc_SysInfoType11 *sib11_ptr = NULL; 
  
  rrc_SysInfoType12 *sib12_ptr = NULL;
  
  rrc_MeasurementControlSysInfo* meas_ctrl_info_ptr;
  
  uint16 meas_id = INVALID_MEAS_ID;
  
  /* Make SIB12 as absent */
    
  /* Reinitialize some key mac params */
  rrctvm_initialize();
  
  /* Get traffic volume params from SIB11 */
  sib11_ptr = (rrc_SysInfoType11*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB11);
  
  /* Call this function which reads SIB11 */ 
  if ( sib11_ptr == NULL )
  {
     WRRC_MSG0_ERROR("SIB 11 empty.");
    return meas_id;
  }
  else
  {
    meas_ctrl_info_ptr = & sib11_ptr->measurementControlSysInfo;
  }

  sib12_ptr = (rrc_SysInfoType12*) 
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB12);

  /* SIB 12 present */
  if ((sib11_ptr->sib12indicator == TRUE) && (sib12_ptr != NULL))
  {
    meas_ctrl_info_ptr = &sib12_ptr->measurementControlSysInfo;
  }
  
  /* Check if Traffic volume meas identity present */
  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(meas_ctrl_info_ptr,trafficVolumeMeasSysInfo))
  {
 
    WRRC_MSG0_HIGH("TVM:Meas Identity present");
    
    meas_id = (uint16)meas_ctrl_info_ptr->trafficVolumeMeasSysInfo.
      trafficVolumeMeasurementID;
      
    
  }
  return meas_id;
}



/*===========================================================================

FUNCTION   RRCTVM_PROCESS_FACH_TO_DCH

DESCRIPTION 

  The main handler function that handles the processing of
  traffic volume measurements when RRC transitions from 
  FACH-->DCH. Check the MEASUREMENT_IDENTITY variable and
  determine for each measurement what has to be done as per
  its validity. The function accordingly updates the
  MEASUREMENT_IDENTITY variable and sends a command to
  MAC to reconfigure the traffic volume measurements.
    

DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_fach_to_dch
(
  void
)
{ 
  uint8 count = 0;
  
  mac_ul_traffic_meas_config_type tvm_cmd;
  
  WRRC_MSG0_HIGH("TVM:Processing Traffic Vol for FACH-->DCH");
  
  /* Check the Validity in Measurement Identity variable for Traffic Vol Meas */
  
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID)
    {      
      switch(tvm_meas_id_list[count].meas_validity)
      {
      case NOT_PRESENT:
        
        WRRC_MSG1_HIGH("TVM:Validity is absent. Delete Meas = %d",
          tvm_meas_id_list[count].meas_id);
        
        /* Build Delete Command and send to MAC */
        
        /* Cmd type */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
        
        /* Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Cnf not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        WRRC_MSG1_HIGH("TVM:Tell MAC to delete meas = %d ", tvm_cmd.meas_id);
        
        /* Set Meas Id in Meas Identity var to Invalid */
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        break;
        
        
      case ALL_STATES:
        
        WRRC_MSG1_HIGH("TVM:Validity is ALL. Continue meas = %d",
          tvm_meas_id_list[count].meas_id);

    /* Since there are scenarios when MAC does not know about DCH to FACH
       trans and a meas could have both DCH and FACH object, so send MAC
       a explicit resume. In case MAC has stopped the meas, it resumes the meas
       with the new object. In case it is already doing, it ignores the cmd */

    /* Set Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Stop the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        /* No Need to send cmd to MAC */
        break;
        
      case STATE_DCH:
        
        WRRC_MSG1_HIGH("TVM:Validity is DCH only. Resume meas= %d",
          tvm_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Stop the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        break;
        
      case ALL_STATES_BUT_DCH:
        WRRC_MSG1_HIGH("TVM:Validity is all except DCH. Stop,Save meas=%d",
          tvm_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
        
        /* Ack not reqd */
        tvm_cmd.config_conf_ind = FALSE;
        
        /* Resume the Measurement */
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_SAVE;
        
        rrctvm_send_mac_cmd(&tvm_cmd);
        
        break; 
        
      default:
        /* Not possible */
        WRRC_MSG0_ERROR("Invalid meas validity in tvm_meas_id_list");
        return;
        
      } /* End of switch */
      
    } /* end of if */
    
  } /* End of for */
}


/*===========================================================================

FUNCTION   RRCTVM_CHECK_MEAS_ID_ABSENT

DESCRIPTION 

  Checks if the traffic volume measurement identity is present
  in MEASUREMENT_IDENTITY variable. 
    

DEPENDENCIES

  None

RETURN VALUE

  Success if Measurement identity is not present. 
  Failure if Measurement identity is present.

SIDE EFFECTS

  If SIB11 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

rrctvm_status_e_type rrctvm_check_meas_id_absent
(
  void
)
{
  uint8 count = 0; 
  rrctvm_status_e_type status = TVM_SUCCESS;
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID) 
    {
      WRRC_MSG1_HIGH("TVM:Meas Id %d exists", tvm_meas_id_list[count].meas_id);
      status = TVM_FAIL;
      break;
    }
  }
  return status;
}
    

/*===========================================================================

FUNCTION   RRCTVM_INITIALIZE

DESCRIPTION 

    Initializes traffic volume configuration parameters.

DEPENDENCIES

    None

RETURN VALUE

    None  

SIDE EFFECTS

    None
===========================================================================*/

void rrctvm_initialize
(
void
)
{ 
  /* Set default values to key params in mac_sib_params */
  mac_sib_params.meas_id = INVALID_MEAS_ID;
  mac_sib_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
  mac_sib_params.meas_obj_incl = FALSE;
  mac_sib_params.meas_obj_list.num_trch = 0;
  mac_sib_params.meas_qty_incl = FALSE;
  mac_sib_params.meas_report_incl = FALSE;
  mac_sib_params.meas_report_qty.rlc_payload = FALSE;
  mac_sib_params.meas_report_qty.avg_payload = FALSE;
  mac_sib_params.meas_report_qty.var_payload = FALSE;
  mac_sib_params.config_conf_ind = FALSE;
  mac_sib_params.meas_criteria_incl = FALSE;
  mac_sib_params.meas_report_criteria.periodic_cfg.report_infinity = TRUE;
  mac_sib_params.meas_start_incl = TRUE;

}

void  rrctvm_append_traffic_vol_results_ptr
(
   rrc_MeasuredResults * measured_results_ptr,
   mac_ul_traffic_meas_ind_type * additional_tvm
)
{         
    
  
  uint8 count = 0;
    
  /* Declare ptr for ease */
  
    rrc_TrafficVolumeMeasuredResultsList * tvm_results_ptr = NULL;
  
    /* Set Choice to Traffic Volume Meas */
    RRC_SET_COMMON_MSG_TYPE_PTR(measured_results_ptr,rrc_MeasuredResults_trafficVolumeMeasuredResultsList);

       
     /* Assign this pointer to TV measured results */
    

     tvm_results_ptr = 
     rtxMemAllocTypeZ (&enc_ctxt, rrc_TrafficVolumeMeasuredResultsList);

    
     measured_results_ptr->u.trafficVolumeMeasuredResultsList=
        tvm_results_ptr;
    
    tvm_results_ptr->n = additional_tvm->num_rbs;
     ALLOC_ASN1ARRAY2(&enc_ctxt,tvm_results_ptr,rrc_TrafficVolumeMeasuredResults);
    
    for (count=0; count < additional_tvm->num_rbs; count++)
    {
      /* Get radio bearer Id */
      tvm_results_ptr->elem[count].rb_Identity = 
        additional_tvm->rb_meas[count].rb_id;
      
      RRC_RESET_MSG_IE_PRESENT(tvm_results_ptr->elem[count]);
      /* Now check the payload in each radio bearer */
      if (additional_tvm->rb_meas[count].raw_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("Raw RLC payload not present",0,0,0);
      }
      else
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(tvm_results_ptr->elem[count],
                                            rlc_BuffersPayload); 
        
        /* Convert the size */        
        tvm_results_ptr->elem[count].rlc_BuffersPayload = 
          TVM_RET_RLC_BUFFER_PAYLOAD(additional_tvm->rb_meas[count].raw_rlc_payload);
        
        WRRC_MSG2_HIGH("TVM:RB = %d, Raw RLC payload sent = %d",
          additional_tvm->rb_meas[count].rb_id,
          additional_tvm->rb_meas[count].raw_rlc_payload);
        
      }
      if (additional_tvm->rb_meas[count].average_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("Avge RLC payload not present",0,0,0);
      }
      else
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
            tvm_results_ptr->elem[count],
            averageRLC_BufferPayload); 
        
        /* Convert the size in bytes */
        tvm_results_ptr->elem[count].averageRLC_BufferPayload = 
          TVM_RET_AVG_BUFFER_PAYLOAD(additional_tvm->rb_meas[count].average_rlc_payload);
        
        WRRC_MSG2_HIGH("TVM:RB = %d, Avge RLC payload sent = %d", 
          additional_tvm->rb_meas[count].rb_id,
          additional_tvm->rb_meas[count].average_rlc_payload);
        
      }
      if (additional_tvm->rb_meas[count].variance_rlc_payload == INVALID_PAYLOAD)
      {
        MSG_LOW("Var RLC payload not present",0,0,0);
      }
      else
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
            tvm_results_ptr->elem[count],
            varianceOfRLC_BufferPayload);
        
        /* Convert the size in bytes */
        tvm_results_ptr->elem[count].varianceOfRLC_BufferPayload = 
          TVM_RET_VAR_BUFFER_PAYLOAD(additional_tvm->rb_meas[count].variance_rlc_payload);
        
        WRRC_MSG2_HIGH("TVM:RB = %d,Var Payload = %ul", 
          additional_tvm->rb_meas[count].rb_id,
          additional_tvm->rb_meas[count].variance_rlc_payload);
        
      }
      
      
      /* TVM additional results successfully appended */
      
    } /* End of for loop */
    
    

}
/*===========================================================================

FUNCTION   RRCTVM_APPEND_TRAFFIC_VOL_RESULTS

DESCRIPTION 
      
  This function appends Traffic Volume Measurements as additional measurements
  to any other Measurement Report 

DEPENDENCIES

  The called function appends the element at the end of 
  the list by allocating memory. The calling function must set
  the first element of the list to NULL if the list is empty

RETURN VALUE

  True
  False

SIDE EFFECTS

  None

===========================================================================*/

boolean rrctvm_append_traffic_vol_results
(
 uint8* additional_meas_id_ptr,
 mac_ul_traffic_meas_ind_type * additional_tvm
)
{
  /* Would store Additional Results from MAC */

  
  uint8 count = 0;
  
  boolean addtl_meas_present = FALSE;
  
  /* Declare ptr for ease */
  
  
  WCDMA_RRC_TASK_LOCK();
  
  addtl_meas_present = mac_get_additional_traffic_vol_msmt_results(*additional_meas_id_ptr, additional_tvm);
  
  WCDMA_RRC_TASK_FREE();
  /* Call MAC function to append Traffic Volume Meas Results */
  if (addtl_meas_present == FALSE)
  {
    WRRC_MSG1_HIGH("TVM:Addl Traffic Vol Meas %d not appended", *additional_meas_id_ptr);
    return FALSE;
  }
  
  /* Check if RB's are not set to 0 */
  if ((additional_tvm->num_rbs == 0) || (additional_tvm->num_rbs >= (MAX_RB-1)))
  {
    WRRC_MSG1_ERROR("TVM:No of RB's for Addl TVM is %d",additional_tvm->num_rbs);
    return FALSE;
  }
  
  /* Validate the Report */
  for (count=0; count < additional_tvm->num_rbs; count++)
  {
    /* Get radio bearer Id */
    if (additional_tvm->rb_meas[count].rb_id > (MAX_RB-1))
    {
      WRRC_MSG1_ERROR("TVM:RB Id %d invalid",additional_tvm->rb_meas[count].rb_id);
      return FALSE;
    }
  } /* End of for */
  
  return TRUE;

}

/*===========================================================================

FUNCTION   RRCTVM_PROCESS_MEAS_IDENTITY

DESCRIPTION 

    Set traffic volume meas identity variable from SIB information.

DEPENDENCIES

    None

RETURN VALUE

    None  

SIDE EFFECTS

    None
===========================================================================*/

void rrctvm_process_mcm_identity
(
uint16* meas_id_ptr
)
{
  uint8 count = 0;
  /* First check if the meas id already exists */
  
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == *meas_id_ptr)
    {
      WRRC_MSG1_HIGH("TVM:Meas Id = %d already existing",*meas_id_ptr);
      /* Set owner to MCM */
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      return;
    }
    
  } /* End of for loop */ 
  
  
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      WRRC_MSG1_HIGH("TVM:Setting Meas Id = %d in tvm_meas_id_list",
        *meas_id_ptr);        
      tvm_meas_id_list[count].meas_id =  (uint8) *meas_id_ptr;
      
      /* Set owner to MCM */
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      return;
    }
  }
  /* The following should never happen */
  WRRC_MSG0_ERROR("TVM:Unable to set TVM Measurement Identity");
  return;
  
}


/*===========================================================================

FUNCTION   RRCTVM_VALIDATE_ADDITIONAL_MEAS

DESCRIPTION 
      
  This function checks if additional measurements exists in RRC database and have the
  same validity as the primary measurement. If it
  exists, it returns its type else returns unknown Measurement which essentially
  implies that the additional measurement does not exist. The function also returns
  the reporting criteria as per the measurement type. The reporting criteria is used by
  the calling function to determine if UTRAN is trying to append an event triggered meas
    

DEPENDENCIES

  None. 

RETURN VALUE

  False if invalid. Meas Ctrl Failure already sent.

SIDE EFFECTS

  None

===========================================================================*/

boolean rrctvm_validate_additional_meas
(
 meas_validity_enum_type* p_meas_validity,
 rrc_AdditionalMeasurementID_List * additional_list_ptr,
 uint32 mcm_tr_id,
 uint32 mid
)
{
  uint8 count = 0;
  uint32 idx=0;
  rrcmeas_layer_e_type additional_meas_type;
  
  /* Reporting Criteria */
  rpt_crit_enum_type a_rpt_crit;
  
  uint16 additional_meas_id;
  
  /* Set validity to default value */
  meas_validity_enum_type addtl_meas_validity = NOT_PRESENT;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;

  /* Now compare if validity is the same */
  while (additional_list_ptr->n > idx)
  {
    additional_meas_id = (uint16) additional_list_ptr->elem[idx];
    
    /* First check if additional measurement exists in RRC DB or not */
    
    additional_meas_type = rrcmeas_find_additional_meas_type(&additional_meas_id, &a_rpt_crit, &addtl_meas_validity);
      if (additional_meas_type == QUALITY_MEAS)
      {
        WRRC_MSG2_ERROR("Addtl QualityMeas %d, for PMeas %d not allowed",additional_meas_id,mid);
        /* Set transaction ID */ 
        rrcmeas_mcf.rrc_transaction_id = mcm_tr_id;

        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return FALSE;
      }
    if (additional_meas_type == UNKNOWN_MEAS)
      
    {
      WRRC_MSG2_ERROR("TVM:Addtl Meas %d unknown for PMeas %d ",additional_meas_id,
        mid);
      /* Set transaction ID */ 
      rrcmeas_mcf.rrc_transaction_id = mcm_tr_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FALSE;
    } /* End of if of rrcmeas_find_additional_meas_type */
    
    /* Check if more than one meas of same type would be include with Prim Meas */
    if (additional_meas_type == MAC_MEAS)
      
    {
      WRRC_MSG2_ERROR("TVM:Addtl Meas %d PMeas of same type %d",additional_meas_id,
        mid);
      /* Set transaction ID */ 
      rrcmeas_mcf.rrc_transaction_id = mcm_tr_id;
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FALSE;
    } /* End of if of rrcmeas_find_additional_meas_type */
    
    
      /* UTRAN might messup by making addtl meas as Event Triggered for 
    Intra Freq, Inter Freq, Inter RAT */
    
    if (((additional_meas_type == INTER_FREQ) && (a_rpt_crit == EVENT)) || 
      ((additional_meas_type == INTER_SYS)  && (a_rpt_crit == EVENT)) ||
      ((additional_meas_type == INTERNAL_MEAS)  && (a_rpt_crit == EVENT)))
    {
      WRRC_MSG0_ERROR("TVM:Addtl Meas Intra/InterF/S/Internal EventTrig");
      /* Set transaction ID */ 
      rrcmeas_mcf.rrc_transaction_id = mcm_tr_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FALSE;
    }
    /* Get VALIDITY of primary measurement */
    
    /* Check validity of the additional and primary meas */
    /* For now, since additional is non traffic volume, 
    it is valid in CELL_DCH only */
    if (addtl_meas_validity == *p_meas_validity)
    {
      WRRC_MSG0_HIGH("TVM:Validity Addtl & Primary Meas Match");
    }
    /* Possible as Traffic Vol is ALL_STATES and addtl is CELL_DCH */
    else if ((*p_meas_validity == ALL_STATES) || ((*p_meas_validity == NOT_PRESENT) &&
      (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
    {
      WRRC_MSG0_HIGH("TVM:P.Meas Validity is all states/Not Present");
    }
    else
    {
      WRRC_MSG2_ERROR("TVM:Validity Addtl Meas %d,P Meas %d Differ",
        additional_meas_id,tvm_meas_id_list[count].meas_id);
      return FALSE;
    }
    
    /* Go to the next addtl Meas to be appended */
    idx++;
    
  } /* End of while */
  /* If we reach this stage, then additional meas are valid */
  return TRUE;  
}



/*===========================================================================

FUNCTION   RRCTVM_CONFIG_TVM_DB

DESCRIPTION 

 Sets Traffic Volume Meas Information in RRC Measurement Database.
 
DEPENDENCIES

 None

RETURN VALUE

 None  

SIDE EFFECTS

 None
===========================================================================*/

boolean rrctvm_config_tvm_db
(
 mac_ul_traffic_meas_config_type* mac_mcm_ptr,
 meas_validity_enum_type* p_meas_validity
)
{
  uint8 count = 0;
  /* First check if the meas id already exists */

  meas_validity_enum_type validity = NOT_PRESENT;
  
  /* SETUP existing & MODIFY scenario */
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == mac_mcm_ptr->meas_id)
    {
      WRRC_MSG1_HIGH("TVM:Meas Id = %d found in RRC db",mac_mcm_ptr->meas_id);
      
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* Check if it is a MODIFY cmd */
      if (mac_mcm_ptr->meas_cmd == MAC_TRAFFIC_MEAS_MODIFY)
      {
        /* Reporting Criteria for MODIFY */
        /* meas_criteria_incl is set to TRUE only if reporting crit is specified */
        if (mac_mcm_ptr->meas_criteria_incl)
        {
          tvm_meas_id_list[count].rpt_crit = TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
          
          WRRC_MSG1_HIGH("TVM:Meas Crit %d saved in RRC", tvm_meas_id_list[count].rpt_crit);
          
          /* If for MODIFY, Validity is not present, then copy it from Meas DB */
          if (*p_meas_validity != validity)
          {
            tvm_meas_id_list[count].meas_validity = *p_meas_validity;
          }
        } /* End of if of meas_criteria_incl */
      } /* End of MODIFY */
      else /* SETUP case */
      {
        tvm_meas_id_list[count].rpt_crit = 
          TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
        
          tvm_meas_id_list[count].meas_validity = *p_meas_validity;
          WRRC_MSG2_HIGH("TVM:Validity : %d Meas Crit : %d saved in RRC DB", *p_meas_validity,tvm_meas_id_list[count].rpt_crit);
      }
      
      return TRUE;
    } /* End of if of Meas Id found */
    
  } /* End of for loop */ 
  WRRC_MSG1_HIGH("TVM:New Meas Id = %d configured. Set params",mac_mcm_ptr->meas_id);
  
  /* SETUP not existing scenario */
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    /* If Meas Id not found, then it is a new Meas. Set it in first available position */
    if (tvm_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      WRRC_MSG1_HIGH("TVM:Setting Meas Id = %d in RRC db",mac_mcm_ptr->meas_id);
      
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      tvm_meas_id_list[count].meas_id = (uint8) mac_mcm_ptr->meas_id;
      
      /* Set Validity to CELL_DCH */
      tvm_meas_id_list[count].meas_validity = *p_meas_validity;
      
      tvm_meas_id_list[count].rpt_crit = TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
      
      return TRUE;
    }
    
  } /* End of for loop */ 
  
  WRRC_MSG0_ERROR("TVM:RRC MeasDB for TVM FULL!!");
  return FALSE;

}


/*===========================================================================

FUNCTION   RRCTVM_SET_REPORTING_CRITERIA

DESCRIPTION 
            
    This function sets reporting criteria in tvm_meas_id_list from SIB's
    

DEPENDENCIES

    None 

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrctvm_set_reporting_criteria
(
  uint16* meas_id_ptr,
  rrc_TrafficVolumeReportCriteriaSysInfo* sib_rpt_crit_ptr
)
{
  uint8 count = 0;
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == *meas_id_ptr)
    {
      /* In SIB's, addtl meas cannot be configured */
      if (RRC_CHECK_COMMON_MSG_TYPE2_PTR(sib_rpt_crit_ptr,
        rrc_TrafficVolumeReportCriteriaSysInfo,trafficVolumeReportingCriteria))
      {
        WRRC_MSG0_HIGH("TVM:SIB:Event Triggered Crit");
        tvm_meas_id_list[count].rpt_crit = EVENT;
      }
      else
      {
        WRRC_MSG0_HIGH("TVM:SIB:Periodic Crit");
        tvm_meas_id_list[count].rpt_crit = PERIODIC;
      }
      break;
    }
  } /* End of for */

}


/*===========================================================================

FUNCTION  RRCTVM_PROCESS_SIB_CHANGE_NOTIFICATION

DESCRIPTION  

 This function processes SIB change notification to modify TVM 

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrctvm_process_sib_change_notification
(
 rrc_sib_change_mask_type sib_change_mask
)
{
  uint16 tvm_meas_id = INVALID_MEAS_ID;
  
  uint32 count = 0;
  
  boolean tvm_sib_valid = FALSE;
  rrc_state_e_type curr_state = rrc_get_state();

  
  mac_ul_traffic_meas_config_type tvm_cmd;
  
  /* Find if it is SIB11 or SIB12 */
  if (sib_change_mask & SIB_11_BIT)
  {
    WRRC_MSG0_HIGH("TVM:SIB11 changed");
  }
  if(sib_change_mask & SIB_12_BIT)
  {
    WRRC_MSG0_HIGH("TVM:SIB12 changed");
  }
  /* delete old SIB11/12 TVM config*/
  for (count=0; count <MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if ((tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID) &&
      (tvm_meas_id_list[count].meas_ctrl_msg == FALSE))
    {
      /* Construct a cmd to tell MAC to delete all Measurements */
      tvm_cmd.config_conf_ind = FALSE;
      
      tvm_cmd.meas_id = tvm_meas_id_list[count].meas_id;
      tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
      
      WRRC_MSG0_HIGH( "TVM:Sending cmd to MAC to delete SIB Meas");
      rrctvm_send_mac_cmd(&tvm_cmd); 
        /* Set Meas Id in Meas Identity var to Invalid */
      tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
      
      tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
      
      tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
      
      tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
      
      tvm_meas_id_list[count].block_rach_incl = FALSE;
      
      break;
    }
  }
  
  /* First do certain checks before processing it.
  Find if default meas identity or other meas identity
  exists in SIB12 or SIB11. If not, then the fn returns
  INVALID_MEAS_ID. */ 
  
  if ((tvm_meas_id = rrctvm_get_meas_id_from_sibs()) == INVALID_MEAS_ID)
  {
    WRRC_MSG0_HIGH("TVM: SIB 11/12 not used");
  }
  else 
  {
    /* Read new SIB's in mac_sib_params */
    if (rrctvm_get_sibs_in_connected_state() == TVM_SUCCESS)
    {
      tvm_sib_valid = TRUE;
      
      /* Find if any MCM is present in DB and has same id as one in tvm_meas_id (New SIB Mid) 
      If yes, then cannot overwrite that with the one from SIB's. */
      for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
      {
        if ((tvm_meas_id_list[count].meas_id == tvm_meas_id) &&
          (tvm_meas_id_list[count].meas_ctrl_msg == FALSE))
        {
          /* SIB11/12 TVM meas id is no confgured by MCM */
          break;
        } /* End of if of meas_id as valid and MCM does not own the meas */
        else if ((tvm_meas_id_list[count].meas_id == tvm_meas_id) &&
          (tvm_meas_id_list[count].meas_ctrl_msg == TRUE))
        {
          WRRC_MSG1_HIGH("TVM:New SIB Meas Id %d == MCM Meas Id .. TVM:No new cmd to be sent to MAC", tvm_meas_id);
          return;
        }
        else
        {
          /* Do Nothing */
        }
      } /* End of for of count of MAX_TRAFFIC_VOLUME_MEASUREMENTS */
      
      WRRC_MSG1_HIGH("TVM:No Meas in RRC DB matches SIB11/12 %d Meas", tvm_meas_id);
      if (tvm_sib_valid == TRUE)
      {
        tvm_cmd = mac_sib_params;
        tvm_cmd.config_conf_ind = FALSE;
        tvm_cmd.meas_cmd = MAC_TRAFFIC_MEAS_START;
        
        /* in PCH state send START with meas_start_incl FALSE otherwise MAC will reject it
        followed by RESUME otherwise MAC will not resume this TVM in FACH/DCH*/

        if((curr_state == RRC_STATE_CELL_PCH) || (curr_state == RRC_STATE_URA_PCH) )
        {
          tvm_cmd.meas_start_incl = FALSE;
        }
          
        rrctvm_send_mac_cmd(&tvm_cmd);  
      
        if((curr_state == RRC_STATE_CELL_PCH) || (curr_state == RRC_STATE_URA_PCH) )
        {  
          WRRC_MSG0_HIGH("RRC_TVM: sending RESUME for meas on reselection/oos");
              
              /* Ack not reqd */
          mac_sib_params.config_conf_ind = FALSE;
              
              /* Resume the Measurement */
          mac_sib_params.meas_cmd = MAC_TRAFFIC_MEAS_RESUME;
            
          rrctvm_send_mac_cmd(&mac_sib_params);  
        }
      }
      
    } /* End of if of SIB's valid in connected mode */
    
  } /* End of else of SIB11/12 Meas Id being valid */

}

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R6

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R6. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r6
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;

  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint32 transaction_identifier = 0;

  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  

  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;
  
  boolean block_set = FALSE;
  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  /* Assign some default values */
  mac_mcm_params.meas_start_incl = TRUE;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6;

  /* Store the Transaction Identifier */
  transaction_identifier = tid;
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r6_setup)
  {
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement, 
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement, 
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }

    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
 
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
        transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        return TVM_FAIL;
      }
      
      /* This means additional meas has been successfully validated */
      addtl_meas_validation = TRUE;
      
    } /* End of if of Additional Meas Present */
    
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);

    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
      
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
    }
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r6_modify)
  {
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r6_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_r6_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
           rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }

        /* Process Additional Meas */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,
            additionalMeasurementList))
        {
          WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

          if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
            transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
          {
            /* Called function has already sent Meas Ctrl Failure */
            return TVM_FAIL;
          }
          
          /* This means additional meas has been successfully validated */
          addtl_meas_validation = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
            return TVM_FAIL;
          
          }
        
        } /* End of if of Additional Meas Present */
        

        rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
        
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }

        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);

      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

        if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
          transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          return TVM_FAIL;
        }
        
        /* This means additional meas has been successfully validated */
        addtl_meas_validation = TRUE;
        
      } /* End of if of Additional Meas Present */
      
      if (rrcmeas_check_mcm_for_setup_modify_additional(
        addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
      {
        /* As UTRAN tried to configure more than 4 Addtl Meas */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
        
      }
      
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }

      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
      }
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        rrc_mcm_ptr->measurementIdentity)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      rrc_mcm_ptr->measurementIdentity)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */
  
  
  /* Now send config command to MAC */
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
  mac_mcm_params.config_ack_num = alloc_idx;
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}

/*===========================================================================

FUNCTION   rrctvm_is_traffic_volume_indicator_to_be_set_in_cu

DESCRIPTION 
  This function checks if there is any TVM with measurement ID 4, if that measurement is event trigerred
  and valid in all states or all states except DCH, if any present calls the mac function to check if the
  traffic volume is above the configured threshold for event 4A for that measurement

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if there is any TVM with measurement ID 4, if that measurement is event trigerred and valid in 
        all states or all states except DCH, if any present calls the mac function and if traffic volume
        is above the configured threshold for event 4A for that measurement
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rrctvm_is_traffic_volume_indicator_to_be_set_in_cu
(
  void
)
{
  int i=0;
  boolean status = FALSE;

#ifdef FEATURE_QCHAT
  if(qchatupk_wcdma_icup_enabled())
  {
    if(qchatupk_rrctvm_is_traffic_volume_indicator_to_be_set_in_cu() == TRUE)
    {
      WRRC_MSG0_HIGH("QCHAT is setting the TVI in CU OTA");
      return qchatupk_rrctvm_is_traffic_volume_indicator_to_be_set_in_cu();
    }
  }
#endif /*FEATURE_QCHAT*/ 
  
  for(i=0;i<MAX_TRAFFIC_VOLUME_MEASUREMENTS;i++)
  {
    /* 25.331 
    if the TCTV is larger than the threshold in the IE "Reporting threshold" for a traffic volume measurement stored in the 
    MEASUREMENT_IDENTITY variable and that traffic volume measurement has "measurement identity" equal to 4, 
    "Traffic volume event identity" equal to "4a", "Measurement validity" equal to "all states" or "all states except CELL_DCH";
    */
    if(tvm_meas_id_list[i].meas_id == 4) 
    {
      if((tvm_meas_id_list[i].rpt_crit == EVENT) && 
        (tvm_meas_id_list[i].meas_validity == ALL_STATES || tvm_meas_id_list[i].meas_validity == ALL_STATES_BUT_DCH))
      {
        /*Call the MAC function to check the threshold and the event identity*/
         WCDMA_RRC_TASK_LOCK();
            status = mac_compute_traffic_volume_above_threshold_cu(4,rrcllc_get_rach_cctrch_ptr()
#ifdef FEATURE_WCDMA_HS_RACH 
            ,rrcllc_get_hsrach_config_ptr()
#endif
            );
        WCDMA_RRC_TASK_FREE();
     
        if(status)
        {
          WRRC_MSG0_HIGH("TVM: Set traffic volume indicator in CU");
        }
        else
        {
          WRRC_MSG0_HIGH("TVM: Threshold is not exceeded");
        }
      }
      else
      {
        WRRC_MSG2_HIGH("TVM: Meas id is 4 report criteria is %d  and validity is %d",tvm_meas_id_list[i].rpt_crit,
            tvm_meas_id_list[i].meas_validity);
        status = FALSE;
      }
      break;
    }
  }
  return status;
}

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R7

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R7. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r7
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;

  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint32 transaction_identifier = 0;

  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  
  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  boolean block_set = FALSE;
  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  /* Assign some default values */
  mac_mcm_params.meas_start_incl = TRUE;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;

  /* Store the Transaction Identifier */
  transaction_identifier = tid;
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r7_setup)
  {
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }

    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement, 
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
 
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
        transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        return TVM_FAIL;
      }
      
      /* This means additional meas has been successfully validated */
      addtl_meas_validation = TRUE;
      
    } /* End of if of Additional Meas Present */
    
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);

    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
      
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
    }
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r7_modify)
  {
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r7_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_r7_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
           rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }

        /* Process Additional Meas */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
        {
          WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

          if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
            transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
          {
            /* Called function has already sent Meas Ctrl Failure */
            return TVM_FAIL;
          }
          
          /* This means additional meas has been successfully validated */
          addtl_meas_validation = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
            return TVM_FAIL;
          
          }
        
        } /* End of if of Additional Meas Present */
        

        rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
        
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }

        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);

      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

        if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
          transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          return TVM_FAIL;
        }
        
        /* This means additional meas has been successfully validated */
        addtl_meas_validation = TRUE;
        
      } /* End of if of Additional Meas Present */
      
      if (rrcmeas_check_mcm_for_setup_modify_additional(
        addtl_meas_validation, MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
      {
        /* As UTRAN tried to configure more than 4 Addtl Meas */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
        
      }
      
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }

      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
      }
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        rrc_mcm_ptr->measurementIdentity)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      rrc_mcm_ptr->measurementIdentity)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
  mac_mcm_params.config_ack_num = alloc_idx;

  /* Now send config command to MAC */
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R8

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R8. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r8
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;

  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint32 transaction_identifier = 0;

  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  
  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  boolean block_set = FALSE;
  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  /* Assign some default values */
  mac_mcm_params.meas_start_incl = TRUE;

  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;

  /* Store actual mcm ptr in a local variable */
  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;

  /* Store the Transaction Identifier */
  transaction_identifier = tid;
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_setup)
  {
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }

    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
 
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
    {
      WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);
      
      /* Call fn which makes check for additional measurements */
      if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
        transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
      {
        /* Called function has already sent Meas Ctrl Failure */
        return TVM_FAIL;
      }
      
      /* This means additional meas has been successfully validated */
      addtl_meas_validation = TRUE;
      
    } /* End of if of Additional Meas Present */
    
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);

    if (rrcmeas_check_mcm_for_setup_modify_additional(
      addtl_meas_validation,  MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
    {
      /* As UTRAN tried to configure more than 4 Addtl Meas */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
      
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
        rrc_mcm_ptr->measurementIdentity);
      WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
    }
  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r8_modify)
  {
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,
        rrc_MeasurementControl_r8_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r8_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_r8_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
           rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }

        /* Process Additional Meas */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
        {
          WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

          if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
            transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
          {
            /* Called function has already sent Meas Ctrl Failure */
            return TVM_FAIL;
          }
          
          /* This means additional meas has been successfully validated */
          addtl_meas_validation = TRUE;
          if (rrcmeas_check_mcm_for_setup_modify_additional(
            addtl_meas_validation,  MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
            &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
          {
            /* As UTRAN tried to configure more than 4 Addtl Meas */
            rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
            /* Set error t */
            rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
          
            /* Send MCF msg to UTRAN */  
            rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
            return TVM_FAIL;
          
          }
        
        } /* End of if of Additional Meas Present */
        

        rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);
        
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }

        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
            rrc_mcm_ptr->measurementIdentity);
          WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity(&mac_mcm_params.meas_id);

      /* Process Additional Meas */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,additionalMeasurementList))
      {
        WRRC_MSG1_HIGH("TVM:Addtl Meas given for Id %d", rrc_mcm_ptr->measurementIdentity);

        if (rrctvm_validate_additional_meas(&prim_meas_validity, &rrc_mcm_ptr->additionalMeasurementList,
          transaction_identifier, rrc_mcm_ptr->measurementIdentity) == FALSE)
        {
          /* Called function has already sent Meas Ctrl Failure */
          return TVM_FAIL;
        }
        
        /* This means additional meas has been successfully validated */
        addtl_meas_validation = TRUE;
        
      } /* End of if of Additional Meas Present */
      
      if (rrcmeas_check_mcm_for_setup_modify_additional(
        addtl_meas_validation,  MAC_MEAS, rrc_mcm_ptr->measurementIdentity,
        &rrc_mcm_ptr->additionalMeasurementList) == FALSE)
      {
        /* As UTRAN tried to configure more than 4 Addtl Meas */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_invalidConfiguration;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
        
      }
      
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }

      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_set_additional_meas_in_db(&meas_type, &rrc_mcm_ptr->additionalMeasurementList,
          rrc_mcm_ptr->measurementIdentity);
        WRRC_MSG0_HIGH("TVM:Additional Meas set in RRC DB");
      }
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    /* Set Meas Identity */
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        rrc_mcm_ptr->measurementIdentity)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      rrc_mcm_ptr->measurementIdentity)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */
  
  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
  mac_mcm_params.config_ack_num = alloc_idx;

  /* Now send config command to MAC */
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}

/*===========================================================================

FUNCTION   rrctvm_delete_sib_tvm_config

DESCRIPTION 

  Main handler that deletes the sib based tvm configuration
 
DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_delete_sib_tvm_config(void)
{

  uint32 count = 0;

    /* For sending command to MAC */
  mac_ul_traffic_meas_config_type traffic_vol_params;

   /* delete old cell's SIB configured TVM*/
  for (count=0; count <MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if ((tvm_meas_id_list[count].meas_id != INVALID_MEAS_ID) &&
      (tvm_meas_id_list[count].meas_ctrl_msg == FALSE))
    {
      /* Construct a cmd to tell MAC to delete all Measurements */
      traffic_vol_params.config_conf_ind = FALSE;
      
      traffic_vol_params.meas_id = tvm_meas_id_list[count].meas_id;
      traffic_vol_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
      
      WRRC_MSG0_HIGH( "TVM:Sending cmd to MAC to delete SIB Meas");
      rrctvm_send_mac_cmd(&traffic_vol_params); 

        /* Set Meas Id in Meas Identity var to Invalid */
      tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
      tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
      tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
      tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
      tvm_meas_id_list[count].block_rach_incl = FALSE;
  
    }
  }
}

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================

FUNCTION   RRCTVM_CONFIG_TVM_DB_R9

DESCRIPTION 

 Sets Traffic Volume Meas Information in RRC Measurement Database for rel 9
 
DEPENDENCIES

 None

RETURN VALUE

 Boolean  

SIDE EFFECTS

 None
===========================================================================*/

boolean rrctvm_config_tvm_db_r9
(
 mac_ul_traffic_meas_config_type* mac_mcm_ptr,
 meas_validity_enum_type* p_meas_validity,
 rrc_MeasurementControl_r9 *mcm_r9 
)
{
  uint8 count = 0;
  uint8 meas_id_val = 0;
  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;
  meas_validity_enum_type validity = NOT_PRESENT;


  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;

  
  /* First check if the meas id already exists */
  /* SETUP existing & MODIFY scenario */
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == mac_mcm_ptr->meas_id)
    {
      WRRC_MSG1_HIGH("TVM:Meas Id = %d found in RRC db",mac_mcm_ptr->meas_id);
      
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* Check if it is a MODIFY cmd */
      if (mac_mcm_ptr->meas_cmd == MAC_TRAFFIC_MEAS_MODIFY)
      {
        /* Reporting Criteria for MODIFY */
        /* meas_criteria_incl is set to TRUE only if reporting crit is specified */
        if (mac_mcm_ptr->meas_criteria_incl)
        {
          tvm_meas_id_list[count].rpt_crit = TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
          
          WRRC_MSG1_HIGH("TVM:Meas Crit %d saved in RRC", tvm_meas_id_list[count].rpt_crit);
          
          /* If for MODIFY, Validity is not present, then copy it from Meas DB */
          if (*p_meas_validity != validity)
          {
            tvm_meas_id_list[count].meas_validity = *p_meas_validity;
          }
        } /* End of if of meas_criteria_incl */
      } /* End of MODIFY */
      else /* SETUP case */
      {
        tvm_meas_id_list[count].rpt_crit = 
          TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
        
          tvm_meas_id_list[count].meas_validity = *p_meas_validity;
        
        
        WRRC_MSG2_HIGH("TVM:Meas Crit %d Validity %d saved in RRC", tvm_meas_id_list[count].rpt_crit, *p_meas_validity);
      }
      
      return TRUE;
    } /* End of if of Meas Id found */
    
  } /* End of for loop */ 
  WRRC_MSG1_HIGH("TVM:New Meas Id = %d configured. Set params",mac_mcm_ptr->meas_id);
  
  /* SETUP not existing scenario */
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    /* If Meas Id not found, then it is a new Meas. Set it in first available position */
    if (tvm_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      WRRC_MSG1_HIGH("TVM:Setting Meas Id = %d in RRC db",mac_mcm_ptr->meas_id);
      
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
      {
        tvm_meas_id_list[count].meas_id = meas_id_val;

        tvm_meas_id_list[count].meas_id_legacy = rrc_mcm_ptr->measurementIdentity;
      }
      else
      {
        tvm_meas_id_list[count].meas_id = rrc_mcm_ptr->measurementIdentity;
      }

      
      /* Set Validity to CELL_DCH */
      tvm_meas_id_list[count].meas_validity = *p_meas_validity;
      
      tvm_meas_id_list[count].rpt_crit = TVM_RPT_CRIT(mac_mcm_ptr->meas_report_mode.report_mode);
      
      return TRUE;
    }
    
  } /* End of for loop */ 
  
  WRRC_MSG0_ERROR("TVM:RRC MeasDB for TVM FULL!!");
  return FALSE;

}


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_MEAS_IDENTITY_R9

DESCRIPTION 

    Set traffic volume meas identity variable from SIB information.

DEPENDENCIES

    None

RETURN VALUE

    None  

SIDE EFFECTS

    None
===========================================================================*/
void rrctvm_process_mcm_identity_r9
(
uint16* meas_id_ptr,
rrc_MeasurementControl_r9 *mcm_r9 

)
{
  uint8 count = 0;
  uint8 meas_id_val = 0;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;
  
  /* First check if the meas id already exists */
  
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == *meas_id_ptr)
    {
      WRRC_MSG1_HIGH("TVM:Meas Id = %d already existing",*meas_id_ptr);
      /* Set owner to MCM */
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      return;
    }
    
  } /* End of for loop */ 
  
  
  for (count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++)
  {
    if (tvm_meas_id_list[count].meas_id == INVALID_MEAS_ID)
    {
      WRRC_MSG1_HIGH("TVM:Setting Meas Id = %d in tvm_meas_id_list",
        *meas_id_ptr);        

      if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
      {
        tvm_meas_id_list[count].meas_id = meas_id_val;

        tvm_meas_id_list[count].meas_id_legacy = rrc_mcm_ptr->measurementIdentity;

        WRRC_MSG1_HIGH("Setting Legacy Meas Id = %d in RRC db",rrc_mcm_ptr->measurementIdentity);

      }
      else
      {
        tvm_meas_id_list[count].meas_id = rrc_mcm_ptr->measurementIdentity;
      }

      
      /* Set owner to MCM */
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      return;
    }
  }
  /* The following should never happen */
  WRRC_MSG0_ERROR("TVM:Unable to set TVM Measurement Identity");
  return;
  
}

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R9

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R8. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r9
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
)
{
  uint8 count = 0, cnt = 0, trch_cnt = 0;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL;

  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  uint32 transaction_identifier = 0;

  /* Declare MCM config structure that goes to MAC */
  mac_ul_traffic_meas_config_type  mac_mcm_params;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  
  /* Variable defined to check if addtl present in MCM is valid
  This varibale is set to TRUE if addtl meas is valid. True means
  in setting addtl meas in RRC db */
  boolean addtl_meas_validation = FALSE;
  
  meas_validity_enum_type prim_meas_validity = NOT_PRESENT;
  
  rrcmeas_layer_e_type meas_type = MAC_MEAS;
  
  uint8 alloc_idx = MAXIMUM_TRANSACTIONS;

  boolean block_set = FALSE;

  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;

  memset(&mac_mcm_params,0,sizeof(mac_ul_traffic_meas_config_type));
  /* Assign some default values */
  mac_mcm_params.meas_start_incl = TRUE;

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
    mac_mcm_params.meas_id = meas_id_val;
  }
  else
  {
    mac_mcm_params.meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
  }

  /* Store the Transaction Identifier */
  transaction_identifier = tid;
  if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r9_setup)
  {
    /* Check if meas quantity and reporting quantity received. If either of them
    * not received, then configuration incomplete */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasQuantity)) &&
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeReportingQuantity)))
    {
    /* If Reporting quantity indicates var or average but measurement quantity
      * does not indicate time interval, then configuration incomplete */
      if ((rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage == TRUE) ||
        (rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
        trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance == TRUE))
      {
        /* Check if meas quantity is equal to RAW. If yes, then ERROR */
        if (RRC_CHECK_COMMON_MSG_TYPE2(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement->
          trafficVolumeMeasQuantity,rrc_TrafficVolumeMeasQuantity,rlc_BufferPayload))
        {
          WRRC_MSG0_ERROR("TVM:R.Quan = Var/Avge, M.Quan= Raw");
          
          /* Tr Id */
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          /* Send Meas Ctrl Failure Message */
          
          return TVM_FAIL;
        }
        
      }
      
      /* Now check if reporting mode present */
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode)))
      {
        /* Meas reporting Mode absent. Send Meas Ctrl Failure */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of if Meas Quan, Reporting Quan present */
    else
    {
      WRRC_MSG0_ERROR("MeasQuantity or ReportingQuantity absent");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
    }
    
    
    /* Time to process the actual parameters */
    if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
      u.setup->u.trafficVolumeMeasurement,&mac_mcm_params)
      == TVM_FAIL)
    {
      /* Send Meas Ctrl Failure Msg */
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
      /* Set Error Cause */
      rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return TVM_FAIL;
    }
    
    /* Check if Meas Validity PRESENT */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,measurementValidity))
    {
      prim_meas_validity = TVM_RET_MEAS_VALIDITY(
        rrc_mcm_ptr->measurementCommand.u.setup->u.
        trafficVolumeMeasurement->measurementValidity.ue_State);
    }
    
    if (rrctvm_check_if_meas_is_valid_for_current_state(prim_meas_validity) ==  FALSE)
    {
      WRRC_MSG0_HIGH("TVM: RRC val, MCM val differ");
      mac_mcm_params.meas_start_incl = FALSE;
    }

    if((prim_meas_validity == ALL_STATES) && (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.setup->u.
      trafficVolumeMeasurement,
      rrc_TrafficVolumeMeasurement,trafficVolumeMeasurementObjectList))))
    {
      WRRC_MSG0_ERROR("Meas validity == all states, but no tvm object");
      /* Send Meas Ctrl Failure Configuration Incomplete */
      
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf); 
      return TVM_FAIL;
    }
 
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_START;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    /* To read traffic volume reporting mode */
    mac_mcm_params.meas_report_mode.transfer_mode = 
      TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
      measurementReportingMode.measurementReportTransferMode);

    if(rrcmeas_process_mcm_addnl_meas
      (mcm_r9,transaction_identifier,&addtl_meas_validation,
       meas_type,mac_mcm_params.meas_id,&prim_meas_validity) == FAILURE)
    {
      return TVM_FAIL;
    }
    
    /* Set Measurement Identity in tvm_meas_id_list for SETUP */
    rrctvm_process_mcm_identity_r9(&mac_mcm_params.meas_id,mcm_r9);
    
    /* Set parameters in RRC DB */
    if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
    {
      rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */  
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      
      return TVM_FAIL;
      
    }
    /* Store additional meas */
    /* Check if Additional Meas has been validated successfully. */
    /* If yes, then store all additional meas in RRC DB */
    if (addtl_meas_validation == TRUE) 
    {
      rrcmeas_check_addtl_meas_in_db(&meas_type, mcm_r9,
        mac_mcm_params.meas_id);
      WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
    }

  } /* End of if of set up */
  else if (rrc_mcm_ptr->measurementCommand.t == T_rrc_MeasurementCommand_r9_modify)
  {
    /* Now set other values */
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_MODIFY;

    mac_mcm_params.config_conf_ind = TRUE;
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr,
        rrc_MeasurementControl_r9_IEs,measurementReportingMode))
    {
      /* To read traffic volume reporting mode */
      mac_mcm_params.meas_report_mode.transfer_mode = 
        TVM_RET_REPORTING_MODE(rrc_mcm_ptr->
        measurementReportingMode.measurementReportTransferMode);  
      
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
        rrc_MeasurementCommand_r9_modify,measurementType))
    {
      if (!(RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand.u.modify->measurementType,
        rrc_MeasurementType_r9_trafficVolumeMeasurement)))
      {
      /* If one comes here, then it means that meas id earlier determined
      * by RRC as traffic volume is actually not Traffic Volume. Send
        * Meas Ctrl failure Msg to UTRAN */
        
        WRRC_MSG0_ERROR("TVM:Meas Id is not Traffic Volume");
        /* Send Meas Ctrl Failure */
        
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
      
      /* Time to process the actual parameters */
      if (rrctvm_read_traffic_vol_params(rrc_mcm_ptr->measurementCommand.
        u.modify->measurementType.u.trafficVolumeMeasurement,
        &mac_mcm_params) == TVM_SUCCESS)
      {
        
        /* Check if Meas Validity PRESENT */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
          trafficVolumeMeasurement,
          rrc_TrafficVolumeMeasurement,measurementValidity))
        {
          prim_meas_validity = TVM_RET_MEAS_VALIDITY(
           rrc_mcm_ptr->measurementCommand.u.modify->measurementType.u.
            trafficVolumeMeasurement->measurementValidity.ue_State);
        }

        /* Process Additional Meas */
        if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_identifier,&addtl_meas_validation,meas_type,mac_mcm_params.meas_id,&prim_meas_validity) == FAILURE)
        {
            return TVM_FAIL;
        }
          

        /* Set Measurement Identity in tvm_meas_id_list for SETUP */
        rrctvm_process_mcm_identity_r9(&mac_mcm_params.meas_id,mcm_r9);
        
        /* Set parameters in RRC DB */
        if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
        {
          rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
          
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
          
          /* Send MCF msg to UTRAN */  
          rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
          
          
          return TVM_FAIL;
          
        }

        /* Store additional meas */
        /* Check if Additional Meas has been validated successfully. */
        /* If yes, then store all additional meas in RRC DB */
        if (addtl_meas_validation == TRUE) 
        {
          rrcmeas_check_addtl_meas_in_db(&meas_type, mcm_r9,
            mac_mcm_params.meas_id);
          WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
        }
        
        
      } /* End if if of Traffic Volume params successfully read */
      else
      {
        /* Send Meas Ctrl Failure Msg */
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
        /* Set Error Cause */
        rrcmeas_mcf.error_cause  = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return TVM_FAIL;
      }
      
    } /* End of If of meas_type present */
    else
    {
      /* Set Measurement Identity in tvm_meas_id_list for MODIFY */
      rrctvm_process_mcm_identity_r9(&mac_mcm_params.meas_id,mcm_r9);
        
        
      /* Process Additional Meas */
     if(rrcmeas_process_mcm_addnl_meas(mcm_r9,transaction_identifier,&addtl_meas_validation,meas_type,mac_mcm_params.meas_id,&prim_meas_validity) == FAILURE)
      {
        return TVM_FAIL;
      }
      
      
      if (rrctvm_config_tvm_db(&mac_mcm_params,&prim_meas_validity) == FALSE)
      {
        rrcmeas_mcf.rrc_transaction_id = transaction_identifier;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;

        /* Send MCF msg to UTRAN */  
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        
        return TVM_FAIL;
        
      }

      /* Store additional meas */
      /* Check if Additional Meas has been validated successfully. */
      /* If yes, then store all additional meas in RRC DB */
      if (addtl_meas_validation == TRUE) 
      {
        rrcmeas_check_addtl_meas_in_db(&meas_type, mcm_r9,
          mac_mcm_params.meas_id);
        WRRC_MSG0_HIGH("REL9:Additional Meas set in RRC DB");
      }
    }
    
    
  } /* End of else if of modify */
  else
  {
  /* Delete chosen. It has already been verified in rrcmeas_find_owner
    * that this meas exists. So let us delete it from tvm_meas_id_list */
    
    mac_mcm_params.meas_cmd = MAC_TRAFFIC_MEAS_DELETE;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&mac_mcm_params.meas_id);
    
    mac_mcm_params.config_ack_num = (uint16) transaction_identifier;
    
    mac_mcm_params.config_conf_ind = TRUE;
    
    for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
    {
      if (tvm_meas_id_list[count].meas_id == 
        mac_mcm_params.meas_id)
      {
        WRRC_MSG0_HIGH("TVM:Delete:Remove Meas Id");
        
        tvm_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        tvm_meas_id_list[count].block_rach_incl = FALSE;
        
        tvm_meas_id_list[count].meas_ctrl_msg = FALSE;
        
        tvm_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        tvm_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also remove any Addtl Meas associated with it */
        for (cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++) 
        {
          tvm_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        }
        
        break;  /* Come out of for */
      } /* End of if */
      
    }
    
    alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
    mac_mcm_params.config_ack_num = alloc_idx;
    rrctvm_send_mac_cmd(&mac_mcm_params);
    
    return TVM_SUCCESS;
    
  }
  
  /* Time to store meas validity for SETUP or Modify 
  It has already been verified that the Measurement exists */ 
  for ( count = 0; count < MAX_TRAFFIC_VOLUME_MEASUREMENTS; count++ )
  {
    if (tvm_meas_id_list[count].meas_id == 
      mac_mcm_params.meas_id)
    {
      tvm_meas_id_list[count].meas_ctrl_msg = TRUE;
      
      /* This means that meas would be saved on trans from DCH to FACH */
      /* Change ownership to Meas Ctrl Msg */      
      
      if (mac_mcm_params.meas_report_mode.report_mode == 
        EVENT_TRIGGERED_MODE) 
        
      {
        for (trch_cnt = 0; trch_cnt < mac_mcm_params.meas_report_criteria.event_cfg.num_trch; trch_cnt++)
        {
          for (cnt = 0; cnt < mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].num_events; cnt++)
          {
            if ((mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].event_id == EVENT_4A) &&
              (mac_mcm_params.meas_report_criteria.event_cfg.trch_event_cfg[trch_cnt].event_param[cnt].tx_interrupt_tmr != QUANTITY_NOT_PRESENT))
            {
              tvm_meas_id_list[count].block_rach_incl = TRUE;
              WRRC_MSG1_HIGH("Block RACH set Meas Id %d",tvm_meas_id_list[count].meas_id);
              
              block_set = TRUE;
              break; 
            } /* End of if of event_id and tx_interrupt_tmr check */
          } /* End of for */
          if (block_set == TRUE)
          {
            WRRC_MSG0_HIGH("Block RACH config finished");
            break;
          }
        } /* end of 2nd for */
        break;
      } /* End of if for Event Triggered Mode */
      break;
    } /* End of if */
    
  } /* End of for loop */

  alloc_idx = rrcmeas_get_int_trans_index_for_trans_id(transaction_identifier);
  mac_mcm_params.config_ack_num = alloc_idx;

  /* Now send config command to MAC */
  rrctvm_send_mac_cmd(&mac_mcm_params);
  return TVM_SUCCESS;
}
#endif

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif
