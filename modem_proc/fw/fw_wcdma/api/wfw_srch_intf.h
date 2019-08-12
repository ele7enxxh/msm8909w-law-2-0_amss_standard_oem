/*!
  @file

  @brief
  WCDMA FW Offline Searcher interface definitions header file
*/

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_srch_intf.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/25/12   EM      Check in Feature frequency binning algorithm interface change
10/27/11   WX      Export CM step1 sample collection for SW for CM usable gap calculation
10/17/11   WB      Fat path support interface changes
02/22/11   WB      Fix mismatch between PN cmd and CM PN cmd buffers 
02/14/11   WB      Fix bitwidth for numTaskFinished for PN search 
03/19/10   NC      Make CM step1 search interface backwards compatible
03/18/10   WB      Interface support for carrierSel bit in CM step1 sync buffer  
05/28/09   NC      Update interface for dual carrier
11/14/08   NB      updated search interface for CM
===========================================================================*/

#ifndef WFW_SRCH_INTF_H
#define WFW_SRCH_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */

/*! @brief numher of compressed mode command queues */
#define WFW_NUM_CM_SRCH_QUEUES        2
/*! @brief number of G2W reconfirmation srch stages */
#define WFW_NUM_G2W_STAGES            2
/*! @brief maximum number of CM list search commands */
#define WFW_CM_MAX_NUM_PN_CMDS       64
/*! @brief maximum number of peaks per CM PN command */
#define WFW_CM_MAX_NUM_PEAKS_PER_TASK  6
/*! @brief number of samples to collect during CM search */
#define WFW_CM_STEP1_NUM_CX1_SAMPLES_TO_COLLECT 2816

#define WFW_MAX_NUM_PN_CMDS         75

/*! @brief number of queues where PN searches can come on. All intra PN search come on QUEUE 1, only IRAT PN searches come on queue 0 */
#define WFW_NUM_INTRA_SRCH_QUEUES    2
#define WFW_INTRA_SRCHQ0             0
#define WFW_INTRA_SRCHQ1             1

/* Structure definitions */
/* --------------------- */

/*! @name Structure definition for demod SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW searcher information update */

typedef struct
{
  /*! 0: No action; 
      1: Step1 search freeze samples  
      2: Nastt search unfrozen samples after it's done; 
      3: PN search unfrozen samples before its execution; 
      4: PN search unfrozen samples after it's done */
  UWord32 srchSSFreezeFlag:3;
  /*! 0: No action; 
      1: Step1 stops RF after step1 search is complete  */
  UWord32 srchStopRFFlag: 1;  
  UWord32 :28;
  /*! 1. For step1, this is the array of frequency hypothesis to be applied to the rotator, 
         the array size is 2 to support up to 2 frequency hypothesis at the same time
      2. For NASTT and PN, if numTcxoFreqHypothesis is set to 1, 
         tcxoFreqHypothesisHw[0] is the frequency error to be applied to rotator */
  Word32 tcxoFreqHypothesisHw[3];
  /*! Enable frequency hypothesis for search if nonzero
      1. For Step1 search, the number indicates how many frequency hypothesis will be tested in step1, maximum is 2
      2. For NASTT and PN search this value can only be either 0 or 1. */
  UWord32 numTcxoFreqHypothesis :2;
  UWord32 :30;
  /*! 0: No action; 
      1: NASTT uses frozen samples in initial acquisition; 
      2: PN uses frozen samples */
  UWord32 srchUseFreezeFlag:2;
  /*! 0: No action; 
      1: Apply step1 scaling/rounding according to RxAGC value; */
  UWord32 srchSclEnable:1;
  UWord32 :29;
  /*! 0: Do not program FreqHypothesis to RxFE rotator 
      1:  Program FreqHGypothesiss toRxFE to RxFE rotator */
  UWord32 programFreqtoRxFE:1;
  UWord32 :31;
  /*! The frequency error that should be applied to RxFE rotator in ACQ search */
  Word32  FreqHypothesistoRxFE; 

} WfwSrchCommonAsyncWriteStruct;


