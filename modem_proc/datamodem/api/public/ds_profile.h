#ifndef DS_PROFILE_H
#define DS_PROFILE_H

/** 
  @file ds_profile.h 
  @brief 
   Data Services Profile Registry API definitions 
*/
/* This file contains common, external header file definitions for Data 
   Services Profile Registry API. This library is thread-safe. 
*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dsProfile group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_profile.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/14/12   gs      Added declaration of ds_profile_close_lib()
08/28/11   sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/23/11   ack     Added 3GPP2 Out of profiles error result 
03/02/11   mg      doxygen comments
02/11/11   ttv     Added changes for consolidated profile family.
01/14/11   ss      Added support for Get/Set Iface linger parameters.
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/
/* group: dsprofile */
#include "comdef.h"
#include "sys.h"
#include "dsumtspdpreg.h"
/** @addtogroup constants 
  @{
*/
/** Data services profile version. The first two digits indicate the major version. The 
  last two digits are the minor version.
*/
#define DS_PROFILE_VERSION      1101

/** Maximum number of handles supported.
*/
#define DS_PROFILE_MAX_NUM_HNDL 32

/** Data services embedded profile family.
*/
#define DS_PROFILE_EMBEDDED_PROFILE_FAMILY  0

/** Data services tethered profile family.
*/
#define DS_PROFILE_TETHERED_PROFILE_FAMILY  1

/** Unspecified profile number
*/
#define DS_PROFILE_UNSPECIFIED_NUM  ((ds_profile_num_type)(-1))

/** Always share profile number
*/
#define DS_PROFILE_ALWAYS_SHARE_PROFILE_NUM  ((ds_profile_num_type)(0xFE))
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** Profile parameter identifier type.
*/
typedef uint32  ds_profile_identifier_type;    

/** Profile number.
*/
typedef uint16  ds_profile_num_type;           

/** Profile handle.
*/
typedef void*   ds_profile_hndl_type;          

/** Iterator used in list functions.
*/
typedef void*   ds_profile_itr_type; 

/** cb handle used in callback functions 
*/
typedef uint32 ds_profile_cb_handle_type;
/** @} */ /* end_addtogroup datatypes */


/** Offset for tech common parameters. The offset is required to avoid the overlap 
  between 3GPP and 3GPP2 parameters. These new values are inline with the 
  updated QMI WDS spec.
*/
#define DS_PROFILE_TECH_COMMON_PARAM_OFFSET                  0x80

/** Identifies technology-common profile parameters.
*/
/** Important, new params should be added in the beginning with negative offset, 
    rather than in the end */
typedef enum
{
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_INVALID                 = 0x0,
    /**< Invalid profile parameter. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MIN                     = -0x4 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,
    /**< Minimum profile parameter. 
         Should be bigger than DS_PROFILE_3GPP_PROFILE_PARAM_MAX 
    */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG     = -0x3 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /** Indicates if P-CSCF address is requested via DHCP. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_DNS_DHCP_REQ_FLAG       = -0x2 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,
    /** Indicates if DNS address is requested via DHCP. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE      = -0x1 +
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,

  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_APP_DATA      = 0x0 +
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,
    /**< 4 byte opaque data */

  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MNC                     = 0x1 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /** MNC */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MCC                     = 0x2 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,
    /** MCC */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_OPERATOR_RESERVED_PCO   = 0x3 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /** OPERATOR RESERVED PCO */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PASSWORD           = 0x4 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,  
    /**< Authentication password. */ 
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_ID                 = 0x5 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< User ID. */    
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PROTOCOL           = 0x6 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< Authentication protocol. */    
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_REQUEST_PCSCF_REQ_FLAG  = 0x7 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< Indicates whether the PCSCF address is needed. */   
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_LINGER_PARAMS           = 0x8 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< Linger parameter. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY   = 0x9 + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< IPv6 secondary DNS address. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY     = 0xA + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< IPv6 primary DNS address. */       
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY   = 0xB + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< IPv4 secondary DNS address. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY     = 0xC + 
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< IPv4 primary DNS address. */    
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_CLASS               = 0xD +
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< APN class. */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_DISABLED            = 0xE +
                                    DS_PROFILE_TECH_COMMON_PARAM_OFFSET,    
    /**< APN disabled. */
  /** @cond
  */
  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MAX  = DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_DISABLED
    /**< Maximum profile parameter (used internally). */
  /** @endcond */
}ds_profile_tech_common_param_enum_type;

typedef enum
{
  DS_PROFILE_GENERAL_PROFILE_PARAM_IPV6_PREFIX_DELEGATION_FLAG  = 0XDF,  
    /**< IPV6 prefix delegation. */
  DS_PROFILE_GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG = 0XDE,
   /**< XLAT enable flag. */
  DS_PROFILE_GENERAL_PROFILE_PARAM_SUBS_ID  = 0XDD  
    /**< Subs ID. */
  /** @cond  */
}ds_profile_tech_general_param_enum_type;

