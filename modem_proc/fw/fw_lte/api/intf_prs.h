/*!
  @file
  intf_prs.h

  @brief
  File containing structures for PRS FW module interface.

  @detail

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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
05/02/11   cj      Added PRS message router interface.
===========================================================================*/

#ifndef INTF_PRS_H
#define INTF_PRS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "intf_common.h"
#include "intf_dl_msg.h"
#include "lte_LL1_log_prs.h"

#ifndef PRS_FD_DMA_AUTO_SLEEP_ENABLED
#define PRS_FD_DMA_AUTO_SLEEP_ENABLED
#endif //PRS_FD_DMA_AUTO_SLEEP_ENABLED

/*===========================================================================

                                 MACROS

===========================================================================*/
#define LTE_LL1_PRS_NUM_CELL_MAX               10
#define LTE_LL1_PRS_NUM_SF_MAX                 6
/* PRS CER results will be 2048 of 12UFL packed to 32S= 2048*12/32 = 768 */
#define LTE_LL1_PRS_CELL_SIZE_MAX           2048
#define LTE_LL1_PRS_CER_RESULT_SIZE         LTE_LL1_PRS_CELL_SIZE_MAX
#define LTE_LL1_PRS_CER_BUF_SIZE_IN_WD      (LTE_LL1_PRS_NUM_CELL_MAX*LTE_LL1_PRS_CELL_SIZE_MAX)

/*===========================================================================

                      EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Response Types for PRS Config and Meas Request.
*/
typedef enum
{
  /*! SUCCESS */
  LTE_LL1_PRS_RSP_SUCCESS = 0,   

  /*! KERNEL Load/Reload Fail */
  LTE_LL1_PRS_RSP_KERNEL_FAIL,

  /*! Invalid Parameters */
  LTE_LL1_PRS_RSP_INVALID_PARAMS,

  /*! Aborted */
  LTE_LL1_PRS_RSP_ABORT,

  /*! Unknown Error */
  LTE_LL1_PRS_RSP_UNKNOWN_ERROR,

  /*! MBSFN Collision */
  LTE_LL1_PRS_RSP_COLLISION_MBSFN,

  /*! PRS CORE still under processing */
  LTE_LL1_PRS_CORE_STILL_IN_PROC,

  /*! FD DMA Error. */
  LTE_LL1_PRS_FD_DMA_ERROR,

  /*! Max enum for PRS Response Type */
  LTE_LL1_PRS_RSP_MAX     
} lte_LL1_prs_rsp_e;

/*! @brief Enum describing LTE PRS Port Configuration
*/
typedef enum
{
  /*! 1 or 2 PBCH antenna ports, Normal CP */
  LTE_LL1_PRS_PORT_1_2_PBCH = 0,   
  
  /*! 4 PBCH antenna ports, Normal CP */
  LTE_LL1_PRS_PORT_4_PBCH,   

  /*! Max enum value for PRS Port Map config. */
  LTE_LL1_PRS_PORT_MAX

} lte_LL1_prs_port_map_e;

/*! @brief Enum describing the LTE PRS Cycle Prefix configuraiton 
*/
typedef enum
{
  /*! Normal CP */
  LTE_LL1_PRS_NORMAL_CP = 0,

  /*! Extended CP */
  LTE_LL1_PRS_EXTENDED_CP,

  /*! MAX enum limit */
  LTE_LL1_PRS_CP_MAX

} lte_LL1_prs_cyclic_prefix_e;

/*! @brief REQUIRED one-sentence brief description of this enumeration typedef
*/
typedef enum
{
  /*!< 1 PRS Subframe in PRS Occasion */
  LTE_LL1_PRS_1_SF = 0,   

  /*!< 2 PRS Subframe in PRS Occasion */
  LTE_LL1_PRS_2_SF,   

  /*!< 4 PRS Subframe in PRS Occasion */
  LTE_LL1_PRS_4_SF,   

  /*!< 6 PRS Subframe in PRS Occasion */
  LTE_LL1_PRS_6_SF,   

  /*!< Maximum support to 6 Tx Antenna */
  LTE_LL1_PRS_TX_MAX = LTE_LL1_PRS_6_SF

} lte_LL1_prs_num_subframe_e;





/*===========================================================================
                                  PRS_CONFIG
===========================================================================*/
/*! @brief PRS Config Info from ML1.
*/
typedef struct
{
  /*!  common req header */
  lte_LL1_req_header_struct req_hdr;

  /*! PRS BW: 1.4, 3, 5, 10, 15, 20 MHz */
  lte_bandwidth_enum bandwidth; 
    
} lte_LL1_dl_prs_config_req_struct;

// LTE_LL1_INTF_MSG( dl_prs_config_req );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_config_req_struct       msg_payload;
} lte_LL1_dl_prs_config_req_msg_struct;



