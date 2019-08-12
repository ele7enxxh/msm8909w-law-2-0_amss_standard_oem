#ifndef _QM_UTIL_H
#define _QM_UTIL_H

/*===========================================================================

                         QM_UTIL.H

DESCRIPTION

  QMI_MMODE header file for utility functions module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Created file
===========================================================================*/

#include "comdef.h"
#include "cm.h"
#include "qm_comdef.h"
#include "qmi_nas_internal.h"

/*---------------------------------------------------------------------------
  data types
---------------------------------------------------------------------------*/

//===========================================================================
// QMI NAS UTILS BCD16 Conversion to UINT16 error
//===========================================================================

#define  QM_UTIL_INVALID_BCD16  ( (uint16) 0xFFFF )   // at least one of the nibbles of a BCD16 is invalid ( 9 < n < 0xF or == 0xF in some cases).

/*===========================================================================
  FUNCTION QM_UTIL_IS_IN_SERVICE()

  DESCRIPTION
    This function is used to check if a given RAT is in service.
    
  PARAMETERS
    subscription: subscription on which RAT to be checked in service
    rat: RAT which is to be checked for service
    p_info: pointer to the latest CM SS EVENT
   consider_limited_as_service: boolean value to indicate if LIMITED service on RAT to be considered as full service

  RETURN VALUE
    TRUE if the given RAT is in service, FALSE otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 boolean qm_util_is_in_service( enum qm_subs_e subscription, enum qm_rat_e rat, cm_mm_msim_ss_info_s_type *p_info, boolean consider_limited_as_service, enum qm_stack_index_e *stack );

/*===========================================================================
  FUNCTION QM_UTIL_BCD16_TO_UINT16()

  DESCRIPTION
    Convert a BCD encoded 2/3 digit MCC/MNC stored in a uint16 to binary uint16 value.

  PARAMETERS
    bcd16     : binary value to convert
    is3digits : pointer to boolean, TRUE if 3 digits, FALSE if 2.
                A NULL may be passed if not interested.  Ex. for mcc.

  RETURN VALUE
    uint16 - The binary value of the 2 or 3 digit BCD number, or
             0xFFFF if the bcd16 number was invalid for some reason.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

  NOTES
     o Values stored in nibble 0x[0-9 F]
     o A 0xF in the LSN indicates the number is 2-digit instead of 3-digit.
     o A 0x0 should always stored in the MSN of the uint16.
     o Invalid if the LSN is in the inclusive range [ 0xA .. 0xE ], or
                  either of the middle two nibbles are > 0x9.

    |MSN|        LSN  Example: 3 digit mcc/mnc
    | 3 | 2 | 1 | 0 | Nibble
    |0x0|100| 10| 1 | Digit magnitude (nibble 3 should be zero, but not enforced).

    |MSN|        LSN  Example: 2 digit mnc
    | 3 | 2 | 1 | 0 | Nibble
    |0x0| 10| 1 | F | Digit magnitude (nibble 3 should be zero, but not enforced).

===========================================================================*/
uint16 qm_util_bcd16_to_uint16( uint16 bcd16, boolean * is3digits );

/*===========================================================================
  FUNCTION QM_UTIL_MAP_RAT_ENUM_TO_BITMASK()

  DESCRIPTION
    This function maps rat enum to rat bitmask

  PARAMETERS
    rat: enum to convert to bitmask

  RETURN VALUE
    uint32: bitmask value for RAT.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qm_util_map_rat_enum_to_bitmask( enum qm_rat_e rat );

/*===========================================================================
  FUNCTION QM_UTIL_MAP_CM_SYS_MODE_TO_QM_BITMASK()

  DESCRIPTION
    This function maps cm sys_mode to qm rat bitmask

  PARAMETERS
    sys_mode: cm sys_mode to convert

  RETURN VALUE
    uint32: bitmask value for RAT.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qm_util_map_cm_sys_mode_to_qm_bitmask( sys_sys_mode_e_type sys_mode );

/*===========================================================================
  FUNCTION QM_UTIL_MAP_CM_ASUBS_TO_QM_SUBS()

  DESCRIPTION
    This function maps cm asubs id to QM subs enum

  PARAMETERS
    asubs_id: cm asubs id to map to QM

  RETURN VALUE
    enum q_subs_e: enum for QM subscription.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_util_map_cm_asubs_to_qm_subs( sys_modem_as_id_e_type asubs_id );

// Function only needed until all client information moves to nas_clnt module
/*===========================================================================
  FUNCTION QM_UTIL_MAP_NAS_SUBS_TO_QM_SUBS()

  DESCRIPTION
    This function maps QMI NAS subs enum to QM subs enum

  PARAMETERS
    subs_id: QMI_NAS subs enum to map to QM

  RETURN VALUE
    enum q_subs_e: enum for QM subscription.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_util_map_nas_subs_to_qm_subs( enum qmi_nas_subs_e subs_id );


/*===========================================================================
FUNCTION qm_util_report_dog

DESCRIPTION
  This function reports to dog

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void qm_util_report_dog
(
  void
);

/*===========================================================================
FUNCTION  QM_UTIL_DECODE_UCS2_80_81_82_ENCODING

  DESCRIPTION
  Extracts the UCS2 80, 81, or 82 encoded buffer and decodes into UCS2 little
  endian.

DEPENDENCIES
  None.

RETURN VALUE
  size_t len: Length of the output buffer, number of uint16 characters.
    
SIDE EFFECTS
  None
==============================================================================*/
size_t qm_util_decode_ucs2_80_81_82_encoding( uint8 *buf_in, 
                                             size_t buf_in_len,
                                             uint16 *buf_out, 
                                             size_t buf_out_max_len);


