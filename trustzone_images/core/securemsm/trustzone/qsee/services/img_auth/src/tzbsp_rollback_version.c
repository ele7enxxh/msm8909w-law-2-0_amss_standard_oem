/**
* @file tzbsp_rollback_version.c
* @brief TZBSP protect TZ application version rollback
*   TZBSP pil image anti-rollback helper function
*
* This file implements the TZBSP version rollback protection 
* TZ doesn't allow an older version TZ application being loaded
* 
* This file implements pil image anti-rollback helper functions
* It can get pil image version from fuse and blow fuse with updated pil image version
*/
/*===========================================================================
   Copyright (c) 2012 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/src/tzbsp_rollback_version.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when          who      what, where, why
--------      ---      ------------------------------------
04/24/2014    hw      suport anti-rollback update for sbl1-loaded image
02/26/2013    yh      Blow RPMB Provision Fuse only for production key
12/14/2012    yh      8974 PIL image anti-rollback
03/14/2012    yh      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <HALhwio.h>
#include "tzbsp.h"         /* errno, log */
#include "qfprom.h"
#include "tzbsp_rollback_version.h"
#include "tzbsp_hwio.h"
#include "tzbsp_secboot.h"
#include "tzbsp_peripheral_info.h"
#include "tzbsp_mem.h"
#include "tz_syscall_pub.h"
#include "stringl.h"
#include "sd_mgr_rpmb.h"
#include "tzbsp_kdf.h"
#include "tzbsp_cipher.h"
#include "tzbsp_chipset.h"
#include "tzbsp_rollback_version_fuseprov.h"

/* pilsubsys anti rollback fuse bit value*/
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PILSUBSYS_ANTI_ROLLBACK_EN_BMSK                           0x00000400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PILSUBSYS_ANTI_ROLLBACK_EN_SHFT                                  0xa



/* SFPB clock defines for Qfprom Driver in khz.
   Clock runs at 64Mhz in sbl3 for 8960. This clock is used
   in Qfprom driver to compute correct timer value. boot_target.h
*/
#define BOOT_QFPROM_SFPB_CLOCK  (0x40 * 1000)
#define QFPROM_PILSUBSYS_MAX_VERSION         24

/* tzapp anti rollback fuse bit value*/
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_TZAPPS_ANTI_ROLLBACK_EN_BMSK                              0x00000200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_TZAPPS_ANTI_ROLLBACK_EN_SHFT                                     0x9


#define  MAX_TZAPP_VERSION_RECORD   40
#define  TZAPP_RPMB_TABLE_VERSION   1

/* 12 bytes*/
typedef struct
{
  uint8  tz_app_id[TZBSP_APP_ID_LEN];
  uint32 sw_version;
}tzapp_version_record_t;

/* 4*3+20 + 12*40 = 512 bytes */
typedef struct tzapp_version_data
{
  uint32  version;
  uint32  record_num;
  uint32  magic_num;
  uint32  qsee_ks_version;
  uint8   reserved[16];
  tzapp_version_record_t        tzapp_version_record[MAX_TZAPP_VERSION_RECORD];
}tzapp_version_data_t;

extern boolean oem_allow_rpmb_key_provision(void);
extern uint8                   global_tz_app_id[TZBSP_APP_ID_LEN];
static tzapp_version_data_t    tzapp_data;

const uint8 debug_hw_key[TZBSP_AES256_KEY_SIZE]= {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 
                                                  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}; 

extern boolean tzbsp_is_debug_enabled(void);

//#define TZBSP_RPMB_DEBUG

#define RB_DEBUG_PIL_FUSE                        1
#define RB_DEBUG_TZAPP_FUSE                      2
#define RB_DEBUG_PIL_FUSE_VALUE                  3
#define RB_DEBUG_PIL_IMAGE_VERSION               4
#define RB_DEBUG_PIL_FUSE_WRITE                  5
#define RB_DEBUG_RPMB_FUSE                       6
#define RB_DEBUG_BLOW_RPMB_FUSE                  7
#define RB_DEBUG_JTAG_FUSE                       8
#define RB_DEBUG_SECBOOT_FUSE                    9
#define RB_DEBUG_HW_KEY_FUSE                   0xa
#define RB_DEBUG_OVERRIDE_FUSE                 0xb
#define RB_DEBUG_PROV_RPMB_KEY                 0xc
#define RB_DEBUG_PROV_RPMB_KEY_NOT_ALLOWED     0xd
#define RB_DEBUG_RPMB_PROD_KEY                 0xe
#define RB_DEBUG_RPMB_TEST_KEY                 0xf
#define RB_DEBUG_READ_TZ_VER_BAD_APP_ID       0x10
#define RB_DEBUG_READ_TZ_VER_OPEN_PAR_FAIL    0x11
#define RB_DEBUG_READ_TZ_VER_READ_SECTOR      0x12
#define RB_DEBUG_READ_TZ_VER_WRITE_SECTOR     0x13
#define RB_DEBUG_READ_TZ_VER_BAD_PAR_DATA     0x14
#define RB_DEBUG_READ_TZ_VER_RECORD_NOT_FOUND 0x15
#define RB_DEBUG_WRITE_TZ_VER_BAD_APP_ID      0x16
#define RB_DEBUG_WRITE_TZ_VER_OPEN_PAR_FAIL   0x17
#define RB_DEBUG_WRITE_TZ_VER_READ_SECTOR     0x18
#define RB_DEBUG_WRITE_TZ_VER_BAD_PAR_DATA    0x19
#define RB_DEBUG_WRITE_TZ_VER_WRITE_SECTOR    0x1a
#define RB_DEBUG_WRITE_TZ_VER_RECORD_NOT_FOUND 0x1b
#define RB_DEBUG_GET_VERSION_SW_TYPE           0x1c
#define RB_DEBUG_GET_VERSION                   0x1d
#define RB_DEBUG_SET_VERSION_SW_TYPE           0x1e
#define RB_DEBUG_SET_VERSION_SW_VER            0x1f
#define RB_DEBUG_SET_VERSION                   0x20
#define RB_DEBUG_PIL_FUSE_ADDR                 0x21
#define RB_DEBUG_SECBOOT_FUSE_ADDR             0x22

