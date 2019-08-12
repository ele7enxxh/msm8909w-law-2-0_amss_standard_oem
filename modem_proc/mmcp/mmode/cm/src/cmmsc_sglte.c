/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc_sglte.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/13   ss      Add transaction to routing table when moving from SINGLE STACK
                   FULL RAT
11/14/13   ss      Correct function call sequence for local act and local deact
02/26/13   cl      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */

#include "comdef.h"   /* Definition for basic types and macros */
#include "cmph.h"     /* Interface to CM ph functions */
#include "cmmsc.h"
#include "cmmsc_sglte.h"
#include "cmsimcoord.h"
#include "cmsoa.h"
#include "mmoc_v.h"
#include "modem_mem.h"
#include "sys.h"      /* Common types. */

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
#include "cmregprxi.h"
#endif

#include "cmpmprx.h"

#ifdef CM_DEBUG
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Routing Table - SGLTE
** EVENT,
** STATE for stack1, 2, ...
** ACTION for stack 1, 2, ... (order matters)
** NEW STATE for stack 1, 2, ...
*/
static cmmsc_routing_table_sglte_entry_s_type cmmsc_sglte_routes[] =
{

  /* SUB available event:
       From NORMAL LPM to NORMAL LPM
       From NORMAL LPM to DUAL LPM
       From NORMAL LPM to ROAM LPM
     SUB lost event:
       From NORMAL LPM to NORMAL LPM
       From DUAL LPM to NORMAL LPM
       From ROAM LPM to NORMAL LPM
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL}
    },

  /* SUB available event:
       From NORMAL to DUAL
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_FORCE_USER}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE}
    },

  /* SUB available event:
       From NORMAL to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* SUB available event:
       From NORMAL to ROAM
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* 4 */
  /* SUB available event:
       From NORMAL to NORMAL
     SUB lost event:
       From NORMAL to NORMAL
       From ROAM to NORMAL
       From FULL_RAT_SEARCH to NORMAL
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* SUB lost event:
       From DUAL to ROAM
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_FORCE_RESTORE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* SUB lost event:
       From SINGLE to ROAM
  */
  { CMMSC_EVENT_SUB,                      CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_BARRED},
    {{0,CMMSC_ACTION_SUB},                {1,CMMSC_ACTION_FORCE_RESTORE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* 7 */
  /* ONLINE event:
       From DUAL LPM to DUAL
  */
  { CMMSC_EVENT_ONLINE,                   CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_FORCE_USER}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE}
    },

  /* ONLINE event:
       From ROAM LPM to ROAM
  */
  { CMMSC_EVENT_ONLINE,                   CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* ONLINE event:
       From ROAM LPM to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_ONLINE,                   CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* 10 */
  /* ONLINE event:
       From NORMAL LPM to NORMAL
  */
  { CMMSC_EVENT_ONLINE,                   CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_INACTIVE}
    },

  /* POWER_DOWN event:
       From DUAL to DUAL LPM
       From NORMAL to NORMAL LPM
       From SINGLE to DUAL LPM
       From ROAM to ROAM LPM
       From FULL_RAT_SEARCH to ROAM LPM
  */
  { CMMSC_EVENT_POWER_DOWN,               CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_POWER_DOWN},         {1,CMMSC_ACTION_POWER_DOWN}},
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL}
    },

  /* Case: user change mode during LPM. we allow mode change go through
  ** For now, we make this case "explicit", in fact, we could make it "generic"
  ** with the source-state = ANY, target-state = NO CHANGE (?)
  */
  /* 12 */
  /* SYS_SEL_PREF_MAIN event:
       From DUAL LPM to DUAL LPM
       From NORMAL LPM to NORMAL LPM
       From NORMAL LPM to ROAM LPM
       From ROAM LPM to ROAM LPM
       From ROAM LPM to NORMAL LPM
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,        CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_NULL,              CMMSC_STACK_STATE_NULL},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NULL,             CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_MAIN event:
       From SINGLE to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,        CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_BARRED},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },

  /* SYS_SEL_PREF_MAIN event:
       From SINGLE to ROAM
  */

  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,        CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
   },
  /* SYS_SEL_PREF_MAIN event:
       From NORMAL to FULL_RAT_SEARCH
       From NORMAL to ROAM
       From NORMAL to NORMAL
       From DUAL to DUAL
       From SINGLE to SINGLE
       From ROAM to ROAM
       From ROAM to FULL_RAT_SEARCH
       From ROAM to NORMAL
       From FULL_RAT_SEARCH to ROAM
       From FULL_RAT_SEARCH to NORMAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAIN,        CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG}
    },

  /* SYS_SEL_PREF_HYBR2 event:
       From FULL_RAT_SEARCH to DUAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,       CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ANY   ,            CMMSC_STACK_STATE_INACTIVE},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_ACTIVE}
    },

  /* 17 */
  /* SYS_SEL_PREF_HYBR2 event:
       From DUAL to DUAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,       CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE}
    },

  /* SYS_SEL_PREF_HYBR2 event:
       From SINGLE to SINGLE
  */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,       CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_BARRED},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_BARRED}
    },

  /* SYS_SEL_PREF_HYBR2 event:
       From NORMAL to NORMAL
       From NORMAL LPM to NORMAL LPM
       From DUAL LPM to DUAL LPM
       From ROAM to ROAM
       From ROAM LPM to ROAM LPM
       From FULL_RAT_SEARCH to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_SYS_SEL_PREF_HYBR2,       CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG}
    },

	/* SYS_SEL_PREF_MAX event:
		  From ROAM to DUAL
		  From FULL_RAT_SEARCH to DUAL
	 */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,		 CMMSC_OP_MODE_SGLTE_DUAL_STACK,
	{CMMSC_STACK_STATE_ACTIVE,				CMMSC_STACK_STATE_INACTIVE},
	{{0,CMMSC_ACTION_SYS_SEL_PREF},		 {1,CMMSC_ACTION_SYS_SEL_PREF}},
	{ CMMSC_STACK_STATE_NO_CHG,			 CMMSC_STACK_STATE_ACTIVE}
	},

  /* 21 */
  /* SYS_SEL_PREF_MAX event:
       From NORMAL to DUAL
       From ROAM to DUAL
       From FULL_RAT_SEARCH to DUAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_INACTIVE},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_FORCE_USER}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE}
    },

   /* SYS_SEL_PREF_MAX event:
       From SINGLE to DUAL
       From DUAL to DUAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE}
    },

  /* SYS_SEL_PREF_MAX event:
       From SINGLE to SINGLE
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_FORCE_RESTORE}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_NO_CHG}
    },

  /* Deactivate hybr2 stack forst and then force preference on main */
  /* SYS_SEL_PREF_MAX event:
       From DUAL to ROAM
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ACTIVE},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },

  /* 25 */
  /* SYS_SEL_PREF_MAX event:
       From SINGLE to ROAM
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_BARRED},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },


  /* SYS_SEL_PREF_MAX event:
       From DUAL to NORMAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ACTIVE},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },

  /* SYS_SEL_PREF_MAX event:
       From SINGLE to NORMAL
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_BARRED},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },

  /* SYS_SEL_PREF_MAX event:
       From DUAL to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_ACTIVE   ,         CMMSC_STACK_STATE_ACTIVE},
    {{1,CMMSC_ACTION_SYS_SEL_PREF},       {0,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_INACTIVE}
    },

  /* 29 */
  /* SYS_SEL_PREF_MAX event:
       From NORMAL to NORMAL
       From NORMAL LPM to DUAL LPM
       From NORMAL LPM to NORMAL LPM
       From DUAL LPM to DUAL LPM
       From DUAL LPM to ROAM LPM
       From DUAL LPM to NORMAL LPM
       From ROAM to ROAM
       From ROAM LPM to ROAM LPM
       From ROAM LPM to DUAL LPM
       From FULL_RAT_SEARCH to FULL_RAT_SEARCH
  */
  { CMMSC_EVENT_SYS_SEL_PREF_MAX,         CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SYS_SEL_PREF},       {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_NO_CHG}
    },


  /* Merge - first, deactivate stack-2 and then force new pref stack 1 */
  /* PS_DOMAIN_TRANSFER event:
       From DUAL to SINGLE ( L/T --> G iRAT (merging) )
  */
  { CMMSC_EVENT_GSM_PS_DOMAIN_TRANSFER,   CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ACTIVE},
    {{1,CMMSC_ACTION_LOCAL_DEACT},        {0,CMMSC_ACTION_G_PS_DOMAIN_TRNSFR}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_BARRED}
    },

  /* Split - first, activate stack-2 and then force stack-1 new pref*/
  /* PS_DOMAIN_TRANSFER event:
       From SINGLE to DUAL ( G --> L/T iRAT (splitting) NAS Splits stacks when found: PS-only / CS-only cell )
  */
  { CMMSC_EVENT_GSM_PS_DOMAIN_TRANSFER,   CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_BARRED},
    {{1,CMMSC_ACTION_LOCAL_ACT},          {0,CMMSC_ACTION_G_PS_DOMAIN_TRNSFR}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE}
    },

  /* DUAL_STK_EMERG_CALL event:
       From DUAL to DUAL
  */
  { CMMSC_EVENT_DUAL_STK_EMERG_CALL,      CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ACTIVE},
    {{0,CMMSC_ACTION_FORCE_USER},         {1,CMMSC_ACTION_SYS_SEL_PREF}},
    { CMMSC_STACK_STATE_NO_CHG,           CMMSC_STACK_STATE_ACTIVE}
    },

  /* 33 */
  /* DUAL_STK_EMERG_CALL event:
       From FULL RAT to DUAL
  */
  { CMMSC_EVENT_DUAL_STK_EMERG_CALL,      CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_FORCE_USER},         {1,CMMSC_ACTION_FORCE_USER}},
    { CMMSC_STACK_STATE_ACTIVE,           CMMSC_STACK_STATE_ACTIVE}
    },

  { CMMSC_EVENT_OFF,                      CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NULL,             CMMSC_STACK_STATE_NULL}
    },	

  { CMMSC_EVENT_ON,                      CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_KICK_HYBR2}},
    { CMMSC_STACK_STATE_ACTIVE,          CMMSC_STACK_STATE_ACTIVE}
    },

    { CMMSC_EVENT_ON,                      CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ACTIVE,            CMMSC_STACK_STATE_BARRED},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,          CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_ON,                      CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,          CMMSC_STACK_STATE_NO_CHG}
    },

    /* SUSPEND STACKS */
    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_ACTIVE,               CMMSC_STACK_STATE_ACTIVE},
    {{1,CMMSC_ACTION_SUSPEND},               {0,CMMSC_ACTION_SUSPEND}},
    { CMMSC_STACK_STATE_SUSPENDED,           CMMSC_STACK_STATE_SUSPENDED}
    },

    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_ACTIVE,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,           CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_ACTIVE,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_ACTIVE,               CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},               {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_ACTIVE,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_SUSPEND},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_SUSPENDED,              CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_SUSPEND,                   CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,              CMMSC_STACK_STATE_NO_CHG}
    },

    /* RESUME STACKS */
    { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_SGLTE_DUAL_STACK,
    {CMMSC_STACK_STATE_SUSPENDED,            CMMSC_STACK_STATE_SUSPENDED},
    {{0,CMMSC_ACTION_RESUME},                {1,CMMSC_ACTION_FORCE_USER_WITH_RESUME}},
    { CMMSC_STACK_STATE_ACTIVE,              CMMSC_STACK_STATE_ACTIVE}
    },

    { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_SGLTE_SINGLE_STACK,
    {CMMSC_STACK_STATE_SUSPENDED,            CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESUME},                {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

    /* TODO: SEEMS INVALID STATE, WILL REMOVE AFTER CODE REVIEW */
    { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_SGLTE_ROAM,
    {CMMSC_STACK_STATE_SUSPENDED,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESUME},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

    { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_NORMAL,
    {CMMSC_STACK_STATE_SUSPENDED,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESUME},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

        { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_FULL_RAT_SEARCH,
    {CMMSC_STACK_STATE_SUSPENDED,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_RESUME},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_ACTIVE,              CMMSC_STACK_STATE_NO_CHG}
    },

        { CMMSC_EVENT_RESUME,                    CMMSC_OP_MODE_ANY,
    {CMMSC_STACK_STATE_ANY,                  CMMSC_STACK_STATE_ANY},
    {{0,CMMSC_ACTION_NONE},                  {1,CMMSC_ACTION_NONE}},
    { CMMSC_STACK_STATE_NO_CHG,              CMMSC_STACK_STATE_NO_CHG}
    },
};

/*===========================================================================

            FUNCTION DECLARATIONS AND IMPLEMENTATION

This section contains function declaration and implementation for this
feature.

===========================================================================*/



/*===========================================================================

FUNCTION cmmsc_get_sglte_routes

DESCRIPTION
  This function return the pointer to sglte_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cmmsc_routing_table_sglte_entry_s_type *cmmsc_get_sglte_routes( void )
{
  return (cmmsc_routing_table_sglte_entry_s_type*)&cmmsc_sglte_routes;
}


/*===========================================================================

FUNCTION cmmsc_get_sglte_routes_size

DESCRIPTION
  This function return the size of the sglte_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int cmmsc_get_sglte_routes_size( void )
{
  return ARR_SIZE(cmmsc_sglte_routes);
}

/*===========================================================================

FUNCTION cmmsc_sglte_routing_table_init

DESCRIPTION
  This function initializes SGLTE Routing Table

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_sglte_routing_table_init( cmmsc_routing_table_s_type *sglte_route_table )
{
  sglte_route_table->n_row = cmmsc_get_sglte_routes_size();
  sglte_route_table->n_stack = CMMSC_SGLTE_STACK_NUM;
  sglte_route_table->route_tbl = (cmmsc_routing_table_entry_base_s_type*)cmmsc_get_sglte_routes();
  sglte_route_table->route_tbl_id = CMMSC_ROUTE_TABLE_SGLTE;
}


/*===========================================================================

FUNCTION cmmsc_set_stack_capability_sglte

DESCRIPTION
  This function set stack capability for sglte.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_sglte( cmmsc_state_machine_s_type *state_machine )
{
  sys_sys_mode_mask_e_type    pm_mode       = SYS_SYS_MODE_MASK_NONE;
  sd_ss_mode_pref_e_type      mode          = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type      band          = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type      tds_band      = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type    lte_band      = SYS_LTE_BAND_MASK_CONST_NONE;

  cmmsc_get_policy_mode_band_cap(state_machine,
                                 &pm_mode,
                                 &band,
                                 &tds_band,
                                 &lte_band);

  mode = cmph_map_sys_mode_mask_to_sd_mode_pref(pm_mode);

  switch (state_machine->op_mode)
  {
    case CMMSC_OP_MODE_SGLTE_DUAL_STACK:
    {
      /* copy previous capability */
      state_machine->stack[0].prev_registration_capability
                                                     = state_machine->stack[0].capablity;
      state_machine->stack[1].prev_registration_capability
                                                     = state_machine->stack[1].capablity;

      /* set capability for SGLTE dual stack */
      state_machine->stack[0].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_TDS_GSM_LTE);
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
      state_machine->stack[0].capablity.band         = band;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_GSM);
      state_machine->stack[1].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
      state_machine->stack[1].capablity.band         = band;
      state_machine->stack[1].capablity.tds_band     = SD_SS_BAND_PREF_NONE;
      SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&state_machine->stack[1].capablity.lte_band);
      break;
    }

    case CMMSC_OP_MODE_SGLTE_SINGLE_STACK:
    {
      /* Transition to single-stack only happens between Single/Dual SGLTE modes
      ** hence, Capability is not changed so we don't need to copy previous capability. */

      /* Set SGLTE single stack capability */
      state_machine->stack[0].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_TDS_GSM_LTE);
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = band;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_GSM);
      state_machine->stack[1].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
      state_machine->stack[1].capablity.band         = band;
      state_machine->stack[1].capablity.tds_band     = SD_SS_BAND_PREF_NONE;
      SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&state_machine->stack[1].capablity.lte_band);
      break;
    }

    case CMMSC_OP_MODE_SGLTE_ROAM:
    {
      /* copy previous capability */
      state_machine->stack[0].prev_registration_capability
                                                     = state_machine->stack[0].capablity;
      state_machine->stack[1].prev_registration_capability
                                                     = state_machine->stack[1].capablity;

      /* Set CSFB capability */
      state_machine->stack[0].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE);
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = band;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
    }

    case CMMSC_OP_MODE_FULL_RAT_SEARCH:
    {
      /** For SGLTE, during FullRAT Mode, we're not expect to do any
      **  registration and hence for FullRAT Mode (CAMP_ONLY), this
      **  prev_registration_capability will not be saved. */

      /* Set CSFB capability */
      state_machine->stack[0].capablity.mode         = SD_GET_COMMON_MODE(mode,SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE);
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = SD_SS_BAND_PREF_ANY; /* add as per policy */
      state_machine->stack[0].capablity.tds_band     = SD_SS_BAND_PREF_TDS_ANY;
      state_machine->stack[0].capablity.lte_band     = SYS_LTE_BAND_MASK_CONST_ANY;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
    }

    case CMMSC_OP_MODE_NORMAL:
    default:
      /* copy previous capability */
      state_machine->stack[0].prev_registration_capability
                                                     = state_machine->stack[0].capablity;
      state_machine->stack[1].prev_registration_capability
                                                     = state_machine->stack[1].capablity;
      state_machine->stack[0].capablity.mode         = mode;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = band;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
  }
}

