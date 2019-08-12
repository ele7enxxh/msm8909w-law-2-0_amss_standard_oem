#ifndef __DS_EPS_MSG_H__
#define __DS_EPS_MSG_H__
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/10   vk      Initial version
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
#define DS_LTE_PHYS_LINK_FLOW_DISABLE_REQ_ID     0x01
#define DS_LTE_PHYS_LINK_FLOW_ENABLE_REQ_ID      0x02

/* Phys link flow disable msg
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /* message header */
  /* Message payload */
  uint32                              bearer_inst; /* bearer instance */
  uint32                              flow_mask;   /* flow mask */
} ds_eps_phys_link_flow_disable_msg_s;

/* Phys link flow enable msg
*/
typedef ds_eps_phys_link_flow_disable_msg_s 
        ds_eps_phys_link_flow_enable_msg_s;

/* UMID definitions
*/
enum
{
  MSGR_DEFINE_UMID(DS, LTE, REQ, PHYS_LINK_FLOW_DISABLE, 
                   DS_LTE_PHYS_LINK_FLOW_DISABLE_REQ_ID, msgr_hdr_s),
  MSGR_DEFINE_UMID(DS, LTE, REQ, PHYS_LINK_FLOW_ENABLE,
                   DS_LTE_PHYS_LINK_FLOW_ENABLE_REQ_ID, msgr_hdr_s),
};

#endif /* !__DS_EPS_MSG_H__ */
