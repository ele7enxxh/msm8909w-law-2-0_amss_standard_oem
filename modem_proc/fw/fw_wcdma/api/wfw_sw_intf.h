/*============================================================================*/
/*!
  @file

  @brief
  WCDMA FW and SW interface definition
*/
/*============================================================================*/

/*===========================================================================

  Copyright (c) 2007-2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_sw_intf.h#1 $
$DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
08/18/14   SK      Enable DR-DSDS support.
03/08/13   EM      Add TxDet done cmd support
07/05/12   MM      Offline PICH feature added.
07/02/12   TL      added RF intf support
05/24/12   SS      Added HS-RACH feature
03/29/12   RP      added ARD V2 support
03/27/12   YL      added EDRX support
10/20/11   RP      update interface for ARD feature
10/19/11   RP      add ARD proc done flag
10/07/11   RP      support ARD feature
04/28/11   YL      logging interface for QICE and cleanup
01/07/10   HK      Add DRX interfaces. 
10/15/10   JC      Add QICE Debug and FFInv logging buffers. Remove ovsolete QICE niterface. 
09/29/10   JP      Enable different Tx Mode from Serving HS DSCH cell support
06/21/10   HK      Add EWC logging buffers 
02/08/10   BL      Remove debugInfo from SW intf structure
01/28/10   BL      Strip out debugInfo by #ifdef FEATURE_FW_STACK_PROF
01/26/10   BL      Add debugInfo, WFW_SW_TCM_READY_FOR_CMD for stack profiling
01/14/10   NC      Reenable pessimistic cqi interface for secondary carrier
11/23/09   WB      Add WFW_SW_AGC_TUNING_DONE_FLAG to indicate to SW cmd done
11/21/09   JP      Extend pessimistic cqi struct for secondary carrier
10/01/09   BL      Add intf for HS-SCCH Order indication to SW
09/23/09   BL      BLAST2.0.1: SHARED MEM ADDR change from 3f00000 to 3b10000
09/02/09   BL      dbackHsDecodeStatusLog buffer indexing back to GSFN%16 method
09/01/09   DG      SWI changes for QICE F/Finv logging
07/31/09   NB      Remove CM gap start / end interface
07/29/09   JP      Added demback async log buffers for A&E and MIMO logging
                   dbackHsAeLogAsyncReadBuf and dbackHsMimoDemodLogAsyncReadBuf
07/16/09   BL      Apply new DC interface:
                   hsInfoTable duplicated
                   dbackHsDecodeStatusLog[] is replaced with dbackHsDecodeAsyncReadBuf
                   Add dbackHsDecStatLogRdPtr
05/28/09   NC      Move SW-FW interface to RUMI temp location 0x03F00000
05/20/09   NC      Merge with Taxis CS2 SWI
05/13/09   DG      Add QICE SFC logging
05/04/09   HK      Added Latency and Bus profiling
03/30/09   EM      Split SWI to LNA processing and dual carrier process
03/27/09   WM      Add QICE async interface
02/06/09   NB      Add CM gap start end responses for SW
01/27/09   PK      MIMO related logging
01/23/09   PK      Added QPST Crash dump support
12/17/08   GS      Changes to support MDSP Csim
12/08/08   GS      Added debug command def
11/25/08   GS      Updated shared buffer start address
11/14/08   NB      Update interface for CM/ IRAT
11/11/08   NC      Fix warning message
10/23/08   YL      Added SCHIC info logging
10/10/08   RK      Changed the name of the EUL DL done to EUL RSLT done flag
08/25/08   WX      Add exported definitions for Sw/Fw sync mechanism
06/06/08   NC      Added misc async interface
05/22/08   PK      Added SW-FW misc async params struct
05/01/08   WX      Add RxAGC async write buffer interface
04/30/08   RK      interchanged WFW_SW_EUL_DL_DONE_FLAG & NUM FLAGS in enum declaration
04/28/08   RK      Added EUL DL result inerrupt flag definition 
04/22/08   ML      Updated HS and MIMO logging
03/12/08   ML      Added Mimo logging
11/08/07   NC      Added HDET done flag
10/19/07   NC      Added RACH done flag
10/15/07   NC      Added HS CM control table and HS decode log struct
08/10/07   NC      Added TX configuration interfaces
08/08/07   GS      Added HS info table
08/01/07   GS      Added PICH/MICH related interfaces
07/01/07   GS      Added demod status dump to mDSP FW/SW interface
===========================================================================*/

