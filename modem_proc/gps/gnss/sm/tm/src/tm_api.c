/*===========================================================================

                            TM_API

DESCRIPTION: TM's APIs.


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_api.c#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  05/01/15    rh      Added get gps lock TM API
   10/27/14   rk      Added LPP RSTD >24 measurement processing support
01/31/13    mj     Modify tm_api_reg_task_init_notifier to handle race condition
01/12/13   gk      LPP ECID changes
04/18/12    ssu    Added tm_api_update_best_avail_pos() 
03/01/12    rh     Added tm_api_send_engine_error_recovery_report()
06/10/09   gk      Added Get UTC offset from NV
1/12/09     atien   XTRA-T support
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
4/25/08     GK      On-Demand Changes.
7/10/07     LT      Initial check-in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "aries_os_api.h"
#include "gps_common.h"
#include "tm_common.h"
#include "tm_api.h"
#include "tm_data.h"
#include "tm_xtra.h"


#include "tm_xspi.h"

 
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif
   


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

uint8 tm_map_client_threadid_to_index(os_ThreadIdEnumType clientThreadId)
{
  switch (clientThreadId)
  {
    case THREAD_ID_LOC_MIDDLEWARE:
		return (uint8)TM_LOC_MW_CLIENT_INDEX;
	
	case THREAD_ID_SDP:
		return (uint8)TM_SDP_CLIENT_INDEX;

	case THREAD_ID_LBS:
		return (uint8)TM_LBS_CLIENT_INDEX;
		
	default:
		return (uint8)TM_UNSUPPORTED_CLIENT_INDEX;		
  }

}  

  
  
		


/*******************************************************************************
 * tm_api_store_ftcal_tt_data
 *
 * Function description:
 *
 * Parameters: 
 *
 *  
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_tt_data (cgps_FtcalTTStruct* p_TTData)
{
  os_IpcMsgType                  *ipc_msg_ptr;
  cgps_FtcalTTStruct             *ipc_data_ptr;
 
  MSG_HIGH("TM_API: Send TT data to TM",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(cgps_FtcalTTStruct), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (cgps_FtcalTTStruct *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_FTCAL_TT_DATA;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(cgps_FtcalTTStruct);
      
    *ipc_data_ptr = *p_TTData;  

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return;
  } /* End ipc_msg_ptr */

}

/*******************************************************************************
 * tm_api_store_ftcal_ppm_data
 *
 * Function description:
 *
 * Parameters: 
 *
 *  
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_ppm_data (cgps_FtcalPpmStruct* p_PpmData)
{
  os_IpcMsgType                  *ipc_msg_ptr;
  cgps_FtcalPpmStruct             *ipc_data_ptr;
 
  MSG_HIGH("TM_API: Send PPM data to TM",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(cgps_FtcalPpmStruct), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (cgps_FtcalPpmStruct *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_FTCAL_PPM_DATA;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(cgps_FtcalPpmStruct);
      
    *ipc_data_ptr = *p_PpmData;  

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return;
  } /* End ipc_msg_ptr */

}


/*===========================================================================
FUNCTION tm_api_update_cell_info

DESCRIPTION: for entities outside TM to provide TM with Cell info update.

RETURN VALUE

DEPENDENCIES
=============================================================================*/

void tm_api_update_cell_info ( const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct * const p_CellInfoCached )
{
  os_IpcMsgType                  *ipc_msg_ptr;
  tm_cell_info_update_type       *ipc_data_ptr;

  MSG_HIGH("TM_API: Cached Cell ID Info updated",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(tm_cell_info_update_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_cell_info_update_type *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_MC_CELLDB_UPDATE;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_cell_info_update_type);

    memscpy((void*)&(ipc_data_ptr->cellInfoCached), sizeof(ipc_data_ptr->cellInfoCached), (void*)p_CellInfoCached, sizeof(*p_CellInfoCached));
    ipc_data_ptr->e_TriggerRAT = e_TriggerRAT;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0);
    return;
  } /* End ipc_msg_ptr */

}


/*===========================================================================
FUNCTION tm_api_update_gps_state_info

DESCRIPTION: for entities outside TM to provide TM with position and its
associated state info update.

RETURN VALUE

DEPENDENCIES
=============================================================================*/

