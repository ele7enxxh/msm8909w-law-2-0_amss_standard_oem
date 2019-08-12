/**
@file tzbsp_fuseprov_chipset.h
@brief Trustzone Fuse Provisioning chipset specific definitions/routines

This file contains the target specific information for Trustzone Fuse Provisioning

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_fuseprov_chipset.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2013/12/10     sm          Initial version
=============================================================================*/

#ifndef TZBSP_FUSEPROV_CHIPSET_H
#define TZBSP_FUSEPROV_CHIPSET_H

#include "comdef.h"
#include "IxErrno.h"

/* This are has been carved out from TZ DDR shared region */
#define TZBSP_SECDAT_BUFFER_OFFSET                (0x82000)                   // the offset of sec.dat ddr memory
#define TZBSP_SECDAT_BUFFER_OFFSET_LOC_32BIT      TZBSP_SECDAT_BUFFER_OFFSET  // the offset for 32bit TZ
#define TZBSP_SECDAT_QSEE_OFFSET                  (0x40000)                   // the offset of qsee image to tz ddr start address
#define TZBSP_SECDAT_BUFFER_OFFSET_LOC_64BIT      TZBSP_SECDAT_QSEE_OFFSET+TZBSP_SECDAT_BUFFER_OFFSET  // the offset for 64bit TZ
#define TZBSP_SECDAT_BUFFER_SIZE                  2048     // the size of the shared region

/* qfprom read fuse through lsb address, so RD_WR_PERM_LSB is used here */
#define TZBSP_FUSEPROV_WR_PERM_HWIO_ADDR                          HWIO_ADDR(QFPROM_RAW_RD_WR_PERM_LSB)
#define TZBSP_FUSEPROV_IS_SHK_WR_PERM_BLOWN(qfprom_fuse_data)     ((qfprom_fuse_data[1] & HWIO_FMSK(QFPROM_RAW_RD_WR_PERM_MSB, SEC_KEY_DERIVATION_KEY)) > 0)
#define TZBSP_FUSEPROV_IS_SECBOOT_WR_PERM_BLOWN(qfprom_fuse_data) ((qfprom_fuse_data[1] & HWIO_FMSK(QFPROM_RAW_RD_WR_PERM_MSB, OEM_SEC_BOOT)) > 0)

#define TZBSP_FUSEPROV_RD_PERM_HWIO_ADDR                          HWIO_ADDR(QFPROM_RAW_RD_WR_PERM_LSB)
#define TZBSP_FUSEPROV_IS_SHK_RD_PERM_BLOWN(qfprom_fuse_data)     ((qfprom_fuse_data[0] & HWIO_FMSK(QFPROM_RAW_RD_WR_PERM_LSB, SEC_KEY_DERIVATION_KEY)) > 0)

#define TZBSP_FUSEPROV_SEC_KEY_LSB_MASK                           HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK
#define TZBSP_FUSEPROV_SEC_KEY_MSB_MASK                           HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK
#endif
