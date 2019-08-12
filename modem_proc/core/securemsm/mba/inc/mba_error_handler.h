#ifndef MBA_ERROR_HANDLER_H
#define MBA_ERROR_HANDLER_H

/**
@file mba_error_handler.h
@brief MBA Error Handler
*/

/*===========================================================================
   Copyright (c) 2012-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_error_handler.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
03/12/12   mm       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"

/* Debug Error Codes */
#define MBA_PIL_GET_SEBOOT_FTBL_FAILED                              0x1
#define MBA_PIL_INIT_IMAGE_NULL_ELF_HDR                             0x2
#define MBA_PIL_INIT_IMAGE_UNSUPPORTED_PROC                         0x3
#define MBA_PIL_INIT_IMAGE_ELF_HDR_NOT_NONSECURE                    0x4
#define MBA_PIL_INIT_IMAGE_IS_NOT_ELF                               0x5
#define MBA_PIL_INIT_IMAGE_POPULATE_ELF_INFO_FAILED                 0x6
#define MBA_PIL_INIT_IMAGE_VERIFY_SIG_FAILED                        0x7
#define MBA_PIL_INIT_IMAGE_PREPARE_SUBSYS_FAILED                    0x8
#define MBA_PIL_AUTH_RESET_UNSUPPORTED_PROC                         0x9
#define MBA_PIL_AUTH_RESET_PROC_NOT_IN_RESET                        0xA
#define MBA_PIL_AUTH_RESET_AUTH_SEGMENTS_FAILED                     0xB
#define MBA_PIL_AUTH_RESET_SUBSYS_BRING_UP_FAILED                   0xC
#define MBA_PIL_VERIFY_SEGMENTS_UNSUPPORTED_PROC                    0xD
#define MBA_PIL_GET_CODE_SEGMENT_FAILED                             0xE
#define MBA_PIL_VERIFY_SIG_HW_IS_AUTH_ENABLED_FAILED                0xF
#define MBA_PIL_VERIFY_SEGMENTS_AUTH_INIT_FAILED                    0x10
#define MBA_PIL_VERIFY_SIG_MPU_LOCK_FAILED                          0x11
#define MBA_PIL_VERIFY_SEGMENTS_AUTH_PARTIAL_SEGMENTS_FAILED        0x12
#define MBA_PIL_GET_ENTRY_UNSUPPORTED_PROC                          0x13
#define MBA_PIL_GET_IMAGE_LENGTH_UNSUPPORTED_PROC                   0x14
#define MBA_PIL_AUTH_SEGMENTS_NULL_ELF                              0x15
#define MBA_PIL_AUTH_SEGMENTS_ZERO_SEGMENTS_FAILED                  0x16
#define MBA_PIL_MI_BOOT_VALID_UNSUPPORTED_PROC                      0x17
#define MBA_PIL_MI_BOOT_VALID_MI_NULL                               0x18
#define MBA_PIL_MI_BOOT_VALID_ZERO_SIZE                             0x19
#define MBA_PIL_IS_ELF_MAGIC_FAILED                                 0x1A
#define MBA_PIL_HW_IS_AUTH_ENBALED_NULL_SECBOOT_ROUTINES            0x1B
#define MBA_PIL_VERIFY_SIG_NULL_ELF                                 0x1C
#define MBA_PIL_VERIFY_SIG_NULL_SECBOOT_ROUTINES                    0x1D
#define MBA_PIL_SECBOOT_INIT_FAILED                                 0x1E
#define MBA_PIL_SECBOOT_GET_SW_VERSION_FAILED                       0x1F
#define MBA_PIL_SECBOOT_GET_SW_ID_FAILED                            0x20
#define MBA_PIL_SECBOOT_AUTHENTICATE_FAILED                         0x21
#define MBA_PIL_POPULATE_ELF_PROG_HDR_NOT_IN_NS_MEMORY              0x22
#define MBA_POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY                  0x23 
#define MBA_POPULATE_ELF_IMAGE_HDR_TOO_BIG                          0x24
#define MBA_POPULATE_ELF_MALLOC_FAILED                              0x25
#define MBA_POPULATE_ELF_HW_IS_AUTH_ENBALED_FAILED                  0x26
#define MBA_POPULATE_ELF_MI_BOOT_INVALID                            0x27
#define MBA_AUTH_SEGMENT_ZERO_ARGS                                  0x28
#define MBA_AUTH_SEGMENT_INVALID_HASH_LEN                           0x29
#define MBA_AUTH_SEGMENT_HASH_FAILED                                0x2A 
#define MBA_AUTH_SEGMENT_INVALID_DIGEST                             0x2B
#define MBA_AUTH_ELF_HEADERS_ELF_IS_NULL                            0x2C
#define MBA_AUTH_ELF_HEADERS_PROG_HDR_NUM_ZERO                      0x2D
#define MBA_AUTH_ELF_HEADERS_HASH_FAILED                            0x2E
#define MBA_AUTH_ELF_HEADERS_NULL_HASH                              0x2F  
#define MBA_AUTH_ELF_HEADERS_INVALID_DIGEST                         0x30
#define MBA_SEC_PIL_GET_SECBOOT_FTBL_FAILED                         0x31
#define MBA_SEC_PIL_GET_SECBOOT_HW_FTBL_FAILED                      0x32
#define MBA_PIL_AUTH_SEG_AUTH_INIT_NULL                             0x33
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_FAILED                        0x34
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_INIT_FAILED              0x35
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_UPDATE_FAILED            0x36
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_FINAL_FAILED             0x37
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_INVALID_DIGEST                0x38
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_CORRUPTED_IMAGE               0x39
#define MBA_PIL_AUTH_SEGMENTS_HASH_NULL                             0x3A
#define MBA_PIL_AUTH_SEGMENTS_LOCK_XPU_FAILED                       0x3B
#define MBA_PIL_AUTH_SEGMENTS_INVALID_HASH_LEN                      0x3C
#define MBA_PIL_AUTH_SEGMENTS_SEGMENT_AUTH_FAILED                   0x3D
#define MBA_PIL_AUTH_SEGMENTS_REMAINING_HASH_BYTES                  0x3E
#define MBA_PIL_HW_IS_AUTH_ENBALED_FAILED                           0x3F
#define MBA_PIL_SECBOOT_DEINIT_FAILED                               0x40
#define MBA_PIL_VERIFY_SIG_MPU_UNLOCK_FAILED                        0x41
#define MBA_PIL_VERIFY_SIG_INVALID_ELF_HEADERS                      0x42
#define MBA_PIL_ZERO_SEGMENTS_SIZE_MISMATCH                         0x43
#define MBA_MI_BOOT_VALID_LENGTH_MISMATCH                           0x44
#define MBA_PIL_IS_ELF_INVALID_CLASS                                0x45
#define MBA_PIL_IS_ELF_INVALID_VERSION                              0x46
#define MBA_PIL_IS_ELF_INVALID_EHSIZE                               0x47
#define MBA_PIL_IS_ELF_INVALID_PHENTSIZE                            0x48
#define MBA_POPULATE_ELF_INCORRECT_HASH_TABLE_SEGMENTS              0x49
#define MBA_POPULATE_ELF_HASH_SEG_TOO_BIG                           0x4A
#define MBA_POPULATE_ELF_ELF_HDR_NOT_IN_NS_MEMORY                   0x4B
#define MBA_POPULATE_ELF_GET_CODE_SEGMENT_FAILED                    0x4C
#define MBA_GET_EBI_CH_INVALID                                      0x4D
#define MBA_GET_META_DATA_LENGTH_UNSUPPORTED_PROC                   0x4E
#define MBA_GET_META_DATA_INSUFFICIENT_MEMORY                       0x4F
#define MBA_GET_META_DATA_NO_PROG_HDR                               0x50
#define MBA_GET_IMAGE_LENGTH_IN_MEMORY_UNSUPPORTED_PROC             0x51 
#define MBA_GET_IMAGE_LENGTH_IN_MEMORY_ERROR                        0x52
#define MBA_PIL_GET_END_ADDRESS_INTEGER_OVERFLOW1                   0x53
#define MBA_PIL_VERIFY_SEGMENTS_GET_END_ADDR_FAILED                 0x54
#define MBA_PIL_GET_END_ADDRESS_DATA_CORRUPTION                     0x55 
#define MBA_PIL_GET_END_ADDRESS_INTEGER_OVERFLOW2                   0x56
#define MBA_POPULATE_ELF_PREVIOUS_MALLOC_IS_NOT_FREED               0x57
#define MBA_PIL_GET_CODE_SEGMENT_UNSUPPORTED_PROC                   0x58
#define MBA_PIL_GET_CODE_SEGMENT_NULL_POINTER                       0x59
#define MBA_PIL_GET_SW_ID_UNSUPORTED_PROC                           0x5A
#define MBA_PIL_GET_SW_ID_NULL_POINTER                              0x5B
#define MBA_PIL_GET_SW_VERISON_UNSUPPORTED_PROC                     0x5C
#define MBA_PIL_GET_SW_VERSION_NULL_POINTER                         0x5D
#define MBA_PIL_UPDATE_HASH_HASH_INIT_FAILED                        0x5E
#define MBA_PIL_UPDATE_HASH_HASH_UPDATE_FAILED                      0x5F
#define MBA_PIL_UPDATE_HASH_HASH_FINAL_FAILED                       0x60
#define MBA_PIL_UPDATE_HASH_INVALID_DIGEST                          0x61
#define MBA_PIL_UPDATE_HASH_CORRUPTED_IMAGE                         0x62
#define MBA_PIL_UPDATE_HASH_LENGTH_LOADED_FAILED                    0x63
#define MBA_PIL_VERIFY_SEGMENTS_INVALID_ADDRESS                     0x64
#define MBA_PIL_VERIFY_SEGMENTS_PIL_STATE_NOT_IN_RESET              0x65
#define MBA_PIL_VERIFY_SEGMENTS_MPU_LOCK_FAILED                     0x66
#define MBA_PIL_UPDATE_HASH_SEGMENTS_NOT_CONTIGUOUS                 0x67
#define MBA_PIL_SECBOOT_INVALID_END_ADDR                            0x68
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_DATA_CORRUPTION               0x69
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_NOT_CONTIGUOUS                0x6A
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_INTEGER_OVERFLOW              0x6B
#define MBA_MI_BOOT_VALID_INTEGER_OVERFLOW1                         0x6C
#define MBA_MI_BOOT_VALID_INTEGER_OVERFLOW2                         0x6D
#define MBA_POPULATE_ELF_HDR_TOO_BIG                                0x6E
#define MBA_PIL_VERIFY_SEGMENTS_ERROR                               0x6F
#define MBA_PIL_GET_LOAD_ADDR_UNSUPORTED_PROC                       0x70
#define MBA_PIL_GET_LOAD_ADDR_NULL_POINTER                          0x71
#define MBA_AUTH_ELF_HEADERS_PROG_HDR_NUM_OVERFLOW                  0x72
#define MBA_PIL_UPDATE_ELF_LEN_EXCEED_FILE_SZ                       0x73
#define MBA_POPULATE_ELF_PROG_TBL_TOO_BIG                           0x74
#define MBA_PIL_AUTH_PARTIAL_SEGMENTS_INVALID_HASH_LEN              0x75
#define MBA_PIL_VERIFY_SEGMENTS_HASH_PENDING                        0x76
#define MBA_PIL_ZERO_SEGMENTS_INTEGER_OVERFLOW                      0x77
#define MBA_PIL_VERIFY_SIG_INVALID_IMAGE_SIGNING                    0x78
#define MBA_POPULATE_ELF_NO_PROG_HDR                                0x79
#define MBA_PIL_UIE_DECRYPT_FAILED                                  0x7A
#define MBA_PIL_SEC_IMG_AUTH_INVALID_ARG                            0x7B
#define MBA_PIL_SEC_IMG_AUTH_HW_FAILURE                             0x7C
#define MBA_PIL_SEC_IMG_AUTH_INVALID_CAL_CERT_NUM                   0x7D
#define MBA_PIL_SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED                    0x7E
#define MBA_PIL_SEC_IMG_AUTH_ENGG_CERT_NOT_ALLOWED                  0x7F
#define MBA_PIL_VERIFY_SEGMENTS_MPU_UNLOCK_FAILED                     0x80

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

void mba_error_handler
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  int  err_code   /* Error code to indicate which error */
);

void mba_debug_log
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  int  dbg_code   /* Debug code to indicate which error */
); 

#define MBA_VERIFY( xx_exp, xx_err_code )                       \
            do{                                                             \
              if( !(xx_exp) )                                               \
              {                                                             \
                mba_error_handler( __FILE__, __LINE__,                      \
                                   (xx_err_code) );   \
              }                                                             \
            } while( 0 )
#endif /* MBA_ERROR_HANDLER_H */
