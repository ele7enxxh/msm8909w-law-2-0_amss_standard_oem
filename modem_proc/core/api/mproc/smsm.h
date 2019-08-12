/*---------------------------------------------------------------------------
   SMSM.H  - Shared Memory Synchronization Mechanism
---------------------------------------------------------------------------*/

/**
  @file smsm.h
*/
/* Contains the SMSM external data structure and APIs. */


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMEM_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMEM_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smsm" group description 
         in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/smsm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/31/13   bt      Simplify and sanitize smsm_set_reset, to avoid exceptions, 
                   errors and overhead after an error has already occurred.
02/21/12   bt      Remove smsm_poll_timeout, no timetick calls in SMSM now.
07/18/11   bm      Removal of deprecated smsm_shim API layer
05/18/11   bm      Changed prototype for smsm_init to conform to RCInit 
                   requirement
05/05/11   tl      Moved enumerations and defines to new header file
04/13/11   bt      Adding 2 WLAN SMSM bits.
04/07/11   bt      Added SMSM_RIVA AND SMSM_DSPS hosts, for 8660 NH.3.0+, 8960.
02/25/11   bt      Remove all old DEM entries and bits, add SMSM_VOICE_DEBUG 
                   bit and entries for new DSPS and RIVA processors.  Move 
                   smsm_host_type and smsm_entry_type back to target header.
08/12/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
06/29/10   hwu     Added SMSM_SYSTEM_REBOOT_USR and SMSM_SYSTEM_POWERDOWN_USR 
                   to let modem initiate the system reboot/powerdown.  
03/31/10   hwu     Added SMSM_MM_QDSP_RESET for Q6. 
02/16/10   tl/esh  Added Doxygen tags
01/06/10   esh     Removed return type for smsm_set_reset
11/19/09   rs      CMI updates.
11/19/09   sa      warning cleanup, sync-up with symbian
04/23/09   hwu     CMI phase I header split.
01/14/09   hwu     Added smsm_process_host_update. This can be called in ISR 
                   and task.
01/07/09   hwu     Fixed the compiler warnings
01/05/09   hwu     Revert the SMSM bits cleanup changes to be backward compatible.
11/26/08   hwu     SMSM Bits cleanup.
11/20/08   bdh     Added support for Apps debug break/continue adding handling
                   SMSM_MODEM_BREAK and SMSM_MODEM_CONTINUE states. 
01/05/08   hwu     Initial version
===========================================================================*/
#ifndef SMSM_H
#define SMSM_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "smsm_type.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

#define SMSM_THIS_HOST      (smsm_shim_get_host_entry(TRUE))
#define SMSM_OTHER_HOST     (smsm_shim_get_host_entry(FALSE))

/** @addtogroup smsm 
@{ */

/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/
/**
  SMSM state type */
typedef uint32  smsm_state_type;

/**
  Callback type that is used to register with SMSM for state-change
  notifications.

  @note The callback is done in interrupt-service task context, so processing
  must be kept to a minimum.
  
  @param[in] entry       State entry.
  @param[in] prev_state  Previous state.
  @param[in] curr_state  Current (new) state.
  @param[in] cb_data     User data provided when the callback was registered.

  @return
  None.

  @sa
  smsm_cb_register, smsm_cb_deregister
*/
typedef void (*smsm_cb_type) (smsm_entry_type entry,
                              uint32 prev_state,
                              uint32 curr_state,
                              void * cb_data);

/** @} */ /* end_addtogroup smsm */


/*--------------------------------------------------------------------------
  Function declarations
---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup smsm 
@{ */

/*===========================================================================
  FUNCTION  smsm_init
===========================================================================*/
/**
  Allocates the SMSM-related items from the shared memory and
  initializes the local data structure. It hooks into the SMSM interrupt and 
  initializes the SMSM_INIT bit.
  
  @param None

  @return
  None.

  @dependencies
  Shared memory must have been initialized by calling smem_init().
*/
/*=========================================================================*/
extern void smsm_init( void );

/*===========================================================================
  FUNCTION  smsm_state_set
===========================================================================*/
/**
  Sets the local processor states and interrupts the other processors as 
  needed. It then processes the local callbacks associated with the state 
  changes.
  
  If the modified state is not owned by the local processor, the function
  generates an err_fatal event.

  @note Depending on the interrupt mask, the state changes may be forwarded to 
  the other hosts.

  @param[in] entry    State entry.
  @param[in] states   States to be set.

  @return
  None.

  @dependencies
  SMSM must have been initialized by calling smsm_init().
*/
/*=========================================================================*/
extern void smsm_state_set
(
  smsm_entry_type   entry,
  uint32            states
);

/*===========================================================================
  FUNCTION  smsm_state_clr
===========================================================================*/
/**
  Clears the local processor states and interrupts the other processors as 
  needed. It then processes the local callbacks associated with this state 
  changes.

  @note Depending on the interrupt mask, the state changes may be forwarded to 
  the other hosts.
  
  @param[in]  entry    State entry.
  @param[in]  states   States to be cleared.

  @return None.

  @dependencies
  SMSM must have been initialized by calling smsm_init().
*/
/*=========================================================================*/
extern void smsm_state_clr
(
  smsm_entry_type   entry,
  uint32            states
);

