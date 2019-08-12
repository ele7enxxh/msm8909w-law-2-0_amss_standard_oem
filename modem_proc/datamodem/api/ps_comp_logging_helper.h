#ifndef PS_COMP_LOGGING_HELPER_H
#define PS_COMP_LOGGING_HELPER_H
/*===========================================================================
             P S _ C O M P _ L O G G I N G _ H E L P E R . H

DESCRIPTION
  Header file for the IP Header/Data Compression Logging support.
  Primarily meant for providing DPL (Data Protocol Logging) capability for 
  header compression modules.   


Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ps_comp_logging_helper.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/06/13    ss     Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ps_iface.h"
#include "queue.h"

#include "sys.h"

/*===========================================================================

                              DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  This is the handle type passed to the clients creating the logging
  instance.
---------------------------------------------------------------------------*/
typedef void* ps_comp_logging_handle_type;

/*---------------------------------------------------------------------------
  This structure holds the information required for creating a new 
  logging instance.
---------------------------------------------------------------------------*/
typedef struct
{
  /* SUB ID Info */
  sys_modem_as_id_e_type sub_id;

  /* RAT Specific Identification Info */
  union 
  {

    uint32 wcdma_lcid;

  }logging_info;

} ps_comp_logging_instance_config_s;

/*---------------------------------------------------------------------------
  This enum identifies the RATs for which this module will handle DPL 
  logging.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_COMP_LOGGING_PKT_FORMAT_INVALID,

  PS_COMP_LOGGING_PKT_FORMAT_DSM      = 1,
  PS_COMP_LOGGING_PKT_FORMAT_RAW_DATA = 2,
  
  PS_COMP_LOGGING_PKT_FORMAT_MAX
  
} ps_comp_logging_pkt_format_e;

/*---------------------------------------------------------------------------
  This structure contains info about the current packet being logged.  
---------------------------------------------------------------------------*/
typedef struct
{
  /* Link to be used while this item is queued to be processed in PS task */
  q_link_type queue_link;
  
  /* Comp Logging instance handle to which this packet belongs */
  ps_comp_logging_handle_type instance_handle;

  /* IP Version of the packet being logged : IPv4 or IPv6 */
  ip_version_enum_type pkt_ip_version;

  /* Packet Content Format : How is the packet to be logged being 
     specified? */
  ps_comp_logging_pkt_format_e pkt_format;

  /* Packet contents
     Either specified in DSM format or directly in Dynamic memory */
  struct
  {
    /* DSM Item Pointer for the packet.
       This item would be freed by the logging module once the packet is 
       logged. */
    dsm_item_type *dsm_item_ptr;

    /* Memory contents to be logged.
       This memory should be allocated from PS System Heap by the caller.
       (Using ps_system_heap_mem_alloc())
       Would be freed by the logging module after packet is logged. */
    struct
    {
      uint8 *raw_data_ptr;
      uint32 raw_data_size;

    }raw_data;

  }pkt_content;
  
} ps_comp_logging_pkt_info_s;

/*===========================================================================

FUNCTION PS_COMP_LOGGING_FETCH_IFACE_CBACK_TYPE

DESCRIPTION
  Callback registered by Mode Handlers to provide iface id based on the
  ps_comp_logging_info passed to them.
 
DEPENDENCIES
  None

PARAMETERS
  info : Logging info to be used as input for deriving the iface pointer
  ip_version : Version of the packet - IPv4 or IPv6
 
RETURN VALUE
  PS_IFACE pointer corresponding to the logging info and ip version 
  specified.

SIDE EFFECTS
  None

===========================================================================*/
typedef ps_iface_type* (*ps_comp_logging_fetch_iface_cback_type)
(
  ps_comp_logging_instance_config_s *logging_info_ptr,
  ip_version_enum_type ip_version
);

/*===========================================================================

FUNCTION PS_COMP_LOGGING_GENERIC_CBACK_TYPE

DESCRIPTION
  Callback function which would be invoked for the owner module in the 
  task context in which PS Comp Logging Helper is operating.
 
DEPENDENCIES
  None

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
typedef void (*ps_comp_logging_generic_cback_type)
(
  void
);

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
void ps_comp_logging_init( void );

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
  ps_comp_logging_instance_config_s *logging_config_ptr
);

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
);

/*===========================================================================
FUNCTION PS_COMP_LOGGING_REGISTER_FETCH_IFACE_CBACK

DESCRIPTION
  This API will 

DEPENDENCIES
  The IPDC instance must have been initialized.

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   0   if successful.
  -1   in case of failure.

SIDE EFFECTS
  Copies the IPDC Instance's V2 Compressor Engine metrics into the IPDC
  Compressor Metrics log packet
===========================================================================*/
int32 ps_comp_logging_register_fetch_iface_cback
(
  sys_sys_mode_e_type sys_mode,
  ps_comp_logging_fetch_iface_cback_type fetch_iface_cback
);

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
  Copies the IPDC Instance's Decompressor Engine metrics into the IPDC
  Decompressor Metrics log packet
===========================================================================*/
int32 ps_comp_logging_log_pkt
(
  ps_comp_logging_pkt_info_s *pkt_info_ptr
);

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
  None.
===========================================================================*/
void ps_comp_logging_invoke_generic_cback
(
  ps_comp_logging_generic_cback_type cback
);

#endif /* PS_COMP_LOGGING_HELPER_H */