#ifdef  TZBSP_RPMB_DEBUG 
#define TZBSP_RPMB_LOG_DBG(xx_fmt, ...)             \
  do {                                                                  \
    tzbsp_log(TZBSP_MSG_DEBUG, xx_fmt, ##__VA_ARGS__);                  \
  } while (0)
#else                                               
#define TZBSP_RPMB_LOG_DBG(xx_fmt, ...)
#endif

#define TZ_RPMB_PARTITION_MAGIC_NUM 0x4C425450 


/*=========================================================================== 
 
                                    GLOBALS
 
===========================================================================*/

extern boolean sd_mgr_no_access_control;

/*===========================================================================

                              FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

**  Function : tzbsp_rollback_count_set_bits
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function Counts the number of bits set in the input value.
*   
* @param[in] input_val:  Input value whose set bits need to be 
*                        counted. 
* 
* @par Dependencies
*    None
* 
* @retval
*    count: Number of bits set to 1.
* 
* @par Side Effects
*    None
* 
*/
static uint32 tzbsp_rollback_count_set_bits(uint32 input_val)
{
  uint32 count = 0;
  for(; input_val; count++)
  {
    input_val &= input_val - 1 ;
  }
  return count;
}

/*===========================================================================

**  Function : tzbsp_is_pilsubsys_anti_rollback_enabled 
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function checks whether PIL images anti rollback version feature
*    is enabled by checking the PILSUBSYS_ANTI_ROLLBACK_EN bit status.
*  
*  QFPROM_RAW_OEM_CONFIG_ROW1_LSB
*  15:8 ANTI_ROLLBACK_FEATURE_EN
*
*  Bit 0 - BOOT_ANTI_ROLLBACK_EN
*  Bit 1 - TZAPPS_ANTI_ROLLBACK_EN
*  Bit 2 - PILSUBSYS_ANTI_ROLLBACK_EN
*  Bit 3 - MSA_ANTI_ROLLBACK_EN
*    
*    
* @param[in] none
* 
* @par Dependencies
*    None
* 
* @retval
*    TRUE :   PIL images anti rollback version feature enabled.
*             [PILSUBSYS_ANTI_ROLLBACK_EN fuse blown]
*    FALSE:  otherwise.
* 
* @par Side Effects
*    None
* 
*/
static boolean  tzbsp_is_pilsubsys_anti_rollback_enabled( void )
{
  static boolean retval = FALSE;
  static boolean fuse_checked = FALSE;
  uint32 fuse_value = 0;

  if(FALSE == fuse_checked)
  { 
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_value =  HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB, 0x00200000);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_checked = TRUE;
    if(fuse_value)
    {
      retval = TRUE;
    }
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE, retval);

  return retval;
}

/*===========================================================================

**  Function : tzbsp_is_tzapps_anti_rollback_enabled 
 
** ==========================================================================
*/
/* see description of tzbsp_is_pilsubsys_anti_rollback_enabled */
static boolean  tzbsp_is_tzapps_anti_rollback_enabled( void )
{
  static boolean retval = FALSE;
  static boolean fuse_checked = FALSE;
  uint32 fuse_value = 0;
 
  if(FALSE == fuse_checked)
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_value = HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB,  0x00100000);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_checked = TRUE;
    if(fuse_value)
    {
      retval = TRUE;
    }
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_TZAPP_FUSE, retval);

  return retval;
}

/*===========================================================================

**  Function : tzbsp_rollback_read_pilsubsys_fuse_version 
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function get the version stored in fuse for PIL images.
*    eFuse configuration for PIL image version:
*    --------------------
*    Non-8092 hardware
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB  0xFC4B80BC(8626)
*    31:0 PIL_SUBSYSTEM_31_0 
*    This field determines the least significant 32 bits for the lowest
*    version of the subsystem images (apart from Modem Subsystem) loaded by Boot or TZ.
*    QFPROM_RAW_SPARE_REG19_LSB            0xFC4B81F8(8626)
*    15:0 PIL_SUBSYSTEM_47_32 
*    This field determines the most significant 16 bits for the lowest
*    version of the subsystem images (apart from Modem Subsystem) loaded by Boot or TZ.
*  
*/
static uint32 tzbsp_rollback_read_pilsubsys_fuse_version( void )
{ 
  uint32 fuse_value = 0;

  if(tzbsp_is_pilsubsys_anti_rollback_enabled())
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    /* Failsafe version value spans accross LSB, MSB 32 bits of Qfprom Fuse. */
    fuse_value = tzbsp_rollback_count_set_bits(HWIO_INM(QFPROM_CORR_ANTI_ROLLBACK_1_LSB, 0xFC000000));
    fuse_value += tzbsp_rollback_count_set_bits(HWIO_INM(QFPROM_CORR_ANTI_ROLLBACK_1_MSB, 0x3FFFF));

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);
  }
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_VALUE, fuse_value);

  return fuse_value;
}

