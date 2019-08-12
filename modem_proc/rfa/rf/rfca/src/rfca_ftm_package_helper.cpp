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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_ftm_package_helper.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/10/14   aki     Added AddFtmGsmGetMeasPwrPrxDrx and removed AddFtmGsmGetMeasPwr
                   and AddFtmGsmGetMeasPwrDrx
10/30/14   aki     Added AddFtmGsmGetMeasPwrDrx
08/29/14   aki     Chnanged GSM gain range to PRx/DRx version
09/18/14   aki     Added support for compressed sweep payload
04/16/14   aki     Rewrote AddFtmCfgAndExecCalSweepFlex
03/20/14   aki     Added AddFtmSetPathOverrideIndex
01/03/14   aki     Removed RFCA_ASSERT
12/09/13   aki     Added AddFtmDoRffeCmd
11/25/13   aki     Log message update
11/19/13   aki     Added AddFtmDoRsbTxloCal
11/14/13   aki     Added AddFtmSetFilteredPath
09/09/13   Aki     Added AddFtmSetAntenna
08/12/13   aki     Fixed KW errors
08/02/13   aki     Removed AddFtmGsmPathDelay and added IQ functions and updated
                   GSM fuctions to include FTM mode id
07/16/13   aki     Added AddFtmCfgAndExecCalSweepFlex
06/29/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
06/04/13   aki     Support variable length op-codes
05/17/13   aki     Updated GSM SWEEP to newly renamed types
05/03/13   aka     Updated GSM SWEEP request size field
05/02/13   aki     Addded AddFtmRxWaitDbmV2
04/29/13   aki     word to int16 and corrected log message
04/25/13   aki     Added GSM sweep functions
04/10/13   aki     Added AddFtmSetFreqAdjust
02/04/13   aki     Added AddFtmLteSetTddParams
01/16/13   aki     Added support for V3 polling
12/10/12   aki     Updated XO
12/03/12   aki     Added AddFtmStopTxWaveform
11/29/12   tws     Rename FTM_GSM_C to FTM_GSM_C0_C to avoid confusion.
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/11/12   aki     Added AddFtmSetGsmLinearRgi and corrected one typo
08/28/12   aki     Updated ftm mode id for tdscdma
08/23/12   aki     Correct log print for AddFtmTdsCdmaSetBurstTxParams
08/16/12   aki     Added AddFtmSetFreqAdjust
07/23/12   aki     Added TDSCDMA TX/RX burst support
07/12/12   aki     Addded ftmModeId to log messages, AddFtmRawMsg function,
                   support for multi-result segments V3 sweep 
05/25/12   aki     Cdma API2 functions for RX cal, correct AddFtmGetRssi
04/23/12   aki     Multi channel support, waveform removed from amam sweep,
                   calibration state takes byte instead of bool, added net sig val,
				   kvco values taken from global define, support for rest atp, 
				   removed old KVCO
				   sweep op codes
03/01/12   aki     Changed <> to "", ATP sweep support, timing marker, cleanup
02/17/12   aki     Added XO, THER and KV support, diagcmd.h
02/08/12   aki     Call Init on FTM req-resp package
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_ftm_package_helper.h"
#include "rfca_defs.h"
#include "rfca_helper.h"
#include "rfca_mem_helper.h"
#include <stringl/stringl.h>

#include "ftm.h"
#include "msg.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_calibration_xo.h"
#include "ftm_command_id.h"
#include "diagcmd.h"

///< Global instance to manage FTM Sequence IDs in RFCA layer
FtmSequenceID ftmSequenceIDManager;

/*----------------------------------------------------------------------------*/
bool ResetOpcode(FTM_Cal_Sweep_Opcode_Header *sweepItemOpcodeHeader)
{
  // Special handling for first word
  switch(FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16)
  {
  case 1:
    sweepItemOpcodeHeader->opcode[0] = 0;
    break;
  case 4:
    sweepItemOpcodeHeader->opcode[0] = 1 << 14;
    break;
  case 8:
    sweepItemOpcodeHeader->opcode[0] = 2 << 14;
    break;
  case 16:
    sweepItemOpcodeHeader->opcode[0] = 3 << 14;
    break;
  default:
    MSG_ERROR("Unsupported opcode field size in FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 = %d", FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16, 0, 0);
    return false;
    break;
  }

  for(int wordIndexInOpcode=FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16-1; wordIndexInOpcode>0; wordIndexInOpcode--)
  {
    sweepItemOpcodeHeader->opcode[wordIndexInOpcode] = 0;
  }
  return true;
}


/*----------------------------------------------------------------------------*/
bool SetFtmCalSweepOpCodeBit(FTM_Cal_Sweep_Opcode_Header *sweepItemOpcodeHeader, ftm_cal_opcode_bit_type opcodeBit)
{
  uint16 wordIndexInOpcode = opcodeBit/16;
  uint16 bitToSetInWord = (opcodeBit - (wordIndexInOpcode * 16));
  uint16 valueToSet = 1 << bitToSetInWord;

  HalLogWrite("Adding opcode bit %u to word index %u as bit %u in that word.", opcodeBit, wordIndexInOpcode, bitToSetInWord);
  if( (FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 < wordIndexInOpcode)
      || ((FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 - 1 == wordIndexInOpcode) && (bitToSetInWord > 13)))
  {
    MSG_ERROR("Unsupported opcode bit %d, RFCA only supports bits up to %d", opcodeBit, (FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 * 2 * 8)-2, 0);
    return false;
  }
  
  sweepItemOpcodeHeader->opcode[FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16-wordIndexInOpcode-1] |= valueToSet;
  return true;
}

/*!
@brief Creates a new FtmPacketQueueItem and assigned SequenceID

*/
FtmPacketQueueItem * NewFtmPacketQueueItem()
{
  FtmPacketQueueItem * newItem = (FtmPacketQueueItem *)RFCA_Malloc(sizeof(FtmPacketQueueItem));
  if(NULL == newItem)
  {
    MSG_ERROR("Failed to allocate memory for new FTM command.\n", 0, 0, 0);
    return NULL;
  }
  // Need to call reset for packet helper
  newItem->Packet.Init();
  newItem->SequenceID = ftmSequenceIDManager.GetNext();
  newItem->ResponseReceived = false;
  newItem->NextItem = NULL;
  return newItem;
}

/*----------------------------------------------------------------------------*/
void FormatFTMHeader(RfcaPacketHelper *packet, word iModeID )
{

  packet->AppendRequest( (byte) DIAG_SUBSYS_CMD_F );	
  packet->AppendRequest( (byte) DIAG_SUBSYS_FTM );
  packet->AppendRequest( (word) iModeID );			
}


