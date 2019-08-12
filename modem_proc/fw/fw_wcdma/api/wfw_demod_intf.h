/*!
  @file

  @brief
  WCDMA FW Offline demod interface definitions header file
*/

/*===========================================================================

  Copyright (c) 2007-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_demod_intf.h#1 $
$DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/23/13   NC/IM   LVA
04/10/13   EM      Add dirty bit to check that Tx Det is requested from SW
02/13/13   EM      New SCHIC R99 support
07/05/12   MM      Offline PICH feature interface changes.
07/02/12   RL      Added finger 4/6 tap logging support
05/24/12   SS      Added HS-RACH feature
04/27/12   NC      Frame Early Termination & DCCH Early Detection Feature Updates
03/29/12   RP      added ARD V2 support
03/27/12   YL      added EDRX support
03/15/12   NC      Frame Early Termination & DCCH Early Detection Feature
03/06/12   YL      QICE uses qiceEn instead of enable bit in SWI to support QPM
02/13/12   RP      ARD logging intf update CR336555
01/13/12   PT      support HS-FACH
12/16/11   SH      Added #define WFW_DEMOD_MAX_NUM_DPCH 
10/20/11   RP      update interface for ARD feature
10/07/11   RP      support ARD feature
08/02/11   YL      EWC debug intf change to fix APEX parsing issue(size/memplacement)
08/01/11   PK      Reduced num cells on prim carrier to 7
05/26/11   RP      EWC logging interface update for ARC support
05/10/11   RP      EQ logging interface update
04/28/11   YL      logging interface for QICE and cleanup
04/28/11   YL      interface related changes for QICE
04/26/11   MM      Interface change for EHICH ERGCH unfication. 
03/26/11   MM      Comments updated. 
03/16/11   JP      Move mimoTwoBeam field from WfwMiscHsStruct to WfwDemodHSDPAChannelInfoStruct
                   Move sub frame number field from Word1 to Word0 in WfwDbackHsAeLogStruct for 0x421D v6.
02/24/11   PT      Increased rateMatchedSizePerRadioFrame bitwidth in TF Pool table
01/28/11   PT      Removed wcb col offset and trCh parameters from SW-FW interface
12/15/10   EM      Add SW control AGCH reconfig flag for FIQ
12/14/10   MM      Update WFW_DEMOD_NUM_MAX_COMBINERS to 4.
11/16/10   PT      Update CCTrCh and TrCh table
10/15/10   JC      Support QICE logging. (A and Cell weight only for now.)
10/06/10   MM      Add DPCH TPC and DP define.
09/16/10   JP      Rank dependent Spatial equalization per receiverfor both CQI and HS demod
09/10/10   JP      Back out from #3 to #1 for TD eTerm and SCCH logging, max finger num for 2nd carrier
09/03/10   JC      Increase max number of fingers on 2nd carrier to support DC neighbor cells. 
08/28/10   JP      TD early termination and HS SCCH decode status logging support
08/20/10   JP      Update MIMO new parameters and logging for MIMO A&E and decoding status
08/08/10   GS      Set number of pri chain fingers to 12 to restrict total to 16
08/02/10   JP      Update WfwDembackHsInfoTableStruct which is removed dummy field(2bytes), not used
07/08/10   JC      Add per-carrier info with CQI to SW demod status dump.
06/22/10   HK      Added EWC logging structures
06/22/10   EM      Add AGCH pilot energy field to AGCH result interface
04/12/10   JP      Update WfwDbackHsDecodeStatusLogStruct: add eqEnb and qiceEnb
04/08/10   HK      Add cellNum to SCH logging interface
04/09/10   PK      Added switching algorithm for MIMO
04/02/10   PK      Added CEQ Demod and CEQ CQI per receiver support
03/30/10   JP      Remove WFW_DBACK_HS_MAX_NUM_SCCH(unused in SW)
02/23/10   RL      FDPCH feature support
02/05/10   JP      Both WfwDbackHsSeqDemodStruct and the WfwDbackHsRWDemodStruct are to be reverted to Taxis interface
01/13/10   JP      Pickup CQI related codes from Taxis
                   Updated WfwDemodHSDPAChannelInfoStruct, WfwDemodCellInfoStruct and WfwMiscHsStruct
11/19/09   JP      Update WfwDembackHsScchOrderFifoTableStruct
10/23/09   JC      Add definition of the count of common info update bits per carrier
10/13/09   RL      Change the comments for WfwDemodCppInfoStruct
10/01/09   BL      Add HS-SCCH order indication to SW
09/11/09   NC      Set num primary carrier fingers to 16 for DC support
09/10/09   JP      Update WfwDbackHsMimoDemodLogStruct and WfwDbackHsAeLogStruct: add hsSubFrmNum field
09/08/09   YL      export EQ positions in status dump
09/02/09   BL      dbackHsDecodeStatusLog buffer indexing back to GSFN%16 method
08/21/09   JP      Added DOB and DRMB define valuses for SW sync up
08/14/09   JC      Remove obsolete QICE loggin info structure.
08/11/09   JC      Restored the number of EQs back to 3.
08/10/09   PK      Enhanced TD detection support
06/12/09   GS      Change cqiAdjustment bitfield to signed number
05/27/08   YL      enhancement for CME/SCHIC cfg cmd acknowledgement
05/06/09   YL      comments only for mimo logging hacks
04/21/09   PK      Added intf for SW to select alternative CQI filter coeffs
03/31/09   PK      Added TFCI log packet
03/25/09   NC      CLTD logging and TD det interface changes
03/25/09   NC      Add apfGain and ttl mode to per-finger status dump
03/21/09   GS      Removed unused part in WfwDemodCellInfoStruct
03/10/09   PK      Increased dob size by 10500, dobStatFifo by 12
03/03/09   FT      Incr CmSlotBitMasks for HICH and RGCH from 4 to WFW_DEMOD_NUM_MAX_CELLS
08/05/09   BL      (From Taxis)Added rawNdi field in WfwDbackHsDecodeStatusLogStruct
07/29/09   JP      Added Demback A&E and MIMO async log interfaces
                   Updated WfwDbackHsAeLogStruct, WfwDbackHsSeqDemodStruct and WfwDbackHsRWDemodStruct
07/16/09   BL      Moved HSL/MIMO/DC_INDICATOR_BIT in from demback header file
                   WfwDemodHSDPAChannelInfoStruct.dbIdx: 1 bit to 2 bits
                   Modified WfwDembackHsInfoTableStruct: hsR5HslMimoIndicator from
                     2 bits to 3 bits, etc.
                   Modified WfwDembackHsDecobStatusFifoTableStruct: hsInfoIdx from
                     1 bit to 2 bits
                   New structure added: WfwDbackHsDecodeStatusAsyncReadStruct
07/16/09   JC      Remove eqCfgPending processing.
07/15/09   JC      Moved eqCfgPending in WfwDemodInfoInterfaceStruct down so
                   that cache flush in NTC scheduler works correctly.
07/01/09   JC      Changed some definitions for dual carrier support.
06/30/09   MM      Add define for secondary carrier enabling.
06/30/09   JC      Changed Common Config Info for dual carrier.
06/08/09   MM      Add define for dual carrier.
05/21/09   NC      Merge with Taxis CS2 branch SWI
05/11/09   RL      Added combId for TC ovsf ch0 DMA address allocation
                   Removed nonMasterDemPath selection from Common Config Info.
05/11/09   RL      Added combId for TC ovsf ch0 DMA address allocation
05/01/09   MM      Fixed missing define which was causing compile error.
04/27/08   JC      Expanded ohcGen of EQInfo into SCH, PCPICH, and SCPICH gen bit fields.
04/07/08   JC      Added eqCfgPending to WfwDemodInfoInterfaceStruct
04/01/09   RL      Added enableAPF to demod per finger cfg
03/30/09   NC      Reduce SW-FW interface to fit in 32k
03/23/09   MM      Updated number of SCCH channels for dual carrier case.
03/10/09   JC      Removed TxD Enable bit from Equalizer info.
03/06/09   JC      Updated Equalizer info, Common Config Info, and QICE logging for Aphrodite SWI.
03/05/09   RL      Change for Aphrodite SWI
01/27/09   PK      MIMO related logging
01/08/09   PK      Added intermittent sched threshold for CQI
12/15/08   NC      Align AGCH SWI fields with RGHI
11/21/08   PK      CQI Q factor changed from 5 to 4 (for 0x4222 packet)
11/11/08   NC      Dynamic lock threshold interface
10/23/08   YL      SCHIC info logging and CME logging update
10/29/08   GS      Add schicRot45d to demod common cfg for SCH info
10/02/08   FT      MIMO CQI logging changes
09/28/08   PK      Cqi adj per receiver, log CQI in Q5
09/24/08   NC      Enable 12 fingers in FW
09/03/08   PK      Added LUT for cqi bler alignment
08/14/08   NC      Change enum def for phch reconfig and no timing change
                   to match combiner enum and L1
08/05/08   NC      Increase size of QFT energy threshold
07/15/08   RK      Changed the pilotEnergy in EUL result structure from Word32
                   to UWord32
07/11/08   PK    Added more HS logging, and phaseNoiseRej enable/disable
07/10/08   NC      Added lock threshold and APF beta value to SWI
06/27/08   NC      btfDelay to TX AGC intf, addl TTL/FTL variables exported,
                   status dump pnPos in Cx32 units
06/17/08   GS      Updated demodCommon and Eq interface for multi receiver
06/11/08   RK      Added pilot Energy to the result buffer in EUL and merged
                   the RGCH & HICH structures
06/06/08   NC      Expand phaseRef to include MIMO-mode SCPICH
05/22/08   PK    Added SW-FW misc async params struct
05/14/08   PK    Added signalledTbs in HSlog and numTfci in TFCInfoTbl
05/14/08   NC      Move divInfo from status dump to async interface
04/28/08   RK    Added EUL DL interface parameters
04/22/08   ML      Updated HS and MIMO logging
04/04/08   PK      Increased hsTrBlkSize field in dobStatFifo to 16bits
04/01/08   ML      Added names to unnamed struct and union
03/27/08   NC      Add TTL/FTL fb factors to SWI
03/12/08   ML      Added Mimo logging
03/07/08   GS      DB index in HS channel cfg, add bit field to CM ctrl table
02/27/08   PK      Added decodedTfci to dobStatFifo
02/15/08   GS      Added CM method type
01/17/08   NC      Modify TxD Det results based on discussions.  Add RSSI to
                   indicator results interface
01/03/08   NC      Changes for TxD determination
01/02/08   NC      Clean up status dump interface
12/21/07   NC      Change freqError to signed value
11/08/07   NC      AIs from TX code review - move AV & AI async vars
10/15/07   NC      Add HS decode log interface and HS CM control table
10/04/07   NC      Added QFT/QTT async interface variables
09/21/07   NC      Update status dump and include BTF delay to interface
08/24/07   GS      Arranged DPCH channel dield to fit expanded comb idx
                   Added finger phase ref bit
08/08/07   GS      Updated HS channel and info table related information
08/01/07   GS      Added PICH/MICH related interfaces
07/26/07   GS      Changes related to demod cfg cmd and pending status
07/01/07   GS      Added demod status dump definition
===========================================================================*/

#ifndef WFW_DEMOD_INTF_H
#define WFW_DEMOD_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */

/*******************************/
/* Demfront related definition */
/*******************************/
/* Secondary carrier support enabled.*/
#define WFW_DEMOD_SEC_CARRIER_ENABLED
/* HS-FACH support enabled. Note that this is NOT used in FW source.
   FW featurization is under different feature name and is in custwfw.h file
   Only to be used outside of FW to know FW has HS-FACH feature enabled.
   Currently used by FW Sim to featurize its code.*/
#define WFW_DEMOD_HS_FACH_ENABLED
/* E_DRX support enabled for CSIM purpose */
#define WFW_DEMOD_E_DRX_ENABLED

/*! @brief Maximum number of cells 6 DPCH RL, 1 PCCPCH setup for primary carrier */
#define WFW_DEMOD_NUM_MAX_CELLS_PRI_CARRIER   7

/*! @brief Number of fingers available in HW  for primary carrier */
#define WFW_DEMOD_NUM_MAX_FINGERS_PRI_CARRIER 12
/*! @brief Maximum of number of physical channels for primary carrier
 6 DPCH radio links (has all 3 multicode) + 7 SCCPCH (1 MCCH, 3x2 MTCH),
 1 AICH, 1 PICH, 1 MICH, 2 PCCPCH, 1 AGCH and 4 HICH, 4 RGCH, 1 HSDPA - Total 28
 AICH and PICH don't exists together
 AICH/PICH and DPCH are not setup together */
#define WFW_DEMOD_NUM_MAX_PHCHS_PRI_CARRIER 19
/*! @brief Number of equalizers for primary carrier */
#define WFW_DEMOD_NUM_MAX_EQS_PRI_CARRIER 3
/*! @brief Index of the primary carrier */
#define WFW_DEMOD_PRIMARY_CARRIER_IDX 0

// Settings for 2 carrier
/*! @brief Number of cells for second carrier  */
#define WFW_DEMOD_NUM_MAX_CELLS_SEC_CARRIER    5
/*! @brief Number of fingers available in HW for second carrier */
#define WFW_DEMOD_NUM_MAX_FINGERS_SEC_CARRIER   6
/*! @brief Maximum of number of physical channels for second carrier
    1 HS-PDSCH and HS-SCCH (grouped as one channel) +
    1 AGCH + 4 RGCH/HICH + 4 FDPCH +1 PCCPCH*/
#define WFW_DEMOD_NUM_MAX_PHCHS_SEC_CARRIER     11
/*! @brief Number of equalizers for second carrier*/
#define WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER       3
/*! @brief Number of carriers */
//#define WFW_DEMOD_NUM_MAX_CARRIERS              3			
#define WFW_DEMOD_NUM_MAX_CARRIERS              2

/*! @brief Number of MIMO streams */
#define WFW_DEMOD_NUM_MIMO_STREAMS              2
/*! @brief Index of the secondary carrier */
#define WFW_DEMOD_SECONDARY_CARRIER_IDX         1


/*! @brief Number of cells for third carrier  */
//#define WFW_DEMOD_NUM_MAX_CELLS_TER_CARRIER     3   
/*! @brief Maximum of number of physical channels for third carrier
  1 HS-PDSCH and HS-SCCH (grouped as one channel) */
//#define WFW_DEMOD_NUM_MAX_PHCHS_TER_CARRIER     1
/*! @brief Index of the third carrier */
#define WFW_DEMOD_TERTIARY_CARRIER_IDX          2
/*! @brief Number of equalizers for third carrier*/
//#define WFW_DEMOD_NUM_MAX_EQS_TER_CARRIER       3		

/*! @brief MASK for accessing the secondary carrier */
#define WFW_DEMOD_SECONDARY_CARRIER_MASK        0x2

/*! @brief MASK for accessing the secondary carrier */
#define WFW_DEMOD_PRIMARY_CARRIER_MASK          0x1

// Settings for 1 carrier
// /*! @brief Number of cells for second carrier  */
// #define WFW_DEMOD_NUM_MAX_CELLS_SEC_CARRIER     0
// /*! @brief Number of fingers available in HW for second carrier */
// #define WFW_DEMOD_NUM_MAX_FINGERS_SEC_CARRIER   0
// /*! @brief Maximum of number of physical channels for second carrier
//     HS-PDSCH and HS-SCCH (grouped as one channel) */
// #define WFW_DEMOD_NUM_MAX_PHCHS_SEC_CARRIER     0
// /*! @brief Number of equalizers for second carrier*/
// #define WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER       0
// /*! @brief Number of carriers */
// #define WFW_DEMOD_NUM_MAX_CARRIERS              1


/*! @brief Maximum number of cells */
//#define WFW_DEMOD_NUM_MAX_CELLS 14  
#define WFW_DEMOD_NUM_MAX_CELLS 12

/*! @brief Number of fingers  */
#define WFW_DEMOD_NUM_MAX_FINGERS 24
/*! @brief Maximum number of fingers per serving cell */
#define WFW_DEMOD_NUM_MAX_FINGERS_PER_SERVING_CELL 12
/*! @brief Maximum of number of physical channels */
//#define WFW_DEMOD_NUM_MAX_PHCHS (WFW_DEMOD_NUM_MAX_PHCHS_PRI_CARRIER + WFW_DEMOD_NUM_MAX_PHCHS_SEC_CARRIER + WFW_DEMOD_NUM_MAX_PHCHS_TER_CARRIER)   
#define WFW_DEMOD_NUM_MAX_PHCHS (WFW_DEMOD_NUM_MAX_PHCHS_PRI_CARRIER + WFW_DEMOD_NUM_MAX_PHCHS_SEC_CARRIER)


/*! @brief Number of equalizers */
//#define WFW_DEMOD_NUM_MAX_EQS (WFW_DEMOD_NUM_MAX_EQS_PRI_CARRIER + WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER + WFW_DEMOD_NUM_MAX_EQS_TER_CARRIER )   
#define WFW_DEMOD_NUM_MAX_EQS (WFW_DEMOD_NUM_MAX_EQS_PRI_CARRIER + WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER)

/*! @brief Number of combiners. Set to 4 because SW uses SCCPCH combiner index 3. 
    Update when accurate number for SCCPCH concurrency is known.*/
#define WFW_DEMOD_NUM_MAX_COMBINERS 5
/*! @brief Number of channels and combiners */
#define WFW_DEMOD_NUM_MAX_PHCH_COMBINERS (WFW_DEMOD_NUM_MAX_PHCHS+WFW_DEMOD_NUM_MAX_COMBINERS)
/*! @brief Maximum number of Uplink supported, 0: primary carrier; 1: secondary carrier */
#define WFW_NUM_MAX_UPLINK_CARRIERS 1

/*! @brief Number of channels per cell
 1 DPCH (has all upto 3 multicode), 3 SCCPCH, 1 AICH, 1 PICH, 1 MICH, 2 PCCPCH,
 1 AGCH, 1 HICH, 1 RGCH, 1 HSDPA
 Total 13
 AICH and PICH don't exists together
 AICH/PICH and DPCH are not setup together */
