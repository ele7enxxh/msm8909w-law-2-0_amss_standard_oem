/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error related functionality for user PD.

Copyright (c) 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_root.c#1 $

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
#include "err_decompress.h"
#include "msg.h"
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
extern void err_target_update_coredump_and_f3_trace( err_fatal_params_type * params );
extern void err_target_get_time(void);
extern void err_target_init (void);
extern void err_write_timestamp_string(void);
extern void err_call_next_to_STM_CB(void);
extern void err_log_ssr_init(void);

/*===========================================================================

                      External Data Vairables

===========================================================================*/

extern err_fatal_params_type err_fatal_params;

/* Struct used to hold coredump data */
extern coredump_type coredump;


/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/


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
  /* Initialize err_log_ssr_init */
  err_log_ssr_init();

  /* Initialize the smem buffer for writing extended log info */
  err_smem_log_init();
  
  /* Write the timestamp string into the smem buffer (only from Root PD).
     * This API should always be called before writing any other info to the
     * SMEM buffer. Since the smem parser tool depends on the below string
     * to locate the err smem buffer.
     */
  err_write_timestamp_string();

  err_target_init();
  
  /* Write the updated image versioning information to smem buffer (only from Root PD) */
  err_smem_log_image_version();

  ERR_LOG_MSG("Err service initialized.");

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
  err_target_get_time();

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

  /* Decompress and f3 trace */
  err_target_update_coredump_and_f3_trace(&err_fatal_params);

} /* err_fatal_post_exception_processing */

