/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides mpss related error reporting services for both 
  fatal and non-fatal errors.  This module is not a task, but rather 
  a set of callable procedures which run in the context of the calling 
  task.

Copyright (c) 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_target_non_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
11/19/14   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "erri.h"
#include "err_decompress.h"
#include "msg.h"
#include "tms_utils.h"

#ifdef FEATURE_SMEM
  #include "smem.h"
#endif /* FEATURE_SMEM */

#include "err_smem_log.h"


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/

extern void err_log_ssr_failure_reason(void); /* Defined in err.c */
extern void err_fatal_jettison_core (
  unsigned int line,       /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format,    /* format string */
  uint32 param1,
  uint32 param2,
  uint32 param3
);



/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
extern coredump_type coredump;

const err_cb_ptr err_preflush_internal[] =
{
  err_log_ssr_failure_reason,
  err_info_to_smem_buffer,

  /* NULL must be last in the array */
  NULL
};

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_TARGET_INIT

DESCRIPTION
  This function err service for non mpss based targets

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

void err_target_init (void)
{
 
} /* err_target_init */



/*===========================================================================

FUNCTION ERR_TARGET_GET_TIME

DESCRIPTION
  Updates coredump with Wall clock time if available.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .
============================================================================*/
void err_target_get_time(void)
{
} /* err_target_get_time */


/*===========================================================================

FUNCTION       ERR_TARGET_DECOMPRESS_AND_F3_TRACE

DESCRIPTION
  Decompresses the error fatal message and pushes it through F3 trace.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_target_update_coredump_and_f3_trace( err_fatal_params_type * params )
{
  err_fatal_jettison_core (params->msg_const_ptr->line,
                           params->msg_const_ptr->fname,
                           params->msg_const_ptr->fmt,
                           params->param1, 
                           params->param2, 
                           params->param3);
} /* err_target_decompress_and_f3_trace */

	
void err_put_log (word line, const char *file_ptr)
{
  //err logging is not supported in this build
}