/** Status return values from functions.
*/ 
typedef enum   
{
  DS_PROFILE_REG_RESULT_SUCCESS    =       0,  
    /**< Successful operation. */
  DS_PROFILE_REG_RESULT_FAIL,                  
    /**< General failure in the library. */
  
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL,        
    /**< Invalid profile handle. */ 
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP,          
    /**< Operation not supported. */
                                                 
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE,
    /**< Invalid technology type. */
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM, 
    /**< Invalid profile number. */
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT,       
    /**< Invalid identifier. */
  DS_PROFILE_REG_RESULT_ERR_INVAL,             
    /**< Other invalid argument. */
                                                 
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED,    
    /**< Library has not been initialized. */
                                                 
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID,       
    /**< Buffer size length is invalid.\n
         For get_param, the buffer size cannot be less than the maximum.\n
         For set_param, the buffer size cannot be greater than the maximum. */
  
  DS_PROFILE_REG_RESULT_LIST_END,              
    /**< End of the list has been reached. This status is the return value for 
         _itr_next. */
  
  DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID,     
    /**< Subscription ID is invalid. */
  DS_PROFILE_REG_INVAL_PROFILE_FAMILY,         
    /**< Invalid profile family. */
  DS_PROFILE_REG_PROFILE_VERSION_MISMATCH,
    /**< Profile Version Mismatch */
DS_PROFILE_REG_RESULT_ERR_OUT_OF_MEMORY,
    /**< Out of memory */
  DS_PROFILE_REG_RESULT_ERR_FILE_ACCESS,            
    /**< Error accessing the embedded file system. */
  DS_PROFILE_REG_RESULT_ERR_EOF,            
    /**< Error end of file. */
  DS_PROFILE_REG_RESULT_ERR_VALID_FLAG_NOT_SET,    
    /**< Profile valid flag is not set.  */
  DS_PROFILE_REG_ERR_OUT_OF_PROFILES,   
    /**< No profiles are available while creating a new profile. */

  DS_PROFILE_REG_NO_EMERGENCY_PDN_SUPPORT,
    /**< Emergency PDN Feature disabled */
  
  DS_PROFILE_REG_NOT_SUPPORTED,
    /**< Operation is not supported */  
  DS_PROFILE_REG_SET_IDENT_NOT_SUPPORTED,
  /**< Set Identifier is not supported*/   
    
  DS_PROFILE_REG_3GPP_SPEC_MIN = 0x1000,     
    /**< Offset for UMTS technology errors. */
  DS_PROFILE_REG_3GPP_INVAL_PROFILE_FAMILY,  
    /**< Invalid profile family. */
  DS_PROFILE_REG_3GPP_ACCESS_ERR,            
    /**< Error accessing the embedded file system. */
  DS_PROFILE_REG_3GPP_CONTEXT_NOT_DEFINED,   
    /**< Profile context is not defined. */
  DS_PROFILE_REG_3GPP_VALID_FLAG_NOT_SET,    
    /**< Profile valid flag is not set.  */
  DS_PROFILE_REG_3GPP_READ_ONLY_FLAG_SET,    
    /**< Read only flag is set in the profile. */
  DS_PROFILE_REG_3GPP_ERR_OUT_OF_PROFILES,   
    /**< No profiles are available while creating a new profile. */
  DS_PROFILE_REG_3GPP_SPEC_MAX = 0x10FF,     
    /**< Maximum for UMTS technology errors. */
  
  DS_PROFILE_REG_3GPP2_SPEC_MIN = 0x1100,    
    /**< Offset for CDMA technology errors. */
  DS_PROFILE_REG_3GPP2_ERR_INVALID_IDENT_FOR_PROFILE, 
    /**< Identifier is not valid for the profile. */
  DS_PROFILE_REG_3GPP2_ERR_OUT_OF_PROFILES,
    /**< Error indicating out of 3GPP2 profiles. */
  DS_PROFILE_REG_3GPP2_SPEC_MAX = 0x11FF,    
    /**< Maximum for CDMA technology errors. */
  /** @cond
  */
  DS_PROFILE_REG_RESULT_MAX    = 0xFFFF
    /**< Maximum value for return type (used internally). */
  /** @endcond */
} ds_profile_status_etype;

/** Profile technology type values.
*/
typedef enum
{
  /** @cond
  */
  DS_PROFILE_TECH_MIN     = 0x00,
    /**< Minimum value for the profile technology type (used internally). */
  /** @endcond */

  DS_PROFILE_TECH_3GPP    = DS_PROFILE_TECH_MIN,
    /**< 3GPP technology type. */
  DS_PROFILE_TECH_3GPP2   = 0x01,
    /**< 3GPP2 technology type. */
  /** @cond
  */
  DS_PROFILE_TECH_EPC     = 0x02,
  DS_PROFILE_TECH_MAX     = 0x03,
    /**< Maximum value for the profile technology type (used internally). */
  /** @endcond */

  DS_PROFILE_TECH_INVALID = 0xFF
    /**< Invalid technology type. */
}ds_profile_tech_etype;

/** Transaction type values.
*/ 
typedef enum
{
  DS_PROFILE_TRN_READ   = 0x01,
    /**< Transaction type is read. */
  DS_PROFILE_TRN_RW     = 0x03,
    /**< Transaction type is write. */
  /** @cond
  */
  DS_PROFILE_TRN_VALID  = DS_PROFILE_TRN_READ | DS_PROFILE_TRN_RW
  /** @endcond */
}ds_profile_trn_etype;

/** Action values for ds_profile_end_transaction().
*/
typedef enum 
{
  /** @cond
  */
  DS_PROFILE_ACTION_MIN    = 0x0,
    /**< Minimum value for the action type (used internally). */
  /** @endcond */

  DS_PROFILE_ACTION_COMMIT = 0x1,
    /**< Commit action type. */
  DS_PROFILE_ACTION_CANCEL = 0x2,
    /**< Cancel action type. */
  /** @cond
  */
  DS_PROFILE_ACTION_MAX    = 0xFF
    /**< Maximum value for the action type (used internally). */
  /** @endcond */
}ds_profile_action_etype;

/** Profile list configuration.
*/
typedef enum
{
  /** @cond
  */
  DS_PROFILE_LIST_DFN_MIN           = 0,
    /**< Minimum value for the list definition type (used internally). */
  /** @endcond */

  DS_PROFILE_LIST_ALL_PROFILES      = 1,
    /**< Profile list includes all profiles. */
  DS_PROFILE_LIST_SEARCH_PROFILES   = 2,
    /**< Profile list is based on key words or values. */
  /** @cond
  */
  DS_PROFILE_LIST_DFN_MAX           = 0xFF
    /**< Maximum value for the list definition type (used internally). */
  /** @endcond */
} ds_profile_list_dfn_etype;

/** Dual SIM subscription information.
*/
typedef enum 
{
  DS_PROFILE_ACTIVE_SUBSCRIPTION_NONE = 0x00,
    /**< No active subscription. */
  DS_PROFILE_ACTIVE_SUBSCRIPTION_1,
    /**< Active subscription 1. */
  DS_PROFILE_ACTIVE_SUBSCRIPTION_2,
    /**< Active subscription 2. */
  DS_PROFILE_ACTIVE_SUBSCRIPTION_3,
    /**< Active subscription 3. */
  DS_PROFILE_ACTIVE_SUBSCRIPTION_MAX = DS_SUBSCRIPTION_MAX
    /**< Maximum active subscription. */
} ds_profile_subs_etype;

/** Callback event types
*/
typedef enum 
{
  DS_PROFILE_INVALID_EVENT          =   0,
    /**< Invalid event. */
  DS_PROFILE_CREATE_PROFILE_EVENT   =   1,
    /**< Profile was created */
  DS_PROFILE_DELETE_PROFILE_EVENT   =   2,
    /**< Profile was deleted */
  DS_PROFILE_MODIFY_PROFILE_EVENT   =   3,
    /**< Profile was modified */
  DS_PROFILE_RESET_PROFILE_EVENT   =    5,
    /**< Profile was reset */
  DS_PROFILE_SUBSCRIPTION_CHANGE_EVENT = 6,
    /**< Profile Subscription changed */
  DS_PROFILE_MAX_EVENT              =   0xff
} ds_profile_event_etype;

/** @brief Stores the information about changes in specific profile 
  used in callback function
*/
typedef struct {
  ds_profile_num_type         profile_num; /** profile number */
  ds_profile_tech_etype       profile_tech; /** profile tech */
  unsigned int                idents_num;  /** the len of idents array. 
                                               If 0, the array is NULL */
  ds_profile_identifier_type* changed_idents_array; /** array of changed idents */
} ds_profile_changes_info;

/** @brief Stores the parameter value for get/set operations.
  if param is a string (like for example apn name) len should be
  equal to strlen of that string in profile
*/
typedef struct 
{
  void     *buf;  /**< Buffer to store the parameter value. */
  uint16   len;   /**< Buffer length. */
} ds_profile_info_type;