#define WFW_DEMOD_NUM_MAX_PHCH_PER_CELL 11
/*! @brief Number of demfront receivers 3 */
#define WFW_DEMOD_NUM_RECE 3

/*! @brief Maximum number of DPCH channel. 6 DPCH + 2 F-DPCH. 8 in total*/
#define WFW_DEMOD_MAX_NUM_DPCH 8
/*! @brief Maximum of number of AGCH result buffer */
#define WFW_DEMOD_NUM_MAX_AGCH_RD_RST_BUF_SIZE 6
/*! @brief Maximum of number of RGCH/HICH result buffer */
#define WFW_DEMOD_NUM_MAX_RGHI_RD_RST_BUF_SIZE 18

/*! @brief Number of buffering of all types required. Every thing is double buffered */
#define WFW_DEMOD_NUM_INFO_BUFS 2
/*! @brief Number of buffering of dirty bits. Every thing is double buffered */
#define WFW_DEMOD_NUM_SEL_BUFS 2

/*! @brief Number of combiner for status dump */
#define WFW_DEMOD_NUM_MAX_COMBINERS_STATUS_DUMP 4

/*!@ brief Wait till next frame ind */
#define WFW_DEMOD_IND_NEXT_DEMOD_IN_NEXT_FR 0xFF
/*!@ brief Number of MICH indicator */
#define WFW_DEMOD_NUM_MICH_INDICATOR 16
/*!@ brief Number of MICH indicator buffer. This is 1 more than max MICH
 to put in reserved indicator value to indicate next info is for next frame */
#define WFW_DEMOD_NUM_MAX_INDICATOR_BUF ((WFW_DEMOD_NUM_MICH_INDICATOR) + 1)

/*! @brief Invalid value for dpch index parameter */
#define WFW_DEMOD_HS_INVALID_DPCH_CHAN_IDX 255

/*! INVALID start subframe number for eDRX */
#define WFW_DEMOD_EDRX_INVALID_SUBFRAME 0x3FFF

/*! @brief Defines bit field indicating update in demod common info */
#define WFW_DEMOD_COMMON_RECE_UPDATE_BIT   0
#define WFW_DEMOD_COMMON_OHC_IC_UPDATE_BIT 1
#define WFW_DEMOD_COMMON_NUM_INFO_WORDS    2 /* count of common info update bits per carrier */

/*! @brief Defines bit field indicating update in demod ard info */
#define WFW_DEMOD_ARD_UPDATE_BIT   0

/*! @brief Number of CLTD TAM modes */
#define WFW_DEMOD_NUM_CLTD_TIMING_MODE 2

/*! @brief Number of max HSDPA channels */
#define WFW_DEMOD_HSDPA_MAX_NUM_PHY_CHANNELS           15
/*! @brief Number of HSDPA symbols per subframe */
#define WFW_DEMOD_HSDPA_NUM_SYMS_PER_CODE_PER_SUBFN    480
/*! @brief Number of HSDPA symbols per bpg */
#define WFW_DEMOD_HSDPA_NUM_SYMS_PER_CODE_PER_BPG      16

/*! @brief DRMB bitwidth */
#define WFW_DEMOD_DRMB_NUM_BANKS               24 /* DRMB bitwidth 24 */

/*! @brief DRMB memory size 
 
    CAT24/26/28/29 (IR buffer = 43200 bits): 
    510 mempool lines * 1024 bits/line / (32 bits/word) = 16320 words
 
    CAT14/20 (IR buffer = 126765 bits)
    746 mempool lines * 1024 bits/line / (32 bits/word) = 23872 words
*/
#define WFW_DEMOD_HS_DRMB_MEM_SIZE_IR_43200   16320
#define WFW_DEMOD_HS_DRMB_MEM_SIZE_IR_126765  23872

/*! @brief Decode buffer memory size 
 
   On Jolokia, this is in dedicated memory (db_buf) and is 11840 words
*/
#define WFW_DEMOD_TD_DOB_MEM_SIZE              11840

/*! @brief Decode buffer memory size of Non HS VD (dedicated memory) */
#define WFW_DEMOD_NON_HS_VD_DOB_MEM_SIZE       864
#define WFW_DEMOD_NON_HS_VD_DOB_MEM_START      0

/*! @brief Decode buffer memory size of Non HS TD (db_buf)
*/
#define WFW_DEMOD_NON_HS_TD_DOB_MEM_SIZE       1216

/*! @brief Decode buffer memory size of HS */
#define WFW_DEMOD_HS_DOB_MEM_SIZE              (WFW_DEMOD_TD_DOB_MEM_SIZE - WFW_DEMOD_NON_HS_TD_DOB_MEM_SIZE)

/*! @brief Start address of DOB for Non HS 
*/
#define WFW_DEMOD_NON_HS_TD_DOB_MEM_START      WFW_DEMOD_HS_DOB_MEM_SIZE

/*! @brief Start address of DOB for HS */
#define WFW_DEMOD_HS_DOB_MEM_START             0

/*! @brief DOB page size in byte */
#define WFW_DEMOD_DOB_PAGE_SIZE_BMSK           0x1FFF
#define WFW_DEMOD_DOB_PAGE_ID_SHFT             14

/******************************/
/* Demback related definition */
/******************************/

/*! @brief Total number of CCTrCh Tables
  1 DPCH or 2 R99 SCCPCH */
#define WFW_DBACK_NUM_CCTRCH_TABLES 2

/*! @brief Total number of entries in the TF Pool Tables */
#define WFW_DBACK_NUM_TF_POOL_TABLE_ENTRIES 128

/*! @brief Total number of entries in the TFCI Pool Tables */
#define WFW_DBACK_NUM_TFCI_POOL_TABLE_ENTRIES 256

/*! @brief Total number of dwords in each TFCI Pool Table */
#define WFW_DBACK_TFCI_POOL_TABLE_SIZE 3

/*! @brief Max number of TrCh per MBMS physical channel */
#define WFW_DBACK_MBMS_NUM_TRCH_PER_PHYCHAN 4

/*! @brief Total number of PCCPCH CCTrCh Tables */
#define WFW_DBACK_NUM_PCCPCH_CCTRCH_TABLES 4

#define WFW_DBACK_PCCPCH_CCTRCH_ID_FIRST 2
#define WFW_DBACK_PCCPCH_CCTRCH_ID_LAST 5

#define WFW_DBACK_NUM_DWORDS_IN_TTI_ORDER_TABLE 3
#define WFW_DBACK_NUM_DWORDS_IN_TF_INFO_TABLE 16
#define WFW_DBACK_NUM_DWORDS_IN_TFC_INFO_TABLE 64
#define WFW_DBACK_NUM_TRCH_PER_CCTRCH 12

#define WFW_DBACK_PCCPCH_TASK_FIELD__TOTAL_NUM_CODED_BITS     0x21C
#define WFW_DBACK_PCCPCH_TASK_FIELD__RM_TYPE                  HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_DRM_RM_TYPE_NO_RM
#define WFW_DBACK_PCCPCH_TASK_FIELD__TTI                      HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_DRM_TTI_TTI_20MS
#define WFW_DBACK_PCCPCH_TASK_FIELD__CODE_TYPE                HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_SVD_CODE_TYPE_RATE_1_2
#define WFW_DBACK_PCCPCH_TASK_FIELD__SEL_DRM_CFG              HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_DRM_SEL_DRM_CFG_USE_HW_CFG_FOR_PCCPCH
#define WFW_DBACK_PCCPCH_TASK_FIELD__EINI_1                   1
#define WFW_DBACK_PCCPCH_TASK_FIELD__EPLUS_1                  0
#define WFW_DBACK_PCCPCH_TASK_FIELD__EMINUS_1                 0
#define WFW_DBACK_PCCPCH_TASK_FIELD__EPLUS_2                  0
#define WFW_DBACK_PCCPCH_TASK_FIELD__EMINUS_2                 0

#define WFW_DBACK_PCCPCH_TASK_FIELD__CRC_TYPE                 HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_SVD_CRC_TYPE_CRC_16_BITS
#define WFW_DBACK_PCCPCH_TASK_FIELD__NUM_PADDED_BITS          0
#define WFW_DBACK_PCCPCH_TASK_FIELD__NUM_TR_BLK               1
#define WFW_DBACK_PCCPCH_TASK_FIELD__CODE_BLK_SIZE            262
#define WFW_DBACK_PCCPCH_TASK_FIELD__DRM_SCALE_FACTOR         HWTASK_W_DBACK_NONHS_W_DBACK_NONHS_SVD_DRM_SCALE_FACTOR_SCALE_1_2
#define WFW_DBACK_PCCPCH_TASK_FIELD__YAM_THRESHOLD            0
#define WFW_DBACK_PCCPCH_TASK_FIELD__NUM_CODE_BLK             1
#define WFW_DBACK_PCCPCH_TASK_FIELD__TR_BLK_SIZE              246
#define WFW_DBACK_PCCPCH_TASK_FIELD__STORAGE_DIR              1

#define WFW_DBACK_PCCPCH_TASK_FIELD__NUM_FULL_ROWS_IN_TRCH    9
#define WFW_DBACK_PCCPCH_TASK_FIELD__NUM_PARTIAL_SYM_IN_LAST_ROW 0
#define WFW_DBACK_PCCPCH_TASK_FIELD__COL_IDX                  0
#define WFW_DBACK_PCCPCH_TASK_FIELD__ROW_IDX                  0

#define WFW_TASK_WORD_0 0
#define WFW_TASK_WORD_1 1
#define WFW_TASK_WORD_2 2
#define WFW_TASK_WORD_3 3
#define WFW_TASK_WORD_4 4
#define WFW_TASK_WORD_5 5
#define WFW_TASK_WORD_6 6
#define WFW_TASK_WORD_7 7
#define WFW_TASK_WORD_8 8

#define WFW_DBACK_NON_HS_DECOB_STATUS_FIFO_SIZE 128
#define WFW_DBACK_HS_DECOB_STATUS_FIFO_SIZE 32
#define WFW_DBACK_HS_SCCH_ORDER_FIFO_SIZE 8

/*! Length of per-frame DPCH FW->SW interface */
#define WFW_DBACK_DPCH_FRAME_FIFO_SIZE 8

/*! @brief Maximum SCCH per HS channel setup
    In tri-carrier case it is 10. */
#define WFW_DEMOD_NUM_MAX_SCCH 10
/*! @brief Total number of HARQ Ids per carrier, including both MIMO streams */
#define WFW_HS_NUM_HARQ_IDS 16

/*! @brief Total number of HARQ Ids for HSL */
#define WFW_HSL_NUM_HARQ_IDS 0
/*! @brief Number of max hs-scch per carrier */
#define WFW_HS_NUM_SCCH_PER_CARRIER 4

/*! @brief Number of TTI in the HS DecOB */
#define WFW_DBACK_HS_DECOB_NUM_TTI    4

/*! @brief Get the starting DOB address for TTI t, carrier c, MIMO stream s */
#define WFW_DBACK_GET_DECOB_ADDR(t,c,s) WFW_HS_DECOB_T##t##_C##c##_S##s

/*! @brief Unsupported combination for target */
#define WFW_DBACK_INVALID_DOB_ADDR      0xffffffff

/* TTI 1 */
#define WFW_HS_DECOB_T0_C0_S0 0
#define WFW_HS_DECOB_T0_C0_S1 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T0_C1_S0 1328
#define WFW_HS_DECOB_T0_C1_S1 WFW_DBACK_INVALID_DOB_ADDR

#if WFW_DEMOD_NUM_MAX_CARRIERS == 3
#define WFW_HS_DECOB_T0_C2_S0 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T0_C2_S1 WFW_DBACK_INVALID_DOB_ADDR
#endif /* WFW_DEMOD_NUM_MAX_CARRIERS == 3 */

 /* TTI 2 */
#define WFW_HS_DECOB_T1_C0_S0 2656
#define WFW_HS_DECOB_T1_C0_S1 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T1_C1_S0 3984
#define WFW_HS_DECOB_T1_C1_S1 WFW_DBACK_INVALID_DOB_ADDR

#if WFW_DEMOD_NUM_MAX_CARRIERS == 3
#define WFW_HS_DECOB_T1_C2_S0 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T1_C2_S1 WFW_DBACK_INVALID_DOB_ADDR
#endif /* WFW_DEMOD_NUM_MAX_CARRIERS == 3 */

/* TTI 3 */
#define WFW_HS_DECOB_T2_C0_S0 5312
#define WFW_HS_DECOB_T2_C0_S1 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T2_C1_S0 6640
#define WFW_HS_DECOB_T2_C1_S1 WFW_DBACK_INVALID_DOB_ADDR

#if WFW_DEMOD_NUM_MAX_CARRIERS == 3
#define WFW_HS_DECOB_T2_C2_S0 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T2_C2_S1 WFW_DBACK_INVALID_DOB_ADDR
#endif /* WFW_DEMOD_NUM_MAX_CARRIERS == 3 */

/* TTI 4 */
#if WFW_DBACK_HS_DECOB_NUM_TTI >= 4

#define WFW_HS_DECOB_T3_C0_S0 7968
#define WFW_HS_DECOB_T3_C0_S1 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T3_C1_S0 9296
#define WFW_HS_DECOB_T3_C1_S1 WFW_DBACK_INVALID_DOB_ADDR

#if WFW_DEMOD_NUM_MAX_CARRIERS == 3
#define WFW_HS_DECOB_T3_C2_S0 WFW_DBACK_INVALID_DOB_ADDR
#define WFW_HS_DECOB_T3_C2_S1 WFW_DBACK_INVALID_DOB_ADDR
#endif /* WFW_DEMOD_NUM_MAX_CARRIERS == 3 */

#endif /* WFW_DBACK_HS_DECOB_NUM_TTI >= 4 */

typedef enum
{
  WFW_DBACK_HS_MIMO_INDICATOR_BIT = 0,
  WFW_DBACK_HS_DC_INDICATOR_BIT,
  WFW_DBACK_HS_3C_INDICATOR_BIT
} __attribute__ ((packed)) WfwDembackModeIndicatorBitEnum;

/*! @brief Total number of HSL sub frames to buffer */
#define WFW_DBACK_HSL_NUM_SUBFN_BUFFER 13

/*! @brief Number of HS decode status log packet buffers */
#define WFW_DEMOD_HS_DEC_STATUS_LOG_PACKET_BUF_SIZE 16
/*! @brief Number of HS MIMO demod log packet buffers */
#define WFW_DEMOD_HS_MIMO_DEMOD_LOG_PACKET_BUF_SIZE 16
/*! @brief Number of HS AE log packet buffers */
#define WFW_DEMOD_HS_AE_LOG_PACKET_BUF_SIZE 16
/*! @brief Size of HS CM control table */
#define WFW_DEMOD_HS_CM_CTRL_TABLE_SIZE 16
/*! @brief Number of HS CM control table buffers */
#define WFW_DEMOD_HS_CM_CTRL_TABLE_NUM_BUF 2

/*! @brief Number of EWC Regular log packet buffers */
#define WFW_DEMOD_EWC_REGULAR_LOG_PACKET_BUF_SIZE 16

/*! @brief Number of EWC debug log packet buffers */
#define WFW_DEMOD_EWC_DEBUG_LOG_PACKET_BUF_SIZE 8

/*! @brief Number of SCHIC debug log packet buffers */
#define WFW_DEMOD_SCHIC_LOG_PACKET_BUF_SIZE 16

/*! @brief Number of Channel Analysis log packet buffers */
#define WFW_DEMOD_HS_CHAN_ANALYSIS_PACKET_BUF_SIZE 16

/*! @brief Number of Pessimistic CQI log packet buffers */
#define WFW_DEMOD_HS_PESSIMISTIC_CQI_PACKET_BUF_SIZE 16

/*! @brief Q factor of SCPICH imbalance scale */
#define WFW_MIMO_SCPICH_IMBALANCE_Q_FAC 14

/*! Max number of TPC symbols in a slot */
#define WFW_DEMOD_NUM_MAX_TPC_SYM_IN_SLOT 8

/*! Max number of DP symbols in a slot */
#define WFW_DEMOD_NUM_MAX_DP_SYM_IN_SLOT 16

/*! max number of resources that VPE supports 
   eventually, these macros can be removed when 
   WFW_DEMOD_NUM_MAX_CARRIERS/FINGERS/CELLS are changed */
#define WFW_VPE_HW_MAX_CELLS    12
#define WFW_VPE_HW_MAX_FINGERS  24
#define WFW_VPE_HW_MAX_CARRIERS 3

/*!@brief define various bits for demodDebugFlag */
#define WFW_DEMOD_DEBUG_EWC_CAUSAL_ENERGY_OVERFLOW  (1 << 0)

/*! PICH log buffer indexes */
typedef enum
{
  WFW_DEMOD_PICH_ASYNC_READ_RXAGC_DONE_IDX=0,
  WFW_DEMOD_PICH_ASYNC_READ_SS_FRZ_TIME_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_END_POS_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_PI_BITS_START_POS_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_SCCPCH_POS_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_PICH_CFG_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_OFFL_START_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_OFFL_PROC_DONE_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_PI_DONE_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_QXT_DONE_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_SS_UNFRZ_TIME_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_UNFRZ_RXAGC_DONE_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_SCCPCH_ON,
  WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_ONL_RPOC_START_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_ON_OFF_DECISION_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_REAL_START_POS_IDX,
  WFW_DEMOD_PICH_ASYNC_READ_NUM_ELEM
} __attribute__((packed)) WfwDemodPichAsyncReadIdx;

/*! @brief Type definition for various physical channels combiners. Combiners are meant
 for R99 chanels that need multi cell and/or multi frame combining or have
 CCTrCh attached to it */