/*===========================================================================

**  Function : tzbsp_rollback_blow_pilsubsys_version 
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function store the version vlaue back to fuse for PIL images.
*    eFuse configuration for PIL image version:
*    --------------------
*    Non-8092 hardware
*    QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB  0xFC4B80BC(8626)
*    31:0 PIL_SUBSYSTEM_31_0 
*    This field determines the least significant 32 bits for the lowest
*    version of the subsystem images (apart from Modem Subsystem) loaded by Boot or TZ.
*    QFPROM_RAW_SPARE_REG19_LSB            0xFC4B81F8(8626)
*    15:0 PIL_SUBSYSTEM_47_32 
*    This field determines the most significant 16 bits for the lowest
*    version of the subsystem images (apart from Modem Subsystem) loaded by Boot or TZ.
* 
* 
*/
static int tzbsp_rollback_blow_pilsubsys_version(uint32 image_version)
{
  uint32 fs_version = 0;
  uint32 bus_clk_khz = BOOT_QFPROM_SFPB_CLOCK ;
  uint32 fuse_rollbck_ver_row_addr = 0;
  uint32 qfprom_service_status = QFPROM_NO_ERR; 
  uint32 fuse_value[2];

  if(!tzbsp_is_pilsubsys_anti_rollback_enabled())
  {
    return E_SUCCESS;
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_IMAGE_VERSION, image_version);

  if(image_version < 6 )
  {
    fs_version = ((2 << ( image_version - 1))- 1) << 26;
  }
  else
  {
    fs_version = 0xFC000000;
  }

  /* get LSB address on purpose, because write can only start from LSB address 
	  register name are same for 8x10 and 8626 */
  fuse_rollbck_ver_row_addr = HWIO_ADDR(QFPROM_CORR_ANTI_ROLLBACK_1_LSB);

  (void)tzbsp_fuse_read(fuse_rollbck_ver_row_addr,
                        QFPROM_ADDR_SPACE_CORR, 
                        fuse_value,
                        &qfprom_service_status);
  if(QFPROM_NO_ERR != qfprom_service_status)
  {
    TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_ADDR, fuse_rollbck_ver_row_addr);
    return -E_FAILURE;
  }

  fuse_value[0]|= fs_version;
  
  fuse_rollbck_ver_row_addr = HWIO_ADDR(QFPROM_RAW_ANTI_ROLLBACK_1_LSB);
  /* Blow lowest version value to rollback qfprom fuse */  
  (void)tzbsp_fuse_write(fuse_rollbck_ver_row_addr,
                        fuse_value, 
                        bus_clk_khz,
                        &qfprom_service_status);
  if(QFPROM_NO_ERR != qfprom_service_status)
  {
    TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_WRITE, fuse_rollbck_ver_row_addr);
    TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_WRITE, fs_version);
    return -E_FAILURE;
  }

  /* write higher version value*/
  if(image_version > 6)
  {
    image_version -= 6;
  }

  else
    return E_SUCCESS;

  if(image_version)
  {
    if (image_version >= 18)
	{
	  fs_version = 0x0003FFFF;
	}
    else
	{
      fs_version = (2 << ( image_version - 1))- 1;
	}
    /* Blow lowest version value to rollback qfprom fuse , always start from LSB */
    fuse_rollbck_ver_row_addr = HWIO_ADDR(QFPROM_CORR_ANTI_ROLLBACK_1_LSB);

    (void)tzbsp_fuse_read(fuse_rollbck_ver_row_addr,
                        QFPROM_ADDR_SPACE_CORR, 
                        fuse_value,
                        &qfprom_service_status);
    if(QFPROM_NO_ERR != qfprom_service_status)
    {
       TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_ADDR, fuse_rollbck_ver_row_addr);
       return -E_FAILURE;
    }

    fuse_value[1] |= fs_version;

    fuse_rollbck_ver_row_addr = HWIO_ADDR(QFPROM_RAW_ANTI_ROLLBACK_1_LSB);
	(void)tzbsp_fuse_write(fuse_rollbck_ver_row_addr,
                           fuse_value, 
                           bus_clk_khz,
                           &qfprom_service_status);
    if(QFPROM_NO_ERR != qfprom_service_status)
    {
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_WRITE, fuse_rollbck_ver_row_addr);
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_WRITE, fs_version);
      return -E_FAILURE;
    }
  }

  return E_SUCCESS;
}

/*===========================================================================

**  Function : tzbsp_is_rpmb_key_provisioned_fuse_blown 
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function checks whether RPMB key is already provisioned
*    by checking the RPMB_KEY_PROVISIONED bit status.
*  
*  QFPROM_RAW_SPARE_REG19_MSB
*  Bit 26 - RPMB_KEY_PROVISIONED Indicates that the RPMB Key has been already provisioned.
*      
*  QFPROM_RAW_SPARE_REG19_MSB  bits 26     0x04000000
* @param[in] none
* 
* @par Dependencies
*    None
* 
* @retval
*    TRUE :    RPMB key is provisioned.
*             [RPMB_KEY_PROVISIONED fuse blown]
*    FALSE:  otherwise.
* 
* @par Side Effects
*    None
* 
*/
boolean  tzbsp_is_rpmb_key_provisioned_fuse_blown( void )
{
  static boolean retval = FALSE;
  static boolean fuse_checked = FALSE;
  uint32 fuse_value = 0;

  if(FALSE == fuse_checked)
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_value = HWIO_INM(QFPROM_CORR_ANTI_ROLLBACK_1_LSB,0x00000001);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);

    fuse_checked = TRUE;
    if(fuse_value)
    {
      retval = TRUE;
    }
  }
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_RPMB_FUSE, retval);
  return retval;
}

/*===========================================================================

**  Function : tzbsp_blow_rpmb_key_provisioned_fuse 
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function blow RPMB_KEY_PROVISIONED fuse.
*  
*  QFPROM_RAW_SPARE_REG19_MSB
*  Bit 26 - RPMB_KEY_PROVISIONED Indicates that the RPMB Key has been already provisioned.
*      
* @param[in] none
* 
* @par Dependencies
*    None
* 
* @retval
*    E_SUCESS on success, negative on failure
* 
* @par Side Effects
*    None
* 
*/
int tzbsp_blow_rpmb_key_provisioned_fuse( void )
{
  uint32 fuse_row_addr = 0;
  uint32 fuse_value[2];
  uint32 bus_clk_khz = BOOT_QFPROM_SFPB_CLOCK ;
  uint32 qfprom_service_status = QFPROM_NO_ERR;

  if (FALSE == tzbsp_is_rpmb_key_provisioned_fuse_blown()) 
  {
    /* Read in LSB address, becuase MSB can't be the write address */
    fuse_row_addr = HWIO_ADDR(QFPROM_CORR_ANTI_ROLLBACK_1_LSB);

    (void)tzbsp_fuse_read(fuse_row_addr,
                        QFPROM_ADDR_SPACE_CORR,
                        fuse_value,
                        &qfprom_service_status);

    if(QFPROM_NO_ERR != qfprom_service_status)
    {
      TZBSP_LOG(TZBSP_MSG_ERROR,"RB {%x %x}", RB_DEBUG_PIL_FUSE_ADDR, fuse_row_addr);
      TZBSP_LOG(TZBSP_MSG_ERROR,"RB {%x %x}", fuse_value[0], fuse_value[1]);
      return -E_FAILURE;
    }
    TZBSP_LOG(TZBSP_MSG_ERROR,"fuse_value {%x %x}", fuse_value[0], fuse_value[1]);

    fuse_value[0] |= 0x00000001;

    fuse_row_addr = HWIO_ADDR(QFPROM_RAW_ANTI_ROLLBACK_1_LSB);
    (void)tzbsp_fuse_write(fuse_row_addr,
                           fuse_value,
                           bus_clk_khz,
                           &qfprom_service_status);
    if(QFPROM_NO_ERR != qfprom_service_status)
    {
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PIL_FUSE_WRITE, fuse_row_addr);
      TZBSP_LOG(TZBSP_MSG_ERROR,"RB {%x %x}", fuse_value[0], fuse_value[1]);
      return -E_FAILURE;
    }
  }
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_BLOW_RPMB_FUSE, E_SUCCESS);
  return E_SUCCESS;
}


