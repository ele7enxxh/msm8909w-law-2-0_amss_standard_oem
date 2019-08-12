/*=========================================================================

                        E C A L L _ M O D E M _ C L I E N T S . C

DESCRIPTION
    This supports the multiple clients to get notification for ivs status
    through registered callback
  
REFERENCES
    none
 
EXTERNALIZED FUNCTIONS
   -ecall_emodem_client_init() 
   -ecall_emodem_client_register()
   -ecall_emodem_client_deregister()

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*=========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_modem_clients.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------
06/25/10    sar     Supports multiple clients for IVS

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_ECALL_IVS

#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "ecall_modem_clients.h"
#include "ecall_ivs_task.h"
#include "ecall_efs.h"



/* Max clients 2^15-1 = 32767 */
#define ECALL_EMODEM_CLIENTS_MAX  32767

static q_type ecall_client_q;
static int16 ecall_modem_client_id = 0;


/*=========================================================================
                eCall modem clients internal Functions
===========================================================================*/

void ecall_emodem_clients_initialise(void)
{
  (void) q_init(&ecall_client_q);
}

/*=========================================================================
FUNCTION     ECALL_PROCESS_CLIENT_REGISTER

DESCRIPTION
    registers the client callback function to get ivs notifications
    overwrites if the client is already registered

DEPENDENCIES
    Need to process the ecall_emodem_client_init before calling register

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_register
(
  ecall_client_type  *client_ptr
)
{

  /* add the new memory location to the end of queue */
  (void) q_link(client_ptr, &client_ptr->link);
  q_put(&ecall_client_q, &client_ptr->link);

  ECALL_MSG_1( ECALL_HIGH, "Registered client id %d",
                            client_ptr->clientid );

}

/*=========================================================================
FUNCTION    ECALL_PROCESS_CLIENT_NOTIFY

DESCRIPTION
       notifies all the registered clients with current ivs status

DEPENDENCIES
  None

RETURN VALUE
  tx_status and user_data for every registered client

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_notify
(
  ecall_cb_data_type ivs_status
)
{
  ecall_client_type *client_ptr = NULL;
  ecall_cb_func_type    cb_func;
  uint32                q_count = 0;
  
  /* returns the head of the queue*/
  client_ptr =  q_check(&ecall_client_q);
  q_count    =  q_cnt(&ecall_client_q);

  /* notify all the registered clients*/
  while( (client_ptr != NULL) && (q_count > 0) && (q_count < ECALL_EMODEM_CLIENTS_MAX) )
  {      
    if((cb_func = client_ptr->cb_func)!= NULL)
    {
      cb_func(client_ptr->user_data, ivs_status);
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "could not notify clientid  %d ",
                                 client_ptr->clientid );
    }
    client_ptr = q_next(&ecall_client_q, &client_ptr->link);
    q_count--;
  }
}


/*=========================================================================
FUNCTION     ECALL_PROCESS_CLIENT_DEREGISTER

DESCRIPTION
     deregisters the client, cannot get any further notifications from ivs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ecall_process_client_deregister
(
  int16  clientid
)
{

  ecall_client_type *client_ptr = NULL;
  uint32             q_count = 0;
  
  /* returns the pointer to the head of the queue*/
  client_ptr =  q_check(&ecall_client_q);
  q_count    =  q_cnt(&ecall_client_q);
 
  ECALL_MSG_1( ECALL_MED, "emodem clients before deregistration%d",
                           q_cnt(&ecall_client_q) );

  /* lookup for the clientid in the queue*/
  while( (client_ptr != NULL) && (q_count > 0) && (q_count < ECALL_EMODEM_CLIENTS_MAX) )
  {
    if(clientid == client_ptr->clientid )
    {
      ECALL_MSG_1( ECALL_HIGH, "found clientid %d free it",
                                clientid );
      q_delete(&ecall_client_q, &client_ptr->link);
      break;
    }
    client_ptr = q_next(&ecall_client_q, &client_ptr->link);
    q_count--;
  }

  ECALL_MSG_1( ECALL_HIGH, "emodem clients after deregistration %d ",
                            q_cnt(&ecall_client_q) );
}

/*=========================================================================
FUNCTION     ECALL_CLIENT_ID_EXISTS

DESCRIPTION
   scans the complete queue to lookup for a clientid 
 
DEPENDENCIES
  Queue need to be initialised

RETURN VALUE
  TRUE if it finds a clientid else returns false

SIDE EFFECTS
  None
===========================================================================*/
boolean ecall_client_id_exists
(
  int16   clientid
)
{
  ecall_client_type *client_ptr = NULL;
  boolean ret_value = FALSE;
  uint32  q_count = 0;

  /* returns the pointer to the head of the queue*/
  client_ptr =  q_check(&ecall_client_q);
  q_count    =  q_cnt(&ecall_client_q);

  /* scan through the queue to lookup clientid*/
  while( (client_ptr !=  NULL) && (q_count > 0) && (q_count < ECALL_EMODEM_CLIENTS_MAX) )
  {
    if(clientid == client_ptr->clientid)
    {
      ECALL_MSG_1( ECALL_HIGH, "Client id already exists %d",
                                clientid );
      ret_value = TRUE;
      break;
    }
    client_ptr = q_next(&ecall_client_q, &client_ptr->link);
    q_count--;
  }
 
  return ret_value;
}