void cmmsc_set_stack_capability_sglte_emergency(
   cmmsc_state_machine_s_type *state_machine  )
{
  sys_sys_mode_mask_e_type    pm_mode       = SYS_SYS_MODE_MASK_NONE;
  sd_ss_band_pref_e_type      band          = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type      tds_band      = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type    lte_band      = SYS_LTE_BAND_MASK_CONST_NONE;

  cmmsc_get_policy_mode_band_cap(state_machine,
                                 &pm_mode,
                                 &band,
                                 &tds_band,
                                 &lte_band);

  switch (state_machine->op_mode)
  {
    case CMMSC_OP_MODE_SGLTE_DUAL_STACK:
    {
      /* set capability for SGLTE dual stack
      ** - Disabled Main (by setting mode = NONE)
      ** - Enabled Hybr-2 (all GSM bands)
      */
      state_machine->stack[0].capablity.mode         = SD_SS_MODE_PREF_NONE;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
      state_machine->stack[0].capablity.band         = band;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_GSM;
      state_machine->stack[1].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
      state_machine->stack[1].capablity.band         = SD_SS_BAND_PREF_ANY;
      break;
    }

    case CMMSC_OP_MODE_SGLTE_SINGLE_STACK:
    {
      /* Set SGLTE single stack capability
      ** - Enabled all bands for GSM. Mode remains the same.
      */
      state_machine->stack[0].capablity.mode         = SD_SS_MODE_PREF_TDS_GSM_LTE;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = SD_SS_BAND_PREF_ANY;
      state_machine->stack[0].capablity.tds_band     = tds_band;
      state_machine->stack[0].capablity.lte_band     = lte_band;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_GSM;
      state_machine->stack[1].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
      break;
    }

    case CMMSC_OP_MODE_SGLTE_ROAM:
    {
      /* Set CSFB capability
      ** - Enabled all bands/ all RATs
      */
      state_machine->stack[0].capablity.mode         = SD_SS_MODE_PREF_ANY;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = SD_SS_BAND_PREF_ANY;
      state_machine->stack[0].capablity.tds_band     = SD_SS_BAND_PREF_TDS_ANY;
      state_machine->stack[0].capablity.lte_band     = SYS_LTE_BAND_MASK_CONST_ANY;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
    }

    case CMMSC_OP_MODE_FULL_RAT_SEARCH:
    {
      /* Full RAT means "CSFB" mode. So applied SGLTE_ROAM setting.
      ** - Disabled Main
      ** - Enabled all bands on GSM on HYBR-2
      */
      state_machine->stack[0].capablity.mode         = SD_SS_MODE_PREF_ANY;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = SD_SS_BAND_PREF_ANY;
      state_machine->stack[0].capablity.tds_band     = SD_SS_BAND_PREF_TDS_ANY;
      state_machine->stack[0].capablity.lte_band     = SYS_LTE_BAND_MASK_CONST_ANY;

      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
    }

    case CMMSC_OP_MODE_NORMAL:
    default:
      state_machine->stack[0].capablity.mode         = SD_SS_MODE_PREF_ANY;
      state_machine->stack[0].capablity.domain       = SD_SS_SRV_DOMAIN_PREF_CS_PS;
      state_machine->stack[0].capablity.band         = SD_SS_BAND_PREF_ANY;
      state_machine->stack[0].capablity.tds_band     = SD_SS_BAND_PREF_TDS_ANY;
      state_machine->stack[0].capablity.lte_band     = SYS_LTE_BAND_MASK_CONST_ANY;
      state_machine->stack[1].capablity.mode         = SD_SS_MODE_PREF_NONE;
      break;
  }
}

