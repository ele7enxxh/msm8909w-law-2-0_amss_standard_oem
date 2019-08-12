#ifndef TM_SUPL_API_H
#define TM_SUPL_API_H

/*===========================================================================
  
                            TM_UMTS_UP_SUPL_API.H  
                   
DESCRIPTION

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/supl/inc/tm_umts_up_supl_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
05/03/07    lt      Added this file header.
04/25/07    lt      Initial check in.
===========================================================================*/


#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "tm_prtl_iface.h"  

void tm_umts_up_supl_init (void);


/************** API functions TmCore uses to access SUPL ****************/
 
prtl_start_sess_req_result_e_type tm_umts_up_supl_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions

);


boolean tm_umts_up_supl_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
);


boolean tm_umts_up_supl_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
);


boolean tm_umts_up_supl_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *sess_info_param
);

#ifdef FEATURE_CGPS_PDCOMM

boolean tm_umts_up_supl_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
);

#endif

void tm_umts_up_supl_timer_cb_handler
(
  void *timerData
);


boolean tm_umts_up_supl_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload

);

boolean tm_umts_up_supl_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type *answer_ptr 
);

#ifdef FEATURE_CGPS_WCDMA
void tm_umts_supl_api_rrc_cell_info_proc
( rrcmeas_plmn_cell_info_struct_type cell_id_info,
  rrc_cell_id_status_type            status
);
#endif /* FEATURE_CGPS_WCDMA */

boolean tm_umts_up_supl_api_close_session
(tm_sess_stop_reason_e_type  stop_reason);

boolean tm_umts_up_supl_gm_event_handler
(
  uint32  msg_type,
  void    *msg_data_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* TM_SUPL_API_H */

