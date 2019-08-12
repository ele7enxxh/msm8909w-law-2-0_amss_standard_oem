
#ifndef PS_EAP_H
#define PS_EAP_H

/** 
  @file ps_eap.h 
  @brief 
   EAP API header file 
   Contains AU level API's & data structures */

/*===========================================================================

   E X T E N S I B L E   A U T H E N T I C A T I O N   P R O T O C O L (EAP)
                       A P I   H E A D E R   F I L E
                   
===========================================================================*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the eap group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_eap.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/16/11    jee    Added Doxygen comments
02/24/11    jee    Added Doxygen comments
08/10/10    jee    To support re-transmission functionality for EAP server
07/14/10  mga/jee  Modifications to support EAP server functionality
08/31/09    mga    Merged from eHRPD branch
12/19/08    pp     Common Modem Interface: Public/Private API split.
10/21/08    scb    Added function to obtain the reauth id from SFS/EFS
07/18/08    rnp    Fixed Compiler warnings
11/28/07    lti    Added changes for supporting EAP Ext
05/14/07    lti    Added meta info for upper EAP methods
08/25/06    lti    Added function for setting the resp required flag
08/25/06    lti    Added result cback registration
04/14/06    lti    Brought eap_get_identity and eap_get_identifier from 
                   ps_eapi.h
                   Brought eap_code_enum_type to ps_eapi.h
                   Brought eap_types_enum_type from ps_eap_peer.c
                   Formerly known as eap_peer_type_enume_type
                   Brought eap_frm_header_type from ps_eap_eap.c
12/05/05    hba    Updated for support for addition authenticatin schemes
                   including PEAP, TTLS, TLV and MsCHAPv2.
11/23/05    lyr    Added close cback registration
03/22/05    lyr    Updated APIs to reflect design changes
03/21/05    sv     Created module.

===========================================================================*/
/* group: eap */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"

/** @addtogroup constants 
  @{
*/

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/** Maximum number of characters in the EAP identity
*/
#define EAP_IDENTITY_MAX_CHAR 255

/** Clients using EAP services must support at least 1020-bytes MTU on the 
  link. RFC minimum: EAP MTU is 1020. 
*/
#define EAP_DEFAULT_MTU_SIZE 1020
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** Handle to the EAP instance.
*/
typedef sint15 eap_handle_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/** Invalid EAP handle value.
*/
#define EAP_INVALID_HANDLE -1

/** Number of elements in the EAP AKA sequence number array.
*/
#define EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS 32

/** EAP shared secret length.
*/
#define EAP_SHARED_SECRET_LEN 16

/** EAP AKA MILENAGE OP parameter length.
*/
#define EAP_AKA_ALGO_MILENAGE_OP_LEN 16

/** Arbitrary value. 
*/
#define EAP_EXT_MAX_VENDOR_ID_PER_INSTANCE  10

/** EAP extended vendor ID length. Refer to RFC 3748 @xhyperref{S34,[S34]}.
*/
#define EAP_EXT_VENDOR_ID_LEN 3

/** The maximum length of the extended master session key - RFC 4186/4187 @xhyperref{S11,[S11]}
*/
#define EAP_SIM_AKA_MAX_EMSK 64
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/

/**
  Indicates the type of AKA/SIM algorithms to be used. The algorithm type is 
  set by the technology-specific mode handler.
*/
typedef enum
{
  EAP_AKA_ALGO_NONE      = 0x0000, /**< EAP AKA algorithm type is NONE. */
  EAP_AKA_ALGO_SHA1      = 0x0001, /**< EAP AKA algorithm type is SHA-1. */
  EAP_AKA_ALGO_MILENAGE  = 0x0002, /**< EAP AKA algorithm type is MILENAGE. */
  EAP_AKA_ALGO_CAVE      = 0x0003, /**< EAP AKA algorithm type is CAVE. */
  EAP_SIM_ALGO_GSM       = 0x0004, /**< EAP AKA algorithm type is GSM.  */
  EAP_SIM_ALGO_USIM_GSM  = 0x0005, /**< EAP AKA algorithm type is USIM GSM. */
  EAP_SIM_AKA_ALGO_MAX             /**< Maximum value. */
} eap_sim_aka_algo_enum_type;

