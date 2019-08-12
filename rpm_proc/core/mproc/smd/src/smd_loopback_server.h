#ifndef _SMD_LOOPBACK_SERVER_H_
#define _SMD_LOOPBACK_SERVER_H_

/*===========================================================================

                S M D   L O O P B A C K   S E R V E R   A P I

===========================================================================*/
/**
  @file smd_loopback_server.h
*/
/* 
  Declares the private API and structures used to implement the N-way shared 
  memory driver loopback servers.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_loopback_server.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/12   bt      Add boolean return to smd_lb_server_lite_event_trigger.
07/31/12   pa      Added smdl_loop_struct and made public the loopback routines
                   needed for the loopback task implementation.
07/01/11   bt      Initial implementation.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd_loopback_n_way.h"
#include "smd_lite.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * The control data for each echo data port.
 *
 * This is allocated from the heap by smd_lb_lite_alloc_handle() and freed
 * by smd_lb_lite_free_handle().
 */
typedef struct smdl_loop_struct
{
  /** The index of this loopback port/server in smd_loopback_info. */
  uint32                idx;
  
  /** The size of the echo buffer. */
  uint32                buffer_len;

  /** The amount of data waiting to be written from the echo buffer. */
  uint32                data_len;

  /**
   * The buffer used for reading data from the echo port.
   *
   * This is allocated from the heap by smd_lb_lite_alloc_handle() and freed
   * by smd_lb_lite_free_handle().
   */
  uint8                *buffer;
  
  smdl_event_type       event;

  smdl_handle_type      port;
  smdl_handle_type      rx_port;

  struct smdl_loop_struct *rx;
} smdl_loop_type;

/*===========================================================================


===========================================================================*/
/**
 * Internal server data structure for SMD memcpy loopback ports.
 *
 * This structure should not be used directly by clients; they should treat
 * smd_lb_mcpy_serv_handle as an opaque handle. */
struct smd_lb_memcpy_server_struct;

/** Opaque handle pointing to the internal server memcpy data structures. */
typedef struct smd_lb_memcpy_server_struct * smd_lb_mcpy_serv_handle;

/**
 * Internal server data structure for SMD DSM loopback ports.
 *
 * This structure should not be used directly by clients; they should treat
 * smd_lb_dsm_serv_handle as an opaque handle. */
struct smd_lb_dsm_server_struct;

/** Opaque handle pointing to the internal server DSM data structures. */
typedef struct smd_lb_dsm_server_struct * smd_lb_dsm_serv_handle;

/* SMD Core uses a port_id for a handle, SMDLite uses smdl_handle_type, and
 * DSM uses a port_id as well. */
typedef union {
  smd_lb_mcpy_serv_handle mcpy;
  smdl_handle_type        lite;
  smd_lb_dsm_serv_handle  dsm;
} smd_loopback_info_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_server_update
===========================================================================*/
/**
  Checks every existing N-way SMD Loopback edge and performs any outstanding 
  commands from clients, or appropriate server state transitions.
  
  Called from smd_main() after all other events have been serviced, if the 
  host processor is a valid server for any interprocessor edge.  Will exit 
  without doing anything if no connected client has an outstanding command 
  it wishes the (host) server to perform.  Will not do anything for loopback
  edges for which there is a pending state transition (such as 
  Opened -> Closed).

  @return
  None.

  @dependencies
  None.
*/
/*==========================================================================*/
void smd_loopback_server_update( void );

/*===========================================================================
  FUNCTION  smd_lb_lite_free_handle
===========================================================================*/
/**
  Frees an SMD Lite echo handle

  @param[in] handle  The SMD Lite echo handle. Must have been allocated by
                     smdl_echo_alloc_handle() and must not be NULL.

  @return None.
*/
/*==========================================================================*/
void smd_lb_lite_free_handle
(
  smdl_loop_type *handle
);

/*===========================================================================
  FUNCTION  smd_lb_lite_read_write
===========================================================================*/
/**
  Reads data "from" and writes data "to"

  Uses the "from"->buffer to store data until it can be written "to".

  @param[in] from  Loopback structure with the buffer to read from.
  @param[in] to    Loopback structure with the buffer to write to.

  @return None.
*/
/*==========================================================================*/
void smd_lb_lite_read_write
(
  smdl_loop_type *from,
  smdl_loop_type *to
);

/*===========================================================================
FUNCTION      smd_lb_start_task
===========================================================================*/
/**
  Creates a the loopback task associated events

  @return   None.
===========================================================================*/
void smd_lb_start_task(void);

/*===========================================================================
FUNCTION      smd_lb_server_lite_event_trigger
===========================================================================*/
/**
  Triggers a loopback event to a given loopback port.

  @param[in] port_idx   Index in the loopback structure of the relevant port.
  @param[in] event      The SMDL loopback event to handle.

  @return
  TRUE if the open was successful.
  FALSE if the open fails for any reason, based on the arguments provided in 
  the loopback command structure.

  @dependencies
  None.
*/
/*=========================================================================*/
boolean smd_lb_server_lite_event_trigger
(
  uint32          port_idx, 
  smdl_event_type event
);

/*===========================================================================
  FUNCTION  smd_lb_lite_cb
===========================================================================*/
/**
  Callback for the SMD Lite N-way loopback data ports.

  When data is waiting to be read (event == SMDL_EVENT_READ), this function
  reads the first packet from the port and writes it to the return FIFO. If
  space is not immediately available in the return FIFO, the data will be
  stored until it can be transmitted (event == SMDL_EVENT_WRITE).  Also
  handles SMDL_EVENT_CLOSE_COMPLETE events appropriately by setting the server
  state to SMD_LOOPBACK_STATE_CLOSED and cleaning up the internal lite
  loopback structure.

  @param[in] port     The loopback data port handle.
  @param[in] event    The event signaled by SMD Lite.
  @param[in] data     Pointer to the loopback data port control structure,
                      of type smdl_loop_type.

  @return     None.
*/
/*==========================================================================*/
void smd_lb_lite_cb
(
  smdl_handle_type port,
  smdl_event_type  event,
  void            *data
);

#ifdef __cplusplus
}
#endif

#endif /* _SMD_LOOPBACK_SERVER_H_ */
