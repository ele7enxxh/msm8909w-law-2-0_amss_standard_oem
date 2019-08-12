/**
@file tzbsp_secapi_chipset.h
@brief Trustzone security api chipset specific definitions/routines

This file contains the target specific information for Trustzone security level api

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_secapi_chipset.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/03/20     hw          Initial version
=============================================================================*/

#ifndef TZBSP_SECAPI_CHIPSET_H
#define TZBSP_SECAPI_CHIPSET_H

#include "comdef.h"
#include "IxErrno.h"
#include "tzbsp_hwio.h"
#include "tzbsp_secapi.h"

#define TZBSP_SECAPI_SECURE_BOOTn_ADDR(index)                      HWIO_SECURE_BOOTn_ADDR(index)
#define TZBSP_SECAPI_PK_HASH_ROWn_ADDR(index)                      HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(index)
#define TZBSP_SECAPI_BIST_CTRL_ADDR                                HWIO_QFPROM_BIST_CTRL_ADDR
/* The bit 8 is the SHK region bit of QFPROM_BIST_ERROR register. It is 1 if SHK fuse is blown. */
#define TZBSP_SECAPI_BIST_ERROR_CHECK_SHK_VALU                     (0x100)
#define TZBSP_SECAPI_OVERRIDE_2_3_ADDR                             HWIO_OVERRIDE_2_ADDR
#define TZBSP_SECAPI_OVERRIDE_4_ADDR                               HWIO_OVERRIDE_4_ADDR
#define TZBSP_SECAPI_RD_WR_PERM_ADDR                               HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR

/* address, lsb, msb, lsb_pattern, msb_pattern */
secctrl_check_fuse_info_t tzbsp_secapi_debug_disabled[] = 
{
  {
  HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR,
  0x0, 0x7F0FE,
  0x0, 0x7F0FE  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};

secctrl_check_fuse_info_t tzbsp_secapi_rd_wr_perm[] = 
{
  {
  HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR,
  0x100, 0xB22,
  0x100, 0xB22  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};

secctrl_check_fuse_info_t tzbsp_secapi_fec_en[] = 
{
  /* oem sec boot */
  {
  HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR,
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};

secctrl_check_fuse_info_t tzbsp_secapi_fec_en_with_pk_hash[] = 
{
  /* oem sec boot */
  {
  HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR,
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  /* oem pk hash */
  {
  TZBSP_SECAPI_PK_HASH_ROWn_ADDR(0),
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  {
  TZBSP_SECAPI_PK_HASH_ROWn_ADDR(1),
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  {
  TZBSP_SECAPI_PK_HASH_ROWn_ADDR(2),
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  {
  TZBSP_SECAPI_PK_HASH_ROWn_ADDR(3),
  0x00000000, 0x80000000,
  0x00000000, 0x80000000  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};

secctrl_check_fuse_info_t tzbsp_secapi_anti_rollback[] = 
{
  {
  HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR,
  0x00000000, 0x00780000,
  0x00000000, 0x00780000  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};

secctrl_check_fuse_info_t tzbsp_secapi_rpmb_provision[] = 
{
  {
  HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR,
  0x00000001, 0x00000000,
  0x00000001, 0x00000000  },
  // terminator
  { 0x0, 0x0, 0x0, 0x0, 0x0 },
};
#endif
