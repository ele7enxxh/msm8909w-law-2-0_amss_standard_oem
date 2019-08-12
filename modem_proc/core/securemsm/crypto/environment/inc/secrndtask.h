#ifndef SECRNDTASK_H
#define SECRNDTASK_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   RND T A S K
                             
GENERAL DESCRIPTION
  This is the task level file for the security services. It defines the
  timers, signals, queues and other necessary handlings for the security rnd
  services task.

EXTERNALIZED FUNCTIONS
  secrnd_task()                     - entry point for the SEC task.
  secrnd_fill_rnd_data()            - gets random data from the searcher task
  secrnd_get_random()               - returns variable length random data
  secrnd_get_random_set_cb()        - allows caller to register callback functions, when SECRND not
                                      initialized
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  MC must initialize the task and set the start signal for this task, 
  before any other functions can be called.

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/environment/inc/secrndtask.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Added a wrapper secrnd_wrapper_get_random_set_cb stub 
                   function returns E_NOT-SUPPORTED
11/25/08    rv     added a new API that will allow tasks that come up before 
                   SECRND to register a CB, which can be fired after SECRND 
                   task starts up
03/21/07   df      changed IPC_SIG definition to eliminate poss L4 collision
04/04/06   df      simplified secrnd task - eliminated two signals
03/27/06   df      changes to support seed update to SFS/NV
03/18/06   df      created

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifdef FEATURE_SECRND
#include "rex.h"
#include "secerrno.h"
#include "secapi.h"
#include "nv_items.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Sec RND Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the security services code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SECRND_START_SIG           0x8000   /* Reserved for TASK_START_SIG     */

#define SECRND_STOP_SIG            0x4000   /* Reserved for TASK_STOP_SIG      */

#define SECRND_OFFLINE_SIG         0x2000   /* Reserved for TASK_OFFLINE_SIG   */

/* The following sigs are all okay, and can be allocated freely */
#define SECRND_GEN_RANDOM_DATA_SIG 0x0001   /* signal set we need to generate Random data */

#define SECRND_NV_CMD_SIG          0x0004   /* signal when NV interaction done */

#define SECRND_CSPRNG_SIG          0x0008   /* signal for CSPRNG */

#define SECRND_IPC_SIG             0x0100   /* secrnd IPC signal */   

/*---------------------------------------------------------------------------
  csprng definitions
---------------------------------------------------------------------------*/
#define SEC_CSPRNG_SEED_MIN_ONES  NV_SEC_CSPRNG_INIT_SEED_LEN        // in bits
#define SEC_CSPRNG_SEED_MAX_RUN_LEN  (NV_SEC_CSPRNG_INIT_SEED_LEN*2) // in bits
#define SEC_CSPRNG_INIT_SEED_LEN  64    // must be power of 2
#define SEC_CSPRNG_SHORT_SEED_LEN 16    // must be power of 2
#define SEC_CSPRNG_LONG_SEED_LEN  1024  // must be power of 2
#define SEC_CSPRNG_USAGE_COUNT    32    // must be power of 2


#include "rex.h"
typedef rex_crit_sect_type secrnd_mutex_type;
#define SECRND_MUTEX_INIT()  rex_init_crit_sect(&secrnd.mutex)
#define SECRND_MUTEX_ENTER() rex_enter_crit_sect(&secrnd.mutex)
#define SECRND_MUTEX_EXIT()  rex_leave_crit_sect(&secrnd.mutex)
#define SECRND_BUF_SIZE      SEC_CSPRNG_LONG_SEED_LEN

#define SECRND_UPDATE_INTERVAL 600  // 10 minutes

typedef enum
{
  SECRND_NO_SEED = 0,
  SECRND_INIT_SEED_LEN  = SEC_CSPRNG_INIT_SEED_LEN,
  SECRND_SHORT_SEED_LEN = SEC_CSPRNG_SHORT_SEED_LEN,
  SECRND_LONG_SEED_LEN  = SEC_CSPRNG_LONG_SEED_LEN
}secrnd_seed_type;

typedef struct
{
  uint8    buf[SECRND_BUF_SIZE];
  uint32   stime;      /* last time csprng was seeded */
  secrnd_mutex_type mutex;  /* mutex */
  secrnd_seed_type seed_len;
  uint16   start;      /* start pointer */
  uint16   end;        /* end pointer */
  uint16   usage;      /* random usage count */
  uint16   err_cnt;    /* sfs or other error count */
  boolean  full;       /* indicates the buffer is full with random data */
  boolean  started;    /* internal csprng flag */
  boolean  secure;     /* not secure until read value is from entropy source */
  boolean  tts;        /* time to seed flag */
  boolean  update;     /* update seed value into sfs or nv */
  boolean  done;       /* for compat w/1x */
}secrnd_type;


/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SECRND_FILL_RND_DATA

