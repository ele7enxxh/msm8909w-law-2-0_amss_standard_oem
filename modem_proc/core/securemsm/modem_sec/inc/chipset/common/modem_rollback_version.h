#ifndef MODEM_ROLLBACK_VERSION_H
#define MODEM_ROLLBACK_VERSION_H

/**
@file modem_rollback_version.h
@brief Modem Rollback Protection
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/inc/chipset/common/modem_rollback_version.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
01/09/13   rsahay       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Retrieved from FLat File - ARM_ADDRESS_FILE.FLAT*/
#include "msmhwiobase.h"
#include "modem_rollback.h"

/* HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ANTI_ROLLBACK_FEATURE_EN_BMSK reads all 4 rollback bits 
 *  So we define our own mask here that reads only if MSA Rollback is enabled.
 */
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_FEATURE_EN_BMSK       0x00400000 
   
static struct modem_version_rollback_img_set modem_sec_mba_img =
{
    /* image_id */
    MODEM_SEC_MBA_ID,

    /* feature_enable_mask */
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_FEATURE_EN_BMSK,
    
     /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
     /* version_mask*/
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK,
    
     /* version_shift */
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT,    
    
     /* version_lsb_base_addr */
     (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PHYS,
    
     /* version_lsb_base_addr */
     (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,

     /* current_version_num */
     0,
};

static struct modem_version_rollback_img_set modem_sec_modem_img =
{
    /* image_id */
    MODEM_SEC_MODEM_ID,

    /* feature_enable_mask */
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_FEATURE_EN_BMSK,
    
     /* feature_enable_base_addr */
     (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
     /* version_mask*/
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK,
    
     /* version_shift */
     HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT,    
    
     /* version_lsb_base_addr */
     (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PHYS,

     /* version_lsb_base_addr */
     (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,
    
     /* current_version_num */
     0,
};

#endif /* MODEM_ROLLBACK_VERSION_H */