typedef enum 
{
  WFW_DEMOD_COMB_CHAN_TYPE_PCCPCH, /*!< P-CCPCH common channel */
  WFW_DEMOD_COMB_CHAN_TYPE_SCCPCH, /*!< S-CCPCH common channel */
  WFW_DEMOD_COMB_CHAN_TYPE_DPCH,   /*!< Dedicated physical channel */
  WFW_DEMOD_COMB_CHAN_TYPE_FDPCH,  /*!< Fractional Dedicated physical channel */
  WFW_DEMOD_COMB_NUM_CHAN_TYPE
} __attribute__ ((packed)) WfwDemodCombChanTypeEnum;


/*! @brief Physical channel configurations types */
typedef enum
{
  /*! Channel is disabled - Note: must be zero */
  WFW_DEMOD_PHCH_CFG_DISABLE = 0,
  /*! Channel is enabled */
  WFW_DEMOD_PHCH_CFG_ENABLE,
  /*! Channel recfg with no timing change*/
  WFW_DEMOD_PHCH_CFG_RECFG_NO_TIMING_CHANGE,
  /*! Channel recfg */
  WFW_DEMOD_PHCH_CFG_RECFG_TIMING_SLAM,
  /*! HICH/RGCH config used to indicate this 
      is HICH/RGCH channel. Individual action types
      are picked up from the
      WfwDemodEulRgHiChannelInfoStruct*/
  WFW_DEMOD_PHCH_CFG_EHIRGCH_CFG,  
  WFW_DEMOD_PHCH_NUM_CFG
} __attribute__ ((packed)) WfwDemodPhChActionTypeEnum;

/*! @brief Physical channel configurations types   
     for EHICH ERGCH channels. */
typedef enum
{
  /*! Channel is disabled - Note: must be zero */
  WFW_DEMOD_EHIRGCH_CFG_DROP = 0,
  /*! Channel is enabled */
  WFW_DEMOD_EHIRGCH_CFG_ADD,
  /*! Channel recfg with no timing change*/
  WFW_DEMOD_EHIRGCH_CFG_RECFG_NO_TIMING_CHANGE,
  /*! Channel recfg */
  WFW_DEMOD_EHIRGCH_CFG_RECFG,
  /*! Incicates no action for this channel but other (EHICH/ERGCH) 
  may have one. */
  WFW_DEMOD_EHIRGCH_CFG_NOOP,  
  WFW_DEMOD_EHIRGCH_CFG_NUM_CFG
} __attribute__ ((packed)) WfwDemodEHiRgChActionTypeEnum;

/*! @brief JUDC command configurations types */
typedef enum
{
  /*! JUDC diable command */
  WFW_DEMOD_JUDC_CFG_DISABLE = 0,
  /*! JUDC enable command */
  WFW_DEMOD_JUDC_CFG_ENABLE,
  /*! FACH to DCH seamless transition */
  WFW_DEMOD_JUDC_CFG_SEAMLESS_TRANS,
  WFW_DEMOD_JUDC_CFG_NUM_CFG
} __attribute__ ((packed)) WfwDemodJUDCActionTypeEnum;

/*! @brief Physical channel types */
typedef enum
{
  WFW_DEMOD_PHCH_TYPE_PCCPCH,   /*!< P-CCPCH common channel */
  WFW_DEMOD_PHCH_TYPE_SCCPCH,   /*!< S-CCPCH common channel */
  WFW_DEMOD_PHCH_TYPE_DPCH,     /*!< Dedicated Physical channel */
  WFW_DEMOD_PHCH_TYPE_FDPCH,    /*!< Fractional Dedicated Physical channel */
  WFW_DEMOD_PHCH_TYPE_PICH,     /*!< Paging indicator channel */
  WFW_DEMOD_PHCH_TYPE_AICH,     /*!< Access indicator channel */
  WFW_DEMOD_PHCH_TYPE_COMB,     /*!< DPCH combiner channel */
  WFW_DEMOD_PHCH_TYPE_HSDPA,    /*!< High Speed Data Packet Access channel */
  WFW_DEMOD_PHCH_TYPE_AGCH,     /*!< Absolute Grant channel */
  WFW_DEMOD_PHCH_TYPE_HIRGCH,   /*!< HICH/RGCH combined channel index */
  WFW_DEMOD_PHCH_TYPE_CPICH,    /*!< Common Pilot channel */
  WFW_DEMOD_PHCH_NUM_TYPE
} __attribute__ ((packed)) WfwDemodPhChTypeEnum;

/*! @brief CPICH Phase reference for channel */
typedef enum
{
  /*! phase reference invalid */
  WFW_DEMOD_PHCH_PHASE_REF_INVALID = 0,
  /*! phase reference if primary CPICH */
  WFW_DEMOD_PHCH_PHASE_REF_PRIMARY,
  /*! phase reference is secondary CPICH */
  WFW_DEMOD_PHCH_PHASE_REF_SEC,
  /*! phase reference is MIMO-mode SCPICH */
  WFW_DEMOD_PHCH_PHASE_REF_MIMO_SCPICH,
  /*! number of supported phase ref for same cell */
  WFW_DEMOD_PHCH_MAX_NUM_PHASE_REF
} __attribute__ ((packed)) WfwDemodPhChPhaseRefEnum;

/*! @brief Finger assignment modes */
typedef enum
{
  /*! Finger is not paired with any one */
  WFW_DEMOD_FING_PAIR_MODE_NONE,
  /*! Finger and its pair are both assigned together */
  WFW_DEMOD_FING_PAIR_MODE_ASSIGN_TOGETHER,
  /*! Finger is paired with other finger already assigned */
  WFW_DEMOD_FING_PAIR_MODE_ALREADY_ASSIGNED,

  WFW_DEMOD_FING_NUM_PAIR_MODE
} __attribute__ ((packed)) WfwDemodFingerPairAsignModeEnum;

/*! @brief Alternate scrambling code types */
typedef enum
{
  WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_NA,    /*!< Alternate scrambling not applicable */
  WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_LEFT,  /*!< Alternate scrambling code left */
  WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_RIGHT, /*!< Alternate scrambling code right */
  WFW_DEMOD_DPCH_NUM_PHCH_ALT_SCR_CODE
} __attribute__ ((packed)) WfwDemodPhChAltScrCodeTypeEnum;

/*! @brief Physical channel diversity modes */
typedef enum
{
  WFW_DEMOD_PHCH_DIV_MODE_NONE=0,      /*!< No diversity mode */
  WFW_DEMOD_PHCH_DIV_MODE_STTD=1,      /*!< Div mode STTD */
  WFW_DEMOD_PHCH_DIV_MODE_CLTD_TAM0=2, /*!< CLTD with timing adjustment mode 0 */
  WFW_DEMOD_PHCH_DIV_MODE_CLTD_TAM1=3, /*!< CLTD with timing adjustment mode 1 */
  WFW_DEMOD_PHCH_NUM_DIV_MODE
} __attribute__ ((packed)) WfwDemodPhChDivTypeEnum;

/*! @brief Cell Diversity Response Modes */
typedef enum
{
  WFW_DEMOD_CELL_DIV_MODE_INVALID,       /*!< Diversity mode invalid or hasnt completed */
  WFW_DEMOD_CELL_DIV_MODE_NOTD,          /*!< Div mode No-TD */
  WFW_DEMOD_CELL_DIV_MODE_TD,            /*!< Div mode TD */
  WFW_DEMOD_CELL_DIV_MODE_UNDECIDED,     /*!< Div mode is undecided */
  WFW_DEMOD_CELL_NUM_DIV_MODE
} __attribute__ ((packed)) WfwDemodCellDivInfoEnum;

/*! @brief Cell Diversity Response Modes */
typedef enum
{
  WFW_DEMOD_CELL_SCPICH_NONMIMO_MODE,    /*!< Non-MIMO-mode S-CPICH */
  WFW_DEMOD_CELL_SCPICH_MIMO_MODE,       /*!< MIMO mode S-CPICH */
  WFW_DEMOD_CELL_NUM_SCPICH_MODE
} __attribute__ ((packed)) WfwDemodCellScpichModeEnum;

/*! @brief HS CM control table bit fields */
typedef enum
{
  WFW_DEMOD_HS_CM_CTRL_RX_ACK_BIT = 0, /*!< Bit for RX N/ACK CM Overlap */
  WFW_DEMOD_HS_CM_CTRL_TX_ACK_BIT,     /*!< Bit for TX N/ACK CM Overlap */
  WFW_DEMOD_HS_CM_CTRL_TX_CQI_BIT,     /*!< Bit for TX CQI CM Overlap */
  WFW_DEMOD_HS_CM_CTRL_RX_CQI_BIT      /*!< Bit for RX CQI CM Overlap */
} __attribute__ ((packed)) WfwDemodHsCmCtrlTableBitFieldEnum;

/*! @brief Compressed mode method type enum */
typedef enum
{
  /*! Compressed mode by HLS */
  WFW_CM_CFG_CM_METHOD_HLS,
  /*! Compressed mode by SF reduction */
  WFW_CM_CFG_CM_METHOD_SF_RED,
} __attribute__ ((packed)) WfwCmCfgCmMethodEnum;

/*! @brief MIMO two beam mode enum */
typedef enum
{
  /*! MIMO two beam mode disable */
  WFW_MIMO_TWO_BEAM_MODE_DIS,
  /*! MIMO two beam mode enable in scpich only */
  WFW_MIMO_TWO_BEAM_MODE_EN_SCPICH,
  /*! MIMO two beam mode enable always */
  WFW_MIMO_TWO_BEAM_MODE_EN_ALWAYS
} __attribute__ ((packed)) WfwMimoTwoBeamModeEnum;

/*! @brief CQI / Demod algorithms */
typedef enum
{
  /* Symbol Equalizer CQI algo */
  WFW_NTC_PROC_CQI_DEMOD_ALGO_SEQ = 0,
  /* Chip Equalizer CQI algo */
  WFW_NTC_PROC_CQI_DEMOD_ALGO_CEQ,
  /* Symbol and Chip Equalizer CQI algo */
  WFW_NTC_PROC_CQI_DEMOD_ALGO_CEQ_SEQ,
  WFW_NTC_PROC_NUM_CQI_DEMOD_ALGO
} __attribute__ ((packed)) WfwNtcProcCqiAlgoTypeEnum;

/*! @brief DOB types */
typedef enum
{
  /* TD in DB_LOCAL_BUF */
  WFW_DEMOD_DOB_TYPE_TD = 0,
  /* DEPRECATED */
  WFW_DEMOD_DOB_TYPE_MEMPOOL = WFW_DEMOD_DOB_TYPE_TD,
  /* VD in DB_LOCAL_BUF */
  WFW_DEMOD_DOB_TYPE_VD = 1,
  /* UNUSED */
  WFW_DEMOD_DOB_TYPE_UNUSED = 2,
  /* VD in FW DECOB memory */
  WFW_DEMOD_DOB_TYPE_FW_VD = 3,

  WFW_NUM_DEMOD_DOB_TYPE
} __attribute__ ((packed)) WfwDemodDobTypeEnum;

/* Structure definitions */
/* --------------------- */

typedef struct{
  /*! @brief UMTS STMR time (BIT 0:18 = subFrameCx8, BIT 19-30 = frame) */
  UWord32 stmrTime[WFW_DEMOD_PICH_ASYNC_READ_NUM_ELEM];
}WfwDemodPichAsyncReadStruct;

/*! @name Structure definition for demod SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW Demod physical channel information update */
typedef struct
{
  /*! Bit 21:0 dirty bit bitmask for phch configuration */
  UWord32 dirtyBit;
  /*! Bit 21:0 double buffer bitmask for phch configuration */
  UWord32 nextDBIndex;

} WfwDemodPhyChanInfoSelStruct;

/*! @brief WCDMA FW Demod combiner channel information update */
typedef struct
{
  /*! Bit 8:0 dirty bit bitmask for combiner configuration */
  UWord32 dirtyBit;
  /*! Bit 8:0 double buffer bitmask for combiner configuration */
  UWord32 nextDBIndex;

} WfwDemodCombinerInfoSelStruct;

/*! @brief WCDMA FW Demod finger information update */
typedef struct
{
  /*! Bit 31:0 dirty bit bitmask for finger configuration */
  UWord32 dirtyBit;
  /*! Bit 31:0 double buffer bitmask for finger configuration */
  UWord32 nextDBIndex;

} WfwDemodFingerInfoSelStruct;

/*! @brief WCDMA FW Demod cell information update */
typedef struct
{
  /*! Bit 7:0 dirty bit bitmask for cell configuration */
  UWord32 dirtyBit;
  /*! Bit 7:0 double buffer bitmask for cell configuration */
  UWord32 nextDBIndex;

} WfwDemodCellInfoSelStruct;

/*! @brief WCDMA FW Demod R99 DL channel information */
typedef struct
{
  /* Word 2 */

  /*! Bit 7:0 Start CFN at action time */
  UWord8 startCfn;
  /*! Bit 15:8 Slot format for DPCH, FDPCH adn SCCPCH, not applicable for PCCPCH */
  UWord8 slotFormat;
  /*! Bit 23:16 CCTrCh index */
  UWord8 cctrchIdx;
  /*! Bit 31:24 OVSF code */
  UWord8 ovsfCode;

  /* Word 3 */

  /*! Bit 15:0 Cells with this combiner active */
  UWord16 :16;
  /*! Bit 23:16 Combienr Channel type, 2: DPCH, 3: FDPCH */
  WfwDemodCombChanTypeEnum combChanType;
  /*! Bit 31:24 Associated Combiner Range 0..9, 15 if not applicable */
  UWord8 combIndex;

  /* Word 4 */

  /*! Bit 7:0 Sec scr code used */
  UWord8 secScrCodeEn;
  /*! Bit 15:8 ASC for DPCH OVSF code 1 (only valid for DPCH) */
  UWord8 altScrCode;
  /*! Bit 23:16 CLTD AV group, index range from 0 to 5 */
  UWord8 cltdAvIndex;
  /*! Bit 31:24 TPC index (only valid for DPCH). Range 0..5 */
  UWord8 tpcIndex;

} WfwDemodR99DataControlChannelInfoStruct;


/*! @brief WCDMA FW Demod combiner channel information */
typedef struct
{
  /*! Bit 7:0 Start CFN at action time */
  UWord8  startCfn;
  /*! Bit 15:8 Slot format for DPCH, FDPCH adn SCCPCH, not applicable for PCCPCH */
  UWord8  slotFormat;
  /*! Bit 23:16 CCTrCh index */
  UWord8  cctrchIdx;
  /*! Bit 31:24 unused */
  UWord8  :8;

  /* Word 3 */

  /*! Bit 15:0 Cells with this combiner active */
  UWord16 activeCellBitmask;
  /*! Bit 23:16 Combienr Channel type, 2: DPCH, 3: FDPCH */
  WfwDemodCombChanTypeEnum combChanType;
  /*! Bit 31:16 unused */
  UWord8 :8;

} WfwDemodCombinerInfoStruct;


/*! @brief WCDMA FW Demod HSDPA DL channel information */
typedef struct
{
  /* Word 2 */

  /*! Bit 15:0  Starting subframe number */
  UWord16 startSubFrNum;
  /*! Bit 31:16  Final subframe number */
  UWord16 finalSubFrNum;

  /* Word 3 */

  /*! Bit 7:0 DPCH channel info index. Used to get DPCH offset for HS channel */
  UWord8  dpchChanInfoIndex;
  /*! Bit 15:8 HS DB Idx for control table and log packet. This is same as index
      passed in UL HS-DPCCH cfg. 0-1 = primary serving cell, 2-3=secondary serving cell */
  UWord8  dbIdx;
  /*! Bit 23:16 Sec scrambling code enable */
  UWord8  secScrCodeEn;
  /*! Bit 31:24 Soft buffer flush */
  UWord8  softBufferFlush;

  /* Word 4 */

  /*! Bit 15:0  CQI adjust in dB Q10 */
  Word16  cqiAdjustment;
  /*! Bit 23:16 MIMO two beam mode enable 
      0: Disable
      1: Enable in S-CPICH
      2: Enable always
  */
  WfwMimoTwoBeamModeEnum  mimoTwoBeamEn;          
  /*! Bit 31:24 Unused */
  UWord8  :8;
} WfwDemodHSDPAChannelInfoStruct;

/*! @brief WCDMA FW Demod PICH physical channel information */
typedef struct
{
  /* Word 2 */

  /*! Bit 7:0 Start CFN at action time */
  UWord8 startCfn;
  /*! Bit 15:8 unused */
  UWord8 :8;
  /*! Bit 23:16 unused */
  UWord8 :8;
  /*! Bit 31:24 OVSF code */
  UWord8  ovsfCode;

  /* Word 3 */

  /*! Bit 7:0 Bits per Indicator */
  UWord8  bitsPerInd:8;
  /*! Bit 15:8 ASAP Bit */
  UWord8  asap;
  /*! Bit 23:16 Unused */
  UWord8 rlpEnabled;
  /*! Bit 31:24 Unused */
  UWord8 :8;

} WfwDemodIndChannelInfoStruct;

/*! @brief WCDMA FW Demod EUL RGCH/HICH physical channel information */
typedef struct
{
  /* Word 2 */

  /*! Bit 10:0 Starting Subframe Number */
  UWord32 startSubFrNumHich:11;
  /*! Bit 21:11 Final Subframe Number */
  UWord32 finalSubFrNumHich:11;
  /*! Bit 23:22 Unused */
  UWord32 :2;
  /*! Bit 31:24 OVSF Code Number (shared for E-HICH and E-RGCH)*/
  UWord32 ovsfCode:8; 

  /* Word 3 */

  /*! Bit 10:0 Starting Subframe Number */
  UWord32 startSubFrNumRgch:11;
  /*! Bit 21:11 Final Subframe Number */
  UWord32 finalSubFrNumRgch:11;
  /*! Bit 25:22 Final Subframe Number, signed number, range -4~4 */
  Word32  rgchStmrActionTimeBpgAdj:4;
  /*! Bit 31:26 Unused */
  UWord32 :6;

  /* Word 4 */

  /*! Bit 7:0 Index into table to select Hardmard sequence for E-HICH */
  UWord8  hichSignature;
  /*! Bit 15:8 Index into table to select Hardmard sequence for E-RGCH 
        Can be same as E-HICH if E-RGCH inactive*/
  UWord8  rgchSignature;
  /*! Bit 13:11 E-RGCH action type 
      0: Drop
      1: Add
      2: Reconfig w timing chg
      3: Reconfig w/o timing chg
      4: No-op */
  UWord8  actionTypeRgch:3;
  /*! Bit 13:11 E-HICH action type 
      0: Drop
      1: Add
      2: Reconfig w timing chg
      3: Reconfig w/o timing chg
      4: No-op */
  UWord8  actionTypeHich:3;
  /*! Bit 22 TTI Type - 0=10ms, 1=2ms */
  UWord8  tti:1;
  /*! Bit 23 Serving Cell Indicator (Valid for RGCH) */
  UWord8  srvRls:1;
  /*! Bit 31:24 Timing offset of E-RGCH channel from DPCH.
               Set to 150 if ERGCH not active*/
  UWord8  rgchTransTimeOffset:8;

} WfwDemodEulRgHiChannelInfoStruct;

