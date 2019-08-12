#ifndef _POLICYMAN_MSGR_I_H_
#define _POLICYMAN_MSGR_I_H_

/**
  @file policyman_msgr_i.h

  @brief Internal interface to Policy Manager Message router functionality
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_msgr_i.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_task.h"

#include "msgr.h"
#include "msgr_umid.h"


/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/



typedef void (*policyman_msgr_cb_fn_t) (msgr_hdr_s *pMsg, void *pContext);


/*-------- policyman_msgr_register --------*/
/**
@brief  Register for a particular message UMID.

@param

@return

*/
void policyman_msgr_register(
  msgr_umid_type           umid,
  policyman_msgr_cb_fn_t   pfnCallback,
  void                    *pContext
);


/*-------- policyman_msgr_init --------*/
/**
@brief  Initialize MSGR client support in policyman.

@param

@return

*/
void policyman_msgr_init(
  policyman_signal_t   sig,
  policyman_tcb_t     *pTCB
);


/*-------- policyman_process_msgr --------*/
/**
@brief  Process any messages that have been received.

@param

@return

*/
void policyman_process_msgr(
  void
);



#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

#endif /* _POLICYMAN_MSGR_I_H_ */
