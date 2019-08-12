/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                           SEC OEM Utilities

GENERAL DESCRIPTION
  Contains OEM utility for selecting SFS options that can be configured
  according to OEM requirements

Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/sfs/custom/src/sfs_oem_utils.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/10   ejt     Added support for sfs_priv_meid_read and sfs_priv_meid_write APIs 
02/26/10   yh      Don't do sfs_flash_dev_init on eMMC build, because flash doesn't exist
11/20/09   yh      Move back to flash API dynamic check after flash team's bug fix.
03/31/09   ejt     featurized include of efuse.h to MSM6xxxx targets
03/20/09   sm      created
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifdef FEATURE_SEC_SFS
#include "sfs_flash_funcs.h"

extern sfs_flash_dev sfs_fdev;

extern "C"
{
#include "sfs_oem_utils.h"
#include "msg.h"
#include "secerrno.h"
#include "assert.h"
#ifndef FEATURE_SEC_EXCLUDE_SFP_10
#ifndef PLATFORM_LTK
#if !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC) &&!defined(T_QSC6695)
#include "efuse.h"
#endif
#endif

/*===========================================================================
*
* FUNCTION: sfs_oem_priv_data_loc_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
sfs_util_selected_priv_data_loc_type sfs_oem_priv_data_loc_get (void )
{
  sfs_util_selected_priv_data_loc_type priv_data_loc_type = E_PRIV_DATA_SFS;

  /* FEATURE_SDCC_BOOT is eMMC build feature, no flash */
#if !defined(FEATURE_SDCC_BOOT) && !defined(PLATFORM_LTK)
  sfs_flash_dev *fd;

  fd = &sfs_fdev;
  if(sfs_flash_dev_init(fd) == E_SUCCESS)
  {
    /* If the Secure Partition can be init successfully,use SFP */
    priv_data_loc_type = E_PRIV_DATA_SFP;
  }
#endif /* FEATURE_SDCC_BOOT && PLATFORM_LTK */

  MSG_MED("Private Data Location Type is: %d ",priv_data_loc_type,0,0);
  return priv_data_loc_type;
}

/*===========================================================================
*
* FUNCTION: sfs_oem_allow_imei_write ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
sfs_util_imei_write_control_type sfs_oem_allow_imei_write(void)
{
	sfs_util_imei_write_control_type imei_write_control = E_IMEI_MULTIPLE_WRITE;

#ifndef PLATFORM_LTK
#if !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC) &&!defined(T_QSC6695)

    /* For MSM6xxx(except SCMM) targets, check if the E-FUSE is blown */
    uint32 enable_bit = 0;
    efuse_err_enum_type efuse_result = EFUSE_NO_ERR;

    efuse_result = efuse_get_item(TRUSTED_BOOT, &enable_bit);
    if ( EFUSE_NO_ERR != efuse_result )
    {
      MSG_ERROR("SFS-IMEI write: EFUSE error! %d", efuse_result, 0, 0);
	  }
    else if ( enable_bit )
    {
      MSG_ERROR("SFS-IMEI write: EFUSE blown", 0, 0, 0);    
      imei_write_control = E_IMEI_READONLY;
    }
#endif /* !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC) &&!defined(T_QSC6695)*/
#endif /* PLATFORM_LTK */

	return imei_write_control;
}

#else // defined FEATURE_SEC_EXCLUDE_SFP_10

sfs_util_imei_write_control_type sfs_oem_allow_imei_write(void)
{
  return E_IMEI_READONLY;
}

#endif // FEATURE_SEC_EXCLUDE_SFP_10
} // extern "C"
#endif //FEATURE_SEC_SFS

