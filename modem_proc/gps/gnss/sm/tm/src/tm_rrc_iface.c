/*===========================================================================

                            TM_RRC_IFACE

DESCRIPTION


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_rrc_iface.c#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  04/03/15   mj     Support handling AS_ID for RRC events as part of W+W
  10/29/13   mj     Update WCDMA and TDSCDMA API calls for segment loading
  06/20/13    rk      RRC LSM messages are permanently allocated on heap. So the rrc call back functions
                      in this module is clean-up to elimiated old way of handling.
  05/10/13    rk      RRC LSM IPC payload takes a heap memory from WCDMA-RRC to GPS. The mem release is owned by GPS. 
  05/30/07    lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR. 
  05/30/07    lt      Initial check in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
 
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif
   

#include "tm_rrc_iface.h"
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) || defined ( FEATURE_WCDMA_GPS_CELL_ID_READING ) || defined (FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH)
#include "tm_umts_cp_wcdma_api.h"
#include "tm_umts_up_supl_api.h"
#endif
#include "err.h"
#include "event.h"
#include "tm_common.h"
#include "tm_data.h"
#include "tm_api.h"
#include "gnss_wwan_iface.h"

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) || defined ( FEATURE_WCDMA_GPS_CELL_ID_READING ) || defined (FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef union
{
  rrc_meas_params_struct_type *p_tm_rrc_meas_param;
} tm_rrc_event_data_union_type;
/* union for possibly expansion as needed */

typedef struct
{
  rrc_cgps_event_e_type          e_event_type;
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  sys_modem_as_id_e_type		 e_event_as_id;
  #endif
  tm_rrc_event_data_union_type   u;
} tm_rrc_event_s_type; 
/* this is used for passing data from RRC to TM through IPC */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
/*===========================================================================

FUNCTION
  tm_umts_cp_wcdma_ue_pos_capability_get

DESCRIPTION
  This function returns a structure containing TM_UMTS_CP_WCDMA current UE positioning 
  capabilities for RRC.

PARAMETERS
  out_capabilities_ptr - Pointer to structure that will be filled in with UE
  capabilities.

RETURN VALUE
  None

NOTE: lsm is used in the function name so that changes to RRC can be 
      minimized to accommodate the addition of CGPS
===========================================================================*/
void tm_rrc_iface_ue_pos_capability_get 
( tm_umts_cp_wcdma_ue_pos_capability_struct_type *out_capabilities_ptr )
{

  /* Give caller a copy of the capabilities structure */
  *out_capabilities_ptr = tm_umts_cp_wcdma_ue_pos_capability_get();

}


