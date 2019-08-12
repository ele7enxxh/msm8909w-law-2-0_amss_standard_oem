#ifndef BOOT_ERROR_IF_H
#define BOOT_ERROR_IF_H

/*===========================================================================

                 Boot Loader Error Handler Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the boot
  error handler interface.
    
Copyright 2007-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/power/boot_error_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14   wg      Added two new Authentication codes for 
				     SAFE 1.0 API migration
12/06/12   jz      Added error code BL_ERR_GET_CDT_FAIL
09/06/12   kpa     Added error code for Subsystem Self Authentication (ssa)
10/30/11   kpa     Added error code for Qfprom test framework  
09/20/11   kpa     Added error code for tpm hash population. 
09/13/11   kpa     Added error code for Rollback prevention feature 
04/07/11   dh      Added error code for DLOAD
08/04/10   aus     Added error code for invalid header
07/22/10   aus     Added error code for SBL error
07/15/10   dh      Modify BL_VERIFY to fix Klockwork error
06/23/10   plc     Add error code for fail to init PBL interface
05/26/10   plc     Added codes for secboot 3.0 merge, and include 
                   boot_comdef.h only if ! _ARM_ASM_
01/05/09   vtw     Added efs error codes.
09/22/09   vw      Added error codes for sd-boot support.
04/15/08   PS      Added enum type BL_ERR_IMG_SIZE_INVALID in bl_err_type
12/04/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef void (*boot_error_handler_ptr_type)
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      error             /* Enum that describes error type */
);

/*---------------------------------------------------------------------------
  Define the SBL ERROR types. This classifies different types of errors 
  encountered in the SBL.
 ---------------------------------------------------------------------------*/
