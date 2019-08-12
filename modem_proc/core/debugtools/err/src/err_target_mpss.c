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

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_target_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/14   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "erri.h"
#include "err_decompress.h"
#include "msg.h"
#include "sys_m.h"
#include "tms_utils.h"
#include "nv.h"
#include "smem.h"
#include "smem_log.h"

#include "stringl.h"  /* memscpy */
#include "time_svc.h"  /* time_get */

extern void dog_force_kick(void);

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
static void err_smem_log(void);
static void err_store_info(void);

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
err_data_type err_data;
err_decompress_struct_type err_decomp_struct;

extern coredump_type coredump;

#if defined(ERR_F3_TRACE_TO_SMEM)
  /* Pointer to buffer used for SMEM crash log */
  static char *err_smem_log_buf;
#endif /* ERR_F3_TRACE_TO_SMEM */

const err_cb_ptr err_preflush_internal[] =
{
  err_smem_log,
  err_store_info,
  err_log_ssr_failure_reason,
  /* NULL must be last in the array */
  NULL
};

/* Event defines for using SMEM to log what occurs during sleep and
 * power collapse.
 */
#define ERR_ERROR_FATAL                (SMEM_LOG_ERROR_EVENT_BASE + 1)
#define ERR_ERROR_FATAL_TASK           (SMEM_LOG_ERROR_EVENT_BASE + 2)



/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_TARGET_INIT

DESCRIPTION
  This function checks if NV has been built.  If so then it loads up the
  error log cache from NV, it initializes error counts and local variables,
  and it sets error services to online.  If NV has not been built then
  error log is not loaded and error services remain offline and error
  logging does not take place.  Any access to NV is performed here directly,
  without relying on the NV task.  Thus error service can be initialized
  and used early in the life of the DMSS and right after boot.  Note
  that if NV has not been built, error services provide no error logging
  capabilities.  However NV is built once, after initial factory startup,
  and rarely thereafter.  Thus except for the first ever powerup of the
  unit after NV build or re-build, error logging will take place as usual.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

void err_target_init (void)
{
  err_nv_log_init();

  #if defined(ERR_F3_TRACE_TO_SMEM)
    err_smem_log_buf = (char *)smem_alloc(SMEM_ERR_CRASH_LOG,ERR_DATA_MAX_SIZE);
  #endif /* ERR_F3_TRACE_TO_SMEM */

} /* err_target_init */

/*=========================================================================

FUNCTION ERR_SMEM_LOG

DESCRIPTION
  Creates error event in SMEM log containing filename and taskname.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .
===========================================================================*/
static void err_smem_log(void)
{
    /* this is intentionally uint32, not char, for smem macro */
    uint32 name[5];  
    rex_tcb_type *tcb_ptr = (rex_tcb_type*)coredump.os.tcb_ptr;
      size_t name_len;
      char rex_task_name[REX_TASK_NAME_LEN+1];

    ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_SMEM_LOG_PRE);
    if(tcb_ptr)
    {
        rex_get_task_name( tcb_ptr, rex_task_name, 
		         sizeof(rex_task_name), &name_len);

        name_len = MIN( sizeof(name), sizeof(rex_task_name) );
        strlcpy ((char*)name, rex_task_name, name_len);

      SMEM_LOG_EVENT6_STM(ERR_ERROR_FATAL_TASK,
        name[0], name[1], name[2], name[3], name[4], (uint32) tcb_ptr);

      name_len = 1 + strlen(coredump.err.filename);
      memscpy (name, sizeof(name), coredump.err.filename, name_len);
      SMEM_LOG_EVENT6_STM(ERR_ERROR_FATAL,
       name[0], name[1], name[2], name[3], name[4], coredump.err.linenum);
    }
}


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
  /* Get timestamp of error (early for accuracy) */
  time_get((unsigned long *)&coredump.err.timestamp);
} /* err_target_get_time */

/*===========================================================================

FUNCTION ERR_STORE_INFO

DESCRIPTION
  Writes some crash related information to a designated RAM buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
============================================================================*/
#define ERR_DATA_MAGIC_NUMBER       (uint64)0x5252452041544144ULL
static void err_store_info ()
{
  char *buf = NULL;

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_STORE_INFO_PRE);
  
  /* If magic numbers are present at the front of the buffer, then previous
   * data has not been written out yet. Return without doing anything in
   * this case. -- this is probably obsolete without nzi, but cheap protection
   */
  if(err_data.err_log.flag == ERR_DATA_MAGIC_NUMBER)
  {
    /* Buffer is already full */
    return;
  }
  err_data.err_log.flag = ERR_DATA_MAGIC_NUMBER;

  /* Copy data needed to store NV log */
  err_data.err_log.nv_log.line_num = coredump.err.linenum;
  err_data.err_log.nv_log.fatal = TRUE;
  (void) strlcpy((char *)err_data.err_log.nv_log.file_name,
					       (char *)coredump.err.filename,
                 MIN( sizeof(err_data.err_log.nv_log.file_name), sizeof(coredump.err.filename) ));

  buf = (char *) err_data.err_log.data;

  /* Interrupts are disabled at this point -- need to make sure we don't
   * timeout.  Use force_kick to avoid intlock attempt in isr (possibly)
   */
  dog_force_kick();
  (void) memset (buf, 0x00, ERR_DATA_MAX_SIZE);
  err_log_init(buf, ERR_DATA_MAX_SIZE);
  err_data.err_log.length = err_generate_log();

  #if defined (ERR_F3_TRACE_TO_SMEM)
  /* If this is the first error logged, copy it to the smem buffer */
  if(i==0)
  {
    /* If we have ptr to ERR smem log region, copy log buffer */
    if(err_smem_log_buf!=NULL)
    {
      memscpy((void*)err_smem_log_buf, ERR_DATA_MAX_SIZE, (void*)buf, ERR_DATA_MAX_SIZE);
    }
    //actual f3 trace is handled exclusively by diag now (using err_crash_cb_register)
  }
  #endif /* ERR_F3_TRACE_TO_SMEM */

  return;
} /* err_store_info */


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
  msg_const_type msg_blk;

  /* Decompress err struct */
  err_decompress_msg_const(params->msg_const_ptr, &err_decomp_struct);

  /* kick the dog */
  dog_force_kick();

  msg_blk.desc.line    = err_decomp_struct.msg_blk.line;
  msg_blk.desc.ss_id   = MSG_SSID_DFLT;
  msg_blk.desc.ss_mask = MSG_LEGACY_FATAL;
  msg_blk.fname   = err_decomp_struct.msg_blk.fname;
  msg_blk.fmt     = err_decomp_struct.msg_blk.fmt;
   
  /* Log failure reason */
  #if defined (FEATURE_SAVE_DEBUG_TRACE)
    ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_F3_TRACE_PRE);
    msg_save_3(&msg_blk, 
                 params->param1, 
                 params->param2, 
                 params->param3, 
                 NULL); 
  #endif /* FEATURE_SAVE_DEBUG_TRACE */

  err_fatal_jettison_core (err_decomp_struct.msg_blk.line,
                          err_decomp_struct.msg_blk.fname,
                          err_decomp_struct.msg_blk.fmt,
                          params->param1, 
                          params->param2, 
                          params->param3);

} /* err_target_update_coredump_and_f3_trace */

