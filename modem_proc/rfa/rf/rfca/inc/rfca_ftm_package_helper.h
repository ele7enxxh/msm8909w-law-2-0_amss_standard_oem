/*!
  @file
  rfca_ftm_package_helper.h
 
  @brief
  Helper functions to construct FTM messages and add them to queue

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_ftm_package_helper.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/10/14   aki     Added AddFtmGsmGetMeasPwrPrxDrx and removed AddFtmGsmGetMeasPwr
                   and AddFtmGsmGetMeasPwrDrx
10/30/14   aki     Added AddFtmGsmGetMeasPwrDrx
09/18/14   aki     Added support for compressed sweep payload
04/16/14   aki     Updated AddFtmCfgAndExecCalSweepFlex
03/20/14   aki     Added AddFtmSetPathOverrideIndex
12/09/13   aki     Added AddFtmDoRffeCmd
11/19/13   aki     Added AddFtmDoRsbTxloCal
11/14/13   aki     Added AddFtmSetFilteredPath
09/09/13   Aki     Added AddFtmSetAntenna
08/02/13   aki     Removed AddFtmGsmPathDelay
07/31/13   aki     Add IQ functions
07/16/13   aki     Added AddFtmCfgAndExecCalSweepFlex
06/29/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
06/10/13   aki     Mainline RFCA
06/04/13   aki     Support 8 byte op-codes
05/17/13   aki     Updated GSM SWEEP to newly renamed types
05/02/13   aki     Added AddFtmRxWaitDbmV2
04/29/13   aki     word to int16
04/25/13   aki     Added GSM Sweep functions
04/14/13   aki     Added AddFtmLteSetLnaStateDbm
04/10/13   aki     Added AddFtmSetFreqAdjust
02/04/13   aki     Added AddFtmLteSetTddParams
01/16/13   aki     Added support for V3 polling
12/10/12   aki     Added AddFtmXoCal
11/26/12   aki     Added AddFtmStopTxWaveform
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/11/12   aki     Added AddFtmSetGsmLinearRgi
08/16/12   aki     Addded AddFtmSetFreqAdjust
07/23/12   aki     Tdscdma TX/RX burst support
07/13/12   aki     AddFtmRawMsg functions
05/25/12   aki     Cdma API2 functions for RX cal
04/23/12   aki     Dual carrier support, removed waveform from amam sweep,
                   Cal state takes byte instead of bool, added net sig val,
				   XO collect samples
03/01/12   aki     Support for APT sweep and self cal
02/17/12   aki     Added XO, THER and KV support
02/08/12   aki     Publish NewFtmPacketQueueItem function, PACK
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_FTM_PACKAGE_HELPER_H_
#define _RFCA_FTM_PACKAGE_HELPER_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_custom_types.h"
#include "rfca_packet_helper.h"
#include "rfca_pending_message_queue.h"
#include "ftm_calibration_xo.h"
#include "ftm_common_calibration_v3.h"


#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

#define MAX_SWEEP_OP_CODE         FTM_CAL_OPCODE_POLLING_MASK
#define NUMBER_OF_SWEEP_SEGMENTS  7 

// @todo This now combines polling and rx wait into single struct
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating debug mode for polling opcode */

  uint16 flag;
  /*!< 16-bit mask where each bit indicates a specific type of action. */

  uint32 timeout_us;
  /*!< Polling Timeout time in microseconds. If the event does not occur after 
  this time, then polling will be stopped with error code.  
  FTM_CAL_POLLING_INFINITE = Infinite microseconds */

  ftm_cal_polling_events polling_event;
  /*!< Event for which the infinite polling is to be done. Each bit will 
  represent a polling event. */

  uint8 primary_device;
  /*!< Primary Device from which RX AGC is read*/

  int8 direction;
  /*!< Trigger for rising signal level = 0, trigger falling = 1 */

  int16 threshold;
  /*!< RX Threshold as dBm16 for GSM and dBm10 for other technologies */
  
} FTM_Cal_Sweep_Rx_Wait_Req;

#define FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 4

typedef PACK(struct)
{
  word opcode[FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16];
  word segLen;
  byte opCodePayload[];
} FTM_Cal_Sweep_Opcode_Header;

typedef PACK(struct)
{
  byte masterDev;
  dword slaveDev;
  byte rfmode;
  word ulChan;
  word txBW;
  word rxBW;
  word chanOffset;
} FTM_Cal_Sweep_Cfg_Req;

typedef PACK(struct)
{
  byte masterDev;
  word ulChan;
  word chanOffset;
} FTM_Cal_Sweep_Retune_Req;

typedef PACK(struct)
{
  byte txWaveform;
  byte numRBs;
  byte startRB;
} FTM_Cal_Sweep_Tx_Wfm_Cfg_Req;

typedef PACK(struct)
{
  byte txAction;
  byte paState;
  byte rgi;
  dword paBias;
  byte txFlag;
} FTM_Cal_Sweep_TxMeas_Req;

typedef PACK(struct)
{
  byte numHdetAvg;
  byte numLpmHdetAvg;
  byte hdetFlag;
} FTM_Cal_Sweep_HDET_Meas_Req;

typedef PACK(struct)
{
  word rxActions;
  byte rxPM;
  byte rxFlag;
} FTM_Cal_Sweep_RxCfg_Req;

typedef PACK(struct)
{
  word rxActions;
  byte rxPM;
  word expRxAGC;
  byte rxFlag;
} FTM_Cal_Sweep_RxMeas_Req;

