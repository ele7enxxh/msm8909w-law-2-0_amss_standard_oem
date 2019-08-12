/*!
  @file
  rflm_dtr_tx_lmem.h

  @brief
  RFLM DTR TX control interface for both DRIF register and LUT
  programming.
 
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/13   st      DRIF Group A/B Support
09.30/13   hdz     Moved rflm_dtr_tx_xfer_list_s to rflm_api_cmn.h
07/18/13   BW      Initial version
===========================================================================*/


//   Pictorial representation of below lines for DRIF and LUT on each chain
/*       ----------------------------------------------------- 
         |<----Banks 0 - 7 ---------------------------------->|<-- 0 (line offset)
         |                  DRIF (set 0)                      |
         |--------------------------------------------------- |<-- 2
         |                  EVT (set 0)                       |
         |----------------------------------------------------|<-- 3
         |                  DRIF (set 1)                      |
         |----------------------------------------------------|<-- 5
         |                  EVT (set 1)                       |
         |----------------------------------------------------|<-- 6
         |                  DRIF (set 2)                      |
         |----------------------------------------------------|<-- 7
         |                  EVT (set 2)                       |
         |----------------------------------------------------|<-- 9 
         |            LUT (11 banks, power level 0)           |
         |----------------------------------------------------|<-- 20
         |            LUT (11 banks, power level 1)           |
         |----------------------------------------------------|<-- 31
         |            LUT (11 banks, power level 2)           |
         |----------------------------------------------------|<-- 42
         |            LUT (11 banks, power level 3)           |
         |----------------------------------------------------|<-- 53
         |            LUT (2 banks, power level 4)            |
         |----------------------------------------------------|<-- 55
         |            LUT (2 banks, power level 5)            |
         |----------------------------------------------------|<-- 57
         |            LUT (2 banks, power level 6)            |
         |----------------------------------------------------|<-- 59
         |            LUT (2 banks, power level 7)            |
         |----------------------------------------------------|<-- 61

     This memory map is the LMEM that FED need to program inadvance when FW try 
     to program the TX for both DRIF register and LUT. Extra set of DRIF is
     for LTE, and power level 4 - 7 is for EPT AMAM/AMPM on GSM. 
*/

#ifndef RFLM_DTR_TX_LMEM_H
#define RFLM_DTR_TX_LMEM_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_dtr_tx_fw_intf_ag.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==========================================================================*/

/*! @brief Number of banks of LUT for each power level */
#define RFLM_DTR_TX_LUT_BANK_NUM_PER_LEVEL      11 

/*! @brief Number of banks of additional 4 power level for EPT AMAM/AMPM */
#define RFLM_DTR_TX_LUT_BANK_NUM_ADDL_LEVEL     2 

/*! @brief Word Size of each LUT which is 64 active/pending 
           memory address with 16 bits */
#define RFLM_DTR_TX_LUT_WORD_SIZE               32  

/*! @brief Number of DRIF register */
#define RFLM_DTR_TX_DRIF_REG_NUM                64

/*! @brief Number of power level for LUT assignment. First 4 power level (0-3) will use 11 banks. 
           Addiotinal 4 power level (4-7) for EPT AMAM/AMPM will use 2 banks. */
#define RFLM_DTR_TX_POWER_LEVEL_NUM             4 

/*! @brief Number of dynamically changed register set, 
           including DRIF reg and EVT reg */
#define RFLM_DTR_TX_DYN_REG_SET                 3 

/*! @brief Maximum number of block data to be DMA transferred */
#define RFLM_DTR_TX_DATA_XFER_NUM               9

/*! @brief Destination Addr for Bridge programming LUT address */
#define RFLM_DTR_TX_LUT_DEST_ADDR(x) 			(((x) == 0) ? (0x0000) : (0x1000))

/*! @brief Destination Addr for Bridge programming DRIF address */
#define RFLM_DTR_TX_DRIF_DEST_ADDR(x)			(((x) == 0) ? (0x2000) : (0x3000))

