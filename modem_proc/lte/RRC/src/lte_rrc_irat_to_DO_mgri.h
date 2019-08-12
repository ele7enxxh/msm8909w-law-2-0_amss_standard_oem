/*!
  @file
  lte_rrc_irat_to_DO_mgri.h

  @brief
  This module contains the entry, exit, and transition functions
  necessary to implement the following state machines:

  @detail
  LTE_RRC_DO_RAT_MGR ( 1 instance/s )


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_DO_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/11   np      Store pre-reg status as static data
06/01/10   np/sk   Support for RTT
01/13/10   sk      Added support for storing resel/redir request from CSP
12/06/09   sk      Added LTE --> DO reselection support
10/06/09   sk      Support for abort cause from CSP
09/02/2009 sk      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_DO_RATI_H
#define LTE_RRC_IRAT_TO_DO_RATI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//! @todo Include necessary files here

#include <sys.h>

#include "lte_rrc_ext_msg.h"
#include "hdrcp_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_cphy_msg.h"

/*===========================================================================

                           MACROS

===========================================================================*/
#define LTE_RRC_IRAT_TO_DO_MAX_RTT_TIME 2000 /* 2 seconds */
#define LTE_RRC_IRAT_TO_DO_CGI_LENGTH_IN_BYTES 16  /*<! DO CGI length in bytes */

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various resume causes
*/
typedef enum
{
    /*! LTE stack resumed due to procedure failure */
    LTE_RRC_TO_DO_PROC_FAILED = 0,

    /*! LTE stack resumed due to procedure being aborted */
    LTE_RRC_TO_DO_PROC_ABORTED = 1,

    /*! LTE stack resumed due to completion of RTT procedure (success/failure) */
    LTE_RRC_TO_DO_PROC_NORMAL_COMPLETION = 2,

    /*! DO PROC NONE */
    LTE_RRC_TO_DO_PROC_NONE = 3,

}lte_rrc_to_DO_resume_cause_type_e;

/*! @brief Enum to represent various states when pre-reg can be sent to eHRPD stack
*/
typedef enum
{
  /*! Pre-reg info not sent since activation/power up */
  LTE_RRC_TO_DO_PRE_REG_ST_NONE = 0,

  /*! Pre-reg info sent from SIB8 */
  LTE_RRC_TO_DO_PRE_REG_FROM_SIB8 = 1,

  /*! Pre-reg info sent from Reconfig with no Handover */
  LTE_RRC_TO_DO_PRE_REG_FROM_RECONFIG_NO_HO = 2,

  /*! Pre-reg info sent from Reconfig with Handover */
  LTE_RRC_TO_DO_PRE_REG_FROM_RECONFIG_WITH_HO = 3,

}lte_rrc_to_DO_pre_reg_state_e;

/*! @brief Structure for redirection to DO freq per band list
*/
typedef struct
{
  /*!< Band Class */
  sys_band_class_e_type band_class;

  /*!< Num frequencies per band*/
  uint8 freq_count;

  /*!< Frequency - 0..2047 */
  sys_channel_num_type 
      freq_list[LTE_RRC_MAX_CDMA2000_FREQUENCIES_PER_BAND];

} lte_rrc_to_DO_freq_per_band_list;

/*! @brief Structure for redirection to DO band-freq list
*/
typedef struct
{
  /*!< Num neighboring bands*/
  uint8 band_count; 

  /*!< Channel info */
  lte_rrc_to_DO_freq_per_band_list 
    freq_per_band_list[LTE_RRC_MAX_CDMA2000_BANDS];

}lte_rrc_to_DO_redir_s;

/*! @brief Structure for reselection to DO
*/
typedef struct
{
  /*!< Band and Frequency  */
  sys_channel_type               resel_info;

  /*!< Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type              pilot_pn;

}lte_rrc_to_DO_resel_s;

/*! @brief Structure for CGI reading in DO
*/
typedef struct
{
  /*!< DO Get CGI Req*/
  lte_rrc_irat_from_lte_to_do_get_cgi_reqi_s req;

  /*!< DO Get CGI Rsp*/
  lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s rsp;

}lte_rrc_to_DO_cgi_s;

/*! @brief Structure for storing the inter-RAT procedure details
*/
typedef struct
{
  /*!< Procedure indicator */
  lte_rrc_irat_proc_e   proc_type;

  union
  {
    /*!< Redirection List Parameters */
    lte_rrc_to_DO_redir_s redir_list;

    /*!< Reselection parameters */
    lte_rrc_to_DO_resel_s resel;

    /*!< Reverse Time Tx parameters */
    lte_cphy_cdma_layer_info_s   rtt;

    /* !< HDR IRAT HO related info */
    lte_irat_tunnel_msg_container_s     ho;

    /* !< HDR IRAT CGI related info */
    lte_rrc_to_DO_cgi_s cgi;

  }proc;

}lte_rrc_to_DO_proc_s;


/*! Dynamic private data 
*/
typedef struct
{
  /*!< Inter-RAT procedure details */
  lte_rrc_to_DO_proc_s to_DO_proc_info;

  /* !< Abort cause if an Abort request is received */
  lte_rrc_irat_abort_cause_type_e  abort_cause;

  /*!< Abort Proc type */
  lte_rrc_proc_e abort_proc;

  /* !< Resume cause if the LTE stack is being resumed */
  lte_rrc_to_DO_resume_cause_type_e resume_cause;

  /* !< Flag to indicate if SIB8 available indication was provided earlier  */
  boolean sib8_ind_sent;

  /* !< LTE cell info field validity */
  boolean lte_cell_info_is_valid;

  /* !< LTE cell for which pre-reg info was sent last */
  lte_rrc_cell_info_s           cell_info;

  /* !< LTE RRC state when last Pre-reg info was sent */
  lte_rrc_to_DO_pre_reg_state_e last_pre_reg_info_state;

  /* !< Pre-reg info to be sent to DO */
  lte_rrc_hdr_pre_reg_info_ind_s last_pre_reg_info;

  stm_state_t  state_prior_to_suspend;


}lte_rrc_irat_to_DO_dynamic_data_s;

typedef struct
{
  boolean                       pre_reg_status;
} lte_rrc_irat_to_DO_static_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /* Dynamic data pointer */
  lte_rrc_irat_to_DO_dynamic_data_s *dd_ptr;
  lte_rrc_irat_to_DO_static_data_s  *sd_ptr;
  
} lte_rrc_irat_to_DO_s;


#endif /* LTE_RRC_IRAT_TO_DO_RATI_H */


