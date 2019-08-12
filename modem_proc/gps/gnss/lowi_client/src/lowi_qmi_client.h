#ifndef __LOWI_QMI_CLIENT_H__
#define __LOWI_QMI_CLIENT_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Qmi Client Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWI QMI client

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
=============================================================================*/
#if APSS
#include <base_util/sync.h>
#include <inc/lowi_request.h>
#include <inc/lowi_response.h>
#include <lowi_server/lowi_measurement_result.h>
#include <lowi_server/lowi_scan_result_receiver.h>
#include "qmi_client.h"
#include <lowi_server/lowi_service_v01.h>
#else
#include "qmi_client.h"
#include "lowi_service_v01.h"
#endif
namespace qc_loc_fw
{

/**
 * Interface to be implemented by the clients who wish to receive the
 * responses and indications for the requests sent using LOWIQMIClient
 * All the notifications are provided in QCCI thread context.
 */
class LOWIQMIClientListenerInterface
{
public:
  /**
   * Notifies acknowledgment to a request
   * @param QmiLOWIGenResp_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyAckResponse (QmiLOWIGenResp_v01* response,
      uint32 msg_id) = 0;

  /**
   * Notifies Discovery Scan response
   * @param QmiLowiDiscoveryResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (QmiLowiDiscoveryResponseInd_v01* response,
      uint32 msg_id) = 0;

  /**
   * Notifies Capabilities response
   * @param QmiLowiCapabilitiesResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiCapabilitiesResponseInd_v01* response,
      uint32 msg_id) = 0;

  /**
   * Notifies Wlan State Events response
   * @param QmiLowiWlanInterfaceEventsResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiWlanInterfaceEventsResponseInd_v01* response,
      uint32 msg_id) = 0;

  /**
   * Notifies Error in QMI messaging with the service
   * @param qmi_client_error_type Error code.
   */
  virtual void notifyError (qmi_client_error_type error) = 0;

  /**
   * Pure virtual destructor.
   */
  virtual ~LOWIQMIClientListenerInterface ()
  {

  }
};

/**
 * This class provides the mechanism to send the QMI messages to
 * LOWI_LP QMI service and get the results. It then posts the events
 * back to LOWIController
 */
class LOWIQMIClient
{
public:

  /**
   *  Supported LOWI QMI service version.
   */
  enum eSupportedVersion
  {
    VERSION_01
  };

private:
  static const char * const         TAG;
  LOWIQMIClientListenerInterface*   mListener;
  bool                              mNewRequestArrived;
  qmi_client_type                   mClnt;
  qmi_client_type                   mNotifier;
  eSupportedVersion                 mVersion;
  qmi_txn_handle                    mTxn;
  qmi_cci_os_signal_type            mOsParams;

  /**
   * Terminates the Receiver thread.
   *
   * @return true if success, false otherwise
   */
//  bool terminate ();

  /**
   * Unblocks the Receiver thread if it is blocked
   * in passive listening mode.
   *
   * @return true if success, false otherwise
   */
//  bool unblockThread ();

protected:

  /**
   * Main loop of the Receiver
   * Blocks on the nl socket to receive the scan measurements
   * from the wifi driver.
   * if results received
   *  Allocate memory on heap to hold the measurement results
   *  Notify the registered listener with the measurement results
   * continue to listen on the socket
   */
//  virtual void run ();

public:

  /**
   * This callback function is called by the QCCI infrastructure when
   * infrastructure receives an indication for this client.
   * NOTE: Callback happens in the QCCI thread context and raises signal.
   * @param[in]  user_handle         Opaque handle used by the infrastructure to
   *                                identify different services.
   * @param[in]   msg_id              Message ID of the indication
   * @param[in]  ind_buf              Buffer holding the encoded indication
   * @param[in]  ind_buf_len          Length of the encoded indication
   * @param[in]  ind_cb_data          Cookie value supplied by the client during registration
   */
  static void indicationCallback (qmi_client_type user_handle,
                                  unsigned int msg_id,
                                  void* ind_buf,
                                  unsigned int ind_buf_len,
                                  void* ind_cb_data);

  /**
   * This callback function is called by the QCCI infrastructure when
   * infrastructure receives an asynchronous response for this client
   * NOTE: Callback happens in the QCCI thread context and raises signal.
   * @param[in]   user_handle         Opaque handle used by the infrastructure to
   *                                  identify different services.
   * @param[in]   msg_id              Message ID of the response
   * @param[in]   buf                 Buffer holding the decoded response
   * @param[in]   len                 Length of the decoded response
   * @param[in]   resp_cb_data        Cookie value supplied by the client
   * @param[in]   transp_err          Error value
   */
  static void responseCallback (qmi_client_type user_handle,
                                unsigned int msg_id,
                                void* buf,
                                unsigned int len,
                                void* resp_cb_data,
                                qmi_client_error_type transp_err);