/*! @brief WCDMA FW Demod EUL AGCH physical channel information */
typedef struct
{
  /* Word 2 */

  /*! Bit 15:0 Starting Subframe Number */
  UWord16 startSubFrNum;
  /*! Bit 23:16 unused */
  UWord8 :8;
  /*! Bit 31:24 OVSF Code Number */
  UWord8  ovsfCode;

  /* Word 3 */

  /*! Bit 15:0 Final Subframe Number */
  UWord16 finalSubFrNum;
  /*! Bit 23:16 TTI Type - 0=10ms, 1=2ms */
  UWord8  tti;
  /*! Bit 31:24 unused */
  UWord8  :8;

  /* Word 4 */

  /*! Bit 15:0 Primary E-RNTI */
  UWord16 priERnti;
  /*! Bit 31:16 Secondary E-RNTI */
  UWord16 secERnti;

} WfwDemodEulAgChannelInfoStruct;

/*! @brief WCDMA FW Demod physical channel information */
typedef struct
{
  /* Common information of channels */

  /* Word 0 */

  /*! Bit 7:0 Channel Type */
  WfwDemodPhChTypeEnum channelType;
  /*! Bit 15:8 Cell index to which this channel belongs */
  UWord8  cellIndex;
  /*! Bit 23:16 Offset of this channel from PCCPCH. Range (0..149) */
  UWord8  transTimeOffset;
  /*! Bit 31:29 Action type Enable. Reslam, Disable, HICH/RGCH action type */
  WfwDemodPhChActionTypeEnum actionType;

  /* Word 1 */

  union
  {
    struct
    {
      /*! Bit 19:0 Action Time in W STMR counter, bit19:8 12 bit frame num (0~4095), bit7:0, bpg num (0~149) */
      UWord32 stmrActionTime:20;
      /*! Bit 20:20 Wait on SCCH order, used by FDPCH, HS and EUL DL channels */
      UWord32 waitOnScchOrder:1;
      /*! Bit 23:21 unused */
      UWord32 :3;
      /*! Bit 31:24 tx diversity type */
      WfwDemodPhChDivTypeEnum txDivType;
    };
    struct
    {
      /*! Bit 23:0 Action Time in W STMR counter, bit19:8 12 bit frame num (0~4095), bit7:0, bpg num (0~149) */
      UWord32 :20;
      /*! Bit 24:24 pchResume, used by SCCPCH */
      UWord32 pchResume:1;
      /*! Bit 23:21 unused */
      /*! Bit 31:24 tx diversity type */
      UWord32 :11;
    };
  };

  /*! Union of all types of channels */
  union
  {
    /*! R99 control/data channels */
    WfwDemodR99DataControlChannelInfoStruct r99DataControlChannelInfo;
    /*! R99 combiner for DPCH/FDPCH, not used by PCCPCH/SCCPCH */
    WfwDemodCombinerInfoStruct              combinerInfo;
    /*! HSDPA channel */
    WfwDemodHSDPAChannelInfoStruct          hsdpaChannelInfo;
    /*! Indicator channel */
    WfwDemodIndChannelInfoStruct            indChannelInfo;
    /*! HICH/RGCH channel */
    WfwDemodEulRgHiChannelInfoStruct        eulRgHiChannelInfo;
    /*! AGCH channel */
    WfwDemodEulAgChannelInfoStruct          eulAgChannelInfo;
  } channelInfo;

} WfwDemodPhyChanInfoStruct;


/*! @brief WCDMA FW Demod finger information */
typedef struct
{
  /* Word 0 */

  /*! Bit 18:0 Finger PN position in Cx8 units. Range 0..307200-1 */
  UWord32 pnPosCx8:19;
  /*! Bit 22:19 Cell index to which finger belongs. Range 0..MaxCells-1 */
  UWord32 cellIdx:4;
  /*! Bit 23 Force lock status to 0xF */
  UWord32 forceLockInit:1;
  /*! Bit 24:24 Unused */
  UWord32 :1;
  /*! Bit 26:25 Phase reference. 1 Primary, 2 Sec, 3 MIMO-Mode SCPICH (0 INVALID) */
  UWord32 phaseRef:2;
  /*! Bit 27 Quick freq and time tracking enabled. Range 0 or 1 */
  UWord32 qxtEnabled:1;
  /*! Bit 28 Finger freq tracking is started from ACQ mode. Range 0 or 1 */
  UWord32 ftInit:1;
  /*! Bit 29 Finger RSSI init. Range 0 or 1 */
  UWord32 rssiInit:1;
  /*! Bit 30 Finger is initialized. Range 0 or 1 */
  UWord32 init:1;
  /*! Bit 31 Indicate reference finger enabled or not. Range 0 or 1 */
  UWord32 enabled:1;

  /* Word 1 */

  /*! Bit 15:0 Initial RSSI */
  UWord32 initRssi:16;
  /*! Bit 31-16 INit freq error */
  Word32 initFreqError:16;

  /* Word 2 */

  /*! Bit 4:0 Finger Pair Id */
  UWord32 fingerPairId:5;
  /*! Bit 5 FTL Trk indicator. 1 if finger starts from ftl trk mode. 0 otherwise */
  UWord32 ftlTrkInd:1;
  /*! Bit 6 JTT enabled. Range 0 or 1 */
  UWord32 fingerJTTEnabled:1;
  /*! Bit 7 JFT enabled. Range 0 or 1 */
  UWord32 fingerJFTEnabled:1;
  /*! Bit 8 MMSE enabled. Range 0 or 1 */
  UWord32 fingerMMSEEnabled:1;
  /*! Bit 10:9 Pairing mode Padding */
  UWord32 pairAssignMode:2;
  /*! Bit 13:11 Antenna on which finger should operate. Range 0 or 3 */
  UWord32 ssId:3;
  /*! Bit 14:14 APF enabled. Range 0 or 1 */
  UWord32 enableAPF:1;
  /*! Bit 30:15 Unused */
  UWord32 :16;
  /*! Bit 31 RxD Init Range 0 or 1 */
  UWord32 rxdInit:1;

} WfwDemodFingerInfoStruct;

/*! @brief WCDMA FW Demod cell information */
typedef struct
{
  /* Word 0 */

  /*! Bit 4:0 Reference finger index. Range 0..31 */
  UWord32 refFingerPri:5;
  /*! Bit 5 Reference finger Valid or not. Range 0 or 1 */
  UWord32 refFingerPriValid:1;
  /*! Bit 6 Reference finger time tracking enabled. Range 0 or 1 */
  UWord32 refFingerPriTTEnabled:1;
  /*! Bit 7 CPICH STTD enabled. Range 0 or 1 (1 bits) */
  UWord32 cpichSTTDEnabled:1;

  /* Following are valid if secCPICHenabled is TRUE */

  /*! Bit 12:8 Reference finger index. Range 0..31 */
  UWord32 refFingerSec:5;
  /*! Bit 13 Reference finger Valid or not. Range 0 or 1 */
  UWord32 refFingerSecValid:1;
  /*! Bit 14 Reference finger time tracking enabled. Range 0 or 1 */
  UWord32 refFingerSecTTEnabled:1;
  /*! Bit 15 Unused */
  UWord32 :1;
  /*! Bit 23:16 Sec CPICH OVSF code. Range 0..255 */
  UWord32 secCPICHovsfCode:8;
  /*! Bit 24 Sec CPICH Enabled, use secScrCodeIdx. Range 0 or 1 */
  UWord32 secCPICHenabled:1;
  /*! Bit 25 Sec CPICH secondary scrambling code select */
  UWord32 secCPICHSecScrCodeSel:1;
  /*! Bit 26 Sec CPICH secondary mode - 0=non-MIMO mode, 1=MIMO mode */
  UWord32 secCPICHMode:1;
  /*! Bit 30:27 Unused */
  UWord32 :4;
  /*! Bit 31 Cell enabled */
  UWord32 enabled:1;

  /* Word 1 */

  /*! Bit 8:0 CPICH primary scrambling code. Range 0..511 */
  UWord32 priScrCode:9;
  /*! Bit 15:9 Unused */
  UWord32 :7;
  /*! Bit 19:16 sec scrambling code. Range 0..15 (4 bits) */
  UWord32 secScrCodeIdx:4;
  /*! Bit 22:20 Unused */
  UWord32 :3;
  /*! Bit 23 SCH in TSTD mode. Range 0 or 1 */
  UWord32 schTSTDEnabled:1;
  /*! Bit 24 Invert SCH (if P-CCPCH is STTD) */
  UWord32 schInvert:1;
  /*! Bit 25 Enable flag for TD determination, valid at initial cell configuration
      If set, schInvert and schTSTDEnabled fields are ignored */
  UWord32 tdDetEn:1;
  /*! Bit 26 Dirty bit to indicate whether to perform TD determination, 
      Only perform TD determination when dirty bit is set */
  UWord32 tdDetDirtyBit:1;
  /*! Bit 28:27 carrier Idx*/
  UWord32 carrierIdx:2;
  /*! Bit 31-29 Unused */
  UWord32 :3;

  /*! SCPICH Power Imbalance scale in Q14 */
  Word16 mimoScpichImbScale;

} WfwDemodCellInfoStruct;

/*! @brief Indicator channel variable required for indicator demod and accum */
typedef struct
{
  /* indicator channel demod and accum input */

  struct
  {
    /*! threshold for ind detection to pass */
    Word32 indThreshold;
    /*! Number of indicators */
    UWord8 numInd;
    /*! ind pn count - BPG relative to ind channel timing (0..149) */
    UWord8 indPnCount[WFW_DEMOD_NUM_MAX_INDICATOR_BUF];
  } input;

  /* ind related output */

  /*! PI detection decision 0 or 1*/
  struct
  {
    UWord8 indValue;
    /*! PI symbol accumulation */
    Word32 indSum;
    /*! RSSI accumulation during indicator */
    Word32 rssiSum;
  } result[WFW_DEMOD_NUM_MAX_INDICATOR_BUF];
} WfwDemodIndInfoStruct;

/*! @brief WCDMA FW Demod equalizer information */
typedef struct
{
  /* Word 0 */

  /*! Bit 0: Indicate EQ enabled. not used by FW. SW usage only */
  UWord32 enabled : 1;
  /*! Bit 1: Initialize EQ, clear filter buffers. unused now */
  UWord32 init : 1;
  /*! Bit 2: Indicate master EQ */
  UWord32 master : 1;
  /*! Bit 3: Rx diversity enable */
  UWord32 rxdEnable : 1;
  /*! Bit 4: QICE enabled. used by FW */
  UWord32 qiceEn : 1;
  /*! Bit 5: PSC enabled. unused now*/
  UWord32  : 1;
  /*! Bit 6: SSC enabled. unused now */
  UWord32  : 1;
  /*! Bit 7: Do reframing */
  UWord32 reframe : 1;
  /*! Bit 10:8: Sample server select. Ignored if rxdEnable set.unused now */
  UWord32 ssId : 3;
  /*! Bit 11: Rx Memory select. Ignored if qiceEnable is not set. unused now */
  UWord32  : 1;
  /*! Bits 15:12: Cell index for this EQ. */
  UWord32 cellIdx : 4;
  /* Bits 31:16: Starting PN position, Cx1 units. */
  UWord32 pnPosCx1 : 16;

  /* Word 1 */

  /*! Bits 1:0 iteration number for this cell */
  UWord32 numIter : 2;
  /*! Bit 31:3 : Reserved */
  UWord32         : 30;

} WfwDemodEqInfoStruct;

/*! @brief WCDMA FW Demod Adaptive Rx Diversity information */
typedef struct
{
  /*! Bit 5:0 Measurement time number of subframe */
  UWord32 numSubFrms   :6;

  /*! Bit 13:6 : Reserved */
  UWord32              :8;

  /*! Bit 14 : Rx type to perform measurement */
  UWord32 receType     :1;

  /*! Bit 15 : ARD Enable bit */
  UWord32 ardEnable    :1;

  /*! Bit 31:16 : Reserved */
  UWord32              :16;

} WfwDemodArdInfoStruct;
#define WFW_DEMFRONT_ARD_RX_RAKE 1
#define WFW_DEMFRONT_ARD_RX_QICE 0

/*! @brief WCDMA FW Demod Adaptive Rx Diversity V2 information */
typedef struct
{
  /*! Bit 29:0 : Reserved */
  UWord32          :30;

  /*! Bit 30 : Indicator from SW to Freeze the ARDv2 Filters */
  UWord32 rxDFiltFreeze    :1;
  /*! Bit 15 : ARD Enable bit */
  UWord32 ardOn    :1;
} WfwDemodArdV2InfoStruct;
/*! @brief WCDMA FW Demod program CIR enhancements through NV*/
typedef struct
{
  /*Bit0: Force CIR interpolation on/off*/
  UWord32 cirInterpEnable            :1;
  /*Bit1:8: set the CIR filter coefficients as follows: [b0 b1 a0 a1] = [alpha 0 (128-alpha) 0]*/ 
  UWord32 oneTapCirAlpha             :8;
  /*Bit9: Controls whether or not Doppler estimation algorithm runs*/
  UWord32 enableDopplerEstimation    :1;
  /*Bit19:10If EnableDopplerEstimation = 0, this value is used as the Doppler estimate (Hz)*/
  UWord32 doppler                    :10;
   /*! Bits 31:20 Reserved */
  UWord32                            :12;
  /*If sigmaScale > 0, it is used in CIR cleaning in place of adaptive value computed by FW*/
  UWord16 sigmaScale;

 /* cirCleanThresh to be controlled by NV Item, Default value should be 20000 */
  UWord16 cirCleanThresh; 

} WfwDemodCirEnhancementStruct;


/*! @brief WCDMA FW Demod common information */
typedef struct
{
  /*! Bit 0: Reserved */
  UWord32             : 1;
  /*! Bits 3:1: bit field for demodulation path selection for R99 channel of
      a Master cell.
     Bit 0 = Rake, Bit 1 = LEQ, Bit 2 = QICE */
  UWord32 r99DemPath : 3;
  /*! Bits 6:4: bit field for demodulation path selection for HS channel
      Bit 0 = Rake, Bit 1 = LEQ, Bit 2 = QICE */
  UWord32 hsDemPath  : 3;
  /*! Bits 15:7: Reserved */
  UWord32            : 9;
  /*! Bit 18:16: Each bit indicates demodulation path on which OHC IC is to be enabled.
      Bit 0 = Rake, Bit 1 = reserved, Bit 2 = QICE */
  UWord32 schIcMask : 3;
  /*! Bits 22:19: Reserved */
  UWord32 schIcCellIdx  : 4;
  /*! Bits 23:23: Each bit is set indicate whether to enable schR99En */
  UWord32 schR99En     : 1;
  /*! Bits 31:24: Reserved */
  UWord32            : 8;

} WfwDemodCommonInfoStruct;

/*! @brief WCDMA FW Demod QTT & QFT information */
typedef struct
{
  /*! QFT threshold for valid energy (Range 0->2^31-1 */
  UWord32 qftEnergyThresh;
  /*! Bits 15:0 Reserved */
  UWord32 reserved : 16;
  /*! Bits 23:16 QFT Integration Length (Range 4->127) */
  UWord32 qftIntLen : 8;
  /*! Bits 27:24 QTT Coherent Integration Length (Range 4->15) */
  UWord32 qttCohIntLen : 4;
  /*! Bits 31:28 QTT Non-Coherent Integration Length (Range 1->15) */
  UWord32 qttNonCohIntLen : 4;
} WfwDemodQxtInfoStruct;

/*! @brief WCDMA FW Demod Antenna Verification information */
typedef struct
{
  /*! @brief Timer threshold for av init state counter.
      Used to switch out of init state after this number of slots. */
  Word16 avInitStateTimerThresh;
  /*! @brief Timer threshold for av error counter.
      Used to switch out of init state after this number of AV errors. */
  Word16 avInitCntThresh;
} WfwDemodAvAsyncWriteStruct;

/*! @brief WCDMA FW Demod Antenna Verification information */
typedef struct
{
  /* @brief AV Error Histogram for previous frame per TPC group / CLTD TAM.
      Each slot is 2-bit s.t.: 0->no AV error, 1->AV error in imag only,
      2->AV error in real only, 3->AV error in real/imag.  This is mapped as:
      BIT(29:28)->slot0 (I,Q), BIT(27:26)->slot1 (I,Q) ... BIT(1:0)->slot14 (I,Q) */
  UWord32 errHist;
  /* @brief NAV Histogram for previous frame per TPC group / CLTD TAM.
      Each slot is 2-bit s.t.: 0->no AV error, 1->AV error in imag only,
      2->AV error in real only, 3->AV error in real/imag.  This is mapped as:
      BIT(29:28)->slot0 (I,Q), BIT(27:26)->slot1 (I,Q) ... BIT(1:0)->slot14 (I,Q) */
  UWord32 navHist;
  /*! AV threshold */
  Word32  thresh;
  /*! Omega value for thresh calculation */
  Word32  omega;
  /*! PDLQ value for thresh calculation */
  Word16  pdlq;
  /*! PJTQ value for thresh calculation */
  Word16  pjtq;
} WfwDemodAvPerAvAsyncReadStruct;

