/*!
  @file
  appmgr.h

  @brief  Application Manager

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/appmgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   ejv     Add appmgr_spawn.
10/16/07   enj     Initial split of umb_app and appmgr

===========================================================================*/

#ifndef APPMGR_H
#define APPMGR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>   /* Definition for basic types and macros */
#include <msgr.h>
#include <msgr_umid.h>
#include <pthread.h>
#include "appmgr_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef for a threadful module init function
 *
 *  @detail This function takes a priority for the new thread and returns the
 *          newly created thread handle.
 */
typedef pthread_t (*appmgr_thread_init_f)(int priority);

/*! @brief Typedef for a threadful module teardown function
 */
typedef void (*appmgr_thread_teardown_f)(void);

/*! @brief Definition for an entry in the module init table.
 *
 *  @detail
 */
typedef struct {
  /*! Text name of the module for debugging purposes */
  const char*               name;
  /*! Message router module id */
  msgr_tech_module_type     module;
  /*! Module init routine (e.g. srch_init) */
  appmgr_thread_init_f      module_init_func;
  /*! Priority for the module's thread */
  int                       priority;
  /*! Used internally to store the module's qthread handle */
  pthread_t                 thread;
  /*! Used internally to store the number of threads for a given module id */
  uint32                    num_threads;
} appmgr_thread_init_entry_s;

/*! @brief Definition for an entry in the service init table
 */
typedef struct {
  /*! Text name of the utility to initialize */
  const char *              name;
  /*! Initialization function */
  appmgr_thread_init_f      service_init_func;
  /*! Priority for service thread */
  int                       priority;
  /*! Teardown function */
  appmgr_thread_teardown_f  service_teardown_func;
} appmgr_service_init_entry_s;

/*! @brief Utility initialization function
 */
typedef void (*appmgr_utility_init_f)(void);

/*! @brief Definition for a utility (non-thread module) initialization entry
 */
typedef struct {
  /*! Text name of the utility to initialize */
  const char *              name;
  /*! Initialization function */
  appmgr_utility_init_f     utility_init_func;
} appmgr_utility_init_entry_s;

/*! @brief Definition for App to be created
 */
typedef struct {
  /*! Text name of the peer App to create */
  const char*               name;
  /*! Message router App id */
  msgr_tech_module_type     app_id;
  /*! Used internally to store whether the App has replied to a Loopback */
  boolean                   awake;
  /*! Used internally to store whether the App has replied to a Spawn Threads
   *  Command.
   */
  boolean                   spawned;
} appmgr_peer_entry_s;


/*! @brief Mapping from App ID to MSGR ID
 */
typedef struct {
  msgr_tech_module_type         app_id;
  const char                   *name;
} appmgr_msgr_entry_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Initialize the App including the Message Router
 */
extern void appmgr_init
(
  msgr_tech_module_type            app,
  appmgr_peer_entry_s             *apps,
  uint32                           num_apps,
  msgr_init_entry_s               *msgr_table,
  uint32                           num_msgrs,
  msgr_init_smd_entry_s           *smd_port_table,
  uint32                           num_smds,
  msgr_tech_module_type            master_app
);

/* Synchronize this App with its peers on other PD's
 */
extern void appmgr_sync
(
  msgr_tech_module_type            app
);

/* Spawn the module threads.
 */
extern void appmgr_spawn
(
  msgr_tech_module_type            app,
  appmgr_utility_init_entry_s     *pre_utilities,
  uint32                           num_pre_utilities,
  appmgr_service_init_entry_s     *services,
  uint32                           num_services,
  appmgr_thread_init_entry_s      *threads,
  uint32                           num_threads,
  appmgr_utility_init_entry_s     *post_utilities,
  uint32                           num_post_utilities
);

/* Run the module threads.
 */
extern void appmgr_run( void );

/* Enter reset manager mode
 */
extern void appmgr_reset_manager(void);

/* Send a loopback to all threads on this PD and wait for replies.
 */
extern boolean appmgr_loopback
(
  msgr_tech_module_type            app
);

/* Send a XXX_YYYY_SPR_THREAD_READY and wait for a APPMGR_THREAD_RUN
 */
void appmgr_thread_init_complete
(
  msgr_tech_module_type   module
);

/* Teardown the threads and Message Router on this PD
 */
extern void appmgr_teardown
(
  msgr_tech_module_type            app
);

/* Lock the printf mutex
*/
extern  void appmgr_msg_mutex_lock( void );

/* Unlock the printf mutex*/
extern  void appmgr_msg_mutex_unlock( void );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Appmgr API specifically for Off-target (QTF) tests                      */
/* CAUTION: These APIs must only be used in an off-target build, if used   */
/*           otherwise; user will see linker errors                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Test routine to initialize umb app
 */
extern void appmgr_test_init
(
  msgr_tech_module_type               app,
  const appmgr_utility_init_entry_s  *utilities,
  uint32                              num_utilities,
  const appmgr_service_init_entry_s  *services,
  uint32                              num_services
);

/* Test routine to spawn a thread on this PD.
 */
extern pthread_t appmgr_test_spawn_thread
(
  const char * const               name,
  msgr_tech_module_type            module,
  appmgr_thread_init_f             init_func,
  int                              priority
);

/* Test routine to send the Thread Run on this PD.
 */
extern void appmgr_test_thread_run(void);

/* Test routine to send a loopback to all threads started with
 * appmgr_test_spawn_thread().
 */
extern boolean appmgr_test_loopback
(
  msgr_tech_module_type               app
);

/* Test routine to teardown the threads and Message Router on this PD.
 */
extern void appmgr_test_teardown
(
  msgr_tech_module_type                app,
  const appmgr_service_init_entry_s   *teardowns,
  uint32                               num_teardowns
);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* End of Off-target (QTF) specific appmgr API                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* APPMGR_H */