/*! @brief PRS Config CNF to ML1.
*/
typedef struct
{
  /*!  common cnf header */
  lte_LL1_cnf_header_struct cnf_hdr;

  /*! PRS Config Status */
  lte_LL1_prs_rsp_e status;

  /*! Result is interfered by other tech */
  boolean  interf_active;
} lte_LL1_dl_prs_config_cnf_struct;

//LTE_LL1_INTF_MSG( dl_prs_config_cnf );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_config_cnf_struct       msg_payload;
} lte_LL1_dl_prs_config_cnf_msg_struct;

/*===========================================================================
                                  PRS_ABORT
===========================================================================*/

/*! @brief PRS Abort Request from ML1.
*/
typedef struct
{
  /*!  common req header */
  lte_LL1_req_header_struct req_hdr;
  
} lte_LL1_dl_prs_abort_req_struct;

//LTE_LL1_INTF_MSG( dl_prs_abort_req );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_abort_req_struct       msg_payload;
} lte_LL1_dl_prs_abort_req_msg_struct;

/*! @brief PRS Config CNF to ML1.
*/
typedef struct
{
  /*!  common cnf header */
  lte_LL1_cnf_header_struct cnf_hdr;

  /*! PRS Config Status */
  lte_LL1_prs_rsp_e status;

} lte_LL1_dl_prs_abort_cnf_struct;

// LTE_LL1_INTF_MSG( dl_prs_abort_cnf );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_abort_cnf_struct       msg_payload;
} lte_LL1_dl_prs_abort_cnf_msg_struct;

/*===========================================================================
                             PRS_MEAS: ML1-PRS FW
===========================================================================*/

/*! @brief PRS Cell Specific Info structure from ML1.
*/
typedef struct
{
  /*! physical cell ID, necessary for RS Tone split. */
  uint16 cell_id;  
  
  /*! PRS BW. One of 1.4, 3, 5, 10, 15, 20 MHz. 
      Needed for (prs BW requeted < prs BW loaded on uKernel) case,
      where FW should zero out the outer bands.
  */
  lte_bandwidth_enum bandwidth; 
  
  /*! 1 or 2 pbch ants, 4 pbch ports; do we need consider
      other mapping for the multiple timing hypos
  */
  lte_LL1_prs_port_map_e port_map; 
   
  /*! Start Tap Index in Ts [0-2047] */
  uint16 start_idx; 
   
  /*! Prune Size in Ts [0-2047] */  
  uint16 prune_size; 
     
  /*! slot offset */
  uint8 slot_offset;

  /*! symbol delay offset. [-4, 4] */
  int8 symbol_offset; 

  /*! the number of the prs subframe per occasion */
  lte_LL1_prs_num_subframe_e n_prs; 

  /*! Cell Center frequency */
  lte_earfcn_t    earfcn;

  /*! Bias or scaling factor for PRS. [-15:13] */
  int8 agc_bias;
   /*! CP: 0=normal CP, 1=extended CP */
  uint8 cp_mode;

  /*! PRS/CRS indication: 0=POS, 1=SM */
#define LTE_LL1_PRS_SEARCH_MODE_POS 0 
#define LTE_LL1_PRS_SEARCH_MODE_SM  1
  uint8 search_mode; 

  /*! Current hypothesis' eNB Tx Antenna. Used in CRS search only. */
  uint8 eNB_tx_ant_num;

 /*PRS IC enable */
  boolean is_offending;

} lte_LL1_prs_cell_info_s;

/*! @brief PRS occasion info
*/
typedef struct
{
  /*!  common req header */
  lte_LL1_req_header_struct req_hdr;

  /*! next starting subframe number (SFN*10+subf). 
      This field is for sanity check only.
      Range: [0-10239]
  */
  uint16 frame_subframe; 
    
  /*! number of cells to be measured. 
      Range: [1-3]
  */
  uint8 num_cells;

  /*! carrier */
  lte_LL1_carrier_type_enum  carrier;

  /*! FDD/TDD flag: 0=FDD, 1=TDD */
  uint8 duplex_mode; 

  /*! SF bitmask: 
      If in FDD, it should be 0xFFFF
      If in TDD, bit should bet set for SF corresponding to PRS, with
      LSB corresponding to the first PRS subframe.
      eg. 0x55 = b01010101 = SF 0, 2, 4, 6 contain PRS signals. 
  */
  uint32 sf_bitmask;

  boolean fd_buffer_logging;
  
  boolean fd_logging_enabled;
#ifdef PRS_FD_DMA_CONTROL
  /*! Flag to indicate new FD DMA request. 
      If true, new FD DMA will take place. If false,
      PRS processing will run on previously stored FD symbols.
    */
  boolean need_fd_dma;
#endif 

  /*! the OTDOA cells to measure */    
  lte_LL1_prs_cell_info_s cell_info[LTE_LL1_PRS_NUM_CELL_MAX]; 
    
} lte_LL1_dl_prs_meas_req_struct;

