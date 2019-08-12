#ifndef SECCRYPTHW_H
#define SECCRYPTHW_H

/*===========================================================================

DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


 Copyright (c) 2009 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccrypthw.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/10   ejt     removed DSP signal info 
06/16/10   nk      Shared Library Framework - Cleaning up FEATURE_SEC_HW_CRYPTO wrap.
06/18/09   bm      second version to suite CE_DAL, CE_HAL approach
                   funtional changes when initing, releasing the HW platform
11/12/07   df      created
===========================================================================*/

#include "comdef.h"
#define SECCRYPTHW_DEBUG

/*===========================================================================

                         CONSTANTS AND DEFINITIONS

===========================================================================*/
/* Task context and wait function for HW CE related signals */
#define SECCRYPTHW_TCB_PTR         &sec_tcb
#define SECCRYPTHW_WAIT            sec_wait

/* Time to wait before disabling the HW CE [ms] */
#define SECCRYPTHW_DISABLE_WAIT_TIME 20

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/* General command structure definition */
typedef struct
{
   /* quick check to see if we are in the middle of an op 
    *  1 = running/continue
    *  0 = idle
    */
   uint32   context_state;

   /* dummy pad bytes to add to the data buffer to make it 16 byte mult */
   uint32   pad[4];
} seccrypthw_ce_cmd_type;

struct seccrypthw_common_data_s
{
   seccrypthw_ce_cmd_type   ce;

   struct
   {
      uint32 encrypt;
      uint32 decrypt;
   } context_state;
};

/* ce applet types */
typedef enum
{
   SECCRYPTHW_NO_APPLET = 0,
   SECCRYPTHW_CIPHER_APPLET,
   SECCRYPTHW_HASH_APPLET
} seccrypthw_applet_enum_type;

/* ce hw states */
typedef enum
{
   SECCRYPTHW_DISABLED_STATE = 0,
   SECCRYPTHW_BUSY_STATE,
   SECCRYPTHW_IDLE_STATE
} seccrypthw_state_enum_type;

/* crypto engine state vars */
typedef struct
{
   boolean initialized;
   seccrypthw_state_enum_type state;

   /* Current and previous image / applet */
   seccrypthw_applet_enum_type  applet;
   seccrypthw_applet_enum_type  previous_applet;

   /* current instance */
   seccrypt_instance_type *instance_ptr;
   uint32                 user_mask;

   /* DSP timers for execution and pending user timeouts */
   rex_timer_type         exec_timer;
   rex_timer_type         user_timer;
   uint32                 user_timeout;
}seccrypthw_type;

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
/* Global declarations */
extern seccrypthw_type  seccrypthw;

/* Queue of commands for the HW platform */
extern const seccrypt_cmd_q_type   seccrypthw_cmd_q;

/*===========================================================================

                           EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/
/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_CMD_SIG

 DESCRIPTION 
   This function processes the signal of the CE HW crypto command queue.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_cmd_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_EXEC_TIMER_SIG

 DESCRIPTION 
   This function handles the execution timeout to abort a CE HW operation.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_exec_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_USER_TIMER_SIG

 DESCRIPTION 
   This function handles the user timeout to disable CE clk regime

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_user_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_OP_COMPLETE_SIG

 DESCRIPTION 
   This function handles the hw op complete, to resume operation

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_op_complete_sig( void );

/*===========================================================================

FUNCTION SECCRYPHW_POWERUP_INIT()

DESCRIPTION
  One-time initialization of DSP platform-specific data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccrypthw_powerup_init( void );

/*===========================================================================

FUNCTION SECCRYPTHW_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of HW CE platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccrypthw_platform_init(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECCRYPTHW_PLATFORM_RELEASE()

DESCRIPTION
  Cleanup and release of HW CE platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccrypthw_platform_release(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECCRYPTHW_RELEASE_APPLET()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccrypthw_release_applet
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state
);

/*==========================================================================
 FUNCTION SECCRYPTHW_SCHEDULE_OP

 DESCRIPTION 
   This function schedules an operation for the CE HW interface task.

 DEPENDENCIES 
   None

 RETURN VALUE 
   Security services error code.

 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type seccrypthw_schedule_op
(
seccrypt_cmd_enum_type   cmd,
seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTHW_ISSUE_APPLET_CMD()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccrypthw_issue_applet_cmd
(
seccrypt_instance_type*  instance_ptr
);

#endif  /* SECCRYPTHW_H */
