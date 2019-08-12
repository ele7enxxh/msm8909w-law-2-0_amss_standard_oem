/*=============================================================================

                             Boot Error Handler

GENERAL DESCRIPTION
  This file contains the QUALCOMM Boot Loader error handler.
  The BOOT error handler is called in the event an error is detected.

Copyright 2009 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_error_handler.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/27/15   aus     Saving registers can be called only when DDR is initialized
06/06/14   ck      Updated NO_FLASH_conditionals to use booting_from_storage_media
03/18/14   kedara  Save the registers early in the error handler.
                   Call sbl_error_handler after saving err_log so that it is present in 
                   the SBL crash dumps
03/18/14   ck      Updated boot_hw_reset calls as they now take a reset type parameter
08/19/13   yp      call boot_pm_vib_off to stop vibrate,when a error happen.
07/10/13   aus     Added support to save the registers and flush the L2 TCM to DDR
                   on SBL errors
05/23/12   kedara  Fix boot error log entry updation.
12/10/12   plc     Fix Klockwork errors
08/31/12   dh      Add log message to boot_error_handler
05/02/12   dh      Add __stack_chk_fail
03/14/12   dh      Rmove cache_mmu_disable
06/27/11   aus     Added support for flashless boot
09/10/10   aus     Enabling data cache and MMU in boot
08/18/10   aus     Code Clean-up
07/22/10   aus     Clean-up secboot 3
07/09/10   aus     Moved stack_ptr_addr definition to sbl specific sconscripts and sbl_dload_entry to 
                   boot_dload_handler.h
06/23/10   plc     Support DLOAD in secboot 3.0
05/26/10   plc     Initial port to secboot 3.0 common code  
12/28/09   ty      Initial port to BOOT
09/09/09   tnk     Removing of  MMU Disable in the osbl_error_handler path .


=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_sbl_shared.h"
#include "boot_error_handler.h"
#include "boot_cache_mmu.h"
#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_logger.h"
#include "boot_extern_crypto_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_util.h"
#include <stdio.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
 
/**
 * Infinite loop
 */
#define INFINITE_LOOP() while(1)

#define MAX_BOOTLOG_SECONDARY_ENTRIES 2

/*---------------------------------------------------------------------------
  Define just to make sure the link list doesn't go off into the weeds.
---------------------------------------------------------------------------*/
#define BOOT_MAX_NUM_ERROR_CALLBACKS 20
#define BOOT_ERROR_MSG_LEN 256

/* Function prototypes */
void boot_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
);
static void boot_install_error_callback
(
  bl_error_callback_func_type,
  void *,
  bl_error_callback_node_type *node_ptr
);

/*---------------------------------------------------------------------------
  Define the BOOT ERROR Log entry types. 
  ---------------------------------------------------------------------------*/
typedef struct boot_err_log_entry_type
{
  uint32 error_type;
  uint32 line;
  const char   *filename;
} boot_err_log_entry_type;

typedef struct boot_err_count_type
{
  char *sbl_type;
  uint32 error_count;
} boot_err_count_type;


/*---------------------------------------------------------------------------
  Define the error logging and error callback variables.
  ---------------------------------------------------------------------------*/
#pragma arm section rwdata = "ERR_DATA", zidata = "ERR_DATA"
/* Structure defining the layout of BOOT error records. */
typedef struct
{
  /* The primary BOOT error record. */
  boot_err_log_entry_type primary_log;

  /* The total number of errors logged. Store this after 1st error record. */
  boot_err_count_type     err_count;

  /* Array of records for subsequent BOOT errors. */
  boot_err_log_entry_type secondary_log[MAX_BOOTLOG_SECONDARY_ENTRIES];
} err_log_type;
static err_log_type err_log;
#pragma arm section rwdata, zidata

#pragma arm section rwdata = "ERR_DATA_PTR", zidata = "ERR_DATA_PTR"
/* Structure defining the layout of BOOT error records. */
err_log_type *boot_err_log_ptr = NULL;
#pragma arm section rwdata, zidata

/* Error handler callback data structures */
static bl_error_callback_node_type *err_cb_head_ptr = NULL;

/*Error message that can be logged using boot logger */
static char error_message[BOOT_ERROR_MSG_LEN];

/* The global boot error interface structure */
boot_error_if_type bl_err_if = 
{
  boot_error_handler,
  boot_install_error_callback
};

