#ifndef MSM_CM_H
#define MSM_CM_H
/*===========================================================================

               MBMS SERVICE MANAGER - CALL MANAGER INTERFACE FILE (MSM_CM.H)

DESCRIPTION
  This file contains interface types for MBMS Service Manager and CM

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mbmsm/inc/msm_cm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   vr      Created initial file


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "sys.h"
#include "sys_v.h"
#include "cm.h"
#include "cm_v.h"


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

typedef struct
{
  uint32 num_mbms_flows;
  sys_bcmcs_flow_addr_s_type mbms_flow_info[SYS_MAX_MBMS_SERVICES];
}msm_cm_service_act_req_s_type;

typedef struct
{
  uint32 num_mbms_flows;
  sys_bcmcs_flow_addr_s_type mbms_flow_info[SYS_MAX_MBMS_SERVICES];
}msm_cm_service_deact_req_s_type;

typedef struct
{
  uint32 num_mbms_flows;
  sys_bcmcs_flow_addr_s_type mbms_flow_info[SYS_MAX_MBMS_SERVICES];
}msm_cm_service_act_cnf_s_type;

typedef struct
{
  uint32 num_mbms_flows;
  sys_bcmcs_flow_addr_s_type mbms_flow_info[SYS_MAX_MBMS_SERVICES];
}msm_cm_service_ind_s_type;

typedef struct
{
  uint32 num_mbms_flows;
  sys_bcmcs_flow_addr_s_type mbms_flow_info[SYS_MAX_MBMS_SERVICES];
}msm_cm_service_deact_cnf_s_type;

/* Phase 2A structures */

/* Struct to hold MBMS commands sent to the lower layers */
typedef struct 
{
   /* MBMS command being sent by the client */
   sys_gw_mbms_req_e_type                          cmd_type;
   union {
     sys_gw_mbms_activate_req_s_type               activate_req;
     sys_gw_mbms_deactivate_req_s_type             deactivate_req;
     sys_gw_mbms_session_complete_ind_s_type       session_complete_ind;
     sys_gw_mbms_abort_req_s_type                  abort_req;
     sys_gw_mbms_suspend_req_s_type                suspend_req;
     sys_gw_mbms_resume_req_s_type                 resume_req;
     sys_gw_mbms_avail_srv_list_req_s_type         avail_srv_list_req;
     sys_gw_mbms_capability_req_s_type             capability_req;
     sys_gw_mbms_set_attr_req_s_type               set_attr;                    
    
   }cmd;   
} msm_cm_mbms_generic_cmd_s_type ;

/* Struct to hold MBMS report parameters sent by lower layers */
typedef struct 
{
  /* MBMS report type */
  sys_gw_mbms_resp_e_type   rpt_type;
  union{
    sys_gw_mbms_resp_activate_cnf_s_type            activate_cnf;
    sys_gw_mbms_resp_deactivate_cnf_s_type          deactivate_cnf;
    sys_gw_mbms_resp_service_ind_s_type             service_ind;
    sys_gw_mbms_resp_capability_ind_s_type          capability_ind;
    sys_gw_mbms_resp_context_activated_ind_s_type   context_activated_ind;
    sys_gw_mbms_resp_context_deactivated_ind_s_type context_deactivated_ind;
    sys_gw_mbms_resp_abort_cnf_s_type               abort_cnf;
    sys_gw_mbms_resp_suspend_cnf_s_type             suspend_cnf;
    sys_gw_mbms_resp_resume_cnf_s_type              resume_cnf;
    sys_gw_mbms_resp_avail_srv_list_cnf_s_type      avail_srv_list_cnf;
    sys_gw_mbms_resp_ll_ready_ind_s_type            ll_ready_ind;
    sys_gw_mbms_resp_set_attr_s_type                set_attr;                    

  }rpt;
} msm_cm_mbms_generic_rpt_s_type;

#endif

