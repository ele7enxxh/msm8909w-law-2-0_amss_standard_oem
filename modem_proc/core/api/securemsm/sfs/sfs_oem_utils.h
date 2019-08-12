#ifndef SFS_OEM_UTILS_H
#define SFS_OEM_UTILS_H

/** 
@file sfs_oem_utils.h 
@brief OEM Secure Flash Partition Configuration Options.
This file contains the definitions of the constants, data 
structures, and  interfaces that allow an OEM to configure 
certain options for Secure Flash Partition 1.0. 
 */

/*=============================================================================
              Copyright (c) 2000-2010 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/sfs/sfs_oem_utils.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/10   sm      added doxygen comments
03/20/09   sm      created
=============================================================================*/

/**
@addtogroup SFP
@{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"

/**
Identifies the location to be used to store private data in the
Secure Flash Partition 1.0. 
 */
typedef enum
{
  E_PRIV_DATA_SFS          = 0,   /**< Use Secure File System to store the private data. */
  E_PRIV_DATA_SFP          = 1,   /**< Use Secure Flash Partition to store the private data. */
  E_PRIV_DATA_UNSUPPORTED  = 2    /**< Do not use the SFP or SFS to store the private data. */
}sfs_util_selected_priv_data_loc_type;

/**
Provides the write controls for IMEI (and MEID) in the SFP. 
 */
typedef enum
{
  E_IMEI_READONLY               =  0,    /**< IMEI is read-only and cannot be written to. */
  E_IMEI_ONETIME_WRITE          =  1,    /**< IMEI can be written only if it has not previously been written. */
  E_IMEI_MULTIPLE_WRITE         =  2     /**< IMEI can be written multiple times. */
}sfs_util_imei_write_control_type;

/**
Controls where to store the private data written through the SFP 1.0 API.

Use E_PRIV_DATA_SFP or E_PRIV_DATA_SFS only if the 
target-specific prerequisites for SFS (i.e, Secure Boot and 
blowing of fuses for the MSM device key) are met or if the OEM has 
configured OEM_DEV_KEY. 

@note Using SFS does not prevent data from being deleted if the file system 
is erased.

@return 
E_PRIV_DATA_SFS          - Store data in the Secure File System.\n
E_PRIV_DATA_SFP          - Store data in the Secure Flash Partition. \n
E_PRIV_DATA_UNSUPPORTED  - DO not use the SFP 1.0 API to store the data.
 
@dependencies
None.
 */
sfs_util_selected_priv_data_loc_type sfs_oem_priv_data_loc_get (void );


/**
Controls whether the SFP 1.0 API can write the IMEI, and if so, how many times
it can be written.
 
This function can be customized to meet OEM requirements. For example, the 
function can be made to read a fuse to decide whether the IMEI is read-only. 
This enables the IMEI to be written to in the factory (as long as the fuse is 
not blown), and then once the fuse is blown, to be read-only. 
 
The default implementation for MSM6xxx (except QSC6695 targets) 
is to check the TRUSTED_BOOT fuse. If the fuse is <i>not</i> blown, the 
function returns E_IMEI_MULTIPLE_WRITE; otherwise, it returns 
E_IMEI_READONLY. 
 
For all other targets, the default implementation is to return
E_IMEI_MULTIPLE_WRITE.

@return 
E_IMEI_READONLY          - IMEI cannot be written to. \n
E_IMEI_ONETIME_WRITE     - IMEI can be written ONLY IF it has not
                            already been written. \n
E_IMEI_MULTIPLE_WRITE    - IMEI can be written multiple times.
 
@dependencies
None.
 
@sideeffects
The controls for IMEI are also applicable for MEID, when an MEID is
written.
 */
sfs_util_imei_write_control_type sfs_oem_allow_imei_write(void);

#ifdef __cplusplus
}
#endif
/** @} */  /* end_addtogroup SFP */
#endif /* SFS_OEM_UTILS_H */

