#ifndef SMD_OS_COMMON_H
#define SMD_OS_COMMON_H

/*===========================================================================

               Shared Memory Driver OS Common Header File

Defines the OS APIs that must be provided to support SMD.


 Copyright (c) 2006 - 2010, 2012  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_os_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/13   bt      Rename string copy function to smd_string_copy.
10/05/12   bt      Fix bad integration to close header comment.
09/04/12   rs      Added MALLOC/FREE functions for SMD
07/31/12   bt      Remove smd_up, smd_down from legacy modem restart.
07/31/12   pa      Cleaned up header inclusions.
11/30/11   bt      Added smd_cs_event_create for SMDL interrupt context.
11/08/10   rs      Protect open ports list for standalone SMD Lite 
11/04/10   tl      Added event to smd_event_send_smd_context()
07/21/10   tl      Implemented SMD API for SMD priority.
06/08/10   tl      Split SMD task into multiple tasks with different priorities
05/03/10   tl      Use critical sections instead of INTLOCK
11/19/09   sa      warning cleanup, sync-up with symbian
03/31/09   hwu     consolidate smd_event_send_xxx to smd_event_send. 
06/11/08   sa      Removing smd_start_ist and using ist table instread
06/05/08   sa      Cleaned up unneeded REX parts
                   Renamed from smd_os.h to smd_os_common.h
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
03/04/08   bfc     Fixed the Critical section headers.
02/21/08   bfc     First SMD Block port check in.
02/16/08   bfc     Added a create/destroy/lock utility for Critical Sections.
01/30/08   hwu     Moved SMD_STRNCPY to smd_target.h to accomadate Q6.
11/15/07   ih      Moved smd_strncpy to smd_main.c since inline functions are 
                   not supported on WM
11/09/07   bfc     Created a strncpy equivalent function to remove the
                   dependency to a safe strncpy.
10/08/07   hwu     Move smd_channel_type to smd.h
09/12/07   bfc     When an EVENT_SIG occurs process the cmd_q as well.
07/05/07   hwu     Changed smd_destination_type to smd_channel_type.
05/17/07   hwu     Added SMD loopback support.
12/20/06   hwu     Added smd_up and smd_down APIs to provide a way for
                   the SMD to signal the upper layer that the SMD is up
                   or down, if it needs to.
11/17/06   ptm     Add L4 interrupt event signal.
11/13/06   hwu     combined the smd_update_sig, smd_transmit_sig and
                   smd_receive_sig into smd_event_sig.
11/03/06   ptm     Delete smd_event_wait API - no longer needed.
10/10/06   ptm     Remove smd_event_set API - no longer needed.
10/03/06   ptm     Add event APIs.
09/05/06   ptm     Initial version.
===========================================================================*/

/*===========================================================================
                           INCLUDES
===========================================================================*/
#include "smd.h"
#include "smd_internal.h"

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smd_string_copy
===========================================================================*/
/**
  Copies the source string into the destination buffer until 
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.
  
  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *smd_string_copy(char *dst, const char *src, uint32 size);

/*===========================================================================
FUNCTION      smd_event_send_update

DESCRIPTION   Send an event to the specified processor.

PARAMETERS    info - the port information pointer

RETURN VALUE  None
===========================================================================*/
void smd_event_send( smd_stream_info_type *info );

