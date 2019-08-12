/*============================================================================
  FILE:         system_db_rpm.cpp
 
  OVERVIEW:     Handle system db events on the RPM processor
 
  DEPENDENCIES: None
 
                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/rpm.bf/2.1.1/core/power/system_db/src/system_db_rpm.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/14   dy      Add Downsample command
10/08/14   dy      Fix KW errors
09/28/14   dy      Add GPIO, RPM DUMP, ERR FATAL commands
                   Include SystemDB Pro in internal builds by default
                   Fix PMIC Read/Write

============================================================================*/
#include "system_db.h"
#include "system_db_rpm.h"
#include "rpmserver_internal.h"
#include "rpm.h"

extern "C" {
#include "DALSys.h"
#include "DALStdDef.h"
#include "CoreVerify.h"
//For logging
#include "swevent.h"
//Needed for the PMIC reads/writes
#include "SpmiBus.h"
//To pet the RPM watchdog
#include "dog.h"
//For clock dump
#include "Clock.h"

#include "msmhwiobase.h"

#include "rpm_config.h"

#define PWRDB_TLMM_GPIO_OFFSET             0
#define PWRDB_TLMM_BASE_ADDR               (TLMM_BASE_PHYS+PWRDB_TLMM_GPIO_OFFSET)
#define PWRDB_GPIO_CFGn_ADDR(n)            ((PWRDB_TLMM_BASE_ADDR) + 0x1000 * (n))
#define PWRDB_GPIO_IN_OUTn_ADDR(n)         ((PWRDB_TLMM_BASE_ADDR)+ 0x4 + 0x1000 * (n))

#define PWRDB_GET_LSB(x) ((x) & 0x1)

#ifdef SYSDB_PC_SIM
#include "string.h"
#endif

}

/** Defines the number of elements allocated for the op queue, kept small
  * due to space constraints on the RPM
  */
#define SYSTEMDB_OP_QUEUE_SIZE 8

/** Structure for the standby and op queue.  Both head and tail are needed
  * so that the whole list does not have to be walked to place something
  * on the end.
  */
static struct systemdb_op_queue_type
{
  SYSTEMDB_OP_QUEUE_ELEM *head;
  SYSTEMDB_OP_QUEUE_ELEM *tail;
} systemdb_op_queue, systemdb_standby_queue;

/** Free queue.  Elements placed on the head so no tail ptr needed */
static SYSTEMDB_OP_QUEUE_ELEM *systemdb_free_queue;
static SYSTEMDB_OP_QUEUE_ELEM *systemdb_queue_alloc;

#ifdef SYSTEMDB_PRO
static uint8 systemdb_pro_enabled = 0;
#endif

static uint32 sysdb_downsample = 0;
static uint32 sysdb_sample_count = 0;

typedef enum
{  
    SYSTEMDB_RPMDUMP_INTERNAL = 0,
    SYSTEMDB_RPMDUMP_RSRC_CURRENT_STATE = 1,
    SYSTEMDB_RPMDUMP_RSRC_SELECTED_REQ = 2,
    SYSTEMDB_RPMDUMP_RSRC_SPARE_REQ = 3,
} SYSTEMDB_RPM_DUMP_TYPE;

static void system_db_rpm_dump_rsrc( uint32 output_type );
static void system_db_rsrc_state_info ( ResourceData* rsrc, uint32* rsrc_ptr, uint32 ee_id, uint32 output_type);

/**
  * system_db_addop
  *
  * @brief Adds a read or write operation to the standby queue.
  *
  * Will draw free elements from the head of queues in the order of
  * free, op, standby.  This function will log the operation that has
  * been added to the queue.
  *
  * @param type: The command type of the element which defines the
  * number of times to execute, addr type, r/w type, and event type
  * @param buffer: uint32* buffer from the callback function.  Stores
  * the data elements for the op (address, number of reads/writes, etc.)
  */