/*=========================================================================
                eCall modem clients External Functions
===========================================================================*/

/*=========================================================================
FUNCTION      ECALL_EMODEM_CLIENT_INIT

DESCRIPTION
  The client would get the client id on initialization,
  which needs to be used during registraion 

DEPENDENCIES
  None

RETURN VALUE 
  ECALL_EMODEM_CLIENT_OK in case of any free slots available
  ECALL_EMODEM_CLIENT_ERR in case of failure with cliet id -1

SIDE EFFECTS
  None
===========================================================================*/
ecall_emodem_client_status_type ecall_emodem_client_init
(
  int16   *client_id_ptr
)
{
  static int16 client_id_count;

  if(client_id_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "client_id_ptr is NULL" );
    return ECALL_EMODEM_CLIENT_ERR;
  }

  client_id_count = 0;

  do
  {
    ecall_modem_client_id = (ecall_modem_client_id + 1)%ECALL_EMODEM_CLIENTS_MAX; 
    client_id_count++;

    /*if the client id does not exist in the queue, it will exit the loop */
  }while( client_id_count < ECALL_EMODEM_CLIENTS_MAX  &&
          ecall_client_id_exists(ecall_modem_client_id) != FALSE);
  
  /*if client_id_count = ECALL_EMODEM_CLIENTS_MAX then it implies 
    we could not get client id */
  if(client_id_count < ECALL_EMODEM_CLIENTS_MAX)
  {
    *client_id_ptr = ecall_modem_client_id;
    return ECALL_EMODEM_CLIENT_OK;  
  }
  else
  { 
    ECALL_MSG_1( ECALL_ERROR, "could not get free client after %d searches",
                               client_id_count );
    return ECALL_EMODEM_CLIENT_ERR;
  }
}


/*=========================================================================
FUNCTION     ECALL_EMODEM_CLIENT_REGISTER

DESCRIPTION
   posts a command to register the client for ivs notifications.
   if we two clients tries to register at same time, we might crash
   so we post a command than doing inline.
   
 
DEPENDENCIES
  ecall_emodem_client_init needs to be called before calling register

RETURN VALUE
  on success returns ECALL_EMODEM_CLIENT_OK
  on failure returns ECALL_EMODEM_CLIENT_ERR

SIDE EFFECTS
  None
===========================================================================*/
ecall_emodem_client_status_type ecall_emodem_client_register
(
  ecall_client_type    *client_ptr
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  /* check if the callback function is NULL*/
  if( client_ptr  == NULL  ||
      client_ptr->cb_func == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "client_ptr or call back function is NULL" );
    return ECALL_EMODEM_CLIENT_ERR;
  }

  /* get a free command buffer */
  cmd_ptr = ecall_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to register the client */
    cmd_ptr->cmd_id           = ECALL_CMD_CLIENT_REG;
    cmd_ptr->data.client_ptr  = client_ptr;

    ecall_put_cmd(cmd_ptr);

    return ECALL_EMODEM_CLIENT_OK;
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to register call back" );
    return ECALL_EMODEM_CLIENT_ERR;
  }
}


/*=========================================================================
FUNCTION     ECALL_EMODEM_CLIENT_DEREGISTER

DESCRIPTION
   de-registers the client. It cannot get further notification from IVS.
   if we two clients tries to deregister at same time, we might crash
   so we post a command than doing inline.

DEPENDENCIES
  None

RETURN VALUE
  on success returns ECALL_EMODEM_CLIENT_OK
  on failure returns ECALL_EMODEM_CLIENT_ERR

SIDE EFFECTS
  None
===========================================================================*/
ecall_emodem_client_status_type ecall_emodem_client_deregister
(
  int16 client_id
)
{

  ecall_cmd_type *cmd_ptr = NULL;

  /* get a free command buffer */
  cmd_ptr = ecall_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to register the client */
    cmd_ptr->cmd_id         = ECALL_CMD_CLIENT_DEREG;
    cmd_ptr->data.clientid  = client_id;
    
    ecall_put_cmd(cmd_ptr);

    return ECALL_EMODEM_CLIENT_OK;
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to deregister " );
    return ECALL_EMODEM_CLIENT_ERR;
  }
}

#else

void ecall_modem_clients_dummy_func(void);

#endif /* End Featurisation */