PACK(struct) configListItem
{
  struct configListItem *next;
  uint16 totalSize;
  FTM_Cal_Sweep_Opcode_Header opCodeHeader;
};

typedef struct configListItem FTM_Cal_Sweep_Opcode_Req_List_Item;

/*----------------------------------------------------------------------------*/
/*!
  @brief Helper function to set op code 

  @param sweepItem  pointer to opcode packet 
  @param opcodeBit  Opcode bit to set
 
  @return true on success, false on fail
*/
bool SetFtmCalSweepOpCodeBit(FTM_Cal_Sweep_Opcode_Header *sweepItemOpcodeHeader, ftm_cal_opcode_bit_type opcodeBit);

/*----------------------------------------------------------------------------*/
/*!
  @brief Resets opcode to opcode size and 0's

  @param sweepItem  pointer to opcode packet 
  @param opcodeBit  Opcode bit to set
 
  @return true on success, false on fail
*/
bool ResetOpcode(FTM_Cal_Sweep_Opcode_Header *sweepItemOpcodeHeader);

/*----------------------------------------------------------------------------*/
/*!
@brief Class to manage unique FTM Sequence IDs

*/
class FtmSequenceID
{
  ///< Keeps track of ftm sequence IDs
  unsigned int nextFtmSequenceID;

public:
  /*!
  @brief Default constructor.

  */
  FtmSequenceID():nextFtmSequenceID(0)  {};

  /*!
  @brief Returns the next ftm sequence identifier.

  @details Each FTM command sent to FTM Sequencer must have 
  unique ID associated to it so that response messages
  can be matched with the request.

  @return The next ftm sequence identifier.
  */
  unsigned int GetNext()
  {
    // This will just overflow back to zero
    this->nextFtmSequenceID++;  
    // TODO might not be thread safe
    return this->nextFtmSequenceID;
  }

  /*!
  @brief Resets the given seed.

  @param [in] seed The seed value to reset ID counter to.
  */
  void Reset(unsigned int seed)
  {
    this->nextFtmSequenceID = seed;
  }
};

/*----------------------------------------------------------------------------*/
typedef PACK(struct) {
  uint16 segmentLength; //!<  Duration of segment. Unit 3.692uS
  byte rgi;             //!<  Rgi of segment
  byte gsmMode;         //!<  Mode of segment, GSM mode - 1, EDGE Mode - 0
  byte paRange;         //!<  PA Range of segment
} GSM_GAIN_SWEEP_DATA;

/*----------------------------------------------------------------------------*/
/*!
  @brief Creates new instance of FtmPacketQueueItem and assigns Sequence ID
  
*/
FtmPacketQueueItem * NewFtmPacketQueueItem(void);

/*----------------------------------------------------------------------------*/
/*!
@brief Initialize FTM DIAG header with given mode

@param packet  Pointer to packet to be initialized
@param iModeID Identifier for the mode.
*/
void FormatFTMHeader(RfcaPacketHelper *packet, word iModeID );

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM sequence request to add given FTM messages to queue

 @param seqPacket     Pointer to packet to be created
 @param nextFtmItem   FTM item in the PendingRfcaResponse::FtmItems queue

 @todo Needs some kind of check that DIAG package is not too big
*/
void FormatFTMSequencerPackage2(RfcaPacketHelper *seqPacket, FtmPacketQueueItem** nextFtmItem);


/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to set mode and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use
 @param rfMode      RF mode to set phone to
 */
FtmPacketQueueItem * AddFtmSetMode(word halOpId, word ftmModeId, word rfMode);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to set channel and adds to pending DIAG queue

 @param halOpId   HAL operation ID
 @param ftmModeId FTM mode to use
 @param channel   Channel to set 
 */
FtmPacketQueueItem * AddFtmSetChan(word halOpId, word ftmModeId, word channel);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to set channel for dual carrier and adds to pending DIAG queue

 @param halOpId   HAL operation ID
 @param ftmModeId FTM mode to use
 @param first_channel   Channel to set 
 @param second_channel  Channel to set 
 */
FtmPacketQueueItem * AddFtmSetMultiChan(word halOpId, word ftmModeId, word first_channel, word second_channel);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to set LNA offset and adds to pending DIAG queue

 @param halOpId   HAL operation ID
 @param ftmModeId FTM mode to use 
 @param lnaIndex  Lna index
 @param lnaOffset Lna offset
 */
FtmPacketQueueItem * AddFtmSetLnaOffset(word halOpId, word ftmModeId, word lnaIndex, word lnaOffset);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to get LNA offset and adds to pending DIAG queue

 @param halOpId       HAL operation ID
 @param ftmModeId     FTM mode to use 
 @param lnaIndex      Lna index
 @param expectedPower Power level to expect
 */
FtmPacketQueueItem * AddFtmGetLnaOffset(word halOpId, word ftmModeId, word lnaIndex, int16 expectedPower);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to set DVGA offset and adds to pending DIAG queue

 @param halOpId       HAL operation ID
 @param ftmModeId     FTM mode to use 
 @param dvgaOffset    Offset to be set
 */
FtmPacketQueueItem * AddFtmSetDvgaOffset(word halOpId, word ftmModeId, word dvgaOffset);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to calibrate CDMA DVGA using V2 AI and adds 
        to pending DIAG queue

 @param halOpId       HAL operation ID
 @param rxPath        Rx path 0 = main, 1 = diversity
 @param inputPower    Power level at antenna, in AGC
 */