/** Result code for authentication.
*/
typedef enum
{
  EAP_RESULT_SUCCESS = 0, /**< Authentication succeeded. */
  EAP_RESULT_FAILURE = 1  /**< Authentication failed. */
} eap_result_enum_type;

/** @brief Information sent in response to a request/identity from the 
  authenticator.
*/
typedef struct
{ 
  uint16  len;                          /**< Length of the identity. */
  uint8  name[EAP_IDENTITY_MAX_CHAR];   /**< User ID. */
} eap_identity_type;


/** EAP codes. Refer to RFC 3748 @xhyperref{S34,[S34]}.
*/
typedef enum
{
  EAP_MIN_CODE      = 0, /**< Minimum value. */
  EAP_REQUEST_CODE  = 1, /**< EAP code for request packets. */
  EAP_RESPONSE_CODE = 2, /**< EAP code for response packets. */
  EAP_SUCCESS_CODE  = 3, /**< EAP code for success packets. */
  EAP_FAILURE_CODE  = 4, /**< EAP code for failure packets. */
  EAP_MAX_CODE           /**< Maximum value. */
} eap_code_enum_type;

/** @brief EAP frame header structure.

  The EAP frame header uses the following format:\n
  @verbatim
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Code      |   Identifier  |               Length          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  @endverbatim
*/
typedef PACKED struct PACKED_POST
{
  uint8  code;       /**< Identifies the type of packet. */
  uint8  identifier; /**< Uniquely identifies each packet. */
  uint16 length;     /**< Length of the packet. */
} eap_frm_hdr_type;

/** EAP extended meta information. Maps the vendor ID to the vendor handler 
  callback.
*/
typedef void (*eap_ext_vendor_id_input_handler_cb_type)
( 
  eap_handle_type    eap_handle, /**< Handle to the EAP instance. */
  dsm_item_type    **msg_ptr     /**< Pointer to the message pointer. */
);

/** Maps the vendor ID to the vendor result handler callback.
*/
typedef void (*eap_ext_vendor_id_result_handler_cb_type)
( 
  eap_handle_type       eap_handle, /**< Handle to the EAP instance. */
  eap_result_enum_type  result      /**< EAP result. */
);

/** Maps the vendor ID to the vendor close handler callback.
*/
typedef void (*eap_ext_vendor_id_close_handler_cb_type)
( 
  eap_handle_type  eap_handle       /**< Handle to the EAP instance. */
);

/** @brief Stores vendor-specific callback information.
*/
typedef struct
{
  uint8                                      vendor_id[EAP_EXT_VENDOR_ID_LEN];
    /**< Vendor ID. */
  eap_ext_vendor_id_input_handler_cb_type    vendor_id_input_handler_ptr;
    /**< Vendor input handler callback. */
  eap_ext_vendor_id_result_handler_cb_type   vendor_id_result_handler_ptr;  
    /**< Vendor result handler callback. */
  eap_ext_vendor_id_close_handler_cb_type    vendor_id_close_handler_ptr;  
    /**< Vendor close handler callback. */
}eap_ext_vendor_id_to_handler_type;

/** @brief Stores vendor-specific information.
*/
typedef struct
{
    uint16                               vendor_id_count;
    /**< Vendor ID count. */
    eap_ext_vendor_id_to_handler_type    vendor_handler_tbl
                                         [EAP_EXT_MAX_VENDOR_ID_PER_INSTANCE];
    /**< Table that holds the vendor handler information. */
}eap_ext_vendor_id_meta_info_type;