/*! @brief WCDMA FW Demod Antenna Verification information */
typedef struct
{
  /*! Buffer for per-TPC per-timing mode information */
  WfwDemodAvPerAvAsyncReadStruct av[6][WFW_DEMOD_NUM_CLTD_TIMING_MODE];
  /*! CLTD init state counter (timer used to exit init mode) */
  UWord8  initStateTimer;
  /*! CLTD init counter (counts consecutive AV failures in init mode) */
  UWord8  initCnt;
} WfwDemodAvAsyncReadStruct;

/*! @brief WCDMA FW Demod AICH async write information */
typedef struct
{
  /*! @brief AICH ACK threshold */
  Word16 aiAckThresh;
  /*! @brief AICH NACK threshold */
  Word16 aiNackThresh;
} WfwDemodAichWriteStruct;

/*! WCDMA FW Demod CPP async write information */
typedef struct
{
  /*! RSSI lock power threshold - nominally v1.0: 147, v1.1: 588 */
  Word32 cppRssiLockPowerThresh;
  /*! RSSI lock upper threshold - nominally v1.0: 33, v1.1: 130 */
  Word32 cppRssiLockUpperThresh;
  /*! RSSI lock lower threshold - nominally v1.0: 33, v1.1: 130 */
  Word32 cppRssiLockLowerThresh;
  /*! RSSI Dynamic lock threshold high limit - nominally -18dB v1.0: 260, v1.1: 1039 */
  Word32 cppRssiDynLockHighLimit;
  /*! RSSI Dynamic comb lock threshold low limit - nominally -27dB v1.0: 33, v1.1: 130 */
  Word32 cppRssiDynLockCombLowLimit;
  /*! RSSI Dynamic power lock threshold low limit - nominally -20dB v1.0: 147, v1.1: 588 */
  Word32 cppRssiDynLockPowerLowLimit;
  /*! RSSI Dynamic comb lock offset shift (2,3,4 -> 6,9,12dB) - nominally 3 */
  Word8  cppRssiDynLockCombOffset;
  /*! RSSI Dynamic lock detect enable - nominally TRUE */
  Word8  cppRssiDynLockDetectEn;
  /*! APF beta in Q32 - nominally 0 */
  Word32 cppApfBeta;
  /*! FTL Gain in Q24 - nominally 2148 */
  Word32 cppFtlAccumAcqFbFactor;
  /*! FTL Gain in Q24 - nominally 271 */
  Word32 cppFtlAccumTrkFbFactor;
  /*! TTL Gain in Q32 - nominally for Cx8: 9830 */
  Word32 cppTtlAccumFbFactor;
  /*! TTL Upper RSSI switching threshold - nominally v1.0: 802, v1.1: 3208 */
  Word32 cppTtlUpperSwitchThresh;
  /*! TTL Lower RSSI switching threshold - nominally v1.0: 637, v1.1: 2548 */
  Word32 cppTtlLowerSwitchThresh;
  /*! TTL Cx32 threshold - nominally 133334 */
  Word32 cppTtlThreshCx32;
  /*! TTL slope threshold - nominally 10240 */
  Word16 cppTtlSlopeThresh;
  /*! TTL slope check count - nominally 3 */
  Word8  cppTtlSlopeCheckCnt;
  /*! TTL approx divide shift factor - nominally 16 */
  Word8  cppTtlNormFactor;
  /*! TTL switch delay in 6-BPG units - nominally 5 (2ms) */
  Word8  cppTtlSwitchDelayTime;
  /*! FTL approx divide shift factor - NOT USED */
  Word8  cppFtlNormFactor;
} WfwDemodCppInfoStruct;

/*! @brief WCDMA FW Demod EUL async write structure */
typedef struct
{
  /*! @brief Even frame compressed mode per slot info bit mask per cell */
  UWord16 rgchCmSlotBitMask0[WFW_DEMOD_NUM_MAX_CELLS];
  /*! @brief Odd frame compressed mode per slot info bit mask per cell */
  UWord16 rgchCmSlotBitMask1[WFW_DEMOD_NUM_MAX_CELLS];
  /*! @brief Even frame compressed mode per slot info bit mask per cell */
  UWord16 hichCmSlotBitMask0[WFW_DEMOD_NUM_MAX_CELLS];
  /*! @brief Odd frame compressed mode per slot info bit mask per cell */
  UWord16 hichCmSlotBitMask1[WFW_DEMOD_NUM_MAX_CELLS];
  /*! @brief Even frame compressed mode per slot info bit mask */
  UWord16 agchCmSlotBitMask0;
  /*! @brief Odd frame compressed mode per slot info bit mask */
  UWord16 agchCmSlotBitMask1;
  /*! @brief AGCH stop due to reconfiguration Flag */
  UWord16 agchStopThruRecfg[WFW_NUM_MAX_UPLINK_CARRIERS];
}WfwDemodEulAsyncWriteStruct;


typedef struct
{
   /*  Word1 */
   /*! @brief agch 6 bit info */
  UWord32 agchData : 6;          /* Bit 0-5 */
  /*! @brief  Secondary ernti pass flag*/
  UWord32 S : 1;              /* Bit 6 */
  /*! @brief  primary ernti pass flag*/
  UWord32 P : 1;              /* Bit 7 */
  /*! @brief Subframe Number */
  UWord32 subframeNum : 11;     /* Bit 8-18 */
  /*! @brief symbol error cnt */
  UWord32 decodeErrCnt : 7;    /* Bit 20-26 */
  /*! @brief quality Bit */
  UWord32 qualityBit : 1;    /* Bit 27 */
  /*! @brief Indicate AGCH decode HW on flag */
  UWord32 agchDecodeHwOnFlag : 1; /* Bit 28 */
  /*! @brief unsued*/
  UWord32 : 4;

  /* Word2 */
  /*! @brief energyMetric */
  UWord32 decodeStat : 16;       /* Bit 0-15 */
  /*! @brief Masked CRC */
  UWord32 maskedCRC : 16;        /* Bit 16:31 */

  /* Word3 */
  /*! @brief Cm Slot Bit Mask */
  UWord32 cmSlotBitMask : 16;     /* Bit 0-15*/
  /*! @brief Carrier Index */
  UWord32 carrierIdx : 2;         /* Bit 16-17*/
  /*! @brief unsued */
  UWord32 : 14;

  /* Word4 */
  /*! @brief Pilot Energy */
  UWord32 pilotEnergy : 32;       /* Bit 0-31 */

}WfwAgchRsltStruct;

typedef struct
{
 /*  Word1 */
  /*! @brief Subframe Number */
  UWord32 subframeNum : 11;       /* Bit 0-10 */
  /*! @brief Cell Number Rgch/hich associated with */
  UWord32 cellNum : 4;            /* Bit 11-14*/
  /*! @brief RGCH valid slots */
  UWord32 validSlotsPerIndCh : 4; /* Bit 15-18 */
  /*! @brief carrier Idx */
  UWord32 carrierIdx : 2;         /* Bit 19-20 */
  /*! @brief unsued*/
  UWord32 : 11;                   /* Bit 21-31 */

  /*! Word2 */
  /*! @brief RGCH/HICH result */
  Word32 result : 32;             /* Bit 0-31 */

  /*! Word3 */
  UWord32 pilotEnergy : 32;       /* BIT 0-31 */

  /*! Word4 */
  UWord32 pilotEnergyRx0 : 32;       /* BIT 0-31 */
  /*! @brief HICH/RGCH sym energy accumulated over TTI */
  UWord32 symEnergy : 32;         /* BIT 0-31 */
}WfwHiRgRsltStruct;

/*! @brief WCDMA FW Demod EUL async read structure */
typedef struct
{
  WfwAgchRsltStruct agchSwRslt[WFW_DEMOD_NUM_MAX_AGCH_RD_RST_BUF_SIZE];
  /*index to the result struct updated for every write*/
  UWord8 agchWrIdx;

  WfwHiRgRsltStruct rgchSwRslt[WFW_DEMOD_NUM_MAX_RGHI_RD_RST_BUF_SIZE];
  /*index to the result struct updated for every write*/
  UWord8 rgchWrIdx;

  WfwHiRgRsltStruct hichSwRslt[WFW_DEMOD_NUM_MAX_RGHI_RD_RST_BUF_SIZE];
  /*index to the result struct updated for every write*/
  UWord8 hichWrIdx;

}WfwDemodEulAsyncReadStruct;

/*! @brief WCDMA FW Demod per-cell async read information structure */
typedef struct
{
  /*! Transmit diversity accumulation returned per cell */
  Word32 divAccum;
  /*! Transmit diversity status returned per cell
      0:Not Ready, 1:No-STTD & No-TSTD, 2:STTD & TSTD, 3:Undecided */
  UWord8 divInfo;

} WfwDemodCellAsyncReadStruct;

/*! @brief WCDMA FW Demod info DB buffer */
typedef struct
{
  /* Demod information (SW WO) section */
  /*************************************/

  /*! cell info database */
  WfwDemodCellInfoStruct     cellInfo[WFW_DEMOD_NUM_MAX_CELLS];
  /*! finger info database */
  WfwDemodFingerInfoStruct   fingerInfo[WFW_DEMOD_NUM_MAX_FINGERS];
  /*! channel info database */ /* NOTE! KEEP CHANNEL AND COMBINER CONSECUTIVE AND IN ORDER */
  WfwDemodPhyChanInfoStruct  channelInfo[WFW_DEMOD_NUM_MAX_PHCHS];
  /*! combiner info database */
  WfwDemodPhyChanInfoStruct  combinerInfo[WFW_DEMOD_NUM_MAX_COMBINERS];

  /*! PICH demod info */
  WfwDemodIndInfoStruct pichDemodInfo;

  /*! MICH demod info */
  WfwDemodIndInfoStruct michDemodInfo;

  /*! EQ info database */
  WfwDemodEqInfoStruct eqInfo[WFW_DEMOD_NUM_MAX_EQS];

  /*! Common info database */
  WfwDemodCommonInfoStruct commonInfo[WFW_DEMOD_NUM_MAX_CARRIERS];

  /*! ARD info database */
  WfwDemodArdInfoStruct ardInfo;

  /*! ARD V2 info database */
  WfwDemodArdV2InfoStruct ardV2Info;

  /*CIR enhancements through NV*/
  WfwDemodCirEnhancementStruct cirEnhancementReadBuf;

  /*! Quick Time and Frequency Tracking database */
  WfwDemodQxtInfoStruct qxtInfo;

  /*! Antenna verification async write interface */
  WfwDemodAvAsyncWriteStruct avAsyncWrite;

  /*! AICH demod info */
  WfwDemodAichWriteStruct aichDemodInfo;

  /*! CPP info */
  WfwDemodCppInfoStruct cppInfo;

  /*! EUL async variable write struct */
  WfwDemodEulAsyncWriteStruct eulDemodWrInfo;

  /*! total number of shadow cells granted for primary carrier */
  UWord8 numShadowCellGrantedPriCarrier;

  /*! various demod debug flags used in field testing */
  UWord8 demodDebugFlag;

  UWord8 padding[2];

  /* Demod information (SW RO) section */
  /*************************************/

  /* TODO: Cleanup - Move this to SW RO area in wfw_sw_intf.h */

  /*! Demod pending cfg flag for various resources */
  UWord32 cellCfgPending;
  UWord32 fingCfgPending;
  union
  {
    UWord64 chanCombCfgPending;
    struct
    {
      UWord64  chanCfgPending : WFW_DEMOD_NUM_MAX_PHCHS;
      UWord64  combCfgPending : WFW_DEMOD_NUM_MAX_COMBINERS;
    };
  };
  /* 3 bits per carrier. 
     For carrier n, bit 3n is for eq 0, bit 3n+1 is for eq 1 and bit 3n+2 is for eq2. 
     For 3 carriers, 9 bits total. */
  UWord32 eqCfgPending;
  /* 2 bits per carrier. 
     For carrier n (n = 0,1 or 2) bit 2n is for receiver switch. bit 2n+1 is for sch-ic */
  UWord32 commonCfgPending;

  /*! total number of shadow cells FW requests for primary carrier */
  UWord8 numShadowCellRequestedPriCarrier;

  /*! Per-cell demod async read values */
  WfwDemodCellAsyncReadStruct cellAsyncRead[WFW_DEMOD_NUM_MAX_CELLS];

  /*! Difference between VSRC and STMR per WB in cx8 or cx16 (DC) */
  Word16 refCntAdj[2];

  /*! Antenna verification async read interface */
  WfwDemodAvAsyncReadStruct avAsyncRead;

  /*! EUL async variable read struct */
  WfwDemodEulAsyncReadStruct eulDemodRdInfo;

  /*! PICH logging async read struct */
  WfwDemodPichAsyncReadStruct pichAsyncReadInfo;

} WfwDemodInfoInterfaceStruct;
/*!@}*/

/*! @brief WCDMA FW Demod status dump per-carrier information structure */
typedef struct
{
  /*! raw CQI in Q4 units, same as the one for DL logging */
  UWord16 rawCqiQ4[WFW_DEMOD_NUM_RECE];

} WfwDemodStatusDumpCarrStruct;

/*! @brief WCDMA FW Demod status dump per-cell information structure */
typedef struct
{
  /* EQ position in Cx1 */
  UWord16 eqPosCx1;

} WfwDemodStatusDumpCellStruct;

/*! @brief WCDMA FW Demod status dump per-finger information structure */
typedef struct
{
  /*! finger PN position in Cx32 units */
  UWord32 pnPosCx32 : 22;
  /*! finger lock status 4 bits:
      Bit 0: Finger lock detect state bit
      Bit 1: Finger time tracking lock state bit
      Bit 2: Finger combiner lock state bit
      Bit 3: Finger power lock state bit */
  UWord32 lockStatus : 4;
  /*! TTL step size: 0-Cx8, 1-Cx32 */
  UWord32 ttlStepSize : 1;
  /*! interpolator taps: 0-4taps, 1-6taps */
  UWord32 sixTapEn : 1;
  /*! reserved */
  UWord32 reserved : 4;

  /*! finger RSSI for TxD antenna 0 and 1 */
  UWord32 rssi0 : 16;
  UWord32 rssi1 : 16;

  /*! Finger TCPF in Q16 units */
  UWord32 tcpfSq0 : 16;
  UWord32 tcpfSq1 : 16;

  /*! finger frequency error estimate, Units = 7.3242/16 Hz per 1 LSB */
  Word16 freqError;
  /*! APF alpha in Q15 */
  UWord16 apfAlpha;
  /*! vrt to cell bpg offset */
  UWord8  vrtCellBpgOffset;
  /*! cell type */
  UWord8  cellRscType;
} WfwDemodStatusDumpFingStruct;

/*! @brief WCDMA FW Demod status dump information structure */
typedef struct
{
  /*! Reference counter in Cx8 units,
      BIT(0:18) = Cx8 Cnt in frame, BIT(19:31) = frame count */
  UWord32 refCnt;

  /*! Tx system time and frame number up to 4096 frames
      BIT(0:18) = Cx8 Cnt in frame, BIT(19:31) = frame count */
  UWord32 txCnt;

  /*! VRT pn in cx1 */
  UWord32 vrtPnPos;

  /*! TX HW/FW BPG alignment Drift, range (-1024Cx8, 1024Cx8) */
  Word16 txHwBpgDrift;

  /*! Per-finger demod status information */
  WfwDemodStatusDumpFingStruct fing[WFW_DEMOD_NUM_MAX_FINGERS];

  /*! Per-cell demod status information */
  WfwDemodStatusDumpCellStruct cell[WFW_DEMOD_NUM_MAX_CELLS];

  /*! Per-carrier demod status information */
  WfwDemodStatusDumpCarrStruct carr[WFW_DEMOD_NUM_MAX_CARRIERS];

  /*! combiner counter in Cx1 units */
  UWord16 combinerCountCx1[WFW_DEMOD_NUM_MAX_COMBINERS_STATUS_DUMP];
} WfwDemodStatusDumpStruct;


/*! @brief WCDMA FW CCTrCh related tables */
typedef struct
{
  union
  {
    UWord32 cfn[WFW_DEMOD_NUM_MAX_PHCH_COMBINERS];
    UWord32 subFrame[WFW_DEMOD_NUM_MAX_PHCH_COMBINERS];
  };
} WfwDemodChannelTakenActionTimeStruct;

/*! @brief WCDMA FW CCTrCh related tables */
typedef struct
{
  UWord32 numTrch:5;                 /* Bits 4:0 */
  UWord32  :10;                      /* Bits 14:5 */
  UWord32 useTfci:1;                 /* Bit 15 */
  UWord32 drmbStartAddress:15;       /* Bits 30:16 */
  UWord32  :1;                       /* Bit 31 */
} WfwDembackCCTrChTableDWord0Struct;

/*! Maximun number of TF supported for TrChA BTFD */
#define WFW_DBACK_MAX_NUM_TRCH_SUBTYPE 3
/*! Q factor for DCCH ED threshold */
#define WFW_DBACK_DCCH_ED_THRESH_SHIFT 8

typedef struct
{
  /* Word 0 */
  UWord32 numTF:7;                   /* Bits 6:0 */
  UWord32 MBMSIndex:3;               /* Bits 9:7 */
  UWord32 TFBase:6;                  /* Bits 15:10 */
  UWord32 codeType:2;                /* Bits 17:16 */
  UWord32 crcLength:3;               /* Bits 20:18 */
  UWord32 TTI:2;                     /* Bits 22:21 */
  UWord32 maxTfId:6;                 /* Bits 28:23 */
  /*! Indicate this TrCh is for DCCH */
  UWord32 dcch:1;                    /* Bit 29 */ 
  UWord32 disable:1;                 /* Bit 30 */
  UWord32 mbmsIndicator:1;           /* Bit 31 */
  
} WfwDembackTrChTableStruct;

