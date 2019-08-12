/*!
  @file
  rfca_helper.h
 
  @brief
  Misc helper routines for RFCA. 

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_helper.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/30/14   aki     Add log message support in off-target
01/23/14   aki     RFCA debug mode as run-time option and move hex_dump helper here
11/14/13   aki     Added GSM_CalculateFrequency
06/10/13   aki     Mainline RFCA 
06/07/13   aki     Removed RFCA_WriteRFNV as it is not used
03/25/13   aki     Added ParseConfigData() and moved RFCA_CONFIG_FILE here
10/31/12   aki     Added RFCA_RefreshConfigurationData
09/04/12   aki     New functions to calculate LTE & TDSCDMA frequencies
07/13/12   aki     Removed RFCA_ModeToFtmModeId
05/25/12   aki     Added LTE_CalculateChannel
04/23/12   aki     Channel and freq support functions
02/24/12   aki     HalLogWrite turned to a macro, doc updates
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_HELPER_H_
#define _RFCA_HELPER_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_defs.h"

// Common Helper Functions and Data Structures


#include "rfca_pending_message_queue.h"


/*! Location of timing config file in EFS */
#define RFCA_CONFIG_FILE "/nv/item_files/factory/rfca/rfca_timing_config.txt"

/*----------------------------------------------------------------------------*/
/*!  
  @brief Sends all queued FTM messages to FTM sequencer.

  @param [in] rfcaResponseItem  Pointer to RFCA item to take FTM messages from

  @return 0 if it succeeds, non-zero if it fails.
*/
int RFCA_SendFtmPackagesToSequencer(PendingRfcaResponse *rfcaResponseItem);

/*----------------------------------------------------------------------------*/
/*!
  @brief Convert RFCA technology and band to FTM RF mode type.
  
  @details In RFCA technology and RF band are separate entried where as in FTM
           they are combined into single entry. This function converts RFCA
           technology and band pair into single value that can be sent to FTM.
  
  @param [in] rfMode        FTM mode for ginven technology and band
  @param [in] technology    RFCA technology identifier
  @param [in] band          RFCA band identifier
  
  @return true if it succeeds, false if it fails.
  
  @roadblock Some CDMA Modes, which is which? Like BC 11?

*/
bool RFCA_ModeToFtmRfMode(unsigned short *rfMode, int technology, int band);

/*----------------------------------------------------------------------------*/
/*!
  @brief Returns the current band for current technology
  
  @param [out] band RFCA band identifier
  
  @todo Another reason to reimplment state cache more OO style...
  
  @return true if it succeeds, false if it fails.

*/
bool RFCA_GetCurrentBand(uint16 *band);

/*----------------------------------------------------------------------------*/
double GSM_CalculateFrequency(HAL_RF_GSM_BAND band,uint16 channel, bool uplink);

/*----------------------------------------------------------------------------*/
/*!
 @brief Calculates UL or DL channel based on the given channel

 @param [in] band              WCDMA band
 @param [in] channel           WCDMA channel, UL or DL, @see inputChannelType
 @param [in] inputChannelType  Indicates the type of @see channel argument

 @return channel number, 0 - fail

*/
uint16 WCDMA_CalculateChannel(HAL_RF_WCDMA_BAND band, uint16 channel, CHANNEL_TYPE inputChannelType);

/*----------------------------------------------------------------------------*/
/*!  
  @brief Calculates frequency based on band and channel

  @param [in] band    WCDMA band
  @param [in] channel WCDMA channel
  @param [in] uplink  Indicated if @see channel is UL channel
 
  @return 0.0 - fail, otherwise the frequency

*/
double WCDMA_CalculateFrequency( HAL_RF_WCDMA_BAND band, long channel, bool uplink );

/*----------------------------------------------------------------------------*/
/*!  
  @brief Calculates frequency based on band and channel

  @param [in] band    TDSCDMA band
  @param [in] channel TDSCDMA channel
  @param [in] uplink  Indicated if @see channel is UL channel
 
  @return 0.0 - fail, otherwise the frequency

*/
double TDSCDMA_CalculateFrequency( HAL_RF_TDSCDMA_BAND band, long channel, bool uplink );

/*----------------------------------------------------------------------------*/
/*!  
  @brief Calculates frequency based on band and channel

  @param [in] band    LTE band
  @param [in] channel LTE channel
  @param [in] uplink  Indicated if @see channel is UL channel (false = RX Freq in QRCT)
 
  @return 0.0 - fail, otherwise the frequency

*/
double LTE_CalculateFrequency( HAL_RF_LTE_BAND band, long channel, bool uplink );
/*----------------------------------------------------------------------------*/
/*!
 @brief Calculates LTE UL or DL channel based on the given channel

 @param [in] band              LTE band
 @param [in] channel           LTE channel, UL or DL, @see inputChannelType
 @param [in] inputChannelType  Indicates the type of @see channel argument

 @return channel number, 0 - fail

*/
uint16 LTE_CalculateChannel(HAL_RF_LTE_BAND band, uint16 channel, CHANNEL_TYPE inputChannelType);

/*----------------------------------------------------------------------------*/
/*!
@brief Reads single RFNV item

@param [in]     id         RFNV item id, 0 - 0xFFFF
@param [out]    buffer     Buffer to copy RFNV data
@param [in,out] bufferSize Size of buffer when in, size of RFNV data when out

@return 0 success
*/
int RFCA_ReadRfNv(word id, uint8 *buffer, uint16 *bufferSize);

/*----------------------------------------------------------------------------*/
/*!
@brief Writes log message

@param [in] format Format string for vsnprintf function

*/
void HalLogWrite_int( const char *format , ... );

/*----------------------------------------------------------------------------*/
/*!
@brief (Re)loads RFCA config file

@return true = success, missing file is considered ok, fail = error
*/
bool RFCA_RefreshConfigurationData(void);



/*----------------------------------------------------------------------------*/
/*!
@brief Parses timing config from a text string
 
@note Modifies configData string as side-effect 
 
@return true = success, fail = error
*/
bool ParseConfigData(char* configData);

/*----------------------------------------------------------------------------*/
/*!
@brief Returns HAL name for id

@param [in] id HAL ID

*/
const char* HalGetMgsName(unsigned short id);

extern bool rfcaDebugMode;
#ifdef _WIN32
  #define HalLogWrite( format , ... ) if (true == rfcaDebugMode) { HalLogWrite_int(format, __VA_ARGS__); } else
#else
  #define HalLogWrite( format , args... ) if (true == rfcaDebugMode) { HalLogWrite_int(format, ##args); } else
#endif
/*----------------------------------------------------------------------------*/
void hex_dump(void *data, int size);


#endif // _RFCA_HELPER_H_

