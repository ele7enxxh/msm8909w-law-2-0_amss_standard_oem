/*!
  @file lte_rrc_diagi.h

  @brief
  Macros and defines for RRC specific diagnostic messages

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

#ifndef LTE_RRC_DIAGI_H
#define LTE_RRC_DIAGI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msg.h>
#include <diag.h>
#include "lte_variation.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* The command id for the Diag Command. */
#define LTE_RRC_DIAG_MIN_CMD_CODE 0x8001 
#define LTE_RRC_DIAG_MAX_CMD_CODE  0x8fff
#define LTE_RRC_GET_CTRL_STATE_CMD 0x8001

typedef PACKED struct   
{
  boolean       place_holder;
} lte_rrc_diag_req_payload_s;

typedef PACKED struct
{
    /*Whether the contents of the payload are valid or not*/
    boolean     valid;
    /* RRC Controller State*/
    uint8      ctrl_stm_state;

} lte_rrc_diag_rsp_payload_s;

/*Structure to receive the message from the DIAG module */
typedef PACKED struct 
{
  diagpkt_subsys_header_type      header;
  lte_rrc_diag_req_payload_s      diag_req_payload;     /* Union of all the Request types expected from the diag */
} lte_rrc_diag_req_s;

/*Structure to send the message to the DIAG module */
typedef PACKED struct 
{
  diagpkt_subsys_header_type    header;
  lte_rrc_diag_rsp_payload_s    diag_rsp_payload;            /* Union of all the Response types expected to the diag */
} lte_rrc_diag_rsp_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern PACKED void* lte_rrc_diag_cmd_dispatch
( 
  PACKED void* diag_msg_ptr, 
  uint16 diag_msg_len 
);

extern void lte_rrc_diag_reg_init(void);

#endif /* __LTE_RRC_DIAG_H__ */
