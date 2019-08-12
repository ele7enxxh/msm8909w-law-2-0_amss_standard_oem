/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as feature mode SVLTE and normal per subscription.

EXTERNALIZED FUNCTIONS
  Functional Interface:



Copyright (c) 2013 - 2013 by QUALCOMM TECHNOLOGIES INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc_auto.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   cl      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */

#include "comdef.h"   /* Definition for basic types and macros */
#include "sys.h"      /* Common types. */
#include "sd_v.h"
#include "modem_mem.h"

#include "cmph.h"     /* Interface to CM ph functions */
#include "cmmsc.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
#include "cmsoa.h"
#include "cmpmprx.h"
#include "cmregprxi.h"
#include "cmcall.h"
#include "cmefs.h"


#include "mmoc_v.h"
#include "policyman.h"
#ifdef FEATURE_CM_LTE
#include "cmsds.h"
#endif
#ifdef CM_DEBUG
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Routing Table - AUTO
** EVENT,
** STATE for stack1, 2, ...
** ACTION for stack 1, 2, ... (order matters)
** NEW STATE for stack 1, 2, ...
*/
static cmmsc_routing_table_auto_entry_s_type cmmsc_auto_routes[] =
{

  /* SUB available event:
       In LPM
     SUB lost event:
       In LPM
  */
  /* 0 */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL}
    },

  /* SUB available event:
       From NORMAL to DUAL
  */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_NO_CHG,                    CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_NO_CHG,                    CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_NO_CHG,                    CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_NO_CHG,                    CMMSC_STACK_STATE_ACTIVE}
    },

  /* 5 */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_NO_CHG}
    },
    
  /* sub ready - hybr2 on to off due to op mode change */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SVLTE_STOP_STACK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_INACTIVE}
    },

  /* SUB lost event:
  */

   /* on-off-off -> on-off-off */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
    },

   /* on-off-on -> on-off-off */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_FORCE_RESTORE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
    },

   /* on-on-off -> on-off-off */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_FORCE_RESTORE},              {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
    },

   /* on-on-on -> on-off-off */
   /* 10 */
  { CMMSC_EVENT_SUB,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},         {1,CMMSC_ACTION_FORCE_RESTORE},              {2,CMMSC_ACTION_FORCE_RESTORE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
    },

  /* ONLINE event:
       From LPM to AUTO_MAIN_HYBR1_HYBR2
  */
  { CMMSC_EVENT_ONLINE,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_ONLINE,            CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_ONLINE,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_INACTIVE}
    },

  /* ONLINE event:
       From LPM to AUTO_MAIN
  */
  { CMMSC_EVENT_ONLINE,            CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_NULL,       CMMSC_STACK_STATE_NULL,                      CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
    },

  /* ONLINE event:
       Diable hybr1
  */
  /* 15 */
  { CMMSC_EVENT_ONLINE_DISABLE_HYBR1,  CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_USER},             {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_INACTIVE}
    },

  { CMMSC_EVENT_ONLINE_DISABLE_HYBR1,  CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_USER},             {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ACTIVE}
    },

  /* POWER_DOWN event:
  */
  { CMMSC_EVENT_POWER_DOWN,            CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_POWER_DOWN},      {1,CMMSC_ACTION_POWER_DOWN},             {2,CMMSC_ACTION_POWER_DOWN}},
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL}
    },


  /* Event ON/OFF */
  { CMMSC_EVENT_ON,                CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SXLTE_KICK_NOT_AUTO}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_ON,                CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                        CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_SUB_CHG_COND_KICK}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_ACTIVE}
    },

  /* 20 */
  { CMMSC_EVENT_ON,                CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                        CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_ACTIVE,                    CMMSC_STACK_STATE_INACTIVE}
    },

  { CMMSC_EVENT_ON,                CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                        CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,     CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
  },

  { CMMSC_EVENT_OFF,               CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,        CMMSC_STACK_STATE_ANY,                       CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},        {1,CMMSC_ACTION_NONE},                       {2,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_INACTIVE,   CMMSC_STACK_STATE_INACTIVE,                  CMMSC_STACK_STATE_INACTIVE}
  },

  /* SYS_SEL_PREF_MAIN event:
       During LPM
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,     CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NULL,          CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_MAIN event:
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,     CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_HYBR1 event:
  */
  /* 25 */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR1,    CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_SYS_SEL_PREF_HYBR1,    CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_SRV,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_SYS_SEL_PREF_HYBR1,    CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_HDR_EXT_NO_SRV,        CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_HYBR2 event:
  */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,    CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{2,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_NONE},                   {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,    CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_MAIN_HYBR1 event: Force to main and hybr1
  ** oprt mode is not online
  */
  /* 30 */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1,  CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },
    
  /* SYS_SEL_PREF_MAX event: Force to all stacks
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,           CMMSC_STACK_STATE_NULL,                  CMMSC_STACK_STATE_NULL},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },
    
  /* SYS_SEL_PREF_MAIN_HYBR1 event: Force to main and hybr1
  ** oprt mode is online
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1,  CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* - - - - - - -  Transition to on-on-on  - - - - - - - - */

  /* on-off-off -> on-on-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ACTIVE}
    },

  /* on-off-on -> on-on-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* on-on-off -> on-on-on */
  /* 35 */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_INACTIVE},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_ACTIVE}
    },

  /* on-on-on -> on-on-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},    {0,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* - - - - - - -  Transition to on-off-on  - - - - - - - - */

  /* on-off-off -> on-off-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ACTIVE}
    },

  /* on-off-on -> on-off-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

  /* on-on-off -> on-off-on */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                  {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ACTIVE}
    },

  /* on-on-on -> on-off-on */
  /* 40 */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

  /* - - - - - - -  Transition to on-on-off  - - - - - - - - */

  /* on-off-off -> on-on-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-off-on -> on-on-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ANY},
    {{2,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-on-off -> on-on-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_INACTIVE},
    {{2,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_SYS_SEL_PREF},           {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-on-on -> on-on-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{2,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_INACTIVE}
    },

  /* - - - - - - -  Transition to on-off-off  - - - - - - - - */

  /* 45 */
  /* on-off-off -> on-off-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE},
    {{2,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-off-on -> on-off-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_ANY},
    {{2,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_NONE},                   {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-on-off -> on-off-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_INACTIVE},
    {{2,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_SYS_SEL_PREF},           {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE}
    },

  /* on-on-on -> on-off-off */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,      CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{2,CMMSC_ACTION_SYS_SEL_PREF},    {1,CMMSC_ACTION_SYS_SEL_PREF},           {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_INACTIVE,              CMMSC_STACK_STATE_INACTIVE}
    },

  /* Event when receiving rpt for irat lte (hybr2) do resel */
  { CMMSC_EVENT_LTE_DO_RESEL,          CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_SRV,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* 50 */
  { CMMSC_EVENT_LTE_DO_RESEL,          CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_SRV,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event when receiving rpt for redial hdr fail */
  { CMMSC_EVENT_RPT_REDIAL_HDR_FAIL,   CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_SRV,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_USER}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event to disable hybr1  */
  { CMMSC_EVENT_FORCE_COLLOC,          CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_PH_EXT},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event to enable hybr1  */
  { CMMSC_EVENT_FORCE_COLLOC,          CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_PH_EXT},           {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event to kick start hybr hdr */

  { CMMSC_EVENT_KICK_HYBY1,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_SUSPENDED,             CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_KICK_HYBY1,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_TOP_EXT},          {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_KICK_HYBY1,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_SUSPENDED,             CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event to kick start hybr hdr */
  /* 57 */
  { CMMSC_EVENT_KICK_HYBY1,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY   ,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_TOP_EXT},          {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* Event to timer expires  */
  { CMMSC_EVENT_TIMER_EXP_ACQ_HYBR1,   CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_SRV,            CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_HDR_EXT_NO_SRV,        CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_TIMER_EXP_MAIN_LOST,   CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_HDR_EXT_NO_SRV,        CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* ACQ FAIL on MAIN events in AUTO only */
  { CMMSC_EVENT_ACQ_FAIL_MAIN,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_HICPS_REMOVE_3GPP}, {1,CMMSC_ACTION_HICPS_COND_KICK},      {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_MAIN_3GPP_STOPPED,        CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_PENDING_KICK},        {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* ACQ FAIL HYBR1 events in AUTO only */
  /* 60 */
  { CMMSC_EVENT_ACQ_FAIL_HYBR1,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SHORTEN_BSR},        {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_ACQ_FAIL_HYBR1,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},              {1,CMMSC_ACTION_NONE},                 {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_ACQ_FAIL_HYBR1,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESTORE_3GPP},       {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* ACQ FAIL MAIN events in SVLTE */

  { CMMSC_EVENT_ACQ_FAIL_MAIN,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SVLTE_REMOVE_3GPP},  {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_SVLTE_COND_KICK}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_RESTORE_DEFERRED_3GPP,    CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_RESTORE_3GPP},       {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* ACQ FAIL HYBR1 events in SVLTE */
  /* 65 */
  { CMMSC_EVENT_ACQ_FAIL_HYBR1,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_SHORTEN_BSR}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_ACQ_FAIL_HYBR1,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_DEFER_3GPP},         {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* ACQ FAIL HYBR2 events in SVLTE */

  { CMMSC_EVENT_ACQ_FAIL_HYBR2,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_SVLTE_COND_KICK},     {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* 3GPP STOP_MODE_CNF events in SVLTE */

  { CMMSC_EVENT_MAIN_3GPP_STOPPED,        CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_PENDING_KICK}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_HYBR_3GPP_STOPPED,        CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_PENDING_KICK},        {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* MAIN SRV IND in non-HYBRID OPR */
  /* 72 */
  { CMMSC_EVENT_1X_SRV,                   CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_UPDATE_SRV},         {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_MAIN_3GPP_SRV,            CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_ENABLE_3GPP},        {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  }, 

  /* SRV LOST in non-HYBRID OPR */

  { CMMSC_EVENT_MAIN_SRV_LOST,            CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_UPDATE_NO_SRV},      {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* SRV IND AUTO+SVLTE */

  { CMMSC_EVENT_HYBR_3GPP_SRV,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_SRV}
  },

  { CMMSC_EVENT_HYBR_HDR_SRV,             CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_UPDATE_SRV},          {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_NO_CHG}
  },
  
  /* 77 */
  { CMMSC_EVENT_HYBR_HDR_SRV,             CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_UPDATE_SRV},          {2,CMMSC_ACTION_SYNC_UP}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_HYBR_HDR_SRV,             CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_UPDATE_SRV},          {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_NO_CHG}
  },

  /* 1x acquire srv - hybr1/hybr2 is enabled
  ** If main and hybr1 both are active, enable 3GPP before sync up
  */
  { CMMSC_EVENT_1X_SRV,                   CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_ENABLE_3GPP},        {1,CMMSC_ACTION_SYNC_UP},             {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_1X_SRV,                   CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_ENABLE_3GPP},        {1,CMMSC_ACTION_SYNC_UP},             {2,CMMSC_ACTION_SYNC_UP}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },
  
  { CMMSC_EVENT_1X_SRV,                   CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_INACTIVE},
    {{1,CMMSC_ACTION_SYNC_UP},            {0,CMMSC_ACTION_ENABLE_3GPP},         {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* 82 */
  { CMMSC_EVENT_1X_SRV,                   CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{1,CMMSC_ACTION_SYNC_UP},            {0,CMMSC_ACTION_ENABLE_3GPP},         {2,CMMSC_ACTION_SYNC_UP}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_MAIN_3GPP_SRV,            CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_ENABLE_3GPP},        {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_MAIN_HDR_SRV,             CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SRV,              CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* MAIN SRV LOST in HYBRID OPR - If hybr2/hybr1 has service, remove 3GPP from main.
  ** If hybr1 has no srv, reset acq_hybr_1_uptime regardless of hybr2 status.
  */

  { CMMSC_EVENT_MAIN_SRV_LOST,            CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_SRV},
    {{0,CMMSC_ACTION_DISABLE_3GPP},       {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_MAIN_SRV_LOST,            CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_DISABLE_3GPP},       {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* 87 */
  { CMMSC_EVENT_MAIN_SRV_LOST,            CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_UPDATE_NO_SRV},      {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* HYBR SRV LOST in HYBRID OPR */
  { CMMSC_EVENT_HYBR2_SRV_LOST,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_SRV},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_ACTIVE}
  },

  { CMMSC_EVENT_HYBR2_SRV_LOST,           CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_SRV},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_SYNC_UP}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_ACTIVE}
  },

  { CMMSC_EVENT_HYBR1_SRV_LOST,           CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_SRV,               CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_UPDATE_NO_SRV},       {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_NO_CHG}
  },

  { CMMSC_EVENT_HYBR1_SRV_LOST,           CMMSC_OP_MODE_AUTO_SVLTE,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_ACTIVE,             CMMSC_STACK_STATE_NO_CHG}
  },

  /* 92 */
  /* RPT_HYBR_BSR_TO_HDR */
  { CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_START, CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_FORCE_PH_EXT},          {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_END,   CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR_END}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_OTHER, CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_STACK_SYNC_UP}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_HICPS_POLICY_CHGD,     CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESTORE_3GPP},    {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
  },

  /* Events from other SUBs */
  
  { CMMSC_EVENT_CALL_END_OTHER_SUB,    CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_RESUME_SRV_ACQ}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
  },
  { CMMSC_EVENT_KICK_HYBR2,   CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},            {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_KICK_HYBR2}},
    { CMMSC_STACK_STATE_NO_CHG,        CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  /* SUSPEND STACKS */
  /* 97 */
  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
    {CMMSC_STACK_STATE_ANY,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},         {1,CMMSC_ACTION_SUSPEND},                {2,CMMSC_ACTION_SUSPEND}},
    { CMMSC_STACK_STATE_SUSPENDED,     CMMSC_STACK_STATE_SUSPENDED,             CMMSC_STACK_STATE_SUSPENDED}
    },


  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},        {1,CMMSC_ACTION_SUSPEND},                 {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,    CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

{ CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_SRV,         CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},        {1,CMMSC_ACTION_SUSPEND},                 {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,    CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

{ CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
    {CMMSC_STACK_STATE_SRV,         CMMSC_STACK_STATE_SRV,                CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},        {1,CMMSC_ACTION_SUSPEND},                 {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,    CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_SUSPEND},            {1,CMMSC_ACTION_NONE},                {2,CMMSC_ACTION_SUSPEND}},
    { CMMSC_STACK_STATE_SUSPENDED,        CMMSC_STACK_STATE_NO_CHG,             CMMSC_STACK_STATE_SUSPENDED}
    },

  /* 102 */
  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_FULL_RAT_SEARCH,
     {CMMSC_STACK_STATE_ACTIVE,        CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
     {{0,CMMSC_ACTION_SUSPEND},        {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
     { CMMSC_STACK_STATE_SUSPENDED,    CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
     },

  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},         {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,     CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },

  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_MAIN,
    {CMMSC_STACK_STATE_SRV,            CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},         {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,     CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
    },


  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_AUTO_SVLTE,
     {CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE,                CMMSC_STACK_STATE_ACTIVE},
     {{0,CMMSC_ACTION_SUSPEND},           {1,CMMSC_ACTION_SUSPEND},                {2,CMMSC_ACTION_SUSPEND}},
     { CMMSC_STACK_STATE_SUSPENDED,       CMMSC_STACK_STATE_SUSPENDED,             CMMSC_STACK_STATE_SUSPENDED}
     },

  /* INVALID_STATE, WILL REMOVE AFTER CODE REVIEW*/
  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_NORMAL,
     {CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
     {{0,CMMSC_ACTION_SUSPEND},           {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
     { CMMSC_STACK_STATE_SUSPENDED,       CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
     },

  /* 107 */
  { CMMSC_EVENT_SUSPEND,               CMMSC_OP_MODE_ANY,
     {CMMSC_STACK_STATE_ANY,           CMMSC_STACK_STATE_ANY,                CMMSC_STACK_STATE_ANY},
     {{0,CMMSC_ACTION_NONE},           {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_NONE}},
     { CMMSC_STACK_STATE_NO_CHG,       CMMSC_STACK_STATE_NO_CHG,                CMMSC_STACK_STATE_NO_CHG}
  },


/* RESUME STACKS */

{ CMMSC_EVENT_RESUME,                  CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
  {CMMSC_STACK_STATE_SUSPENDED,        CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_SUSPENDED},
  {{0,CMMSC_ACTION_RESUME},            {1,CMMSC_ACTION_FORCE_USER_WITH_RESUME},  {2,CMMSC_ACTION_FORCE_USER_WITH_RESUME}},
  { CMMSC_STACK_STATE_ACTIVE,          CMMSC_STACK_STATE_ACTIVE,                 CMMSC_STACK_STATE_ACTIVE}
  },

{ CMMSC_EVENT_RESUME,                  CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
  {CMMSC_STACK_STATE_SUSPENDED,        CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_ANY},
  {{0,CMMSC_ACTION_RESUME},            {1,CMMSC_ACTION_FORCE_USER_WITH_RESUME}, {2,CMMSC_ACTION_NONE}},
  { CMMSC_STACK_STATE_ACTIVE,          CMMSC_STACK_STATE_ACTIVE,                 CMMSC_STACK_STATE_NO_CHG}
  },

/* Though This state, where stack 0 is not active and others are active is not feasible,
   but adding the entry to complete combination  INVALID_STATE, WILL REMOVE AFTER CODE REVIEW*/
{ CMMSC_EVENT_RESUME,                 CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
  {CMMSC_STACK_STATE_SUSPENDED,       CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_SUSPENDED},
  {{0,CMMSC_ACTION_RESUME},             {1,CMMSC_ACTION_NONE},                   {2,CMMSC_ACTION_FORCE_USER_WITH_RESUME}},
  { CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_NO_CHG,                  CMMSC_STACK_STATE_ACTIVE}
  },

{ CMMSC_EVENT_RESUME,                 CMMSC_OP_MODE_FULL_RAT_SEARCH,
   {CMMSC_STACK_STATE_SUSPENDED,      CMMSC_STACK_STATE_ANY,                     CMMSC_STACK_STATE_ANY},
   {{0,CMMSC_ACTION_RESUME},          {1,CMMSC_ACTION_NONE},                     {2,CMMSC_ACTION_NONE}},
   { CMMSC_STACK_STATE_ACTIVE,        CMMSC_STACK_STATE_NO_CHG,                  CMMSC_STACK_STATE_NO_CHG}
   },

/* 112 */
{ CMMSC_EVENT_RESUME,                 CMMSC_OP_MODE_AUTO_MAIN,
  {CMMSC_STACK_STATE_SUSPENDED,       CMMSC_STACK_STATE_ANY,                     CMMSC_STACK_STATE_ANY},
  {{0,CMMSC_ACTION_RESUME},           {1,CMMSC_ACTION_NONE},                     {2,CMMSC_ACTION_NONE}},
  { CMMSC_STACK_STATE_ACTIVE,         CMMSC_STACK_STATE_NO_CHG,                  CMMSC_STACK_STATE_NO_CHG}
  },

{ CMMSC_EVENT_RESUME,                 CMMSC_OP_MODE_AUTO_SVLTE,
   {CMMSC_STACK_STATE_SUSPENDED,            CMMSC_STACK_STATE_SUSPENDED,               CMMSC_STACK_STATE_SUSPENDED},
   {{0,CMMSC_ACTION_RESUME},            {1,CMMSC_ACTION_RESUME},                   {2,CMMSC_ACTION_RESUME}},
   { CMMSC_STACK_STATE_ACTIVE,        CMMSC_STACK_STATE_ACTIVE,                  CMMSC_STACK_STATE_ACTIVE}
   },

{ CMMSC_EVENT_RESUME,                 CMMSC_OP_MODE_NORMAL,
   {CMMSC_STACK_STATE_SUSPENDED,            CMMSC_STACK_STATE_ANY,                     CMMSC_STACK_STATE_ANY},
   {{0,CMMSC_ACTION_RESUME},            {1,CMMSC_ACTION_NONE},                     {2,CMMSC_ACTION_NONE}},
   { CMMSC_STACK_STATE_ACTIVE,        CMMSC_STACK_STATE_NO_CHG,                  CMMSC_STACK_STATE_NO_CHG}
   },

{ CMMSC_EVENT_RESUME,                CMMSC_OP_MODE_ANY,
  {CMMSC_STACK_STATE_SUSPENDED,        CMMSC_STACK_STATE_ANY,                     CMMSC_STACK_STATE_ANY},
  {{0,CMMSC_ACTION_RESUME},            {1,CMMSC_ACTION_NONE},                    {2,CMMSC_ACTION_NONE}},
  { CMMSC_STACK_STATE_ACTIVE,          CMMSC_STACK_STATE_NO_CHG,                  CMMSC_STACK_STATE_NO_CHG}
  },

{ CMMSC_EVENT_RESUME,                CMMSC_OP_MODE_ANY,
   {CMMSC_STACK_STATE_ANY,           CMMSC_STACK_STATE_ANY,                   CMMSC_STACK_STATE_ANY},
   {{0,CMMSC_ACTION_NONE},           {1,CMMSC_ACTION_NONE},                      {2,CMMSC_ACTION_NONE}},
   { CMMSC_STACK_STATE_NO_CHG,       CMMSC_STACK_STATE_NO_CHG,                   CMMSC_STACK_STATE_NO_CHG}
  }
  
};

/*===========================================================================

            FUNCTION DECLARATIONS AND IMPLEMENTATION

This section contains function declaration and implementation for this
feature.

===========================================================================*/

static void   cmmsc_auto_determine_is_hybr_allowed (
    cmmsc_state_machine_s_type  *state_machine,
    sd_ss_mode_pref_e_type       mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref,
    boolean                      is_cdma_sub_avail,
    boolean                      is_gwl_sub_avail,
    /* in */

    boolean                     *is_hybr1_allowed,
    boolean                     *is_hybr2_allowed
    /* out */
);

/*===========================================================================

FUNCTION cmmsc_get_auto_routes

DESCRIPTION
  This function return the pointer to auto_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cmmsc_routing_table_auto_entry_s_type *cmmsc_get_auto_routes( void )
{
  return (cmmsc_routing_table_auto_entry_s_type*)&cmmsc_auto_routes;
}


/*===========================================================================

FUNCTION cmmsc_get_auto_routes_size

DESCRIPTION
  This function return the size of the auto_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int cmmsc_get_auto_routes_size( void )
{
  return ARR_SIZE(cmmsc_auto_routes);
}

/*===========================================================================

FUNCTION cmmsc_auto_routing_table_init

DESCRIPTION
  This function initializes AUTO Routing Table

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_auto_routing_table_init( cmmsc_routing_table_s_type *auto_route_table )
{
  auto_route_table->n_row = cmmsc_get_auto_routes_size();
  auto_route_table->n_stack = CMMSC_AUTO_STACK_NUM;
  auto_route_table->route_tbl = (cmmsc_routing_table_entry_base_s_type*)cmmsc_get_auto_routes();
  auto_route_table->route_tbl_id = CMMSC_ROUTE_TABLE_AUTO;
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_lte_do_irat

DESCRIPTION
  This function should be called to process the reselection reports
  from SD20/MMOC. currently used only for CM_LTE_DO_IRAT_F

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_rpt_lte_do_irat (
  cmmsc_param_base_s_type  *param
)
{
  cmmsc_state_machine_s_type  *state_machine;
  cmmsc_event_e_type          event = CMMSC_EVENT_NONE;
  cmmsc_event_base_s_type     event_lte_do_resel;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  }

  CM_MSG_LOW_0("RPT->MSC: IRAT");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If LTE is on main stack, remove GWL so that we don't trigger gwl acquisition
  */
  if (!cmss_is_hybr2_operational())
  {

    (void)cmph_remove_orig_mode_with_priority_end(CM_SS_MAIN);    

    #if defined( FEATURE_HICPS_STACK_SYNC_UP )
    (void)cmmsc_auto_update_gwl_operation (FALSE,
                                           SD_SS_PREF_REAS_USER,
                                           CM_MODE_PREF_GWL,
                                           FALSE,
                                           state_machine->asubs_id);
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send STACK_SYNC_UP_BSR indication to SD if there is service on MAIN or
  ** on HYBR HDR.
  */
  else
  {
    event = CMMSC_EVENT_LTE_DO_RESEL;
    event_lte_do_resel.event = event;
    event_lte_do_resel.state_machine = state_machine;

    cmmsc_state_machine_process(state_machine, &event_lte_do_resel);
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_redial_hdr_fail

DESCRIPTION
  This function should be called to process the faked ACQ_FAIL report
  from SD20 to trigger 1s BSR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_auto_proc_rpt_redial_hdr_fail(
  cmmsc_param_base_s_type    *param
)
{
  cmmsc_state_machine_s_type  *state_machine;
  cmmsc_event_e_type          event = CMMSC_EVENT_NONE;
  cmmsc_event_base_s_type     event_pkg;
  cmph_s_type                 *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  }

  CM_MSG_MED_0("RPT->MSC: REDIAL_HDR_FAIL");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybr 2 stack is not active, proceed with legacy operation
  */
  if(!cmss_is_hybr2_operational())
  {
    #if defined( FEATURE_HICPS_STACK_SYNC_UP )
    (void)cmmsc_auto_update_gwl_operation (TRUE,
                                            SD_SS_PREF_REAS_STACK_SYNC_UP,
                                            CM_MODE_PREF_GWL,
                                            TRUE ,
                                            state_machine->asubs_id);
    #endif
  }
  else
  {
    event = CMMSC_EVENT_RPT_REDIAL_HDR_FAIL;
    event_pkg.event = event;
    event_pkg.state_machine = state_machine;

    cmmsc_state_machine_process(state_machine, &event_pkg);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined FEATURE_CM_LTE) && (defined FEATURE_HDR_HANDOFF)
  if(ph_ptr->is_shorten_bsr)
  {
    ph_ptr->is_shorten_bsr = FALSE;
  }
  #endif

}

/*===========================================================================

FUNCTION cmmsc_auto_proc_disable_hybr_hdr

DESCRIPTION
  This function should be called to disable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_disable_hybr_hdr (
  cmmsc_param_base_s_type    *param
)
{
  cmmsc_state_machine_s_type     *state_machine;
  cmmsc_event_e_type             event = CMMSC_EVENT_NONE;
  cmmsc_event_force_pref_s_type  event_pkg;
  cm_orig_q_s_type               *ph_orig_top_ptr
    = cmtask_orig_para_get_top( CM_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  } 

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("ph_orig_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in hybrid operation, else ignore it.
  */
  if ( !cmss_is_hybr_status_on() )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is already disabled.
  */
  if (cmmsc_auto_get_is_hybr1_disabled(state_machine))
  {
    CM_MSG_HIGH_0( "HDR already disabled");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !sd_misc_is_mode_pref(
             cmph_map_cm_mode_pref_to_sd_mode_pref(
             ph_orig_top_ptr->orig->orig_mode_pref),
             SD_SS_MODE_PREF_HDR)
  )
  {
    /* HDR is already disabled.
    */
    CM_MSG_HIGH_0( "HDR already disabled");
    return;
  }

  /* Set is_hybr1_disabled flag to TRUE */
  cmmsc_auto_update_is_hybr1_disabled( state_machine, TRUE);

  /* Remove HDR from the capability */
  //cmmsc_set_stack_capability_auto(state_machine);
  cmmsc_set_stack_capability(state_machine);


  /* Force phone preference on SS-HDR instance if not in HDR call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    event = CMMSC_EVENT_FORCE_COLLOC;
    event_pkg.event = event;
    event_pkg.state_machine = state_machine;
    event_pkg.orig_mode = SD_SS_ORIG_MODE_COLLOC;
    event_pkg.hybr_pref = CM_HYBR_PREF_CDMA__HDR;
    event_pkg.pref_reas = SD_SS_PREF_REAS_USER;

    cmmsc_state_machine_process(state_machine, (cmmsc_event_base_s_type *)&event_pkg);
  }

  /* Set HDR pref. sys chg pending */
  else
  {
    CM_MSG_HIGH_0("not force preference due to call in progress");
    cmmsc_auto_update_is_kick_hdr_pending(state_machine, TRUE);
  }
} /* cmpref_proc_cmd_disable_hybr_hdr() */

/*===========================================================================

FUNCTION cmmsc_auto_proc_enable_hybr_hdr

DESCRIPTION
  This function should be called to enable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void     cmmsc_auto_proc_enable_hybr_hdr (
  cmmsc_param_proc_enable_hybr_hdr_s_type  *param
)
{
  cmmsc_state_machine_s_type        *state_machine;
  cmmsc_event_e_type                event = CMMSC_EVENT_NONE;
  cmmsc_event_force_pref_s_type     event_pkg;

  cm_orig_q_s_type      *ph_orig_hybr1_top_ptr = \
                        cmtask_orig_para_get_top( CM_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  } 

  if (ph_orig_hybr1_top_ptr == NULL)
  {
    CM_ERR_0("ph_orig_hybr1_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is already enabled */
  if (!cmmsc_auto_get_is_hybr1_disabled(state_machine))
  {
    return;
  }

  /* Set is_hybr1_disabled flag to FALSE */
  cmmsc_auto_update_is_hybr1_disabled( state_machine, FALSE);

  /* Update the capability to include HDR */
  //cmmsc_set_stack_capability_auto(state_machine);
  cmmsc_set_stack_capability(state_machine);

  /* Check if we are in hybrid operation, else ignore it.
  */
  if ( !cmss_is_hybr_status_on() )
  {
    CM_MSG_HIGH_0("Enabling hdr; not in hybrid" );
    return;
  }

  /* Inform SS-HDR instance if requested for.
  */
  if ( !param->send_pref_sys_cmd )
  {
    CM_MSG_HIGH_0("Skip sending pref.sys chgd cmd");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force phone preference on SS-HDR instance if not in HDR call.
  */
  if ( !cmcall_is_mobile_in_hdr_call() )
  {
    event = CMMSC_EVENT_FORCE_COLLOC;
    event_pkg.event = event;
    event_pkg.state_machine = state_machine;
    event_pkg.orig_mode = SD_SS_ORIG_MODE_COLLOC;
    event_pkg.hybr_pref =
      cmph_map_sd_hybr_pref_to_cm_hybr_pref(
      ph_orig_hybr1_top_ptr->orig->orig_int_hybr_pref);
    event_pkg.pref_reas = SD_SS_PREF_REAS_USER;

    cmmsc_state_machine_process(state_machine, (cmmsc_event_base_s_type *)&event_pkg);
  }

  /* Set HDR pref. sys chg pending */
  else
  {
    CM_MSG_HIGH_0("not force preference due to call in progress");
    cmmsc_auto_update_is_kick_hdr_pending(state_machine, TRUE);
  }
} /* cmpref_proc_cmd_enable_hybr_hdr() */

/*===========================================================================

FUNCTION cmmsc_auto_proc_call_end

DESCRIPTION
  This function should be called to process the call end event from CMCALL.
  This function would reinitiate SS-HDR acquisition if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_call_end (
  cmmsc_param_base_s_type    *param
)
{
  cmmsc_state_machine_s_type     *state_machine;
  cmmsc_event_e_type             event = CMMSC_EVENT_NONE;
  cmmsc_event_force_pref_s_type  event_pkg;

  cm_orig_q_s_type    *ph_orig_hybr1_top_ptr = \
                        cmtask_orig_para_get_top( CM_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  } 
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ph_orig_hybr1_top_ptr == NULL )
  {
    CM_ERR_0("ph_orig_hybr1_top_ptr null ptr");
    return;
  }

  CM_MSG_LOW_0("RPT->MSC: CALL_END");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if all HDR calls have ended.
  */
  if ( cmcall_is_mobile_in_hdr_call() )
  {
    /* HDR call still pending.
    */
    CM_MSG_HIGH_0("MSC: still in hdr call");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Kick HDR if one is pending and we are still in hybrid operation.
  */
  if (cmmsc_auto_is_kick_hdr_pending(state_machine))
  {
    CM_MSG_HIGH_2("MSC: call end on asubs_id %d, kick hdr, hybr_pref %d", state_machine->asubs_id, 
                     ph_orig_hybr1_top_ptr->orig->orig_int_hybr_pref);
    
    event = CMMSC_EVENT_KICK_HYBY1;
    event_pkg.event = event;
    event_pkg.state_machine = state_machine;
    event_pkg.orig_mode = SD_SS_ORIG_MODE_COLLOC;
    event_pkg.pref_reas = SD_SS_PREF_REAS_USER;
    event_pkg.hybr_pref = cmph_map_sd_hybr_pref_to_cm_hybr_pref(
                            ph_orig_hybr1_top_ptr->orig->orig_int_hybr_pref);

    cmmsc_state_machine_process(state_machine, (cmmsc_event_base_s_type *)&event_pkg);
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_timer

DESCRIPTION
  This function should be called to process the CM's 1 second timer tick.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_timer (
  cmmsc_param_base_s_type    *param
)
{
  cmmsc_state_machine_s_type  *state_machine;
  cmmsc_event_base_s_type      event_pkg;
  cmmsc_event_e_type           event = CMMSC_EVENT_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  } 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if "lost_main_timer" expired and reset the timer
  */
  if(cmmsc_auto_get_lost_main_uptime(state_machine)<= time_get_uptime_secs() )
  {
    cmmsc_auto_set_lost_main_uptime(state_machine, CMMSC_UPTIME_NONE);
    CM_MSG_HIGH_0("lost_main_timer expired");

    event = CMMSC_EVENT_TIMER_EXP_MAIN_LOST;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if "acq_hdr_timer" expired and reset the timer.
  */
  if(cmmsc_auto_get_acq_hybr_1_uptime(state_machine) <= time_get_uptime_secs() )
  {
    cmmsc_auto_set_acq_hybr_1_uptime(state_machine, CMMSC_UPTIME_NONE);
    CM_MSG_HIGH_0("acq_hybr_1_uptime expired");

    event = CMMSC_EVENT_TIMER_EXP_ACQ_HYBR1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if "avoid_hybr_hdr_timer" expired.
  */
  if(cmmsc_auto_get_avoid_hybr_hdr_uptime(state_machine) <= time_get_uptime_secs() )
  {
    /* reset the expired timer.
    */
    CM_MSG_HIGH_0("Avoid hybr hdr timer expired");
    cmmsc_auto_set_avoid_hybr_hdr_uptime(state_machine, CMMSC_UPTIME_NONE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  event_pkg.event = event;
  event_pkg.state_machine = state_machine;

  cmmsc_state_machine_process(state_machine, &event_pkg);
}

/*===========================================================================

FUNCTION cmpref_proc_rpt_hybr_bsr_to_hdr

DESCRIPTION
  This function should be called to process CM_HYBR_BSR_TO_HDR_F report
  from SD to trigger BSR.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsc_auto_proc_rpt_hybr_bsr_to_hdr(
  cmmsc_param_proc_rpt_hybr_bsr_to_hdr_s_type  *param
)
{
  cmmsc_state_machine_s_type     *state_machine;
  cmmsc_event_e_type             event = CMMSC_EVENT_NONE;
  cmmsc_event_force_pref_s_type  event_pkg;
  sd_hybr_bsr_to_hdr_s_type      hybr_bsr_to_hdr_status = SD_HYBR_BSR_TO_HDR_NONE;
  cmph_s_type                    *ph_ptr = cmph_ptr();
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  }
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hybr_bsr_to_hdr_status = param->hybr_bsr_to_hdr_status;
  
  CM_MSG_HIGH_1("RPT->MSC: BSR_TO_HDR hybr_bsr_to_hdr_status=%d",
                 hybr_bsr_to_hdr_status);

  /* HYBR_BSR_TO_HDR START indication is sent from hybr2 stack
  ** to do more preferred HDR scan. Hybr2 stack will enter into
  ** PWR SAVE and STACK_SYNC_UP_BSR indication is sent to
  ** HDR stack to do reselection.
  */
  if (hybr_bsr_to_hdr_status == SD_HYBR_BSR_TO_HDR_START)
  {
    event = CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_START;
    event_pkg.orig_mode = (cmss_is_hybr_status_on()?
                                          SD_SS_ORIG_MODE_COLLOC :
                                          SD_SS_ORIG_MODE_NONE );
    event_pkg.hybr_pref =
      cmph_map_sd_hybr_pref_to_cm_hybr_pref(   ph_ptr->int_hybr_pref);
    event_pkg.pref_reas = SD_SS_PREF_REAS_STACK_SYNC_UP_BSR;

  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* On HYBR_BSR_TO_HDR END if BSR success send stack syncup bsr to hybr 2 
  ** to perform more preferred LTE system scan after reselection
  ** timer expires.
  ** On BSR failure kick hybr2 stack to reacq LTE ACQ/SRV SYS.
  */
  else if (hybr_bsr_to_hdr_status == SD_HYBR_BSR_TO_HDR_END_SUCCESS ) 
  {
    event = CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_END;
  }
  else
  {
    event = CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_OTHER;
  }

  event_pkg.event = event;
  event_pkg.state_machine = state_machine;

  cmmsc_state_machine_process(state_machine, (cmmsc_event_base_s_type *)&event_pkg);
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_kick_hybr2

DESCRIPTION
  This function should be called to process CM_KICK_HYBR2_F report
  from SD indicates get net on main is done kick hybr2.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsc_auto_proc_rpt_kick_hybr2(
  cmmsc_param_base_s_type  *param
)
{
  cmmsc_state_machine_s_type     *state_machine;
  cmmsc_event_e_type             event = CMMSC_EVENT_NONE;
  cmmsc_event_force_pref_s_type  event_pkg;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_0("cmmsc_auto_proc_rpt_kick_hybr2()");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = param->state_machine;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
 
  event = CMMSC_EVENT_KICK_HYBR2;

  event_pkg.event = event;
  event_pkg.state_machine = state_machine;

  cmmsc_state_machine_process(state_machine, (cmmsc_event_base_s_type *)&event_pkg);
}


/*===========================================================================

FUNCTION cmmsc_auto_restore_3gpp_policy_chgd

DESCRIPTION
  This function restores 3GPP back to phone object if all the conditions meet.
  Conditions:
  1. 3GPP is removed due to HICPS.
  2. and current max mode pref capability is 3GPP only.
  3. and top of the priority queue is phone object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_restore_3gpp_policy_chgd (
  cmmsc_state_machine_s_type  *state_machine,
  cm_ss_e_type                 cm_ss,
  cm_mode_pref_e_type         *mode_pref
)
{
  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    return;
  }   

  if (cm_ss == CM_SS_MAIN || cm_ss == CM_SS_MAX)
  {
    cm_orig_q_s_type        *ph_orig_top_ptr = cmtask_orig_para_get_top(CM_SS_MAIN);
    cmph_s_type             *ph_ptr   = cmph_ptr();
    boolean                  is_top_ph_obj = FALSE;
    boolean                  is_ph_obj_3gpp_mode   = FALSE;
    boolean                  is_ph_ptr_3gpp_mode   = FALSE;
    sd_ss_mode_pref_e_type   mode_cap = SD_SS_MODE_PREF_NONE;
    boolean                  is_3gpp_only = FALSE;
    sd_ss_mode_pref_e_type   ph_ptr_3gpp_mode = SD_SS_MODE_PREF_NONE;

    if(ph_orig_top_ptr == NULL)
    {
      CM_ERR_0("Top ptr is NULL for MAIN stack");
      return;
    }

    /* Check if the top of the priority queue is phone obj */
    is_top_ph_obj = (ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ);

    /* Check if the phone obj has 3GPP mode */
    is_ph_obj_3gpp_mode =
      cmcall_misc_is_mode_pref(ph_orig_top_ptr->orig->orig_mode_pref, 
                               CM_MODE_PREF_GWL);

    /* Check if phone ptr has 3GPP */
    is_ph_ptr_3gpp_mode =
      cmcall_misc_is_mode_pref(ph_ptr->main_stack_info.pref_info.mode_pref ,
                               CM_MODE_PREF_GWL);
    /* Get 3GPP mode from ph ptr */
    ph_ptr_3gpp_mode = SD_GET_COMMON_MODE(SD_SS_MODE_PREF_GWL,
                      cmph_map_cm_mode_pref_to_sd_mode_pref(
                             ph_ptr->main_stack_info.pref_info.mode_pref));

    /* Check if the mode cap is 3GPP only */
    mode_cap = cmph_map_sys_mode_mask_to_sd_mode_pref(
                      state_machine->stack_common_info.ue_mode_capability);
    is_3gpp_only = (SD_GET_COMMON_MODE(mode_cap, SD_SS_MODE_PREF_CDMA_HDR)
                  == SD_SS_MODE_PREF_NONE);

    /* If conditions meet, update ph obj and mode pref passed into the func */
    if (is_top_ph_obj && is_3gpp_only && !is_ph_obj_3gpp_mode && is_ph_ptr_3gpp_mode)
    {
      ph_orig_top_ptr->orig->orig_mode_pref = 
       cm_add_mode_pref_components(ph_orig_top_ptr->orig->orig_mode_pref,1,
                                      ph_ptr_3gpp_mode);
      *mode_pref = cm_add_mode_pref_components(*mode_pref,1,
                                      ph_ptr_3gpp_mode);
      CM_MSG_HIGH_2("MSC: HICPS: Restoring 3GPP policy chgd, ph obj mode_pref=%d, pref mode_pref=%d", 
                     ph_orig_top_ptr->orig->orig_mode_pref, 
                     *mode_pref);
    }
    
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_update_gwl_operation

DESCRIPTION
  This function should be called to update the GWL operation on MAIN stack.
  Since HDR and GWL can’t operate simultaneously, whenever CMPREF is kicking HDR (or)
  starting HDR (or) knows that HDR is in power_save. it should set the GWL
  operation accordingly

  allowed = TRUE will enable GWL operation of main stack if GWL is supported
                 by the phone+user and current preferences don’t include GWL
            FALSE will disable GWL operation on main stack when CDMA (any-gwl)
                  is allowed on the main stack.

  reason = USER           will trigger SD to restart the OOS scripts.
           STACK_SYNC_UP  will trigger SD to add GWL but continue with previous
                          script
           STACk_SYNC_UP_BSR will trigger SD to add GWL and start BSR
                             algorithm.
  is_hdr_acq_fail = TRUE will restore GWL on main and shorten LTE BSR to 1 sec if 1x has
                 service and HDR acq failed.
DEPENDENCIES
  None.

RETURN VALUE
  Boolean: TRUE if action has been taken
           FALSE if no action was taken.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_update_gwl_operation(

       boolean                  allowed,
        /* is GWL operation allowed or not */

       sd_ss_pref_reas_e_type   reason,
         /* Reason for change in GWL operation */

       cm_mode_pref_e_type      cm_mode_pref,
         /* 3gpp mode that needs to be updated */

       boolean                  is_hdr_acq_fail,
         /* is HDR acq fail happened on SD_SS_HYBR_HDR stack */

       sys_modem_as_id_e_type   asubs_id
)
{
  #ifndef FEATURE_HICPS_STACK_SYNC_UP
  SYS_ARG_NOT_USED(allowed);
  SYS_ARG_NOT_USED(reason);
  SYS_ARG_NOT_USED(cm_mode_pref);
  SYS_ARG_NOT_USED(is_hdr_acq_fail);
  return FALSE;

  #else

  boolean is_gw_mode          = FALSE;
  boolean is_cdma_mode        = FALSE;
  boolean is_ph_obj_cdma_mode = FALSE;
  boolean is_ph_obj_gw_mode   = FALSE;
  boolean is_ph_ptr_gw_mode   = FALSE;
  boolean is_cap_hdr_mode     = FALSE;
  cm_act_type_e_type pm_orig_obj        = CM_ACT_TYPE_POLICYMAN_RESTRICT;
  cm_orig_q_s_type  *pm_orig_ptr =
                        cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                          CM_ACT_TYPE_POLICYMAN_RESTRICT );
  cm_orig_q_s_type  *ph_orig_ptr =
                        cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                          CM_ACT_TYPE_PH_OBJ);

  cmph_s_type       *ph_ptr   = cmph_ptr();
  cm_pref_s_type    *main_stack_pref    = NULL;
  sd_ss_mode_pref_e_type  mode_cap = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type  sd_mode_pref  =
                          cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);

  #ifdef FEATURE_CM_LTE
  cmss_s_type              *ss_ptr = cmss_ptr();
      /* Point at serving system object */
  CM_ASSERT( ss_ptr != NULL );
  #endif

  if(pm_orig_ptr == NULL)
  {
    pm_orig_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                    CM_ACT_TYPE_PH_OBJ );
    pm_orig_obj = CM_ACT_TYPE_PH_OBJ;
  }

  if(pm_orig_ptr == NULL || ph_ptr == NULL || ph_orig_ptr == NULL)
  {
    CM_ERR_0( "ph_orig_ptr is NULL");
    return FALSE;
  }

  CM_MSG_HIGH_1("HICPS: Performing operation on Object %d",pm_orig_obj);
  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(asubs_id))
  {
    return FALSE;
  } 
  
  /* update cm_mode_pref based on the mode mask on MAIN stack
  */
  mode_cap = cmmsc_get_cm_ss_mode_cap(CM_SS_MAIN);

  is_cap_hdr_mode = SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_HDR);

  sd_mode_pref = sd_misc_get_common_mode_pref(sd_mode_pref, mode_cap);

  main_stack_pref = &(ph_ptr->main_stack_info.pref_info);
  /*
  ** CHECK HARDWARE CAPABILITY.
  */
  /* Check the CDMA mode against the phone's band pref
  */
  is_cdma_mode = cmph_is_valid_mode_pref( CM_MODE_PREF_CDMA_ONLY, asubs_id);

  /* Check the GW mode against the phone's band pref
  */
  is_gw_mode = cmph_is_valid_mode_pref( cm_mode_pref, asubs_id);

  /*
  ** CHECK USER PREFERENCE ON THE PHN OBJECT.
  */
  /* Check if the phone has CDMA mode
  */
  is_ph_obj_cdma_mode =
  cmcall_misc_is_mode_pref( pm_orig_ptr->orig->orig_mode_pref ,CM_MODE_PREF_CDMA_HDR_ONLY );

  /* Check if the phone has GW mode
  */
  is_ph_obj_gw_mode = SD_MODE_CONTAIN(sd_mode_pref, 
                           cmph_map_cm_mode_pref_to_sd_mode_pref(
                             pm_orig_ptr->orig->orig_mode_pref));

  /*
  ** Check if phone ptr has GW.
  */
  is_ph_ptr_gw_mode = SD_MODE_CONTAIN(sd_mode_pref, 
                           cmph_map_cm_mode_pref_to_sd_mode_pref(
                             main_stack_pref->mode_pref));

  CM_MSG_HIGH_6("MSC: HICPS: mode_pref: cm %d ph obj %d ph_ptr %d, is_cdma_mode %d is_ph_obj_cdma_mode %d 1x_subs %d",
                 cm_mode_pref,
                 ph_orig_ptr->orig->orig_mode_pref,
                 ph_ptr->main_stack_info.pref_info.mode_pref,
                 is_cdma_mode,
                 is_ph_obj_cdma_mode,
                 ph_ptr->is_cdma_subscription_available);
  CM_MSG_HIGH_3("MSC: HICPS: is_gw_mode: %d, is_ph_ptr_gw_mode: %d, is_ph_obj_gw_mode: %d",
                 is_gw_mode,
                 is_ph_ptr_gw_mode,
                 is_ph_obj_gw_mode);

  /* If this report is for main system determination, we did not kick
  ** HDR so far and hybrid is on, kick HDR into acquisition.
  */
  if( !allowed            &&
      is_cdma_mode        &&
      is_ph_obj_gw_mode   &&
      is_ph_obj_cdma_mode &&
      ph_ptr->is_cdma_subscription_available &&
      is_cap_hdr_mode
    )
  {
    /* Need to update the priority Q's hicps object to reflect acq fail
    ** on MAIN, force the main stack to CDMA only.
    */
    /* If all the conditions are met, kick the main stack
    */
    /* For 9K fusion allow removing GWL from main stack even when there is
    ** no CDMA in mode preference. Main would be in power save
    */
    cm_mode_pref_e_type mode_pref = CM_MODE_PREF_NONE;

    /* Remove GWL From the mode pref and insert in the priority Q
    */
    mode_pref = cmcall_remove_mode_pref_components( main_stack_pref->mode_pref,
                                                    1,
                                                    SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE);

    CM_MSG_HIGH_1("HICPS:Removing 3GPP from MAIN, Attempt to force pref %d",mode_pref);
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_ASSERT( mode_pref != CM_MODE_PREF_NONE );

    /* If there is an error, break
    */
    if (mode_pref == CM_MODE_PREF_NONE )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    {
      cm_network_sel_mode_pref_e_type local_net_sel_pref;

      local_net_sel_pref = (pm_orig_obj == CM_ACT_TYPE_PH_OBJ)?
                            main_stack_pref->network_sel_mode_pref:
                            cmpmprx_compute_restricted_mode(
                                      cmph_map_cm_ss_to_subs(CM_SS_MAIN),
                                      main_stack_pref->network_sel_mode_pref);

    (void) cmph_force_pref_on_the_fly(
                  CM_SS_MAIN,
                  ph_ptr,
                  reason,
                    pm_orig_obj,
                    main_stack_pref->pref_term,
                  mode_pref,
                    main_stack_pref->band_pref,
                    main_stack_pref->lte_band_pref,
                    main_stack_pref->tds_band_pref,
                    main_stack_pref->prl_pref,
                    main_stack_pref->roam_pref,
                    main_stack_pref->hybr_pref,
                    main_stack_pref->plmn,
                    main_stack_pref->srv_domain_pref,
                  CM_OTASP_ACT_CODE_NONE,
                    main_stack_pref->acq_order_pref,
                    local_net_sel_pref,
                    pm_orig_ptr->orig->act_id,
                  CM_ACT_UPDATE_REAS_ACT_START,
                  FALSE,
                  cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                  CM_DEFAULT_VALUE,
                    main_stack_pref->csg_id,
                    main_stack_pref->csg_rat);

      /*Update HICPS in Phone Object Also in FULL RAT MODE*/
	  
      if ( pm_orig_obj == CM_ACT_TYPE_POLICYMAN_RESTRICT &&
	       ph_orig_ptr != NULL)
      {
	    (void) cmph_force_pref_on_the_fly(
                  CM_SS_MAIN,
                  ph_ptr,
                  reason,
                    CM_ACT_TYPE_PH_OBJ,
                    main_stack_pref->pref_term,
                  mode_pref,
                    main_stack_pref->band_pref,
                    main_stack_pref->lte_band_pref,
                    main_stack_pref->tds_band_pref,
                    main_stack_pref->prl_pref,
                    main_stack_pref->roam_pref,
                    main_stack_pref->hybr_pref,
                    main_stack_pref->plmn,
                    main_stack_pref->srv_domain_pref,
                  CM_OTASP_ACT_CODE_NONE,
                    main_stack_pref->acq_order_pref,
                    main_stack_pref->network_sel_mode_pref,
                    ph_orig_ptr->orig->act_id,
                  CM_ACT_UPDATE_REAS_ACT_START,
                  FALSE,
                  cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                  CM_DEFAULT_VALUE,
                    main_stack_pref->csg_id,
                    main_stack_pref->csg_rat);
    }

    }

    /* Start the timer for avoiding GWL. */
    cmph_activate_hicps_gwl_avoid_timer();

    return TRUE;
  } /* !allowed */
  else if ( allowed             &&
            is_gw_mode          &&
            is_ph_ptr_gw_mode   &&
            !is_ph_obj_gw_mode
          )
  {

    if (ph_ptr->is_mode_pref_none == TRUE)
    {
      CM_MSG_HIGH_0("HICPS: Skip Restoring GWL to MAIN stack due to DAM ");
      return FALSE;
    }

    CM_MSG_HIGH_0("HICPS: Restoring 3GPP to MAIN stack");

    {
      cm_network_sel_mode_pref_e_type local_net_sel_pref;

      local_net_sel_pref = (pm_orig_obj == CM_ACT_TYPE_PH_OBJ)?
                            main_stack_pref->network_sel_mode_pref:
                            cmpmprx_compute_restricted_mode(
                                      cmph_map_cm_ss_to_subs(CM_SS_MAIN),
                                      main_stack_pref->network_sel_mode_pref);

    (void) cmph_force_pref_on_the_fly(CM_SS_MAIN,
                                      ph_ptr,
                                      reason,
                                        pm_orig_obj,
                                        main_stack_pref->pref_term,
                                        main_stack_pref->mode_pref,
                                        main_stack_pref->band_pref,
                                        main_stack_pref->lte_band_pref,
                                        main_stack_pref->tds_band_pref,
                                        main_stack_pref->prl_pref,
                                        main_stack_pref->roam_pref,
                                        main_stack_pref->hybr_pref,
                                        main_stack_pref->plmn,
                                        main_stack_pref->srv_domain_pref,
                                      CM_OTASP_ACT_CODE_NONE,
                                        main_stack_pref->acq_order_pref,
                                        local_net_sel_pref,
                                        pm_orig_ptr->orig->act_id,
                                      CM_ACT_UPDATE_REAS_ACT_START,
                                      FALSE,
                                      cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                      CM_DEFAULT_VALUE,
                                        main_stack_pref->csg_id,
                                        main_stack_pref->csg_rat);

	  /*Exit HICPS in Phone Object Also in FULL RAT MODE*/
	  
      if ( pm_orig_obj == CM_ACT_TYPE_POLICYMAN_RESTRICT &&
	       ph_orig_ptr != NULL)
      {
        (void) cmph_force_pref_on_the_fly(CM_SS_MAIN,
                                      ph_ptr,
                                      reason,
                                        CM_ACT_TYPE_PH_OBJ,
                                        main_stack_pref->pref_term,
                                        main_stack_pref->mode_pref,
                                        main_stack_pref->band_pref,
                                        main_stack_pref->lte_band_pref,
                                        main_stack_pref->tds_band_pref,
                                        main_stack_pref->prl_pref,
                                        main_stack_pref->roam_pref,
                                        main_stack_pref->hybr_pref,
                                        main_stack_pref->plmn,
                                        main_stack_pref->srv_domain_pref,
                                      CM_OTASP_ACT_CODE_NONE,
                                        main_stack_pref->acq_order_pref,
                                        main_stack_pref->network_sel_mode_pref,
                                        ph_orig_ptr->orig->act_id,
                                      CM_ACT_UPDATE_REAS_ACT_START,
                                      FALSE,
                                      cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                      CM_DEFAULT_VALUE,
                                        main_stack_pref->csg_id,
                                        main_stack_pref->csg_rat);
    }
    }

    *cmph_get_is_kicked_hybr_ptr() = FALSE;

    /* Deactivate the GWL avoidance timer. */
    cmph_deactivate_hicps_gwl_avoid_timer();
    return TRUE;
  } /* allowed */
  #if (defined FEATURE_CM_LTE) && (defined FEATURE_HDR_HANDOFF)
  /* If 1x has service in main stack, while HYBR HDR stack is PWR_SAVE,
  ** shorten the BSR timer to 1 sec.
  */
  else if( is_hdr_acq_fail && is_cdma_mode && is_gw_mode && is_ph_ptr_gw_mode &&
     ph_ptr->is_shorten_bsr
     &&
     (!cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1) ||
      (cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1) && !cmss_ptr()->info.is_hybr_gw_operational))
     &&
     (!cmcall_check_if_no_srv( FALSE ) &&
      (ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA)))
  {
   dword curr_uptime  = time_get_uptime_secs();

   CM_MSG_HIGH_0("HICPS: Restoring 3GPP to MAIN stack, LTE BSR 1 sec");

    (void) cmph_force_pref_on_the_fly(
                CM_SS_MAIN,
                ph_ptr,
                SD_SS_PREF_REAS_USER,
                pm_orig_obj,
                main_stack_pref->pref_term,
                main_stack_pref->mode_pref,
                main_stack_pref->band_pref,
                main_stack_pref->lte_band_pref,
                main_stack_pref->tds_band_pref,
                main_stack_pref->prl_pref,
                main_stack_pref->roam_pref,
                main_stack_pref->hybr_pref,
                main_stack_pref->plmn,
                main_stack_pref->srv_domain_pref,
                CM_OTASP_ACT_CODE_NONE,
                main_stack_pref->acq_order_pref,
                main_stack_pref->network_sel_mode_pref,
                pm_orig_ptr->orig->act_id,
                CM_ACT_UPDATE_REAS_USER,
                FALSE,
                cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                CM_DEFAULT_VALUE,
                main_stack_pref->csg_id,
                main_stack_pref->csg_rat);

    *cmph_get_is_kicked_hybr_ptr() = FALSE;

    if(ph_ptr->shorten_bsr_timer < curr_uptime)
    {
      ph_ptr->shorten_bsr_timer = curr_uptime+3;
      CM_MSG_HIGH_1("start shorten bsr timer=%d",ph_ptr->shorten_bsr_timer );
    }

    return TRUE;

  }
  #else
  SYS_ARG_NOT_USED(is_hdr_acq_fail);
  #endif
  #ifdef FEATURE_CM_LTE
  ss_ptr = NULL; //This is to suppress lint wanring.
  /* lint -e{529} */
  #endif //FEATURE_CM_LTE
  return FALSE;

  #endif /* FEATURE_HICPS_STACK_SYNC_UP */
}

/*===========================================================================

FUNCTION cmmsc_action_force_ph_ext

DESCRIPTION
  Force the phone preference on stack_idx with additional params carried by evt_package.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void    cmmsc_action_force_ph_ext (
    cmmsc_state_machine_s_type        *state_machine,
    cmmsc_event_force_pref_s_type     *evt_package,
    int                                stack_idx
)
{
  cm_preference_info_s_type      pref_info;
  cmph_s_type                    *ph_ptr = cmph_ptr();
  cm_ss_e_type                   cm_ss = CM_SS_MAX;
  cm_orig_q_s_type               *ph_obj = NULL;

  if (state_machine == NULL || evt_package == NULL)
  {
    CM_ERR_2("null ptr %d %d", state_machine, evt_package);
    return;
  }

  cm_ss = cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine);
  ph_obj = cmtask_orig_para_search_act_id(cm_ss, (cm_act_id_type)cmph_ptr());

  pref_info.pref_reas = evt_package->pref_reas;
  pref_info.orig_mode = evt_package->orig_mode;
  pref_info.hybr_pref = evt_package->hybr_pref;
  
  pref_info.mode_pref = ph_ptr->main_stack_info.pref_info.mode_pref ;
  pref_info.band_pref = ph_ptr->main_stack_info.pref_info.band_pref ;
  pref_info.lte_band_pref = ph_ptr->main_stack_info.pref_info.lte_band_pref ;
  pref_info.tds_band_pref = ph_ptr->main_stack_info.pref_info.tds_band_pref;
  pref_info.prl_pref = ph_ptr->main_stack_info.pref_info.prl_pref;
  pref_info.roam_pref = ph_ptr->main_stack_info.pref_info.roam_pref;
  pref_info.srv_domain_pref = ph_ptr->main_stack_info.pref_info.srv_domain_pref;
  pref_info.acq_order_pref = ph_ptr->main_stack_info.pref_info.acq_order_pref;
  pref_info.rat_acq_order_ptr
    = (cmph_get_acq_pri_order_per_sub(cm_ss));
  pref_info.camp_mode_pref = SD_SS_PREF_CAMP_MODE_NO_CHANGE;
  pref_info.sys_id.sys_id.id.plmn = ph_ptr->main_stack_info.pref_info.plmn;
  pref_info.sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info.sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
  pref_info.otasp_band = cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE);
  pref_info.otasp_blksys = cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE);
  pref_info.avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info.avoid_time = 0;

  pref_info.hybr1_pref.hybr_mode_pref = ph_ptr->main_stack_info.pref_info.mode_pref;
  pref_info.hybr1_pref.hybr_orig_mode = evt_package->orig_mode;
  if (ph_obj != NULL)
  {
    pref_info.pref_update_reason = ph_obj->orig->pref_update_reason;
  }
  else
  {
    pref_info.pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  } 
  /* Force preference to the stack */
  cmmsc_send_sys_sel_pref_chgd_cmd(state_machine, stack_idx, &pref_info, NULL);
}

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto

