/**
 * @file tzbsp_ext_os.c
 * @brief Provides external OS related functionality.

*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_ext_os.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
12/11/12   th       Initial version
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <HALhwio.h>
#include <string.h>
#include <busywait.h>

#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_syscall.h"
#include "tz_syscall_pub.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_mem.h"
#include "tzbsp_target.h"
#include "tzbsp_mmu_cache.h"
#include "tt_sd.h"
#include "tt_tz.h"
#include "tzbsp_krait_cp15.h"
#include "tzbsp_log.h"
#include "tzbsp_ext_os.h"
#include "tzbsp_memory_map.h"
#include "IxErrno.h"
#include <stringl/stringl.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Size of the remapped region of the L1 translation table */
#define TZBSP_EXT_OS_RELOC_SIZE         (TZBSP_EXT_OS_RELOC_0_SIZE +    \
                                         TZBSP_EXT_OS_RELOC_1_SIZE +    \
                                         TZBSP_EXT_OS_RELOC_2_SIZE)
#define TZBSP_EXT_OS_NUM_REMAP_TTES     ((TZBSP_EXT_OS_RELOC_SIZE /  \
                                          L1_MAP_SIZE) + \
                                         ((TZBSP_EXT_OS_RELOC_SIZE % \
                                           L1_MAP_SIZE) > 0))


/*----------------------------------------------------------------------------
 * External References
 * -------------------------------------------------------------------------*/

int tzbsp_fuse_write(uint32 raw_row_address, uint32 *row_data,
                     uint32 bus_clk_khz,     uint32* qfprom_api_status);
int tzbsp_fuse_read(uint32 row_address, int32 addr_type,
                    uint32 *row_data,   uint32 *qfprom_api_status);
int tz_is_sw_fuse_blown_secure(uint32   fuse_num,
                                  boolean* is_blown,
                                  uint32   is_blown_sz);
int tz_blow_sw_fuse(uint32 fuse_num);

extern l1_tt_t l1;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
  uint32 vbar;
  uint32 ttbr0;
  sd_tte_t *tt_entry;
} t_ext_os_l1_tt_store;

typedef struct
{
  uint32               ref_count;
  t_ext_os_l1_tt_store tzbsp_l1_tt;
  t_ext_os_l1_tt_store tzbsp_ext_os_l1_tt;
} t_ext_os_l1_tt;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* Structure used to store/restore the page table entries swapped out during
 * execution of the external OS */
static t_ext_os_l1_tt ext_os_l1_tt = {0};

static uint32 cpsr_saved_state;
static uint32 scr_saved_state;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @copydoc tzbsp_sysdbg_flat_mapped
 */