/*===========================================================================

FUNCTION tm_rrc_iface_send_event

DESCRIPTION
  This function is used by RRC to send events and/or data to CGPS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_rrc_iface_send_event( rrc_cgps_event_data_s_type*  rrc_event_data_ptr )
{
  os_IpcMsgType               *ipc_msg_ptr = NULL;
  tm_rrc_event_s_type *msg_to_tm_ptr = NULL;
  rrc_meas_params_struct_type *rrc_meas_param_ptr = NULL;

  if (NULL == rrc_event_data_ptr)
  {
    MSG_ERROR( "Null ptr sent to tm_rrc_iface_send_event", 0, 0, 0);
    return FALSE;
  }

  rrc_meas_param_ptr = (rrc_meas_params_struct_type*)rrc_event_data_ptr->u.z_rrc_cgps_msg_data.p_data;

  if ( rrc_event_data_ptr->e_event_type != RRC_CGPS_RRC_MSG_EVENT )
  {
    MSG_HIGH( "Unsupported event type: %u", 
              rrc_event_data_ptr->e_event_type, 0, 0 );
    return FALSE;
  }
  if ( rrc_meas_param_ptr == NULL )
  {
    MSG_HIGH( "rrc_meas_param_ptr is NULL", 0, 0, 0);
    return FALSE;
  }

  if ( ( rrc_meas_param_ptr->params_type != RRC_ASSISTANCE_DATA_PARMS ) &&
       ( rrc_meas_param_ptr->params_type != RRC_MEAS_CTRL_PARMS ) &&
       ( rrc_meas_param_ptr->params_type != RRC_STATE_CHANGE_PARMS )
     )  
  {
    MSG_HIGH( "meas_param_type not supported %d", 
              (uint8)rrc_meas_param_ptr->params_type, 0, 0);
    return FALSE;
  }


  ipc_msg_ptr = os_IpcCreate( sizeof(tm_rrc_event_s_type), 
                              IPC_ALLOCATION_DYNAMIC,
                              (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "IPC message allocation failed",0,0,0 );
    return FALSE;
  }

  msg_to_tm_ptr = ( tm_rrc_event_s_type *)ipc_msg_ptr->p_Data;
  msg_to_tm_ptr->e_event_type = rrc_event_data_ptr->e_event_type;
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  msg_to_tm_ptr->e_event_as_id = rrc_event_data_ptr->as_id;
  #endif  

  /* Use rrc measurement struct allocated on heap by WCDMA-RRC module. GPS-engine will release after sub-protocol module
  processed the rrc message */
  msg_to_tm_ptr->u.p_tm_rrc_meas_param = rrc_meas_param_ptr;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC Meas struct ptr %p. %p %lu", rrc_meas_param_ptr,
        rrc_event_data_ptr->u.z_rrc_cgps_msg_data.p_data,
        rrc_event_data_ptr->u.z_rrc_cgps_msg_data.length); 
 

  ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_RRC;
  ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
  ipc_msg_ptr->q_Size           = sizeof(tm_rrc_event_s_type);

  #ifdef FEATURE_CGPS_DUAL_WCDMA
  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sending RRC LSM meas message of %lu bytes to TM thread for as_id %d", ipc_msg_ptr->q_Size,(uint8)rrc_event_data_ptr->as_id);
  #else
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sending RRC LSM meas message of %lu bytes to TM thread", ipc_msg_ptr->q_Size);
  #endif

  if ( os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM) == FALSE )
  {
    MSG_ERROR("Error in sending RRC msg to TM thread",0,0,0);
    (void)os_IpcDelete(ipc_msg_ptr);
    ipc_msg_ptr = NULL;
  } /* End status */

  return TRUE;
}

#endif /* FEATURE_CGPS_UMTS_CP_WCDMA && ! FEATURE_GNSS_LOW_MEMORY*/

/*===========================================================================

FUNCTION tm_rrc_iface_proc_event

DESCRIPTION
  This function is used by TM to process an RRC Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rrc_iface_proc_event( void *p_msg )
{
  tm_rrc_event_s_type              *msg_from_rrc_ptr; 
  prtl_rrc_event_s_type            z_tm_to_prtl_rrc_event; 
  tm_prtl_rrc_event_data_s_type    z_rrc_event_data;
  
  if( p_msg == NULL )
  {
    MSG_ERROR( "TM_RRC_IFACE: Invalid NULL pointer in proc_event", 0, 0, 0 );
    return;
  }

  msg_from_rrc_ptr = ( tm_rrc_event_s_type *)p_msg;
  z_tm_to_prtl_rrc_event.rrc_event_data_ptr = &z_rrc_event_data;
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  z_rrc_event_data.e_event_type = msg_from_rrc_ptr->e_event_type;
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  z_rrc_event_data.e_event_as_id = msg_from_rrc_ptr->e_event_as_id;
  #endif
  #endif /*  */
  z_rrc_event_data.u.rrc_meas_param_ptr = msg_from_rrc_ptr->u.p_tm_rrc_meas_param;
 
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED , "TM_RRC_IFACE: tm_rrc_iface_proc_event called for as_id %u", (uint8)z_rrc_event_data.e_event_as_id);  
  #else
  MSG_MED( "TM_RRC_IFACE: tm_rrc_iface_proc_event called", 0, 0, 0 );
  #endif
  
  tm_core_rrc_notify_event( &z_tm_to_prtl_rrc_event );

  /* RRC measurement struct is processed by sub-protocol modules, so release the memory */
  if ( z_rrc_event_data.u.rrc_meas_param_ptr != NULL )
  {
    os_MemFree((void**)&z_rrc_event_data.u.rrc_meas_param_ptr);
    msg_from_rrc_ptr->u.p_tm_rrc_meas_param = NULL;
  }
}