FtmPacketQueueItem * AddFtmCdmaApi2CalibrateDvga(word halOpId, byte rxPath, int16  inputPower);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to calibrate CDMA LNA using V2 AI and adds 
        to pending DIAG queue

 @param halOpId       HAL operation ID
 @param rxPath        Rx path 0 = main, 1 = diversity
 @param gainStep      0 – first LNA gain step (G0-G1), 1 – second LNA gain step (G1-G2), 
                      2 – third LNA gain step (G2-G3), 3 – fourth LNA gain step (G3-G4)
 @param inputPower    Power level at antenna, in AGC
 */
FtmPacketQueueItem * AddFtmCdmaApi2CalibrateLna(word halOpId, byte rxPath, byte gainStep, int16 inputPower);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to calibrate CDMA IntelliCeiver using V2 AI and adds 
        to pending DIAG queue

 @param halOpId       HAL operation ID
 @param rxPath        Rx path 0 = main, 1 = diversity
 @param inputPower    Power level at antenna, in AGC
 */
FtmPacketQueueItem * AddFtmCdmaApi2CalibrateIntelliCeiver(word halOpId, byte rxPath, int16 inputPower);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to get DVGA offset and adds to pending DIAG queue

 @param halOpId       HAL operation ID
 @param ftmModeId     FTM mode to use 
 @param expectedPower Power level to expect
 */
FtmPacketQueueItem * AddFtmGetDvgaOffset(word halOpId, word ftmModeId, int16 expectedPower);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to add wait step to FTM sequence and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param waitTime_us The wait time in us.
 */
FtmPacketQueueItem * AddFtmSeqAddWait(word halOpId, dword waitTime_us);

/*----------------------------------------------------------------------------*/
/*!
 @brief Creates FTM request to add timing marker step to FTM sequence and adds to pending DIAG queue

 @param halOpId     HAL operation ID 
 */
FtmPacketQueueItem * AddFtmSeqAddTimingMarker(word halOpId);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set LNA range and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use 
 @param lnaRange    The LAN range
 */
FtmPacketQueueItem * AddFtmSetLnaRange(word halOpId, word ftmModeId, word lnaRange);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to get CAGC RX AGC and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use 
 */
