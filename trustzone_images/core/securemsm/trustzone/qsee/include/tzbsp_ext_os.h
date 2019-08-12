#ifndef TZBSP_EXT_OS_H
#define TZBSP_EXT_OS_H

/**
@file tzbsp_ext_os.h
@brief Provides API for external OS functionality
*/

/*===========================================================================
   Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_ext_os.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/11/12   th       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Header files used for definitions required during wrapping calls to TZBSP
 * -------------------------------------------------------------------------*/
#include "tzbsp_chipset.h"
#include "tzbsp_mmu_cache.h"
#include "tz_securechannel.h"
#include "tzbsp_i2c.h"
#include "crypto/tzbsp_crypto.h"
#include "crypto/tzbsp_hash.h"
#include "crypto/tzbsp_cipher.h"
#include "crypto/tzbsp_hmac.h"
#include "crypto/tzbsp_prng.h"
#include "crypto/tzbsp_kdf.h"
#include "tt_sd.h"
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Virtual memory addresses for memory relocated when the external OS is
 * being executed */
#define TZBSP_EXT_OS_RELOC_BASE_0_PHYS   0x00000000
#define TZBSP_EXT_OS_RELOC_BASE_0_VIRT   0xF8200000
#define TZBSP_EXT_OS_RELOC_0_SIZE        0x00C00000

#define TZBSP_EXT_OS_RELOC_BASE_1_PHYS  (TZBSP_EXT_OS_RELOC_BASE_0_PHYS + \
                                         TZBSP_EXT_OS_RELOC_0_SIZE)
#define TZBSP_EXT_OS_RELOC_BASE_1_VIRT   0xF9400000
#define TZBSP_EXT_OS_RELOC_1_SIZE        0x00400000

#define TZBSP_EXT_OS_RELOC_BASE_2_PHYS  (TZBSP_EXT_OS_RELOC_BASE_1_PHYS + \
                                         TZBSP_EXT_OS_RELOC_1_SIZE)
#define TZBSP_EXT_OS_RELOC_BASE_2_VIRT   0xFF000000
#define TZBSP_EXT_OS_RELOC_2_SIZE        0x01000000

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*
* @brief
*   Checks the state of the memory mapping and returns TRUE if the system is
*   currently flat mapped (1-1 PtoV addressing)
*
* @param[in]
*   None
*
* @par Dependencies
*   None
*
* @retval
*   TRUE  if flat mapped
*   FALSE if memory is currently not 1-1 VtoP mapped
*
* @par Side Effects
*   None
*
*/
boolean tzbsp_sysdbg_flat_mapped(void);

/**
 * @brief Configure the L1 TT copy used by TrustZone
 */
void tzbsp_configure_ext_os_l1tt(void);

/**
 * @brief Obtains L1 table base address from TTBR0 or
 * chooses default if TTBR0 not configured
 */
l1_tt_t *tzbsp_get_l1_ptr(void);

/**
 * @brief Stores the TZBSP L1 TT entries and maps in the first 32MB of virtual
 *        address space for use by the external OS. The previous L1 TT entries
 *        will be saved so that they can be restored once the external OS has
 *        finished execution.
 *
 *        The first 32MB of physical memory is remapped so that it can still be
 *        accessed by the external OS, but now without a 1-1 mapping. The
 *        external OS must handle the translation of virtual to physical for
 *        the first 32MB of memory.
 *
 * @note  Should only be called from Core 0
 */
void tzbsp_map_in_ext_os(void);

/**
 * @brief The mapping from 0-32MB will be reverted to its original state.
 *
 *        The reserved regions will be unmapped to prevent access.
 *
 * @note  Should only be called from Core 0
 */
void tzbsp_map_out_ext_os(void);

/**
 * @brief Reads the current value of TTBR0
 *        Sets TTBR0 to be the value required by the external OS
 *        Returns the previous value of TTBR0
 *
 * @return \c The value of TTBR0 before it was updated for the external OS
 *
 * @note  Should only be called from Core 0 in FIQ context
 */
uint32 tzbsp_map_in_ext_os_fiq(void);

/**
 * @brief Sets TTBR0 to the value passed in
 *
 * @param vbar   [in] Value to write in to the TTBR0 register
 *
 * @note  Should only be called from Core 0 and in FIQ context
 */
void tzbsp_map_out_ext_os_fiq(uint32 ttbr0);

/**
 * @brief Sets VBAR for QSEE.
 *
 * @return \c The value of VBAR before it was updated for QSEE
 */
uint32 tzbsp_vbar_qsee(void);

/**
 * @brief Restores VBAR to the value it was set to prior to tzbsp_vbar_qsee()
 *
 * @param vbar   [in] Value to write in to the VBAR register
 */
void tzbsp_vbar_restore(uint32 vbar);

/* The following functions wrap calls to TZBSP from the external OS, but ensure
 * that the VBAR is correctly set for QSEE to enable demand paging. Without this
 * wrapping it is not possible to page in memory to TZBSP for functions that are
 * not currently mapped. */

/* Interrupt Group */
uint32 tzbsp_int_disable_all_ext_os(void);

void tzbsp_int_restore_ext_os(uint32 flags);

/* Core Group */
int tzbsp_is_sw_fuse_blown_secure_ext_os(uint32   fuse_num,
                                         boolean* is_blown,
                                         uint32   is_blown_sz);

int tzbsp_blow_sw_fuse_ext_os(uint32 fuse_num);

int tzbsp_fuse_write_ext_os(uint32  raw_row_address,
                            uint32 *row_data,
                            uint32  bus_clk_khz,
                            uint32* qfprom_api_status);

int tzbsp_fuse_read_ext_os(uint32    row_address,
                           int32     addr_type,
                           uint32*   row_data,
                           uint32*   qfprom_api_status);