/** Prototype for the transmit function that the lower layer
  registers to allow the EAP to transmit a packet.
*/
typedef void (*eap_trp_tx_cback_type)
(
  void           *userdata,  /**< User data */
  dsm_item_type **pdu        /**< Data in DSM item format. */
);

/** Prototype for the authentication complete callback that the lower layer
  registers to allow the EAP to convey the authetication result.
   
  @note1hang The pre-master key must be copied by the client.
*/
typedef void (*eap_result_ind_cback_type)
( 
  eap_handle_type       handle, 
    /**< Handle to the EAP instance. */
  void                 *result_ind_user_data,
  /**< Result indication user data. */
  eap_result_enum_type  result,
  /**< Indicates authentication success/failure. */
  uint8                *pre_master_key,
  /**< Master key. */
  uint16                pre_master_key_len
  /**< Master key length. */
);

/** Identifies what supplementary information is needed.\n
 @note1hang The mask will also provide the order in which supplementary info is
 returned to the client during the supplementary result ind callback. The least
 significant bit of the mask will indicate the first value in supp_auth_info_ptr
 and the most significant bit will indicate the last value returned to the client.
*/
typedef enum
{

  EAP_SUPP_AUTH_INFO_INVALID = 0,
    /**< Invalid supplementary auth info mask */
  EAP_SUPP_AUTH_INFO_EMSK = 1,
    /**< EMSK mask */    
  EAP_SUPP_AUTH_INFO_MAX = 0xFF
    /**< Max supplementary auth info mask */
} eap_sim_aka_supp_auth_mask_type;

/** @brief Supplementary SIM/AKA auth information - Used to store the emsk
   value currently. 
*/
typedef struct
{
  uint16                               emsk_len;
    /**< Length of extended master session key */
  uint8                                emsk[EAP_SIM_AKA_MAX_EMSK];
    /**< Extended master session key */
} eap_sim_aka_supp_auth_type;

/** Prototype for the supplementary result complete callback that the lower
  layer registers to allow EAP to convey the authentication result as well
  as any supplementary authentication information.\n 
  @note1hang The supplementary auth info has to be copied by client. 
  Supplementary info in the supp_auth_info_ptr will be filled out with 
  length-value pair where the supp_auth_info_mask will give the ordering
  of the info (for further clarification see comments in 
  eap_sim_aka_supp_auth_info_mask_type).
*/
typedef void (*eap_supp_result_ind_cback_type)
( 
  eap_handle_type       handle,
    /**< EAP handle */
  void                 *supp_result_ind_user_data,
    /**< Supplementary result indication user data */
  eap_result_enum_type  result,
    /**< Indicates auth failure/success */
  eap_sim_aka_supp_auth_mask_type
                        supp_auth_info_mask,
    /**< Supplementary auth info mask */
  uint16                supp_auth_info_size,
    /**< Supplementary auth info size */
  void                 *supp_auth_info_ptr
    /**< Supplementary auth info */
);

/** Prototype for the method-specific input callback function,
  which each method must register at power-up. 
*/
typedef void (*eap_method_input_cback_type)
(
 eap_handle_type   handle, /**< Handle to the EAP instance. */
 dsm_item_type   **sdu     /**< DSM item used for storing the packet data. */
);

/** Prototype for the method-specific close callback function,
  which each method must register at power-up.
*/
typedef void (*eap_method_close_cback_type)
(
 eap_handle_type   handle  /**< Handle to the EAP instance. */
);

/** Prototype for the method-specific result callback function,
  which each method must register at power-up.
*/
typedef void (*eap_method_result_cback_type)
(
 eap_handle_type   handle,     /**< Handle to the EAP instance. */
 eap_result_enum_type  result  /**< EAP result. */
);

