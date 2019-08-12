/*!
  @file lte_app_msg.h

  @brief Message definitions for LTE Application Services
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/lte_app_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   ck      File moved to mcs/api/... for CMI Phase 4 effort
10/31/08   awj     Initial Revision

===========================================================================*/

#ifndef __LTE_APP_MSG_H__
#define __LTE_APP_MSG_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include <appmgr.h>
#include <msgr_lte.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* LTE App definitions for the ID portion of the UMID's */

/*-----------------------------------------------------------------------*/
/*     LTE_APP COMMAND ID'S                                              */
/*-----------------------------------------------------------------------*/

/*! @brief ID for a Spawn Threads Command and Response
 */
#define LTE_APP_ID_SPAWN_THREADS         APPMGR_ID_SPAWN_THREADS

/*-----------------------------------------------------------------------*/
/*     LTE_APP SUPERVISORY ID'S                                          */
/*-----------------------------------------------------------------------*/

/*! @brief ID for a Loopback Supervisory
 */
#define LTE_APP_ID_LOOPBACK              APPMGR_ID_LOOPBACK

/*! @brief Used to make a LTE App-specific Loopback Reply UMID
 */
#define LTE_APP_ID_LOOPBACK_REPLY        APPMGR_ID_LOOPBACK_REPLY

/*! @brief ID for Thread Kill Supervisory
 */
#define LTE_APP_ID_THREAD_KILL           APPMGR_ID_THREAD_KILL

/*-----------------------------------------------------------------------*/
/*     LTE_APP INDICATION ID'S                                           */
/*-----------------------------------------------------------------------*/

/*! @brief ID for Thread Ready Indication
 *
 *  @detail This ID is to be used by external modules to define their own
 *          LTE_XXXX_IND_THREAD_READY message.  It is defined at the top end of
 *          the ID spectrum so that modules don't accidently define an
 *          overlapping message.
 */
#define LTE_APP_ID_THREAD_READY          APPMGR_ID_THREAD_READY

/*! @brief ID for Thread Run Indication
 */
#define LTE_APP_ID_THREAD_RUN            APPMGR_ID_THREAD_RUN

/*-----------------------------------------------------------------------*/
/*     LTE_APP COMMAND UMID's                                            */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of LTE App Command UMID's
 */
enum {
  /*! Spawn Threads Command - defined in lte_app_cmd_spawn_threads_s
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, CMD, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_cmd_spawn_threads_s ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, CMD, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_cmd_spawn_threads_s ),

  /*! Sync Command - defined in appmgr_cmd_sync_s 
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, CMD, SYNC, APPMGR_ID_SYNC,
                    appmgr_cmd_sync_s ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, CMD, SYNC, APPMGR_ID_SYNC,
                    appmgr_cmd_sync_s ),
};

/*-----------------------------------------------------------------------*/
/*     LTE_APP Response UMID's                                            */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of LTE App Response UMID's
 */
enum {
  /*! Spawn Threads Response - defined in lte_app_rsp_spawn_threads_s
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, RSP, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_rsp_spawn_threads_s ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, RSP, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_rsp_spawn_threads_s ),

  /*! Sync Response - defined in appmgr_rsp_sync_s
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, RSP, SYNC, APPMGR_ID_SYNC,
                    appmgr_rsp_sync_s ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, RSP, SYNC, APPMGR_ID_SYNC,
                    appmgr_rsp_sync_s ),

};

/*-----------------------------------------------------------------------*/
/*     LTE_APP SUPERVISORY UMID's                                        */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of LTE App Supervisory UMID's
 */
enum {
  /*! Loopback Supervisory - defined in msgr_spr_loopback_struct_type
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, SPR, LOOPBACK, APPMGR_ID_LOOPBACK,
                    msgr_spr_loopback_struct_type ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, SPR, LOOPBACK, APPMGR_ID_LOOPBACK,
                    msgr_spr_loopback_struct_type ),

  /*! Loopback Reply Supervisory - defined in msgr_spr_loopback_reply_struct_type
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, SPR, LOOPBACK_REPLY, APPMGR_ID_LOOPBACK_REPLY,
                    msgr_spr_loopback_reply_struct_type ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, SPR, LOOPBACK_REPLY, APPMGR_ID_LOOPBACK_REPLY,
                    msgr_spr_loopback_reply_struct_type ),

  /*! Thread Ready Supervisory
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, none ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, none ),

  /*! Thread Kill Supervisory
   */
  MSGR_DEFINE_UMID( LTE, APP_DSPSW, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, none ),
  MSGR_DEFINE_UMID( LTE, APP_DSPFW, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, none ),
};


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* __LTE_APP_MSG_H__ */