/* ***************** 8974 hardware key logic ************************
   8974 Security HDD

 
   Table 15-6 Secure State Definition
 ------------------------------------------------------------------------------------------------------------------------
   Secure State |   JTAG Debug Fuse (AND)                                 |               Secure Boot Fuse
 ------------------------------------------------------------------------------------------------------------------------
   Apps_secure  |fuse_apps_spiden_disable | fuse_qc_apps_spiden_disable   |apps_secure_boot_auth_en_blown_seg1 |
                |---------------------------------------------------------|(|(apps_secure_boot_auth_en_blown_seg4to28)).
                |fuse_apps_spniden_disable| fuse_qc_apps_spniden_disable  |
                |---------------------------------------------------------|
                |fuse_dap_spiden_disable | fuse_qc_dap_spiden_disable     |Any auth_en of secure boot regions 1, 4-28
                |---------------------------------------------------------|determines Apps secure boot
                |fuse_dap_spniden_disable | fuse_qc_dap_spniden_disable   |
 ------------------------------------------------------------------------------------------------------------------------
                                                                               BMSK value
 fuse_apps_spiden_disable        QFPROM_RAW_OEM_CONFIG_ROW0_MSB    bits 2      0x00000004
 fuse_apps_spniden_disable       QFPROM_RAW_OEM_CONFIG_ROW0_MSB    bits 3      0x00000008
 fuse_qc_apps_spiden_disable     QFPROM_RAW_FEAT_CONFIG_ROW1_LSB   bits 15     0x00008000
 fuse_qc_apps_spniden_disable    QFPROM_RAW_FEAT_CONFIG_ROW1_LSB   bits 16     0x00010000
 fuse_dap_spiden_disable         QFPROM_CORR_OEM_CONFIG_ROW0_MSB   bits 6      0x00000040
 fuse_dap_spniden_disable        QFPROM_CORR_OEM_CONFIG_ROW0_MSB   bits 7      0x00000080
 fuse_qc_dap_spiden_disable      QFPROM_CORR_FEAT_CONFIG_ROW1_LSB  bits 19     0x00080000
 fuse_qc_dap_spniden_disable     QFPROM_CORR_FEAT_CONFIG_ROW1_LSB  bits 20     0x00100000
 
 apps_secure_boot_auth_en_blown_seg1   QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(0)  bits5  0x00000020
 apps_secure_boot_auth_en_blown_seg2   QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(0)  bits13 0x00002000
 apps_secure_boot_auth_en_blown_seg3   QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(0)  bits21 0x00200000
 apps_secure_boot_auth_en_blown_seg4   QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(0)  bits29 0x20000000
 apps_secure_boot_auth_en_blown_seg5   QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(1)  bits5  0x00000020
 ...
 apps_secure_boot_auth_en_blown_seg28  QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB(3)  bits29 0x20000000
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Note!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 99% OEMs blow  apps_secure_boot_auth_en_blown_seg1 to control secure boot fuse
 */ 

/*===========================================================================

**  Function : tzbsp_is_jtag_disable_fuse_blown 
 
** ==========================================================================
*/
static boolean tzbsp_is_jtag_disable_fuse_blown( void )
{
  static boolean retval = FALSE;

  static boolean fuse_checked = FALSE;
  uint32 apps_spiden_disable = 0;      
  uint32 qc_apps_spiden_disable = 0;
  uint32 apps_spniden_disable = 0;
  uint32 qc_apps_spniden_disable = 0;
  uint32 dap_spiden_disable = 0;
  uint32 qc_dap_spiden_disable = 0;
  uint32 dap_spniden_disable = 0;
  uint32 qc_dap_spniden_disable = 0;

  if(FALSE == fuse_checked)
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SEC_JTAG);

    apps_spiden_disable = HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB, 0x00001000);
    apps_spniden_disable = HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB,0x00002000);
    qc_apps_spiden_disable = HWIO_INM(QFPROM_CORR_FEAT_CONFIG_ROW1_LSB, 0x00004000);
    qc_apps_spniden_disable = HWIO_INM(QFPROM_CORR_FEAT_CONFIG_ROW1_LSB,0x00008000);
    dap_spiden_disable = HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB, 0x00010000);
    dap_spniden_disable = HWIO_INM(QFPROM_CORR_OEM_CONFIG_ROW0_MSB,0x00020000);
    qc_dap_spiden_disable = HWIO_INM(QFPROM_CORR_FEAT_CONFIG_ROW1_LSB, 0x00040000);
    qc_dap_spniden_disable = HWIO_INM(QFPROM_CORR_FEAT_CONFIG_ROW1_LSB,0x00080000);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_CORR);
    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SEC_JTAG);

    TZBSP_RPMB_LOG_DBG( "Jtag fuse {%x %x %x %x}", apps_spiden_disable,apps_spniden_disable,qc_apps_spiden_disable,qc_apps_spniden_disable);
    TZBSP_RPMB_LOG_DBG( "Jtag fuse {%x %x %x %x}", dap_spiden_disable,dap_spniden_disable,qc_dap_spiden_disable,qc_dap_spniden_disable);

    apps_spiden_disable |= qc_apps_spiden_disable;
    apps_spniden_disable |= qc_apps_spniden_disable;
    dap_spiden_disable |= qc_dap_spiden_disable;
    dap_spniden_disable |= qc_dap_spniden_disable;

    fuse_checked = TRUE;
    if(apps_spiden_disable && apps_spniden_disable && dap_spiden_disable && dap_spniden_disable)
    {
      retval = TRUE;
    }
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_JTAG_FUSE, retval);

  return retval;
}