typedef struct
{
  /*! Non-coherent integration length (in units of coherent integration length) 
      for each slot-timing hypothesis in step1 search. */
  UWord32 srchStep1NonCohIntgLen:8;
  UWord32 :24;
  /*! Energy threshold for step1 peaks */
  UWord32 srchStep1EnrgTh:16;
  UWord32 :16;
  /*! 0 to select antenna 0
      1 to select antenna 1, 
      this only applies in non-RxD mode */
  UWord32 srchStep1AntSel:1;
  /*! Bit 1:31 unused */
  UWord32 :31;
  /*! Rx mode for Step1 search, 
      bit 0: RxD (add mode), 
      bit 1: STTD mode, 
      bit 2 RxD (separate mode) 
      Note: step1 should not use STTD mode
      Note: For current FW, RxD separate mode is not supported for Step1 */
  UWord32 srchStep1TaskMode:3;
  UWord32 :29;
  /*! Peak detect mode for step1 
      00: normal, 
      01: Bypass, 
      10: Shoulder
      Shoulder mode is not currently supported in FW for step1 */
  UWord32 srchStep1PdMode:2;
  UWord32 :30;
  /*! Step1 search latest finish time (in frame unit, using frame_cnt) */
  UWord32 srchStep1LateTime:16;
  UWord32 :16;
  /*! Number of noncoherent accumulations to be executed 
      before FW starts to freeze the sample server for later NAS-TT, 
      this variable is only used when srchSSFreezeFlag is set to 1 */
  UWord32 srchStep1NNcohInitAcq:8;
  UWord32 :24;
  /*! Step1 False Alarm reduction enable flag, 
      1: the FA feature is enabled, 
      0: the FA feature is disabled */
  UWord32 srchStep1FAFlag:1;
  UWord32 :31;
  /*! Number of peaks to be returned in step1 */
  UWord32 srchStep1NumPeaks :7;
  UWord32 :25;
} WfwSrchStep1AsyncWriteStruct;

typedef struct
{
  /*! noncoherent integration length */
  UWord32 srchStep2NonCohIntgLen:5;
  UWord32 :27;
  /*! energy threshold */
  UWord32 srchStep2EnrgTh:16;
  UWord32 :16;
  /*! HW done interrupt enable */
  UWord32 srchStep2HwDoneIntEna:1;
  UWord32 :31;
} WfwSrchStep2AsyncWriteStruct;

/* NAS-TT LPQ/HPQ Buffer Structure */
typedef struct
{ 
  /*! the scrambling code used for this NASTT search, 
      there are a maximum of 32 scrambling codes for 
      NAS-TT search in asynchrnous search mode */
  UWord16 scrIndex:9;
  UWord16 :7;
} WfwSrchNASTTScrIndexUnitStruct;

typedef struct
{
  /*! the step1 peak position in chipx8 resolution */         
  UWord16 peakPosLSW:3;
  /*! 0 to select carrier 0
      1 to select carrier 1
      2 to select carrier 2*/
  UWord16 carrierSel:2;
  UWord16 :11; 
  /*! the step1 peak position in chipx1 resolution */
  UWord16 peakPosMSW:16;
} WfwSrchNASTTPNPosUnitStruct;

typedef struct     
{
  /*! 1: NASTT Asynchronous set search 
      0: NASTT unmonitored set search */
  UWord16 asyncNasTT:1;
  UWord16 :15;
  /*! Number of tasks (N<=12) */
  UWord16 numTask:6;
  /*! Number of SC index (K<=32) */
  UWord16 numSCIndex:6;
  UWord16 :4;
  /*! SC code index array */
  WfwSrchNASTTScrIndexUnitStruct srchNASTTScrIndex[32];
  /*! peak position array */
  WfwSrchNASTTPNPosUnitStruct srchNASTTPNPos[64];
} WfwSrchNASTTLpqHpqStruct;


typedef struct
{
  /*! Non-coherent integration length 
      (in units of coherent integration length) 
      in NAS-TT search, should always be an even number 
      since FW will split it into two separate coh/ncoh tasks */
  UWord32 srchNASTTNonCohIntgLen:7;
  UWord32 :25;
  /*! Energy threshold for NAS-TT peaks */
  UWord32 srchNASTTEnrgTh:16;
  UWord32 :16;
  /*! Coherent integration length in NAS-TT, 
      recommended value is 128 or multiple of 512 */
  UWord32 srchNASTTCohIntgLen:14;
  UWord32 :18;
  /*! Number of peaks reported per task for NAS-TT search */
  UWord32 srchNASTTNumResPerTask:3;
  UWord32 :29;
  /*! Rx mode for NAS-TT search, 
      bit 0: RxD (add mode), 
      bit 1: STTD mode, 
      bit 2 RxD (separate mode)
      Note: For current FW, RxD separate mode is not supported for NASTT */
  UWord32 srchNASTTTaskMode:3;
  UWord32 :29;
  /*! 0 to select antenna 0
      1 to select antenna 1, 
      this only applies in non-RxD mode */
  UWord32 srchNASTTAntSel:1;
  UWord32 :31;
  /*! Peak detect mode for NAS-TT, 
      00: normal, 
      01: Bypass, 
      10: Shoulder
      Note: normal mode and shoulder mode is not supported in current FW for NASTT */
  UWord32 srchNASTTPdMode:2;
  UWord32 :30;
  /*! NAS-TT search latest finish time */
  UWord32 srchNASTTLateTime:16;
  UWord32 :16;
} WfwSrchNASTTAsyncWriteStruct; 


