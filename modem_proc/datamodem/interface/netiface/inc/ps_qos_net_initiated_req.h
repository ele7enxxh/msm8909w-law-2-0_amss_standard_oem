#ifndef PS_QOS_NET_INITIATED_REQ_H
#define PS_QOS_NET_INITIATED_REQ_H
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           P S _ Q O S _ N E T _ I N I T I A T E D _ R E Q . H

DESCRIPTION
  Header file defining the control block to represent an request for monitoring
  Network Initiated QOS and functions to manipulate it.

EXTERNAL FUNCTIONS
  PS_IFACE_CREATE_QOS_NET_INITIATED_REQUEST()
  PS_IFACE_RELEASE_QOS_NET_INITIATED()    

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/**===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_qos_net_initiated_req.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
06/17/11    sz     Created the file.

===========================================================================*/


/**===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "dss_iface_ioctl.h"
#include "ps_ipfltr_defs.h"
#include "ps_iface_defs.h"

/**===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/**
  @brief Maximum number of Network Initiated QoS requests. Must be at least 5 

  @see PS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM
  TODO : reconsider the value - should depends on MAX_SYSTEM_IFACES?
*/
#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_QOS_NET_INITIATED_MAX_REQS  25

#else

#define PS_QOS_NET_INITIATED_MAX_REQS  5

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/**
  @brief QOS information block stored with the flow providing the QOS.
*/
// TODO: change name to ps_qos_net_initiated_req_info_type
typedef struct
{
  /** Structure holding Rx QOS parameters */
  struct
  {
    list_type        fltr_list;       /**< List of Rx filters   */
  } rx;

  /** Structure holding Tx QOS parameters */
  struct
  {
     list_type        fltr_list;       /**< List of Rx filters  */
  } tx;
} qos_net_initiated_req_info_type;

/**
  @brief Control block for a network initiated request. An instance of this type
  is created when application registers for network initiated monitoring.
*/
typedef struct ps_qos_net_initiated_req_s
{
  list_link_type          link;             /**< Ptr to next node in a list  */
  
  /** The private information that should only ever be changed through ps_flow
      functions. The data in here should NOT be changed directly by a client. */
  struct
  {
    qos_net_initiated_req_info_type * qos_info_ptr;  /**< Ptr to QOS info    */
    ps_iface_type       * iface_ptr;                 /**< Associated iface   */
  } qos_net_initiated_req_private;
} ps_qos_net_initiated_req_type; 


/**===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


/**===========================================================================

                             UTILITY FUNCTIONS

===========================================================================*/
/**
  @brief FUNCTION PS_QOS_NET_INITIATED_CHECK_REQUESTS()

  DESCRIPTION
    Goes over all Network Initiated QoS application requests.
    For each one checks if the filters of the provided QoS Spec matches the filters
    of the application request. If yes notifies the application on the existence
    of matching Network initiated QoS.

  @param [in] iface_ptr      pointer to the iface of the flow which qos spec is provided
  @param [in] qos_spec_ptr   pointer to the qos spec of the flow

  @return
  0 : on success
  -1 : on failure
*/
int32 ps_qos_net_initiated_check_requests
(
  ps_iface_type        * iface_ptr,  
  ps_flow_type         * flow_ptr,
  qos_spec_type        * qos_spec_ptr
);


/**
  @brief FUNCTION PS_QOS_NET_INITIATED_CHECK_FLOW()

  DESCRIPTION
    Check if a specific flow can fulfil a request's requirements.

  @param [in] flow_ptr                     pointer to the flow
  @param [in] net_initiated_qos_spec_ptr   pointer to the request

  @return
  0 : if there is a match
  -1 : otherwise
*/
int32 ps_qos_net_initiated_check_flow
(
  ps_flow_type                     * flow_ptr,
  net_initiated_qos_spec_type      * net_initiated_qos_spec_ptr
);


