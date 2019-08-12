#ifndef TZBSP_XPU_H
#define TZBSP_XPU_H

/*===========================================================================
Copyright (c) 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_xpu.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/12   sg      Add APIs to add/remove VMIDs permissions from an xPU
09/14/10   tk      First version.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "tz_syscall_pub.h"
#include <HALxpu2.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TZBSP_XPU_ENABLE          0x01 /**< Enables an XPU. */
#define TZBSP_XPU_NO_INTERRUPTS   0x02 /**< Disables XPU error interrupts. */
#define TZBSP_XPU_MODEM_PROT      0x04 /**< Marks modem only items. */
#define TZBSP_XPU_SEC             0x08 /**< Secure partition */
#define TZBSP_XPU_NON_SEC         0x10 /**< Non Secure partition */

#define TZBSP_RWGE                0x0100 /**< Read/write global enable. */
#define TZBSP_ROGE                0x0200 /**< Read-only global enable. */
#define TZBSP_RWE                 0x0400 /**< Read/write VMID enable. */
#define TZBSP_ROE                 0x0800 /**< Read-only VMID enable. */
#define TZBSP_MSA_ROE             0x1000 /**< MSA Read-only enable for secure partitions. */
#define TZBSP_MSA_RWE             0x2000 /**< MSA Read-write enable for secure partitions. */
#define TZBSP_NON_SEC_RWE         0x4000 /**< Read-write VMID enable on secure RG. */

#define TZBSP_XPU_READ_ACCESS     1 /**< Read only access */
#define TZBSP_XPU_RW_ACCESS       2 /**< Read/Write access */
#define TZBSP_XPU_WRITE_ACCESS    3 /**< Write only access */

#define TZBSP_XPU_DYN_AREA_CERT   1 /**< Dynamic area ID for PIL certificate. */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Resource group configuration for MPUs. All MPUs are multi-VMID.
 */
typedef struct tzbsp_mpu_rg_s
{
  uint16 index;      /* Index of the MPU resource group. */
  /** XPU status bits, currently \c TZBSP_XPU_ENABLE, \c
   * TZBSP_XPU_NO_INTERRUPTS, \c TZBSP_XPU_SEC, \c TZBSP_XPU_NON_SEC and
   * \c TZBSP_XPU_MODEM_PROT are supported.
   */
  uint16 flags;
  uint32 read_vmid;  /* VMIDs able to read this partition. */
  uint32 write_vmid; /* VMIDs able to write this partition. */
  uint32 start;  /* Start of the partition. */
  uint32 end;    /* End of the partition, not included in the range */
} tzbsp_mpu_rg_t;

/**
 * Resource group configuration for APUs/RPUs. Depending on APU/RPU, a resource
 * group is single VMID or multi VMID.
 */
typedef struct tzbsp_rpu_rg_s
{
  /** Index of the APU/RPU resource group. */
  uint16 index;
  /**
   * Valid for single VMID resource groups only. Bits \c TZBSP_RWGE and \c
   * TZBSP_ROGE allow defining read/write global enable and read-only global
   * enable. Bit \c TZBSP_RWE enables the read/write access VMID(s). Bit \c
   * TZBSP_ROE enables the read access VMID(s).
   */
  uint16 flags;
  /**
   * For multi VMID resource groups contains a bitmap of VMIDs that can read
   * the resource. For single VMID resource groups contains a VMID value that
   * can read the resource.
   */
  uint32 read_vmid;
  /**
   * For multi VMID resource groups contains a bitmap of VMIDs that can
   * read/write the resource. For single VMID resource groups contains a VMID
   * value that can read/write the resource.
   */
  uint32 write_vmid;
} tzbsp_rpu_rg_t;

/**
 * Root level structure for XPU configuration.
 */