void system_db_addop( uint32 *buffer )
{
  SYSTEMDB_OP_QUEUE_ELEM *elem;

  if ( systemdb_free_queue )
  { /* We have a free element so use it */
    elem = systemdb_free_queue;
    systemdb_free_queue = systemdb_free_queue->next;
  }
  else
  { /* Out of free elements so recycle the front and put it on the back */
      if ( systemdb_op_queue.head )
      {
          elem = systemdb_op_queue.head;
          systemdb_op_queue.head = systemdb_op_queue.head->next;

          /* if the systemdb_op_queue is empty or contains single element, adjust the tail accordingly */
          if ( ( NULL == systemdb_op_queue.head ) || ( NULL == systemdb_op_queue.head->next ) )
              systemdb_op_queue.tail = systemdb_op_queue.head;
      }
      else if ( systemdb_standby_queue.head != NULL )
      {
          elem = systemdb_standby_queue.head;
          systemdb_standby_queue.head = systemdb_standby_queue.head->next;

          /* if the systemdb_standby_queue is empty or contains single element, adjust the tail accordingly */
          if ( ( NULL == systemdb_standby_queue.head ) || ( NULL == systemdb_standby_queue.head->next ) )
              systemdb_standby_queue.tail = systemdb_standby_queue.head;
      }
      else
      {
          /* This scenario should never occur. If it does, something is seriously wrong. Reset the queue and start from the beginning */
          system_db_clearall();
          elem = systemdb_free_queue;
          systemdb_free_queue = systemdb_free_queue->next;
      }
  }
  /* Set elem fields */
  elem->next = NULL; /* Goes at tail of queue */
  elem->type = *buffer++;
  elem->addr = *buffer++;
  elem->num = *buffer++;
  if( GET_OP_TYPE_RW(elem->type) == SYSTEMDB_OP_READ )
  {
    //log the add read
    SWEVENT(SYSTEM_DB_ADD_READ, elem->type, elem->addr, elem->num);
  }
  else
  {
    elem->data = *buffer++;
    elem->mask = *buffer++;
    //log the add write
    SWEVENT(SYSTEM_DB_ADD_WRITE, elem->type, elem->addr, elem->num, elem->data, elem->mask);
  }

  /* Add to tail of standby queue */
  if( systemdb_standby_queue.tail != NULL )
  {
    systemdb_standby_queue.tail->next = elem;
  }
  else
  {
    systemdb_standby_queue.head = elem;
  }
  systemdb_standby_queue.tail = elem;
}


/**
  * system_db_perform_op
  *
  * @brief performs a read/write operation and logs the outcome.
  *
  * @param op: Operation to perform
  */
void system_db_perform_op( SYSTEMDB_OP_QUEUE_ELEM *op )
{
  uint32 addr_ptr = op->addr;
  uint32 addr_val;
  uint32 output_type = op->type;
  
  if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_GPIO) 
  {
    uint32 gpio_cfg = 0;
    uint32 gpio_in_out = 0;
    
    uint8 gpio_index = addr_ptr;
    uint8 max_range = op->num + gpio_index;
    
    while( gpio_index < max_range )
    {
      if ( GET_OP_TYPE_RW(op->type) == SYSTEMDB_OP_READ )
      {
         uint32 gpio_cfg_addr = PWRDB_GPIO_CFGn_ADDR(gpio_index);
         uint32 gpio_in_out_addr = PWRDB_GPIO_CFGn_ADDR(gpio_index);
         
         gpio_cfg = *(uint32 *) gpio_cfg_addr;
         gpio_in_out = *(uint32 *) gpio_in_out_addr;
         
         /* Check address range */
         if (gpio_cfg_addr > TLMM_BASE && ( gpio_cfg_addr < (TLMM_BASE + TLMM_BASE_SIZE) ))
         {
           addr_val = ((gpio_in_out & 0xFFFF) << 16) | (0xFFFF & gpio_cfg);
         
           SWEVENT(SYSTEM_DB_READ, op->type, gpio_index, addr_val);
         }
      }
      
      gpio_index++;
    }
  }
#ifdef SYSTEMDB_PRO
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_MEMORY) 
  {
    uint32 total_bytes = op->addr + op->num*sizeof(uint32);
    
    if (systemdb_pro_enabled)
    {
      /* Set the addr_ptr to the value of op->addr, dereference and log. */
      while( addr_ptr < total_bytes )
      {
        addr_val = *(uint32 *)addr_ptr;
        if ( GET_OP_TYPE_RW(op->type) == SYSTEMDB_OP_READ )
        {
          SWEVENT(SYSTEM_DB_READ, op->type, addr_ptr, addr_val);
        }
        else
        {
          *(uint32 *)addr_ptr = ( addr_val & ~( op->mask ) ) | ( op->data & op->mask );
          SWEVENT(SYSTEM_DB_WRITE, op->type, addr_ptr, op->data, op->mask);
        }

        addr_ptr += sizeof(uint32);
      }
    }
  }
