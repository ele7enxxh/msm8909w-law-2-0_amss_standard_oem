#ifndef BOOT_ERROR_IF_H
#define BOOT_ERROR_IF_H

/*===========================================================================

                 Boot Loader Error Handler Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the boot
  error handler interface.
    
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/boot/boot_error_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/14   tj      Added error code BL_ERR_ADDR_INVALID_RANGE & BL_ERR_ADDR_OVER_LAPPING
02/12/14   ck      Added error code BL_ERR_AARCH64_NOT_ENABLED
02/05/14   ck      Added error code BL_ERR_MISALIGNMENT
02/04/14   ck      Added error code BL_ERR_VALUE_TOO_LARGE
01/14/13   ck      Added error code BL_ERR_ELF_CLASS_INVALID
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
  BL_ERR_NONE                           = 0,      /* No BL ERR, SUCCESS, PASS */
  BL_ERR_NAND_TIMEOUT                   = 0x3000, /* Timeout occurred while waiting for NAND 
                                                     operation to complete */
  BL_ERR_NAND_CFG_INVALID               = 0x3001, /* NAND configuration data invalid */
  BL_ERR_FLASH_TYPE_NONE                = 0x3002, /* Failed to detect flash type booting 
                                                     from */
  BL_ERR_NULL_PTR_PASSED                = 0x3003, /* Function input parameter is NULL */
  BL_ERR_BLHD_NOT_FOUND                 = 0x3004, /* Boot loader header not found */
  BL_ERR_IMG_SIZE_ZERO                  = 0x3005, /* Image to be loaded has size zero */
  BL_ERR_IMG_SIZE_TOO_BIG               = 0x3006, /* Image size is not within the range
                                                     which partition table allows  */
  BL_ERR_IMG_SIZE_INVALID               = 0x3007, /* Image size doesn't match the sum of 
                                                     code_size, cert_chain_size & 
                                                     signature_size */
  BL_ERR_IMG_NOT_FOUND                  = 0x3008, /* Failed to find the image in flash */
  BL_ERR_IMG_OVWR_OSBL                  = 0x3009, /* Image to be loaded would overwrite
                                                     the OSBL memory space */
  BL_ERR_IMG_OVWR_APPSBL                = 0x300A, /* Image to be loaded would overwrite
                                                     the APPS boot loader memory space */
  BL_ERR_IMG_OVWR_SMEM                  = 0x300B, /* Image to be loaded would overwrite
                                                     the shared memory segment */
  BL_ERR_SGMT_TYPE_INVALID              = 0x300C, /* Invalid segment type in ELF image */
  BL_ERR_NULL_PTR                       = 0x300D, /* Null pointer encountered */
  BL_ERR_NO_NONPGD_SEG_FOUND            = 0x300E, /* No non-paged segment found in the
                                                     ELF image */
  BL_ERR_MAX_BBTB_TOO_BIG               = 0x300F, /* Bad block table does not fit in 
                                                     one block of NAND flash */
  BL_ERR_TOO_MANY_BB                    = 0x3010, /* Total bad block exceeds allowed 
                                                     upper limit */
  BL_ERR_DM_INIT_FAIL                   = 0x3011, /* Data mover initialization failed */
  BL_ERR_NAND_READ_FAIL                 = 0x3012, /* Page read from NAND failed */
  BL_ERR_WRG_IMGHD_REQUESTED            = 0x3013, /* Invalid image header requested */
  BL_ERR_DM_CPY_FAIL                    = 0x3014, /* Memcpy using Data Mover failed */
  BL_ERR_SEG_CPY_FAIL                   = 0x3015, /* ELF Segment copy failed */
  BL_ERR_MULTI_HASH_SEG_FOUND           = 0x3016, /* Multiple hash segment found in
                                                     ELF image */
  BL_ERR_NO_HASH_SEG_FOUND              = 0x3017, /* No hash segment found in ELF image */
  BL_ERR_HASH_MATCH_FAIL                = 0x3018, /* Image doesn't match hash segment or
                                                     hash segment not present */
  BL_ERR_ELF_PARSE_FAIL                 = 0x3019, /* Failed to parse ELF image header */
  BL_ERR_NAND_WRITE_FAIL                = 0x301A, /* Page write to NAND failed */
  BL_ERR_HW_REV_INVALID                 = 0x301B, /* Invalid Hardware revision number */
  BL_ERR_CERT_INDEX_INVALID             = 0x301C, /* Commercial x509 certificate selected
                                                     for authentication by PUB_KEY_SEL
                                                     e-fuse does not exist */
  BL_ERR_IMG_AUTH_CODE_SIZE_ZERO        = 0x301D, /* Image code size to authenticate 
                                                     is zero */
  BL_ERR_NULL_ATTEST_CRT_PTR            = 0x301E, /* Attestation Certification pointer 
                                                     is NULL */
  BL_ERR_CODE_SIG_VERIFY_FAIL           = 0x301F, /* Signature verification failed */
  BL_ERR_OPER_ON_WRG_FLASH_TYPE         = 0x3020, /* Operation performed on wrong flash type */ 
  BL_ERR_OPER_INVALID_ON_NOR            = 0x3021, /* Invalid operation performed on NOR
                                                     flash type */
  BL_ERR_MIBIB_NOT_FOUND                = 0x3022, /* MIBIB page not found in flash */
  BL_ERR_PARTI_TABLE_NOT_FOUND          = 0x3023, /* Partition table not found in flash */
  BL_ERR_WRG_PARTI_ENTRY_NAME_FMT       = 0x3024, /* Invalid partition entry format */
  BL_ERR_PARTI_ENTRY_NOT_FOUND          = 0x3025, /* Partition entry not found */
  BL_ERR_IMG_DEST_PTR_INVALID           = 0x3026, /* Invalid image destination pointer in 
                                                     image header */
  BL_ERR_UNDEF_INSTR                    = 0x3027, /* Undefined Instruction exception 
                                                     occurred */
  BL_ERR_SWI                            = 0x3028, /* Software Interrupt (SWI) exception 
                                                     occurred */
  BL_ERR_PREFETCH_ABORT                 = 0x3029, /* Prefetch abort exception occurred */
  BL_ERR_DATA_ABORT                     = 0x302A, /* Data abort exception occurred */
  BL_ERR_RESERVED_HANDLER               = 0x302B, /* Reserved exception occurred */
  BL_ERR_IRQ                            = 0x302C, /* IRQ exception occurred */
  BL_ERR_FIQ                            = 0x302D, /* FIQ exception occurred */
  BL_ERR_IMG_SECURITY_FAIL              = 0x302E, /* Image authentication failed */
  BL_ERR_OTHERS                         = 0x302F, /* Unsupported flash type and other
                                                     unclassified errors */
  BL_ERR_OSBL                           = 0x3030, /* OSBL error */
  BL_ERR_AMSS_ENRTY_POINT_INVALID       = 0x3031, /* Entry Point into AMSS code base is not valid */
  BL_ERR_ROM_DATA_NOT_FOUND             = 0x3032, /* Data passed by ROM does not exist */
  BL_ERR_IMEM_PBL_LIB                   = 0x3033, /* IMEM code or PBL library error */
  BL_ERR_C_LIB_RT                       = 0x3034, /* C library runtime exception occurred */
  BL_ERR_ELF_START                      = 0x3035, /* Placeholder for the first ELF error */
  BL_ERR_ELF_NONE = BL_ERR_ELF_START,             /* Indicates no ELF error occurred */
  BL_ERR_ELF_NULL_PTR                   = 0x3036, /* Null parameter to ELF loader */
  BL_ERR_ELF_INVAL_PARAM                = 0x3037, /* Otherwise invalid param to ELF loader */
  BL_ERR_ELF_FLASH                      = 0x3038, /* Flash-related error in ELF loader */
  BL_ERR_ELF_PARSE                      = 0x3039, /* Parse error in ELF loader */
  BL_ERR_ELF_FORMAT                     = 0x303A, /* ELF file format error in ELF loader */
  BL_ERR_ELF_HASH_TABLE_MISSING         = 0x303B, /* Missing ELF hash segment */
  BL_ERR_ELF_HASH_MISMATCH              = 0x303C, /* Hash compare error in ELF loader */
  BL_ERR_ELF_CLASS_INVALID              = 0x303D, /* ELF class not valid (32 or 64 bit) */
  BL_ERR_ELF_END = BL_ERR_ELF_HASH_MISMATCH,      /* Last ELF error placeholder */
  BL_ERR_BOOT_FLASH_ACCESS              = 0x303E, /* Error in the boot flash access module */
  BL_ERR_CLOBBER_PROTECTION_VIOLATION   = 0x303F, /* Address range validation failed
                                                     and a protected address range 
                                                     would be clobbered */
  BL_ERR_FOTA_UPDATE                    = 0x3040, /* FOTA Update error*/
  BL_ERR_FAIL_ALLOCATE_SMEM             = 0x3041,
  BL_ERR_FAIL_SAVE_TO_SMEM              = 0x3042,
  BL_ERR_FLASH_DRIVER                   = 0x3043, /* Fatal error within the flash driver */
  BL_ERR_FEATURE_NOT_SUPPORTED          = 0x3044,
  BL_ERR_RESOURCE_NOT_AVAILABLE         = 0x3045,
  BL_ERR_HOTPLUG                        = 0x3046, /* Error in the hotplug function */
  BL_ERR_HOTPLUG_READ                   = 0x3047, /* Hotplug read APIs failed. */
  BL_ERR_SDC_INVALID_SIZE               = 0x3048, /* Invalid device or block size */
  BL_ERR_UNIFIED_BOOT_FAIL              = 0x3049, /* error during unified boot */
  BL_ERR_OS_LOAD_FAIL                   = 0x304A, /* error during os loading */
  BL_ERR_EFS_OPEN_FAIL                  = 0x304B, /* Failed to open FAT file  */
  BL_ERR_EFS_CLOSE_FAIL                 = 0x304C, /* Failed to close FAT file  */
  BL_ERR_SDC_INVALID_PARTI              = 0x304D, /* Partition type error  */
  BL_ERR_EFS_INVALID_HANDLE             = 0x304E, /* File handle error  */
  BL_ERR_EFS_READ                       = 0x304F, /* EFS read APIs failed. */
  BL_ERR_SBL2                           = 0x3050, /* error during SBL2 */
  BL_ERR_SBL3                           = 0x3051, /* error during SBL3 */
  BL_ERR_FAILED_TO_COPY_IMG_HDR         = 0x3052, /* Unable to copy header while loading */
  BL_ERR_FAILED_TO_COPY_IMG             = 0x3053, /* Failure in loading image */
  BL_ERR_CLK_SPEED_NOT_SUPPORTED        = 0x3054, /* Invalid Clock Speed */
  BL_ERR_FAILED_TO_INIT_PBL_INTERFACE   = 0x3055, /* Failure with PBL_INIT */
  BL_ERR_SBL                            = 0x3056, /* Error during SBL */
  BL_ERR_IMG_HDR_INVALID                = 0x3057, /* Image header is invalid */
  BL_ERR_DLOAD                          = 0x3058, /* Error during DLOAD mode */
  BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL = 0x3059,  /* Error during version rollback
                                                         check */
  BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL = 0x305A, /* Error blowing version numbers */
  BL_ERR_TPM_HASH_BLK_STR_FAIL          = 0x305B, /* Error storing Image hash */
  BL_ERR_QFPROM_TEST_FRAMEWORK_FAIL     = 0x305C, /* Error in qfprom test framework */
  BL_ERR_SUBSYS_BOOT_FAIL               = 0x305D, /* Error in booting Modem/subsystem proc */
  BL_ERR_GET_CDT_FAIL                   = 0x305E, /* Failed to retrieve CDT from device */ 
  BL_ERR_UNSUPPORTED_HASH_ALGO          = 0x305F,       
  BL_ERR_RAM_DUMP_FAIL                  = 0x3060, /* Failed in ram dump */
  BL_ERR_VALUE_TOO_LARGE                = 0x3061, /* Value will not fit in var type */
  BL_ERR_MISALIGNMENT                   = 0x3062, /* Value is not aligned properly */
  BL_ERR_AARCH64_NOT_ENABLED            = 0x3063, /* AARCH64 enable fuse bit not set */
  BL_ERR_ADDR_INVALID_RANGE             = 0x3064, /* Invalid address range */
  BL_ERR_ADDR_OVER_LAPPING              = 0x3065,
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