/*----------------------------------------------------------------------------*/
void FormatFTMSequencerPackage2(RfcaPacketHelper *seqPacket, FtmPacketQueueItem** nextFtmItem)
{
  const uint16 sizeOfQueueItemHeader = 6;
  // TODO: Move this to some other place
  const int maxDiagMessageSize = RFCA_PACKET_HELPER_MAX_PACKET_SIZE-1;

  seqPacket->AppendRequest( (byte) DIAG_SUBSYS_CMD_F );	
  seqPacket->AppendRequest( (byte) DIAG_SUBSYS_FTM );
  seqPacket->AppendRequest( (word) FTM_SEQ_C );	
  seqPacket->AppendRequest( (word) FTM_SEQ_QUEUE_COMMANDS );
  seqPacket->AppendRequest( (word) 0 ); // Place holder for now, we'll fill this at the end of the function
  seqPacket->AppendRequest( (word) 12 ); // Response size
  seqPacket->AppendRequest( (word) 0 ); // Place holder for now, we'll fill this at the end of the function

  word sizeOfDiagHeader = seqPacket->GetRequestLength();
  word queueSize = sizeOfDiagHeader;
  word itemCount = 0;
  FtmPacketQueueItem *ftmItem = *nextFtmItem;
  while(NULL != ftmItem) 
  {
    if((queueSize + ftmItem->Packet.GetRequestLength() + sizeOfQueueItemHeader) > (maxDiagMessageSize))
    {
      break;
    }
    itemCount++;
    queueSize += ftmItem->Packet.GetRequestLength() + sizeOfQueueItemHeader;
    seqPacket->AppendRequest( (word) ftmItem->Packet.GetRequestLength() );  
    dword seqId = ftmItem->SequenceID; // Use separate variable as we don't know if sizeof(ftmItem->SequenceID) is same as sizeof(dword)
    seqPacket->AppendRequest( (byte*) &seqId, sizeof(dword) );
    seqPacket->AppendRequest( ftmItem->Packet.GetRequestDataPtr(), ftmItem->Packet.GetRequestLength() );
    *nextFtmItem = ftmItem = ftmItem->NextItem;
  }

  *(word*)(&seqPacket->GetRequestDataPtr()[6]) = queueSize - sizeOfDiagHeader;
  *(word*)(&seqPacket->GetRequestDataPtr()[10]) = itemCount;

}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSetMode(word halOpId, word ftmModeId, word rfMode)
{  
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }
  
  HalLogWrite("F(%05d):_FTM_SET_MODE(ftmModeId = %d, rfMode = %d)\n", ftmMsg->SequenceID, ftmModeId, rfMode);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word)  _FTM_SET_MODE );  
  ftmMsg->Packet.AppendRequest( (word) rfMode );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSetChan(word halOpId, word ftmModeId, word channel)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_CHAN(ftmModeId = %d, channel = %d)\n", ftmMsg->SequenceID, ftmModeId, channel);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_CHAN );  
  ftmMsg->Packet.AppendRequest( (word) channel );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSetMultiChan(word halOpId, word ftmModeId, word first_channel, word second_channel)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_MULTI_CHAN(ftmModeId = %d, first_channel = %d, second_channel = %d)\n", ftmMsg->SequenceID, ftmModeId, first_channel, second_channel);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_MULTI_CHAN );  
  ftmMsg->Packet.AppendRequest( (word) first_channel );
  ftmMsg->Packet.AppendRequest( (word) second_channel );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSetLnaOffset(word halOpId, word ftmModeId, word lnaIndex, word lnaOffset)  
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_LNA_OFFSET(ftmModeId = %d, lnaIndex = %d, lnaOffset = %d)\n", ftmMsg->SequenceID, ftmModeId, lnaIndex, lnaOffset);
  
  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_LNA_OFFSET );  
  ftmMsg->Packet.AppendRequest( (word) lnaIndex ); 
  ftmMsg->Packet.AppendRequest( (word) lnaOffset );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmGetLnaOffset(word halOpId, word ftmModeId, word lnaIndex, int16 expectedPower)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_LNA_OFFSET(ftmModeId = %d, lnaIndex = %d, expectedPower = %d)\n", ftmMsg->SequenceID, ftmModeId, lnaIndex, expectedPower);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_LNA_OFFSET );  
  ftmMsg->Packet.AppendRequest( (word) lnaIndex ); 
  ftmMsg->Packet.AppendRequest( (word) expectedPower );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSetDvgaOffset(word halOpId, word ftmModeId, word dvgaOffset)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_DVGA_OFFSET(ftmModeId = %d, dvgaOffset = %d)\n", ftmMsg->SequenceID, ftmModeId, dvgaOffset);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_DVGA_OFFSET );  
  ftmMsg->Packet.AppendRequest( (word) dvgaOffset );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmGetDvgaOffset(word halOpId, word ftmModeId, int16 expectedPower)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_DVGA_OFFSET(ftmModeId = %d, expectedPower = %d)\n", ftmMsg->SequenceID, ftmModeId, expectedPower);

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_DVGA_OFFSET );  
  ftmMsg->Packet.AppendRequest( (word) expectedPower );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