#ifndef WFW_SW_INTF_H
#define WFW_SW_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "wfw_sw_cmd_proc_intf.h"
#include "wfw_demod_intf.h"
#include "wfw_srch_intf.h"
#include "wfw_rxagc_intf.h"
#include "wfw_mod_intf.h"
#include "wfw_misc_intf.h"
#include "wfw_drx_intf.h"
#include "modem_fw_memmap.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */

/* @brief SW-FW shared memory start address */
#define MDSP_SHARED_MEM_ADDR_BASE FW_SMEM_WCDMA_ADDR
#define MDSP_SHARED_MEM_ADDR_SIZE FW_SMEM_WCDMA_SIZE

/* @brief SW-FW command interface ready and ack flag values */
#define WFW_SW_READY_FOR_CMD     0x6789ABCD
#define WFW_SW_READY_FOR_CMD_ACK 0xABCD6789

/* @brief Number of debug cmd W32 entries from SW */
#define WFW_SW_DEBUG_CMD_NUM_W32 64

/* @brief size of RF taskbuf in 32-bit words */
#define WFW_RF_TASK_BUF_SIZE        (512+128)

/*! @brief Various command done flags */
typedef enum
{
  WFW_SW_READY_FOR_CMD_FLAG,
  WFW_SW_COMMAND_DONE_FLAG,
  WFW_SW_SEARCH_DONE_FLAG,
  WFW_SW_OVERLAPPED_SRCH_DONE_FLAG,
  WFW_SW_CM_SEARCH_DONE_FLAG,
  WFW_SW_STATUS_DUMP_DONE_FLAG,
  /* demod config related flags */
  WFW_SW_DEMOD_CFG_DONE_FLAG,
  WFW_SW_DEMOD_R99_CHAN_CFG_DONE_FLAG,
  WFW_SW_DEMOD_EDL_CHAN_CFG_DONE_FLAG,
  WFW_SW_DEMOD_HS_CHAN_CFG_DONE_FLAG,
  WFW_SW_CME_DEMOD_CFG_DONE_FLAG,
  WFW_SW_DEMOD_PICH_DONE_FLAG,
  WFW_SW_DEMOD_QXT_DONE_FLAG,

  WFW_SW_NONHS_DEC_DONE_FLAG,
  WFW_SW_HS_DEC_DONE_FLAG,
  WFW_SW_HS_SCCH_ORDER_FLAG, /* HS-SCCH Order indication to SW */
  /* HS A&E and MIMO LOG to indicate to SW */
  WFW_SW_HS_LOG_IND_FLAG,
  WFW_SW_RACH_DONE_FLAG,
  WFW_SW_HDET_CONV_DONE_FLAG,
  WFW_SW_EUL_DL_RSLT_DONE_FLAG,
  WFW_SW_X2W_RF_ON_FLAG,
  WFW_SW_G2W_RF_ON_FLAG = WFW_SW_X2W_RF_ON_FLAG,
  WFW_SW_X2W_SAMPLE_RAM_DONE_FLAG,
  WFW_SW_G2W_SAMPLE_RAM_DONE_FLAG = WFW_SW_X2W_SAMPLE_RAM_DONE_FLAG,
  WFW_SW_CM_GAP_START_FLAG,
  WFW_SW_CM_GAP_END_FLAG,
  /* Use this to indicate to SW that we are done preparing for crash */
  WFW_SW_MDSP_CRASH_PREP_DONE_FLAG,
  /* Use this to indicate to SW that we are beginning preparing for crash */
  WFW_SW_MDSP_CRASH_IND_FLAG,
  /* Use this to indicate to SW that AGC tuning is done */
  WFW_SW_AGC_TUNING_DONE_FLAG,
  /* Use this to indicate to SW AGC command processing for some chain(s) is complete. 
     rxagcCmdProcPendingBmsk should be polled to see which chains are done. */
  WFW_SW_AGC_CFG_DONE_FLAG,
  /* HS-SCCH Decode Done  */
  WFW_SW_HS_SCCH_DEC_DONE_FLAG,
  /* Indicated F-DPCH enable done in HS-RACH state*/
  WFW_SW_HS_RACH_FDPCH_SETUP_DONE_FLAG,
  /* Use this to indicate to SW that UL DPCCH DISABLE cmd have been applied */
  WFW_SW_UL_DPCCH_DISABLE_DONE_FLAG,
  /* E-AGCH Decode Done  */
  WFW_SW_EAGCH_RSLT_DONE_FLAG,
  /* Sample server freeze/unfreeze done */
  WFW_SW_SS_FREEZE_UNFREEZE_DONE_FLAG,
  /* TxD determination done indicator CMD DONE ID 27*/
  WFW_SW_TXDET_DONE_FLAG,
  /* make request to SW to adjust MCVS for shadow cell */
  WFW_SW_SHADOW_CELL_REQUEST_FLAG,
  /*QTA command done flag*/
  WFW_SW_QTA_CMD_DONE_FLAG,
  /* Channel ID update done flag */
  WFW_SW_COEX_CHANNELID_UPDATE_DONE_FLAG,
  /*FreqId command done flag*/
  WFW_SW_COEX_FREQID_UPDATE_DONE_FLAG,
  /*priority table command done flag*/
  WFW_SW_COEX_PRIORITY_TABLE_UPDATE_DONE_FLAG,
  /* Indication for RF to start retune for DC-HSUPA */
  WFW_SW_DCUPA_RF_RETUNE_FLAG,
  /* Antenna switch done */
  WFW_SW_ANTENNA_SWITCH_DONE_FLAG,
  /*! signal SW that NBR is done, result has been written to AsyncRead buf, and G rlxm chain has been deallocated */
  WFW_SW_NBR_DONE_FLAG,
  /* Rx re-activation due to HS-SCCH order */
  WFW_SW_RX_RESTART_FLAG,
  /* New CCTrCh tables have been latched, and demback is idle */
  WFW_SW_CCTRCH_UPDATE_DONE_FLAG,
  /* CPC DTX config done */
  WFW_SW_CPC_DTX_CFG_DONE_FLAG,
  /* CPC DRX config done */
  WFW_SW_CPC_DRX_CFG_DONE_FLAG,
  /* EUL config done flag */  
  WFW_SW_DEMOD_EUL_CHAN_CFG_DONE_FLAG,
  /* HS-DPCCH config done flag */  
  WFW_SW_DEMOD_HS_DPCCH_CHAN_CFG_DONE_FLAG,
  /*CPC DRX power optimization - Readiness for Demback/Searcher HW Power Up/Collapse*/
  WFW_SW_CPC_DRX_POWER_UP_FLAG,
  WFW_SW_CPC_DRX_POWER_DOWN_FLAG,
  /* Flag to indicate Error scenario in FW.*/
  WFW_SW_FATAL_ERROR_RECOVERY,
  WFW_SW_NUM_FLAGS
} __attribute__ ((packed)) WfwSWCmdDoneFlagsEnum;