/*===========================================================================

FUNCTION tm_rrc_iface_init

DESCRIPTION
  This function initializes the TM-RRC IFACE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rrc_iface_init( void )
{
  #if defined (FEATURE_CGPS_UMTS_CP_WCDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
    /* Register an event handler with RRC */
	#if defined (FEATURE_DUAL_WCDMA) && defined (FEATURE_CGPS_DUAL_WCDMA)
    /* W+W Dual target. Register callbacks for both the subs */
    if ((gnss_wwan_iface_rrcgps_register_cgps_event_cb_sub(tm_rrc_iface_send_event, SYS_MODEM_AS_ID_1) == FALSE) ||
		(gnss_wwan_iface_rrcgps_register_cgps_event_cb_sub(tm_rrc_iface_send_event, SYS_MODEM_AS_ID_2) == FALSE))
    {
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_rrc_iface: event_cb reg. failed for DUAL WCDMA (W+W)");
    }
    #else
	/* Fall back to legacy method of registering callback with primary sub (sub0)*/
	#ifdef FEATURE_CGPS_DUAL_WCDMA
    if(gnss_wwan_iface_rrcgps_register_cgps_event_cb_sub(tm_rrc_iface_send_event, SYS_MODEM_AS_ID_1) == FALSE)
	#else
	if ( gnss_wwan_iface_rrcgps_register_cgps_event_cb( tm_rrc_iface_send_event ) == FALSE )
	#endif
    {
      MSG_ERROR("tm_rrc_iface: event_cb reg. failed", 0, 0, 0);
    }
    #endif /*FEATURE_DUAL_WCDMA && FEATURE_CGPS_DUAL_WCDMA*/
    else /* register another call back if the previous one succeeds. */
    {
      #if defined (FEATURE_DUAL_WCDMA) && defined (FEATURE_CGPS_DUAL_WCDMA)
	  /* W+W Dual target. Register callbacks for both the subs */
	  if ((gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb_sub(tm_rrc_iface_ue_pos_capability_get, SYS_MODEM_AS_ID_1) == FALSE) ||
	  	  (gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb_sub(tm_rrc_iface_ue_pos_capability_get, SYS_MODEM_AS_ID_2) == FALSE))
	  {
		MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_rrc_iface: ue_cap. reg. failed for DUAL WCDMA (W+W)");
	  }
	  #else
	  /* Fall back to legacy method of registering the capabilities with primary sub (sub0)*/
	  #ifdef FEATURE_CGPS_DUAL_WCDMA
	  if (gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb_sub(tm_rrc_iface_ue_pos_capability_get, SYS_MODEM_AS_ID_1) == FALSE)
	  #else
	  if ( gnss_wwan_iface_rrcgps_register_cgps_ue_pos_capability_cb ( tm_rrc_iface_ue_pos_capability_get ) == FALSE )
	  #endif
	  {
	    MSG_ERROR("tm_rrc_iface: ue_cap. reg. failed", 0, 0, 0);
	  }
	  #endif /*FEATURE_DUAL_WCDMA && FEATURE_CGPS_DUAL_WCDMA*/    
    }
  #endif /* (FEATURE_CGPS_UMTS_CP_WCDMA) && ! (FEATURE_GNSS_LOW_MEMORY) */

  #ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
  (void)rrc_register_cell_id_cnf_cb(tm_rrc_iface_cell_info_dch_report_callback);
  #endif
}

#endif  /* FEATURE_CGPS_UMTS_CP_WCDMA || FEATURE_WCDMA_GPS_CELL_ID_READING */

#ifdef FEATURE_CGPS_WCDMA
/*===========================================================================

FUNCTION
  tm_rrc_iface_cell_info_dch_report

DESCRIPTION
  This function receives RRC response to cell id request sent by TM.

   Parameters:
    Cell ID information: 
    rrcmeas_plmn_cell_info_struct_type cell_id_info
  Status: (RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)  
    rrc_cell_id_status_type            status 

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void tm_rrc_iface_cell_info_dch_report(
    rrcmeas_plmn_cell_info_struct_type cell_id_info,
    rrc_cell_id_status_type            status )
{

#if (defined(FEATURE_CGPS_UMTS_UP) && defined(FEATURE_WCDMA))
  tm_umts_supl_api_rrc_cell_info_proc(cell_id_info, status);
#endif /*FEATURE_CGPS_UMTS_UP && FEATURE_WCDMA*/
}

#endif /*FEATURE_CGPS_WCDMA*/

