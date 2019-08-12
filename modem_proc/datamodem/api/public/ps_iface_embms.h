#ifndef PS_IFACE_EMBMS_H
#define PS_IFACE_EMBMS_H
/*===========================================================================

                          PS_IFACE_EMBMS.H

DESCRIPTION
  This file defines the data structures and functions that will be used in
  the implementation of eMBMS on the modem

EXTERNALIZED FUNCTIONS
  None

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_embms.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/01/12    hs     Created module.
===========================================================================*/

#include "comdef.h"

/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/
/**
  @brief Defines the length of TMGI in bytes
*/
#define PS_IFACE_EMBMS_TMGI_SIZE 6

/**
  @brief Defines an invalid session ID. If the application does not specify a
         session id, this value is passed to lower layers
*/
#define PS_IFACE_EMBMS_INVALID_SESSION_ID 0xFF

/**
  @brief This is a reserved client id that will be used to identify if an
         indication has to be deliverd to a particular client or to
         all clients that have registered for it.
*/
#define PS_IFACE_EMBMS_RESERVED_CLIENT_ID 0xFFFF

/**
  @brief Max number of Freq searched from the list of 
  frequencies sent by the upper layers, to activate a 
  TMGI. Currently LTE modem will pick atmost 8 freq from this 
  list, based on the max inter-freq neighbors specified in SIB5 
*/ 
#define PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH (64)

/**
  @brief Max number of Service Area ID's associated with a 
  particular frequency that LTE Modem will search to activate a 
  TMGI. 
*/ 
#define PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH  (64)

/** 
 @brief max number of frequencies for SAI infomation
 */  
#define PS_IFACE_EMBMS_MAX_NUM_FREQ_FOR_SAI_INFO 9

/**
  @brief This is the TMGI type that will be passed in the activate request for
         eMBMS
*/
typedef struct
{
  byte    tmgi[PS_IFACE_EMBMS_TMGI_SIZE];
  /**< Temporary Mobile Group Identifier */

  boolean session_id_present;
  /**< Flag for session ID presence      */

  uint8   session_id;
  /**< Session ID                        */
} ps_iface_embms_tmgi_type;

typedef uint32 ps_iface_earfcn_type;

typedef uint16 ps_iface_embms_sai_type;

/**
  @brief This is the payload for the activate TMGI request IOCTL 
*/
typedef struct
{
  void                       *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */

  int16                      debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  uint8                      preempt_priority; 
  /**< Preemption priority of the TMGI to be activated, range 0 - 5(highest),
       If value > 5, modem will treat it as 5 (highest priority) */

  uint8                      num_earfcn_to_search; 
  /**< Number of EARFCNs in the list below */

  ps_iface_earfcn_type       *earfcn_list; 
  /** Array of EARFCNs where TMGI can be found. Max value
      PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH */

  ps_iface_embms_tmgi_type   tmgi_session_info;
  /**< TMGI and Session information for TMGIs to be activated */

  /*------------------------------------------
    Either the SAI list (or) the EARFCN list 
    is valid, not both 
   -------------------------------------------*/
  uint8                      num_sai_to_search; 
  /**< Number of SAIs in the list below */

  ps_iface_embms_sai_type    *sai_list;
  /** List of SAIs where TMGI can be found. Max value 
    PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH */

} ps_iface_embms_tmgi_act_req_type;

/**
  @brief This is the payload for the deactivate TMGI request IOCTL 
*/
typedef struct
{
  void                       *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */

  int16                      debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  ps_iface_embms_tmgi_type   tmgi_session_info;
  /**< TMGI and Session information for TMGIs to be activated */

} ps_iface_embms_tmgi_deact_req_type;


