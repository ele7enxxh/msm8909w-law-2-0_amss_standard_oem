#ifndef NVRUIM_REQUEST_H
#define NVRUIM_REQUEST_H
/*===========================================================================


              N V  R-U I M  R E Q U E S T  P R O C  F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014 QUALCOMM Technologies, Incorporated and its
licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/nvruim/src/nvruim_request.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/10/14   av      Clean up NVRUIM crit sect macro definition
08/28/14   av      Change nvruim crit sect to mutex-based from pimutex-based
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
01/10/14   df      Use v2.0 diag macros
11/01/13   at      Support for guaranteed response for each NVRUIM request
10/28/13   kb      Add offtarget support to check mutex is owned or not
05/13/13   vs      Initial revision
=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ===========================================================================
   MACRO:       NVRUIM_MSG_CRIT_SECT_ENTER_DEBUG_LOG
   DESCRIPTION: To print debug msg for entering NVRUIM critical section
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define NVRUIM_MSG_CRIT_SECT_ENTER_DEBUG_LOG                            \
    UIM_MSG_HIGH_0("Entering NVRUIM critical section");
#else
  #define NVRUIM_MSG_CRIT_SECT_ENTER_DEBUG_LOG
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       NVRUIM_MSG_CRIT_SECT_LEAVE_DEBUG_LOG
   DESCRIPTION: To print debug msg for leaving NVRUIM critical section
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define NVRUIM_MSG_CRIT_SECT_LEAVE_DEBUG_LOG                            \
    UIM_MSG_HIGH_0("Leaving NVRUIM critical section");
#else
  #define NVRUIM_MSG_CRIT_SECT_LEAVE_DEBUG_LOG
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       NVRUIM_MSG_CRIT_SECT_ENTER_DEBUG_LOG
   DESCRIPTION: To print debug msg for entering NVRUIM critical section
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define NVRUIM_MSG_TIMER_CRIT_SECT_ENTER_DEBUG_LOG                          \
    UIM_MSG_HIGH_0("Entering NVRUIM Timer critical section");
#else
  #define NVRUIM_MSG_TIMER_CRIT_SECT_ENTER_DEBUG_LOG
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       NVRUIM_MSG_CRIT_SECT_LEAVE_DEBUG_LOG
   DESCRIPTION: To print debug msg for leaving NVRUIM critical section
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define NVRUIM_MSG_TIMER_CRIT_SECT_LEAVE_DEBUG_LOG                           \
    UIM_MSG_HIGH_0("Leaving NVRUIM Timer critical section");
#else
  #define NVRUIM_MSG_TIMER_CRIT_SECT_LEAVE_DEBUG_LOG
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       NVRUIM_ENTER_CRIT_SECT
   DESCRIPTION: To enter NVRUIM critical section
   INPUT:       crit_sect_ptr:    critical section pointer
   ===========================================================================*/
#define NVRUIM_ENTER_CRIT_SECT                                                 \
  NVRUIM_MSG_CRIT_SECT_ENTER_DEBUG_LOG                                         \
  qurt_mutex_lock(nvruim_get_nvruim_crit_sect());

/* ===========================================================================
   MACRO:       NVRUIM_LEAVE_CRIT_SECT
   DESCRIPTION: To leave NVRUIM critical section
   INPUT:       crit_sect_ptr:    critical section pointer
   ===========================================================================*/
#define NVRUIM_LEAVE_CRIT_SECT                                                 \
  qurt_mutex_unlock(nvruim_get_nvruim_crit_sect());                            \
  NVRUIM_MSG_CRIT_SECT_LEAVE_DEBUG_LOG

/* ===========================================================================
   MACRO:       NVRUIM_ENTER_TIMER_CRIT_SECT
   DESCRIPTION: To enter NVRUIM's timer critical section
   INPUT:       crit_sect_ptr:    timer critical section pointer
   ===========================================================================*/
#define NVRUIM_ENTER_TIMER_CRIT_SECT                                           \
  NVRUIM_MSG_TIMER_CRIT_SECT_ENTER_DEBUG_LOG                                   \
  rex_enter_crit_sect(nvruim_get_nvruim_timer_crit_sect());

