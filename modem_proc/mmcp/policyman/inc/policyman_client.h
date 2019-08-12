#ifndef _POLICYMAN_CLIENT_H_
#define _POLICYMAN_CLIENT_H_

/**
  @file policyman_client.h

  @brief Handling of Policy Manager clients.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_client.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "msgr_types.h"
#include "ref_cnt_obj.h"


#define MAX_ITEM_REGISTRATIONS    32

typedef struct
{
  _REF_CNT_OBJ;

  msgr_client_t *pMsgrClient;
} policyman_client;


typedef struct
{
  _REF_CNT_OBJ;

  size_t  index;
} policyman_client_iterator;


/*-------- policyman_client_init --------*/
/**
@brief  Initialize the PM client subsystem.  Private function for use of
        PM task code.

@return
  None.
*/
void policyman_client_init(
  void
);

/*-------- policyman_client_deinit --------*/
/**
@brief  Deinitialize the PM client subsystem.  Private function for use of
        PM task code.

@return
  None.
*/
void policyman_client_deinit(
  void
);


/*-------- policyman_client_new --------*/
/**
@brief  Create a new client and add it to the client array.

@param[in]  cb  Pointer to callback function for notifications of
                configuration changes.

@return
  - ID of the new client if successful
  - 0 if unsuccessful
*/
policyman_status_t policyman_client_new(
  msgr_client_t *pMsgrClient
);


/*-------- policyman_client_release --------*/
/**
@brief  Release a client and remove it from the client array.

@param[in]  id    ID of client to release.

@return
  None
*/
void policyman_client_release(
  msgr_client_t *pMsgrClient
);


/*-------- policyman_client_get_client --------*/
/**
@brief  Get a pointer to a given client.

Returns a pointer to the client with a given ID.  The client referenced by the
pointer has had a reference added to it, so when the caller of this function is
done with the client it should call ref_cnt_obj_release() on it.

@param[in]  id    ID of client desired.

@return
  - Pointer to the desired client if it is present.
  - NULL if the desired client is not present.
*/
policyman_client * policyman_client_get_client(
  msgr_client_t *pMsgrClient
);



/*-------- policyman_client_iterator_new --------*/
policyman_client_iterator * policyman_client_iterator_new(
  void
);


/*-------- policyman_client_iterator_next_client --------*/
policyman_client * policyman_client_iterator_next_client(
  policyman_client_iterator *pIter
);

/*-------- policyman_client_notify --------*/
void policyman_client_notify(
  policyman_client  *pClient,
  msgr_hdr_s        *pMsg,
  size_t             size
);

#endif /* _POLICYMAN_CLIENT_H_ */
