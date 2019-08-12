#ifndef SECBOOT_HW_I_H
#define SECBOOT_HW_I_H
/*****************************************************************************
*
* @file  secboot_hw_i.h (Secboot Hardware internal API)
*
* @brief API to read Security Control Fuses containing authentication
*        information
*       
* Copyright (c) 2010-2013 by QUALCOMM, Technology Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secboot/hw/secctrl/msm8937/src/secboot_hw_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/13   hw      Init version 

============================================================================*/

/**
 * @brief This function returns the root selection index value. It reads the fuse when
 *        being called for the first time and stores the index into the global variable.
 *        For the subsequent calls, it returns the stored value.
 * 
 * @param[in,out]  oem_root_cert_sel_ptr    Pointer to a uint32 which will be
 *                                          populated with the ROOT_CERT_PK_HASH_INDEX
 *                                          fuse value. The uint32 is allocated by
 *                                          the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_root_cert_sel
(
  uint32* oem_root_cert_sel_ptr
);


/// @}
//
/// @}
#endif //SECBOOT_HW_I_H
