/*!
  @file
  tfw_sw_intf_debug.h

  @brief
  TD-SCDMA debug FW-SW Interface Definitions 

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

$Header: //source/qcom/qct/modem/fw/api/tdscdma/rel/01.00/tfw_sw_intf_debug.h#1 

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef __TFW_SW_INTF_DEBUG_H
#define __TFW_SW_INTF_DEBUG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "msgr.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/

/**
    \defgroup cmdDebugUL Debug commands  
    */
/*\{*/

/*\}*/

/* The following is a structure used for system CSIM only */
typedef struct
{
  /* up to 3 TFC entries w/ one for each cctrch. 
              TFCI index needs to be different for different cctrch */
  /* populate 1st one in entry ( tfcInfoBase+ cctrch[0].tfciIndex )*/
  /* populate 2nd one in entry ( tfcInfoBase + cctrch[1].tfciIndex )*/
  /* populate 3rd one in entry ( tfcInfoBase + cctrch[2].tfciIndex )*/

  /* numTFC here means number of valid cctrch in this structure
   and tfcInfoBase measn starting entry in tfc pool table to be
   populated. No gap between cctrches  */
  uint32            numTFC;
  uint32            tfcInfoBase;
  struct
  {
    tfw_tfc_pool_t    tfcEntry;
    uint32            tfciIndex;
  } cctrch[3];

  /* up to 8 TF. Populate TF pool entries from (tfInfoBase+0) to (tfInfoBase + totalNumTF-1) */
  /* totalNumTF means total number of TFs in up to 3 cctrch. tfInfoBase means the
  1st entry in TF pool to be populated. No gap between cctrches */
  uint32            totalNumTF;
  uint32            tfInfoBase;
  tfw_tf_pool_t     tfEntry[8];

  /* up to 8 Trch. Populate TRCH entry from (trchInfoBase+0) to(trchInfoBase+numTrCH -1) */
  /* numTrCH measn total number of Trches in up to 3 cctrch. trchInfoBase means the
  1st entry in Trch info pool to populate. No gap betweem cctrcges */
  uint32            numTrCH;
  uint32            trchInfoBase;
  tfw_trch_config_t trchEntry[8];
} tfw_demback_system_info_t ;

/* FW Debug Command Structure. SW to ignore */
typedef struct
{
  /* Message header */
  msgr_hdr_struct_type      hdr;
  /* This flag contains bits that control the debug behavior of DL
  set to Bit[0] = 1 so that DL can go to normal mode w/o blind PCCPCH. It
  should be sent after tfw state has transitioned to Tracking */
  uint32                    dlDebugFlag;
  /* Expandable to other modules */
  /* genie TFCI that will be used to overwrite decoded TFCI when Bit[4] in
  dlDebugFlag is set. genieTfci[n] is for cctrch[n], n=0..2 */
  uint32                    genieTfci[3];
} tfw_debug_config_cmd_t ;

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR HS-SICH setup command                         */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdDebugUL DEBUG_HSSICH_CONFIG Command
   \ingroup cmdDebugUL
  @verbatim
   Usage:
     1) This command shall only be sent when FW is in TRACKING state.
   @endverbatim 
*/
typedef struct {
  /*! \brief Subframe number where HS-SICH needs to be scheduled. 
    Valid [0..8191] */
  uint16          subframeNum;
  /*! \brief HS-SICH TX power, includes compensation for pathloss, if any. 
  Valid <TBD> [-50dBm .. 33dBm] in 1/16 dBm step */
  int16           hssichTxPwr;
  /*! \brief Absolute Midamble Shift to be used for HS-SICH. 
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] */
  uint16          midambleShift : 8; 
  /*! \brief OVSF code number. valid: [0..sf-1]*/
  uint16          chanCode : 8;
  /** Spreading factor for HS-SICH. */
  /** Valid value : 8/16  */
  uint8           sf : 5;
  /*! \brief Time Slot number where HS-SICH needs to be scheduled. 
   Valid [1 to 5] */
  uint8           timeSlot : 3;
  /*! \brief Recommeded transport block size. */
  uint8           rtbs : 6;
  /*! \brief Recommended modulation format. */ 
  uint8           rmf : 1;
  /*! \brief ACK/NACK bit. */
  uint8           ackNack : 1;
  /*! \brief number of TPC/SS bits. valid: 2/4 */
  uint8           numSsTpc;
  /*! \brief TPC to be transmitted on HS-SICH. */
  uint8           tpc : 4;
  /*! \brief SS to be transmitted on HS-SICH. */ 
  uint8           ss : 4;

  /*! \brief Power difference between Ack and Nack (in 1 dB), valid : -7..8 dB */
  int16           ackNakPwrOffset;      
  /*! \brief Closed or open loop decision. 1: use Closed loop TX power Control, 0 : use open loop */
  uint8           useClosedLoop;
  /*! \brief valid only if useClosedLoop = 1, i.e. closed loop.
  If updatePathLossInClosedLoop is set, i.e ,1: Perform compensation from pathloss update 
  0: otherwise */
  uint8           updatePathLossInClosedLoop;
  /*! \brief TPC for UL HS-SICH power control. Valid [-1, 0, 1] for
  down, hold and up. Check!! */
  int8            dlTpc;
  /*! \brief SS for HS-SICH. Valid range [-1,0,1] */
  int8            dlSS;
  /*! \brief HS-SICH closed loop TPC step size. Valid : 1,2,3 (dB) */
  uint8           tpcStep;
} tfw_hssich_cfg_t;

/*! @brief DEBUG_HSSICH_CONFIG Command
 *
 *   This is the message definition for DEBUG_HSSICH_CONFIG Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief HS-SICH config */
  tfw_hssich_cfg_t          cfg;
} tfw_debug_hssich_config_cmd_t;
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR UL TPC and SS config                          */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdDebugUL DEBUG_UL_TPC_SS_CONFIG Command
   \ingroup cmdDebugUL
  @verbatim
   Usage:
     1) This command shall only be sent when FW is in TRACKING state.
   @endverbatim 
*/
typedef struct {
  /*! \brief UL TPC */
  uint32          tpc;
  /*! \brief UL SS */
  uint32          ss;
  /*! \brief number of TPC/SS bits */
  uint8           numSsTpc;
  /*! \brief OVSF code number. Valid: [0..sf-1]*/
  uint8           chanCode;
  /*! \brief Spreading factor. Valid: [1,2,4,8,16]  */
  uint8           sf;
  /*! \brief CCTrCH ID. Valid: 0/1 */
  uint8           cctrchId;
} tfw_tpc_ss_cfg_t;

/*! @brief DEBUG_UL_TPC_SS_CONFIG Command
 *
 *   This is the message definition for DEBUG_UL_TPC_SS_CONFIG Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief HS-SICH config */
  tfw_tpc_ss_cfg_t          cfg[TFW_MAX_UL_SLOT][TFW_MAX_UL_PHY_PER_SLOT];
} tfw_debug_ul_tpc_ss_config_cmd_t;
/*@}*/

#endif /* __TFW_SW_INTF_DEBUG_H */
