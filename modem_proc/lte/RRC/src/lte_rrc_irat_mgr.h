/*!
  @file
  lte_rrc_irat_mgr.h

  @brief
  External API for RRC IRAT STMs - DO-RAT Mgr, G-RAT MGr, W-RAT Mgr, 1X-RAt Mgr.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/10   np/sk   New proc for LTE_RRC_IRAT_FROM_LTE_RTT_PROC
09/02/2009 sk      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_H
#define LTE_RRC_IRAT_H

/*! Max Requested PLMN list provided by HDR during ehrpd to LTE Reselection */
#define LTE_RRC_MAX_ENH_REQ_PLMN_LIST    (8)

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//! @todo Include necessary files here

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various inter-RAT procedures
*/
typedef enum
{
  /* Proc 0 is equivalent to no inter-RAT procedure in progress */
  LTE_RRC_IRAT_NO_PROC = 0,

  /*! From LTE Redirection procedure */
  LTE_RRC_IRAT_FROM_LTE_REDIR_PROC,     

  /*! From LTE Internal Redirection procedure */
  LTE_RRC_IRAT_FROM_LTE_INTERNAL_REDIR_PROC,  

  /*! To LTE Redirection procedure */
  LTE_RRC_IRAT_TO_LTE_REDIR_PROC, 

  /*! From LTE Reselection procedure */
  LTE_RRC_IRAT_FROM_LTE_RESEL_PROC,

  /*! From LTE Reverse Time Transfer (CDMA) procedure */
  LTE_RRC_IRAT_FROM_LTE_RTT_PROC,

  /*! To LTE Reselection procedure */
  LTE_RRC_IRAT_TO_LTE_RESEL_PROC,

  /*! From LTE CCO/NACC procedure - Applicable to LTE -> GERAN only */
  LTE_RRC_IRAT_FROM_LTE_CCO_NACC_PROC,

  /*! To LTE PCCO procedure - Applicable to GERAN -> LTE only */
  LTE_RRC_IRAT_TO_LTE_PCCO_PROC,

  /*! From LTE PLMN Search - Applicable to LTE -> GERAN/WCDMA only */
  LTE_RRC_IRAT_FROM_LTE_PLMN_SEARCH_PROC,

  /*! To LTE PLMN Search - Applicable to GERAN/WCDMA -> LTE only*/
  LTE_RRC_IRAT_TO_LTE_PLMN_SEARCH_PROC,

  /*! From LTE handover procedure */
  LTE_RRC_IRAT_FROM_LTE_HANDOVER_PROC,

  /*! To LTE handover procedure */
  LTE_RRC_IRAT_TO_LTE_HANDOVER_PROC,

  /*! From LTE 1x ECSFB procedure */
  LTE_RRC_IRAT_FROM_LTE_1XECSFB_PROC,

  /*! From LTE 1x SRVCC procedure */
  LTE_RRC_IRAT_FROM_LTE_1XSRVCC_PROC,

  /*! From LTE CGI Request procedure */
  LTE_RRC_IRAT_FROM_LTE_CGI_REQUEST_PROC,


}lte_rrc_irat_proc_e;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern boolean lte_rrc_irat_proc_is_in_progress (void);

extern boolean lte_rrc_irat_ho_from_lte_in_progress (void);

extern void lte_rrc_irat_from_G_init_mem(void);

extern void lte_rrc_irat_from_DO_init_mem(void);

extern void lte_rrc_irat_from_1x_init_mem(void);

extern void lte_rrc_irat_to_DO_init_mem(void);

extern void lte_rrc_irat_to_G_init_mem(void);

extern void lte_rrc_irat_from_G_deinit_mem(void);

extern void lte_rrc_irat_from_DO_deinit_mem(void);

extern void lte_rrc_irat_from_1x_deinit_mem(void);

extern void lte_rrc_irat_to_DO_deinit_mem(void);

extern void lte_rrc_irat_to_G_deinit_mem(void);

extern boolean lte_rrc_irat_to_DO_mgr_ho_in_prog(void);

extern boolean lte_rrc_irat_to_G_mgr_ho_in_prog(void);

extern boolean lte_rrc_irat_from_g_mgr_is_redir_ue_initiated(void);

extern lte_earfcn_t lte_rrc_irat_from_g_mgr_get_target_earfcn_for_g2l_irat(void);

extern lte_earfcn_t lte_rrc_irat_from_DO_mgr_get_target_earfcn_for_d2l_irat(void);

extern uint8 lte_rrc_irat_to_G_read_sib7_freq(
  lte_rrc_osys_SystemInformationBlockType7  *sib7_ptr
);
#endif /*LTE_RRC_IRAT_H */