boolean tzbsp_sysdbg_flat_mapped(void)
{
  if (!tzbsp_mmu_is_enabled())
  {
    return TRUE;
  }
  if( ( ( rcp15_ttbr0() & 0xFFFFC000 )        != (uint32)ext_os_l1_tt.tzbsp_l1_tt.tt_entry     ) && 
      ( ( rcp15_ttbr0() & 0xFFFFC000 )        !=         ext_os_l1_tt.tzbsp_ext_os_l1_tt.ttbr0 ) &&
      ( ext_os_l1_tt.tzbsp_ext_os_l1_tt.ttbr0 != 0 ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * @copydoc tzbsp_configure_ext_os_l1tt
 */
void tzbsp_configure_ext_os_l1tt(void)
{
  /* This function is called before trusted OS is executed the first time */
  ext_os_l1_tt.tzbsp_l1_tt.tt_entry        = (uint32 *)&l1.tt[0];
  ext_os_l1_tt.tzbsp_l1_tt.vbar            = rcp15_vbar();
  ext_os_l1_tt.tzbsp_ext_os_l1_tt.vbar     = ext_os_l1_tt.tzbsp_l1_tt.vbar;
  ext_os_l1_tt.tzbsp_l1_tt.ttbr0           = rcp15_ttbr0();
  ext_os_l1_tt.tzbsp_ext_os_l1_tt.tt_entry = (uint32 *)(TZBSP_EXT_ELF_TT_BASE);

  memscpy(ext_os_l1_tt.tzbsp_ext_os_l1_tt.tt_entry, 
          NUM_L1_TTES*TTE_SIZE, 
          ext_os_l1_tt.tzbsp_l1_tt.tt_entry, 
          NUM_L1_TTES*TTE_SIZE);

  ext_os_l1_tt.tzbsp_ext_os_l1_tt.ttbr0 = (((uint32)(ext_os_l1_tt.tzbsp_ext_os_l1_tt.tt_entry)) | 
                                                    (ext_os_l1_tt.tzbsp_l1_tt.ttbr0 & 0x00003FFF));
}

/**
 * @copydoc tzbsp_get_l1_ptr
 */
l1_tt_t * tzbsp_get_l1_ptr(void)
{
  // if we are configured
  if(ext_os_l1_tt.tzbsp_l1_tt.ttbr0 ==0)
  {
    // get base of L1 table
    return &l1;
  }
   else
  {
    // get base of L1 table from ttbr0
    return (l1_tt_t *)(rcp15_ttbr0() & 0xFFFFC000);
  }
}

/**
 * @copydoc tzbsp_map_in_ext
 */
void tzbsp_map_in_ext(void)
{
  ext_os_l1_tt.tzbsp_l1_tt.vbar = rcp15_vbar();
  wcp15_ttbr0_safe(ext_os_l1_tt.tzbsp_ext_os_l1_tt.ttbr0);
  wcp15_vbar_safe(ext_os_l1_tt.tzbsp_ext_os_l1_tt.vbar);
}

/**
 * @copydoc tzbsp_map_in_ext_os
 */
void tzbsp_map_in_ext_os(void)
{
  uint32 state = 0;

  /* Disable interrupt before modifying TTBR0 */
  state = tzbsp_int_disable_all();

  /* Reference counting */
  if(0 == ext_os_l1_tt.ref_count)
  {
    cpsr_saved_state = state;
    tzbsp_map_in_ext();

    /*
    We force the I/F bit the be on when entering in t-base
    and they will disable them afterwards. This is to avoid race conditions
    during the transition between tzbsp and t-base.
    */

    state |= (TZBSP_IRQ_BIT | TZBSP_FIQ_BIT);

    /*
    For the core tbase is executing on then we set the SCR to ensure 
    that FIQ/IRQ/EA are all caught in SVC mode. This will ensure that 
    these aborts are caught by the tbase kernel. The SCR register 
    must be restored when tbase execution has completed so that the 
    FIQ/IRQ/EA aborts are once again caught in the MON mode. 
    */

    scr_saved_state = rcp15_scr();
    wcp15_scr(scr_saved_state & ~(SCR_IRQ_BIT | SCR_FIQ_BIT | SCR_EA_BIT));

  }

  /* Increment the reference count */
  ext_os_l1_tt.ref_count += 1;

  /* Re-enable interrupts */
  tzbsp_int_restore(state);
}

/**
 * @copydoc tzbsp_map_in_ext_os_fiq
 */
uint32 tzbsp_map_in_ext_os_fiq(void)
{
  uint32 ttbr0 = rcp15_ttbr0();
  wcp15_ttbr0_safe(ext_os_l1_tt.tzbsp_ext_os_l1_tt.ttbr0);
  return ttbr0;
}

/**
 * @copydoc tzbsp_map_out_ext
 */
void tzbsp_map_out_ext(void)
{
  ext_os_l1_tt.tzbsp_ext_os_l1_tt.vbar = rcp15_vbar();
  wcp15_vbar_safe(ext_os_l1_tt.tzbsp_l1_tt.vbar);
  wcp15_ttbr0_safe(ext_os_l1_tt.tzbsp_l1_tt.ttbr0);
}

/**
 * @copydoc tzbsp_map_out_ext_os
 */
void tzbsp_map_out_ext_os(void)
{
  uint32 state = 0;

  /* Disable interrupt before modifying the page tables */
  state = tzbsp_int_disable_all();

  /* Decrement the reference count */
  ext_os_l1_tt.ref_count -= 1;

  /* Reference counting */
  if(0 == ext_os_l1_tt.ref_count)
  {
    tzbsp_map_out_ext();
    state = cpsr_saved_state;
    wcp15_scr(scr_saved_state);
  }

  tzbsp_int_restore(state);
}

/**
 * @copydoc tzbsp_map_out_ext_os_fiq
 */
void tzbsp_map_out_ext_os_fiq(uint32 ttbr0)
{
   wcp15_ttbr0_safe(ttbr0);
}

/**
 * @copydoc tzbsp_vbar_qsee
 */
uint32 tzbsp_vbar_qsee(void)
{
  uint32 state;
  uint32 vbar;
  state = tzbsp_int_disable_all();
  vbar  = rcp15_vbar();
  wcp15_vbar_safe(ext_os_l1_tt.tzbsp_l1_tt.vbar);
  tzbsp_int_restore(state);
  return vbar;
}

/**
 * @copydoc tzbsp_vbar_restore
 */
void tzbsp_vbar_restore(uint32 vbar)
{
  uint32 state;
  state = tzbsp_int_disable_all();
  wcp15_vbar_safe(vbar);
  tzbsp_int_restore(state);
}

/* The following functions wrap calls to TZBSP from the external OS, but ensure
 * that the VBAR is correctly set for QSEE to enable demand paging. Without this
 * wrapping it is not possible to page in memory to TZBSP for functions that are
 * not currently mapped. */

/* Interrupt Group */
uint32 tzbsp_int_disable_all_ext_os(void)
{
  uint32 vbar;
  uint32 rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_int_disable_all();
  tzbsp_vbar_restore(vbar);
  return rval;
}

void tzbsp_int_restore_ext_os(uint32 flags)
{
  uint32 vbar;
  vbar = tzbsp_vbar_qsee();
  tzbsp_int_restore(flags);
  tzbsp_vbar_restore(vbar);
}

/* Core Group */
int tzbsp_is_sw_fuse_blown_secure_ext_os(uint32   fuse_num,
                                         boolean* is_blown,
                                         uint32   is_blown_sz)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tz_is_sw_fuse_blown_secure(fuse_num, is_blown, is_blown_sz);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_blow_sw_fuse_ext_os(uint32 fuse_num)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tz_blow_sw_fuse(fuse_num);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_fuse_write_ext_os(uint32  raw_row_address,
                            uint32 *row_data,
                            uint32  bus_clk_khz,
                            uint32* qfprom_api_status)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_fuse_read_ext_os(uint32    row_address,
                           int32     addr_type,
                           uint32*   row_data,
                           uint32*   qfprom_api_status)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_fuse_read(row_address, addr_type,
                         row_data,    qfprom_api_status);
  tzbsp_vbar_restore(vbar);
  return rval;

}

int tzbsp_jtag_override_ext_os(uint32 debug)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_jtag_override(debug);
  tzbsp_vbar_restore(vbar);
  return rval;
}

boolean tzbsp_is_ns_range_ext_os(const void* start, uint32 len)
{
  uint32  vbar;
  boolean rval;
  vbar  = tzbsp_vbar_qsee();
  rval  = (!tzbsp_mpu_amt_is_overlap_amt((uint32) start, len) &&
           tzbsp_is_ns_area(start, (const void*)((uint32)start + len - 1)));
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_mmss_hdmi_status_read_ext_os(uint32* hdmi_enable,
                                       uint32* hdmi_sense,
                                       uint32* hdcp_auth)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_mmss_hdmi_status_read(hdmi_enable, hdmi_sense, hdcp_auth);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_i2c_open_ext_os(tzbsp_i2cpd_device_id_t device_id)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_i2c_open(device_id);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_i2c_read_ext_os(tzbsp_i2cpd_device_id_t        device_id,
                          const tzbsp_i2c_config_t*      p_config,
                          tzbsp_i2c_transaction_info_t*  p_read_info)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_i2c_read(device_id, p_config, p_read_info);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_i2c_write_ext_os(tzbsp_i2cpd_device_id_t       device_id,
                           const tzbsp_i2c_config_t*     p_config,
                           tzbsp_i2c_transaction_info_t* p_write_info)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_i2c_write(device_id, p_config, p_write_info);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_i2c_close_ext_os(tzbsp_i2cpd_device_id_t device_id)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_i2c_close(device_id);
  tzbsp_vbar_restore(vbar);
  return rval;
}

/* MMU Group */
void tzbsp_tlb_inval_all_ext_os(void)
{
  uint32 vbar;
  vbar = tzbsp_vbar_qsee();
  tzbsp_tlb_inval_all();
  tzbsp_vbar_restore(vbar);
}

void tzbsp_cache_flush_ext_os(void)
{
  uint32 vbar;
  vbar = tzbsp_vbar_qsee();
  tzbsp_cache_flush();
  tzbsp_vbar_restore(vbar);
}

void tzbsp_dcache_flush_all_ext_os(void)
{
  uint32 vbar;
  vbar = tzbsp_vbar_qsee();
  tzbsp_dcache_flush_all();
  tzbsp_vbar_restore(vbar);
}

int tzbsp_mem_usage_count_ext_os(uint32 usage)
{

  return -E_NOT_SUPPORTED;
}

int tzbsp_sd_get_session_ext_os(void)
{

  return -E_NOT_SUPPORTED;
}

int tzbsp_tag_mem_ext_os(uint32 tag, uintptr_t start, uintptr_t end)
{
 
  return -E_NOT_SUPPORTED;
}

int tzbsp_itlb_inval_all_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_dtlb_inval_all_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int  tzbsp_dcache_inval_all_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_dcache_clean_all_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_dcache_inval_region_ext_os(void *addr, unsigned int length)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_dcache_flush_region_ext_os(void *addr, unsigned int length)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_dcache_clean_region_ext_os(void *addr, unsigned int length)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_tlb_inval_all_is_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_tlb_inval_by_asid_is_ext_os(uint32 asid)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_tlb_inval_by_mva_is_ext_os(uint32 mva)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_bp_invalidate_all_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_icache_inv_ext_os(void)
{
  return -E_NOT_SUPPORTED;
}

/* Secure Channel Group */
int tzbsp_secure_message_ext_os(tz_sc_ss_e_type        ssid,
                                tz_sc_cid_e_type       cid,
                                const uint8*           input_msg_ptr,
                                uint32                 input_msg_len,
                                uint8*                 output_msg_ptr,
                                uint32*                output_msg_len_ptr)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tz_secure_message(ssid,           cid,
                              input_msg_ptr,  input_msg_len,
                              output_msg_ptr, output_msg_len_ptr);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_authenticate_decrypt_message_ext_os(
                                tz_sc_ss_e_type        ssid,
                                tz_sc_cid_e_type       cid,
                                const uint8*           input_msg_ptr,
                                uint32                 input_msg_len,
                                uint8*                 output_msg_ptr,
                                uint32*                output_msg_len_ptr)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tz_authenticate_decrypt_message(ssid,           cid,
                                            input_msg_ptr,  input_msg_len,
                                            output_msg_ptr, output_msg_len_ptr);
  tzbsp_vbar_restore(vbar);
  return rval;
}