/* Structure definitions */
/* --------------------- */

/*! @brief WFW debug command */
typedef struct
{
  UWord32 active;
  UWord32 info[WFW_SW_DEBUG_CMD_NUM_W32];
} WfwSwDebugCmdStruct;

/*! @brief WCDMA FW-SW interface buffer */
typedef struct
{
  /*! @brief Size of this structure 
   
      WFW populates this with sizeof(WfwSwIntfStruct). VP and L1 SW should 
      ASSERT if it does not equal sizeof(WfwSwIntfStruct). Mismatches will occur
      if non-portable types are used (e.g. UWord32 *, enum), or if the WFW
      image does not match the API header files used. */
  UWord32 size;

  /* mDSP SW sync interface - RW area 1 */
  /**************************************/

  /*! @brief mDSP sync interface Cmd response buffer */
  WfwSWCmdRspStruct syncBuf;

  /* Padding to move past current cache line */
  UWord8 padding1[32];

  /* mDSP SW interface - SW WO only area */
  /***************************************/

  /* Searcher interface */
  /* ------------------ */

  WfwSrchLpqHpqCmdStruct        srchLpqHpqCmdBuf[WFW_NUM_INTRA_SRCH_QUEUES];
  WfwCmSrchqCmdStruct           cmSrchqCmdBuf;
  WfwSrchAsyncWriteStruct       srchAsyncWriteBuf;

  /* RF related interface */
  /* -------------------- */

  /* RxAGC interface */
  WfwRxAgcAsyncWrStruct rxagcAsyncWriteBuf[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_CARRIERS];

  /* RxAGC interface for LNA */
  WfwRxAgcLnaAsyncWrStruct rxagcLnaAsyncWriteBuf[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_LNAS_PER_ANTENNA];

  /*! NBR tunable parameters - should be populated before enabling NBR in RX_AGC start cmd */
  WfwNbrAsyncWrStruct nbrAsyncWriteBuf;

  /*! @brief Structure definition for async CM Rf control buffer write interface */
  WfwCmRFControlInfoAsyncWriteStruct cmRfControlInfoWriteBuf;

  /*! @brief DRX Async Write interface  */
  WfwCpcDrxAsyncWriteStruct drxAsyncWriteBuf;
  /*! @brief eDRX Async Write interface  */
  WfwEdrxAsyncWriteStruct   eDrxAsyncWriteBuf; 

  /* Demodfront interface */
  /* -------------------- */

  /* IRPM timeline control structure */
  WfwDemodIrpmAsyncWriteStruct demodIrpmCtrl[WFW_DEMOD_NUM_MAX_CARRIERS];

  /* VLC timeline control structure */
  WfwDemodVlcAsyncWriteStruct  demodVlcCtrl;

  /* Ewc debug Log packet control structure */
  WfwEwcDebugAsyncWriteStruct ewcDebugLogCtrlPtr;

  /* QICE Debug Log packet control structure */
  WfwQiceDebugAsyncWriteStruct qiceDebugLogCtrlPtr;

  /* rdPtr for SCHIC logging*/
  UWord8 schicInfoLogRdPtr;


  /*! @brief DL cell, finger, channel, combiner database
      Caution - This has SW WO followed by SW RO section and has padding. Need cleanup */
  WfwDemodInfoInterfaceStruct demodInfo;

  WfwTcResultsRlpInfoStruct swFwRlpInfoIntf;

  /* Demback interface */
  /* ----------------- */

  /* CCTrCh/TFC, MBMS table, HS info table */
  WfwDembackNonHsIntfStruct dbackNonHsIntf;
  /* HSDPA info tables */
  WfwDembackHsInfoTableStruct hsInfoTable[WFW_DEMOD_NUM_MAX_CARRIERS];
  UWord8 dbackHsAeLogRdPtr;
  UWord8 dbackHsMimoDemodRdPtr;
  /* HSDPA CM Control Table */
  WfwDemodHsCmCtrlTableStruct hsCmCtrlTable[WFW_DEMOD_HS_CM_CTRL_TABLE_NUM_BUF];
  /* NonHS Dob Status Fifo read index */
  UWord16 nonHsDobStatFifoReadIndex;
  /* HS Dob Status Fifo read index */
  UWord16 hsDobStatFifoReadIndex;
  /* HS SCCH Order Fifo read index */
  UWord16 hsScchOrderFifoReadIndex;

  /* Modulator/Encoder interface */
  /* --------------------------- */

  /*! @brief Structure definition for async TX SW-FW write interface - READ/WRITE BY L1 */
  WfwTxAsyncWriteStruct         txAsyncWriteBuf;

  /*! @brief Structure definition for misc async SW-FW write interface - READ/WRITE BY L1 */
  WfwMiscAsyncWriteStruct       miscAsyncWrBuf;

  /* Misc debug interface */
  /* ---------------------*/

  /*! Misc interface related to MIMO params */
  /* Enable different Tx Mode from Serving HS DSCH cell support
     FEATURE_WFW_DC_MODE_DIFF_TX_MODE */
  WfwMiscAsyncStruct miscAsyncBuf[WFW_DEMOD_NUM_MAX_CARRIERS]; 

  /* ASYNC struct mapping EDCH resource index to channel parameters.
  Populated by SW for use in HS RACH */
  WfwEdchRsrcChanMappingStruct edchRsrcChanMap;

  /* Padding to move past current cache line */
  UWord8 padding2[32];

  /* mDSP SW interface - SW RO only area */
  /***************************************/

  /* Searcher interface */
  /* ------------------ */

  WfwSrchLpqHpqRsltStruct       srchLpqHpqRsltBuf[WFW_NUM_INTRA_SRCH_QUEUES];
  WfwCmInterratRsltUnion        cmInterratRsltUnion;
  WfwSrchAsyncReadStruct        srchAsyncReadBuf;

  /* RxAGC interface */
  WfwRxAgcAsyncRdStruct rxagcAsyncReadBuf[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_CARRIERS];

  /* RxAGC interface for AGC cmd processing. Indicates if SW issued RxAgc cmd processing is pending (1)
     or done (0) for each ant/car combination.  */
  WfwRxAgcCmdProcBmskStruct rxagcCmdProcPendingBmsk;

  /* RxAGC interface for LNA */
  WfwRxAgcLnaAsyncRdStruct rxagcLnaAsyncReadBuf[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_LNAS_PER_ANTENNA];

  /*! NBR result(s) */
  WfwNbrAsyncRdStruct nbrAsyncReadBuf;

  /*! Sample server information */
  WfwSSAsyncRdStruct ssAsyncReadBuf;

  /* Demodfront interface */
  /* -------------------- */
  /*! @brief Channel taken action time */
  WfwDemodChannelTakenActionTimeStruct        demodChannelTakenActionTime;

  /*! @brief EWC logging packet */
  WfwEwcDebugAsyncReadStruct                  ewcDebugAsyncReadBuf;

  /*! @brief QICE Debug logging packet */
  WfwQiceDebugAsyncReadStruct                 qiceDebugAsyncReadBuf;

  /*! @brief ARD logging packet */
  WfwQiceArdV2AsyncReadStruct                 ardV2AsyncReadBuf;

  /*! @brief ARD logging packet */
  WfwQiceArdAsyncReadStruct                   ardAsyncReadBuf;
  
  /*! @brief DRX log packet */
  WfwCpcDrxLogAsyncReadStruct                 drxAsyncReadBuf;
  /*! @brief eDRX log packet */
  WfwEdrxLogAsyncReadStruct                   eDrxAsyncReadBuf; 
  /*! @brief SCHIC logging packet */
  WfwSchicAsyncReadStruct                     schicAsyncReadBuf;

  /*! @brief Demod status dump */
  WfwDemodStatusDumpStruct demodStatusDump;

  /*! @brief Channel analysis packet */
  WfwChannelAnalysisStruct chanAnalysis[WFW_DEMOD_HS_CHAN_ANALYSIS_PACKET_BUF_SIZE];

  /*! @brief Pessimistic CQI packet */
  WfwHsPessCqiLogStruct pessCqi[WFW_DEMOD_HS_PESSIMISTIC_CQI_PACKET_BUF_SIZE][WFW_DEMOD_NUM_MAX_CARRIERS];

  /* Demback interface */
  /* ----------------- */

  WfwDembackNonHsDecobStatusFifoStruct dbackNonHSDecobStatusFifo;
  WfwDembackHsDecobStatusFifoStruct    dbackHSDecobStatusFifo;
  WfwDbackHsAeLogAsyncReadStruct         dbackHsAeLogAsyncReadBuf;
  WfwDbackHsMimoDemodLogAsyncReadStruct  dbackHsMimoDemodLogAsyncReadBuf;
  WfwDbackHsDecodeStatusLogFifoStruct      dbackHsDecodeStatusLog;
  WfwDbackHsScchOrderFifoStruct         dbackHsScchOrderFifo;

  WfwTfciLogStruct                     dbackTfciLog[WFW_DEMOD_NUM_INFO_BUFS][WFW_DBACK_NUM_CCTRCH_TABLES];
  UWord8                               dbackDecodedTfci[WFW_DBACK_NUM_CCTRCH_TABLES];

  /* Modulator interface */
  /* ------------------- */

  /*! @brief Structure definition for async TX SW-FW read interface - READ ONLY BY L1 */
  WfwTxAsyncReadStruct          txAsyncReadBuf;

  /*! @brief Structure definition for async HS-RACH SW-FW read interface - READ ONLY BY L1 */
  WfwHsRachChanTimingLogStruct  hsRachChanTimingLog;

  /*! @brief Structure definition for misc async SW-FW read interface - READ ONLY BY L1 */
  WfwMiscAsyncReadStruct        miscAsyncRdBuf;


  /* mDSP SW interface flags counters */
  /* -------------------------------- */

  /*! @brief mDSP Reason for crash when FW indicates to SW */
  UWord32 crashReason;

  /*! @brief mDSP various prc done flags counters */
  UWord32 swCmdDoneFlagCounter[WFW_SW_NUM_FLAGS];

  /* Padding to move past current cache line */
  UWord8 padding3[32];

  /* mDSP SW interface - Debug Cmd area */
  /**************************************/

  /* WFW debug command */
  WfwSwDebugCmdStruct debugCmd;

  /* storage for RF scripts */
  UWord32 rfTaskBuf[WFW_RF_TASK_BUF_SIZE];

  WfwCoexMgrLogAsyncReadStruct cxmLogAsyncReadBuf;
  
  WfwAgcPowerLogAsyncReadStruct agcPwrLogAsyncReadBuf;

  WfwCoexMgrParamsAsyncWriteStruct cxmParamsAsyncWriteBuf;

  UWord16 swErrorRecoveryReason;

} WfwSwIntfStruct;

/*===========================================================================

                    EXTERNAL VARIBLE DECLARATION

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_SW_INTF_H */

