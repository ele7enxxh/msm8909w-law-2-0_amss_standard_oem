/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

FILES STRUCTURE

  *** PLEASE NOTE BEFORE MAKING MODIFICATIONS ***

  This file is organized in below structures:
  Section 1: Includes files
  Section 2: Static/Local Variables
  Section 3: Static Function Forward Declaration
  Section 4: SIM Coordinator Core-Functions (Init, get_ptr, find_routes, etc.)
  Section 5: SIM Coordinator Process Functions (subsc, oprt_mode, pref_sys_chgd)
  Section 6: Action Functions (route request, subsc_chgd, etc.)
  Section 7: Utilities/Helper Functions (including static functions, get/set)

  The section are arranged in the logical order, increased by frequency-of-change.
  Means the last section (section 7) are most likely to be updated/added.

  *** PLEASE NOTE BEFORE MAKING MODIFICATIONS ***


Copyright (c) 2013 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsimcoord.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/7/2013   cl     DSDX/TSTX Migration
07/09/1413  cl     Initial Release for MultiSim Framework
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "cmtaski.h"

#include "cmph.h"     /* Interface to CM ph functions */
#include "cmcsfbcall.h"
#ifdef CM_GW_SUPPORTED
#include "cmwcall.h"
#endif
#include "cmmsc.h"
#include "cmmsc_sglte.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
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


static cmmsimc_state_s_type   sim_coord_state;

/* Routing Table - SGLTE
** EVENT,
** STATE for stack1, 2, ...
** ACTION for stack 1, 2, ... (order matters)
** NEW STATE for stack 1, 2, ...
*/
static cmmsimc_correlation_table_entry_s_type correlation_table[] =
{
  /* ------------------------------- SINGLE SIM -------------------------------- */
   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_SINGLE_SIM,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE} //NULL or INACTIVE?
   },

   {CMMSIMC_EVENT_OFFLINE,                CMMSIMC_COORDINATION_SINGLE_SIM,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE} //NULL or INACTIVE?
   },

   { CMMSIMC_EVENT_SUB,                   CMMSIMC_COORDINATION_SINGLE_SIM,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE} //NULL or INACTIVE?
   },

   /* Catch all - Single SIM*/
   {CMMSIMC_EVENT_ANY,                    CMMSIMC_COORDINATION_SINGLE_SIM,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_NO_CHG,                 CMMSIMC_SIM_STATE_NO_CHG}
   },
   /* ------------------------------ SINGLE SIM END ------------------------------ */

   /* -------------------------------- MULTI-SIM --------------------------------- */

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL}

   },
   /* 5 */
   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_SINGLE_SIM1,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_NO_CHG,                 CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_SINGLE_SIM2,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_SINGLE_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_NO_CHG,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   /* 10 */

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_DUAL_SIM2_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_XSIM_TRIPLE_STANDBY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   /* Catch all */
   {CMMSIMC_EVENT_SUB,                    CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_ACT_SUBS_CHANGE},         {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_NO_CHG,                 CMMSIMC_SIM_STATE_NO_CHG}
   },

   /* ----------------------------------------------------------------------------- */
   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_SINGLE_SIM1,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ON},            {1,CMMSIMC_SIM_ACT_OFF},                 {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE}
   },

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_SINGLE_SIM2,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_OFF},            {1,CMMSIMC_SIM_ACT_ON},                 {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_INACTIVE,          CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_INACTIVE}
   },

   /* 15 */

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_SINGLE_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_OFF},            {1,CMMSIMC_SIM_ACT_OFF},                 {2,CMMSIMC_SIM_ACT_ON}},
    {CMMSIMC_SIM_STATE_INACTIVE,          CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ON},            {1,CMMSIMC_SIM_ACT_ON},                 {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_INACTIVE}
   },

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ON},            {1,CMMSIMC_SIM_ACT_OFF},                 {2,CMMSIMC_SIM_ACT_ON}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_DUAL_SIM2_SIM3,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_OFF},            {1,CMMSIMC_SIM_ACT_ON},                 {2,CMMSIMC_SIM_ACT_ON}},
    {CMMSIMC_SIM_STATE_INACTIVE,          CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_STANDBY_PREF_CHGD,      CMMSIMC_COORDINATION_XSIM_TRIPLE_STANDBY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                    CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ON},            {1,CMMSIMC_SIM_ACT_ON},                 {2,CMMSIMC_SIM_ACT_ON}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_ACTIVE}
   },

   /* ----------------------------------------------------------------------------- */

   /* 20 */
   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_NONE,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                 {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_INACTIVE,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE}
   },

   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_XSIM_SINGLE_SIM1,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_OFF},                  {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_INACTIVE}
   },

   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_XSIM_SINGLE_SIM2,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_OFF},             {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},        {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_INACTIVE,          CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_INACTIVE}
   },

   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_XSIM_SINGLE_SIM3,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_OFF},             {1,CMMSIMC_SIM_ACT_OFF},                  {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_INACTIVE,          CMMSIMC_SIM_STATE_INACTIVE,               CMMSIMC_SIM_STATE_ACTIVE}
   },

   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},        {2,CMMSIMC_SIM_ACT_OFF}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                 CMMSIMC_SIM_STATE_INACTIVE}
   },

   /* 25 */
   {CMMSIMC_EVENT_ONLINE,                 CMMSIMC_COORDINATION_XSIM_TRIPLE_STANDBY,
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                  CMMSIMC_SIM_STATE_NULL},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                CMMSIMC_SIM_STATE_ACTIVE}
   },

   /* ----------------------------------------------------------------------------- */

   {CMMSIMC_EVENT_OFFLINE,                CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NULL,              CMMSIMC_SIM_STATE_NULL,                   CMMSIMC_SIM_STATE_NULL}
   },


   /* ----------------------------------------------------------------------------- */

   {CMMSIMC_EVENT_PREF_SYS_SIM1,          CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_PREF_SYS_SIM2,          CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_NONE},            {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_PREF_SYS_SIM3,          CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_NONE},            {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   /* ----------------------------------------------------------------------------- */
   /* 30 */
   {CMMSIMC_EVENT_SRV_SIM1,               CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_WAKE_UP},             {2,CMMSIMC_SIM_ACT_WAKE_UP}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SRV_SIM2,               CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_WAKE_UP},         {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_WAKE_UP}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SRV_SIM3,               CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_WAKE_UP},         {1,CMMSIMC_SIM_ACT_WAKE_UP},             {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   /* ----------------------------------------------------------------------------- */

   {CMMSIMC_EVENT_CALL_SIM2,              CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_PEER_SUBS_CALL},  {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

/* ----------------------------------------------------------------------------- */

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                CMMSIMC_SIM_STATE_ACTIVE},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ACTIVE,                CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ACTIVE,                CMMSIMC_SIM_STATE_ACTIVE},
    {{0,CMMSIMC_SIM_ACT_NONE},            {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},       {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ACTIVE},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ACTIVE,            CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_ROUTE_REQUEST},   {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   },

  {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
   {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ACTIVE,                CMMSIMC_SIM_STATE_ANY},
   {{0,CMMSIMC_SIM_ACT_NONE},           {1,CMMSIMC_SIM_ACT_ROUTE_REQUEST},        {2,CMMSIMC_SIM_ACT_NONE}},
   {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
  },

  {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
   {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ACTIVE},
   {{0,CMMSIMC_SIM_ACT_NONE},             {1,CMMSIMC_SIM_ACT_NONE},               {2,CMMSIMC_SIM_ACT_ROUTE_REQUEST}},
   {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
  },

   {CMMSIMC_EVENT_SUSPEND_RESUME,         CMMSIMC_COORDINATION_ANY,
    {CMMSIMC_SIM_STATE_ANY,               CMMSIMC_SIM_STATE_ANY,                   CMMSIMC_SIM_STATE_ANY},
    {{0,CMMSIMC_SIM_ACT_NONE},            {1,CMMSIMC_SIM_ACT_NONE},                {2,CMMSIMC_SIM_ACT_NONE}},
    {CMMSIMC_SIM_STATE_NO_CHG,            CMMSIMC_SIM_STATE_NO_CHG,                CMMSIMC_SIM_STATE_NO_CHG}
   }

   /* ------------------------------ MULTI-SIM END -------------------------------- */
};

/*===========================================================================

            FUNCTION DECLARATIONS AND IMPLEMENTATION

This section contains function declaration and implementation for this
feature.

===========================================================================*/

static void cmmsimc_send_mmoc_subsc_chgd
(
        mmoc_subsc_chg_e_type                chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                prot_subsc_chg,

        boolean                              hybr_gw_subs_chg,

        boolean                              hybr3_gw_subs_chg,


        byte                                 nam,

        const mmoc_subs_data_s_type          *cdma_subs_ptr,
        /* CDMA subscription info */

        const mmoc_subs_data_s_type          *gw_subs_ptr,
           /* GW subscription info */

        const mmoc_subs_data_s_type          *gw_hybr_subs_ptr,
           /* GW HYBR subscription info */

       const mmoc_subs_data_s_type          *gw3_hybr_subs_ptr,

       uint8                                 active_subs

);

static int cmmsimc_mapping_asubs_id_to_sim_idx(
  sys_modem_as_id_e_type asubs_id
);
static int cmmsimc_mapping_ss_to_sim_idx(
  cm_ss_e_type ss
);

/*===========================================================================

FUNCTION cmmsimc_print_simc_info

DESCRIPTION
This function prints the sim coordinator info.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_simc_info(void);

/*===========================================================================

FUNCTION cmmsimc_print_corr_table_info

DESCRIPTION
This function prints the correlation table information.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_corr_table_info(
  int                   correlation_tbl_idx,
  cmmsimc_event_e_type  event
);

/*===========================================================================

FUNCTION cmmsimc_print_sub_feature_mode

DESCRIPTION
This function prints sub feature mode information.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_sub_feature_mode(void);

/*===========================================================================

FUNCTION cmmsimc_get_param_base_per_ss

DESCRIPTION
This function updates the param base base on the specified ss

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void cmmsimc_get_param_base_per_ss (
  cm_ss_e_type             ss,
  cmmsc_param_base_s_type  *param
);


/****************************************************************************
**    Function definitions
****************************************************************************/

/* Static function */
static void cmmsimc_sim_init( void )
{
  size_t i = 0;
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();

  for(i=0; i < cmpmprx_get_num_of_sims(); i++)
  {
    coordinator->sim[i].state = CMMSIMC_SIM_STATE_NULL;
    if (i == 0)
    {
      coordinator->sim[i].capability.mode = SD_SS_MODE_PREF_ALL_MODES;
    }
    else
    {
      coordinator->sim[i].capability.mode = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_remove_mode_pref_components(
                                             cmph_map_sd_mode_pref_to_cm_mode_pref(SD_SS_MODE_PREF_ALL_MODES),
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA));
    }
  }
}

