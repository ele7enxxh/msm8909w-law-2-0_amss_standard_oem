#ifndef TZ_SECURECHANNEL_H
#define TZ_SECURECHANNEL_H

/**
 * @file tz_securechannel.h
 * @brief Secure channel between TZ and Peripheral Sub Systems
 *
 * These APIs are used by TZ applications via TZOS APIs to secure outgoing messages 
 * and authenticate incoming messages. In addition a few APIs are exposed as syscalls 
 * that will invoked by HLOS
 */

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox contains all descriptions that are
      displayed in the output PDF generated using Doxygen and LaTeX. To edit
      or update any of the file/group text in the PDF, edit the
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2011-2013 by Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tz_securechannel.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/19/15   wt       Removed AOSTLM id
07/24/14   wt       Added AOSTLM and Modem QMI client IDs
05/08/13   leo      (Tech Pubs) Edited/added Doxygen comments and markup.
12/12/12   sm       updated tz_secure_message and tz_authenticate_decrypt_message
                    to support subsystem ID and client ID
10/09/11   jay      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** @ingroup tz_datatypes
    Subsystem IDs that can establish a secure channel.
*/
typedef enum
{
  TZ_SC_TZ=0,                  /**< Secure execution environment.         */ 
  TZ_SC_MPSS,                  /**< Modem subsystem.                      */
  TZ_SC_LPASS,                 /**< Low power audio subsystem.            */
  TZ_SC_WCNSS,                 /**< Wireless conectivity subsystem.       */
  TZ_SC_COUNT,                 /**< Number of supported subsystems for a
                                       secure channel.                     */
  TZ_SC_UNSUPPORTED_SS=0x7F    /**< Subsystem not supported.               */
}tz_sc_ss_e_type;

/** @ingroup tz_datatypes
    Client IDs that can establish a secure channel.
*/
typedef enum
{
  TZ_SC_CID_SSM=0,             /**< Secure execution environment.          */ 
  TZ_SC_CID_UIM,               /**< Modem subsystem.                       */
  TZ_SC_CID_DATA_QMI,          /**< Qmi client inside modem                */  
  TZ_SC_CID_COUNT,             /**< Number of supported subsystems for a
                                       secure channel.                     */
  TZ_SC_UNSUPPORTED_CID=0x7F   /**< Subsystem not supported.               */
}tz_sc_cid_e_type;

/** @cond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** 
  @brief 
  This is used to secure the the input message
   
  @details
  The output buffer should be large enough to hold the encrypted message 
  and some internal headers and possible padding. Recommended output buffer size 
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.
   
  @param[in] ssid                    Destination Subsystem ID
  @Param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the secure blob 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the secure blob by the function.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
int tz_secure_message
(
  tz_sc_ss_e_type        ssid,
  tz_sc_cid_e_type       cid,
  const uint8*           input_msg_ptr,
  uint32                 input_msg_len,
  uint8*                 output_msg_ptr,
  uint32*                output_msg_len_ptr
);

/** 
  @brief  
  This is used to authenticate and decrypt the secure blob
   
  @details 
  The output buffer should be large enough to hold the decrypted message 
  Recommended output buffer size is atleast input_msg_len. 
  The memory has to be managed by the caller.
   
  @param[in] ssid                    Subsystem ID
  @Param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the secure blob.
  @param[in] input_msg_len           Length of the secure blob in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the decrypted data 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the decrypted data on return.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
int tz_authenticate_decrypt_message
(
  tz_sc_ss_e_type        ssid,
  tz_sc_cid_e_type       cid,
  const uint8*           input_msg_ptr,
  uint32                 input_msg_len,
  uint8*                 output_msg_ptr,
  uint32*                output_msg_len_ptr
);

/** @endcond */

#endif /* TZ_SECURECHANNEL_H */
