#ifndef SECDSM_H
#define SECDSM_H

/** 
@file secdsm.h
@brief Cryptographic Data Format - SEC DSM to IOVEC manipulation.
   This file contains the definitions of the constants, data structures, and
   interfaces that provide Cryptographic Data Format. These utilities are used 
   for sec dsm to iovec manipulation. Currently, there is no dynamic allocation for 
   growing or shrinking vectors. This could change in the future. 
*/

/*===========================================================================
    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/common/inc/secdsm.h#1 $ 
  $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/11   ms      Initial Version


===========================================================================*/

#include "comdef.h"
#include "memory.h"
#include "dsm.h"
#include "secapi.h"


/**
  Counts the number of DSM items in the chain.
  
  @param[in] dsm_item  Pointer to the DSM chain.
  
  @return
  The total number of DSM items in chain.

  @dependencies
  None.
*/
uint32 seccom_dsm_item_count(dsm_item_type *dsm_item);

/**
  Counts the number of DSM items in the chain with a specific byte count.
  
  @param[in] dsm_item   Pointer to the DSM chain.
  @param[in] dsm_len    DSM chain length.
  
  @return
  The total number of DSM items in the chain.

  @dependencies
  None.
*/
uint32 seccom_dsm_item_ncount(dsm_item_type *dsm_item, uint16 dsm_len);

/**
  Transforms data in the DSM chains in the iovector elements.
  
  @param[in] dsm_item       Pointer to the DSM chain.
  @param[in] dsm_len        DSM chain length.
  @param[in,out] iovt       Pointer to iovector.
  
  @return
  The total number of bytes in the DSM item that have been stored.

  @dependencies
  None.
*/
uint16 seccom_dsm_2iovect(dsm_item_type *dsm_item, uint16 dsm_len, IOVEC_T *iovt);
/** @} */  /* end_addtogroup_crypto_secodfapi */

uint32 seccom_dsm_item_lcount(dsm_item_type *dsm_item, uint16 *len);

secerrno_enum_type seccom_enc_encrypt_dsm
(
  secapi_handle_type         handle,
  secapi_algo_enum_type      algo,
  dsm_item_type**            in_msg_ptr,
  dsm_item_type**            out_msg_ptr,
  uint16*                    msg_byte_len,
  boolean                    consume_input,
  secapi_mac_pkt_info_type*  mac_pkt_info
);

secerrno_enum_type seccom_enc_decrypt_dsm
(
  secapi_handle_type         handle,
  secapi_algo_enum_type      algo,
  dsm_item_type**            in_msg_ptr,
  dsm_item_type**            out_msg_ptr,
  uint16*                    msg_byte_len,
  boolean                    consume_input,
  secapi_mac_pkt_info_type*  mac_pkt_info
);

secerrno_enum_type seccom_hsh_create_digest_dsm
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  dsm_item_type*                msg_ptr,
  uint8*                        data_ptr,
  uint16                        msg_byte_len,
  dsm_item_type**               msg_digest_ptr,
  secapi_hsh_mode_enum_type	hsh_mode
);

secerrno_enum_type seccom_hsh_create_hmac_dsm
(
  secapi_handle_type         handle,
  secapi_hsh_hmac_enum_type  hmac_type,
  secapi_algo_enum_type      hmac_algo,
  dsm_item_type*             inner_msg_ptr,
  uint8*                     inner_data_ptr,
  uint16                     inner_byte_len,
  dsm_item_type*             outer_msg_ptr,
  uint8*                     outer_data_ptr,
  uint16                     outer_byte_len,
  dsm_item_type**            hmac_ptr
);
#endif // SECDSM_H
