/*!
  @file
  lte_LL1_log_prs.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_prs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/17/12   cj      Update PRS log B11A to include S/S+N data.
08/29/11   cj      Update PRS FW log structure to meet 32-bit alignment.
06/03/11   cj      Created header file to define PRS logs in LTE FW.
===========================================================================*/

#ifndef LTE_LL1_LOG_PRS_H
#define LTE_LL1_LOG_PRS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*==========================================================================*/
/*! @brief
 * 5 LTE LL1 PRS occasion (Log code ID: 0xB11A)
 * 
 *  This log packet gives information regarding PRS processing for a PRS occasion.   
 *  The log record. is generated every PRS occasion in which PRS is present.
 ---------------------------------------------------------------------------*/

#define LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES 6
#define LTE_LL1_LOG_PRS_FW_VERSION 22

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*! 14-bit sequence corresponding to PRS symbols processed encoded as:
        '0': corresponding symbol was not used in PRS processing
        '1': corresponding symbol was used in PRS processing
  */
  uint32 symbol_bitmap                    : 14;

  /*! Estimated Signal to Signal+noise in Q31 format for Rx0 chain */
  uint32 s_sn_est_rx0                     : 31;

  /*! Estimated Signal to Signal+noise in Q31 format for Rx1 chain */
  uint32 s_sn_est_rx1                     : 31;

  /*! Reserved */
  uint32 reserved                         : 32;
  
} lte_LL1_log_prs_subframe_log_struct;



/*! @brief Structure for LTE_LL1_PRS_FW_LOG
*/
typedef struct
{
  /*!< Version number (Range 0 to 255) */
  uint32 version                           : 8;

  /*!< Cell ID of Serving Cell (Range 0 to 504) */
  uint32 serving_cell_id                   : 9;

  /*!< Cyclic Prefix of Serving Cell (0=normal, 1=extended) */
  uint32 serving_cell_cp                   : 1;

  /*!< Number of Tx Antenna of Serving Cell 
        0x00 = 1 Antenna
        0x01 = 2 Antenna
        0x02 = 4 Antenna
        0x03 = Invalid
  */
  uint32 serving_cell_num_tx_ant           : 2;

  /*!< Cell ID of Measured Cell (Range 0 to 504) */
  uint32 measured_cell_id                  : 9;

  /*!< Number of Tx Antenna of Measured Cell 
        0x00 = 1 Antenna
        0x01 = 2 Antenna
        0x02 = 4 Antenna
        0x03 = Invalid
  */
  uint32 measured_cell_num_tx_ant          : 2;
  uint32 reserved0                         : 1; // 32 bit boundary
  /*!< PRS Bandwidth of Measured Cell 
        0x0 = 1.44 MHz
        0x1 = 3 MHz
        0x2 = 5 MHz
        0x3 = 10 MHz
        0x4 = 15 MHz
        0x5 = 20 MHz
        0x6-0x7 = Reserved
  */
  uint32 prs_bw                            : 3;

  /*!< PRS Bandwidth set in VPE */
  uint32 prs_bw_vpe                        : 3;

  /*!< Number of PRS subframes of Measured Cell (Range 1, 2, 4, or 6) */
  uint32 prs_dl_subframes                  : 3;

  /*!< Symbol Offset of Measured Cell w.r.t Serving Cell. (Range -4 to +4) */
  int32  symbol_offset                     : 4;

  /*!< Pruning Window size (Range 0 to 2048) */
  uint32 prune_win_size                    : 12;
  uint32 carrier_index                     : 4; ///< 0-PCC, 1-SCC 
  uint32 reserved1                         : 3; // 32 bit boundary
  uint32 prune_win_start                   : 12;
  uint32 bin                               : 2;
  uint32 rx0_scaling_factor                : 8;
  uint32 rx1_scaling_factor                : 8;
  uint32 reserved2                         : 2; // 32 bit bondary
  int16  ttl_adj                           :16;
  int64  mstmr_adj                         :64;
  int32  com_adj                           :32;
  uint16 reserved3                         :16; // 32 bit boundary.
  /*! Per-Subframe Log Data */
  lte_LL1_log_prs_subframe_log_struct lte_LL1_log_prs_subframe_log_data[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES];

} lte_LL1_log_prs_fw_ind_struct;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* LTE_LL1_LOG_PRS_H */