/** External methods supported by the EAP implementation.
*/
typedef enum
{
  /* 0-2 are reserved values */

  EAP_MIN_EXT_METHOD       = 3,  /**< Minimum value. */

  EAP_MD5_METHOD           = EAP_MIN_EXT_METHOD, /**< MD5 method. */
  EAP_TLS_METHOD           = 4,  /**< TLS method. */
  EAP_PEAP_METHOD          = 5,  /**< PEAP method. */
  EAP_TTLS_METHOD          = 6,  /**< TTLS method. */
  EAP_TLV_METHOD           = 7,  /**< TLV method. */
  EAP_MSCHAP_V2_METHOD     = 8,  /**< MSCHAP v2 method. */
  EAP_SIM_METHOD           = 9,  /**< SIM method. */
  EAP_AKA_METHOD           = 10, /**< AKA method. */
  EAP_AKA_PRIME_METHOD     = 11, /**< AKA' method. */
  EAP_EXT_METHOD           = 12, /**< Extended method. */
  EAP_MAX_METHOD                 /**< Maximum value. */

} eap_method_enum_type;

/** External methods supported by the EAP implementation.
*/
typedef enum
{
  /* 0-2 are reserved values */

  EAP_MIN_EXT_METHOD_MASK       = 1 << EAP_MIN_EXT_METHOD,
     /**< Minimum value. */
  EAP_MD5_METHOD_MASK           = EAP_MIN_EXT_METHOD_MASK,
    /**< MD5 method. */
  EAP_TLS_METHOD_MASK           = 1 << EAP_TLS_METHOD,
    /**< TLS method. */
  EAP_PEAP_METHOD_MASK          = 1 << EAP_PEAP_METHOD,
    /**< PEAP method. */
  EAP_TTLS_METHOD_MASK          = 1 << EAP_TTLS_METHOD,
    /**< TTLS method. */
  EAP_TLV_METHOD_MASK           = 1 << EAP_TLV_METHOD,
    /**< TLV method. */
  EAP_MSCHAP_V2_METHOD_MASK     = 1 << EAP_MSCHAP_V2_METHOD,
    /**< MSCHAP v2 method. */
  EAP_SIM_METHOD_MASK           = 1 << EAP_SIM_METHOD,
    /**< SIM method. */
  EAP_AKA_METHOD_MASK           = 1 << EAP_AKA_METHOD,
    /**< AKA method. */
  EAP_AKA_PRIME_METHOD_MASK     = 1 << EAP_AKA_PRIME_METHOD,
    /**< AKA' method. */
  EAP_EXT_METHOD_MASK           = 1 << EAP_EXT_METHOD,
    /**< Extended method. */
  EAP_MAX_METHOD_MASK           = 1 << EAP_MAX_METHOD
    /**< Maximum value. */

} eap_method_mask_enum_type;

/** Identifies the role supported: Peer, Authenticator, or both.
*/
typedef enum
{
  EAP_INVALID_ROLE         = -1,
    /**< Invalid EAP role. */
  EAP_PEER_ROLE            = 0,
    /**< Peer role. */
  EAP_AUTHENTICATOR_ROLE   = 1,
    /**< Authenticator role. */
  EAP_ANY_ROLE             = 2
    /**< Peer or Authenticator role. */
} eap_role_enum_type;

/** Identifies the type of EAP client is using the EAP library.
*/
typedef enum
{
  EAP_CLIENT_DEFAULT = 0,
  /**< DEFAULT EAP client with default configurations. */
  EAP_CLIENT_IPSEC,
  /**< IPSEC EAP client. */
  EAP_CLIENT_EHRPD,
  /**< eHRPD EAP client. */
  EAP_CLIENT_QMI_AUTH,
  /**< QMI Auth EAP client. */
  EAP_CLIENT_WLAN,
  /**< WLAN EAP client. */
  EAP_CLIENT_ATCOP,
  /**< ATCOP EAP client. */
  EAP_CLIENT_MAX
    /**< Maximum value */
}eap_client_enum_type;