/** @brief Stores the node for the profile list. For name, this structure 
  allocates memory as specified by the profile_name_max_len parameter in the
  corresponding technology header.
*/
typedef struct
{
  ds_profile_num_type   num;
    /**< Profile number. */
  ds_profile_info_type *name;
    /**< Profile name. */
} ds_profile_list_info_type;

/** @brief Identifies the type of list to be returned, depending on the DFN.
*/
typedef struct
{
  ds_profile_list_dfn_etype  dfn;
    /**< List definition type. */
  ds_profile_identifier_type ident;
    /**< Profile parameter identifier. Not used for 
         #DS_PROFILE_LIST_ALL_PROFILES. 
         see comment regarding len inside of ds_profile_info_type
         for idents that point to string arguments*/
  ds_profile_info_type       info;
    /**< Profile parameter value.  Not used for 
         #DS_PROFILE_LIST_ALL_PROFILES. */
} ds_profile_list_type;

/** Type of APN disable flag. */
typedef boolean  ds_profile_apn_disable_flag_type;

/** Type of CLAT enable flag.
    Data type to be used with 
    DS_PROFILE_GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG */
typedef boolean  ds_profile_clat_enable_flag_type;

/** Type of APN class. */
typedef uint8    ds_profile_apn_class_type;

/** @brief Stores the IPv4 address. This structure is used for a DNS address
    similar to ps_in_addr structures.
*/
typedef PACKED struct PACKED_POST
{
  uint32 ds_profile_s_addr; /**< Socket address. */
} ds_profile_in_addr_type;

/** @brief Stores the IPv6 address. This structure is used for a DNS address
    similar to ps_in6_addr structures.
*/
typedef PACKED struct PACKED_POST
{
    /** Stores the IPv6 address. */
    union
  {
    uint8   ds_profile_u6_addr8[16];  /**< IPv6 8-bit socket address. */
    uint16  ds_profile_u6_addr16[8];  /**< IPv6 16-bit socket address. */
    uint32  ds_profile_u6_addr32[4];  /**< IPv6 32-bit socket address. */
    uint64  ds_profile_u6_addr64[2];  /**< IPv6 64-bit socket address. */
  } ds_profile_in6_u;
#define ds_profile_s6_addr    ds_profile_in6_u.ds_profile_u6_addr8
  /**< IPv6 8-bit socket address type. */
#define ds_profile_s6_addr16  ds_profile_in6_u.ds_profile_u6_addr16
  /**< IPv6 16-bit socket address type. */
#define ds_profile_s6_addr32  ds_profile_in6_u.ds_profile_u6_addr32
  /**< IPv6 32-bit socket address type. */
#define ds_profile_s6_addr64  ds_profile_in6_u.ds_profile_u6_addr64
  /**< IPv6 64-bit socket address type. */
} ds_profile_in6_addr_type;

/** @brief Linger parameters.
*/
typedef PACKED struct PACKED_POST
{
  boolean allow_linger_flag;   /**< Flag to indicate whether lingering is 
                                    supported for this profile. */
  uint16  linger_timeout_val;  /**< Linger timeout value in seconds. */
} ds_profile_linger_params_type;

/** Indicates whether the PCSCF address is needed. */
typedef boolean ds_profile_is_pcscf_req_flag_type; 

/** Indicates whether the DNS address via DHCP needed. */
typedef boolean ds_profile_req_dhcp_dns_address_flag_type; 

/** Indicates whether the PCSCF address via DHCP needed. */
typedef boolean ds_profile_req_dhcp_pcscf_address_flag_type; 

/** Common PDP type
 *  This enum should be used to configure the common pdp
 *  parameter
*/
typedef enum 
{
  DS_PROFILE_COMMON_PDP_TYPE_IP     = 0,     
  /**< PDP type IP. */
  DS_PROFILE_COMMON_PDP_TYPE_IPV4   = 0,
  /**< PDP type IPv4. */  
  DS_PROFILE_COMMON_PDP_TYPE_IPV6   = 1,
  /**< PDP type IPv6. */     
  DS_PROFILE_COMMON_PDP_TYPE_IPV4V6 = 2
  /**< PDP type IPv4v6. */      
} ds_profile_common_pdp_etype;


/** 4 byte opaque user data. */
typedef uint32  ds_profile_user_profile_class;

/** Authentication type for a PDP context.
*/
typedef enum
{
  DS_PROFILE_AUTH_PREF_PAP_CHAP_NOT_ALLOWED = 0x0,
    /**< No authentication.  */
  DS_PROFILE_AUTH_PREF_PAP_ONLY_ALLOWED     = 0x1,
    /**< PAP authentication. */
  DS_PROFILE_AUTH_PREF_CHAP_ONLY_ALLOWED    = 0x2,
    /**< CHAP authentication. */
  DS_PROFILE_AUTH_PREF_PAP_CHAP_ALLOWED     = 0x3,
    /**< PAP/CHAP authentication. */
  DS_PROFILE_AUTH_PREF_EAP_ALLOWED          = 0x4,
    /**< EAP authentication. */
  /** @cond
  */
  DS_PROFILE_AUTH_PREF_MAX             = 0xff
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
} ds_profile_auth_pref_type;

/** Maximum length for userid. Supported value: Same as defined in #PPP_MAX_PASSWD_LEN.
*/
#define DS_PROFILE_PPP_MAX_USER_ID_LEN       127

/** Maximum length for password. Supported value: Same as defined in #PPP_MAX_USER_ID_LEN.
*/
#define DS_PROFILE_PPP_MAX_PASSWD_LEN        127

/** Operator reserved pco id
*/
typedef uint16  ds_profile_op_pco_id_type;


/** A 16-bit integer representation of MCC.
    Range: 0 to 999.
*/
typedef uint16  ds_profile_mcc_type;

/** @brief Mobile Network Code
*/
typedef PACKED struct PACKED_POST
{
  uint16  ds_profile_mnc;
  /**< actual mnc digit */
  boolean mnc_includes_pcs_digit; 
  /**< This field is used to interpret the length of the corresponding
       MNC reported Values:
       - TRUE -- MNC is a three-digit value; e.g., a reported value of 
         90 corresponds to an MNC value of 090
       - FALSE -- MNC is a two-digit value; e.g., a reported value of 
          90 corresponds to an MNC value of 90 */
} ds_profile_mnc_type;


/** Profile configuration definitions.
*/
typedef enum
{
  DS_PROFILE_CONFIG_MASK_NONE        = 0x00, /**< No mask. */
  DS_PROFILE_CONFIG_MASK_PERSISTENCE = 0x01, /**< Persistence mask. */
  DS_PROFILE_CONFIG_MASK_SUBS_ID     = 0x02, /**< Subscription ID mask. */
  DS_PROFILE_CONFIG_MASK_PROFILE_ID  = 0x04, /**< Profile ID mask. */
  DS_PROFILE_CONFIG_MASK_ALL =         0x07  /**< All mask. */
} ds_profile_config_mask_etype;