typedef struct
{
  /*! coherent integration length */
  UWord32 srchStep3CohIntgLen:6;
  UWord32 :26;
  /*! noncoherent integration length */
  UWord32 srchStep3NonCohIntgLen:8;
  UWord32 :24;
  /*! window size */
  UWord32 srchStep3WindowSize:16;
  UWord32 :16;
  /*! energy threshold */
  UWord32 srchStep3EnrgTh:15;
  UWord32 :17;
  /*! number of results per task */
  UWord32 srchStep3NumResPerTask:3;
  UWord32 :29;
} WfwSrchStep3AsyncWriteStruct;

typedef struct
{
  /*! number of results per task */
  UWord32 srchListLpqNumResPerTask:3;
  UWord32 :29;
  /*! number of results per task on HPQ */
  UWord32 srchListHpqNumResPerTask:3;
  UWord32 :29;
} WfwSrchListAsyncWriteStruct;


typedef struct
{
  /*! Non-coherent integration length 
      (in units of coherent integration length) 
      for each slot-timing hypothesis in CMQ0 step1 search. */
  UWord32 cmq0SrchStep1NonCohIntgLen:8;
  UWord32 :24;
  /*! 0 to select antenna 0
      1 to select antenna 1, 
      this only applies in non-RxD mode */
  UWord32 srchStep1AntSel:1;
  UWord32 :31;
  /*! Rx mode for step1 search in cmq0, 
      bit 0: RxD (add mode), 
      bit 1: STTD mode, 
      bit 2 RxD (separate mode)
      Note: step1 should not use STTD mode
      Note: For current FW, RxD separate mode is not supported for Step1 */
  UWord32 cmq0SrchStep1TaskMode:3;
  UWord32 :29;
  /*! Peak detect mode for step1 
      00: normal, 
      01: Bypass, 
      10: Shoulder
      Shoulder mode is not currently supported in FW for step1
 */
  UWord32 cmq0SrchStep1PdMode:2;
  UWord32 :30;
  /*! Step1 search latest finish time 
      (in frame unit, using frame_cnt) in CMQ0 */
  UWord32 cmq0SrchStep1LateTime :16;
  UWord32 :16;
  /*! Number of peaks to be returned in step1 in CMQ0 */
  UWord32 cmq0SrchStep1NumPeaks :7;
  UWord32 :25;
} WfwSrchCmq0Step1AsyncWriteStruct;

typedef struct
{
  /*! Non-coherent integration length 
      (in units of coherent integration length) 
      for each slot-timing hypothesis in CMQ1 step1 search */
  UWord32 cmq1SrchStep1NonCohIntgLen:8;
  UWord32 :24;
  /*! 0 to select antenna 0
      1 to select antenna 1, 
      this only applies in non-RxD mode */
  UWord32 srchStep1AntSel:1;
  UWord32 :31;
  /*! Rx mode for step1 search in cmq1, 
      bit 0: RxD (add mode), 
      bit 1: STTD mode, 
      bit 2 RxD (separate mode)
      Note: step1 should not use STTD mode
      Note: For current FW, RxD separate mode is not supported for Step1 */
  UWord32 cmq1SrchStep1TaskMode:3;
  UWord32 :29;
  /*! Peak detect mode for step1 
      00: normal, 
      01: Bypass, 
      10: Shoulder
      Shoulder mode is not currently supported in FW for step1 */
  UWord32 cmq1SrchStep1PdMode:2;
  UWord32 :30;
  /*! Step1 search latest finish time 
      (in frame unit, using frame_cnt) in CMQ1 */
  UWord32 cmq1SrchStep1LateTime :16;
  UWord32 :16;
  /*! Number of peaks to be returned in step1 in CMQ1 */
  UWord32 cmq1SrchStep1NumPeaks :7;
  UWord32 :25;
} WfwSrchCmq1Step1AsyncWriteStruct;