#endif
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_PMIC)
  {
    /* op->addr is split into two 16 bit fields for pmic 
       slave id and pmic register */
    uint32 pmic_slave_id = ( op->addr >> 16 ) & 0xFFFF;
    uint32 end_addr = op->addr + op->num;
    uint8 bytes_to_read;
    
    while( addr_ptr < end_addr )
    {
      /* clear out addr_val */
      addr_val = 0;
      if( addr_ptr % sizeof(uint32)*64 == 0 )
      { /* read/write 64 words and then kick the dog */
        dog_kick();
      }
        bytes_to_read = addr_ptr + sizeof(uint32) < end_addr ? 
        sizeof(uint32) : end_addr - addr_ptr;
      /* perform the read */
      if( SPMI_BUS_SUCCESS == SpmiBus_ReadLong( pmic_slave_id, 
                                                SPMI_BUS_ACCESS_PRIORITY_LOW,
                                                addr_ptr & 0xFFFF,
                                                (uint8*)&addr_val, 
                                                bytes_to_read,
                                                NULL ) )
      {
        if ( GET_OP_TYPE_RW(op->type) == SYSTEMDB_OP_READ )
        {
          SET_OP_TYPE_COMMAND_ID(output_type, bytes_to_read);
          SWEVENT(SYSTEM_DB_READ, output_type, addr_ptr, addr_val);
        }
#ifdef SYSTEMDB_PRO
        else if (systemdb_pro_enabled)
        {
          addr_val = (addr_val & ~(op->mask)) | ( op->data & op->mask);
          SpmiBus_WriteLong( pmic_slave_id, 
                             SPMI_BUS_ACCESS_PRIORITY_LOW,
                             addr_ptr & 0xFFFF, 
                             (uint8*)&addr_val, 
                             1 );
          SWEVENT(SYSTEM_DB_WRITE, op->type, addr_ptr, op->data, op->mask);
        }
#endif /* SYSTEMDB_LITE */
      }
      addr_ptr += sizeof(uint32);
    }
  }
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_CLK)
  {
    Clock_LogState();
  }
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_RPMDUMP)
  {
    system_db_rpm_dump_rsrc( op->type );
  }
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_ERRFATAL_CMD)
  {
    ERR_FATAL( "Err Fatal sent by SystemDB command", op->type, addr_ptr, 0 );
  }
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_DOWNSAMPLE)
  {
    sysdb_downsample = addr_ptr;
  }
#ifdef SYSTEMDB_PRO
  else if(GET_OP_TYPE_ADDR_TYPE(op->type) == SYSTEMDB_ENABLE_PRO)
  {
    systemdb_pro_enabled = addr_ptr ? 1 : 0;
  }
#endif
  /* else undefined addr type */
}

/**
  * system_db_smd_cb
  *
  * @brief Interpret and perform action from master
  *
  * cb registered with the RPM to be called whenever a message comes in with
  * the SYSTEMDB service code.  It will interpret the action to perform based
  * on the command id, and call the relevant function.
  *
  * @param ee: Master that message came from
  * @param data: message received from master
  * @param len: length of message
  * @param context: context passed in when registering cb, not used by systemdb
  */