/* Stack protection canary. 
   With stack protection, when a vulnerable function is called, 
   the initial value of its guard variable is taken from this variable

   Canary is set to a random value in boot_init_stack_chk_canary
*/
void *__stack_chk_guard = (void*)0x00040A0D;

/* Indicates ddr initialization is done or not */
extern boolean ddr_init_done;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_error_handler

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is the error handler for the BOOT and records the line 
*   number, filename & error type before jumping into the downloader. 
*   This function is also shared by exception handler and can log
*   multiple errors if called more than once.
* 
* @param[in] filename_ptr File this function was called from defined by __FILE__
* @param[in] line Line number this function was called from defined by __LINE__ 
* @param[in] err_code Enum that describes error type
*
* @par Dependencies
*   Exception handler. 
*   
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void boot_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
)
{
  register boot_err_log_entry_type *boot_err_log_entry_ptr = NULL;
  static uint32 error_handler_entry_count = 0;
  register uint32 i = 0;

  /* Save the registers only once DDR is initialized as the registers are saved into DDR memory */
  if(ddr_init_done)
    sbl_save_regs();
  
  error_handler_entry_count++; /* Count the number of calls to this fn */
  
  boot_pm_vib_off();
  
  /* Ensure boot error log entries are within bounds. Only
     MAX_BOOTLOG_SECONDARY_ENTRIES+1 error logs supported/
  */
  if((MAX_BOOTLOG_SECONDARY_ENTRIES + 1) >= error_handler_entry_count)
  {
    if(error_handler_entry_count == 1) /* Entering Error Handler for first time */
    {
      /* Only log to boot logger if this is the first time entering error handler */
      snprintf(error_message, BOOT_ERROR_MSG_LEN, 
             "Error code %lx at %s Line %lu", err_code, filename_ptr, line);
             
      boot_log_message(error_message);
    
      /* Pointer to the primary error log */
      boot_err_log_ptr = &err_log;
      boot_err_log_entry_ptr = &err_log.primary_log;

      /* Log that this is BOOT error */
      err_log.err_count.sbl_type = "BOOT";
    }
    else /* For all subsequent calls to this fn */
    {
      /* Pointer to the error log for subsequent errors */
      /* error_handler_entry_count of 2 corresponds to first
          element of secondary_log */
      boot_err_log_entry_ptr = &err_log.secondary_log[(error_handler_entry_count-2)];
    }

    /* Save total number of errors logged */
    err_log.err_count.error_count = error_handler_entry_count;

    /* Log the error type */
    boot_err_log_entry_ptr->error_type = err_code;

    /* Log the line number */
    boot_err_log_entry_ptr->line = line;

    /* Log the filename; no need to copy, const mem */
    boot_err_log_entry_ptr->filename = filename_ptr;


    if ( error_handler_entry_count == 1 )
    {
      /* Only try to execute the error callbacks the first time through
         the error handler in case one of the callbacks caused the
         subsequent error. */

      register bl_error_callback_node_type *node_ptr;

      for ( node_ptr = err_cb_head_ptr, i = 0;
            node_ptr != NULL && i < BOOT_MAX_NUM_ERROR_CALLBACKS;
            node_ptr = node_ptr->next_ptr, i++ )
      {
        node_ptr->data.cb_func( node_ptr->data.data_ptr );
      }
    }
  
    /* Call SBL error handler to flush the memory into DDR */
    sbl_error_handler();  
  
    /* If flashless boot then we need to reset as there is no dload mode in 
     this scenario */
    if(!booting_from_storage_media())
    {
       boot_hw_reset(BOOT_HARD_RESET_TYPE);  
    }
    else
    {
      /* If in SBL that supports DLOAD, then enter downloader for QPST.
      Else jump back to PBL's provided error-routine, for Emergency
      downloader.  This will be handled in SBL* specific implementation
      of sbl_dload_entry()
      */    
      sbl_dload_entry();
    }
  }
  /* In case of re-entrant error handler or dload entry failure, just loop */
  INFINITE_LOOP();

} /* boot_error_hander() */


/*===========================================================================

**  Function :  boot_install_error_callback

** ==========================================================================
*/
/*!
* 
* @brief
*   This function installs a callback function that will be executed when
*   an error occurs.
* 
* @param[in] cb_func Call back function 
* @param[in] data_ptr Pointer to the data
* @param[in] node_ptr Pointer to the node
*
* @par Dependencies
*   None 
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_install_error_callback
(
  bl_error_callback_func_type cb_func,
  void *data_ptr,
  bl_error_callback_node_type *node_ptr
)
{
  BL_VERIFY( cb_func != NULL && node_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  node_ptr->data.cb_func = cb_func;
  node_ptr->data.data_ptr = data_ptr;
  node_ptr->next_ptr = err_cb_head_ptr;

  err_cb_head_ptr = node_ptr;

} /* boot_install_error_callback() */


