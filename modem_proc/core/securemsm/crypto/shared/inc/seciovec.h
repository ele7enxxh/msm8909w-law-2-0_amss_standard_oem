#ifndef SECIOVEC_H
#define SECIOVEC_H

/** 
@file seciovec.h
@brief Cryptographic Data Format - SEC IOVEC manipulation.
   This file contains the definitions of the constants, data structures, and
   interfaces that provide Cryptographic Data Format. These utilities are used 
   for sec iovec manipulation. Currently, there is no dynamic allocation for 
   growing or shrinking vectors. This could change in the future. 
*/

/*===========================================================================
  Copyright (c) 2005 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: $
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/inc/seciovec.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/10   dm      Crypto API files rearchitecture
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
11/22/06   df      changed fnames to reflect sec instead of dsm
11/28/05   df      Allow trimming to 0 len
08/31/05   df      Initial revision

===========================================================================*/

#include "comdef.h"
#include "memory.h"
#include "dsm.h"
#include "secapi.h"

/** @addtogroup crypto_secodfapi 
  @{ */


/**
  Gets the number of bytes in the iovec array.
  
  @param[in] iovt     Pointer of the iovector array.
  
  @return
  The number of bytes in the iovector array.

  @dependencies
  None.
*/
uint32 iovect_len(IOVEC_T *iovt);

/**
  Extracts to a specified destination buffer from the IO vector with count members, exactly the 
  number of bytes starting at the offset from the beginning and keeps the IOVEC intact.
  
  @param[in] dest    Pointer to the destination buffer.
  @param[in] iovt    Pointer to the iovector array.
  @param[in] num     Extracts the number of bytes.
  
  @return
  Total number of bytes extracted.

  @dependencies
  Remember to set iovt to offset before calling this function. 
*/
uint32 iovect_extract(uint8 *dest, IOVEC_T *iovt, uint32 num);

/**
  Truncates the bytes in IOVECT from offset to end.
  
  This function shortens the IOVECT by the total number of offset bytes. This version does not free 
  any bytes.

  @param[in] iovt   Pointer to iovector.
  
  @return
  Number of bytes truncated.

  @dependencies
  None.
*/
uint32 iovect_trunc(IOVEC_T *iovt);

/**
  Removes the first item of the IOVEC table, adjusting the size.

  @param[in] iovt  Pointer to iovector.
  
  @return
  The total bytes of iovector.
  
  @dependencies
  None.
*/
int iovect_trim(IOVEC_T *iovt);

/**
  Frees all vectors in the IOVEC table.

  @param[in] iovt   Pointer to iovector.
  
  @return
  The total bytes that have been freed.

  @dependencies
  None.
*/
uint32 iovect_free(IOVEC_T *iovt);

/**
  Inserts a pre-allocated buffer into the IOVEC table, adjusting the size.
  
  @param[in] iovt    Pointer to iovector.
  @param[in] src     Pointer to the source buffer.
  @param[in] num     Number of bytes.
  
  @return
  - The number of bytes successfully prepended.
  - The number of elements in the iovector.

  @dependencies
  None.
*/
uint32 iovect_prepend(IOVEC_T* iovt, uint8* src, uint32 num);

/**
  Appends the number of bytes to the end of this iovt.

  @param[in] iovt   Pointer to iovector.
  @param[in] src    Pointer to the source buffer.
  @param[in] num    Number of bytes.
  
  @return
  The number of bytes successfully appended.

  @dependencies
  - The buffer must be pre-allocated. 
  - The IOVEC table must be big enough to hold an extra vector.
*/
uint32 iovect_append(IOVEC_T* iovt, uint8* src, uint32 num);

/**
  Writes the number of bytes to iovt, starting at the current offset of 
  iovector.
  
  @param[in] iovt   Pointer to iovector.
  @param[in] src    Pointer to the source buffer.
  @param[in] num    Number of bytes.
  
  @return
  The number of bytes successfully written.

  @dependencies
  None.
*/
uint32 iovect_write(IOVEC_T* iovt, uint8* src, uint32 num);

/**
  Counts the number of DSM items in the chain.
  
  @param[in] dsm_item  Pointer to the DSM chain.
  
  @return
  The total number of DSM items in chain.

  @dependencies
  None.
*/
uint32 sec_dsm_item_count(dsm_item_type *dsm_item);

/**
  Counts the number of DSM items in the chain with a specific byte count.
  
  @param[in] dsm_item   Pointer to the DSM chain.
  @param[in] dsm_len    DSM chain length.
  
  @return
  The total number of DSM items in the chain.

  @dependencies
  None.
*/
uint32 sec_dsm_item_ncount(dsm_item_type *dsm_item, uint16 dsm_len);

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
uint16 sec_dsm_2iovect(dsm_item_type *dsm_item, uint16 dsm_len, IOVEC_T *iovt);
/** @} */  /* end_addtogroup_crypto_secodfapi */
#endif // SECIOVEC_H