/*===========================================================================

**  Function : tzbsp_is_secboot_fuse_blown 
 
** ==========================================================================
*/
static boolean  tzbsp_is_secboot_fuse_blown( void )
{
  static boolean retval = FALSE;

  static boolean fuse_checked = FALSE;

  uint32 fuse_oem_sec[2];
  uint32 fuse_row_addr = 0;
  uint32 qfprom_service_status = QFPROM_NO_ERR; 

  if(FALSE == fuse_checked)
  {
    fuse_row_addr = HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR;
    TZBSP_RPMB_LOG_DBG("RB {%x %x}", RB_DEBUG_SECBOOT_FUSE_ADDR, fuse_row_addr);
    (void)tzbsp_fuse_read(fuse_row_addr,
                        QFPROM_ADDR_SPACE_CORR, 
                        fuse_oem_sec,
                        &qfprom_service_status);
    TZBSP_RPMB_LOG_DBG("secboot fuse {%x} {%x}", fuse_oem_sec[0],fuse_oem_sec[1]);

    if ((fuse_oem_sec[0]& 0x20000020) || (fuse_oem_sec[1]& 0x202020))
    {
      retval = TRUE;
    }
    fuse_checked = TRUE;
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_SECBOOT_FUSE, retval);

  return retval;
}

/* HW_KEY_STATUS
   KDF_DONE  bit 4
   This bit indicates that the Key derivation operation has completed. It is recommended for the boot software to
   poll this bit before using the derived keys for
  Cryptographic operations.
  0: KDF operation is progress
  1: KDF operation has completed
 
  SEC_KEY_DERIVATION_KEY_BLOWN bit 1
  This bit indicates if the Secondary Key Derivation Key is provisioned. This is determined as an OR reduction
  of all the secondary key derivation key fuse bits. If any are blow the key is considered blown.
  0: Not Blown
  1: Blown
  10010  = 0x12 */
static boolean tzbsp_is_hw_key_ready(void)
{
  static boolean retval = FALSE;
  static boolean fuse_checked = FALSE;
  uint32 fuse_value = 0;

  if(FALSE == fuse_checked)
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SEC_JTAG);

    fuse_value = HWIO_INM(HW_KEY_STATUS, 0x00000012);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SEC_JTAG);

    fuse_checked = TRUE;
    if(fuse_value)
    {
      retval = TRUE;
    }
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_HW_KEY_FUSE, retval);

  return retval;

}

boolean tzbsp_apps_secure_state( void )
{
  boolean jtag_disable = tzbsp_is_jtag_disable_fuse_blown();
  boolean secboot_fuse = tzbsp_is_secboot_fuse_blown();
  boolean hw_key_ready = tzbsp_is_hw_key_ready();

  return (jtag_disable && secboot_fuse && hw_key_ready);
}

/*
 Table 15-8 Debug States Definition
  --------------------------------------------------------------------------------------------------------------------
   Debug state  |   Override Register Bit (OR)                            |          Sec Boot Reenable
 ---------------------------------------------------------------------------------------------------------------------
   Apps_debug   |ovrid_apps_spiden_disable                                |debug_open
                |---------------------------------------------------------|any re-enable through Certicom ACC
                |ovrid_apps_spniden_disable                               |
                |---------------------------------------------------------|This fuse can only be blown by Qualcomm
                |ovrid_dap_spiden_disable                                 |and there is no software interface 
                |---------------------------------------------------------|to check this fuse. So TZ doesn't check
                |ovrid_dap_spniden_disable                                |the value.
 --------------------------------------------------------------------------------------------------------------------
ovrid_apps_spiden_disable        OVERRIDE_3 bits1
ovrid_apps_spniden_disable       OVERRIDE_3 bits2
ovrid_dap_spiden_disable         OVERRIDE_3 bits3
ovrid_dap_spniden_disable        OVERRIDE_3 bits4 
*/

/*===========================================================================

**  Function : tzbsp_apps_debug_state 
 
** ==========================================================================
*/
boolean  tzbsp_apps_debug_state( void )
{
  static boolean retval = FALSE;

  static boolean fuse_checked = FALSE;
  int    fuse_value = 0;

  if(FALSE == fuse_checked)
  {
    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

    fuse_value = HWIO_INM(OVERRIDE_3, 0x0000001E);

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

    if(fuse_value)
    {
      retval = TRUE;
    }
    fuse_checked = TRUE;
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_OVERRIDE_FUSE, retval);

  return retval;
}



/*===========================================================================

**  Function : tzbsp_provision_rpmb_key 
 
** ==========================================================================
*/

/* *****************RPMB key provision logic************************
------------------------------------------------------------------------------------------------------------------
Sl No|Apps_debug    |Apps_secure     |Key Returned to CG  |Provision Type       |Comments
------------------------------------------------------------------------------------------------------------------
     |              |                |                    |Test       Prod      | 
------------------------------------------------------------------------------------------------------------------
1    |              |Yes             |Derived from SKDK   |Fail(a)       Pass(b)|Consumer, Production field scenario
-----|NO            |----------------------------------------------------------------------------------------------
2    |              |No              |Derived from Debug  |Pass(c)       Fail(d)|Development
------------------------------------------------------------------------------------------------------------------
3    |              |Yes             |Derived from Debug  |Fail(e)       Fail(f)|Factory floor after Fuse blown, before/after provision, Field return debugging
-----|YES           |----------------------------------------------------------------------------------------------
4    |              |No              |Derived from Debug  |Pass(g)       Fail(h)|Factory floor before Fuse blown
------------------------------------------------------------------------------------------------------------------
 */