DESCRIPTION
  This function set stack capability for auto.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto(
    cmmsc_state_machine_s_type  *state_machine
)
{
  sys_sys_mode_mask_e_type  pm_mode    = SYS_SYS_MODE_MASK_NONE;
  sd_ss_mode_pref_e_type    mode       = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type    band       = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type    tds_band   = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type  lte_band   = SYS_LTE_BAND_MASK_CONST_NONE;
  size_t                    stack_idx  = 0;
  size_t                    table_idx  = 0;

  typedef struct {
    cmmsc_state_machine_op_mode_e_type   op_mode;
    sd_ss_mode_pref_e_type               max_cap[3];
  } cmmsc_op_mode_max_cap_table_entry;

  /* Max cap per op mode and stack */
  static cmmsc_op_mode_max_cap_table_entry  op_mode_max_cap_table[] =
  {
    { CMMSC_OP_MODE_AUTO_MAIN,
       {SD_SS_MODE_PREF_ANY,            SD_SS_MODE_PREF_ANY,  SD_SS_MODE_PREF_NONE}
       },
    { CMMSC_OP_MODE_AUTO_MAIN_HYBR1,
       {SD_SS_MODE_PREF_ANY,            SD_SS_MODE_PREF_ANY,   SD_SS_MODE_PREF_NONE}
       },
    { CMMSC_OP_MODE_AUTO_MAIN_HYBR2,
       {SD_SS_MODE_PREF_ANY_BUT_LTE,    SD_SS_MODE_PREF_ANY,  SD_SS_MODE_PREF_LTE}
       },
    { CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2,
       {SD_SS_MODE_PREF_ANY_BUT_LTE,    SD_SS_MODE_PREF_ANY_BUT_LTE,   SD_SS_MODE_PREF_LTE}
       }
  };

  /* Make sure the max cap size is not smaller than CMMSC_AUTO_STACK_NUM */
  if (ARR_SIZE(op_mode_max_cap_table[0].max_cap) < CMMSC_AUTO_STACK_NUM)
  {
    CM_ERR_FATAL_2("op_mode_max_cap_table max cap array size %d is smaller than %d",
                             ARR_SIZE(op_mode_max_cap_table[0].max_cap),
                             CMMSC_AUTO_STACK_NUM);
    return;
  }

  /* Get PM capability */
  cmmsc_get_policy_mode_band_cap(state_machine,
                                 &pm_mode,
                                 &band,
                                 &tds_band,
                                 &lte_band);

  mode = cmph_map_sys_mode_mask_to_sd_mode_pref(pm_mode);

  /* Go through table and match op_mode.
  ** Set cap for each stack
  */
  for (table_idx = 0; ARR_SIZE(op_mode_max_cap_table); table_idx ++)
  {
    if (op_mode_max_cap_table[table_idx].op_mode == state_machine->op_mode)
    {
      for (stack_idx = 0; stack_idx < CMMSC_AUTO_STACK_NUM; stack_idx ++)
      {
        /* Set mode cap per intersection of PM cap and max_cap per op mode
        */
        state_machine->stack[stack_idx].capablity.mode
           = SD_GET_COMMON_MODE(mode,
              (op_mode_max_cap_table[table_idx]).max_cap[stack_idx]);

        /* Remove hdr if hybr1 is disabled
        */
        if (cmmsc_auto_get_is_hybr1_disabled(state_machine))
        {
          state_machine->stack[stack_idx].capablity.mode
             = sd_misc_remove_mode_pref(
                state_machine->stack[stack_idx].capablity.mode,
                SD_SS_MODE_PREF_HDR);
        }

        /* Set other capabilities
        */
        state_machine->stack[stack_idx].capablity.domain   = SD_SS_SRV_DOMAIN_PREF_CS_PS;
        state_machine->stack[stack_idx].capablity.band     = band;
        state_machine->stack[stack_idx].capablity.tds_band = tds_band;
        state_machine->stack[stack_idx].capablity.lte_band = lte_band;
      }
      return;
    }
  }

  /* If reach here, op mode is found in the table
  */
  CM_ERR_1("Op mode %dis not found in the table", state_machine->op_mode);
}

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto_emergency