void system_db_smd_cb( unsigned ee, void *data, unsigned len, void *context )
{
  uint32 *msg_data = (uint32 *)data;
  const unsigned header_len = 6;

  /* check to see if the msg data passed in is sane */
  if ( NULL == msg_data || len < sizeof(uint32)*(header_len + 1) )
  {
    return;
  }

  //adjust data pointer to account for the header length
  msg_data += header_len;
  len -= header_len * sizeof(uint32);

  /* Check the command type */
  if( msg_data[0] == CLEAR_ALL )
  {
    //clearall
    system_db_clearall();
  }
  else if( msg_data[0] == END_OF_CMD )
  {
    // end of commands, put command queue into op queue
    system_db_end_of_commands();
  }
  else 
  {
    /* make sure the msg size corresponds to the expected size of the command */
    if( ( SYSTEMDB_OP_READ == GET_OP_TYPE_RW( msg_data[0] ) && 
          ( len != SYSTEMDB_READ_SIZE * sizeof(uint32) ) ) ||
        ( SYSTEMDB_OP_WRITE == GET_OP_TYPE_RW( msg_data[0] ) &&
          ( len != SYSTEMDB_WRITE_SIZE * sizeof(uint32) ) ) )
    {
      return;
    }
    
    if( SYSTEMDB_IMMEDIATE == GET_OP_TYPE_EVENT_TYPE( msg_data[0] ) )
    {
      unsigned times_to_execute  = GET_OP_TYPE_NUM_OPS( msg_data[0] );
      
      while (times_to_execute-- > 0)
        system_db_perform_op( (SYSTEMDB_OP_QUEUE_ELEM *)(msg_data - sizeof(SYSTEMDB_OP_QUEUE_ELEM *)/sizeof(uint32) ) );
    }
    else
    {
      system_db_addop( msg_data );
    }
  }
}

extern "C" {
/**
  * system_db_init
  *
  * @brief initialize systemdb structures
  *
  * Allocates and initializes elements for the free queue and registers callback
  */
void system_db_init(){
  // set up queues
  CORE_VERIFY( DAL_SUCCESS == DALSYS_Malloc( 
      SYSTEMDB_OP_QUEUE_SIZE * sizeof(SYSTEMDB_OP_QUEUE_ELEM),
      (void **)(&systemdb_queue_alloc)));
  system_db_clearall();
  // register rpm server callback
  rpmserver_register_handler( RPM_SYSTEMDB_SERVICE, system_db_smd_cb, NULL);
}

/**
  * system_db_statehandler
  *
  * @brief: Performs ops that match the current event
  *
  * Checks to see if the operation's event type matches the current event.  If
  * it does, then it will perform the op, and if the times to execute timer is
  * non-zero, decrement the times to execute counter.  If the operation has
  * been performed the number of times it's been specified, remove the
  * operation from the op queue.
  *
  * @param curr_event: Power state that the RPM is transitioning to.
  */
void system_db_statehandler(SYSTEMDB_EVENT_TYPE curr_event)
{
  SYSTEMDB_OP_QUEUE_ELEM *elem, *follow;
  unsigned times_to_execute;
  SWEVENT(SYSTEM_DB_STATE_HANDLER, curr_event);
  uint8 action_performed = 0;

  follow = NULL;
  elem = systemdb_op_queue.head;
  //scan through the write list to see if we need to do anything
  while ( elem != NULL )
  {
    //Check if event type matches operation event
    if ( GET_OP_TYPE_EVENT_TYPE( elem->type ) & curr_event )
    {
      if ((sysdb_downsample != 0) && sysdb_sample_count < sysdb_downsample)
      {
        // Skip events using the downsample count
        sysdb_sample_count++;
      }
      else
      {
        //perform op
        system_db_perform_op(elem);
        action_performed = 1;
        times_to_execute = GET_OP_TYPE_NUM_OPS( elem->type );
        if( times_to_execute != 0 && --times_to_execute == 0 )
        { /* op has expired, move it to the free queue */
          SYSTEMDB_OP_QUEUE_ELEM *next_elem = elem->next;
          if( follow == NULL )
          {
            systemdb_op_queue.head = elem->next;
          }
          else
          {
            follow->next = elem->next;
          }
          elem->next = systemdb_free_queue;
          systemdb_free_queue = elem;
          if( systemdb_op_queue.tail == elem )
          {
            systemdb_op_queue.tail = follow;
          }
          
          /* set elem to the next unread element -- follow doesn't change because
           of the element removal*/
          elem = next_elem;
          continue;
        }
        SET_OP_TYPE_NUM_OPS( elem->type, times_to_execute );
      }
    }
    /* set follow so we can do the element removal if we need to */
    follow = elem;
    elem = elem->next;
  }
  
  /* Reset sample count once pending actions have been performed */
  if (action_performed)
    sysdb_sample_count = 0;
}


/**
  * systemdb_end_of_commands
  *
  * @brief moves the commands in the standby queue to the operation queue
  */
void system_db_end_of_commands( void )
{
  /* if there are already commands on the op queue, put the new 
     ones on the end */
  if ( NULL != systemdb_op_queue.tail )
  {
    systemdb_op_queue.tail->next = systemdb_standby_queue.head;
    systemdb_op_queue.tail = systemdb_standby_queue.tail;
  }
  /* Else, just assign the op queue to the cmd queue */
  else
  {
    systemdb_op_queue.head = systemdb_standby_queue.head;
    systemdb_op_queue.tail = systemdb_standby_queue.tail;
  }

  /* empty the cmd queue */
  systemdb_standby_queue.head = NULL;
  systemdb_standby_queue.tail = NULL;
}


/**
  * system_db_clearall
  *
  * @brief Clear the op and standby queues
  */
void system_db_clearall() 
{
  unsigned i;
  for ( i = 0; i < SYSTEMDB_OP_QUEUE_SIZE - 1; i++ )
  {
    systemdb_queue_alloc[i].next = &systemdb_queue_alloc[i+1];
  }
  systemdb_queue_alloc[i].next = NULL;
  systemdb_free_queue = systemdb_queue_alloc;
  systemdb_op_queue.head = NULL;
  systemdb_op_queue.tail = NULL;
  systemdb_standby_queue.head = NULL;
  systemdb_standby_queue.tail = NULL;
  SWEVENT(SYSTEM_DB_FLUSH_QUEUE);
}
}


