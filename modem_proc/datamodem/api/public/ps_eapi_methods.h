
#ifndef PS_EAPI_METHODS_H
#define PS_EAPI_METHODS_H

/** 
  @file ps_eapi_methods.h 
  @brief 
   The EAP INTERNAL METHODS API header file.
*/

/*===========================================================================

   E X T E N S I B L E   A U T H E N T I C A T I O N   P R O T O C O L (EAP)
         I N T E R N A L  M E T H O D S  A P I   H E A D E R   F I L E
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
Copyright (c) 2006, 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/* group: eap */
/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


 when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/16/11    jee     Added Doxygen comments
02/24/11    jee     Added Doxygen comments
03/06/06    lti     Created module.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup constants 
  @{
*/
/** EAP password maximum length.
*/
#define EAPI_METHODS_PASSWORD_MAX_CHAR 255
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** @brief EAP password structure.
*/
typedef struct
{ 
  uint16  len;                                      
    /**< Length of the password. */
  uint8   password[EAPI_METHODS_PASSWORD_MAX_CHAR]; 
    /**< Password. */
}eapi_methods_password_type;

/** @brief Stores EAP MD5 credentials.
*/
typedef struct
{
  eap_identity_type              user_id;   /**< User ID.  */
  eapi_methods_password_type     pass;      /**< Password. */
}eapi_methods_md5_credentials_type;

/** Stores EAP OTP credentials.
*/
typedef eapi_methods_md5_credentials_type eapi_methods_otp_credentials_type;

/** Stores EAP GTC credentials.
*/
typedef eapi_methods_md5_credentials_type eapi_methods_gtc_credentials_type;

/** @brief Stores EAP internal methods credentials.
*/
typedef union
{
  eapi_methods_md5_credentials_type  md5;  /**< MD5 credentials. */
  eapi_methods_otp_credentials_type  otp;  /**< OTP credentials. */
  eapi_methods_gtc_credentials_type  gtc;  /**< GTC credentials. */
}eapi_methods_credentials_type;

/** Allows the EAP to retrieve the inner methods data.  
*/
typedef boolean (*eapi_methods_get_credentials_cback_type)
(
  eap_method_enum_type            type,           /**< EAP external method. */
  eapi_methods_credentials_type*  eap_credentials /**< Required credentials. */
);

/** @brief Authenticator manager structure.
*/
typedef struct
{
  eapi_methods_get_credentials_cback_type  eapi_methods_get_credentials_f_ptr;
    /**< Function pointer to get EAP method specific credentials. */
}eapi_methods_auth_mgr_type;

/** @} */ /* end_addtogroup datatypes */

#endif /* PS_EAPI_METHODS_H */