/**
  @brief This is the payload for the simultaneous activate/deactivate TMGI 
  request IOCTL 
*/
typedef struct
{
  void                       *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */

  int16                      debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  uint8                      preempt_priority; 
  /**< Preemption priority of the TMGI to be activated, range 0 - 5(highest),
       If value > 5, modem will treat it as 5 (highest priority) */

  uint8                      num_earfcn_to_search; 
  /**< Number of EARFCNs in the list below */

  ps_iface_earfcn_type          *earfcn_list; 
  /** List of EARFCNs where TMGI can be found */

  ps_iface_embms_tmgi_type   act_tmgi_session_info;
  /**< TMGI and Session information for TMGIs to be activated */

  ps_iface_embms_tmgi_type   deact_tmgi_session_info;
  /**< TMGI and Session information for TMGIs to be deactivated */

  /*------------------------------------------
    Either the SAI list (or) the EARFCN list 
    is valid, not both 
   -------------------------------------------*/
  uint8 num_sai_to_search; 
  /**< Number of SAIs in the list below */

  ps_iface_embms_sai_type *sai_list;
  /** List of SAIs where TMGI can be found. Max value 
    PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH */

} ps_iface_embms_tmgi_act_deact_req_type;


/**
  @brief This is the payload for all TMGI related query IOCTLS
*/
typedef struct
{

  void *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */
  int16  debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */
} ps_iface_embms_tmgi_related_query_type;

/** 
  @brief This is the payload for the available SAI LIST REQ ioctl
*/
typedef struct
{

  void *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */
  int16  debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */
} ps_iface_embms_sai_list_req_type;

/**
  This macro is used to construct the numerical value of an enum given
  the category (type) and the cause (reason).
*/
#define EMBMS_TYPE_REASON_VAL(type, reason)                              \
  ((0xFFFF0000 & (type << 16)) | (0x0000FFFF & reason))

/**
  This macro defines the category into which the activation
  causes may be classified under

  Category 0 corresponds to activation success
*/
#define PS_IFACE_EMBMS_CAUSE_CATEGORY_0 0
/**
  This macro defines the category into which the deactivation
  causes may be classified under

  Category 1 corresponds to activation failure
*/
#define PS_IFACE_EMBMS_CAUSE_CATEGORY_1 1

/**
  This macro defines the category into which the deactivation
  causes may be classified under

  Category 2 corresponds to client initiated deactivation
*/
#define PS_IFACE_EMBMS_CAUSE_CATEGORY_2 2

/**
  This macro defines the category into which the deactivation
  causes may be classified under

  Category 3 corresponds to modem initiated deactivation
*/
#define PS_IFACE_EMBMS_CAUSE_CATEGORY_3 3

/**
  This macro defines the category into which the warning
  causes may be classified under

  Category 4 corresponds to warning cause codes
*/

#define PS_IFACE_EMBMS_CAUSE_CATEGORY_4 4
/**
  This enumeration is used to identify different deactivation causes.

  Three major categories are as follows:
   0. Activation success
   1. Activation failure (response to a client activation request)
   2. Client initiated deactivation
   3. Modem initiated deactivation
   4. Warning indication

  The upper 16 bits are used to indicate this category and the lower 16 bits
  indicate the cause of failure.

  The macro seen below is used to assign a numeric value to the enums
*/

