#ifndef TM_JGPS_H
#define TM_JGPS_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM JGPS Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for Japan 
  GPS specification for KDDI.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/inc/tm_jgps.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*=============================================================================

                                Include Files

=============================================================================*/
#include "tm_common.h"
#include "pdapi.h"

#ifdef FEATURE_JCDMA_GPSONE

#include "tm_1x_up_smod_iface.h"

/*=============================================================================

                                Extern Functions

=============================================================================*/

/*=============================================================================

       Extern Functions: Interface with 1x UP (1x UP => JGPS)

=============================================================================*/
  
/*===========================================================================

FUNCTION: tm_1x_up_smod_start_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver(1x UP) to Japan-GPS sub-module
  to see if a request to start an IS-801 session should be allowed or not

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
tm_1x_up_smod_req_result_e_type tm_jgps_smod_start_sess_req
(
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_1x_up_smod_curr_op_mode_e_type  curr_mode,
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
);


/*===========================================================================

FUNCTION: tm_jgps_smod_stop_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module to stop
  current session.
  

PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_jgps_smod_stop_sess_req
(  
  pdsm_pd_end_e_type                  end_status,
  pdsm_pd_dload_data_s_type           *dload_info_ptr,
  tm_1x_up_smod_curr_op_mode_e_type   curr_mode,
  tm_1x_up_smod_req_status_e_type     *req_status_ptr
);

/*===========================================================================

FUNCTION: tm_jgps_smod_info_available

DESCRIPTION:
  This function is called by IS-801 Driver to provide information to 
  1x UP sub-modules.  In most cases this is the LR data.

   
PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_jgps_smod_info_available
(
  pdsm_pd_info_s_type                *smod_pd_info_ptr,
  pdsm_pd_end_e_type                 smod_end_status, 
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
);


/*===========================================================================

FUNCTION: tm_jgps_smod_data_pipe_req

DESCRIPTION:
  This function is called by IS-801 Driver to request that protocol sub-module
  (Vx or JGPS/KDDI) to append any specific header information before sending 
  the data on the TCP/IP connection

   
PARAMETERS:
  
===========================================================================*/
int tm_jgps_smod_data_pipe_req
(
  boolean   close_flag,
  byte      *data_ptr,
  int16     data_ptr_len
);


/*=============================================================================

       Extern Functions: Interface with TM-Core (TM-Core => JGPS)

=============================================================================*/

/*===========================================================================

FUNCTION tm_jgps_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize JGPS module
  
DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
extern void tm_jgps_prtl_init(void);


#endif /* FEATURE_JCDMA_GPSONE */
  
#ifdef __cplusplus
}
#endif

#endif /* TM_JGPS_H */
