#ifndef TM_1X_UP_SMOD_IFACE_H
#define TM_1X_UP_SMOD_IFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM 1x UP Sub-Module interface Header File

DESCRIPTION
  This header file contains definitions of data structure necessary 
  for the 1x User Plane Sub-Module interface.
  By providing API functions to IS-801 Driver, sub-module protocols,
  such as V1/V2 or Japan GPS (JGPS/KDDI), will be able to provide services to 
  IS-801 Driver.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/prot/tm_1x_up_smod_iface.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*=============================================================================

                                Include Files

=============================================================================*/
#include "tm_common.h"
#include "pdapi.h"

#include "tm_prtl_iface.h"
/*=============================================================================

                                Data Structures

=============================================================================*/

/*--------------------------------------------------

   Request API return value enum 

---------------------------------------------------*/
typedef enum
{
  TM_1X_UP_SMOD_REQ_FAIL    = -1,
  TM_1X_UP_SMOD_REQ_SUCCESS = 0      
} tm_1x_up_smod_req_result_e_type;


/*--------------------------------------------------

   Request API status enum 

---------------------------------------------------*/
typedef enum
{
  TM_1X_UP_SMOD_REQ_STATUS_NONE                = 0,
  TM_1X_UP_SMOD_REQ_STATUS_GRANT               = 1,
  TM_1X_UP_SMOD_REQ_STATUS_WAIT                = 2,
  TM_1X_UP_SMOD_REQ_DENIED                     = 3,
  TM_1X_UP_SMOD_REQ_ERR_DATA_INVALID_SIZE      = 101,
  TM_1X_UP_SMOD_REQ_ERR_SEND_DATA              = 102,
  TM_1X_UP_SMOD_REQ_ERR_BUF_UNAVAIL            = 103,
  TM_1X_UP_SMOD_REQ_ERR_INVALID_MODE           = 104,
  TM_1X_UP_SMOD_REQ_ERR_UNKNOWN                = 105,
  TM_1X_UP_SMOD_REQ_STATUS_MAX  
} tm_1x_up_smod_req_status_e_type;


/*--------------------------------------------------

   Current 1x UP (IS-801 Driver) Operation mode

---------------------------------------------------*/
typedef enum
{
  TM_1X_UP_SMOD_CURR_OP_MODE_NONE = 0,
  TM_1X_UP_SMOD_CURR_OP_MODE_MSA,      /* Ms-Assisted Fix */
  TM_1X_UP_SMOD_CURR_OP_MODE_MSB,      /* Ms-Based Fix */
  TM_1X_UP_SMOD_CURR_OP_MODE_DLOAD,    /* Navigation Data Download via PDE */
  TM_1X_UP_SMOD_CURR_OP_MODE_MAX
}tm_1x_up_smod_curr_op_mode_e_type;

/*--------------------------------------------------

   Request data pipe status

---------------------------------------------------*/
typedef enum
{
  TM_1X_UP_SMOD_DATA_CMD_NONE = 0,
  TM_1X_UP_SMOD_DATA_CMD_IS801_RLINK,  
  TM_1X_UP_SMOD_DATA_CMD_MAX
} tm_1x_up_smod_data_pipe_cmd_e_type;

#define TM_1X_UP_SMOD_DATA_PIPE_REQ_FAIL           0
#define TM_1X_UP_SMOD_DATA_PIPE_REQ_CLOSE_GRANT    1

/*=============================================================================

                                Extern Functions

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
extern tm_1x_up_smod_req_result_e_type 
tm_1x_up_smod_start_sess_req
(
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_1x_up_smod_curr_op_mode_e_type  curr_mode,
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
);

/*===========================================================================

FUNCTION: tm_1x_up_smod_stop_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module to stop
  current session.
  

PARAMETERS:
  
===========================================================================*/
extern tm_1x_up_smod_req_result_e_type 
tm_1x_up_smod_stop_sess_req
(  
  pdsm_pd_end_e_type                end_status,
  pdsm_pd_dload_data_s_type         *dload_info,
  tm_1x_up_smod_curr_op_mode_e_type curr_mode,
  tm_1x_up_smod_req_status_e_type   *req_status_ptr
);


/*===========================================================================

FUNCTION: tm_1x_up_smod_info_available

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module 
  to send information such as LR, AA, PPM, or PRM.

  The protocol sub-module (Vx or JGPS/KDDI) will determine if IS-801 should
  go to wait state or continue 
 
  
PARAMETERS:
  
===========================================================================*/
extern tm_1x_up_smod_req_result_e_type
tm_1x_up_smod_info_available
(
  pdsm_pd_info_s_type                *pd_info_ptr,
  pdsm_pd_end_e_type                 end_status,
  qword                              time, 
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
);


/*===========================================================================

FUNCTION: tm_1x_up_smod_data_pipe_req

DESCRIPTION:
  This function is called by IS-801 Driver to request that protocol sub-module
  (Vx or JGPS/KDDI) to append any specific header information before sending 
  the data on the TCP/IP connection

   
PARAMETERS:
  
===========================================================================*/
extern int tm_1x_up_smod_data_pipe_req
(
  boolean                             close_connection, 
  tm_1x_up_smod_data_pipe_cmd_e_type  cmd,
  byte                                *msg_data_ptr,
  int16                               msg_data_ptr_len
);

/*===========================================================================

FUNCTION: tm_1x_up_smod_prtl_init

DESCRIPTION:
  Init the 1x UP sub-module PRTL interface to initialize either Vx or JGPS
  module based on feature enabled.

   
PARAMETERS:
  
===========================================================================*/
extern void tm_1x_up_smod_prtl_init( void );

  
#ifdef __cplusplus
}
#endif

#endif /* TM_1X_UP_SMOD_IFACE_H */