/*===========================================================================

**  Function :  boot_nested_exception_c_handler

** ==========================================================================
*/
/*!
* 
* @brief
*   This function enables the JTAG interface and never returns.  It is
*   called when a nested exception occurs.
* 
*   Note: For security purposes, our main concern is concealing the hardware
*         key when the JTAG interface is enabled.  This is handled by hardware.
*         Because of this, it is not required that all memory and registers be
*         cleaned out before enabling the JTAG interface.  The hardware key is
*         never accessed or used by the BOOT. 
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void boot_nested_exception_c_handler( void )
{
 sbl_loop_here();  
} /* boot_nested_exception_c_handler() */


/*===========================================================================

**  Function :  boot_err_fatal

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls the SBL error handler.
*   
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_err_fatal( void )
{
  BL_ERR_FATAL( BL_ERR_OTHERS );
}


/*===========================================================================


**  Function :  boot_<exception_type>_c_handler

** ==========================================================================
*/
/*!
* 
* @brief
*   These are the exception handlers for the BOOT.  They include handlers for
*   each of the following exception types:
* 
*     - undefined instruction
*     - software interrupt
*     - prefetch abort
*     - data abort
*     - reserved
*     - IRQ
*     - FIQ
*   
*   The BOOT considers each of these exceptions a fatal error and handles
*   them by calling the boot_error_handler().  A separate handler is assigned
*   to each exception for the purpose of providing additional debug
*   information.  Each call to the BOOT_ERR_FATAL() provides filename, file
*   line number & SBL ERROR TYPE information.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   These handlers never return.
* 
*/
void boot_undefined_instruction_c_handler( void )
{
  BL_ERR_FATAL(BL_ERR_UNDEF_INSTR);
}

void boot_swi_c_handler( void )
{
  BL_ERR_FATAL( BL_ERR_SWI );
}

void boot_prefetch_abort_c_handler( void )
{
  /* Don't use the global error handler pointer in case it's invalid and
     caused the abort */
  boot_error_handler( __FILE__, __LINE__, BL_ERR_PREFETCH_ABORT );
}

void boot_data_abort_c_handler( void )
{
  /* Don't use the global error handler pointer in case it's invalid and
     caused the abort */
  boot_error_handler( __FILE__, __LINE__, BL_ERR_DATA_ABORT );
}

void boot_reserved_c_handler( void )
{
  BL_ERR_FATAL( BL_ERR_RESERVED_HANDLER );
}

void boot_irq_c_handler( void )
{
  BL_ERR_FATAL( BL_ERR_IRQ );
}

void boot_fiq_c_handler( void )
{
  BL_ERR_FATAL( BL_ERR_FIQ );
}


/*===========================================================================


**  Function :  __rt_raise

** ==========================================================================
*/
/*!
* 
* @brief
*   This is the re-implementation of the C library runtime exception handler.
*   This gets called from the C library, for example, when integer
*   divide-by-zero occurs.  Re-implementing this handler prevents linking in
*   the standard C library signal handling routines and reduces the overall
*   image size.  The BOOT considers a C library runtime exception fatal.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This handler never returns.
* 
*/
void __rt_raise
( 
  int signal, 
  int type 
)
{
  BL_ERR_FATAL( BL_ERR_C_LIB_RT );
}


/*===========================================================================


**  Function :  __stack_chk_fail

** ==========================================================================
*/
/*!
* 
* @brief
*   This is the implementation of the C library runtime stack error handler.
*   It is called by the checking code on detection of corruption of the stack guard.
*   It will never return.   
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This handler never returns.
* 
*/
void __stack_chk_fail(void)
{
  /* Loop forever in case of stack overflow */
  INFINITE_LOOP();
}


/*===========================================================================

**  Function :  boot_init_stack_chk_canary

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize stack protection canary to a random number  
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_init_stack_chk_canary()
{
  BL_VERIFY
  (
    boot_PrngML_getdata_lite((void *)&__stack_chk_guard, 
                             sizeof(__stack_chk_guard)) == PRNGML_ERROR_NONE, 
    BL_ERR_IMG_SECURITY_FAIL
  );
}