typedef enum
{
  BL_ERR_NONE = 0,                  /* No BL ERR, SUCCESS, PASS */
  BL_ERR_NAND_TIMEOUT = 0x3000,     /* Timeout occurred while waiting for NAND 
                                        operation to complete */
  BL_ERR_NAND_CFG_INVALID,          /* NAND configuration data invalid */
  BL_ERR_FLASH_TYPE_NONE,           /* Failed to detect flash type booting 
                                        from */
  BL_ERR_NULL_PTR_PASSED,           /* Function input parameter is NULL */
  BL_ERR_BLHD_NOT_FOUND,            /* Boot loader header not found */
  BL_ERR_IMG_SIZE_ZERO,             /* Image to be loaded has size zero */
  BL_ERR_IMG_SIZE_TOO_BIG,          /* Image size is not within the range
                                        which partition table allows  */
  BL_ERR_IMG_SIZE_INVALID,          /* Image size doesn't match the sum of 
                                       code_size, cert_chain_size & 
                                       signature_size */
  BL_ERR_IMG_NOT_FOUND,             /* Failed to find the image in flash */
  BL_ERR_IMG_OVWR_OSBL,             /* Image to be loaded would overwrite
                                        the OSBL memory space */
  BL_ERR_IMG_OVWR_APPSBL,           /* Image to be loaded would overwrite
                                        the APPS boot loader memory space */
  BL_ERR_IMG_OVWR_SMEM,             /* Image to be loaded would overwrite
                                        the shared memory segment */
  BL_ERR_SGMT_TYPE_INVALID,         /* Invalid segment type in ELF image */
  BL_ERR_NULL_PTR,                  /* Null pointer encountered */
  BL_ERR_NO_NONPGD_SEG_FOUND,       /* No non-paged segment found in the
                                        ELF image */
  BL_ERR_MAX_BBTB_TOO_BIG,          /* Bad block table does not fit in 
                                        one block of NAND flash */
  BL_ERR_TOO_MANY_BB,               /* Total bad block exceeds allowed 
                                        upper limit */
  BL_ERR_DM_INIT_FAIL,              /* Data mover initialization failed */
  BL_ERR_NAND_READ_FAIL,            /* Page read from NAND failed */
  BL_ERR_WRG_IMGHD_REQUESTED,       /* Invalid image header requested */
  BL_ERR_DM_CPY_FAIL,               /* Memcpy using Data Mover failed */
  BL_ERR_SEG_CPY_FAIL,              /* ELF Segment copy failed */
  BL_ERR_MULTI_HASH_SEG_FOUND,      /* Multiple hash segment found in
                                        ELF image */
  BL_ERR_NO_HASH_SEG_FOUND,         /* No hash segment found in ELF image */
  BL_ERR_HASH_MATCH_FAIL,           /* Image doesn't match hash segment or
                                        hash segment not present */
  BL_ERR_ELF_PARSE_FAIL,            /* Failed to parse ELF image header */
  BL_ERR_NAND_WRITE_FAIL,           /* Page write to NAND failed */
  BL_ERR_HW_REV_INVALID,            /* Invalid Hardware revision number */
  BL_ERR_CERT_INDEX_INVALID,        /* Commercial x509 certificate selected
                                        for authentication by PUB_KEY_SEL
                                        e-fuse does not exist */
  BL_ERR_IMG_AUTH_CODE_SIZE_ZERO,   /* Image code size to authenticate 
                                        is zero */
  BL_ERR_NULL_ATTEST_CRT_PTR,       /* Attestation Certification pointer 
                                        is NULL */
  BL_ERR_CODE_SIG_VERIFY_FAIL,      /* Signature verification failed */
  BL_ERR_OPER_ON_WRG_FLASH_TYPE,    /* Operation performed on wrong flash type */ 
  BL_ERR_OPER_INVALID_ON_NOR,       /* Invalid operation performed on NOR
                                        flash type */
  BL_ERR_MIBIB_NOT_FOUND,           /* MIBIB page not found in flash */
  BL_ERR_PARTI_TABLE_NOT_FOUND,     /* Partition table not found in flash */
  BL_ERR_WRG_PARTI_ENTRY_NAME_FMT,  /* Invalid partition entry format */
  BL_ERR_PARTI_ENTRY_NOT_FOUND,     /* Partition entry not found */
  BL_ERR_IMG_DEST_PTR_INVALID,      /* Invalid image destination pointer in 
                                        image header */
  BL_ERR_UNDEF_INSTR,               /* Undefined Instruction exception 
                                        occurred */
  BL_ERR_SWI,                       /* Software Interrupt (SWI) exception 
                                        occurred */
  BL_ERR_PREFETCH_ABORT,            /* Prefetch abort exception occurred */
  BL_ERR_DATA_ABORT,                /* Data abort exception occurred */
  BL_ERR_RESERVED_HANDLER,          /* Reserved exception occurred */
  BL_ERR_IRQ,                       /* IRQ exception occurred */
  BL_ERR_FIQ,                       /* FIQ exception occurred */
  BL_ERR_IMG_SECURITY_FAIL,         /* Image authentication failed */
  BL_ERR_OTHERS,                    /* Unsupported flash type and other
                                        unclassified errors */
  BL_ERR_OSBL,                    /* OSBL error */
  BL_ERR_AMSS_ENRTY_POINT_INVALID,  /* Entry Point into AMSS code base is not valid */
  BL_ERR_ROM_DATA_NOT_FOUND,        /* Data passed by ROM does not exist */
  BL_ERR_IMEM_PBL_LIB,               /* IMEM code or PBL library error */
  BL_ERR_C_LIB_RT,                  /* C library runtime exception occurred */
  BL_ERR_ELF_START,                 /* Placeholder for the first ELF error */
  BL_ERR_ELF_NONE = BL_ERR_ELF_START, /* Indicates no ELF error occurred */
  BL_ERR_ELF_NULL_PTR,              /* Null parameter to ELF loader */
  BL_ERR_ELF_INVAL_PARAM,           /* Otherwise invalid param to ELF loader */
  BL_ERR_ELF_FLASH,                 /* Flash-related error in ELF loader */
  BL_ERR_ELF_PARSE,                 /* Parse error in ELF loader */
  BL_ERR_ELF_FORMAT,                /* ELF file format error in ELF loader */
  BL_ERR_ELF_HASH_TABLE_MISSING,    /* Missing ELF hash segment */
  BL_ERR_ELF_HASH_MISMATCH,         /* Hash compare error in ELF loader */
  BL_ERR_ELF_END = BL_ERR_ELF_HASH_MISMATCH, /* Last ELF error placeholder */
  BL_ERR_BOOT_FLASH_ACCESS,         /* Error in the boot flash access module */
  BL_ERR_CLOBBER_PROTECTION_VIOLATION, /* Address range validation failed
                                          and a protected address range 
                                          would be clobbered */
  BL_ERR_FOTA_UPDATE,                /* FOTA Update error*/
  BL_ERR_FAIL_ALLOCATE_SMEM,
  BL_ERR_FAIL_SAVE_TO_SMEM,
  BL_ERR_FLASH_DRIVER,               /* Fatal error within the flash driver */
  BL_ERR_FEATURE_NOT_SUPPORTED,
  BL_ERR_RESOURCE_NOT_AVAILABLE,
  BL_ERR_HOTPLUG,                     /* Error in the hotplug function */
  BL_ERR_HOTPLUG_READ,                /* Hotplug read APIs failed. */
  BL_ERR_SDC_INVALID_SIZE,            /* Invalid device or block size */
  BL_ERR_UNIFIED_BOOT_FAIL,           /* error during unified boot */
  BL_ERR_OS_LOAD_FAIL,                /* error during os loading */
  BL_ERR_EFS_OPEN_FAIL,               /* Failed to open FAT file  */
  BL_ERR_EFS_CLOSE_FAIL,              /* Failed to close FAT file  */
  BL_ERR_SDC_INVALID_PARTI,           /* Partition type error  */
  BL_ERR_EFS_INVALID_HANDLE,          /* File handle error  */
  BL_ERR_EFS_READ,                    /* EFS read APIs failed. */
  BL_ERR_SBL2,                        /* error during SBL2 */
  BL_ERR_SBL3,                        /* error during SBL3 */
  BL_ERR_FAILED_TO_COPY_IMG_HDR,      /* Unable to copy header while loading */
  BL_ERR_FAILED_TO_COPY_IMG,          /* Failure in loading image */
  BL_ERR_CLK_SPEED_NOT_SUPPORTED,     /* Invalid Clock Speed */
  BL_ERR_FAILED_TO_INIT_PBL_INTERFACE,/* Failure with PBL_INIT */
  BL_ERR_SBL,                         /* Error during SBL */
  BL_ERR_IMG_HDR_INVALID,             /* Image header is invalid */
  BL_ERR_DLOAD,                       /*Error during DLOAD mode*/
  BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL,  /*Error during version rollback
                                              check */
  BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL, /*Error blowing version numbers*/
  BL_ERR_TPM_HASH_BLK_STR_FAIL,       /*Error storing Image hash */
  BL_ERR_QFPROM_TEST_FRAMEWORK_FAIL,  /*Error in qfprom test framework */
  BL_ERR_SUBSYS_BOOT_FAIL,            /*Error in booting Modem/subsystem proc */
  BL_ERR_GET_CDT_FAIL,                /* Failed to retrieve CDT from device */ 
  BL_ERR_UNSUPPORTED_HASH_ALGO,       
  BL_ERR_RAM_DUMP_FAIL,               /* Failed in ram dump */
  BL_ERR_CORE_VERIFY,
  BL_ERR_IMG_META_DATA_AUTH_FAIL,	/* Failed in authentication of image meta data */
  BL_ERR_AUTH_ELF_HDR_FAIL, 				/* Failed in authentication of elf header */
  /* add above this enum */
  BL_ERR_MAX = 0x7FFFFFFF
} bl_error_type;