/*===========================================================================

FUNCTION cmmsc_sglte_retrieve_op_mode

DESCRIPTION
  This function gets the current value of cmmsc op mode.

DEPENDENCIES
  None.

RETURN VALUE
  NONE
===========================================================================*/
cmmsc_state_machine_op_mode_e_type cmmsc_sglte_retrieve_op_mode(
   cmmsc_state_machine_s_type * state_machine
)
{
  sys_ue_mode_e_type curr_ue_mode;

  if( cmpmprx_get_ue_mode(SYS_MODEM_AS_ID_1,&curr_ue_mode)== TRUE)
  {
    if( curr_ue_mode != state_machine->stack_common_info.ue_mode )
    {
      state_machine->stack_common_info.ue_mode = curr_ue_mode;
      state_machine->stack_common_info.ue_mode_forced = FALSE;
      return cmmsc_map_ue_mode_pm2msc(curr_ue_mode);
    }
  }

  return state_machine->op_mode;
}

/*===========================================================================

FUNCTION cmmsc_sglte_update_op_mode

DESCRIPTION
  This function updates the op mode upon subscription change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_sglte_update_op_mode (
  cmmsc_state_machine_s_type  *state_machine,
  boolean                     is_gwl_sub_avail
)
{
  /* Before gw sub avail, UE always operates on normal mode */
  if (is_gwl_sub_avail == TRUE)
  {

    cmmsc_state_machine_op_mode_e_type op_mode = cmmsc_sglte_retrieve_op_mode(state_machine);
    if (op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK ||
        op_mode == CMMSC_OP_MODE_SGLTE_SINGLE_STACK)
    {
      cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_SGLTE_DUAL_STACK );
    }

    else if(op_mode == CMMSC_OP_MODE_SGLTE_ROAM )
    {
      cm_orig_q_s_type *ph_orig_top_ptr =
                                cmtask_orig_para_get_top( CM_SS_MAIN );

      if( ph_orig_top_ptr != NULL &&
          ph_orig_top_ptr->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT )
      {
        cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_FULL_RAT_SEARCH );
      }
      else
      {
      cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_SGLTE_ROAM );
      }
    }
    else if( op_mode == CMMSC_OP_MODE_FULL_RAT_SEARCH )
    {
      cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_FULL_RAT_SEARCH );
    }
    else
    /* Default handling */
    {
      cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_NORMAL );
    }
  }
  else
  {
    state_machine->stack_common_info.ue_mode = SYS_UE_MODE_NORMAL;
    cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_NORMAL );
  }
}