typedef struct
{
  /*! number of results per task on cmq0 */
  UWord32 cmq0SrchListNumResPerTask:8;
  UWord32 :24;
} WfwSrchCmq0ListAsyncWriteStruct;

typedef struct
{
  /*! number of results per task on cmq1 */
  UWord32 cmq1SrchListNumResPerTask:8;
  UWord32 :24;
} WfwSrchCmq1ListAsyncWriteStruct;

typedef struct
{
  /*! Number of peaks reported per task in the result buffer */
  UWord32 srchPNNumResPerTask:3;
  /*! Number of peaks reported by the additional PS task 
      with peak detection disabled when fat path detection
      algorithm is enabled*/
  UWord32 srchPNNumResPerTaskFatPath:4;
  /*! Flag to indicate idle interF PN search */
  UWord32 srchPNIdleInterfFlag:2;
  UWord32 :23;
} WfwSrchPNAsyncWriteStruct;

typedef struct
{
  /*! legacy step1 async variable */
  WfwSrchStep1AsyncWriteStruct srchStep1AsyncWrite;
  /*! legacy step2 async variable */
  WfwSrchStep2AsyncWriteStruct srchStep2AsyncWrite;
  /*! legacy step3 async variable */
  WfwSrchStep3AsyncWriteStruct srchStep3AsyncWrite;
  /*! legacy list srch async variable */
  WfwSrchListAsyncWriteStruct srchListAsyncWrite;
} WfwSrchonLineAsyncWriteStruct;

typedef struct
{
  /*! common async variable */
  WfwSrchCommonAsyncWriteStruct srchCommonAsyncWrite[WFW_NUM_INTRA_SRCH_QUEUES];
  /*! offline step1 srch async variable */
  WfwSrchStep1AsyncWriteStruct srchOfflineStep1AsyncWrite;
  /*! offline nastt srch async variable */
  WfwSrchNASTTAsyncWriteStruct srchNasttAsyncWrite;
  /*! offline pn srch async variable */
  WfwSrchPNAsyncWriteStruct srchPNAsyncWrite;
} WfwSrchoffLineAsyncWriteStruct;

/* union structure for either online/offline sets of commands */
typedef union
{
  /*! legacy online srch async variable */
  WfwSrchonLineAsyncWriteStruct onlineSrchAsyncWriteStruct;
  /*! offline srch async variable */
  WfwSrchoffLineAsyncWriteStruct offlineSrchAsyncWriteStruct;
} WfwSrchOnlineOfflineAsyncWriteUnion;

typedef struct
{
  /*! union structure for either online/offline sets of commands */
  WfwSrchOnlineOfflineAsyncWriteUnion srchAsyncWriteUnion;

  /*! Async write variables for CM mode commands */
  WfwSrchCmq0Step1AsyncWriteStruct srchCmq0Step1AsyncWrite;
  WfwSrchCmq1Step1AsyncWriteStruct srchCmq1Step1AsyncWrite;
  WfwSrchCmq0ListAsyncWriteStruct srchhCmq0ListAsyncWrite; 
  WfwSrchCmq1ListAsyncWriteStruct srchhCmq1ListAsyncWrite; 
  /*! The async variables above are replaced with the ones below */
  WfwSrchStep1AsyncWriteStruct cmqStep1[WFW_NUM_CM_SRCH_QUEUES];
  WfwSrchPNAsyncWriteStruct  cmqPn[WFW_NUM_CM_SRCH_QUEUES]; 
} WfwSrchAsyncWriteStruct;

typedef struct
{
  /*! search command type */
  UWord32 srchCmdType:4; 
  /*! high priority queue srch abort */
  UWord32 nonCmHpqSrchAborted:1;
 /*! default queue srch abort, all searches that came on Queue0 */
  UWord32 nonCmLpqSrchAborted:1;
 /*! search that came on queue 1, used for overlapped PN search*/
  UWord32 nonCmOverlappedSrchAborted:1;
  UWord32 :25;
  /*! srch done on cmq0 */
  UWord32 cmq0SrchDone:1;
  /*! srch done on cmq1 */
  UWord32 cmq1SrchDone:1;
  UWord32 :30;
  /*! srch done indicator */
  UWord32 mdspToArmCmSrchDoneInt:1;
  /*! srch step2 done */
  UWord32 srchStep2HWDoneInt:1;
  /*! srch rxagc value */
  UWord32 cmSrchRxAgcVal:10;
  UWord32 :20;
} WfwSrchAsyncReadStruct;