/** Callback type for the client start indication callback. This is
    used only for the server role functionality.
*/
typedef void (*eap_client_ind_cb_type)
(
  eap_handle_type       handle,
  void                 *client_user_data
);

/** Switches the task after the timeout of the retransmission timer. This 
  function is used for the server functionality only. 
*/
typedef void (* eap_server_timer_cback_type ) (void *user_data_eap_server);

/** EAP timer callback type. */
typedef boolean (* eap_client_timer_cback_type) (
                                    eap_server_timer_cback_type  timer_cback,
                                    void *eap_server_user_data,
                                    void *client_user_data);

/** @brief Holds all relevant information to be passed from the lower layer 
  during the EAP initialization. 
*/
typedef struct
{
  eap_identity_type          user_id;             
    /**< User ID.*/
  eap_trp_tx_cback_type      trp_tx_f_ptr;        
    /**< Transport Tx function pointer.   */
  void                      *trp_tx_user_data;    
    /**< User data for the Tx function.   */
  eap_result_ind_cback_type  result_ind_f_ptr;    
    /**< Authentication completed callback.  */
  void                      *result_ind_user_data;
    /**< Authentication callback user data.  */
  
  /* The following authentication protocol mask is a bitmask
      derived using the EAP method enumeration above
  */
  uint32                     eap_auth_prot_mask;  
    /**< Authentication protocol mask. */
  boolean                    raw_mode;            
    /**< Indicates whether the mode is Passthrough.    */

  /* Indicates whether the EAP state machine should
    wait for a SUCCESS from the EAP Server before deeming successful the
    EAP conversation.
  */ 
  boolean                    authenticator_rsp_required;
    /**< Indicates whether an authentication response from the EAP server is
         required. */ 
  boolean                    support_eap_ext; 
    /**< Indicates whether support for the EAP external method is required. */

  eap_client_ind_cb_type     eap_client_start_ind_cb; 
    /**< Client start indication callback.  */
  eap_role_enum_type         eap_role;                
    /**< Peer or server role. */
  eap_client_timer_cback_type timer_f_ptr;     
    /**< Timer context switch callback. */
  void                       *timer_user_data; 
    /**< User data for the timer callback. */
  void                       *start_ind_user_data;    
    /**< Start indication callback user data. */
  eap_supp_result_ind_cback_type 
                             supp_result_ind_f_ptr;
    /**< Supplementary result completed callback. */
  void                      *supp_result_ind_user_data;
    /**< Supplementary result user data. */

} eap_info_type;
/** @} */ /* end_addtogroup datatypes */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_METHOD_INPUT_CBACK_REG
===========================================================================*/
/** @ingroup eap_method_input_cback_reg
  Registers the method-specific input function with EAP services.

  @datatypes
  #eap_method_input_cback_type\n
  #eap_method_enum_type\n
  #eap_role_enum_type

  @vertspace
  @param[in] cback   Callback to be registered.
  @param[in] method  Method for which the callback is registered.
  @param[in] role    Role is an EAP peer or server.

  @return 
  None.
	
  @dependencies 
  This function must be called once at power-up because all EAP instances use 
  the same input function for a given method.
  @newpage
*/
void eap_method_input_cback_reg
(
  eap_method_input_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
);

/*===========================================================================
FUNCTION EAP_METHOD_CLOSE_CBACK_REG
===========================================================================*/
/** @ingroup eap_method_close_cback_reg
  Registers the method-specific close function with EAP services.

  @datatypes
  #eap_method_close_cback_type\n
  #eap_method_enum_type\n
  #eap_role_enum_type

  @vertspace
  @param[in] cback   Callback to be registered.
  @param[in] method  Method for which the callback is registered.
  @param[in] role    Role is an EAP peer or server.

  @return 
  None.
	
  @dependencies 
  This function must be called once at power-up because all EAP instances use 
  the same close function for a given method.
  @newpage
*/
void eap_method_close_cback_reg
(
  eap_method_close_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
);

