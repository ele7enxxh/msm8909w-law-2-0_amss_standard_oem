/*===========================================================================

                            TM_L1_IFACE

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_l1_iface.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ------------------------------------------------------
  06/09/15    mj     Add support for W+W feature
04/22/14      ssu     API to request WCDMA cell measurements.
  07/17/07    lt      Initial check-in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#ifdef FEATURE_USER_PLANE_MRL 

#include "l1lsmif.h"
#include "tm_common.h"
#include "tm_data.h"
#include "gnss_wwan_iface.h"

#ifdef FEATURE_CGPS_LTE_CELLDB   
  #include "lte_ml1_ext_api.h"
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef union 
{
  wl1_lsm_cell_meas_update_type      z_tm_l1_cell_meas_update;
  #ifdef FEATURE_CGPS_LTE_CELLDB
  lte_ml1_gps_mrl_cell_meas_update_t z_tm_lte_l1_cell_meas_update;
  #endif 
} tm_l1_event_data_union_type;
/* union for possibly expansion as needed */

typedef struct
{
  l1_cgps_event_e_type          e_event_type;
  tm_l1_event_data_union_type   u;
} tm_l1_event_s_type; 
/* this is used for passing data from WCDMA L1 to TM through IPC */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION
  tm_wcdma_cell_meas_update

DESCRIPTION
  This function sends a cell-measurement update message (MRL) to TM

   Parameters:
   - lsm_cell_meas_info, Pointer to a structure containing cell measurements

DEPENDENCIES
   None.

RETURN VALUE
   - TRUE => Succesful, FALSE => Failure

SIDE EFFECTS
   None.
===========================================================================*/
static boolean l1_wcdma_cell_meas_update( l1_cgps_event_data_s_type *p_l1_event_data )
{

  os_IpcMsgType                   *ipc_msg_ptr = NULL;
  tm_l1_event_s_type              *msg_to_tm_ptr = NULL;
  wl1_lsm_cell_meas_update_type   *l1_cell_meas_update_ptr = NULL;

  l1_cell_meas_update_ptr = 
    (wl1_lsm_cell_meas_update_type *)p_l1_event_data->z_l1_cgps_msg_data.p_data;

  if ( p_l1_event_data->e_event_type != L1_CGPS_EVENT_CELL_MEAS_UPDATE )
  {
    MSG_HIGH( "Unsupported event type: %u", 
              p_l1_event_data->e_event_type, 0, 0 );
    return FALSE;
  }
  if ( l1_cell_meas_update_ptr == NULL )
  {
    MSG_HIGH( "l1_cell_meas_update_ptr is NULL", 0, 0, 0);
    return FALSE;
  }

  ipc_msg_ptr = os_IpcCreate( sizeof(tm_l1_event_s_type), 
                              IPC_ALLOCATION_DYNAMIC,
                              (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "IPC message allocation failed",0,0,0 );
    return FALSE;
  }

  msg_to_tm_ptr = ( tm_l1_event_s_type *)ipc_msg_ptr->p_Data;
  msg_to_tm_ptr->e_event_type = p_l1_event_data->e_event_type;

  msg_to_tm_ptr->u.z_tm_l1_cell_meas_update = 
    *l1_cell_meas_update_ptr;  

  ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_L1;
  ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
  ipc_msg_ptr->q_Size           = sizeof(tm_l1_event_s_type);


  if ( os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM) == FALSE )
  {
    MSG_ERROR("Error in sending L1 msg to TM thread",0,0,0);
    (void)os_IpcDelete(ipc_msg_ptr);
    ipc_msg_ptr = NULL;
  } /* End status */
  else
  {
    MSG_HIGH("sending L1 msg to TM thread", 0,0,0);
  } /* end else status */


  return TRUE;

}


#ifdef FEATURE_CGPS_LTE_CELLDB

