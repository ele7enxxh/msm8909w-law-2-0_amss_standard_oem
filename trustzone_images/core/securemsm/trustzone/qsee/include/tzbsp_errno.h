#ifndef TZBSP_ERRNO_H
#define TZBSP_ERRNO_H
/**
@file tzbsp_error.h
@brief Provide TZBSP error codes
*/

/*===========================================================================
   Copyright (c) 2010-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_errno.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/25/16   dpk      Added error code for blsp framework
08/20/10   cap      Initial version.

===========================================================================*/


/*===========================================================================

                              DEFINES

===========================================================================*/

/* Base error numbers */
#include <IxErrno.h>
#include "tzbsp_log.h"

/* Start of TZ Specific error codes */
#define E_NEXT_ERROR_CODE 256

/* Logs an error code. */
#define TZBSP_ERROR_CODE(xx) \
  TZBSP_LOG_ERR("(%u)", TZBSP_EC_##xx)

/* Conditionally logs an error code. Error code is logged only if error is set
 * in \c pred. */
#define TZBSP_ERROR_CODE_E(pred, xx) \
  TZBSP_LOG_ERR_E((pred),  "(%u)", TZBSP_EC_##xx)

/* Error codes for logging only. */
#define TZBSP_EC_SUBSYS_BRING_UP_INVALID_SUBSYS                         1
#define TZBSP_EC_SUBSYS_BRING_UP_FAILED                                 2
#define TZBSP_EC_SUBSYS_TEAR_DOWN_INVALID_SUBSYS                        3
#define TZBSP_EC_SUBSYS_TEAR_DOWN_FAILED                                4
#define TZBSP_EC_SUBSYS_IS_SUPPORTED_INVALID_SUBSYS                     5
#define TZBSP_EC_SUBSYS_IS_MANDATED_INVALID_SUBSYS                      6
#define TZBSP_EC_IS_BIND_IMAGE_INVALID_SUBSYS                           7
#define TZBSP_EC_GET_MMAP_AREAS_INVALID_SUBSYS                          8
#define TZBSP_EC_SUBSYS_IS_VALID_AREA_INVALID_SUBSYS                    9
#define TZBSP_EC_SUBSYS_IS_VALID_AREA_INVALID_SEGMENT                   10
#define TZBSP_EC_SUBSYS_LOCK_INVALID_SUBSYS                             11
#define TZBSP_EC_SUBSYS_LOCK_INVALID_BLIST                              12
#define TZBSP_EC_SUBSYS_LOCK_XPU_LOCK_AREA_FAILED                       13
#define TZBSP_EC_SUBSYS_LOCK_CONFIG_S_AREA_FAILED                       14
#define TZBSP_EC_SUBSYS_LOCK_ENABLE_S_AREA_FAILED                       15
#define TZBSP_EC_SUBSYS_UNLOCK_INVALID_SUBSYS                           16
#define TZBSP_EC_SUBSYS_UNLOCK_XPU_UNLOCK_FAILED                        17
#define TZBSP_EC_SUBSYS_UNLOCK_DISABLE_S_AREA_FAILED                    18
#define TZBSP_EC_PIL_MEM_AREA_UNSUPPORTED_PROC                          19
#define TZBSP_EC_PIL_MEM_AREA_NULL_START_ADDR                           20
#define TZBSP_EC_PIL_MEM_AREA_ZERO_IMAGE_LEN                            21
#define TZBSP_EC_PIL_MEM_AREA_NULL_PROG_HDR                             22
#define TZBSP_EC_PIL_MEM_AREA_INVALID_IMAGE_AREA                        23
#define TZBSP_EC_PIL_UNLOCK_AREA_USUPPORTED_PROC                        24
#define TZBSP_EC_PIL_UNLOCK_AREA_TEAR_DOWN_FAILED                       25
#define TZBSP_EC_PIL_UNLOCK_AREA_UNLOCK_XPU_FAILED                      26
#define TZBSP_EC_PIL_UNLOCK_AREA_UNLOCK_XPU_ZERO_FAILED                 27
#define TZBSP_EC_PIL_UNLOCK_AREA_UNLOCK_XPU_FAILED2                     28
#define TZBSP_EC_PIL_INIT_IMAGE_NULL_ELF_HDR                            29
#define TZBSP_EC_PIL_INIT_IMAGE_UNSUPPORTED_PROC                        30
#define TZBSP_EC_PIL_INIT_IMAGE_ELF_HDR_NOT_NONSECURE                   31
#define TZBSP_EC_PIL_INIT_IMAGE_IS_NOT_ELF                              32
#define TZBSP_EC_PIL_INIT_IMAGE_POPULATE_ELF_INFO_FAILED                33
#define TZBSP_EC_PIL_INIT_IMAGE_VERIFY_SIG_FAILED                       34
#define TZBSP_EC_POPULATE_ELF_NO_PROG_HDR                               35
#define TZBSP_EC_POPULATE_ELF_NUMBER_OF_SEGMENTS_INVALID                36
#define TZBSP_EC_POPULATE_ELF_ELF_HDR_NOT_IN_NS_MEMORY                  37
#define TZBSP_EC_POPULATE_ELF_PROG_HDR_NOT_IN_NS_MEMORY                 38
#define TZBSP_EC_POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY                 39
#define TZBSP_EC_POPULATE_ELF_IMAGE_HDR_TOO_SMALL                       40
#define TZBSP_EC_POPULATE_ELF_MALLOC_FAILED                             41
#define TZBSP_EC_POPULATE_ELF_MI_BOOT_INVALID                           42
#define TZBSP_EC_PIL_AUTH_RESET_UNSUPPORTED_PROC                        43
#define TZBSP_EC_PIL_AUTH_RESET_PROC_NOT_IN_RESET                       44
#define TZBSP_EC_PIL_AUTH_RESET_DECRYPT_FAILED                          45
#define TZBSP_EC_PIL_AUTH_RESET_AUTH_SEGMENTS_FAILED                    46
#define TZBSP_EC_PIL_IS_ELF_MAGIC_FAILED                                47
#define TZBSP_EC_PIL_IS_ELF_INVALID_CLASS                               48
#define TZBSP_EC_PIL_IS_ELF_INVALID_VERSION                             49
#define TZBSP_EC_PIL_IS_ELF_INVALID_EHSIZE                              50
#define TZBSP_EC_PIL_IS_ELF_INVALID_PHENTSIZE                           51
#define TZBSP_EC_MI_BOOT_VALID_MI_NULL                                  52
#define TZBSP_EC_MI_BOOT_VALID_ZERO_SIZE                                53
#define TZBSP_EC_MI_BOOT_VALID_LENGTH_MISMATCH                          54
//#define TZBSP_EC_MI_BOOT_VALID_UNSUPPORTED_PROC                         55
#define TZBSP_EC_IS_SUBSYSTEM_SUPPORTED_VERDICT_SIZE                    56
#define TZBSP_EC_IS_SUBSYSTEM_SUPPORTED_VERDICT_NOT_IN_NS_MEMORY        57
#define TZBSP_EC_IS_SUBSYSTEM_MANDATED_VERDICT_SIZE                     58
#define TZBSP_EC_IS_SUBSYSTEM_MANDATED_VERDICT_NOT_IN_NS_MEMORY         59
#define TZBSP_EC_AUTH_ELF_HEADERS_ELF_IS_NULL                           60
#define TZBSP_EC_AUTH_ELF_HEADERS_PROG_HDR_NUM_ZERO                     61
#define TZBSP_EC_AUTH_ELF_HEADERS_HASH_FAILED                           62
#define TZBSP_EC_AUTH_ELF_HEADERS_NULL_HASH                             63
#define TZBSP_EC_AUTH_ELF_HEADERS_INVALID_DIGEST                        64
#define TZBSP_EC_PIL_ZERO_SEGMENTS_SIZE_MISMATCH                        65
#define TZBSP_EC_AUTH_SEGMENT_ZERO_ARGS                                 66
#define TZBSP_EC_AUTH_SEGMENT_INVALID_HASH_LEN                          67
#define TZBSP_EC_AUTH_SEGMENT_HASH_FAILED                               68
#define TZBSP_EC_AUTH_SEGMENT_INVALID_DIGEST                            69
#define TZBSP_EC_PIL_AUTH_SEGMENTS_NULL_ELF                             70
#define TZBSP_EC_PIL_AUTH_SEGMENTS_ZERO_SEGMENTS_FAILED                 71
#define TZBSP_EC_PIL_AUTH_SEGMENTS_HASH_NULL                            72
#define TZBSP_EC_PIL_AUTH_SEGMENTS_LOCK_XPU_FAILED                      73
#define TZBSP_EC_PIL_AUTH_SEGMENTS_INVALID_HASH_LEN                     74
#define TZBSP_EC_PIL_AUTH_SEGMENTS_SEGMENT_AUTH_FAILED                  75
#define TZBSP_EC_PIL_AUTH_SEGMENTS_REMAINING_HASH_BYTES                 76
#define TZBSP_EC_PIL_VERIFY_SIG_NULL_ELF                                77
#define TZBSP_EC_PIL_VERIFY_SIG_ZERO_ARGS                               78
#define TZBSP_EC_PIL_VERIFY_SIG_ROT_NOT_FOUND                           79
#define TZBSP_EC_PIL_VERIFY_SIG_MPU_LOCK_FAILED                         80
#define TZBSP_EC_PIL_VERIFY_SIG_NO_ATTESTATION_CERT                     81
#define TZBSP_EC_PIL_VERIFY_SIG_BIND_IMAGE_FAILED                       82
#define TZBSP_EC_PIL_VERIFY_SIG_INVALID_SIG_SZ                          83
#define TZBSP_EC_PIL_VERIFY_SIG_MALLOC_FAILED                           84
#define TZBSP_EC_PIL_VERIFY_SIG_INVALID_SIGNATURE                       85
#define TZBSP_EC_PIL_VERIFY_SIG_INVALID_ELF_HEADERS                     86
#define TZBSP_EC_VALIDATE_IMAGE_AREA_INVALID_ENTRY                      87
#define TZBSP_EC_VALIDATE_IMAGE_AREA_INVALID_SEGMENT                    88
#define TZBSP_EC_PIL_LOCK_XPU_IMAGE_AREA_INVALID                        89
#define TZBSP_EC_PIL_LOCK_XPU_SUBSYS_LOCK_FAILED                        90
#define TZBSP_EC_PIL_UNLOCK_XPU_TEAR_DOWN_FAILED                        91
#define TZBSP_EC_PIL_UNLOCK_XPU_UNLOCK_FAILED                           92
#define TZBSP_EC_PIL_UNLOCK_ZERO_XPU_TEAR_DOWN_FAILED                   93
#define TZBSP_EC_PIL_UNLOCK_ZERO_XPU_UNLOCK_FAILED                      94
#define TZBSP_EC_PIL_INIT_IMAGE_SSD_PARSE_MD_FAILED                     95
#define TZBSP_EC_PIL_AUTH_RESET_SUBSYS_BRING_UP_FAILED                  96
#define TZBSP_EC_PIL_ROLLBACK_VERIFY_VERSION                            97
#define TZBSP_EC_VIDEO_TEAR_DOWN_SMMU_DEINIT_FAIL                       98
#define TZBSP_EC_VIDEO_TEAR_DOWN_SMMU_CONFIG_FAIL                       99
#define TZBSP_EC_VIDEO_BRING_UP_SMMU_CONFIG_FAIL                        100
#define TZBSP_EC_VIDEO_BRING_UP_SMMU_INIT_FAIL                          101
#define TZBSP_EC_SMMU_CONFIG2_ENABLE_FAIL                               102
#define TZBSP_EC_SMR_CPDATA_INIT_FAIL                                   103
#define TZBSP_EC_SMMU_CONFIG_ENABLE_FAIL                                104
#define TZBSP_EC_SMR_SMEM_INIT_FAIL                                     105
#define TZBSP_EC_SMMU_CONFIG2_RESTORE_FAIL                              106
#define TZBSP_EC_SMMU_CONFIG_RESTORE_FAIL                               107
#define TZBSP_EC_SMR_CPDATA_DEINIT_FAIL                                 108
#define TZBSP_EC_SMR_SMEM_DEINIT_FAIL                                   109
#define TZBSP_EC_TZBSP_CONFIG_S_AREA_FAIL                               110
#define TZBSP_EC_TZBSP_ENABLE_S_AREA_FAIL                               111
#define TZBSP_EC_TZBSP_DISABLE_S_AREA_FAIL                              112
#define TZBSP_EC_CPDATA_TZBSP_VIDEO_INIT_FAILED                         113
#define TZBSP_EC_SMEM_TZBSP_VIDEO_INIT_FAILED                           114
#define TZBSP_EC_SMR_TZBSP_VIDEO_DEINIT_FAILED                          115
#define TZBSP_EC_PIL_INIT_SEC_APP_ELF_HDR_NOT_SECURE                    116
#define TZBSP_EC_INIT_MMU_INIT_FAILED                                   117
#define TZBSP_EC_INIT_DEVICE_UNMAP_FAILED                               118
#define TZBSP_EC_INIT_DDR_INIT_FAILED                                   119
#define TZBSP_EC_INIT_HW_INIT_FAILED                                    120
#define TZBSP_EC_INIT_CHIPSET_INIT_FAILED                               121
#define TZBSP_EC_INIT_COLD_INIT_HANDLER_FAILED                          122
#define TZBSP_EC_INIT_PIL_INIT_FAILED                                   123
#define TZBSP_EC_INIT_BOOT_TAMPER_CHECK_FAILED                          124

#define TZBSP_EC_SUBSYS_INVALID_PROC_SW_TYPE                            125
#define TZBSP_EC_XPU_STATIC_CFG_FAIL                                    126
#define TZBSP_EC_VMIDMT_CFG_FAIL                                        127
#define TZBSP_EC_PMIC_INIT_FAIL                                         128
#define TZBSP_EC_DEHR_CFG_FAIL                                          129
#define TZBSP_EC_BAM_INIT_FAIL                                          130
#define TZBSP_EC_SMMU_STATIC_INIT_FAIL                                  131
#define TZBSP_EC_HW_CRYPTO_INIT_FAIL                                    132
#define TZBSP_EC_PIL_INIT_IMAGE_SYBSYS_IMAGE_INIT_OK_FAILED             133
#define TZBSP_EC_INIT_SECURE_CHANNEL_KEY_INIT_FAILED                    134
#define TZBSP_EC_INIT_RESET_EXEC_FAILED                                 135
#define TZBSP_EC_STACK_CHECK_FAILED                                     136
#define TZBSP_EC_RESTORE_LPASS_VMIDMT_FAILED                            137
#define TZBSP_EC_RESTORE_LPASS_BAMCFG_FAILED                            138
#define TZBSP_EC_INIT_CHIPSET_REINIT_FAILED                             139

#define TZBSP_EC_INIT_DEMAND_PAGE_FAILED                                140

#define TZBSP_EC_PIL_MEM_AREA_PROC_BAD_STATE                            141
#define TZBSP_EC_SUBSYS_SET_MEM_INVALID_IMAGE_AREA                      142
#define TZBSP_EC_SUBSYS_SET_MEM_INVALID_ADDR                            143
#define TZBSP_EC_SUBSYS_SET_MEM_INVALID_SUBSYS                          144
#define TZBSP_EC_SUBSYS_SET_MEM_ADDR_IN_USE                             145
#define TZBSP_EC_VIDEO_SMMU_INIT_FAILED                                 146
#define TZBSP_EC_VIDEO_SMMU_PT_CLEAR_FAILED                             147
#define TZBSP_EC_VIDEO_SMMU_MAP_SEGMENT_FAILED                          148
#define TZBSP_EC_VIDEO_INVALID_FW_ADDR_RANGE                            149
#define TZBSP_EC_PIL_AUTH_RESET_ELF_ADDR_ADJUST_FAILED                  150
#define TZBSP_EC_PIL_ADJUST_ELF_GET_MEM_RANGE_FAILED                    151
#define TZBSP_EC_PIL_ADJUST_ELF_SIZE_MISMATCH                           152
#define TZBSP_EC_VIDEO_FW_NOT_LOADED                                    153
#define TZBSP_EC_VIDEO_XPU_CONFIG_FAILED                                154
#define TZBSP_EC_VIDEO_VMIDMT_CONFIG_FAILED                             155
#define TZBSP_EC_VIDEO_BRING_UP_FAILED                                  156
#define TZBSP_EC_VIDEO_FW_CB_TLB_INVALIDATE_FAILED                      157
#define TZBSP_EC_PIL_PARSE_MD_S_FAILED                                  158
#define TZBSP_EC_PIL_DECRYPT_ELF_SEG_S_FAILED                           159
#define TZBSP_EC_IMAGE_VERSION_POPULATE_FAILED                          160

#define TZBSP_EC_BLACK_LIST_ADJUST_FAILED                               161
#define TZBSP_EC_SUBSYS_INFO_HASH_FAILED                                162
#define TZBSP_EC_SUBSYS_INFO_INV_PARAMS                                 163

#define TZBSP_EC_GFX_DCVS_INIT_BAD_DATA_RANGE                           162
#define TZBSP_EC_GFX_DCVS_INIT_BAD_PWRLEVEL_1                           163
#define TZBSP_EC_GFX_DCVS_INIT_BAD_PWRLEVEL_2                           164
#define TZBSP_EC_GFX_DCVS_INIT_NULL_DATA                                165

#define TZBSP_EC_SYSCALL_DENIED_DPC_BLOCKED                             166

#define TZBSP_EC_VPU_FW_NOT_LOADED                                      167
#define TZBSP_EC_VPU_XPU_CONFIG_FAILED                                  168
#define TZBSP_EC_VPU_VMIDMT_CONFIG_FAILED                               169
#define TZBSP_EC_VPU_BRING_UP_FAILED                                    170
#define TZBSP_EC_VPU_FW_CB_TLB_INVALIDATE_FAILED                        171
#define TZBSP_EC_VPU_SMMU_INIT_FAILED                                   172
#define TZBSP_EC_VPU_SMMU_PT_CLEAR_FAILED                               173
#define TZBSP_EC_VPU_SMMU_MAP_SEGMENT_FAILED                            174
#define TZBSP_EC_VPU_INVALID_FW_ADDR_RANGE                              175

#define TZBSP_EC_BCSS_FW_NOT_LOADED                                     176
#define TZBSP_EC_BCSS_XPU_CONFIG_FAILED                                 177
#define TZBSP_EC_BCSS_VMIDMT_CONFIG_FAILED                              178
#define TZBSP_EC_BCSS_BRING_UP_FAILED                                   179
#define TZBSP_EC_BCSS_FW_CB_TLB_INVALIDATE_FAILED                       180
#define TZBSP_EC_BCSS_SMMU_INIT_FAILED                                  181
#define TZBSP_EC_BCSS_SMMU_PT_CLEAR_FAILED                              182
#define TZBSP_EC_BCSS_SMMU_MAP_SEGMENT_FAILED                           183
#define TZBSP_EC_BCSS_INVALID_FW_ADDR_RANGE                             184

#define TZBSP_EC_VCAP_SMMU_INIT_FAILED                                  185
#define TZBSP_EC_VCAP_XPU_CONFIG_FAILED                                 186

#define TZBSP_EC_OCMEM_0                                                187
#define TZBSP_EC_OCMEM_1                                                188
#define TZBSP_EC_OCMEM_2                                                189
#define TZBSP_EC_OCMEM_3                                                190
#define TZBSP_EC_OCMEM_4                                                191
#define TZBSP_EC_OCMEM_5                                                192
#define TZBSP_EC_OCMEM_6                                                193
#define TZBSP_EC_OCMEM_7                                                194
#define TZBSP_EC_OCMEM_8                                                195
#define TZBSP_EC_OCMEM_9                                                196
#define TZBSP_EC_OCMEM_A                                                197
#define TZBSP_EC_OCMEM_B                                                198
#define TZBSP_EC_OCMEM_C                                                199
#define TZBSP_EC_OCMEM_D                                                200
#define TZBSP_EC_OCMEM_E                                                201

#define TZBSP_EC_BLSP_INIT_FAIL                                         219

#define TZBSP_EC_CRYPTO_NULL_KEY_ERROR                                  220

#define TZBSP_EC_VIDEO_ALREADY_OUT_OF_RESET                             221

#define  TZBSP_EC_MEM_ALLOCATE_ALIGNED_BAD_INPUT                        232
#define  TZBSP_EC_MEM_REGION_NOT_DEFINED                                233
#define  TZBSP_EC_MEM_SET_MASK_BAD_INPUT                                234
#define  TZBSP_EC_MEM_FREE_AND_UNMAP_VTOP                               235
#define  TZBSP_EC_MEM_FREE_ALIGNED_BAD_INPUT                            236
#define  TZBSP_EC_MEM_COUNTER_TOO_HIGH                                  237
#define  TZBSP_EC_MEM_PRNG_FAIL                                         238
#define  TZBSP_EC_MEM_RANDOM_MALLOC_FAIL                                239
#define  TZBSP_EC_MEM_RANDOM_BAD_INPUT                                  240
#define  TZBSP_EC_MEM_BESTFIT_BAD_INPUT                                 241
#define  TZBSP_EC_MEM_ALLOCATE_NO_SPACE                                 242

#define TZBSP_EC_MEM_CFG_ERR_OVER_4GB                                   260
#define TZBSP_EC_INIT_NAND_QPIC_MPU_CONFIG_FAIL                         261
  
#define TZBSP_EC_CLK_SUPPORT_NOT_AVAILABLE                              262
#define TZBSP_EC_CLK_CLKID_INVALID                                      263
#define TZBSP_EC_CLK_ENABLE_FAIL                                        264
#define TZBSP_EC_CLK_DISABLE_FAIL                                       265

/* This should be the last error code. */
#define TZBSP_EC_SSD_ERROR_BASE                                         300

/* Integer overflow check */
#define TZBSP_EC_MI_BOOT_INVALID_SIZE                         			301

#define TZBSP_EC_CLOCK_DRIVER_BASE                                      500


#endif
