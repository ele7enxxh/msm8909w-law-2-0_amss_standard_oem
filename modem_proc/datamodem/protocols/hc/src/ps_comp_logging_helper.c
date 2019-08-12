/*===========================================================================
             P S _ C O M P _ L O G G I N G _ H E L P E R . C

DESCRIPTION
  Implementation file for the IP Header/Data Compression Logging support.
  Primarily meant for providing DPL (Data Protocol Logging) capability for 
  header compression modules.   


Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/hc/src/ps_comp_logging_helper.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/20/14    vl     Changed the PS_COMP_LOGGING_HELPER_SIGNAL to 
                   PS_COMP_LOGGING_HELPER_CMD
11/06/13    ss     Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_comp_logging_helper.h"
#include "ps_svc.h"
#include "list.h"
#include "queue.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_logging.h"
#include "ps_system_heap.h"

/*===========================================================================

                        CONFIGURATION ELEMENTS

===========================================================================*/
#define PS_COMP_LOGGING_MAX_INSTANCE_COUNT 5

/* Random value used to validate instance memory passed in by user */
#define PS_COMP_LOGGING_INSTANCE_COOKIE ((uint32)0xBEC9D8F2)

/*===========================================================================

                           LOCAL DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  This structure contains all information for this comp logging helper 
  instance.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Link to maintain the list of comp logging instances. Has to be the 
     first element in the structure. */
  list_link_type list_link;

  /* Logging info passed in by the user during instance allocation */
  ps_comp_logging_instance_config_s config;

  /* Iface information required DPL logging */
  ps_iface_type *v4_iface_ptr;
  ps_iface_type *v6_iface_ptr;

  /* Will be populated with a predefined value while instance allocation.
     This field is meant to validate instance memory. */
  uint32 cookie;
}ps_comp_logging_instance_s;

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Linked list containing all the existing Comp Logging instances.
---------------------------------------------------------------------------*/
static list_type ps_comp_logging_instance_list;

/*---------------------------------------------------------------------------
  Queue holding packets to be logged via DPL.
  This is required for context switch from caller to PS task.
---------------------------------------------------------------------------*/
static q_type ps_comp_logging_log_pkt_q;

/*---------------------------------------------------------------------------
  Callback to fetch iface from mode handler
---------------------------------------------------------------------------*/
static ps_comp_logging_fetch_iface_cback_type 
         ps_comp_logging_fetch_iface_cback;

/*---------------------------------------------------------------------------
  Generic callback registered for context switch
---------------------------------------------------------------------------*/
static ps_comp_logging_generic_cback_type ps_comp_logging_generic_cback = NULL;

/*===========================================================================

                      PRIVATE UTILITY FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_COMP_LOGGING_FREE_PKT

DESCRIPTION
  Frees the packet specified by the packet info pointer.
  Will free the memory for the packet based on the packet details.
  Then frees the memory for this packet info.

DEPENDENCIES
  Valid packet info pointer must be specified.

PARAMETERS
  pkt_info_ptr : Packet to be freed.

RETURN VALUE
  0  : If packet successfully freed.
  -1 : If case of some error.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_comp_logging_free_pkt
(
  ps_comp_logging_pkt_info_s *pkt_info_ptr
)
{
/*-------------------------------------------------------------------------*/

  /* Free the appropriate internal pkt memory */
  if( pkt_info_ptr->pkt_content.dsm_item_ptr != NULL )
  {
    dsm_free_packet(&pkt_info_ptr->pkt_content.dsm_item_ptr);
  }

  if( pkt_info_ptr->pkt_content.raw_data.raw_data_ptr != NULL )
  {
    PS_SYSTEM_HEAP_MEM_FREE(
      pkt_info_ptr->pkt_content.raw_data.raw_data_ptr);
  }

  if( pkt_info_ptr->pkt_format != PS_COMP_LOGGING_PKT_FORMAT_DSM &&
      pkt_info_ptr->pkt_format != PS_COMP_LOGGING_PKT_FORMAT_RAW_DATA )
  {
    LOG_MSG_ERROR_1("Invalid pkt format %d", pkt_info_ptr->pkt_format);
  }

  /* Free the packet info pointer memory */
  PS_SYSTEM_HEAP_MEM_FREE(pkt_info_ptr);
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_FETCH_IFACE

DESCRIPTION
  Frees the packet specified by the packet info pointer.
  Will free the memory for the packet based on the packet details.
  Then frees the memory for this packet info.