/*===========================================================================

FUNCTION
  tm_lte_l1_cell_meas_update

DESCRIPTION
  This function sends a cell-measurement update message (MRL) to from LTE L1 to
  TM

   Parameters:

DEPENDENCIES
   None.

RETURN VALUE
   - TRUE => Succesful, FALSE => Failure

SIDE EFFECTS
   None.
===========================================================================*/
static boolean l1_lte_cell_meas_update( lte_ml1_gps_event_data_s *p_l1_event_data  )
{
  os_IpcMsgType                       *ipc_msg_ptr = NULL;
  tm_l1_event_s_type                  *msg_to_tm_ptr = NULL;
  lte_ml1_gps_mrl_cell_meas_update_t  *l1_cell_meas_update_ptr = NULL;

  l1_cell_meas_update_ptr = 
    (lte_ml1_gps_mrl_cell_meas_update_t *)p_l1_event_data->z_l1_cgps_msg_data.p_data;


  if ( p_l1_event_data->e_event_type != LTE_ML1_GPS_EVENT_CELL_MEAS_UPDATE )
  {
    MSG_HIGH( "Unsupported event type: %u", 
              p_l1_event_data->e_event_type, 0, 0 );
    return FALSE;
  }
  if ( l1_cell_meas_update_ptr == NULL )
  {
    MSG_HIGH( "l1_cell_meas_update_ptr is NULL", 0, 0, 0);
    return FALSE;
  }


  ipc_msg_ptr = os_IpcCreate( sizeof(tm_l1_event_s_type), 
                              IPC_ALLOCATION_DYNAMIC,
                              (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "IPC message allocation failed",0,0,0 );
    return FALSE;
  }

  msg_to_tm_ptr = ( tm_l1_event_s_type *)ipc_msg_ptr->p_Data;
  msg_to_tm_ptr->e_event_type = (l1_cgps_event_e_type)(p_l1_event_data->e_event_type);

  msg_to_tm_ptr->u.z_tm_lte_l1_cell_meas_update = 
    *l1_cell_meas_update_ptr;      

  ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_LTE_L1;
  ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
  ipc_msg_ptr->q_Size           = sizeof(tm_l1_event_s_type);


  if ( os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM) == FALSE )
  {
    MSG_ERROR("Error in sending LTE L1 msg to TM thread",0,0,0);
    (void)os_IpcDelete(ipc_msg_ptr);
    ipc_msg_ptr = NULL;
	return FALSE;
  } /* End status */
  else
  {
    MSG_HIGH("sending L1 msg to TM thread", 0,0,0);
	return TRUE;
  } /* end else status */

}

#endif /* SUPL2 + LTE_CELL_DB */


/*===========================================================================

FUNCTION tm_l1_iface_proc_event

DESCRIPTION
  This function is used by TM to process an L1 Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_l1_iface_proc_event( void *p_msg )
{

  tm_l1_event_s_type               *msg_from_l1_ptr; 
  prtl_l1_event_s_type             z_tm_to_prtl_l1_event; 
  tm_prtl_l1_event_data_s_type     z_l1_event_data;
  
  if( p_msg == NULL )
  {
    MSG_ERROR( "TM_L1_IFACE: Invalid NULL pointer in proc_event", 0, 0, 0 );
    return;
  }

  msg_from_l1_ptr = ( tm_l1_event_s_type *)p_msg;
  z_tm_to_prtl_l1_event.l1_event_data_ptr = &z_l1_event_data;
#if defined(FEATURE_CGPS_UMTS_UP) || defined (FEATURE_FSM_V2)
  z_l1_event_data.e_event_type = msg_from_l1_ptr->e_event_type;

  if ( z_l1_event_data.e_event_type == L1_CGPS_EVENT_CELL_MEAS_UPDATE )
  {
    z_l1_event_data.u.l1_cell_meas_update_ptr = &msg_from_l1_ptr->u.z_tm_l1_cell_meas_update;
  }
#endif /* defined(FEATURE_CGPS_UMTS_UP) || defined (FEATURE_FSM_V2) */

  MSG_MED( "TM_L1_IFACE: tm_l1_iface_proc_event called", 0, 0, 0 );
  tm_core_l1_notify_event( &z_tm_to_prtl_l1_event );


}

#ifdef FEATURE_CGPS_LTE_CELLDB   

/*===========================================================================

FUNCTION tm_lte_l1_iface_proc_event

DESCRIPTION
  This function is used by TM to process an LTE L1 Event Message. This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_lte_l1_iface_proc_event( void *p_msg )
{

  tm_l1_event_s_type                 *msg_from_l1_ptr; 
  prtl_lte_l1_event_s_type           z_tm_to_prtl_l1_event; 
  tm_prtl_lte_l1_event_data_s_type   z_l1_event_data;
  
  if( p_msg == NULL )
  {
    MSG_ERROR( "TM_L1_IFACE: Invalid NULL pointer in proc_event", 0, 0, 0 );
    return;
  }

  msg_from_l1_ptr = ( tm_l1_event_s_type *)p_msg;
  z_tm_to_prtl_l1_event.l1_event_data_ptr = &z_l1_event_data;
  z_l1_event_data.e_event_type = (lte_ml1_gps_event_e)(msg_from_l1_ptr->e_event_type);

  if ( z_l1_event_data.e_event_type == LTE_ML1_GPS_EVENT_CELL_MEAS_UPDATE )
  {
    z_l1_event_data.u.l1_cell_meas_update_ptr = &msg_from_l1_ptr->u.z_tm_lte_l1_cell_meas_update;
  }

  MSG_MED( "TM_L1_IFACE: tm_lte_l1_iface_proc_event called", 0, 0, 0 );
  tm_core_lte_l1_notify_event( &z_tm_to_prtl_l1_event );

}
#endif /* SUPL2 + LTE_CELL_DB */


