/*!
  @file appmgr_msg.h

  @brief Contains message definitions for App Manager
  
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/appmgr_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   ckk     Add MCS_APPGR UMIDs for MCS APP
05/22/09   ejv     Remove SMD_PORTS_OPEN_IND.
03/12/09   ejv     Updates to MSGR for supporting SMD-Lite.
05/16/08   ejv     Update for QDSP6 beta release.
12/07/07   enj     Initial Revision

===========================================================================*/

#ifndef __APPMGR_MSG_H__
#define __APPMGR_MSG_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_attach.h>  /* attachment types & defines */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* App definitions for the ID portion of the UMID's */

/*-----------------------------------------------------------------------*/
/*     APPMGR COMMAND ID'S                                               */
/*-----------------------------------------------------------------------*/

/*! @brief ID for a Spawn Threads Command and Response
 */
#define APPMGR_ID_SPAWN_THREADS         0x01

/*! @brief ID for App Sync Command and Response
 */
#define APPMGR_ID_SYNC                  0x02

/*-----------------------------------------------------------------------*/
/*     APPMGR SUPERVISORY ID'S                                           */
/*-----------------------------------------------------------------------*/

/*! @brief ID for a Loopback Supervisory
 */
#define APPMGR_ID_LOOPBACK              MSGR_ID_LOOPBACK

/*! @brief Used to make a UMB App-specific Loopback Reply UMID
 */
#define APPMGR_ID_LOOPBACK_REPLY        MSGR_ID_LOOPBACK_REPLY

/*! @brief ID for Thread Ready Supervisory
 */
#define APPMGR_ID_THREAD_READY          0x02

/*! @brief ID for Thread Kill Supervisory
 */
#define APPMGR_ID_THREAD_KILL           0x03

/*-----------------------------------------------------------------------*/
/*     APPMGR INDICATION ID'S                                            */
/*-----------------------------------------------------------------------*/

/*! @brief ID for Thread Run Indication
 */
#define APPMGR_ID_THREAD_RUN            0x00

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR APPMGR COMMANDS                               */
/*-----------------------------------------------------------------------*/

/*! @brief Spawn Threads Command
 *
 *  @detail This is the message definition for a Spawn Threads Command used
 *          between UMB Apps on different PD's
 */
typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} appmgr_cmd_spawn_threads_s;

/*! @brief App Sync Command
 *
 *  @detail This is the message definition for a App Sync Command used between
 *          UMB Apps on different PD's
 */
typedef struct {
  msgr_hdr_struct_type     hdr;                /*!< MSGR header */
} appmgr_cmd_sync_s;

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR APPMGR RESPONSES                             */
/*-----------------------------------------------------------------------*/

/*! @brief Spawn Threads Response
 *
 *  @detail This is the message definition for a Spawn Threads Response used
 *          between UMB Apps on different PD's.
 */
typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} appmgr_rsp_spawn_threads_s;

/*! @brief App Sync Response
 *
 *  @detail This is the message definition for a App Sync Response used
 *          between UMB Apps on different PD's.
 */
typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} appmgr_rsp_sync_s;

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR APPMGR SUPERVISORY'S                         */
/*-----------------------------------------------------------------------*/

typedef msgr_spr_loopback_struct_type       appmgr_spr_loopback_s;
typedef msgr_spr_loopback_reply_struct_type appmgr_spr_loopback_reply_s;

/*! @brief Definition of the UMB App Thread Kill Supervisory message
 */
typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} appmgr_spr_thread_kill_s;

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR APPMGR INDICATIONS                           */
/*-----------------------------------------------------------------------*/

/*! @brief Definition of the UMB App Thread Run Indication message
 */
typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} appmgr_ind_thread_run_s;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-----------------------------------------------------------------------*/
/*     APPMGR Indication UMID's                                          */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of UMB App Indication UMID's
 */
enum {
  /*! Thread Run Indication - defined in appmgr_ind_thread_run_s
   */
  MSGR_DEFINE_UMID( MCS, APPMGR, IND, THREAD_RUN, APPMGR_ID_THREAD_RUN,
                    appmgr_ind_thread_run_s )
};

/*-----------------------------------------------------------------------*/
/*     APPMGR Command UMID's                                             */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of AppMgr Command UMID's
 */
enum {
  /*! Spawn Threads Command - defined in app_cmd_spawn_threads_s
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, CMD, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_cmd_spawn_threads_s ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, CMD, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_cmd_spawn_threads_s ),

  /*! Sync Command - defined in appmgr_cmd_sync_s 
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, CMD, SYNC, APPMGR_ID_SYNC,
                    appmgr_cmd_sync_s ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, CMD, SYNC, APPMGR_ID_SYNC,
                    appmgr_cmd_sync_s ),
};

/*-----------------------------------------------------------------------*/
/*     APPMGR Response UMID's                                            */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of AppMgr Response UMID's
 */
enum {
  /*! Spawn Threads Response - defined in app_rsp_spawn_threads_s
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, RSP, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_rsp_spawn_threads_s ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, RSP, SPAWN_THREADS, APPMGR_ID_SPAWN_THREADS,
                    appmgr_rsp_spawn_threads_s ),

  /*! Sync Response - defined in appmgr_rsp_sync_s
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, RSP, SYNC, APPMGR_ID_SYNC,
                    appmgr_rsp_sync_s ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, RSP, SYNC, APPMGR_ID_SYNC,
                    appmgr_rsp_sync_s ),

};

/*-----------------------------------------------------------------------*/
/*     APPMGR Supervisory UMID's                                         */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of AppMgr Supervisory UMID's
 */
enum {
  /*! Loopback Supervisory - defined in msgr_spr_loopback_struct_type
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, SPR, LOOPBACK, APPMGR_ID_LOOPBACK,
                    msgr_spr_loopback_struct_type ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, SPR, LOOPBACK, APPMGR_ID_LOOPBACK,
                    msgr_spr_loopback_struct_type ),

  /*! Loopback Reply Supervisory - defined in msgr_spr_loopback_reply_struct_type
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, SPR, LOOPBACK_REPLY, APPMGR_ID_LOOPBACK_REPLY,
                    msgr_spr_loopback_reply_struct_type ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, SPR, LOOPBACK_REPLY, APPMGR_ID_LOOPBACK_REPLY,
                    msgr_spr_loopback_reply_struct_type ),

  /*! Thread Ready Supervisory
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, none ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, none ),

  /*! Thread Kill Supervisory
   */
  MSGR_DEFINE_UMID( MCS, APP_DSPSW, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, none ),
  MSGR_DEFINE_UMID( MCS, APP_DSPFW, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, none ),
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* __APPMGR_MSG_H__ */