/* List Search LPQ-HPQ Buffer */
typedef struct
{
  /*! OVSF code index: 0-255 */
  UWord16 ovsfCodeIndex:8;
  /*! STTD mode */
  UWord16 sttd:1;
  /*! 0=last sub-task in CM mode, 
      1=otherwise */
  UWord16 last:1;
  /*! 0 = first sub-task in CM mode, 
      1=otherwise */
  UWord16 first:1;
  /*! align bit */
  UWord16 align:3;
  /*! peak detection mode */
  UWord16 peakDetectMode:2;
  /*! coherent integration length */
  UWord16 cohIntgLen:7;
  /*! rxd indicator */
  UWord16 rxd:2;
  /*! noncoherent integration length */
  UWord16 nonCohIntgLen:7;
  /*! scrambling code index */
  UWord16 scrCodeIndex:13;
  /*! chipx8 portion of position */
  UWord16 posLSW:3;
  /*! chipx1 portion of position */
  UWord16 winStrtPosMSW:16;
  /*! window size */
  UWord16 winSize:16;
} WfwSrchListCodeUnitStruct;

typedef struct
{
  /*! OVSF code index: 0-255 */
  UWord16 ovsfCodeIndex:8;
  /*! STTD mode */
  UWord16 sttd:1;
  /*! 0=last sub-task in CM mode, 
      1=otherwise */
  UWord16 last:1;
  /*! 0 = first sub-task in CM mode, 
      1=otherwise */
  UWord16 first:1;
  /*! 1 = select antenna 1, 
      0 = select antenna 0 */
  UWord16 antSel:1;
  /*! reserved */
  UWord16 :1;
  /*! Set to 1 if fat path detection is enabled */
  UWord16 fatPath:1;
  /*! Peak detect mode 
      (00: normal mode; 01: bypass mode; 10: shoulder mode) */
  UWord16 peakDetectMode:2;
  /*! in 64 chipx1 unit, range is 0-127, 
      the coherent accumulation length will be (Nc+1)*64 chipx1, 
      recommended coherent accumulation length is multiple of 128 for PN search */
  UWord16 cohIntgLen:7;
  /*! RxD mode, 
      0: no Rxd 
      1: Rxd seperate mode 
      2: Rxd add mode Note: RxD mode not supported in PN full window search */
  UWord16 rxd:2;
  /*! range is 0-127, 
      the noncoherent integration length will be Nn+1 */
  UWord16 nonCohIntgLen:7;
  /*! the scrambling code used for this PN search, 0-8176 */
  UWord16 scrCodeIndex:13;
  /*! the start position for PN search, 
      in chipx8 */
  UWord16 winStrtPosLSW:3;
  /*! the start position for PN search, 
      in chipx1 */
  UWord16 winStrtPosMSW:16;
  /*! the window size for PN search to search for the peak, 
      in chipx1 */
  UWord16 winSize:16;
  UWord16 carrierSel:2;
  UWord16 :14;
} WfwSrchPNCodeUnitStruct;


typedef struct
{
  /*! number of list search tasks */
  UWord16 numTask:7;
  /*! Set to 1 if the PN search needs 
      to bypass the FW scheduler*/
  UWord16 flagHQ:1;
  UWord16 :8;
  /*! chipx8 portion of start position for PN search */
  UWord16 posLSW:3;
  UWord16 :11;
  UWord16 sr:1;
  UWord16 rw:1;
  /*! Word 2 */
  UWord16 :16;
  /*! list search async variable array */
  WfwSrchListCodeUnitStruct srchListCodeInfo[WFW_CM_MAX_NUM_PN_CMDS];
} WfwSrchListLpqHpqStruct;

typedef struct
{
  /*! bit 0 to select carrier 0
      bit 1 to select carrier 1
      bit 2 to select carrier 2*/
  UWord16 carrierSel:3;
  UWord16 :13;
} WfwSrchStep1LpqHpqStruct;

typedef struct
{
  /*! Number of tasks 
	  Assuming number of peaks for each task is 8
	  1. in normal mode, maximum is 64 
	  2. in shoulder mode, maximum is 21 
	  3. in separate RxD plus shoulder mode, maximum is 10 
	  4. in separate RxD plus normal mode, maximum is 32 */
  UWord16 numTask:7;
  /*! Set to 1 if the PN search needs 
      to bypass the FW scheduler*/
  UWord16 flagHQ:1;
  UWord16 :8;
  /*! the latest start time for PN search to finish, 
      FW sends a timeout interrupt to SW if the search expires */
  UWord16 latestFinishTime:16;
  /*! the earliest start time for PN search HW task to execute, 
      this parameter can be used in reacquisition PN search, 
      in which the earliest start time takes the AGC settling time 
      into account and enables FW to put the PN search task directly 
      into HW task queue without scheduling */ 
  UWord16 earliestStartTime:16;
  /*! PN search async variable array */
  WfwSrchPNCodeUnitStruct srchListCodeInfo[WFW_MAX_NUM_PN_CMDS];
} WfwSrchPNLpqHpqStruct;

