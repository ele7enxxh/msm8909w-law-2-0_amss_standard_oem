/*!
  @file
  lte_app.h

  @brief
  LTE - Application service types and interfaces.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/lte_app.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   ck      Remove TESTFRAMEWORK feature to prep file for CMI Phase 4
11/02/09   ejv     Start clean up of LTE references.
10/07/09   ejv     Add lte_app_spawn.
07/30/09   ejv     Add lte_app_get_my_msgr_id().
07/23/09   ejv     Add lte_app_get_master_msgr_id().
10/30/08   awj     Initial release

===========================================================================*/

#ifndef LTE_APP_H
#define LTE_APP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>   /* Definition for basic types and macros */
#include <msgr_lte.h>
#include <appmgr.h>
#include "lte_app_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Initialize the LTE App including the Message Router
 */
extern void lte_app_init
(
  msgr_tech_module_type            app
);

extern msgr_tech_module_type lte_app_get_master_msgr_id( void );
extern msgr_tech_module_type lte_app_get_my_msgr_id( void );

/*! @brief Map lte_app_thread_init_complete to appmgr_thread_init_complete
 *  @see   appmgr_thread_init_complete
 */
#define lte_app_thread_init_complete   appmgr_thread_init_complete

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* lte_app API specifically for Off-target (QTF) tests                     */
/* CAUTION: These APIs must only be used in an off-target build, if used   */
/*           otherwise; user will see linker errors                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Test routine to initialize lte app
 */
extern void lte_app_test_init(void);

/* Test routine to spawn a thread on this PD.
 */
extern pthread_t lte_app_test_spawn_thread
(
  const char * const               name,
  msgr_tech_module_type            module,
  appmgr_thread_init_f            init_func,
  int                              priority
);

/* Test routine to send the Thread Run on this PD.
 */
extern void lte_app_test_thread_run(void);

/* Test routine to send a loopback to all threads started with
 * lte_app_test_spawn_thread().
 */
extern boolean lte_app_test_loopback(void);

/* Test routine to teardown the threads and Message Router on this PD.
 */
extern void lte_app_test_teardown(void);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* End of Off-target (QTF) specific lte_app API                            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#endif /* LTE_APP_H */