/* ===========================================================================
   MACRO:       NVRUIM_LEAVE_TIMER_CRIT_SECT
   DESCRIPTION: To leave NVRUIM's Timer critical section
   INPUT:       crit_sect_ptr:    timer critical section pointer
   ===========================================================================*/
#define NVRUIM_LEAVE_TIMER_CRIT_SECT                                           \
  rex_leave_crit_sect(nvruim_get_nvruim_timer_crit_sect());                    \
  NVRUIM_MSG_TIMER_CRIT_SECT_LEAVE_DEBUG_LOG

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION NVRUIM_IS_TASK_NVRUIM

DESCRIPTION
  This function is called to find out if the passed tcb ptr matches the nvruim
  task pointer or not.

DEPENDENCIES
  nvruim_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
===========================================================================*/
boolean nvruim_is_task_nvruim (
  rex_tcb_type *in_tcb_ptr
);

/*===========================================================================

FUNCTION NVRUIM_CHECK_RTRE_CONFIG

DESCRIPTION
Check to see RTRE CONFIG is Initialized

RETURN VALUE
TRUE - If Config is Initilaized
FALSE - If Config  is not intialized

DEPENDENCIES/SIDE EFFECTS/NOTES
None

===========================================================================*/
boolean nvruim_check_rtre_config (
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
);

/*===========================================================================

FUNCTION NVRUIM_GET_NVRUIM_CRIT_SECT

DESCRIPTION
  Get the pointer to the NVRUIM critical section

RETURN VALUE
  qurt_mutex_t*

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
qurt_mutex_t* nvruim_get_nvruim_crit_sect (
  void
);

/*===========================================================================

FUNCTION NVRUIM_GET_NVRUIM_TIMER_CRIT_SECT

DESCRIPTION
  Get the pointer to the NVRUIM Timer critical section

RETURN VALUE
  rex_crit_sect_type*

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
rex_crit_sect_type* nvruim_get_nvruim_timer_crit_sect (
  void
);

/*===========================================================================

FUNCTION NVRUIM_GET_RTRE_CONTROL_VALUE

DESCRIPTION
  This function returns the global RTRE control value that is set based on the
  NV 855 RUIM Run-Time Enable (RTRE)  configuration.

RETURN VALUE
  nv_rtre_control_type: NV_RTRE_CONTROL_USE_RUIM or NV_RTRE_CONTROL_NO_RUIM

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_rtre_control_type nvruim_get_rtre_control_value (
  void
);

/*===========================================================================

FUNCTION NVRUIM_PROCESS_NV_ACCESS

DESCRIPTION
  This function uses the NVRUIM read, write APIs to attempts to queue requests to MMGSDI
  and return the appropriate response to NV

RETURN VALUE
  nv_stat_enum_type: Returned value depends on the value returned by NVRUIM
  core logic. NV_RUIM_NOT_SUPPORTED_S is returned if NVRUIM returns
  NV_RUIM_ITEM_NOT_SUPPORTED. If NVRUIM returns NV_RUIM_SUPPORTS_ITEM then
  the operation status returned by NVRUIM is directly returned.

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_nv_access (
  nv_cmd_type *nv_cmd_ptr
);

/*===========================================================================

FUNCTION NVRUIM_PROCESS_NV_REQ

DESCRIPTION
  This function processes the following requests from NV task:
  NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL
    - Verify if and NV item maps to EF on card and if so access (read/write) it
  NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL
    - Update/fetch rtre_control_value
  NV_RUIM_CMD_RTRE_NAM_IOCTL
    - Update/fetch nv_rtre_use_ruim_for_nam
  NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL
    - Update/fetch nv_rtre_polling_control
  NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL
    - Update/fetch nv_rtre_polling_for_nam
  NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL
    - Update/fetch nv_max_size_of_roaming_list
  NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL
    - Process RTRE configuration command

RETURN VALUE
  Of the enumerations defined in nv_stat_enum_type this function will return:
    NV_DONE_S      - Request processed or to be processed by MMGSDI
    NV_BADPARM_S   - Bad parameter in the command block
    NV_RUIM_NOT_SUPPORTED_S - Indicates an NV item not supported in RUIM

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_nv_req (
  nv_ruim_enum_type  type,       /* Command type to be processed */
  void              *in_ptr,     /* Input buffer for the command type */
  void              *out_ptr     /* Output buffer for the command type */
);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* NVRUIM_REQUEST_H */