typedef struct
{
  /** Index of the XPU, @see \c HAL_xpu_XPUType. */
  uint16 id;
  /** XPU status bits, currently \c TZBSP_XPU_ENABLE, \c
   * TZBSP_XPU_NO_INTERRUPTS and \c TZBSP_XPU_MODEM_PROT are supported.
   */
  uint16 flags;
  /** Read access VMIDs for unmapped area */
  uint32 unmapped_rvmid;
  /** Write access VMIDs for unmapped area */
  uint32 unmapped_wvmid;
  /** Number of configured resource groups. */
  uint16 nrg;
  union
  {
    const void* any;           /* A dummy to keep compiler happy. */
    const tzbsp_rpu_rg_t* rpu; /* Can be NULL. */
    const tzbsp_mpu_rg_t* mpu; /* Can be NULL. */
  } rg;
} tzbsp_xpu_cfg_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Lock a resource for a specific bus master.
 *
 * @param[in] resource    The resource to lock
 * @param[in] id          The id of the bus master. This get translated into
 *                        the corresponding vmid (or multi-vmid)
 *
 * @return 0 on success
 *         1 if the lock is already held by the master
 *         2 if the lock is held by another master
 *         negative if an error occured
 *
 * @note Currently on one bus master can own a resource. TZ always
 * implicitly has access since the config port is always programmable.
 */
int tzbsp_lock_resource(TZ_XPU_RESOURCE_ET resource, uint32 id);

/* Unlock a resource for a specific bus master.
 *
 * @param[in] resource The resource to lock
 * @param[in] id          The id of the bus master. This get translated into
 *                        the corresponding vmid (or multi-vmid)
 *
 * @return 0 on success, negative if an error occured
 */
int tzbsp_unlock_resource(TZ_XPU_RESOURCE_ET resource, uint32 id);

/**
 * Converts 16-bit VMID read/write bitmasks to 32-bit access bitmask for XPU
 * HAL API, which uses 2-bits per VMID for read/write access bits.
 *
 * @param read_vmids  VMID bitmap for read access.
 *
 * @param write_vmids VMID bitmap for write access.
 *
 * @return The expanded access bitmap, 2-bits per VMID.
 */
uint32 tzbsp_vmids_to_perm(uint16 read_vmids, uint16 write_vmids);

/**
 * Configures M2VMT mapping tables.
 */
void tzbsp_m2vmt_config(void);

/**
 * Applies static XPU configuration. The static configuration is always assumed
 * to be present and not dynamically changed (e.g. by PIL).
 */
int tzbsp_xpu_static_config(void);

/**
 * Restores the xPU configuration for a power collapsible block
 *
 * @param [in] device device for which sec config has to be restored
 */
int tzbsp_xpu_restore_config(tz_device_e_type device);

/**
 * Locks an EBI1 memory region and enables it as a secure area in the
 * blacklist.
 *
 * @param [in] id  The area to be locked. Must be one of the blacklist
 *                 item IDs. Currently \c TZBSP_BLIST_MSS and
 *                 \c TZBSP_BLIST_LPASS are supported.
 * @param [in] start  Start address of the region to be locked.
 * @param [in] end    End address of the region to be locked.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_xpu_lock_area(uint32 id, uint32 start, uint32 end,
                               uint32 rvmid, uint32 wvmid);

/**
 * Unlocks an EBI1 memory region and disables it as a secure area in the
 * blacklist.
 *
 * @param [in] id  The area to be unlocked. Must be one of the blacklist
 *                 item IDs. Currently \c TZBSP_BLIST_MSS and
 *                 \c TZBSP_BLIST_LPASS are supported.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_xpu_unlock_area(uint32 id);

/**
 * Dumps an XPU's error syndrome registers to log.
 *
 * @param [in] xpu_id  ID of the errored XPU.
 * @param [in] type    Secure/Non Secure Error
 */
void tzbsp_xpu_error_log(HAL_xpu2_XPU2Type xpu, HAL_xpu2_ConfigType type);

/**
 * Dynamically reconfigures an RPU/APU resource group VMID assignment. If the
 * configuration is for secure partition, then setting \c TZBSP_ROGE in the RPU
 * flags will enable \c VMIDCLROE and setting \c TZBSP_RWGE in the RPU flags
 * will enable \c VMIDCLRWE.
 *
 * @param [in] xpu_id   HAL ID of the RPU/APU.
 * @param [in] rpu      The RPU to reconfigure
 * @param [in] sec      If \c TRUE, then partition is configured as secure
 *                      partition. If \c FALSE, then partition is configured
 *                      as VMID based partition.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_rpu_reconfigure(uint32 xpu_id, const tzbsp_rpu_rg_t *rpu,
                          boolean sec);

/**
 * Determines if an XPU is single or multi VMID.
 *
 * @param [in] xpu_id   HAL ID of the RPU/APU.
 *
 * @return TRUE if the XPU is multi VMID, FALSE if the XPU is single VMID.
 */
