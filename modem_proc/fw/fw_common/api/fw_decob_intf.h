/*!
  @file
  fw_decob_intf.h

  @brief
  Decoder output results buffer definitions

  @details
  Decoder output is written to the mempool unlike a separate DECOB buffer in
  HW like legacy. This file has details regarding information like
  start line, min bank, max_bank, vbuf_len (for all techs) to SW to use
  in reading out the results.
 
  L1 SW does not need to program any HW, it just needs to read out results from
  page 1 by setting the correct page
  To set page: HWIO_OUTI(MEM_PL_MEM_CHm_PAGE_NUM,1,offset>>14)
      To read: header = HWIO_IN(MEM_POOL_MEM_PAGE1_START+(offset&0x3FFF));
 
  A2 SW has a separate interface of its own and hence needs to have a separate
  VBUF length for every tech

*/

/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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
03/24/14   BW      Updated DECOB vbuf length
06/10/12   NB      Initial DECOB interface 
 
===========================================================================*/

#ifndef FW_DECOB_INTF_H
#define FW_DECOB_INTF_H

/*! @brief Mempool bridge ID to be used by SW */
#define FW_DECOB_OUT_MEMPL_BRDG_ID          1

/*! @brief Init bank in mempool to start writing results */
#define FW_DECOB_OUT_MEMPL_INIT_BANK        0

/*! @brief Min bank in mempool to start writing results */
#define FW_DECOB_OUT_MEMPL_MIN_BANK         0

/*! @brief Max bank in mempool to start writing results */
#define FW_DECOB_OUT_MEMPL_MAX_BANK         7

/*! @brief Start line in mempool for start of DECOB results */
#define FW_DECOB_OUT_MEMPL_START_LINE       1182

/* The VBUF length in words for all techs
*/ 

/*! @brief VBUF length for LTE */
#define FW_DECOB_OUT_MEMPL_VBUF_LEN_WORDS_LTE     (9856)

/*! @brief VBUF length for WCDMA */
#define FW_DECOB_OUT_MEMPL_VBUF_LEN_WORDS_WCDMA  (11840)

/*! @brief VBUF length for DO/HDR */
#define FW_DECOB_OUT_MEMPL_VBUF_LEN_WORDS_DO      (6528)

/*! @brief VBUF length for TDSCDMA */
#define FW_DECOB_OUT_MEMPL_VBUF_LEN_WORDS_TDSCDMA (2298)


#endif /* FW_DECOB_INTF_H */