/*===========================================================================

FUNCTION cmmsc_action_local_act

DESCRIPTION
  The function will do the following:
  1. Activate the specified stack in CM.
  2. send local activate indication to mmoc.

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_local_act(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx)
{
  cmmsc_update_cmss_is_operational(state_machine, stack_idx, TRUE);

  /* Set skip_srv_ind_update to true, so that srv change will not notify the clients until
   ** CM_SRV_IND/CM_SRV_CONF is received from cmregprx to sd to CM.
   */
  cmss_update_skip_srv_ind_update(TRUE);

  /* Send local act ind to mmoc */
  mmoc_cmd_prot_local_act_ind(cmmsc_sglte_mapping_stack_id_to_sd_ss(stack_idx));

  /* Force new preferences on top (it possibly that we have switch the calls)
  ** from HYBR-2 to MAIN.
  */
  cmmsc_action_force_top(state_machine, stack_idx, SD_SS_PREF_REAS_RESTORE);

}

/*===========================================================================

FUNCTION cmmsc_action_local_deact

DESCRIPTION
  The function will do the following:
  1. Deactivate stack locally
  2. Transfer data

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_local_deact(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx)
{
  if (state_machine == NULL)
    sys_err_fatal_null_ptr_exception();

  /* Set skip_srv_ind_update to true, so that srv change will not notify the clients until
   ** CM_SRV_IND/CM_SRV_CONF is received from cmregprx to sd to CM.
   */
  cmss_update_skip_srv_ind_update(TRUE);

  cmmsc_update_cmss_is_operational(state_machine, stack_idx, FALSE);


  /* Send local deact ind to mmoc */
  mmoc_cmd_prot_local_deact_ind(cmmsc_sglte_mapping_stack_id_to_sd_ss(stack_idx));

  /* Force new preferences on top (it possibly that we have switch the calls)
  ** from HYBR-2 to MAIN
  */
  cmmsc_action_force_top(state_machine, stack_idx, SD_SS_PREF_REAS_RESTORE);

}

