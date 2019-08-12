/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/ipcns.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "msg.h"
#include "rex.h"

#include "ipcrex.h"
#include "ipctypes.h"
#include "ipcns.h"
#include "ipcdefi.h"

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/

#define IPC_ECODE_EOK         0x0
#define IPC_ECODE_EXISTS      0x1
#define IPC_ECODE_EALREADY    0x2
#define IPC_ECODE_ERANGE      0x3
#define IPC_ECODE_EFULL       0x4

static ipc_node_id_type ipcns_generate_node_id
(
   void
);

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/
typedef struct
{
   void* node_ptr;
} ipc_node_type;

static ipc_node_type ipc_node_table[ IPC_NODE_MAX_ID + 1 ];
static uint16        ipc_node_id_generator = IPC_NODE_FIRST_DYNAMIC_ID;
rex_crit_sect_type   ipcns_table_crit_sect;

/*---------------------------------------------------------------------------
  External function definition
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION IPC_NODE_REGISTER

DESCRIPTION
 
PARAMETERS
  thread_ptr:  the thread to register

RETURN VALUE
  TRUE  - Registration was successful
  FALSE - Registration failed
 
SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
boolean ipcns_node_register
(
   void*    tcb_ptr
)
{
   rex_ipc_info_type* info_ptr = IPC_NODE_INFO(tcb_ptr);
   ipc_node_id_type   node_id;
   int err_code;

   /* check if the task is already registered */
   /* this handles cases where ipcns_node_register() is 
      called multiple time for the same task */
   if( IPC_NODE_RESERVED_ID != info_ptr->node_id )
   {
     return TRUE;
   }

   err_code = IPC_ECODE_EOK;
   rex_enter_crit_sect(&ipcns_table_crit_sect);
   do
   {
      /* generate a new node_id */
      node_id = ipcns_generate_node_id();

      if ((node_id == IPC_NODE_RESERVED_ID) || (node_id > IPC_NODE_MAX_ID))
      {
         err_code = IPC_ECODE_EFULL;
         break;
      }

      /* Set up the space for the priority queue */
      info_ptr->mq_ptr = (struct pq_msg_queue_type*) info_ptr->mq;

      /*
      ** Set the Name Server table with this new node
      */
      ipc_node_table[node_id].node_ptr = tcb_ptr;

      /*
      ** Set the IPC information in the thread TCB
      */
      info_ptr->node_id = node_id;
   } while(0);
   rex_leave_crit_sect(&ipcns_table_crit_sect);

   if (IPC_ECODE_EOK == err_code)
   {
      return TRUE;
   }
   else
   {
      if (IPC_ECODE_EFULL == err_code)
      {
         MSG_ERROR("Cannot find a free Node ID", 0, 0, 0);
      }
      else
      {
         MSG_ERROR("Error in IPC ipcns_node_register REX TCB 0x(%p)",
                   tcb_ptr, 0, 0);      
      }   
      return FALSE;
   }
}


/*===========================================================================
FUNCTION IPC_NODE_DEREGISTER

DESCRIPTION
 
PARAMETERS
  thread_ptr:  the thread to deregister

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ipcns_node_deregister
(
   void*    tcb_ptr
)
{
   rex_ipc_info_type* info_ptr = IPC_NODE_INFO(tcb_ptr);
   
   if (info_ptr->node_id != IPC_NODE_RESERVED_ID)
   {
        rex_enter_crit_sect(&ipcns_table_crit_sect);
        {
            /* Clear the Name Server table for this tcb */
            ipc_node_table[info_ptr->node_id].node_ptr = NULL;
            
            /* Clear the IPC information in the thread TCB */
            info_ptr->node_id = IPC_NODE_RESERVED_ID;
            
            /* Clear up the space for the priority queue */
            info_ptr->mq_ptr = NULL;
        }      
        rex_leave_crit_sect(&ipcns_table_crit_sect);
   }
   else
   {
      MSG_MED("Node is not registered", 0, 0, 0);
   }
}


/*===========================================================================
FUNCTION IPCNS_SERVICE_REGISTER

DESCRIPTION
 
PARAMETERS
  service_id:  the service provided by the thread
  thread_ptr:  the thread to register

RETURN VALUE
  TRUE  - Registration was successful
  FALSE - Registration failed
 
SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
boolean ipcns_service_register
(
   ipc_node_id_type service_id,
   void*            tcb_ptr
)
{
   rex_ipc_info_type* info_ptr = IPC_NODE_INFO(tcb_ptr);
   int err_code = IPC_ECODE_EOK;

   rex_enter_crit_sect(&ipcns_table_crit_sect);
   do
   {
      /*
      ** Range check service ID
      */
      if ( (service_id < IPC_NODE_FIRST_STATIC_ID)
           ||
           (service_id > IPC_NODE_LAST_STATIC_ID) )
      {
         err_code = IPC_ECODE_ERANGE;
         break;
      }

      /*
      ** Verify that nobody else already providing this Service
      */
      if ( ipc_node_table[service_id].node_ptr != NULL )
      {
         err_code = IPC_ECODE_EXISTS;
         break;
      }

      /*
      ** A thread can only provide a single service
      */
      if ( info_ptr->service_id != 0 )
      {
         err_code = IPC_ECODE_EALREADY;
         break;         
      }
      
      /*
      ** Configure the thread to provide the service
      */
      info_ptr->service_id = service_id;

      /*
      ** Set the Name Server table with this new node
      */
      ipc_node_table[service_id].node_ptr = tcb_ptr;
   
   } while(0);
   rex_leave_crit_sect(&ipcns_table_crit_sect);
   
   if (IPC_ECODE_EOK == err_code)
   {
      return TRUE;
   }
   else
   {
      if (IPC_ECODE_ERANGE == err_code)
      {
         MSG_ERROR("Service identifier (%d) out of range [%d,%d]",
             service_id,
             IPC_NODE_FIRST_STATIC_ID,
             IPC_NODE_LAST_STATIC_ID );
      }
      else if (IPC_ECODE_EXISTS == err_code)
      {
         MSG_ERROR("Service (%d) already registered", service_id, 0, 0);      
      }
      else if(IPC_ECODE_EALREADY == err_code)
      {
         MSG_ERROR("Thread already providing service (%d)",
                   info_ptr->service_id, 0, 0);      
      }
      else
      {
         MSG_ERROR("Error in IPC Service Register (%d) REX TCB 0x(%p)",
                   service_id, tcb_ptr, 0);      
      }
      return FALSE;
   }
}