/*! @brief Compressed Mode PN command strucutre */
typedef struct
{
  /*! Number of tasks 
	  Assuming number of peaks for each task is 8
	  1. in normal mode, maximum is 64 
	  2. in shoulder mode, maximum is 21 
	  3. in separate RxD plus shoulder mode, maximum is 10 
	  4. in separate RxD plus normal mode, maximum is 32 */
  UWord16 numTask:7;
  /*! Set to 1 if the PN search needs 
      to bypass the FW scheduler*/
  UWord16 flagHQ:1;
  UWord16 carrier_bit1:1;
  UWord16 :7;
  /*! the latest start time for PN search to finish, 
      FW sends a timeout interrupt to SW if the search expires */
  UWord16 latestFinishTime:16;
  /*! the earliest start time for PN search HW task to execute, 
      this parameter can be used in reacquisition PN search, 
      in which the earliest start time takes the AGC settling time 
      into account and enables FW to put the PN search task directly 
      into HW task queue without scheduling */ 
  UWord16 earliestStartTime:16;
  /*! PN search async variable array */
  WfwSrchPNCodeUnitStruct srchListCodeInfo[WFW_CM_MAX_NUM_PN_CMDS];
} WfwCmSrchPNLpqHpqStruct;

/* Step2 LPQ-HPQ structure */
typedef struct
{
  /*! position info in chipx1 */
  UWord16 pscPosMSW:13;
  UWord16 :3;
} WfwSrchStep2PscPosUnitStruct; 

typedef struct
{
  /*! code group */
  UWord16 codeGrp:6;
  UWord16 :10;
} WfwSrchStep2CodeGrpUnitStruct;

typedef struct
{
  /*! number of PSC */
  UWord16 numPSC:3;
  /*! chipx8 part of position info */
  UWord16 posLSW:2;
  UWord16 :7;
  /*! number of code groups */
  UWord16 numCodeGrp:3;
  /*! enable 64 code groups */
  UWord16 en64:1;
  /*! step2 position array */
  WfwSrchStep2PscPosUnitStruct srchStep2PscPos[8];
  /*! step2 code group array */
  WfwSrchStep2CodeGrpUnitStruct srchStep2CodeGrp[8];
} WfwSrchStep2LpqHpqStruct;

/* Step3 Search LPQ-HPQ Buffer */
typedef struct
{
  /*! chipx8 part of position info */
  UWord16 posLSW:3;
  /*! align bit */
  UWord16 align:3;
  UWord16 :10;
  /*! position info in chipx1 */
  UWord16 winStrtPosMSW:16;
  /*! code group */
  UWord16 codeGrp:6;
  /*! sttd indicator */
  UWord16 sttd:1;
  UWord16 :1;
  /*! ovsf code index */
  UWord16 ovsfCodeIndex:8;
} WfwSrchStep3CodeGrpUnitStruct;

typedef struct
{
  /*! number of tasks */
  UWord16 numTask:6;
  UWord16 :10;
  /*! misc control bit */
  UWord16 :14;
  UWord16 sr:1;
  UWord16 :1;
  /*! step3 code group array */
  WfwSrchStep3CodeGrpUnitStruct srchStep3CodeGrp[64];
} WfwSrchStep3LpqHpqStruct;

typedef union
{
  /*! legacy list srch cmd buffer */
  WfwSrchListLpqHpqStruct srchListLpqHpqBuf;
  /*! legacy step2 srch cmd buffer */
  WfwSrchStep2LpqHpqStruct srchStep2LpqHpqBuf;
  /*! legacy step3 srch cmd buffer */
  WfwSrchStep3LpqHpqStruct srchStep3LpqHpqBuf;
  /*! offline nastt srch cmd buffer */
  WfwSrchNASTTLpqHpqStruct srchNasttLpqHpqBuf;
  /*! offline pn srch cmd buffer */
  WfwSrchPNLpqHpqStruct srchPNLpqHpqBuf;
  /*! offline ste1 srch cmd buffer */
  WfwSrchStep1LpqHpqStruct srchStep1LpqHpqBuf;
} WfwSrchLpqHpqStruct;