FtmPacketQueueItem * AddFtmCdmaApi2CalibrateDvga(word halOpId, byte rxPath, int16  inputPower)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CAL_V2_CAL_DVGA(ftmModeId = %d, rxPath = %d, inputPower = %d)\n", ftmMsg->SequenceID, FTM_1X_CAL_V2_C, rxPath, inputPower);

  FormatFTMHeader(&ftmMsg->Packet,  FTM_1X_CAL_V2_C );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CAL_V2_CAL_DVGA );  // Calibrate DVGA
  ftmMsg->Packet.AppendRequest( (word) 0 );                          // Request length
  ftmMsg->Packet.AppendRequest( (word) 0 );                          // Response length
  ftmMsg->Packet.AppendRequest( (byte) rxPath);	                    // Path
  ftmMsg->Packet.AppendRequest( (word) inputPower);                  // Input power Rx AGC
   
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmCdmaApi2CalibrateLna(word halOpId, byte rxPath, byte gainStep, int16 inputPower)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CAL_V2_CAL_LNA(ftmModeId = %d, rxPath = %d, gainStep = %d, inputPower = %d)\n", ftmMsg->SequenceID, FTM_1X_CAL_V2_C, rxPath, gainStep, inputPower);

  FormatFTMHeader(&ftmMsg->Packet,  FTM_1X_CAL_V2_C );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CAL_V2_CAL_LNA );  // Calibrate DVGA
  ftmMsg->Packet.AppendRequest( (word) 0 );                         // Request length
  ftmMsg->Packet.AppendRequest( (word) 0 );                         // Response length
  ftmMsg->Packet.AppendRequest( (byte) rxPath);	                    // Path
  ftmMsg->Packet.AppendRequest( (byte) gainStep);	                  // LNA
  ftmMsg->Packet.AppendRequest( (word) inputPower);                 // Input power Rx AGC
   
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmCdmaApi2CalibrateIntelliCeiver(word halOpId, byte rxPath, int16 inputPower)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CAL_V2_INTELLICEIVER(ftmModeId = %d, rxPath = %d, inputPower = %d)\n", ftmMsg->SequenceID, FTM_1X_CAL_V2_C, rxPath, inputPower);

  FormatFTMHeader(&ftmMsg->Packet,  FTM_1X_CAL_V2_C );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CAL_V2_INTELLICEIVER );  // Calibrate DVGA
  ftmMsg->Packet.AppendRequest( (word) 0 );                         // Request length
  ftmMsg->Packet.AppendRequest( (word) 0 );                         // Response length
  ftmMsg->Packet.AppendRequest( (byte) rxPath);	                    // Path
  ftmMsg->Packet.AppendRequest( (word) inputPower);                 // Input power Rx AGC
   
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSeqAddWait(word halOpId, dword waitTime_us)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SEQ_ADD_WAIT(ftmModeId = %d, waitTime_us = %lu)\n", ftmMsg->SequenceID, FTM_SEQ_C, waitTime_us);
  
  FormatFTMHeader(&ftmMsg->Packet,  FTM_SEQ_C );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SEQ_ADD_WAIT );  
  ftmMsg->Packet.AppendRequest((word) 0 ); // size of payload
  ftmMsg->Packet.AppendRequest((word) 12 ); // size of response
  ftmMsg->Packet.AppendRequest( (byte*)&waitTime_us, sizeof(dword) ); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmSeqAddTimingMarker(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SEQ_ADD_TIMING_MARKER(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_SEQ_C);
  
  FormatFTMHeader(&ftmMsg->Packet,  FTM_SEQ_C );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_SEQ_ADD_TIMING_MARKER );  
  ftmMsg->Packet.AppendRequest((word) 0 ); // size of payload
  ftmMsg->Packet.AppendRequest((word) 0 ); // size of response

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetLnaRange(word halOpId, word ftmModeId, word lnaRange)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_LNA_RANGE(ftmModeId = %d, lnaRange = %d)\n", ftmMsg->SequenceID, ftmModeId, lnaRange);
  
  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_LNA_RANGE);
  ftmMsg->Packet.AppendRequest( (word) lnaRange);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetRxAgc(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_RX_AGC(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId );

  FormatFTMHeader(&ftmMsg->Packet,  ftmModeId );	      
  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_RX_AGC);
  ftmMsg->Packet.AppendRequest( (word) 0);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxOn(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_ON(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId );

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_ON );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxOff(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_OFF(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId );

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_OFF );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}
//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetPaRange(word halOpId, word ftmModeId, word paRange)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_PA_RANGE(ftmModeId = %d, paRange = %d)\n", ftmMsg->SequenceID, ftmModeId, paRange);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_PA_RANGE );
  ftmMsg->Packet.AppendRequest( (word) paRange ); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetPdm(word halOpId, word ftmModeId, word pdmType, word pdmValue)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_PDM(ftmModeId = %d, pdmType = %d, pdmValue = %d)\n", ftmMsg->SequenceID, ftmModeId, pdmType, pdmValue);
  
  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_PDM );
  ftmMsg->Packet.AppendRequest( (word) pdmType ); 
  ftmMsg->Packet.AppendRequest( (word) pdmValue ); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetCwWaveformOn(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CW_WAVEFORM(ftmModeId = %d, waveform = ON)\n", ftmMsg->SequenceID, ftmModeId);
  
  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CW_WAVEFORM );
  ftmMsg->Packet.AppendRequest( (byte) CW_WAVEFORM ); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetCwWaveformOff(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CW_WAVEFORM(ftmModeId = %d, waveform = OFF)\n", ftmMsg->SequenceID, ftmModeId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId );
  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CW_WAVEFORM );
  ftmMsg->Packet.AppendRequest( (byte) CDMA_WAVEFORM ); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmLoadRfNv(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_LOAD_RF_NV(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C);
  
  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C );
  ftmMsg->Packet.AppendRequest( (word) _FTM_LOAD_RF_NV );
  ftmMsg->Packet.AppendRequest( (word)  0 ); 
  ftmMsg->Packet.AppendRequest( (word) 12 );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmCommitRfToNv(word halOpId, byte bandClass)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CDMA_CAL_V2_COMMIT_NV(ftmModeId = %d, bandClass = %d)\n", ftmMsg->SequenceID, FTM_1X_CAL_V2_C, bandClass);

  FormatFTMHeader(&ftmMsg->Packet, FTM_1X_CAL_V2_C );

  ftmMsg->Packet.AppendRequest( (word) _FTM_CDMA_CAL_V2_COMMIT_NV ); // Commit RF To NV
  ftmMsg->Packet.AppendRequest( (word) 0 );	// Request length
  ftmMsg->Packet.AppendRequest( (word) 0 ); // Rsponse length
  ftmMsg->Packet.AppendRequest( (byte) bandClass);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetSecondaryChain(word halOpId, word ftmModeId, word chain)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_SECONDARY_CHAIN(ftmModeId = %d, chain = %d)\n", ftmMsg->SequenceID, ftmModeId, chain);
  
  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_SECONDARY_CHAIN);
  ftmMsg->Packet.AppendRequest( (word) chain );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetRxBurst(word halOpId, word ftmModeId, byte slotNumber, word rxFunction)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_RX_BURST(ftmModeId = %d, slotNumber = %d, rxFunction = %d)\n", ftmMsg->SequenceID, ftmModeId, slotNumber, rxFunction);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_RX_BURST);
  ftmMsg->Packet.AppendRequest( (word) 0 );						// Reserved
  ftmMsg->Packet.AppendRequest( (byte) slotNumber );
  ftmMsg->Packet.AppendRequest( (word) rxFunction );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetRxContinuous(word halOpId, word ftmModeId, word rxFunction)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_SET_RX_CONTINUOUS(ftmModeId = %d, rxFunction = %d)\n", ftmMsg->SequenceID, ftmModeId, rxFunction);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_SET_RX_CONTINUOUS);
  ftmMsg->Packet.AppendRequest( (word) 0 );						// Reserved
  ftmMsg->Packet.AppendRequest( (byte) 0 );
  ftmMsg->Packet.AppendRequest( (word) rxFunction );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxBurst(word halOpId, word ftmModeId, byte slotNumber, word dataSource, byte tscIndex, dword numberOfBursts, byte isInfiniteDuration)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TRANSMIT_BURST(ftmModeId = %d, slotNumber = %d, dataSource = %d, tscIndex = %d, numberOfBursts = %li, isInfiniteDuration = %d)\n", ftmMsg->SequenceID, ftmModeId, slotNumber, dataSource, tscIndex, numberOfBursts, isInfiniteDuration );

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TRANSMIT_BURST );	// Command
	ftmMsg->Packet.AppendRequest( (word) 0 );							// Reserved
	ftmMsg->Packet.AppendRequest( (byte)  slotNumber );
	ftmMsg->Packet.AppendRequest( (word)  dataSource );
	ftmMsg->Packet.AppendRequest( (byte)  tscIndex );
	ftmMsg->Packet.AppendRequest( (byte*) &numberOfBursts, sizeof(dword) );
	ftmMsg->Packet.AppendRequest( (byte)  isInfiniteDuration );


  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmRxGainRangeCal(word halOpId, word ftmModeId, byte rxRangeBitMask, byte numberOfAverages, byte toggleRx)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_RX_GAIN_RANGE_PRX_DRX_CAL(ftmModeId = %d, rxRangeBitMask = %d, numberOfAverages = %d, toggleRx = %d)\n", ftmMsg->SequenceID, ftmModeId, rxRangeBitMask, numberOfAverages, toggleRx);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);
  
  ftmMsg->Packet.AppendRequest( (word) FTM_RX_GAIN_RANGE_PRX_DRX_CAL );
  ftmMsg->Packet.AppendRequest( (word) 0 );	// Reserved
  ftmMsg->Packet.AppendRequest( (byte) rxRangeBitMask );
  ftmMsg->Packet.AppendRequest( (byte) numberOfAverages );
  ftmMsg->Packet.AppendRequest( (byte) toggleRx );
  const int rssiBufferSize = 16*4;
  byte rssiBuffer[rssiBufferSize];
  memset(rssiBuffer, 0, rssiBufferSize);
  ftmMsg->Packet.AppendRequest(rssiBuffer, rssiBufferSize);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetRssi(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_RSSI(ftmModeId = %d, )\n", ftmMsg->SequenceID, ftmModeId);


  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);
  
  dword placeHolder = 0x00000000;
  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_RSSI);
  ftmMsg->Packet.AppendRequest( (word) 0						);	// Reserved
  ftmMsg->Packet.AppendRequest( (byte*)&placeHolder, sizeof(dword) ); 
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmRfnvWrite(word halOpId, word rfnvID, uint8 *buffer, word bufferSize)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_RFNV_WRITE(ftmModeId = %d, rfnvID = %d)\n", ftmMsg->SequenceID, FTM_RFNV_C, rfnvID);

  FormatFTMHeader(&ftmMsg->Packet, FTM_RFNV_C);

  ftmMsg->Packet.AppendRequest( (word)_FTM_RFNV_WRITE );
  ftmMsg->Packet.AppendRequest( (word)(0x0000) );
  ftmMsg->Packet.AppendRequest( (word)(0x000E) );  // 0x0E always
  ftmMsg->Packet.AppendRequest( (word) rfnvID );
  ftmMsg->Packet.AppendRequest( (word) bufferSize );
  ftmMsg->Packet.AppendRequest( buffer, bufferSize );
 
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmRfnvRead(word halOpId, word rfnvID)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_RFNV_READ(ftmModeId = %d, rfnvID = %d)\n", ftmMsg->SequenceID, FTM_RFNV_C, rfnvID);

  FormatFTMHeader(&ftmMsg->Packet, FTM_RFNV_C);

  ftmMsg->Packet.AppendRequest( (uint16)_FTM_RFNV_READ );
  ftmMsg->Packet.AppendRequest( (uint16)(0x0000) );
  ftmMsg->Packet.AppendRequest( (uint16)(0x0090) );  // 144 = 128 nv item size + 16 message header size
  ftmMsg->Packet.AppendRequest( (word) rfnvID );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetAdc(word halOpId, word ftmModeId, word adcChannel)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_ADC_VAL(ftmModeId = %d, adcChannel = %d)\n", ftmMsg->SequenceID, ftmModeId, adcChannel);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);
  
  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_ADC_VAL);
  ftmMsg->Packet.AppendRequest( (word) adcChannel );	
  ftmMsg->Packet.AppendRequest( (word) 0 );	
 
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxBandwidth(word halOpId, word ftmModeId, byte bandwidth)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_BANDWIDTH(ftmModeId = %d, bandwidth = %d)\n", ftmMsg->SequenceID, ftmModeId, bandwidth);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_BANDWIDTH );
	ftmMsg->Packet.AppendRequest( (byte) bandwidth);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetRxBandwidth(word halOpId, word ftmModeId, byte bandwidth)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_RX_BANDWIDTH(ftmModeId = %d, bandwidth = %d)\n", ftmMsg->SequenceID, ftmModeId, bandwidth);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_RX_BANDWIDTH );
	ftmMsg->Packet.AppendRequest( (byte) bandwidth);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxModulationType(word halOpId, word ftmModeId, dword modulation)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_LTE_SET_MODULATION_TYPE(ftmModeId = %d, modulation = %d)\n", ftmMsg->SequenceID, ftmModeId, modulation);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_LTE_SET_MODULATION_TYPE );
	ftmMsg->Packet.AppendRequest( (byte*) &modulation, sizeof(dword));

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxWaveform(word halOpId, word ftmModeId, byte waveform, byte inumRBsPUSCH, byte inumRBsPUCCH, byte iPUSCHStartRBIndex)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_LTE_WAVEFORM(ftmModeId = %d, waveform = %d, inumRBsPUSCH = %d, inumRBsPUCCH = %d, iPUSCHStartRBIndex = %d)\n", ftmMsg->SequenceID, ftmModeId, waveform, inumRBsPUSCH, inumRBsPUCCH, iPUSCHStartRBIndex);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_LTE_WAVEFORM );
	ftmMsg->Packet.AppendRequest( (byte) waveform);
  ftmMsg->Packet.AppendRequest( (byte) inumRBsPUSCH);
  ftmMsg->Packet.AppendRequest( (byte) inumRBsPUCCH);
  ftmMsg->Packet.AppendRequest( (byte) iPUSCHStartRBIndex);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmStopTxWaveform(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_STOP_LTE_WAVEFORM(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_STOP_LTE_WAVEFORM );
	ftmMsg->Packet.AppendRequest( (byte) 0); // Error in response

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxGainIndex(word halOpId, word ftmModeId, word gainIndex)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_GAIN_INDEX(ftmModeId = %d, gainIndex = %d\n", ftmMsg->SequenceID, ftmModeId, gainIndex);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_GAIN_INDEX );
	ftmMsg->Packet.AppendRequest( (word) gainIndex);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxFrameMatrix(word halOpId, word ftmModeId, byte slotNumber, byte isSlotOn, word powerLevel, byte modulationType)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_FRAME_MATRIX(ftmModeId = %d, slotNumber = %d, isSlotOn = %d, powerLevel = %d, modulationType = %d)\n", ftmMsg->SequenceID, ftmModeId, slotNumber, isSlotOn, powerLevel, modulationType);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_FRAME_MATRIX );	// Command
	ftmMsg->Packet.AppendRequest( (word) 0 );						// Reserved
	ftmMsg->Packet.AppendRequest( (byte) slotNumber );
	ftmMsg->Packet.AppendRequest( (byte) isSlotOn );
	ftmMsg->Packet.AppendRequest( (word) powerLevel );
	ftmMsg->Packet.AppendRequest( (byte) modulationType) ;

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmTxGainSweep(word halOpId, word ftmModeId, word listSize, GSM_GAIN_SWEEP_DATA *configData)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_TX_GAIN_SWEEP(ftmModeId = %d, listSize = %d)\n", ftmMsg->SequenceID, ftmModeId, listSize);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_TX_GAIN_SWEEP	);	// Command

	// Add payload size
	ftmMsg->Packet.AppendRequest( (word) ((word)listSize*5)	);  // Packet Length - 5 bytes/segment
	ftmMsg->Packet.AppendRequest( (word)	0	);	                  // Response packet length, unused, set to 0

	for(word segmentIndex = 0; segmentIndex < listSize; segmentIndex++)
	{
    ftmMsg->Packet.AppendRequest( (word) configData[segmentIndex].segmentLength );
		ftmMsg->Packet.AppendRequest( (byte) configData[segmentIndex].rgi );
		ftmMsg->Packet.AppendRequest( (byte) configData[segmentIndex].gsmMode );
		ftmMsg->Packet.AppendRequest( (byte) configData[segmentIndex].paRange );
	}

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}



//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetGsmLinearPaRange(word halOpId,  word ftmModeId, word slotNumber, word paRange)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_GSM_LINEAR_PA_RANGE(ftmModeId = %d, slotNumber = %d, paRange = %d)\n", ftmMsg->SequenceID, ftmModeId, slotNumber, paRange);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_SET_GSM_LINEAR_PA_RANGE	);	// Command

	ftmMsg->Packet.AppendRequest( (word)	0	);	                  
	ftmMsg->Packet.AppendRequest( (word) slotNumber );
	ftmMsg->Packet.AppendRequest( (word) paRange );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetGsmLinearRgi(word halOpId, word ftmModeId, word slotNumber, word rgi, byte modulation)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_SET_LINEAR_RGI(ftmModeId = %d, slotNumber = %d, rgi = %d, modulation = %d)\n", ftmMsg->SequenceID, ftmModeId, slotNumber, rgi, modulation);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_SET_LINEAR_RGI	);	// Command

	ftmMsg->Packet.AppendRequest( (word)	0	);	          // Reserved           
	ftmMsg->Packet.AppendRequest( (word) slotNumber );
	ftmMsg->Packet.AppendRequest( (word) rgi );
  ftmMsg->Packet.AppendRequest( (byte) modulation );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmTxCfg2AmamSweepV2(word halOpId, word ftmModeId, word preDcDuration, word edgeDuration, word calRgi)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_TX_CFG2_AMAM_SWEEP_V2(ftmModeId = %d, preDcDuration = %d, edgeDuration = %d, calRgi = %d)\n", ftmMsg->SequenceID, ftmModeId, preDcDuration, edgeDuration, calRgi);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_TX_CFG2_AMAM_SWEEP_V2	);	// Command

	// Add payload size
	ftmMsg->Packet.AppendRequest( (word)  8	);  
	ftmMsg->Packet.AppendRequest( (word)  0	);	

  ftmMsg->Packet.AppendRequest( (word) preDcDuration );
  ftmMsg->Packet.AppendRequest( (word) edgeDuration );
  ftmMsg->Packet.AppendRequest( (word) calRgi );

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmEnvDcSweep(word halOpId, word ftmModeId, word listSize, GSM_ENVDC_CS_CAL_DATA* configData)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_TX_ENVDC_CS_SWEEP(ftmModeId = %d, listSize = %d)\n", ftmMsg->SequenceID, ftmModeId, listSize);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

	ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_TX_ENVDC_CS_SWEEP	);	// Command

  // Add payload size
	ftmMsg->Packet.AppendRequest( (word) (listSize*4) );  //Length in bytes of the command data = Number of steps * 4 = number of steps *(2 bytes for step duration, 2 bytes for Envelope DC value )
	ftmMsg->Packet.AppendRequest( (word)	0	);					  // Response packet length, unused, set to 0

	for( word step = 0; step < listSize; step++ )
	{
		ftmMsg->Packet.AppendRequest( (word) configData[step].stepDuration );
		ftmMsg->Packet.AppendRequest( (word) configData[step].envelopeDc );
	}

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetCalibrationState(word halOpId, byte state)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_CALIBRATION_STATE(ftmModeId = %d, state = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, state);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_CALIBRATION_STATE);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 12);
	ftmMsg->Packet.AppendRequest( (byte) state );
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmRxWaitDbm(word halOpId, word rfTechnology, int16 triggerLevel, dword timeout)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_RX_WAIT_DBM(ftmModeId = %d, rfTechnology = %d, triggerLevel = %d, timeout = %lu)\n", ftmMsg->SequenceID, FTM_COMMON_C, rfTechnology, triggerLevel, timeout);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_RX_WAIT_DBM);
  ftmMsg->Packet.AppendRequest( (word) 8);
  ftmMsg->Packet.AppendRequest( (word) 16);
	ftmMsg->Packet.AppendRequest( (word) rfTechnology);
  ftmMsg->Packet.AppendRequest( (word) triggerLevel);
  ftmMsg->Packet.AppendRequest( (byte*)(&timeout), sizeof(dword));

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmRxWaitDbmV2(word halOpId, word rfTechnology, uint16 deviceId, uint16 signalDirection, int16 triggerLevel, dword timeout)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_RX_WAIT_DBM_V2(ftmModeId = %d, rfTechnology = %d, deviceId = %d, signalDirection = %d, triggerLevel = %d, timeout = %lu)\n", ftmMsg->SequenceID, FTM_COMMON_C, rfTechnology, deviceId, signalDirection, triggerLevel, timeout);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_RX_WAIT_DBM_V2);
  ftmMsg->Packet.AppendRequest( (word) 12);
  ftmMsg->Packet.AppendRequest( (word) 14);
	ftmMsg->Packet.AppendRequest( (word) rfTechnology);
  ftmMsg->Packet.AppendRequest( (word) deviceId);
  ftmMsg->Packet.AppendRequest( (word) signalDirection);
  ftmMsg->Packet.AppendRequest( (word) triggerLevel);
  ftmMsg->Packet.AppendRequest( (byte*)(&timeout), sizeof(dword));

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmSetupRxBurstForExpectedPower(word halOpId, word ftmModeId, int16 expectedLevel, word averages)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR(ftmModeId = %d, expectedLevel = %d, averages = %u)\n", ftmMsg->SequenceID, ftmModeId, expectedLevel, averages);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR );
  ftmMsg->Packet.AppendRequest( (word) 4);
  ftmMsg->Packet.AppendRequest( (word) 16);
  ftmMsg->Packet.AppendRequest( (word) expectedLevel);
  ftmMsg->Packet.AppendRequest( (word) averages);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetTxPowerDbm(word halOpId, word ftmModeId, boolean enable, word powerLevel)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_TX_POWER_DBM(ftmModeId = %d, enable = %s, powerLevel = %d)\n", ftmMsg->SequenceID, ftmModeId, (TRUE==enable)?"TRUE":"FALSE", (signed short)powerLevel);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_TX_POWER_DBM );
  ftmMsg->Packet.AppendRequest( (byte) ((TRUE==enable)?1:0));
  ftmMsg->Packet.AppendRequest( (word) powerLevel);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmLteSetNetSigValue(word halOpId, word ftmModeId, byte signalValue)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_LTE_SET_NET_SIG_VALUE(ftmModeId = %d, signalValue = %d)\n", ftmMsg->SequenceID, ftmModeId, signalValue);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_LTE_SET_NET_SIG_VALUE );
  ftmMsg->Packet.AppendRequest( (word) signalValue);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmLteSetTddParams(word halOpId, word ftmModeId, byte subframeAssignment, byte continuousTx)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_LTE_SET_TDD_PARAMS(ftmModeId = %d, subframeAssignment = %d, continuousTx = %d)\n", ftmMsg->SequenceID, ftmModeId, subframeAssignment, continuousTx);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_LTE_SET_TDD_PARAMS );
  ftmMsg->Packet.AppendRequest( (byte) subframeAssignment);
  ftmMsg->Packet.AppendRequest( (byte) continuousTx);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmGetMeasPwrPrxDrx(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_GSM_GET_MEAS_PWR_PRX_DRX(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_GSM_GET_MEAS_PWR_PRX_DRX );
  ftmMsg->Packet.AppendRequest( (word) 0); // Req data len
  ftmMsg->Packet.AppendRequest( (word) 0); // Resp len
  ftmMsg->Packet.AppendRequest( (word) 0); // Measured dBm PRx
  dword zero = 0;
  ftmMsg->Packet.AppendRequest((byte*)&zero, sizeof(dword)); // Raw RSSI PRx
  ftmMsg->Packet.AppendRequest( (word) 0); // Measured dBm DRx
  ftmMsg->Packet.AppendRequest((byte*)&zero, sizeof(dword)); // Raw RSSI DRx
  ftmMsg->Packet.AppendRequest( (word) 0); // Error code
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;  
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetRxLevelDbm(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_RX_LEVEL_DBM(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_RX_LEVEL_DBM );
  //ftmMsg->Packet.AppendRequest( (word) 8); // Req data len
  //ftmMsg->Packet.AppendRequest( (word) 18); // Resp len
  ftmMsg->Packet.AppendRequest( (word) 0); // RX AGC
  ftmMsg->Packet.AppendRequest( (word) 0); // Measured power
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
kv_v2_parameters currentKvParameters={
  4,  //.numberOfBands
  10, //.numberOfTxCycles
  12, //.numberOfMeasurements
  {
    PHONE_MODE_GSM_850, //18–GSM850
    PHONE_MODE_GSM_900, //10–GSM900
    PHONE_MODE_GSM_1800, //11–GSM1800
    PHONE_MODE_GSM_1900  //12–GSM1900
  }, //.gsmRfModeList
  KVCO_DEFAULT_CHANNEL_TABLE //.channelList
};

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmTxKvCalV2(word halOpId, word ftmModeId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_TX_KV_CAL_V2(ftmModeId = %d, numberOfBands = %d, numberOfTxCycles = %d, numberOfMeasurements = %d )\n", ftmMsg->SequenceID, ftmModeId, currentKvParameters.numberOfBands, currentKvParameters.numberOfTxCycles, currentKvParameters.numberOfMeasurements );

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_TX_KV_CAL_V2 );
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (byte*)(&currentKvParameters), sizeof(currentKvParameters) ); 
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
ftm_xo_cal_parameter_type currentXoParameters = {
            1, //.technology; 0 – 1X, 1 – WCDMA, 2 – GSM//
            0, //.rx_freq_khz;
       200000, //.offset;
         1024, //.temp_span;
            5, //.min_ft_samples;
          100, //.maxFmax_ft_samplestSample;
          200, //.substage_time;
        30000, //.timeout_time;
            3, //.update_nv;
            0, //.wb_id;
        30000, //.temp_grad_timeout;
            1, //.xtal
};

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmXoCal(word halOpId, uint16 calAction, byte version)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_XO_CALIBRATION(ftmModeId = %d, calAction = %d, version = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, calAction, version);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_XO_CALIBRATION );
  ftmMsg->Packet.AppendRequest( (word) sizeof(currentXoParameters)); // Length of req
  ftmMsg->Packet.AppendRequest( (word) 0);                           // Length of resp
  ftmMsg->Packet.AppendRequest( (word) calAction);                   // Cal command
  ftmMsg->Packet.AppendRequest( (byte) version);                     // Version
  ftmMsg->Packet.AppendRequest( (byte*)(&currentXoParameters), sizeof(currentXoParameters) ); 
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetEnhTherm(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_ENH_THERM(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GET_ENH_THERM );
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 16); // Reserved  
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmDoSelfCal(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_DO_SELF_CAL(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_DO_SELF_CAL );
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved  

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmDoTunerSelfCal(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_DO_TUNER_SELF_CAL(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_DO_TUNER_SELF_CAL );
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved  

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetPowerMode(word halOpId, word ftmModeId, byte powerMode)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_RF_POWER_MODE(ftmModeId = %d, powerMode = %d)\n", ftmMsg->SequenceID, ftmModeId, powerMode);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_RF_POWER_MODE );
  ftmMsg->Packet.AppendRequest( (byte) powerMode); 

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}



//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmCfgAndExecCalSweep(word halOpId, byte startCalSweep, byte lastSweepFlag, byte enableF3msg, word numberOfSegments, FTM_Cal_Sweep_Opcode_Req_List_Item *sweepList)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_CFG_AND_EXEC_CAL_SWEEP(ftmModeId = %d, startCalSweep = %d, lastSweepFlag = %d, enableF3msg = %d, numberOfSegments = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, startCalSweep, lastSweepFlag, enableF3msg, numberOfSegments );

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_CALIBRATION_SWEEP /* _FTM_CFG_AND_EXEC_CAL_SWEEP */);
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved  
  ftmMsg->Packet.AppendRequest( (byte) startCalSweep); 
  ftmMsg->Packet.AppendRequest( (byte) lastSweepFlag);  
  ftmMsg->Packet.AppendRequest( (byte) enableF3msg); 
  ftmMsg->Packet.AppendRequest( (word) numberOfSegments); 
  ftmMsg->Packet.AppendRequest( (word) 0); // Place holder of payload size
  ftmMsg->Packet.AppendRequest( (word) 0); // Place holder of result size
  
  uint16 sizeOfSweepPayload = 0;
  uint16 numberOfResults = 0;
  uint16 numberOfAdditionalResults = 0;
  uint16 lastWordToCheckOpcodes = FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16 - 1 - (FTM_CAL_OPCODE_MAX/16);
  FTM_Cal_Sweep_Opcode_Req_List_Item *currentSweepItem = sweepList;
  HalLogWrite("%8s%8s   %s\n", "LEN", "OPCODE", "SWEEP CONFIG");
  while (NULL != currentSweepItem)
  {    
    uint16 sizeOfItemPayload = sizeof(FTM_Cal_Sweep_Opcode_Header);
    byte *currentOpCodeSegment = (byte*)&(currentSweepItem->opCodeHeader)+sizeof(FTM_Cal_Sweep_Opcode_Header);
    bool foundOpCodes = false;
    uint16 effectiveOpcodeBit = 0;
    const int msgBufferMaxSize = 512;
    char msgBuffer[msgBufferMaxSize] = "";
    char msgBufferTemp[msgBufferMaxSize] = "";
    snprintf(msgBufferTemp, msgBufferMaxSize, "%8u", currentSweepItem->opCodeHeader.segLen);
    strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
    for(uint16 wordIndex=FTM_CAL_SWEEP_OPCODE_SIZE_AS_UINT16-1; wordIndex>=lastWordToCheckOpcodes; wordIndex--)
    {
      if(0 == currentSweepItem->opCodeHeader.opcode[wordIndex])
      {
        continue;
      }
      snprintf(msgBufferTemp, msgBufferMaxSize, "%8d", currentSweepItem->opCodeHeader.opcode[wordIndex]);
      strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
      for(uint16 opCodeBit=0; (15 >= opCodeBit) && (FTM_CAL_OPCODE_MAX > effectiveOpcodeBit); opCodeBit++, effectiveOpcodeBit++)
      {
        //HalLogWrite("wordIndex = %u, opCodeBit = %u, effectiveOpcodeBit = %u, currentSweepItem->opCodeHeader.opcode[wordIndex] = %u, (1<<opCodeBit) = %u", wordIndex, opCodeBit, effectiveOpcodeBit, currentSweepItem->opCodeHeader.opcode[wordIndex], (1<<opCodeBit));
        if(0 == (currentSweepItem->opCodeHeader.opcode[wordIndex] & (1<<opCodeBit)))
        {
          continue;
        }
        switch(effectiveOpcodeBit) 
        {
        case FTM_CAL_OPCODE_CONFIG_RADIO:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_Cfg_Req);
            FTM_Cal_Sweep_Cfg_Req *currSeg = (FTM_Cal_Sweep_Cfg_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_CONFIG_RADIO( MASTER_DEVICE_MASK = %d, SLAVE_DEVICE_ID = 0x%08lX, RF MODE = %d, CHANNEL = %d, TX_BANDWIDTH = %d, RX_BANDWIDTH = %d, DC_CHAN_OFFSET = %d ),", currSeg->masterDev, currSeg->slaveDev, currSeg->rfmode, currSeg->ulChan, currSeg->txBW, currSeg->rxBW, currSeg->chanOffset);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_Cfg_Req);
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_RETUNE_RADIO:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_Retune_Req);
            FTM_Cal_Sweep_Retune_Req *currSeg = (FTM_Cal_Sweep_Retune_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_RETUNE_RADIO( MASTER_DEVICE_MASK = %d, CHANNEL = %d, DC_CHAN_OFFSET = %d ),", currSeg->masterDev, currSeg->ulChan, currSeg->chanOffset);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_Retune_Req);
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_CONFIG_TX_WF:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_Tx_Wfm_Cfg_Req); 
            FTM_Cal_Sweep_Tx_Wfm_Cfg_Req *currSeg = (FTM_Cal_Sweep_Tx_Wfm_Cfg_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_CONFIG_TX_WF( NUM_RBS = %d, START_RB = %d, TX_WAVEFORM = %d ),", currSeg->numRBs, currSeg->startRB, currSeg->txWaveform);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);            
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_Tx_Wfm_Cfg_Req);
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_CONFIG_TX:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_TxMeas_Req); 
            FTM_Cal_Sweep_TxMeas_Req *currSeg = (FTM_Cal_Sweep_TxMeas_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_CONFIG_TX( TX_ACTION = %d, PA_STATE = %d, RGI = %3d, PA_BIAS = %3lu, TX_FLAG = %d ),", currSeg->txAction, currSeg->paState, currSeg->rgi, currSeg->paBias, currSeg->txFlag);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);            
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_TxMeas_Req);
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_MEAS_HDET:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_HDET_Meas_Req);
            FTM_Cal_Sweep_HDET_Meas_Req *currSeg = (FTM_Cal_Sweep_HDET_Meas_Req*)currentOpCodeSegment;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_MEAS_HDET( HDET_AVG = %d, LPM_HDET_AVG = %d, HDET_FLAG = %d ),", currSeg->numHdetAvg, currSeg->numLpmHdetAvg, currSeg->hdetFlag);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);            
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_HDET_Meas_Req);
            foundOpCodes = true;
            if( (0 != currSeg->numHdetAvg) || (0 != currSeg->numLpmHdetAvg) )
            {
              numberOfResults++;
            }
            if( (0 != currSeg->numHdetAvg) && (0 != currSeg->numLpmHdetAvg) )
            {
              numberOfAdditionalResults++;
            }
          }
          break;          
        case FTM_CAL_OPCODE_CONFIG_RX:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_RxCfg_Req);
            FTM_Cal_Sweep_RxCfg_Req *currSeg = (FTM_Cal_Sweep_RxCfg_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_CONFIG_RX( RX_CAL_ACTION = %d, RX_PWR_MODE = %d, RX_FLAG = %d ),", currSeg->rxActions, currSeg->rxPM, currSeg->rxFlag);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);            
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_RxCfg_Req);
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_MEAS_RX:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_RxMeas_Req);
            FTM_Cal_Sweep_RxMeas_Req *currSeg = (FTM_Cal_Sweep_RxMeas_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_MASK_MEAS_RX( RX_CAL_ACTION = %d, RX_PWR_MODE = %d, RX_FLAG = %d, EXP_RXAGC = %d ),", currSeg->rxActions, currSeg->rxPM, currSeg->rxFlag, (signed short)currSeg->expRxAGC);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_RxMeas_Req);
            numberOfResults++;
            foundOpCodes = true;
          }
          break;
        case FTM_CAL_OPCODE_POLLING:
          {
            sizeOfItemPayload += sizeof(FTM_Cal_Sweep_Rx_Wait_Req);
            FTM_Cal_Sweep_Rx_Wait_Req *currSeg = (FTM_Cal_Sweep_Rx_Wait_Req*)currentOpCodeSegment;
            currSeg = currSeg;
            snprintf(msgBufferTemp, msgBufferMaxSize, " FTM_CAL_OPCODE_POLLING_MASK( DEVICE = %d, TIMEOUT = %lu, THRESHOLD = %d, DIRECTION = %d),", 
                        currSeg->primary_device, (uint32)(currSeg->timeout_us), (int16)(currSeg->threshold), currSeg->direction);
            strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);
            currentOpCodeSegment += sizeof(FTM_Cal_Sweep_Rx_Wait_Req);
            foundOpCodes = true;
          }
          break;
        default:
          {
            MSG_ERROR(" ***** USUPPORTED CAL SWEEP OPCODE BIT %d *****", effectiveOpcodeBit, 0, 0);
            RFCA_Free(ftmMsg);
            ftmMsg = NULL;
            return NULL; 
          }
          break;
        }
      }
    }
    if(false == foundOpCodes)
    {
      snprintf(msgBufferTemp, msgBufferMaxSize, "%8d%s", 0, " FTM_CAL_OPCODE_NO_OP");
      strlcat(msgBuffer, msgBufferTemp, msgBufferMaxSize);      
    }
    ftmMsg->Packet.AppendRequest( (byte*)(&currentSweepItem->opCodeHeader), sizeOfItemPayload); 
    sizeOfSweepPayload += sizeOfItemPayload;
    currentSweepItem = currentSweepItem->next;
    HalLogWrite("%s", msgBuffer);
  }

  ftmMsg->Packet.AppendRequest( (word) 0xDAAD); // Check bit

  // Now we still need to fill the payload size
  const uint16 payloadSizeFieldOffset = 15;
  byte* rawReqData = ftmMsg->Packet.GetRequestDataPtr();
  memscpy(&rawReqData[payloadSizeFieldOffset], sizeof(uint16), 
          &sizeOfSweepPayload, sizeof(uint16));  

  // Now we still need to fill the result size
  // Results are 2 bytes each but result header is 3 bytes for each segment
  numberOfResults = (numberOfResults * 5) + (numberOfAdditionalResults * 2); 
  const uint16 resultSizeFieldOffset = 17;
  memscpy(&rawReqData[resultSizeFieldOffset], sizeof(uint16), &numberOfResults, sizeof(uint16));  

  AddToFtmIdQueue(halOpId, ftmMsg);

  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmCfgAndExecCalSweepFlex(word halOpId, bool compressedPayload, byte startCalSweep, byte lastSweepFlag, byte enableF3msg, HAL_RF_SWEEP_SEGMENT_CONFIG_DATA *sweepData)
{  
  if(sweepData->segmentDataSize > RFCA_PACKET_HELPER_MAX_PACKET_SIZE - 20 )
  {
    MSG_ERROR("Flex sweep payload of %d bytes, is too big, max size is %d", sweepData->segmentDataSize, RFCA_PACKET_HELPER_MAX_PACKET_SIZE - 20, 0);
    return NULL;
  }

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):%s(ftmModeId = %d, startCalSweep = %d, lastSweepFlag = %d, enableF3msg = %d, numberOfSegments = %u)\n", ftmMsg->SequenceID, 
              (compressedPayload ? "FTM_CALIBRATION_SWEEP_COMPRESSED" : "FTM_CALIBRATION_SWEEP"),
              FTM_COMMON_C, startCalSweep, lastSweepFlag, enableF3msg, sweepData->numberOfSegments);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) (compressedPayload ? FTM_CALIBRATION_SWEEP_COMPRESSED : FTM_CALIBRATION_SWEEP));
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (byte) startCalSweep);
  ftmMsg->Packet.AppendRequest( (byte) lastSweepFlag);
  ftmMsg->Packet.AppendRequest( (byte) enableF3msg);
  ftmMsg->Packet.AppendRequest( (word) sweepData->numberOfSegments);
  ftmMsg->Packet.AppendRequest( (word) sweepData->segmentDataSize);
  ftmMsg->Packet.AppendRequest( (word) sweepData->resultSize);
  HalLogWrite("Copying %d bytes of %d segments to FTM sweep request", sweepData->segmentDataSize, sweepData->numberOfSegments);
  ftmMsg->Packet.AppendRequest( (byte*) (sweepData->segmentData), sweepData->segmentDataSize );
  ftmMsg->Packet.AppendRequest((word)0xDAAD); // Check bits

  AddToFtmIdQueue(halOpId, ftmMsg); 
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGetCalSweepResults(word halOpId)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GET_CAL_SWEEP_RESULTS(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_CALIBRATION_SWEEP_GET_RESULTS /*_FTM_GET_CAL_SWEEP_RESULTS*/ );
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Reserved  

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * AddFtmRawMsg(word halOpId, word size, byte* rawMsg)
{  
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  
  const uint16 bufferSize = 1024;
  char tmpBuffer[bufferSize] = {0};
  for(int i = 0; i<(bufferSize/3) && i<size; i++)
  {
    const uint16 hexCharSize = 4;
    char tmpBuffer2[hexCharSize] = {0};
    snprintf(tmpBuffer2, hexCharSize, "%02X ", rawMsg[i]);
    strlcat(tmpBuffer, tmpBuffer2, bufferSize);
  }
  
  HalLogWrite("F(%05d):_FTM_Send_Raw(size = %d, data = %s)\n", ftmMsg->SequenceID, size, tmpBuffer);
  	      
  ftmMsg->Packet.AppendRequest( rawMsg, size );  
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmTdsCdmaBurstTx(word halOpId, word ftmModeId, byte state)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_TDSCDMA_BURST_TX(ftmModeId = %d, state = %d)\n", ftmMsg->SequenceID, ftmModeId, state);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_TDSCDMA_BURST_TX  );
  ftmMsg->Packet.AppendRequest( (byte) state); 
  ftmMsg->Packet.AppendRequest( (word) 0); // Error code in resp

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmTdsCdmaSetBurstTxParams(word halOpId, word ftmModeId, byte numberOfSlots, byte modulation, word powerLevels[5])
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_TDSCDMA_SET_BURST_TX_PARAMS(ftmModeId = %d, numberOfSlots = %d, modulation = %d, poweLevels = { %6d, %6d, %6d, %6d, %6d} )\n", ftmMsg->SequenceID, ftmModeId, numberOfSlots, modulation, (signed short)powerLevels[0], (signed short)powerLevels[1], (signed short)powerLevels[2], (signed short)powerLevels[3], (signed short)powerLevels[4] );

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_TDSCDMA_SET_BURST_TX_PARAMS  );
  ftmMsg->Packet.AppendRequest( (byte) numberOfSlots); 
  ftmMsg->Packet.AppendRequest( (byte) modulation);
  ftmMsg->Packet.AppendRequest( (word) powerLevels[0]);
  ftmMsg->Packet.AppendRequest( (word) powerLevels[1]);
  ftmMsg->Packet.AppendRequest( (word) powerLevels[2]);
  ftmMsg->Packet.AppendRequest( (word) powerLevels[3]);
  ftmMsg->Packet.AppendRequest( (word) powerLevels[4]);
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0);// Error code in resp

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmTdsCdmaBurstRx(word halOpId, word ftmModeId, word expectedLevel, byte state)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_TDSCDMA_BURST_RX(ftmModeId = %d, expectedLevel = %d, state = %d)\n", ftmMsg->SequenceID, ftmModeId, (signed short)expectedLevel, state);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_TDSCDMA_BURST_RX  );
  ftmMsg->Packet.AppendRequest( (word) expectedLevel); 
  ftmMsg->Packet.AppendRequest( (byte) state); 
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0);// Reserved
  ftmMsg->Packet.AppendRequest( (word) 0); // Error code in resp

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmTdsCdmaIqLogging(word halOpId, word ftmModeId, word port)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_TDSCDMA_IQ_LOGGING (ftmModeId = %d, port = %d)\n", ftmMsg->SequenceID, ftmModeId, port);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_TDSCDMA_IQ_LOGGING );
  ftmMsg->Packet.AppendRequest( (word) port); 
  ftmMsg->Packet.AppendRequest( (word) 0); // Error code in resp

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetFreqAdjust(word halOpId, word ftmModeId, dword userFreqAdjustPpb, dword centerFreqHz, byte enableXoFreqAdjust)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_SET_FREQ_ADJUST(ftmModeId = %d, userFreqAdjustPpb = %ld, centerFreqHz = %lu, enableXoFreqAdjust = %d)\n", ftmMsg->SequenceID, ftmModeId, userFreqAdjustPpb, centerFreqHz, enableXoFreqAdjust);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_SET_FREQ_ADJUST  );
  ftmMsg->Packet.AppendRequest( (byte*)&userFreqAdjustPpb, sizeof(dword) ); 
  ftmMsg->Packet.AppendRequest( (byte*)&centerFreqHz, sizeof(dword) ); 
  ftmMsg->Packet.AppendRequest( (byte) enableXoFreqAdjust);
  ftmMsg->Packet.AppendRequest( (word) 0); // totalFreqAdjustHz (LSW) in resp
  ftmMsg->Packet.AppendRequest( (word) 0); // totalFreqAdjustHz (MSW) in resp
  ftmMsg->Packet.AppendRequest( (word) 0); // ftmErrorCode in resp

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmLteSetLnaStateDbm(word halOpId, word ftmModeId, int16 expectedRxPowerDbm)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_LTE_SET_LNA_STATE_DBM(ftmModeId = %d, expectedRxPowerDbm = %d)\n", ftmMsg->SequenceID, ftmModeId, expectedRxPowerDbm);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) _FTM_LTE_SET_LNA_STATE_DBM );
  ftmMsg->Packet.AppendRequest( (word) expectedRxPowerDbm);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmFrameSweepExec(word halOpId)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_FRAME_SWEEP_EXEC_CMD(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_GSM_EXTENDED_C0_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_GSM_EXTENDED_C0_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_FRAME_SWEEP_EXEC_CMD );
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (byte) 0);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmFrameSweepConfig(word halOpId, ftm_gsm_frame_sweep_config_type* masterSweepConfig)
{
  uint16 totalBytesToSend = masterSweepConfig->used_bytes_count;
  uint16 nextChunkOffset = 0;
  const uint16 MaxSweepPayloadSize = 3400;

  FtmPacketQueueItem *ftmMsg = NULL;
  while(0 != totalBytesToSend )
  {    
    ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

    HalLogWrite("F(%05d):_FTM_GSM_FRAME_SWEEP_CFG_CMD(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_GSM_EXTENDED_C0_C);

    FormatFTMHeader(&ftmMsg->Packet, FTM_GSM_EXTENDED_C0_C);

    ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_FRAME_SWEEP_CFG_CMD );
  
    uint16 bytesToSendNow = ((totalBytesToSend>MaxSweepPayloadSize)?MaxSweepPayloadSize:totalBytesToSend);

    ftmMsg->Packet.AppendRequest( (word) bytesToSendNow);
    ftmMsg->Packet.AppendRequest( (word) 0);
    
    ftmMsg->Packet.AppendRequest( &(((byte*)masterSweepConfig->buffer)[nextChunkOffset]), bytesToSendNow ); 
    
    AddToFtmIdQueue(halOpId, ftmMsg);
    
    nextChunkOffset+=bytesToSendNow;
    totalBytesToSend-=bytesToSendNow;
  }
  return ftmMsg;
}


