#ifndef TZBSP_VERSION_ROLLBACK_H
#define TZBSP_VERSION_ROLLBACK_H

/**
@file tzbsp_version_rollback.h
@brief Check TZ application version, rollback protection
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/inc/tzbsp_rollback_version.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
3/14/12     yh      Initial version.


 =========================================================================== */

/*=========================================================================== 
 
                           INCLUDE FILES

===========================================================================*/
#include "secboot.h"
#include "tzbsp_secboot.h"


/**
    RPMB key provision types.
 */
typedef enum
{
  TZBSP_SD_DEVICE_RPMB_PROVISION_PRODUCTION,        /**< @RPMB key derived from secure hardware key */
  TZBSP_SD_DEVICE_RPMB_PROVISION_TEST               /**< @RPMB dummy key */
} tzbsp_rpmb_key_provision_e_type;

typedef enum
{
  TZ_RB_SBL    = SECBOOT_SBL_SW_TYPE,
  TZ_RB_RPM    = SECBOOT_RPM_FW_SW_TYPE,
  TZ_RB_TZ     = SECBOOT_TZ_KERNEL_SW_TYPE,
  TZ_RB_APPSBL = SECBOOT_APPSBL_SW_TYPE,
  TZ_RB_QHEE   = SECBOOT_QHEE_SW_TYPE,
  TZ_RB_WCNSS = SECBOOT_WCNSS_SW_TYPE,
  TZ_RB_ADSP  = SECBOOT_ADSP_SW_TYPE,
  TZ_RB_VIDEO = SECBOOT_VIDEO_SW_TYPE,
  TZ_RB_EXEC  = SECBOOT_TZ_EXEC_SW_TYPE,
  TZ_RB_VPU    = SECBOOT_VPU_SW_TYPE,
  TZ_RB_BCSS   = SECBOOT_BCSS_SW_TYPE,
  /* Keys are not part of the secboot sw_type enum. Anything else
     added that's not part of the enum should be added to the function
     tzbsp_convert_sw_type_to_rb_data. */
  TZ_RB_KEYS  = (int) (SECBOOT_INVALID_SW_TYPE - 1),

  TZ_RB_INVAL = SECBOOT_INVALID_SW_TYPE,

  TZ_RB_PAD   = (uint32) 0x7FFFFFFF,
} tzbsp_rb_data_t;



/*===========================================================================

**  Function : tzbsp_rollback_get_version
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function get the image version stored in device
*    For PIL images ( SECBOOT_ADSP_SW_TYPE, SECBOOT_WCNSS_SW_TYPE, SECBOOT_VIDEO_SW_TYPE):
*    This function get the version stored in fuse
*    --------------------
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB
*    31:0 bits:  Number of bits set in this range are concatenated with the 
*                range immediately below to specify the version of the PIL image 
*    QFPROM_RAW_SPARE_REG19_ROW0_LSB
*    15:0 bits   Number of bits set in this range are concatenated with the range
*                immediately above in this fashion 
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB Bit 0-31 | QFPROM_RAW_SPARE_REG19_ROW0_LSB 0-15
*
*    For TZ apps (SECBOOT_TZ_EXEC_SW_TYPE):
*    This function get the version stored in RPMB partition
*  
* @param[in] sw_type   software type, see definition in tzbsp_secboot.h
* 
* @par Dependencies
*   None
*  
* @retval
*    version value or minus value for failure case
*  
* @par Side Effects
*    None
*    
*/
int tzbsp_rollback_get_version(tzbsp_rb_data_t data_desc, uint32* version);

/*===========================================================================

**  Function : tzbsp_rollback_set_version
 
** ==========================================================================
*/
/*!
*  
* @brief
*    This function write the image version to device
*    For PIL images( SECBOOT_ADSP_SW_TYPE, SECBOOT_WCNSS_SW_TYPE, SECBOOT_VIDEO_SW_TYPE):
*    This function blow the version to fuse
*    --------------------
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB
*    31:0 bits:  Number of bits set in this range are concatenated with the 
*                range immediately below to specify the version of the PIL image 
*    QFPROM_RAW_SPARE_REG19_ROW0_LSB
*    15:0 bits   Number of bits set in this range are concatenated with the range
*                    immediately above in this fashion
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB Bit 0-31 | QFPROM_RAW_SPARE_REG19_ROW0_LSB 0-15
*  
*    For TZ apps:
*    This function save the version to RPMB partition
*  
*  
* @param[in] sw_type   software type, see definition in tzbsp_secboot.h
* 
* @par Dependencies
*   None
* 
* @retval
*    E_SUCCESS or minus value for failure case
* 
* @par Side Effects
*    This function will blow the PIL image fuse when sw_type is one of the PIL images
* 
*/
int tzbsp_rollback_set_version(tzbsp_rb_data_t data_desc, uint32 version);


int tzbsp_get_rpmb_key(uint8 *tzbsp_rpmb_key);

int tzbsp_provision_rpmb_key(uint32 provision_type);

int tzbsp_blow_rpmb_key_provisioned_fuse( void );
                                      
/*===========================================================================

**  Function : tzbsp_convert_sw_type_to_rb_data

** ==========================================================================
*/
static inline tzbsp_rb_data_t tzbsp_convert_sw_type_to_rb_data(uint32 sw_type)
{
  tzbsp_rb_data_t secboot_input = (tzbsp_rb_data_t) sw_type;

  /* Right now the keys are the only thing supported by rollback
     protection that are not part of the secboot type. */
  if (TZ_RB_KEYS == secboot_input)
  {
    return TZ_RB_INVAL;
  }

  return secboot_input;
};

#endif /*TZBSP_VERSION_ROLLBACK_H*/