  /**
  * This callback function is called by the QCCI infrastructure when the service
  * terminates or deregisters.
  * NOTE: Callback happens in the QCCI thread context and raises signal.
  * @param[in]   user_handle        Handle used by the infrastructure to
  *                              identify different clients.
  * @param[in]   error              Error code
  * @param[in]   err_cb_data        User-data
  */
  static void errorCallback (qmi_client_type user_handle,
                             qmi_client_error_type error,
                             void *err_cb_data);
#if APSS // Not needed
  /**
   * Executes LOWIRequest.
   * @param LOWIRequest*               LOWIRequest that needs to be
   *                                   sent over QMI
   * @return bool true is success, false otherwise
   */
  virtual bool execute (LOWIRequest* request);
#endif
  /**
   * Sends Discovery scan request to LOWI QMI Service
   * Response contains the discovery scan results
   * @param LOWIDiscoveryScanRequest_v01*  Discovery scan request that
   *                                   needs to be sent over QMI
   * @param[out] QMI error code       Only valid if the return value is true
   * @return bool true if there was an attempt to send the request over QMI
   */
  virtual bool sendRequest (
      QmiLowiDiscoveryRequest_v01* request, qmi_client_error_type* err);

  /**
   * Sends WLAN Events Subscription request to WLAN QMI Service
   * The response to this request is just an acknowledgment followed
   * bunch of indications containing WLAN events
   * @param QmiLowiWlanInterfaceEventsSubscriptionRequest_v01* LOWIRequest that
   *                                     needs to be sent over QMI
   * @param[out] QMI error code       Only valid if the return value is true
   * @return bool true if there was an attempt to send the request over QMI
   */
  virtual bool sendRequest (
      QmiLowiWlanInterfaceEventsSubscriptionRequest_v01* request,
      qmi_client_error_type* err);

  /**
   * Sends WLAN Events query request to WLAN QMI Service
   * The response to this query contains the WLAN state.
   * @param QmiLowiWlanInterfaceEventTriggerRequest_v01* LOWIRequest that
   *                                     needs to be sent over QMI
   * @param[out] QMI error code       Only valid if the return value is true
   * @return bool true if there was an attempt to send the request over QMI
   */
  virtual bool sendRequest (
      QmiLowiWlanInterfaceEventTriggerRequest_v01* request,
      qmi_client_error_type* err);

  /**
   * Sends Snoop Scan Subscription request to WLAN QMI Service
   * The response to this request is just an acknowledgment followed
   * bunch of indications containing WLAN events
   * @param QmiLowiSnoopScanSubscriptionRequest_v01* LOWIRequest that needs
   *                                              to be sent over QMI
   * @param[out] QMI error code       Only valid if the return value is true
   * @return bool true if there was an attempt to send the request over QMI
   */
  virtual bool sendRequest (
      QmiLowiSnoopScanSubscriptionRequest_v01* request,
      qmi_client_error_type* err);

  /**
   * Sends Capability request to WLAN QMI Service
   * Response contains the capabilities
   * @param QmiLowiCapabilitiesRequest_v01* LOWIRequest that needs to be sent
   *                                   over QMI
   * @param[out] QMI error code       Only valid if the return value is true
   * @return bool true if there was an attempt to send the request over QMI
   */
  virtual bool sendRequest (
      QmiLowiCapabilitiesRequest_v01* request,
      qmi_client_error_type* err);

  /**
   * Initializer.
   * It's a blocking call and blocks the thread until
   * initialization completes.
   * This will be blocked until a valid QMI Service is detected
   * or time out.
   * NOTE: Must be called before sending any request.
   * @param uint32 Timeout in milliseconds. This is the max time
   *                LOWIQMIClient will wait for the service to come up.
   *                0 indicates wait forever.
   * @param eSupportedVersion Supported service version on client side.
   *                          Any supported version can be chosen by the
   *                          client. LOWI QMI service might be supporting
   *                          an advanced version but the client will use
   *                          the specified version to communicate with the
   *                          service.
   */
  virtual bool init (uint32 timeout_ms, eSupportedVersion version);

  /**
   * Constructor
   * @param LOWIQMIClientListenerInterface* listener to be notified
   *                                   with measurement results.
   */
  LOWIQMIClient(LOWIQMIClientListenerInterface* listener);

  /** Destructor*/
  virtual ~LOWIQMIClient();

};
} // namespace
#endif //#ifndef __LOWI_QMI_CLIENT_H__