//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmFrameSweepGetResults(word halOpId)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):_FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD(ftmModeId = %d)\n", ftmMsg->SequenceID, FTM_GSM_EXTENDED_C0_C);

  FormatFTMHeader(&ftmMsg->Packet, FTM_GSM_EXTENDED_C0_C);

  ftmMsg->Packet.AppendRequest( (word) _FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD );
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (byte) 0);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmIqCaptureGetConfig(word halOpId, word ftmModeId)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_IQ_CAPTURE_GET_CONFIG(ftmModeId = %d)\n", ftmMsg->SequenceID, ftmModeId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_IQ_CAPTURE_GET_CONFIG );
  ftmMsg->Packet.AppendRequest( (word) 0);  // error
  ftmMsg->Packet.AppendRequest( (word) 0);  // max samples
  ftmMsg->Packet.AppendRequest( (byte) 0);  // data format
  dword zero = 0;
  ftmMsg->Packet.AppendRequest((byte*)&zero, sizeof(dword)); // sampling freq
  ftmMsg->Packet.AppendRequest((byte*)&zero, sizeof(dword)); // max sample size
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmIqCaptureAquireSamples(word halOpId, word ftmModeId, dword sampleSize, byte carrierId)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_IQ_CAPTURE_ACQUIRE_SAMPLES(ftmModeId = %d, sampleSize = %lu, carrierId = %d)\n", ftmMsg->SequenceID, ftmModeId, sampleSize, carrierId);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_IQ_CAPTURE_ACQUIRE_SAMPLES );
  ftmMsg->Packet.AppendRequest( (word) 0);  // error  
  ftmMsg->Packet.AppendRequest( (byte*)&sampleSize, sizeof(dword)); // sample size
  ftmMsg->Packet.AppendRequest( (byte) carrierId);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmIqCaptureGetSamples(word halOpId, word ftmModeId, dword sampleOffset, dword sampleSize)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_IQ_CAPTURE_GET_SAMPLES(ftmModeId = %d, sampleOffset = %lu, sampleSize = %lu)\n", ftmMsg->SequenceID, ftmModeId, sampleOffset, sampleSize);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_IQ_CAPTURE_GET_SAMPLES );
  ftmMsg->Packet.AppendRequest( (word) 0);  // error  
  ftmMsg->Packet.AppendRequest( (byte*)&sampleOffset, sizeof(dword)); // sample offset
  ftmMsg->Packet.AppendRequest( (byte*)&sampleSize, sizeof(dword)); // sample size
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmGsmCaptureIq(word halOpId, word ftmModeId, word captureMode)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_GSM_CAPTURE_IQ(ftmModeId = %d, captureMode = %lu)\n", ftmMsg->SequenceID, ftmModeId, captureMode);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_GSM_CAPTURE_IQ );
  ftmMsg->Packet.AppendRequest( (word) captureMode); 
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetAntenna(word halOpId, uint8 device, uint8 position, uint8 actionImmediate)
{

  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }
  uint16 ftmModeId = FTM_RFM_C;
  HalLogWrite("F(%05d):FTM_COMMON_SET_ANTENNA(ftmModeId = %d, device = %d, position = %d, actionImmediate = %d)\n", ftmMsg->SequenceID, ftmModeId, device, position, actionImmediate);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_COMMON_SET_ANTENNA );
  ftmMsg->Packet.AppendRequest( (word) 13);  // req size
  ftmMsg->Packet.AppendRequest( (word) 0);   // resp size
  ftmMsg->Packet.AppendRequest( (byte) device);
  ftmMsg->Packet.AppendRequest( (byte) position);
  ftmMsg->Packet.AppendRequest( (byte) actionImmediate);
  
  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmSetFilteredPath(word halOpId, word filteredPatatState)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_SET_FILTERED_PATH(ftmModeId = %d, filteredPatatState = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, filteredPatatState);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) FTM_SET_FILTERED_PATH );
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) filteredPatatState);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

