#ifndef PS_EAP_SIM_AKA_H
#define PS_EAP_SIM_AKA_H

/** 
  @file ps_eap_sim_aka.h 
  @brief 
  This file contains common EAP-SIM/AKA processing functions.
*/
/*===========================================================================

       E A P   S U B S C R I B E R   I D E N T I T Y   M O D U L E
        A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
 
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
Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_eap_sim_aka.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/08/13    fn      Added notification support for multiple users
02/07/13    fn      Added support for multi SIM support
05/01/12    op      Added support for the additional meta_identity
08/26/11    op      Added changes to obtain supplementary SIM/AKA auth info
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/16/11    jee     Added Doxygen comments
02/24/11    jee     Added Doxygen comments
===========================================================================*/
/* group: eap */


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

               EAP SIM DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/
/** @addtogroup datatypes 
  @{
*/
/** Task service function type.   
*/
typedef void (* eap_sim_aka_task_srv_req_cb_type )( void * user_data);

/** EAP SIM AKA task service request type.
*/
typedef boolean (* eap_sim_aka_task_srv_req_type)
                 ( eap_sim_aka_task_srv_req_cb_type  req_cb,
                   void*                             user_data);

/** EAP AKA message notification receive function type.
*/
typedef void (* eap_sim_aka_notification_rcv_cb_type )( void * user_data);
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/** Permanent ID length. This is an arbitrary value. 
*/
#define EAP_SIM_AKA_MAX_ID_LEN            255
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief EAP SIM-AKA specific method meta information. 
*/
typedef struct
{
  /** ID information. */
  struct 
  {
    uint8   provided_id_len;                     /**< Length of the preset identity. */
    uint8   provided_id[EAP_SIM_AKA_MAX_ID_LEN]; /**< Preset identity. */
  } id;

  /** IMSI information. */
  struct
  {
    uint16 mcc;  /**< IMSI MCC. */
    uint8  mnc;  /**< IMSI MNC. */
    uint32 s1;   /**< IMSI s1 / IMSI min1. */
    uint16 s2;   /**< IMSI s2 / IMSI min2. */
  } imsi;

  
  eap_sim_aka_task_srv_req_type  task_srv_fct; 
    /**< Task service function. */
  
  eap_sim_aka_notification_rcv_cb_type notification_rcv_cb_fct;
    /**< Notification receive callback function. */

  uint64 aka_algo_seqnum_arr[EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS];
    /**< Sequence number details.  */

  uint8  aka_algo_seqnum_arr_num_elements;
    /**< Number of elements in the sequence number array. */

  char eap_shared_secret[EAP_SHARED_SECRET_LEN]; 
    /**< EAP-shared secret details. */

  boolean new_client; 
    /**< Indicates whether the client is new. */

  eap_sim_aka_algo_enum_type sim_aka_algo_type;
    /**< Algorithm to be used for the EAP_AKA/SIM. */

  boolean eap_aka_in_sw;
    /**< Indicates whether to use EAP-AKA in the software or use the USIM 
         card. */
 
  uint8  aka_algo_milenage_op_data[EAP_AKA_ALGO_MILENAGE_OP_LEN];
    /**< Attributes required to store data to be passed to the AKAALAGO module, 
         which implements the MILENAGE algorithm.   */

  uint32 aka_algo_milenage_op_data_len;
    /**< AKA MILENAGE OP data length. */

  eap_sim_aka_supp_auth_mask_type supp_result_auth_info_mask;
    /**< Supplementary auth information mask. */

  eap_identity_type eap_meta_identity;
    /**< Meta identity to be used for any additional EAP identity. */

  boolean eap_session_is_set;
    /**< Indication from client to use eap_session_type for multi-sim */

  uint32 eap_session_type;
    /**< Session type for multiple slots (mmgsdi_session_type_enum_type) */

}eap_sim_aka_meta_info_type;

/** AKA notification codes according to RFC 4187 @xhyperref{S11,[S11]}. 
*/
typedef enum
{
   EAP_AKA_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH   = 0,
    /**< General failure after authentication. */
   EAP_AKA_NOTIFICATION_USER_DENIED_TEMP             = 1026,
    /**< User denied. */
   EAP_AKA_NOTIFICATION_USER_NOT_SUBSCRIBED          = 1031,
    /**< User not subscribed. */
   EAP_AKA_NOTIFICATION_GENERAL_FAILURE              = 16384,
    /**< General failure. */
   EAP_AKA_NOTIFICATION_REALM_UNAVAILABLE            = 16385,
    /**< Realm unavailable. */ 
   EAP_AKA_NOTIFICATION_USER_NAME_UNAVAILABLE        = 16386,
    /**< User name unavailable. */ 
   EAP_AKA_NOTIFICATION_CALL_BARRED                  = 16387,
    /**< Call barred. */
   EAP_AKA_NOTIFICATION_SUCCESS                      = 32768
    /**< Notification success. */
}eap_sim_aka_notification_enum_type;

/** SIM/AKA notification data structure use to transport code to clients. 
*/
typedef struct
{
  eap_sim_aka_notification_enum_type notification_code;
  eap_handle_type                    eap_handle;
}eap_sim_aka_notification_s_type;

/** @} */ /* end_addtogroup datatypes */
#endif /* PS_EAP_SIM_AKA_H */