typedef union
{
  struct
  {
    /* Word 0 */
    UWord32 tfi0:6;                    /* Bits 5:0 */
    UWord32  :2;                       /* Bits 7:6 */
    UWord32 tfi1:6;                    /* Bits 13:8 */
    UWord32  :2;                       /* Bits 15:14 */
    UWord32 tfi2:6;                    /* Bits 21:16 */
    UWord32  :2;                       /* Bits 23:22 */
    UWord32 tfi3:6;                    /* Bits 29:24 */
    UWord32  :1;                       /* Bit 30 */
    UWord32 invalid:1;                 /* Bit 31 */

    /* Word 1 */
    UWord32 tfi4:6;                    /* Bits 5:0 */
    UWord32  :2;                       /* Bits 7:6 */
    UWord32 tfi5:6;                    /* Bits 13:8 */
    UWord32  :2;                       /* Bits 15:14 */
    UWord32 tfi6:6;                    /* Bits 21:16 */
    UWord32  :2;                       /* Bits 23:22 */
    UWord32 tfi7:6;                    /* Bits 29:24 */
    UWord32  :2;                       /* Bits 31:30 */

    /* Word 2 */
    UWord32 tfi8:6;                    /* Bits 5:0 */
    UWord32  :2;                       /* Bits 7:6 */
    UWord32 tfi9:6;                    /* Bits 13:8 */
    UWord32  :2;                       /* Bits 15:14 */
    UWord32 tfi10:6;                   /* Bits 21:16 */
    UWord32  :2;                      /* Bits 23:22 */
    UWord32 tfi11:6;                   /* Bits 29:24 */
    UWord32  :2;                      /* Bits 31:30 */
  } tfciPoolTableStruct;
  UWord32 v[WFW_DBACK_TFCI_POOL_TABLE_SIZE];
} WfwDembackTFCIPoolTableStruct;

typedef struct
{
  UWord32 tfcIndex[WFW_DBACK_NUM_DWORDS_IN_TFC_INFO_TABLE];
  UWord32 numTfci;
} WfwDembackTfcInfoTableStruct;

typedef struct
{
  WfwDembackCCTrChTableDWord0Struct dembackCCTrChTableDWord0;
  UWord32                           dembackTTIorderTable[WFW_DBACK_NUM_DWORDS_IN_TTI_ORDER_TABLE];
  WfwDembackTrChTableStruct         dembackTrChTable[WFW_DBACK_NUM_TRCH_PER_CCTRCH];
  UWord32                           dembackTFInfoTable[WFW_DBACK_NUM_DWORDS_IN_TF_INFO_TABLE];
  WfwDembackTfcInfoTableStruct      dembackTFCInfoTable;
} WfwDembackCCTrChTableStruct;

typedef struct
{
  /* Word 0 */
  UWord32 :10;                       /* Bits 9:0 */
  UWord32 numTrBlk:6;                /* Bits 15:10 */
  UWord32 rateMatchedSizePerRadioFrame:15; /* Bits 30:16 */
  UWord32 :1;                        /* Bits 31 */

  /* Word 1 */
  UWord32 trBlkSize:13;              /* Bits 12:0 */
  UWord32 :3;                        /* Bits 15:13 */
  UWord32 codeBlkSize:13;            /* Bits 28:16 */
  UWord32 :3;                        /* Bits 31:29 */

  /* Word 2 */
  UWord32 ePlusOverA:19;             /* Bits 18:0 */
  UWord32 :13;                       /* Bits 31:19 */

  /* Word 3 */
  UWord32 eMinusOverA:18;            /* Bits 17:0 */
  UWord32 :1;                        /* Bit 18 */
  UWord32 rmType:2;                  /* Bits 20:19 */
  UWord32 yamThreshold:4;            /* Bits 24:21 */
  UWord32 numFillerBits:6;           /* Bits 30:25 */
  UWord32 deltaNOdd:1;               /* Bit 31 */

  /* Word 4 */
  UWord32 maxIterationNum:4;       /* Bits 3:0 */
  UWord32 primeNumIndex:6;         /* Bits 9:4 */
  UWord32 numCodeBlk:5;            /* Bits 14:10 */
  UWord32 :1;                      /* Bit 15 */
  UWord32 numColumn:8;             /* Bits 23:16 */
  UWord32 numRowIndex:2;           /* Bits 25:24 */
  UWord32 lastRowBitExch:1;        /* Bit 26 */
  UWord32 drmScaleFactor:2;        /* Bits 28:27 */
  UWord32 :3;                      /* Bits 31:29 */

  /* Word 5 */
  UWord32 lastColumn:8;            /* Bits 7:0 */
  UWord32 lastRow:5;               /* Bits 12:8 */
  UWord32 :3;                      /* Bits 15:13 */
  UWord32 primeNum:9;              /* Bits 24:16 */
  UWord32 turboBetaSel:4;          /* Bits 28:25 */
  UWord32 :3;                      /* Bits 31:29 */

} WfwDembackTFPoolTableStruct;

typedef struct
{
  UWord16 cctrchIdx:4;               /* Bits 3:0  */
  UWord16 wcbScale:5;                /* Bits 8:4 */
  UWord16 serv:1;                    /* Bit 9 */
  UWord16  :5;                       /* Bits 14:10 */
  UWord16 soft:1;                    /* Bit 15 */
} WfwDembackMbmsInfoTableCommonWordStruct;

typedef struct
{
  /* Word0 */
  UWord16 trchIdx:4;                 /* Bits 3:0  */
  UWord16 softCombChIdx:4;           /* Bits 7:4  */
  UWord16  :6;                       /* Bits 13:8 */
  UWord16 trChSoft:1;                /* Bit 14    */
  UWord16 enable:1;                  /* Bit 15    */

  /* Word1 */
  UWord16 drmbAddr:15;               /* Bits 14:0 */
  UWord16 dir:1;                     /* Bit 15    */
} WfwDembackMbmsTrchTableStruct;

typedef struct
{
  WfwDembackMbmsInfoTableCommonWordStruct dembackMbmsCommonWord;
  WfwDembackMbmsTrchTableStruct dembackMbmsTrch[WFW_DBACK_MBMS_NUM_TRCH_PER_PHYCHAN];
} WfwDembackMbmsInfoTableStruct;


typedef struct
{
  UWord16 evenEn:1;                  /* Bit 0 */
  UWord16 drmbIdxEven:3;             /* Bits 3:1 */
  UWord16 oddEn:1;                   /* Bit 4 */
  UWord16 drmbIdxOdd:3;              /* Bits 7:5 */
  UWord16 :8;                        /* Bits 15:8 */
} WfwDembackPccpchCctrchInfoTableStruct;
/*!@}*/

/*! @brief WCDMA FW HS demback related tables */
typedef struct
{
  /* Word0 */
  UWord32 scchPart1ScramblerMsb:8;   /* Bits 7:0 */
  UWord32 nAckNackTx:3;              /* Bits 10:8 */
  UWord32 :2;                        /* Bits 12:11 */
  UWord32 tfriTblType:1;             /* Bit  13 */
  UWord32 numValidHarqs:5;           /* Bits 18:14 */
  UWord32 hsRelIndicator:3;          /* Bits 21:19 */
  UWord32 sixtyfourQamIndicator:1;   /* Bit 22 */
  UWord32 drmbStorageDir:1;          /* Bit 23 */
  UWord32 :4;                        /* Bits 27:24 */
  UWord32 scchEn:4;                  /* Bits 31:28 */

  /* Word1 */
  UWord32 scchPart1ScramblerLsb;

  /* Word2 */
  UWord32 hrnti;

  /* Word3 */
  UWord8 scchOvsfCode[WFW_HS_NUM_SCCH_PER_CARRIER];

  /* Word 4 - Word 11 */
  UWord32 virBufSizeHarqIds[WFW_HS_NUM_HARQ_IDS + WFW_HSL_NUM_HARQ_IDS];
  /* Word 12 */
  UWord32 scchBcchPart1ScramblerMsb:8;  /* Bits 0:7 */
  UWord32 :5;                      /* Bits 8:12 */
  UWord32 oVal:4;                  /* Bits 13:16 */
  UWord32 TBSindex0:5;             /* Bits 17:21 */
  UWord32 TBSindex1:5;             /* Bits 22:26 */
  UWord32 numRetx:3;               /* Bits 27:29 */
  UWord32 numHrnti:1;              /* Bits 30 */
  UWord32 blindDecEn:1;            /* Bits 31 */
  
  /* Word 13 */
  UWord32 scchBcchPart1ScramblerLsb;

  /* Word 14 */
  UWord32 bcchHrnti;

} WfwDembackHsInfoTableStruct;

/*! Index of bump if DCCH or FULL detected */
#define WFW_DBACK_DCCH_DET_BUMP_DETECTED_IDX 0
/*! Index of bump if no-DCCH or no-FULL detected */
#define WFW_DBACK_DCCH_DET_BUMP_NOT_DETECTED_IDX 1
/*! Index of bump if first frame of DCCH or FULL TTI */
#define WFW_DBACK_DCCH_DET_BUMP_FIRST_FRAME_IDX 2
/*! Number of supported bump indexes for FULL & DCCH */
#define WFW_DBACK_DCCH_DET_NUM_IDX 3
/*! Number of supported bump indexes for DCCH */
#define WFW_DBACK_DCCH_DET_DCCH_NUM_IDX WFW_DBACK_DCCH_DET_NUM_IDX
/*! Number of supported bump indexes for FULL */
#define WFW_DBACK_DCCH_DET_FULL_NUM_IDX WFW_DBACK_DCCH_DET_NUM_IDX

/*! Index for TPC power offset */
#define WFW_DBACK_POWER_OFFSET_TPC_IDX 0
/*! Index for TPC power offset */
#define WFW_DBACK_POWER_OFFSET_TFCI_IDX 1
/*! Index for TPC power offset */
#define WFW_DBACK_POWER_OFFSET_DP_IDX 2
/*! Number of supported power offsets */
#define WFW_DBACK_POWER_OFFSET_NUM_IDX 3

/*! Maximum number of supported TTI length in frames for FULL & DCCH */
#define WFW_DBACK_MAX_SUPPORTED_TTI_LEN 8
/*! Index used for DCCH */
#define WFW_DBACK_DCCH_DET_DCCH_TYPE 0
/*! Index used for FULL */
#define WFW_DBACK_DCCH_DET_FULL_TYPE 1
/*! Index used for FET DCCH detection */
#define WFW_DBACK_DCCH_DET_DCCH_HIGH_TYPE 2
/*! Number of supported types */
#define WFW_DBACK_DCCH_DET_NUM_TYPE 3

/*! ILPC aware DCCH/DTCH disabled */
#define WFW_DBACK_DCCH_DET_ILPC_DIS 0
/*! ILPC aware DCCH/DTCH DCCH enabled */
#define WFW_DBACK_DCCH_DET_ILPC_DCCH_EN 1
/*! ILPC aware DCCH/DTCH DCCH+FR enabled */
#define WFW_DBACK_DCCH_DET_ILPC_DCCH_FR_EN 2

/*! DCCH detection SW->FW async interface - FW latch at frame boundary */
typedef struct
{
  /*! Additional DCCH/FULL threshold 16Q8 linear units, default = 0x133 = 1.2 in Q8. 
      threshold indexed by number of frames into TTI (N=1,2,3,4,5...) */
  UWord16 thresh[WFW_DBACK_DCCH_DET_NUM_TYPE][WFW_DBACK_MAX_SUPPORTED_TTI_LEN];
  /*! SIRT bump 16Q8 in linear units, default = 0x100 = 1 in Q8 (0dB) 
      Outer index: DCCH, Inner index: FULL */
  UWord16 sirtBump[WFW_DBACK_DCCH_DET_DCCH_NUM_IDX][WFW_DBACK_DCCH_DET_FULL_NUM_IDX];
  /*! PO1,2,3 in dB Q2 (Range 0-24 = 0-6dB), default = 0 = 0dB */
  UWord8 powerOffset[WFW_DBACK_POWER_OFFSET_NUM_IDX];
  /*! Enable bit for DCCH detection feature */
  UWord8 enable;
  /*! Enable bit for FR detection feature */
  UWord8 frEnable;
  /*! ILPC-aware DCCH/DTCH mode (0=disabled 1=DCCH only 2=DCCH+FR) */
  UWord8 ilpcEn;
} WfwDembackDcchDetStruct;

/*! Firmware viterbi decoder interface */
typedef struct
{
  /*! List Viterbi Decoder Enable interface*/
  UWord8 lvaEnable;
  /*! Enable FW DECOB read interface */
  UWord8 fwDecobEn;
} WfwDembackFWVDStruct;

/*! FW decob size in words */
#define WFW_DBACK_FW_VD_DECOB_SIZE 1024

/*! Firmware viterbi decoder interface */
typedef struct
{
  /*! FW viterbi decoder DECOB */
  UWord32 fwVdDecob[WFW_DBACK_FW_VD_DECOB_SIZE];
} WfwDembackFWVDReadStruct;

/*! DPCH frame FW->SW logging interface */
typedef struct
{
  /*! CFN number mod 256 */
  UWord32 cfn : 8;
  /*! FET attempted for this cfn */
  UWord32 fetAttempted : 8;
  /*! FET enabled for this cfn - 2nd frame terminated */
  UWord32 fetEnable : 8;
  /*! FET success slot: 10,12,14,16ms (15,18,21,24) */
  UWord32 fetSlot : 8;
  
  /*! DCCH detection enabled */
  UWord32 dcchDetEn : 8;
  /*! DCCH detected */
  UWord32 dcchDet : 8;
  /*! FULL detected */
  UWord32 fullDet : 8;
  /*! Reserved */
  UWord32 reserved : 8;
  
  /*! SIRT bump ASAP in Q8 */
  UWord32 sirtBumpAsap : 16;
  /*! SIRT bump 10ms latch in Q8 */
  UWord32 sirtBumpNext : 16;

  /*! DCCH energy metric */
  UWord32 dcchEnergy;
  /*! Basline energy metric for DCCH */
  UWord32 baseDcchEnergy;
  
  /*! DCCH num bits */
  UWord32 dcchNumBit : 16;
  /*! Basline num bits for DCCH */
  UWord32 baseDcchNumBit : 16;
  
  /*! FULL energy metric */
  UWord32 fullEnergy;  
  /*! Basline energy metric for FULL */
  UWord32 baseFullEnergy;
  
  /*! FULL num bits */
  UWord32 fullNumBit : 16;
  /*! Basline num bits for FULL */
  UWord32 baseFullNumBit : 16;
  
} WfwDembackDpchFrameStruct;

typedef struct
{
  WfwDembackCCTrChTableStruct           dembackCCTrChTable[WFW_DBACK_NUM_CCTRCH_TABLES];
  WfwDembackTFCIPoolTableStruct         dembackTFCIPoolTable[WFW_DBACK_NUM_TFCI_POOL_TABLE_ENTRIES];
  WfwDembackTFPoolTableStruct           dembackTFPoolTable[WFW_DBACK_NUM_TF_POOL_TABLE_ENTRIES];
  WfwDembackPccpchCctrchInfoTableStruct dembackPccpchCctrchTable[WFW_DBACK_NUM_PCCPCH_CCTRCH_TABLES];
  UWord16                               dembackPccpchDrmbAddr[WFW_DBACK_NUM_PCCPCH_CCTRCH_TABLES * 2];
  /*! DCCH detection related parameters */
  WfwDembackDcchDetStruct               dcchDet;
  /*! List Viterbi Decoder Enable interface*/
  WfwDembackFWVDStruct                  vd;
} WfwDembackNonHsIntfStruct;

/*! @brief WCDMA FW decob status FIFO */
typedef struct
{
  /* Word 0 */
  UWord16 hsInfoIdx:3;                 /* Bits 2:0 */
  UWord16 hsHarqId:4;                  /* Bits 6:3 */
  UWord16 crcPassed:1;                 /* Bit 7 */
  UWord16 :5;                          /* Bits 12:8 */
  UWord16 hrntiType:1;                 /* Bit 13 */
  UWord16 blindDecEn:1;                /* Bit 14 */
  UWord16 hsNewData:1;                 /* Bit 15 */

  /* Word1 */
  UWord16 hsTrChStartAddr:15;          /* Bits 14:0 */
  UWord16 :1;                          /* Bit 15 */

  /* Word2 */
  UWord16 hsSubFrmNum:11;              /* Bits 10:0 */
  UWord16 :5;                          /* Bits 15:11 */

  /* Word3 */
  UWord16 hsTrBlkSize;                 /* Bits 15:0 */

} WfwDembackHsDecobStatusFifoTableStruct;

typedef struct
{
  /* Word 0 */
  UWord16 nonHsCctrchId:3;             /* Bits 2:0 */
  UWord16 skippedDec:1;                /* Bit 3 */
  UWord16 nonHsTrChId:4;               /* Bits 7:4 */
  UWord16 nonHsDecodedTfci:7;          /* Bit 14:8 */
  UWord16 nonHsTfciDecPass:1;          /* Bit 15 */

  /* Word1 */
  UWord16 nonHsTrChStartAddr:15;       /* Bits 14:0 */
  UWord16 dcchDet:1;                   /* Bit 15 */

  /* Word2 */
  UWord16 nonHsCfn:8;                  /* Bits 7:0 */
  UWord16 nonHsTfId:8;                 /* Bits 15:8 */

  /* Word3 */
  UWord16 nonHsTrBlkSize:13;           /* Bits 12:0 */
  UWord16 nonHsDobType:2;              /* Bit 13-14 - WfwDemodDobTypeEnum */
  UWord16 nonHsNonFirstCrc:1;          /* Bit 15 */

  /* Word4 */
  UWord16 qtaGap:1;                    /* Bit 0 */
  UWord16 desenseGap:1;                /* Bit 1 */
  UWord16 :14;                         /* Bits 15:2*/

} WfwDembackNonHsDecobStatusFifoTableStruct;