/*===========================================================================
FUNCTION EAP_METHOD_RESULT_CBACK_REG
===========================================================================*/
/** @ingroup eap_method_result_cback_reg
  Registers the method-specific result function with EAP services.

  @datatypes
  #eap_method_result_cback_type\n
  #eap_method_enum_type\n
  #eap_role_enum_type

  @vertspace
  @param[in] cback   Callback to be registered.
  @param[in] method  Method for which the callback is registered.
  @param[in] role    Role is an EAP peer or server.

  @return 
  None.
	
  @dependencies 
  This function must be called once at power-up because all EAP instances use 
  the same result function for a given method.
  @newpage
*/
void eap_method_result_cback_reg
(
  eap_method_result_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
);

/*===========================================================================
FUNCTION EAP_CREATE_INSTANCE
===========================================================================*/
/** @ingroup eap_create_instance
  Initializes the EAP control block and returns the handle to the EAP instance.

  @datatypes
  #eap_info_type

  @vertspace
  @param[in] eap_info  EAP information from the lower layers.

  @return 
  @ge 0 -- Handle to the EAP control block. \n
  -1    -- Indicates a failure. 
	
  @dependencies 
  EAP services must have been initialized.

  @sideeffects
  This function allocates an EAP instance to the specified client.
  @newpage
*/
eap_handle_type eap_create_instance
(  
  eap_info_type *eap_info
);

/*===========================================================================
FUNCTION EAP_DELETE_INSTANCE
===========================================================================*/
/** @ingroup eap_delete_instance
  Shuts down the EAP instance.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle   Handle to the EAP instance.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.

  @sideeffects
  This function causes any subsequent packets sent from an EAP method to be 
  discarded by the EAP layer.
  @newpage
*/
void eap_delete_instance
(  
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_INPUT
===========================================================================*/
/** @ingroup eap_input
  Rx function for the EAP. This function parses the EAP header and 
  calls the upper layer input function, if necessary.

  @datatypes
  #eap_handle_type\n
  #dsm_item_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.
  @param[in] sdu         EAP input packet.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.

  @sideeffects
  Calls the authentication protocol Rx function.
  @newpage
*/
void eap_input
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);

/*===========================================================================
FUNCTION EAP_XMIT_PKT
===========================================================================*/
/** @ingroup eap_xmit_pkt
  Tx function for the EAP. This function adds the EAP header and sends
  the packet on the associated link layer.

  @datatypes
  #eap_handle_type\n
  #dsm_item_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.
  @param[in] sdu         EAP packet to be transmitted.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.

  @sideeffects
  This function sends the packet on the associated link layer.
  @newpage
*/
void eap_xmit_pkt
(
   eap_handle_type   eap_handle,
   dsm_item_type   **sdu
); 


/*===========================================================================
FUNCTION EAP_AUTH_COMPLETE_IND
===========================================================================*/
/** @ingroup eap_auth_complete_ind
  Called by the authentication protocol to notify the EAP of the result of the 
  authentication process.

  @datatypes
  #eap_handle_type\n
  #eap_result_enum_type

  @vertspace
  @param[in] handle              Handle to the EAP instance.
  @param[in] result              Result of the EAP authentication process.
  @param[in] pre_master_key      Pre-master key. 
  @param[in] pre_master_key_len  Pre-master key length.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.

  @sideeffects
  This function calls the client's authentication completed callback.
  @newpage
*/
void eap_auth_complete_ind 
(
  eap_handle_type          handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
);