FtmPacketQueueItem * AddFtmSetPathOverrideIndex(word halOpId, word ftmModeId, byte carrierId, byte band, byte txPathSelect, byte prxPathSelect, byte drxPathSelect)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_SET_PATH_SEL_OVERRIDE_INDEX(ftmModeId = %d, carrierId = %d, band = %d, txPathSelect = %d, prxPathSelect = %d, drxPathSelect = %d)\n", ftmMsg->SequenceID, ftmModeId, carrierId, band, txPathSelect, prxPathSelect, drxPathSelect);

  FormatFTMHeader(&ftmMsg->Packet, ftmModeId);

  ftmMsg->Packet.AppendRequest( (word) FTM_SET_PATH_SEL_OVERRIDE_INDEX );
  ftmMsg->Packet.AppendRequest( (byte) carrierId);
  ftmMsg->Packet.AppendRequest( (byte) band);
  ftmMsg->Packet.AppendRequest( (byte) txPathSelect);
  ftmMsg->Packet.AppendRequest( (byte) prxPathSelect);
  ftmMsg->Packet.AppendRequest( (byte) drxPathSelect);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

//------------------------------------------------------------------------------------
FtmPacketQueueItem * AddFtmDoRsbTxloCal(word halOpId, byte rfMode, byte device, byte divDevice, byte exitRequired)
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_DO_RSB_TXLO_CAL(ftmModeId = %d, rfMode = %d, device = %d, divDevice = %d, exitRequired = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, rfMode, device, divDevice, exitRequired);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) FTM_DO_RSB_TXLO_CAL );
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (byte) rfMode);
  ftmMsg->Packet.AppendRequest( (byte) 0);
  ftmMsg->Packet.AppendRequest( (byte) device);
  ftmMsg->Packet.AppendRequest( (byte) divDevice);
  ftmMsg->Packet.AppendRequest( (byte) exitRequired);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}


