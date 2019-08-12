#ifndef RFLM_TXC_H
#define RFLM_TXC_H
/*
  @file
  rflm_txc.h

  @brief
  This file contails APIs used to access TXC for TxAGC

  @addtogroup RFLM_TXAGC
  @{
*/

/*==============================================================================

Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Dates
are in ISO format.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/rflm/txagc/inc/rflm_txc.h#1 $

when         who    what, where, why
---------   ---     -----------------------------------------------------------
06/11/14     zhw    Compiler warning fix
04/04/14     zhw    Update DPD size enum according to SWI
03/27/14     zhw    Field based update support
02/20/14     zhw    Initial creation
==============================================================================*/
#include "comdef.h"
#include "msm.h"
#include "txlm_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! reg: Register name */
/*! idx: Which TX chain */
/*! fmask: Mask for fields to be updated */
/*! updateval: Values to be OR'ed in. Note that the values 
  have to ALREADY shifted to right bitfield locations */
#define RFLM_UPDATE_FIELDSI(reg, idx, fmask, updateval ) \
  HWIO_##reg##_OUTMI(idx, fmask, updateval)

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Maximum number of continuous DPD tables writes. For 8926+FED practice, use
    DIMEPM implementation.
    4 Banks of 256 entries of 32 bits and 2 Banks of 64 enrties of 32 bits
    for unified txc memory. The 2 small banks are not used by common.
                            
    DIMEPM - 4 Banks of 256 entries of 32 bit  
*/
#define RFLM_TXC_BANKS_NUM                                                     4

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Size of TXC Memory banks
*/

#define RFLM_TXC_MEM_BANK_SIZE                                              1024

/*----------------------------------------------------------------------------*/
/*! @brief Structure for mapping LUT sizes in words to
           what needs to be in the register based on SWI */
typedef enum
{
  RFLM_TXC_LUT_512_WORDS_FVAL,
  RFLM_TXC_LUT_256_WORDS_FVAL,
  RFLM_TXC_LUT_128_WORDS_FVAL,
  RFLM_TXC_LUT_64_WORDS_FVAL,
  RFLM_TXC_LUT_32_WORDS_FVAL,
  RFLM_TXC_LUT_NUM_SIZES
} rflm_txc_lut_size_fval;

/*----------------------------------------------------------------------------*/
void 
rflm_txc_update_amam_pending_t1
(
  uint32 chain_idx,
  uint32 bank,
  uint32 base,
  rflm_txc_lut_size_fval size 
);

/*----------------------------------------------------------------------------*/
void 
rflm_txc_update_ampm_pending_t1
(
  uint32 chain_idx,
  uint32 bank,
  uint32 base,
  rflm_txc_lut_size_fval size 
);

/*----------------------------------------------------------------------------*/
void 
rflm_txc_update_amam_pending_t2
(
  uint32 chain_idx,
  uint32 bank,
  uint32 base,
  rflm_txc_lut_size_fval size 
);

/*----------------------------------------------------------------------------*/
void 
rflm_txc_update_ampm_pending_t2
(
  uint32 chain_idx,
  uint32 bank,
  uint32 base,
  rflm_txc_lut_size_fval size 
);

/*----------------------------------------------------------------------------*/
void 
rflm_txc_write_lut
( 
  /*! number of words to program */
  uint32 num_words,
  /*! bank index */
  uint32 bank,
  /*! starting address relative to bank boundary */
  uint32 start_addr,
  /*! pointer to LUT data */
  uint32 *data 
);

#ifdef __cplusplus
}
#endif

#endif /* RFLM_TXAGC_DPD_H */
