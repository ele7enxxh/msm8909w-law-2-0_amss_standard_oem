/**
@file tzbsp_log.c
@brief Logging for Trustzone
*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/logging/src/tzbsp_log.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
12/16/12   mg       Support new log buffer management
09/26/11   mm       Moved the log ring buffer into the diag table
04/29/10   ssm      Created

===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include <stdio.h>
#include <string.h>  /* for memset */
#include <comdef.h>
#include "tzbsp_log.h"
#include "tzbsp_diag.h"
#include "tzbsp_sys.h"

/* Pointer to the diagnostics table in IMEM */
extern tzbsp_diag_t *g_tzbsp_diag;

/* Mutex to ensure only one CPU logs at a time */
static tzbsp_mutex_t log_lock;

/* Flag indicating whether logging has been initialized (i.e. enabled) */
static boolean logging_initialized = FALSE;

uint32 g_log_counter = 0;

/* Declarations of functions used */
boolean tzbsp_oem_allow_logging(void);
void tzbsp_vjprintf(const char* fmt, va_list listp);
void tzbsp_jflush(void);

/* Current tzbsp log write position */
tzbsp_log_pos_t g_tzbsp_log_pos = {0};

/* Transient values for managing current log buffer */
static tzbsp_diag_log_t*  g_log;
static tzbsp_log_pos_t*   g_log_pos;
static uint32             g_log_len;

/* @copydoc tzbsp_log_put */
void tzbsp_log_put(const char c)
{
  if(g_tzbsp_diag != NULL)
  {
    g_log->log_buf[g_log_pos->offset] = c;
    g_log_pos->offset = (g_log_pos->offset + 1) % g_log_len;

    if (0 == g_log_pos->offset)
      ++g_log_pos->wrap;
  }
}

/* @copydoc tzbsp_log */
void tzbsp_log(uint32 pri, const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  tzbsp_log_with_ap(pri, fmt, ap);
  va_end(ap);
}

/* @copydoc tzbsp_log_with_ap */
void tzbsp_log_with_ap(uint32 pri, const char* fmt, va_list ap)
{
  tzbsp_log_to_buffer(fmt, ap, &g_tzbsp_diag->log, &g_tzbsp_log_pos,
      g_tzbsp_diag->ring_len);
}

/* @copydoc tzbsp_log_to_buffer */
void tzbsp_log_to_buffer(const char* fmt, va_list ap, void* log, void* log_pos,
      uint32 log_len)
{
  if(logging_initialized && tzbsp_oem_allow_logging())
  {
    uint32 mask;

    if (NULL == fmt || NULL == log || NULL == log_pos)
        return;

    /* ISRs use logging function, hence the ring buffer and JTAG accesses are
     * intlocked. */
    mask = tzbsp_int_disable_all();

    /* Grab the logging mutex, incase multiple CPUs are attempting to LOG
     * (i.e. multiple FIQs being serviced with debug info) 
     */
    tzbsp_mutex_lock(&log_lock);
  
    /* Store values for future use in tzbsp_log_put */
    g_log = (tzbsp_diag_log_t*)log;
    g_log_pos = (tzbsp_log_pos_t*)log_pos;
    g_log_len = log_len;
    
    /* Log the message */
    tzbsp_vjprintf(fmt, ap);
    tzbsp_jflush();

    /* Update log struct */
    ((tzbsp_diag_log_t*)log)->log_pos.wrap =
        ((tzbsp_log_pos_t*)g_log_pos)->wrap;

    ((tzbsp_diag_log_t*)log)->log_pos.offset =
        ((tzbsp_log_pos_t*)g_log_pos)->offset;

    /* Done updating shared info */
    tzbsp_mutex_unlock(&log_lock);
    
    tzbsp_int_restore(mask);
  }
}

/* @copydoc tzbsp_log_clear */
void tzbsp_log_clear(void)
{
  uint32 mask;

  /* ISRs use logging function, hence the ring buffer and JTAG accesses are
   * intlocked. */
  if(logging_initialized && g_tzbsp_diag != NULL)
  {   
    mask = tzbsp_int_disable_all();
    tzbsp_mutex_lock(&log_lock);
    memset(&g_tzbsp_diag->log, 0,
        g_tzbsp_diag->ring_len + sizeof(tzbsp_log_pos_t));
    tzbsp_mutex_unlock(&log_lock);
    tzbsp_int_restore(mask);		
  }
}

/* @copydoc tzbsp_log_init */
void tzbsp_log_init(void)
{
  if(!logging_initialized)
  {
    /* Marking logging enabled and initialize the mutex */
    logging_initialized = TRUE;
    tzbsp_mutex_init(&log_lock);
  }
}