/* The BL error callback function */
typedef void (*bl_error_callback_func_type)
(
  void *data_ptr
);

/* Structure for storing error handler callback data */
typedef struct bl_error_callback_node_type
{
  struct
  {
    bl_error_callback_func_type cb_func;
    void *data_ptr;
  } data;

  struct bl_error_callback_node_type *next_ptr;
} bl_error_callback_node_type;

/* The BL error interface */
typedef struct boot_error_if_type
{
/*===========================================================================

**  Function :  error_handler

** ==========================================================================
*/
/*!
* 
*   @brief
*     This function handles boot errors.
* 
*   @par Dependencies
*     None
*   
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
boot_error_handler_ptr_type error_handler;

/*===========================================================================

**  Function :  install_callback

** ==========================================================================
*/
/*!
* 
*   @brief
*     Installs a callback that will be executed when an error occurs.
* 
*     NOTE: The caller must allocate a node for use in the error callback
*           linked list data structure, but does not need to initialize
*           the structure, as this function does the initialization of
*           the linked list data structure.
* 
*   @par Dependencies
*     None
*   
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
  void (*install_callback)
  (
    bl_error_callback_func_type cb_func,  /* The callback function */
    void *data_ptr,       /* Data to pass to the callback function */
    bl_error_callback_node_type *node_ptr /* Data structure allocated by
                                        caller to store linked list node */
  );
} boot_error_if_type;

/* The global boot error interface structure */
extern boot_error_if_type bl_err_if;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Macro :  bl_verify

** ==========================================================================
*/
/*!
* 
* @brief
*   Given a boolean expression, this macro will verify the condition is TRUE
*   and do nothing if the condition is TRUE, otherwise it will call the
*   SBL error handler.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This macro never returns if the condition is FALSE.
* 
*/
/*The forever loop after error_handler will never be executed, it is added to fix klockwork warning*/
#define BL_VERIFY( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 bl_err_if.error_handler( __FILE__, __LINE__, \
                                        ( (uint32) ( error_type ) ) ); \
                 while(1) \
                 { \
                 } \
               } \
            } while(0)

/*===========================================================================

**  Macro :  bl_err_fatal

** ==========================================================================
*/
/*!
* 
* @brief
*   This macro calls the error handler.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This macro never returns.
* 
*/
/*The forever loop after error_handler will never be executed, it is added to fix klockwork warning*/
#define BL_ERR_FATAL( error_type )  \
            do{ \
                bl_err_if.error_handler( __FILE__, __LINE__, \
                                       ( (uint32) error_type ) ); \
                while(1) \
                { \
                } \
              } while (0)

#endif  /* BOOT_ERROR_IF_H */
