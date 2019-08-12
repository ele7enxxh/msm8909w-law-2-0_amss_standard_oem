/*===========================================================================
                     D S _ D S D _ E V E N T _ D P . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_event_dp.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "queue.h"
#include "data_msg.h"
#include "dstaski.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_ext_i.h"

static q_type ds_dsd_event_q = {{NULL}};
static q_type ds_dsd_event_free_q = {{NULL}};


typedef struct  
{
  q_link_type                   link; 
  dsd_event_client_type         dsd_event_client_id;
  ds_dsd_event_notify_func_type dsd_event_notify_func_ptr;
  dsd_radio_type                radio;
}ds_dsd_event_data_block;


ds_dsd_event_data_block ds_dsd_event_client[DS_DSD_CLIENT_MAX] = {{{NULL}}};

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      ds_dsd_event_dp_compare_function

DESCRIPTION   Utility function to compare two items.  

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal 
              0: if itemsa are not equal
 
SIDE EFFECTS  None.
===========================================================================*/
int ds_dsd_event_dp_compare_function
(
  void *item_ptr, 
  void *target_val
)
{
  ds_dsd_event_data_block *data_block;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((item_ptr == NULL) ||(target_val == NULL))
  {
    return FALSE;
  }
  data_block = (ds_dsd_event_data_block *)item_ptr;

  if(data_block->dsd_event_client_id ==*((dsd_event_client_type *)(target_val)))
  { 
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* ds_dsd_event_dp_compare_function() */

/*===========================================================================
FUNCTION      DS_DSD_EVENT_DP_DISPATCH

DESCRIPTION   This function dispatches DSD events to its statemachines

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Dispatch event to registered clients
===========================================================================*/
void ds_dsd_event_dp_dispatch
(
  dsd_event_type              event_input,
  dsd_radio_type              source_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info
)
{
  uint8                     q_count = 0;
  uint8                     iteration_num;
  ds_dsd_event_data_block  *event_block_temp_hold = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG3(MSG_LEGACY_LOW,
            "ds_dsd_event_dp_dispatch radio:%d event:%d sub id:%d",
            source_radio, event_input, subs_id);

  for(iteration_num = 0; iteration_num<2; iteration_num++)
  {
    q_count = q_cnt(&ds_dsd_event_q);
    while(q_count--)
    {
      event_block_temp_hold = (ds_dsd_event_data_block *)q_get(&ds_dsd_event_q);
      if(event_block_temp_hold)
      {
        switch(event_input)
        {
          case DSD_OOS_RADIO_EV:
            if(iteration_num == 0)
            {
              if(event_block_temp_hold->radio == source_radio)
              {
                event_block_temp_hold->
                  dsd_event_notify_func_ptr(
                     event_input,
                     event_block_temp_hold->dsd_event_client_id,
                     source_radio,
                     subs_id,
                     aux_info);
              }
            }
            else 
            {
              if(event_block_temp_hold->radio != source_radio)
              {
                event_block_temp_hold->
                  dsd_event_notify_func_ptr(
                     event_input,
                     event_block_temp_hold->dsd_event_client_id,
                     source_radio,
                     subs_id,
                     aux_info);
              }
            }
            break;

          default:
            if(iteration_num == 0)
            {
              if(event_block_temp_hold->radio != source_radio)
              {
                event_block_temp_hold->
                  dsd_event_notify_func_ptr(
                     event_input,
                     event_block_temp_hold->dsd_event_client_id,
                     source_radio,
                     subs_id,
                     aux_info);
              }
            }
            else
            {
              if(event_block_temp_hold->radio == source_radio)
              {
                event_block_temp_hold->
                  dsd_event_notify_func_ptr(
                     event_input,
                     event_block_temp_hold->dsd_event_client_id,
                     source_radio,
                     subs_id,
                     aux_info);
              }
            }
            break;
        }
        q_put(&ds_dsd_event_q, &(event_block_temp_hold->link));
      }
      else
      {
        ASSERT(0); 
      }
    }
  }
}/* ds_dsd_event_dp_dispatch() */

/*===========================================================================

                      PUBLIC FUNCTIONS DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_EVENT_DP_REG_CLIENT

DESCRIPTION   Called by clients to register to get event notification.  

DEPENDENCIES  None.

RETURN VALUE  TRUE: if registration successful 
                    if client previously registgered
 
              FALSE: if registration unsuccessful
 
SIDE EFFECTS  Registered client is entered on a queue
===========================================================================*/
boolean ds_dsd_event_dp_reg_client
(
  dsd_event_client_type         dsd_client, 
  ds_dsd_event_notify_func_type client_notify_func,
  dsd_radio_type                radio
)
{
  boolean                   ret_val = FALSE;
  ds_dsd_event_data_block * event_block_to_insert = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(q_linear_search(&ds_dsd_event_q, ds_dsd_event_dp_compare_function, 
                     (void *)&dsd_client) != NULL)
  {
    DATA_MSG1(MSG_LEGACY_MED,"Client %d already registered", dsd_client); 
    ret_val = TRUE;
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_HIGH,"Registering client %d", dsd_client); 
    event_block_to_insert = 
      (ds_dsd_event_data_block *) q_get(&ds_dsd_event_free_q);
    if(event_block_to_insert != NULL)
    {
      event_block_to_insert->dsd_event_client_id = dsd_client;
      event_block_to_insert->dsd_event_notify_func_ptr = client_notify_func;
      event_block_to_insert->radio   = radio;
      q_put(&ds_dsd_event_q, &(event_block_to_insert->link));
      ret_val = TRUE;
    }
    else
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"No Free event buffer to register client");
      ret_val = FALSE;
    }
  }
  return ret_val;
}/* ds_dsd_event_dp_reg_client() */

