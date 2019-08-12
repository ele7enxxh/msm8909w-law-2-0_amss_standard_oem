#ifndef SECBOOT_HW_H
#define SECBOOT_HW_H
/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief API to read Security Control Fuses containing authentication
*        information
*       
* Copyright (c) 2010-2013 by Qualcomm, Technologies, Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secboot/api/secboot_hw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/12   hw,mm   Add support for multiple root selection
05/27/10   sm      Created module

============================================================================*/
/** @ingroup SecureMSM
 *  @{
 */

/** @defgroup SecbootHardware Secboot Hardware Library */

/**
 * @addtogroup SecbootHardware
 *
 * @{
 */
#include "comdef.h"

/**
 Identifies the error type returned by the API.
 */
typedef enum
{
  E_SECBOOT_HW_SUCCESS                = 0x0, /**< Operation was successful. */
  E_SECBOOT_HW_FAILURE                = 0x1, /**< General failure. */
  E_SECBOOT_HW_INVALID_PARAM          = 0x2, /**< Parameter passed in was invalid */
  E_SECBOOT_HW_OUT_OF_RANGE           = 0x3, /**< Index out of bounds */
  E_SECBOOT_HW_FEC_ERROR              = 0x4, /**< FEC error status detected */
  E_SECBOOT_HW_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
} secboot_hw_etype;

/**
 Identifies the secure boot fuses which represent the
 authentication information for the code.
 */
#define SECBOOT_HW_APPS_CODE_SEGMENT   1 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the application processors images */     
#define SECBOOT_HW_MBA_CODE_SEGMENT    2 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the Modem Boot Authentication (MBA) image */     
#define SECBOOT_HW_MSS_CODE_SEGMENT    3 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the modem image */     

/* Length of a SHA256 hash */
#define SHA256_HASH_LEN 32

/**
 * @brief Secboot Function table
 */
typedef struct secboot_hw_ftbl_type
{
  secboot_hw_etype (*secboot_hw_is_auth_enabled)
                    (uint32    code_segment,
                     volatile uint32*   auth_enabled_ptr);

  secboot_hw_etype (*secboot_hw_get_root_of_trust)
                    (uint32 code_segment,
                     uint8 root_of_trust[SHA256_HASH_LEN]);

  secboot_hw_etype (*secboot_hw_get_msm_hw_id)
                    (uint32        code_segment,
                     const uint8   root_of_trust[SHA256_HASH_LEN],
                     uint64*       msm_hw_id_ptr);

  secboot_hw_etype (*secboot_hw_get_use_serial_num)
                    (uint32         code_segment,
                     uint32*        auth_use_serial_num_ptr);

  secboot_hw_etype (*secboot_hw_get_serial_num)
                    (uint32*  serial_num_ptr);

  secboot_hw_etype (*secboot_hw_get_root_cert_info)
                    (uint32 code_segment,
		     uint32* root_index_ptr, 
                     uint32* is_root_cert_enabled,
                     uint32* root_cert_total_num);
 
}secboot_hw_ftbl_type;

/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment       Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 * @param[in,out] auth_enabled_ptr   Pointer to a uint32 indicating whether
 *                                   authentication is required. Will be
 *                                   populated to 0 if authentication
 *                                   is not required, 1 if authentication
 *                                   is required.
 *         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must NOT allow execution to continue if a failure is returned.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_is_auth_enabled
(
  uint32    code_segment,
  volatile uint32*   auth_enabled_ptr
);