/**
  @brief FUNCTION PS_QOS_NET_INITIATED_CHECK_ONE_REQUEST()

  DESCRIPTION
    This function receives filters specification of Network Initiated QoS
    and checks if there is any corresponding application request, i.e.
    if any application registered a request to be notified when QoS with such
    filters specification is established by the network.

  @param [in] ps_qos_net_initiated_req   pointer to an application request object
  @param [in] qos_spec_ptr               pointer to filters specifications of the
                                         Network Initiated QoS

  @return
  0 : Filters specifications of provided Application Request match the provided 
      filters specifications of the Network Initiated QoS.
  -1 : Filters specifications of provided Application Request do not match the 
      provided filters specifications of the Network Initiated QoS.
*/
int32 ps_qos_net_initiated_check_one_request
(
  ps_qos_net_initiated_req_type    * ps_qos_net_initiated_req,
  qos_spec_type                    * qos_spec_ptr
);


/**
  @brief FUNCTION PS_QOS_NET_INITIATED_CHECK_ONE_FILTER()

  DESCRIPTION
    This function receives one filter specification of Network Initiated QoS
    and checks if it matches a filter from an application request.

  @param [in] app_specified_filter   pointer to a filter specified by application
  @param [in] filter_spec_of_flow    pointer to one of the filters of Network Initiated QoS


  @return
  0  : Filters specifications match.
  -1 : Filters specifications do not match.
*/
int32 ps_qos_net_initiated_check_one_filter
(  
  ip_filter_type                   * app_specified_filter,
  ip_filter_spec_type              * filter_spec_of_flow
);


/**
  @brief FUNCTION  PS_IFACE_CREATE_QOS_NET_INITIATED_REQUEST()

  DESCRIPTION
    Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST ioctl.

  @param [in] iface_ptr                      corresponding iface on which QOS Net Initiated
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the 
                                             request information
  @param [out] ps_errno                      specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ps_iface_create_qos_net_initiated_request
(
  ps_iface_type                                 * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_request_type * qos_net_initiated_request_ptr,
  int16                                         * ps_errno
);


/**
  @brief FUNCTION  PS_IFACE_RELEASE_QOS_NET_INITIATED()

  DESCRIPTION
    Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE ioctl.

  @param [in] iface_ptr                      corresponding iface on which QOS Net Initiated
                                             registration cancellation is requested.
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the 
                                             request cancellation information.
  @param [out] ps_errno                       specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ps_iface_release_qos_net_initiated
(
  ps_iface_type                                 * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_release_type * qos_net_initiated_release_ptr,
  int16                                         * ps_errno
);

/**
  @brief FUNCTION  PS_IFACE_QOS_NET_INITIATED_REAL_TIME_CHECK()

  DESCRIPTION
    Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK ioctl.

  @param [in] iface_ptr                      corresponding iface on which QOS Net Initiated
                                             registration cancellation is requested.
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the
                                             real-time check information.
  @param [out] ps_errno                      specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ps_iface_qos_net_initiated_real_time_check
(
  ps_iface_type                                         * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_real_time_check_type * qos_net_initiated_real_time_check_ptr,
  int16                                                 * ps_errno
);

/**
  @brief FUNCTION  PS_IFACE_QOS_NET_INITIATED_REQ_INIT()

  DESCRIPTION
    Initializes QoS network initiated data structures

*/
void ps_iface_qos_net_initiated_req_init
(
  void
);

/*===========================================================================
FUNCTION PS_QOS_CHECK_ONE_FILTER()

DESCRIPTION
  This function receives one filter specification of Network Initiated QoS
  and checks if it matches a filter from an application request.

PARAMETERS
  app_specified_filter : pointer to a filter specified by application
  filter_spec_of_flow  : pointer to one of the filters of Network Initiated QoS
  ps_fltr_direction_enum_type  : Uplink or downlink direction

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_check_one_filter
(  
  ip_filter_type                               * app_specified_filter,
  ps_iface_ioctl_filter_match_request_type     * filter_info,
  ps_fltr_direction_enum_type                    direction
);

/**
  @brief FUNCTION  PS_IFACE_CHECK_FLTR_MATCH()

  DESCRIPTION
    Initializes QoS network initiated data structures

*/
int ps_iface_check_fltr_match
(
  ps_iface_type                            * iface_ptr,
  ps_iface_filter_match_info               * filtr_info_ptr
);


#ifdef __cplusplus
}
#endif

#endif /* PS_QOS_NET_INITIATED_REQ_H */

