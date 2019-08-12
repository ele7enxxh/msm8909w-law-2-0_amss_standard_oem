/*!
  @file
  rfca_helper.c

  @brief
  This module contains Control code for RFCA.
*/

/*==============================================================================

  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_helper.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/09/15   aki     Reduce log message buffer size
03/04/15   am      RFCA: Lte B30 Tx frequency incorrectly calculated for FREQ_ADJUST 
09/18/14   aki     Silenced compiler warning comparing int to enum
01/30/14   aki     Fix KW error NPD.FUNC.MIGHT and CWARN.NOEFFECT.UCMP.GE
01/23/14   aki     RFCA debug mode as run-time option and move hex_dump helper here
01/07/14   svi     LTE B27 Support for BOLT - Updated
01/03/14   svi     LTE B27 Support for BOLT
01/03/14   aki     Removed RFCA_ASSERT 
11/19/13   aki     Update LTE_CalculateChannel with new bands 
11/14/13   aki     Added LTE bands 22-32 
11/14/13   aki     GSM_CalculateFrequency, update config parsing
08/12/13   aki     Fixed KW errors differently
08/12/13   sar     Fixed KW errors.
08/09/13   aki     Added pre exec for GSM IQ config item
08/09/13   aki     Added IQ config items
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA  
06/07/13   aki     Removed RFCA_WriteRFNV as it is not used
05/02/13   aki     Update LTE channels to HAL_RF_BAND_LTE_44, added new function 
                   names to the look up table 
04/30/13   aki     Handle empty config string
04/29/13   aki     Added LTE pre-exec configs for RX WAIT and RSSI 
03/25/13   aki     Refactored ParseConfigData() out of RFCA_RefreshConfigurationData()
02/08/13   aki     Removed TX DBM config and tune segments
02/01/13   aki     Updated state cache band and channel
01/30/13   aki     Removed RX ALIGN timing config
01/16/13   aki     Corrected copy paste error in GSM config items 
12/27/12   aki     Added support for GSM timing config
10/31/12   aki     Added support to read Timing config data from file
10/02/12   aki     Remove RFCA_QMSL ifdefs 
07/13/12   aki     TDSCDMA support, HAL function names to id map
05/25/12   aki     Added LTE_CalculateChannel, log buffer size increase
04/24/12   aki     Added channel and frequency calculation functions
02/24/12   aki     Use RFCA_FUNCTION_SUCCESS/FAIL intead of 0/1, use HalLogWrite
02/17/12   aki     Added rfcommon_nv.h
02/08/12   aki     Response buffer allocation, MSG cleanup
10/28/11   aki     Initial revision 

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca_helper.h"
#include "rfca_ftm_package_helper.h"
#include "rfca_to_ftm_adapter.h"
#include "rfca_defs.h"
#include "rfca_state_cache.h"
#include "rfca_handler.h"

#include "ftm.h"
#include "msg.h"
#include "rfcommon_nv.h"

/*----------------------------------------------------------------------------*/
int RFCA_SendFtmPackagesToSequencer(PendingRfcaResponse *rfcaResponseItem)
{
  const int ftmSeqQueueItemsResponseSize = 12;
  FtmPacketQueueItem * nextFtmItem = rfcaResponseItem->FTM.FtmItems;

  while (NULL != nextFtmItem)
  {
    RfcaPacketHelper addCommandsToQueuePacket;
    if(NULL == addCommandsToQueuePacket.GetRequestDataPtr())
    {
      MSG_ERROR("No memory for FTM request, will not be able to execute command", 0 ,0 ,0);
      return RFCA_FUNCTION_FAIL;
    }
    if(false == addCommandsToQueuePacket.AllocateResponse(ftmSeqQueueItemsResponseSize))
    {
      HalLogWrite("Failed to allocate memory for response.\n");
      addCommandsToQueuePacket.FreeResponse();
      return RFCA_FUNCTION_FAIL;
    }
  
    *(addCommandsToQueuePacket.GetResponseLengthPtr()) = ftmSeqQueueItemsResponseSize;
    FtmPacketQueueItem * prevFtmItem = nextFtmItem;
    FormatFTMSequencerPackage2(&addCommandsToQueuePacket, &nextFtmItem);
    if(prevFtmItem == nextFtmItem)
    {
      HalLogWrite("Failed to send FTM packages to FTM sequencer, send buffer possibly too small.\n");
      return RFCA_FUNCTION_FAIL;
    }
    // Send and Check FTM sequencer response
    if ( (0 != RFCA_SendToFtmTaskSync(addCommandsToQueuePacket.GetRequestDataPtr(), addCommandsToQueuePacket.GetRequestLength(), addCommandsToQueuePacket.GetResponseDataPtr(), addCommandsToQueuePacket.GetResponseLengthPtr()))
      || (0!= addCommandsToQueuePacket.GetResponseDataPtr()[10]))
    {
      HalLogWrite("Failed to send FTM packages to FTM sequencer.\n");
      addCommandsToQueuePacket.FreeResponse();
      return RFCA_FUNCTION_FAIL;
    }
    addCommandsToQueuePacket.FreeResponse();
    NewMessagesBeenSentToFtmSequencer(true);
  } 
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
bool RFCA_ModeToFtmRfMode(unsigned short *rfMode, int technology, int band)
{

  switch(technology)
  {
  case HAL_RF_PROTOCOL_SYSTEM_GSM: 
    switch(band)
    {
    case HAL_RF_BAND_GSM_850:
      *rfMode = PHONE_MODE_GSM_850;
      break;
    case HAL_RF_BAND_GSM_900:
      *rfMode = PHONE_MODE_GSM_900;
      break;
    case HAL_RF_BAND_GSM_1800:
      *rfMode = PHONE_MODE_GSM_1800;
      break;
    case HAL_RF_BAND_GSM_1900:
      *rfMode = PHONE_MODE_GSM_1900;
      break;
    default:
      HalLogWrite("Given band %d is not supported or is invalid.\n", band);
      return false;
      break;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_CDMA:
    switch(band)
    {
    case HAL_RF_BAND_CDMA2K_0:
      *rfMode = PHONE_MODE_CDMA_800;
      break;
    case HAL_RF_BAND_CDMA2K_1:
      *rfMode = PHONE_MODE_CDMA_1900;
      break;
    case HAL_RF_BAND_CDMA2K_3:
      *rfMode = PHONE_MODE_J_CDMA;
      break;
    case HAL_RF_BAND_CDMA2K_4:
      *rfMode = PHONE_MODE_CDMA_1800;
      break;
    case HAL_RF_BAND_CDMA2K_5:
      *rfMode = PHONE_MODE_CDMA_450;
      break;
    case HAL_RF_BAND_CDMA2K_6:
      *rfMode = PHONE_MODE_CDMA_IMT;
      break;
    case HAL_RF_BAND_CDMA2K_10:
      *rfMode = PHONE_MODE_CDMA_800_SEC;
      break;
    case HAL_RF_BAND_CDMA2K_14:
      *rfMode = PHONE_MODE_CDMA_1900_EXT;
      break;
    case HAL_RF_BAND_CDMA2K_15:
      *rfMode = PHONE_MODE_CDMA_BC15;
      break;
    default:
      HalLogWrite("Given band %d is not supported or is invalid.\n", band);
      return false;
      break;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
    switch(band)
    {/*
     *PHONE_MODE_WCDMA_IMT   =9      (WCDMA IMT, Band I)
     PHONE_MODE_WCDMA_1900A =15,    (WCDMA 1900 A, Band II Add)
     PHONE_MODE_WCDMA_1900B =16,    (WCDMA 1900 B, Band II Gen)
     PHONE_MODE_WCDMA_800   =22,    (WCDMA 800, Band V Gen)
     PHONE_MODE_WCDMA_800A  =23,    (WCDMA 800, Band V Add)
     *PHONE_MODE_WCDMA_1800  =25,    (WCDMA 1800, Band III)
     *PHONE_MODE_WCDMA_BC4   =28,    (WCDMA BC4-used for both Band IV Gen and Band IV Add)
     *PHONE_MODE_WCDMA_BC8   =29,    (WCDMA BC8, Band VIII)
     *PHONE_MODE_WCDMA_BC9   =31,    (WCDMA BC9 (1750MHz & 1845MHz), Band IX) */
    case HAL_RF_BAND_WCDMA_I:
      *rfMode = PHONE_MODE_WCDMA_IMT;
      break;
    case HAL_RF_BAND_WCDMA_II :
      *rfMode = PHONE_MODE_WCDMA_1900B;  
      break;
    case HAL_RF_BAND_WCDMA_III:
      *rfMode = PHONE_MODE_WCDMA_1800;
      break;
    case HAL_RF_BAND_WCDMA_IV:
      *rfMode = PHONE_MODE_WCDMA_BC4;
      break;
    case HAL_RF_BAND_WCDMA_V:
      *rfMode = PHONE_MODE_WCDMA_800;     
      break;
    case HAL_RF_BAND_WCDMA_VI:
      HalLogWrite("Given band %d is not supported.\n", band);
      return false; // TODO: ?????
      break;
    case HAL_RF_BAND_WCDMA_VII:
      HalLogWrite("Given band %d is not supported.\n", band);      
      return false; // TODO: ?????
      break;
    case HAL_RF_BAND_WCDMA_VIII:
      *rfMode = PHONE_MODE_WCDMA_BC8;
      break;
    case HAL_RF_BAND_WCDMA_IX:
      *rfMode = PHONE_MODE_WCDMA_BC9;
      break;
    case HAL_RF_BAND_WCDMA_X:
      HalLogWrite("Given band %d is not supported.\n", band);      
      return false; // TODO: ?????
      break;
    default:
      HalLogWrite("Given band %d is not supported or is invalid.\n", band);      
      return false;
      break;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_LTE:
    switch(band)
    {
    case HAL_RF_BAND_LTE_1:
      *rfMode = PHONE_MODE_LTE_B1;
      break;
    case HAL_RF_BAND_LTE_2:
      *rfMode = PHONE_MODE_LTE_B2;
      break;
    case HAL_RF_BAND_LTE_3:
      *rfMode = PHONE_MODE_LTE_B3;
      break;
    case HAL_RF_BAND_LTE_4:
      *rfMode = PHONE_MODE_LTE_B4;
      break;
    case HAL_RF_BAND_LTE_5:
      *rfMode = PHONE_MODE_LTE_B5;
      break;
    case HAL_RF_BAND_LTE_7:
      *rfMode = PHONE_MODE_LTE_B7;
      break;
    case HAL_RF_BAND_LTE_8:
      *rfMode = PHONE_MODE_LTE_B8;
      break;
    case HAL_RF_BAND_LTE_9:
      *rfMode = PHONE_MODE_LTE_B9;
      break;
    case HAL_RF_BAND_LTE_10:
      *rfMode = PHONE_MODE_LTE_B10;
      break;
    case HAL_RF_BAND_LTE_11:
      *rfMode = PHONE_MODE_LTE_B11;
      break;
    case HAL_RF_BAND_LTE_12:
      *rfMode = PHONE_MODE_LTE_B12;
      break;
    case HAL_RF_BAND_LTE_13:
      *rfMode = PHONE_MODE_LTE_B13;
      break;
    case HAL_RF_BAND_LTE_14:
      *rfMode = PHONE_MODE_LTE_B14;
      break;
    case HAL_RF_BAND_LTE_17:
      *rfMode = PHONE_MODE_LTE_B17;
      break;
    case HAL_RF_BAND_LTE_18:
      *rfMode = PHONE_MODE_LTE_B18;
      break;
    case HAL_RF_BAND_LTE_19:
      *rfMode = PHONE_MODE_LTE_B19;
      break;
    case HAL_RF_BAND_LTE_20:
      *rfMode = PHONE_MODE_LTE_B20;
      break;
    case HAL_RF_BAND_LTE_21:
      *rfMode = PHONE_MODE_LTE_B21;
      break;
    case HAL_RF_BAND_LTE_22:
      *rfMode = PHONE_MODE_LTE_B22;
      break;
    case HAL_RF_BAND_LTE_23:
      *rfMode = PHONE_MODE_LTE_B23;
      break;
    case HAL_RF_BAND_LTE_24:
      *rfMode = PHONE_MODE_LTE_B24;
      break;
    case HAL_RF_BAND_LTE_25:
      *rfMode = PHONE_MODE_LTE_B25;
      break;
    case HAL_RF_BAND_LTE_26:
      *rfMode = PHONE_MODE_LTE_B26;
      break;
    case HAL_RF_BAND_LTE_27:
      *rfMode = PHONE_MODE_LTE_B27;
      break;
    case HAL_RF_BAND_LTE_28:
      *rfMode = PHONE_MODE_LTE_B28;
      break;
    case HAL_RF_BAND_LTE_29:
      *rfMode = PHONE_MODE_LTE_B29;
      break;
    case HAL_RF_BAND_LTE_30:
      *rfMode = PHONE_MODE_LTE_B30;
      break;
    case HAL_RF_BAND_LTE_31:
      *rfMode = PHONE_MODE_LTE_B31;
      break;
    /*case HAL_RF_BAND_LTE_32:
      *rfMode = PHONE_MODE_LTE_B32;
      break;*/
    case HAL_RF_BAND_LTE_33:
      *rfMode = PHONE_MODE_LTE_B33;
      break;
    case HAL_RF_BAND_LTE_34:
      *rfMode = PHONE_MODE_LTE_B34;
      break;
    case HAL_RF_BAND_LTE_35:
      *rfMode = PHONE_MODE_LTE_B35;
      break;
    case HAL_RF_BAND_LTE_36:
      *rfMode = PHONE_MODE_LTE_B36;
      break;
    case HAL_RF_BAND_LTE_37:
      *rfMode = PHONE_MODE_LTE_B37;
      break;
    case HAL_RF_BAND_LTE_38:
      *rfMode = PHONE_MODE_LTE_B38;
      break;
    case HAL_RF_BAND_LTE_39:
      *rfMode = PHONE_MODE_LTE_B39;
      break;
    case HAL_RF_BAND_LTE_40:
      *rfMode = PHONE_MODE_LTE_B40;
      break;
    case HAL_RF_BAND_LTE_41:
      *rfMode = PHONE_MODE_LTE_B41;
      break;
    case HAL_RF_BAND_LTE_42:
      *rfMode = PHONE_MODE_LTE_B42;
      break;
/*
    case HAL_RF_BAND_LTE_43:
      *rfMode = PHONE_MODE_LTE_B43;
      break;
    case HAL_RF_BAND_LTE_44:
      *rfMode = PHONE_MODE_LTE_B44;
      break;
 */
    default:
      HalLogWrite("Given band %d is not supported or is invalid.\n", band);      
      return false;
      break;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
    switch(band)
    {
    case HAL_RF_BAND_TDSCDMA_I:
      *rfMode = PHONE_MODE_TDSCDMA_B34;
      break;
    //case HAL_RF_BAND_TDSCDMA_II:
    // #error check this before enabling
    //  *rfMode = PHONE_MODE_TDSCDMA_B36; 
    //  break;
    //case HAL_RF_BAND_TDSCDMA_III:
      // #error check this before enabling
    //  *rfMode = PHONE_MODE_TDSCDMA_B37;
    //  break;
    //case HAL_RF_BAND_TDSCDMA_IV:
      // #error check this before enabling
    //  *rfMode = PHONE_MODE_TDSCDMA_B38;
    //  break;
    case HAL_RF_BAND_TDSCDMA_V:
      *rfMode =  PHONE_MODE_TDSCDMA_B40;
      break;
    case HAL_RF_BAND_TDSCDMA_VI:
      *rfMode = PHONE_MODE_TDSCDMA_B39;
      break;
    default:
      HalLogWrite("Given band %d is not supported or is invalid.\n", band);      
      return false;
      break;
    }
    break;
  default:
    HalLogWrite("Invalid technology %d!\n", technology);      
    return false;
    break;
  }
  return true;
}

/*----------------------------------------------------------------------------*/
bool RFCA_GetCurrentBand(uint16 *band)
{
  if(((InvalidStateCacheValue)) == StateCache.RF_TECHNOLOGY)
  {
    return false;
  }

  if(InvalidStateCacheValue == StateCache.BAND)
  {
    return false;
  }

  *band = StateCache.BAND;
  return true;
}

/*----------------------------------------------------------------------------*/
double GSM_CalculateFrequency(HAL_RF_GSM_BAND band,uint16 channel,bool uplink)
{
  double freq =-999.;
  double ul_fOffset = 0.0;
  double dl_fOffset = 0.0;
  switch (band) 
  {
  case HAL_RF_BAND_GSM_850:
  {
	  ul_fOffset=0.2 * (channel - 128);
      dl_fOffset=(0.2 * (channel - 128))+45.;
      if(uplink)
		  freq=824.2 +ul_fOffset;
	  else
		  freq=824.2 +dl_fOffset;
  }
  break;
  case HAL_RF_BAND_GSM_900:
  {
    if(channel <= 124 )
	  {
      ul_fOffset=0.2*channel;
		  dl_fOffset=0.2*channel;
	  }
    else if( channel >= 975 && channel <= 1023 )
	  {
	    ul_fOffset=0.2*(channel-1024);
		  dl_fOffset=0.2*(channel-1024);
	  }

	  if(uplink) 
		  freq=890. +ul_fOffset;
	  else
		  freq=890. +dl_fOffset;
	  }
   break;
   case HAL_RF_BAND_GSM_1800:
   {
      ul_fOffset=0.2 * (channel - 512);
      dl_fOffset=(0.2 * (channel - 512))+95.;
	  if(uplink)
		  freq=1710.2 +ul_fOffset;
      else 
		  freq=1710.2 +dl_fOffset;
   }
   break;
   case HAL_RF_BAND_GSM_1900:
   {
	   ul_fOffset=0.2 * (channel - 512);
     dl_fOffset=(0.2 * (channel - 512))+80.;
	   if(uplink) 
		   freq=1850.2 +ul_fOffset;
	   else
		   freq=1850.2 +dl_fOffset;
   }
   break;
   default:
	  HalLogWrite("Invalid GSM band enum (%d) for frequency calculation\n", band);
   break;
  }
  return freq;
}


/*----------------------------------------------------------------------------*/
uint16 WCDMA_CalculateChannel(HAL_RF_WCDMA_BAND band, uint16 channel, CHANNEL_TYPE inputChannelType)
{
   uint16 channelOffsets[HAL_RF_BAND_WCDMA_X+1] = {
      950,
      400,
      225,
      225,
      225,
      225,
      225,
      225,
      475,
      225};

  if(HAL_RF_BAND_WCDMA_X<band)
  {
    HalLogWrite("WCDMA_CalculateChannel: Invalid band = %d\n", band);
    return 0;
  }

   switch( inputChannelType )
   {
   case UL_CHANNEL:
      return (channel + channelOffsets[band]);
      break;
   case DL_CHANNEL:
      return (channel - channelOffsets[band]);
      break;
   default:
     HalLogWrite("WCDMA_CalculateChannel: Invalid inputChannelType = %d\n", inputChannelType);
     break;
   }

  return 0;  
}

/*----------------------------------------------------------------------------*/
double WCDMA_CalculateFrequency( HAL_RF_WCDMA_BAND band, long channel, bool uplink )
{
   double ul_fOffset = 0.0;
   double dl_fOffset = 0.0;
   double freq = 0.0;

   switch( band )
   {
   case HAL_RF_BAND_WCDMA_I:
      ul_fOffset = 0.0;
      dl_fOffset = 0.0;
      break;

   case HAL_RF_BAND_WCDMA_II:
      if( channel > 4000 ) // general channel
      {
         ul_fOffset = 0.0;
         dl_fOffset = 0.0;
      }			
      else  // additional channel
      {
         ul_fOffset = 1850.1;
         dl_fOffset = 1850.1;
      }			
      break;

   case HAL_RF_BAND_WCDMA_III:
      ul_fOffset = 1525.0;
      dl_fOffset = 1575.0;
      break;

   case HAL_RF_BAND_WCDMA_IV:
      if( (uplink && channel < 1600) ||
         (!uplink && channel < 1800) ) // general channel
      {
         ul_fOffset = 1450.0;
         dl_fOffset = 1805.0;
      }				
      else  // additional channel
      {
         ul_fOffset = 1380.1;
         dl_fOffset = 1735.1;
      }			
      break;

   case HAL_RF_BAND_WCDMA_V:
   case HAL_RF_BAND_WCDMA_VI:
      if( channel > 4000 ) // general channel
      {
         ul_fOffset = 0.0;
         dl_fOffset = 0.0;
      }			
      else  // additional channel
      {
         ul_fOffset = 670.1;
         dl_fOffset = 670.1;
      }
      break;

   case HAL_RF_BAND_WCDMA_VII:
      if( (uplink && channel < 2360) ||
         (!uplink && channel < 2580) ) // general channel
      {
         ul_fOffset = 2100.0;
         dl_fOffset = 2175.0;
      }			
      else  // additional channel
      {
         ul_fOffset = 2030.1;
         dl_fOffset = 2105.1;
      }
      break;

   case HAL_RF_BAND_WCDMA_VIII:
      ul_fOffset = 340.0;
      dl_fOffset = 340.0;
      break;

   case HAL_RF_BAND_WCDMA_IX:
      ul_fOffset = 0.0;
      dl_fOffset = 0.0;
      break;

   case HAL_RF_BAND_WCDMA_X:
      if( channel < 3164 )
      {
         ul_fOffset = 1135.0;
         dl_fOffset = 1490.0;
      }
      else  // additional channel
      {
         ul_fOffset = 1075.1;
         dl_fOffset = 1430.1;
      }
      break;

   default:
     HalLogWrite("Invalid WCDMA band enum (%d) for frequency calculation\n", band);
     return 0.0;
     break;
   }

   if( uplink )
   {
      freq = channel * 0.2 + ul_fOffset;
   }
   else
   {
      freq = channel * 0.2 + dl_fOffset;
   }

   return freq;
}

/*----------------------------------------------------------------------------*/
double TDSCDMA_CalculateFrequency( HAL_RF_TDSCDMA_BAND band, long channel, bool uplink )
{
  return (double) channel / 5.0;
}

/*----------------------------------------------------------------------------*/
int LTE_GetRxTxChanSep( HAL_RF_LTE_BAND band )
{
   int chanSep = 0;

   switch( band )
   {
   case HAL_RF_BAND_LTE_1:
   case HAL_RF_BAND_LTE_2:
   case HAL_RF_BAND_LTE_3:
   case HAL_RF_BAND_LTE_4:
   case HAL_RF_BAND_LTE_5:
   //case HAL_RF_BAND_LTE_6:
   case HAL_RF_BAND_LTE_7:
   case HAL_RF_BAND_LTE_8:
   case HAL_RF_BAND_LTE_9:
   case HAL_RF_BAND_LTE_10:
   case HAL_RF_BAND_LTE_11:
   case HAL_RF_BAND_LTE_12:
   case HAL_RF_BAND_LTE_13:
   case HAL_RF_BAND_LTE_14:
   case HAL_RF_BAND_LTE_17:
   case HAL_RF_BAND_LTE_18:
   case HAL_RF_BAND_LTE_19:
   case HAL_RF_BAND_LTE_20:
   case HAL_RF_BAND_LTE_21:
   case HAL_RF_BAND_LTE_22:
   case HAL_RF_BAND_LTE_23:
   case HAL_RF_BAND_LTE_24:
   case HAL_RF_BAND_LTE_25:
   case HAL_RF_BAND_LTE_26:
   case HAL_RF_BAND_LTE_27:
   case HAL_RF_BAND_LTE_28:
      chanSep = -18000;
      break;
   case HAL_RF_BAND_LTE_30:
   case HAL_RF_BAND_LTE_31:
      chanSep = -17890;
      break;
   case HAL_RF_BAND_LTE_29:
   case HAL_RF_BAND_LTE_33:
   case HAL_RF_BAND_LTE_34:
   case HAL_RF_BAND_LTE_35:
   case HAL_RF_BAND_LTE_36:
   case HAL_RF_BAND_LTE_37:
   case HAL_RF_BAND_LTE_38:
   case HAL_RF_BAND_LTE_39:
   case HAL_RF_BAND_LTE_40:
   case HAL_RF_BAND_LTE_41:
   case HAL_RF_BAND_LTE_42:
   case HAL_RF_BAND_LTE_43:
   case HAL_RF_BAND_LTE_44:
      chanSep = 0;
      break;
   }

   return chanSep;
}

/*----------------------------------------------------------------------------*/
double LTE_GetRxTxFreqSep( HAL_RF_LTE_BAND band )
{
   double freqSep = 0;
   switch( band )
   {
   case HAL_RF_BAND_LTE_1:
      freqSep = 190;
      break;
   case HAL_RF_BAND_LTE_2:
      freqSep = 80;
      break;
   case HAL_RF_BAND_LTE_3:
      freqSep = 95;
      break;
   case HAL_RF_BAND_LTE_4:
      freqSep = 400;
      break;
   case HAL_RF_BAND_LTE_5:
//   case HAL_RF_BAND_LTE_6:
      freqSep = 45;
      break;
   case HAL_RF_BAND_LTE_7:
      freqSep = 120;
      break;
   case HAL_RF_BAND_LTE_8:
      freqSep = 45;
      break;
   case HAL_RF_BAND_LTE_9:
      freqSep = 95;
      break;
   case HAL_RF_BAND_LTE_10:
      freqSep = 400;
      break;
   case HAL_RF_BAND_LTE_11:
      freqSep = 48;
      break;
   case HAL_RF_BAND_LTE_12:
      freqSep = 30;
      break;
   case HAL_RF_BAND_LTE_13:
      freqSep = -31;
      break;
   case HAL_RF_BAND_LTE_14:
      freqSep = -30;
      break;
   case HAL_RF_BAND_LTE_17:
      freqSep = 30;
      break;
   case HAL_RF_BAND_LTE_18:
   case HAL_RF_BAND_LTE_19:
      freqSep = 45;
      break;
   case HAL_RF_BAND_LTE_20:
      freqSep = -41;
      break;
   case HAL_RF_BAND_LTE_21:
      freqSep = 48;
      break;
   case HAL_RF_BAND_LTE_22:
      freqSep = 100;
      break;
   case HAL_RF_BAND_LTE_23:
      freqSep = 180;
      break;
   case HAL_RF_BAND_LTE_24:
      freqSep = -101.5;
      break;
   case HAL_RF_BAND_LTE_25:
      freqSep = 80;
      break;
   case HAL_RF_BAND_LTE_26:
   case HAL_RF_BAND_LTE_27:
      freqSep = 45;
      break;
   case HAL_RF_BAND_LTE_28:
      freqSep = 55;
      break;
   case HAL_RF_BAND_LTE_30:
      freqSep = 45;
      break;
   case HAL_RF_BAND_LTE_31:
      freqSep = 10;
      break;
   case HAL_RF_BAND_LTE_29:
   case HAL_RF_BAND_LTE_33:
   case HAL_RF_BAND_LTE_34:
   case HAL_RF_BAND_LTE_35:
   case HAL_RF_BAND_LTE_36:
   case HAL_RF_BAND_LTE_37:
   case HAL_RF_BAND_LTE_38:
   case HAL_RF_BAND_LTE_39:
   case HAL_RF_BAND_LTE_40:
   case HAL_RF_BAND_LTE_41:
   case HAL_RF_BAND_LTE_42:
   case HAL_RF_BAND_LTE_43:
   case HAL_RF_BAND_LTE_44:
      freqSep = 0;
      break;

   }
   return freqSep;
}

/*----------------------------------------------------------------------------*/
double LTE_CalculateFrequency( HAL_RF_LTE_BAND band, long channel, bool uplink )
{
   //FDL = FDL_low + 0.1(NDL ?NOffs-DL)

   //Table 5.7.3-1 of 36.101-900
   double dl_NOffset = 0.0;
   double ul_NOffset = 0.0;
   double dl_FreqLow = 0.0;
   double ul_FreqLow = 0.0;

   double freq;

   switch( band )
   {
   case HAL_RF_BAND_LTE_1:
      dl_NOffset = 0;
      dl_FreqLow = 2110.0;
      ul_FreqLow = 1920.0;
      break;
   case HAL_RF_BAND_LTE_2:
      dl_NOffset = 600;
      dl_FreqLow = 1930.0;
      ul_FreqLow = 1850.0;
      break;
   case HAL_RF_BAND_LTE_3:
      dl_NOffset = 1200;
      dl_FreqLow = 1805.0;
      ul_FreqLow = 1710.0;
      break;
   case HAL_RF_BAND_LTE_4:
      dl_NOffset = 1950;
      dl_FreqLow = 2110.0;
      ul_FreqLow = 1710.0;
      break;
   case HAL_RF_BAND_LTE_5:
      dl_NOffset = 2400;
      dl_FreqLow = 869.0;
      ul_FreqLow = 824.0;
      break;
   //case HAL_RF_BAND_LTE_6:
   //   dl_NOffset = 2650;
   //   dl_FreqLow = 875.0;
   //   ul_FreqLow = 830.0;
   //   break;
   case HAL_RF_BAND_LTE_7:
      dl_NOffset = 2750;
      dl_FreqLow = 2620.0;
      ul_FreqLow = 2500.0;
      break;
   case HAL_RF_BAND_LTE_8:
      dl_NOffset = 3450;
      dl_FreqLow = 925.0;
      ul_FreqLow = 880.0;
      break;
   case HAL_RF_BAND_LTE_9:
      dl_NOffset = 3800;
      dl_FreqLow = 1844.9;
      ul_FreqLow = 1749.9;
      break;
   case HAL_RF_BAND_LTE_10:
      dl_NOffset = 4150;
      dl_FreqLow = 2110;
      ul_FreqLow = 1710;
      break;
   case HAL_RF_BAND_LTE_11:
      dl_NOffset = 4750;
      dl_FreqLow = 1475.9;
      ul_FreqLow = 1427.9;
      break;
   case HAL_RF_BAND_LTE_12:
      dl_NOffset = 5000;
      dl_FreqLow = 728;
      ul_FreqLow = 698;
      break;
   case HAL_RF_BAND_LTE_13:
      dl_NOffset = 5180;
      dl_FreqLow = 746;
      ul_FreqLow = 777;
      break;
   case HAL_RF_BAND_LTE_14:
      dl_NOffset = 5280;
      dl_FreqLow = 758;
      ul_FreqLow = 788;
      break;
   case HAL_RF_BAND_LTE_17:
      dl_NOffset = 5730;
      dl_FreqLow = 734;
      ul_FreqLow = 704;
      break;
   case HAL_RF_BAND_LTE_18:
      dl_NOffset = 5850;
      dl_FreqLow = 860;
      ul_FreqLow = 815;
      break;
   case HAL_RF_BAND_LTE_19:
      dl_NOffset = 6000;
      dl_FreqLow = 875;
      ul_FreqLow = 830;
      break;
   case HAL_RF_BAND_LTE_20:
      dl_NOffset = 6150;
      dl_FreqLow = 791;
      ul_FreqLow = 832;
      break;
   case HAL_RF_BAND_LTE_21:
      dl_NOffset = 6450;
      dl_FreqLow = 1495.9;
      ul_FreqLow = 1447.9;
      break;
   case HAL_RF_BAND_LTE_22:
      dl_NOffset = 6600;
      dl_FreqLow = 3510;
      ul_FreqLow = 3410;
      break;
   case HAL_RF_BAND_LTE_23:
      dl_NOffset = 7500;
      dl_FreqLow = 2180;
      ul_FreqLow = 2000;
      break;
   case HAL_RF_BAND_LTE_24:
      dl_NOffset = 7700;
      dl_FreqLow = 1525.0;
      ul_FreqLow = 1626.5;
      break;
   case HAL_RF_BAND_LTE_25:
      dl_NOffset = 8040;
      dl_FreqLow = 1930.0;
      ul_FreqLow = 1850.0;
      break;
   case HAL_RF_BAND_LTE_26:
      dl_NOffset = 8690;
      dl_FreqLow = 859.0;
      ul_FreqLow = 814.0;
      break;
   case HAL_RF_BAND_LTE_27:
      dl_NOffset = 9040;
      dl_FreqLow = 852.0;
      ul_FreqLow = 807.0;
      break;
   case HAL_RF_BAND_LTE_28:
      dl_NOffset = 9210;
      dl_FreqLow = 758.0;
      ul_FreqLow = 703.0;
      break;
   case HAL_RF_BAND_LTE_29:
      dl_NOffset = 9660;
      dl_FreqLow = 717.0;
      ul_FreqLow = 717.0; //??
      break;
   case HAL_RF_BAND_LTE_30:
      dl_NOffset = 9770;
      dl_FreqLow = 2350.0;
      ul_FreqLow = 2305.0;
      break;
   case HAL_RF_BAND_LTE_31:
      dl_NOffset = 9870;
      dl_FreqLow = 462.5;
      ul_FreqLow = 452.5;
      break;
   case HAL_RF_BAND_LTE_33:
      dl_NOffset = 36000;
      dl_FreqLow = 1900;
      ul_FreqLow = 1900;
      break;
   case HAL_RF_BAND_LTE_34:
      dl_NOffset = 36200;
      dl_FreqLow = 2010;
      ul_FreqLow = 2010;
      break;
   case HAL_RF_BAND_LTE_35:
      dl_NOffset = 36350;
      dl_FreqLow = 1850;
      ul_FreqLow = 1850;
      break;
   case HAL_RF_BAND_LTE_36:
      dl_NOffset = 36950;
      dl_FreqLow = 1930;
      ul_FreqLow = 1930;
      break;
   case HAL_RF_BAND_LTE_37:
      dl_NOffset = 37550;
      dl_FreqLow = 1910;
      ul_FreqLow = 1910;
      break;
   case HAL_RF_BAND_LTE_38:
      dl_NOffset = 37750;
      dl_FreqLow = 2570;
      ul_FreqLow = 2570;
      break;
   case HAL_RF_BAND_LTE_39:
      dl_NOffset = 38250;
      dl_FreqLow = 1880;
      ul_FreqLow = 1880;
      break;
   case HAL_RF_BAND_LTE_40:
      dl_NOffset = 38650;
      dl_FreqLow = 2300;
      ul_FreqLow = 2300;
      break;
   case HAL_RF_BAND_LTE_41:
      dl_NOffset = 39650;
      dl_FreqLow = 2496;
      ul_FreqLow = 2496;
      break;
   case HAL_RF_BAND_LTE_42:
      dl_NOffset = 41590;
      dl_FreqLow = 3400;
      ul_FreqLow = 3400;
      break;
   case HAL_RF_BAND_LTE_43:
      dl_NOffset = 43590;
      dl_FreqLow = 3600;
      ul_FreqLow = 3600;
      break; 
   case HAL_RF_BAND_LTE_44:
      dl_NOffset = 45590;
      dl_FreqLow = 703;
      ul_FreqLow = 703;
      break; 
   default:
     HalLogWrite("Invalid LTE band enum (%d) for frequency calculation\n", band);
     return 0.0;
     break;
   }

   ul_NOffset = dl_NOffset - LTE_GetRxTxChanSep( band );
   if( uplink )
   {  
      //	FUL = FUL_low + 0.1(NUL ?NOffs-UL) = FUL_low + 0.1(NDL - NOffs-DL)
      //channel is the Downlink Channel EARFCN#
      freq = ul_FreqLow + 0.1 * ( channel - ul_NOffset );
   }
   else
   {
      //FDL = FDL_low + 0.1(NDL ?NOffs-DL)
      freq = dl_FreqLow + 0.1 * ( channel - dl_NOffset );
   }

   return freq;
}

/*----------------------------------------------------------------------------*/
uint16 LTE_CalculateChannel(HAL_RF_LTE_BAND band, uint16 channel, CHANNEL_TYPE inputChannelType)
{
  const uint16 LTE_FDD_CH_OFFSET = 18000;
  const uint16 LTE_FDD_CH_OFFSET_2 = 17890;
  const uint16 LTE_TDD_CH_OFFSET = 0;

  uint16 channelOffset = 0;

  if(HAL_RF_BAND_LTE_28 >= band)
  {
    // FDD bands 1-28
    channelOffset = LTE_FDD_CH_OFFSET;
  }
  else if(HAL_RF_BAND_LTE_29 == band)
  {
    // Do nothing, at most fail if trying to get up link
    channelOffset = 0;
  }
  else if((HAL_RF_BAND_LTE_30 <= band) && (HAL_RF_BAND_LTE_31 >= band) )
  {
    channelOffset = LTE_FDD_CH_OFFSET_2;
  }
  else if ((HAL_RF_BAND_LTE_33 <= band) && (HAL_RF_BAND_LTE_44 >= band) )
  {
    // TDD bands 33-40
    channelOffset = LTE_TDD_CH_OFFSET;
  }
  else
  {
    HalLogWrite("LTE_CalculateChannel: Invalid band = %d\n", band);
    return 0;
  }
  
  switch( inputChannelType )
   {
   case UL_CHANNEL:
      return (channel - channelOffset);
      break;
   case DL_CHANNEL:
      return (channel + channelOffset);
      break;
   default:
     HalLogWrite("LTE_CalculateChannel: Invalid inputChannelType = %d\n", inputChannelType);
     break;
   }
  return 0;
}


/*----------------------------------------------------------------------------*/
int RFCA_ReadRfNv(word id, uint8 *buffer, uint16 *bufferSize)
{

  const int maxRfNvItemSize = 1024; // RFNV_DATA_SIZE_MAX is 8K but this is enough here

  RfcaPacketHelper packet;

  if(NULL == packet.GetRequestDataPtr())
  {
    MSG_ERROR("No memory for FTM request, will not be able to execute command", 0 ,0 ,0);
    return RFCA_FUNCTION_FAIL;
  }

  FormatFTMHeader(&packet, FTM_RFNV_C );
  packet.AppendRequest( (uint16)_FTM_RFNV_READ );
  packet.AppendRequest( (uint16)(0x0000) );
  packet.AppendRequest( (uint16)(0x0090) );  // 144 = 128 nv item size + 16 message header size
  packet.AppendRequest( id );

  if(false == packet.AllocateResponse(maxRfNvItemSize))
  {
    HalLogWrite("RFNV read: Failed to allocate memory for reponse buffer\n");      
    return RFCA_FUNCTION_FAIL;
  }

  *(packet.GetResponseLengthPtr()) = maxRfNvItemSize;
  if(0 != RFCA_SendToFtmTaskSync(packet.GetRequestDataPtr(), packet.GetRequestLength(), packet.GetResponseDataPtr(), packet.GetResponseLengthPtr()))
  {
    packet.FreeResponse();
    return RFCA_FUNCTION_FAIL;
  }
  word rfnvStatus = RFCOMMON_NV_WRITE_FAILED;

  packet.GetResponseWordFromOffset(10, &rfnvStatus);

  // See if read failed or not
  if(RFCOMMON_NV_READ_FAILED == rfnvStatus)
  {
    HalLogWrite("RFNV read: FAILED\n");      
    packet.FreeResponse();
    return RFCA_FUNCTION_FAIL;
  }

  // If item was not found and we got RFCOMMON_NV_INACTIVE_ITEM then we just say OK and return 0 bytes
  if(RFCOMMON_NV_INACTIVE_ITEM == rfnvStatus)
  {
    HalLogWrite("RFNV read: INACTIVE ITEM\n");      
    *bufferSize = 0;
    packet.FreeResponse();
    return RFCA_FUNCTION_SUCCESS;
  }

  word rfnvSize = 0; 
  packet.GetResponseWordFromOffset(14, &rfnvSize);

  // Data size vs buffer size
  if(*bufferSize < rfnvSize)
  {
    // Buffer too small
    HalLogWrite("RFNV read: Allocated buffer (%d) too small for data (%d)\n", *bufferSize, rfnvSize);      
    packet.FreeResponse();
    return RFCA_FUNCTION_FAIL;
  }

  *bufferSize = rfnvSize;
  memscpy(buffer, rfnvSize, packet.GetResponseDataPtr() + 16, rfnvSize);
  packet.FreeResponse();

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
bool rfcaDebugMode = false;

void HalLogWrite_int( const char *format , ... )
{
  va_list args;
  va_start( args, format );
  char msg[MAX_HAL_LOG_BUFFER_SZIE];  

  vsnprintf(msg, sizeof(msg)-1, format, args);

  MSG_SPRINTF_FMT_VAR_3 (MSG_SSID_FTM, MSG_LEGACY_HIGH, msg, 0, 0, 0);
  
}

struct HAL_MSG_NAME{
unsigned short id;
const char* name;
} HAL_MSG_IDS_AS_STRINGS[] = {
{0x0001,(const char*)"HAL_RF_INIT_REQ_ID"},
{0x0002,(const char*)"HAL_RF_INIT_RESP_ID"},
{0x0003,(const char*)"HAL_RF_GSM_RX_RSSI_REQ_ID"},
{0x0004,(const char*)"HAL_RF_GSM_RX_RSSI_RESP_ID"},
{0x0005,(const char*)"HAL_RF_GSM_RX_WAIT_REQ_ID"},
{0x0006,(const char*)"HAL_RF_GSM_RX_WAIT_RESP_ID"},
{0x0007,(const char*)"HAL_RF_GSM_RX_IQ_REQ_ID"},
{0x0008,(const char*)"HAL_RF_GSM_RX_IQ_RESP_ID"},
{0x0009,(const char*)"HAL_RF_GSM_TX_PCL_REQ_ID"},
{0x000A,(const char*)"HAL_RF_GSM_TX_PCL_RESP_ID"},
{0x000B,(const char*)"HAL_RF_GSM_TX_DAC_REQ_ID"},
{0x000C,(const char*)"HAL_RF_GSM_TX_DAC_RESP_ID"},
{0x000D,(const char*)"HAL_RF_WCDMA_RX_RSSI_REQ_ID"},
{0x000E,(const char*)"HAL_RF_WCDMA_RX_RSSI_RESP_ID"},
{0x000F,(const char*)"HAL_RF_WCDMA_RX_WAIT_REQ_ID"},
{0x0010,(const char*)"HAL_RF_WCDMA_RX_WAIT_RESP_ID"},
{0x0011,(const char*)"HAL_RF_WCDMA_RX_IQ_REQ_ID"},
{0x0012,(const char*)"HAL_RF_WCDMA_RX_IQ_RESP_ID"},
{0x0013,(const char*)"HAL_RF_WCDMA_TX_DBM_REQ_ID"},
{0x0014,(const char*)"HAL_RF_WCDMA_TX_DBM_RESP_ID"},
{0x0015,(const char*)"HAL_RF_WCDMA_TX_DAC_REQ_ID"},
{0x0016,(const char*)"HAL_RF_WCDMA_TX_DAC_RESP_ID"},
{0x0017,(const char*)"HAL_RF_TDSCDMA_RX_RSSI_REQ_ID"},
{0x0018,(const char*)"HAL_RF_TDSCDMA_RX_RSSI_RESP_ID"},
{0x0019,(const char*)"HAL_RF_TDSCDMA_RX_WAIT_REQ_ID"},
{0x001A,(const char*)"HAL_RF_TDSCDMA_RX_WAIT_RESP_ID"},
{0x001B,(const char*)"HAL_RF_TDSCDMA_RX_IQ_REQ_ID"},
{0x001C,(const char*)"HAL_RF_TDSCDMA_RX_IQ_RESP_ID"},
{0x001D,(const char*)"HAL_RF_TDSCDMA_TX_DBM_REQ_ID"},
{0x001E,(const char*)"HAL_RF_TDSCDMA_TX_DBM_RESP_ID"},
{0x001F,(const char*)"HAL_RF_TDSCDMA_TX_DAC_REQ_ID"},
{0x0020,(const char*)"HAL_RF_TDSCDMA_TX_DAC_RESP_ID"},
{0x0021,(const char*)"HAL_RF_STOP_REQ_ID"},
{0x0022,(const char*)"HAL_RF_STOP_RESP_ID"},
{0x0023,(const char*)"HAL_RF_FTC_ADJUST_REQ_ID"},
{0x0024,(const char*)"HAL_RF_FTC_ADJUST_RESP_ID"},
{0x0025,(const char*)"HAL_RF_CALIBRATION_VALUES_LOAD_REQ_ID"},
{0x0026,(const char*)"HAL_RF_CALIBRATION_VALUES_LOAD_RESP_ID"},
{0x0027,(const char*)"HAL_RF_CALIBRATION_VALUES_STORE_REQ_ID"},
{0x0028,(const char*)"HAL_RF_CALIBRATION_VALUES_STORE_RESP_ID"},
{0x0029,(const char*)"HAL_RF_RFIC_READ_REQ_ID"},
{0x002A,(const char*)"HAL_RF_RFIC_READ_RESP_ID"},
{0x002B,(const char*)"HAL_RF_RFIC_WRITE_REQ_ID"},
{0x002C,(const char*)"HAL_RF_RFIC_WRITE_RESP_ID"},
{0x002D,(const char*)"HAL_RF_INFORM_REQ_ID"},
{0x002E,(const char*)"HAL_RF_INFORM_RESP_ID"},
{0x0031,(const char*)"HAL_RF_LTE_RX_RSSI_REQ_ID"},
{0x0032,(const char*)"HAL_RF_LTE_RX_RSSI_RESP_ID"},
{0x0033,(const char*)"HAL_RF_LTE_RX_WAIT_REQ_ID"},
{0x0034,(const char*)"HAL_RF_LTE_RX_WAIT_RESP_ID"},
{0x0035,(const char*)"HAL_RF_LTE_RX_IQ_REQ_ID"},
{0x0036,(const char*)"HAL_RF_LTE_RX_IQ_RESP_ID"},
{0x0037,(const char*)"HAL_RF_LTE_TX_DBM_REQ_ID"},
{0x0038,(const char*)"HAL_RF_LTE_TX_DBM_RESP_ID"},
{0x0039,(const char*)"HAL_RF_LTE_TX_DAC_REQ_ID"},
{0x003A,(const char*)"HAL_RF_LTE_TX_DAC_RESP_ID"},
{0x003B,(const char*)"HAL_RF_ARB_TX_REQ_ID"},
{0x003C,(const char*)"HAL_RF_ARB_TX_RESP_ID"},
{0x003D,(const char*)"HAL_RF_GSM_RX_ALIGN_REQ_ID"},
{0x003E,(const char*)"HAL_RF_GSM_RX_ALIGN_RESP_ID"},
{0x003F,(const char*)"HAL_RF_GSM_TX_ALIGN_REQ_ID"},
{0x0040,(const char*)"HAL_RF_GSM_TX_ALIGN_RESP_ID"},
{0x0041,(const char*)"HAL_RF_WCDMA_RX_ALIGN_REQ_ID"},
{0x0042,(const char*)"HAL_RF_WCDMA_RX_ALIGN_RESP_ID"},
{0x0043,(const char*)"HAL_RF_WCDMA_TX_ALIGN_REQ_ID"},
{0x0044,(const char*)"HAL_RF_WCDMA_TX_ALIGN_RESP_ID"},
{0x0045,(const char*)"HAL_RF_TDSCDMA_RX_ALIGN_REQ_ID"},
{0x0046,(const char*)"HAL_RF_TDSCDMA_RX_ALIGN_RESP_ID"},
{0x0047,(const char*)"HAL_RF_TDSCDMA_TX_ALIGN_REQ_ID"},
{0x0048,(const char*)"HAL_RF_TDSCDMA_TX_ALIGN_RESP_ID"},
{0x0049,(const char*)"HAL_RF_LTE_RX_ALIGN_REQ_ID"},
{0x004A,(const char*)"HAL_RF_LTE_RX_ALIGN_RESP_ID"},
{0x004B,(const char*)"HAL_RF_LTE_TX_ALIGN_REQ_ID"},
{0x004C,(const char*)"HAL_RF_LTE_TX_ALIGN_RESP_ID"},
{0x004D,(const char*)"HAL_RF_CDMA2K_RX_RSSI_REQ_ID"},
{0x004E,(const char*)"HAL_RF_CDMA2K_RX_RSSI_RESP_ID"},
{0x004F,(const char*)"HAL_RF_CDMA2K_RX_WAIT_REQ_ID"},
{0x0050,(const char*)"HAL_RF_CDMA2K_RX_WAIT_RESP_ID"},
{0x0051,(const char*)"HAL_RF_CDMA2K_RX_IQ_REQ_ID"},
{0x0052,(const char*)"HAL_RF_CDMA2K_RX_IQ_RESP_ID"},
{0x0053,(const char*)"HAL_RF_CDMA2K_TX_DBM_REQ_ID"},
{0x0054,(const char*)"HAL_RF_CDMA2K_TX_DBM_RESP_ID"},
{0x0055,(const char*)"HAL_RF_CDMA2K_TX_DAC_REQ_ID"},
{0x0056,(const char*)"HAL_RF_CDMA2K_TX_DAC_RESP_ID"},
{0x0057,(const char*)"HAL_RF_CDMA2K_RX_ALIGN_REQ_ID"},
{0x0058,(const char*)"HAL_RF_CDMA2K_RX_ALIGN_RESP_ID"},
{0x0059,(const char*)"HAL_RF_CDMA2K_TX_ALIGN_REQ_ID"},
{0x005A,(const char*)"HAL_RF_CDMA2K_TX_ALIGN_RESP_ID"},
{0x005B,(const char*)"HAL_RF_LTEFDD_RX_RSSI_REQ_ID"},
{0x005C,(const char*)"HAL_RF_LTEFDD_RX_RSSI_RESP_ID"},
{0x005D,(const char*)"HAL_RF_LTEFDD_RX_WAIT_REQ_ID"},
{0x005E,(const char*)"HAL_RF_LTEFDD_RX_WAIT_RESP_ID"},
{0x005F,(const char*)"HAL_RF_LTEFDD_RX_IQ_REQ_ID"},
{0x0060,(const char*)"HAL_RF_LTEFDD_RX_IQ_RESP_ID"},
{0x0061,(const char*)"HAL_RF_LTEFDD_TX_DBM_REQ_ID"},
{0x0062,(const char*)"HAL_RF_LTEFDD_TX_DBM_RESP_ID"},
{0x0063,(const char*)"HAL_RF_LTEFDD_TX_DAC_REQ_ID"},
{0x0064,(const char*)"HAL_RF_LTEFDD_TX_DAC_RESP_ID"},
{0x0065,(const char*)"HAL_RX_LTEFDD_RX_ALIGN_REQ_ID"},
{0x0066,(const char*)"HAL_RX_LTEFDD_RX_ALIGN_RESP_ID"},
{0x0067,(const char*)"HAL_RX_LTEFDD_TX_ALIGN_REQ_ID"},
{0x0068,(const char*)"HAL_RX_LTEFDD_TX_ALIGN_RESP_ID"},
{0x0069,(const char*)"HAL_RF_LTETDD_RX_RSSI_REQ_ID"},
{0x006A,(const char*)"HAL_RF_LTETDD_RX_RSSI_RESP_ID"},
{0x006B,(const char*)"HAL_RF_LTETDD_RX_WAIT_REQ_ID"},
{0x006C,(const char*)"HAL_RF_LTETDD_RX_WAIT_RESP_ID"},
{0x006D,(const char*)"HAL_RF_LTETDD_RX_IQ_REQ_ID"},
{0x006E,(const char*)"HAL_RF_LTETDD_RX_IQ_RESP_ID"},
{0x006F,(const char*)"HAL_RF_LTETDD_TX_DBM_REQ_ID"},
{0x0070,(const char*)"HAL_RF_LTETDD_TX_DBM_RESP_ID"},
{0x0071,(const char*)"HAL_RF_LTETDD_TX_DAC_REQ_ID"},
{0x0072,(const char*)"HAL_RF_LTETDD_TX_DAC_RESP_ID"},
{0x0073,(const char*)"HAL_RF_LTETDD_RX_ALIGN_REQ_ID"},
{0x0074,(const char*)"HAL_RF_LTETDD_RX_ALIGN_RESP_ID"},
{0x0075,(const char*)"HAL_RF_LTETDD_TX_ALIGN_REQ_ID"},
{0x0076,(const char*)"HAL_RF_LTETDD_TX_ALIGN_RESP_ID"},
{0x0077,(const char*)"HAL_RF_GSM_RX_WAIT_V2_REQ_ID"},
{0x0078,(const char*)"HAL_RF_GSM_RX_WAIT_V2_RESP_ID"},
{0x0079,(const char*)"HAL_RF_WCDMA_RX_WAIT_V2_REQ_ID"},
{0x007A,(const char*)"HAL_RF_WCDMA_RX_WAIT_V2_RESP_ID"},
{0x007B,(const char*)"HAL_RF_TDSCDMA_RX_WAIT_V2_REQ_ID"},
{0x007C,(const char*)"HAL_RF_TDSCDMA_RX_WAIT_V2_RESP_ID"},
{0x007D,(const char*)"HAL_RF_LTEFDD_RX_WAIT_V2_REQ_ID"},
{0x007E,(const char*)"HAL_RF_LTEFDD_RX_WAIT_V2_RESP_ID"},
{0x007F,(const char*)"HAL_RF_CDMA2K_RX_WAIT_V2_REQ_ID"},
{0x0080,(const char*)"HAL_RF_CDMA2K_RX_WAIT_V2_RESP_ID"},
{0x1000,(const char*)"HAL_RF_FINISH_REQ_ID"},
{0x1001,(const char*)"HAL_RF_FINISH_RESP_ID"}
};






int HalMsgIdCmp( const void *a, const void *b) 
{

  struct HAL_MSG_NAME *msgA = (struct HAL_MSG_NAME *)a;
  struct HAL_MSG_NAME *msgB = (struct HAL_MSG_NAME *)b;
  if(msgA->id<msgB->id)
  {
    return -1;
  }
  else if(msgA->id>msgB->id)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#include <stdlib.h>
const char* HalGetMgsName(unsigned short id)
{
  struct HAL_MSG_NAME toLookFor = {id, NULL};
  struct HAL_MSG_NAME *whatWeFound = (HAL_MSG_NAME *)bsearch(&toLookFor, HAL_MSG_IDS_AS_STRINGS, sizeof(HAL_MSG_IDS_AS_STRINGS)/sizeof(HAL_MSG_IDS_AS_STRINGS[0]), sizeof(struct HAL_MSG_NAME), HalMsgIdCmp);
  if(NULL != whatWeFound )
  {
    return whatWeFound->name;
  }
  else
  {
    return "";
  }

}


struct ConfigKeyToValueTarget
{
  const char* configKey;
  uint32* valueTarget;
};

// Keep in sorted order, binary search is used 
ConfigKeyToValueTarget configMap[] = {
{ "CDMA.RX_IQ.CONFIG", &CdmaRfcaTimeConfig.RX_IQ.rfConfig },
{ "CDMA.RX_IQ.PREEXECUTE", &CdmaRfcaTimeConfig.RX_IQ.rfPreExec },
{ "CDMA.RX_IQ.TUNE", &CdmaRfcaTimeConfig.RX_IQ.rfTune },
{ "CDMA.RX_RSSI.CONFIG", &CdmaRfcaTimeConfig.RX_RSSI.rfConfig },
{ "CDMA.RX_RSSI.PREEXECUTE", &CdmaRfcaTimeConfig.RX_RSSI.rfPreExec },
{ "CDMA.RX_RSSI.TUNE", &CdmaRfcaTimeConfig.RX_RSSI.rfTune },
{ "CDMA.RX_WAIT.CONFIG", &CdmaRfcaTimeConfig.RX_WAIT.rfConfig },
{ "CDMA.RX_WAIT.PREEXECUTE", &CdmaRfcaTimeConfig.RX_WAIT.rfPreExec },
{ "CDMA.RX_WAIT.TUNE", &CdmaRfcaTimeConfig.RX_WAIT.rfTune },
{ "GSM.ARB_TX.DRIVER_AMP.CONFIG", &GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfConfig },
{ "GSM.ARB_TX.DRIVER_AMP.EXECUTE", &GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfExec },
{ "GSM.ARB_TX.DRIVER_AMP.TUNE", &GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfTune },
{ "GSM.ARB_TX.PRE_DIST.CONFIG", &GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfConfig },
{ "GSM.ARB_TX.PRE_DIST.EXECUTE", &GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfExec },
{ "GSM.ARB_TX.PRE_DIST.TUNE", &GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfTune },
{ "GSM.RX_ALIGN.CONFIG", &GsmRfcaTimeConfig.RX_ALIGN.rfConfig },
{ "GSM.RX_ALIGN.TUNE", &GsmRfcaTimeConfig.RX_ALIGN.rfTune },
{ "GSM.RX_IQ.CONFIG", &GsmRfcaTimeConfig.RX_IQ.rfConfig },
{ "GSM.RX_IQ.EXECUTE", &GsmRfcaTimeConfig.RX_IQ.rfExec },
{ "GSM.RX_IQ.PREEXECUTE", &GsmRfcaTimeConfig.RX_IQ.rfPreExec },
{ "GSM.RX_IQ.TUNE", &GsmRfcaTimeConfig.RX_IQ.rfTune },
{ "GSM.RX_RSSI.CONFIG", &GsmRfcaTimeConfig.RX_RSSI.rfConfig },
{ "GSM.RX_RSSI.EXECUTE", &GsmRfcaTimeConfig.RX_RSSI.rfExec },
{ "GSM.RX_RSSI.TUNE", &GsmRfcaTimeConfig.RX_RSSI.rfTune },
{ "GSM.RX_WAIT.CONFIG", &GsmRfcaTimeConfig.RX_WAIT.rfConfig },
{ "GSM.RX_WAIT.TUNE", &GsmRfcaTimeConfig.RX_WAIT.rfTune },
{ "GSM.TX_PCL.CONFIG", &GsmRfcaTimeConfig.TX_PCL.rfConfig },
{ "GSM.TX_PCL.EXECUTE", &GsmRfcaTimeConfig.TX_PCL.rfExec },
{ "GSM.TX_PCL.TUNE", &GsmRfcaTimeConfig.TX_PCL.rfTune },
{ "LTE.RX_IQ.CONFIG", &LteRfcaTimeConfig.RX_IQ.rfConfig },
{ "LTE.RX_IQ.EXECUTE", &LteRfcaTimeConfig.RX_IQ.rfExec },
{ "LTE.RX_IQ.PREEXECUTE", &LteRfcaTimeConfig.RX_IQ.rfPreExec },
{ "LTE.RX_IQ.TUNE", &LteRfcaTimeConfig.RX_IQ.rfTune },
{ "LTE.RX_RSSI.CONFIG", &LteRfcaTimeConfig.RX_RSSI.rfConfig },
{ "LTE.RX_RSSI.EXECUTE", &LteRfcaTimeConfig.RX_RSSI.rfExec },
{ "LTE.RX_RSSI.PREEXECUTE", &LteRfcaTimeConfig.RX_RSSI.rfPreExec },
{ "LTE.RX_RSSI.TUNE", &LteRfcaTimeConfig.RX_RSSI.rfTune },
{ "LTE.RX_WAIT.CONFIG", &LteRfcaTimeConfig.RX_WAIT.rfConfig },
{ "LTE.RX_WAIT.PREEXECUTE", &LteRfcaTimeConfig.RX_WAIT.rfPreExec },
{ "LTE.RX_WAIT.TUNE", &LteRfcaTimeConfig.RX_WAIT.rfTune },
{ "LTE.TX_DBM.EXECUTE", &LteRfcaTimeConfig.TX_DBM.rfExec },
{ "TDSCDMA.RX_IQ.CONFIG", &TdsCdmaRfcaTimeConfig.RX_IQ.rfConfig },
{ "TDSCDMA.RX_IQ.EXECUTE", &TdsCdmaRfcaTimeConfig.RX_IQ.rfExec },
{ "TDSCDMA.RX_IQ.PREEXECUTE", &TdsCdmaRfcaTimeConfig.RX_IQ.rfPreExec },
{ "TDSCDMA.RX_IQ.TEARDOWN", &TdsCdmaRfcaTimeConfig.RX_IQ.rfTeardown },
{ "TDSCDMA.RX_IQ.TUNE", &TdsCdmaRfcaTimeConfig.RX_IQ.rfTune },
{ "TDSCDMA.RX_RSSI.CONFIG", &TdsCdmaRfcaTimeConfig.RX_RSSI.rfConfig },
{ "TDSCDMA.RX_RSSI.EXECUTE", &TdsCdmaRfcaTimeConfig.RX_RSSI.rfExec },
{ "TDSCDMA.RX_RSSI.PREEXECUTE", &TdsCdmaRfcaTimeConfig.RX_RSSI.rfPreExec },
{ "TDSCDMA.RX_RSSI.TEARDOWN", &TdsCdmaRfcaTimeConfig.RX_RSSI.rfTeardown },
{ "TDSCDMA.RX_RSSI.TUNE", &TdsCdmaRfcaTimeConfig.RX_RSSI.rfTune },
{ "TDSCDMA.RX_WAIT.CONFIG", &TdsCdmaRfcaTimeConfig.RX_WAIT.rfConfig },
{ "TDSCDMA.RX_WAIT.PREEXECUTE", &TdsCdmaRfcaTimeConfig.RX_WAIT.rfPreExec },
{ "TDSCDMA.RX_WAIT.TEARDOWN", &TdsCdmaRfcaTimeConfig.RX_WAIT.rfTeardown },
{ "TDSCDMA.RX_WAIT.TUNE", &TdsCdmaRfcaTimeConfig.RX_WAIT.rfTune },
{ "TDSCDMA.TX_DBM.EXECUTE", &TdsCdmaRfcaTimeConfig.TX_DBM.rfExec },
{ "WCDMA.RX_IQ.CONFIG", &WcdmaRfcaTimeConfig.RX_IQ.rfConfig },
{ "WCDMA.RX_IQ.PREEXECUTE", &WcdmaRfcaTimeConfig.RX_IQ.rfPreExec },
{ "WCDMA.RX_IQ.TUNE", &WcdmaRfcaTimeConfig.RX_IQ.rfTune },
{ "WCDMA.RX_RSSI.CONFIG", &WcdmaRfcaTimeConfig.RX_RSSI.rfConfig },
{ "WCDMA.RX_RSSI.PREEXECUTE", &WcdmaRfcaTimeConfig.RX_RSSI.rfPreExec },
{ "WCDMA.RX_RSSI.TUNE", &WcdmaRfcaTimeConfig.RX_RSSI.rfTune },
{ "WCDMA.RX_WAIT.CONFIG", &WcdmaRfcaTimeConfig.RX_WAIT.rfConfig },
{ "WCDMA.RX_WAIT.PREEXECUTE", &WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec },
{ "WCDMA.RX_WAIT.TUNE", &WcdmaRfcaTimeConfig.RX_WAIT.rfTune },
};

int ConfigKeyCmp( const void *a, const void *b) 
{

  struct ConfigKeyToValueTarget *keyA = (struct ConfigKeyToValueTarget *)a;
  struct ConfigKeyToValueTarget *keyB = (struct ConfigKeyToValueTarget *)b;
  return strcmp(keyA->configKey, keyB->configKey);
}
#include <ctype.h>
uint32* GetConfigTarget(char* configKey)
{
  char* configKeyCp=configKey;  
  // We know that there are no leading spaces and key cannot have space 
  // Look up until fist space
  while((NULL != configKeyCp) && (0 != *configKeyCp) && ('=' != *configKeyCp) && (!isspace(*configKeyCp))) configKeyCp++;
  //
  if((NULL == configKeyCp) || (0 == *configKeyCp))
  {
    return NULL;
  }
  char oldChar = *configKeyCp;
  *configKeyCp = 0;
  struct ConfigKeyToValueTarget toLookFor = {configKey, NULL};
  struct ConfigKeyToValueTarget *whatWeFound = (ConfigKeyToValueTarget *)bsearch(&toLookFor, configMap, sizeof(configMap)/sizeof(configMap[0]), sizeof(struct ConfigKeyToValueTarget), ConfigKeyCmp);
  *configKeyCp = oldChar;
  if(NULL != whatWeFound )
  {
    return whatWeFound->valueTarget;
  }
  else
  {
    return NULL;
  }

}

extern "C" 
{
  #include "fs_lib.h"
  #include "fs_public.h"
}
#include "rfca_mem_helper.h"

bool CheckConfigKey(char *key)
{
   if('#' != *key)
   {
      uint32 *configTarget = NULL;
      if(NULL != (configTarget = GetConfigTarget(key)))
      {
         char* cp2 = key;
         while(isspace(*cp2)) cp2++;
         while( (0 != *cp2) && ('=' != *cp2)) cp2++;
         if('=' != *cp2)
         {
            HalLogWrite("%s: MALFORMED, missing '='", key);
            return false;
         }
         else
         {
            cp2++;
            while(isspace(*cp2)) cp2++;
            if(!isdigit(*cp2))
            {
               HalLogWrite("%s: MALFORMED, missing or invalid number", key);
               return false;
            }
            // At least one digit after '=', rest we do not care
            *configTarget=atoi(cp2);
            HalLogWrite("%s: OK (%i)", key, *configTarget);
            return true;
         }
      }
      else
      {
         HalLogWrite("%s: UNSUPPORTED", key);
         return false;
      }
   }
   return true;
}

bool ParseConfigData(char* configData)
{

   bool status = true;

   if((NULL == configData) || (0 == *configData))
   {
      HalLogWrite("Timing config string is empty");    
      return false;
   }

   char* lineStart = NULL;
   char* cp = configData;

   while(isspace(*cp)) cp++;
   lineStart = cp;

   if(0 != *cp)
   {
      bool noEndingLf = true; // Use this to indicate that last entry did not have LF
      while(0 != *cp)
      {
         if(('\n' == *cp) || ('\r' == *cp))
         {
            *cp = 0;
            status = CheckConfigKey(lineStart);
            cp++;
            while(isspace(*cp)) cp++;
            lineStart = cp;
            if(0 == *cp) {
               noEndingLf = false;
            }
         }
         else
         {
            cp++;
         }
      }
      if(true == noEndingLf)
      {
         status = CheckConfigKey(lineStart);
      }
   }
   else
   {
      HalLogWrite("Timing config was a blank line");    
      return false;
   }
   return status;
}

bool RFCA_RefreshConfigurationData(void)
{
  const int configDataMaxSize = 2048;
  char configData[configDataMaxSize] = {0};
  timetick_type startTime;
  struct fs_stat temp_buf;
  bool status = true;

  HalLogWrite("RFCA_RefreshConfigurationData:BEGIN");
  startTime = timetick_get( );

  if(-1 == efs_stat(RFCA_CONFIG_FILE, &temp_buf))
  {
    // If file does not exist it is considered success
    // and is assumed that file was intentionally left out
    SetConfigHasBeenLoaded(true);
    HalLogWrite("RFCA_RefreshConfigurationData:END:%u", timetick_get_elapsed(startTime, T_USEC));  
    return true;
  }
  //HalLogWrite("Config file exists, size is %d", temp_buf.st_size);

  // Read file to buffer  
  if(-1 == efs_get(RFCA_CONFIG_FILE, configData, temp_buf.st_size))
  {
    HalLogWrite("Failed to load RFCA config file: efs_errno = %d", efs_errno);
    HalLogWrite("RFCA_RefreshConfigurationData:END:%u", timetick_get_elapsed(startTime, T_USEC));  
    return false;
  }

  status = ParseConfigData(configData);

  if(false == status)
  {
    HalLogWrite("Failed to parse RFCA timing config");
  }

  SetConfigHasBeenLoaded(true);
  HalLogWrite("RFCA_RefreshConfigurationData:END:%u", timetick_get_elapsed(startTime, T_USEC));  
  return status;
}

/*----------------------------------------------------------------------------*/
void hex_dump(void *data, int size)
{
   /* 
   * [0000] 75 6E 6B 6E 6F 77 6E 20 30 FF 00 00 00 00 39 00 unknown 0.....9.
   */
   char buffer[MAX_HAL_LOG_BUFFER_SZIE];
   buffer[0]=0;
   unsigned char *p = (unsigned char *)data;
   unsigned char c;
   int n;

   const size_t byteStrMaxLength = 4;
   const size_t addrStrMaxLength = 10;
   const size_t hexStrMaxLength = 512;
   const size_t charStrMaxLength = 512;
   const size_t tempBufferMaxLength = 1024;

   char bytestr[byteStrMaxLength] = {0};
   char addrstr[addrStrMaxLength] = {0};
   char hexstr[hexStrMaxLength] = {0};
   char charstr[charStrMaxLength] = {0};

    // Just to make sure we don't overflow buffer
    if(MAX_HAL_LOG_BUFFER_SZIE/5 < size) {
      size = MAX_HAL_LOG_BUFFER_SZIE/5;
    }
   for(n=1;n<=size;n++) {
      if (n%16 == 1) {
         /* store address for this line */
        snprintf(addrstr, addrStrMaxLength, "%.4x", ((unsigned int)p-(unsigned int)data) );
      }

      c = *p;
      //if (isalnum(c) == 0) {
      if (( (32 > c) || '%' == c) ) {
         c = '.';
      }

      /* store hex str (for left side) */
      snprintf(bytestr, byteStrMaxLength, "%02X ", *p);
      strlcat(hexstr,  bytestr, hexStrMaxLength);

      /* store char str (for right side) */
      snprintf(bytestr, byteStrMaxLength, "%c", c);
      strlcat(charstr, bytestr, charStrMaxLength);

      if(n%16 == 0) {
         /* line completed */
         char temp[tempBufferMaxLength];
         snprintf(temp, tempBufferMaxLength, "[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
         strlcat(buffer, temp, MAX_HAL_LOG_BUFFER_SZIE);
         HalLogWrite("%s", buffer);
         buffer[0]=0;
         hexstr[0] = 0;
         charstr[0] = 0;
      } else if(n%8 == 0) {
         /* half line: add whitespaces */
         strlcat(hexstr, "  ", hexStrMaxLength);
         strlcat(charstr, " ", charStrMaxLength);
      }
      p++; /* next byte */
   }

   if (0 != hexstr[0] ) {
      /* print rest of buffer if not empty */
      char temp[tempBufferMaxLength];
      snprintf(temp, tempBufferMaxLength, "[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
      strlcat(buffer, temp, MAX_HAL_LOG_BUFFER_SZIE);
      HalLogWrite("%s", buffer);
   }
}

