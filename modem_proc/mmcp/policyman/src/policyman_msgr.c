/**
  @file policyman.c

  @brief Public interface to Policy Manager Message router functions
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_msgr.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_msgr_i.h"
#include "policyman_msg.h"
#include "policyman_task.h"
#include "policyman_dbg.h"
#include "policyman_util.h"
#include "queue.h"
#include <stringl/stringl.h>

#include "msgr_rex.h"


typedef struct
{
 msgr_umid_type           umid;
 policyman_msgr_cb_fn_t   pfnCallback;
 void                    *pContext;
} policyman_msgr_reg_entry_t;


#define POLICYMAN_MSGR_BUF_CNT 5

/*  Registered messages
 */
static  policyman_msgr_reg_entry_t  msgr_set[POLICYMAN_MSGR_BUF_CNT];

/*  The policyman MSGR client
 */
static  msgr_client_t policyman_msgr_client;

/*  The MSGR queue.
 */
static  q_type        policyman_msgr_q;


#define POLICYMAN_OFFSETOF(type,member) (((char*)(&((type*)1)->member))-((char*)1))



typedef struct
{
  q_link_type link;           /* For linking onto msgr queue         */

  msgr_hdr_s  hdr;            /*  Message router header */
} policyman_msgrq_entry_t;




/*-------- policyman_msgr_set_find --------*/
/**
@brief  Find a registration entry in the set of registrations.

@param

@return

*/
static policyman_msgr_reg_entry_t * policyman_msgr_set_find(
  msgr_umid_type  umid
)
{
  policyman_msgr_reg_entry_t  *ptr = NULL;
  size_t                       i;

  // search in msgr_set for the matching UMID and non-Null callback function
  for (i = 0 ; i < POLICYMAN_MSGR_BUF_CNT ; ++i)
  {
    if (msgr_set[i].umid == umid && msgr_set[i].pfnCallback != NULL)
    {
      ptr = &msgr_set[i];
      break;
    }
  }

  return ptr;
}


/*-------- policyman_msgr_set_add --------*/
static boolean policyman_msgr_set_add(
  msgr_umid_type          umid,
  policyman_msgr_cb_fn_t  pfnCallback,
  void                   *data
)
{
  size_t i;
  size_t found = 0;

  // parse the msgr_set to get any empty location
  for (i = 0; i < POLICYMAN_MSGR_BUF_CNT; i++)
  {
    if (msgr_set[i].umid == 0 && msgr_set[i].pfnCallback == NULL && msgr_set[i].pContext == NULL)
    {
      found++;
      break;
    }
  }

  // add an element to set at the index where empty space is found
  if ( i < POLICYMAN_MSGR_BUF_CNT && found > 0 )
  {
    msgr_set[i].umid        = umid;
    msgr_set[i].pfnCallback = pfnCallback;
    msgr_set[i].pContext    = data;
  }
  else
  {
    POLICYMAN_MSG_ERROR_2( "item not added to set, index %d, found %d",
                           i,
                           found );
  }

  return (found > 0 ? TRUE : FALSE);
}



/*-------- policyman_msgr_register --------*/
void policyman_msgr_register(
  msgr_umid_type           umid,
  policyman_msgr_cb_fn_t   pfnCallback,
  void                    *pContext
)
{
  errno_enum_type err = E_FAILURE;

  if (policyman_msgr_set_find(umid) == NULL)
  {
    err = msgr_register(MSGR_POLICYMAN_CFG, &policyman_msgr_client, MSGR_ID_REX, umid);
  }

  if (err == E_SUCCESS)
  {
    policyman_msgr_set_add(umid, pfnCallback, pContext);
  }
  else
  {
    POLICYMAN_MSG_ERROR_2( "Unable to register UMID 0x%x with MSGR, err %d",
                           umid,
                           err );
  }
}

/*-------- policyman_msgr_set_init --------*/
static void policyman_msgr_set_init(
  void
)
{
  size_t i = 0;

  // search in msgr_set for the matching UMID and non-Null callback function
  for (i = 0; i < POLICYMAN_MSGR_BUF_CNT; i++)
  {
    msgr_set[i].umid = 0;
    msgr_set[i].pfnCallback = NULL;
    msgr_set[i].pContext  = NULL;
  }
}


/*-------- policyman_msgr_init --------*/
void policyman_msgr_init(
  policyman_signal_t   sig,
  policyman_tcb_t     *pTCB
)
{
  errno_enum_type msgr_err;

  /*  Initialize the message router queue
   */
  (void) q_init( &policyman_msgr_q );

  /*  Create a message router client
   */
  msgr_err = msgr_client_create( &policyman_msgr_client );
  POLICYMAN_MSG_HIGH_1("msgr_client_create returned %d", msgr_err );
  if (msgr_err != E_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_0("Unable to create MSGR client");
    return;
  }

  msgr_err = msgr_client_set_rex_q( &policyman_msgr_client,
                                    (rex_tcb_type *) pTCB,
                                    (rex_sigs_type) sig,
                                    &policyman_msgr_q,
                                    MSGR_NO_QUEUE,
                                    POLICYMAN_OFFSETOF(policyman_msgrq_entry_t, hdr),
                                    MSGR_NO_OFFSET,
                                    0xffff );

  POLICYMAN_MSG_HIGH_1("msgr_client_set_rex_q returned %d", msgr_err);

  policyman_msgr_set_init();
}


/*-------- policyman_process_msgr --------*/
void policyman_process_msgr(
  void
)
{
  policyman_msgrq_entry_t  *pEntry;

  POLICYMAN_MSG_HIGH_0("Processing MSGR queue");

  while ((pEntry = q_get(&policyman_msgr_q)) != NULL)
  {
    msgr_hdr_s                  *pMsg = &pEntry->hdr;
    policyman_msgr_reg_entry_t  *pRegistration;

    pRegistration = policyman_msgr_set_find(pMsg->id);
    if (pRegistration != NULL)
    {
      POLICYMAN_MSG_HIGH_1("msg id %d found in set, calling callback function", pMsg->id);
      pRegistration->pfnCallback(pMsg, pRegistration->pContext);
    }

    msgr_rex_free_msg_buf(&(pEntry->link));
  }
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */
