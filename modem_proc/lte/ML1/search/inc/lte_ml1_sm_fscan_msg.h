
/*!
  @file
  lte_ml1_sm_fscan_msg.h

  @brief
  Message router UMIDs header for SM FSCAN
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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



$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/search/inc/lte_ml1_sm_fscan_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/14   lec     Initial submission

===========================================================================*/

#ifndef LTE_ML1_SM_FSCAN_MSG_H
#define LTE_ML1_SM_FSCAN_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>
#include <msgr_types.h>
#include <msgr_lte.h>
#include <lte_ml1_schdlr_types.h>
#include <lte_ml1_schdlr.h>

#include "lte_ml1_dlm_types.h"
#include "lte_ml1_rfmgr_types.h"
#include "lte_cphy_msg.h"
#include "lte_ml1_sm_idle_if.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief
    Data Structure for LTE_ML1_SM_FSCAN_BAND_ALGO_START_REQ
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
  /*! Band to perform algo, if it mismatches with FSCAN, assert */
  sys_sband_lte_e_type             band;

} lte_ml1_sm_fscan_band_algo_start_req_s;

typedef enum
{

  /* LTE_ML1_SM_FSCAN_BAND_ALGO_START_REQ */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, REQ, BAND_ALGO_START, 0x0, 
                   lte_ml1_sm_fscan_band_algo_start_req_s
                   ),

  /* LTE_ML1_SM_FSCAN_BAND_ALGO_STOP_REQ */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, REQ, BAND_ALGO_STOP, 0x1, 
                   msgr_hdr_s
                   ),

  /* LTE_ML1_SM_FSCAN_FS_DB_DEINIT_REQ */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, REQ, FS_DB_DEINIT, 0x2, 
                   msgr_hdr_s
                   ),

  /* LTE_ML1_SM_FSCAN_BAND_ALGO_CONTINUE_REQ */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, REQ, BAND_ALGO_CONTINUE, 0x3, 
                   msgr_hdr_s
                   ),

  /* supervisor messages */
  /* LTE_ML1_SM_FSCAN_LOOPBACK_SPR */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s
                   ),

  /* LTE_ML1_SM_FSCAN_LOOPBACK_REPLY_SPR */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s
                   ),

 /* LTE_ML1_SM_FSCAN_THREAD_READY_SPR, appmgr automatically registers itself for this */
  MSGR_DEFINE_UMID(LTE, ML1_SM_FSCAN, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, 
                   none
                   ),

 /* LTE_ML1_SM_FSCAN_THREAD_KILL_SPR */
  MSGR_DEFINE_UMID(
                   LTE, ML1_SM_FSCAN, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none
                   )

}lte_ml1_sm_fscan_msg_e;

typedef union {
  /* FSCAN requests */
  lte_ml1_sm_fscan_band_algo_start_req_s  sm_fscan_band_algo_start_req;
  msgr_hdr_s                              sm_fscan_band_algo_stop_req;
  msgr_hdr_s                              sm_fscan_fs_db_deinit;

  /* Supervisor messages */
  msgr_spr_loopback_s                     msgr_spr_loopback;
  msgr_spr_loopback_reply_s               msgr_spr_loopback_reply;
} lte_ml1_sm_fscan_msg_u;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_ML1_SM_IDLE_MSG_H */