DESCRIPTION
  This function set stack capability for auto in emergency call scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto_emergency(
    cmmsc_state_machine_s_type  *state_machine
)
{
  size_t                    index = 0;

  /* For emergency call, enable all capability per PM max mode cap for stack 0 */
  if(cmph_is_msim())
  {
    state_machine->stack[0].capablity.mode      
      = cmph_map_sys_mode_mask_to_sd_mode_pref(
          (state_machine->stack_common_info.ue_max_mode_capability));
  }
  else
  {
    state_machine->stack[0].capablity.mode = SD_SS_MODE_PREF_ANY;
  }
  state_machine->stack[0].capablity.domain    = SD_SS_SRV_DOMAIN_PREF_CS_PS;
  state_machine->stack[0].capablity.band      = SD_SS_BAND_PREF_ANY;
  state_machine->stack[0].capablity.tds_band  = SD_SS_BAND_PREF_TDS_ANY;
  state_machine->stack[0].capablity.lte_band  = SYS_LTE_BAND_MASK_CONST_ANY;

  for (index = 1; index < CMMSC_AUTO_STACK_NUM; index ++)
  {
    state_machine->stack[index].capablity.mode      = SD_SS_MODE_PREF_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_is_ue_mode_csfb

DESCRIPTION
  This function returns if the current ue mode is csfb.

DEPENDENCIES
  None.

RETURN VALUE
  sys_ue_mode_e_type.

===========================================================================*/
boolean  cmmsc_auto_is_ue_mode_csfb (
    cmmsc_state_machine_s_type  *state_machine
)
{
  sys_ue_mode_e_type ue_mode = cmmsc_get_curr_msc_ue_mode(state_machine);

  switch(ue_mode)
  {
    case SYS_UE_MODE_CSFB:
    case SYS_UE_MODE_CSFB_ONLY:
      return TRUE;

    #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    case SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED:
      /* CSFB is enabled only when SOA also in CSFB */
      if(cmsoa_get_current_sv_oprt_state() == CMSOA_SV_OPRT_MODE_SVLTE)
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    #endif
    
    default:
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_retrieve_pm_ue_mode

DESCRIPTION
  This function gets the policy manager ue mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void    cmmsc_auto_retrieve_pm_ue_mode (
    cmmsc_state_machine_s_type  *state_machine
)
{
  sys_ue_mode_e_type curr_ue_mode;
    
  /* Get new ue mode if policy manager has updated ue mode */
  if( cmpmprx_get_ue_mode(state_machine->asubs_id,&curr_ue_mode)== TRUE)
  {
    if( curr_ue_mode != state_machine->stack_common_info.ue_mode )
    {
      uint8              stackMask = BM(SYS_MODEM_STACK_ID_1) | BM(SYS_MODEM_STACK_ID_2);
      CM_MSG_HIGH_3("PMPRX -> CM: ue_mode_chgd: %d -> %d, asubs_id=%d", 
                        state_machine->stack_common_info.ue_mode, 
                        curr_ue_mode, 
                        state_machine->asubs_id);
      #if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)                        
      if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id))
      {
        cmsoa_srlte_ue_mode_update(curr_ue_mode);
      }
      #endif
      state_machine->stack_common_info.ue_mode = curr_ue_mode;
      state_machine->stack_common_info.ue_mode_forced = FALSE;
      policyman_reset_acq_fail(state_machine->asubs_id, stackMask);
      CM_MSG_HIGH_1("Reset acq_fail on satck %d", stackMask);
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr2_switched_on

DESCRIPTION
  The function returns if hybr2 stack is switched on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched on.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean  cmmsc_auto_is_hybr2_switched_on (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
)
{
  if ( (new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR2||
      new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
      &&
     (old_op_mode == CMMSC_OP_MODE_AUTO_MAIN ||
      old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1 )
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr2_switched_off

DESCRIPTION
  The function returns if hybr2 stack is switched off.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched on.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean  cmmsc_auto_is_hybr2_switched_off (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
)
{
  if ( (new_op_mode == CMMSC_OP_MODE_AUTO_MAIN||
      new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1)
      &&
     (old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR2 ||
      old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION cmmsc_auto_is_hybr2_on

DESCRIPTION
  The function returns if hybr2 stack is on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr2 is on.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_hybr2_on (cmmsc_state_machine_op_mode_e_type op_mode)
{
  if (op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR2||
      op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_set_pref_update_reas

DESCRIPTION
  The function compute pref update reas in , when ue_mode changes as below 
  CSFB -> CSFB_PREF 
  CSFB_PREF (single stack) -> CSFB
  this helps SD to isolate this case to return continue
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void   cmmsc_auto_set_pref_update_reas (
    cmmsc_state_machine_s_type         *state_machine,
    cmmsc_state_machine_op_mode_e_type  old_op_mode,
    cmmsc_state_machine_op_mode_e_type  new_op_mode,
    sys_ue_mode_e_type                  old_ue_mode,
    sys_ue_mode_e_type                  new_ue_mode,
    cm_preference_info_s_type           *pref_info
)
{
  /* SRLTE handling */
  if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id))
  {
    /* compute pref update reas in , when ue_mode changes as below 
    ** CSFB -> CSFB_PREF 
    ** CSFB_PREF (single stack) -> CSFB
    ** this helps SD to isolate this case to return continue
    */
    if(cmmsc_auto_is_hybr2_switched_on(old_op_mode, new_op_mode) == FALSE &&
       cmmsc_auto_is_hybr2_switched_off(old_op_mode, new_op_mode) == FALSE &&
      ((SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == old_ue_mode &&
        SYS_UE_MODE_CSFB == new_ue_mode)
           ||
       (SYS_UE_MODE_CSFB == old_ue_mode &&
        SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED  == new_ue_mode)))    
    {
      pref_info->pref_update_reason = SD_SS_PREF_UPDATE_REASON_SWITCH_UE_MODE;
      CM_MSG_HIGH_0("srlte: ue_mode_chg hybr2 not toggled");
    }
  }
  
}

#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
/*===========================================================================

FUNCTION cmmsc_auto_unblock_1x_sxlte_plmn

DESCRIPTION
  For all PLMNs in svlte plmn list, send unblocking cmd to NAS.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_unblock_1x_sxlte_plmn(
  sys_modem_as_id_e_type              asubs_id
)
{

  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  cmsoa_svlte_plmn_id_node_s_type *node_ptr = NULL;
  
  int q_counter = 0;
  uint32 loop = 0;

  sys_plmn_list_s_type *local_fplmn_list = NULL;

  cmregprx_info_s_type           *cmregprx_info_ptr = cmregprx_get_info_ptr();
  /* Store reported FPLMN list */
  local_fplmn_list = &(cmregprx_info_ptr->fplmn_list[SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN].plmn_list);
	
CM_MSG_HIGH_5("subsfmode: srlte %d, sxlte %d, svlte %d, local_fplmn_list.length %d, asubs_id %d",
              (cmph_is_subs_feature_mode_srlte(asubs_id)),
              cmph_is_sxlte(),
              (cmph_is_subs_feature_mode_svlte(asubs_id)),
              local_fplmn_list->length, asubs_id);
  
  if ((cmph_is_subs_feature_mode_srlte(asubs_id) || 
  	   (cmph_is_sxlte() &&
        cmph_is_subs_feature_mode_svlte(asubs_id))) &&
        (local_fplmn_list != NULL) && (local_fplmn_list->length > 0)) 
  {  
    boolean is_1xsrlte_mcc = FALSE;
	
    for(loop=0 ; loop<(local_fplmn_list->length) ; loop++)
    {
      if(policyman_svd_possible_on_plmn(&(local_fplmn_list->plmn[loop]), 
                                         FALSE, &is_1xsrlte_mcc ) ==
                                         POLICYMAN_STATUS_SUCCESS)
      {
         CM_MSG_HIGH_4("SRLTE: UNBLOCK_PLMNs: PLMN:0x%x 0x%x 0x%x.  is_1xsrlte_mcc:%d",
                       local_fplmn_list->plmn[loop].identity[0],
                       local_fplmn_list->plmn[loop].identity[1],
                       local_fplmn_list->plmn[loop].identity[2], is_1xsrlte_mcc);
                         
        if(is_1xsrlte_mcc == TRUE)
        {
          cmph_reg_send_unblock_plmn_req(local_fplmn_list->plmn[loop], asubs_id, SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE);
        }
        
      }
    }

  }
  else if(!cmph_is_sxlte() &&
     cmph_is_subs_feature_mode_svlte(asubs_id) &&
     (cmsoa_state != NULL ) && (cmsoa_state->is_activated))
  {
    node_ptr = (cmsoa_svlte_plmn_id_node_s_type *)q_check(&(cmsoa_state->svlte_plmn_list));	

    q_counter = q_cnt(&(cmsoa_state->svlte_plmn_list));

    if( q_counter > 0)
    {
      while(node_ptr != NULL)
      {
        CM_MSG_HIGH_3("SVLTE: UNBLOCK_PLMNs: PLMN:0x%x 0x%x 0x%x",
                       node_ptr->plmn.identity[0],
                       node_ptr->plmn.identity[1],
                       node_ptr->plmn.identity[2]);
        cmph_reg_send_unblock_plmn_req(node_ptr->plmn, asubs_id, SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE);
        node_ptr = q_next(&(cmsoa_state->svlte_plmn_list), &(node_ptr->link));
      }
    }

  }
  CM_MSG_HIGH_2("unblock_set_flags: wait_for_resp:%d, unblock_on_fplmn:%d",
                                 cmregprx_info_ptr->is_waiting_for_plmn_block_resp ,
  	                          cmregprx_info_ptr->is_send_unblock_on_next_flpmn);

  /* set flag to unblock plmns in next fplmn_list_ind 
  this flag has to be set only if CM is waiting for plmn blok response */ 
 if(cmregprx_info_ptr->is_waiting_for_plmn_block_resp == TRUE &&
    cmregprx_info_ptr->is_send_unblock_on_next_flpmn == FALSE)
 {
   CM_MSG_HIGH_0("set send_unblock flag to TRUE");
   cmregprx_info_ptr->is_send_unblock_on_next_flpmn = TRUE;
 }
 

}
#endif


/*===========================================================================

FUNCTION cmmsc_auto_update_op_mode

DESCRIPTION
  The function updates op mode base on various conditions.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void   cmmsc_auto_update_op_mode (
    cmmsc_state_machine_s_type  *state_machine,
    boolean                      is_new_policy_tobe_read,
    boolean                      is_oprt_sub_change,
    sd_ss_mode_pref_e_type       mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref,
    boolean                      is_gwl_sub_avail,
    boolean                      is_cdma_sub_avail,
    cm_preference_info_s_type           *pref_info
)
{
  cmmsc_state_machine_op_mode_e_type  new_op_mode
    = CMMSC_OP_MODE_AUTO_MAIN;
  sys_ue_mode_e_type old_ue_mode = state_machine->stack_common_info.ue_mode;

  /* Determine op mode per subscription, mode pref, hybr pref, etc.
  */

  /* Table entry type */
  typedef struct {

    /* In */
    boolean                          is_hybr1_allowed;
    boolean                          is_hybr2_allowed;

    /* Out */
    cmmsc_state_machine_op_mode_e_type   op_mode;
  } cmmsc_auto_op_mode_table_entry;

  int i;
  cmmsc_auto_op_mode_table_entry *tep;

  /* ue mode does not depend on oprt mode */
  static cmmsc_auto_op_mode_table_entry ue_mode_determination_table[] =
  {
    /* is_hybr1_allowed -- is_hybr2_allowed -- op_mode */
    { FALSE,   FALSE,    CMMSC_OP_MODE_AUTO_MAIN},
    { TRUE,    FALSE,    CMMSC_OP_MODE_AUTO_MAIN_HYBR1},
    { FALSE,   TRUE,     CMMSC_OP_MODE_AUTO_MAIN_HYBR2},
    { TRUE,    TRUE,     CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2}
  };

  boolean is_hybr1_allowed = FALSE;
  boolean is_hybr2_allowed = FALSE;
  #if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  #endif

  /* Get new ue mode from pm if any */
  if (is_new_policy_tobe_read)
  {
    cmmsc_auto_retrieve_pm_ue_mode(state_machine);
  }
  /* if this is sub or oprt mode change */
  else if (is_oprt_sub_change)
  {
    cmmsc_auto_retrieve_pm_ue_mode(state_machine);
  }

  if(is_new_policy_tobe_read || is_oprt_sub_change)
  {
    CM_MSG_HIGH_2("OP_MODE: is_new_policy_tobe_read %d, is_oprt_sub_change %d",
                   is_new_policy_tobe_read, 
                   is_oprt_sub_change);   
  }

  /* Determine if hybr1 and hybr2 allowed */
  cmmsc_auto_determine_is_hybr_allowed(state_machine, mode_pref, hybr_pref,
    is_cdma_sub_avail, is_gwl_sub_avail, &is_hybr1_allowed, &is_hybr2_allowed);

  /* Get op mode if there is a match */
  for(i = 0; i < (int) (ARR_SIZE(ue_mode_determination_table)); i++)
  {
    tep = &ue_mode_determination_table[i];

    /* Match against the entries */
    if( (tep->is_hybr1_allowed == is_hybr1_allowed) &&
        (tep->is_hybr2_allowed == is_hybr2_allowed)
     )
    {
      new_op_mode = tep->op_mode;
      break;
    }
  }

  /* Set ue_mode_forced flag per ue_mode or op_mode change 
  ** Only applies to srlte/srlte+g and svlte+g.
  */
  if (cmph_is_subs_feature_mode_srlte(state_machine->asubs_id) 
       ||
    (cmph_is_sxlte() &&
     cmph_is_subs_feature_mode_svlte(state_machine->asubs_id))
  )
  {
    cmmsc_auto_set_ue_mode_forced(state_machine,
                                     state_machine->op_mode,
                                     new_op_mode,
                                     old_ue_mode,
                                     state_machine->stack_common_info.ue_mode
                                    );
    if(pref_info)
    {
      cmmsc_auto_set_pref_update_reas(state_machine,
                                       state_machine->op_mode,
                                       new_op_mode,
                                       old_ue_mode,
                                       state_machine->stack_common_info.ue_mode,
                                       pref_info
                                      );
    }

  }

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  {
    cm_srlte_switch_s_type   srlte_switch_msg;
  if(cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id))
  {
    if(cmmsc_auto_is_hybr2_switched_on(state_machine->op_mode, new_op_mode) &&
      (!cmmsc_auto_is_ue_mode_csfb(state_machine)))
    {
      cmmsc_auto_unblock_1x_sxlte_plmn(state_machine->asubs_id);
        /*indicate SRLTE switch happen through msgr*/
        memset(&srlte_switch_msg, 0, sizeof(cm_srlte_switch_s_type));
        srlte_switch_msg.msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(state_machine->asubs_id);

        (void) cm_msgr_send( MM_CM_SRLTE_SWITCH_IND, MSGR_MM_CM,
                     (msgr_hdr_s*)&srlte_switch_msg, sizeof(cm_srlte_switch_s_type) );

      }
    }
  }
  #endif
  /** When switching stacks from single to dual stack, update ss for default LTE data calls
  ** from MAIN to HYBR2 and vice -versa.
  */
  if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id) &&
     state_machine->op_mode != new_op_mode)
  {
    cmmsc_auto_update_lte_data_ss_srlte(state_machine->op_mode, new_op_mode);
  }

  #ifdef FEATURE_CM_LTE
  if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id) &&
     cmmsc_auto_is_hybr2_switched_on(state_machine->op_mode, new_op_mode))
  {
    cmsds_ptr()->curr_voice_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
    cmsds_ptr()->curr_sms_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
    CM_MSG_HIGH_0("Reset voice and SMS domains during the csfb to SR switch ");
  }
  #endif

  #if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
  if(pref_info != NULL
    && cmsoa_state->is_pwrup_from_last_submode
    && cmph_is_subs_feature_mode_srlte(state_machine->asubs_id)
    && pref_info->pref_reas == SD_SS_PREF_REAS_USER
    && ((is_new_policy_tobe_read && 
        (cmmsc_auto_is_hybr2_switched_on(state_machine->op_mode, new_op_mode) ||
        cmmsc_auto_is_hybr2_switched_off(state_machine->op_mode, new_op_mode)))
      || (pref_info->pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE
        || pref_info->pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_CSFB
        || pref_info->pref_update_reason == SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2
        || pref_info->pref_update_reason == SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2_SILENT))
    )
  {
    CM_MSG_HIGH_2 ("CM->EFS: write current sub mode: %d, pref_update_reas: %d",
                    cmsoa_state->config.curr_sv_oprt_mode,
                    pref_info->pref_update_reason);
    cmsoa_write_soa_config_efs(&cmsoa_state->config);
  }
  #endif

  /* Update op_mode */
  cmmsc_state_machine_op_mode_update(state_machine, new_op_mode);

  if(state_machine->asubs_id <= SYS_MODEM_AS_ID_NONE 
    || state_machine->asubs_id >= ARR_SIZE(cmph_ptr()->rat_disabled_mask))
  {
    CM_MSG_HIGH_1("RAT_DISABLED_MASK: incorrect as_id %d", state_machine->asubs_id);
    return;
  }

  /* For single sim svlte, mode pref is hdr only, new op mode requires multi stack
  ** and rat disabled mask is empty. this should not happen.
  */
  if(cmph_is_subs_feature_mode_svlte(state_machine->asubs_id) && !cmph_is_msim()
    && CMPH_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr()) == CM_MODE_PREF_HDR_ONLY
    && cmmsc_is_msc_multi_stacks(state_machine->asubs_id)
    && cmph_ptr()->rat_disabled_mask[state_machine->asubs_id] == 0)
  {
    CM_ERR_FATAL_1("mode pref hdr only, new op mode %d should not be allowed",new_op_mode);
  }

}

/*===========================================================================

FUNCTION cmmsc_auto_set_ue_mode_forced

DESCRIPTION
  The function processes op mode and ue_mode change, and sets ue_mode_forced flag.
  CMMSC will check the flag before sending command to mmoc to decide if additional action
  ue mode change needs to be set.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void   cmmsc_auto_set_ue_mode_forced (
    cmmsc_state_machine_s_type         *state_machine,
    cmmsc_state_machine_op_mode_e_type  old_op_mode,
    cmmsc_state_machine_op_mode_e_type  new_op_mode,
    sys_ue_mode_e_type                  old_ue_mode,
    sys_ue_mode_e_type                  new_ue_mode
)
{
  /* SRLTE handling */
  if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id))
  {
    /* CSFB -> SRLTE */
    /* SRLTE -> CSFB */
    if(cmmsc_auto_is_hybr2_switched_on(old_op_mode, new_op_mode) ||
       cmmsc_auto_is_hybr2_switched_off(old_op_mode, new_op_mode))    
    {
      state_machine->stack_common_info.is_force_reg_pending = TRUE;
      state_machine->stack_common_info.ue_mode_forced = FALSE;
    }
    CM_MSG_HIGH_2("srlte: is_ue_moved_to_srlte %d ue_mode_forced %d",
                    state_machine->stack_common_info.is_force_reg_pending,
                    state_machine->stack_common_info.ue_mode_forced);
  }
  
  /* SVLTE+G handling */
  else if (cmph_is_sxlte() &&
         cmph_is_subs_feature_mode_svlte(state_machine->asubs_id) &&
         old_ue_mode != new_ue_mode
  )
  {
    state_machine->stack_common_info.ue_mode_forced = FALSE;
  }
}

