/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - Vx module

General Description
  This file contains implementations for TM Vx Module
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up_smod_iface.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
05/27/09   ank     For optimal modes, allow fallback from MS-A to MS-B and vice-a-versa.
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "sm_nv.h"
#include "pdapibuf.h"


#include "tm_1x_up_smod_iface.h"
#include "tm_1x_up_is801_drv_iface.h"


#include "tm_vx.h"
#include "tm_jgps.h"


/*=============================================================================

                                Functions

=============================================================================*/

/*===========================================================================

FUNCTION: tm_1x_up_smod_start_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver(1x UP) to 1x UP protocol sub-module 
  (Vx or JGPS/KDDI) to see if a request to start an IS-801 session 
  should be allowed or not

  If the incoming session req is granted, a 1x UP protocol sub-module 
  should return 1X_UP_PRTL_REQ_STATUS_SUCCESS and 
  populate the "reason" pointer to tell IS-801 drv either grant or wait.

  If a protocol sub-module needs authenticate the session, it should return 
  TM_1X_UP_SMOD_REQ_STATUS_WAIT, to put IS-801 Drv in wait state.
  Once the protocol sub-module (Vx or JGPS/KDDI) it authenticates the session, 
  it should send a asynchronous response back to IS-801 Driver 
  by calling tm_1x_up_post_event() func to complete the session request.
  
  If for any reason, a protocol sub-module does not allow the session request, 
  it should return TM_1X_UP_SMOD_REQ_FAIL with req_status pointer set the 
  valid stop reason status. 

PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_1x_up_smod_start_sess_req
(
  prtl_start_sess_req_param_s_type   *start_sess_req_param,
  tm_1x_up_smod_curr_op_mode_e_type  curr_mode,
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
)
{
  /* Local Variables */
  tm_1x_up_smod_req_result_e_type start_req_retval = TM_1X_UP_SMOD_REQ_FAIL;

  /*-------------------------------------------------------------------------
     Check input parameters
  -------------------------------------------------------------------------*/
  if(req_status_ptr == NULL)
  {
    return start_req_retval;
  }
  
#ifdef FEATURE_JCDMA_GPSONE
  /*-------------------------------------------------------------------------
     Japan CDMA - KDDI protocol
  -------------------------------------------------------------------------*/
  start_req_retval = tm_jgps_smod_start_sess_req( start_sess_req_param,
                                                  curr_mode,
                                                  req_status_ptr ); 
#else
  /*-------------------------------------------------------------------------
     Default Behavior (i.e. Either Vx module is defined or undefined
  -------------------------------------------------------------------------*/
  /* Return Success */
  start_req_retval = TM_1X_UP_SMOD_REQ_SUCCESS;
  
  /* Status set to grant session */
  *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_GRANT;

#endif /* FEATURE_JCDMA_GPSONE */

  return start_req_retval;
}/* tm_1x_up_smod_start_sess_req */


/*===========================================================================

FUNCTION: tm_1x_up_smod_stop_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module to stop
  current session.
  

PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_1x_up_smod_stop_sess_req
(  
  pdsm_pd_end_e_type                end_status,
  pdsm_pd_dload_data_s_type         *dload_info_ptr,  
  tm_1x_up_smod_curr_op_mode_e_type curr_mode,
  tm_1x_up_smod_req_status_e_type   *req_status_ptr
)
{
  /* Local Variables */
  tm_1x_up_smod_req_result_e_type stop_req_retval = TM_1X_UP_SMOD_REQ_FAIL;

  /*-------------------------------------------------------------------------
     Check input parameters
  -------------------------------------------------------------------------*/
  if(req_status_ptr == NULL || dload_info_ptr == NULL)
  {
    return stop_req_retval;
  }
  
#ifdef FEATURE_JCDMA_GPSONE
  
  if((tm_1x_up_is801_drv_is_optimal_mode()) && 
     ((curr_mode != TM_1X_UP_SMOD_CURR_OP_MODE_DLOAD) && (end_status != PDSM_PD_END_SESS_NONE))
    )
  {
    MSG_HIGH("Optimal mode. For curr_mode=%d end_status=%d, return from here",curr_mode,end_status,0);
    stop_req_retval = TM_1X_UP_SMOD_REQ_SUCCESS;
  }
  else
  {
  /*-------------------------------------------------------------------------
     Japan CDMA - KDDI protocol
  -------------------------------------------------------------------------*/
  stop_req_retval = tm_jgps_smod_stop_sess_req(end_status,
                                               dload_info_ptr, 
                                               curr_mode,
                                               req_status_ptr); 
  }