/*===========================================================================
FUNCTION EAP_AUTH_TRANSACTION_CLOSING
===========================================================================*/
/** @ingroup eap_auth_transaction_closing
  Called by the authentication protocol to notify the  EAP peer layer to 
  perform the appropriate state transitions, after indicating that the EAP 
  authorization transaction is closing.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle   Handle to the EAP instance.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void eap_auth_transaction_closing 
(
  eap_handle_type          eap_handle
);

/*===========================================================================
FUNCTION EAP_SET_AUTHENTICATOR_RSP_REQUIRED
===========================================================================*/
/** @ingroup eap_set_authenticator_rsp_required
  Offers support for dynamically setting the authenticator_rsp_required flag.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle                  Handle to the EAP instance.
  @param[in] authenticator_rsp_required  Indicates whether a reponse from the 
                                         server is required.

  @return 
  None.
	
  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void eap_set_authenticator_rsp_required
(
  eap_handle_type          eap_handle,
  boolean                  authenticator_rsp_required 
);

/*===========================================================================
FUNCTION EAP_HANDLE_IS_VALID
===========================================================================*/
/** @ingroup eap_handle_is_valid
  Verifies the validity of the EAP instance.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.

  @return 
  True  -- Handle is valid. \n
  False -- Handle is invalid. 
	
  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
boolean eap_handle_is_valid
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_SET_AUTH_MGR
===========================================================================*/
/** @ingroup eap_set_auth_mgr
  Sets the authenticator manager for a particular upper method. Each EAP 
  instance can have a separate authenticator manager regardless of the method
  in use.

  The EAP is just a conduit for the authenticator manager information. One 
  authenticator manager is stored per EAP instance and is not associated with
  any method. The caller is expected to store the information in the auth_mgr 
  entity.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.
  @param[in] auth_mgr    Authenticator manger information to be stored.

  @return 
  None.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void eap_set_auth_mgr
(
  eap_handle_type       eap_handle,
  void                 *auth_mgr
);

/*===========================================================================
FUNCTION EAP_SET_AUTH_MGR
===========================================================================*/
/** @ingroup eap_get_auth_mgr
  Gets the authenticator manager information for a specific upper method.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.

  @return 
  Pointer to the authenticator manager information.\n
  @note1hang The returned pointer is abstract. The caller must cast the concrete 
  manager.
  @vertspace
  @return
  NULL -- Failure.  

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void* eap_get_auth_mgr
(
  eap_handle_type   eap_handle
);

/*===========================================================================
FUNCTION EAP_SET_META_INFO
===========================================================================*/
/** @ingroup eap_set_meta_info
  Sets the meta information for a particular upper method. Each EAP instance 
  can have a separate meta information regardless of the method in use.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle     Handle to the EAP instance.
  @param[in] meta_info_ptr  Meta information pointer to be set.

  @return 
  None.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void eap_set_meta_info
(
  eap_handle_type       eap_handle,
  void                 *meta_info_ptr
);

/*===========================================================================
FUNCTION EAP_GET_META_INFO
===========================================================================*/
/** @ingroup eap_get_meta_info
  Gets the meta information for a specific upper method.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle   Handle to the EAP instance.

  @return 
  A pointer to the meta information.\n
  @note1hang The returned pointer is abstract. The caller must cast the concrete 
  manager.
  @vertspace
  @return
  NULL -- Failure. 

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void* eap_get_meta_info
(
  eap_handle_type   eap_handle
);

/*===========================================================================
FUNCTION EAP_GET_IDENTITY
===========================================================================*/
/** @ingroup eap_get_identity
  Called by a method to request the identity of the EAP instance.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle     Handle to the EAP instance

  @return 
  Identity of the EAP instance.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
eap_identity_type eap_get_identity
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_GET_IDENTIFIER
===========================================================================*/
/** @ingroup eap_get_identifier
  Called by a method to request the identifier of the last EAP packet.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle  Handle to the EAP instance.

  @return 
  Last EAP packet identifier.\n
  0 -- Failure. 

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
uint8 eap_get_identifier
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_GET_REAUTH_ID
===========================================================================*/
/** @ingroup eap_get_reauth_id
  Called by a method to request the reauthentication identifier of the last 
  EAP packet.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle     Handle to the EAP instance.
  @param[out] data          EAP reauthentication ID.
  @param[out] max_data_len  Length of the EAP reauthentication ID.

  @return 
  Length of the EAP reauthentication ID.\n
  -1 -- Failure. 

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
int eap_get_reauth_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
);

/*===========================================================================
FUNCTION EAP_GET_PSEUDONYM_ID
===========================================================================*/
/** @ingroup eap_get_pseudonym_id
  Called by a method to request the pseudonym identifier of the last EAP packet.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle     Handle to the EAP instance.
  @param[out] data          EAP pseudonym ID.
  @param[out] max_data_len  Length of the EAP pseudonym ID.

  @return 
  Length of the EAP pseudonym ID.\n
  -1 -- Failure.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
int eap_get_pseudonym_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
);

/*===========================================================================
FUNCTION EAP_START_IND
===========================================================================*/
/** @ingroup eap_start_ind
  Called by the lower layer (e.g., LAN 1.x) to start the identity phase by the
  EAP server. This function is used only by EAP server functionality.

  @datatypes
  #eap_handle_type

  @vertspace
  @param[in] eap_handle     Handle to the EAP instance.

  @return 
  0  -- Success. \n
  -1 -- Failure.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
int eap_start_ind 
( 
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_EXT_XMIT_PKT
===========================================================================*/
/** @ingroup eap_ext_xmit_pkt 
  Tx function for the EAP. This function adds the EAP extention header 
  and sends the packet to the lower EAP layer.

  @datatypes
  #eap_handle_type\n
  #dsm_item_type

  @vertspace
  @param[in] eap_handle   Handle to the EAP instance.
  @param[in] sdu          Pointer to the EAP packet to be transmitted.
  @param[in] vendor_id    EAP vendor ID.

  @return 
  None.

  @dependencies 
  An EAP instance must be created before this function is called.

  @sideeffects
  This function sends the packet on the associated link.
  @newpage
*/
void eap_ext_xmit_pkt
(
   eap_handle_type   eap_handle,
   dsm_item_type   **sdu,
   uint8*            vendor_id
); 

