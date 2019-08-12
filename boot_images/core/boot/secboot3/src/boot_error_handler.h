#ifndef BOOT_ERROR_HANDLER_H
#define BOOT_ERROR_HANDLER_H

/*===========================================================================

                       Boot Error Handler Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the BOOT
  error handler.  It is called in the event a BOOT failure is detected.
  Once this boot_error_handler is called. It will never return. 
    
Copyright 2010, 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_error_handler.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/25/14   kedara  Added sbl_pcie_coredump_addr, sbl_pcie_coredump_size
03/18/14   kedara  Moved sbl_save_regs() definition
11/12/13   kedara  Added changes to support SBL crash dumps
07/10/13   aus     Added support to save the registers and flush the L2 TCM to DDR
                   on SBL errors
08/04/10   aus     Clean-up secboot 3.0
05/26/10   plc     Initial port to secboot 3.0 common code  
01/28/10   ty      Initial revision

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#include "boot_error_if.h"

/*==========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/
#define BOOT_CRASH_DUMP_VERSION 1
#define BOOT_CRASH_DUMP_COOKIE 0x45525220 // Cookie "ERR "

typedef enum
{
  ARM_SVC_R0 = 0,
  ARM_SVC_R1,
  ARM_SVC_R2,
  ARM_SVC_R3,
  ARM_SVC_R4,
  ARM_SVC_R5,
  ARM_SVC_R6,
  ARM_SVC_R7,
  ARM_SVC_R8,
  ARM_SVC_R9,
  ARM_SVC_R10,
  ARM_SVC_R11,
  ARM_SVC_R12,
  ARM_SVC_SP,
  ARM_SVC_LR,
  ARM_SVC_SPSR,
  ARM_SVC_PC,
  ARM_CPSR,
  ARM_SYS_SP,
  ARM_SYS_LR,
  ARM_IRQ_SP,
  ARM_IRQ_LR,
  ARM_IRQ_SPSR,
  ARM_ABT_SP,
  ARM_ABT_LR,
  ARM_ABT_SPSR,
  ARM_UDF_SP,
  ARM_UDF_LR,
  ARM_UDF_SPSR,
  ARM_FIQ_R8,
  ARM_FIQ_R9,
  ARM_FIQ_R10,
  ARM_FIQ_R11,
  ARM_FIQ_R12,
  ARM_FIQ_SP,
  ARM_FIQ_LR,
  ARM_FIQ_SPSR,
  SIZEOF_ARM_REGISTERS
} register_type;

typedef struct
{
  uint32 crash_dump_cookie;            // Crash dump magic cookie
  uint32 version;                      // Version of the data structure
  uint32 regs[SIZEOF_ARM_REGISTERS];   // Register array
  uint32 sbl_ddr_dump_addr;            // Location in DDR where SBL DDR region is dumped
  uint32 sbl_ddr_addr;                 // Address of SBL DDR region
  uint32 sbl_ddr_size;                 // Size of SBL DDR region
  uint32 sbl_l2_dump_addr;             // Location in DDR where SBL L2 region is dumped
  uint32 sbl_l2_addr;                  // Address of SBL L2 region
  uint32 sbl_l2_size;                  // Size of SBL L2 region
  uint32 sbl_pcie_coredump_addr;       // Address of PCIe register dump
  uint32 sbl_pcie_coredump_size;       // Size of PCIe register dump region  
} boot_crash_dump_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
       
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
void boot_err_fatal( void );


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
void boot_init_stack_chk_canary(void);


/*===========================================================================

**  Function :  sbl_error_handler

** ==========================================================================
*/
/*!
* 
* @brief
*   SBL specific error handling  
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
void sbl_error_handler(void);

/*===========================================================================
**  Function :  sbl_save_regs
** ==========================================================================
*/
/*!
* 
* @brief
*   Save current registers at the time of crash
*
* @par Dependencies
*  None        
*   
*/
void sbl_save_regs(void);

#endif

/*===========================================================================

**  Macro :  boot_valid_addr_inc_imem 

** ==========================================================================
*/
/*!
* 
* @brief
*   This macro validates the address passed in to insure that it is within
*   the address space that SDRAM or IMEM can exist on the
*   MSM.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   Calls the SBL error handler if the address falls outside the valid range.
* 
*/
#ifndef BOOT_VALID_ADDR_INC_IMEM
#define BOOT_VALID_ADDR_INC_IMEM( addr, error_type ) \
           BL_VERIFY( (((uint32)(addr) <= RANGE_END_ADDR)) || \
                      (((uint32)(addr) < IMEM_RANGE_END_ADDR) && \
                       ((uint32)(addr) >= IMEM_RANGE_START_ADDR)), error_type )
#endif


#endif  /* BOOT_ERROR_HANDLER_H */
