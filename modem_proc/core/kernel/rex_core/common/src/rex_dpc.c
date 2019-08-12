#include "rex.h"
#include "rex_private.h"
#include "rex_port.h"

/*=================================================================================================
                                       DATA DECLARATIONS
=================================================================================================*/
/* Priority of the DPC Task */
#define REX_APC_THREAD_PRI                (243 - (MODEM_FW_NUM_PRIO + 13))

/* Defines the signal on which the DPC task waits. */
#define REX_DPC_SIG                       0x01

/* Defines the number of DPC slots */
#define REX_DPC_NUM_RING_ITEMS            256

/* Used to perform modulo operation.*/
#define REX_DPC_RING_MASK                 0x0FF

/* Number of bytes in the stack for the DPC task.*/
// moved to scons
//#define REX_DPC_STACK_SIZE                1024

/* DPC ring buffer. Holds currently queued DPCs */
static rex_apc_item_type rex_core_dpc_ring[REX_DPC_NUM_RING_ITEMS];

/* DPC ring buffer read index */
static int rex_core_dpc_rd_index = 0;

/* DPC ring buffer write index */
static int rex_core_dpc_wr_index = 0;

static rex_tcb_type rex_core_dpc_tcb;

static unsigned char rex_core_dpc_stack[ REX_DPC_STACK_SIZE ];

static rex_crit_sect_type rex_dpc_lock;

/*=================================================================================================
                                       FORWARD DECLARATIONS
=================================================================================================*/
/**
Reads the next DPC from the DPC ring buffer and executes it.
The read index will be updated.

@return
TRUE  --    If a DPC was executed
FALSE --    Otherwise.

@dependencies
None.
*/
static boolean rex_execute_next_dpc(void);

/**
Loops executing DPC's while DPC's are present. Otherwise blocks
waiting for DPC's to be added to the DPC ring buffer.

@note This function never returns.

@return
None.

@dependencies
None.
*/
static void rex_dpc_task(rex_task_func_param_type param);

/*=================================================================================================
                                       FUNCTION DEFINITIONS
=================================================================================================*/
int rex_create_dpc_task(void)
{
   rex_task_attributes_type rex_attr;

   rex_task_attr_init(&rex_attr);
   rex_task_attr_set_stack_addr(&rex_attr, rex_core_dpc_stack);
   rex_task_attr_set_stack_size(&rex_attr, REX_DPC_STACK_SIZE);
   rex_task_attr_set_prio(&rex_attr, REX_APC_THREAD_PRI);
   rex_task_attr_set_entry_func(&rex_attr, rex_dpc_task, 0);
   rex_task_attr_set_task_name(&rex_attr, "DPC Task");
   rex_task_attr_set_initially_suspended(&rex_attr, FALSE);

   return rex_common_def_task(&rex_core_dpc_tcb, &rex_attr);
}

static void rex_dpc_task(rex_task_func_param_type param __attribute__((unused)))
{
   rex_init_crit_sect(&rex_dpc_lock);
   for(;;)
   {
      /* Block here waiting for a DPC to be ready */
      (void) rex_wait(REX_DPC_SIG);
      (void) rex_clr_sigs(rex_self(), REX_DPC_SIG);

      /* Execute DPC's until the ring buffer is empty */
      while(rex_execute_next_dpc() == TRUE)
      {};
   }
}

static boolean rex_execute_next_dpc(void)
{
   rex_apc_func_type apc;
   unsigned long     apc_param = 0;
   boolean           apc_found;

   REX_ISR_LOCK(&rex_dpc_lock);
   {
      if (rex_core_dpc_rd_index != rex_core_dpc_wr_index)
      {
         apc = rex_core_dpc_ring[ rex_core_dpc_rd_index ].apc;
         apc_param  = rex_core_dpc_ring[ rex_core_dpc_rd_index ].apc_param;
         rex_core_dpc_rd_index = (rex_core_dpc_rd_index+1) & REX_DPC_RING_MASK;
         apc_found = TRUE;
      }
      else
      {
         apc = NULL;
         apc_found = FALSE;
      }
   }
   REX_ISR_UNLOCK(&rex_dpc_lock);

   if (NULL != apc)
   {
      apc(apc_param);
   }

   return apc_found;
}

boolean rex_queue_dpc(rex_apc_func_type apc, unsigned long param)
{
   boolean ret_status = FALSE;

   if (NULL != apc)
   {
      REX_ISR_LOCK(&rex_dpc_lock);
      {
         /* Make sure there is space in the ring buffer, add the DPC to the
         ** ring and signal the DPC task.
         */
         if (((rex_core_dpc_wr_index+1) & REX_DPC_RING_MASK) != rex_core_dpc_rd_index)
         {
            rex_core_dpc_ring[rex_core_dpc_wr_index].apc   = apc;
            rex_core_dpc_ring[rex_core_dpc_wr_index].apc_param = param;
            rex_core_dpc_wr_index = (rex_core_dpc_wr_index + 1) & REX_DPC_RING_MASK;

            (void) rex_set_sigs(&rex_core_dpc_tcb, REX_DPC_SIG);
            ret_status = TRUE;
         }
      }
      REX_ISR_UNLOCK(&rex_dpc_lock);
   }

   return ret_status;
}

boolean rex_queue_apc(rex_apc_func_type apc, rex_tcb_type *p_ignored __attribute__((unused)), unsigned long param)
{
   return rex_queue_dpc(apc, param);
}