typedef enum
{
  IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID = -1,

  /*--------------------------------------------------------------------------
    Category 0
  --------------------------------------------------------------------------*/  
  IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS                = 
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_0, 0),
  /**< Activation success */  
  IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_INFO_DUPLICATE = 
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_0, 1),
  /**< Duplicate Activation, success */  
  IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_IDLE_RADIO_TUNE = 
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_0, 2),
  /**<  TMGI requested on a different frequency while UE is idle, success */ 
  IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_CONN_RADIO_TUNE = 
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_0, 3),
  /**<  TMGI requested on a different frequency while UE is connected. 
        eMBMS interest has been sent on target frequency. Success, as
        far as upper layers are concerned. */
  
 
  /*--------------------------------------------------------------------------
    Category 1
  --------------------------------------------------------------------------*/
  IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_UNKNOWN =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 4),
  /**< TMGI activation failure, failure unknown. Client can retry 
    later*/
  IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_NOT_ALLOWED =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 5),
  /**< TMGI activation failure, operation not allowed. Client need 
    not retry */
  IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MISSING_CONTROL_INFO =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 6),
  /**< TMGI activation failure due to missing control info (MCCH). 
    Client can retry later */
  IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MISSING_TMGI =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 7),
  /**< TMGI activation failure, missing TMGI in MCCH. Client can 
    retry later */
   IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MULTICAST_OOS =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 8),
  /**< TMGI activation failure, multicast OOS (sib13 missing). 
    Client can retry later */
   IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_UNICAST_OOS =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 9),
  /**< TMGI activation failure, unicast OOS. Client can retry 
    later  */
   IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_CAMPED_ON_OTHER_FREQ =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 10),
  /**< TMGI activation failure, TMGI is requested on a freq different
    from one on which it is being received. Client can retry */
   IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_SAI_MISMATCH =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 107),
  /**< TMGI activation failure, none of the SAIs in the Act TMGI req 
    are available */
   IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MAX_TMGI_ALREADY_ACTIVE =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_1, 108),
  /**< TMGI activation failure, max TMGIs already active */
  /*--------------------------------------------------------------------------
    Category 2
  --------------------------------------------------------------------------*/
  IFACE_EMBMS_TMGI_DEACTIVATION_CLIENT_INITIATED   =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_2, 0),
  /** < Client initated TMGI deactivation is in progress */
  /*--------------------------------------------------------------------------
    Category 3 - Used only while deactivating TMGIs that were previously active
  --------------------------------------------------------------------------*/
  IFACE_EMBMS_TMGI_DEACTIVATION_LTE_STOPPED        =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_3, 0),
  /** < TMGI deactivated, LTE stopped.
        Client can try activation again  */
  IFACE_EMBMS_TMGI_DEACTIVATION_SESSION_END        =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_3, 1),
  /** < TMGI deactivated, session ended.
        Client can try activation again  */
  IFACE_EMBMS_TMGI_DEACTIVATION_OUT_OF_COVERAGE    =
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_3, 2),
  /** < TMGI deactivated, UE in OOC area
        Client can try activation later */
  IFACE_EMBMS_TMGI_DEACTIVATION_SESSION_NOT_FOUND  = 
    EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_3, 3),
  /** < TMGI deactivated, UE in OOC area
        Client can try activation later */
  /*--------------------------------------------------------------------------
    Category 4 - Used for warning indication
  --------------------------------------------------------------------------*/
    IFACE_EMBMS_TMGI_WARNING_UNICAST_OOS           =
  EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_4, 0),
  /**<  currently under Unicast OOS. Client can try 
        after the TMGI is sent as part of OOS Warnng cleared list */

  IFACE_EMBMS_TMGI_WARNING_MULITCAST_OOS   = 
  EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_4, 1),
  /**<  currently under Multicast OOS. Client can try 
        after the TMGI is sent as part of OOS Warnng cleared list */

  IFACE_EMBMS_TMGI_WARNING_OOS_CLEARED     = 
  EMBMS_TYPE_REASON_VAL(PS_IFACE_EMBMS_CAUSE_CATEGORY_4, 2),
  /**<  List of TMGIs with OOS Warning cleared. Client can try activating 
        these TMGIs */

  IFACE_EMBMS_TMGI_EVENT_INFO_CODE_MAX /* Do not use */
} ps_iface_embms_tmgi_event_info_code_type;


/**
  @brief This is the payload carried along with the event
         IFACE_EMBMS_TMGI_ACTIVATED_EV
*/
typedef struct
{
  void                                  *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */
  int16                                        debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  ps_iface_embms_tmgi_type                     tmgi_session_info;
  /**< TMGI and Session information */

  ps_iface_embms_tmgi_event_info_code_type     act_info_code;
  /**< Extended info code if any    */
} ps_iface_embms_tmgi_activation_info_type;