#else
  /*-------------------------------------------------------------------------
     Default Behavior (i.e. Either Vx module is defined or undefined
  -------------------------------------------------------------------------*/
  stop_req_retval = TM_1X_UP_SMOD_REQ_SUCCESS;

  *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_NONE;
#endif /* FEATURE_JCDMA_GPSONE */

  return stop_req_retval;
} /* tm_1x_up_smod_stop_sess_req */


/*===========================================================================

FUNCTION: tm_1x_up_smod_info_available

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module 
  to send information such as LR, AA, PPM, or PRM.

  The protocol sub-module (Vx or JGPS/KDDI) will determine if IS-801 should
  go to wait state or continue 
 
  
PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_1x_up_smod_info_available
(
  pdsm_pd_info_s_type                *pd_info_ptr,
  pdsm_pd_end_e_type                 end_status,
  qword                              qwtime, 
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
)
{
  /* Local Variables */
  tm_1x_up_smod_req_result_e_type info_req_retval = TM_1X_UP_SMOD_REQ_FAIL;
  
#ifdef FEATURE_JCDMA_GPSONE

  /*-------------------------------------------------------------------------
     Japan CDMA - KDDI protocol
  -------------------------------------------------------------------------*/
  if( req_status_ptr != NULL )
  {
    info_req_retval = tm_jgps_smod_info_available(pd_info_ptr, 
                                                  end_status, 
                                                  req_status_ptr);
  }

#else

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  if(req_status_ptr == NULL)
  {
    /*-------------------------------------------------------------------------
     Vx Protocol
    -------------------------------------------------------------------------*/
    tm_vx_pd_info_available(pd_info_ptr, qwtime, end_status);
  }

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
  /*-------------------------------------------------------------------------
     Default Behavior
  -------------------------------------------------------------------------*/
  info_req_retval = TM_1X_UP_SMOD_REQ_SUCCESS;

  if(req_status_ptr != NULL)
  {
    *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_NONE;
  }
  
#endif /* FEATURE_JCDMA_GPSONE */

  return info_req_retval;
} /* tm_1x_up_smod_info_available */


/*===========================================================================

FUNCTION: tm_1x_up_smod_data_pipe_req

DESCRIPTION:
  This function is called by IS-801 Driver to request that protocol sub-module
  (Vx or JGPS/KDDI) to append any specific header information before sending 
  the data on the TCP/IP connection

   
PARAMETERS:
  
===========================================================================*/
int tm_1x_up_smod_data_pipe_req
(
  boolean                             close_connection, 
  tm_1x_up_smod_data_pipe_cmd_e_type  cmd,
  byte                                *msg_data_ptr,
  int16                               msg_data_ptr_len
)
{
  /* Local Variables */
  int data_req_retval = TM_1X_UP_SMOD_DATA_PIPE_REQ_FAIL;
  
#ifdef FEATURE_JCDMA_GPSONE
  /*-------------------------------------------------------------------------
     Japan CDMA - KDDI protocol
  -------------------------------------------------------------------------*/
    data_req_retval = tm_jgps_smod_data_pipe_req(close_connection, 
                                                 msg_data_ptr, 
                                                 msg_data_ptr_len); 
#else
  /*-------------------------------------------------------------------------
     Default Behavior (i.e. Either Vx module is defined or undefined JCDAME)
     Call IS-801 Driver function to send the data.
  -------------------------------------------------------------------------*/
  if (close_connection)
  {
    data_req_retval = TM_1X_UP_SMOD_DATA_PIPE_REQ_CLOSE_GRANT;
  }
  else
  {
    data_req_retval = tm_1x_up_is801_drv_send_data(msg_data_ptr, msg_data_ptr_len);
  }
  
#endif /* FEATURE_JCDMA_GPSONE */

  return data_req_retval;
} /* tm_1x_up_smod_data_pipe_req */


/*===========================================================================

FUNCTION: tm_1x_up_smod_prtl_init

DESCRIPTION:
  Init the 1x UP sub-module PRTL interface to initialize either Vx or JGPS
  module based on feature enabled.

   
PARAMETERS:
  
===========================================================================*/
void tm_1x_up_smod_prtl_init (void)
{
#ifndef FEATURE_JCDMA_GPSONE
  tm_vx_prtl_init();
#else
  tm_jgps_prtl_init();
#endif /* FEATURE_JCDMA_GPSONE */
} /* tm_1x_up_smod_prtl_init */