void tm_api_update_gps_state_info ( const sm_PositionReportStructType* const p_PosInfo )
{

  os_IpcMsgType                  *ipc_msg_ptr;
  sm_PositionReportStructType    *ipc_data_ptr;

  MSG_HIGH("TM_API: POS PUNC TUNC updated",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(sm_PositionReportStructType), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (sm_PositionReportStructType*)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(sm_PositionReportStructType);

    *ipc_data_ptr = *p_PosInfo;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0);
    return;
  } /* End ipc_msg_ptr */

}

/*******************************************************************************
 * tm_api_report_nv_utc_offset
 *
 * Function description:
 *
 * Parameters:
 *
 *  p_UtcOffset - Pointer to NV UTC offset information
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/
void tm_api_report_nv_utc_offset(uint8 *p_UtcOffset)
{
  tm_core_get_nv_utc_offset(p_UtcOffset);
}


/**
@brief dispatches an asynchronous request for TM to raise the external spi
       injection event and request SPI injection
@param
@return
*/
void tm_api_xspi_start_injection(void)
{
   /* send TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST ipc message to tm */
   os_IpcMsgType  *p_ipcMsg = NULL;
   tm_xspi_request_type* p_reqData = NULL;

   p_ipcMsg = os_IpcCreate(sizeof(tm_xspi_request_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
   if(p_ipcMsg)
   {
      p_reqData = (tm_xspi_request_type*)p_ipcMsg->p_Data;
      p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST;
      p_reqData->q_stream_status = TM_XSPI_STREAM_STATUS_ENABLE;

      if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
      {
         MSG_ERROR("TM_API: Failed to send IPC message",0,0,0);
         (void)os_IpcDelete(p_ipcMsg);
      }
   }
   else
   {
      MSG_ERROR("TM_API: Failed to create IPC message",0,0,0);
   }
   return;
}

/**
@brief dispatches an asynchronous request for TM to raise the external spi
       injection event and stop SPI injection
@param
@return
*/
void tm_api_xspi_stop_injection(void)
{
  /* send TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST ipc message to tm */
  os_IpcMsgType  *p_ipcMsg = NULL;
  tm_xspi_request_type* p_reqData = NULL;

  p_ipcMsg = os_IpcCreate(sizeof(tm_xspi_request_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if(p_ipcMsg)
  {
     p_reqData = (tm_xspi_request_type*)p_ipcMsg->p_Data;
     p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST;
     p_reqData->q_stream_status = TM_XSPI_STREAM_STATUS_DISABLE;

    if (!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("TM_API: Failed to send IPC message",0,0,0);
      (void)os_IpcDelete(p_ipcMsg);
    }
  }
  else
  {
    MSG_ERROR("TM_API: Failed to create IPC message",0,0,0);
  }
}

/*===========================================================================
  tm_loc_lte_otoda_recv_measurements

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_loc_lte_otdoa_recv_measurements(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info )
{
  
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  sm_loc_lte_otdoa_meas_info_type *ipc_data_ptr;

  ipc_msg_ptr = os_IpcCreate(sizeof(sm_loc_lte_otdoa_meas_info_type), 
                            IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_MGP_MC);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (sm_loc_lte_otdoa_meas_info_type*)ipc_msg_ptr->p_Data;
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_SM_LOC_LTE_OTDOA_MEASUREMENTS;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(sm_loc_lte_otdoa_meas_info_type);
    /* Internal p_meas_info pointer storage is persistant until the SM LPP CP session ends */
    *ipc_data_ptr = *p_otdoa_measurement_info;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR( "TM_API: Failed to send IPC message (OTDOA Measurements)",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    else
    {
      MSG_HIGH( "TM_API: successfully sent an IPC message (OTDOA measurements)",0,0,0);
      return TRUE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR( "TM_API: Failed to create IPC message (tm_api_xtm_nv_status_report)",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}



/*******************************************************************************
 * tm_api_XTM_NV_status_report
 *
 * Function description:
 *  Called by XTRAT module to report the success of NV write to TM thread.
 *
 * Parameters:
 * which nv item has been set
 *
 * Return value:
 * none
 *
 *******************************************************************************/

boolean tm_api_xtm_nv_status_report(tm_xtm_struct_nv_write_status* xtm_nv_set )
{
  os_IpcMsgType                  *ipc_msg_ptr;
  tm_xtm_struct_nv_write_status   *ipc_data_ptr;

  ipc_msg_ptr = os_IpcCreate(sizeof(tm_xtm_struct_nv_write_status), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_xtm_struct_nv_write_status*)ipc_msg_ptr->p_Data;
    switch (xtm_nv_set->xtm_nv_item_set)
    {
      case TM_XTM_SET_CLIENT_TOKEN:
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_XTM_CLIENT_TOKEN;
        break;

      case TM_XTM_SESSION_CONTROL:
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_XTM_SESSION_CONTROL;
        break;
    }

    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_xtm_struct_nv_write_status);
    *ipc_data_ptr = *xtm_nv_set;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR( "TM_API: Failed to send IPC message (tm_api_xtm_nv_status_report)",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    else
    {
      MSG_HIGH( "TM_API: successfully sent an IPC message (tm_api_xtm_nv_status_report)",0,0,0);
      return TRUE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR( "TM_API: Failed to create IPC message (tm_api_xtm_nv_status_report)",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}

/*=============================================================================

FUNCTION
  tm_api_reg_task_init_notifier

DESCRIPTION
  Called by potential clients to register their callback. The callback is
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called
  inline.  The caller should be ready to handle this and queue an IPC to itself.

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered
  clientThreadId - Client Thread ID to identify the index within the array

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_reg_task_init_notifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId)
{
  os_IpcMsgType             *ipc_msg_ptr = NULL;
  tm_api_notifier_type	    *ipc_data_ptr = NULL;
  uint8 errCode = 0;
  boolean ipc_Status = FALSE;
  uint8 index;

  if (pNotifierCb)
  {
	index = tm_map_client_threadid_to_index(clientThreadId);
  
    if (index < TM_MAX_NOTIFIERS)
    {
	  if ( FALSE == pdsm_get_ready_flag() )
      { /* TM not ready to call the callback, so save the callbacks to call later when TM is up */
        pz_SmNotifierCb[index] = pNotifierCb;
      }	
      else 
      {
        /* TM is ready, so send an IPC to TM with index and callback */
	    ipc_msg_ptr = os_IpcCreate(sizeof(tm_api_notifier_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
		
	    if (ipc_msg_ptr)
	    {
	      ipc_data_ptr = (tm_api_notifier_type*)ipc_msg_ptr->p_Data; 
		
	      ipc_data_ptr->pNotifierCb = pNotifierCb;
	      ipc_data_ptr->clientIndex = index;
	      ipc_data_ptr->broadcast = FALSE;
			
	      ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_API_REGISTRATION;
	      ipc_Status  = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
	      if (ipc_Status == FALSE)
	      {
			ipc_Status = os_IpcDelete(ipc_msg_ptr);
		    ipc_msg_ptr = NULL;
			return FALSE;
	      } 
	    }
		else
		{ /* ipc_msg_ptr == NULL */
		  errCode = 1;
		}  
	  }
    }
	else
	{ /* (index >= TM_MAX_NOTIFIERS) */
	  errCode = 2;
	}
  }
  else
  { /* pNotifierCb == NULL */
    errCode = 3;
  }	

  if (errCode > 0)
  {
    MSG_ERROR("tm_api_reg_task_init_notifier failed with error code = %d", errCode, 0, 0);
	return FALSE;
  }
  else
  {
  	return TRUE;
  }
}

/**
@brief Calls the tm_xtra function for indicating xtra injection 
       has completed
@param None
@return None
*/
void tm_api_xtra_inj_done(void)
{
  tm_xtra_inj_done();
}


/*******************************************************************************
 * tm_api_report_wbiq_info
 *
 * Function description:
 *  Called by LM to update TM with the latest WBIQ FFT info
 *
 * Parameters: 
 *
 *  p_ReportWBIQInfo - WBIQ Info
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_wbiq_info (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo)
{
  os_IpcMsgType                  *ipc_msg_ptr;
  sm_ReportWBIQInfoStructType    *ipc_data_ptr;

  MSG_HIGH("TM_API: Report WBIQ Info",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(sm_ReportWBIQInfoStructType), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (sm_ReportWBIQInfoStructType*)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MGP_WBIQ_INFO;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(sm_ReportWBIQInfoStructType);

    /* Copy all the data over */
    ipc_data_ptr->u_GnssConfig = p_ReportWBIQInfo->u_GnssConfig;
    ipc_data_ptr->f_GpsCNoDBHz = p_ReportWBIQInfo->f_GpsCNoDBHz;
    ipc_data_ptr->f_GpsFreqHz = p_ReportWBIQInfo->f_GpsFreqHz; 
    ipc_data_ptr->f_GloCNoDBHz = p_ReportWBIQInfo->f_GloCNoDBHz;
    ipc_data_ptr->f_GloFreqHz = p_ReportWBIQInfo->f_GloFreqHz; 
    ipc_data_ptr->f_BdsCNoDBHz = p_ReportWBIQInfo->f_BdsCNoDBHz;
    ipc_data_ptr->f_BdsFreqHz = p_ReportWBIQInfo->f_BdsFreqHz; 
    ipc_data_ptr->f_GalCNoDBHz = p_ReportWBIQInfo->f_GalCNoDBHz;
    ipc_data_ptr->f_GalFreqHz = p_ReportWBIQInfo->f_GalFreqHz; 
    ipc_data_ptr->u_GnssConfig = p_ReportWBIQInfo->u_GnssConfig;
    ipc_data_ptr->l_AdcMeanIp1mV = p_ReportWBIQInfo->l_AdcMeanIp1mV;
    ipc_data_ptr->l_AdcMeanQp1mV = p_ReportWBIQInfo->l_AdcMeanQp1mV;
    ipc_data_ptr->q_AdcAmpIp1mV = p_ReportWBIQInfo->q_AdcAmpIp1mV;
    ipc_data_ptr->q_AdcAmpQp1mV = p_ReportWBIQInfo->q_AdcAmpQp1mV;
    ipc_data_ptr->u_AdcIqSatPerStatus = p_ReportWBIQInfo->u_AdcIqSatPerStatus;
    ipc_data_ptr->q_ISatPer = p_ReportWBIQInfo->q_ISatPer;
    ipc_data_ptr->q_QSatPer = p_ReportWBIQInfo->q_QSatPer;
    ipc_data_ptr->l_AdcIqWbJammerPwrEstDb = p_ReportWBIQInfo->l_AdcIqWbJammerPwrEstDb;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return;
  } /* End ipc_msg_ptr */

}

#ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
/*******************************************************************************
*   tm_rrc_iface_cell_info_dch_report_callback
* 
* Function description:
*   This function is registered as a callback function with RRC module and
*   called whenever RRC responds to cell id request sent by TM.
* 
* Parameters:
*   Cell ID information: 
*      rrcmeas_plmn_cell_info_struct_type cell_id_info
*   Status: (RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)  
*      rrc_cell_id_status_type            status 
* 
* RETURN VALUE
*   None 
*
 *******************************************************************************/

void tm_rrc_iface_cell_info_dch_report_callback(
    rrcmeas_plmn_cell_info_struct_type cell_id_info,
    rrc_cell_id_status_type            status )
{
  /*Pass the cell information from RRC to modules to TM CORE*/
  os_IpcMsgType                       *ipc_msg_ptr;
  tm_rrc_cell_info_dch_report_type    *ipc_data_ptr;

  MSG_HIGH("TM_API: tm_rrc_iface_cell_info_dch_report_callback",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(tm_rrc_cell_info_dch_report_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_rrc_cell_info_dch_report_type*)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_RRC_CELL_INFO_DCH_REPORT;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_rrc_cell_info_dch_report_type);

    memscpy((void*)&(ipc_data_ptr->cell_id_info), sizeof(ipc_data_ptr->cell_id_info), (void*)&(cell_id_info), sizeof(cell_id_info));
    ipc_data_ptr->status = status;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return;
  } /* End ipc_msg_ptr */  
}
#endif /*FEATURE_WCDMA_GPS_CELL_ID_READING  */

/*=============================================================================

FUNCTION
  tm_api_reset_location_service_done

DESCRIPTION
  Indication from MGP that location service reset id completed.

DEPENDENCIES
  None.

PARAMETERS
 status: Status of reset location service.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_reset_location_service_done(pdsm_pd_ext_reset_location_service_status_e_type status)
{
  /*Pass the reset done indication to TM CORE*/
  os_IpcMsgType                       *ipc_msg_ptr;

  MSG_HIGH("TM_API: Report Reset Location Done",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_pd_ext_reset_location_service_status_e_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_RESET_LOCATION_SERVICE_DONE;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(pdsm_pd_ext_reset_location_service_status_e_type);

    memscpy( ipc_msg_ptr->p_Data, sizeof(status), (void*)&status, sizeof(status));

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return;
  } /* End ipc_msg_ptr */  
}

/*******************************************************************************
 * tm_api_ftcal_request
 *
 * Function description:
 *  Called by LM to update TM that FTCal is enabled
 *
 * Parameters: 
 *
 *  b_Enable - TRUE to enable FTCal
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_ftcal_request (boolean b_Enable)
{
  os_IpcMsgType                  *ipc_msg_ptr;

  /* Create a message with no payload */
  ipc_msg_ptr = os_IpcCreate(0, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    if (b_Enable == TRUE)
    {
      /* Send Enable F-TCal message */
      MSG_HIGH("TM_API: Enable F-TCal",0,0,0);
      ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_FTCAL_ENABLE;
    }
    else
    {
      /* Send Disable F-TCal message */    
      MSG_HIGH("TM_API: Disable F-TCal",0,0,0);
      ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_FTCAL_DISABLE;    
    }

    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
  } /* End ipc_msg_ptr */
}
/*=============================================================================

FUNCTION
  tm_api_send_engine_error_recovery_report

DESCRIPTION
  TM API to send engine error recovery report to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/

void tm_api_send_engine_error_recovery_report( gnss_engine_err_recovery_rpt *pErrReport )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;

  MSG_LOW("tm_api_send_engine_error_recovery_report",0,0,0); 

  ipc_msg_ptr = os_IpcCreate(sizeof(gnss_engine_err_recovery_rpt), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT;

    memscpy((void*)(ipc_msg_ptr->p_Data), sizeof(*pErrReport), (void*)(pErrReport), sizeof(*pErrReport));

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
    }
  }
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
  }
}


/*=============================================================================

FUNCTION
  tm_api_send_dpo_status_report

DESCRIPTION
  TM API to send engine error recovery report to SM .

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/

void tm_api_send_dpo_status_report( sm_DpoStatusReportStructType *p_DpoStatusReport )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("tm_api_send_dpo_status_report",0,0,0); 

  msg_id = (uint32)TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT;
  size   = sizeof(sm_DpoStatusReportStructType);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size, 
    (uint32)THREAD_ID_SM_TM, (void *)p_DpoStatusReport);

  if(ipc_msg_ptr == NULL)
  {
    MSG_ERROR("Failed to send error report IPC mesage",0,0,0);     
  }
}

/*
******************************************************************************
* tm_read_qmi_services_configuration
*
* Function description:
*
*  This function is called by QMI services to read which QMI services are 
*  enabled. This function can be called only after SM has been initialized. 
*
* Parameters:
*  output_cfg : QMI Configuration read from NV
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_read_qmi_services_configuration( pdsm_QMIServiceConfig* output_cfg)
{
  if( NULL == output_cfg )
  {
    MSG_ERROR("Null pointer passed!!", 0, 0, 0);
    return;
  }

  memscpy( output_cfg, sizeof(*output_cfg), &tm_core_info.config_info.qmi_cfg, sizeof(tm_core_info.config_info.qmi_cfg));
  return;
}


/*===========================================================================
  tm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements

  Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_loc_lte_ecid_recv_measurements(gnss_lpp_ecid_meas_info_type *p_otdoa_measurement_info )
{
  
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  gnss_lpp_ecid_meas_info_type *ipc_data_ptr;

  ipc_msg_ptr = os_IpcCreate(sizeof(gnss_lpp_ECID_ProvideLocationInformation), 
                            IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_MGP_MC);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (gnss_lpp_ecid_meas_info_type*)ipc_msg_ptr->p_Data;
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_GNSS_LPP_ECID_MEASUREMENTS;

    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(gnss_lpp_ECID_ProvideLocationInformation);
    *ipc_data_ptr = *p_otdoa_measurement_info;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR( "TM_API: Failed to send IPC message (ECID Measurements)",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    else
    {
      MSG_ERROR( "TM_API: successfully sent an IPC message (ECID measurements)",0,0,0);
      return TRUE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR( "TM_API: Failed to create IPC message (tm_api_xtm_nv_status_report)",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}

/*
******************************************************************************
* tm_retrieve_IMSI
*
* Function description:
*
*  This function is called by XTM to request TM core to retrieve the IMSI
*
* Parameters:
*  NONE
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_retrieve_IMSI( uint8 sub, tm_RetrieveIMSICb p_RetrieveImsiInfoCb )
{
  os_IpcMsgType  *p_ipcMsg = NULL;
  tm_RetrieveIMSIType z_retrieveImsiInfo;
    
  MSG_LOW("tm_retrieve_IMSI()", 0,0,0);

  z_retrieveImsiInfo.sub = sub;
  z_retrieveImsiInfo.pz_RetrieveIMSICb = p_RetrieveImsiInfoCb;
  
  p_ipcMsg = os_IpcCreate(sizeof(z_retrieveImsiInfo), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if(p_ipcMsg)
  { 
     p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_RETRIEVE_IMSI; 
     memscpy((void*)(p_ipcMsg->p_Data), sizeof(z_retrieveImsiInfo), (void*)(&z_retrieveImsiInfo), sizeof(z_retrieveImsiInfo));
  
     if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
     {
        MSG_ERROR("TM_API: Failed to send IPC message",0,0,0);
        (void)os_IpcDelete(p_ipcMsg);
     }
  }
} 

/*===========================================================================
  tm_core_handle_wifi_fix_response

  Description:

     This function is used by SM to provide Wifi fix response to TM
 Parameters:
   sm_GnssFixRptStructType: Pointer to Position Report
                                        Note that this may be non Wifi position 
                                        if there was a better
                                        position available in ALE.
    wifi_ResultFlagsType: Wifi Results Flag
          The result flag indicates any error during Wifi pos fix calculation
          like no modem cache, no measurements, etc. It also indicates 
          whether a better non Wifi fix was sent instead. 
    sm_WifiPosReqSrcType: Wifi Position fix source (originiator)
  Return value: 
   VOID
=============================================================================*/
void tm_handle_wifi_fix_response(const sm_GnssFixRptStructType* pz_PosFixRpt,
                                 wl_ResultFlagsType z_wifi_result,
                                 wl_PosReqSrcType e_wifi_pos_src)
{
  os_IpcMsgType  *p_ipcMsg = NULL;
  tm_wifi_fix_response_type *p_hdr_ptr = NULL;
    
  MSG_LOW("tm_handle_wifi_fix_response()", 0,0,0);

  if(NULL == pz_PosFixRpt)
  {
    MSG_ERROR("Null pointer in handling Wifi fix response",0,0,0);
    return;
  }
  
  p_ipcMsg = os_IpcCreate(sizeof(*p_hdr_ptr), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  
  if(p_ipcMsg)
  { 
    p_hdr_ptr = (tm_wifi_fix_response_type*) p_ipcMsg->p_Data; /*lint !e826*/
    
    p_ipcMsg->q_MsgId          = (uint32)TM_CORE_MSG_ID_WIFI_FIX_RESP;
    p_ipcMsg->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    p_ipcMsg->q_Size           = sizeof(tm_wifi_fix_response_type);
    
    p_hdr_ptr->z_PosFixRpt = (*pz_PosFixRpt);
    p_hdr_ptr->z_wifi_result = z_wifi_result;
    p_hdr_ptr->e_wifi_pos_src = e_wifi_pos_src;
    
    if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("TM_API: Failed to send IPC message",0,0,0);
      (void)os_IpcDelete(p_ipcMsg);
    }
  }

}

/*===========================================================================

FUNCTION tm_api_get_gps_lock_status()

DESCRIPTION
  TM module can call this function to get current GPS lock status
 
  This is a synchronous API, will return immediately
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
pdsm_gps_lock_e_type tm_api_get_gps_lock_status(void)
{
  return TM_CORE_CONFIG_DD(gps_lock);
}