typedef struct
{
  WfwDembackNonHsDecobStatusFifoTableStruct dobStatFifo[WFW_DBACK_NON_HS_DECOB_STATUS_FIFO_SIZE];
  UWord16 dobStatFifoWriteIndex;
  /*! Per-frame DPCH FW->SW interface */
  WfwDembackDpchFrameStruct dpchFrameFifo[WFW_DBACK_DPCH_FRAME_FIFO_SIZE];
  /*! Current write pointer - reset to 0 only at boot time */
  UWord8 dpchFrameWriteIndex;
  /*! List Viterbi Decoder SW Read interface*/
  WfwDembackFWVDReadStruct  vd;
} WfwDembackNonHsDecobStatusFifoStruct;

typedef struct
{
  WfwDembackHsDecobStatusFifoTableStruct dobStatFifo[WFW_DBACK_HS_DECOB_STATUS_FIFO_SIZE];
  UWord16 dobStatFifoWriteIndex;
} WfwDembackHsDecobStatusFifoStruct;

typedef struct
{
  UWord32 scchIdx             :8;  /*! 0-3: primary carrier, 4-7: secondary carrier, 8-11: third carrier */
  UWord32 orderType           :3;  /*! 0: CPC/HSL, 1: DC activation/deactivation type */
  UWord32 order               :3;  /*! Actual order received */

  UWord32 hsSubFrmNum         :11; /*! HS-SCCH subframe number */
  UWord32 orderIgnored        :1;  /*! Indicates FW ignored (DTXed) order since wfwSwIntfBufPtr->miscAsyncBuf[carIdx].miscHs.ignoreScchOrder was set or the order was invalid/could not be processed */

  UWord32 secCarActionsValid  :1;  /*! Indicates if dlSecCarAction and ulSecCarAction are valid */
  UWord32 dlSecCarAction      :2;  /*! LSB = C1, MSB = C2. 0 = deactivate, 1 = activate */
  UWord32 ulSecCarAction      :1;  /*! 0 = deactivate DC-HSUPA, 1 = activate DC-HSUPA */

  UWord32 cpcActionsValid     :1;  /*! Indicates if cpcDtxAction and cpcDrxAction are valid */
  UWord32 cpcDtxAction        :1;  /*! 0 = deactivate DTX, 1 = activate DTX */

  /****/

  UWord32 stmrRefCntFrmNum    :12; /*! STMR_REF_COUNT_STATUS_SNP FRAME_NUM at time of SCCH order processing */
  UWord32 stmrRefCntFrmRefCnt :19; /*! STMR_REF_COUNT_STATUS_SNP FRAME_REF_COUNT at time of SCCH order processing */

  UWord32 cpcDrxAction        :1;  /*! 0 = deactivate DRX, 1 = activate DRX */

} WfwDembackHsScchOrderFifoTableStruct;

typedef struct
{
  WfwDembackHsScchOrderFifoTableStruct hsScchOrderFifo[WFW_DBACK_HS_SCCH_ORDER_FIFO_SIZE];
  UWord16 hsScchOrderFifoWriteIndex;
} WfwDbackHsScchOrderFifoStruct;

/*! @brief HS SCCH decode status log structure */
typedef struct
{
  /* Word 0*/
  /* SCCH Amplitudes valuse for SCCH slot 1, 2 and 3 */
  UWord32 ampSlot1 : 10;   /* Bits 9 - 0 */
  UWord32 ampSlot2 : 10;   /* Bits 19-10 */
  UWord32 ampSlot3 : 10;   /* Bits 29-20 */
  UWord32          : 2;    /* Bits 31-30 */
  /* Byte 0 */
  /* Error count from SCCH Part1 VD decoding, rounded to 4 bits */
  UWord8  errCntP1 : 4;    /* Bits 3-0 */
  /* Error count from SCCH Part2 VD decoding, rounded to 4 bits */
  UWord8  errCntP2 : 4;    /* Bits 7-4 */
} WfwDbackHsScchDecodeStruct;

/*! @brief HS demback decode status log structure */
typedef struct
{
  /* Word 0 */
  UWord32 scchPt1Bits          : 12; /* Bits 11-0 */
  UWord32 scchValid            : 1;  /* Bit 12 */
  UWord32 scchDemod            : 1;  /* Bit 13 */
  UWord32 newTx0               : 1;  /* Bit 14 */
  UWord32 newTx1               : 1;  /* Bit 15 */
  UWord32 scchPt2Xrv0          : 3;  /* Bits 18-16 */
  UWord32 scchPt2Xrv1          : 3;  /* Bits 21-19 */
  UWord32 scchPt2Xhap0         : 5;  /* Bits 26-22 */
  UWord32 scchPt2Xhap1         : 5;  /* Bits 31-27 */

  /* Word 1 */
  UWord32 pdschTbs0            : 16; /* Bits 15-0 */
  UWord32 pdschTbs1            : 16; /* Bits 31-16 */

  /* Word 2 */
  UWord32 signalledTbs0        : 6;  /* Bits 5-0 */
  UWord32 signalledTbs1        : 6;  /* Bits 11-6 */
  UWord32 modScheme0           : 2;  /* Bits 13-12 */
  UWord32 modScheme1           : 2;  /* Bits 15-14 */
  UWord32 codeOffset           : 4;  /* Bits 19-16 */
  UWord32 numCodes             : 4;  /* Bits 23-20 */
  UWord32 numStreams           : 1;  /* Bit  24 */
  UWord32 rawNdi               : 1;  /* Bit  25 */
  UWord32 hrntiType            : 1;  /* Bits 26 */  
  UWord32 blindDecEn           : 1;  /* Bits 27 */
  UWord32 numFullIteration     : 4;  /* Bits 31-28 */

  /* Word 3 */
  UWord32 winningScch          : 2;  /* Bits 1-0 */
  UWord32 ackNack              : 2;  /* Bits 3-2 */
  UWord32 ackNackMimo          : 2;  /* Bits 5-4 */
  UWord32 demodPath            : 2;  /* Bits 7-6 */
  /*! SCCH order type 0: DTX, DRX and HSL 1: DC */
  UWord32 orderType            : 3;  /* Bits 10-8 */
  /*! 0: deactivation, 1: activation */
  UWord32 order                : 3;  /* Bits 13-11 */
  /*! TD early termination enabled */
  UWord32 tdEarlyTerminationEn : 1;  /* Bit  14      */
  UWord32 minLlr               : 12; /* Bits 26-15  */
  /*! Set 1 when current subframe contains SCCH order */
  UWord32 scchOrder            : 1;  /* Bit  27 */
  UWord32 eqEnb                : 1;  /* Bits 28 */
  UWord32 qiceEnb              : 1;  /* Bits 29 */
  UWord32 pciMimo              : 2;  /* Bits 31-30 */

  /* Word 4 */
  UWord32 rakeCqiMimoQ4        : 10; /* Bits 9-0 */
  UWord32 eqCqiMimoQ4          : 10; /* Bits 19-10 */
  UWord32 qIceCqiMimoQ4        : 10; /* Bits 29-20 */
  /* Indicate HS decode HW on flag */
  UWord32 hsDecodeHwOnFlag     : 1;  /* Bit  30 */
  UWord32                      : 1;  /* Bit  31 */

  /* Word 5 */
  UWord32 rakeCqiQ4            : 10;  /* Bits 9-0 */
  UWord32 eqCqiQ4              : 10;  /* Bits 19-10 */
  UWord32 qIceCqiQ4            : 10;  /* Bits 29-20 */
  UWord32 dlDeneseflag         : 1;   /* Bit  30 */
  UWord32                      : 1;   /* Bit  31 */

  /* Word 6 */
  UWord32 rxBlankBmsk            : 30;  /* Bits 29-0 */
  UWord32 rakeRxdEn              : 1;   /* Bit  30 */
  UWord32 qiceRxdEn              : 1;   /* Bit 31*/

  /* Word 7 */
  UWord32 cmGapBmsk            : 30;  /* Bits 29-0 */
  UWord32                      : 2;   /* Bit 30-31, Reserved */


  /*! HS SCCH decoding status */
  WfwDbackHsScchDecodeStruct hsScchDecode[WFW_HS_NUM_SCCH_PER_CARRIER];
} WfwDbackHsDecodeStatusLogStruct;

typedef struct
{
  /* HS global subframe index */
  UWord16 gSubFn;

  /* Decode status per carrier */
  WfwDbackHsDecodeStatusLogStruct WfwDbackHsDecodeStatusLogPerCarrier[WFW_DEMOD_NUM_MAX_CARRIERS];

} WfwDbackHsDecodeStatusLogEntryStruct;

typedef struct
{
  /*! FIFO read ptr.  Updated by L1SW */
  UWord8                                dbackHsDecodeStatusLogRdPtr;
  /*! FIFO write ptr.  Updated by FW.  Entries between read and write ptrs are ready to be read by L1SW */
  UWord8                                dbackHsDecodeStatusLogWrPtr;
  WfwDbackHsDecodeStatusLogEntryStruct WfwDbackHsDecodeStatusLogEntry[WFW_DEMOD_HS_DEC_STATUS_LOG_PACKET_BUF_SIZE];
} WfwDbackHsDecodeStatusLogFifoStruct;

/*! @brief Number of WfwDbackHsDecodeStatusLogStruct words that are not 
           written by CQI */
#define WFW_DBACK_HS_DECODE_STAT_NUM_NON_CQI_WORDS     4

/*! @brief EWC Regular log packet structure */
typedef struct {

  /* HWord0     */
  UWord16 gsfn:11;       
  UWord16 carrierIdx:2;  
  UWord16 cellBmask:3;    /* (cell2, cell1, cell0)   */
 
  /* HWord1     */ 
  UWord16 txDBmask:3;     /* (cell2, cell1, cell0) */
  UWord16 ec:1;           /* speed adaptation error control */ 
  UWord16 brachSel:6;     /* (cell2-rxd1,cell2-rxd0, cell1-rxd1, cell1-rxd0, cell0-rxd1, cell0-rxd0) */
  UWord16 ttSel:6;        /* (cell2, cell1, cell0) */



  /*HWord44 */
  /*CIRE enhancements logging */
  UWord16 doppler:10;          /*estimate of channel Doppler bandwidth in Hz */
  UWord16 dopplerMode:2;      /*dopplerMode value indicates low (0), middle (1) or high (2) mode of Doppler algorithm operation*/
  UWord16 :4;      /*reserved*/
  
   /* HWord2     */    
  UWord16 rawCirDmaEn:5;     /*Boolean state for last 5 blocks indicating whether the CIR was DMA’d to Mempool (5-bit bitmask)*/  
  UWord16 pnPosCx1 :8 ;     /* LSB 8 bits of pnPosCx1 for primary cell */
  UWord16 :3 ;     /* reserved */
  
  /* HWord3-14     */ 
  UWord16 causalEng[3][2][2];/* causal Energy [cell][txd][rxd] */

  /* HWord15-16     */ 
  UWord16 antCorr[2];     /* ant correlation for primary cell per Tx, Tx1|Tx0, 8SC7 each */

  /* HWord17-28 :  FlatChannel Matrices [cell][dot Products of CIR Tx and MMSE Tx] */
  UWord16 scaleMmse[3][4];

  /* HWord29-30 : raw mmse Qfactor perCellperTx 5bits each, 2MSB reserved     */ 
  UWord16 rawMmseQf[2];

  /* HWord31-32 : cellWeight for HS cell only, per Tx1|Tx0     */ 
  UWord16 cellWeight[2];

  /* HWord33-36 */
  UWord16 beamforming[2][2];

  /* HWord37-40 */
  UWord16 mmseTapEnergy[2][2];

  /* HWord41-42: raw mmse shifting factor perCellperTx 5bits each, 2MSB reserved  */ 
  UWord16 mmseSF[2];


  /* HWord45     */ 
  UWord16 reframeCntr:5;     /* reframing counter */
  UWord16 rxImbEn:1;         /* enabling flag for Rx Imb adaptive scaling */
  UWord16 weakChain:1;       /* weakChain for Rx Imb adaptive scaling */
  UWord16 mimoEn:1;          /* MIMO enabled flag */
  UWord16 weakChainScl:5;    /* 5MSB of Rx Imb adaptive combining scaling factor for the weaker chain */
  UWord16 rxImbMode: 1;      /* Rx Imb mode selection: 0, 2/3 switching; 1: ARC */
  UWord16 rxD:1;             /* RxD enable*/
  UWord16 :1;                /* reserved*/

  /*HWord45-46*/
  UWord16 tapID_0:6;         /*Bits 0: 5 – tap ID 0 */
  UWord16 tapID_1:6;         /*Bits 6: 11 – tap ID 1*/
  UWord16 :4; 
  
  UWord16 tapID_2:6;         /*Bits 0:5 – tap ID 2*/ 
  UWord16 tapID_3:6;         /*Bits 6:11 – tap ID 2*/
  UWord16 :4;

  UWord16 slowFtlDivergence:8; /*Bits 0:7 – Measure of divergence of raw and filtered FreqCommon : abs(slowFTL – FTL) >> 4*/
  UWord16 :8;               

  UWord8  vlc_mode;           /* VLC mode: 0: Td, 1: Ts */
  UWord8  vlc_cir_begin[3];   /* CME CIR begin index per cell */
  UWord8  vlc_cir_length[3];  /* CME CIR length per cell */
  UWord8  vlc_cov_length;     /* COV length */
} WfwEwcInfoDebugLogStruct;

/*!@brief Demod IRPM control paraemeters */
typedef struct
{
  UWord8 tMeas;     /* Measurement time in subframes */
  UWord8 tSub;      /* Subsampling time in subframes */
  UWord8 tDwell;    /* Dwell time in subframes */
  UWord8 irpmEn;    /* IRPM enable (1) disable (0) */
  UWord32 delta;     /* Delta value in Q15 */
} WfwDemodIrpmAsyncWriteStruct;

typedef struct
{
	UWord16 cir_delta_serving;     /* CIR delta for serving cell; 82 in Q15 (-26 dB)*/
	UWord16 cir_delta_interfering; /* CIR delta for interfering cells; 207 in Q15 (-22 dB) */
	UWord16 cov_delta;             /* COV delta; 52 in Q15 (-28 dB)*/
	UWord8  tSub;  					 /* Subsampling time in subframes */
	UWord8  vlcEn;     				 /* VLC enable (1) disable (0) */
}
WfwDemodVlcAsyncWriteStruct;

/*! @brief EWC async read structure */
typedef struct
{
  WfwEwcInfoDebugLogStruct ewcDebugLog[WFW_DEMOD_NUM_MAX_CARRIERS][WFW_DEMOD_EWC_DEBUG_LOG_PACKET_BUF_SIZE];
  UWord8 wrPtr;
} WfwEwcDebugAsyncReadStruct;

/*! @brief EWC async write structure */
typedef struct
{
  UWord8 logEnable;
  UWord8 rdPtr;
} WfwEwcDebugAsyncWriteStruct;

/*! @brief SCHIC debug info log structure */
typedef struct
{
  /* Word 0 */
  UWord32 pschGain : 8;     /* Bits 7-0 */
  UWord32 sschGain : 8;     /* Bits 15-8 */
  UWord32 subframeCntr: 11; /* Bits 26-16 */
  UWord32 cellNum: 4;       /* Bits 30-27 */
  UWord32 : 1;              /* Bit  31 */

  /* Word 1 */
  UWord32 pschSymReal : 32; /* Bit 31-0 */

  /* Word 2 */
  UWord32 pschSymImag : 32; /* Bit 31-0 */

  /* Word 3 */
  UWord32 sschSymReal : 32; /* Bit 31-0 */

  /* Word 4 */
  UWord32 sschSymImag : 32; /* Bit 31-0 */

  /* Word 5 */
  UWord32 pilotSymReal : 32; /* Bit 31-0 */

  /* Word 6 */
  UWord32 pilotSymImag : 32; /* Bit 31-0 */

} WfwSchicInfoLogStruct;

typedef struct
{
  WfwSchicInfoLogStruct                 schicInfoLog[WFW_DEMOD_SCHIC_LOG_PACKET_BUF_SIZE];
  UWord8 wrPtr;

} WfwSchicAsyncReadStruct;

/*! @brief HS CM Control Table structure */
typedef struct
{
  union
  {
    /*! BIT0 - demod enable, BIT1 - TX N/ACK, BIT2 - TX CQI, BIT3 - RX CQI */
    UWord8 cmGapBmsk;
    struct
    {
      /*! Bit 0: HS SCCH/PDSCH demod enabled */
      UWord8 demodEn : 1;
      /*! Bit 1: Ack/Nack Tx enable */
      UWord8 ackNackTx : 1;
      /*! Bit 2: Cqi Tx enable */
      UWord8 cqiTx : 1;
      /*! Bit 3: CQI valid. Transmit DTX if not valid and cqiTx is enabled */
      UWord8 cqiValid : 1;
    } mask;
  } ctrl[WFW_DEMOD_HS_CM_CTRL_TABLE_SIZE];
} WfwDemodHsCmCtrlTableStruct;

/*! @brief SEQ structure, used in Mimo demod log struct below */
/* mjl - is this the way to do it?  Or can we put a gap in? */
typedef struct
{
  /* Word 0 */
  UWord32 seq00Real : 12;        /* Bits 11:0  */
  UWord32 seq00Imag : 12;        /* Bits 23:12 */
  UWord32 seq01RealPt1 : 8;      /* Bits 31:24, lsbs of Seq01Real */

  /* Word 1 */
  UWord32 seq01RealPt2 : 4;      /* Bits 3:0, msbs of Seq01Real  */
  UWord32 seq01Imag : 12;        /* Bits 15:4 */
  UWord32 : 16;                  /* Bits 31:16 */

} WfwDbackHsSeqDemodStruct;