boolean tzbsp_xpu_is_multi_vmid(uint32 xpu_id);

/**
 * XPU summary interrupt service routine. An XPU client or configuration error
 * has occured. This is an ISR, executed directly in the interrupt context,
 * hence it cannot access non-intlocked TZ state.
 */
void* tzbsp_xpu_isr(void* ctx);

/**
 * @brief Check if unlock xpu is allowed for modem subsystem restart
 * param[in] none
 * param[out] none
 */
boolean tzbsp_allow_unlock_xpu(void);

/**
 * Add VMIDs (read/write) to any RG
 *
 * @param [in] xpu        XPU Device ID
 * @param [in] index      Resource group index
 * @param [in] rvmid      Bitmask  of read access VMIDs to be added. 0 if no
 *                        change needed in rvmid
 * @param [in] wvmid      Bitmask of write access VMIDs to be added. 0 if no
 *                        change needed in wvmid
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_xpu_add_vmid_perms(HAL_xpu2_XPU2Type xpu, uint32 index, uint32 rvmid,
                             uint32 wvmid);

/**
 * Remove VMIDs (read/write) to any RG
 *
 * @param [in] xpu        XPU Device ID
 * @param [in] index      Resource group index
 * @param [in] rvmid      Bitmask  of read access VMIDs to be removed. 0 if no
 *                        change needed in rvmid
 * @param [in] wvmid      Bitmask of write access VMIDs to be removed. 0 if no
 *                        change need in wvmid
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_xpu_remove_vmid_perms(HAL_xpu2_XPU2Type xpu, uint32 index,
                                uint32 rvmid, uint32 wvmid);

/**
 * Locks a dynamic XPU partition.
 *
 * @param [in] area_id  Dynamic area id, one of TZBSP_XPU_DYN_AREA_*.
 * @param [in] start    Start address of the dynamic area. Alignment is
 *                      dependent on the DDR memory MPU configuration at HW
 *                      level.
 * @param [in] end      End address of the dynamic area, exclusive of the
 *                      memory range.  Alignment is dependent on the DDR memory
 *                      MPU configuration at HW level.
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_mpu_lock_dyn_area(uint32 area_id, uint32 start, uint32 end);

/**
 * Unlocks a dynamic XPU partition.
 *
 * @param [in] area_id  Dynamic area id, one of TZBSP_XPU_DYN_AREA_*.
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_mpu_unlock_dyn_area(uint32 area_id);

/**
 * Applies XPU configs applicable post boot milestone
 */
int tzbsp_xpu_config_post_milestone(uint32 mode);

/**
 * Syscall API to enable/disable xpu violations becoming error fatals
 * This can also be used to check if xpu violations are currently error fatals
 * or not by sending \c TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter
 * TZ may choose to force enable/disable xpu violations as error fatals by
 * default. It may also choose to disallow HLOS from overriding this
 * configuration. So, HLOS must always check the output parameter (enabled)
 * to see the current behavior and the return code to see if it was allowed to
 * carry out the operation (-E_NOT_SUPPORTED is returned if HLOS isn't allowed
 * to change the behavior).
 *
 * @param [in] config     Enable/Disable/Noop
 * @param [in] spare      Unused
 *
 * @param[out] status     Enable/Disable
 *
 * @param [in] buf_size_sz   Size of the response buffer.
 *
 * @return  Zero on success.
 *          Negative error code otherwise.
 */
int tzbsp_xpu_config_violation_err_fatal(
                                  tz_xpu_violation_err_fatal_t  config,
                                  uint32                           spare,
                                  tz_xpu_violation_err_fatal_t* status,
                                  uint32                           buf_size_sz);
#endif /* TZBSP_XPU_H */
