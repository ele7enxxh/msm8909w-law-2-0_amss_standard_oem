

/*!
  @file
  emm_connection_handler.h

  @brief
  EMM internal data structure definitions.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/emm_connection_type.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
10/06/09   RI      Fix new line warning at EOF.
08/14/09   zren    Modified procedure connection release handling
05/19/09   hnam    Added TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN 
                   in lte_nas_emm_connection_cause_type
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
===========================================================================*/

#ifndef _EMM_CONNECTION_TYPE_H_
#define _EMM_CONNECTION_TYPE_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "lte_rrc_ext_msg.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/
typedef enum
{
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_SUCCESS,
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN,
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO,
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL,
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF,
  TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN,
  CONN_CNF_LTE_RRC_CONN_EST_SUCCESS,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT,
  CONN_REL_LTE_RRC_CONN_REL_NORMAL,
  CONN_REL_LTE_RRC_CONN_REL_RLF,
  CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE,
  CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE,
  CONN_REL_LTE_RRC_CONN_REL_ABORTED,
  CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR,
  CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS,
  EMM_SRV_REQ_FAILURE_NO_THROTTLE,
  EMM_SRV_REQ_FAILURE_THROTTLE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE,
  CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB
} lte_emm_connection_cause_type ;

typedef enum
{
  EMM_IDLE_STATE,
  EMM_WAITING_FOR_RRC_CONFIRMATION_STATE,
  EMM_CONNECTED_STATE,
  EMM_RELEASING_RRC_CONNECTION_STATE
} emm_connection_state_type ;

/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/


/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

#endif /* _EMM_CONNECTION_TYPE_H_ */


#endif /*FEATURE_LTE*/