/*===========================================================================

FUNCTION  cmmsc_auto_is_valid_stack_idx

DESCRIPTION
 Return if the stack_idx is valid per the state machine.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_valid_stack_idx (
  cmmsc_state_machine_s_type  *state_machine,
  int                          stack_idx
)
{
  if (!state_machine)
  {
    CM_MSG_HIGH_0("NULL_PTR: state_machine");
    return FALSE;
  }

  if (cmph_is_ssim())
  {
    if (cmph_is_subs_feature_mode_normal(state_machine->asubs_id) &&
        !(stack_idx == 0 || stack_idx == 1))
    {
      return FALSE;
    }
    return TRUE;
  }
  
  if (cmph_is_subs_feature_mode_normal(state_machine->asubs_id) &&
     ((state_machine->asubs_id != SYS_MODEM_AS_ID_1 && stack_idx != 0) 
       ||
      (stack_idx != 0 && stack_idx != 1) ))
  {   
    CM_MSG_HIGH_2("MSC: asubs_id %d stack_idx %d is not expected!!!", 
                     state_machine->asubs_id, stack_idx);
    return FALSE;
  }
  else
  {
    return TRUE;
  }  
}

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_gwl_allowed

DESCRIPTION
  This function returns whether hybrid HDR is allowed or not based on the
  hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_is_hybr_gwl_allowed
(
   cmmsc_state_machine_s_type     *state_machine,
   /* State machine ptr.
   */
   
   sd_ss_hybr_pref_e_type          hybr_pref,
   /* Hybrid preference to use.
   */

   boolean                         is_cdma_subsc_avail
   /* The  current value of CDMA subscription
   */

)
{
  /* If ue mode is csfb, not allow hybr2 */
  boolean is_ue_mode_csfb = cmmsc_auto_is_ue_mode_csfb(state_machine);
  sys_ue_mode_e_type ue_mode = state_machine->stack_common_info.ue_mode;
  sys_modem_as_id_e_type asubs_id = state_machine->asubs_id;

  /* Indicate if sub feature mode is not svlte, not allow hybr2 */
  boolean is_sub_svlte = cmph_is_subs_feature_mode_1x_sxlte(asubs_id);

  /* Indicate if PM UE_MODE is normal for multi-sim config.  If yes, not activate hybr2 stack */
  boolean is_msim_normal_mode = FALSE;
  boolean is_cdma_valid_mode_pref 
    = cmph_is_valid_mode_pref(CM_MODE_PREF_CDMA_ONLY, asubs_id);

  boolean is_lte_valid_mode_pref 
    = cmph_is_valid_mode_pref(CM_MODE_PREF_LTE_ONLY, asubs_id);

  cm_mode_pref_e_type ph_mode_pref = CMPH_SS_MODE_PREF(asubs_id, cmph_ptr());

  sd_ss_mode_pref_e_type mode_cap 
    = cmph_map_sys_mode_mask_to_sd_mode_pref(
      state_machine->stack_common_info.ue_mode_capability);

  if (cmph_is_msim() && ue_mode == SYS_UE_MODE_NORMAL)
  {
    is_msim_normal_mode = TRUE;
  }

  if (!is_sub_svlte)
  {
    return FALSE;
  }

  CM_MSG_HIGH_5("IS_HYBR2_ALLOWED: sfmode1=%d, ue_mode=%d, hybr_pref %d, 1x_subsc %d, oprt_mode=%d",
                 cmph_get_subs_feature_mode(asubs_id),
                 ue_mode,
                 hybr_pref,is_cdma_subsc_avail,cmph_ptr()->oprt_mode);


  switch(hybr_pref)
  {
    case SD_SS_HYBR_PREF_CDMA__LTE__HDR:
      if(cmcall_misc_is_mode_pref (CM_MODE_PREF_CDMA_ONLY, ph_mode_pref) &&
         cmcall_misc_is_mode_pref (CM_MODE_PREF_LTE_ONLY, ph_mode_pref) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_CDMA) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_LTE) &&
         is_cdma_valid_mode_pref && is_lte_valid_mode_pref &&
         is_cdma_subsc_avail &&
         !is_ue_mode_csfb &&
         !is_msim_normal_mode)
      {
        return TRUE;
      }

      break;

    case SD_SS_HYBR_PREF_CDMA__GWL__HDR:
      if(cmcall_misc_is_mode_pref (CM_MODE_PREF_CDMA_ONLY, ph_mode_pref) &&
         cmcall_misc_is_mode_pref (CM_MODE_PREF_LTE_ONLY, ph_mode_pref) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_CDMA) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_LTE) &&
         is_cdma_valid_mode_pref &&
         cmph_is_valid_mode_pref(CM_MODE_PREF_GWL, asubs_id)&&
         is_cdma_subsc_avail &&
         !is_ue_mode_csfb &&
         !is_msim_normal_mode)
      {
        return TRUE;
      }
      break;

    case SD_SS_HYBR_PREF_CDMA_HDR:
      if(cmcall_misc_is_mode_pref (CM_MODE_PREF_CDMA_ONLY, ph_mode_pref) &&
         cmcall_misc_is_mode_pref (CM_MODE_PREF_LTE_ONLY, ph_mode_pref) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_CDMA) &&
         SD_MODE_CONTAIN(mode_cap, SD_SS_MODE_PREF_LTE) &&
         is_cdma_valid_mode_pref &&
         cmph_is_valid_mode_pref(CM_MODE_PREF_GWL, asubs_id)&&
         is_cdma_subsc_avail)
      {
        /* For fmode=SRLTE & ue-mode=SRLTE_ONLY,
           ignore hybr_pref and enable HYBR2 stack */
        if(cmph_is_subs_feature_mode_srlte(asubs_id) &&
           cmmsc_get_curr_msc_ue_mode(state_machine) == SYS_UE_MODE_1XSRLTE_ONLY)
        {
          return TRUE;
        }
      }
      break;

    default:
      break;
  }

  /* Hybrid is not allowed.
  */
  return FALSE;

} 

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_allowed

