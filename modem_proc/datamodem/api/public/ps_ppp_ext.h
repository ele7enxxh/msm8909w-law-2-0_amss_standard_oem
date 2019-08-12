#ifndef PS_PPP_EXT_H
#define PS_PPP_EXT_H

/**
  @file ps_ppp_ext.h

  @brief
   This file definitions that are used for PPP externally (at
   Modem level).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the linkLayer group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2008-2015 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_defs.h_v   1.7   03 Feb 2003 15:01:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ppp_ext.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/15    sd     Fallback support for CHAP challenge response calculation.
11/18/14    skc    Removing usage of PS_RX_UW_FMC_SIGNAL 
11/03/13    cx     Add support for second RM device in DUN call
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/11/11    ssh    Doxygen comments
05/06/11    sd     (Tech Pubs) Edited/added Doxygen markup and comments.
02/28/11    op     Doxygen writeup 
08/31/09   mga     Merged from eHRPD branch
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/

/* group: linkLayer */
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_eap.h"

/** @addtogroup datatypes 
  @{
*/
/** Defines the PPP devices.
 */
typedef enum
{
  PPP_MIN_DEV                   = 0,   /**< Lower bound value. */
  PPP_UM_SN_DEV                 = 0,   /**< UM service network instance. */
  PPP_RM_DEV                    = 1,   /**< RM instance. */
  PPP_UM_AN_DEV                 = 2,   /**< UM AN instance. */
  PPP_RM_2_DEV                  = 3,   /**< Second RM instance. */
  PPP_MAX_DEV,                         /**< Maximum value for array sizing. */
  PPP_INVALID_DEV               = 255  /**< Invalid device. */
} ppp_dev_enum_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/**
  Maximum length for the user ID. 

  This user ID length must be equal to or greater than NV_MAX_PPP_USER_ID_LENGTH 
  defined in nv.h. The user ID length may need to be greater than 
  NV_MAX_PPP_USER_ID_LENGTH to store the TE2 user ID for network model packet
  calls.
*/
#define PPP_MAX_USER_ID_LEN 127

/**
  Maximum length for passwords used during authentication (i.e.,
  PAP or CHAP). 

  This password length must be equal to or greater than 
  NV_MAX_PAP_PASSWORD_LENGTH defined in nv.h.

  In the network model, the password used during resynchronization for CHAP is
  different than the initial password.
*/
#define PPP_MAX_PASSWD_LEN  127


#define PPP_CHAP_CHAL_LEN      16 /**< Maximum CHAP challenge length. */
#define PPP_CHAP_CHAL_NAME_LEN 16 /**< Maximum CHAP challenge name length. */

#define PPP_EAP_MAX_USER_ID_LEN   255 /**< Maximum EAP user ID length. */
#define PPP_EAP_SHARED_SECRET_LEN 16  /**< Maximum EAP password length. */
 
/** Maximum length for APN or NULL APN (APN length equals zero) is supported. */
#define VSNCP_3GPP2_APN_MAX_VAL_LEN  100
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** PPP EAP results.
 */
typedef enum 
{
  PS_PPP_EAP_RESULT_SUCCESS = 0, /**< EAP operation succeeded. */
  PS_PPP_EAP_RESULT_FAILURE = 1  /**< EAP operation failed. */ 
}ps_ppp_eap_result_enum_type;

/**
    Mode handler function type that is registerd with the PS.
    This is called by the PS when the EAP returns the result.
*/
typedef void (* ppp_eap_config_data_after_auth )
(
  uint8 *master_session_key,   
    /**< EAP master session key. */ 
  uint16 master_session_key_len,               
    /**< EAP master session key length. */ 
  ps_ppp_eap_result_enum_type ps_eap_result,   
    /**< EAP result. */ 
  void *eap_user_data,   
    /**< EAP user data. */ 
  uint64 *aka_algo_seqnum_arr,   
    /**< EAP AKA algorithm sequence number array. */ 
  uint8 aka_algo_seqnum_arr_num_elements   
    /**< Number of elements in the EAP AKA algorithm sequence number array. */ 
);

/**
  @brief Contains the authentication information (i.e., username and password).

  All lengths indicate the amount of data in the corresponding buffers. If
  nothing is stored, the lengths are to be set to 0.

  In Client mode:\n
  - user_id_info and passwd_info must be filled in (i.e., input parameters)
    when authentication is accepted. 
  - challenge_info and challenge_name are not used as input or output.

  In Server mode:\n
  - challenge_name is to be filled in when doing CHAP authentication. Values
    passed here appear in the challange name field of the CHAP challenge. 
  - The other three fields are filled in when the client sends its information.
*/
typedef struct
{
  char  user_id_info[PPP_MAX_USER_ID_LEN];     
    /**< PPP user ID. */
  char  passwd_info[PPP_MAX_PASSWD_LEN];       
    /**< PPP password. */
  char  challenge_info[PPP_CHAP_CHAL_LEN];     
    /**< CHAP challenge information. */
  char  challenge_name[PPP_CHAP_CHAL_NAME_LEN];
    /**< CHAP challenge name. */
  uint8 user_id_len;                           
    /**< PPP user ID length. */
  uint8 passwd_len;                            
    /**< PPP password length. */
  uint8 challenge_len;                         
    /**< CHAP challenge information length. */
  uint8 challenge_name_len;                    
    /**< CHAP challenge name length. */
  uint8 nai_entry_index;                       
    /**< NAI entry index. */
  char    eap_user_id[PPP_EAP_MAX_USER_ID_LEN];
    /**< EAP user ID. */
  uint8   eap_user_id_len;                     
    /**< EAP user ID length. */ 
  char    eap_shared_secret[PPP_EAP_SHARED_SECRET_LEN];
    /**< EAP shared secret. */
  void    *eap_user_data;                      
    /**< EAP user data. */

  ppp_eap_config_data_after_auth  get_config_data_after_auth_ptr;
    /**< Pointer to the EAP configuration data after authentication. */

  uint64 aka_algo_seqnum_arr[EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS];
    /**< Pointer to the EAP configuration data after authentication. */

  uint8 aka_algo_seqnum_arr_num_elements;
    /**< Number of elements in the EAP AKA algorithm sequence number. */
  eap_sim_aka_algo_enum_type aka_algo_type;
    /**< Algorithm to be used for EAP AKA (e.g., SHA1 or MILENAGE).  */
  boolean ehrpd_usim_present;
    /**< Indicates whether to use EAP AKA in the software or on the USIM card. */
  uint8  aka_algo_milenage_op_data[EAP_AKA_ALGO_MILENAGE_OP_LEN];
    /**< Attributes required to store data to be passed to the AKAALAGO
         module that implements the MILENAGE algorithm.  */
  uint32 aka_algo_milenage_op_data_len;
    /**< Length of stored attribute data. */

  boolean is_userid_set_by_client;
    /**< flag to indicate if user-id is set by client. */
  boolean is_pwd_set_by_client;
    /**< flag to indicate if password is set by client. */
} ppp_auth_info_type;
    
/** @} */ /* end_addtogroup datatypes */

#endif /* PS_PPP_EXT_H */