/*===========================================================================
FUNCTION IPCNS_SERVICE_LOCATE

DESCRIPTION
 
PARAMETERS
  service_id:  the service to locate
  block:       blocking call

RETURN VALUE
  The node ID of the service - 0 if not found
 
SIDE EFFECTS
  None.
===========================================================================*/
ipc_node_id_type ipcns_service_locate
(
   ipc_node_id_type  service_id,
   boolean           block
)
{
   int err_code = IPC_ECODE_EOK;
   (void)block;
   /* Init'ed to NOT Found */
   ipc_node_id_type ret_val = IPC_NODE_RESERVED_ID;

   rex_enter_crit_sect(&ipcns_table_crit_sect);
   do
   {
      /* ID must be in correct range service range */
      if ( (service_id < IPC_NODE_FIRST_STATIC_ID)
        ||
        (service_id > IPC_NODE_LAST_STATIC_ID) )
      {
         err_code = IPC_ECODE_ERANGE;
         break;
      }
   
      if ( ipc_node_table[service_id].node_ptr == NULL )
      {
         err_code = IPC_ECODE_EALREADY;
         break;
      }

      ret_val = IPC_NODE_INFO(ipc_node_table[service_id].node_ptr)->node_id;

   } while(0);
   rex_leave_crit_sect(&ipcns_table_crit_sect);

   if (IPC_ECODE_EOK != err_code)
   {
      if (IPC_ECODE_ERANGE == err_code)
      {
         MSG_ERROR("Service identifier (%d) out of range [%d,%d]",
             service_id,
             IPC_NODE_FIRST_STATIC_ID,
             IPC_NODE_LAST_STATIC_ID );
      }
      else if (IPC_ECODE_EALREADY == err_code)
      {
         MSG_ERROR("Service (%d) not available", service_id, 0, 0);      
      }
      else
      {
         MSG_ERROR("Error in IPC Service De-Register (%d)",
                   service_id, 0, 0);
      }
   }

   return ret_val;
}



/*---------------------------------------------------------------------------
  Internal function definition.
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION IPC_SERVICE_LOCATE

DESCRIPTION
 
PARAMETERS
  service_id:  the service to locate
  block:       blocking call

RETURN VALUE
  The node ID of the service - 0 if not found
 
SIDE EFFECTS
  None.
===========================================================================*/
void* ipcns_node_get
(
   ipc_node_id_type node_id
)
{
   if ( node_id > IPC_NODE_MAX_ID )
   {
      MSG_ERROR("Node ID out of range (%d/%d)",
                node_id, IPC_NODE_MAX_ID, 0 );
      return NULL;
   }
   
   return ipc_node_table[node_id].node_ptr;
}


/*===========================================================================
FUNCTION IPCNS_GENERATE_NODE_ID

DESCRIPTION
 
PARAMETERS
  None.

RETURN VALUE
  A free nodeID or 0 if out of free nodeIDs
 
SIDE EFFECTS
  None.
===========================================================================*/
static ipc_node_id_type ipcns_generate_node_id
(
   void
)
{
   /* The generator points to the next unassigned nodeID */
   uint32  original_generator = ipc_node_id_generator;
   boolean found            = FALSE;
   
   /* Search the right part of the range */
   while (ipc_node_id_generator <= IPC_NODE_MAX_ID)
   {
      if (ipc_node_table[ipc_node_id_generator].node_ptr == NULL)
      {
         found = TRUE;
         break;
      }
      ipc_node_id_generator++;
   }

   /* Search the left part of the range */
   if ( !found )
   {
      ipc_node_id_generator = IPC_NODE_FIRST_DYNAMIC_ID;
      while (ipc_node_id_generator != original_generator)
      {
         if (ipc_node_table[ipc_node_id_generator].node_ptr == NULL)
         {
            found = TRUE;
            break;
         }
         ipc_node_id_generator++;
      }
   }
   
   return (found ? ipc_node_id_generator++ : IPC_NODE_RESERVED_ID);
}

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* FEATURE_REX_IPC */
