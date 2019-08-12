/*!
  @file
  esm_common.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/inc/esm_common.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/12/11   HC      Replacing the #include "sm.h" with #include "sm_v.h" 
                   as a part of CMI activity 
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/28/09   MNK     FIx warnings 
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/02/09   MNK     Added new fields to pdn_connection_data_T
05/21/09   MNK     Added new enum values
04/28/09   MNK     Changed esm_information to esm_information_transfer_bit
02/23/09   MNK     Added constant definitions for bearer id
02/05/09   MNK     Added definition for function pointer for ds_eps_get_pdn_connectivity_msg
==============================================================================*/

#ifndef ESM_COMMON_H
#define ESM_COMMON_H

#include "mmcp_variation.h"
#include<customer.h>
#ifdef  FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "cm_gw.h"
#include "sm_v.h"
#include "lte_nas_esm_message_description.h"
#include "lte_nas.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief External type for my clients
*/

/*typedef byte eps_bearer_id_T  ; */

/*
#define MAX_PACKET_FILTERS 8        

#define ESM_PD 0xa        
#define ESM_LEN_FOUR 0x4       
#define ESM_LEN_EIGHT 0x8       

#define ESM_MAX_BP_PROCS 11

#define ESM_INVALID_CON_ID 0xff
#define ESM_INVALID_PTI 0xff
#define ESM_INVALID_SDF_ID 0xffff
#define ESM_INVALID_BEARER_ID 0x0 
#define ESM_PTI_OFFSET 0x1
#define ESM_BEARER_ID_MASK 0xf0
#define ESM_BEARER_ID_OFFSET 0x0
#define ESM_MSG_TYPE_OFFSET 0x2
#define IEI_PROTOCOL_CFG_OPTIONS 0x27
#define IEI_ACC_POINT_NAME  0x28


typedef struct esm_hdr_struct
{
  byte              connection_id;  
  word            sdf_id;  
}esm_header_T;*/

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
/* ---------------------------------------------------------------------------
*  Signifies if the bearer context requests are a result of UE initiated request or NW autonoumously setting/modifying the context
   e.g default bearer context request or dedicated bearer context request or modify context request
* --------------------------------------------------------------------------*/
typedef enum
{
  ESM_NW_INITIATED   = 0x0,
  ESM_UE_INITIATED   = 0x1,
  ESM_NONE_INITIATED = 0x3
} req_direction_T;

#define  ESM_RESERVED_CONNECTION_ID  0xff
#define  MIN_NSAPI_NUM               5
#define  TFT_OP_DEL_EXISTING_TFT     2
#define  UNASSIGNED_BEARER_ID        0

typedef enum
{
  ESM_BC_ON_ATTACH_DEF_BEARER_PENDING,
  ESM_BC_ON_ATTACH_DEF_BEARER_FAILED,
  ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS,
  ESM_BC_ON_ATTACH_DEF_BEARER_NONE
} esm_bc_on_attach_def_bearer_state_T;

typedef struct esm_bearer_context_info_shared
{
  boolean   bearer_id_exist;
  boolean   default_bearer_ctxt;
#ifdef FEATURE_LTE_REL9
  boolean   emc_bc;
#endif
  byte      lbi;
}esm_bearer_context_info_shared_T;

typedef enum
{
  ESM_TIMER_INVALID,
  /* Timer for UE requested bearer resource allocation procedure */
  ESM_TIMER_T3480,
  /* Timer for UE requested bearer resource modification procedure */
  ESM_TIMER_T3481,
  /* Timer for UE requested PDN connectivity procedure */
  ESM_TIMER_T3482,
  /* Timer for UE requested PDN disconnect procedure */
  ESM_TIMER_T3492
}esm_timer_type; 

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#endif /* ESM_COMMON_H */


#endif /*FEATURE_LTE*/