/**
 * @brief This function returns the hash of the trusted root certificate for
 *        the image. The image's certificate chain must chain back to this
 *        trusted root certificate
 *
 * @param[in]      code_segment       Code segment in SECURE_BOOTn register
 *                                    containing authentication information
 *                                    of the image.
 * @param[in,out]  root_of_trust      32 byte buffer which will be
 *                                    populated with the SHA256 hash of
 *                                    the trusted root certificate.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned. 
 *
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_root_of_trust
(
  uint32 code_segment,
  uint8 root_of_trust[SHA256_HASH_LEN]
);

/**
 * @brief This function returns the msm_hw_id used to authenticate the image's
 *        signature. The 64 bit msm_hw_id is comprised of the 32 bit JTAG ID
 *        (with the tapeout version in the upper 4 bits masked out) + the 32 bit
 *        OEM_ID or SERIAL_NUM value
 *
 * @param[in]      code_segment      Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 *
 * @param[in]      root_of_trust     32 bytes buffer containing the root of
 *                                   trust hash which was populate by
*                                    calling secboot_hw_get_root_of_trust()
 *
 * @param[in,out] msm_hw_id_ptr      Pointer to a uint64 which will
 *                                   be populated with the msm hardware id.
 *                                   The uint64 is allocated by the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned  
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_msm_hw_id
(
  uint32        code_segment,
  const uint8   root_of_trust[SHA256_HASH_LEN],
  uint64*       msm_hw_id_ptr
);

/**
 * @brief This function returns if the serial number is to be used for authentication
 *
 * @param[in]      code_segment             Code segment in SECURE_BOOTn register
 *                                          containing authentication information
 *                                          of the image.
 * @param[in,out]  auth_use_serial_num_ptr  Pointer to a uint32 which will be
 *                                          populated with the USE_SERIAL_NUM
 *                                          fuse value. The uint32 is allocated by
 *                                          the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_use_serial_num
(
  uint32         code_segment,
  uint32*        auth_use_serial_num_ptr
);

/**
 * @brief This function returns the serial number of the chip
 *                         
 * @param[in,out]  serial_num_ptr       Pointer to a uint32 which will
 *                                      be populated with the SERIAL_NUM
 *                                      fuse value. The uint32 is allocated by
 *                                      the caller.
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
secboot_hw_etype secboot_hw_get_serial_num
(
  uint32*  serial_num_ptr
);

/**
 * @brief This function retrieves the index of root cert selection, 
 * checks if root cert selection is enabled and sets the total number
 * of root certs hashed in OEM_PK_HASH. 
 * The index of the root cert is encoded in the following manner:
 * Possible ROOT_CERT_PK_HASH_INDEX values for Root Selection
 * Value	Fuse		Meaning
 * 0	0000 0000	Production device, no certificate selected: Default to certificate 0
 * F0	1111 0000	Production device fixed to certificate 0  (Use for disabling the feature)
 * E1	1110 0001	Device fixed to certificate 1
 * D2	1101 0010	Device fixed to certificate 2
 * C3	1100 0011	Device fixed to certificate 3
 * B4	1011 0100	Device fixed to certificate 4
 * A5	1010 0101	Device fixed to certificate 5
 * 96	1001 0110	Device fixed to certificate 6
 * 87	1000 0111	Device fixed to certificate 7
 * 78	0111 1000	Device fixed to certificate 8
 * 69	0110 1001	Device fixed to certificate 9
 * 5A	0101 1010	Device fixed to certificate 10
 * 4B	0100 1011	Device fixed to certificate 11
 * 3C	0011 1100	Device fixed to certificate 12
 * 2D	0010 1101	Device fixed to certificate 13
 * 1E	0001 1110	Device fixed to certificate 14
 * 0F	0000 1111	Device fixed to certificate 15
 * Others		Invalid value. Disable boot.  
 *  
 *
 * @param[out] root_index_ptr     Pointer to uint32 that will be
 *                                populated with the index. uint32 is
 *                                allocated by the caller
 * @param[out] is_root_cert_enabled   Pointer to uint32 that will be
 *                                indicate if root cert is enabled. uint32 is
 *                                allocated by the caller
 * @param[out] root_cert_total_num Pointer to uint32 that will be
 *                                 populated with total number of root certs.
 *                                 uint32 is allocated by the caller
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_root_cert_info
(
  uint32 code_segment,
  uint32* root_index_ptr, 
  uint32* is_root_cert_enabled,
  uint32* root_cert_total_num
);

/**
 * @brief This function return pointers to the secboot functions linked into
 *        the image
 *
 * @param[in,out] ftbl_ptr              Pointer to the function table structure
 *                                      to populate. The pointer must be allocated
 *                                      by the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
secboot_hw_etype secboot_hw_get_ftbl(secboot_hw_ftbl_type* ftbl_ptr);

/// @}
//
/// @}
#endif //SECBOOT_HW_H
