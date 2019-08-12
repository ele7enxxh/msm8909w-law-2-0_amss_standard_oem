#ifndef MSM_SM_H
#define MSM_SM_H
/*========================================================================================

                                    M S M  S M  I N T E R F A C E

DESCRIPTION

  This file contains the interface commands and stuctures between SM and MBMS Service Manager


  Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/msm_sm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------    ---     ----------------------------------------------------------


===========================================================================*/
                   

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "sm_cm.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*===========================================================================
**                       ENUMERATED TYPES
**=========================================================================*/

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/* --------------------------------------------------------------------------
** Primitive  : MSM_SM_ACTIVATION_REQ, 
** Description: Mobile originated (MO) MBMS context activation Req.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_activate_req
{
  byte                         connection_id;
  sys_ip_address_s_type        multicast_addr;
  apn_T                        apn_name;

  mbms_prot_config_options_T   mbms_config_options;
  mbms_bearer_cap_T            mbms_bearer_cap;

  nsapi_T                      nsapi;
  byte                         linked_ti;

  sys_modem_as_id_e_type    as_id;
} smreg_mbms_activate_req_T;

/* --------------------------------------------------------------------------
** Primitive  : MSM_SM_MBMS_ACTIVATE_REJ_RESP,
** Description: Mobile sends the MBMS activation failed response to the 
                (Mobile Terminated) MBMS activation request.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_activate_rej_resp
{
  sm_status_T                  cause;
  nsapi_T                      nsapi; 
  sys_ip_address_s_type        multicast_addr; 

  mbms_prot_config_options_T   mbms_config_options;


  sys_modem_as_id_e_type    as_id;

} smreg_mbms_activate_rej_resp_T;


/* --------------------------------------------------------------------------
** Primitive  : MSM_SM_ABORT_REQ,
** Description: CM sends the Abort Request to SM to deactivate all/one MBMS context.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_abort_req
{
  byte                  length;
  sys_ip_address_s_type multicast_addr[SYS_MBMS_NUM_MAX_SRVS_PER_REQ]; 
  byte                  trans_id[SYS_MBMS_NUM_MAX_SRVS_PER_REQ];

  sys_modem_as_id_e_type    as_id;

} smreg_mbms_abort_req_T;

/* --------------------------------------------------------------------------
** Primitive  : SM_MSM_MBMS_ACTIVATE_CNF, 
** Description: Confirmation for the MO MBMS activation request message.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_activate_cnf
{
  byte                         connection_id;
  sys_ip_address_s_type        multicast_addr;
 
  mbms_prot_config_options_T   mbms_config_options;
  tmgi_T                       tmgi;
 
  nsapi_T                      nsapi;

  sys_modem_as_id_e_type    as_id;

} smreg_mbms_activate_cnf_T;

/* --------------------------------------------------------------------------
** Primitive  : SM_MSM_MBMS_ACTIVATE_REJ, 
** Description: MO MBMS activation request rejected by the network.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_activate_rej
{
  byte                        connection_id;
  cause_enum_type_T           cause_type;
  sys_ip_address_s_type       multicast_addr;
  pdp_cause_T                 cause;
  
  mbms_prot_config_options_T  mbms_config_options;
 
  nsapi_T                     nsapi;

  sys_modem_as_id_e_type    as_id;

} smreg_mbms_activate_rej_T;

/* --------------------------------------------------------------------------
** Primitive  : SMREG_MBMS_ACTIVATE_IND,
** Description: Mobile Terminated (MT) MBMS activation indication. Network
                has initiated a MBMS activation request.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_activate_ind
{
  byte                         connection_id;
  sys_ip_address_s_type        multicast_addr;
  apn_T                        apn_name;
  
  mbms_prot_config_options_T   mbms_config_options;

  nsapi_T                      nsapi;
  byte                         linked_ti;

  sys_modem_as_id_e_type    as_id;

} smreg_mbms_activate_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SM_MSM_MBMS_DEACTIVATE_CNF
** Description: Confirmation for the MO PDP deactivation request.
** -------------------------------------------------------------------*/
typedef struct smreg_mbms_deactivate_cnf
{
  byte                       connection_id;
  nsapi_T                    nsapi;
  sys_ip_address_s_type      multicast_addr;

  protocol_cfg_options_T     config_options;

 
  mbms_prot_config_options_T mbms_config_options;
 

  sys_modem_as_id_e_type    as_id;

}smreg_mbms_deactivate_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SM_MSM_MBMS_DEACTIVATE_IND
** Description: Mobile Terminated (MT) PDP deactivation indication.
** -------------------------------------------------------------------*/
typedef struct smreg_mbms_deactivate_ind
{
  byte                       connection_id;
  nsapi_T                    nsapi;
  sys_ip_address_s_type      multicast_addr;
  ti_T                       tear_down_ind;
  cause_enum_type_T          cause_type;
  pdp_cause_T                cause;

  protocol_cfg_options_T     config_options;

 
  mbms_prot_config_options_T mbms_config_options;
 

  sys_modem_as_id_e_type    as_id;

}smreg_mbms_deactivate_ind_T;


/* --------------------------------------------------------------------------
** Primitive  : SM_MSM_ABORT_CNF,
** Description: CM sends the Abort Request to SM to deactivate all/one MBMS context.
** ------------------------------------------------------------------------*/
typedef struct smreg_mbms_abort_cnf
{
  byte                         length;
  sys_ip_address_s_type        multicast_addr[SYS_MBMS_NUM_MAX_SRVS_PER_REQ];
  byte                         trans_id[SYS_MBMS_NUM_MAX_SRVS_PER_REQ];

  sys_modem_as_id_e_type    as_id;

} smreg_mbms_abort_cnf_T;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

#endif