DESCRIPTION
  This function gets random data from the searcher task and merges it into
  the random data buffer storage area.  It notifies the sec task to seed the
  csprng if there is enough data, and if a "time to seed" flag has been set.

PARAMETERS
  rnd_data_ptr - Pointer to collected random data
  rnd_data_len - Length of collected random data in bytes

DEPENDENCIES
  The task calling this function is assumed to have higher priority
  than the tasks using the random data. 

RETURN VALUE    
  none

SIDE EFFECTS    
  Modified the random data buffer.
  
===========================================================================*/

void secrnd_fill_rnd_data
(
  uint8*  rnd_data_ptr,
  uint16  rnd_data_len
);

/*===========================================================================

FUNCTION SECRND_GET_RANDOM_DATA

DESCRIPTION     
  This function is used to obtain a random number for Security
  layer encryption functions.  

  Now using csprng as random source;  csprng is seeded periodically
  by searcher data as before;

PARAMETERS
  random_ptr - Buffer to be filled in with the random data.
  random_len - Amount of Random Data in bytes

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS     if success
  E_BAD_ADDRESS if NULL pointer was passed

SIDE EFFECTS  
  None
===========================================================================*/  
secerrno_enum_type secrnd_get_random_data(uint8* random_ptr, uint16 random_len); 

/*===========================================================================

FUNCTION SECRND_GET_RANDOM_DATA_CRITICAL

DESCRIPTION     
  This function is used to obtain a random number for Security
  layer encryption functions.  

  Now using csprng as random source;  csprng is seeded periodically
  by searcher data as before;

PARAMETERS
  random_ptr - Buffer to be filled in with the random data.
  random_len - Amount of Random Data in bytes

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS     if success
  E_BAD_ADDRESS if NULL pointer was passed

SIDE EFFECTS  
  None
===========================================================================*/  
secerrno_enum_type secrnd_get_random_data_critical(uint8* random_ptr, uint16 random_len); 

/*===========================================================================

FUNCTION SECRND_GENERATE_RANDOM_DATA_CB

DESCRIPTION     
  This CB function is used to signal the Random Task to generate and store random
  data in the random_data_buff.

PARAMETERS
  None

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS     if success

SIDE EFFECTS  
  None
===========================================================================*/ 
secerrno_enum_type secrnd_fill_random_buffer_cb(void); 

/*===========================================================================

FUNCTION SECRND_GET_RANDOM

DESCRIPTION     
  This function is used to obtain a random number for Security
  layer encryption functions.  

  Now using csprng as random source;  csprng is seeded periodically
  by searcher data as before;

PARAMETERS
  random_ptr - Buffer to be filled in with the random data.
  random_len - Amount of Random Data in bytes

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS     if success
  E_BAD_ADDRESS if NULL pointer was passed

SIDE EFFECTS  
  None
===========================================================================*/
secerrno_enum_type secrnd_get_random(uint8* random_ptr, uint16 random_len);

/*===========================================================================

FUNCTION SECRND_GET_RANDOM_SET_CB()

DESCRIPTION
  Register a callback function to be fired when SECRND task
  has completed initialization
    
DEPENDENCIES

PARAMETERS
  get_random_cb - user defined callback function
  usr_data      - user defined data

RETURN VALUE
    E_SUCCESS       if successful
    E_INVALID_ARG   if CB FP is null
    E_NO_MEMORY     reached MAX number of CB FP that can be registered 
    E_FAILURE       if the SECRND task has already  been initialized

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secrnd_get_random_set_cb
(
  secapi_get_random_cb_fptr_type  get_random_cb,
  void*                           usr_data
);
/*~ FUNCTION secapi_get_random_set_cb */   

/*===========================================================================

FUNCTION SECRND_TASK

DESCRIPTION
  This function is the entry point and main processing loop for the security
  services task.  This function contains the infinite while loop.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void secrnd_task
(
  dword dummy                            /* unused dummy var */
);

#endif /* FEATURE_SEC */

/*===========================================================================

FUNCTION SECRND_WRAPPER_GET_RANDOM_SET_CB()

DESCRIPTION
  A wrapper stub function for SECRND_GET_RANDOM_SET_CB()
  Returns E_NOT_SUPPORTED when Feature Secrnd not defined.
  This function should be outside FEATURE_SECRND wrap.
    
DEPENDENCIES

PARAMETERS
  get_random_cb - user defined callback function
  usr_data      - user defined data

RETURN VALUE
    E_SUCCESS       if successful
    E_INVALID_ARG   if CB FP is null
    E_NO_MEMORY     reached MAX number of CB FP that can be registered 
 	E_FAILURE       if the SECRND task has already  been initialized
 	E_NOT_SUPPORTED if Feature Secrnd not defined.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secrnd_wrapper_get_random_set_cb
(
  secapi_get_random_cb_fptr_type  get_random_cb,
  void*                           usr_data
);
/*~ FUNCTION secrnd_wrapper_get_random_set_cb */
#endif /* SECTASK_H */