/*===========================================================================
FUNCTION      DS_DSD_EVENT_DP_DEREG_CLIENT

DESCRIPTION   Called by clients to de-register for event notification.  

DEPENDENCIES  None.

RETURN VALUE  TRUE: if de-registration successful 
                    if client previously de-registgered
 
              FALSE: if de-registration unsuccessful
 
SIDE EFFECTS  De-registered client is dequeued 
===========================================================================*/
boolean ds_dsd_event_dp_dereg_client
(
  dsd_event_client_type dsd_client
)
{
  boolean ret_val = FALSE;
  ds_dsd_event_data_block * event_block_deleted = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_MED,"ds_dsd_event_dp_dereg_client"); 

  event_block_deleted = 
    q_linear_delete_new(&ds_dsd_event_q,ds_dsd_event_dp_compare_function, 
                        (void *)&dsd_client, NULL, NULL );
  if(event_block_deleted != NULL)
  {
    DATA_MSG1(MSG_LEGACY_HIGH,"De-registered %d", dsd_client);
    event_block_deleted->dsd_event_client_id = DS_DSD_CLIENT_INVALID;
    event_block_deleted->dsd_event_notify_func_ptr = NULL;
    q_put(&ds_dsd_event_free_q,&(event_block_deleted->link));
    ret_val = TRUE;
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"Could not de-reg client %d", dsd_client);
    ret_val = FALSE;
  }

  return ret_val;
}
/*===========================================================================
FUNCTION      DS_DSD_EVENT_DP_INIT

DESCRIPTION   Called at initialization.  

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Queue is defined and initialized.
===========================================================================*/
void ds_dsd_event_dp_init(void)
{
  int i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_init(&ds_dsd_event_q);
  q_init(&ds_dsd_event_free_q);
  for(i = 0; i<DS_DSD_CLIENT_MAX; i++)
  {
    memset(&ds_dsd_event_client[i],0, sizeof(ds_dsd_event_data_block));
    ds_dsd_event_client[i].dsd_event_client_id = DS_DSD_CLIENT_INVALID;
    ds_dsd_event_client[i].dsd_event_notify_func_ptr = NULL;

    (void)q_link((void *)&ds_dsd_event_client[i], &ds_dsd_event_client[i].link);
    q_put(&ds_dsd_event_free_q, &ds_dsd_event_client[i].link);
  }
  return;
} /* ds_dsd_event_dp_init() */

/*===========================================================================
FUNCTION      DS_DSD_EVENT_DP_INPUT

DESCRIPTION   This function gets events from: 
                    - Timers
                    - Registered callbacks (e.g. CM, Mode handlers, HDR CP etc)

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Dispatch event to registered clients
===========================================================================*/
void ds_dsd_event_dp_input
(
  dsd_event_type              event_input,
  dsd_radio_type              source_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG3(MSG_LEGACY_LOW,"ds_dsd_event_dp_input radio:%d event:%d sub id:%d",
            source_radio,
            event_input,
            subs_id);

  
  /*------------------------------------------------------------------------
    Make sure that this is running in DS task context. This function is the 
    entry function and should provide alternative if running thread is not 
    in DS task.
  ------------------------------------------------------------------------*/
  ASSERT(IS_IN_DS_TASK());
  if((event_input == DS_DSD_INVALID_EVENT) ||
     (source_radio > DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid event:%d", event_input);
    return;
  }

  /* If radio SM is not active, create a new radio SM instance */
  if(ds_dsd_proc_is_radio_sm_active(subs_id, source_radio) == FALSE)
  {
    if(ds_dsd_proc_create_radio_sm_instance(subs_id, source_radio) == FALSE)
    {
      DATA_MSG0(MSG_LEGACY_ERROR, "Create radio SM failed. return.");
      return;
    }
  }

  ds_dsd_event_dp_dispatch(event_input, source_radio, subs_id, aux_info);

  return;
}/* ds_dsd_event_dp_input()*/