/*===========================================================================

FUNCTION cmmsc_action_g_ps_domain_transfer

DESCRIPTION
  The function will do the following:
  1. force the top preference onto SD with reason RESTORE after
     service has been transfered to Main or Hybrd

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_g_ps_domain_transfer(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx)
{
  /* Force preferences base on the new ue mode */
  cmmsc_action_force_top(state_machine, stack_idx, SD_SS_PREF_REAS_RESTORE);
}


#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION cmmsc_proc_stack_deact_ind

DESCRIPTION
  This function should be called when NAS wants to deactivate a stack.
  In SC SGLTE, it could be due to L->G Reselection or PS Domain Transfer

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_stack_deact_ind( cm_ss_e_type ss )
{
  //SGLTE NEED CHANGE
  cmmsc_state_machine_s_type   *state_machine;
  cmmsc_event_e_type            event = CMMSC_EVENT_NONE;
  cmmsc_event_base_s_type       event_ps_domain_transfer;
  sys_modem_as_id_e_type        as_id = cmph_map_cm_ss_to_subs(ss);

  state_machine = cmmsimc_state_machine_ptr(as_id);  

  /* Update the UE-Mode to SGLTE Single Stack operation */
  cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_SGLTE_SINGLE_STACK );

  /* Set event = SERVICE_TRANSFER */
  event = CMMSC_EVENT_GSM_PS_DOMAIN_TRANSFER;

  /* Reset stack capability */
  cmmsc_set_stack_capability(state_machine);

  /* Package data */
  event_ps_domain_transfer.event = event;
  event_ps_domain_transfer.state_machine = state_machine;

  /* Consult & Process in Routing Table */
  cmmsc_state_machine_process(state_machine,&event_ps_domain_transfer);

}

