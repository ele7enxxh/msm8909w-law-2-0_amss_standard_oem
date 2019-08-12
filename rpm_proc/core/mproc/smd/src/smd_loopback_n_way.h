#ifndef _SMD_LOOPBACK_H_
#define _SMD_LOOPBACK_H_

/*===========================================================================

              S M D   N W A Y   L O O P B A C K   P R O T O C O L

===========================================================================*/
/**
  @file smd_loopback.h
*/
/* 
  Defines the private structures used to implement the N-way shared 
  memory driver loopback clients and servers.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_loopback_n_way.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/12   bt      Hardcode SMD_LOOPBACK_NUM_CHANNELS to 15, to remove false 
                   dependency on smd edges, avoid changing size of structures.
09/05/12   pa      Added smd.h to remove compile errors.
07/31/12   pa      Cleaned up include files.  Added private loopback events.
10/31/11   bm      Moved smd_loopback_command_type structure definition into
                   smem_boot.h as smem_smd_loopback_command_type.
05/04/11   bt      Initial implementation.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_boot.h"
#include "smd_lite.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/* The possible states that the client or server of a loopback channel could 
 * be in. */
typedef enum
{
  /* The default state that both client and server must be in upon system 
   * start-up, before the loopback channel is opened.  After it is opened, 
   * each endpoint will return to this state when it closes the channel 
   * locally. */
  SMD_LOOPBACK_STATE_CLOSED,
  
  /* Each endpoint enters this state when it opens the local side of the 
   * loopback channel.  Both the client and the server must be in this state
   * in order to change signals on the loopback port or close it. */
  SMD_LOOPBACK_STATE_OPEN,
  
  /* The server will only enter this state if the client requests the loopback 
   * channel to open using \c smd_loopback_open, and the server is unable to 
   * successfully open for any reason.  The client does not use this state. */
  SMD_LOOPBACK_STATE_NAK,
  
  /* The client will enter this state when it wishes the server side to 
   * change any combination of its RS-232 signals (DTR, CTS, CD, RI).  The 
   * server side will enter this state when it confirms that it has made those
   * changes. */
  SMD_LOOPBACK_STATE_CHANGE_SIG
} smd_loopback_state_type;

/* Indicates which SMD API is being used to implement read, write, and 
 * signaling functions by the server of a loopback channel. */
typedef enum
{
  SMD_LOOPBACK_API_MEMCPY,
  SMD_LOOPBACK_API_SMDLITE,
  SMD_LOOPBACK_API_DSM
} smd_loopback_api_type;

/* Indicates whether the server of a loopback channel echoes, sources, or sinks
 * data. */
typedef enum
{
  /* When the client writes data to the channel, the server will read it and 
   * then immediately begin writing it back on the same channel, in the 
   * opposite direction.  The client will then be able to read out of the 
   * same channel the same data that it had previously written.  The time 
   * required for the server to echo the data back and the events that result
   * from those actions depend on the \c smd_loopback_api_type used by the 
   * server. */
  SMD_LOOPBACK_ECHO,
  
  /* The server will only write data to the loopback channel.
   * Not implemented yet. */
  SMD_LOOPBACK_SOURCE,
  
  /* The server will only read data from the loopback channel.
   * Not implemented yet. */
  SMD_LOOPBACK_SINK
} smd_loopback_mode_type;

/* A copy of this command structure is used for each loopback channel.
 * It contains information that is used by the client API, as well as by the 
 * server when responding to the client's commands.
 *
 * The actual structure is defined in smem_boot.h to make SMEM boot code
 * independent of SMD drivers.
 */
typedef smem_smd_loopback_command_type smd_loopback_command_type;

/*===========================================================================

                        MACRO AND DATA DEFINITIONS

===========================================================================*/
#define SMD_LOOPBACK_NUM_CHANNELS SMEM_SMD_LOOPBACK_NUM_CHANNELS

#define SMD_NWAY_EDGE_UNUSED      SMEM_SMD_NWAY_EDGE_UNUSED

#define SMD_NWAY_LOOP_VERSION 0x00000001

/* Define the private loopback events to open the port. 
 * These are not included with SMDL_EVENT_* to prevent clients from using them */
#define SMDL_EVENT_LOOPBACK_OPEN  (SMDL_EVENT_LAST+1)
#define SMDL_EVENT_LOOPBACK_CLOSE (SMDL_EVENT_LAST+2)

/* Max number of events, including the private loopback events */
#define SMDL_EVENT_LOOPBACK_MAX   SMDL_EVENT_LOOPBACK_CLOSE

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_nway_init
===========================================================================*/
/**
  Initializes the SMD Nway Loopback structures, to avoid locking the
  smem allocation spinlock more than once.

  @return
  None.

  @dependencies
  Shared memory must have been initialized, and the SMD Nway loopback register
  must have already been allocated by the bootloaders.
*/
/*==========================================================================*/
void smd_loopback_nway_init( void );

#ifdef __cplusplus
}
#endif

#endif /* _SMD_LOOPBACK_H_ */
