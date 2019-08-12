
/*==============================================================================

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


==============================================================================*/

/*Author: hnam  
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
    START DATE: 12/01/2008*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/lte_nas_esm_message_description.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/08/10   zr/fj   Moved apn_ambr_T to cm_gw.h
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures 
06/30/09   hnam    Removed sdf_qos_iei from the sdf_qos_T message structure
06/02/09   MNK     Added LBI to the esm_header
05/15/09   hnam    Changed the value of pdn_type_T (from IPV4 to NAS_ESM_IPV4)
04/28/09   MNK     Removed esm_information
04/10/09   hnam    Added a valid flag to apn_ambr_T.
04/10/09   hnam    Added parameterlist to TFT (it was not present previously)
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/20/09   hnam    Added apn_ambr_T structure, required for lte_nas_esm_act_default_bearer_context_req
03/20/09   hanm    Added last 2 IEs (ext_gtd_bit_rate_uplink and ext_max_bit_rate_uplink) for qos_T
02/27/09   MNK     Include nas_common.h to support the message validation
02/25/09   MNK     Added NAS_ESM_PTI_MISMATCH 
==============================================================================*/

#ifndef _LTE_NAS_ESM_MESSAGE_DESCRIPTION_H_
#define _LTE_NAS_ESM_MESSAGE_DESCRIPTION_H_ 

#include<customer.h>


#include "lte_nas_common.h"

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/

#define ESM_INVALID_CON_ID 0xff
#define ESM_INVALID_SDF_ID 0xffff
#define ESM_INVALID_BEARER_ID 0x0
/* Unassigned PTI */
#define ESM_INVALID_PTI 0x0

typedef struct esm_hdr_struct
{
  /* Connection ID of the bearer context */
  byte                   connection_id;  
  /* Service Data Flow id. Returned back to DS and also used in the procedure abort */
  word                   sdf_id;
  /* Linked EPS bearer ID */
  byte                   lbi;
  /* Trigger message ID */
  lte_nas_message_id     trigger_msg_id;

}esm_header_T;

/*----------------------------------------------------------
                   TRANSACTION ID
  ----------------------------------------------------------
Refer  Doc: 3GPP TS 24.008 [6].
         Section: 10.5.6.7
*/

typedef struct tran_id
{
  boolean valid;
  boolean ti_flag;
  byte length_of_transaction_id;
  byte val_of_transaction_id[2];
}gsm_umts_transaction_id_T;



#define MAX_FILTER_CONTENT_LENGTH 60

#endif