//LTE_LL1_INTF_MSG( dl_prs_meas_req );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_meas_req_struct       msg_payload;
} lte_LL1_dl_prs_meas_req_msg_struct;

/*! @brief Cnf msg sent to ML1 when PRS occasion processing is done.
*/
typedef struct
{
  /*!  common cnf header */
  lte_LL1_cnf_header_struct cnf_hdr;

  /*! Status of PRS Occasion processing. */
  lte_LL1_prs_rsp_e occasion_status;  

  /*! PRS Occasion Sequence Number, to be used 
      by Location SW for data validity check.
  */
  uint32 occasion_seq_num;  
  /*! Result is interfered by other tech */
  boolean  interf_active;
} lte_LL1_dl_prs_meas_cnf_struct;

//LTE_LL1_INTF_MSG( dl_prs_meas_cnf );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_meas_cnf_struct       msg_payload;
} lte_LL1_dl_prs_meas_cnf_msg_struct;

typedef struct
{
  /*! Pointer to FD DMA Rx0 buffer. One word is 32 bit. */
  uint32* unpacked_fd_dma_rx0_buf_ptr;

  /*! Pointer to FD DMA Rx1 buffer. One word is 32 bit. */
  uint32* unpacked_fd_dma_rx1_buf_ptr;

  /*! Number of 32-bit words to copy/read, same for Rx0 and Rx1 buf */
  uint32 fd_buf_num_words;
}lte_LL1_prs_fd_buf_info_per_sf_struct;

typedef struct
{
  /*!  common cnf header */
  lte_LL1_ind_header_struct ind_hdr;

  /*! Status of FD DMA status. */
  lte_LL1_prs_rsp_e fd_dma_status;  

  /*! PRS Occasion Sequence Number, to be used 
      by Location SW for data validity check. */
  uint32 occasion_seq_num;

  /*! 10*frame_num + subframe_num */
  uint32 frame_subframe; 

  /*! search mode: 0=PRS, 1=CRS */
  uint8 search_mode;

  /*! Carrier */
  lte_LL1_carrier_type_enum carrier_type;

  /*! LTE SYS BW for the given carrier */
  lte_bandwidth_enum lte_sys_bw;
  
  /*! Array of FD buf info per SF. */
  lte_LL1_prs_fd_buf_info_per_sf_struct fd_buf_info[LTE_LL1_PRS_NUM_SF_MAX];

//  /*! Pointer to FD DMA buffer. One word is 32 bit. */
  uint32* unpacked_fd_dma_buf_ptr;

  /*! Number of 32-bit words to copy/read */
  void* prs_fd_log_ptr;

} lte_LL1_dl_prs_fd_dma_ind_struct;

//LTE_LL1_INTF_MSG( dl_prs_meas_req );
typedef struct {                                      
  msgr_hdr_struct_type                   msg_hdr;
  lte_LL1_dl_prs_fd_dma_ind_struct       msg_payload;
} lte_LL1_dl_prs_fd_dma_ind_msg_struct;

typedef struct
{
  /*!  common cnf header */
  lte_LL1_ind_header_struct ind_hdr;

  /*! Status of FD DMA status. */
  lte_LL1_prs_rsp_e fd_dma_status;  

  /*! PRS Occasion Sequence Number, to be used 
      by Location SW for data validity check. */
  uint32 occasion_seq_num;

  /*! 10*frame_num + subframe_num */
  uint32 frame_subframe; 

  /*! PRS fd dma buffer index to process */
  //uint8 fd_dma_buf_idx;

  /*! Carrier */
  lte_LL1_carrier_type_enum carrier_type;

  /*! LTE SYS BW for the given carrier */
  lte_bandwidth_enum lte_sys_bw;

  /*! Array of FD buf info per SF. */
  lte_LL1_prs_fd_buf_info_per_sf_struct fd_buf_info[LTE_LL1_PRS_NUM_SF_MAX];

//  /*! Pointer to FD DMA buffer. One word is 32 bit. */
  uint32* unpacked_fd_dma_buf_ptr;

  /*! Number of 32-bit words to copy/read */
  void* prs_fd_log_ptr;

} lte_LL1_dl_prs_fd_buffer_logging_ind_struct;

typedef struct {
  msgr_hdr_struct_type                             msg_hdr;
  lte_LL1_dl_prs_fd_buffer_logging_ind_struct      msg_payload;
} lte_LL1_dl_prs_fd_buffer_logging_ind_msg_struct;


#endif /* INTF_PRS_H */