/*===========================================================================

FUNCTION cmmsc_proc_stack_act_ind

DESCRIPTION
  This function should be called when CM receives stack activation indication from NAS.
  In SC SGLTE, it could be due to G->L Reselection or PS Domain Transfer

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_stack_act_ind(
    cm_ss_e_type  ss
)
{
  //SGLTE NEED CHANGE
  cmmsc_state_machine_s_type   *state_machine;
  cmmsc_event_e_type            event = CMMSC_EVENT_NONE;
  cmmsc_event_base_s_type       event_ps_domain_transfer;
  sys_modem_as_id_e_type        as_id = cmph_map_cm_ss_to_subs(ss);

  state_machine = cmmsimc_state_machine_ptr(as_id);

  /* Update the UE-Mode */
  cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_SGLTE_DUAL_STACK);

  /* Set event = SERVICE_TRANSFER */
  event = CMMSC_EVENT_GSM_PS_DOMAIN_TRANSFER;

  /* Set stack capability */
  cmmsc_set_stack_capability(state_machine);

  /* Package data */
  event_ps_domain_transfer.event = event;
  event_ps_domain_transfer.state_machine = state_machine;

  /* Process in Routing Table */
  cmmsc_state_machine_process(state_machine,&event_ps_domain_transfer);
}
#endif /* defined CM_GW_SUPPORTED ||defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)*/


/*===========================================================================

FUNCTION cmmsc_special_mode_update_for_main_stack

DESCRIPTION
  This function updates main stack with special handling in case MAIN has
  GSM only. This is SGLTE specific requirements.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_special_mode_update_for_main_stack (
  cmmsc_state_machine_s_type  *state_machine,
  int                     stack_idx,
  sd_ss_mode_pref_e_type  *mode
)
{
  /* For sglte dual stack mode, if main stack has GSM only, then set mode to none.
   ** This is done due to NAS limiation - NAS is unable to handle G + G
   */
  if (mode == NULL)
  {
    CM_ERR_0("SC_SGLTE: mode null ptr");
    return;
  }

  if (state_machine->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK &&
      stack_idx == 0 &&
      *mode == SD_SS_MODE_PREF_GSM)
  {
    *mode = SD_SS_MODE_PREF_NONE;
  }
}