static void cmmsimc_msc_state_machine_init( void )
{
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();

  if(cmph_is_feature_mode_1x_sxlte() )
  {
    /* Dynamically allocate state machine for SVLTE */
    coordinator->sim[0].msc_machine = (cmmsc_state_machine_s_type *)modem_mem_alloc(
                   sizeof(cmmsc_state_machine_s_type),MODEM_MEM_CLIENT_MMODE);

    if (coordinator->sim[0].msc_machine)
    {
      memset(coordinator->sim[0].msc_machine, 0, sizeof(cmmsc_state_machine_s_type));
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* initialize state machine and setting routing table */
    cmmsc_state_machine_init(coordinator->sim[0].msc_machine);
    coordinator->sim[0].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
    coordinator->sim[0].msc_machine->asubs_id  = SYS_MODEM_AS_ID_1;
  }
  else if(cmph_is_feature_mode_sglte() )
  {
    /* Dynamically allocate state machine for SGLTE */
    coordinator->sim[0].msc_machine = (cmmsc_state_machine_s_type *)modem_mem_alloc(
                   sizeof(cmmsc_state_machine_s_type),MODEM_MEM_CLIENT_MMODE);

    if (coordinator->sim[0].msc_machine)
    {
      memset(coordinator->sim[0].msc_machine, 0, sizeof(cmmsc_state_machine_s_type));
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* initialize state machine and setting routing table */
    cmmsc_state_machine_init(coordinator->sim[0].msc_machine);
    coordinator->sim[0].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_SGLTE;
    coordinator->sim[0].msc_machine->asubs_id = SYS_MODEM_AS_ID_1;
  }
  /* DSDS/TSTS feature */
  else if(cmph_is_msim())
  {
    size_t i = 0;

    for(i = 0; i < cmpmprx_get_num_of_sims(); i++)
    {
      /* Dynamically allocate state machine for sub-1 */
      coordinator->sim[i].msc_machine = (cmmsc_state_machine_s_type *)modem_mem_alloc(
                     sizeof(cmmsc_state_machine_s_type),MODEM_MEM_CLIENT_MMODE);

      if (coordinator->sim[i].msc_machine)
      {
        memset(coordinator->sim[i].msc_machine, 0, sizeof(cmmsc_state_machine_s_type));
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }

      coordinator->sim[i].msc_machine->asubs_id = (sys_modem_as_id_e_type)(i);
      /* initialize state machine and setting routing table */
      cmmsc_state_machine_init(coordinator->sim[i].msc_machine);
      if(cmph_is_subs_feature_mode_sglte((sys_modem_as_id_e_type)i))
      {
        coordinator->sim[i].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_SGLTE;
      }
      else
      {
        coordinator->sim[i].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
      }
    }

  }
  /* SGLTE+G NEED CHANGE */
  /* SVLTE+G NEED CHANGE */
  /* Legacy */
  else
  {
    /* Dynamically allocate state machine for SVLTE */
    coordinator->sim[0].msc_machine = (cmmsc_state_machine_s_type *)modem_mem_alloc(
                   sizeof(cmmsc_state_machine_s_type),MODEM_MEM_CLIENT_MMODE);

    if (coordinator->sim[0].msc_machine)
    {
      memset(coordinator->sim[0].msc_machine, 0, sizeof(cmmsc_state_machine_s_type));
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* initialize state machine and setting routing table */
    cmmsc_state_machine_init(coordinator->sim[0].msc_machine);
    coordinator->sim[0].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
    coordinator->sim[0].msc_machine->asubs_id  = SYS_MODEM_AS_ID_1;
  }
}

void cmsimcoord_reinit(void)
{
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();
  uint32 i;

  for(i = 0; i < cmpmprx_get_num_of_sims(); i++)
  {
    /* initialize state machine and setting routing table */
    cmmsc_state_machine_init(coordinator->sim[i].msc_machine);
    if(cmph_is_subs_feature_mode_sglte((sys_modem_as_id_e_type)i))
    {
      coordinator->sim[i].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_SGLTE;
    }
    else
    {
      coordinator->sim[i].msc_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
    }
  }

}

/*===========================================================================

FUNCTION cmmsimc_proc_cmd_oprt_mode_per_state_multi_sim

DESCRIPTION
  This function processes multi-sim state upon online

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmmsimc_proc_cmd_oprt_mode_per_state_multi_sim (void)
{
  cmph_s_type          *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("MSIMC: oprt_chgd: ph_ptr->oprt_mode=%d, curr is_hybr_gw_operational=%d, ph_ptr->active_subs=%d", 
                   ph_ptr->oprt_mode, 
                   cmss_ptr()->info.is_hybr_gw_operational , 
                   ph_ptr->active_subs);            

  cmss_ptr()->info.is_main_operational =   FALSE;
  cmss_ptr()->info.is_hybr_gw_operational = FALSE;
  cmss_ptr()->info.is_gw3_operational = FALSE;

  if(ph_ptr->active_subs & BM(ph_ptr->main_stack_info.asubs_id))
  {
    cmss_ptr()->info.is_main_operational =   TRUE;
  }

  if((ph_ptr->active_subs & BM(ph_ptr->hybr_2_stack_info.asubs_id)) &&
  	  !cmph_is_sxlte())
  {
   cmss_ptr()->info.is_hybr_gw_operational = TRUE;
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(ph_ptr->active_subs & BM(ph_ptr->hybr_3_stack_info.asubs_id))
  {
    cmss_ptr()->info.is_gw3_operational =   TRUE;
  }
  
  cmss_ptr()->info.gw3_asubs_id = ph_ptr->hybr_3_stack_info.asubs_id;
  #endif

     /* CR460366: Removed the condition based on subscritption availablility
           irrespective of subscription availablity on main , update asubid in cmss
           so that if none of the subscription is available , it should atleast
           contain asub_id as 1 instead of -1(NONE: invalid value ).
       */
  cmss_ptr()->info.asubs_id    = ph_ptr->main_stack_info.asubs_id;

  cmss_ptr()->info.gw_asubs_id = ph_ptr->hybr_2_stack_info.asubs_id;

}


/*===========================================================================

FUNCTION cmsimcoord_state_ptr

DESCRIPTION
  This function returns the SIM Coordinator pointer

DEPENDENCIES
  None.

RETURN VALUE
  cmmsc_state_machine_s_type*

===========================================================================*/
cmmsimc_state_s_type *cmmsimc_state_ptr( void )
{
  return &sim_coord_state;
}


/*===========================================================================

FUNCTION cmmsimc_route_table_ptr

DESCRIPTION
  This function returns the a pointer to the entry of the Correlation Table

DEPENDENCIES
  None.

RETURN VALUE
  cmmsimc_correlation_table_entry_s_type*

===========================================================================*/
cmmsimc_correlation_table_entry_s_type* cmmsimc_correlation_table_ptr( void )
{
  return (cmmsimc_correlation_table_entry_s_type*)&correlation_table;
}

/*===========================================================================

FUNCTION route_table_entry_ptr

DESCRIPTION
  This function returns the pointer to an index of the correlation table

DEPENDENCIES
  None.

RETURN VALUE
  cmmsc_routing_table_entry_s_type*

===========================================================================*/
cmmsimc_correlation_table_entry_s_type *correlation_table_entry_ptr(int index)
{
  cmmsimc_correlation_table_entry_s_type *correlation_table
    = cmmsimc_correlation_table_ptr();

  return &correlation_table[index];
}

/*===========================================================================

FUNCTION cmmsimc_correlation_table_size

DESCRIPTION
  This function returns the size of the correlation table

DEPENDENCIES
  None.

RETURN VALUE
  int

===========================================================================*/
int cmmsimc_correlation_table_size (void)
{
  return ARR_SIZE(correlation_table);
}



/*===========================================================================

FUNCTION cmmsimc_state_init

DESCRIPTION
  This function should be called to initialize the whole SIM Coordinator

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_state_init( void )
{
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();

  if(coordinator == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* initialize the coordinator */
  coordinator->standby_pref = SYS_MODEM_DS_PREF_NONE;
  cmmsimc_sim_init();
  cmmsimc_msc_state_machine_init();
  coordinator->active_subs = 0;
  coordinator->device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
  coordinator->esr_state = CMSIMC_ESR_NONE;
  coordinator->device_mode = cmpmprx_get_device_mode();

  /* Initialize the coordination for single SIM */
  if(cmph_is_ssim())
  {
    coordinator->coordination = CMMSIMC_COORDINATION_SINGLE_SIM;
    coordinator->active_subs  = SYS_MODEM_AS_ID_1_MASK;
  }

}

cmmsc_state_machine_s_type     *cmmsimc_state_machine_ptr(
  sys_modem_as_id_e_type        as_id
)
{
  size_t i = 0;
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();
  if(coordinator == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  for(i = 0; i < cmpmprx_get_num_of_sims(); i++)
  {
    if(coordinator->sim[i].msc_machine->asubs_id == as_id)
    {
      return coordinator->sim[i].msc_machine;
    }
  }
  sys_err_fatal_invalid_value_exception((int)as_id);
  return NULL;
}

cmmsc_state_machine_s_type     *cmmsimc_state_machine_ptr_per_sim_idx(
  size_t                        sim_idx
)
{
  cmmsimc_state_s_type *coordinator = cmmsimc_state_ptr();
  if(coordinator == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return NULL;
  }

  if(!(sim_idx < CMMSIMC_SIM_NUM))
  {
    sys_err_fatal_invalid_value_exception((int)sim_idx);
  }

  if(coordinator->sim[sim_idx].msc_machine != NULL)
  {
    return coordinator->sim[sim_idx].msc_machine;
  }

  sys_err_fatal_invalid_value_exception((int)sim_idx);
  return NULL;
}



/*===========================================================================

FUNCTION cmmsimc_compare_sim_states

DESCRIPTION
  This function compares two sim states. SIM States are enums and
  not always the same to be 'equal'.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if equals
===========================================================================*/
boolean cmmsimc_compare_sim_states(
   cmmsimc_sim_state_e_type   state1,

   cmmsimc_sim_state_e_type   state2
)
{
  if(state1 == state2)
  {
    return TRUE;
  }

  /* If either of mode says "ANY", then they're matched */
  if(state1 == CMMSIMC_SIM_STATE_ANY ||
     state2 == CMMSIMC_SIM_STATE_ANY )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmmsimc_compare_sim_events

DESCRIPTION
  This function compares two sim events. SIM events are enums and
  not always the same to be 'equal'.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if equals
===========================================================================*/
boolean cmmsc_compare_sim_events(
   cmmsimc_event_e_type   event1,

   cmmsimc_event_e_type   event2
)
{
  if(event1 == event2)
  {
    return TRUE;
  }

  /* If either of mode says "ANY", then they're matched */
  if(event1 == CMMSIMC_EVENT_ANY ||
     event2 == CMMSIMC_EVENT_ANY )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmmsc_compare_coordinations

DESCRIPTION
  This function compares two sim coordinations setups.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if equals
===========================================================================*/
boolean cmmsc_compare_coordinations(
   cmmsimc_coordination_e_type   coordination1,

   cmmsimc_coordination_e_type   coordination2
)
{
  if(coordination1 == coordination2)
  {
    return TRUE;
  }

  /* If either of mode says "ANY", then they're matched */
  if(coordination1 == CMMSIMC_COORDINATION_ANY ||
     coordination2 == CMMSIMC_COORDINATION_ANY )
  {
    return TRUE;
  }
  return FALSE;
}

/* Finding Correlation Index in the Correlation Table based on current states &
** events
*/
int cmmsimc_find_correlation_idx( cmmsimc_event_e_type event )
{
  cmmsimc_state_s_type                   *cmmsimc_ptr = cmmsimc_state_ptr();
  cmmsimc_correlation_table_entry_s_type *corr_table    =
                                                  cmmsimc_correlation_table_ptr();
  int                                max_entry     =
                                                  cmmsimc_correlation_table_size();
  size_t                             col_idx;
  int                                corr_tbl_idx;
  boolean                            is_match      = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate input parameter */
  if(event == CMMSIMC_EVENT_NONE)
  {
    return CMMSIMC_CORRELATION_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First- take the event and check for action */
  for(corr_tbl_idx = 0; corr_tbl_idx < max_entry; corr_tbl_idx++)
  {
    /* Check for event && current mode. */
    if(cmmsc_compare_sim_events(corr_table[corr_tbl_idx].event, event) &&
       cmmsc_compare_coordinations(cmmsimc_ptr->coordination,
                                   corr_table[corr_tbl_idx].coordination)
      )
    {
      /* If event matched then check for each individual states */
      for(col_idx = 0; col_idx < cmpmprx_get_num_of_sims(); col_idx++)
      {
        if(!cmmsimc_compare_sim_states(cmmsimc_ptr->sim[col_idx].state,
                                      corr_table[corr_tbl_idx].state[col_idx]))
        {
          break;
        }
      }
      /* all states collumns matched */
      if(col_idx == cmpmprx_get_num_of_sims())
      {
        is_match = TRUE;
        break;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If match, take action for each stack (this gonna be a different function) */
  if(is_match)
  {
    return corr_tbl_idx;
  }

  return CMMSIMC_CORRELATION_NONE;
}


/*
** Entry point for Multi-Sim Controller. It will find:
** 1. Routing index in the Correlation Table
** 2. Take action upon that routing index
*/
void cmmsimc_sim_coordinate(cmmsc_param_base_s_type * event_package )
{
  int correlation_tbl_idx     = CMMSIMC_CORRELATION_NONE;
  cmmsimc_event_e_type  event = event_package->event;

  correlation_tbl_idx = cmmsimc_find_correlation_idx(event);

  /* Print correlation table info */
  cmmsimc_print_corr_table_info(correlation_tbl_idx, event);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  //cmdbg_add_to_dbg_buffer(CM_BUFF_SIMC_ROUTING_ENTRY_TYPE, 0, ((void*)&correlation_tbl_idx));
  #endif
  
  /* Execute sim action */
  if (correlation_tbl_idx != CMMSIMC_CORRELATION_NONE)
  {
    /* Perform the action */
    cmmsimc_execute_action(correlation_tbl_idx, event_package);

    /* Update the state */
    cmmsimc_update_state(correlation_tbl_idx);
  }
}

/*===========================================================================

FUNCTION cmmsimc_execute_action

DESCRIPTION
  This function takes action based on the matched index action in
  Correlation Table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_execute_action(
     int                        correlation_tbl_idx,
     cmmsc_param_base_s_type   *event_package
)
{
  size_t act_idx;
  cmmsimc_correlation_table_entry_s_type * corr_table_entry =
                            correlation_table_entry_ptr(correlation_tbl_idx);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Search through each act_idx and perform the relative actions */
  for(act_idx=0; act_idx < cmpmprx_get_num_of_sims() && act_idx < MAX_SIMS; act_idx++)
  {
    int sim_idx = corr_table_entry->action[act_idx].sim_idx;
    event_package->state_machine = cmmsimc_state_machine_ptr_per_sim_idx(sim_idx);

    switch( corr_table_entry->action[act_idx].action)
    {
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       case CMMSIMC_SIM_ACT_ROUTE_REQUEST:
         cmmsimc_act_route_request(event_package);
         break;

       case CMMSIMC_SIM_ACT_POWER_DOWN:
         cmmsimc_act_power_down(
            (cmmsc_param_oprt_mode_s_type *)event_package);
         break;

       case CMMSIMC_ACT_SUBS_CHANGE:
       {
         /* Send sub command to MSC and then process sub-command
              - CMMSIMC: 1. set subscription capability per stack (cmmsc_proc_cmd_subsc_chgd_set_capability)
              - CMMSIMC: 2. send subscription on MAIN (SIM-1) to MMOC
              - CMMSIMC: 3. Trigger CMMSC.
            Key note: we don't want to trigger CMMSC Routing Table before CMMSIMC because
            we might take action on CMMSC, which we don't want to take before subscription
            are sent.
         */
         cmmsc_param_subs_chgd_s_type *subs_param =
                                 (cmmsc_param_subs_chgd_s_type *)event_package;
         cmmsc_proc_cmd_subsc_chgd_set_capability(subs_param);
         cmmsimc_act_subs_change(event_package);
         cmmsc_proc_cmd_subsc_chgd(subs_param);
         break;
       }

       case CMMSIMC_SIM_ACT_WAKE_UP:
         cmmsimc_act_wake_up(sim_idx,
             (cmmsc_param_srv_ind_s_type*)event_package);
         break;

       case CMMSIMC_SIM_ACT_ON:
       case CMMSIMC_SIM_ACT_OFF:
       {
         cmmsimc_act_on_off(corr_table_entry->action[act_idx].action,
                            event_package);
         break;
       }

       case CMMSIMC_SIM_ACT_PEER_SUBS_CALL:
        cmsimc_act_peer_subs_call((cmmsc_param_call_ind_s_type*)event_package);
        break;

       case CMMSIMC_SIM_ACT_MAX:
       default:
        break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}


/*===========================================================================

FUNCTION cmmsimc_update_state

DESCRIPTION
  This function update the current states to a new states in given
  correlation table index.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_update_state( int corr_table_idx )
{
  size_t                             sim_idx;
  cmmsimc_correlation_table_entry_s_type *corr_table_entry =
                                          correlation_table_entry_ptr(corr_table_idx);
  cmmsimc_state_s_type                   *cmmsimc_ptr      =
                                          cmmsimc_state_ptr();

  for(sim_idx = 0; sim_idx < cmpmprx_get_num_of_sims(); sim_idx++)
  {
    if( corr_table_entry->new_state[sim_idx] != CMMSIMC_SIM_STATE_NO_CHG )
    {
      CM_MSG_HIGH_3("MSIMC: updating sim %d, from state %d to %d",sim_idx,
                  cmmsimc_ptr->sim[sim_idx].state,
                  corr_table_entry->new_state[sim_idx]);
      /* Update stack state */
      cmmsimc_ptr->sim[sim_idx].state = corr_table_entry->new_state[sim_idx];
    }
  }
}

/*===========================================================================

FUNCTION cmmsimc_print_simc_info

DESCRIPTION
This function prints the sim coordinator info.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_simc_info(void)
{
  cmmsimc_state_s_type *cmmsimc_ptr = cmmsimc_state_ptr();
  size_t sim_idx = 0;

  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_dsda() || cmph_is_dsds())
  {
     CM_MSG_HIGH_4("MSIMC: info DSDX coordination=%d, standby_pref=%d, sim1_state=%d, sim2_state=%d",
                 cmmsimc_ptr->coordination,
                 cmmsimc_ptr->standby_pref,
                 cmmsimc_ptr->sim[0].state,
                 cmmsimc_ptr->sim[1].state);
  }
  else
  #endif
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if (cmph_is_tsts())
  {
     CM_MSG_HIGH_5("MSIMC: info DSDX coordination=%d, standby_pref=%d, sim1_state=%d, sim2_state=%d, sim3_state=%d",
                 cmmsimc_ptr->coordination,
                 cmmsimc_ptr->standby_pref,
                 cmmsimc_ptr->sim[0].state,
                 cmmsimc_ptr->sim[1].state,
                 cmmsimc_ptr->sim[2].state);
  }
  else
  #endif
  {
    CM_MSG_HIGH_3("MSIMC: info SSIM coordination=%d, standby_pref=%d, sim_state=%d",
               cmmsimc_ptr->coordination,
               cmmsimc_ptr->standby_pref,
               cmmsimc_ptr->sim[0].state);
  }
}

/*===========================================================================

FUNCTION cmmsimc_print_corr_table_info

DESCRIPTION
This function prints the correlation table information.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_corr_table_info(
  int                   correlation_tbl_idx,
  cmmsimc_event_e_type  event
)
{
  if (correlation_tbl_idx != CMMSIMC_CORRELATION_NONE)
  {
    cmmsimc_correlation_table_entry_s_type * corr_table_entry =
                            correlation_table_entry_ptr(correlation_tbl_idx);
    size_t sim_idx = 0;

    /* Print the state, action, new state per sim */
    for(sim_idx=0; sim_idx < cmpmprx_get_num_of_sims(); sim_idx++)
    {
      if(corr_table_entry->action[sim_idx].action != CMMSIMC_SIM_ACT_NONE
          && corr_table_entry->action[sim_idx].action != CMMSIMC_SIM_ACT_ROUTE_REQUEST)
      {
        CM_MSG_HIGH_5("MSIMC: table event=%d, corr_tbl_idx=%d, table state=%d, action=%d, new_state=%d",
                     event, correlation_tbl_idx,
                     corr_table_entry->state[sim_idx],
                     corr_table_entry->action[sim_idx].action,
                     corr_table_entry->new_state[sim_idx]);
      }
    }
  }
}

/*===========================================================================

FUNCTION cmmsimc_print_sub_feature_mode

DESCRIPTION
This function prints sub feature mode information.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  cmmsimc_print_sub_feature_mode(void)
{
  if (cmpmprx_get_num_of_sims() > MAX_SIMS)
  {
    CM_ERR_FATAL_2("MSIMC: %d exceeds maximum supported num of sims %d",
                   cmpmprx_get_num_of_sims(), MAX_SIMS );
    return;
  }
  else
  {
    size_t sim_idx;

    /* Print the state, action, new state per sim */
    for(sim_idx=0; sim_idx < cmpmprx_get_num_of_sims(); sim_idx++)
    {
      sys_modem_as_id_e_type  as_id = (sys_modem_as_id_e_type)sim_idx;
      CM_MSG_HIGH_2("MSIMC: as_id=%d, sub_fmode=%d",
                     as_id,
                     cmph_get_subs_feature_mode(as_id));
    }
  }
}

/*===========================================================================

FUNCTION cmmsimc_get_param_base_per_ss

DESCRIPTION
This function updates the param base base on the specified ss

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void cmmsimc_get_param_base_per_ss (
  cm_ss_e_type             ss,
  cmmsc_param_base_s_type  *param
)
{
  cmmsc_state_machine_s_type  *state_machine = NULL;
  sys_modem_as_id_e_type      asub_id = cmph_map_cm_ss_to_subs(ss);

  if (param == NULL)
  {
    CM_ERR_0("param null ptr");
    return;
  }

  state_machine = cmmsimc_state_machine_ptr(asub_id);
  param->state_machine = state_machine;
}

/*===========================================================================

  DESCRIPTION:
   active_subs has bits set for corresponding enum of sys_modem_as_id_e_type.
   Based on bits sets in it, standby pref is decided and set in SIMC.

===========================================================================*/

static void cmsimc_set_coordination(uint8 active_subs)
{
  cmmsimc_state_s_type *msimc = cmmsimc_state_ptr();

  switch(active_subs)
  {
    case SYS_MODEM_AS_ID_1_MASK:
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_SINGLE_SIM1;
      break;

    case SYS_MODEM_AS_ID_2_MASK:
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_SINGLE_SIM2;
      break;

    case SYS_MODEM_AS_ID_3_MASK:
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_SINGLE_SIM3;
      break;

    case (SYS_MODEM_AS_ID_1_MASK | SYS_MODEM_AS_ID_2_MASK):
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2;
      break;

    case (SYS_MODEM_AS_ID_1_MASK | SYS_MODEM_AS_ID_3_MASK):
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM3;
      break;

    case (SYS_MODEM_AS_ID_2_MASK | SYS_MODEM_AS_ID_3_MASK):
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_DUAL_SIM2_SIM3;
      break;

    case (SYS_MODEM_AS_ID_1_MASK | SYS_MODEM_AS_ID_2_MASK | SYS_MODEM_AS_ID_3_MASK):
      msimc->coordination = CMMSIMC_COORDINATION_XSIM_TRIPLE_STANDBY;
      break;

    default:
      msimc->coordination = CMMSIMC_COORDINATION_NONE;
      break;
  }

}

/*===========================================================================

FUNCTION cmmsimc_proc_cmd_oprt_mode_chgd

DESCRIPTION
This function handle Oprt_mode Change command at the SIM level

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                                    cmmsimc_proc_cmd_oprt_mode_chgd
(
  sys_oprt_mode_e_type                  oprt_mode
  /* New operating mode of the phone.*/
)
{
  cmmsimc_state_s_type             *sim_coord_state = cmmsimc_state_ptr();
  cmmsc_state_machine_s_type       *state_machine = NULL;
  cmmsimc_event_e_type              event = CMMSIMC_EVENT_NONE;
  cmmsc_param_oprt_mode_s_type      event_oprt_mode;
  cmph_s_type                      *ph_ptr = cmph_ptr();

  /* Print sub feature mode */
  cmmsimc_print_sub_feature_mode();

  /* Print oprt mode change */
  CM_MSG_HIGH_2("MSIMC: oprt_chgd: %d->%d", ph_ptr->oprt_mode, oprt_mode);

  /*1. Setting event & coordination */
  switch(oprt_mode)
  {
    case SYS_OPRT_MODE_ONLINE:
    case SYS_OPRT_MODE_PSEUDO_ONLINE:
    {
      cmsimc_set_coordination(sim_coord_state->active_subs);

      /* Generate event */
      event = CMMSIMC_EVENT_ONLINE;
      break;
    }

    /* going offline */
    default:
      cmsimc_set_coordination(sim_coord_state->active_subs);

      event = CMMSIMC_EVENT_OFFLINE;
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* mode pref mask - how do we updates the mode pref mask for each SIM? */
  if (cmph_is_msim())
  {
    size_t i;
    sim_coord_state->sim[0].capability.mode = SD_SS_MODE_PREF_ALL_MODES;
    for (i = 1; i < cmpmprx_get_num_of_sims(); i++)
    {
      sim_coord_state->sim[i].capability.mode = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_remove_mode_pref_components(
                                             cmph_map_sd_mode_pref_to_cm_mode_pref(SD_SS_MODE_PREF_ALL_MODES),
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA));
    }
  }

  CM_MSG_HIGH_6( "MSIMC: oprt_chgd: cdma sub %d gwl sub %d gw2 sub %d gw3 sub %d device_mode %d, ph mode_cap=0x%x",
                 ph_ptr->is_cdma_subscription_available,
                 ph_ptr->is_gwl_subscription_available,
                 ph_ptr->is_hybr_gw_subscription_available,
                 ph_ptr->is_hybr_gw3_subscription_available,
                 cm_get_device_mode(),
                 ph_ptr->mode_capability);
  
  if (cmph_is_sxlte())
  {
    CM_MSG_HIGH_5("SFMODE: oprt_chgd: sub0=%d, sub1=%d, pm sub0=%d max_cap sub0=0x%x, sub1=0x%x",
                   cmph_get_subs_feature_mode(SYS_MODEM_AS_ID_1),
                   cmph_get_subs_feature_mode(SYS_MODEM_AS_ID_2),
                   cmpmprx_get_pm_sfmode(SYS_MODEM_AS_ID_1),
                   cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1),
                   cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_2));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Setting "is_operational_flag" */
  /* If the phone is moving any operating mode other than ONLINE, then
  ** set the operational status accordingly
  */
  if( oprt_mode != SYS_OPRT_MODE_ONLINE &&
      oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
    )
  {
    cmss_ptr()->info.is_main_operational = FALSE;
    cmss_ptr()->info.is_hybr_gw_operational = FALSE;
    cmss_ptr()->info.is_gw3_operational = FALSE;
  }
  else
  {
    if (cmph_is_msim())
    {
      cmmsimc_proc_cmd_oprt_mode_per_state_multi_sim();
    }
    else
    {
      cmss_ptr()->info.is_main_operational = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Package the event */
  /* Multi-Sim coordinator needs to pass in:
  ** 1. the state_machine pointer
  ** 2. parameters of the command
  ** 3. AS_ID
  */
  event_oprt_mode.event = event;
  event_oprt_mode.state_machine = state_machine;
  event_oprt_mode.oprt_mode = oprt_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print sim coordinator info */
  cmmsimc_print_simc_info();

  /* Call coordination function */
  cmmsimc_sim_coordinate( (cmmsc_param_base_s_type *)&event_oprt_mode );

}


/*===========================================================================

FUNCTION cmmsimc_proc_cmd_pref_sys_chgd

DESCRIPTION
This function handle preference Change command at the SIM level

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_proc_cmd_pref_sys_chgd
(
  cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cmmsc_state_machine_s_type       *state_machine = NULL;
  cmmsimc_event_e_type              event = CMMSIMC_EVENT_PREF_SYS_SIM1;
  cmmsc_param_pref_sys_s_type       event_pref_sys;
  sd_ss_pref_reas_e_type            pref_reas = cmd->pref_info->pref_reas;
  cm_orig_q_s_type                 *ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );
  int                               sim_idx = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For multi-SIM, update multi-mode subscription */
  if (cmph_is_msim() && ph_orig_top_ptr)
  {
    cm_orig_q_s_type     *ph_orig_top_ptr2 = cmtask_orig_para_get_top( CM_SS_HYBR_2);
    cm_mode_pref_e_type   mode_pref_hybr2  = CM_MODE_PREF_NONE;
    cm_mode_pref_e_type   mode_pref_hybr3  = CM_MODE_PREF_NONE;

    cm_mode_pref_e_type   mode_pref_main = ph_orig_top_ptr->orig->orig_mode_pref;

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    cm_orig_q_s_type     *ph_orig_top_ptr3 = cmtask_orig_para_get_top( CM_SS_HYBR_3);
    if(ph_orig_top_ptr3 != NULL)
    {
      mode_pref_hybr3                        = ph_orig_top_ptr3->orig->orig_mode_pref;
    }  
    if (cmd->cm_ss == CM_SS_HYBR_3)
    {
      mode_pref_hybr3 = cmd->pref_info->mode_pref;
    }
    #endif
    
    if(ph_orig_top_ptr2 != NULL && !cmph_no_hybr2())
    {
      mode_pref_hybr2  = ph_orig_top_ptr2->orig->orig_mode_pref;
    } 
    if (cmd->cm_ss == CM_SS_HYBR_2)
    {
      mode_pref_hybr2 = cmd->pref_info->mode_pref;
    }
    
    /* If Main is changing mode preference, make sure we use new mode_pref */
    if (cmd->cm_ss == CM_SS_MAIN)
    {
      mode_pref_main = cmd->pref_info->mode_pref;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*2. Set Event */
  /* Map ss -> as_id as we're entering 'multi-SIM' */
  if (cmph_is_msim())
  {
    sim_idx = cmmsimc_mapping_ss_to_sim_idx(cmd->cm_ss);
    if (sim_idx == 0)
      event = CMMSIMC_EVENT_PREF_SYS_SIM1;
    else if (sim_idx == 1)
      event = CMMSIMC_EVENT_PREF_SYS_SIM2;
    else if (sim_idx == 2)
      event = CMMSIMC_EVENT_PREF_SYS_SIM3;
  }

  event_pref_sys.event = event;
  event_pref_sys.state_machine = state_machine;
  event_pref_sys.cmd = cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*4. Trigger Coordinator */
  cmmsimc_sim_coordinate( (cmmsc_param_base_s_type *)&event_pref_sys );

  /* Generate additional events to sim coord if needed if no active call pending */
  if(cmcall_find_active_cs_call_per_subs(cmph_map_cm_ss_to_subs(cmd->cm_ss)) == CM_CALL_ID_INVALID)
  {
    cmmsimc_proc_call_event(cmd->cm_ss,
                            CM_CALL_TYPE_UNKNOWN, CM_CALL_EVENT_END);
  }

  /* to make sure that for all error conditions also pref_info memory gets free */
  if(cmd->pref_info != NULL)
  {
    cm_mem_free(cmd->pref_info);
  }
}

/*===========================================================================

FUNCTION cmmsimc_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function will pack the info and in turn call the function
  cmmsc_proc_cmd_subsc_chgd during processing based on which subscription.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_cmd_subsc_chgd
(

        mmoc_subsc_chg_e_type           chg_type,
             /* Subscription change type.
             */

        byte                            nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        cm_network_sel_mode_pref_e_type network_sel_mode_pref,
            /* The network sel mode associated with the selected NAM */

        cm_mode_pref_e_type             mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        cm_band_pref_e_type             band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type        lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        cm_band_pref_e_type             tds_band_pref,
           /* TD-SCDMA band preference that is associated with the selected NAM.
           */

        cm_prl_pref_e_type              prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        cm_roam_pref_e_type             roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        cm_hybr_pref_e_type             hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sys_plmn_id_s_type              plmn,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        cm_srv_domain_pref_e_type       srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        cm_gw_acq_order_pref_e_type     acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        cm_pref_s_type                 *hybr_2_pref_info_ptr,
           /* hybr2 preference info pointer.
           */

        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        cm_pref_s_type                  *hybr_3_pref_info_ptr,
           /* Roam preference that is associated with the selected NAM.
           */
        #endif

        boolean                         is_gwl_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_hybr_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_hybr_3_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available, false = subscription not available. */

        prot_subsc_chg_e_type           prot_subsc_chg,
           /* Enumeration of protocols that have a change in
           ** subscription available status */

        prot_subsc_chg_e_type           hybr_2_prot_subsc_chg,
           /* Enumeration of protocols that have a change in
              subscription available status */

        prot_subsc_chg_e_type           hybr_3_prot_subsc_chg
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Variable used for Multi-Sim Framework processing */
  cmmsimc_event_e_type              event;
  cmmsc_param_subs_chgd_s_type      event_subs;
  cmph_s_type                      *ph_ptr = cmph_ptr();
  cmmsimc_state_s_type             *sim_coord_state = cmmsimc_state_ptr();
  size_t                            i;

  /* Packed subscription info into array format */
  cm_pref_s_type                   *pref_info[MAX_SIMS];
  prot_subsc_chg_e_type             prot_sub_chg[MAX_SIMS];
  mmoc_subs_data_s_type            *sub_data[MAX_SUB_SESSION_TYPES];
  boolean                           sub_avail[MAX_SUB_SESSION_TYPES];

  /* Misc data */
    /* main info, which packs all preference for Main into 1 info-pointer */
  cm_pref_s_type                    main_info;
  size_t                            size_of_sub_avail = cmpmprx_get_num_of_sims()+1;
  size_t                            size_of_sub_chg = cmpmprx_get_num_of_sims();
  size_t                            size_of_pref_info_ptr = cmpmprx_get_num_of_sims()+1;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Validate params
   ** hybr_2_pref_info_ptr and hybr_3_pref_info_ptr should be null if num_of_sim = 1
   ** hybr_2_pref_info_ptr/hybr_3_pref_info_ptr should not be null for multi-sim.
   */
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  CM_ASSERT( ((hybr_2_pref_info_ptr == NULL && hybr_3_pref_info_ptr == NULL) &&
                cmph_is_ssim()
               )  ||
              ((hybr_2_pref_info_ptr != NULL || hybr_3_pref_info_ptr != NULL) &&
               cmph_is_msim())
             );
             
  if (((hybr_2_pref_info_ptr == NULL && hybr_3_pref_info_ptr == NULL) &&
       cmph_is_msim()
     )  ||
     ((hybr_2_pref_info_ptr != NULL || hybr_3_pref_info_ptr != NULL) &&
       cmph_is_ssim())
  )
  {
    CM_ERR_FATAL_3("MSIMC: hybr_2_pref_info_ptr=%d, hybr_3_pref_info_ptr=%d, feature_mode=%d",
                  hybr_2_pref_info_ptr,hybr_3_pref_info_ptr,cmph_ptr()->feature_mode);
  }
  #else
  CM_ASSERT( (hybr_2_pref_info_ptr == NULL && cmph_is_ssim()
               )  ||
              (hybr_2_pref_info_ptr != NULL && cmph_is_msim())
             );
             
  if ((hybr_2_pref_info_ptr == NULL && cmph_is_msim()
     )  ||
     (hybr_2_pref_info_ptr != NULL && cmph_is_ssim())
  )
  {
    CM_ERR_FATAL_2("MSIMC: hybr_2_pref_info_ptr=%d, feature_mode=%d",
                  hybr_2_pref_info_ptr, cmph_ptr()->feature_mode);
  }
  #endif
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Nothing has changed */
  if( prot_subsc_chg == PROT_SUBSC_CHG_NONE &&
    hybr_2_prot_subsc_chg == PROT_SUBSC_CHG_NONE  &&
    hybr_3_prot_subsc_chg == PROT_SUBSC_CHG_NONE)
  {
    CM_MSG_HIGH_0("Ignore due to no change in subscription status");
    return;
  }

  /* Subscription change shouldn't be sent to lower layers other than in ONLINE/PSEUDO_ONLINE,
  LPM,POWER OFF */
  if (!cm_is_valid_state_for_sub_proc(cmph_ptr()->true_oprt_mode))
  {
    CM_MSG_HIGH_1("Ignore subs change due to oprt_mode %d", cmph_ptr()->true_oprt_mode);
    return;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Pack pref info for sim0. This function only called for SIM-0 because SIM-2
  ** preference has already been packed in pointer hybr_2_pref_info_ptr. */
  cmmsc_pack_pref_info(
                         network_sel_mode_pref,
                         mode_pref,
                         band_pref,
                         lte_band_pref,
                         tds_band_pref,
                         prl_pref,
                         roam_pref,
                         hybr_pref,
                         plmn,
                         srv_domain_pref,
                         acq_order_pref,
                         &main_info);

 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Remove mode as per NV item when moving to triple standby */
  if( ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY )
  {
    cmph_remove_mode_in_TSTS(&prot_subsc_chg,
                             &hybr_2_prot_subsc_chg,
                             &hybr_3_prot_subsc_chg);

    if(cmph_is_lte_capable_on_ss(CM_SS_MAIN))
    {
      CM_MSG_HIGH_2("MAIN:ph_ptr->mode_pref:%d mode_pref here:%d ", ph_ptr->main_stack_info.pref_info.mode_pref,main_info.mode_pref);
      main_info.mode_pref = ph_ptr->main_stack_info.pref_info.mode_pref;
    }
    else if( hybr_2_pref_info_ptr != NULL && 
           cmph_is_lte_capable_on_ss(CM_SS_HYBR_2) && 
           !cmph_is_sxlte())
    {
      CM_MSG_HIGH_2("HYBR2:ph_ptr->mode_pref:%d mode_pref here:%d ", ph_ptr->hybr_2_stack_info.pref_info.mode_pref,hybr_2_pref_info_ptr->mode_pref);
      hybr_2_pref_info_ptr->mode_pref = ph_ptr->hybr_2_stack_info.pref_info.mode_pref;
    }
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    else if(hybr_3_pref_info_ptr != NULL && 
          cmph_is_lte_capable_on_ss(CM_SS_HYBR_3))
    {
      
      CM_MSG_HIGH_2("HYBR3:ph_ptr->mode_pref:%d mode_pref here:%d ", ph_ptr->hybr_3_stack_info.pref_info.mode_pref,hybr_3_pref_info_ptr->mode_pref);
      hybr_3_pref_info_ptr->mode_pref = ph_ptr->hybr_3_stack_info.pref_info.mode_pref;
    }
    #endif
	

  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Pack multiple pointers / preference into array of data which used to passed
     to MMOC */
  /* Pack sub-1 data */
  pref_info[0] = &main_info;
  prot_sub_chg[0] = prot_subsc_chg;
  sub_avail[0] = is_gwl_subsc_avail;
  sub_avail[1] = is_cdma_subsc_avail;

  /* Pack sub-2 data */
  if(!cmph_is_sxlte())
  {
    /* hybr_2 goes in as sub2 */
    if (hybr_2_pref_info_ptr != NULL &&
        ARR_SIZE(sub_avail) > 2 && ARR_SIZE(pref_info) > 1)
    {
      pref_info[1] = hybr_2_pref_info_ptr;
      prot_sub_chg[1] = hybr_2_prot_subsc_chg;
      sub_avail[2] = is_hybr_gw_subsc_avail;
    }
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(cmph_is_sxlte())
  {
    /* hybr_3 goes in as sub2 */
    if (hybr_3_pref_info_ptr != NULL &&
        ARR_SIZE(sub_avail) > 2 && ARR_SIZE(pref_info) > 1)
    {
      pref_info[1] = hybr_3_pref_info_ptr;
      prot_sub_chg[1] = hybr_3_prot_subsc_chg;
      sub_avail[2] = is_hybr_3_gw_subsc_avail;
    }
  }
  else
  {
    #ifdef FEATURE_MMODE_TRIPLE_SIM
    /* Pack sub-3 data */
    if (hybr_3_pref_info_ptr != NULL &&
      ARR_SIZE(sub_avail) > 3 && ARR_SIZE(pref_info) > 2)
    {
      pref_info[2] = hybr_3_pref_info_ptr;
      prot_sub_chg[2] = hybr_3_prot_subsc_chg;
      sub_avail[3] = is_hybr_3_gw_subsc_avail;
    }
    #endif
  }
  #endif

  /* Pack all sub information to mmoc_data type (subs_data)*/
  for (i = 0; i < size_of_sub_avail; i++)
  {
    sub_data[i] = (mmoc_subs_data_s_type *)modem_mem_alloc(
                    sizeof(mmoc_subs_data_s_type),MODEM_MEM_CLIENT_MMODE);
    if (sub_data[i])
      memset(sub_data[i], 0, sizeof(mmoc_subs_data_s_type));
    else
      sys_err_fatal_null_ptr_exception();
      
  }

  cmmsc_form_subs_data(sub_avail,
                       (const cm_pref_s_type **)pref_info,
                       sub_data,
                       size_of_sub_avail,
                       size_of_sub_chg,
                       size_of_pref_info_ptr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Update SIM Capabilities */
  sim_coord_state->sim[0].capability.mode = SD_SS_MODE_PREF_ALL_MODES;
  for (i = 1; i < cmpmprx_get_num_of_sims(); i++)
  {
    sim_coord_state->sim[i].capability.mode = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_remove_mode_pref_components(
                                             cmph_map_sd_mode_pref_to_cm_mode_pref(SD_SS_MODE_PREF_ALL_MODES),
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA));

  }

  CM_MSG_HIGH_5( "MSIMC: sub_chgd: cdma sub %d -> %d  gw sub %d -> %d   oprt_mode %d",
                 ph_ptr->is_cdma_subscription_available,
                 is_cdma_subsc_avail,
                 ph_ptr->is_gwl_subscription_available,
                 is_gwl_subsc_avail,
                 ph_ptr->oprt_mode);

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  CM_MSG_HIGH_4( "MSIMC: sub_chgd: gw3 sub %d -> %d, gw2 sub %d -> %d",
                 ph_ptr->is_hybr_gw3_subscription_available,
                 is_hybr_3_gw_subsc_avail,
                 ph_ptr->is_hybr_gw_subscription_available,
                 is_hybr_gw_subsc_avail);
  
  if (cmph_is_sxlte())
  {
    CM_MSG_HIGH_3("SFMODE: sub_chgd: sub0=%d, sub1=%d, pm sub0=%d",
                   cmph_get_subs_feature_mode(SYS_MODEM_AS_ID_1),
                   cmph_get_subs_feature_mode(SYS_MODEM_AS_ID_2),
                   cmpmprx_get_pm_sfmode(SYS_MODEM_AS_ID_1));
  }
  #elif defined(FEATURE_MMODE_DUAL_SIM)
  CM_MSG_HIGH_2( "MSIMC: sub_chgd: gw2 sub %d -> %d",
                 ph_ptr->is_hybr_gw_subscription_available,
                 is_hybr_gw_subsc_avail);
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Adjust the coordination */
  cmsimc_set_coordination(sim_coord_state->active_subs);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*2. Set Event */
  event = CMMSIMC_EVENT_SUB;

  /*3. Package event */
  event_subs.event = event;
  event_subs.nam = nam;
  event_subs.chg_type = chg_type;

  /* Pass active_subs to msc so that only sub event is only generated for active ss */
  event_subs.active_subs = sim_coord_state->active_subs;


  /* Send previously changed standby pref due to this subs & reset in SIMC */
  event_subs.apply_standby_pref_chg = sim_coord_state->standby_pref_chg;
  sim_coord_state->standby_pref_chg = FALSE;

  event_subs.size_of_sub_data = size_of_sub_avail;
  memscpy( event_subs.prot_sub_chg,
           sizeof(prot_subsc_chg_e_type)*MAX_SIMS,
           prot_sub_chg,
           sizeof(prot_subsc_chg_e_type)*MAX_SIMS );
  memscpy( event_subs.sub_data,
           sizeof(mmoc_subs_data_s_type*)*MAX_SUB_SESSION_TYPES,
           sub_data,
           sizeof(mmoc_subs_data_s_type*)*MAX_SUB_SESSION_TYPES );

  /* Print sim coordinator info */
  cmmsimc_print_simc_info();

  /*3. Trigger Coordinator */
  cmmsimc_sim_coordinate( (cmmsc_param_base_s_type *)&event_subs );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Free memory for sub_data */
  for (i = 0; i < size_of_sub_avail; i ++)
  {
    if(sub_data[i] != NULL)
    {
      modem_mem_free((void *) sub_data[i], MODEM_MEM_CLIENT_MMODE);
      sub_data[i] = NULL;
    }
  }
}

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_lte_do_irat

DESCRIPTION
This function process the reselection reports from SD20/MMOC at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_lte_do_irat (void)
{
  cmmsc_param_base_s_type  param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_MAIN, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_rpt_lte_do_irat(&param);
}

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_redial_hdr_fail

DESCRIPTION
This function process the faked ACQ_FAIL report from SD20 to trigger 1s BSR at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_redial_hdr_fail (void)
{
  cmmsc_param_base_s_type  param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_HYBR_1, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_rpt_redial_hdr_fail(&param);
}

/*===========================================================================

FUNCTION  cmmsimc_proc_disable_hybr_hdr

DESCRIPTION
This function process disable hybr hdr at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_disable_hybr_hdr (void)
{
  cmmsc_param_base_s_type  param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_HYBR_1, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_disable_hybr_hdr(&param);
}

/*===========================================================================

FUNCTION  cmmsimc_proc_enable_hybr_hdr

DESCRIPTION
This function process enable hybr hdr at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_enable_hybr_hdr (

        boolean                        send_pref_sys_cmd
        /* Flag to indicate if pref. sys changed command has to be sent.
        */
)
{
  cmmsc_param_proc_enable_hybr_hdr_s_type  param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_HYBR_1, (cmmsc_param_base_s_type *)&param);

  /* Package additional params */
  param.send_pref_sys_cmd = send_pref_sys_cmd;

  /* Multi-stack process */
  cmmsc_auto_proc_enable_hybr_hdr(&param );
}

/*===========================================================================

FUNCTION  cmmsimc_proc_call_end

DESCRIPTION
This function process call end at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_proc_call_end (void)
{
  cmmsc_param_base_s_type  param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_MAIN, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_call_end(&param);
}

/*===========================================================================

  DESCRIPTION:

  This is called when any type of call event occurs in CMCALL module.
  Based on the ss and mapping as_id, event is converted to SIM based.
  This event is sent to SIMC statemachine, where depending current
  requirements, either the call event is ignored or routing to msc.

===========================================================================*/
void cmmsimc_proc_call_event(

  cm_ss_e_type         ss,
    /* ss on which call event was generated */

  cm_call_type_e_type call_type,

  cm_call_event_e_type call_event
)
{
  cmmsc_param_call_ind_s_type call_ind;
  int sim_idx = cmmsimc_mapping_ss_to_sim_idx(ss);

  if (sim_idx == 1)
  {
    call_ind.event = CMMSIMC_EVENT_CALL_SIM2;
  }
  else if (sim_idx == 2)
  {
    call_ind.event = CMMSIMC_EVENT_CALL_SIM3;
  }
  else
  {
    call_ind.event = CMMSIMC_EVENT_CALL_SIM1;
  }

  call_ind.call_event = call_event;
  call_ind.call_type = call_type;
  call_ind.state_machine = NULL;

  cmmsimc_sim_coordinate((cmmsc_param_base_s_type*)&call_ind);
}

/*===========================================================================

FUNCTION  cmmsimc_proc_timer

DESCRIPTION
This function process timer expiration at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_proc_timer (void)
{
  cmmsc_param_base_s_type     param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_MAIN, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_timer(&param);
}

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_hybr_bsr_to_hdr

DESCRIPTION
This function process CM_HYBR_BSR_TO_HDR_F rpt at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_hybr_bsr_to_hdr (
  sd_hybr_bsr_to_hdr_s_type  hybr_bsr_to_hdr_status
)
{
  cmmsc_param_proc_rpt_hybr_bsr_to_hdr_s_type param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_MAIN, (cmmsc_param_base_s_type *)&param);

  /* Package additional params */
  param.hybr_bsr_to_hdr_status = hybr_bsr_to_hdr_status;

  /* Multi-stack process */
  cmmsc_auto_proc_rpt_hybr_bsr_to_hdr(&param );
}


/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_kick_hybr2

DESCRIPTION
This function process CM_KICK_HYBR2_F rpt at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_kick_hybr2 (void
)
{
  cmmsc_param_base_s_type     param;

  /* Update param base on ss */
  cmmsimc_get_param_base_per_ss(CM_SS_MAIN, &param);

  /* Multi-stack process */
  cmmsc_auto_proc_rpt_kick_hybr2(&param );
}

/*===========================================================================
FUNCTION cmmsimc_proc_cmd_standby_pref_chgd

DESCRIPTION
  This function should be called when the operating mode of the phone has
  changed.This function should be called just before sending the operating
  mode changed command to MMOC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_cmd_standby_pref_chgd
(
        sys_modem_dual_standby_pref_e_type    standby_pref,
           /* New standby preference of the phone.
           */

        uint8                                 active_subs_id,
           /* New active subscription when the standby preference is
           ** single
           */
        boolean                               is_cdma_subscription_available,
          /**< Subscription status of CDMA system */

        boolean                               is_gwl_subscription_available,
          /**< Subscription status of GSM/WCDMA system */

        boolean                               is_hybr_gw_subscription_available,
          /**< Subscription status of GSM/WCDMA system */

        boolean                               is_hybr_gw3_subscription_available,

        boolean                               force_pref_standby_change_to_mmoc
)
{
  cmmsimc_state_s_type             *cmmsimc_state = cmmsimc_state_ptr();
  cmph_s_type                      *ph_ptr = cmph_ptr();
  cmss_s_type                      *ss_ptr = cmss_ptr();
  cmmsc_param_base_s_type           param;

  boolean                           is_subs_chgd = FALSE;
  prot_subsc_chg_e_type             prot_subsc_chg = PROT_SUBSC_CHG_NONE;
  prot_subsc_chg_e_type             hybr_2_prot_subsc_chg = PROT_SUBSC_CHG_NONE;
  prot_subsc_chg_e_type             hybr_3_prot_subsc_chg = PROT_SUBSC_CHG_NONE;
  sys_modem_dual_standby_pref_e_type prev_standby_pref = cmmsimc_state->standby_pref;
  sys_modem_dual_standby_pref_e_type new_standby_pref = ph_ptr->internal_standby_pref;

  uint8                             active_ss = ACTIVE_SS_NONE;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Setting Active SS */
  if(active_subs_id & BM(ph_ptr->main_stack_info.asubs_id))
  {
    active_ss |= ACTIVE_SS_MAIN;
  }
  if((active_subs_id & BM(ph_ptr->hybr_2_stack_info.asubs_id)) &&
      cmph_is_msim() && !cmph_is_sxlte())
  {
    active_ss |= ACTIVE_SS_HYBR_2;
  }
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if((active_subs_id & BM(ph_ptr->hybr_3_stack_info.asubs_id)) &&
      (cmph_is_tsts() || cmph_is_sxlte()))
  {
    active_ss |= ACTIVE_SS_HYBR_3;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_4( "MSIMC: standby_chgd: cdma sub %d gwl sub %d gw2 sub %d gw3 sub %d",
                 is_cdma_subscription_available,
                 is_gwl_subscription_available,
                 is_hybr_gw_subscription_available,
                 is_hybr_gw3_subscription_available);

  CM_MSG_HIGH_5( "MSIMC: standby_chgd: oprt_mode %d, oprt_mode_send_time %d standby pref %d -> %d, active_ss %d",
                 ph_ptr->oprt_mode,
                 ph_ptr->oprt_mode_send_time,
                   cmmsimc_state->standby_pref, standby_pref,active_ss);

  /* If subscription has changed, do not send device mode from standby pref proc */
  if(is_cdma_subscription_available != ph_ptr->is_cdma_subscription_available ||
     is_gwl_subscription_available != ph_ptr->is_gwl_subscription_available ||
     is_hybr_gw_subscription_available != ph_ptr->is_hybr_gw_subscription_available ||
     is_hybr_gw3_subscription_available != ph_ptr->is_hybr_gw3_subscription_available)
  {
    is_subs_chgd = TRUE;
  }

  /* send the standby preferences to MMOC 
     In case if subscription changed and standby pref was buffered during a call, 
     no need to send explicit standby change command to mmoc as subscription change 
     will be able to handle */ 

  if(((ph_ptr->user_standby_pref != SYS_MODEM_DS_PREF_AUTO
     && ph_ptr->user_standby_pref != SYS_MODEM_DS_PREF_AUTO_NO_TUNEAWAY)
     || ((ph_ptr->is_standby_pref_buffered || force_pref_standby_change_to_mmoc) &&  !cmpmprx_is_device_mask_set(PM_CHG_MAX_MODE_CAP,NULL)) // this should have been reset earlier itself but keeping it as it is for safe guard.
     ||  (cmmsimc_state->device_mode != cmpmprx_get_device_mode() &&
         is_subs_chgd == FALSE)) && !ph_ptr->is_mmoc_standby_pref_not_req)
  {
    /* If we are moving to triple standby, remove modes from multimode sub */
    if(new_standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
    {
      cmph_remove_mode_in_TSTS(&prot_subsc_chg,
                     &hybr_2_prot_subsc_chg,
                     &hybr_3_prot_subsc_chg);
    }

    CM_MSG_HIGH_3( "MSIMC: standby_chgd: CM->MMOC, user_standby_pref %d is_standby_pref_buffered %d oprt_mode %d",
                      ph_ptr->user_standby_pref,
                      ph_ptr->is_standby_pref_buffered,
                      cmph_ptr()->oprt_mode);
    mmoc_cmd_dual_standby_chgd( standby_pref, active_ss, cmpmprx_get_device_mode());

    /* If we are moving from triple standby to dual/single standby, 
    ** add modes to multimode sub */
    if(prev_standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
    {
      cmph_add_mode_in_DSDX();
    }

    ph_ptr->is_standby_pref_buffered = FALSE;

  }

  cmmsimc_state->device_mode = cmpmprx_get_device_mode();
  if(cmmsimc_state->standby_pref == standby_pref &&
    cmmsimc_state->active_subs == active_subs_id)
  {
    CM_MSG_HIGH_0("MSIMC: standby_chgd: No change");
    return;
  }
  
  /* Update the standby preferences to be used in subscription handling */
  cmmsimc_state->standby_pref = standby_pref;
  cmmsimc_state->active_subs = active_subs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If ph is not Online no need to update state machine
  */
  if (!( ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE ||
         ph_ptr->oprt_mode == SYS_OPRT_MODE_PSEUDO_ONLINE )||
         ph_ptr->oprt_mode_send_time != CMPH_MAX_UPTIME )
  {
    return;
  }

  /* Even if we are not sending corresponding cmd to MMOC, active subs will get updated via Max Cap switch, 
  ** thus num standby req processing need to be incremented */
  ph_ptr->num_standby_requests_processing++;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This part setting the standby preference for STANDBY_PREF CHANGE */
  {
    ss_ptr->info.asubs_id = ph_ptr->main_stack_info.asubs_id;
    ss_ptr->info.gw_asubs_id = ph_ptr->hybr_2_stack_info.asubs_id;
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    ss_ptr->info.gw3_asubs_id = ph_ptr->hybr_3_stack_info.asubs_id;
    #endif

    cmsimc_set_coordination(cmmsimc_state->active_subs);

    if(cmph_is_sxlte())
    {
      cmmsimc_state->standby_pref_chg = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  param.event = CMMSIMC_EVENT_STANDBY_PREF_CHGD;

  /* Print sim coordinator info */
  cmmsimc_print_simc_info();

    /* Trigger Coordinator */
  cmmsimc_sim_coordinate(&param );

  /* Update the current priority sub if required */
  #if defined FEATURE_MMODE_TRIPLE_SIM
  if(cmpmprx_get_device_mode() == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY)
  {
    cmph_update_curr_priority_sub();
  }
  #endif

}


/*===========================================================================

DESCRIPTION
  Process acquisition fail report from system selection layer. After reporting
  this indication, system selection either continues with second round of
  acquisition or put access layers in pwr save.

  This function is going to decide which subscription should consume the rpt.
===========================================================================*/
void cmmsimmsc_proc_rpt_acq_fail( cm_ss_e_type ss, 
 sd_ss_mode_pref_e_type  mode_pref)
{
  cmmsc_param_base_s_type  param;

  cmmsimc_get_param_base_per_ss(ss, &param);

  switch(param.state_machine->route_tbl)
  {
    case CMMSC_ROUTE_TABLE_AUTO:
      cmmsc_auto_proc_acq_fail_rpt(param.state_machine, ss,mode_pref);
      break;

    case CMMSC_ROUTE_TABLE_SGLTE:
    default:
      break;
  }
}


/*===========================================================================

DESCRIPTION
  Identify subscription
  invoke corresponding MSC

===========================================================================*/
void cmmsimmsc_proc_rpt_srv_ind(

  cmmsc_param_srv_ind_s_type *srv_ind

)
{
  switch(srv_ind->state_machine->route_tbl)
  {
    case CMMSC_ROUTE_TABLE_AUTO:
      cmmsc_auto_proc_srv_ind_rpt(srv_ind->state_machine, srv_ind->rpt_ptr,
                                  srv_ind->prev_srv_status_ss);
      break;

    case CMMSC_ROUTE_TABLE_SGLTE:
    default:
      CM_MSG_HIGH_0("acq fail rpt ignored");
  }
}

/*===========================================================================

===========================================================================*/
void cmmsimmsc_proc_rpt_3gpp_stopped( sd_ss_e_type sd_ss )
{
  cmmsc_param_base_s_type  param;
  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(sd_ss);

  cmmsimc_get_param_base_per_ss(cm_ss, &param);

  switch(param.state_machine->route_tbl)
  {
    case CMMSC_ROUTE_TABLE_AUTO:
      cmmsc_auto_proc_3gpp_stopped_rpt(param.state_machine, cm_ss);
      break;

    case CMMSC_ROUTE_TABLE_SGLTE:
    default:
      CM_MSG_HIGH_1("3gpp stopped ignored for ss %d", cm_ss);
  }
}

/*===========================================================================

FUNCTION cmmsimc_proc_rpt_srv_ind

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_rpt_srv_ind
(
        const cm_sd_rpt_u_type         *rpt_ptr,
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
        sys_srv_status_e_type         prev_srv_status_ss

)
{

  cm_ss_e_type                   ss = cmph_map_sd_ss_type_to_cm_ss_type(rpt_ptr->srv_ind_info.ss);
    /* SS on which the service has changed */

  sys_srv_status_e_type          srv_status =
                                    rpt_ptr->srv_ind_info.si_info.srv_status;
    /* New service status of the SS */

  sd_mode_e_type                 mode = rpt_ptr->srv_ind_info.si_info.mode;
     /* Mode acquired on the SS */

  cmmsimc_event_e_type           event = CMMSIMC_EVENT_NONE;
  int                            sim_idx = 0;
  cmmsc_param_srv_ind_s_type     event_srv_ind;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( rpt_ptr != NULL);
  CM_ASSERT( rpt_ptr->hdr.cmd == CM_SRV_IND_INFO_F );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process event based on the state.
  */
  
  CM_MSG_HIGH_6("RPT->MSIMC: srv_ind: ss %d, srv %d, mode %d is_pref_srv_acq %d realign_hdr %d, realign_lte %d", 
  			  ss, srv_status, mode,
              rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq,
              rpt_ptr->srv_ind_info.si_info.realign_hdr,
              rpt_ptr->srv_ind_info.si_info.realign_lte);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_msim())
    {
    sim_idx = cmmsimc_mapping_ss_to_sim_idx(ss);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Setting the event */

  if (sim_idx == 0)
    event = CMMSIMC_EVENT_SRV_SIM1;
  else if (sim_idx == 1)
    event = CMMSIMC_EVENT_SRV_SIM2;
  else if (sim_idx == 2)
    event = CMMSIMC_EVENT_SRV_SIM3;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Pack the event */
  event_srv_ind.event = event;
  event_srv_ind.state_machine = NULL;
  event_srv_ind.rpt_ptr = rpt_ptr;
  event_srv_ind.prev_srv_status_ss = prev_srv_status_ss;

  /* Trigger Coordinator */
  cmmsimc_sim_coordinate((cmmsc_param_base_s_type*)&event_srv_ind );

}


/* ============================== ACTION FUNCTIONS ============================ */
void cmmsimc_act_route_request( cmmsc_param_base_s_type * event_package)
{
  cmmsimc_event_e_type event;

  if(event_package == NULL )
    sys_err_fatal_null_ptr_exception();

  event = event_package->event;

  switch (event)
  {
     case CMMSIMC_EVENT_ONLINE:
     case CMMSIMC_EVENT_OFFLINE:
     {
       cmmsc_param_oprt_mode_s_type *oprt_mode_param =
                                 (cmmsc_param_oprt_mode_s_type *)event_package;
       cmmsc_proc_cmd_oprt_mode_chgd(oprt_mode_param);
       break;
     }

     case CMMSIMC_EVENT_SUB:
     {
       cmmsc_param_subs_chgd_s_type *subs_param =
                                 (cmmsc_param_subs_chgd_s_type *)event_package;
       cmmsc_proc_cmd_subsc_chgd(subs_param);
       break;
     }

     case CMMSIMC_EVENT_PREF_SYS_SIM1:
     case CMMSIMC_EVENT_PREF_SYS_SIM2:
     case CMMSIMC_EVENT_PREF_SYS_SIM3:
     {
       cmmsc_param_pref_sys_s_type *pref_sys_param =
                                (cmmsc_param_pref_sys_s_type *)event_package;
       cmmsc_proc_cmd_pref_sys_chgd(pref_sys_param);
       break;
     }

     case CMMSIMC_EVENT_SRV_SIM1:
     case CMMSIMC_EVENT_SRV_SIM2:
     case CMMSIMC_EVENT_SRV_SIM3:
     {
       cmmsc_param_srv_ind_s_type *srv_ind = (cmmsc_param_srv_ind_s_type*)event_package;
       cmmsimmsc_proc_rpt_srv_ind(srv_ind);
       break;
     }

     case CMMSIMC_EVENT_SUSPEND_RESUME:
     {
       cmmsc_param_suspend_resume_subs_s_type *suspend_resume_param =
                                 (cmmsc_param_suspend_resume_subs_s_type *)event_package;
       cmmsc_proc_cmd_suspend_resume(suspend_resume_param);
       break;
     }
     
     default:
       break;
  }

  return;
}

void cmmsimc_act_power_down( cmmsc_param_oprt_mode_s_type * event_package)
{
  cmmsimc_state_s_type          *cmmsimc_ptr = cmmsimc_state_ptr();

  if(event_package == NULL )
    sys_err_fatal_null_ptr_exception();

  cmmsimc_ptr->active_subs = 0;
  cmmsimc_ptr->coordination = CMMSIMC_COORDINATION_NONE;

  // We don't re-initialize this:
  // 1. cmmsimc_ptr->ss_with_mm
  // 2. cmmsimc_ptr->standby_pref
  // 3. cmmsimc_ptr->msc_machine
  //    MSC will clear the state_machine, but we don't have to.
  //    This avoid deallocate and reallocate memory.

  /* Initialize all SIM */
  cmmsimc_sim_init();

  /* Forward the command to cmmsc */
  cmmsc_proc_cmd_oprt_mode_chgd(event_package);

}

void cmmsimc_act_on_off(
     cmmsimc_action_e_type     action,
     cmmsc_param_base_s_type * event_package
     )
{
  if(event_package == NULL )
    sys_err_fatal_null_ptr_exception();

  if (action == CMMSIMC_SIM_ACT_ON)
    cmmsc_proc_cmd_on_off(FALSE, event_package);
  else if (action == CMMSIMC_SIM_ACT_OFF)
    cmmsc_proc_cmd_on_off(TRUE, event_package);
}

void cmmsimc_act_subs_change( cmmsc_param_base_s_type * event_package)
{
  cmmsc_param_subs_chgd_s_type *subs_change =
                               (cmmsc_param_subs_chgd_s_type*)event_package;
  size_t                        sub_idx, sim_idx;

  boolean                       hybr2_prot_sub_chg = FALSE;
  mmoc_subs_data_s_type        *hybr2_sub_data     = NULL;
  boolean                       hybr3_prot_sub_chg = FALSE;
  mmoc_subs_data_s_type        *hybr3_sub_data     = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(subs_change->size_of_sub_data > MAX_SUB_SESSION_TYPES)
  {
    subs_change->size_of_sub_data = MAX_SUB_SESSION_TYPES;
  }

  /* Apply filter on SIM information - As this filter is not acting  */
  for (sub_idx = 0; sub_idx < subs_change->size_of_sub_data; sub_idx++ )
  {
    /* to resolve kw errors */
    if(sub_idx < MAX_SUB_SESSION_TYPES)
    {
      /* For SIM-1, update 2 sessions (0 = gw, 1 = cdma) */
      if (sub_idx == 0 || sub_idx == 1)
      {
        sim_idx = 0 ;
        cmmsc_update_sub_data_per_cap(subs_change->sub_data[sub_idx], sim_idx);
        
        /* Update ue_mode */
        subs_change->sub_data[sub_idx]->curr_ue_mode =
          cmmsimc_state_machine_ptr_per_sim_idx(sim_idx)->stack_common_info.ue_mode;

        if (cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1))
        {
          subs_change->sub_data[sub_idx]->is_ue_mode_substate_srlte =
            (cmmsimc_state_machine_ptr_per_sim_idx(sim_idx)->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2);
        }
        else
        {
          subs_change->sub_data[sub_idx]->is_ue_mode_substate_srlte = FALSE;
        }
          
        CM_MSG_HIGH_2("curr_ue_mode %d is_ue_mode_substate_srlte %d",
          subs_change->sub_data[sub_idx]->curr_ue_mode,
          subs_change->sub_data[sub_idx]->is_ue_mode_substate_srlte);
      }
      else
      {
        /* for now, all SIM-0 has 2 sessions (cdma and gw), the rest has 1 session.
           so sub(0) & sub(1) belong to sim-0, sub(2) belongs to sim-1 */
        sim_idx = sub_idx - 1;
          cmmsc_update_sub_data_per_cap(subs_change->sub_data[sub_idx], sim_idx);
        subs_change->sub_data[sub_idx]->curr_ue_mode = SYS_UE_MODE_NORMAL;
        subs_change->sub_data[sub_idx]->is_ue_mode_substate_srlte = FALSE;
      }
      subs_change->sub_data[sub_idx]->is_perso_locked = FALSE;
    }
  }

  // Based on Future Reqs, this code would be made generic for covering both SUB1 and SUB2. 
  if(cmpm_ptr()->ims_for_nondds && subs_change->sub_data[0]  && 
    (subs_change->sub_data[0]->mode_pref & SD_SS_MODE_PREF_LTE))
  {
    if(subs_change->sub_data[0]->prm.gw.srv_domain_pref == SD_SS_SRV_DOMAIN_PREF_CS_PS)
    {
      subs_change->sub_data[0]->prm.gw.srv_domain_pref = SD_SS_SRV_DOMAIN_PREF_CS_VOLTE;
    }
    else if(subs_change->sub_data[0]->prm.gw.srv_domain_pref == SD_SS_SRV_DOMAIN_PREF_PS_ONLY)
    {
      subs_change->sub_data[0]->prm.gw.srv_domain_pref = SD_SS_SRV_DOMAIN_PREF_VOLTE;
    }

    CM_MSG_HIGH_2("SIMC: Due to ims_for_nondds, srv_domain change to %d: change_in_ims_for_nondds %d",
      subs_change->sub_data[0]->prm.gw.srv_domain_pref,cmpm_ptr()->change_in_ims_for_nondds);

    cmpm_ptr()->change_in_ims_for_nondds = FALSE;
  }

  /* For dsdx, get the hybr2 data from event pkg.
   ** For num of sim = 1 (ie, sglte, svlte), fill in default value for hybr2.
   ** Note ! If num of sim > 2, this part needs to be modified.
   */



  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(cmph_is_sxlte())
  {
    hybr2_prot_sub_chg = 0;
    hybr2_sub_data = NULL;

    hybr3_prot_sub_chg = (subs_change->prot_sub_chg[1] & PROT_SUBSC_CHG_GW )?TRUE:FALSE;
    hybr3_sub_data = subs_change->sub_data[2];
  }
  else if (cmph_is_msim())
  {
    hybr2_prot_sub_chg = (subs_change->prot_sub_chg[1] & PROT_SUBSC_CHG_GW )?TRUE:FALSE;
    hybr2_sub_data = subs_change->sub_data[2];

    #ifdef FEATURE_MMODE_TRIPLE_SIM
    if(cmph_is_tsts())
    {
      hybr3_prot_sub_chg = (subs_change->prot_sub_chg[2] & PROT_SUBSC_CHG_GW )?TRUE:FALSE;
      hybr3_sub_data = subs_change->sub_data[3];
    }
    else
    #endif
    {
      hybr3_sub_data = NULL;
      hybr3_prot_sub_chg = 0;
    }
  }
  #elif defined (FEATURE_MMODE_DUAL_SIM)
  if (cmph_is_msim())
  {
    hybr2_prot_sub_chg = (subs_change->prot_sub_chg[1] & PROT_SUBSC_CHG_GW )?TRUE:FALSE;
    hybr2_sub_data = subs_change->sub_data[2];
  }
  #endif



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send sub chg command to mmoc */
  cmmsimc_send_mmoc_subsc_chgd( subs_change->chg_type,
                                 subs_change->prot_sub_chg[0],
                                 hybr2_prot_sub_chg,
                                 hybr3_prot_sub_chg,
                                 subs_change->nam,
                                 subs_change->sub_data[1],
                                 subs_change->sub_data[0],
                                 hybr2_sub_data,
                                 hybr3_sub_data,
                                 subs_change->active_subs
                                );
}



void cmmsimc_act_wake_up(
     int                          sim_idx,
     cmmsc_param_srv_ind_s_type  *event_package)
{
  cmmsimc_state_s_type           *cmmsimc_ptr = cmmsimc_state_ptr();
  sys_srv_status_e_type           srv_status =
                                  event_package->rpt_ptr->srv_ind_info.si_info.srv_status;
  sys_srv_status_e_type           curr_srv_status = SYS_SRV_STATUS_NO_SRV;
  sd_ss_e_type                    ss;

  /* Only wake up a stack if it's ACTIVE */
  if (cmmsimc_ptr->sim[sim_idx].state != CMMSIMC_SIM_STATE_ACTIVE)
  {
    return;
  }

  /* Also, only wake up if we found service.
  ** If NOT(Full Service) || Is-Still-Acquiring-More-Preferred-Service)
  */
  if(!sys_srv_status_is_full_srv( srv_status ) ||
      event_package->rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq )
  {
    return;
  }

  /* Map SIM & Stack-0 -->  ss (as we only concern about service on stack-0) */
  ss = cmph_map_subs_stack_to_ss((sys_modem_as_id_e_type)sim_idx, 0);
  if (ss == SD_SS_MAIN)
  {
    curr_srv_status = cmss_ptr()->info.srv_status;
  }
  else if (ss == SD_SS_HYBR_2)
  {
    curr_srv_status = cmss_ptr()->info.gw_srv_status;
  }
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  else if (ss == SD_SS_HYBR_3)
  {
    curr_srv_status = cmss_ptr()->info.gw3_srv_status;
  }
  #endif

  /* Conditions to wake up a SIM:
     - If the current stack is not in Power-save
  */
  if((curr_srv_status == SYS_SRV_STATUS_PWR_SAVE ) &&
     (event_package->prev_srv_status_ss == SYS_SRV_STATUS_PWR_SAVE ||
      event_package->prev_srv_status_ss == SYS_SRV_STATUS_PWR_SAVE_INTERNAL ))
  {
    CM_MSG_HIGH_2("MSIMC: Wake up simx_idx=%d ss=%d",sim_idx, ss);
  }
  else
  {
    return;
  }

  mmoc_cmd_wakeup_from_pwr_save(ss);
}

/*===========================================================================

DESCRIPTION:

  Action function to handle when call events occur in peer subs.
  For e.g. is a call is ended on SUB2 and SIMC needs to fwd the evt to SUB0
  then this action/call used.

===========================================================================*/
void cmsimc_act_peer_subs_call(

  cmmsc_param_call_ind_s_type *call_ind

)
{
  cmmsimc_state_s_type  *simc = cmmsimc_state_ptr();
  switch(call_ind->state_machine->route_tbl)
  {
    case CMMSC_ROUTE_TABLE_AUTO:
      cmmsc_auto_proc_peer_subs_call(call_ind->state_machine, call_ind);

      /* If TAU is due, then perform TAU on SIM2 call end */
      if(simc->esr_state == CMSIMC_ESR_TAU_PENDING &&
         call_ind->call_event == CM_CALL_EVENT_END)
      {
        #ifdef FEATURE_LTE_TO_1X
        cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();
        #endif
        simc->esr_state = CMSIMC_ESR_NONE;
      }
      break;

    case CMMSC_ROUTE_TABLE_SGLTE:
    default:
      CM_MSG_HIGH_0("call rpt ignored");
  }
}
/*===========================================================================

FUNCTION cmmsimc_get_sim_cap

DESCRIPTION
  The function gets the sim capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsimc_get_sim_cap (
  cm_ss_e_type  cm_ss
)
{
  int sim_idx = cmmsimc_mapping_ss_to_sim_idx(cm_ss);
  cmmsimc_state_s_type  *sim_coord_state = cmmsimc_state_ptr();
  
  if(sim_idx >= CMMSIMC_SIM_NUM)
  {
    return SD_SS_MODE_PREF_NONE;
  }
  
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_NO_CHANGE)
  #elif defined FEATURE_MMODE_DUAL_SIM
  if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_3)
  #else
  if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_2)
  #endif
  {
    return sim_coord_state->sim[sim_idx].capability.mode;
  }
  return SD_SS_MODE_PREF_NONE;
}

/*===========================================================================

FUNCTION cmmsimc_send_mmoc_subsc_chgd

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void    cmmsimc_send_mmoc_subsc_chgd
(
        mmoc_subsc_chg_e_type                chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                prot_subsc_chg,

        boolean                              hybr_gw_subs_chg,

        boolean                                   hybr3_gw_subs_chg,


        byte                                 nam,

        const mmoc_subs_data_s_type          *cdma_subs_ptr,
        /* CDMA subscription info */

        const mmoc_subs_data_s_type          *gw_subs_ptr,
           /* GW subscription info */

        const mmoc_subs_data_s_type          *gw_hybr_subs_ptr ,

           /* GW HYBR subscription info */
        const mmoc_subs_data_s_type          *gw3_hybr_subs_ptr,
        

        uint8                                 active_subs


)
{
  uint8                 active_ss = ACTIVE_SS_NONE;


  if(active_subs & SYS_MODEM_AS_ID_1_MASK)
  {
     active_ss |= ACTIVE_SS_MAIN;
  }

  if(active_subs & SYS_MODEM_AS_ID_2_MASK)
  {
    if(cmph_is_sxlte())
    {
      active_ss |= ACTIVE_SS_HYBR_3;
    }
    else
    {
      active_ss |= ACTIVE_SS_HYBR_2;
    }
  }

  if(active_subs & SYS_MODEM_AS_ID_3_MASK)
  {
     active_ss |= ACTIVE_SS_HYBR_3;
  }


  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_subs_cmd_to_buffer( chg_type,
                                prot_subsc_chg,
                                hybr_gw_subs_chg,
                                hybr3_gw_subs_chg,
                                nam,
                                cdma_subs_ptr,
                                gw_subs_ptr,
                                gw_hybr_subs_ptr,
                                gw3_hybr_subs_ptr,
                                active_ss,
                                cmpmprx_get_device_mode());
  
  #endif
  mmoc_cmd_subscription_chgd_new( chg_type,
                                  prot_subsc_chg,
                                  hybr_gw_subs_chg,
                                  hybr3_gw_subs_chg,
                                  nam,
                                  cdma_subs_ptr,
                                  gw_subs_ptr,
                                  gw_hybr_subs_ptr,
                                  gw3_hybr_subs_ptr,
                                  active_ss,
								  cmpmprx_get_device_mode()
                                 );

}

/*===========================================================================

FUNCTION cmmsimc_send_subsc_chgd_current_pref

DESCRIPTION
  This function should be called when the subscrptions need to be refreshed
  using the current preferences on both the stacks.This function should be
  called just before sending the subsc_chgd command to MMOC.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_send_subsc_chgd_current_pref(
	prot_subsc_chg_e_type main_subsc_chg,
	
	prot_subsc_chg_e_type hybr_2_subsc_chg,
	
	prot_subsc_chg_e_type hybr_3_subsc_chg 

)
{
  #ifdef FEATURE_MMODE_DUAL_SIM
  cmph_s_type       *ph_ptr          = cmph_ptr();

  cm_pref_s_type    *hybr_2_pref_ptr = NULL;

  cm_orig_q_s_type  *ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );

  cm_orig_q_s_type  *ph_hybr_2_orig_top_ptr
                                     = cmtask_orig_para_get_top( CM_SS_HYBR_2 );

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_pref_s_type    *hybr_3_pref_ptr = NULL;
  cm_orig_q_s_type  *ph_hybr_3_orig_top_ptr
    = cmtask_orig_para_get_top( CM_SS_HYBR_3 );

  #endif

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("ph_orig_top_ptr null");
    return;
  }

  if (ph_hybr_2_orig_top_ptr == NULL && !cmph_no_hybr2())
  {
    CM_ERR_0("ph_hybr_2_orig_top_ptr null");
    return;
  }

  if (!cmph_is_msim())
  {
    CM_ERR_0("not multi-sim");
    return;
  }

  /* Allocate buffer only if multi-sim */
  if(!cmph_no_hybr2())
  {
    hybr_2_pref_ptr = cm_pref_ptr();
    memscpy(hybr_2_pref_ptr, sizeof(cm_pref_s_type),
                    &(ph_ptr->hybr_2_stack_info.pref_info), sizeof(cm_pref_s_type));
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
              
  if(cmph_is_tsts() ||cmph_is_sxlte())
  {
    hybr_3_pref_ptr = cm_pref_ptr();
    memscpy(hybr_3_pref_ptr, sizeof(cm_pref_s_type),
                    &(ph_ptr->hybr_3_stack_info.pref_info), sizeof(cm_pref_s_type));
  }
            
  #endif

  /* Fill the current hybr_gw stacks preferences
  */
  if (ph_hybr_2_orig_top_ptr != NULL && hybr_2_pref_ptr != NULL)
  {
    hybr_2_pref_ptr->band_pref = ph_hybr_2_orig_top_ptr->orig->orig_band_pref;
    hybr_2_pref_ptr->tds_band_pref = ph_hybr_2_orig_top_ptr->orig->orig_tds_band_pref;
    hybr_2_pref_ptr->lte_band_pref = ph_hybr_2_orig_top_ptr->orig->orig_lte_band_pref;
    hybr_2_pref_ptr->hybr_pref = ph_hybr_2_orig_top_ptr->orig->orig_hybr_pref;
    hybr_2_pref_ptr->mode_pref = ph_hybr_2_orig_top_ptr->orig->orig_mode_pref;
    hybr_2_pref_ptr->prl_pref = ph_hybr_2_orig_top_ptr->orig->orig_prl_pref;
    hybr_2_pref_ptr->roam_pref = ph_hybr_2_orig_top_ptr->orig->orig_roam_pref;
    hybr_2_pref_ptr->srv_domain_pref = cmtask_compute_srv_domain_pref(CM_SS_HYBR_2);
    hybr_2_pref_ptr->pref_term = ph_hybr_2_orig_top_ptr->orig->orig_pref_term;
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if (ph_hybr_3_orig_top_ptr != NULL  && hybr_3_pref_ptr != NULL)
  {
    hybr_3_pref_ptr->band_pref = ph_hybr_3_orig_top_ptr->orig->orig_band_pref;
    hybr_3_pref_ptr->tds_band_pref = ph_hybr_3_orig_top_ptr->orig->orig_tds_band_pref;
    hybr_3_pref_ptr->lte_band_pref = ph_hybr_3_orig_top_ptr->orig->orig_lte_band_pref;
    hybr_3_pref_ptr->hybr_pref = ph_hybr_3_orig_top_ptr->orig->orig_hybr_pref;
    hybr_3_pref_ptr->mode_pref = ph_hybr_3_orig_top_ptr->orig->orig_mode_pref;
    hybr_3_pref_ptr->prl_pref = ph_hybr_3_orig_top_ptr->orig->orig_prl_pref;
    hybr_3_pref_ptr->roam_pref = ph_hybr_3_orig_top_ptr->orig->orig_roam_pref;
    hybr_3_pref_ptr->srv_domain_pref = cmtask_compute_srv_domain_pref(CM_SS_HYBR_3);
    hybr_3_pref_ptr->pref_term = ph_hybr_3_orig_top_ptr->orig->orig_pref_term;
  }
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(!cmph_is_tsts() && !cmph_is_sxlte())
  {
    hybr_3_subsc_chg = PROT_SUBSC_CHG_NONE;
  }
  #endif
  if( ph_orig_top_ptr != NULL )
  {
    cmmsimc_proc_cmd_subsc_chgd( MMOC_SUBSC_CHG_MODE_CHNG,
                                (byte)ph_ptr->curr_nam,
                              ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                              ph_orig_top_ptr->orig->orig_mode_pref,
                              ph_orig_top_ptr->orig->orig_band_pref,
                              ph_orig_top_ptr->orig->orig_lte_band_pref,
                              ph_orig_top_ptr->orig->orig_tds_band_pref,
                              ph_orig_top_ptr->orig->orig_prl_pref,
                              ph_orig_top_ptr->orig->orig_roam_pref,
                              ph_orig_top_ptr->orig->orig_hybr_pref,
                              ph_ptr->main_stack_info.pref_info.plmn,
                              cmtask_compute_srv_domain_pref(CM_SS_MAIN),
                              ph_ptr->main_stack_info.pref_info.acq_order_pref,
                              hybr_2_pref_ptr,
                              #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                              hybr_3_pref_ptr,
                              #endif
                              ph_ptr->is_gwl_subscription_available,
                              ph_ptr->is_hybr_gw_subscription_available,
                              ph_ptr->is_hybr_gw3_subscription_available,
                              ph_ptr->is_cdma_subscription_available,
                              main_subsc_chg,
                              hybr_2_subsc_chg,
                              hybr_3_subsc_chg );
    }

  /* Free allocated buffer */
  if (hybr_2_pref_ptr != NULL)
  {
    cm_mem_free (hybr_2_pref_ptr);
  }
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if (hybr_3_pref_ptr != NULL)
  {
    cm_mem_free (hybr_3_pref_ptr);
  }
  #endif

  #endif
} /* cmmsimc_send_subsc_chgd_current_pref */


#ifdef FEATURE_MMODE_DUAL_ACTIVE

/*===========================================================================

FUNCTION cmmsimc_suspend_or_resume_other_subs
DESCRIPTION
  Function to suspend or resume other SS

DEPENDENCIES

RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/
void cmmsimc_suspend_or_resume_other_subs
(
  sys_modem_as_id_e_type asubs_id, 
  boolean                suspend
)
{
  sys_modem_as_id_e_type other_subs;
  cm_ss_e_type           other_ss;
  
  cmmsimc_event_e_type                   event;
  cmmsc_param_suspend_resume_subs_s_type      event_suspend_resume;
  
   CM_MSG_HIGH_2("asubs_id %d int_standby_pref %d", asubs_id, cmph_ptr()->internal_standby_pref);
  if((asubs_id != SYS_MODEM_AS_ID_1) && (asubs_id != SYS_MODEM_AS_ID_2))
  {
    return;
  }

  if( cmph_ptr()->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY)
  {
    return;
  }

  other_subs = (asubs_id == SYS_MODEM_AS_ID_2)?SYS_MODEM_AS_ID_1:SYS_MODEM_AS_ID_2;
  other_ss = cmph_map_subs_to_ss(other_subs);
 
  if ((other_ss > CM_SS_NONE) && (other_ss < CM_SS_MAX) && 
       cmph_ptr()->ss_susp_info[other_ss].is_suspend == suspend &&
	   !cmph_ptr()->ss_susp_info[other_ss].num_requests)
           // Above condition is needed in case when CM is yet to receive suspend CNF from SD
  {
    CM_MSG_HIGH_2("SS %d already in the expected suspend state %d",other_ss,suspend);
    return;
  }

  event = CMMSIMC_EVENT_SUSPEND_RESUME;
 
  event_suspend_resume.event       = event;
  event_suspend_resume.asubs_id    = other_subs;
  event_suspend_resume.suspend     = suspend;
   
  CM_MSG_HIGH_2("suspend resume : asubs_id %d, suspend %d",asubs_id,suspend);
  
  cmmsimc_sim_coordinate( (cmmsc_param_base_s_type *)&event_suspend_resume );

}/* cmmsimc_suspend_or_resume_other_subs */

#endif /* FEATURE_MMODE_DUAL_ACTIVE */


/*===========================================================================

FUNCTION cmmsimc_mapping_asubs_id_to_sim_idx

DESCRIPTION
  The function maps asubs_id to sim index.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
static int cmmsimc_mapping_asubs_id_to_sim_idx(
  sys_modem_as_id_e_type asubs_id
)
{
  switch (asubs_id)
  {
    case SYS_MODEM_AS_ID_1:
    case SYS_MODEM_AS_ID_2:
	#ifdef FEATURE_MMODE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3:
    #endif
      return (int)asubs_id;

    default:
      CM_ERR_1("Invalid asubs_id %d", asubs_id);
      return 0;
  }
}

/*===========================================================================

FUNCTION cmmsimc_mapping_ss_to_sim_idx

DESCRIPTION
  The function maps cm ss to sim index.

DEPENDENCIES
  None

RETURN VALUE
  sim index

SIDE EFFECTS
  None

===========================================================================*/
static int cmmsimc_mapping_ss_to_sim_idx(
  cm_ss_e_type ss
)
{
  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(ss);
  return cmmsimc_mapping_asubs_id_to_sim_idx(asubs_id);
}

/*===========================================================================

FUNCTION cmmsimc_get_msc_ptr_per_cm_ss

DESCRIPTION
  The function gets the MSC state machine pointer base on the specified cm_ss.

DEPENDENCIES
  None

RETURN VALUE
 cmmsc_state_machine_s_type *

SIDE EFFECTS
  None

===========================================================================*/
cmmsc_state_machine_s_type * cmmsimc_get_msc_ptr_per_cm_ss (
  cm_ss_e_type  cm_ss
)
{
  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(cm_ss);
  cmmsc_state_machine_s_type * state_machine = cmmsimc_state_machine_ptr(asubs_id);

  return state_machine;
}

/*===========================================================================

FUNCTION cmmsimc_get_msc_stack_state

DESCRIPTION
  Get state of a stack from MSC

===========================================================================*/
cmmsc_stack_state_e_type  cmmsimc_get_msc_stack_state(
  cm_ss_e_type  cm_ss
)
{
  int stk_id = cmmsc_mapping_ss_to_stack_idx(cm_ss);

  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(cm_ss);
  cmmsc_state_machine_s_type * state_machine = cmmsimc_state_machine_ptr(asubs_id);

  return state_machine->stack[stk_id].state;
}

/*===========================================================================

FUNCTION cmmsc_is_in_srv_state

DESCRIPTION
  The function returns if the stack is in service.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsimc_is_in_srv (
  cm_ss_e_type  cm_ss
)
{
  return (cmmsimc_get_msc_stack_state(cm_ss) == CMMSC_STACK_STATE_SRV);
}

/*===========================================================================

FUNCTION cmmsimc_get_msc_ptr_per_sd_ss

DESCRIPTION
  The function gets the MSC state machine pointer base on the specified sd_ss.

DEPENDENCIES
  None

RETURN VALUE
 cmmsc_state_machine_s_type *

SIDE EFFECTS
  None

===========================================================================*/
cmmsc_state_machine_s_type * cmmsimc_get_msc_ptr_per_sd_ss (
  sd_ss_e_type  sd_ss
)
{
  return cmmsimc_get_msc_ptr_per_cm_ss(cmph_map_sd_ss_type_to_cm_ss_type(sd_ss));
}

/*===========================================================================
  This function decides if ESR needs to performed due to call originations
  in msim standby.
===========================================================================*/

boolean cmmsimc_is_esr_on_call_required
(
  const cmcall_s_type *call_ptr
)
{
  cm_ss_e_type ss;  
  cmmsimc_state_s_type  *simc = cmmsimc_state_ptr();
  
  /* Necessity of sending ESR is controlled by NV. If device supports
     dedicated RF between LTE and voice RAT of another subscription then
     this NV should be disabled */
  if(!cmph_is_msim_standby_esr_enabled())
  {
    CM_MSG_HIGH_0("ESR not enabled");
    return FALSE;
  }

  /* As of now, NAS is not able to handle ESR when not in SRLTE sub-mode
  */
  if(!cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1))
  {
    CM_MSG_HIGH_0("ESR not required, sub1 not in SRLTE sub mode");
    return FALSE;
  }

  #ifdef FEATURE_LTE_TO_1X
  if((ss = cmss_lte_srv_on_any_stack()) == CM_SS_NONE)
  #endif
  {
    return FALSE;
  }

  /* call being sent on LTE stack , ESR not needed here.
  ** since this ESR is to suspend LTE for voice on different stack 
  ** this check is to make sure that CSFB calls on SUB1 do not trigger
  ** ESR procedure for msim case
  */
  if (ss == call_ptr->ss)
  {
    return FALSE;
  }

  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined FEATURE_IP_CALL
  /* for CSFB E911 calls we do not need to do ESR from here */
  if(call_ptr->orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ||
  	call_ptr->orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG)
  {
    return FALSE;
  }
  #endif

  if (cmss_intl_srv_info_ptr(ss)->srv_status != SYS_SRV_STATUS_SRV)
  {
    return FALSE;
  }
  
  #ifdef FEATURE_CM_LTE
  /* if lte service found ss is not active, do not do ESR */
  if(cmregprx_get_stack_info_ptr(cmph_map_cm_ss_type_to_sd_ss_type(ss))->state != CMREGPRX_STATE_ACTIVE)
  #endif
  {
    return FALSE;
  }

  if(!cmph_is_subs_feature_mode_srlte(cmph_map_cm_ss_to_subs(ss)))
  {
    return FALSE;
  }

  if(simc->esr_state != CMSIMC_ESR_NONE)
  {
    CM_MSG_HIGH_2("skip ESR, state %d call id %d", simc->esr_state,
                                                  call_ptr->call_id);
    return FALSE;
  }

  CM_MSG_HIGH_1("MSIM decides to do ESR, call id %d", call_ptr->call_id);
  
  return TRUE;
}


/*===========================================================================
  Sends ESR request when RF used by LTE is going to be occupied by a higher
  priority voice RAT from another subscription due to voice call orig.
===========================================================================*/
void cmmsimc_send_esr_req
(
  const cmcall_s_type *call_ptr
)
{
  cmmsimc_state_s_type  *simc = cmmsimc_state_ptr();

  #ifdef FEATURE_LTE_TO_1X
  cmcsfbcall_send_esr_req(call_ptr->call_id);

  CM_MSG_HIGH_2("ESR req sent, call id %d as id %d",
               call_ptr->call_id, call_ptr->asubs_id);
  
  simc->esr_state = CMSIMC_ESR_RSP_PENDING;
  #endif
}

/*===========================================================================
  This is called on recving MM_CM_1XCSFB_CALL_RSP. If call was waiting to
  due to ESR response, then MSIM call orig is resumed.
===========================================================================*/
void cmmsimc_proc_rpt_esr_rsp
(
  uint8 context_id
)
{
  cmmsimc_state_s_type  *simc = cmmsimc_state_ptr();
  cmcall_s_type *call_ptr;

  CM_MSG_HIGH_1("MSIM: esr rsp recvd call-id %d", context_id);

  call_ptr = cmcall_ptr ( context_id );
           
  if (call_ptr != NULL &&
      call_ptr->is_waiting_on_esr_rsp &&
      call_ptr->direction == CM_CALL_DIRECTION_MO)
  {
    call_ptr->is_waiting_on_esr_rsp = FALSE;
    #ifdef CM_GW_SUPPORTED
    cmwcall_send_orig( call_ptr );
    #else
    CM_MSG_HIGH_1("Call %d dropped, mSIM without GW", call_ptr->call_id);
    #endif
    simc->esr_state = CMSIMC_ESR_TAU_PENDING;
  }
}



/* ============================================================================ */
/* ==========================END ULTILITIES FUNCTIONS ========================= */
/* ============================================================================ */

