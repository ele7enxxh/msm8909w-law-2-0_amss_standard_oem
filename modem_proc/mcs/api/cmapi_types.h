#ifndef CMAPI_TYPES_H
#define CMAPI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  @file   cmapi_types.h
  @brief  This header file contains all the definitions necessary for clients 
          to interface with the Common Modem Data module.
*/

/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The cmapi_mainpage.dox file contains the group/module descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the group/module text in the PDF, edit the
      cmapi_mainpage.dox file or contact Tech Pubs.
=============================================================================*/

/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
=============================================================================*/
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/** @addtogroup cmapi_constants
@{ */
/** CMAPI equivalent definition for an assertion.  */
#define CMAPI_ASSERT(exp) ASSERT(exp)
/** @} */ /* end_addtogroup cmapi_constants */


/** @addtogroup cmapi_datatypes
@{ */
/** Error codes for the CMAPI.
*/
typedef enum cmapi_err_e{

  CMAPI_SUCCESS,             /**< Command was successful. */   
  CMAPI_INVALID_SYS_MODE,    /**< Input system mode is unsupported. */
  CMAPI_NO_SERVICE,          /**< No service on the input system mode. */
  CMAPI_INVALID_INFO_VALUE,  /**< CM received an invalid signal information 
                                  value. */
  CMAPI_INVALID_PARAM,       /**< Invalid input parameters. */
  CMAPI_LL_ERROR,            /**< Lower-layer function returns FALSE. */
  CMAPI_NOT_SUPPORTED        /**< Not supported. **/
}cmapi_err_e_type;

/** Call Type enumeration to be used for modem data reporting
*/
typedef enum{
  CMAPI_CALL_TYPE_NONE      = 0,
  CMAPI_CALL_TYPE_VOICE     = 1,
  CMAPI_CALL_TYPE_VIDEO     = 2,
  CMAPI_CALL_TYPE_EMERGENCY = 3,
  CMAPI_CALL_TYPE_VOLTE     = 4
} cmapi_call_type_e;

/** Call State enumeration to be used for modem data reporting
*/
typedef enum{
  CMAPI_CALL_STATE_NONE       = 0,
  CMAPI_CALL_STATE_OUTGOING   = 1,
  CMAPI_CALL_STATE_INCOMING   = 2, 
  CMAPI_CALL_STATE_CONNECTED  = 3,
  CMAPI_CALL_STATE_RELEASED   = 4
} cmapi_call_state_e;
/** @} */ /* end_addtogroup cmapi_datatypes */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*CMAPI_TYPES_H*/