FtmPacketQueueItem * AddFtmGetRxAgc(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set TX on and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use 
 */
FtmPacketQueueItem * AddFtmSetTxOn(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set TX off and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use 
 */
FtmPacketQueueItem * AddFtmSetTxOff(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set PA range and adds to pending DIAG queue

 @param halOpId     HAL operation ID
 @param ftmModeId   FTM mode to use 
 @param paRange     PA range
 */
FtmPacketQueueItem * AddFtmSetPaRange(word halOpId, word ftmModeId, word paRange);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to set PDM and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 @param ftmModeId  FTM mode to use 
 @param pdmType    PDM type, 2 (FTM_SET_PDM_TYPE_TX_AGC_ADJ), 4 (FTM_SET_PDM_TYPE_TRK_LO_ADJ)
 @param pdmValue   PDM value (0-511)
 */
FtmPacketQueueItem * AddFtmSetPdm(word halOpId, word ftmModeId, word pdmType, word pdmValue);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to select CW waveform and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 @param ftmModeId  FTM mode to use 
 
 */
FtmPacketQueueItem * AddFtmSetCwWaveformOn(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to deselect CW waveform and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 @param ftmModeId  FTM mode to use 
 
 */
FtmPacketQueueItem * AddFtmSetCwWaveformOff(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to load RF NV itmes and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 
 */
FtmPacketQueueItem * AddFtmLoadRfNv(word halOpId);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to load RF NV itmes and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 @param bandClass  Band class 0-6 

 */
FtmPacketQueueItem * AddFtmCommitRfToNv(word halOpId, byte bandClass);

//------------------------------------------------------------------------------------
/*!
@brief Creates FTM request to set secondary chain and adds to pending DIAG queue

 @param halOpId    HAL operation ID
 @param ftmModeId  FTM mode to use 
 @param chain      From QMSL:
				            0.. Disabled puts the secondary chain ZIFRIC and ZIFLNA in disabled 
                        mode via SBI
                    1..	Receive Diversity  enables the secondary chain ZIFRIC and ZIFLN 
                        via SBI and switches the RX_VCO_SEL MUX to the primary chain PLL; 
                        this means that both the primary and secondary chain are set to 
                        the same band class and channel.
				            2.. Off frequency search (OFS) = allows you to set the secondary 
                        chain to a band class and channel that is independent of the 
                        primary chain settings
 */
FtmPacketQueueItem * AddFtmSetSecondaryChain(word halOpId, word ftmModeId, word chain);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set RX burst and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param slotNumber  The slot number
 @param rxFunction  The RX function, see table below
 @code
			0  - RX_FUNCTION_GET_RSSI
			1  - RX_FUNCTION_GET_DC_OFFSETS
			2  - RX_FUNCTION_STOP_CONTINOUS_RX
 @endcode

 */
FtmPacketQueueItem * AddFtmSetRxBurst(word halOpId, word ftmModeId, byte slotNumber, word rxFunction);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set RX on countinuously.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param rxFunction  The RX function, see table below
 @code
      3 – RX_FUNCTION_START_CONTINOUS_RX
      4 – RX_FUNCTION_TURN_OFF_RECEIVER
 @endcode
 
 */
FtmPacketQueueItem * AddFtmSetRxContinuous(word halOpId, word ftmModeId, word rxFunction);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set TX burst and adds to pending DIAG queue.

 @param halOpId             HAL operation ID.
 @param ftmModeId           FTM mode to use.
 @param slotNumber          ???? NULL
 @param dataSource          See table below for possible values
 @param tscIndex            Training sequence index, 0 to 9
 @param numberOfBursts      ???? NULL
 @param isInfiniteDuration  Tx duration; should set to 1 for transmitting continuously

 @code
			0  - Random data
			1  - Tone
			2  - Buffer data
 @endcode

 */
FtmPacketQueueItem * AddFtmSetTxBurst(word halOpId, word ftmModeId, byte slotNumber, word dataSource, byte tscIndex, dword numberOfBursts, byte isInfiniteDuration);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to calibrate RX gain ranges and adds to pending DIAG queue.

 @param halOpId          HAL operation ID.
 @param ftmModeId        FTM mode to use.
 @param rxRangeBitMask   0xF for 4 gain ranges, 0x1F for 5 gain ranges
 @param numberOfAverages Number of averages, use > 1
 @param toggleRx         Flag to toggle RX or leave it alone

 */
FtmPacketQueueItem * AddFtmRxGainRangeCal(word halOpId, word ftmModeId, byte rxRangeBitMask, byte numberOfAverages, byte toggleRx);



//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to read RSSI and adds to pending DIAG queue.

 @param halOpId          HAL operation ID.
 @param ftmModeId        FTM mode to use.

 */
FtmPacketQueueItem * AddFtmGetRssi(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to write RFNV item and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param rfnvID      Identifier for the RFNV.
 @param buffer      RFNV item data
 @param bufferSize  Size of the buffer.

 */
FtmPacketQueueItem * AddFtmRfnvWrite(word halOpId, word rfnvID, uint8 *buffer, word bufferSize);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to read RFNV item and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param rfnvID      Identifier for the RFNV.

 */
FtmPacketQueueItem * AddFtmRfnvRead(word halOpId, word rfnvID);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to read ADC and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param adcChannel  ADC channel to read.

 */
FtmPacketQueueItem * AddFtmGetAdc(word halOpId, word ftmModeId, word adcChannel);


//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set TX bandwidth and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param bandwidth   Bandwidth
                    0 – 1.4 MHz
                    1 – 3 MHz
                    2 – 5 MHz
                    3 – 10 MHz
                    4 – 15 MHz
                    5 – 20 MHz

 */
FtmPacketQueueItem * AddFtmSetTxBandwidth(word halOpId, word ftmModeId, byte bandwidth);

//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set RX bandwidth and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param bandwidth   Bandwidth
                    0 – 1.4 MHz
                    1 – 3 MHz
                    2 – 5 MHz
                    3 – 10 MHz
                    4 – 15 MHz
                    5 – 20 MHz

 */
FtmPacketQueueItem * AddFtmSetRxBandwidth(word halOpId, word ftmModeId, byte bandwidth);


//------------------------------------------------------------------------------------
/*!
 @brief Creates FTM request to set TX modulation type and adds to pending DIAG queue.

 @param halOpId     HAL operation ID.
 @param ftmModeId   FTM mode to use.
 @param modulation  0 - QPSK, 1 - 16 QAM, 2 - 64 QAM

*/
FtmPacketQueueItem * AddFtmSetTxModulationType(word halOpId, word ftmModeId, dword modulation);


//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set TX waveform type and adds to pending DIAG queue.

  @param halOpId            HAL operation ID.
  @param ftmModeId          FTM mode to use.
  @param waveform           0-CW, 1- PUSCH, 2- PUCCH, 3 - PRACH, 4 - SRS, 5 - UpPTS
  @param inumRBsPUSCH       Number of UL resource blocks to transmit on the PUSCH.
  @param inumRBsPUCCH       Number of UL resource blocks to transmit on the PUCCH.
  @param iPUSCHStartRBIndex Starting resource block index on the PUSCH.

*/
FtmPacketQueueItem * AddFtmSetTxWaveform(word halOpId, word ftmModeId, byte waveform, byte inumRBsPUSCH, byte inumRBsPUCCH, byte iPUSCHStartRBIndex);

//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to stop TX waveform type and adds to pending DIAG queue.

  @param halOpId            HAL operation ID.
  @param ftmModeId          FTM mode to use.

*/
FtmPacketQueueItem * AddFtmStopTxWaveform(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set TX gain index type and adds to pending DIAG queue.

  @param halOpId            HAL operation ID.
  @param ftmModeId          FTM mode to use.
  @param gainIndex          ?????

*/
FtmPacketQueueItem * AddFtmSetTxGainIndex(word halOpId, word ftmModeId, word gainIndex);


//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set TX frame matrix and adds to pending DIAG queue.

  @param halOpId            HAL operation ID.
  @param ftmModeId          FTM mode to use.
  @param slotNumber         Slot number 0-7
  @param isSlotOn           0 = OFF, 1 = ON
  @param powerLevel         dBm * 100
  @param modulationType     0 = MCS1, 1 = MCS2, 8 = MCS9

*/
FtmPacketQueueItem * AddFtmSetTxFrameMatrix(word halOpId, word ftmModeId, byte slotNumber, byte isSlotOn, word powerLevel, byte modulationType);


//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to perform GSM TX gain sweep and adds to pending DIAG queue.

  @param halOpId      HAL operation ID.
  @param ftmModeId    Ftm mode is GSM EXTENDED C0 or C2
  @param listSize     Number of items in configData
  @param configData   Inputs for each segment in sweep, see GSM_DRIVER_AMP_CAL_DATA

*/
FtmPacketQueueItem * AddFtmGsmTxGainSweep(word halOpId, word ftmModeId, word listSize, GSM_GAIN_SWEEP_DATA *configData);


//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set GSM TX linear PA range and adds to pending DIAG queue.

  @param halOpId      HAL operation ID.
  @param ftmModeId    Ftm mode is GSM C0 or C2
  @param slotNumber   The slot for which the PA range is set
  @param paRange      The PA Range

*/
FtmPacketQueueItem * AddFtmSetGsmLinearPaRange(word halOpId, word ftmModeId, word slotNumber, word paRange);

//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set GSM TX linear RGI and adds to pending DIAG queue.

  @param halOpId      HAL operation ID.
  @param ftmModeId    Ftm mode is GSM C0 or C2
  @param slotNumber   The slot for which the RGI range is set
  @param rgi          RGI
  @param modulation   Modulation, 1 – GMSK, 0 – 8PSK

*/
FtmPacketQueueItem * AddFtmSetGsmLinearRgi(word halOpId, word ftmModeId, word slotNumber, word rgi, byte modulation);

//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to set GSM TX AMAM sweep version 2 and adds to pending DIAG queue.

  @param halOpId        HAL operation ID.
  @param ftmModeId      Ftm mode is GSM EXTENDED C0 or C2
  @param preDcDuration  Duration of the DC waveform (Units : Quarter Symbols)
  @param edgeDuration   Duration of the calibrated EDGE waveform (Units : Quarter Symbols)
  @param calRgi         Calibration RGI (Units : None)

*/
FtmPacketQueueItem * AddFtmTxCfg2AmamSweepV2(word halOpId, word ftmModeId, word preDcDuration, word edgeDuration, word calRgi);

//------------------------------------------------------------------------------------
/*!
  @brief Creates FTM request to perform GSM TX ENVDC (Envelope DC Carrier Suppression Sweep) 
         calibration and adds to pending DIAG queue.

  @param halOpId      HAL operation ID.
  @param ftmModeId    Ftm mode is GSM EXTENDED C0 or C2
  @param listSize     Number of items in configData
  @param configData   Inputs for each segment in sweep, see GSM_ENVDC_CS_CAL_DATA
*/
FtmPacketQueueItem * AddFtmGsmEnvDcSweep(word halOpId, word ftmModeId, word listSize, GSM_ENVDC_CS_CAL_DATA* configData);

//------------------------------------------------------------------------------------
/*!
  @brief Set phones phone calibration state, see FTM_SET_CALIBRATION_STATE in 
         80-VA888-1 for more information.

  @param halOpId     HAL operation ID.
  @param stateActive CAL_STATE_INACTIVE = 0, CAL_STATE_ACTIVE = 1
                    
*/
FtmPacketQueueItem * AddFtmSetCalibrationState(word halOpId, byte state);

//------------------------------------------------------------------------------------
/*!
  @brief This command waits RX to raise to specified level or timeout to happen if
         level is not reached.

  @param halOpId        HAL operation ID.
  @param rfTechnology   Technology See @ref _80-VA888-1
  @param triggerLevel   Power in 1/16 dBm for GSM and 1/10 dBm for other technologies
  @param timeout        Timeout in microseconds, maximum value is 5,000,000 ie. 5 seconds
                    
*/
FtmPacketQueueItem * AddFtmRxWaitDbm(word halOpId, word rfTechnology, int16 triggerLevel, dword timeout);

//------------------------------------------------------------------------------------
/*!
  @brief This command waits RX to raise to specified level or timeout to happen if
         level is not reached.

  @param halOpId          HAL operation ID.
  @param rfTechnology     Technology See @ref _80-VA888-1
  @param deviceId         Device ID to use
  @param signalDirection  Direction of signel to trigger to, 0-rising, 1-falling
  @param triggerLevel     Power in 1/16 dBm for GSM and 1/10 dBm for other technologies
  @param timeout          Timeout in microseconds, maximum value is 30,000,000 ie. 30 seconds
                    
*/
FtmPacketQueueItem * AddFtmRxWaitDbmV2(word halOpId, word rfTechnology, uint16 deviceId, uint16 signalDirection, int16 triggerLevel, dword timeout);

//------------------------------------------------------------------------------------
/*!
  @brief This command expects PC/user to pass power in dbm16 and number of averages. 
         This command then chooses the right LNA gain state based on the powerdbm16 passed.

  @param halOpId       HAL operation ID.
  @param ftmModeId     Ftm mode is GSM EXTENDED C0 or C2
  @param expectedLevel Expected Rx power in 1/16 dBm units
  @param averages      Number of averages
                    
*/
FtmPacketQueueItem * AddFtmGsmSetupRxBurstForExpectedPower(word halOpId, word ftmModeId, int16 expectedLevel, word averages);

//------------------------------------------------------------------------------------
/*!
  @brief Set TX power as dBm units

  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use.
  @param enable         TRUE or FASLE
  @param powerLevel     Power level to set
                    
*/
FtmPacketQueueItem * AddFtmSetTxPowerDbm(word halOpId, word ftmModeId, boolean enable, word powerLevel);

//------------------------------------------------------------------------------------
/*!
  @brief This command configures the LTE Network Signal (NS) value that will be used 
         for the FTM_SET_TX_POWER_DBM command. This value is used to determine the 
         Additional Max Power Backoff to reduce spectrum emissions.

  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use.
  @param signalValue    Configures the NS value, which affects maximum output power
                    
*/
FtmPacketQueueItem * AddFtmLteSetNetSigValue(word halOpId, word ftmModeId, byte signalValue);

//------------------------------------------------------------------------------------
/*!
  @brief This command is specific to LTE TDD mode of operation. It serves two purposes:
         - To set the TDD uplink-downlink configuration as described in 3GPP spec
         - To specify if the UE has to transmit on all subframes within a frame
 
  @param halOpId             HAL operation ID.
  @param ftmModeId           FTM mode to use.
  @param subframeAssignment  Subframe config, 0-6
  @param continuousTx        0 = subframeAssignment is used, 1 countinuous TX
                    
*/
FtmPacketQueueItem * AddFtmLteSetTddParams(word halOpId, word ftmModeId, byte subframeAssignment, byte continuousTx);


//------------------------------------------------------------------------------------
/*!
  @brief Get GSM RX PRx and DRx power as dBm units

  @param halOpId     HAL operation ID.
  @param ftmModeId   Ftm mode is GSM EXTENDED C1 or C3
                    
*/
FtmPacketQueueItem * AddFtmGsmGetMeasPwrPrxDrx(word halOpId, word FtmModeId);


//------------------------------------------------------------------------------------
/*!
  @brief Get RX power as dBm units for WCDMA, CDMA & LTE
  
  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use.
                    
*/
FtmPacketQueueItem * AddFtmGetRxLevelDbm(word halOpId, word ftmModeId);

typedef enum {
  XO_COARSE_CAL      = 1,
  XO_COLLECT_SAMPLES = 2,
  XO_FT_CURVE        = 3
} XO_CAL_ACTION;

typedef PACK(struct) {
  uint16 lowChannel;
  uint16 highChannel;
} kv_channel;


typedef PACK(struct) {
  uint16 numberOfBands;
  uint16 numberOfTxCycles;
  uint16 numberOfMeasurements;
  uint16 gsmRfModeList[4];
  kv_channel channelList[4];
} kv_v2_parameters;

extern kv_v2_parameters currentKvParameters;
extern ftm_xo_cal_parameter_type currentXoParameters;

//------------------------------------------------------------------------------------
/*!
  @brief Perform Kvco cal V2

  @details Function takes parameters from @ref currentKvParameters data type which can
           be populated with RFIC_WRITE function.

  @param halOpId      HAL operation ID.
  @param ftmModeId    Ftm mode is GSM EXTENDED C0 or C2
  
*/
FtmPacketQueueItem * AddFtmGsmTxKvCalV2(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
  @brief Perform XO cal
  
  @details Function takes parameters from @ref currentXoParameters data type which can
           be populated with RFIC_WRITE function.


  @param halOpId        HAL operation ID.
  @param calAction      XO command to perform, see XO_CAL_ACTION
  @param version        Version
                    
*/
FtmPacketQueueItem * AddFtmXoCal(word halOpId, uint16 calAction, byte version);

//------------------------------------------------------------------------------------
/*!
  @brief Read thermistor
  
  @details This command is designed to allow the user to measure the enhanced thermistor 
           ADC (ADC width > 8 bits). Before the command is called, the phone must be in 
           a particular mode (CDMA, WCDMA, etc) by first calling the FTM_SET_MODE command.

  @param halOpId        HAL operation ID.
                    
*/
FtmPacketQueueItem * AddFtmGetEnhTherm(word halOpId);

//------------------------------------------------------------------------------------
/*!
  @brief Do self calibration
  
  @details This command performs required internal device calibration for all devices 
           on the current target (e.g., RC-tuner calibration and DAC calibration). 
           It performs the calibration, regardless of the state of the NV items.

  @param halOpId        HAL operation ID.
                    
*/
FtmPacketQueueItem * AddFtmDoSelfCal(word halOpId);

//------------------------------------------------------------------------------------
/*!
  @brief Do tuner calibration
  
  @details This command performs required internal tuner self calibration.
 
  @note This FTM command might not be in all modem builds

  @param halOpId        HAL operation ID.
                    
*/
FtmPacketQueueItem * AddFtmDoTunerSelfCal(word halOpId);

//------------------------------------------------------------------------------------
/*!
  @brief   Set power mode
  
  @details This command overrides the automatic IntelliCeiver™ Power mode decision. 
           This setting is lost when the channel is changed with the FTM_SET_CHAN API, 
           or when the phone is returned to Online mode.

  @param halOpId     HAL operation ID.
  @param ftmModeId   FTM mode to use 
  @param powerMode   0 – High Power mode, 1 – Mid Power mode, 2 – Low Power mode

                    
*/
FtmPacketQueueItem * AddFtmSetPowerMode(word halOpId, word ftmModeId, byte powerMode);

//------------------------------------------------------------------------------------
/*!
  @brief   Config cal sweep
  
  @details This command is used by enhanced sweep calibration to configure the packet and 
           execute the calibration sweep. The size limitation of this single autonomous 
           packet is limited by the RFCA packet helper module.

  @param halOpId          HAL operation ID.
  @param startCalSweep    1 - final packet, execute sweep, 0 - more data will be sent
  @param lastSweepFlag    1 – last sweep, clean up done, 0 - more same sweeps to follw
  @param enableF3msg      1 - enable F3 messages
  @param numberOfSegments Number of segments in the linked list
  @param sweepList        Pointer to the first item in the list
                    
*/
FtmPacketQueueItem * AddFtmCfgAndExecCalSweep(word halOpId, byte startCalSweep, byte lastSweepFlag, byte enableF3msg, word numberOfSegments, FTM_Cal_Sweep_Opcode_Req_List_Item *sweepList);

//------------------------------------------------------------------------------------
/*!
  @brief   Config cal sweep
  
  @details This command is used by enhanced sweep calibration to configure the packet and 
           execute the calibration sweep. The size limitation of this single autonomous 
           packet is limited by the RFCA packet helper module.

  @param halOpId            HAL operation ID.
  @param compressedPayload  true or false
  @param startCalSweep      1 - final packet, execute sweep, 0 - more data will be sent
  @param lastSweepFlag      1 – last sweep, clean up done, 0 - more same sweeps to follw
  @param enableF3msg        1 - enable F3 messages
  @param sweepData          Pointer to the sweep data
                   
*/
FtmPacketQueueItem * AddFtmCfgAndExecCalSweepFlex(word halOpId, bool compressedPayload, byte startCalSweep, byte lastSweepFlag, byte enableF3msg, HAL_RF_SWEEP_SEGMENT_CONFIG_DATA *sweepData);

//------------------------------------------------------------------------------------
/*!
  @brief   Get cal sweep results @see AddFtmCfgAndExecCalSweep
  
  @param halOpId          HAL operation ID.

*/
FtmPacketQueueItem * AddFtmGetCalSweepResults(word halOpId);

//------------------------------------------------------------------------------------
/*!
  @brief   Add raw byte data to message queue
  
  @param halOpId   HAL operation ID.
  @param size      Size of data in bytes
  @param rawMsg    Data to be added

*/
FtmPacketQueueItem * AddFtmRawMsg(word halOpId, word size, byte* rawMsg);


//------------------------------------------------------------------------------------
/*!
  @brief   Enables or disables TDSCDMA burst mode TX
  
  @param halOpId    HAL operation ID.
  @param ftmModeId  FTM mode to use 
  @param state      TX mode: 0 = off, 1 = on

*/
FtmPacketQueueItem * AddFtmTdsCdmaBurstTx(word halOpId, word ftmModeId, byte state);


//------------------------------------------------------------------------------------
/*!
  @brief   Configures TDSCDMA burst mode frame output
  
  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use 
  @param numberOfSlots  Size of data in bytes
  @param modulation     Modulation: 0 = QPSK, 1 = 8PSK ??, 2 = 16QAM
  @param powerLevels    Array of power levels in dBm10 for each slot 

*/
FtmPacketQueueItem * AddFtmTdsCdmaSetBurstTxParams(word halOpId, word ftmModeId, byte numberOfSlots, byte modulation, word powerLevels[5]);

//------------------------------------------------------------------------------------
/*!
  @brief   Enables or disables TDSCDMA burst mode RX
  
  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use 
  @param expectedLevel  Expected signal level as dBm10
  @param state          RX mode: 0 = off, 1 = on

*/
FtmPacketQueueItem * AddFtmTdsCdmaBurstRx(word halOpId, word ftmModeId, word expectedLevel, byte state);

//------------------------------------------------------------------------------------
/*!
  @brief   Enables or disables TDSCDMA burst mode RX
  
  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use 
  @param port           Antenna port 0 – Rx0, 1 – Rx1, 2 – Rx0 + Rx1 

*/
FtmPacketQueueItem * AddFtmTdsCdmaIqLogging(word halOpId, word ftmModeId, word port);

//------------------------------------------------------------------------------------
/*!
  @brief   Set frequency adjustment
  
  @param halOpId        HAL operation ID.
  @param ftmModeId      FTM mode to use 
  @param expectedLevel  Expected signal level as dBm10
  @param state          RX mode: 0 = off, 1 = on

  @param userFreqAdjustPpb   User defined frequency adjustment in ppb units, ppb = Frequency Adjust (Hz)/ Center Frequency (MHz) * 1000
  @param centerFreqHz        Carrier frequency for the current cellular band/channel, in Hz’s units.
  @param enableXoFreqAdjust  0 – Total Frequency Adjust = User defined frequency adjustment (i.e. ,user_freq_adjust_ppb)
                             1 – Total Frequency Adjust = User defined frequency adjustment (i.e., user_freq_adjust_ppb) + Frequency error reported by XO manager.

*/
FtmPacketQueueItem * AddFtmSetFreqAdjust(word halOpId, word ftmModeId, dword userFreqAdjustPpb, dword centerFreqHz, byte enableXoFreqAdjust);

//------------------------------------------------------------------------------------
/*!
  @brief   Sets the Rx LNA range based on a target dBm downlink level.
  
  @param halOpId              HAL operation ID.
  @param ftmModeId            FTM mode to use 
  @param expectedRxPowerDbm   Expected signal level as dBm10

*/
FtmPacketQueueItem * AddFtmLteSetLnaStateDbm(word halOpId, word ftmModeId, int16 expectedRxPowerDbm);

//------------------------------------------------------------------------------------
/*!
  @brief   Executes GSM frame sweep.
  
  @param halOpId      HAL operation ID.

*/
FtmPacketQueueItem * AddFtmGsmFrameSweepExec(word halOpId);

//------------------------------------------------------------------------------------
/*!
  @brief   Configures GSM sweep
  
  @param halOpId             HAL operation ID.
  @param masterSweepConfig   Pointer to master sweep configuration

  @note masterSweepConfig->used_bytes_count bytes are sent and
        FTM requests are split to multiple diag messages if needed

*/
FtmPacketQueueItem * AddFtmGsmFrameSweepConfig(word halOpId, ftm_gsm_frame_sweep_config_type* masterSweepConfig);

//------------------------------------------------------------------------------------
/*!
  @brief   Executes GSM frame sweep.
  
  @param halOpId      HAL operation ID.
  
*/
FtmPacketQueueItem * AddFtmGsmFrameSweepGetResults(word halOpId);


//------------------------------------------------------------------------------------
/*!
  @brief   Gets IQ capture configuration
  
  @param halOpId     HAL operation ID.
  @param ftmModeId   FTM mode to use

*/
FtmPacketQueueItem * AddFtmIqCaptureGetConfig(word halOpId, word ftmModeId);

//------------------------------------------------------------------------------------
/*!
  @brief   Aquires IQ samples
  
  @param halOpId     HAL operation ID.
  @param ftmModeId   FTM mode to use
  @param sampleSize  Number of samples to capture
  @param ftmModeId   Carrier ID, 0 or 1

*/
FtmPacketQueueItem * AddFtmIqCaptureAquireSamples(word halOpId, word ftmModeId, dword sampleSize, byte carrierId);

//------------------------------------------------------------------------------------
/*!
  @brief   Get IQ samples
  
  @param halOpId     HAL operation ID.
  @param ftmModeId   FTM mode to use

*/
FtmPacketQueueItem * AddFtmIqCaptureGetSamples(word halOpId, word ftmModeId, dword sampleOffset, dword sampleSize);

//------------------------------------------------------------------------------------
/*!
  @brief   Get IQ samples
  
  @param halOpId      HAL operation ID.
  @param ftmModeId    FTM mode to use
  @param captureMode  Capture mode:
                      0 = Single shot logging
                      1 = Continuous logging
                      2 = Stop IQ logging

*/
FtmPacketQueueItem * AddFtmGsmCaptureIq(word halOpId, word ftmModeId, word captureMode);

//------------------------------------------------------------------------------------
/*!
  @brief   Switch antenna
  
  @param halOpId          HAL operation ID
  @param device           RFM device id
  @param position         Position
  @param actionImmediate  Switch immediately

*/
FtmPacketQueueItem * AddFtmSetAntenna(word halOpId, uint8 device, uint8 position, uint8 actionImmediate);

//------------------------------------------------------------------------------------
/*!
  @brief   Enables or disabled filtered path
 
  @param halOpId          HAL operation ID
  @param filterStatus     Filter status
 
*/
FtmPacketQueueItem * AddFtmSetFilteredPath(word halOpId, word filteredPatatState);

//------------------------------------------------------------------------------------
/*!
  @brief   Sets path override with FTM_SET_PATH_SEL_OVERRIDE_INDEX
 
  @param halOpId        HAL operation ID
  @param ftmModeId      FTM mode to use
  @param carrierId      0 - Primary cell, 1 - Secondary cell
  @param band           B28 = 64
  @param txPathSelect   0 – Duplexer A, 1 - Duplexer B, 255 - Normal (online) operation
  @param prxPathSelect  0 – Duplexer A, 1 - Duplexer B, 255 - Normal (online) operation
  @param drxPathSelect  0 – Duplexer A, 1 - Duplexer B, 255 - Normal (online) operation
 
*/
FtmPacketQueueItem * AddFtmSetPathOverrideIndex(word halOpId, word ftmModeId, byte carrierId, byte band, byte txPathSelect, byte prxPathSelect, byte drxPathSelect);

//------------------------------------------------------------------------------------
/*!
  @brief   Performs TxLO and Residual Side Band (RSB) calibration 
 
  @param halOpId      HAL operation ID
  @param rfMode       FTM RF mode from FTM_RF_Mode_Enum
  @param device       Main device 0 or 2
  @param divDevice    Diversity device 1 or 3
  @param exitRequired Set to 1 if rf teardown is required
 
*/
FtmPacketQueueItem * AddFtmDoRsbTxloCal(word halOpId, byte rfMode, byte device, byte divDevice, byte exitRequired);

//------------------------------------------------------------------------------------
/*!
  @brief   RFFE command
  
  @param halOpId      HAL operation ID.
  @param extMode      Use extended mode? 0 – Use non-extended mode, 1 – Use extended mode
  @param readWrite    Read or write transaction? 0 – Write, 1 – Read
  @param channel      RFFE channel (bus)
  @param slave        RFFE device slave ID
  @param address      RFFE device address
  @param speed        Use half speed (not applicable to write transactions),
                      0 – Do not use half speed reads
                      1 – Use half speed reads
  
*/
FtmPacketQueueItem * AddFtmDoRffeCmd(word halOpId, byte extMode, byte readWrite, byte channel, byte slave, word address, byte speed );

#ifdef _WIN32
  #pragma pack(pop) // Restore
#endif

#endif // _RFCA_FTM_PACKAGE_HELPER_H_
