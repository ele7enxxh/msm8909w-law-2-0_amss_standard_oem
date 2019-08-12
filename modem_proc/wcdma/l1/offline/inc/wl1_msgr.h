#ifndef WL1_MSGR_H
#define WL1_MSGR_H

/*===========================================================================

            W2LTE Message Router R E L A T E D   D E C L A R A T I O N S

DESCRIPTION 

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES

Copyright (c) 2001-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/wl1_msgr.h_v   1.2   04 Jun 2002 13:29:18   kailasam  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1_msgr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   mk      adding  x2g_irat_cm_gsm_init_rsp_type rsp_msg in wl1_msgr_cmd_code_type 
06/23/16   abs     Cleaning up DSDA~CXM(FR:2334) changes
01/21/14   abs     Mods to support WL1 freq/state reporting for DDR/DSDA & SAR FR's
                   using the unified WL1~CXM interface
09/23/13   as      Porting Dual Sim from Triton to Dime.
07/15/13   abs     Moving MSGR changes outside of LTE featurization 
10/15/10   yh      Added support for W2L Compressed mode
06/02/10   stk     Initial Version. Added support for W2LTE measurements and reselection 
                   under FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "msgr.h"
#include "msgr_umid.h"
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
#include "lte_cphy_irat_meas_msg.h"
#endif /* (FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY) */
#ifdef FEATURE_WCDMA_COEXISTENCE_SW
#include "wwan_coex_mgr.h"
#endif /* FEATURE_WCDMA_COEXISTENCE_SW */

#include "gl1_hw.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define MSGR_MODULE_L1                 0x01
#define MSGR_WCDMA_L1                  MSGR_TECH_MODULE( MSGR_TECH_WCDMA, MSGR_MODULE_L1 )

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
typedef lte_cphy_irat_meas_cleanup_cnf_s wl1_msgr_w2l_cleanup_cnf;
typedef lte_cphy_irat_meas_meas_cnf_s wl1_msgr_w2l_meas_cnf;
typedef lte_cphy_irat_meas_search_cnf_s wl1_msgr_w2l_search_cnf;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
typedef lte_cphy_irat_meas_timed_srch_meas_cnf_s wl1_msgr_w2l_timed_srch_meas_cnf;
#endif /* FEATURE_WCDMA_CM_LTE_SEARCH */
#endif /* (FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY) */

#ifdef FEATURE_WCDMA_COEXISTENCE_SW
typedef cxm_freqid_info_ind_s wl1_msgr_cxm_freqid_ind;
#endif /* FEATURE_WCDMA_COEXISTENCE_SW */
#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
typedef cxm_wwcoex_state_update_s wl1_msgr_cxm_desense_state_ind;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */


typedef union
{
  /* Message Router Header field */
  msgr_hdr_s             msgr_hdr;

  /* Struct's to support W2L measurements, reselections and compressed mode */
  #if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  wl1_msgr_w2l_cleanup_cnf cleanup_cnf;
  wl1_msgr_w2l_search_cnf search_cnf;
  wl1_msgr_w2l_meas_cnf meas_cnf;
  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  wl1_msgr_w2l_timed_srch_meas_cnf timed_srch_meas_cnf;
  #endif
  #endif /* (FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY) */

  /*Struct's to support CXM freqId and desense state indications*/
  #ifdef FEATURE_WCDMA_COEXISTENCE_SW
  /*Struct to hold CXM freq info*/
  wl1_msgr_cxm_freqid_ind freqid_ind;
  #endif /* FEATURE_WCDMA_COEXISTENCE_SW */
  #ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
  /*Struct to hold CXM de-sense state info*/
  wl1_msgr_cxm_desense_state_ind desense_state;
  #endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */
  
  x2g_irat_cm_gsm_init_rsp_type rsp_msg;
} wl1_msgr_cmd_code_type;

typedef struct
{
  q_link_type link;

  wl1_msgr_cmd_code_type cmd_code;

} wl1_msgr_cmd_type;

#endif /* WL1_MSGR_H */
