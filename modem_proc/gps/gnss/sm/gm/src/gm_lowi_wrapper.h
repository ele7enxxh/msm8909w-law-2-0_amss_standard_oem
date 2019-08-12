/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        GM LOWI Wrapper

GENERAL DESCRIPTION
  This file contains the Wrapper around LOWI Client

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
 
  Copyright (c) 2015 Qualcomm Atheros, Inc. All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#ifndef __GM_LOWI_WRAPPER_H__
#define __GM_LOWI_WRAPPER_H__

#ifdef __cplusplus
extern "C"
{
#endif
  /* The different error types returned by LOWI service. */
  typedef enum
  {
    GM_LOWI_ERROR_NONE,
    GM_LOWI_ERROR_ALREADY_REGISTERED,
    GM_LOWI_ERROR_CLIENT_UNINITIAILIZED,
    GM_LOWI_ERROR_GENERAL,
    GM_LOWI_ERROR_MAX
  }gm_lowi_error_e_type;

/*============================================================================================= 
 * gm_lowi_wrapper_init
 * 
 * Function description:
 *   This function initializes the GM LOWI client.
 *   Please note that this function shall be called only once before destroy is called
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_wrapper_init ();

/*============================================================================================= 
 * gm_lowi_wrapper_destroy
 * 
 * Function description:
 *   This function destroys the GM LOWI client.
 *   Please note that this function shall be called once when the procC daemon starts
 *
 * Parameters:
 *    none
 *
 * Return value:
 *    Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_wrapper_destroy ();

/*=============================================================================================
 * gm_lowi_queue_passive_scan_req
 *
 * Description:
 *   This function requests for passive scan on all channels.
 *
 * Parameters: None
 *
 * Return value:
 *   Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_queue_passive_scan_req( void );

/*=============================================================================================
 * gm_lowi_queue_capabilities_req
 *
 * Description:
 *   This function requests for driver capabilities from lowi
 *
 * Parameters: None
 *
 * Return value:
 *   Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_queue_capabilities_req( void );

/*=============================================================================================
 * gm_lowi_queue_async_discovery_scan_result_req
 *
 * Description:
 *   This function requests for async discovery scan results
 *
 * Parameters: uint32 : Timeout in seconds after which the request can be dropped
 *
 * Return value:
 *   Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_queue_async_discovery_scan_result_req( uint32 q_timeout_secs );

/*=============================================================================================
 * gm_lowi_queue_wlan_events_subscription_req
 *
 * Description:
 *   This function is used to request for WLAN events. 
 *
 * Parameters: None
 *
 * Return value:
*  Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_queue_wlan_events_subscription_req (void);

/*=============================================================================================
 * gm_lowi_queue_wlan_state_query_req
 *
 * Description:
 *   This function is used to query the current WLAN state 
 *
 * Parameters: None
 *
 * Return value:
 *  Returns an error of type gm_lowi_error_e_type
 =============================================================================================*/
extern gm_lowi_error_e_type gm_lowi_queue_wlan_state_query_req (void);

#ifdef __cplusplus
}
#endif

#endif /* __GM_LOWI_WRAPPER_H__ */
