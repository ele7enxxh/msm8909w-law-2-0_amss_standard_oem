#ifndef _POLICYMAN_MSG_H_
#define _POLICYMAN_MSG_H_

/**
  @file policyman_msg.h

  @brief
*/
/*
    Copyright (c) 2013,2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/policyman_msg.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "msgr.h"

#define MSGR_MODULE_CFG       0x01
#define MSGR_POLICYMAN_CFG    MSGR_TECH_MODULE(MSGR_TECH_POLICYMAN, MSGR_MODULE_CFG)

enum
{
  MSGR_DEFINE_UMID(POLICYMAN, CFG, IND, UPDATE,      0x01, policyman_cfg_update_ind),
  MSGR_DEFINE_UMID(POLICYMAN, CFG, IND, UPDATE_MSIM, 0x02, policyman_cfg_update_msim_ind)
};


#endif /* _POLICYMAN_MSG_H_*/
