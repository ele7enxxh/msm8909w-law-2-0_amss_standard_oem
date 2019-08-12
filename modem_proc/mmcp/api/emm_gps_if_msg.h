
/*!
Description: 
   Interface b/w EMM & GPS module to transfer the DL_GENERIC_NAS_TRANSPORT 
     & UL_GENERIC_NAS_TRANSPORT message to & from GPD module
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/emm_gps_if_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef _EMM_GPS_IF_MSG_H_
#define _EMM_GPS_IF_MSG_H_

#include <customer.h>

#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "queue.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include <dsm_item.h>
#include "emm_irat_if_msg.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

enum
{
  MSGR_DEFINE_UMID(NAS, EMM, REQ, UL_GENERIC_NAS_TRANSPORT, 0x07, lte_nas_emm_ul_generic_transport_s_type ), /*UL_GENERIC_NAS_TRANSPORT*/
  MSGR_DEFINE_UMID(NAS, EMM, IND, DL_GENERIC_NAS_TRANSPORT, 0x09, lte_nas_emm_dl_generic_transport_s_type ), /*DL_GENERIC_NAS_TRANSPORT*/
  MSGR_DEFINE_UMID(NAS, EMM, CNF, UL_GENERIC_NAS_TRANSPORT, 0x0A, lte_nas_emm_ul_generic_transport_cnf_type ), /*UL_GENERIC_NAS_TRANSPORT_CNF*/
  MSGR_DEFINE_UMID(NAS, EMM, IND, GENERIC_NAS_TRANSPORT_FAILURE, 0x0B, emm_irat_failure_ind_type),
} ;

#define EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE 2 


typedef enum
{
  LPP_MSG_CONTAINER = 1,
  LOCATION_SERVICE_MSG_CONTAINER = 2
}lte_nas_generic_msg_container_type;

/**  
 The following structure is used by EMM module, to get the 
 UL_GENERIC_NAS_TRANSPORT message from GPS module.
 Ref: 3GPP 24.301 v9.2
 Section: 8.2.3o 'Uplink generic NAS transport'
*/
typedef struct
{
   msgr_hdr_s                             msg_hdr;  
   
   /*!< 1st Index is for 'Generic_container_value'. 
        2nd Index for 'additional_info' */
   msgr_attach_s                          dsm_attach[EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE];

   /*!< Transaction Id to communicate for a given msg */
   dword                                   trans_id; 

   lte_nas_generic_msg_container_type     generic_msg_container_type; 

}lte_nas_emm_ul_generic_transport_s_type;

/*!< 
 The following structure is used by EMM module, to get the UL_GENERIC_NAS_TRANSPORT message from queue.
*/
typedef struct
{

  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type. 
                            MSGR expects q_link to be the first element and the msgr_hdr_s 
                            as the second element.So, after link_type no element should be 
                            added and the only element is the union below*/

  /*! @brief List all the incoming messages/commands*/ 
  lte_nas_emm_ul_generic_transport_s_type gps_msgr_cmd;
} emm_gps_cmd_type ;


/**
 The following structure is used by the GPS module, to get the 
 DL_GENERIC_NAS_TRANSPORT message from NAS.
 Ref: 3GPP 24.301 v9.2
 Section: 8.2.31 'Downlink generic NAS transport'
*/

typedef struct
{
   msgr_hdr_s                             msg_hdr;    
   /*!< 1st Index is for Generic_container_value. 
     2nd Index is used for additional info*/
   msgr_attach_s                          dsm_attach[EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE];  

   lte_nas_generic_msg_container_type     generic_msg_container_type; 
}lte_nas_emm_dl_generic_transport_s_type;


typedef struct
{
  /**< Message router header */
  msgr_hdr_s                                   msg_hdr;  

  /**< Transaction ID. TECH field should be unique to make it unique for EMM
       For example, NAS_1XCP for 1XCP */
  dword                                         trans_id; 

}lte_nas_emm_ul_generic_transport_cnf_type;

#endif /* _EMM_GPS_IF_MSG_H_ */