/** Profile configuration mask type.
*/
typedef uint32  ds_profile_config_mask_type;

/** @brief Profile configuration mask. */
typedef struct
{
  ds_profile_config_mask_type  config_mask;  
    /**< Configuration mask. */
  boolean                      is_persistent;
    /**< Boolean indicating the profile configuration is persistent. */
  ds_profile_subs_etype       subs_id;
    /**< Subscription ID for the profile. */
  ds_profile_num_type num;
    /** profile number for the profile to be created with */
} ds_profile_config_type;
/** @} */ /* end_addtogroup datatypes */

/*---------------------------------------------------------------------------
                       PUBLIC ROUTINES
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION DS_PROFILE_INIT_LIB 
============================================================================*/
/** @ingroup ds_profile_init_lib
  Initializes the DS profile library. On the modem, this function is 
  called only once at initialization. This function initializes the library for
  that process domain.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS -- Operation successful.\n
  DS_PROFILE_REG_RESULT_FAIL    -- General error. 

  @dependencies 
  This function is called only once during startup.
  @newpage
*/
ds_profile_status_etype ds_profile_init_lib( void );

/*===========================================================================
FUNCTION DS_PROFILE_CLOSE_LIB 
============================================================================*/
/** @ingroup ds_profile_close_lib
  Closes the DS profile library and releases any associated resources. 

  @return
  DS_PROFILE_REG_RESULT_SUCCESS -- Operation successful.\n
  DS_PROFILE_REG_RESULT_FAIL    -- General error. 

  @dependencies 
  @newpage
*/
ds_profile_status_etype ds_profile_close_lib( void );

/*===========================================================================
FUNCTION DS_PROFILE_BEGIN_TRANSACTION 
============================================================================*/
/** @ingroup ds_profile_begin_transaction 
  Returns a handle that the clients of this software library can use for 
  subsequent profile operations. The handle returned is of the requested 
  transaction type.
 
  @datatypes
  #ds_profile_trn_etype\n
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_hndl_type

  @vertspace
  @param[in] trn     Requested transaction type.
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[out] hndl   Pointer to the return requested handle.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n 
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.\n
  All profile operations using this handle must call 
  ds_profile_end_transaction() at the end of the operation.
  @newpage
*/
ds_profile_status_etype ds_profile_begin_transaction (
  ds_profile_trn_etype   trn,
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  ds_profile_hndl_type  *hndl
);

/*===========================================================================
FUNCTION DS_PROFILE_BEGIN_TRANSACTION_PER_SUB 
============================================================================*/
/** @ingroup ds_profile_begin_transaction_per_sub 
  Returns a handle that the clients of this software library can use for 
  subsequent profile operations. The handle returned is of the requested 
  transaction type.
 
  @datatypes
  #ds_profile_trn_etype\n
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_hndl_type

  @vertspace
  @param[in] trn     Requested transaction type.
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[in] subs_id Subscription ID
  @param[out] hndl   Pointer to the return requested handle.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n 
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.\n
  All profile operations using this handle must call 
  ds_profile_end_transaction() at the end of the operation.
  @newpage
*/
ds_profile_status_etype ds_profile_begin_transaction_per_sub (
  ds_profile_trn_etype   trn,
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  ds_profile_subs_etype  subs_id,
  ds_profile_hndl_type  *hndl 
);

/*===========================================================================
FUNCTION DS_PROFILE_END_TRANSACTION 
============================================================================*/
/** @ingroup ds_profile_end_transaction
  Commits the prefetched, modified profile to the persistent storage on the 
  modem. This function also invokes cleanup routines for the profile
  handle specified. On return, the handle becomes unusable.
 
  @datatypes
  #ds_profile_hndl_type\n
  #ds_profile_action_etype

  @vertspace
  @param[in] hndl  Profile handle.
  @param[in] act   Commits the profile to persistent storage and invokes the 
                   clean up routines, or cancels the actions. 
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS             -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED  -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL      -- Invalid handle.\n
  DS_PROFILE_REG_RESULT_FAIL                -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_end_transaction (
  ds_profile_hndl_type    hndl,
  ds_profile_action_etype act
);

/*===========================================================================
FUNCTION DS_PROFILE_CREATE 
============================================================================*/
/** @ingroup ds_profile_create
  Returns a profile number from a pool of free profiles. 

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech   Technology type for which a profile needs to be created.
  @param[out] num   Pointer to the profile number to be created.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported  
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_create (
  ds_profile_tech_etype  tech,
  ds_profile_num_type   *num
);

/*===========================================================================
FUNCTION DS_PROFILE_CREATE_EX
=============================================================================*/
/** @ingroup ds_profile_create_ex
  Returns a profile number from a pool of free profiles. 

  @note1hang Not all technology types support this operation.

  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_config_type\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech       Technology type.
  @param[in] config_ptr Pointer to the configuration for creating profiles.
  @param[in] num        Pointer to the profile number of the profile created.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation was successful.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library has not been 
                                                  initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General error. 
  
  @dependencies
  None.
  @newpage
*/
ds_profile_status_etype ds_profile_create_ex
(
  ds_profile_tech_etype     tech,
  ds_profile_config_type *  config_ptr,
  ds_profile_num_type *     num
);

