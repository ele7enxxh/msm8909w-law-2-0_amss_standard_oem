#ifndef PS_RSVP_H
#define PS_RSVP_H
/*===========================================================================
   R E S O U R C E   R E S E R V A T I O N   P R O T O C O L (RSVP)
                       A P I   H E A D E R   F I L E
===========================================================================*/

/**
  @file ps_rsvp.h
  @brief 
  The RSVP API header file.
  Contains APIs for sending and receiving RSVP messages.
 */
 
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dssSvc group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_rsvp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Edited/added Doxygen markup and comments.
12/14/08    pp     Common Modem Interface: Public/Private API split.
07/18/08    dm     Fixed compiler warnings
04/03.07    ks     Support REG_FILTER and DEREG_FILTER options.
11/29/05    sv     Created module.

===========================================================================*/
/* group: dssSvc */
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "dssocket_defs.h"
#include "dss_netpolicy.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup constants 
  @{
*/
/** 
  RSVP message ID.
*/
#define PS_RSVP_LOCAL_MSGID 0

/**
  Maximum number of allowed RSVP filters.
*/
#define PS_RSVP_MAX_FILTERS 1
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  RSVP message types.
*/
typedef enum
{
  PS_RSVP_MIN_MSG       = 0,  /**< Minimum value for this enumerator. */
  PS_RSVP_PATH_MSG      = 1,  /**< RSVP path message. */
  PS_RSVP_RESV_MSG      = 2,  /**< RSVP reservation message. */
  PS_RSVP_PATH_ERR_MSG  = 3,  /**< RSVP path error message. */
  PS_RSVP_RESV_ERR_MSG  = 4,  /**< RSVP reservation error message. */
  PS_RSVP_PATH_TEAR_MSG = 5,  /**< RSVP path teardown message. */
  PS_RSVP_RESV_TEAR_MSG = 6,  /**< RSVP reservation teardown message. */
  PS_RSVP_RESV_CONF_MSG = 7,  /**< RSVP reservation confirmation message. */
  PS_RSVP_MAX_MSG             /**< Maximum value for this enumerator. */
} ps_rsvp_msg_enum_type;

/**
  RSVP object types.
*/
typedef enum
{
  PS_RSVP_TIME_VALUE_OBJ      = 0x1,  
    /**< Contains the value for the refresh period R used by the creator of the
         message. */
  PS_RSVP_SENDER_TEMPLATE_OBJ = 0x2,  
    /**< Contains the IP address of a sender. */
  PS_RSVP_SENDER_TSPEC_OBJ    = 0x4,  
    /**< Defines the traffic characteristics of a sender's data flow. */
  PS_RSVP_ADSPEC_OBJ          = 0x8,  
    /**< Carries OPWA data in a path message. */
  PS_RSVP_RESV_CONF_OBJ       = 0x10,  
    /**< Contains the reservation confirmation. */
  PS_RSVP_POLICY_OBJ          = 0x20,  
    /**< Defines the reservation policy. */
  PS_RSVP_STYLE_OBJ           = 0x40,  
    /**< Defines the reservation style. */
  PS_RSVP_FLOW_DESCRIPTOR_OBJ = 0x80,  
    /**< Defines a QoS in a reservation message. */
  PS_RSVP_SCOPE_OBJ           = 0x100,  
    /**< Carries an explicit list of sender hosts to which the information in
         the message is to be forwarded. */
  PS_RSVP_ERROR_SPEC_OBJ      = 0x200,  
    /**< Specifies an error in a PathErr or ResvErr message, or a confirmation
         in a ResvConf message. */
  PS_RSVP_OPAQUE_OBJ          = 0x400,  
    /**< Contains opaque objects to be sent in the reservation message. */
  PS_RSVP_SESSION_OBJ         = 0x800  
    /**< Contains the IP information to define a specific session. */
} ps_rsvp_obj_enum_type;

/**
  RSVP time value object type.
*/
typedef  uint32    ps_rsvp_time_value_obj_type;

/**
  Indicates the RSVP reservation confirmation object type.
*/
typedef  boolean ps_rsvp_resv_conf_obj_type;

/**
  @brief Opaque policy type object that holds the policy data and the length of
  the data to be sent.
*/
typedef struct
{
  char    * policy_data;  /**< Passes policy data for the sender. */
  int32     len;          /**< Length of the policy data. */
} ps_rsvp_policy_obj_type;

/**
  RSVP style object types.
*/
typedef enum
{
  PS_RSVP_STYLE_WF  = 0x11, /**< Wildcard-filter style. */
  PS_RSVP_STYLE_FF  = 0xA,  /**< Fixed-filter style. */
  PS_RSVP_STYLE_SE  = 0x12, /**< Shared-explicit style. */
  PS_RSVP_STYLE_MAX = 0x1F  /**< Maximum value of the enumerator. */
} ps_rsvp_style_obj_enum_type;

/**
  @brief Specifies an error in a reservation error message or a 
  confirmation in a reservation confirmation message.
*/
typedef struct
{
  /** IP address of the node in which the error was detected. */
  struct ps_in6_addr  error_node;  

  uint8               flags;       /**< Associated error flags. */
  uint8               error_code;  /**< One-octet error description. */
  uint16              error_value;  
    /**< Two-octet field containing additional information about the 
         error. */
} ps_rsvp_error_spec_obj_type;

/**
  @brief Opaque objects to be appended to the messages.
*/
typedef struct
{
  char    * buffer;   /**< Buffer that contains the data opaque to the RSVP. */
  uint16    buf_len;  /**< Size of the buffer. */
} ps_rsvp_opaque_obj_type;

/**
  RSVP session options.
*/
typedef  enum
{
  PS_RSVP_TTL_OPT,  /**< Sets the TTL value in the RSVP control block. */
  PS_RSVP_PROTOCOL_OPT,  
    /**< Sets the transport protocol information. */    
  PS_RSVP_RFC_COMPLIANT,  
    /**< Sets the RFC_COMPLIANT parameter to TRUE or FALSE. */
  PS_RSVP_REG_FILTER,   /**< Registers the RSVP filters. */
  PS_RSVP_DEREG_FILTER  /**< Deregisters the RSVP filters. */
} ps_rsvp_option_enum_type;

/**
  @brief RSVP protocol option type passed while setting the 
  #PS_RSVP_PROTOCOL_OPT option.
*/
typedef struct
{
  int32                     protocol;  
    /**< Protocol used in the RSVP session. */
  uint16                    port;  
    /**< Source/distribution port used for RSVP communication. */
  dss_net_policy_info_type  policy_info;  
    /**< Policy information for the RSVP session. */
} ps_rsvp_protocol_opt_type;

/**
  @brief RSVP filter information required to set up filters for laptop calls.
*/
typedef struct
{
  /** Socket address used to set the RSVP filter. */
  struct ps_sockaddr  * src_addr[PS_RSVP_MAX_FILTERS];  
    
  uint8                 num_filters;  /**< Number of filters. */
} ps_rsvp_reg_filter_opt_type;

/**
  RSVP event enumeration.
*/
typedef enum
{
  PS_RSVP_PATH_EVENT,  
    /**< Path event upcall that results from the receipt of the first path
         message for this session. */
  PS_RSVP_RESV_EVENT,  
    /**< Reservation event upcall triggered by the receipt of the first
         reservation message. */
  PS_RSVP_PATH_ERR_EVENT,  
    /**< Path error event that indicates an error occurred in the sender
         information specified in a sender call. */
  PS_RSVP_RESV_CONF_EVENT,  
    /**< Confirmation event that indicates a reservation confirmation message
         was received. */
  PS_RSVP_RESV_ERR_EVENT  
    /**< Reservation error event that indicates an error occurred in a
         reservation message to which this application contributed. */
} ps_rsvp_event_enum_type;

/**
  RSVP event call back function type.

  @param[in] sid        RSVP session ID.
  @param[in] user_data  Pointer to the user data passed in while creating the
                        session.
  @param[in] event      RSVP event for which this callback is being called.
  @param[in] obj_mask   Mask that indicates the RSVP object types present in
                        the message.
  @param[in] msgid      RSVP message ID.

  @return
  None.

  @dependencies
  None.
*/
typedef void  (*ps_rsvp_cback_fcn_type)(int32                      sid,
                                        void                     * user_data,
                                        ps_rsvp_event_enum_type    event,
                                        uint32                     obj_mask,
                                        uint32                     msgid);

/** @} */ /* end_addtogroup datatypes */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_RSVP_CREATE_SESSION
===========================================================================*/
/** @ingroup ps_rsvp_create_session
  Creates an RSVP session.

  @datatypes
  #ps_sockaddr\n
  #ps_rsvp_cback_fcn_type

  @vertspace
  @param[in] dest           Pointer to the destination socket address.
  @param[in] prot_id        RSVP session protocol ID. Valid value: TCP/UDP.
  @param[in] rsvp_cb_fptr   RSVP event callback function.
  @param[in] user_data      Pointer to the user data to be passed while calling
                            the callback.
  @param[out] ps_errno_ptr  Pointer to the the error value for a failure.

  @return
  RSVP session ID -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Allocates and initializes the RSVP control block.
  @newpage
*/
int32 ps_rsvp_create_session
(
  struct ps_sockaddr     * dest,
  int32                    prot_id,
  ps_rsvp_cback_fcn_type   rsvp_cb_fptr,
  void                   * user_data,
  int16                  * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_SET_MSG_OBJECT
===========================================================================*/
/** @ingroup ps_rsvp_set_msg_object
  Sets an RSVP object.

  @datatypes
  #ps_rsvp_msg_enum_type\n
  #ps_rsvp_obj_enum_type

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] msg_type       RSVP message type. Valid value: PS_RSVP_RESV_MSG.
  @param[in] obj            RSVP object type to be set.
  @param[in] data           Pointer to the data corresponding to the object
                            type passed in.
  @param[in] data_len       Pointer to the length of the data buffer passed in.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Creates an RSVP object and stores it in a session object.
  @newpage
*/
int32 ps_rsvp_set_msg_object
(
  int32                    sid,
  ps_rsvp_msg_enum_type    msg_type,
  ps_rsvp_obj_enum_type    obj,
  void                   * data,
  uint16                 * data_len,
  int16                  * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_GET_MSG_OBJECT
===========================================================================*/
/** @ingroup ps_rsvp_get_msg_object
  Reads the requested RSVP object from the message specified. Message ID 0
  corresponds to the messages sent by this RSVP session. The Message ID is
  assigned and returned to the application in the event callback for all the
  received messages.
  
  @datatypes
  #ps_rsvp_msg_enum_type\n
  #ps_rsvp_obj_enum_type

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] msg_type       RSVP message type.
  @param[in] msgid          Message ID corresponding to the message from which
                            a specified object is to be retrieved.
  @param[in] obj            RSVP object type value to be retrieved.
  @param[out] data          Pointer to the data buffer to hold the returned object.
  @param[in] data_len       Pointer to the length of the data buffer passed in.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  None.
  @newpage
*/
int32 ps_rsvp_get_msg_object
(
  int32                    sid,
  ps_rsvp_msg_enum_type    msg_type,
  uint32                   msgid,
  ps_rsvp_obj_enum_type    obj,
  void                   * data,
  uint16                 * data_len,
  int16                  * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_SET_OPTION
===========================================================================*/
/** @ingroup ps_rsvp_set_option
  Sets the specified RSVP option.
  
  @datatypes
  #ps_rsvp_option_enum_type

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] op             RSVP option to be set.
  @param[in] data           Pointer to the data corresponding to the option to
                            be set.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Stores the RSVP option in the RSVP control block.
  @newpage
*/
int32 ps_rsvp_set_option
(

  int32                       sid,
  ps_rsvp_option_enum_type    op,
  void                      * data,
  int16                     * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_GET_OPTION
===========================================================================*/
/** @ingroup ps_rsvp_get_option
  Gets the specified RSVP option.
  
  @datatypes
  #ps_rsvp_option_enum_type

  @vertspace
  @param[in] sid           RSVP session ID.
  @param[in] op            RSVP option value to be retrieved.
  @param[out] data         Pointer to the data buffer to hold the requested
                           option value.
  @param[out] ps_errno_ptr Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.
  @newpage
*/
int32 ps_rsvp_get_option
(
  int32                       sid,
  ps_rsvp_option_enum_type    op,
  void                      * data,
  int16                     * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_SENDER
===========================================================================*/
/** @ingroup ps_rsvp_sender
  Sends the RSVP PATH message.
  The sending of the PATH message is currently not supported. The DS_EOPNOTSUPP
  error value is returned.
  
  @datatypes
  #ps_sockaddr

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] dest_addr      Pointer to the destination address to which the
                            message is sent.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Sends the PATH message to the receivers.
  @newpage
*/
int32 ps_rsvp_sender
(
  int32                sid,
  struct ps_sockaddr * dest_addr,
  int16              * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_RESERVE
===========================================================================*/
/** @ingroup ps_rsvp_reserve
  Sends the RSVP reserve message.  
  
  @datatypes
  #ps_sockaddr

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] dest_addr      Pointer to the destination address to which the
                            message is sent.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Sends the reservation message to the network.
  @newpage
*/
int32 ps_rsvp_reserve
(
  int32                sid,
  struct ps_sockaddr * dest_addr,
  int16              * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_SEND_RESV_ERR
===========================================================================*/
/** @ingroup ps_rsvp_reserve
  Sends the RSVP RESV_ERR message.
  
  @datatypes
  #ps_sockaddr

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] dest_addr      Pointer to the destination address to which the
                            message is sent.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  None.
  @newpage
*/
int32 ps_rsvp_send_resv_err
(
  int32                sid,
  struct ps_sockaddr * dest_addr,
  int16              * ps_errno_ptr
);


/*===========================================================================
FUNCTION PS_RSVP_RELEASE
===========================================================================*/
/** @ingroup ps_rsvp_release
  Frees the RSVP control block. The sending of a TEAR message is currently not
  supported. The DS_EOPNOTSUPP error value is returned.
  
  @datatypes
  #ps_sockaddr

  @vertspace
  @param[in] sid            RSVP session ID.
  @param[in] dest_addr      Pointer to the destination address to which the
                            TEAR message is sent.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Frees the RSVP control block.\n 
  Sends a TEAR message to the specified peer.
  @newpage
*/
int32 ps_rsvp_release
(
  int32                sid,
  struct ps_sockaddr * dest_addr,
  int16              * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_FREE_MSG
===========================================================================*/
/** @ingroup ps_rsvp_free_msg
  Frees the received RSVP message.
  
  @param[in] sid            RSVP session ID.
  @param[in] msgid          Message ID of the message to be freed.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Frees the specified message from the RSVP control block.
  @newpage
*/
int32 ps_rsvp_free_msg
(
  int32    sid,
  uint32   msgid,
  int16  * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVP_DELETE_SESSION
===========================================================================*/
/** @ingroup ps_rsvp_delete_session
  Frees the RSVP control block.
  
  @param[in] sid            RSVP session ID.
  @param[out] ps_errno_ptr  Pointer to the error value for a failure.

  @return
  DSS_SUCCESS -- Success.\n
  DSS_ERROR -- Failure.

  @dependencies
  None.

  @sideeffects
  Frees the RSVP control block.
  @newpage
*/
int32 ps_rsvp_delete_session
(
  int32   sid,
  int16 * ps_errno_ptr
);


/*===========================================================================
FUNCTION PS_RSVP_COPY_SOCK_BETWEEN_SESSIONS
===========================================================================*/
/** @ingroup ps_rsvp_copy sock_between sessions
  Copies Sock information from SRC to Destination RSVP sessions
  
  @param[in] src_sid            Source RSVP session ID.
  @param[in] dst_sid            Destination RSVP session ID.

  @return
  TRUE -- Success.\n
  FALSE -- Failure.

  @dependencies
  None.

  @sideeffects
  None.
  @newpage
*/
boolean ps_rsvp_copy_sock_between_sessions
(
  int32   src_sid,
  int32   dst_sid
);

#endif  /* PS_RSVP_H */