DEPENDENCIES
  Valid packet info pointer must be specified.

PARAMETERS
  pkt_info_ptr : Packet to be freed.

RETURN VALUE
  0  : If packet successfully freed.
  -1 : If case of some error.

SIDE EFFECTS
  None.
===========================================================================*/
static ps_iface_type* ps_comp_logging_fetch_iface
(
  ps_comp_logging_instance_s *instance_ptr,
  ps_comp_logging_pkt_info_s *pkt_info_ptr
)
{
  ps_iface_type *ps_iface_ptr = NULL;
/*-------------------------------------------------------------------------*/

  /* Fetch based on version */
  if( pkt_info_ptr->pkt_ip_version == IP_V4 )
  {
    /* Check if v4 iface is populated in instance */
    if( instance_ptr->v4_iface_ptr == NULL )
    {
      /* Check if we have a callback registered */
      if( ps_comp_logging_fetch_iface_cback == NULL )
      {
        LOG_MSG_ERROR_0( "No fetch iface cback registered" );
        return NULL;
      }

      LOG_MSG_INFO1_1( "Fetching iface via cback 0x%x", 
                       ps_comp_logging_fetch_iface_cback );

      instance_ptr->v4_iface_ptr = 
        ps_comp_logging_fetch_iface_cback(
            &instance_ptr->config, IP_V4 );

      /* Validate iface */
      if( !PS_IFACE_IS_VALID(instance_ptr->v4_iface_ptr) )
      {
        LOG_MSG_ERROR_1("Invalid iface returned 0x%x", instance_ptr->v4_iface_ptr);
        instance_ptr->v4_iface_ptr = NULL;
        return NULL;
      }
    }

    ps_iface_ptr = instance_ptr->v4_iface_ptr;

  }
  else if( pkt_info_ptr->pkt_ip_version == IP_V6 )
  {
    /* Check if v6 iface is populated in instance */
    if( instance_ptr->v6_iface_ptr == NULL )
    {
      /* Check if we have a callback registered */
      if( ps_comp_logging_fetch_iface_cback == NULL )
      {
        LOG_MSG_ERROR_0( "No fetch iface cback registered" );
        return NULL;
      }

      LOG_MSG_INFO1_1( "Fetching iface via cback 0x%x",
                       ps_comp_logging_fetch_iface_cback );

      instance_ptr->v6_iface_ptr = 
        ps_comp_logging_fetch_iface_cback(
            &instance_ptr->config, IP_V6 );

      /* Validate iface */
      if( !PS_IFACE_IS_VALID(instance_ptr->v6_iface_ptr) )
      {
        LOG_MSG_ERROR_1("Invalid iface returned 0x%x", instance_ptr->v6_iface_ptr);
        instance_ptr->v6_iface_ptr = NULL;
        return NULL;
      }
    }

    ps_iface_ptr = instance_ptr->v6_iface_ptr;
  }
  else
  {
    LOG_MSG_ERROR_1( "Invalid IP version %d", pkt_info_ptr->pkt_ip_version );
    return NULL;
  }

  return ps_iface_ptr;
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_CMD_HDLR

DESCRIPTION
  Handler for the PS_COMP_LOGGING_CMD.
  This PS command is used to ensure that DPL logging happens in the PS task
  context.

DEPENDENCIES
  There must be DSM items present in the ps_comp_logging_watermark when this
  command handler is invoked.

PARAMETERS
  cmd_name : Command to be handled
  user_data_ptr : Pointer specified during command handler registration.
                  Would be the comp logging handle in our case.

RETURN VALUE
  TRUE  : In case of success processing of all packets in queue.
  FALSE : In case some packets left in the queue while exiting.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_comp_logging_cmd_hdlr
(
  ps_cmd_enum_type  cmd_name,
  void*             user_data_ptr
)
{
  ps_comp_logging_pkt_info_s *pkt_info_ptr = NULL;
  ps_comp_logging_instance_s *instance_ptr = NULL;
  ps_iface_type              *ps_iface_ptr = NULL;
/*-------------------------------------------------------------------------*/

  /* Validate the command */
  if( cmd_name != PS_COMP_LOGGING_HELPER_CMD )
  {
    LOG_MSG_ERROR_1("Invalid command %d", cmd_name);
    return;
  }

  /* Check if we need to invoke generic callback */
  if( ps_comp_logging_generic_cback != NULL )
  {
    //LOG_MSG_INFO2_1( "Invoking generic cback 0x%x", 
    //                 ps_comp_logging_generic_cback );
    ps_comp_logging_generic_cback();
    ps_comp_logging_generic_cback = NULL;
  }

  /* Now carry on with the DPL logging.
     Dequeue packet info pointer from the global queue */
  pkt_info_ptr = 
    (ps_comp_logging_pkt_info_s *)q_get(&ps_comp_logging_log_pkt_q);

  /* Fetch packets from the global queue and log via DPL */
  while( pkt_info_ptr != NULL )
  {
    /* Fetch and validate instance */
    instance_ptr = 
      (ps_comp_logging_instance_s *)pkt_info_ptr->instance_handle;
    if( instance_ptr == NULL || 
        instance_ptr->cookie != PS_COMP_LOGGING_INSTANCE_COOKIE )
    {
      LOG_MSG_ERROR_0("Invalid instance in pkt info ptr");
      ps_comp_logging_free_pkt(pkt_info_ptr);
      return;
    }

    /* Fetch iface pointer if not already present */
    if( (ps_iface_ptr = ps_comp_logging_fetch_iface(
                          instance_ptr, pkt_info_ptr)) == NULL )
    {
      LOG_MSG_ERROR_0("Invalid iface fetched.");
      ps_comp_logging_free_pkt(pkt_info_ptr);
      return;
    }

    /* Fetch DSM item to be logged based on the pkt format */
    if( pkt_info_ptr->pkt_format == PS_COMP_LOGGING_PKT_FORMAT_DSM )
    {
      /* Validate that dsm_item_ptr is NOT null */
      if( pkt_info_ptr->pkt_content.dsm_item_ptr == NULL )
      {
        LOG_MSG_ERROR_0( "ps_comp_logging_cmd_hdlr() NULL DSM Item" );
        ps_comp_logging_free_pkt(pkt_info_ptr);
        return;
      }
    }
    else if( pkt_info_ptr->pkt_format == PS_COMP_LOGGING_PKT_FORMAT_RAW_DATA )
    {
      /* Validate raw data is NOT null */
      if( pkt_info_ptr->pkt_content.raw_data.raw_data_ptr == NULL ||
          pkt_info_ptr->pkt_content.raw_data.raw_data_size == 0 )
      {
        LOG_MSG_ERROR_1( "Invalid raw data pointer, size %d", 
                         pkt_info_ptr->pkt_content.raw_data.raw_data_size );
        ps_comp_logging_free_pkt(pkt_info_ptr);
        return;
      }

      /* Validate dsm_item_ptr IS null */
      if( pkt_info_ptr->pkt_content.dsm_item_ptr != NULL )
      {
        LOG_MSG_ERROR_0( "ps_comp_logging_cmd_hdlr() non NULL DSM Item" );
        ps_comp_logging_free_pkt(pkt_info_ptr);
        return;
      }

      /* Create a new DSM item from the raw packet data specified */
      dsm_pushdown( &pkt_info_ptr->pkt_content.dsm_item_ptr, 
                    pkt_info_ptr->pkt_content.raw_data.raw_data_ptr,
                    (uint16)pkt_info_ptr->pkt_content.raw_data.raw_data_size,
                    DSM_DS_LARGE_ITEM_POOL );
    }
    else
    {
      LOG_MSG_ERROR_1("Invalid pkt format %d", pkt_info_ptr->pkt_format);
      ps_comp_logging_free_pkt(pkt_info_ptr);
      return;
    }

    /* Invoke DPL logging API */
    DPL_LOG_NETWORK_TX_PACKET( ps_iface_ptr, 
                               pkt_info_ptr->pkt_content.dsm_item_ptr, 
                               DPL_IID_NETPROT_IP );

    /* Free the packet */
    ps_comp_logging_free_pkt(pkt_info_ptr);

    /* Dequeue packet info pointer from the global queue */
    pkt_info_ptr = (ps_comp_logging_pkt_info_s *)
                     q_get(&ps_comp_logging_log_pkt_q);
  }
 
  return;
}

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_COMP_LOGGING_INIT

DESCRIPTION
  Initializes the comp logging module.
  Should be invoked in the PS task initialization function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
   None.
   
SIDE EFFECTS
  None.
===========================================================================*/
void ps_comp_logging_init( void )
{
/*-------------------------------------------------------------------------*/

  /* Initialize the packet queue */
  (void)q_init(&ps_comp_logging_log_pkt_q);

  /* Initialize the comp logging instance list */
  (void)list_init(&ps_comp_logging_instance_list);

  /* Register the PS CMD handler */
  (void) ps_set_cmd_handler(PS_COMP_LOGGING_HELPER_CMD,
                            ps_comp_logging_cmd_hdlr);
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_ALLOC

DESCRIPTION
  Creates an instance of the Comp Logging Helper.
  Returns the handle to the logging helper instance.

DEPENDENCIES
  None.

PARAMETERS
  info : The RAT for which this instance is being created and its 
         corresponding information.

RETURN VALUE
   NULL   If failed to create instance.
   Handle To the logging helper instance in case of success.

SIDE EFFECTS
  None.
===========================================================================*/
ps_comp_logging_handle_type ps_comp_logging_alloc
(
  ps_comp_logging_instance_config_s *config_ptr
)
{
  ps_comp_logging_instance_s *instance_ptr = NULL;
/*-------------------------------------------------------------------------*/
  /* Validation checks */
  if( list_size(&ps_comp_logging_instance_list) >= 
        PS_COMP_LOGGING_MAX_INSTANCE_COUNT )
  {
    LOG_MSG_ERROR_0("ps_comp_logging_alloc(): Max instances reached");
    return NULL;
  }
  if( config_ptr == NULL )
  {
    LOG_MSG_ERROR_0("ps_comp_logging_alloc(): NULL config");
    return NULL;
  }

  /* Allocate memory on modem heap for the new comp logging instance */
  instance_ptr = (ps_comp_logging_instance_s *) 
    ps_system_heap_mem_alloc(sizeof(ps_comp_logging_instance_s));

  if(instance_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_comp_logging_alloc(): Memory alloc failed");
    return NULL;
  }

  /* Memset allocated memory */
  memset(instance_ptr, 0, sizeof(ps_comp_logging_instance_s));

  /* Copy the specified configuration in the instance info */
  memscpy( &instance_ptr->config, sizeof(ps_comp_logging_instance_config_s),
           config_ptr, sizeof(ps_comp_logging_instance_config_s) );

  /* Add the newly created instance to the list */
  list_push_front(&ps_comp_logging_instance_list, &instance_ptr->list_link);

  /* Set cookie value in the instance.
     Would be used to validate instance when passed back by user */
  instance_ptr->cookie = PS_COMP_LOGGING_INSTANCE_COOKIE;

  LOG_MSG_INFO1_1( "Allocated Comp Logging Instance: 0x%x", instance_ptr );

  /* typecast instance pointer to handle and return to caller */
  return (ps_comp_logging_handle_type)instance_ptr;
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_FREE

DESCRIPTION
  Frees up the memory for the specified Logging instance.

DEPENDENCIES
  There must be an instance for the handle being passed in.

PARAMETERS
  handle : Handle provided to the client while creating logging instance.

RETURN VALUE
   0   if successful.
  -1   in case of failure.

SIDE EFFECTS
  None.
===========================================================================*/
int32 ps_comp_logging_free
(
  ps_comp_logging_handle_type handle
)
{
  ps_comp_logging_instance_s *instance_ptr = NULL;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_1("ps_comp_logging_free(): instance 0x%x", handle);

  /* Validation checks */
  if( (instance_ptr = (ps_comp_logging_instance_s *)handle) == NULL )
  {
    LOG_MSG_ERROR_0("NULL handle");
    return -1;
  }
  if( instance_ptr->cookie != PS_COMP_LOGGING_INSTANCE_COOKIE )
  {
    LOG_MSG_ERROR_0("Invalid instance cookie.");
    return -1;
  }

  /* Remove instance from global list */
  list_pop_item(&ps_comp_logging_instance_list, &instance_ptr->list_link);

  /* Free any other dynamic memory within instance structure.
     None right now. */

  /* Free the dynamic memory allocated for this instance */
  PS_SYSTEM_HEAP_MEM_FREE(instance_ptr);

  return 0;
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_REGISTER_FETCH_IFACE_CBACK

DESCRIPTION
  This API will 

DEPENDENCIES
  

PARAMETERS
  sys_sys_mode_e_type                    sys_mode,
  ps_comp_logging_fetch_iface_cback_type fetch_iface_cback

RETURN VALUE
   0   if successful.
  -1   in case of failure.

SIDE EFFECTS
  
===========================================================================*/
int32 ps_comp_logging_register_fetch_iface_cback
(
  sys_sys_mode_e_type sys_mode,
  ps_comp_logging_fetch_iface_cback_type fetch_iface_cback
)
{
/*-------------------------------------------------------------------------*/
 
  /* sys mode not used currently */
  (void)sys_mode;

  LOG_MSG_INFO1_0( "ps_comp_logging_register_fetch_iface_cback()" );

  if( fetch_iface_cback == NULL )
  {
    LOG_MSG_ERROR_0("NULL fetch iface cback");
    return -1;
  }

  /* Save the fetch iface callback */
  ps_comp_logging_fetch_iface_cback = fetch_iface_cback;

  return 0;
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_LOG_PKT

DESCRIPTION
  Logs the specified DSM item on to DPL(Data Protocol Logging) framework.
  The item passed in will be queued on to an internal DSM watermark.

DEPENDENCIES
  The handle must correspond to a valid comp logging instance.

PARAMETERS
  handle     : Comp Logging module handle.
  pkt_ptr    : Pointer to DSM item to be logged
  ip_version : IP version for the passed in packet.

RETURN VALUE
   0   if successful.
  -1   in case of failure.

SIDE EFFECTS
  
===========================================================================*/
int32 ps_comp_logging_log_pkt
(
  ps_comp_logging_pkt_info_s *pkt_info_ptr
)
{
  ps_comp_logging_instance_s *instance_ptr = NULL;
  ps_comp_logging_pkt_info_s *pkt_info_ptr_i = NULL;
/*-------------------------------------------------------------------------*/

  /* Validate input arguments */
  if( pkt_info_ptr == NULL )
  {
    LOG_MSG_ERROR_0("NULL pkt info ptr");
    return -1;
  }
  /* Validate instance */
  if((instance_ptr = 
       (ps_comp_logging_instance_s *)pkt_info_ptr->instance_handle) == NULL )
  {
    LOG_MSG_ERROR_0("NULL handle");
    return -1;
  }
  if( instance_ptr->cookie != PS_COMP_LOGGING_INSTANCE_COOKIE )
  {
    LOG_MSG_ERROR_1("Invalid instance cookie 0x%x", instance_ptr->cookie );
    return -1;
  }

  /* Allocate dynamic memory for queuing packet info */
  pkt_info_ptr_i = (ps_comp_logging_pkt_info_s *)
    ps_system_heap_mem_alloc(sizeof(ps_comp_logging_pkt_info_s));

  if( pkt_info_ptr_i == NULL )
  {
    LOG_MSG_ERROR_0("Out of memory");
    return -1;
  }
  
  /* Memset allocated memory */
  memset(pkt_info_ptr_i, 0, sizeof(ps_comp_logging_pkt_info_s));

  /* Copy contents from user passed memory */
  memscpy( pkt_info_ptr_i, sizeof(ps_comp_logging_pkt_info_s), 
           pkt_info_ptr, sizeof(ps_comp_logging_pkt_info_s) );

  /* Add packet info pointer to the log packet queue */
  q_put( &ps_comp_logging_log_pkt_q, 
         q_link(pkt_info_ptr_i, &pkt_info_ptr_i->queue_link) );

  /* Set cmd to switch context to PS task */
  ps_send_cmd(PS_COMP_LOGGING_HELPER_CMD, NULL);

  return 0;
}

/*===========================================================================
FUNCTION PS_COMP_LOGGING_INVOKE_GENERIC_CBACK

DESCRIPTION
  Invokes the registered generic callback in the task context in which PS 
  Comp Logging Helper module is operating.

DEPENDENCIES
  None.

PARAMETERS
  cback : Callback to be invoked in new task context.

RETURN VALUE
  None.
  
SIDE EFFECTS
 Sends the PS command to invoke the callback.
===========================================================================*/
void ps_comp_logging_invoke_generic_cback
(
  ps_comp_logging_generic_cback_type cback
)
{
/*-------------------------------------------------------------------------*/

  //LOG_MSG_INFO1_1( "ps_comp_logging_invoke_generic_cback(): set cback 0x%x",
  //                 cback );

  /* Save the callback */
  ps_comp_logging_generic_cback = cback;

  /* Set cmd to switch context to PS task*/
  ps_send_cmd(PS_COMP_LOGGING_HELPER_CMD, NULL);
}