typedef union
{
  /*! default queue */
  WfwSrchLpqHpqStruct           srchLpqBuf;
  /*! high priority queue */
  WfwSrchLpqHpqStruct           srchHpqBuf;
} WfwSrchLpqHpqCmdStruct;

typedef struct
{
  /*! 0 to select carrier 0
      1 to select carrier 1 */
  UWord16 carrierSel:1;
  UWord16 :15;
} WfwCmSrchStep1LpqHpqStruct;

/*! @brief CM PN command structure */
typedef struct 
{
  /*! Step1 command buffer for two CM queus */
  WfwCmSrchStep1LpqHpqStruct cmqStep1[WFW_NUM_CM_SRCH_QUEUES];
  /*! PN command buffer for two CM queues */
  WfwCmSrchPNLpqHpqStruct cmq[WFW_NUM_CM_SRCH_QUEUES];
} WfwCmSrchqCmdStruct;


/* Result Buffers */
/* List Search Result Buffers*/
typedef struct
{
  /*! 16bit non-coherent accumulation energy, 
      if the energy value is 0, it¡¯s an invalid peak result */
  UWord16 energy:16;
  /*! chipx8 part of the reported PN position */
  UWord16 posLSW:3;
  UWord16 carrier:2;
  UWord16 :6;
  /*! Set to 0 when results are from normal PS task
      Set to 1 when results are from extra PS task with peak detection disabled when fat path is on */
  UWord16 fatPath:1;
  /*! set to 0 when results are from carrier 0
      set to 1 when results are from carrier 1 */
  UWord16 :1;
  /*! One bit per PN search task, 
      set to 0, when searching results are from antenna 0, 
      set to 1, when searching results are from antenna 1*/
  UWord16 div:1;
  /*! One bit per PN search task, 
      set to 0, when searching results are not obtained through RxD mode, 
      1 when search results are obtained through RxD separate mode, 
      2 when search results are obtained through RxD add mode */
  UWord16 rxd:2;
  /* chipx1 part of reported PN position */
  UWord16 posMSW:16;
} WfwSrchListRsltUnitStruct;

typedef struct
{
  /*! Number of tasks finished, 
      should be equal to N specified in the PN input buffer */
  UWord16 numTaskFinished:7;
  UWord16 :9;
  /*! PN result array 
      Max KxM, K = 64(num of tasks), M = 8(peaks per task) */
  /* Increase size from 512 to 1368 to support fat path detection
     38*2*(6+12) = 1368 (38 is max number of cells in DC, 2 is for RxD, 6 and 12 are number of results in the 2 PS tasks) */
  WfwSrchListRsltUnitStruct srchListRslt[1368];
} WfwSrchListRsltStruct;

/*! @brief Compressed mode PN list search result strcuture */
typedef struct
{
  /*! Number of tasks finished, 
      should be equal to N specified in the PN input buffer */
  UWord16 numTaskFinished:7;
  UWord16 :9;
  /*! PN result array 
      Max KxM, K = 32(num of tasks), M = 6(peaks per task) */
  WfwSrchListRsltUnitStruct srchListRslt[WFW_CM_MAX_NUM_PN_CMDS*WFW_CM_MAX_NUM_PEAKS_PER_TASK];
} WfwCmSrchListRsltStruct;

/* Step1 Search Result Structure */
typedef struct
{
  /*! the non-coherent energy, 
      16 bit wide */
  UWord16 energy:16;
  /*! frequency hypothesis associated with this peak result, 
      only used in initial acquisition search */
  UWord16 freqIndex:2;
  UWord16 :14;
  /*! Reported in unit of chipx8. Range is 0-20476 
      (step1 is searching on chipx2 resolution, 
      the range of PSC position is thus from 0 to 5119, 
      which translates into 0 to 20476 in chipx8) */
  UWord16 pscPos:15;
  UWord16 :1;
} WfwSrchStep1RsltUnitStruct;

typedef struct
{
  /*! Number of peaks found. 
      Range is 0-64 in Rxd Add or normal mode, 
      0-32 in Rxd separate mode */
  UWord16 numPeakFound:7;
  /*! One bit per search command, 
      set to 0, when searching results are not obtained through RxD mode, 
      1 when search results are obtained through RxD separate mode, 
      2 when search results are obtained through RxD add mode */
  UWord16 rxd:2;
  /*! Number of peaks found for second antenna 
      (this field is only valid when RxD is set to 1). 
      Range is 0-32 */
  UWord16 numPeakFoundSec:7;
  /*! step1 result array */
  WfwSrchStep1RsltUnitStruct srchStep1Rslt[64];
  /*! step1 non coh int completed */
  /*! If this is less than what WL1 issued then STEP1 is not complete */
  /*! Hence peak results are not valid */
  UWord32 step1NonCohIntDone:6;

} WfwSrchStep1RsltStruct;