/* Crypto Group */
int tzbsp_hash_ext_os(TZBSP_HASH_ALGO_ET alg,
                      const uint8       *msg,
                      uint32             msg_len,
                      uint8             *digest,
                      uint32             digest_len)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_hash_init_ext_os(TZBSP_HASH_ALGO_ET   alg,
                           tzbsp_hash_ctx     **hash_ctx)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_hash_update_ext_os(const tzbsp_hash_ctx  *hash_ctx,
                             const uint8           *msg,
                             uint32                 msg_len)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_hash_final_ext_os(const tzbsp_hash_ctx  *hash_ctx,
                            uint8                 *digest,
                            uint32                 digest_len)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_hash_free_ctx_ext_os(tzbsp_hash_ctx *hash_ctx)
{
  return -E_NOT_SUPPORTED;
}

int tzbsp_cipher_init_ext_os(TZBSP_CIPHER_ALGO_ET   alg,
                             tzbsp_cipher_ctx     **cipher_ctx)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_init(alg, cipher_ctx);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_cipher_free_ctx_ext_os(tzbsp_cipher_ctx *cipher_ctx)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_free_ctx(cipher_ctx);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_cipher_set_param_ext_os(tzbsp_cipher_ctx     *cipher_ctx,
                                  TZBSP_CIPHER_PARAM_ET param_id,
                                  const void           *param,
                                  uint32                param_len)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_set_param(cipher_ctx, param_id, param, param_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_cipher_get_param_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                  TZBSP_CIPHER_PARAM_ET   param_id,
                                  void                   *param,
                                  uint32                 *param_len)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_get_param(cipher_ctx, param_id, param, param_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_cipher_encrypt_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                const uint8            *pt,
                                uint32                  pt_len,
                                uint8                  *ct,
                                uint32                 *ct_len)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_encrypt(cipher_ctx, pt, pt_len, ct, ct_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_cipher_decrypt_ext_os(const tzbsp_cipher_ctx *cipher_ctx,
                                const uint8            *ct,
                                uint32                  ct_len,
                                uint8                  *pt,
                                uint32                 *pt_len)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_cipher_decrypt(cipher_ctx, ct, ct_len, pt, pt_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}

int tzbsp_hmac_ext_os(TZBSP_HMAC_ALGO_ET alg,
                      const uint8       *msg,
                      uint32             msg_len,
                      const uint8       *key,
                      uint16             key_len,
                      uint8             *msg_digest)
{
  uint32 vbar;
  int    rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_hmac(alg, msg, msg_len, key, key_len, msg_digest);
  tzbsp_vbar_restore(vbar);
  return rval;
}

uint32 tzbsp_prng_getdata_ext_os(uint8 *out, uint32 out_len)
{
  uint32 vbar;
  uint32 rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_prng_getdata(out, out_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}

boolean tzbsp_crypto_lock_engine_ext_os(boolean lock)
{
  uint32  vbar;
  boolean rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_crypto_lock_engine(lock);
  tzbsp_vbar_restore(vbar);
  return rval;
}

uint32 tzbsp_kdf_ext_os(const void  *input_key,
                        uint32       input_key_len,
                        const void  *label,
                        uint32       label_len,
                        const void  *context,
                        uint32       context_len,
                        void        *output_key,
                        uint32       output_key_len)
{
  uint32 vbar;
  uint32 rval;
  vbar = tzbsp_vbar_qsee();
  rval = tzbsp_kdf(input_key, input_key_len, label, label_len, context, 
                   context_len, output_key, output_key_len);
  tzbsp_vbar_restore(vbar);
  return rval;
}