/*===========================================================================

FUNCTION cmmsc_sglte_get_ss_for_domain

DESCRIPTION
  This function return the stack per domain input based on SGLTE requirement.
  This is SGLTE specific.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cm_ss_e_type cmmsc_sglte_get_ss_for_domain (
    sys_modem_as_id_e_type       as_id,
    sd_ss_srv_domain_pref_e_type domain
)
{
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);

  CM_ASSERT_ENUM_IS_INRANGE(domain, SD_SS_SRV_DOMAIN_PREF_MAX);
  if (domain == SD_SS_SRV_DOMAIN_PREF_NONE)
  {
    CM_ERR_0("SC_SGLTE: SD_SS_SRV_DOMAIN_PREF_NONE");
    return CM_SS_NONE;
  }

  switch(state_machine->op_mode)
  {
    case CMMSC_OP_MODE_SGLTE_DUAL_STACK:
    {
      if (domain == SD_SS_SRV_DOMAIN_PREF_CS_ONLY)
        return CM_SS_HYBR_2;
      else if (domain == SD_SS_SRV_DOMAIN_PREF_PS_ONLY)
        return CM_SS_MAIN;
      else
        return CM_SS_NONE;
    }

    default:
    {
      SYS_ARG_NOT_USED(domain);
      return CM_SS_MAIN;
    }
  }
}

/****************************************************************************
** Static Function Misc Implementation
****************************************************************************/


/*===========================================================================

FUNCTION cmmsc_is_ps_detach_required

DESCRIPTION
  Stack previous preference that were used to attached to network
  If the preference is changed, CM will triggers PS_DETACH.
  For SGLTE, during FullRAT Mode, we're not expect to do any registration and
  hence for FullRAT Mode (CAMP_ONLY), this prev_registration_capability will not
  be saved.

  Implemented per NAS team requirement:
     Expectation of Local/OTA detach or no detach in SGLTE context.
  1. SGLTE -> FULL RAT (in OOS) -> SGLTE ( NO Detach as start ue_mode and end ue_mode are same)
  2. SGLTE -> FULL RAT (in OOS) -> CSFB ( Local Detach)
  3. CSFB -> FULL RAT (in OOS) -> CSFB ( No detach)
  4. CSFB -> FULL RAT (in OOS) -> SGLTE (Local detach)
  5. CSFB -> SGLTE (OTA detach)
  6. SGLTE -> CSFB (OTA detach)

DEPENDENCIES
  None.

RETURN VALUE
  Boolean TRUE: ps_detach required
          FALSE: ps_detach not required

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_evaluate_ps_detach_required_sglte(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx,cm_mode_band_pref_s_type old_stack_pref)
{
  cmmsc_stack_s_type         *stack_pointer;
  sd_ss_mode_pref_e_type      ph_mode;
  sd_ss_band_pref_e_type      ph_band;
  sd_ss_band_pref_e_type      ph_tds_band;
  sys_lte_band_mask_e_type    ph_lte_band;

  sd_ss_mode_pref_e_type      old_mode;
  sd_ss_band_pref_e_type      old_band;
  sd_ss_band_pref_e_type      old_tds_band;
  sys_lte_band_mask_e_type    old_lte_band;

  sd_ss_mode_pref_e_type      new_mode;
  sd_ss_band_pref_e_type      new_band;
  sd_ss_band_pref_e_type      new_tds_band;
  sys_lte_band_mask_e_type    new_lte_band;
  cmph_s_type  *ph_ptr = cmph_ptr();
  /* We only check for ps_detach on MAIN because for SGLTE, only MAIN has PS */
  if(stack_idx != 0 && stack_idx != CMMSC_STACK_ID_ALL)
  {
    CM_MSG_HIGH_1("SC_SGLTE: stack_idx=%d",stack_idx);
    return;
  }

  /* Obtain an overlap between capability and preference for MAIN only (SGLTE) */
  stack_pointer = &(state_machine->stack[0]);

    ph_mode      = cmph_map_cm_mode_pref_to_sd_mode_pref(CMPH_SS_MODE_PREF(state_machine->asubs_id, ph_ptr));
  ph_band      = cmph_map_cm_band_pref_to_sd_band_pref(CMPH_SS_BAND_PREF(state_machine->asubs_id, ph_ptr));
  ph_lte_band  = CMPH_SS_LTE_BAND_PREF(state_machine->asubs_id, ph_ptr);
  ph_tds_band  = cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(CMPH_SS_TDS_BAND_PREF(state_machine->asubs_id, ph_ptr));

  old_mode = SD_GET_COMMON_MODE(ph_mode,stack_pointer->prev_registration_capability.mode);
  old_band = SD_GET_COMMON_BAND(ph_band,stack_pointer->prev_registration_capability.band);
  old_lte_band = SYS_LTE_BAND_MASK_COMMON(&ph_lte_band, &(stack_pointer->prev_registration_capability.lte_band));
  old_tds_band = SD_GET_COMMON_BAND(ph_tds_band, stack_pointer->prev_registration_capability.tds_band);

  new_mode = SD_GET_COMMON_MODE(ph_mode,stack_pointer->capablity.mode);
  new_band = SD_GET_COMMON_BAND(ph_band,stack_pointer->capablity.band);
  new_lte_band = SYS_LTE_BAND_MASK_COMMON(&ph_lte_band, &(stack_pointer->capablity.lte_band));
  new_tds_band = SD_GET_COMMON_BAND(ph_tds_band, stack_pointer->capablity.tds_band);

  CM_MSG_HIGH_3("ph_mode %d old mode 0x%x, new mode 0x%x",ph_mode, old_mode, new_mode);

  CM_MSG_MED_4("old bands(MSB) (LSB): cgw 0x%08x %08x,, tds 0x%08x %08x",
                                  QWORD_HIGH(old_band),
                                  QWORD_LOW(old_band),
                                  QWORD_HIGH(old_tds_band),
                                  QWORD_LOW(old_tds_band));
  cm_print_lte_band_mask(old_lte_band);


  CM_MSG_MED_4("new bands(MSB)(LSB): cgw 0x%08x %08x, tds 0x%08x %08x",
                                  QWORD_HIGH(new_band),
                                  QWORD_LOW(new_band),
                                  QWORD_HIGH(new_tds_band),
                                  QWORD_LOW(new_tds_band));
  cm_print_lte_band_mask(new_lte_band);

  /* If is_force_reg_pending flag is turned on, we detects LTE bands change
  as a local change to avoid the Band-39 limitation in SGLTE.
  With this local change, we don't want to UE to perform DETACH because ue-capability is not really changing.
  This is implemented per NAS requirement.

  Exceptions are:
  In scenario where B39 is removed due to B39 requirement in SGLTE,
  and Band-39 is the only band that support, we have to trigger PS_DETACH.
  Same for TDS.
  */
  if (state_machine->stack_common_info.is_force_reg_pending)
  {
     /* When old LTE band is not changing from or to NONE */
     if ( !(!SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&old_lte_band , &new_lte_band)
             &&
            (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&new_lte_band) ||
             SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&old_lte_band))
            )
         )
     {
        return;
     }
     /* When old TDS band is not changing from or to NONE */
     if ( !( old_tds_band != new_tds_band && (new_tds_band == 0 || old_tds_band == 0 )))
     {
        return;
     }
  }
  ph_ptr->is_ps_detach_required = 
  cmph_cmd_evaluate_is_ps_detach_required(
     state_machine->asubs_id,
     cmph_map_sd_mode_pref_to_cm_mode_pref(old_mode),
     cmph_map_sd_band_pref_to_cm_band_pref(old_band),
     old_lte_band,
     cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(old_tds_band),
     cmph_map_sd_mode_pref_to_cm_mode_pref(new_mode),
     cmph_map_sd_band_pref_to_cm_band_pref(new_band),
     new_lte_band,
     cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(new_tds_band) );

  return;
}

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_ss_to_stack_idx