int tzbsp_provision_rpmb_key(uint32 provision_type)
{
  uint8               tzbsp_rpmb_key[TZBSP_AES256_KEY_SIZE];
  int                 ret = E_SUCCESS;

  if(provision_type > TZBSP_SD_DEVICE_RPMB_PROVISION_TEST)
    return -E_INVALID_ARG;

  if(tzbsp_is_rpmb_key_provisioned_fuse_blown() && 
    FALSE == oem_allow_rpmb_key_provision())
  {
    return -E_ALREADY_DONE;
  }

  do
  {
    /* production key */
    if(FALSE == tzbsp_apps_debug_state())
    {
      if(TRUE == tzbsp_apps_secure_state())
      {
        if(TZBSP_SD_DEVICE_RPMB_PROVISION_PRODUCTION == provision_type)
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY, provision_type);
          ret = tzbsp_get_rpmb_key(tzbsp_rpmb_key); /* b */
        }
        else
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY_NOT_ALLOWED, provision_type);
          ret = -E_NOT_ALLOWED; /* a */
          break;
        }
      }
      else
      {
        if(TZBSP_SD_DEVICE_RPMB_PROVISION_TEST == provision_type)
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY, provision_type);
          ret = tzbsp_get_rpmb_key(tzbsp_rpmb_key); /* c */
        }
        else
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY_NOT_ALLOWED, provision_type);
          ret = -E_NOT_ALLOWED; /* d */
          break;
        }
      }
    }
    else  /* AP debug override == TRUE */ 
    {
      if(TRUE == tzbsp_apps_secure_state())
      {
        TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY_NOT_ALLOWED, provision_type);
        ret = -E_NOT_ALLOWED;  /* e, f */
      }
      else /* dummy key */
      {
        if(TZBSP_SD_DEVICE_RPMB_PROVISION_TEST == provision_type)
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY, provision_type);
          ret = tzbsp_get_rpmb_key(tzbsp_rpmb_key);  /* g */
        }
        else
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_PROV_RPMB_KEY_NOT_ALLOWED, provision_type);
          ret = -E_NOT_ALLOWED;  /* h */
          break;
        }
      }
    }

    if(ret)
    {
      break;
    }

    /* Call PM provsion key function */
    ret = sd_mgr_rpmb_provision_key(tzbsp_rpmb_key, TZBSP_AES256_KEY_SIZE);
    if(ret)
    {
      break;
    }


    /* Only blow fuse for production key*/
    if(E_SUCCESS == ret && TZBSP_SD_DEVICE_RPMB_PROVISION_PRODUCTION == provision_type)
    {
      ret = tzbsp_blow_rpmb_key_provisioned_fuse();
      if(ret)
      {
        break;
      }
    }
  }while(0);

  return ret;
}

static const char label[] = "tzbsp application rpmb version rollback label";
static const uint8 context[TZBSP_AES256_KEY_SIZE] = {0x8b,0x95,0x08,0x4a,0x0c,0xc6,0x41,0x66,0x71,0x47,0xca,0xca,0x20,0xcd,0x7e,0xa5,
                                                    0x7c,0xf5,0x50,0x9f,0x3e,0xeb,0x5d,0x05,0x55,0x8b,0x58,0x7a,0x13,0xa7,0x9b,0x87};

/*===========================================================================

**  Function : tzbsp_get_rpmb_key 
 
** ==========================================================================
*/
int tzbsp_get_rpmb_key(uint8 *tzbsp_rpmb_key)
{
  int ret;
  const uint8 *seed_key = debug_hw_key;

  if(!tzbsp_rpmb_key)
    return -E_INVALID_ARG;

  memset(tzbsp_rpmb_key, 0, TZBSP_AES256_KEY_SIZE);

  if(FALSE == tzbsp_apps_debug_state() && TRUE == tzbsp_apps_secure_state())
  {
    /* production key */
    seed_key = NULL;
    TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_RPMB_PROD_KEY);

  }
  else
  {  
    /* test key */
    TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_RPMB_TEST_KEY);
  }
  ret = tzbsp_kdf((const void  *)seed_key, TZBSP_AES256_KEY_SIZE,
                  (const void  *)label, strlen(label),
                  (const void  *)context,32,
                  (void *)tzbsp_rpmb_key,TZBSP_AES256_KEY_SIZE);

#ifdef  TZBSP_RPMB_DEBUG 
  {
    int i;
    for (i = 0; i<TZBSP_AES256_KEY_SIZE; i+=4)
    {
      TZBSP_RPMB_LOG_DBG(" %x%x%x%x ", tzbsp_rpmb_key[i], tzbsp_rpmb_key[i+1], tzbsp_rpmb_key[i+2], tzbsp_rpmb_key[i+3]);
    }
  }
#endif
  return ret;

}

static  qsee_sd_mgr_device_handle_t   sd_mgr_device_handle = NULL; 
static  qsee_sd_mgr_client_handle_t   sd_mgr_client_handle = NULL;
static  boolean           read_tzapp_partition = FALSE;

int tzbsp_open_tz_rpmb_partition(void)
{
  int ret = E_SUCCESS;

  if(!sd_mgr_device_handle || !sd_mgr_client_handle)
  {
    uint32                   partition_id = 0; 

    if(!sd_mgr_device_handle)
    {
      ret = sd_mgr_device_init(SD_MGR_EMMC_RPMB, NULL, &sd_mgr_device_handle);
      if(SD_MGR_SUCCESS != ret)
      {
        TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
        return ret;
      }
    }

    if(!sd_mgr_client_handle)
    {
      sd_mgr_no_access_control = TRUE;
      ret = sd_mgr_open_partition(&sd_mgr_device_handle, partition_id, &sd_mgr_client_handle);
      sd_mgr_no_access_control = FALSE;
      if(SD_MGR_PARTI_NOT_FOUND_ERROR == ret)
      {
        sd_mgr_no_access_control = TRUE;
        ret = sd_mgr_add_partition(&sd_mgr_device_handle, partition_id, 4); //TZ apps partition is 4 sectors
        
        if(SD_MGR_SUCCESS != ret)
        {
          TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", partition_id);
          TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
          return ret;
        }
        ret = sd_mgr_open_partition(&sd_mgr_device_handle, partition_id, &sd_mgr_client_handle);          
		sd_mgr_no_access_control = FALSE;               
      }
    }
  }

  return ret;

}

