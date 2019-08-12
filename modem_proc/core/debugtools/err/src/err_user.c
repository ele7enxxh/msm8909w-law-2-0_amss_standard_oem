/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error related functionality for root PD.

Copyright (c) 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_user.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "err_qdi_client.h"
#include "err_smem_log.h"


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_fatal_handler( void ) ;
void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);



/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void err_call_next_to_STM_CB(void);
extern void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);


/*===========================================================================

                      External Data Vairables

===========================================================================*/

extern err_fatal_params_type err_fatal_params;

/* Struct used to hold coredump data */
extern coredump_type coredump;


/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/

const err_cb_ptr err_preflush_internal[] =
{
  /* Function pointer for extended smem logging */
  err_info_to_smem_buffer,
  /* NULL must be last in the array */
  NULL
};
 
/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_PD_INIT

DESCRIPTION
  This function initializes error services and calls into target & process 
  based error intializing routine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  For Root PD on MPSS, Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

void err_pd_init (void)
{
  /* Initialize the extended smem logging buffer */
  err_smem_log_init();

  /* Calculate the offset from which smem log needs to be written.
     * (only for User PD)
     */
  err_smem_calculate_offset();

  //MSG(MSG_SSID_TMS, MSG_LEGACY_LOW,"Err service initialized.");

  
} /* err_pd_init */

/*===========================================================================

FUNCTION       err_fatal_post_exception_processing

DESCRIPTION
  This is called from exception handler after error fatal raises an 
  exception

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_fatal_post_exception_processing(void)
{
  char pfr_str[ERR_LOG_MAX_MSG_LEN];
  
  /* Copy trace log from island mode data structure */
  coredump.err.crumb_trail_bmsk |= err_fatal_params.crumb_trail_bmsk; 
  
  /* Copy compressed prt to coredump */
  coredump.err.compressed_ptr = err_fatal_params.msg_const_ptr;

  /* Copy register content */
  memscpy( coredump.arch.regs.array, SIZEOF_ALLOCATED_COREDUMP_REG,
             err_fatal_params.array,  SIZEOF_ARCH_COREDUMP_REGISTERS * sizeof(uint32) );

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_CACHCE_CLEAN_PRE);

  /* Clean Cache */
  qurt_mem_cache_clean(0,0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE);

  err_call_next_to_STM_CB();
  
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_COREDUMP_UPDATED_PRE);

  if ( err_fatal_params.msg_const_ptr->fmt != NULL )
  {
    strlcpy( (char *)pfr_str, (char *)err_fatal_params.msg_const_ptr->fmt, 
		    ERR_LOG_MAX_MSG_LEN);
  }

  /* Inform PFR reason to root PD */
  err_qdi_invoke_inform_pfr( strnlen(pfr_str, 
                             ERR_LOG_MAX_MSG_LEN), pfr_str);

  err_fatal_jettison_core (err_fatal_params.msg_const_ptr->line,
                           err_fatal_params.msg_const_ptr->fname,
                           err_fatal_params.msg_const_ptr->fmt,
                           err_fatal_params.param1, 
                           err_fatal_params.param2, 
                           err_fatal_params.param3);

} /* err_fatal_post_exception_processing */

void err_put_log (word line, const char *file_ptr)
{
  //err logging is not supported in this build
}