/* FEATURE_DBACK_NEW_AE_AND_MIMO_LOG_PKT */
/*! @brief RW structure, used in Mimo demod log struct below */
typedef struct
{
  /* NOTE:
     Taxis has no SIC hence WFW_MIMO_OT_SIC_SUPPORT is not there.
     Logging fields are used for alternate purpose as mentioned in comments */

  /* Word 0 */
  UWord32 rw00Real    : 13;     /* Bits 12:0  */
  UWord32 rw00Imag    : 13;     /* Bits 25:13 */
  UWord32 rw01RealPt1 :  6;     /* Bits 31:26 lsbs of rw01Real */

  /* Word 1 */
  UWord32 rw01RealPt2 :  7;     /* Bits 6:0 msbs of rw01Real */
  UWord32 rw01Imag    : 13;     /* Bits 19:7  */
  UWord32             : 12;     /* Bits 31:20 */

} WfwDbackHsRWDemodStruct;

/*! @brief HS demback Mimo demod log structure, per subframe */
typedef struct
{
  /* Word 0 */
  /*! Set to 1 when PDSCH transmission is detected */
  UWord32 pdschExists : 1;          /* Bit 0 */
  /*! Set to 1 when SIC was done for stream decoded second */
  UWord32 sicDone2ndDecode   : 1;   /* Bit 1 */
  /*! Set to 1 when SIC was done for stream decoded second */
  UWord32 sicDone3rdDecode   : 1;   /* Bit 2 */
  /*! set to 1 when 2nd stream is decoded first, 0 if primary stream is decoded first */
  UWord32 firstDecodedStream : 1;   /* Bit 3 */
  /*! set to 1 if RW and updated SEQ are computed twice for SIC decoding : 
      once for A&E, once for DRM with re-encoded stream correlation */
  UWord32 genieCorrelaton    : 1;   /* Bit 4 */
  UWord32 reserved           : 11;  /* Bits 15:5 */
  UWord32 hsSubFrmNum        : 11;  /* Bits 26:16 */
  UWord32                    : 5;   /* Bits 31:27 */

  /* Word 1 */
  /* H is the 2x2 flat channel estimate.
     Elements are 16-bit (Q,I) with I in lower 16 bits
     For now, assume only one set of H and Ryy per subframe */
  UWord32 H00;

  /* Word 2 */
  UWord32 H01;

  /* Word 3 */
  UWord32 H10;

  /* Word 4 */
  UWord32 H11;

  /* Word 5 */
  /* MIMO correlations. Autocorrelations are real */
  UWord16 Ryy00;
  UWord16 Ryy01Real;

  /* Word 6 */
  UWord16 Ryy01Imag;
  UWord16 Ryy11;

  /* Words 7 to 10 */
  /*! 2x2 Symbol Equalizer (SEQ) matrix 
      0 : Primary stream
      1 : Secondary stream
      2 : SIC done 2nd decode and genie correlation used for DRM
      3 : SIC done 3rd decode and genie correlation used for DRM
  */
  WfwDbackHsSeqDemodStruct seq[4];

  /* Word 11 */
  /* Round bits - 9 (so values 9 to 16 are encoded as 0 to 7) */
  UWord32 Rnd0 : 4;            /* Bits 3:0 */
  UWord32 Rnd1 : 4;            /* Bits 7:4 */
  UWord32 Rnd2 : 4;            /* Bits 11:8 */
  UWord32 Rnd3 : 4;            /* Bits 15:12 */
  UWord32      : 16;           /* Bits 31:16 */

  /* Words 12 to 13 */
  /*! Reconstruction Weights(RW) in Q3
      0 : For second decode
      1 : For second decode and genie correlation
      2 : For third decode
      3 : For third decode and genie correlation
  */
  WfwDbackHsRWDemodStruct rw[4];

} WfwDbackHsMimoDemodLogStruct;

/*! @brief HS MIMO demod log async structure */
typedef struct
{
  WfwDbackHsMimoDemodLogStruct dbackHsMimoDemodLog[WFW_DEMOD_HS_MIMO_DEMOD_LOG_PACKET_BUF_SIZE];
  UWord8                       wrPtr;
} WfwDbackHsMimoDemodLogAsyncReadStruct;

/*! @brief Amplitude and energy structure, used in A&E log struct below */
typedef struct
{
  UWord32 ampEst : 13;         /* Bits 12:0 */
  UWord32 energyEst : 19;      /* Bits 31:13 */
} WfwDbackHsAeDemodStruct;

/* FEATURE_DBACK_NEW_AE_AND_MIMO_LOG_PKT */
/*! @brief HS amplitude and energy log structure, per subframe */
typedef struct
{

  /* Word 0*/
  UWord32 hsSubFrmNum: 11;           /* Bits 10:0 */
  UWord32            : 21;           /* Bits 31:11*/

  /* Word 1 */
  /*! Demfront mode 
      0: Rake, 1:EQ, 2:QICE */
  UWord32 demfrontMode : 2;          /* Bits 1:0 */
  /*! MIMO enabled falg, set to 1 when MIMO is enabled */
  UWord32 mimoEnabledFlag : 1;       /* Bit 2    */
  /*! HSDPA modulation scheme 
      1) non-MIMO ( if mimoEnabledFlag is 0)
         0: QPSK, 1:16QAM, 2:64QAM
      2) MIMO (Primary stream modulation is equal or higer than secondary stream)
         0: QPSK/-
         1: QPSK/QPSK
         2: 16QAM/-
         3: 16QAM/QPSK
         4: 16QAM/16QAM
         5: 64QAM/-
         6: 64QAM/QPSK
         7: 64QAM/16QAM
         8: 64QAM/64QAM
  */
  UWord32 modScheme               : 4;          /* Bits 6:3 */
  /*! Reserved */
  UWord32                         : 1;          /* Bit 7 */
  /*! New Transmission indicator bits 
      1) In non-MIMO:
         00: old transmission
         01: new transmission
      2) In MINO
         00: old/old
         01: old/new
         10: new/old
         11: new/new
  */
  UWord32 newTrans                : 2;          /* Bits 9:8 */
  /*! TD Scale of primary stream */
  UWord32 tdScale                 : 3;          /* Bits 12:10 */
  /*! TD scale of secondary stream (not used in non-MIMO) */
  UWord32 tdScaleMimo             : 3;          /* Bits 15:13 */
  /*! Log the following only if MIMO is enabled */
  /*! MIMO decoding order 
      0: Primary decoded first
      1: Secondary decoded first
  */
  UWord32 mimoDecodingOrder       : 1;          /* Bit 16 */
  /*! MIMO SIC enabled secode_decode */
  UWord32 momoSicEnabled2ndDecode : 1;          /* Bit 17 */
  /*! MIMO SIC enabled third_decode */
  UWord32 momoSicEnabled3rdDecode : 1;          /* Bit 18 */
  /*! TD scale of 3rd decode in MIMO */
  UWord32 tdScaleMimo3rd          : 3;          /* Bits 21:19 */
  /*! Reserved */
  UWord32                         : 10;         /* Bits 31:22 */

  /* Words 2 to 6 */
  /* For non-MIMO, amplitude and energy for each AE group in a subframe
     (5 per subframe)
     For MIMO, amplitude and energy averaged over AE groups in subframe
     Index 0: A&E of MIMO primary stream
           1: A&E of MIMO secondary stream
           2: A&E of MIMO post-SIC second decode
           3: A&E of MIMO post-SIC third decode
           4: Reserved
  */
  WfwDbackHsAeDemodStruct Ae[5];

} WfwDbackHsAeLogStruct;

/*! @brief HS amplitude and energy Aynch log structure, 2 carriers per subframe */
typedef struct
{
  WfwDbackHsAeLogStruct dbackHsAeLog[WFW_DEMOD_HS_AE_LOG_PACKET_BUF_SIZE][WFW_DEMOD_NUM_MAX_CARRIERS];
  UWord8                wrPtr;
} WfwDbackHsAeLogAsyncReadStruct;

/*! @brief SW to FW CQI piecewise linear adjustment table */
typedef struct
{
  Word32 start;
  Word32 thresh0;
  Word32 thresh1;
  Word32 thresh2;
  Word32 thresh3;
  Word32 thresh4;
  Word16 grad0;
  Word16 grad1;
  Word16 grad2;
  Word16 grad3;
} WfwCqiPieceWiseLinearAdjStruct;

/*! @brief SW to FW Misc Demback HS Async parameters */
typedef struct
{
  /* Word 0 */
  /*! Symbol Equalizer disable flag */
  /* CS3 SIC support */
  /*! SIC mode */
  /*!  0: SIC disabled
       1: baseline (hard) SIC: hard SIC if CRC passes, no SIC if CRC fails, non iterative
       2: hard SIC if CRC passes, soft SIC if CRC fails, non iterative
       3: reserved / invalid
       4: soft SIC (CRC unconditional)
       5: iterative SIC (second iteration only if first decode fails).
          Soft SIC if CRC fails, hard SIC if CRC passes
       6: iterative SIC (second iteration only if first decode fails).
          Soft SIC (CRC unconditional)
       7: reserved / invalid
  */
  UWord32 mimoSicMode : 3;           /* Bits 2:0 */
  /*! PCI selection method 0: CIR energy maximizing, 1: SEQ maximizing */
  UWord32 mimoSsPciSel : 1;            /* Bit  3 */
  /*! Constant number of streams 
      00: pick number of streams dynamically
      01: reserved
      10: force single stream request
      11: force dual stream request
  */
  UWord32 mimoConstNumStreams : 2;   /* Bits 5:4 */
  /*! Constant CQI enable */
  UWord32 mimoConstCqiEn : 1;        /* Bit  6 */
  /*! Constant CQI0 */
  UWord32 mimoConstCqi0 : 5;         /* Bits 11:7 */
  /*! Constant CQI1 */
  UWord32 mimoConstCqi1 : 4;         /* Bits 15:12 */
  /*! Constant PCI enable */
  UWord32 mimoConstPciEn : 1;        /* Bit  16 */
  /*! Constant PCI */
  UWord32 mimoConstPci : 4;          /* Bits 20:17 */
  /*! Active receiver for MIMO 0: Rake, 1: EQ 
      0: Supported data rate based switching
      other values (1,2,3): unexpected behavior
  */
  UWord32 mimoActiveRece: 2;         /* Bits 22:21 */
  /*! Phase noise rejection enable */
  UWord32 phaseNoiseRejEn: 1;        /* Bit  23 */
  /*! Phase noise rejection, always ignore Q component */
  UWord32 phaseNoiseIgnoreQ: 1;      /* Bit  24 */
  /*! Phase noise rejection, disable noise rotation 
      0 if P-CPICH mode
      1 if S-CPICH mode
  */
  UWord32 phaseNoiseDisRot: 1;       /* Bit  25 */
  /*! Phase noise rejection logging enable */
  UWord32 phaseNoiseRejLoggingEn: 1; /* Bit  26 */
  /*! Cqi filter coeffs idx for 3 tap pilot filter */
  UWord32 cqiFilterCoeffIdx: 1;      /* Bit  27 */
  /*! MIMO two beam mode enable 
      0: Disable
      1: Enable in S-CPICH
      2: Enable always
  */
  //UWord32 mimoTwoBeamEn: 2;          /* Bits 29:28 */
  UWord32 : 4;                       /* Bits 31:30 */

  /* Word 1 */
  /*! MIMO backoff for CQI bler alignment */
  /*! CS3 Support features : from taxis Rev#97 FEATURE_WFW_MIMO_MODE_BACKOFF_CBA_PER_RECE */
  /*! Single Streeam 
    15: 0 : PCPICH
    31:16 : SCPICH
  */
  Word32 mimoCqiMarginSS[WFW_DEMOD_NUM_RECE];
  /*! Dual Streams */
  Word32 mimoCqiMarginDS[WFW_DEMOD_NUM_RECE];
  /*! Dual Streams and SIC */
  Word32 mimoCqiMarginDsSIC[WFW_DEMOD_NUM_RECE];

  /*! Phase noise inverse threshold */
  UWord32 phaseNoiseInvThresh;

  /*! Phase noise filter gain */
  UWord32 phaseNoiseFiltGain;
  UWord32 phaseNoiseItoQ;

  /*! Threshold for intermittent scheduling CQI protection */
  UWord32 undoAgcThresh;

  /*! FFT ratio for CME */
  UWord32 mimoFftLowBnd;

  /*! Conditioning scale for CME */
  UWord32 mimoCmeEwcCondScalePkd;

  /*! CS3 Support features : from taxis Rev#95 and 96 */
  /*! Receiver switching bias for active receiver */
  Word32 mimoRxSwitchingBias;

  /*! Receiver switching hold time */
  Word32 mimoRxSwitchingHoldTime;

  /*! Filter coeff for mimo rx switching */
  Word32 mimoRxSwitchingFilterCoeff[WFW_DEMOD_NUM_RECE];

  /*! Receiver switching weights for mimo rx switching */
  Word32 mimoRxSwitchingWts[WFW_DEMOD_NUM_RECE];

  /*! Rank dependent Spatial equalization per receiverfor both CQI and HS demod */
  /*! CQI algo bmsk: Bit2 1:0: Rake, Bit 3:2: EQ, Bit 5:4: QICE 
      0: SEQ, 1: ChipEQ, 2: CEQ for SS, SEQ for DS */
  UWord32 mimoCqiAlgoBmsk: 6;         /* Bits 5:0 */

  /*! Demod algo bmsk: Bit 7:6: Rake, Bit 9:8: EQ, Bit 11:10: QICE 
      0: SEQ, 1: ChipEQ, 2: CEQ for SS, SEQ for DS */
  UWord32 mimoDemodAlgoBmsk: 6;       /* Bits 11:6 */

  UWord32 : 20;                       /* Bits 31:12 */

  /*! Piecewise linear adjustment for cqi bler alignment */
  WfwCqiPieceWiseLinearAdjStruct cqiPiecewiseLnrAdj[WFW_DEMOD_NUM_RECE];

  /* Ignore SCCH orders for this carrier when there is a physical channel 
     reconfiguration. If order is ignored, inform SW via orderIgnored in 
     SCCH order FIFO */
  UWord8 ignoreScchOrder;
  /*If a QTA gap is detected between consecutive HS-SCCH decode separated
   * by more than the period set by SW corresponding HARQ processes are
   *  treated as new Transmission*/
  UWord8 qtaHarqFlushTimeout;

  /* OMRD CQI Back off Indicator */ 
  UWord8 cqiBackoffOmrd;
  
  /* OMRD CQI Adjustment Offset */ 
  Word8 cqiAdjOmrd;

  /* RAKE CQI BIAS indicator to add 1dB bias from the first gap till
   the last gap in CM pattern */
  UWord8 cmGapRakeCqiBiasEn;

  /*! Disregard DC-HSUPA act order if DC-HSUPA is not configured, but 3C is
     configured */
  UWord8 disregardUncfgDcHsupaActOrd;
  /*! Disregard orders to activate 4C (rather than DTX) */
  UWord8 disregard4COrd;

} WfwMiscHsStruct ;

#define WFW_DEMOD_IGNORE_SCCH_ORD_MC            0x01
#define WFW_DEMOD_IGNORE_SCCH_ORD_CPC           0x02
#define WFW_DEMOD_IGNORE_SCCH_ORD_ALL           0xff

/*! @brief SW to FW Misc Async parameters */
typedef struct
{
  WfwMiscHsStruct miscHs;
} WfwMiscAsyncStruct;


/*! @brief FW to SW Channel Analysis packet */
typedef struct
{
  /* 2x2 Channel estimate
     I in lower halfword
     Q in upper halfword */

  Word32 H00;
  Word32 H01;
  Word32 H10;
  Word32 H11;

  /* 2x2 Noise Covariance matrix
     (matrix is conjugate symmetric so need only
     three elements) */
  Word32 Rnn00_real;
  Word32 Rnn01_real;
  Word32 Rnn01_imag;
  Word32 Rnn11_real;

  UWord32 dualStreamRate : 16;    /* Bits 15 - 0  */
  UWord32 singleStreamRate : 15;  /* Bits 30 - 16 */
  UWord32 ssDsPreference : 1;     /* Bit  31      */

} WfwChannelAnalysisStruct;

/*! @brief FW to SW TFCI log packet */
typedef struct
{
  /* Word 0 */
  UWord16 maxEgyUnmasked;
  UWord16 maxEgyMasked;

  /* Word 1 */
  UWord32 maxEgyIndexUnmasked : 7; /* Bits  6 - 0  */
  UWord32 maxEgyIndexMasked : 7;   /* Bits 13 - 7  */
  UWord32 egySignUnmasked : 1;     /* Bit  14      */
  UWord32 egySignMasked : 1;       /* Bit  15      */
  UWord32 decodedTfci : 8;         /* Bits 23 - 16 */
  UWord32 cfn : 8;                 /* Bits 31 - 24 */

} WfwTfciLogStruct;

/*! @brief SW to FW Pessimistic CQI packet */
typedef struct
{
  /* Word 0 */
  Word32 cpichSnrRake : 10;      /* Bits 9:0   */
  Word32 cpichSnrEq : 10;        /* Bits 19:10 */
  Word32 cpichSnrQice: 10;       /* Bits 29:20 */
  Word32 dbg: 2;                 /* Bits 31:30 */

  /* Word 1 */
  UWord16 pnrCountRake : 5;      /* Bits 4:0   */
  UWord16 pnrCountEq : 5;        /* Bits 9:5   */
  UWord16 pnrCountQice: 5;       /* Bits 14:10 */
  UWord16 :1;                    /* Bit 15     */

  UWord16 numUndoAgcBpgs : 5;    /* Bits 4:0   */
  UWord16 intermittentSchedGrpsBmsk : 5; /* Bits 9:5    */
  UWord16 : 6;                   /* Bits 15:10 */
} WfwHsPessCqiLogStruct;


typedef struct{ 

    UWord32 rawPilotSqrAnt0;

    UWord32 rawPilotSqrAnt1;

    UWord32 rawPilotSqrLog10Ant0;

    UWord32 rawPilotSqrLog10Ant1;

    UWord32 rawPilotSqrPerBpgAnt0[15];

    UWord32 rawPilotSqrPerBpgAnt1[15];

    UWord32 rawPilot[15];

    UWord32 pichSym[15];
    
    UWord32 bpgIdx;

    UWord32 numFing;

    UWord32 avgNumFing;

}WfwTcResultsRlpInfoStruct;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_DEMOD_INTF_H */
