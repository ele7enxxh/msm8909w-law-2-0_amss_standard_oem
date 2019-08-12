#ifndef __DS3GPP_EXT_MSG_H__
#define __DS3GPP_EXT_MSG_H__
/*===========================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ds3gpp_ext_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/12   ssk      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "msgr.h"
#include "msgr_ds.h"

/* Message IDs
*/

#define DS_3GPP_INTERNAL_MSGR_ID_OFFSET         0x00
#define DS_3GPP_EXTERNAL_MSGR_ID_OFFSET         0x20


/* Enumeration for Traffic class information available as a part of
   UMTS QOS */
typedef enum { 
	DS_TRF_CLASS_CONVERSATIONAL = 1, 
	DS_TRF_CLASS_STREAMING = 2, 
	DS_TRF_CLASS_INTERACTIVE = 3, 
	DS_TRF_CLASS_BACKGROUND = 4, 
} ds_3gpp_traffic_class_enum_type; 

/* Enumeration for QCI information available as a part of
   LTE QOS */
typedef enum { 
	DS_LTE_QCI_1 = 1,
	DS_LTE_QCI_2 = 2, 
	DS_LTE_QCI_3 = 3, 
	DS_LTE_QCI_4 = 4, 
	DS_LTE_QCI_5 = 5, 
	DS_LTE_QCI_6 = 6, 
	DS_LTE_QCI_7 = 7, 
	DS_LTE_QCI_8 = 8, 
	DS_LTE_QCI_9 = 9,
 } ds_3gpp_lte_qci_enum_type; 


typedef enum
{
  DS_LOW_LATENCY_TRAFFIC_STATE_STOP,
  DS_LOW_LATENCY_TRAFFIC_STATE_START,
  DS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE,
  DS_LOW_LATENCY_TRAFFIC_STATE_MAX 
} ds_low_latency_traffic_state_enum_type;

/* Enumeration for supported RATs */
typedef enum {   
  DS_RAT_WCDMA = 0, 
  DS_RAT_TDS = 1, 
  DS_RAT_LTE = 2, 
} ds_3gpp_rat_enum_type; 

typedef struct { 
  byte rab_id; /* Network service access point identifier for this UMTS bearer.*/           
  ds_3gpp_traffic_class_enum_type traffic_class; /* QOS information of this UMTS bearer. */ 
} ds_3gpp_tds_w_qos_info_type; 

typedef struct { 
  byte eps_bearer_id; /* Id of this EPS Bearer. */ 
  ds_3gpp_lte_qci_enum_type qci_value ; /* QOS information of this EPS bearer. */ 
} ds_3gpp_lte_qos_info_type; 

/* Bearer QOS Info Message */
typedef struct { 
  msgr_hdr_struct_type hdr; /* message header */ 
  
  /* Message Payload */ 
  ds_3gpp_rat_enum_type current_bearer_rat; 
  
  union { 
     ds_3gpp_tds_w_qos_info_type tds_w_qos_info; // Valid when bearer is in WCDMA or TDSCDMA
     ds_3gpp_lte_qos_info_type lte_qos_info; //Valid when bearer is in LTE
  } u;

} ds_3gpp_bearer_qos_info_msg_s;

/* embms content desc update msg */
typedef struct
{
  msgr_hdr_s                         msg_hdr;         /* message header */

  /*------------------------------------
   |   payload size  N   |  payload  |
           m                            
   |   4 bytes (uin32)   |  N bytes  |
  -------------------------------------*/
  msgr_attach_s                      dsm_attach;
} ds_3gpp_embms_content_desc_update_msg_s;

/* embms content desc control msg */
typedef struct
{
  msgr_hdr_s                         msg_hdr;         /* message header */

  /*------------------------------------
   |   payload size  N   |  payload  |
           m                            
   |   4 bytes (uin32)   |  N bytes  |
  -------------------------------------*/
  msgr_attach_s                      dsm_attach;
} ds_3gpp_embms_content_desc_control_msg_s;

/* UMID definitions
*/
enum
{ 
  MSGR_DEFINE_UMID(DS, 3GPP, IND, BEARER_QOS_INFO, 
                   0x21, ds_3gpp_bearer_qos_info_msg_s),  
};

enum
{
  MSGR_DEFINE_UMID(DS, LTE, REQ, EMBMS_CONTENT_DESC_UPDATE, 
                   0x21, ds_3gpp_embms_content_desc_update_msg_s),
  MSGR_DEFINE_UMID(DS, LTE, IND, EMBMS_CONTENT_DESC_CONTROL,
                   0x22, ds_3gpp_embms_content_desc_control_msg_s),
};

typedef struct
{
   msgr_hdr_struct_type hdr; /* message header */ 
  /*! Traffic State */  
  ds_low_latency_traffic_state_enum_type   traffic_state;
  
  /*! Filter result */  
  uint32    filter_handle;    
  /*! Expected periodicity of the packet arrivals for this low latency 
    traffic (in ms).
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_interval_time_in_ms;
  
  /*! Expected delta time of the packet from its interval time to wake up
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_arrival_delta_in_ms;
  /*! Expected time to wait for the packets at every period 
    for this low latency traffic (in ms).
    Value 0xFFFFFFFF - means unknown  */
  uint32                     pkt_max_wait_time_in_ms;
} ds_mgr_latency_info_ext_type;



enum
{ 
  MSGR_DEFINE_UMID(DS, MGR, IND, LOW_LATENCY, 0x24, ds_mgr_latency_info_ext_type),
};


#endif /* !__DS3GPP_EXT_MSG_H__ */
