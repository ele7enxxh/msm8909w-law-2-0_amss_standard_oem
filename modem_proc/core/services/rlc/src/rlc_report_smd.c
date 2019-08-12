/**
@file rlc_report_smd.c
@brief
This module implements reporting through the shared memory driver. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through SMD.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_report_smd.h"
#include "rlc_report_smd_protocol.h"
#include "smd_lite.h"
#include "comdef.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/** RLC Router Name */
#define RLC_SMD_ROUTER_NAME "RLC_Router"


#define RLC_MAX_CHANNEL_NUM SMD_CHANNEL_TYPE_LAST

/* Default FIFO Size */
#define RLC_SMDL_DEFAULT_FIFO SMD_MIN_FIFO

/* Default open flags Currently only packet mode is supported */
#define RLC_SMDL_DEFAULT_MODE SMDL_OPEN_FLAGS_MODE_PACKET

/* Mask to get the port number */
#define RLC_SMD_PORT_MASK       0x000000FFuL

/* Macro to get the port number from the reporting information */
#define RLC_SMD_GET_PORT(x) (x & RLC_SMD_PORT_MASK)

/* Macro to get the remote subsystem information */
#define RLC_SMD_GET_REMOTE_SS(x) ((x & RLC_REPORT_TO_MASK) >> RLC_REPORT_TO_BITSHIFT)

/**
Static Variables
*/
static rlc_smd_protocol_table rlc_smd_protocol[RLC_MAX_CHANNEL_NUM];

static rlc_smd_internal_type rlc_smd_internal;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Main callback function to be passed into SMD protocol.

@return
None.
 
@dependencies  
None.
*/

static void rlc_smd_event_notify
(
  smdl_handle_type handle,
  smdl_event_type  event,
  void             *cb_data
)
{
  switch(event)
  {
  case SMDL_EVENT_OPEN:
  case SMDL_EVENT_READ:
  case SMDL_EVENT_WRITE:
  case SMDL_EVENT_CLOSE_COMPLETE:
  case SMDL_EVENT_REMOTE_CLOSE:
  default:
    break;
  }
}

/** 
Private function for openning SMD ports that are available in order to
allow for reporting to various peripheral images.

@return
None.
 
@dependencies  
None.
*/

static void rlc_smd_populate_report_to_table(void)
{
  uint32 smdPorts,smdTablePorts;
  
  for(smdPorts = RLC_REPORT_TO_MIN; smdPorts < RLC_REPORT_TO_MAX; smdPorts++)
  {
    for(smdTablePorts = SMD_CHANNEL_TYPE_FIRST; 
                        (smdTablePorts < SMD_CHANNEL_TYPE_LAST) &&
                        (rlc_smd_internal.rlc_smd_report_to_table[smdPorts].smdPortPtr == NULL); 
                        smdTablePorts++)
    {
      if(rlc_smd_protocol[smdTablePorts].bit_mask & (1 << smdPorts))
      {
        rlc_smd_internal.rlc_smd_report_to_table[smdPorts].smdPortPtr = 
                        (void*) smdl_open( RLC_SMD_ROUTER_NAME,
                                           smdPorts,
	  		                                   RLC_SMDL_DEFAULT_MODE,
	  	                                     RLC_SMDL_DEFAULT_FIFO,
	  		                                   rlc_smd_event_notify,
	 	 	                                     NULL );
      }
    }
  }
}

/** 
Private function for getting the SMD port pointer for
the SMD protocol from the internal RLC SMD table.

@return
None.
 
@dependencies  
None.
*/

static void* rlc_smd_get_report_ptr(uint32 bit_mask)
{
  uint32 bit_iterator = 0;
  uint32 internal_bit_mask = bit_mask;

  /* We need a bit mask in order for this to work */
  if( bit_mask == 0)
  {
    return NULL;
  }

  while( (internal_bit_mask & 1) != 1 )
  {
    internal_bit_mask >>= 1;
    bit_iterator += 1;
  }

  if(bit_iterator >= RLC_REPORT_TO_MAX)
  {
    return NULL;
  }

  return rlc_smd_internal.rlc_smd_report_to_table[bit_iterator].smdPortPtr;
}

/** 
Private function for openning SMD ports that are available in order to
allow for reporting to various peripheral images.

@return
None.
 
@dependencies  
None.
*/

void rlc_smd_init(void)
{
  uint32 uSmdPort;

  for(uSmdPort = 0; uSmdPort < RLC_SMD_NUM_PORTS; uSmdPort++)
  {
    rlc_smd_internal.rlc_smd_port_table[uSmdPort].txPort = (void*)RLC_SMD_MAGIC;
    rlc_smd_internal.rlc_smd_port_table[uSmdPort].rxPort = (void*)RLC_SMD_MAGIC;
  }

  for(uSmdPort = 0; uSmdPort < RLC_REPORT_TO_MAX; uSmdPort++)
  {
    rlc_smd_internal.rlc_smd_report_to_table[uSmdPort].smdPortPtr = NULL;
  }

  rlc_report_smd_protocol_fill_table(rlc_smd_protocol, (uint32)RLC_MAX_CHANNEL_NUM);

  rlc_smd_populate_report_to_table();

}

/** 
Private function for setting up client information within internal attribute.

@param[in]  pAttribute      Internal attribute pointer to pass in 
                            and write client info.
@param[in]  reporting_data  Data to pass into the SMD initialization.  This info
                            is used in setting up the SMD port.

@return
RLC_SUCCESS --  Successfully setup SMD client for reporting.
RLC_FAILED  --  Error setting up SMD client for reporting.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_smd_init(rlc_attribute_type *pAttribute, uint32 reporting_data)
{
  rlc_smd_client_handle *smdHandle;

  if(pAttribute == NULL)
  {
    return RLC_FAILED;
  }

  if(pAttribute->report_ptr == NULL)
  {
    return RLC_FAILED;
  }

  smdHandle = ( rlc_smd_client_handle* ) rlc_malloc ( sizeof(rlc_smd_client_handle) );
  
  if(smdHandle == NULL)
  {
    return RLC_NO_MEMORY;
  }

  smdHandle->reporting_port = RLC_SMD_GET_PORT(reporting_data);
  smdHandle->smd_port       = RLC_SMD_GET_REMOTE_SS(reporting_data);
  

  pAttribute->report_ptr->report_handle = (rlc_report_output_handle)smdHandle;

  return RLC_SUCCESS;
}


/** 
Private function for reporting through SMD.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS --  Successfully reported through SMD.
RLC_FAILED  --  Error reporting through SMD.
 
@dependencies  
SMD should be initialized by this point and attribute information
should be filled out in init function.
*/

rlc_error_type rlc_report_smd(rlc_attribute_type *pAttribute, char* string)
{
  if(pAttribute == NULL ||
    pAttribute->report_ptr == NULL ||
    pAttribute->report_ptr->report_handle == NULL ||
    string == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }
  
  //if ( smdl_write( ssm_smd_handle, length, write_buffer, SMDL_WRITE_FLAGS_NONE ) < 0 )
  //{
  //  return RLC_FAILED;
  //}

  return RLC_SUCCESS;
}