/*===========================================================================

**  Function : tzbsp_rollback_read_rpmb_version
 
** ==========================================================================
*/
static int tzbsp_rollback_read_rpmb_version(tzbsp_rb_data_t data_desc,
                                            uint32* version)
{
  int                      ret;
  int                      i;


  *version = 0;

  if (TZ_RB_EXEC == data_desc)
  {
  if( !tzbsp_is_tzapps_anti_rollback_enabled())
  {
      return E_SUCCESS;
  }

  for(i=0; i<TZBSP_APP_ID_LEN; i++ )
  {
    if(global_tz_app_id[i]!=0)
    {
      break;
    }
  }

  if(TZBSP_APP_ID_LEN == i)
  {
    TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_READ_TZ_VER_BAD_APP_ID);
    return -E_FAILURE;;
    }
  }

  if(!read_tzapp_partition)
  {
      ret = tzbsp_open_tz_rpmb_partition();

    if (SD_MGR_SUCCESS != ret)
      {
        TZBSP_RPMB_LOG_DBG("RB {%x %x}",RB_DEBUG_READ_TZ_VER_OPEN_PAR_FAIL, ret);
        TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
      return ret;
      }

      memset((void *)&tzapp_data, 0, sizeof(tzapp_version_data_t));
      sd_mgr_no_access_control = TRUE;
    ret = sd_mgr_read_sectors(&sd_mgr_client_handle, 0, 1,
                              (uint8 *)&tzapp_data);
      sd_mgr_no_access_control = FALSE;
      if(SD_MGR_SUCCESS != ret)
      {
        TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_READ_TZ_VER_READ_SECTOR, ret);
        TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
      return ret;
      }
      read_tzapp_partition = TRUE;

      if( TZ_RPMB_PARTITION_MAGIC_NUM != tzapp_data.magic_num )
      {
      /* If come here, it is first time read TZ RPMB partition, init
         tzapp_data */
        memset((void *)&tzapp_data, 0, sizeof(tzapp_version_data_t));
        tzapp_data.record_num = 0;
        tzapp_data.version = TZAPP_RPMB_TABLE_VERSION;
        tzapp_data.magic_num = TZ_RPMB_PARTITION_MAGIC_NUM;
      tzapp_data.qsee_ks_version = 0;
        tzapp_data.tzapp_version_record[tzapp_data.record_num].sw_version = 0;
      memcpy(tzapp_data.tzapp_version_record[tzapp_data.record_num].tz_app_id,
             global_tz_app_id, TZBSP_APP_ID_LEN);
        tzapp_data.record_num++;

        sd_mgr_no_access_control = TRUE;
      ret = sd_mgr_write_sectors(&sd_mgr_client_handle, 0, 1,
                                 (uint8 *) &tzapp_data);
        sd_mgr_no_access_control = FALSE;
        if(ret)
        {
          TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_READ_TZ_VER_WRITE_SECTOR, ret);
          TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
        }
      }
  }

  if(tzapp_data.record_num > MAX_TZAPP_VERSION_RECORD)
  {
    TZBSP_RPMB_LOG_DBG("record_num %d, ver=0xFFFFFFFF",
                       tzapp_data.record_num );
    TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_READ_TZ_VER_BAD_PAR_DATA, tzapp_data.record_num);

    return -E_FAILURE;
  }

  if (TZ_RB_KEYS == data_desc)
  {
    *version = tzapp_data.qsee_ks_version;
    return E_SUCCESS;
  }

  for(i=0; i<tzapp_data.record_num; i++)
  {
    /* search for sw_version of global_tz_app_id in rpmb */
    if(0 == memcmp((tzapp_data.tzapp_version_record[i].tz_app_id),
                   global_tz_app_id, TZBSP_APP_ID_LEN))
    {
      *version = tzapp_data.tzapp_version_record[i].sw_version;
      return E_SUCCESS;
    }
  }

  /* It is first time load this TZ application, add it to TZ app table */
  if(tzapp_data.record_num < MAX_TZAPP_VERSION_RECORD)
  {
    tzapp_data.tzapp_version_record[tzapp_data.record_num].sw_version = 0;
    memcpy(tzapp_data.tzapp_version_record[tzapp_data.record_num].tz_app_id,
            global_tz_app_id,TZBSP_APP_ID_LEN);
    tzapp_data.record_num++;

    sd_mgr_no_access_control = TRUE;
    ret = sd_mgr_write_sectors(&sd_mgr_client_handle, 0, 1,
                               (uint8 *)&tzapp_data);
    sd_mgr_no_access_control = FALSE;
    if(ret)
    {
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_READ_TZ_VER_WRITE_SECTOR, ret);
      TZBSP_LOG(TZBSP_MSG_ERROR, "(0x%08x)", *((uint32 *)global_tz_app_id));
      TZBSP_LOG(TZBSP_MSG_ERROR, "(0x%08x)", *((uint32 *)global_tz_app_id + 1));
      TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
    }
  }

  TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_READ_TZ_VER_RECORD_NOT_FOUND);
  return E_SUCCESS;
}