/*===========================================================================

FUNCTION tm_l1_iface_init

DESCRIPTION
  This function initializes the TM-L1 IFACE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_l1_iface_init( void )
{

  /* Register an event handler with WCDMA L1 */
  #if defined (FEATURE_DUAL_WCDMA) && defined (FEATURE_CGPS_DUAL_WCDMA)
    /* W+W Dual target. Register callbacks for both the subs */
    if ((gnss_wwan_iface_wl1_cgps_register_event_cb(l1_wcdma_cell_meas_update, SYS_MODEM_AS_ID_1) == FALSE) ||
		(gnss_wwan_iface_wl1_cgps_register_event_cb(l1_wcdma_cell_meas_update, SYS_MODEM_AS_ID_2) == FALSE))
    {
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "WCMDA L1 event_cb reg. failed for W+W target");
    }
  #else
  	/* Fall back to legacy method of registering callback with primary/default sub (sub0)*/
    #ifdef FEATURE_CGPS_DUAL_WCDMA
    if(gnss_wwan_iface_wl1_cgps_register_event_cb(l1_wcdma_cell_meas_update, SYS_MODEM_AS_ID_1) == FALSE)
	#else
	if ( gnss_wwan_iface_l1_cgps_register_event_cb( l1_wcdma_cell_meas_update ) == FALSE )
	#endif
    {
      MSG_ERROR("WCMDA L1 event_cb reg. failed", 0, 0, 0);
    }
  #endif /*FEATURE_DUAL_WCDMA && FEATURE_CGPS_DUAL_WCDMA*/
  
  /* register an event handler with LTE L1 
     API for GPS to register call back function for LTE to report MRL:
     the function is defined in lte_ml1_common_gps_stm.c, and is called by
     the GPS function of tm_l1_iface_init() in tm_l1_iface.c, for registering 
     the call back. 

     The call back function itself is:
       l1_lte_cell_meas_update(), also in tm_l1_iface.c, written and maintained
       by the GPS team. 

     The function that LTE uses to invoke the GPS registered call back function 
     for sending MRL info. to GPS is:  
      l1_cgps_lte_cell_meas_update(), in lte_ml1_common_gps_stm.c

     LTE has a function that defines a structure of the type of 
     lte_ml1_gps_mrl_cell_meas_update_t as defined above in this file, populates it 
     with the relevant information, and passes the pointer of this structure to 
     l1_cpg_lte_cell_meas_update() when ready to send such info. to GPS. 
 */
 
  #ifdef FEATURE_CGPS_LTE_CELLDB
  lte_ml1_register_gps_event_cb( l1_lte_cell_meas_update ); 
  #endif
}

#ifdef FEATURE_CGPS_WCDMA
#ifdef FEATURE_CGPS_DUAL_WCDMA
/*===========================================================================

FUNCTION
  tm_l1_iface_send_wcdma_cell_meas_req

DESCRIPTION
  This function requests cell measurement data from WCDMA L1.

PARAMETERS
   sys_modem_as_id_e_type. Specifies the sub id to which the request must be sent

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_l1_iface_send_wcdma_cell_meas_req(sys_modem_as_id_e_type as_id)
{
  /* local cmd var to send to L1 */
  l1_ext_cmd_type  *cmd_ptr;

  cmd_ptr = gnss_wwan_iface_l1_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_l1_iface_send_wcdma_cell_meas_req: cmd_ptr is NULL");
    return;
  }

  cmd_ptr->cmd_hdr.cmd_id = CPHY_LSM_CELL_MEAS_REQ;

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Sending L1 cmd CPHY_LSM_CELL_MEAS_REQ(0x%x) for sub id %u", cmd_ptr->cmd_hdr.cmd_id, (uint8)as_id);
  gnss_wwan_iface_wl1_put_cmd(cmd_ptr, as_id);
}
#else
/*===========================================================================

FUNCTION
  tm_l1_iface_send_wcdma_cell_meas_req

DESCRIPTION
  This function requests cell measurement data from WCDMA L1.

PARAMETERS
   None

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_l1_iface_send_wcdma_cell_meas_req(void)
{
  /* local cmd var to send to L1 */
  l1_ext_cmd_type  *cmd_ptr;

  cmd_ptr = gnss_wwan_iface_l1_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
    MSG_ERROR("tm_l1_iface_send_wcdma_cell_meas_req: cmd_ptr is NULL",0,0,0);
  return;
  }

  cmd_ptr->cmd_hdr.cmd_id = CPHY_LSM_CELL_MEAS_REQ;

  MSG_HIGH( "Sending L1 cmd CPHY_LSM_CELL_MEAS_REQ(0x%x)", cmd_ptr->cmd_hdr.cmd_id, 0, 0 );
  gnss_wwan_iface_l1_put_cmd( cmd_ptr );
}
#endif /*FEATURE_CGPS_DUAL_WCDMA*/

#endif /* FEATURE_CGPS_WCDMA */

#endif  /* FEATURE_USER_PLANE_MRL */

