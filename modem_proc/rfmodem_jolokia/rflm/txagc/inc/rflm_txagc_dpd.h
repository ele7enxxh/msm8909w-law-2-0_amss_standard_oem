#ifndef RFLM_TXAGC_DPD_H
#define RFLM_TXAGC_DPD_H
/*
  @file
  rflm_txagc_dpd.h

  @brief
  This file contails APIs used to access DPD tables

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/rflm/txagc/inc/rflm_txagc_dpd.h#1 $

when         who    what, where, why
---------    ---    -----------------------------------------------------------
03/06/14     zhw    Move Init to RFLM API
02/20/14     zhw    Initial creation
==============================================================================*/
#include "comdef.h"
#include "rflm_defs.h"
#include "rflm_txc.h"
#include "rflm_txagc_modem_api.h"
#include "qurt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number of tables that fit in a bank */
#define RFLM_MAX_DPD_TBLS_PER_BANK                 RFLM_TXC_MEM_BANK_SIZE/(64*2)
/* Number of banks allocated to a tx path */
#define RFLM_MAX_DPD_BANKS_PER_CHAIN 2 /* WARNING: If this val changes, search for refs in SW for 
                                        calculations that depend on this val */

#define RFLM_MAX_DPD_EPT_LUT_NUM                                               8
#define RFLM_DPD_INDEX_INVALID_VAL                                          0xFF

typedef enum
{
  RFLM_LOAD_DPD_TYPE_EPT,
  RFLM_LOAD_DPD_TYPE_NONE
} rflm_load_dpd_type;

typedef enum
{
  RFLM_DPD_INFO_BANK_0,
  RFLM_DPD_INFO_BANK_1,
  RFLM_DPD_INFO_BANK_2,
  RFLM_DPD_INFO_BANK_3,
  RFLM_DPD_INFO_BANK_NUM
} rflm_dpd_info_bank_id_type;

/* Dime has 12 DPD banks. Choose to use only 4 for 8926+FED. Remove Dime feature
when migrate to DIMEPM model*/
#ifdef FEATURE_DIME_MODEM
COMPILE_ASSERT(RFLM_DPD_INFO_BANK_NUM == 4);
#else
COMPILE_ASSERT(RFLM_DPD_INFO_BANK_NUM == RFLM_TXC_BANKS_NUM);
#endif

typedef enum
{ 
  RFLM_DPD_DATA_BANK_PER_CHAIN_0,
  RFLM_DPD_DATA_BANK_PER_CHAIN_1,
  RFLM_DPD_DATA_BANK_PER_CHAIN_NUM,
} rflm_dpd_bank_index_per_chain_type;

#define RFLM_DPD_TYPE_AMAM_EXPANDED_TBL_SIZE                                  64
#define RFLM_DPD_TYPE_AMPM_EXPANDED_TBL_SIZE                                  64

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing xpt dpd data.
*/ 
typedef struct
{
  /*! Contains the amount of used space in the corresponding bank */
  uint16 bank_used_space[RFLM_MAX_DPD_BANKS_PER_CHAIN];
  /*! Indicates the type of dpd data contained in the bank */
  rflm_dpd_tbl_type dpd_types_in_bank[RFLM_MAX_DPD_BANKS_PER_CHAIN];
  /*! Contains the actual dpd data */
  uint8 dpd_data[RFLM_MAX_DPD_BANKS_PER_CHAIN][RFLM_TXC_MEM_BANK_SIZE] ALIGN(4);
  /*! TxAGC DPD LUT for different pwr_level_idx at TxAGC */
  rflm_txagc_xpt_dpd_lut_data txagc_dpd_lut;

} rflm_dpd_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief Information regarding one continuous block of data to be 
         written to the DPD tables */
typedef struct 
{
  /*! @brief TXC bank ID to write data */
  uint8 bank_id;

  /*! @brief Start addr within bank (0-255) */
  uint8 start_addr;

  /*! @brief Number of words to write in a bank (if the table is 16 bit value, 
    RF should still send the writes as 32 bit writes) */
  uint16 num_words;

  /*! @brief Physical address from where to copy table to TXC memory */
  uint32 *data_phys_addr;
} rflm_dpd_info_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by RF COMMON Driver. 
*/ 
typedef struct
{
  /*! Common driver status */
  rflm_dpd_data_type dpd_data[TXLM_CHAIN_MAX];

  /*! DPD Info */
  rflm_dpd_info_type dpd_info[RFLM_TXC_BANKS_NUM];

  /*! Record of number of used banks */
  uint32 num_banks_used;

  /*! Record of used bank mask */
  uint32 used_bank_mask;

  /*! Mutex lock that protects chain-common record keeping variables from 
    concurrency */
  qurt_mutex_t dpd_bank_record_lock;

} rflm_txagc_dpd_data_type;

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_dpd_init_data
(
  uint8 chain
);

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_dpd_save_data
(
  uint8   chain,
  uint16* dpd_tbl_ptr,
  uint16  dpd_tbl_sz,
  uint8   tbl_type,
  uint8*  power_level_index,
  uint16* bank_offset
);

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_dpd_commit_data
(
  uint8 chain
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_dpd_update_dpd_txc_pending_t1
( 
   uint8 chain, 
   uint8 pwr_lvl_idx
);

#ifdef __cplusplus
}
#endif

#endif /* RFLM_TXAGC_DPD_H */