/*===========================================================================
  FUNCTION QM_UTIL_READ_EFS()

  DESCRIPTION
    This function retrieves the data from EFS item pointed to by the item_file_path

  PARAMETERS
    item_file_path: EFS item path
    efs_info_ptr :  pointer to EFS data buffer
    efs_info_size : Size of the EFS item buffer

  RETURN VALUE
    TRUE if EFS item is fetched successfully. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_util_read_efs
(
  const char      *item_file_path,
  void            *efs_info_ptr,
  uint32          efs_info_size
);


/*===========================================================================
  FUNCTION QM_UTIL_PRINT_BYTES()

  DESCRIPTION
    This function prints out a byte string of a specified length

  PARAMETERS
    data_ptr: pointer to byte string to print
    data_len: length of byte string to print

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_util_print_bytes
(
   uint8  *data_ptr,
   uint32  data_len
);

/*===========================================================================
  FUNCTION QM_UTIL_RETRIEVE_CSG_ID()

  DESCRIPTION
    This function is used to retrieve the CSG ID for a given stack.
    
  PARAMETERS
    p_info: pointer to the latest CM SS EVENT
    stack_index: stack to retrieve CSG ID for

  RETURN VALUE
    CSG ID if 3GPP stack, SYS_CSG_ID_INVALID if not
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sys_csg_id_type qm_util_retrieve_csg_id( cm_mm_msim_ss_info_s_type *p_info, uint8 stack_index );


/*=============================================================================
FUNCTION: qm_util_gsm7_to_utf16

DESCRIPTION:
   Converts an unpacked 7-bit GSM string to an ISO 8859-1 string.

PARAMETERS:
  gsm7_string       Unpacked GSM 7-bit string.
  gsm7_len          Number of bytes in gsm7_string.
  use_extension     If TRUE, use Basic Character Set Extension of 3GPP TS 23.038.
                    If FALSE, only return characters from the Basic Character Set.
  utf16_string      Pointer to buffer into which to convert to UTF-16
  utf16_len         Number of characters (*not* bytes) in buffer pointed to by
                    utf16_string.
  num_converted     Pointer to storage for number of characters that were
                    written to utf16_string (not counting the terminating NUL).

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
=============================================================================*/
uint16 qm_util_gsm7_to_utf16(
  const byte  *gsm7_string,     /* unpacked gsm 7-bit string */
  size_t      gsm7_len,         /* number of bytes in gsm7_string */
  boolean     use_extension,    /* should the GSM 7-bit extension table be used? */
  uint16      *utf16_string,    /* pointer to where to put converted string */
  size_t      utf16_len,        /* size of buffer pointed to by utf16_string */
  size_t      *num_converted_ptr/* pointer in which to store number of
                                   characters written to utf16_string */
  );
/*=============================================================================*/
void qmi_voice_mem_fatal
(
  void
);
/*=============================================================================*/
void qmi_voice_cmd_fatal
(
  void
);
/*=============================================================================*/
void qmi_voice_mem_error
(
  void
);

#ifdef FEATURE_DUAL_SIM
void qmi_voice_asid
(  qmi_voice_cm_if_info_item_s *const params_ptr
);
#endif
/*=============================================================================*/
void qmi_nas_invalid_len_tlv
(
  void
);
/*=============================================================================*/
void qmi_err_write
(
  uint8 resp_type,
  void* value
);  
#endif // !_QM_UTIL_H