/*===========================================================================
  FUNCTION  smsm_state_modify
===========================================================================*/
/**
  Clears and sets the local processor state and interrupts the other
  processors as needed. It then processes the local callbacks associated 
  with these state changes.

  @note Depending on the interrupt mask, the state changes may be forwarded to 
  the other hosts.

  @param[in]  entry        State entry.
  @param[in]  clr_states   States to be cleared.
  @param[in]  set_states   States to be set.

  @return
  None.

  @dependencies
  SMSM must have been initialized by calling smsm_init().
*/
/*=========================================================================*/
extern void smsm_state_modify
(
  smsm_entry_type   entry,
  uint32            clr_states,
  uint32            set_states
);

/*===========================================================================
  FUNCTION  smsm_state_get
===========================================================================*/
/**
  Reads an entry state from the shared memory. It returns the entry state out 
  of the shared memory.

  @param[in] entry     State entry.

  @return
  State of the specified entry.

  @dependencies
  SMSM must have been initialized by calling smsm_init().
*/
/*=========================================================================*/
extern uint32 smsm_state_get( smsm_entry_type entry );

/*==============================================================================
  FUNCTION:  smsm_set_reset
===============================================================================*/
/**
  This function sets the SMSM_RESET state and clears the specified states.
  
  It then sends an SMSM interrupt to all processors registered for changes in
  the local SMSM_RESET bit.  This is usually Apps, to handle Error cases and
  Subsystem Restart.

  @param[in]    SMSM bit states to be cleared

  @return       None

  @note
  1.) This function should only be called from single-threaded environment, 
  it's NOT thread-safe.
  2.) It must not cause any further fatal exceptions, errors, or assertions.
  3.) It does no logging or other extraneous overhead.

  @sideeffects
  May notify remote subsystems that the local processor has hit an error 
  condition, which may trigger either a system crash or a subsystem restart.
*/
/*============================================================================*/
extern void smsm_set_reset( uint32 state );

/*===========================================================================
  FUNCTION  smsm_cb_register
===========================================================================*/
/**
  Registers a client callback function that is called when the specified bits 
  in a specified entry have changed.
  
  By default, the interrupt mask for each processor is set to zero, which
  disables entry change notifications. As callback functions are 
  registered, their masks are logically OR'ed and used as the interrupt
  mask for the corresponding entry. When an entry changes, this interrupt 
  mask is checked for each processor; if it has not been set, the processor 
  does not have its interrupt triggered. This prevents sending interrupts that 
  do not result in any callback notifications.

  @param[in] entry      State entry.
  @param[in] mask       Bits in the entry.
  @param[in] cb_func    Client-specified callback function.
  @param[in] cb_data    Client-specified callback data.

  @return
  0 -- Callback could not be registered, which may occur if the callback table 
    is full.\n
  1 -- Callback was successfully registered.

  @dependencies
  SMSM must have been initialized by calling smsm_init().
*/
/*=========================================================================*/
extern int smsm_cb_register
(
  smsm_entry_type     entry,
  uint32              mask,
  smsm_cb_type        cb_func,
  void               *cb_data
);

/*===========================================================================
  FUNCTION  smsm_cb_deregister
===========================================================================*/
/**
  Deregisters a client callback function and updates the interrupt mask for
  the entry.

  @param[in] entry     State entry.
  @param[in] mask      Bits in the entry.
  @param[in] cb_func   Client-specified callback function.

  @return
  0 -- Callback was not found in the callback table.\n
  1 -- Callback was successfully deregistered.

  @dependencies
  SMSM must have been initialized by calling smsm_init().\n
  The callback must have been registered by calling smsm_cb_register().
*/
/*=========================================================================*/
extern int smsm_cb_deregister
(
  smsm_entry_type     entry,
  uint32              mask,
  smsm_cb_type        cb_func
);

/** @} */ /* end_addtogroup smsm */

/*===========================================================================
   The following is for backward compatibility - To be removed
===========================================================================*/

/**
  Returns the smsm_entry_type corresponding to a host's primary entry. This
  is used by the SMSM_THIS_HOST and SMSM_OTHER_HOST macros.
  
  @note   SMSM must have been initialized via smsm_init().
 
  @param  this_host - Is the requested host SMSM_THIS_HOST?

  @return Per the following table:

        Host     this_host  Return
        ----     ---------  ------
        Apps     FALSE      SMSM_MODEM_STATE
        Apps     TRUE       SMSM_APPS_STATE
        Modem    FALSE      SMSM_APPS_STATE
        Modem    TRUE       SMSM_MODEM_STATE
        Other               SMSM_INVALID_ENTRY

*/
smsm_entry_type smsm_shim_get_host_entry
(
  boolean             this_host
);

#ifdef __cplusplus
}
#endif

#endif /* SMSM_H */