int tzbsp_jtag_override_ext_os(uint32 debug);

boolean tzbsp_is_ns_range_ext_os(const void* start, uint32 len);

int tzbsp_mmss_hdmi_status_read(uint32* hdmi_enable,
                                uint32* hdmi_sense,
                                uint32* hdcp_auth);

int tzbsp_mmss_hdmi_status_read_ext_os(uint32* hdmi_enable,
                                       uint32* hdmi_sense,
                                       uint32* hdcp_auth);

int tzbsp_i2c_open_ext_os(tzbsp_i2cpd_device_id_t device_id);

int tzbsp_i2c_read_ext_os(tzbsp_i2cpd_device_id_t        device_id,
                          const tzbsp_i2c_config_t*      p_config,
                          tzbsp_i2c_transaction_info_t*  p_read_info);

int tzbsp_i2c_write_ext_os(tzbsp_i2cpd_device_id_t       device_id,
                           const tzbsp_i2c_config_t*     p_config,
                           tzbsp_i2c_transaction_info_t* p_write_info);

int tzbsp_i2c_close_ext_os(tzbsp_i2cpd_device_id_t device_id);

int tzbsp_mem_usage_count_ext_os(uint32 usage);

int tzbsp_sd_get_session_ext_os(void);

int tzbsp_tag_mem_ext_os(uint32 tag,
                          uintptr_t start, uintptr_t end);

/* MMU Group */
void tzbsp_tlb_inval_all_ext_os(void);

int tzbsp_itlb_inval_all_ext_os(void);

int tzbsp_dtlb_inval_all_ext_os(void);

void tzbsp_cache_flush_ext_os(void);

int tzbsp_dcache_inval_all_ext_os(void);

void tzbsp_dcache_flush_all_ext_os(void);

int tzbsp_dcache_clean_all_ext_os(void);

int tzbsp_dcache_inval_region_ext_os(void *addr, unsigned int length);

int tzbsp_dcache_flush_region_ext_os(void *addr, unsigned int length);

int tzbsp_dcache_clean_region_ext_os(void *addr, unsigned int length);

int tzbsp_tlb_inval_all_is_ext_os(void);

int tzbsp_tlb_inval_by_asid_is_ext_os(uint32 asid);

int tzbsp_tlb_inval_by_mva_is_ext_os(uint32 mva);

int tzbsp_bp_invalidate_all_ext_os(void);

int tzbsp_icache_inv_ext_os(void);

/* Secure Channel Group */
int tzbsp_secure_message_ext_os(tz_sc_ss_e_type        ssid,
                                tz_sc_cid_e_type       cid,
                                const uint8*           input_msg_ptr,
                                uint32                 input_msg_len,
                                uint8*                 output_msg_ptr,
                                uint32*                output_msg_len_ptr);

int tzbsp_authenticate_decrypt_message_ext_os(
                                tz_sc_ss_e_type        ssid,
                                tz_sc_cid_e_type       cid,
                                const uint8*           input_msg_ptr,
                                uint32                 input_msg_len,
                                uint8*                 output_msg_ptr,
                                uint32*                output_msg_len_ptr);

/* Crypto Group */
int tzbsp_hash_ext_os(TZBSP_HASH_ALGO_ET alg,
                      const uint8       *msg,
                      uint32             msg_len,
                      uint8             *digest,
                      uint32             digest_len);

int tzbsp_hash_init_ext_os(TZBSP_HASH_ALGO_ET   alg,
                           tzbsp_hash_ctx     **hash_ctx);

int tzbsp_hash_update_ext_os(const tzbsp_hash_ctx  *hash_ctx,
                             const uint8           *msg,
                             uint32                 msg_len);

int tzbsp_hash_final_ext_os(const tzbsp_hash_ctx  *hash_ctx,
                            uint8                 *digest,
                            uint32                 digest_len);

int tzbsp_hash_free_ctx_ext_os(tzbsp_hash_ctx *hash_ctx);

int tzbsp_cipher_init_ext_os(TZBSP_CIPHER_ALGO_ET   alg,
                             tzbsp_cipher_ctx     **cipher_ctx);

int tzbsp_cipher_free_ctx_ext_os(tzbsp_cipher_ctx *cipher_ctx);

int tzbsp_cipher_set_param_ext_os(tzbsp_cipher_ctx     *cipher_ctx,
                                  TZBSP_CIPHER_PARAM_ET param_id,
                                  const void           *param,
                                  uint32                param_len);

int tzbsp_cipher_get_param_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                  TZBSP_CIPHER_PARAM_ET   param_id,
                                  void                   *param,
                                  uint32                 *param_len);

int tzbsp_cipher_encrypt_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                const uint8            *pt,
                                uint32                  pt_len,
                                uint8                  *ct,
                                uint32                 *ct_len);

int tzbsp_cipher_decrypt_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                const uint8            *ct,
                                uint32                  ct_len,
                                uint8                  *pt,
                                uint32                 *pt_len);

int tzbsp_hmac_ext_os(TZBSP_HMAC_ALGO_ET alg,
                      const uint8       *msg,
                      uint32             msg_len,
                      const uint8       *key,
                      uint16             key_len,
                      uint8             *msg_digest);

uint32 tzbsp_prng_getdata_ext_os(uint8 *out, uint32 out_len);

boolean tzbsp_crypto_lock_engine_ext_os(boolean lock);

uint32 tzbsp_kdf_ext_os(const void  *input_key,
                        uint32       input_key_len,
                        const void  *label,
                        uint32       label_len,
                        const void  *context,
                        uint32       context_len,
                        void        *output_key,
                        uint32       output_key_len);

#endif /* TZBSP_EXT_OS_H */

