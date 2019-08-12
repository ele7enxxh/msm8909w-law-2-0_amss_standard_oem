#ifndef __LOWI_SCAN_MEASUREMENT_H__
#define __LOWI_SCAN_MEASUREMENT_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Scan Measurement Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIScanMeasurement

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <inc/lowi_request.h>
#include <inc/lowi_ssid.h>
#include <inc/lowi_mac_address.h>
#else
#include "lowi_request.h"
#include "lowi_ssid.h"
#include "lowi_mac_address.h"
#include "vector.h"
#endif

namespace qc_loc_fw
{

/**
 * This struct defines the MSAP related data.
 */
struct LOWIMsapInfo
{
  uint8 protocolVersion;
  uint32        venueHash;
  uint8 serverIdx;
};

/**
 * This struct defines Measurement Info per Wifi Node.
 * For a DiscoveryScan only rssi and rssi_timestamp are valid fields
 * For a Ranging scan rtt, rtt_timestamp and rssi are valid fields.
 */
struct LOWIMeasurementInfo
{
  /**
   * Measurement age msec
   * In units of 1 msec, -1 means info not available
   */
  int32         meas_age;
  /** RTT - value in nsec. 0 is considered to be invalid rtt*/
  int32         rtt;
  /* For RangingScanResponse
   * time stamp is an averaged time stamp, if there are multiple
   * measurements for a Wifi Node.
   */
  int64         rtt_timestamp;
  /** Measurement time stamp.
   * For DiscoveryScanResponse
   *  time stamp is corresponding to when the beacon was received.
   */
  int64         rssi_timestamp;
  /** Signal strength in 0.5dBm*/
  int16         rssi;
};

/**
 * This class defines the measurement taken for every scan request.
 * This contains the measurements corresponding the discovery / ranging
 * scan requests. However, the fields are valid / invalid based on type
 * of scan as documented below.
 */
class LOWIScanMeasurement
{
private:
  static const char* const TAG;
public:
  /** BSSID of the wifi node*/
  LOWIMacAddress    bssid;
  /** Frequency at which the node is detected*/
  uint32          frequency;
  /** Secure access point or not. Only valid for DiscoveryScanResponse*/
  bool            isSecure;
  /** Type of the Wifi Node. Only valid for DiscoveryScanResponse*/
  eNodeType       type;
  /** SSID. Only valid for DiscoveryScanResponse*/
  LOWISsid        ssid;
  /** MsapInfo - valid if not NULL. Only valid for DiscoveryScanResponse*/
  LOWIMsapInfo*   msapInfo;
  /** Cell power limit in dBm. Only valid for discovery scan results,
   *  if available. For ranging scan results will be always 0. */
  int8            cellPowerLimitdBm;
  /**
   * Dynamic array containing measurement info per Wifi node.
   * DiscoveryScan will only have one measurements where as the
   * the vector can contain multiple MeasurementInfo for a Ranging scan.
   */
  vector <LOWIMeasurementInfo*> measurementsInfo;

  /** Constructor*/
  LOWIScanMeasurement ();
  /** Destructor*/
  ~LOWIScanMeasurement ();

  /** Copy constructor **/
  LOWIScanMeasurement( const LOWIScanMeasurement& rhs );
  /** Assignment operator **/
  LOWIScanMeasurement& operator=( const LOWIScanMeasurement& rhs );
};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_SCAN_MEASUREMENT_H__