/*===========================================================================
FUNCTION DS_PROFILE_DELETE 
============================================================================*/
/** @ingroup ds_profile_delete
  Resets a profile to undefined and return to free pool. 
  Not allowed for default profiles

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech  Technology.
  @param[in] num   Profile number to be deleted.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported  
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_delete ( 
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION DS_PROFILE_DELETE_EX
============================================================================*/
/** @ingroup ds_profile_delete_ex
  Resets a profile to undefined and return to free pool. 
  Default profiles can be deleted only if the flag is explicitly set to true

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #boolean\n


  @vertspace
  @param[in] tech                              Technology.
  @param[in] num                               Profile number to be deleted.
  @param[in] enable_deleting_default_profile   Enable deleting default profile
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported  
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_delete_ex ( 
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  boolean                enable_deleting_default_profile
);

/*===========================================================================
FUNCTION DS_PROFILE_DELETE_PER_SUB 
============================================================================*/
/** @ingroup ds_profile_delete_per_sub
  Resets a profile to undefined and return to free pool. 
  Not allowed for default profiles

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech  Technology.
  @param[in] num   Profile number to be deleted.
  @param[in] subs  Subscription ID
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported  
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_delete_per_sub ( 
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  ds_profile_subs_etype  subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM 
============================================================================*/
/** @ingroup ds_profile_get_param
  Gets the profile data element identified by the identifier. The identifiers 
  are specified in the corresponding technology header file. The data elements 
  are read from the prefetched profile, and information is returned with that 
  value and length.
 
  @datatypes
  #ds_profile_hndl_type\n
  #ds_profile_identifier_type\n
  ds_profile_info_type

  @vertspace
  @param[in] profile_hndl  Handle to the profile to get the profile data 
                           element.
  @param[in] identifier    Identifier of the profile data element.
  @param[out] info         Pointer to the stored value and length of the data 
                           element. The size of the buffer allocated is to be 
                           atleast be the maximum size of the parameter to be 
                           fetched.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_param (  
  ds_profile_hndl_type        profile_hndl,
  ds_profile_identifier_type  identifier,
  ds_profile_info_type       *info
);

/*===========================================================================
FUNCTION DS_PROFILE_SET_PARAM 
============================================================================*/
/** @ingroup ds_profile_set_param
  Sets the profile data element identified by the identifier. The identifiers 
  are specified in the corresponding technology header file. The prefetched 
  copy is modified. The ds_profile_end_transaction() function modifies the 
  profile on the modem. 
 
  @datatypes
  #ds_profile_hndl_type\n
  #ds_profile_identifier_type\n
  #ds_profile_info_type

  @vertspace
  @param[in] profile_hndl  Handle to profile to set profile data elements.
  @param[in] identifier    Identifies the profile data elements.
  @param[in] info          Pointer to the value to which the data element is to 
                           be set. The size of the buffer passed can be up to 
                           the maximum size of the parameter that needs to be 
                           set.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_set_param (  
  ds_profile_hndl_type         profile_hndl,
  ds_profile_identifier_type   identifier,
  const ds_profile_info_type  *info
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_PARAM_TO_DEFAULT 
============================================================================*/
/** @ingroup ds_profile_reset_param_to_default
  Resets a parameter value in a profile to the default. This function
  directly changes the value on the modem, so a call to begin/end a transaction 
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_identifier_type

  @vertspace
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[in] ident   Identifies the profile parameter to be set to the default.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_param_to_default (  
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_PARAM_TO_DEFAULT_PER_SUB 
============================================================================*/
/** @ingroup ds_profile_reset_param_to_default
  Resets a parameter value in a profile to the default. This function
  directly changes the value on the modem, so a call to begin/end a transaction 
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_identifier_type

  @vertspace
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[in] ident   Identifies the profile parameter to be set to the default.
  @param[in] subs_id Subscription ID
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_param_to_default_per_sub (  
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident,
  ds_profile_subs_etype       subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_PARAM_TO_INVALID 
============================================================================*/
/** @ingroup ds_profile_reset_param_to_invalid
  Resets a parameter value in a profile to the default. This function
  directly changes the value on the modem, so a call to begin/end a transaction 
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_identifier_type

  @vertspace
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[in] ident   Identifies the profile parameter to be set to the default.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_param_to_invalid (  
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident
);


/*===========================================================================
FUNCTION DS_PROFILE_RESET_PARAM_TO_INVALID_PER_SUBS 
============================================================================*/
/** @ingroup ds_profile_reset_param_to_invalid_per_subs
  Resets a parameter value in a profile to the default. This function
  directly changes the value on the modem, so a call to begin/end a transaction 
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type\n
  #ds_profile_identifier_type

  @vertspace
  @param[in] tech    Technology.
  @param[in] num     Profile number.
  @param[in] ident   Identifies the profile parameter to be set to the default.
  @param[in] subs_id Subscription ID
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_param_to_invalid_per_sub (  
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident,
  ds_profile_subs_etype       subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_PROFILE_TO_DEFAULT 
============================================================================*/
/** @ingroup ds_profile_reset_profile_to_default
  Resets all the parameters of the profile to the default. This function 
  directly changes the value on the modem, so a call to begin/end a transaction
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech   Technology.
  @param[in] num    Profile number.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_profile_to_default (  
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_PROFILE_TO_DEFAULT_PER_SUBS 
============================================================================*/
/** @ingroup ds_profile_reset_profile_to_default_per_subs
  Resets all the parameters of the profile to the default. This function 
  directly changes the value on the modem, so a call to begin/end a transaction
  is not required before/after this function.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech   Technology.
  @param[in] num    Profile number.
  @param[in] subs_id Subscription ID

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_reset_profile_to_default_per_sub (  
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  ds_profile_subs_etype  subs_id
);


/*===========================================================================
FUNCTION ds_profile_reset_pdp_context

DESCRIPTION
  This function is used to send a request to reset a pdp context 
  value to default.

PARAMETERS 
  tech - technology
  num  - profile number


DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_reset_pdp_context(
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num
);

/*===========================================================================
FUNCTION ds_profile_reset_pdp_context_per_sub

DESCRIPTION
  This function is used to send a request to reset a pdp context 
  value to default.

PARAMETERS 
  tech - technology
  num  - profile number
  subs_id - Subscription ID


DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_reset_pdp_context_per_sub(
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_subs_etype       subs_id
);


/*===========================================================================
FUNCTION DS_PROFILE_SET_DEFAULT_PROFILE_NUM 
============================================================================*/
/** @ingroup ds_profile_set_default_profile_num
  Sets the given profile number as default profile for the family of the 
  specified technology.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech    Technology type.
  @param[in] family  Profile family.
  @param[in] num     Profile number.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_set_default_profile_num (  
  ds_profile_tech_etype  tech,
  uint32                 family, 
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION DS_PROFILE_SET_DEFAULT_PROFILE_NUM_PER_SUBS 
============================================================================*/
/** @ingroup ds_profile_set_default_profile_num_per_subs
  Sets the given profile number as default profile for the family of the 
  specified technology and subscription.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_subs_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech      Technology type. 
  @param[in] family    Profile family.  
  @param[in] subs_id   Subscription ID. Supported values: subs_id 1 or subs_id 2.
  @param[in] num       Profile number.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID      -- Invalid subscription ID.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_set_default_profile_num_per_subs (
  ds_profile_tech_etype  tech,
  uint32                 family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_DEFAULT_PROFILE_NUM 
============================================================================*/
/** @ingroup ds_profile_get_default_profile_num
  Gets the default profile number for the family of the specified technology.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech     Technology type.
  @param[in] family   Profile family.
  @param[out] num     Pointer to store default profile number.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_default_profile_num (  
  ds_profile_tech_etype   tech,
  uint32                  family, 
  ds_profile_num_type    *num
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_DEFAULT_PROFILE_NUM_PER_SUBS 
============================================================================*/
/** @ingroup ds_profile_get_default_profile_num_per_subs
  Gets the default profile number for the family of the specified technology 
  and subscription.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_subs_etype\n
  #ds_profile_num_type

  @vertspace
  @param[in] tech     Technology type. 
  @param[in] family   Profile family.  
  @param[in] subs_id  Subscription ID. Supported values: subs_id 1 or subs_id 2.
  @param[out] num     Profile number.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID      -- Invalid subscription ID.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_default_profile_num_per_subs (  
  ds_profile_tech_etype   tech,
  uint32                  family,
  ds_profile_subs_etype   subs_id,
  ds_profile_num_type     *num
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_MAX_NUM 
============================================================================*/
/** @ingroup ds_profile_get_max_num
  Returns the maximum number of profiles possible for a given technology type.
 
  @datatypes
  #ds_profile_tech_etype

  @vertspace
  @param[in] tech       Technology type.
  @param[out] max_num   Pointer to the value that indicates the maximum number 
                        of profiles possible to be stored.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_max_num (  
  ds_profile_tech_etype   tech,
  uint32                 *max_num
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_LIST_ITR 
============================================================================*/
/** @ingroup ds_profile_get_list_itr
  Gets the list of profiles of a particular technology type. This 
  function returns an iterator. The iterator is traversed using 
  ds_profile_itr_next(). After traversal is complete, the caller is 
  expected to call ds_profile_itr_destroy().
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_list_type\n
  #ds_profile_itr_type

  @vertspace
  @param[in] tech   Technology type.
  @param[in] lst    Pointer to a list of profiles generated from a search.
  @param[out] itr   Pointer to an iterator to traverse through the search 
                    results.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_list_itr ( 
  ds_profile_tech_etype  tech,
  ds_profile_list_type  *lst,
  ds_profile_itr_type   *itr
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_LIST_ITR_PER_SUB 
============================================================================*/
/** @ingroup ds_profile_get_list_itr
  Gets the list of profiles of a particular technology type. This 
  function returns an iterator. The iterator is traversed using 
  ds_profile_itr_next(). After traversal is complete, the caller is 
  expected to call ds_profile_itr_destroy().
 
  @datatypes
  #ds_profile_tech_etype\n
  #ds_profile_list_type\n
  #ds_profile_itr_type

  @vertspace
  @param[in] tech   Technology type.
  @param[in] lst    Pointer to a list of profiles generated from a search.
  @param[out] itr   Pointer to an iterator to traverse through the search 
                    results.
  @param[in] subs_id   Subscription ID. Supported values: subs_id 1 or subs_id 2.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid profile type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_list_itr_per_sub ( 
  ds_profile_tech_etype  tech,
  ds_profile_list_type  *lst,
  ds_profile_itr_type   *itr,
  ds_profile_subs_etype  subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_ITR_NEXT 
============================================================================*/
/** @ingroup ds_profile_itr_next
  Advances the iterator to the next element.

  @datatypes
  #ds_profile_itr_type

  @vertspace
  @param[in] itr   Iterator to traverse through the list.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS    -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_FAIL       -- General failure.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL  -- Invalid argument (iterator).\n
  DS_PROFILE_REG_RESULT_LIST_END   -- End of the list. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_itr_next (  
  ds_profile_itr_type   itr
);

/*===========================================================================
FUNCTION DS_PROFILE_ITR_FIRST 
============================================================================*/
/** @ingroup ds_profile_itr_first
  Resets the iterator to the beginning of the list.

  @datatypes
  #ds_profile_itr_type

  @vertspace
  @param[in] itr Iterator to traverse through the list.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS    -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_FAIL       -- General failure.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL  -- Invalid argument (iterator).

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_itr_first ( 
  ds_profile_itr_type   itr
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_INFO_BY_ITR 
============================================================================*/
/** @ingroup ds_profile_get_info_by_itr
  Gets the information stored in the iterator node. 

  @datatypes
  #ds_profile_itr_type\n
  #ds_profile_list_info_type

  @vertspace
  @param[in] itr         Iterator to traverse through the list.
  @param[out] list_info  Pointer to the structure to return the profile 
                         information.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS    -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_FAIL       -- General failure.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL  -- Invalid argument (iterator).

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_info_by_itr ( 
  ds_profile_itr_type         itr,
  ds_profile_list_info_type  *list_info
);

/*===========================================================================
FUNCTION DS_PROFILE_ITR_DESTROY 
============================================================================*/
/** @ingroup ds_profile_itr_destroy
  Destroys the iterator.

  @datatypes
  #ds_profile_itr_type

  @vertspace
  @param[in] itr   Iterator to traverse through the list.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS    -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_FAIL       -- General failure.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL  -- Invalid argument (iterator).

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_itr_destroy ( 
  ds_profile_itr_type   itr
);

/*===========================================================================
FUNCTION TYPE DS_PROFILE_CB_TYPE
============================================================================*/
/** @ingroup ds_profile_cb_type
  Invoked when changes occur to the observed profile / technology

  *** Warning ***
  1.  No "heavy" or potentially blocking operations 
      should be performed withing the context of the callback 
      (post a command to some other task instead)
      Also, no ds profile API's should be invoked.       
  2.  profiles_changes array should be released using 
      ds_profile_release_profiles_changes_array()
  
  @datatypes
  #ds_profile_event_etype\n
  #ds_profile_changes_info\n  

  @vertspace
  @param[in] event            Event type  
  @param[in] profile_count    The amount of nodes in profiles_changes array.
                              If 0, profiles_changes will be NULL.
  @param[in] profiles_changes The array of nodes with changes per each profile 
                              relevant to the event. Each node will contain a 
                              profile number , profile technology and array of identifiers 
                              that were changed in that profile.
                              The array may contain DS_PROFILE_3GPP2_PROFILE_PARAM_INVALID
                                                    DS_PROFILE_3GPP_PROFILE_PARAM_INVALID
                              those imply that some parameters not supported by ds profile were
                              changed internally by ds_profil_db. Should be ignored.                              

                              DS_PROFILE_CREATE_PROFILE_EVENT - the array will contain a single 
                              profile that was created. The array of changed identifiers 
                              will contain the idents that were explicitly specified during 
                              creation (currently can only contain 
                              DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID)

                              DS_PROFILE_DELETE_PROFILE_EVENT - the array will contain a single
                              profile that was deleted. The array of changed identifiers 
                              will be empty (NULL)

                              DS_PROFILE_MODIFY_PROFILE_EVENT - the array will contain a single
                              profile that was modified. The array of changed identifiers 
                              will contain the idents that were modified during a single
                              transaction. 

                              DS_PROFILE_RESET_PROFILE_EVENT - the array will contain a single
                              profile that was reset. The array of changed identifiers 
                              will be empty (NULL)

  @param[in] user_data        data provided by user during registration
*/
typedef void (*ds_profile_cb_type) (
  ds_profile_event_etype event,  
  unsigned int profile_count,
  ds_profile_changes_info* profiles_changes,
  void* user_data
);