/* Step2 Search Result Structure */
typedef struct
{
  /*! energy result */
  UWord16 energy:16;
  /*! chipx8 part of position */
  UWord16 posLSW:3;
  UWord16 :7;
  /*! code group */
  UWord16 codeGrp:6;
  /*! chipx1 part of position */
  UWord16 posMSW:16;
} WfwSrchStep2RsltUnitStruct;

typedef struct
{
  /*! number of peaks found */
  UWord16 numPeak:3;
  UWord16 :13;
  /*! step2 result array */
  WfwSrchStep2RsltUnitStruct srchStep2Rslt[8];
} WfwSrchStep2RsltStruct;

/* Nastt Search Result Structure */
typedef struct
{
  /*! 16 bit non-coherent accumulation energy, 
      if the energy is 0, it¡¯s an invalid peak */
  UWord16 energy:16;
  /*! chipx8 part of peak position */
  UWord16 posLSW:3;
  /*! Scrambling code index: 0-512 */
  UWord16 scrCodeIndex:9;
  UWord16 :4;
  /*! chipx1 part of peak position */
  UWord16 posMSW:16;
} WfwSrchNASTTRsltUnitStruct;

typedef struct
{
  /*! Number of tasks finished, 
      each task corresponds to one step1 peak */
  UWord16 numTaskDone:6;
  UWord16 :1;
  /*! One bit per search command, 
      set to 0, when searching results are not obtained through RxD mode, 
      1 when search results are obtained through RxD separate mode, 
      2 when search results are obtained through RxD add mode */
  UWord16 rxd:2;
  UWord16 :7;
  /*! nastt srch result array */
  WfwSrchNASTTRsltUnitStruct srchNASTTRslt[512];
} WfwSrchNASTTRsltStruct;

/* Step3 Search Result Structure */
typedef struct
{
  /*! energy result */
  UWord16 energy:16;
  /*! chipx8 part of peak position */
  UWord16 posLSW:3;
  /*! scrambling code index */
  UWord16 scrCodeIndex:13;
  /*! chipx1 part of peak position */
  UWord16 posMSW:16;
} WfwSrchStep3RsltUnitStruct;

typedef struct
{
  /*! number of tasks finished */
  UWord16 numTaskFinished:6;
  UWord16 :10;
  /*! step3 result array*/
  WfwSrchStep3RsltUnitStruct srchStep3Rslt[512];
} WfwSrchStep3RsltStruct;

typedef union
{
  /*! step1 result buffer */
  WfwSrchStep1RsltStruct srchStep1RsltBuf;
  /*! legacy step2 result buffer */
  WfwSrchStep2RsltStruct srchStep2RsltBuf;
  /*! legacy step3 result buffer */
  WfwSrchStep3RsltStruct srchStep3RsltBuf;
  /*! list/pn srch result buffer */
  WfwSrchListRsltStruct srchListRsltBuf;
  /*! nastt srch result buffer */
  WfwSrchNASTTRsltStruct srchNasttRsltBuf;
} WfwSrchRsltBufStruct;

typedef union
{
  /*! result buffer for default queue */
  WfwSrchRsltBufStruct srchLpqRsltBuf;
  /*! result buffer for high priority queue */
  WfwSrchRsltBufStruct srchHpqRsltBuf;
} WfwSrchLpqHpqRsltStruct;

/*! @brief Union of CM step1/ PN search result struct */
typedef union
{
  /*! step1 result buffer */
  WfwSrchStep1RsltStruct srchStep1RsltBuf;
  /*! list/pn srch result buffer */
  WfwCmSrchListRsltStruct srchListRsltBuf;
} WfwCmSrchRsltBufStruct;

/*! @brief Union of results shared between CM and G2W reconf srch */
typedef union
{
  /*! result buffer for compressed mode */
  WfwCmSrchRsltBufStruct cmRslt[WFW_NUM_CM_SRCH_QUEUES];
  /*! result buffer for G2W dedicated mode stage1/2 search */
  WfwCmSrchRsltBufStruct g2wRslt[WFW_NUM_G2W_STAGES];
} WfwCmInterratRsltUnion;

 
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_SRCH_INTF_H */