/*===========================================================================

**  Function : tzbsp_rollback_write_rpmb_version
 
** ==========================================================================
*/
static int tzbsp_rollback_write_rpmb_version(tzbsp_rb_data_t data_desc,
                                             uint32 version)
{
  int                      ret = E_SUCCESS;
  int                      i;


  if (TZ_RB_EXEC == data_desc)
  {
  if( !tzbsp_is_tzapps_anti_rollback_enabled())
  {
    return E_SUCCESS;
  }

  for(i=0; i<TZBSP_APP_ID_LEN; i++ )
  {
    if(global_tz_app_id[i]!=0)
    {
      break;
    }
  }
  if(TZBSP_APP_ID_LEN == i)
  {
    TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_WRITE_TZ_VER_BAD_APP_ID);
    return -E_NOT_ALLOWED;
  }
  }

  ret = tzbsp_open_tz_rpmb_partition();
  if(ret)
  {
    TZBSP_RPMB_LOG_DBG("RB {%x %x}",RB_DEBUG_WRITE_TZ_VER_OPEN_PAR_FAIL, ret);
    TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
    return ret;
  }

  if(!read_tzapp_partition)
  {
    memset((void *)&tzapp_data, 0, sizeof(tzapp_version_data_t));
    sd_mgr_no_access_control = TRUE;
    ret = sd_mgr_read_sectors(&sd_mgr_client_handle, 0, 1,
                              (uint8 *)&tzapp_data);
    sd_mgr_no_access_control = FALSE;
    if(SD_MGR_SUCCESS != ret)
    {
      TZBSP_LOG(TZBSP_MSG_ERROR, "read_sectors 0 {%d}", ret);
      TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
      return -E_FAILURE;
    }

    /* this function was called after tzbsp_rollback_read_tzapp_version
       the TZapp partition table must be initialized in previous*/
    if( tzapp_data.magic_num != TZ_RPMB_PARTITION_MAGIC_NUM)
    {
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_WRITE_TZ_VER_BAD_PAR_DATA, tzapp_data.magic_num);
      return -E_NOT_AVAILABLE;
    }

    if(tzapp_data.record_num > MAX_TZAPP_VERSION_RECORD)
    {
      TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_WRITE_TZ_VER_BAD_PAR_DATA, tzapp_data.record_num);
      TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", tzapp_data.record_num);
      return -E_DATA_TOO_LARGE;
    }
  }

  if (TZ_RB_KEYS == data_desc)
  {
    tzapp_data.qsee_ks_version = version;
    sd_mgr_no_access_control = TRUE;
    ret = sd_mgr_write_sectors(&sd_mgr_client_handle, 0, 1,
                               (uint8 *)&tzapp_data);
    sd_mgr_no_access_control = FALSE;
    if (ret)
    {
      TZBSP_RPMB_LOG_DBG("write_sectors {%d}", ret );
      TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
    }

    return ret;
  }

  for(i=0; i<tzapp_data.record_num; i++)
  {
    /* search for sw_version of global_tz_app_id in rpmb */
    if(0 == memcmp((tzapp_data.tzapp_version_record[i].tz_app_id),
                   global_tz_app_id, TZBSP_APP_ID_LEN))
    {
      tzapp_data.tzapp_version_record[i].sw_version = version;
      sd_mgr_no_access_control = TRUE;
      ret = sd_mgr_write_sectors(&sd_mgr_client_handle, 0, 1,
                                 (uint8 *)&tzapp_data);
     sd_mgr_no_access_control = FALSE;
      if(ret)
      {
        TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_WRITE_TZ_VER_WRITE_SECTOR, ret);
        TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);
      }
      break;
    }
  }

  if(i == tzapp_data.record_num )
  {
    TZBSP_RPMB_LOG_DBG( "RB {%x}", RB_DEBUG_WRITE_TZ_VER_RECORD_NOT_FOUND);
    TZBSP_LOG(TZBSP_MSG_ERROR, "(0x%08x)", *((uint32 *)global_tz_app_id));
    TZBSP_LOG(TZBSP_MSG_ERROR, "(0x%08x)", *((uint32 *)global_tz_app_id + 1));
    TZBSP_LOG(TZBSP_MSG_ERROR, "(%d)", ret);

    ret = -E_FAILURE;
  }

  return ret;
}

/*===========================================================================

**  Function : tzbsp_rollback_get_version 
 
** ==========================================================================
*/
int tzbsp_rollback_get_version(tzbsp_rb_data_t data_desc, uint32* version)
{
  int ret = 0;

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_GET_VERSION_SW_TYPE, data_desc);

  switch(data_desc)
  {
    case TZ_RB_WCNSS:
      /* Intentional fallthrough */
    case TZ_RB_ADSP:
      /* Intentional fallthrough */
    case TZ_RB_VIDEO:
    case TZ_RB_VPU:
    case TZ_RB_BCSS:
      *version = tzbsp_rollback_read_pilsubsys_fuse_version();
      ret     = E_SUCCESS;
      break;

    case TZ_RB_EXEC:
      /* Intentional fallthrough */
    case TZ_RB_KEYS:
      ret = tzbsp_rollback_read_rpmb_version(data_desc, version);
      break;

    default:
      ret = -E_NOT_SUPPORTED;
      break;
  }
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_GET_VERSION, ret);
  return ret;
}

/*===========================================================================

**  Function : tzbsp_rollback_set_version 
 
** ==========================================================================
*/
int tzbsp_rollback_set_version(tzbsp_rb_data_t data_desc, uint32 version)
{
  int ret = E_SUCCESS;

  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_SET_VERSION_SW_TYPE, data_desc);
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_SET_VERSION_SW_VER, version);

  switch(data_desc)
    {
    case TZ_RB_SBL:
      /* Intentional fallthrough. */
    case TZ_RB_RPM:
      /* Intentional fallthrough. */
    case TZ_RB_TZ:
      /* Intentional fallthrough. */
    case TZ_RB_APPSBL:
      /* Intentional fallthrough. */
    case TZ_RB_QHEE:
    {
      ret = rollback_update_version_fuse(data_desc, version);
      if (RBV_E_SUCCESS != ret)
      {
          ret = -E_FAILURE;    
      }
      break;
    }
    case TZ_RB_WCNSS:
      /* Intentional fallthrough. */
    case TZ_RB_ADSP:
      /* Intentional fallthrough. */
    case TZ_RB_VIDEO:
    case TZ_RB_VPU:
    case TZ_RB_BCSS:
      ret = tzbsp_rollback_blow_pilsubsys_version(version);
      break;

    case TZ_RB_EXEC:
      /* Intentional fallthrough. */
    case TZ_RB_KEYS:
      ret = tzbsp_rollback_write_rpmb_version(data_desc, version);
      break;

    default:
      ret = -E_NOT_SUPPORTED;
      break;
  }
  TZBSP_RPMB_LOG_DBG( "RB {%x %x}", RB_DEBUG_SET_VERSION, ret);
  return ret;
}
 