DESCRIPTION
  The function maps cm ss to stack index.

DEPENDENCIES
  None

RETURN VALUE
 int

SIDE EFFECTS
  None

===========================================================================*/
int cmmsc_sglte_mapping_ss_to_stack_idx(cm_ss_e_type ss)
{
  switch( ss )
  {
    case CM_SS_MAIN:
    case CM_SS_HYBR_1:
      return 0;

    case CM_SS_HYBR_2:
      return 1;

    case CM_SS_MAX:
      return CMMSC_STACK_ID_ALL;

    default:
      return CMMSC_STACK_ID_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_stack_id_to_ss

DESCRIPTION
  The function maps stack index to cm ss.

DEPENDENCIES
  None

RETURN VALUE
  cm_ss_e_type

SIDE EFFECTS
  None

===========================================================================*/
cm_ss_e_type  cmmsc_sglte_mapping_stack_id_to_ss(int stack_idx)
{
  switch( stack_idx )
  {
    case 0:
      return CM_SS_MAIN;

    case 1:
      return CM_SS_HYBR_2;

    #ifdef FEATURE_MMODE_TRIPLE_SIM
      case 2:
        return CM_SS_HYBR_3;
    #endif

    default:
      CM_ERR_1("MSC: Invalid stack_idx",stack_idx);
      return CM_SS_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_stack_id_to_sd_ss

DESCRIPTION
  The function maps stack index to sd ss.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
sd_ss_e_type cmmsc_sglte_mapping_stack_id_to_sd_ss(int stack_idx)
{
  switch( stack_idx )
  {
    case 0:
      return SD_SS_MAIN;

    case 1:
      return SD_SS_HYBR_2;

    #ifdef FEATURE_MMODE_TRIPLE_SIM
        case 2:
          return SD_SS_HYBR_3;
    #endif

    default:
      CM_ERR_1("MSC: Invalid stack_idx",stack_idx);
      return SD_SS_MAX;
  }
}

