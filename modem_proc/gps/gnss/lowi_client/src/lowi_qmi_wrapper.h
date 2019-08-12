#ifndef __LOWI_QMI_WRAPPER_H__
#define __LOWI_QMI_WRAPPER_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Low Power Scan Result Receiver Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWI LP Scan Result Receiver

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <base_util/sync.h>
#include <inc/lowi_request.h>
#include <inc/lowi_response.h>
#include <lowi_server/lowi_measurement_result.h>
#include <lowi_server/lowi_scan_result_receiver.h>
#include <lowi_server/lowi_qmi_client.h>
#else
#include "lowi_request.h"
#include "lowi_response.h"
#include "lowi_qmi_client.h"
#endif

namespace qc_loc_fw
{

// Forward declaration
class LOWIClientListener;

/**
 * This class provides the mechanism to get the scan measurements
 * from LOWI LP.
 * It's responsibility is to receive the results and provide the results
 * to it's listener. It's responsibility is to convert the data between
 * LOWI structures to LOWIQMIClient structures and pass the request and
 * responses. Also provides the mechanism to notify the listener when the
 * communication is lost with the QMI service.
 */
class LOWIQMIWrapper : public LOWIQMIClientListenerInterface
{
private:
  static const char * const         TAG;
  LOWIClientListener*               mListener;
  LOWIQMIClient*                    mQMIClient;
  bool                              mQMIClientReady;
  /** Vector containing the scan measurements for each AP */
  vector <LOWIScanMeasurement*>     mScanMeasurements;
  /** Vector containing the scanned frequencies */
  vector <LOWIChannelInfo>          mScannedFreq;

  LOWIResponse::eScanStatus injectScanMeasurements
  (vector <LOWIScanMeasurement*>& result,
      QmiLowiDiscoveryResponseInd_v01* measurements);

  bool injectMeasurementInfo
  (vector <LOWIMeasurementInfo*>& vec,
      sLOWIDiscoveryMeasurementInfo_v01* pAp,
      uint32 num_of_meas);

public:


  /**
   * It is a blocking call until the registeration
   * with the underlying messaging hub (LOWI QMI service)
   * It is a blocking call until the initialization completes or fails.
   * Must be called before issuing requests.
   * @param timeout_ms Init timeout with failure or success in msec
   *        default (0) - it is going to wait forever
   *
   * @return bool true - success, false - failure
   */
  bool init (uint32 timeout_ms);

  /**
   * Sends LOWIRequest.
   * @param LOWIRequest*               LOWIRequest for which the
   *                                   scan is to be performed
   * @return bool true is success, false otherwise
   */
  virtual bool sendRequest (LOWIRequest* request);

  /**
   * Constructor
   * @param LOWIClientListener* listener to be notified
   *                            with measurement results / responses.
   */
  LOWIQMIWrapper(LOWIClientListener* listener);

  /** Destructor*/
  virtual ~LOWIQMIWrapper();

public:
  /**
   * Notifies acknowledgment to a request
   * @param QmiLOWIGenResp_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyAckResponse (QmiLOWIGenResp_v01* response,
      uint32 msg_id);

  /**
   * Notifies Discovery Scan response
   * @param QmiLowiDiscoveryResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (QmiLowiDiscoveryResponseInd_v01* response,
      uint32 msg_id);

  /**
   * Notifies Capabilities response
   * @param QmiLowiCapabilitiesResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiCapabilitiesResponseInd_v01* response,
      uint32 msg_id);

  /**
   * Notifies Wlan State Events response
   * @param QmiLowiWlanInterfaceEventsResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiWlanInterfaceEventsResponseInd_v01* response,
      uint32 msg_id);
  /**
   * Notifies Error in QMI messaging with the service
   * @param Error code.
   */
  virtual void notifyError (qmi_client_error_type error);

};
} // namespace
#endif //#ifndef __LOWI_QMI_WRAPPER_H__
