#ifndef SECCRYPTS_H
#define SECCRYPTS_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
              C R Y P T O G R A P H I C    I N T E R F A C E

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This is the header file for the ARM-QDSP2 Cryptographic Interface. It
  defines the external interfaces for key exchange, message digest, and
  encrypt/decrypt for various algorithms. Those applications who want to
  use crypto services should include this header file.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2003-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/seccrypts.h_v   1.0   03 Sep 2003 11:13:00   omichael  $
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccrypts.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/10   nk      Shared Library Framework - Cleaning up FEATURE_SEC wrap.
06/28/04   jay     Removed #include self
06/04/03   om      Fixed mac_info for cipher params.
06/02/03   om      Added debug shortcuts to enc structures.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "secerrno.h"


/*===========================================================================

                            TYPE DECLARATIONS

===========================================================================*/
/* ------------------------------------------------------ */
/* State definitions for a crypto instance                */
/* ------------------------------------------------------ */
typedef enum
{
  SECCRYPT_INST_UNUSED       = 0,
  SECCRYPT_INST_ALLOCATED    = 1,
  SECCRYPT_INST_INITIALIZED  = 2,
  SECCRYPT_INST_PARAM_READY  = 3,
  SECCRYPT_INST_EXEC_INIT    = 4,
  SECCRYPT_INST_EXEC_START   = 5,
  SECCRYPT_INST_EXEC_CONT    = 6,
  SECCRYPT_INST_EXEC_FINISH  = 7,
  SECCRYPT_INST_FAILURE      = 8,
  SECCRYPT_INST_MAX_STATE,

  /* These names are used internally to divide the state function */
  /* tables into common and platform-specific functions.          */
  SECCRYPT_INST_EXEC_STATES     = SECCRYPT_INST_EXEC_INIT,
  SECCRYPT_INST_NUM_EXEC_STATES = 4

} seccrypts_instance_state_enum_type;
 
/* ------------------------------------------------------ */
/* State transition table data                            */
/* ------------------------------------------------------ */
/* State transition function signature */
typedef secerrno_enum_type (*seccrypts_state_x_func_type)
  (
    seccrypt_instance_type*              instance_ptr,
    seccrypts_instance_state_enum_type   old_state,
    seccrypts_instance_state_enum_type   new_state
  );

/* State entry and exit transition functions */
typedef struct
{
  /* Function pointers for transition functions */
  seccrypts_state_x_func_type  entry_fptr;
  seccrypts_state_x_func_type  exit_fptr;

  /* Transition is inherited from parent */
  boolean                      inherited;

} seccrypts_state_x_funcs_type;

/* Definition of a state transition table */
typedef seccrypts_state_x_funcs_type seccrypts_state_x_funcs_table_type
          [SECCRYPT_INST_MAX_STATE];

/* ------------------------------------------------------ */
/* Tables to define allowed state transitions             */
/* ------------------------------------------------------ */
/* List of valid transitions for a specific state */
typedef boolean seccrypts_state_x_allowed_table_entry_type
          [SECCRYPT_INST_MAX_STATE];

/* Definition of a table for transition validities */
typedef seccrypts_state_x_allowed_table_entry_type
          seccrypts_state_x_allowed_table_type[SECCRYPT_INST_MAX_STATE];

/* ------------------------------------------------------ */
/* Complete state information of an instance              */
/* ------------------------------------------------------ */
struct seccrypts_state_info_s
{
  seccrypts_instance_state_enum_type           state;  
  seccrypts_state_x_funcs_type*                x_funcs_ptr;
  seccrypts_state_x_allowed_table_entry_type*  x_allowed_ptr;
};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                            MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECCRYPTS_SET_SM_TYPE()

DESCRIPTION
  This function initializes the state transition table of an instance
  according to the instance type.

DEPENDENCIES
  The instance type has to be initialized before.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypts_set_sm_type
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTS_SET_STATE_FUNCS()

DESCRIPTION
  This function sets / overwrites the state transition functions
  for a given instance and state.
  This is used to initialize sub-parts of the state machine that
  depend on the execution platform.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypts_set_state_funcs
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  state,
  seccrypts_state_x_func_type         entry_fptr,
  seccrypts_state_x_func_type         exit_fptr
);

/*===========================================================================

FUNCTION SECCRYPTS_INHERIT_X_TABLES()

DESCRIPTION
  This function sets / overwrites the state transition functions
  for a given instance and state.
  This is used to initialize sub-parts of the state machine that
  depend on the execution platform.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seccrypts_inherit_x_tables
(
  seccrypts_state_x_funcs_type*                child_x_funcs_ptr,
  seccrypts_state_x_allowed_table_entry_type*  child_x_allowed_ptr,
  seccrypts_state_x_funcs_type*                parent_x_funcs_ptr,
  seccrypts_state_x_allowed_table_entry_type*  parent_x_allowed_ptr
);

/*===========================================================================

FUNCTION SECCRYPTS_CHANGE_STATE()

DESCRIPTION
  This function attempts to change the instance state to the new
  state specified if the transition is allowed.
  A failure of the exit function causes the state to remain unchanged.

DEPENDENCIES
  None

RETURN VALUE
  Standard Security Services error code (see secerrno.h):

    E_SUCCESS      - State changed successfully
    E_NOT_ALLOWED  - State transition is not allowed by SM
    E_AGAIN        - State exit condition not met, state unchanged
    E_FAILURE      - Transition to new state failed, undefined behavior

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type seccrypts_change_state
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  new_state
);

#endif /*SECCRYPTS_H */
