/*!
  @file
  tfw_sw_intf_common.h

  @brief
  TD-SCDMA common FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef TFW_SW_INTF_COMMON_H
#define TFW_SW_INTF_COMMON_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_const.h"
#include "msgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/**
    \defgroup cmdCommon Common Configuration Commands & Responses
    */
/*\{*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR CELL_SERVING_JDCS COMMAND                     */
  /*-----------------------------------------------------------------------*/

/*\}*/

/**
   \defgroup cmdCommonRxD RxD Command
   \ingroup cmdCommon
   @verbatim
   @endverbatim 
*/
/*@{*/
/*! \brief Antenna Selection */ 
typedef enum {
   /*! \brief Use primary antenna only */ 
  TFW_ANT_0,
   /*! \brief Use secondary/diversity antenna only
    @verbatim
       This particular antenna selection is strictly for testing purpose only. In this mode of selection,
       both antennas are actually enabled. But FW discards the IQs received on the primary one.
    @endverbatim
  */ 
  TFW_ANT_1,
   /*! \brief Use both antennas */ 
  TFW_ANT_BOTH
} tfw_ant_sel_e;
  /*! \brief sawless lna mode
  @verbatim
  SAWLESS_INVALID: saw mode
  SAWLESS_HL: sawless lna high linearity mode
  SAWLESS_LL: sawless lna low linearity mode
  @endverbatim 
  */
typedef enum
{
  SAWLESS_LL,
  SAWLESS_HL,
  SAWLESS_INVALID
} tfw_lna_linearity_e;

typedef enum{
  TFW_LNA_G0 = 0x0,
  TFW_LNA_G1 = 0x1,
  TFW_LNA_G2 = 0x2,
  TFW_LNA_G3 = 0x3
} tfw_lna_state_e;

typedef struct {
  tfw_lna_state_e lnaState;
  /*! \brief DVGA compensation (G(-1)) status
      @verbatim
      0: OFF
      1: ON
      @endverbatim 
  */
  boolean dvgaCompensation;
  tfw_lna_state_e lnaStateDiv;
  boolean dvgaCompensationDiv;
//#ifdef __TFW_FEATURE_SAWLESS	
  boolean isSawless;
  /*! \brief sawless lna mode
  @verbatim
  SAWLESS_INVALID: saw mode
  SAWLESS_HL: sawless lna high linearity mode
  SAWLESS_LL: sawless lna low linearity mode
  @endverbatim 
  */
  tfw_lna_linearity_e lnaLinearity;
  /* Reserved. This is needed for D128 Log packet */
  uint32 :16;
//#endif	
} tfw_rx_lna_t;

typedef enum {
  TFW_IRAT_COMMAND_ACTION_NONE = 0,
  TFW_IRAT_COMMAND_ACTION_COLLECT_PROCESS_SAMPLES,
  TFW_IRAT_COMMAND_ACTION_COLLECT_SAMPLES,
  TFW_IRAT_COMMAND_ACTION_PROCESS_SAMPLES
} tfw_irat_cmd_action_t;

typedef struct{
  /*! \brief DC settling value to be passed from Step0 to Step1/2 */     
  struct 
  {
    /*! @brief To load EST1 I DC estimate value */
    uint32 est1_load_i;
    /*! @brief To load EST1 Q DC estimate value */
    uint32 est1_load_q;
    /*! @brief To load EST2 I DC estimate value */
    uint32 est2_load_i;
    /*! @brief To load EST2 Q DC estimate value */
    uint32 est2_load_q;
  }rxlmWbdcEst;
  /*! \brief To kickstart HW in settled state due to lack of time budget to allow settling */     
  uint32  hwinitDvgaBias; 
  /*! \brief Scaling used for converting samples in mempool from 20pcfl to linear format in lmem */
  uint32  dvgaBias;
  /*! \brief  Step1/2 sample activation time */
  uint32  activateTime;
  /*! \brief  LNA offset computed for rscp meas samples*/
  uint32  lnaOffset;
  /*! \brief Specifies the buffer - Ping or Pong - for processing measurement samples */
  uint8 bufferIndex; 
  /*! \brief 6 dB boost */
  uint8 vpeBiasFlag;
}tfw_irat_state_t;

/*@}*/    
#endif /* TFW_SW_INTF_COMMON_H */