/**
  @brief This type is a part of the payload sent for the event
         IFACE_EMBMS_TMGI_DEACTIVATED_EV. This type os also used
         whenever we send IFACE_EMBMS_ACTIVE_TMGI_LIST_EV and
         IFACE_EMBMS_AVAILABLE_TMGI_LIST_EV.
*/
typedef struct
{
  ps_iface_embms_tmgi_type  *list;
  /**< List of TMGI            */

  uint8                     num_tmgi;
  /**< Number of TMGI in the list */
} ps_iface_embms_tmgi_list_type;


/**
  @brief Event info for IFACE_EMBMS_ACTIVE_TMGI_LIST_EV,
         IFACE_EMBMS_AVAILABLE_TMGI_LIST_EV, and
         IFACE_EMBMS_WARNING_TMGI_LIST_EV
*/
typedef struct
{
  void                      *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */

  int16                     debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  ps_iface_embms_tmgi_list_type tmgi_list;
  /**< List of TMGIs                                 */

  ps_iface_embms_tmgi_event_info_code_type info_code;
  /**< Extended event information */

} ps_iface_embms_tmgi_list_info_type;

/**
  @brief This is the payload carried along with the
         IFACE_EMBMS_TMGI_DEACTIVATED_EV. 
*/
typedef  ps_iface_embms_tmgi_list_info_type ps_iface_embms_tmgi_deactivation_info_type;

/**
 *@brief Struct describes SAI info associated with each frequency */ 
typedef struct 
{
  uint32                    frequency;             
    /** frequency associated with sai_list*/
  boolean                   is_serving_frequency;  
    /** is this a serving frequency */
  uint8                     num_sais;              
    /** number of elements in the sai_list*/
  ps_iface_embms_sai_type   sai_list[PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH];  
    /** SAIs associated with the frequency*/
                                 
}ps_iface_embms_frequency_sai_info;

/**
  @brief This is the payload carried along with the event
         IFACE_EMBMS_AVAILABLE_SAI_LIST_EV */
typedef struct
{
  void                      *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */

  int16                     debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  uint8 num_frequencies; 
  /**< Number of SAIs in the list below */

  ps_iface_embms_frequency_sai_info *freq_sai_info;
  /** SAI information where TMGIs can be found */

} ps_iface_embms_sai_info_type;

/**
  @brief This is the payload carried along with the event
         IFACE_EMBMS_TMGI_ACTIVATE_DEACTIVATE_RESPONSE_EV */

typedef struct
{
  void                                         *client_id;
  /**< Client identifier to uniquely identify the intended recipient for this
       event. Set to reserved value if the event is broadcasted to
       all clients */
  int16                                        debug_trace_id;
  /**< Debug trace ID for the message. -1 when not used */

  ps_iface_embms_tmgi_type                     act_tmgi_session_info;
  /**< Activated TMGI and Session information */

  ps_iface_embms_tmgi_type                     deact_tmgi_session_info;
  /**< Deactivated TMGI and Session information */

  ps_iface_embms_tmgi_event_info_code_type     act_info_code;
  /**< Extended info code for activation info if any   */

  ps_iface_embms_tmgi_event_info_code_type     deact_info_code;
  /**< Extended info code for deactivation reason*/

} ps_iface_embms_tmgi_activation_deactivation_info_type;

typedef enum
{
  PS_IFACE_EMBMS_GENERATE_SAI_LIST_EV
  /**< SAI(Service Area Identity List) */  

} ps_iface_embms_events_type;

/**
  @brief This is the payload carried along with 
         PS_IFACE_IOCTL_EMBMS_GENERATE_EVENTS */

typedef struct
{
  ps_iface_embms_events_type    embms_event;
  /**< Indicates this is a modem initiated request */

} ps_iface_embms_generate_events_info_type;

/**
  @brief This is the payload carried along with 
         PS_IFACE_IOCTL_EMBMS_CONTENT_DESC_UPDATE 
         IFACE_EMBMS_CONTENT_DESC_CONTROL_EV*/

typedef struct
{
  uint32    size;
  /**< Size of the content description passed by the 
       content_description pointer */
  void     *content_description;
  /**< Pointer to the content description payload */
} ps_iface_embms_content_desc_update_type;


#endif /* PS_IFACE_EMBMS_H */