/*===========================================================================
FUNCTION TYPE DS_PROFILE_CB_TYPE_PER_SUB
============================================================================*/
/** @ingroup ds_profile_cb_type_per_sub
  Invoked when changes occur to the observed profile / technology

  *** Warning ***
  1.  No "heavy" or potentially blocking operations 
      should be performed withing the context of the callback 
      (post a command to some other task instead)
      Also, no ds profile API's should be invoked.       
  2.  profiles_changes array should be released using 
      ds_profile_release_profiles_changes_array()
  3.  Please use the registration per subscription and older
      registration wihout subscription will be deprecated.
  
  @datatypes
  #ds_profile_event_etype\n
  #ds_profile_changes_info\n  

  @vertspace
  @param[in] event            Event type  
  @param[in] profile_count    The amount of nodes in profiles_changes array.
                              If 0, profiles_changes will be NULL.
  @param[in] profiles_changes The array of nodes with changes per each profile 
                              relevant to the event. Each node will contain a 
                              profile number , profile technology and array of identifiers 
                              that were changed in that profile.
                              The array may contain DS_PROFILE_3GPP2_PROFILE_PARAM_INVALID
                                                    DS_PROFILE_3GPP_PROFILE_PARAM_INVALID
                              those imply that some parameters not supported by ds profile were
                              changed internally by ds_profil_db. Should be ignored.                              

                              DS_PROFILE_CREATE_PROFILE_EVENT - the array will contain a single 
                              profile that was created. The array of changed identifiers 
                              will contain the idents that were explicitly specified during 
                              creation (currently can only contain 
                              DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID)

                              DS_PROFILE_DELETE_PROFILE_EVENT - the array will contain a single
                              profile that was deleted. The array of changed identifiers 
                              will be empty (NULL)

                              DS_PROFILE_MODIFY_PROFILE_EVENT - the array will contain a single
                              profile that was modified. The array of changed identifiers 
                              will contain the idents that were modified during a single
                              transaction. 

                              DS_PROFILE_RESET_PROFILE_EVENT - the array will contain a single
                              profile that was reset. The array of changed identifiers 
                              will be empty (NULL)

  @param[in] user_data        data provided by user during registration
  @param[in] subs_id          subscription_id
*/
typedef void (*ds_profile_cb_type_per_sub) (
  ds_profile_event_etype event,  
  unsigned int profile_count,
  ds_profile_changes_info* profiles_changes,
  void* user_data,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_REGISTER_CALLBACK
============================================================================*/
/** @ingroup ds_profile_register_callback
  Registers for notifications triggered by changes in profiles.
  User can watch either specific profile, or all profiles of a particular technology.
 
  @datatypes
  #ds_profile_num_type\n
  #ds_profile_tech_etype\n
  #ds_profile_cb_handle_type\n
  #ds_profile_cb_type\n

  @vertspace
  @param[in] num           Profile number to observe. If you do not wish to observe a particular
                           profile, use DS_PROFILE_UNSPECIFIED_NUM
                           Registration per particular number and technology is only allowed 
                           for existing profile with matching technology
  @param[in] tech          Technology to observe (if a particular profile is observed, technology should
                           match profile's technology)
                           Events will be triggered for technologies that not just match the provided
                           tech, but also support it. This mainly concerns EPC profiles. 
                           For example, if user register for 3GPP, but there is an EPC profile which had some
                           modification of it's 3GPP part, the event will be send as well (the technology
                           that will be specified in callback info - EPC). Please note, there will be 
                           no event to 3GPP clients if only 3GPP2 part was modified. 
                           For create/delete/reset - events will always be send for all underlying clients, 
                           which is currently 3GPP / 3GPP2
  @param[in] cback         Function to be called upon event
  @param[in] user_data     User can provide data to be passed to callback function upon event
 
  @return
  Valid cb handle in case of success, 0 in case of failure

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_cb_handle_type ds_profile_register_callback(
  ds_profile_num_type num,
  ds_profile_tech_etype tech, 
  ds_profile_cb_type cback,
  void* user_data
);

/*===========================================================================
FUNCTION DS_PROFILE_REGISTER_CALLBACK_PER_SUB
============================================================================*/
/** @ingroup ds_profile_register_callback_per_sub
  Registers for notifications triggered by changes in profiles.
  User can watch either specific profile, or all profiles of a particular technology.
 
  @datatypes
  #ds_profile_num_type\n
  #ds_profile_tech_etype\n
  #ds_profile_cb_handle_type\n
  #ds_profile_cb_type\n

  @vertspace
  @param[in] num           Profile number to observe. If you do not wish to observe a particular
                           profile, use DS_PROFILE_UNSPECIFIED_NUM
                           Registration per particular number and technology is only allowed 
                           for existing profile with matching technology
  @param[in] tech          Technology to observe (if a particular profile is observed, technology should
                           match profile's technology)
                           Events will be triggered for technologies that not just match the provided
                           tech, but also support it. This mainly concerns EPC profiles. 
                           For example, if user register for 3GPP, but there is an EPC profile which had some
                           modification of it's 3GPP part, the event will be send as well (the technology
                           that will be specified in callback info - EPC). Please note, there will be 
                           no event to 3GPP clients if only 3GPP2 part was modified. 
                           For create/delete/reset - events will always be send for all underlying clients, 
                           which is currently 3GPP / 3GPP2
  @param[in] cback         Function to be called upon event
  @param[in] user_data     User can provide data to be passed to callback function upon event
  @param[in] subs_id       Subscription ID 
 
  @return
  Valid cb handle in case of success, 0 in case of failure

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_cb_handle_type ds_profile_register_callback_per_sub(
  ds_profile_num_type num,
  ds_profile_tech_etype tech, 
  ds_profile_cb_type_per_sub cback,
  void* user_data,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DS_PROFILE_UNREGISTER_CALLBACK
============================================================================*/
/** @ingroup ds_profile_unregister_callback
  Unregisters from notifications  
 
  @datatypes
  #ds_profile_cb_handle_type\n

  @vertspace
  @param[in] cb_handle     Cb handle obtained by ds_profile_register_callback()
   
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Invoked from within callback
  DS_PROFILE_REG_RESULT_ERR_INVAL              -- Invalid callback id
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_unregister_callback(
  ds_profile_cb_handle_type cb_handle
);


//Debug : debatable
/*===========================================================================
FUNCTION DS_PROFILE_UNREGISTER_CALLBACK_PER_SUB
============================================================================*/
/** @ingroup ds_profile_unregister_callback_per_sub
  Unregisters from notifications  
 
  @datatypes
  #ds_profile_cb_handle_type\n

  @vertspace
  @param[in] cb_handle     Cb handle obtained by ds_profile_register_callback()
  @param[in] subs_id       Subscription Id.
   
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Invoked from within callback
  DS_PROFILE_REG_RESULT_ERR_INVAL              -- Invalid callback id
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_unregister_callback_per_sub(
  ds_profile_cb_handle_type cb_handle,
  ds_profile_subs_etype subs_id
);


/*===========================================================================
FUNCTION DS_PROFILE_RELEASE_PROFILES_CHANGES_ARRAY
============================================================================*/
/** @ingroup ds_profile_release_profiles_changes_array
  Release profiles_changes array received as part in callback function.
  Also releases internal arrays per every node
 
  @datatypes
  #ds_profile_changes_info\n  

  @vertspace
  @param[in] profiles_changes - array to be released
  @param[in] profile_count -   number of profiles in array
 
  @dependencies 
  The software library must be initialized.
  @newpage
*/
void ds_profile_release_profiles_changes_array(
  ds_profile_changes_info* profiles_changes,
  unsigned int profile_count
);


#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_PROFILE_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES 
============================================================================*/
/** @ingroup ds_profile_update_lte_attach_pdn_list_profiles
  Updates the profile parameters in the lte attach pdn list of the 
  specified technology.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype
  
  @vertspace
  @param[in] tech    Technology type.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_update_lte_attach_pdn_list_profiles (  
  ds_profile_tech_etype  tech
);

/*===========================================================================
FUNCTION DS_PROFILE_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES_PER_SUBS 
============================================================================*/
/** @ingroup ds_profile_update_lte_attach_pdn_list_profiles_per_subs
  Updates the profile parameters in the lte attach pdn list of the 
  specified technology.

  @note1hang Not all technology types support this operation.
 
  @datatypes
  #ds_profile_tech_etype
  
  @vertspace
  @param[in] tech    Technology type.
  @param[in] subs_id Subscription ID. Supported values: subs_id 1 or subs_id 2.

  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP           -- Operation is not supported 
                                                  for the technology type.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_update_lte_attach_pdn_list_profiles_per_subs (  
  ds_profile_tech_etype  tech,
  ds_profile_subs_etype  subs_id
);
#endif /* FEATURE_DATA_LTE */



/** @ingroup ds_profile_get_tech_type_from_profile_num
  Get the tech type given the profile number 
 
  @datatypes
  #ds_profile_num_type
  
  @vertspace
  @param[in]  profile_num Profile number
  @param[out] tech        Technology type.

  @return 
  ds_profile_tech_etype 
   
  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_tech_etype ds_profile_get_tech_type_from_profile_num(
  ds_profile_num_type profile_num
);


/** @ingroup ds_profile_get_tech_type_from_profile_num_per_sub
  Get the tech type given the profile number 
 
  @datatypes
  #ds_profile_num_type
  
  @vertspace
  @param[in]  profile_num Profile number
  @param[out] tech        Technology type.
  @param[in] subs_id      Subscription ID. Supported values: 
                          subs_id 1 or subs_id 2.

  @return 
  ds_profile_tech_etype 
   
  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_tech_etype ds_profile_get_tech_type_from_profile_num_per_sub(
  ds_profile_num_type profile_num,
  ds_profile_subs_etype subs_id
);


/** @ingroup ds_profile_get_persitence_from_profile_num
  Get whether the given profile number is persistent or not.
 
  @datatypes
  #ds_profile_num_type
  
  @vertspace
  @param[in] profile_num        Profile Number. 
  @param[in_out] is_persistent  Persistence Flag 
   
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 
   
  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_persistence_from_profile_num(
  ds_profile_num_type profile_num,
  boolean *is_persistent
);

/** @ingroup ds_profile_get_persitence_from_profile_num_per_sub
  Get whether the given profile number is persistent or not.
 
  @datatypes
  #ds_profile_num_type
  
  @vertspace
  @param[in] profile_num        Profile Number. 
  @param[in_out] is_persistent  Persistence Flag. 
  @param[in] subs_id            Subscription ID. Supported 
                                values: subs_id 1 or subs_id 2.
   
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 
   
  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_persistence_from_profile_num_per_sub(
  ds_profile_num_type profile_num,
  boolean *is_persistent,
  ds_profile_subs_etype subs_id
);


/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM_IN_USE
============================================================================*/
/** @ingroup ds_profile_get_param_in_use
  Gets the profile data element identified by the identifier given the tech type.
  The identifiers are specified in the corresponding technology header file.
  The data elements are read from the prefetched profile, and information is
  returned with that value and length. The tech should be of EPC and identifier
  should be Common parameters
 
  @datatypes
  #ds_profile_hndl_type\n
  #ds_profile_identifier_type\n
  ds_profile_info_type

  @vertspace
  @param[in] profile_hndl  Handle to the profile to get the profile data 
                           element.
  @param[in] identifier    Identifier of the profile data element.
  @param[out] info         Pointer to the stored value and length of the data 
                           element. The size of the buffer allocated is to be 
                           atleast be the maximum size of the parameter to be 
                           fetched.
  @param[in] tech_type     Tech Type for which the info is needed.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_param_in_use (  
  ds_profile_hndl_type        profile_hndl,
  ds_profile_identifier_type  identifier,
  ds_profile_info_type       *info,
  ds_profile_tech_etype       tech_type
);


/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM_IN_USE_PER_SUB
============================================================================*/
/** @ingroup ds_profile_get_param_in_use_per_sub
  Gets the profile data element identified by the identifier given the tech type.
  The identifiers are specified in the corresponding technology header file.
  The data elements are read from the prefetched profile, and information is
  returned with that value and length. The tech should be of EPC and identifier
  should be Common parameters
 
  @datatypes
  #ds_profile_hndl_type\n
  #ds_profile_identifier_type\n
  ds_profile_info_type

  @vertspace
  @param[in] profile_hndl  Handle to the profile to get the profile data 
                           element.
  @param[in] identifier    Identifier of the profile data element.
  @param[out] info         Pointer to the stored value and length of the data 
                           element. The size of the buffer allocated is to be 
                           atleast be the maximum size of the parameter to be 
                           fetched.
  @param[in] tech_type     Tech Type for which the info is needed.
  @param[in] subs_id       Subscription ID. Supported values: subs_id 1 or subs_id 2.
 
  @return
  DS_PROFILE_REG_RESULT_SUCCESS                -- Operation succeeded.\n
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED     -- Library is not initialized.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL         -- Invalid handle.\n 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT        -- Invalid identifier.\n
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID        -- Buffer size is less than 
                                                  required.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE -- Invalid technology type.\n
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  -- Invalid profile number.\n
  DS_PROFILE_REG_RESULT_FAIL                   -- General failure. 

  @dependencies 
  The software library must be initialized.
  @newpage
*/
ds_profile_status_etype ds_profile_get_param_in_use_per_sub (  
  ds_profile_hndl_type        profile_hndl,
  ds_profile_identifier_type  identifier,
  ds_profile_info_type       *info,
  ds_profile_tech_etype       tech_type,
  ds_profile_subs_etype       subs_id
);


/** @ingroup DS_PROFILE_IS_VALID_PROFILE_PER_SUB
  Converys the profile is valid for a given technology and 
  subscription 
 
  @datatypes
  #ds_profile_num_type
  
  @vertspace
  @param[in]  profile_num Profile number
  @param[out] tech        Technology type.
  @param[in] subs_id      Subscription ID. Supported values: 
                          subs_id 1 or subs_id 2.

  @return 
  ds_profile_tech_etype 
   
  @dependencies 
  The software library must be initialized.
  @newpage
*/

ds_profile_status_etype ds_profile_is_valid_profile_per_sub(
  ds_profile_num_type profile_num,
  ds_profile_subs_etype subs_id,
  ds_profile_tech_etype tech_type,
  boolean *is_valid
);

#endif /* DS_PROFILE_H */