/* **************************** */
/* RPM Dump Internal Functions  */
/* **************************** */

static void system_db_rsrc_state_info ( ResourceData* rsrc, uint32* rsrc_ptr, uint32 ee_id, uint32 output_type)
{
  ResourceClassData* rc = &(rpm->classes[rsrc->class_idx]);
  
  uint32 class_name = rc->type;
  uint8 rsrc_size = rc->inrep_size / sizeof(uint32);
  
  /* Loop through resource data, and dump data in 32 bit segments */
  for (uint8 i = 0; i < rsrc_size; i++)
  {
    if (rsrc_ptr)
    {
      uint32 req_data = *rsrc_ptr++;
      uint32 msg_id = (rsrc->id << 16) | (ee_id << 8) | i; 
      
      SWEVENT(SYSTEM_DB_WRITE, output_type, class_name, msg_id , req_data);
    }
  }
}

static void system_db_rpm_dump_rsrc ( uint32 output_type )
{
  /* Dump Resource States */
  for (uint8 rsrcIndex = 0; rsrcIndex < rpm->supported_resources; rsrcIndex++)
  {
    ResourceData* this_resource = &(rpm->resources[rsrcIndex]);
    EEClient* client = this_resource->ees;

    uint32* current_state_ptr = (uint32*)this_resource->current_state;
    
    /* Current State */
    SET_OP_TYPE_COMMAND_ID(output_type, SYSTEMDB_RPMDUMP_RSRC_CURRENT_STATE);
    system_db_rsrc_state_info (this_resource, current_state_ptr, 0, output_type);
    
    /* EE Requests */
    for (uint8 eeIndex = 0; eeIndex < this_resource->num_eecs; eeIndex++)
    {
      if (client)
      {
        uint32 *select_req_ptr = (uint32*)client->request[PWRDB_GET_LSB(client->selected)];
        uint32 *spare_req_ptr = (uint32*)client->request[PWRDB_GET_LSB(client->selected ^ 1)];
        uint32 ee_id = client->ee_id;
        
        /* Selected EE Request */
        SET_OP_TYPE_COMMAND_ID(output_type, SYSTEMDB_RPMDUMP_RSRC_SELECTED_REQ);
        system_db_rsrc_state_info (this_resource, select_req_ptr, ee_id, output_type);
        
        /* Spare EE Request */
        SET_OP_TYPE_COMMAND_ID(output_type, SYSTEMDB_RPMDUMP_RSRC_SPARE_REQ);
        system_db_rsrc_state_info (this_resource, spare_req_ptr, ee_id, output_type);
        
        client = client->next;
      }
    } /* for eeIndex  */

  } /* for rsrcIndex  */
}

