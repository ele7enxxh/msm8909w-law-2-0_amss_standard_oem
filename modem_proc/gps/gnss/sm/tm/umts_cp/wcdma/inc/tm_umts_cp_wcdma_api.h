#ifndef TM_UMTS_CP_WCDMA_API_H
#define TM_UMTS_CP_WCDMA_API_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-WCDMA API Header File 

GENERAL DESCRIPTION
  Prototypes of API functions defined in tm_umts_cp_wcdma_api.c and used by
  tm_umts_cp.c.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

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

  $Header: 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/07   LT      Initial version

============================================================================*/


#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif


#ifdef TM_UMTS_CP_WCDMA_API_USED_BY_UMTS_CP

void tm_umts_cp_wcdma_init (void);

prtl_start_sess_req_result_e_type tm_umts_cp_wcdma_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
);

boolean tm_umts_cp_wcdma_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
);

boolean tm_umts_cp_wcdma_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
);

boolean tm_umts_cp_wcdma_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *p_sess_info_param
);

void tm_umts_cp_wcdma_timer_cb_handler ( void *timerData );

boolean tm_umts_cp_wcdma_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload
);

boolean tm_umts_cp_wcdma_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type   *answer_ptr 
);

void tm_umts_cp_wcdma_clear_all_holdoff (void);

#endif /*  TM_UMTS_CP_WCDMA_API_USED_BY_UMTS_CP */


tm_umts_cp_wcdma_ue_pos_capability_struct_type tm_umts_cp_wcdma_ue_pos_capability_get(void); /* used by cgps_api.c */


#ifdef __cplusplus
}
#endif


#endif /* TM_UMTS_CP_WCDMA_API_H */