/*! @brief Address offset corresponding to DRIF SET 0 */
#define RFLM_DTR_TX_DRIF_OFFSET_0				0

/*! @brief Address offset corresponding to DRIF SET 1 */
#define RFLM_DTR_TX_DRIF_OFFSET_1				RFLM_DTR_TX_DRIF_REG_NUM

/*! @brief Address offset corresponding to DRIF SET 2 */
#define RFLM_DTR_TX_DRIF_OFFSET_2				RFLM_DTR_TX_DRIF_REG_NUM*2

/*! @brief Address offset corresponding to DRIF SET */
#define RFLM_DTR_TX_DRIF_OFFSET(x) 				RFLM_DTR_TX_DRIF_OFFSET_##x

//XXX fw_txlm_intf_ag.h define DRIF register struction as fw_txlm_tx_drif_group_struct

/*! @brief Structure for EVT register, maximum word size at 1 LMEM line */
//XXX fw_txlm_intf_ag.h define EVT register structure as fw_txlm_tx_evt_group_struct with 10 words
typedef struct
{
  uint32 dtr_tx_evt_reg[128/sizeof(uint32)]; // 128 byte per line cross eight bank
} rflm_dtr_tx_evt_reg_s;

/*! @brief Structure for dynamically changed register, 
           including DRIF register and EVT register */
typedef struct 
{
  /*! @brief 64 DRIF register */
//  rflm_dtr_tx_txfe_drif_group_struct drif_reg;
  /*! @brief EVT register */
  union // Use max size EVT structure (1 line) to align LUT LMEM0 start line
  {
    rflm_dtr_tx_evt_reg_s evt_reg;
//    rflm_dtr_tx_txfe_evt_group_struct evt_group;
  };
} rflm_dtr_tx_dyn_reg_s;

/*! @brief Structure for LUT programming on first four power level */
typedef struct 
{
  /* Total 11 banks of LUT programming for first 4 power level (power level 0 - 3) */
  uint32 dtr_tx_lut[RFLM_DTR_TX_LUT_BANK_NUM_PER_LEVEL][RFLM_DTR_TX_LUT_WORD_SIZE];
} rflm_dtr_tx_lut_s;

/*! @brief Structure for EPT LUT programming on additional four power level */
typedef struct 
{
  /* Two banks for EPT AMAM/AMPM additional 4 power level (power level 4 - 7) */
  uint32 dtr_tx_lut_addl[RFLM_DTR_TX_LUT_BANK_NUM_ADDL_LEVEL][RFLM_DTR_TX_LUT_WORD_SIZE];
} rflm_dtr_tx_lut_addl_s;

/*! @brief Structure for DRIF reg and LUT on each chain */
typedef struct
{
 /*! @brief Dynamically changed register */
 rflm_dtr_tx_dyn_reg_s dyn_reg[RFLM_DTR_TX_DYN_REG_SET];
 /*! @brief LUT programming on first four power level */
 rflm_dtr_tx_lut_s dtr_tx_lut[RFLM_DTR_TX_POWER_LEVEL_NUM];
 /*! @brief LUT programming on additional four power level */
 rflm_dtr_tx_lut_addl_s dtr_tx_lut_addl[RFLM_DTR_TX_POWER_LEVEL_NUM];
} rflm_dtr_tx_ctrl_intf_s;

typedef struct {
	uint32 drif_group_A_offsets;
	uint32 drif_group_B_offsets;
} rflm_dtr_tx_lmem_drif_grp_start_info_t;


typedef struct
{
  uint32 mem0_groupA;
  uint32 mem1_groupA;
  uint32 mem0_groupB;
  uint32 mem1_groupB;
} rflm_dtr_tx_set_shared_mem_input_t;

typedef struct
{
  uint32 reserved;
} rflm_dtr_tx_set_shared_mem_output_t; 

#endif
