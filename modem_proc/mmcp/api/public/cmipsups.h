#ifndef CMIPSUPS_H
#define CMIPSUPS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   I P   SUPPLEMENTARY  SERVICE

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface an application (VoIP) to
   Call Manage for supplementary services. 

  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/12   xs      Added PS supplemetary service support
===========================================================================*/
#include "cm.h"
#include "cm_gw.h"

/*=========================================================================
**                            IP supps command type definition section
*==========================================================================*/

#define CMIPSUPS_SS_ERROR_CODE_TAG   0x90
#define CMIPSUPS_USS_ERROR_CODE_TAG  0x91


/*ss_code: ss_operation_code_T
*/
#define CMIPSUPS_CMD                         \
          cm_sups_cmd_e_type  cmd_type;      \
          byte                cm_ss_ref;     \
          byte                ss_code;       \
          basic_service_T     		  basic_service; 				\
          cm_call_barring_list_s_type *call_barring_num_list


#define CMIPSUPS_CONF                            \
          boolean                  success;      \
          byte                     cm_ss_ref


/* basic IP supps command type */
typedef struct 
{
  CMIPSUPS_CMD;
}cmipsups_cmd_s_type;

/*Register SS request */
typedef struct 
{

   CMIPSUPS_CMD;
   cm_called_party_bcd_no_T        forwarded_to_number;
   cm_called_party_subaddress_T    forwarded_to_subaddress;
   ie_no_reply_condition_timer_T   cm_no_reply_condition_timer;   
   cm_ip_sups_time_info_s_type     sups_time_info;
} cmipsups_register_ss_req_e_type;

/* Activate/Deactivate common SS request */
typedef struct 
{

   CMIPSUPS_CMD;
   cm_npi_type pi;
    /**< Presentation Indicator. */
} cmipsups_common_ss_req_e_type;

typedef cmipsups_common_ss_req_e_type cmipsups_activate_ss_req_e_type;
typedef cmipsups_common_ss_req_e_type cmipsups_deactivate_ss_req_e_type;


/*=========================================================================
**                            IP supps confirm type definition section
*==========================================================================*/

typedef struct
{
   cm_ss_conf_e_type              info_type;
   union
   {
      ie_forwarding_info_T            forwarding_info;
      ie_call_barring_info_T          call_barring_info;
      ie_cm_ss_data_T                ss_data   ;
      ie_cm_ss_error_T               cm_ss_error;
      
   } info;
} cmipsups_ss_info_s_type;


typedef struct 
{
  CMIPSUPS_CONF;
}cmipsups_conf_s_type;

/* Activate SS conf */
/* Deactivate SS conf */
/* Erase SS conf */
/* Register SS conf */
typedef struct
{
  CMIPSUPS_CONF;
  cmipsups_ss_info_s_type  sups_ss_info;
  cm_ip_sups_error_s_type  * sups_ss_error_text;
  /* To be used by IMS for propagating error description
     to AP during failure */
  uint16 retry_duration;
  /*passed on from IMS to AP to retry sups after duration
    elapses */

}cmipsups_common_ss_conf_type;

typedef cmipsups_common_ss_conf_type cmipsups_activate_ss_conf_type;
typedef cmipsups_common_ss_conf_type cmipsups_deactivate_ss_conf_type;
typedef cmipsups_common_ss_conf_type cmipsups_erase_ss_conf_type;
typedef cmipsups_common_ss_conf_type cmipsups_register_ss_conf_type;

/* Interrogate SS Conf */
typedef struct 
{
   ie_ss_status_T                 cm_ss_status  ;
   cm_npi_type 					  pi;
   basic_service_group_list_T     basic_service_group_list   ;
   ie_forwarding_feature_list_T   forwarding_feature_list;
   cli_restriction_info_T         cli_restriction_info;
   ie_call_barring_num_info_list_T *call_barring_num_info_list;
   ie_cm_ss_error_T               cm_ss_error;
   ie_cm_cc_cause_T               cm_cc_cause;
   cm_ip_sups_time_info_s_type    sups_time_info;
    
} cmipsups_interrogatess_res_s_type;

typedef struct
{
  CMIPSUPS_CONF;
  cm_ss_conf_e_type  res_type;
  cmipsups_interrogatess_res_s_type interrogate_ss_res;
  uint16 retry_duration;
  /* passed on from IMS to AP to retry sups after duration
     elapses */

}cmipsups_interrogate_ss_conf_type;


/* CM_SUPS_CMD_PROCESS_USS  */
typedef struct 
{
   CMIPSUPS_CMD;
   byte                            ss_operation;
   uss_data_T                      uss_data;
} cmipsups_process_unstructured_ss_req_s_type;


/* CM_SUPS_CMD_USS_RES  */
typedef struct 
{
   CMIPSUPS_CMD;
   byte                            invoke_id;
   uss_res_data_T                  uss_res_data;
} cmipsups_unstructured_ss_res_s_type;


/* USSD reject Reason Enum */
typedef enum {
  USS_REJECT_REASON_NONE,
  USS_REJECT_REASON_USSD_BUSY,
  USS_REJECT_REASON_MAX
  /* FOR INTERNAL USE OF CM ONLY!
  */
} cmipsups_ussd_reject_cause_e_type;

/* USSD reject cause structure */
typedef struct {
  boolean present;
  cmipsups_ussd_reject_cause_e_type cause_value;
} cmipsups_ussd_reject_cause_s_type;


/* CM_RELEASE_SS_REQ  */
typedef struct 
{
   CMIPSUPS_CMD;                 // only ss_ref and ss_code will be used.
   byte                            ss_operation;
   byte invoke_id;
   cmipsups_ussd_reject_cause_s_type release_cause;
} cmipsups_release_ss_req_s_type;


/* CM_UNSTRUCTURED_SS_IND */
typedef struct
{
  byte                            invoke_id;
  uss_data_T                      uss_data;
}cmipsups_unstructured_ss_ind_s_type;


/* CM_UNSTRUCTURED_SS_NOTIFY_IND */
typedef struct
{
  byte                            invoke_id;
  uss_data_T                      uss_data;
}cmipsups_unstructured_ss_notify_ind_s_type;


/* CM_SUPS_CMD_USS_NOTIFY_RES  */
typedef struct 
{
   CMIPSUPS_CMD;
   byte                            invoke_id;
   ie_cm_ss_error_T                cm_ss_error;
} cmipsups_unstructured_ss_notify_res_s_type;


/* CM_RELEASE_UNSTRUCTURED_SS_IND  */
typedef struct
{
  CMIPSUPS_CONF;  
}cmipsups_release_unstructured_ss_ind_s_type;


/* CM_PROCESS_UNSTRUCTURED_SS_CONF  */
typedef struct
{
  cm_ss_conf_e_type             data_type;
  union 
  {
    uss_data_T                  uss_data;
    ie_cm_ss_error_T            cm_ss_error;
    ie_cm_cc_cause_T            cm_cc_cause;
  } data;
} cmipsups_process_unstructured_ss_res_s_type; 

typedef struct
{
  CMIPSUPS_CONF; 
  cmipsups_process_unstructured_ss_res_s_type  ss_data_info;
}cmipsups_process_unstructured_ss_conf_s_type;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMIPSUPS_H */

