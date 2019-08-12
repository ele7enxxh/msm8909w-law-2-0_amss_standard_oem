#ifndef __TZBSP_EXEC_DEFS_H__
#define __TZBSP_EXEC_DEFS_H__

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_exec_defs.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
03/31/2011 pre      Initial
===========================================================================*/

#define TZBSP_END_OF_EXEC_TBL_ID 0xffffffff

#define TZBSP_BOOT_GROUP_ID                    1
#define TZBSP_REGISTER_MONITOR_HANDLERS_ID     1
#define TZBSP_T32_PRINTF_ID                    2
#define TZBSP_READ_FNGRP_EXT                   3
#define TZBSP_GET_DDR_LAYOUT_EXT               4
#define TZBSP_SSD_DECRYPT_IMG                  5

#define TZBSP_INTERRUPT_GROUP_ID               2
#define TZBSP_REGISTER_INT_ID                  1
#define TZBSP_DEREGISTER_INT_ID                2
#define TZBSP_READ_INT_MSK_ID                  3
#define TZBSP_CLR_INT_ID                       4
#define TZBSP_INT_DISABLE_ALL_ID               5
#define TZBSP_INT_RESTORE_ID                   6
#define TZBSP_GEN_IRQ_ID                       7
#define TZBSP_EXEC_GEN_FIQ_ID                  8

#define TZBSP_TIMER_GROUP_ID                   3
#define TZBSP_GPT_TIMER_START_ID               1
#define TZBSP_GPT_STOP_ID                      2
#define TZBSP_GPT_ENABLE_INT_ID                3
#define TZBSP_GPT_DISABLE_INT_ID               4
#define TZBSP_SEC_CTRL_TIMER_START_ID          5
#define TZBSP_SEC_CTRL_TIMER_COUNT_ID          6
#define TZBSP_WDT_START_ID                     7
#define TZBSP_WDT_STOP_ID                      8
#define TZBSP_WDT_PET_ID                       9

#define TZBSP_CRYPTO_GROUP_ID                  4
#define TZBSP_HASH_ID                          1
#define TZBSP_HASH_INIT_ID                     2
#define TZBSP_HASH_UPDATE_ID                   3
#define TZBSP_HASH_FINAL_ID                    4
#define TZBSP_HASH_FREE_CTX_ID                 5
#define TZBSP_CIPHER_INIT_ID                   6
#define TZBSP_CIPHER_FREE_CTX_ID               7
#define TZBSP_CIPHER_SET_PARAM_ID              8
#define TZBSP_CIPHER_GET_PARAM_ID              9
#define TZBSP_CIPHER_ENCRYPT_ID               10
#define TZBSP_CIPHER_DECRYPT_ID               11
#define TZBSP_HMAC_ID                         12
#define TZBSP_PRNG_ID                         13
#define TZBSP_LOCK_CRYPTO_ID                  14
#define TZBSP_KDF_ID                          15
#define TZBSP_CMAC_ID                         16
#define TZBSP_CIPHER_RESET_ID                 17
#define TZBSP_HASH_RESET_ID                   18
#define TZBSP_HASH_SET_PARAM_ID               19
#define TZBSP_HMAC_INIT_ID                    20
#define TZBSP_HMAC_UPDATE_ID                  21
#define TZBSP_HMAC_FINAL_ID                   22
#define TZBSP_HMAC_FREE_CTX_ID                23


#define TZBSP_CORE_GROUP_ID                    5
#define TZBSP_PROTECT_MEM                      1
#define TZBSP_IS_SW_FUSE_BLOWN                 2
#define TZBSP_BLOW_SW_FUSE                     3
#define TZBSP_READ_JTAG_ID                     4
#define TZBSP_READ_SERIAL_NUM                  5
#define TZBSP_FUSE_WRITE                       6
#define TZBSP_FUSE_READ                        7
#define TZBSP_JTAG_OVERRIDE                    8
#define TZBSP_IS_NS_RANGE                      9
#define TZBSP_IS_S_TAG_AREA                   10
#define TZBSP_HDMI_STATUS_READ                11
#define TZBSP_I2C_OPEN                        12
#define TZBSP_I2C_READ                        13
#define TZBSP_I2C_WRITE                       14
#define TZBSP_I2C_CLOSE                       15
#define TZBSP_MEM_USAGE_COUNT                 16
#define TZBSP_SD_GET_SESSION                  17
#define TZBSP_TAG_MEM                         18

#define TZBSP_MMU_CACHE_GROUP_ID               6
#define TZBSP_TLB_INVAL_ALL_ID                 1
#define TZBSP_ITLB_INVAL_ALL_ID                2
#define TZBSP_DTLB_INVAL_ALL_ID                3
#define TZBSP_CACHE_FLUSH_ID                   4
#define TZBSP_DCACHE_INVAL_ALL_ID              5
#define TZBSP_DCACHE_FLUSH_ALL_ID              6
#define TZBSP_DCACHE_CLEAN_ALL_ID              7
#define TZBSP_DCACHE_INVAL_REGION_ID           8
#define TZBSP_DCACHE_FLUSH_REGION_ID           9
#define TZBSP_DCACHE_CLEAN_REGION_ID          10
#define TZBSP_TLB_INVAL_ALL_IS_ID             11
#define TZBSP_TLB_INVAL_BY_ASID_IS_ID         12
#define TZBSP_TLB_INVAL_BY_MVA_IS_ID          13
#define TZBSP_BP_INVALIDATE_ALL_ID            14
#define TZBSP_ICACHE_INV_ALL_IS_ID            15

#define TZBSP_SECURE_CHANNEL_GROUP_ID          7
#define TZBSP_SECURE_MESSAGE_ID                1
#define TZBSP_AUTHENTICATE_DECRYPT_MESSAGE_ID  2


/** @deprecated Use \c TZBSP_EXEC_PROTECT_MEM instead. SMI0 was specific to
 * 8660 only. */
#define TZBSP_EXEC_SMI0                        1

/** Should be passed in as a dummy argument to memory protection. Replaces the
 * now deprecated usage of \c TZBSP_EXEC_SMI0. */
#define TZBSP_EXEC_PROTECT_MEM                 1

#endif