DESCRIPTION
  This function returns whether hybrid HDR is allowed or not based on the
  mode_pref and hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                         cmmsc_auto_is_hybr_allowed
(

       sd_ss_mode_pref_e_type          mode_pref,
          /* Mode preference to use.
          */

       sd_ss_hybr_pref_e_type          hybr_pref,
          /* Hybrid preference to use.
          */

       sys_modem_as_id_e_type          asubs_id
)
/*lint -esym(715,mode_pref,hybr_pref)*/
{
  #if  ( defined(FEATURE_HDR_HYBRID))



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if Hybrid is allowed.
  */

  /* Derive the mode_pref in use.
  */
  //mode_pref = SD_GET_COMMON_MODE( mode_pref, cmmsc_get_cm_ss_mode_cap(CM_SS_MAIN));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( mode_pref == SD_SS_MODE_PREF_NONE )
  {
    /* Hybrid is not allowed.
    */
    CM_MSG_HIGH_0("mode_pref is NONE return FALSE");
    return FALSE;
  }

  
  /* Check if the mode is HDR only mode.
  */
  if( mode_pref == SD_SS_MODE_PREF_HDR )
  {
    /* Hybrid is not allowed.
    */
    CM_MSG_HIGH_0("mode_pref is HFR Only return FALSE");
    return FALSE;
  }

  /* Check if hybrid is allowed.
  */
  if( hybr_pref != SD_SS_HYBR_PREF_NONE)
  {
    /* Hybrid is allowed.
    */
    return TRUE;
  }
  #endif


  /* Hybrid is not allowed.
  */
  return FALSE;

} 

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_hdr_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid HDR call is allowed in the specified asubs_id.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_is_hybr_hdr_call_allowed (
  sys_modem_as_id_e_type  asubs_id
)
{
  /* cmmsc_stack_state_e_type main_state; */
  cmmsc_stack_state_e_type hybr1_state = CMMSC_STACK_STATE_NULL;

  cmmsc_state_machine_s_type *state_machine = NULL;

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(asubs_id))
  {
    return FALSE;
  }   
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_machine = cmmsimc_state_machine_ptr(asubs_id);

  /* Check if we are in avoid hybrid HDR period
  */
  if (cmmsc_auto_get_avoid_hybr_hdr_uptime(state_machine) != CMMSC_UPTIME_NONE )
  {
    /* we are in avoid hybrid HDR period.
    */
    return FALSE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR is masked out.
  */
  if ( !sd_misc_is_mode_pref( cmmsc_get_cm_ss_mode_cap(CM_SS_HYBR_1),
                              SD_SS_MODE_PREF_HDR))
  {
    /* HDR is masked out, so hybrid HDR call is not allowed.
    */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have hybrid HDR service, originate with HDR. It is necessary to check
  ** hdr_hybrid flag because tech specific srv indications may be enabled.
  */
  if ( cmss_ptr()->info.hdr_hybrid &&
       sys_srv_status_is_srv( cmss_ptr()->info.hdr_srv_status ) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If OPT HO is on, allow Hybrid HDR call.
  */
  if ( cmss_ptr()->info.hdr_hybrid && sd_misc_is_opt_ho_lte_do())
  {
    CM_MSG_HIGH_0("OPT HO: allow hybr hdr data call");
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*main_state = cmmsimc_get_msc_stack_state(CM_SS_MAIN); */
  hybr1_state = cmmsimc_get_msc_stack_state(CM_SS_HYBR_1);

  if(hybr1_state == CMMSC_STACK_STATE_SRV)
  {
    return TRUE;
  }
  else if(hybr1_state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV ||
          !cmss_ptr()->info.hdr_hybrid)
  {
    return FALSE;
  }
  else if(sys_srv_status_is_srv(cmss_ptr()->main_srv_status) &&
          !cmss_ptr()->is_colloc )
  {
    return FALSE;
  }
  
  return TRUE;

} 


/*===========================================================================

FUNCTION cmmsc_auto_set_hybr1_pref

DESCRIPTION
  The function sets the hybr1 specific pref to be forced to SD.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_set_hybr1_pref (
  cmmsc_state_machine_s_type           *state_machine,
  cmmsc_state_machine_op_mode_e_type   old_op_mode,
  cm_preference_info_s_type            *pref
)
{
  cmmsc_state_machine_op_mode_e_type   new_op_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pref == NULL || state_machine == NULL)
  {
    CM_ERR_0("NULL ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_op_mode = state_machine->op_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the default preferences.
  */
  pref->hybr1_pref.hybr_mode_pref = pref->mode_pref;
  pref->hybr1_pref.hybr_orig_mode = SD_SS_ORIG_MODE_COLLOC;

  /* Update orig_mode if emerg(/callback)
  */
  if(pref->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ||
    pref->orig_mode == SD_SS_ORIG_MODE_EMERG_CB
  )
  {
    pref->hybr1_pref.hybr_orig_mode = pref->orig_mode;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is a hybr1 state toggle,
  ** 1. Update hybr1 specific pref base on op mode change,
  ** 2. Update timer if needed.
  */
  if (cmmsc_auto_is_hybr1_switched_off(old_op_mode, new_op_mode))
  {
    pref->hybr_pref = CM_HYBR_PREF_OFF;

    /* If already has 1x srv, force mode pref none to hybr1 stack */
    if (state_machine->stack[0].state == CMMSC_STACK_STATE_SRV)
    {
      pref->hybr1_pref.hybr_mode_pref = CM_MODE_PREF_NONE;
      pref->hybr1_pref.hybr_orig_mode = SD_SS_ORIG_MODE_NONE;
    }

    if (cmmsc_is_in_srv_state(state_machine, 0))
    {
      pref->hybr1_pref.hybr_orig_mode = SD_SS_ORIG_MODE_NONE;
      cmmsc_auto_set_acq_hybr_1_uptime(state_machine, CMMSC_UPTIME_NONE);
    }
    else
    {
      cmmsc_auto_set_lost_main_uptime(state_machine, CMMSC_UPTIME_NONE);
    }
  }
  else if (cmmsc_auto_is_hybr1_switched_on(old_op_mode, new_op_mode))
  {
    pref->hybr1_pref.hybr_orig_mode = SD_SS_ORIG_MODE_COLLOC;

    /* If 1x has service, start acq_hybr_1_uptime */
    if (cmmsc_is_in_srv_state(state_machine, 0))
    {
      cmmsc_auto_set_acq_hybr_1_uptime(state_machine,
                            time_get_uptime_secs() + CMMSC_ACQ_HDR_TIMER);
    }
    else
    {
      cmmsc_auto_set_acq_hybr_1_uptime(state_machine, CMMSC_UPTIME_NONE);
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_determine_is_hybr_allowed

DESCRIPTION
  The function determines if hybr1/hybr2 allowed base on mode pref, capability,
  hybr pref, subscription.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void   cmmsc_auto_determine_is_hybr_allowed (
    cmmsc_state_machine_s_type  *state_machine,
    sd_ss_mode_pref_e_type       mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref,
    boolean                      is_cdma_sub_avail,
    boolean                      is_gwl_sub_avail,
    /* in */

    boolean                     *is_hybr1_allowed,
    boolean                     *is_hybr2_allowed
    /* out */
)
{
  if (!state_machine)
  {
    CM_ERR_0("NULL_PTR: state_machine");
    return;
  }

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(state_machine->asubs_id))
  {
    *is_hybr1_allowed = FALSE;
    *is_hybr2_allowed = FALSE;
  } 
  else
  {
    sys_sys_mode_mask_e_type  pm_mode       = SYS_SYS_MODE_MASK_NONE;
    sd_ss_mode_pref_e_type    mode              = SD_SS_MODE_PREF_NONE;
    sd_ss_band_pref_e_type    band                = SD_SS_BAND_PREF_NONE;
    sd_ss_band_pref_e_type    tds_band          = SD_SS_BAND_PREF_NONE;
    sys_lte_band_mask_e_type  lte_band      = SYS_LTE_BAND_MASK_CONST_NONE;
    sd_ss_mode_pref_e_type    common_mode        = SD_SS_MODE_PREF_NONE;
    #ifndef CM_DEBUG
    sys_lte_band_mask_e_type  lte_band_local= CMPH_SS_LTE_BAND_PREF(state_machine->asubs_id, cmph_ptr());
    #endif
    /* Get PM capability */
    cmmsc_get_policy_mode_band_cap(state_machine,
                                   &pm_mode,
                                   &band,
                                   &tds_band,
                                   &lte_band);
  
    mode = cmph_map_sys_mode_mask_to_sd_mode_pref(pm_mode);
  
    /* Get the intersection of mode pref and capability */
    common_mode = sd_misc_get_common_mode_pref(mode_pref, mode);
    
    CM_MSG_HIGH_3("OP_MODE: mode_pref 0x%x & PM mode_cap 0x%x = 0x%x", 
                     mode_pref, mode, common_mode);
  
    /* Determine is hybr allowed base on mode pref, sub, hybr pref */
    *is_hybr2_allowed =
      cmmsc_auto_is_hybr_gwl_allowed(state_machine,
                                       hybr_pref,
                                       is_cdma_sub_avail);
    
    *is_hybr1_allowed =
      cmmsc_auto_is_hybr_allowed(common_mode,
                                   hybr_pref,
                                   state_machine->asubs_id);

    /* Error fatal in CM to capture the error state
    ** LTE layer will crash if ue_mode is SVLTE but 1x is in the RAT priority list
    */
    #ifndef CM_DEBUG
    if (state_machine->stack_common_info.ue_mode == SYS_UE_MODE_SVLTE &&
       hybr_pref == SD_SS_HYBR_PREF_CDMA__LTE__HDR &&
       is_cdma_sub_avail && 
       cmcall_misc_is_mode_pref(CM_MODE_PREF_CDMA_ONLY,
        CMPH_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr())) &&
       cmph_is_valid_mode_pref(CM_MODE_PREF_CDMA_ONLY, state_machine->asubs_id) &&
       !SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_local) &&
       cmph_is_valid_mode_pref(CM_MODE_PREF_LTE_ONLY, state_machine->asubs_id) &&
       is_gwl_sub_avail &&
       *is_hybr2_allowed == FALSE
    )
    {
      if(!cmcall_misc_is_mode_pref(CM_MODE_PREF_LTE_ONLY,
        CMPH_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr())))
      {
        CM_ERR_FATAL_1("OP_MODE: UE_MODE is SVLTE but LTE not in mode_pref %d",
          CMPH_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr()));
      }
      else
      {
        CM_ERR_FATAL_0("OP_MODE: UE_MODE is SVLTE but hybr2 is not allowed");
      }
    }
    #endif
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_online_disable_hybr1

DESCRIPTION
  The function returns whether or not to disable hybr1 depending on oprt mode,
  mode pref, and hybr pref.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is to be disabled.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
 boolean    cmmsc_auto_online_disable_hybr1 (
    sys_oprt_mode_e_type         oprt_mode,
    cm_mode_pref_e_type          mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref
)
{
  boolean disable_hybr1 = FALSE;
  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(CM_SS_MAIN);
  boolean is_hybr1_allowed =
    cmmsc_auto_is_hybr_allowed(
               cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),
               hybr_pref,
               asubs_id
               );

  /* If hybr1 is allowed but op mode calculated does not allow it, then CM needs
  ** to disable hybr1 by force preferences specifically.
  ** Reason: hybr1 preference has been forced before online.
  */
  if (is_hybr1_allowed &&
      oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE)
  {
    disable_hybr1 = TRUE;
  }

  return disable_hybr1;
}


/*===========================================================================

FUNCTION cmmsc_auto_is_op_mode_hybr1_enabled

DESCRIPTION
  The function returns if hybr1 is enabled for the specified op mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is enabled for the specified op mode.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_op_mode_hybr1_enabled (
    cmmsc_state_machine_op_mode_e_type  op_mode
)
{
  switch(op_mode)
  {
    case CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2:
    case CMMSC_OP_MODE_AUTO_MAIN_HYBR1:
      return TRUE;

    default:
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_update_is_hybr1_disabled

DESCRIPTION
  The function updates is_hybr1_diabled in MSC state machine.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_hybr1_disabled (
  cmmsc_state_machine_s_type  *state_machine,
  boolean                     is_hybr1_diabled
)
{
  state_machine->stack_common_info.is_hybr1_diabled = is_hybr1_diabled;
}

/*===========================================================================

FUNCTION cmmsc_auto_get_is_hybr1_disabled

DESCRIPTION
  The function returns is_hybr1_diabled in MSC state machine.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is disabled in state machine.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_get_is_hybr1_disabled (
  cmmsc_state_machine_s_type  *state_machine
)
{
  return (state_machine->stack_common_info.is_hybr1_diabled);
}

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr1_switched_off

DESCRIPTION
  The function returns if hybr1 stack is switched off.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched off.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_hybr1_switched_off (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
)
{
  if ( (old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1 ||
      old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
      &&
     (new_op_mode == CMMSC_OP_MODE_AUTO_MAIN ||
      new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR2)
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr1_switched_on

DESCRIPTION
  The function returns if hybr1 stack is switched on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched on.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_hybr1_switched_on (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
)
{
  if ( (new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1 ||
      new_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
      &&
     (old_op_mode == CMMSC_OP_MODE_AUTO_MAIN ||
      old_op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR2)
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_hybr2_pending

DESCRIPTION
  The function updates is_kick_hybr2_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_hybr2_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_pending
)
{
  state_machine->stack_common_info.stack_kick.is_hybr_2_kick_pending = is_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_hdr_pending

DESCRIPTION
  The function updates is_kick_hdr_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_hdr_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_kick_hdr_pending
)
{
  state_machine->stack_common_info.stack_kick.is_kick_hdr_pending = is_kick_hdr_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_update_is_stack_sync_bsr_pending

DESCRIPTION
  The function updates is_stack_sync_bsr_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_stack_sync_bsr_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_stack_sync_bsr_pending
)
{
  state_machine->stack_common_info.stack_kick.is_stack_sync_bsr_pending 
    = is_stack_sync_bsr_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_gw

DESCRIPTION
  The function updates is_kick_gw.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_gw (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_kick_gw
)
{
  state_machine->stack_common_info.stack_kick.is_kick_gw = is_kick_gw;
}

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_hdr_pending

DESCRIPTION
  The function returns is_kick_hdr_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_hdr_pending (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.stack_kick.is_kick_hdr_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_hybr2_pending

DESCRIPTION
  The function returns is_hybr_2_kick_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_hybr2_pending (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.stack_kick.is_hybr_2_kick_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_is_is_stack_sync_bsr_pending

DESCRIPTION
  The function returns is_stack_sync_bsr_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_stack_sync_bsr_pending (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.stack_kick.is_stack_sync_bsr_pending;
}

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_gw

DESCRIPTION
  The function returns is_kick_gw.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_gw (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.stack_kick.is_kick_gw;
}

/*===========================================================================

FUNCTION cmmsc_auto_get_lost_main_uptime

DESCRIPTION
  This function gets the current acq_hybr_1_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_acq_hybr_1_uptime (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.timer.acq_hybr_1_uptime;
}

/*===========================================================================

FUNCTION cmmsc_auto_get_lost_main_uptime

DESCRIPTION
  This function gets the current lost_main_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_lost_main_uptime (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.timer.lost_main_uptime;
}

/*===========================================================================

FUNCTION cmmsc_auto_get_avoid_hybr_hdr_uptime

DESCRIPTION
  This function gets the current avoid_hybr_hdr_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_avoid_hybr_hdr_uptime (
  cmmsc_state_machine_s_type     *state_machine
)
{
  return state_machine->stack_common_info.timer.avoid_hybr_hdr_uptime;
}

/*===========================================================================

FUNCTION cmmsc_auto_set_acq_hybr_1_uptime

DESCRIPTION
  This function sets the current acq_hybr_1_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_acq_hybr_1_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
)
{
  state_machine->stack_common_info.timer.acq_hybr_1_uptime = uptime;
  if(CMMSC_UPTIME_NONE != uptime)
  {
    CM_MSG_HIGH_1("new timer set for acq_hybr_1_uptime: %d", uptime);
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_set_lost_main_uptime

DESCRIPTION
  This function sets the current lost_main_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_lost_main_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
)
{
  if(uptime != state_machine->stack_common_info.timer.lost_main_uptime)
  {
    state_machine->stack_common_info.timer.lost_main_uptime = uptime;
    if(CMMSC_UPTIME_NONE != uptime)
    {
      CM_MSG_HIGH_1("new timer set for lost_main_uptime: %d", uptime);
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_set_avoid_hybr_hdr_uptime

DESCRIPTION
  This function sets the current avoid_hybr_hdr_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_avoid_hybr_hdr_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
)
{
  state_machine->stack_common_info.timer.avoid_hybr_hdr_uptime = uptime;
  CM_MSG_HIGH_1("new timer set for avoid_hybr_hdr_uptime", uptime);
}

/*===========================================================================

FUNCTION cmmsc_is_hicps_hybr1_kick_allowed

DESCRIPTION
  This function returns whether it is allowed to kick hybr1 activity.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmmsc_is_hicps_hybr1_kick_allowed(

  cmmsc_state_machine_s_type *msc

)
{
  cm_orig_q_s_type *hybr1_top = cmtask_orig_para_get_top(CM_SS_HYBR_1);

  if(hybr1_top == NULL)
  {
    CM_ERR_0("Top ptr is NULL for HYBR1 stack");
    return FALSE;
  }

  if((hybr1_top->act_type != CM_ACT_TYPE_PH_OBJ &&
	  hybr1_top->act_type != CM_ACT_TYPE_POLICYMAN_RESTRICT)
      #ifdef CM_GW_SUPPORTED
      ||(cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state == CMREGPRX_STATE_ACTIVE) || 
      (cmph_is_subs_feature_mode_1x_sxlte(msc->asubs_id) && 
       cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->state == CMREGPRX_STATE_ACTIVE) 
      #endif
     )
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

===========================================================================*/
boolean cmmsc_is_3gpp_forced_on_main(

  cmmsc_state_machine_s_type *msc

)
{
  cm_orig_q_s_type *main_top = cmtask_orig_para_search_act_type(
                                            CM_SS_MAIN, CM_ACT_TYPE_PH_OBJ);

  if(main_top == NULL)
  {
    CM_ERR_0("Top_ptr is NULL for MAIN stack");
    return FALSE;
  }

  return cmcall_misc_is_mode_pref(main_top->orig->orig_mode_pref,
                                  CM_MODE_PREF_GWL);
}

/*===========================================================================

===========================================================================*/
boolean cmmsc_is_3gpp_in_user_mode_pref(

  cmmsc_state_machine_s_type *msc

)
{
  return cmcall_misc_is_mode_pref( cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                   CM_MODE_PREF_GWL );
}

/*===========================================================================

===========================================================================*/
boolean cmmsc_is_srv_on_any_stack(

  cmmsc_state_machine_s_type *msc
)
{
  boolean any_stack_srv = FALSE;

  if(msc->stack[0].state == CMMSC_STACK_STATE_SRV)
  {
    any_stack_srv = TRUE;
  }

  if(msc->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1 &&
     msc->stack[1].state == CMMSC_STACK_STATE_SRV)
  {
    any_stack_srv = TRUE;
  }

  if(msc->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2 &&
     msc->stack[2].state == CMMSC_STACK_STATE_SRV)
  {
    any_stack_srv = TRUE;
  }

  return any_stack_srv;
}
/*===========================================================================

===========================================================================*/
void cmmsc_action_hicps_remove_3gpp(

    cmmsc_state_machine_s_type *msc,

    int stack_idx,

    cmmsc_event_acq_fail_s_type *acq_fail

)
{
  cm_orig_q_s_type *hybr1_top = cmtask_orig_para_get_top(CM_SS_HYBR_1);
  boolean is_hdr_present = FALSE, is_3gpp_ph_obj = TRUE;
  cmph_s_type*      ph_ptr = cmph_ptr();
  
  if(hybr1_top == NULL)
  {
    CM_ERR_0("Top ptr is NULL for HYBR1 stack");
    return;
  }

  is_hdr_present = cmcall_misc_is_mode_pref(
                                          hybr1_top->orig->orig_mode_pref,
                                          CM_MODE_PREF_HDR_ONLY);

  /* This check is to avoid hicups due to race conditions in subscription handling between CM & SD */					    
  if(ph_ptr->is_cdma_subscription_available &&
    ph_ptr->is_gwl_subscription_available)	
  {    
    is_3gpp_ph_obj = sd_misc_is_mode_pref( acq_fail->acq_fail_mode_pref,
                                          SD_SS_MODE_PREF_GWL);
  }
  
  if(is_hdr_present && is_3gpp_ph_obj)
  {
    cmmsc_auto_update_gwl_operation(FALSE, 
                                      SD_SS_PREF_REAS_STACK_SYNC_UP,
                                      CM_MODE_PREF_GWL, 
                                      FALSE, 
                                      msc->asubs_id);
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_hicps_cond_kick(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cm_orig_q_s_type *hybr1_top = cmtask_orig_para_get_top(CM_SS_HYBR_1);
  boolean is_hdr_present = FALSE;

  if(hybr1_top == NULL)
  {
    CM_ERR_0("Top ptr is NULL for HYBR1 stack");
    return;
  }
  
  is_hdr_present = cmcall_misc_is_mode_pref(
                                          hybr1_top->orig->orig_mode_pref,
                                          CM_MODE_PREF_HDR_ONLY);
  if(!is_hdr_present)
  {
    return;
  }

  if (!cmph_is_subs_feature_mode_1x_sxlte(msc->asubs_id)   ||
      (cmph_is_subs_feature_mode_1x_sxlte(msc->asubs_id) &&
      (cmmsc_auto_is_kick_hdr_pending(msc) ||
      #ifdef FEATURE_HDR_HYBRID
      ((cmss_ptr()->hdr_no_srv_uptime != CMSS_NO_SRV_UPTIME_NONE )&&
       (sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_HYBR_1)->srv_status) == FALSE ))||
      #endif
       (sys_srv_status_is_srv(cmss_ptr()->hybr_1_srv_status) == FALSE )))
  )
  {   
    CM_MSG_MED_1("execute action hybr1_kick_allowed %d",
                cmmsc_is_hicps_hybr1_kick_allowed(msc));

    if(!cmmsc_is_hicps_hybr1_kick_allowed(msc))
    {
      *cmph_hdr_kick_pending() = TRUE;
    }
    else
    {
      *cmph_hdr_kick_pending() = TRUE;
      cmmsc_action_pending_kick(msc, stack_idx, TRUE);
      *cmph_get_is_kicked_hybr_ptr() = TRUE;
    }
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_pending_kick(
    cmmsc_state_machine_s_type *msc,
    int                        stack_idx,
    boolean                    force_top
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc );
  int main_stk_id = cmmsc_mapping_ss_to_stack_idx(CM_SS_MAIN);

  cmmsc_event_force_pref_s_type pref_package;
  cm_orig_q_s_type *top = cmtask_orig_para_get_top( CM_SS_MAIN);

  switch(ss)
  {
    case CM_SS_HYBR_1:
    {
      CM_MSG_MED_1("execute action is kick pending %d",
                  *cmph_hdr_kick_pending());
      
      if(*cmph_hdr_kick_pending())
      {
        if(cmmsc_auto_is_kick_hdr_pending(msc) &&
           cmmsc_is_in_srv_state(msc, main_stk_id))
        {
          pref_package.pref_reas = SD_SS_PREF_REAS_USER;
        }
        else
        {
          pref_package.pref_reas = SD_SS_PREF_REAS_STACK_SYNC_UP;
        }

        pref_package.pref_reas = cmcall_is_hybr_hdr_orig()?
                                               SD_SS_PREF_REAS_ORIG_START_PS:
                                               pref_package.pref_reas;

        pref_package.orig_mode = SD_SS_ORIG_MODE_COLLOC;
        pref_package.hybr_pref = cmph_map_sd_hybr_pref_to_cm_hybr_pref(
                                   cmph_ptr()->int_hybr_pref);

        cmmsc_action_force_ph_ext(msc, &pref_package, stack_idx);

        *cmph_hdr_kick_pending() = FALSE;
      }
      break;
    }

    case CM_SS_HYBR_2:
    {
      if(cmph_ptr()->ph_state == CM_PH_STATE_EMERG_CB ||
         cmcall_emergency_call_active())
      {
        break;
      }

      /* If hybr2 kick isn't pending or force top not reqd then skip */
      if(!cmmsc_auto_is_kick_hybr2_pending(msc) && !force_top)
      {
        break;
      }

      if(top == NULL)
      {
        CM_ERR_0("Top ptr is NULL for MAIN stack");
        return;
      }

      if(*cmph_get_is_kicked_hybr_2_ptr() == FALSE )
      {
        pref_package.pref_reas= SD_SS_PREF_REAS_USER;
      }
      else
      {
        pref_package.pref_reas = SD_SS_PREF_REAS_STACK_SYNC_UP;
      }

      pref_package.orig_mode = SD_SS_ORIG_MODE_NONE;
      pref_package.hybr_pref = top->orig->orig_hybr_pref;

      cmmsc_action_force_top_ext(msc, &pref_package, stack_idx);

      cmmsc_auto_update_is_kick_hybr2_pending(msc, FALSE);
      *cmph_get_is_kicked_hybr_2_ptr() = TRUE;

      break;
    }

   default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_shorten_bsr(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);

  switch(ss)
  {
    case CM_SS_MAIN:
    {
      cmmsc_auto_update_gwl_operation(TRUE, SD_SS_PREF_REAS_STACK_SYNC_UP,
                                        CM_MODE_PREF_GWL, 
                                        TRUE, 
                                        msc->asubs_id);
      break;
    }

    case CM_SS_HYBR_2:
    {
      CM_MSG_MED_1("execute action is is_shorten_bsr %d",
                  cmph_ptr()->is_shorten_bsr);
      
      if(cmph_ptr()->is_shorten_bsr)
      {
        /* called in place of cmpref2_send_pref_sys_chgd_cmd */
        cmmsc_action_force_top(msc, stack_idx, SD_SS_PREF_REAS_USER);

        cmph_ptr()->is_shorten_bsr = FALSE;
      }
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_restore_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cm_mode_pref_e_type mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  cmph_s_type*      ph_ptr = cmph_ptr();

  /* if any network scan is ongoing restoring 3gpp pref may cause a new service req
        or stop_mode_req which can abort ongoing network scan.
        Considering user triggered network scan being higher priority skip restoring 3gpp pref.
        Can be restored with next HDR_ACQ_FAIL */
  if( cmph_is_get_networks_ongoing(msc->asubs_id) == TRUE &&
  	  ph_ptr->ss_for_get_networks == CM_SS_MAIN )
  {
    CM_MSG_HIGH_1("get_networks ongoing on asubs_id=%d. skip restore 3gpp", msc->asubs_id);
    return;
  }

  if(msc->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1)
  {
    mode_pref = CM_MODE_PREF_GWL;
  }

  switch(ss)
  {
    case CM_SS_MAIN:
    {
      cmmsc_auto_update_gwl_operation (TRUE, SD_SS_PREF_REAS_STACK_SYNC_UP, 
                                         mode_pref, 
                                         FALSE, 
                                         msc->asubs_id);
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_svlte_remove_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);

  cm_orig_q_s_type *top_ph = cmtask_orig_para_search_act_type( ss,
                                                          CM_ACT_TYPE_PH_OBJ);
  boolean is_3gpp_ph_obj = FALSE ;
  
  if(top_ph != NULL)
  {
    is_3gpp_ph_obj = cmcall_misc_is_mode_pref( top_ph->orig->orig_mode_pref,
                                          CM_MODE_PREF_GSM_WCDMA_ONLY );
  }
  
  switch(ss)
  {
    case CM_SS_MAIN:
    {
      if(is_3gpp_ph_obj)
      {
        (void)cmmsc_auto_update_gwl_operation(FALSE, 
                                               SD_SS_PREF_REAS_STACK_SYNC_UP,
                                               CM_MODE_PREF_GSM_WCDMA_ONLY,
                                               FALSE, 
                                               msc->asubs_id);
        if(!cmmsc_is_srv_on_any_stack(msc))
        {
          cmmsc_auto_update_is_kick_gw(msc, FALSE);
        }
      }
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

FUNCTION cmmsc_action_svlte_stop_stack

DESCRIPTION
  This function forces mode pref null to stack_idx.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_action_svlte_stop_stack(
    cmmsc_state_machine_s_type  *msc,
    int                          stack_idx
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  cmmsc_event_force_pref_s_type pref_package;
  cm_orig_q_s_type           *ph_orig_top_ptr = NULL;
  
  ph_orig_top_ptr = cmtask_orig_para_get_top(ss);
  if(ph_orig_top_ptr == NULL)
  {
    CM_ERR_1("Top_ptr NULL for stack %d",ss);
    return;
  }
  
  switch(ss)
  {
    case CM_SS_HYBR_2:
    {
      
      pref_package.pref_reas = SD_SS_PREF_REAS_USER;
      pref_package.orig_mode = SD_SS_ORIG_MODE_NONE;
      pref_package.hybr_pref = ph_orig_top_ptr->orig->orig_hybr_pref;
      cmmsc_action_force_top_ext(msc, &pref_package, stack_idx);      
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_svlte_cond_kick(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_event_acq_fail_s_type *acq_fail_evt,
    boolean                    is_oprt_or_sub_chgd
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);

  switch(ss)
  {
    case CM_SS_HYBR_1:
    {
      if(cmcall_misc_is_mode_pref( acq_fail_evt->main_orig_mode_pref,
                                     CM_MODE_PREF_HDR_ONLY ))
      {
        if (!cmph_is_subs_feature_mode_1x_sxlte(msc->asubs_id)   ||
            (cmph_is_subs_feature_mode_1x_sxlte(msc->asubs_id) &&
            (cmmsc_auto_is_kick_hdr_pending(msc) ||
            #ifdef FEATURE_HDR_HYBRID
            ((cmss_ptr()->hdr_no_srv_uptime != CMSS_NO_SRV_UPTIME_NONE )&&
             (sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_HYBR_1)->srv_status) == FALSE ))||
            #endif
             (sys_srv_status_is_srv(cmss_ptr()->hybr_1_srv_status) == FALSE ))))
        {        
          if( !cmmsc_is_hicps_hybr1_kick_allowed(msc))
          {
            *cmph_hdr_kick_pending() = TRUE;
          }
          else
          {
            *cmph_hdr_kick_pending() = TRUE;
            cmmsc_action_pending_kick(msc, stack_idx, TRUE);
            *cmph_get_is_kicked_hybr_ptr() = TRUE;
          }
        }
      }
      else 
      {
        cmmsc_action_defer_3gpp(msc, stack_idx);
      }
      break;
    }

    case CM_SS_HYBR_2:
    {
      /* If kicking hybr2 is triggerred by online or subscription change */
      if (is_oprt_or_sub_chgd)
      {
        if(!cmmsc_is_hybr2_kick_allowed(msc))
        {
          cmmsc_auto_update_is_kick_hybr2_pending(msc, TRUE);
        }
        else
        {
          cmmsc_action_pending_kick(msc, stack_idx, TRUE);
        }
        break;
      }

      /* kicking hybr2 is triggerred by acq failure rpt from SD */
      if(cmcall_misc_is_mode_pref(acq_fail_evt->main_orig_mode_pref, CM_MODE_PREF_GSM_WCDMA_ONLY))
      {
        if(!cmmsc_is_hybr2_kick_allowed(msc))
        {
          cmmsc_auto_update_is_kick_hybr2_pending(msc, TRUE);
        }
        else
        {
          cmmsc_action_pending_kick(msc, stack_idx, TRUE);
        }
      }
      else if(!cmcall_misc_is_mode_pref( cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                          CM_MODE_PREF_GSM_WCDMA_ONLY ))
      {

        if ( (cmss_ptr()->info.gw_srv_status == SYS_SRV_STATUS_PWR_SAVE ||
              (cmss_ptr()->hybr_gw_no_srv_uptime != CMSS_NO_SRV_UPTIME_NONE &&
               cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == \
               SYS_SRV_STATUS_PWR_SAVE_INTERNAL)) ||
             (*cmph_get_is_kicked_hybr_2_ptr() == FALSE)
           )
        {
          cmmsc_action_pending_kick(msc, stack_idx, TRUE);
        }
      }
      break;
    }

    default:
      break;
  }
}

/*===========================================================================
HYBR_L scan is done and HDR is not supported, next time 1x fails, restore GW
===========================================================================*/
void cmmsc_action_defer_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cmmsc_auto_update_is_kick_gw(msc, TRUE); 
}


/*===========================================================================

===========================================================================*/
void cmmsc_action_enable_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  int hybr1_stk_id = cmmsc_mapping_ss_to_stack_idx(CM_SS_HYBR_1);

  switch(ss)
  {
    case CM_SS_MAIN:
    {
      if(!cmmsc_is_in_ext_no_srv(msc, hybr1_stk_id) ||
         msc->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2)
      {
        (void)cmmsc_auto_update_gwl_operation(TRUE,
                                               SD_SS_PREF_REAS_STACK_SYNC_UP,
                                               CM_MODE_PREF_GWL, 
                                               FALSE, 
                                               msc->asubs_id);
      }
      cmmsc_auto_set_lost_main_uptime(msc, CMMSC_UPTIME_NONE);
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_disable_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  int hybr1_stk_id = cmmsc_mapping_ss_to_stack_idx(CM_SS_HYBR_1);

  switch(ss)
  {
    case CM_SS_MAIN:
    {
      cmmsc_auto_update_gwl_operation(FALSE,
                                        SD_SS_PREF_REAS_STACK_SYNC_UP,
                                        CM_MODE_PREF_GWL,
                                        FALSE, 
                                        msc->asubs_id);

      if(msc->stack[hybr1_stk_id].state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV)
      {
        cmmsc_auto_set_lost_main_uptime(msc, 
                         time_get_uptime_secs() + CMMSC_LOST_MAIN_TIMER);
      }
      else if (msc->stack[hybr1_stk_id].state != CMMSC_STACK_STATE_SRV)
      {
        cmmsc_auto_set_acq_hybr_1_uptime(msc, CMMSC_UPTIME_NONE);
      }

      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_sync_up(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  int hybr1_stk_id = cmmsc_mapping_ss_to_stack_idx(CM_SS_HYBR_1);
  
  boolean is_realign_hdr = srv->rpt_ptr->srv_ind_info.si_info.realign_hdr;
  boolean is_realign_lte = srv->rpt_ptr->srv_ind_info.si_info.realign_lte;
  sd_ss_e_type srv_ind_ss = srv->rpt_ptr->srv_ind_info.ss;
  sys_srv_status_e_type srv_status = srv->rpt_ptr->srv_ind_info.si_info.srv_status;
  sd_mode_e_type mode = srv->rpt_ptr->srv_ind_info.si_info.mode;
  boolean kick_hdr = FALSE;

  cmmsc_event_force_pref_s_type pref_package;

  switch(ss)
  {
    case CM_SS_HYBR_1:
    {      
      /* Action_synn_up for hybr1 stack is only triggerred for cdma in srv
      ** If both stacks are in CMMSC_STACK_STATE_ACTIVE, kick hdr stack;
      ** Otherwise, kick hdr only if is_realign_hdr flag is set.
      */
      if ( srv_ind_ss == SD_SS_MAIN && 
         sys_srv_status_is_srv(srv_status) &&
         mode == SYS_SYS_MODE_CDMA)
      {
        if (msc->stack[0].state == CMMSC_STACK_STATE_ACTIVE && 
          msc->stack[hybr1_stk_id].state == CMMSC_STACK_STATE_ACTIVE)
        {
          kick_hdr = TRUE;
        }  
        else if (is_realign_hdr)
        {
          kick_hdr = TRUE;
        }
      }
      
      /* Kick hdr only if meeting the above conditions */
      if(kick_hdr)
      {
        if(msc->op_mode != CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2 &&
           !cmcall_is_mobile_in_hdr_call())
        {
          pref_package.orig_mode = SD_SS_ORIG_MODE_COLLOC;
          pref_package.pref_reas = SD_SS_PREF_REAS_USER;
          pref_package.hybr_pref 
            = cmph_map_sd_hybr_pref_to_cm_hybr_pref(cmph_ptr()->int_hybr_pref);
          cmmsc_action_force_ph_ext(msc, &pref_package, stack_idx);
        }
        else
        {
          cmmsc_auto_update_is_kick_hdr_pending(msc,   TRUE);
        }
      }

      if(!sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_HYBR_1)->srv_status) &&
         msc->stack[hybr1_stk_id].state != CMMSC_STACK_STATE_HDR_EXT_NO_SRV)
      {
        cmmsc_auto_set_acq_hybr_1_uptime(msc,
                             time_get_uptime_secs() + CMMSC_ACQ_HDR_TIMER);
      }

      if(msc->stack[hybr1_stk_id].state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV &&
         is_realign_hdr)
      {
        msc->stack[hybr1_stk_id].state = CMMSC_STACK_STATE_ACTIVE;
        cmmsc_auto_set_acq_hybr_1_uptime(msc,
                          time_get_uptime_secs() + CMMSC_ACQ_HDR_TIMER);
      }

      break;
    }

    case CM_SS_HYBR_2:
    {
      if(srv->rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_2)
      {
        CM_MSG_HIGH_2("hybr2 stack sync-up %d %d",cmss_ptr()->is_pref_srv_acq, cmss_ptr()->hybr_1_is_pref_srv_acq);
        if ((sys_srv_status_is_full_srv( cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status ) &&
             cmss_ptr()->is_pref_srv_acq == FALSE ) ||
            (sys_srv_status_is_full_srv( cmss_intl_srv_info_ptr(CM_SS_HYBR_1)->srv_status ) &&
             cmss_ptr()->hybr_1_is_pref_srv_acq == FALSE))
        {
          cmmsc_action_force_top(msc, stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP_BSR);
        }
      }
      else
      {
        if(!sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_HYBR_1)->srv_status))
        {
          cmmsc_action_force_top(msc, stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP_BSR);
        }
        else if(sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status) &&
                (is_realign_hdr || is_realign_lte))
        {
          cmmsc_action_force_top(msc, stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP_BSR);
        }
        else if(srv->rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_1)
        {
          cmmsc_action_force_top(msc, stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP_BSR);
        }

      }
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_update_srv(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
)
{
  sd_mode_e_type mode = srv->rpt_ptr->srv_ind_info.si_info.mode;

  switch(stack_idx)
  {
    case 0:
    {
      if ( mode != SD_MODE_LTE)
      {
        msc->stack[stack_idx].state = CMMSC_STACK_STATE_SRV;
      }

      break;
    }
    case 1:
    {
      cmmsc_auto_set_acq_hybr_1_uptime(msc,    CMMSC_UPTIME_NONE);
      break;
    }

    case 2:
    {
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

===========================================================================*/
void cmmsc_action_update_no_srv(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);
  int hybr1_stk_id = cmmsc_mapping_ss_to_stack_idx(CM_SS_HYBR_1);

  switch(ss)
  {
    case CM_SS_MAIN:
    {
      if(msc->stack[hybr1_stk_id].state == CMMSC_STACK_STATE_ACTIVE)
      {
        cmmsc_auto_set_acq_hybr_1_uptime(msc, CMMSC_UPTIME_NONE);
      }
      else if(msc->stack[hybr1_stk_id].state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV)
      {
        cmmsc_auto_set_lost_main_uptime(msc, 
                              time_get_uptime_secs() + CMMSC_LOST_MAIN_TIMER);
      }
      break;
    }

    case CM_SS_HYBR_1:
    {
      cmmsc_auto_set_acq_hybr_1_uptime(msc, 
                              time_get_uptime_secs() + CMMSC_ACQ_HDR_TIMER);
      break;
    }

    default:
      break;
  }
}

/*===========================================================================

DESCRIPTION
  This function takes care of the feature where service acquisition is
  trigerred in a subscription due to a given activity in peer sub.
  Currently it is ending of GSM call on SUB2. This can be expanded
  later to ther events.
  Same action function can be used even for different SIM events.
  
===========================================================================*/
void cmmsc_action_resume_srv_acq(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_call_ind_s_type *call_evt
)
{

  switch(stack_idx)
  {
    case 0:
    case 1:
      CM_MSG_HIGH_2("cmmsc no action on stk %d due to callevt %d",
                    stack_idx, call_evt->call_event);
      break;

    case 2:
    {
      cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   addl_info;
      sd_ss_pref_update_reason_e_type pref_update_reas = 
                                        SD_SS_PREF_UPDATE_REASON_TRM_UNLOCK;

      /* TAU Update takes care of triggerring LTE in SRLTE */
      if(cmph_is_subs_feature_mode_srlte(msc->asubs_id))
      {
        CM_MSG_HIGH_0("Skip forcing LTE(HYBR2) when G2 ended");
        return;
      }

      if(cmph_ptr()->ph_state == CM_PH_STATE_EMERG_CB ||
         cmcall_emergency_call_active())
      {
        break;
      }
               
      addl_info.pref_update_reas = &pref_update_reas;
      cmmsc_action_force_top_addl(msc, stack_idx,
                                  SD_SS_PREF_REAS_STACK_SYNC_UP_BSR,
                                  &addl_info);
    }
    break;
    
    default:
      break;
  }
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_acq_fail_rpt

DESCRIPTION
  This function process acq failure rpt from SD. It provides following functions as result of acq 
  fail on a given stack/sub
    - add/remove mode capability for the stack on which acq failed
    - add/remove mode capability on peer stacks within same sub and kick


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_proc_acq_fail_rpt(

  cmmsc_state_machine_s_type *msc,

  cm_ss_e_type                ss,

  sd_ss_mode_pref_e_type      mode_pref

)
{
  cmmsc_event_acq_fail_s_type  acq_fail;
  int                          stack_idx;
  cm_orig_q_s_type  *ph_orig_ptr =
                        cmtask_orig_para_search_act_type( ss,
                                                          CM_ACT_TYPE_PH_OBJ);
  cmmsc_event_e_type           event = CMMSC_EVENT_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(msc->asubs_id))
  {
    return;
  } 
  if( ph_orig_ptr == NULL )
  {
   CM_ERR_0("ph_orig_ptr is null");
   return;
  }

  CM_MSG_HIGH_1("RPT->MSC: SRV_IND ss=%d",ss);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss != CM_SS_MAX)
  {
    stack_idx = cmph_ptr()->ss_subs_map[ss].stack_id;
  }
  else
  {
    stack_idx = cmph_ptr()->ss_subs_map[CM_SS_MAIN].stack_id;
  }

  if(!cmmsc_is_in_activated_state(msc, stack_idx))
  {
    CM_MSG_HIGH_1("acq_fail_rpt on stk %d ignored", stack_idx);
    return;
  }
  
  switch(stack_idx)
  {
    case 0:

      if(cmmsc_auto_is_kick_gw(msc)  == TRUE &&
                (!cmcall_misc_is_mode_pref( ph_orig_ptr->orig->orig_mode_pref,
                                          CM_MODE_PREF_GSM_WCDMA_ONLY )) &&
                  cmcall_misc_is_mode_pref( cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                          CM_MODE_PREF_GSM_WCDMA_ONLY ))
      {
        event = CMMSC_EVENT_RESTORE_DEFERRED_3GPP;
      }
      else
      {
        event = CMMSC_EVENT_ACQ_FAIL_MAIN;
      }
      break;

    case 1:
      event = CMMSC_EVENT_ACQ_FAIL_HYBR1;
      break;

    case 2:

      if( *cmph_get_is_kicked_hybr_2_ptr() == FALSE )
      {
        break;
      }

      event = CMMSC_EVENT_ACQ_FAIL_HYBR2;
      break;

    default:
      break;
  }
  
  acq_fail.event = event;
  acq_fail.state_machine = msc;
  acq_fail.main_orig_mode_pref = ph_orig_ptr->orig->orig_mode_pref;
  acq_fail.acq_fail_mode_pref  = mode_pref;
  cmmsc_state_machine_process(msc, (cmmsc_event_base_s_type*)&acq_fail);
}

/*===========================================================================

FUNCTION cmmsc_auto_proc_srv_ind_rpt

DESCRIPTION
  This function process srv ind rpt from SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_proc_srv_ind_rpt
(
  cmmsc_state_machine_s_type *msc,

  const cm_sd_rpt_u_type *rpt_ptr,

  sys_srv_status_e_type prev_srv_status_ss
)
{
  cmmsc_event_srv_ind_s_type  srv_evt;
  int                         stack_idx;
  cmmsc_event_e_type          event = CMMSC_EVENT_NONE;
  cm_ss_e_type ss = cmph_map_sd_ss_type_to_cm_ss_type(rpt_ptr->srv_ind_info.ss);
  sys_srv_status_e_type srv_status = rpt_ptr->srv_ind_info.si_info.srv_status;
  boolean is_pref_srv_acq = rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq;
  sd_mode_e_type mode = rpt_ptr->srv_ind_info.si_info.mode;
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(msc->asubs_id))
  {
    return;
  } 
  
  CM_MSG_HIGH_3("RPT->MSC: SRV_IND ss=%d, srv_status=%d, mode=%d",
                 ss, srv_status, mode);

  /* Skip procesing LTE service on main if hybr2 is already operational and on hybr2
  ** if hybr2 is not operational, as it will be a stale LTE service
  */
  if( cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1) &&
      mode == SYS_SYS_MODE_LTE &&
      cmmsc_get_curr_msc_ue_mode(state_machine) == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED &&
     ((cmmsc_auto_is_hybr2_on(state_machine->op_mode))?( ss == CM_SS_MAIN):( ss == CM_SS_HYBR_2)) )
  {
    CM_MSG_HIGH_1("op_mode is %d, don't process stale LTE srv ind", state_machine->op_mode);
    return;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss != CM_SS_MAX)
  {
    stack_idx = cmph_ptr()->ss_subs_map[ss].stack_id;
  }
  else
  {
    stack_idx = cmph_ptr()->ss_subs_map[CM_SS_MAIN].stack_id;
  }

  if(!cmmsc_is_in_activated_state(msc, stack_idx))
  {
    CM_MSG_HIGH_3("srv_ind_rpt status %d on mode %d stk %d ignored",
                                                srv_status, mode, stack_idx);
    return;
  }

  if(sys_srv_status_is_srv(srv_status) && is_pref_srv_acq)
  {
    /* still doing BSR, don't act on srv ind yet */
    CM_MSG_HIGH_3("srv_ind_rpt status %d on mode %d stk %d ignored due to pref_srv_ind",
                                                srv_status, mode, stack_idx);
    return;
  }

  switch(stack_idx)
  {
    case 0:

      if(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(mode), SD_SS_MODE_PREF_CDMA)&&
         sys_srv_status_is_srv(srv_status))
      {
        event = CMMSC_EVENT_1X_SRV;
      }
      else if(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(mode), SD_SS_MODE_PREF_HDR)&&
              sys_srv_status_is_srv(srv_status))
      {
        event = CMMSC_EVENT_MAIN_HDR_SRV;
      }
      else if(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(mode), SD_SS_MODE_PREF_GWL)&&
         sys_srv_status_is_srv(srv_status))
      {
        event = CMMSC_EVENT_MAIN_3GPP_SRV;
      }
      else
      {
        event = CMMSC_EVENT_MAIN_SRV_LOST;
      }
      break;

    case 1:
      if(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(mode), SD_SS_MODE_PREF_HDR)&&
         sys_srv_status_is_srv(srv_status))
      {
        event = CMMSC_EVENT_HYBR_HDR_SRV;
      }
      else
      {
        event = CMMSC_EVENT_HYBR1_SRV_LOST;
      }
      break;

    case 2:
      if(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(mode), SD_SS_MODE_PREF_LTE)&&
         (sys_srv_status_is_full_srv( srv_status ) || srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL))
      {
        event = CMMSC_EVENT_HYBR_3GPP_SRV;
      }
      else
      {
        event = CMMSC_EVENT_HYBR2_SRV_LOST;
      }
      break;
  }

  srv_evt.event = event;
  srv_evt.state_machine = msc;
  srv_evt.rpt_ptr = rpt_ptr;
  srv_evt.prev_srv_status_ss = prev_srv_status_ss;

  cmmsc_state_machine_process(msc, (cmmsc_event_base_s_type*)&srv_evt);

}

/*===========================================================================

===========================================================================*/
void cmmsc_auto_proc_3gpp_stopped_rpt(

  cmmsc_state_machine_s_type *msc,

  cm_ss_e_type ss
)
{
  cmmsc_event_base_s_type  stopped_rpt;
  cmmsc_event_e_type       event = CMMSC_EVENT_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit if not 3GPP is not allowed on the sub */
  if (!cmph_msim_is_3gpp2_allowed(msc->asubs_id))
  {
    return;
  }

  switch(ss)
  {
    case CM_SS_MAIN:
      event = CMMSC_EVENT_MAIN_3GPP_STOPPED;
      break;
    case CM_SS_HYBR_2:
      event = CMMSC_EVENT_HYBR_3GPP_STOPPED;
      break;
    default:
      break;
  }
  
  stopped_rpt.event = event;
  stopped_rpt.state_machine = msc;
  cmmsc_state_machine_process(msc, &stopped_rpt);
}

/*===========================================================================

  DESCRIPTION:
   This is entry function from SIMC to MSC to handle all call events that
   happen either in peer subs. Currently its implemented for handling GSM2
   voice call end and trigerring hybr-LTE service acquisition.
===========================================================================*/
void cmmsc_auto_proc_peer_subs_call
(
  cmmsc_state_machine_s_type *msc,

  const cmmsc_param_call_ind_s_type* call_ind
)
{
  cmmsc_event_base_s_type  call_evt;
  cmmsc_event_e_type event;

  if(call_ind->call_event == CM_CALL_EVENT_END)
  {
    event = CMMSC_EVENT_CALL_END_OTHER_SUB;
  }
  else
  {
    CM_MSG_HIGH_2("call ind rpt %d ignored in as_id %d",
                  call_ind, msc->asubs_id);
    return;
  }
  
  call_evt.event = event;
  call_evt.state_machine = msc;

  cmmsc_state_machine_process(msc, &call_evt);

}


/*===========================================================================

FUNCTION cmmsc_auto_update_lte_data_ss_srlte

DESCRIPTION
 It will update call_ptr->ss for LTE data calls, from MAIN to HYBR_2 when 
 switch to dual stack happens in SRLTE.
 Similarly, when switching from dual to single stack, update call_ptr->ss
 to MAIN .

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmmsc_auto_update_lte_data_ss_srlte(
cmmsc_state_machine_op_mode_e_type prev_op_mode, 
cmmsc_state_machine_op_mode_e_type new_op_mode
)
{
  cmcall_s_type	*call_ptr;	/* Call object pointer */
  cm_iterator_type call_itr;
  /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE
       )  
    {
      CM_MSG_MED_2("lte data call call:%d ss;%d",call_ptr->call_id, call_ptr->ss);
      if(cmmsc_auto_is_hybr2_switched_on(prev_op_mode, new_op_mode) &&
	  	 call_ptr->ss == CM_SS_MAIN)
      {      	
        CM_MSG_HIGH_1("Updating ss for call_id:%d to HYBR2",call_ptr->call_id);
        call_ptr->ss = CM_SS_HYBR_2;
      }
	  
      if(cmmsc_auto_is_hybr2_switched_off(prev_op_mode, new_op_mode) &&
         call_ptr->ss == CM_SS_HYBR_2)
      {		
        CM_MSG_HIGH_1("Updating ss for call_id:%d to MAIN",call_ptr->call_id);
        call_ptr->ss = CM_SS_MAIN;
      }

    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */


}