FtmPacketQueueItem * AddFtmDoRffeCmd(word halOpId, byte extMode, byte readWrite, byte channel, byte slave, word address, byte speed )
{
  FtmPacketQueueItem *ftmMsg = NewFtmPacketQueueItem();
  if(NULL == ftmMsg)
  {
    MSG_ERROR("Failed to create new FTM diag response. FTM command will not be added to sequence.", 0, 0, 0);
    return NULL;
  }

  HalLogWrite("F(%05d):FTM_DO_RFFE_CMD(ftmModeId = %d, extMode = %d, readWrite = %d, channel = %d, slave = %d, address = %u, speed = %d)\n", ftmMsg->SequenceID, FTM_COMMON_C, extMode, readWrite, channel, slave, address, speed);

  FormatFTMHeader(&ftmMsg->Packet, FTM_COMMON_C);

  ftmMsg->Packet.AppendRequest( (word) FTM_DO_RFFE_CMD ); 
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0);
  ftmMsg->Packet.AppendRequest( (word) 0 ); // RFFE sub command ID
  ftmMsg->Packet.AppendRequest( (byte) extMode);
  ftmMsg->Packet.AppendRequest( (byte) readWrite);
  ftmMsg->Packet.AppendRequest( (byte) channel);
  ftmMsg->Packet.AppendRequest( (byte) slave);
  ftmMsg->Packet.AppendRequest( (word) address);
  ftmMsg->Packet.AppendRequest( (byte) 0);    // data
  ftmMsg->Packet.AppendRequest( (byte) speed);

  AddToFtmIdQueue(halOpId, ftmMsg);
  return ftmMsg;
}

