#ifndef __LOWI_UTILS_H__
#define __LOWI_UTILS_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Utils Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWI Utils

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <base_util/postcard.h>
#include <inc/lowi_const.h>
#include <inc/lowi_request.h>
#include <inc/lowi_response.h>
#include <inc/lowi_scan_measurement.h>
#else
#include "comdef.h"
#include "lowi_request.h"
#include "lowi_client.h"
#include "lowi_log.h"
#endif

namespace qc_loc_fw
{

/**
 * Utility Class
 */
class LOWIUtils
{
private:
  static const char * const TAG;

public:
#if APSS
  /**
   * Parses the Scan Measurements from the InPostCard
   * @param InPostcard* InPostCard from which the scan measurements
   *                    to be parsed
   * @param  [in] vector<LOWIScanMeasurement*>& Measurement vector for all
   *                                       measurements.
   * @return true - success, false - otherwise
   */
  static bool parseScanMeasurements
  (InPostcard* const postcard, vector <LOWIScanMeasurement*> & scan);

  /**
   * Parses the Measurements Info from the InPostCard
   * @param InPostcard* InPostcard from which the measurement info to be parsed
   * @param  [in] vector<LOWIMeasurementInfo*>& Measurement info
   *                                            for all measurements.
   * @return true - success, false - otherwise
   */
  static bool parseMeasurementInfo
  (InPostcard* const card, vector <LOWIMeasurementInfo*>& meas_info);

  /**
   * Composes an OutPostCard from the Request created by the client.
   *
   * This API is intended for the clients that have there own socket based IPC
   * implementation and are registered with the IPC hub. Such clients just need
   * to call this API to convert the Request to an OutPostCard which could then
   * be sent to the IPC Hub by the client. The recipient field of the Postcard
   * is populated by this API.
   *
   * Note: Memory should be deallocated by the client
   *
   * @param LOWIRequest* Request to be converted to an OutPostCard
   * @param char* ID of the originator which will be added to the OutPostCard
   * @return OutPostCard
   */
  static OutPostcard* requestToOutPostcard (LOWIRequest* const request,
      const char* const originatorId);

  /**
   * Parses an InPostCard and generates the Response needed by the client.
   *
   * This API is intended for the clients that have there own socket based IPC
   * implementation and are registered with the IPC hub. Such clients communicate
   * with the uWifiPsoAPI process through IPC hub to send and receive Postcards
   * on there own and need this API to parse the InPostCard into a Response.
   *
   * Note: Memory should be deallocated by the client
   *
   * @param InPostcard* InPostcard to be parsed
   * @return LOWIResponse
   */
  static LOWIResponse* inPostcardToResponse (InPostcard* const postcard);

  /**
   * Creates a Request from a InPostcard
   * Used by the LOWI server to parse the InPostcard and create a Request
   * @param InPostcard* Postcard
   * @return LOWIRequest
   */
  static LOWIRequest* inPostcardToRequest (InPostcard* const card);

  /**
   * Creates a OutPostcard from the response.
   * Used by the LOWI server to create a OutPostcard to be sent to the Hub.
   * @param LOWIResponse* Response for which the Postcard is to be created
   * @param char* ID of the receiver of this postcard
   */
  static OutPostcard* responseToOutPostcard (LOWIResponse* resp,
      const char* to);

  /**
   * Injects the MeasurementInfo into the Post card
   * @param OutPostcard Card to be filled with Measurement Info
   * @param vector <LOWIMeasurementInfo*> Measurements container from where
   *        the measurement info is to be extracted
   * @return true - success, false otherwise
   */
  static bool injectMeasurementInfo (OutPostcard & card,
      vector <LOWIMeasurementInfo*> & info);

  /**
   * Injects the ScanMeasurements into the Postcard
   * @param OutPostcard Card to be filled with Scan measurements
   * @param vector <ScanMeasurement*> Scan Measurements
   * @return true - success, false otherwise
   */
  static bool injectScanMeasurements (OutPostcard & card,
      vector <LOWIScanMeasurement*> & meas);

  static LOWIResponse::eResponseType to_eResponseType (int a);
  static LOWIResponse::eScanStatus to_eScanStatus (int a);
  static LOWIDiscoveryScanResponse::eScanTypeResponse to_eScanTypeResponse
  (int a);
  static eNodeType to_eNodeType (int a);
  static eRttType to_eRttType (unsigned char a);
  static eRttReportType to_eRttReportType (unsigned char a);
  static LOWIDiscoveryScanRequest::eBand to_eBand (int a);
  static LOWIDiscoveryScanRequest::eScanType to_eScanType (int a);
  static LOWIDiscoveryScanRequest::eRequestMode to_eRequestMode (int a);
  static qc_loc_fw::ERROR_LEVEL to_logLevel (int a);

  /**
   * Function description:
   *    This function will return current time in number of milli-seconds
   *    since Epoch (00:00:00 UTC, January 1, 1970Jan 1st, 1970).
   *
   * Parameters:
   *    none
   *
   * Return value:
   *    number of milli-seconds since epoch.
   */
  static int64 currentTimeMs ();
#endif
  /**
   * Returns Channel corresponding the frequency
   * @param uint32 Frequency which can be in 2.4 GHz or 5 Ghz band
   * @return 0 for frequency out of 2.4 / 5 Ghz band
   *         Valid channel number otherwise
   */
  static uint32 freqToChannel (uint32 freq);

  /**
   * Returns the band for the frequency passed.
   * @param uint32 Frequency in 2.4 / 5 Ghz band
   * @return associated band
   */
  static LOWIDiscoveryScanRequest::eBand freqToBand( uint32 freq );

  /**
   * Returns the frequency for the band and channel passed.
   * @param uint32 Channel number
   * @param LOWIDiscoveryScanRequest::eBand Band of the channel
   * @return Frequency
   */
  static uint32 channelBandToFreq (uint32 channel,
      LOWIDiscoveryScanRequest::eBand band);

  /**
   * Get channels or frequency's corresponding to the band
   * @param LOWIDiscoveryScanRequest::eBand Band
   * @param unsigned char Num of channels
   * @param bool flag to indicate if freqency's or channels needed.
   * @return Pointer to the channels array
   */
  static int * getChannelsOrFreqs (LOWIDiscoveryScanRequest::eBand,
      unsigned char & channels, bool freq);

  /**
   * Get channels or freqency's corresponding to the ChannelInfo
   * @param vector<LOWIChannelInfo> & Channels
   * @param unsigned char Num of channels
   * @param bool flag to indicate if freqency's or channels needed.
   * @return Pointer to the array of channels
   */
  static int * getChannelsOrFreqs (vector<LOWIChannelInfo> & v,
      unsigned char & channels, bool freq);

  static qc_loc_fw::ERROR_LEVEL to_logLevel (LOWIClient::eLogLevel level);

};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_UTILS_H__
