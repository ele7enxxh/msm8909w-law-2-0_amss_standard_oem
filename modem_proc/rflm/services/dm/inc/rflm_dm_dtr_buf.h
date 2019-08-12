/*!
  @file
  rflm_dm_dtr_buf.h

  @brief
  The header file defines the dtr buffer pool structure, for both rx and tx.

  @detail
  OPTIONAL detailed description of this C header file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_dtr_buf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/14   dr      Payload pointer datatype change
07/30/14   zhw     Add cplusplus wrap around to export DTR counts to C file
02/13/14   cvd     Increase number of DTR-Tx handles to 4, total DTR handles to 18
10/18/13   qwu     Changed char to uint32
09/06/13   JYW     Added init support
08/08/13   vrb     Removing dependence on rfmodem_xyz
08/08/13   vrb     Increase the number of RFLM handles to 16 (11 Rx, 5 Tx)
07/30/13   jyw     Created the initial version

===========================================================================*/

#ifndef RFLM_DM_DTR_H
#define RFLM_DM_DTR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus

#include "rflm_dm_api.h"
#include "rflm_dm_buf_mgr_t.h"
#include "rflm_dtr_rx_struct_ag.h"
#include "rflm_dtr_tx_struct_ag.h"
#endif /* __cplusplus */


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#ifdef __cplusplus
/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*!< point to the memory location could either inside or outside of the FED */
  rflm_dtr_rx_settings_type_ag payload_ptr;

  /*!< flag to indicate if the buffer has been populated. */
  boolean populated_flag;

  /*!< barrier */
  uint32 barrier_pattern;
  /*!@todo RXLM structue */
} rflm_dm_dtr_rx_buf_s; 



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*!< point to the memory location could either inside or outside of the FED */
  rflm_dtr_tx_settings_type_ag payload_ptr;

  int32 device_id;

  /*!< flag to indicate if the buffer has been populated. */
  boolean populated_flag;

  /*!< barrier */
  uint32 barrier_pattern;

  /*!@todo RXLM structue */
} rflm_dm_dtr_tx_buf_s; 


/*! @brief DTRRxBuf class is based on the BufMgrTemplate */
class DtrRxBuf: private MyBufMgrTemplate<rflm_dm_dtr_rx_buf_s, RFLM_DM_DTR_RX_BUF_COUNTS>
{
public:
  DtrRxBuf() {init();}
  void preset_buf(void);
  uint32* get_new_buf(void);
  boolean delete_buf(uint32* buf_ptr);
  boolean validate_buf_ptr(uint32* buf_ptr);
  uint32 get_num_free_buf(void);
  boolean get_populated_flag(uint32 idx);
  void set_populated_flag(uint32 idx, boolean flag);
};

/*! @brief DTRTxBuf class is based on the BufMgrTemplate */
class DtrTxBuf: private MyBufMgrTemplate<rflm_dm_dtr_tx_buf_s, RFLM_DM_DTR_TX_BUF_COUNTS>
{
public:
  DtrTxBuf() {init();}
  void preset_buf(void);
  uint32* get_new_buf(void);
  boolean delete_buf(uint32* buf_ptr);
  boolean validate_buf_ptr(uint32* buf_ptr);
  uint32 get_num_free_buf(void);
  int32 get_device_id(uint32 idx);
  void set_device_id(uint32 idx, int32 device_id);
  boolean get_populated_flag(uint32 idx);
  void set_populated_flag(uint32 idx, boolean flag);
};

#endif /* __cplusplus */
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* RFLM_DM_DTR_H */
