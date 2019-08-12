/*!
  @file
  intf_prs_sm.h

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

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/09/13   cj      Adding COM adjustment value to PRS smem interface. 
04/17/12   cj      Add PRS S/(S+N) data to sharedmem interface.
11/01/11   cj      Add params for PRS start index and prune window to per-cell 
                   structure in shared mem.  
05/02/11   cj      Added PRS message router interface.
===========================================================================*/

#ifndef INTF_PRS_SM_H
#define INTF_PRS_SM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "intf_common.h"
#include "intf_prs.h"
#include "lte_LL1_log_prs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifndef PRS_SFINFO_DBG_LOGGING
#define PRS_SFINFO_DBG_LOGGING
#endif 

/*! @brief cell-specific PRS Meas descriptor.
*/
typedef struct
{
  /*! Physical Cell ID corresponding to the CER vector. 
      Range: 0~503
  */
  uint16  cer_cell_id;  
    
  /*! slot Offset */
  uint8 slot_offset;

  /*! Symbol Tap Offset */
  int8 symbol_offset;

  /*! Start Tap Index in Ts [0-2048] */
  uint16 start_idx; 

  /*! PRS Search Window Prune Size in Ts [0-2048]*/
  uint16 prune_size; 

  /*! Symbol Bitmap corresponding to CER */
  uint16 symbol_bitmap[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES];

  /*! S/(S+N) data in Q31 format for Rx 0 */
  uint32 avg_s_sn_rx0;

  /*! S/(S+N) data in Q31 format for Rx 1 */
  uint32 avg_s_sn_rx1;
    
  /*! the number of the prs subframe per occasion */
  lte_LL1_prs_num_subframe_e n_prs;

  /*! Cell Center frequency */
  lte_earfcn_t    earfcn;

  /*! Memory offset for raw CER vectors relative to beginning of PRS CER buffer. */
   uint16    mem_offset;

  /*! Bias or scaling factor for PRS. Range [-15:13] */
  int8 agc_bias;

  /*! ECP/NCP indication: 0=NCP, 1=ECP */
  uint8 cp_mode;

  /*! CRS/PRS indication: 0=PRS, 1=CRS */
  uint8 search_mode;

  /*! eNB Tx Ant that was searched. Used only for CRS search */
  uint8 eNB_tx_ant_num; 

} lte_LL1_prs_cell_desc_s;

/*! @brief occasion-specific PRS Meas descriptor.
    This descriptor will be use for both messaging
    ML1, and to write to SharedMem.
*/
typedef struct
{
  /*! TimeStamp aligned to frame boundary of PRS Occasion,
      time read from 32-bit VSRC sample counter. 
      (alternatively 64-bit DL Link counter)
  */
  int64  timestamp; 

  int16    ttl_adj[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES];

  /*! Accumulated COM (Center-Of-Mass) Adjustment, in Ts units, Q6 format */
  int32    com_adj[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES]; 

  /*! Accumulated MSTMR adjustment, in Ts units, Q0 format. */
  int64    mstmr_adj[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES];
  int64    mstmr_adj_offset_logging[LTE_LL1_LOG_PRS_NUMBER_OF_SUBFRAMES];

  /*! cell ID of the serving sector in this PRS occasion */
  uint16  serv_cell_id; 
      
  /*! PRS Meas Result Sequence Number. This will be used 
    by GNSS SW to check if the data they read have been
    overwritten or not.
  */
  uint32  occasion_seq_num;

  /*! the number of the prs cell data in this prs occasion */
  uint8  num_cell; 
  
  /*! Flag indicating if FD DMA was newly done for this prs occasion or not */
  boolean fd_dma_is_new;

  /*! TDD_FDD mode: 0=FDD, 1=TDD */
  uint8 duplex_mode;

  /*! SF bitmask. LSB corresponds to first subframe */
  uint32 sf_bit_mask;

  boolean fd_buffer_logging;
  /*! SF Info Debug parameters */
  #ifdef PRS_SFINFO_DBG_LOGGING
  int64 dl_subframe_ref_time; ///< ref time at start of DL subframe
  int64 dl_subfn_ref_time_raw;///< ref time without mstmr correction
  int64 dl_subframe_ref_time_offline;
                              ///< ref time at start of DL subframe until
                              ///< sample recording is ON = dl_subframe_ref_time
                              ///< Once sample recording is frozen, 
                              ///< dl_subframe_ref_time_offline is also frozen.

  int64 dl_subframe_ext_mstmr;///< unwrapped MSTMR count at DL subframe
  int64 dl_subframe_rtc;      ///< ref time without TTL correction and mstmr correction
  uint32 dl_subframe_cnt;     ///< subframe counter
  int32 dl_subframe_mstmr;    ///< O_STMR MSTMR count at start of DL subframe
  uint32 dl_sf_univ_stmr_cnt; ///< UNIV_STMR MSTMR count at start of DL subframe
  int32 dl_subframe_sr_addr;  ///< SR addr at start of DL subframe
  int32 ul_subframe_mstmr;    ///< O_STMR MSTMR count at start of UL subframe
  uint32 ul_sf_univ_stmr_cnt; ///< UNIV_STMR MSTMR count at start of UL subframe
  int16 subframe_num;         ///< subframe number 
  int16 frame_num;            ///< radio frame number
  uint16 subframe_frame_num;  ///< bits 0~3  = subframe number
  #endif //PRS_SFINFO_DBG_LOGGING
  
  /*! Header containing relevant info for PRS measurement vector */
  lte_LL1_prs_cell_desc_s cell_info[LTE_LL1_PRS_NUM_CELL_MAX]; 
  
} lte_LL1_prs_occasion_desc_s;

/*! @brief cell-specific PRS Meas Data 
*/
typedef struct
{
  /*! Vector containing PRS measurement of a single cell, 
    consisting of 2048 array of 12-bit data.
  */
  uint32 cer_vector[LTE_LL1_PRS_CELL_SIZE_MAX]; 
  
} lte_LL1_prs_cell_data_s;

/*! @brief PRS Meas descriptor and data for the 
    whole PRS occasion. This will be written to
    SharedMem.
*/
typedef struct
{  
  /*! Individual Cell Data, up to 4 cells */
  //uint32 cell_data[LTE_LL1_PRS_NUM_CELL_MAX][LTE_LL1_PRS_CELL_SIZE_MAX];//4*2048
   uint32 cell_data[LTE_LL1_PRS_CER_BUF_SIZE_IN_WD]; //8*2048 
  /*! PRS Occasion Meas Descriptor */
  lte_LL1_prs_occasion_desc_s occasion_info;
  
  /*! Jolokia does not support UFL12 well. Flag=1 indicates using int16, Flag=0 indicate using UFL12. For jolokia it will always be 1. */
  boolean use_int16;

} lte_LL1_prs_occasion_meas_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* INTF_PRS_SM_H */