/*===========================================================================
FUNCTION      smd_event_send_smd

DESCRIPTION   Send an event to the smd thread that handles a particular port

PARAMETERS    port_id - port id to send the event to

RETURN VALUE  None
===========================================================================*/
void smd_event_send_smd( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smd_event_send_smd_context

DESCRIPTION   Send an event to the smd thread specified by context

PARAMETERS    context - SMD context to send the event to

              event - an event mask, which directly translates to rex signals

RETURN VALUE  None
===========================================================================*/
void smd_event_send_smd_context( smd_context_type *context, uint32 event );

/*===========================================================================
FUNCTION      smd_event_start_loopback

DESCRIPTION   This function sets the SMSM_SMD_LOOPBACK state for the
              local host.

PARAMETERS    None

RETURN VALUE  None
===========================================================================*/
void smd_event_start_loopback(void);

/*===========================================================================
FUNCTION      smd_dtr_lock

DESCRIPTION   Lock the DTR lock.

RETURN VALUE  None.
===========================================================================*/
void smd_dtr_lock( boolean lock );

/*===========================================================================
FUNCTION      smd_ioctl_lock

DESCRIPTION   Lock the IOCTL lock.

RETURN VALUE  None.
===========================================================================*/
void smd_ioctl_lock( boolean lock );

/*===========================================================================
FUNCTION      smd_dsm_lock

DESCRIPTION   Lock the DSM Lock.

RETURN VALUE  None.
===========================================================================*/
void smd_dsm_lock( boolean lock );

/*===========================================================================
FUNCTION      smd_cs_lock

DESCRIPTION   lock/unlock a critical section

PARAMETERS    cs - pointer the the critical section

              lock - lock or unlock the critical section

RETURN VALUE  None.
===========================================================================*/
void smd_cs_lock( void *cs, boolean lock );

/*===========================================================================
FUNCTION      smd_cs_create

DESCRIPTION   Initializes a Critical Section

PARAMETERS    None

RETURN VALUE  The critical section.
===========================================================================*/
void *smd_cs_create(void);

/*===========================================================================
FUNCTION      smd_cs_destroy

DESCRIPTION   Destroys a Critical Section

PARAMETERS    None

RETURN VALUE  none.
===========================================================================*/
void smd_cs_destroy( void *cs );

/*===========================================================================
FUNCTION      smd_assign_context
===========================================================================*/
/**
@brief        Assigns an SMD context to the provided SMD port, based on the
              requested priority.

              This function must be called after populating the info structure
              from the API function that opens the SMD port, before calling
              smd_event_send_smd() to notify the particular task responsible
              for handling the port.

@param[in,out]  info    Pointer to the SMD info structure on which to assign
                        the SMD context. This function will populate
                        info->context with the appropriate task context.

@param[in]    priority  The priority requested by the client. If SMD is
                        implemented using multiple tasks with different OS
                        priorities for ports of different priorities, this
                        function will use this parameter to assign the context
                        to the appropriate task.

@return       None
*/
/*=========================================================================*/
void smd_assign_context( smd_info_type * info, smd_priority_type priority );

/*===========================================================================
FUNCTION      smd_context_get_default
===========================================================================*/
/**
@brief        Returns a pointer to the default SMD context.

              This function is used by smd_cmd() to get the default SMD
              context used for handling loopback servers.

@return       Default SMD context
*/
/*=========================================================================*/
smd_context_type * smd_context_get_default(void);

/*===========================================================================
FUNCTION      smd_open_list_lock
===========================================================================*/
/**
  Locks/unlocks SMD Lite open ports list critical section.

  This function protects the standalone SMD Lite open ports list, accessed from
  both client's and ISR context.

  @param[in] context      Owner of the SMD Lite open ports list
 
  @param[in] lock         TRUE locks; FALSE unlocks

@return       None
*/
/*=========================================================================*/
void smd_open_list_lock( smd_context_type *context, boolean lock );

/*===========================================================================
  FUNCTION  smd_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c smd_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *smd_malloc( uint32 size );

/*===========================================================================
  FUNCTION  smd_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c smd_malloc.

  @param[in] pMem    The reference to the region of memory to be freed.

  @return    0 if succesful.
             Negative value if an error occurred.
*/
/*=========================================================================*/
int32 smd_free( void *pMem );

/*===========================================================================
FUNCTION      smd_cs_event_create
===========================================================================*/
/**
  Initializes a Critical Section suitable for being locked from 
  taskless SMDL's interrupt context.

  @return   The critical section, as a void pointer.
===========================================================================*/
void *smd_cs_event_create(void);

#ifdef __cplusplus
}
#endif

#endif /* SMD_OS_COMMON_H */
