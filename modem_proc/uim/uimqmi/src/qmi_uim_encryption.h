/*================================================================================

DESCRIPTION


Copyright (c) 2015 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_encryption.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/20/15    at     Initial version
=============================================================================*/

#ifndef QMI_UIM_ENCRYPTION_H
#define QMI_UIM_ENCRYPTION_H

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"


/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION  QMI_UIM_ENCRYPT_PAYLOAD()

  DESCRIPTION
    Creates an encrypted message from the input plain text message. When
    this function returns successfully, the caller is responsbile for freeing
    the returned encrypted pointer.

  PARAMETERS
    input_data            : Pointer to input plain text message
    encrypted_data_ptr    : Pointer to output encrypted message

  RETURN VALUE
    boolean          : TRUE if successful in creating an encrypted message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_encrypt_payload
(
  mmgsdi_data_type            input_data,
  mmgsdi_data_type          * encrypted_data_ptr
);

/*===========================================================================
  FUNCTION  QMI_UIM_DECRYPT_PAYLOAD()

  DESCRIPTION
    Creates an decrypted message from the input message buffer. Caller is
    responsbile for passing output buffer large enough to hold the decrypted
    buffer. Recommended output buffer size is atleast in_data_len.

  PARAMETERS
    in_data_ptr            : Pointer to input encrypted message
    in_data_len            : Length of input encrypted message
    out_data_ptr           : Pointer to output plaintext message
    out_data_len_ptr       : Pointer to length of output plaintext message

  RETURN VALUE
    boolean          : TRUE if successful in decrypting the message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_decrypt_payload
(
  const uint8               * in_data_ptr,
  uint32                      in_data_len,
  uint8                     * out_data_ptr,
  uint32                    * out_data_len_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_GENERATE_HMAC()

  DESCRIPTION
    Generated the HMAC out of the passed input buffer. in the PIN encryption
    case, input buffer consists of IV (16 bytes) + encrypted PIN (16 bytes)
    
  PARAMETERS
    in_data_len            : input data length whose HMAC has to be found
    in_data_ptr            : input data pointer whose HMAC has to be found
    out_hmac_data_len_ptr  : pointer to length of HMAC data to be generated
    out_hmac_data_ptr      : pointer to HMAC data to be generated

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_generate_hmac
(
  uint8             in_data_len,
  const uint8     * in_data_ptr,
  uint16          * out_hmac_data_len_ptr,
  uint8           * out_hmac_data_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_PIN()

  DESCRIPTION
    Decrypts the input PIN buffer. Result of the security API is also unpacked
    appropriately. First byte of the raw decrypted data is length of the
    original un-encrypted PIN, next is PIN data, rest of them are padded bytes.

  PARAMETERS
    in_pin_length       : input pin length of the encrypted PIN
    in_pin_value_ptr    : pointer to input encrypted pin buffer
    out_pin_length_ptr  : pointer to length of decrypted pin buffer
    out_pin_value_ptr   : pointer to store decrypted pin data

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_decrypt_pin
(
  uint8                   in_pin_length,
  const uint8           * in_pin_value_ptr,
  uint8                 * out_pin_length_ptr,
  uint8                 * out_pin_value_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_ENCRYPT_PIN()

  DESCRIPTION
    Encrypts the input PIN buffer after packing it appropriately. Construction
    of the final encrypted TLV consists of the following parts & steps:
    Final TLV structure:[16-byte IV][16-byte encrypted PIN][32-byte HMAC]
    1. The plaintext PIN needs to be packed & padded into a 16-byte
    block of buffer (limitation in security API to pass a min input buffer
    of 16 bytes). First byte is length of the original un-encrypted PIN,
    next is PIN data, rest of the bytes padded with trailing zeroes.
    2. Fetch the IV (initialiation vector) & copy to TLV (16 bytes)
    3. Encrypt the plaintext PIN data & append to the TLV (16 bytes)
    4. Create the HMAC & append to the TLV (32 bytes)
    Note that the encrypted TLV can only have a max length of 255 bytes.

  PARAMETERS
    in_pin_length        : input pin length
    in_pin_value_ptr     : pointer to input pin buffer
    out_pin_length_ptr   : pointer to length of encrypted pin buffer
    out_pin_value_ptr    : pointer to encrypted pin buffer

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_encrypt_pin
(
  uint8                   in_pin_length,
  const uint8           * in_pin_value_ptr,
  uint8                 * out_pin_length_ptr,
  uint8                 * out_pin_value_ptr
);

#endif /* QMI_UIM_ENCRYPTION_H */

