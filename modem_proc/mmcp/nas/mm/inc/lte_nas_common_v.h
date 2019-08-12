/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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


/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_common_v.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $


when         who                          what                              
--------     --------  ------------------------------------------------ 
07/10/10     zren      Updated nas_tech_id enum with NAS_1XCP
12/10/09     zren      Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
10/13/09     zren      Enabled FEATURE_NAS_PDN_CONNECTIVITY_REQ_RSP
09/16/09     zren      Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/14/09     MNK       Remove Bearer resource release and add modification messages
07/29/09     hnam      Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                        used the standard macro commonly used by LTE_MODEM_SW
07/07/09     zren      Moved define FEATURE_RELEASE_LTE_MARCH_SPEC from 
                       msg_lib_encode_emm.c
04/25/09     hnam      Removed Length parameter in EPS_BEARER_CONTEXT_STATUS
04/24/09     MNK       Moved lte_nas_eps_bearer_context_status_type from emm_message_description.
04/06/09     MNK       Added definitions for TRANSACTION_ID
03/25/09     hnam      Changed emm cause "LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE" from 100 to 101
02/27/09     MNK       Added emm and esm cause values and valid flag in the header
02/03/09     hnam      Removed MSG_LOW, MSG_MED, MSG_HIGH, ERR
==============================================================================*/
#ifndef _LTE_NAS_COMMON_V_H_
#define _LTE_NAS_COMMON_V_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#include "lte_nas_common.h"
#include "comdef.h"
#include "cm_gw.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#if defined (TEST_FRAMEWORK) || defined (TF_UNIT_TEST)
#error code not present
#endif

#endif



