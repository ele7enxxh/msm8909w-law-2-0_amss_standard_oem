/*!
  @file
  lte_rrc_irat_to_1x_mgri.h

  @brief

  @detail

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_1x_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/10   np      New modes for CSFB HO
02/15/10   np      Support for new IRAT Abort causes
12/04/09   np      LTE -> 1x IRAT Resel Support
10/06/2009 np      Abort Cause
09/21/2009 np      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_1X_MGRI_H
#define LTE_RRC_IRAT_TO_1X_MGRI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_rrc_irat_to_1x_mgr.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

#define LTE_RRC_IRAT_TO_1X_CGI_LENGTH_IN_BYTES    6   /*! 1X CGI length in bytes */


typedef enum
{
  /*! Default value */
  LTE_RRC_IRAT_TO_1X_MODE_INITIAL            = 0,

  /*! IRAT Resel to 1x started */
  LTE_RRC_IRAT_TO_1X_MODE_RESEL_STARTED      = 1,

  /*! IRAT Resel to 1x failed */
  LTE_RRC_IRAT_TO_1X_MODE_RESEL_FAILED       = 2,

  /*! IRAT Resel to 1x aborted */
  LTE_RRC_IRAT_TO_1X_MODE_RESEL_ABORTED      = 3,

  /*! IRAT Redir to 1x Started */
  LTE_RRC_IRAT_TO_1X_MODE_REDIR_STARTED      = 4,

  /*! IRAT Redir to 1x Failed */
  LTE_RRC_IRAT_TO_1X_MODE_REDIR_FAILED       = 5,

  /*! IRAT Redir to 1x Aborted */
  LTE_RRC_IRAT_TO_1X_MODE_REDIR_ABORTED      = 6,

  /*! IRAT e1xCSFB HO to 1x Started */
  LTE_RRC_IRAT_TO_1X_MODE_HO_STARTED         = 7,

  /*! IRAT e1xCSFB HO to 1x Failed */
  LTE_RRC_IRAT_TO_1X_MODE_HO_FAILED          = 8,

  /*! IRAT e1xCSFB HO to 1x Failed */
  LTE_RRC_IRAT_TO_1X_MODE_HO_ABORTED         = 9,

  /*! IRAT CGI to 1x Started */
  LTE_RRC_IRAT_TO_1X_MODE_CGI_STARTED        = 10,

  /*! IRAT CGI to 1x Failed */
  LTE_RRC_IRAT_TO_1X_MODE_CGI_FAILED         = 11,

  /*! IRAT CGI to 1x Aborted */
  LTE_RRC_IRAT_TO_1X_MODE_CGI_ABORTED        = 12,

  /*! IRAT CGI to 1x Aborted due to CONN ABORT*/
  LTE_RRC_IRAT_TO_1X_MODE_CGI_ABORTED_CONN_ABORT        = 13,

} lte_rrc_irat_to_1x_mode_e;


/*! @brief Structure for CGI reading in 1X
*/
typedef struct
{
  /*!< 1X Get CGI Req*/
  lte_rrc_irat_from_lte_to_1x_get_cgi_reqi_s req;

  /*!< 1X Get CGI Rsp*/
  lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s rsp;

}lte_rrc_to_1x_cgi_s;

typedef union
{
  /* !< 1x Cell Info for reselections */
  lte_cphy_cdma_cell_info_s           cdma_cell_info;

  /* !< 1x IRAT Redirection related info */
  lte_rrc_redir_info_s                redir_info;

  /* !< 1x IRAT HO related info */
  lte_irat_tunnel_msg_container_s     ho_info;

  /* !< 1x IRAT CGI related info */
  lte_rrc_to_1x_cgi_s                 cgi;

}lte_rrc_from_1x_req_proc_u;

/*! @brief Structure for storing dynamic IRAT data
*/
typedef struct
{
  /*!< Current Inter-RAT procedure details */
  lte_rrc_irat_proc_e                 proc;

  lte_rrc_from_1x_req_proc_u          proc_req;

  /* !< Abort cause if an Abort request is received */
  lte_rrc_irat_abort_cause_type_e     abort_cause;

  /*!< Abort Proc type */
  lte_rrc_proc_e abort_proc;

  /* !< Boolean to keep track if SIB-8 was sent */
  boolean                             sib8_sent_earlier;

  /* !< Keeps track of IRAT mode */
  lte_rrc_irat_to_1x_mode_e           mode;

  /* !< LTE cell for which sib8 info was sent last */
  lte_rrc_cell_info_s                 cell_info;
} lte_rrc_irat_to_1x_mgr_dynamic_data_s;

/*! @brief Structure for storing static IRAT data
*/
typedef struct
{
  /*! Field to indicate if 1xCSFB is enabled based on the EFS item, non zero value
    indicates 1xCSFB reporting is enabled */
  uint8                               csfb_enabled;
  
} lte_rrc_irat_to_1x_mgr_static_data_s;

/*! @brief Structure for storing IRAT data
 */
typedef struct
{ 
  /* Dynamic data pointer */
  lte_rrc_irat_to_1x_mgr_dynamic_data_s     *dd_ptr;

  /* Static data pointer */
  lte_rrc_irat_to_1x_mgr_static_data_s      *sd_ptr;
  
} lte_rrc_irat_to_1x_mgr_s;

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* LTE_RRC_IRAT_TO_1X_MGRI_H */