/*===========================================================================
FUNCTION EAP_EXT_AUTH_COMPLETE_IND
===========================================================================*/
/** @ingroup eap_ext_auth_complete_ind
  Called by the authentication protocol to notify the EAP of the
  authentication process result. The extended method acts as 
  passthrough and sends the result to the lower EAP layer.

  @datatypes
  #eap_handle_type\n
  #eap_result_enum_type

  @vertspace
  @param[in] eap_handle         Handle to the EAP instance.
  @param[in] result             Result of the EAP authentication process.
  @param[in] pre_master_key     Pointer to the pre-master key.
  @param[in] pre_master_key_len Pre-master key length.

  @return 
  None.

  @dependencies 
  An EAP instance must be created before this function is called.
  @newpage
*/
void eap_ext_auth_complete_ind 
(
  eap_handle_type          handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
);

/*===========================================================================
FUNCTION EAP_SET_AUTH_CLIENT_ID
===========================================================================*/
/** @ingroup eap_set_auth_client_id
  This function set the client EAP expects with a given version for EAP
  to do client specific configurations. 
  
  Each EAP instance can have a separate client/version combination
  irrespective of the method in use.
  
  @datatypes
  #eap_handle_type\n
  #eap_client_enum_type

  @vertspace
  @param[in] eap_handle         Handle to the EAP instance.
  @param[in] eap_client_type    Specifies type of EAP client.
  @param[in] eap_client_version Client version for EAP config           

  @return 
  None.

  @dependencies 
  EAP instance should have been created and this function should be
  call only during config time and not during EAP transactions for the
  client specific changes to take affect properly

  @newpage
*/
void eap_set_auth_client_id
(
  eap_handle_type       eap_handle,
  eap_client_enum_type  eap_client_type,
  uint32                eap_client_version
);

#endif  /* PS_EAP_H */
