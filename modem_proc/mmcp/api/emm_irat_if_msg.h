/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/emm_irat_if_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
#ifndef _EMM_IRAT_IF_H_
#define _EMM_IRAT_IF_H_

#include <customer.h>

#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include "lte_irat_types.h"
#include "lte_rrc_ext_msg.h"
#include "lte_nas_common.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/
#define LTE_NAS_IRAT_ACCESS_BARRING_NONE  0
#define LTE_NAS_IRAT_FAILURE_CAUSE_NONE   0

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/
enum
{
  MSGR_DEFINE_UMID(NAS, EMM, REQ, IRAT_UL_MSG,      0x03, emm_irat_ul_msg_req_type),
  MSGR_DEFINE_UMID(NAS, EMM, REQ, IRAT_HDR_UL_MSG,  0x04, emm_irat_3gpp2_ul_msg_req_type),
  MSGR_DEFINE_UMID(NAS, EMM, CNF, IRAT_UL_MSG,      0x01, emm_irat_ul_msg_cnf_type),
  MSGR_DEFINE_UMID(NAS, EMM, CNF, IRAT_HDR_UL_MSG,  0x02, emm_irat_3gpp2_ul_msg_cnf_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_DL_MSG,      0x01, emm_irat_dl_msg_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_IMSI_ATTACH, 0x02, emm_irat_imsi_attach_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_CTXT_LOST,   0x03, emm_irat_ctxt_lost_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_FAILURE,     0x04, emm_irat_failure_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_HDR_FAILURE, 0x07, emm_irat_failure_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, IRAT_HDR_DL_MSG,  0x08, emm_irat_3gpp2_dl_msg_ind_type),
};

/* Indicate failure cause type */
typedef enum
{
  LTE_NAS_IRAT_NONE,
  LTE_NAS_IRAT_INVALID_STATE,
  LTE_NAS_IRAT_CONN_EST_FAILURE,
  LTE_NAS_IRAT_CONN_REL,
  LTE_NAS_IRAT_UL_MSG_CNF_FAILURE,
  LTE_NAS_IRAT_ACCESS_BARRING, 
  LTE_NAS_IRAT_LOWER_LAYER_FAILURE,
  LTE_NAS_IRAT_T3417_EXPIRY,
  LTE_NAS_IRAT_SR_REJECT,
  LTE_NAS_IRAT_SIM_BUSY
   
}emm_irat_failure_cause_type ;

typedef union
{
  /**< Detailed conn est status */
  lte_rrc_conn_est_status_e      est_status;
  
  /**< Detailed conn release reason */
  lte_rrc_conn_rel_reason_e      rel_reason;
  
  /**< Detailed data cnf status */
  lte_rrc_ul_data_cnf_status_e   cnf_status;
  
  /**< Detailed emm cause in SR reject */
  lte_nas_emm_cause_type         emm_cause;
}emm_failure_cause_value_type;


/* Causes for ctxt lost */
typedef enum
{
  LTE_NAS_CTXT_LOST_NONE,
  LTE_NAS_CTXT_LOST_IMSI_PAGING,
  LTE_NAS_CTXT_LOST_IDENTITY_REQUEST
   
}emm_irat_ctxt_lost_cause_type;

/* Uplink irat tunnel message request */
typedef struct
{
  /**<  Message router header */
  msgr_hdr_s                    msg_hdr;  

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
       For example, NAS_1XCP for 1XCP */
  dword                         trans_id; 

  /* Uplink tunnel message */
  lte_irat_tunnel_ul_msg_req_s  msg;

}emm_irat_ul_msg_req_type;


/* Uplink irat tunnel message cnf */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                    msg_hdr;  

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
     For example, NAS_1XCP for 1XCP */
  dword                         trans_id; 

}emm_irat_ul_msg_cnf_type;


/* Downlink irat tunnel message indication */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                    msg_hdr;

  /**< Downlink tunnel message */
  lte_irat_tunnel_dl_msg_ind_s  msg;

}emm_irat_dl_msg_ind_type;


/* Uplink irat tunnel message request with dsm item */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                    msg_hdr;  

  msgr_attach_s                 dsm_attach[LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE];

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
     For example, NAS_1XCP for 1XCP */
  dword                         trans_id; 

  /**< Uplink tunnel message */
  lte_irat_cdma_tunnel_ul_msg_s msg; 

}emm_irat_3gpp2_ul_msg_req_type;

/* Uplink irat tunnel message cnf */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                    msg_hdr;  

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
       For example, NAS_1XCP for 1XCP */
  dword                         trans_id; 

}emm_irat_3gpp2_ul_msg_cnf_type;

/* Downlink irat tunnel message indication with dsm item */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                     msg_hdr;

  /**< IRAT message container for dedicated CDMA 2000 Info */
  msgr_attach_s                  dsm_attach;    

  /* Downlink tunnel message */
  lte_irat_cdma_tunnel_dl_msg_s  dl_msg_info;

}emm_irat_3gpp2_dl_msg_ind_type;


/* IMSI Attach indication */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s  msg_hdr;

}emm_irat_imsi_attach_ind_type;

/* Context lost indication */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                     msg_hdr;

  /**< Cause */
  emm_irat_ctxt_lost_cause_type  cause;

}emm_irat_ctxt_lost_ind_type;

/*! emm failure type and cause */
typedef struct
{
  emm_irat_failure_cause_type cause_type;   /**< Failure cause type */

  emm_failure_cause_value_type cause;

  /**< Remaining access barring timer value if failure is because of access
       barring */
  uint64                           access_barring_rem_time;

}emm_failure_type;

/* Irat failure indication */
typedef struct
{
  /**< Message router header */
  msgr_hdr_s                       msg_hdr;

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
       For example, NAS_1XCP for 1XCP */
  dword                            trans_id;

  /**< Failure cause type */
  emm_irat_failure_cause_type      cause_type;

  emm_failure_cause_value_type     cause;

  /**< Remaining access barring timer value if failure is because of access
       barring */
  uint64                           access_barring_rem_time;

}emm_irat_failure_ind_type;

typedef union
{  
  msgr_hdr_s                      msg_hdr; /**< Message router header */

  emm_irat_ul_msg_req_type        irat_ul_msg_req;  
  emm_irat_3gpp2_ul_msg_req_type  irat_3gpp2_ul_msg_req;
}emm_csfb_msgr_cmd_type;


typedef struct
{
  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type. 
                            MSGR expects q_link to be the first element and the msgr_hdr_s 
                            as the second element.So, after link_type no element should be 
                            added and the only element is the union below*/

  emm_csfb_msgr_cmd_type  cmd;

}emm_csfb_cmd_type;


#endif /* _EMM_IRAT_IF_H_ */

