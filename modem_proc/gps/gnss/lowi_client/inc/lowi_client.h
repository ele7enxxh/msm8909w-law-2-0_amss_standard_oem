#ifndef __LOWI_CLIENT_H__
#define __LOWI_CLIENT_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Client Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIClient

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <base_util/postcard.h>
#include <base_util/sync.h>
#include <inc/lowi_const.h>
#include <inc/lowi_request.h>
#include <inc/lowi_response.h>
#include <mq_client/mq_client.h>
#else
#include "lowi_const.h"
#include "lowi_request.h"
#include "lowi_response.h"
#endif
namespace qc_loc_fw
{

#if APSS
// Forward declaration
class LOWIClientReceiver;
#else
class LOWIQMIWrapper;
#endif

/**
 * This class should be sub classed by the client of LOWI to receive
 * responses to the requests made through LOWIClient.
 */
class LOWIClientListener
{
public:
  /**
   *  Notifies the listener whenever the Response is received
   *  for a Request sent by the client.
   *
   *  NOTE: The notification happens in a different thread as compared
   *  to the thread in which the request was made by the client.
   *
   *  @param LOWIResponse* Response received
   *
   *  @usage Client upon receiving this callback should inspect the type
   *          of response by response->getResponseType () and then type cast
   *          the response to retrieve the response data.
   *          i.e. Sample usage of a Capabilities response is
   *          if (response->getResponseType () == LOWIResponse.CAPABILITY)
   *          {
   *            LOWICapabilities* cap =
   *             ((LOWICapabilityResponse)response)->getCapabilities();
   *          }
   */
  virtual void responseReceived (LOWIResponse* response) = 0;

#if !APSS
  /**
   * Notifies the listener with the error if any.
   * Generally this is an error indicating the communication
   * with the LOWI QMI service is lost and client should
   * restart the LOWIClient.
   */
  virtual void errorReceived (LOWIResponse::eScanStatus) = 0;
#endif

  /** Destructor */
  virtual ~LOWIClientListener () = 0;
};

/**
 * API for requesting scans using LOWIClient
 */
#if APSS
class LOWIClient : public MessageQueueServiceCallback
#else
class LOWIClient
#endif
{
private:
  static const char * const   TAG;
#if APSS
  LOWIClientListener* const   mLOWIClientListener;
  LOWIClientReceiver*         mLOWIClientReceiver;
  MessageQueueClient*         mConn;
  BlockingQueue *             mLocalMsgQueue;
  char*                       mClientName;
#else
  LOWIQMIWrapper*             mLowiQmiWrapper;
#endif
  /**
   * Constructor
   * @param LOWIClientListener* Pointer to the LOWIClientListener
   *                              which will be notified when the
   *                              response is received.
   */
  LOWIClient(LOWIClientListener* const ptr);

  /**
   * It is a blocking call until the Instance is created and registered
   * with the underlying messaging hub (IPC hub on APSS / LOWI QMI
   * service on MPSS and LPASS)
   * NOTE: On MPSS the blocking call uses the following signal masks
   *
   * C_OS_FLAG_QMI_EVENT
   *
   * It is a blocking call until the initialization completes or fails.
   * Client must init the LOWIClient before issuing requests.
   * @param timeout_ms Init timeout with failure or success in msec
   *        default (0) - it is going to wait forever
   *        The timeout is not applicable to the APSS clients.
   *
   * @return bool true - success, false - failure
   */
  bool init (uint32 timeout_ms = 0);

  // private copy constructor and assignment operator so that the
  // the instance can not be copied.
  LOWIClient( const LOWIClient& rhs );
  LOWIClient& operator=( const LOWIClient& rhs );

public:
  /**
   * Defines the status of the Request.
   */
  enum eRequestStatus
  {
    /** Request is successfully sent to LOWI*/
    STATUS_OK,
    /** Error due to bad parameters*/
    BAD_PARAMS,
    /** Failure to send request to LOWI*/
    SEND_FAILURE
  };

  /**
   * Defines the log level.
   */
  enum eLogLevel
  {
    /** No logging*/
    LL_LOG_OFF = 0,
    /** Error level*/
    LL_ERROR = 1,
    /** Warning level*/
    LL_WARNING = 2,
    /** Info level*/
    LL_INFO = 3,
    /** Debug level*/
    LL_DEBUG = 4,
    /** Verbose level*/
    LL_VERBOSE = 5,
    /** Log all*/
    LL_LOG_ALL = 100
  };

  /**
   * Creates instance of LOWIClient and initializes it.
   * It is a blocking call until the Instance is created and registered
   * with the underlying messaging hub (IPC hub on APSS / LOWI QMI
   * service on MPSS and LPASS)
   * @param LOWIClientListener* Pointer to the LOWIClientListener
   *                              which will be notified when the
   *                              response is received.
   * @param bool     bool To enable / disable logging(optional).
   * @param eLogLevel Log level at which the library should be logging.
   *                  This will only take effect if the logging is
   *                  enabled in previous parameter(optional).
   * @param timeout_ms Init timeout with failure or success in msec
   *        default (0) - it is going to wait forever
   *        The timeout is not applicable to the APSS clients.
   */
  static LOWIClient* createInstance (LOWIClientListener* const ptr,
      bool enableLogging = false, eLogLevel log_level = LL_INFO,
      uint32 timeout_ms = 0);

  /** Destructor*/
  virtual ~LOWIClient();

  /**
   * Sends the Request created by the client.
   * @param   LOWIRequest* Request to be processed
   * @return  eReturnSatus  Return status of the request
   */
  eRequestStatus sendRequest (LOWIRequest* const request);

#if APSS
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
  static OutPostcard* composePostCard (LOWIRequest* const request,
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
  static LOWIResponse* parsePostCard (InPostcard* const postcard);

private:
  /**
   * Handles Response. Called by LOWIClientReceiver thread in it's own thread
   * context.
   * @param InMemoryStream*  Message received from the IPC hub
   */
  int newMsg(qc_loc_fw::InMemoryStream * new_buffer);
#endif
};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_CLIENT_H__
