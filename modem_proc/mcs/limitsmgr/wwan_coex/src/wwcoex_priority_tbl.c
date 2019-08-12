
/*!
  @file
  wwcoex_priority_tbl.c

  @brief
  This file contains the activity tier priorities and related functions

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-2016 Qualcomm Technologies Incorporated.
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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwcoex_priority_tbl.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/10/16   jm      Reducing F3 messages in Limits Manager
02/11/16   jm      Convert high-rate F3 messaging to off-target only
01/20/15   sg      Added partial support for WCDMA2 client
11/11/14   ag      Added api to combine Rx/Tx priorities for FDD tech
09/23/14   ag      Set different values to lowest priority for each tech to 
                   avoid equal priority crashes in corner cases.
09/10/14   jm      Modify Tx channel id value for STX-DSDA
08/04/14   jm      channel id support for Single-TX DSDA
08/01/14   ag      unified micro priority
07/31/14   ag      Added support for TIER_30
07/04/14   ag      data+mms support
06/13/14   jm      Reduce priority for HDR TIER_25 for QTA
03/21/14   rj      Reduced priority for W TIER_25 for split reg QTA
03/19/14   ag      Added TIER_3 with max priority for all techs
03/19/14   ag      Fix for equal priority upon late registration
03/14/14   ag      Removed static from priority table circular buffer
03/11/14   rj      Changes to avoid and debug GSM CXM Priority becomes same
01/15/14   rj      Adding support for SGLTE+G channel ID info 
01/08/14   ag      Adding priority table for LTE 
12/12/13   jm      LLVM Compiler Warning Fixes
12/07/13   ag      Updated priority table for T+G
11/16/13   rj      Adding macro prio callback support
09/09/13   ag      TIER_15 for W2G IRAT measurements
05/13/13   ag      Adding different priorities for G+G
03/27/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "wwan_coex_mgr.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"
#include "wwan_coex.h"


/*=============================================================================

                            TYPEDEFS

=============================================================================*/
#define WWCOEX_PRIO_MIN_VALUE 15

#define STX_DSDA_TX_CHANNEL_ID 7 /* Must be 3 bits max */

#define IS_CXM_GSM_TECH(tech_id) ((tech_id) == CXM_TECH_GSM1 || (tech_id) == CXM_TECH_GSM2 || (tech_id) == CXM_TECH_GSM3)

static trm_single_tx_dsda_enum_type is_trm_stx_dsda = TRM_SINGLE_TX_DSDA_MAX;

#ifdef FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY
  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_wcdma_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  204, 
    /* ACTIVITY_TIER_10 */  144,
    /* ACTIVITY_TIER_15 */  104, 
    /* ACTIVITY_TIER_20 */  64,
    /* ACTIVITY_TIER_25 */  49,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_onex_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  205, 
    /* ACTIVITY_TIER_10 */  145,
    /* ACTIVITY_TIER_15 */  105,
    /* ACTIVITY_TIER_20 */  65,
    /* ACTIVITY_TIER_25 */  50,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_hdr_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  204, 
    /* ACTIVITY_TIER_10 */  144,
    /* ACTIVITY_TIER_15 */  104, 
    /* ACTIVITY_TIER_20 */  64,
    /* ACTIVITY_TIER_25 */  49,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_gsm1_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  195, 
    /* ACTIVITY_TIER_10 */  160,
    /* ACTIVITY_TIER_15 */  120,
    /* ACTIVITY_TIER_20 */  80,
    /* ACTIVITY_TIER_25 */  40,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_gsm2_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  210, 
    /* ACTIVITY_TIER_10 */  170,
    /* ACTIVITY_TIER_15 */  130,
    /* ACTIVITY_TIER_20 */  95,
    /* ACTIVITY_TIER_25 */  55,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_tdscdma_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  204, 
    /* ACTIVITY_TIER_10 */  180,
    /* ACTIVITY_TIER_15 */  140,
    /* ACTIVITY_TIER_20 */  90,
    /* ACTIVITY_TIER_25 */  49,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_gsm3_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  185, 
    /* ACTIVITY_TIER_10 */  150,
    /* ACTIVITY_TIER_15 */  110,
    /* ACTIVITY_TIER_20 */  70,
    /* ACTIVITY_TIER_25 */  30,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  static const uint32 wwcoex_lte_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  204, 
    /* ACTIVITY_TIER_10 */  144,
    /* ACTIVITY_TIER_15 */  104, 
    /* ACTIVITY_TIER_20 */  64,
    /* ACTIVITY_TIER_25 */  49,
    /* ACTIVITY_TIER_30 */  20
  };

  /* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
  /* TODO: Revisit WCDMA2 priorities */
  static const uint32 wwcoex_wcdma2_activity_tbl[MAX_ACTIVITY_TIERS]=
  {
    /* ACTIVITY_TIER_3  */  220, 
    /* ACTIVITY_TIER_5  */  204, 
    /* ACTIVITY_TIER_10 */  144,
    /* ACTIVITY_TIER_15 */  104, 
    /* ACTIVITY_TIER_20 */  64,
    /* ACTIVITY_TIER_25 */  49,
    /* ACTIVITY_TIER_30 */  20
  };

#else /* not unified priority */

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_wcdma_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  200, 
   /* ACTIVITY_TIER_10 */  200,
   /* ACTIVITY_TIER_15 */  100, 
   /* ACTIVITY_TIER_20 */  100,
   /* ACTIVITY_TIER_25 */   41,
   /* ACTIVITY_TIER_30 */   41
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_onex_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  200, 
   /* ACTIVITY_TIER_10 */  200,
   /* ACTIVITY_TIER_15 */  100,
   /* ACTIVITY_TIER_20 */  100,
   /* ACTIVITY_TIER_25 */  100,
   /* ACTIVITY_TIER_30 */  100
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_hdr_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  199, 
   /* ACTIVITY_TIER_10 */  199,
   /* ACTIVITY_TIER_15 */  99,
   /* ACTIVITY_TIER_20 */  99,
   /* ACTIVITY_TIER_25 */  49,
   /* ACTIVITY_TIER_30 */  49
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_gsm1_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  190, 
   /* ACTIVITY_TIER_10 */  150,
   /* ACTIVITY_TIER_15 */  120,
   /* ACTIVITY_TIER_20 */  70,
   /* ACTIVITY_TIER_25 */  30,
   /* ACTIVITY_TIER_30 */  30
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_gsm2_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  210, 
   /* ACTIVITY_TIER_10 */  160,
   /* ACTIVITY_TIER_15 */  130,
   /* ACTIVITY_TIER_20 */  90,
   /* ACTIVITY_TIER_25 */  50,
   /* ACTIVITY_TIER_30 */  50
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_tdscdma_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  200, 
   /* ACTIVITY_TIER_10 */  170,
   /* ACTIVITY_TIER_15 */  100,
   /* ACTIVITY_TIER_20 */   80,
   /* ACTIVITY_TIER_25 */   40,
   /* ACTIVITY_TIER_30 */   40
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_gsm3_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  180, 
   /* ACTIVITY_TIER_10 */  140,
   /* ACTIVITY_TIER_15 */  110,
   /* ACTIVITY_TIER_20 */   60,
   /* ACTIVITY_TIER_25 */   20,
   /* ACTIVITY_TIER_30 */   20
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
static const uint32 wwcoex_lte_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220,
   /* ACTIVITY_TIER_5  */  201,
   /* ACTIVITY_TIER_10 */  201,
   /* ACTIVITY_TIER_15 */  101,
   /* ACTIVITY_TIER_20 */  101,
   /* ACTIVITY_TIER_25 */   41,
   /* ACTIVITY_TIER_30 */   41
};

/* Note: the priority values should always be > WWCOEX_PRIO_MIN_VALUE */
/* TODO: Revisit WCDMA2 priorities */
static const uint32 wwcoex_wcdma2_activity_tbl[MAX_ACTIVITY_TIERS]=
{
   /* ACTIVITY_TIER_3  */  220, 
   /* ACTIVITY_TIER_5  */  200, 
   /* ACTIVITY_TIER_10 */  200,
   /* ACTIVITY_TIER_15 */  100, 
   /* ACTIVITY_TIER_20 */  100,
   /* ACTIVITY_TIER_25 */   41,
   /* ACTIVITY_TIER_30 */   41
};

#endif

static const uint32* wwcoex_activity_tbls[(uint8)CXM_TECH_MAX]=
{
   wwcoex_lte_activity_tbl,
   wwcoex_tdscdma_activity_tbl,
   wwcoex_gsm1_activity_tbl,
   wwcoex_onex_activity_tbl,
   wwcoex_hdr_activity_tbl,
   wwcoex_wcdma_activity_tbl,
   wwcoex_gsm2_activity_tbl,
   wwcoex_gsm3_activity_tbl,
   wwcoex_wcdma2_activity_tbl
};

typedef struct 
{
  /* Callback for a tech */
  cxm_prio_table_cb_t  tech_prio_cb;

  /* Last Priority update info set */
  uint8  prio;

  /* Current offset that is added to the static prio value to ensure we dont
     return same prio as old value sent to other tech */
  uint8  curr_offset;

  /* indicates which tier is super critical for this tech */
  cxm_activity_type super_critical_tier;

  /* Current activity table */
  cxm_activity_table_s activity_tbl[MAX_ACTIVITY_TIERS];

  /* this flag is set once at bootup when the acitvity_tbl is first
     initialized  */
  boolean             initialized;
}cxm_prio_table_info;

static cxm_prio_table_info  tech_prio_info[CXM_TECH_MAX] = {{ 0 }};

/*----------------------------------------------------------------------------
  GSM Prio circular buffer
----------------------------------------------------------------------------*/
#define LMTSMGR_HIST_RECS  30

typedef struct 
{
  boolean is_register_req;

  cxm_tech_type tech;

  /* Priority value */
  uint32             priority;
  /* Last Priority update info set */
  uint8  tech_prio;
  /* */
  uint8  curr_offset;

  uint8 prio_upd_action;
}cxm_history_prio_table_info;

uint8 cxm_time_history_index = 0;

cxm_history_prio_table_info  tech_prio_info_history[LMTSMGR_HIST_RECS];

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/


/*=============================================================================

  FUNCTION:  lmtsmgr_update_client_prio_table_cb

=============================================================================*/
/*!
    @brief
    update client's prio_table_cb internally.
 
    @return
    None
*/
/*===========================================================================*/
static void cxm_update_tech_prio_history
(
  boolean is_register_req,
  cxm_tech_type       tech_id,
  uint8  tech_prio,
  uint8  curr_offset,
  /* Priority value */
  uint32             priority,
  uint8              prio_upd_action
)
{
  cxm_time_history_index %= LMTSMGR_HIST_RECS;
  tech_prio_info_history[cxm_time_history_index].is_register_req = is_register_req;
  tech_prio_info_history[cxm_time_history_index].tech = tech_id;
  tech_prio_info_history[cxm_time_history_index].tech_prio = tech_prio;
  tech_prio_info_history[cxm_time_history_index].curr_offset = curr_offset;
  tech_prio_info_history[cxm_time_history_index].priority = priority;
  tech_prio_info_history[cxm_time_history_index].prio_upd_action = prio_upd_action;
  cxm_time_history_index++;
}

/*=============================================================================

  FUNCTION:  cxm_init_tech_prio_table

=============================================================================*/
/*!
    @brief
    initialize the priority table for a tech
 
    @return
    None
*/
/*===========================================================================*/
static void cxm_init_tech_prio_table
(
  cxm_tech_type index
)
{
    cxm_activity_type tier;
  tech_prio_info[index].tech_prio_cb = NULL;
  tech_prio_info[index].curr_offset = 0;
  if (index == (uint8)CXM_TECH_GSM1)
  {
    tech_prio_info[index].prio = 1;
  }
  else if (index == (uint8)CXM_TECH_GSM2)
  {
    tech_prio_info[index].prio = 0;
  }
  else if (index == (uint8)CXM_TECH_GSM3)
  {
    tech_prio_info[index].prio = 2;
  }
 
  /* Store the initial table as current table */
  for (tier=0; tier<MAX_ACTIVITY_TIERS; tier++)
  {
    tech_prio_info[index].activity_tbl[tier].activity = tier;
    tech_prio_info[index].activity_tbl[tier].priority = wwcoex_activity_tbls[index][tier]; 
  }

  #ifdef FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY
  tech_prio_info[index].super_critical_tier = ACTIVITY_TIER_5;
  #else
  /* Set the super critical tier for each tech */
  switch (index)
  {
     case CXM_TECH_LTE:
     case CXM_TECH_ONEX:
     case CXM_TECH_HDR:
     case CXM_TECH_WCDMA:
       tech_prio_info[index].super_critical_tier = ACTIVITY_TIER_10;
       break;

     case CXM_TECH_TDSCDMA:
     case CXM_TECH_GSM1:
     case CXM_TECH_GSM2:
     case CXM_TECH_GSM3:
     default:
       tech_prio_info[index].super_critical_tier = ACTIVITY_TIER_5;
       break;
  }
  #endif

  /* set the init flag to TRUE */
  tech_prio_info[index].initialized = TRUE;
}
/*=============================================================================

  FUNCTION:  cxm_update_prio_table_init

=============================================================================*/
/*!
    @brief
    Init client's prio table info.
 
    @return
    None
*/
/*===========================================================================*/
void cxm_update_prio_table_init( void )
{
  uint8 index;

  for (index=0; index < (uint8)CXM_TECH_MAX; index++)
  {
    /* Update the prio table only if it is not initialized */
    if (tech_prio_info[index].initialized == FALSE)
    {
      cxm_init_tech_prio_table(index);
    }
  }
}

/*=============================================================================

  FUNCTION:  lmtsmgr_update_client_prio_table_cb

=============================================================================*/
/*!
    @brief
    update client's prio_table_cb internally.
 
    @return
    None
*/
/*===========================================================================*/
void cxm_update_prio_table
(
  cxm_tech_type       tech_id,
  uint8  prio,
  uint8  curr_offset,
  wwcoex_prio_table_upd_type prio_upd_action
)
{
  //cxm_activity_table_s activity_tbl[MAX_ACTIVITY_TIERS];
  const uint32* priority_tbl = NULL;
  uint32 i;

  if (!LMTSMGR_IS_TECH_VALID(tech_id))
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect tech_id %d for action %d in prio update api",
                  tech_id, prio_upd_action);
    return;
  }

  LMTSMGR_DBG_MSG_4(HIGH, "PrioTblUpdate: Tech %d Prio %d Offset %d ReqAction %d",
                          tech_id, prio, curr_offset, prio_upd_action);

  if (prio_upd_action == PRIO_TABLE_SET_MIN)
  {
    /* Set all tiers to MIN except the ASDIV tier i.e TIER_3 */
    for (i=ACTIVITY_TIER_3+1; i<MAX_ACTIVITY_TIERS; i++)
    {
      tech_prio_info[tech_id].activity_tbl[i].activity = i;
      /* Subtract tech_id so that the priority is unique for each tech */
      tech_prio_info[tech_id].activity_tbl[i].priority = WWCOEX_PRIO_MIN_VALUE - (uint8)tech_id; 
    }
  }
  else if (prio_upd_action == PRIO_TABLE_SET_MIN_EXCLUDE_SUPER_CRIT)
  {
    /* Set all tiers lower than super-critical tier to MIN  */
    for (i=tech_prio_info[tech_id].super_critical_tier+1; i<MAX_ACTIVITY_TIERS; i++)
    {
      tech_prio_info[tech_id].activity_tbl[i].activity = i;
      /* Subtract tech_id so that the priority is unique for each tech */
      tech_prio_info[tech_id].activity_tbl[i].priority = WWCOEX_PRIO_MIN_VALUE - (uint8)tech_id; 
    }
  }
  else if (prio_upd_action == PRIO_TABLE_RESET_PREV)
  {
    /* Reset the prio table to previous state */

    if (IS_CXM_GSM_TECH(tech_id))
    {
      /* Get table based on current macro prio index*/
      switch (tech_prio_info[tech_id].prio)
      {
        case 0:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM2];
          break;

        case 1:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM1];
          break;

        case 2:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM3];
          break;

        default:
          LMTSMGR_MSG_1(ERROR,"cxm_update_prio_table wrong prio %d ",
                             prio);
          break;
      }

      /* Use current offset from the last stored value */
      curr_offset = tech_prio_info[tech_id].curr_offset;
    }
    else /* For Non GSM Techs */
    {
      priority_tbl = wwcoex_activity_tbls[tech_id];
      curr_offset = 0;
    }

    if (priority_tbl != NULL)
    {
      for (i=0; i<MAX_ACTIVITY_TIERS; i++)
      {
        tech_prio_info[tech_id].activity_tbl[i].activity = i;
        tech_prio_info[tech_id].activity_tbl[i].priority = (priority_tbl[i] + curr_offset);
      }
    }
  }
  else if (prio_upd_action == PRIO_TABLE_MACRO_OFFSET_BASED)
  {
    /* Set the priority table based on macro prio index and offset */

    if (IS_CXM_GSM_TECH(tech_id))
    {
      /* Get new macro prio index */
      tech_prio_info[tech_id].prio = prio;
      tech_prio_info[tech_id].curr_offset = curr_offset;        

      /* Get table based on new macro prio index */
      switch (prio)
      {
        case 0:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM2];
          break;

        case 1:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM1];
          break;

        case 2:
          priority_tbl = wwcoex_activity_tbls[CXM_TECH_GSM3];
          break;

        default:
          LMTSMGR_MSG_1(ERROR,"cxm_update_prio_table wrong prio %d ",
                             prio);
          break;
      }

      if (priority_tbl != NULL)
      {
        for (i=0; i<MAX_ACTIVITY_TIERS; i++)
        {
          tech_prio_info[tech_id].activity_tbl[i].activity = i;
          tech_prio_info[tech_id].activity_tbl[i].priority = (priority_tbl[i] + curr_offset); 
        }
      }
    }
  }
  else
  {
    /* Shouldn't come here */
    LMTSMGR_MSG_1(FATAL, "Unexpected prio_set_enum value %d", prio_upd_action);
    return;
  }

  /* Send the activity table  */
  if (tech_prio_info[tech_id].tech_prio_cb != NULL)
  { 
    /* Send the activity tbl in the callback */
    tech_prio_info[tech_id].tech_prio_cb(tech_id, tech_prio_info[tech_id].activity_tbl);

    LMTSMGR_DBG_MSG_5(MED, "cxm_update_prio_table prio %d %d %d %d %d",
                           tech_prio_info[tech_id].activity_tbl[0].priority, 
                           tech_prio_info[tech_id].activity_tbl[1].priority, 
                           tech_prio_info[tech_id].activity_tbl[2].priority, 
                           prio, curr_offset);

    cxm_update_tech_prio_history(FALSE, tech_id, prio, curr_offset, 
                                 tech_prio_info[tech_id].activity_tbl[0].priority, 
                                 prio_upd_action);
  }
}

/*============================================================================

FUNCTION CXM_REGISTER_PRIO_TABLE_CB

DESCRIPTION
  Function used by tech L1 to register its priority table callback.
  To deregister, NULL can be passed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_register_prio_table_cb
(
  cxm_tech_type       tech_id,
  cxm_prio_table_cb_t  prio_cb
)
{
  if (tech_id > CXM_TECH_DFLT_INVLD && tech_id < CXM_TECH_MAX && prio_cb != NULL)
  {
    /* Update the prio table if it is not initialized... This is needed when
       tech task inits before limitsmgr and requests for the table */
    if (tech_prio_info[tech_id].initialized == FALSE)
    {
      cxm_init_tech_prio_table(tech_id);
    }

    /* Store the callback */
    tech_prio_info[tech_id].tech_prio_cb = prio_cb;

    /* Call tech callback with the current table */
    prio_cb(tech_id, tech_prio_info[tech_id].activity_tbl);

    LMTSMGR_MSG_4(MED,"cxm_register_prio_tbl_cb tech %d prio %d %d %d",
                  tech_id,
                  tech_prio_info[tech_id].activity_tbl[0].priority, 
                  tech_prio_info[tech_id].activity_tbl[1].priority, 
                  tech_prio_info[tech_id].activity_tbl[2].priority);
  }
  
  return TRUE;
}


/*============================================================================

FUNCTION CXM_GET_CHANNEL_ID

DESCRIPTION
  Function used by tech L1 to get channel ID correcponding to RF device.
       Eg. 
       input.rf_device = 0 
       rx_channel_id = 0
       tx_channel_id = 0

       input.rf_device = 1 ==> Secondary chain
       rx_channel_id = 1
       tx_channel_id = 1

DEPENDENCIES
  None

RETURN VALUE
  Channel ID for both Rx and Tx chain for RF device 

SIDE EFFECTS
  None

============================================================================*/
cxm_channel_out_type cxm_get_channel_id
(
  cxm_channel_in_type input
)
{
  trm_get_info_input_type trm_info_in;
  trm_get_info_return_type trm_info_out;
  cxm_channel_out_type output;
  
  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( !LMTSMGR_IS_TECH_VALID(input.techid) || 
       (input.rf_device == RFM_INVALID_DEVICE) || 
       (input.rf_device == RFM_MAX_DEVICES) )
  {
    ERR_FATAL("Incorrect tech %d or rf device %d in cxm_get_channel_id", 
               input.techid, input.rf_device,0 );
  }

  /* If Single-Tx DSDA setting is unknown */
  if (is_trm_stx_dsda == TRM_SINGLE_TX_DSDA_MAX)
  {
    /* Retrieve Single-TX DSDA setting
       This should only be queried once */
    trm_info_in.info_type = TRM_SINGLE_TX_CAPABILITY;
    trm_info_in.client_id = 0;
    trm_get_info(&trm_info_in, &trm_info_out);
    LMTSMGR_ASSERT(trm_info_out.info_type == TRM_SINGLE_TX_CAPABILITY);
    is_trm_stx_dsda = trm_info_out.info.single_tx_info.info;
  }

  /* In Single-Tx DSDA, Tx channel ids should be the same and
     not overlap any Rx channel id */
  output.tx_channel_id = (is_trm_stx_dsda == TRM_SINGLE_TX_DSDA_ENABLED)
                          ? (STX_DSDA_TX_CHANNEL_ID) : input.rf_device;
  output.rx_channel_id = input.rf_device;

  return output;
}

/*============================================================================

FUNCTION CXM_GET_COMBINED_PRIORITY

DESCRIPTION
  Function used by tech L1 to combine Tx and Rx priority. This is typically
  required for FDD techs in single Tx DSDA to ensure Rx and Tx priority are 
  equal and it doesnt cause double blanking.
  So in single Tx DSDA, this api returns a MAX priority in both Tx and Rx prio
  pointers. In other scenarios, it will not change the priority values.
  The api also provides option to enter arguments as TIERs or priority values.
  Please refer to enum cxm_combine_prio_input_type...
       
DEPENDENCIES
  None

RETURN VALUE
  FALSE if NULL pointers are passed as arguments
  TRUE otherwise with the priority/tier value being updated in the pointer 
  memory locations

SIDE EFFECTS
  None

============================================================================*/  
boolean cxm_get_combined_priority
(
  cxm_combine_prio_input_type  input
)
{
  uint8 final_prio;
  trm_get_info_input_type trm_info_in;
  trm_get_info_return_type trm_info_out;
  
  if ((input.rx_prio == NULL) || (input.tx_prio == NULL))
  {
    LMTSMGR_MSG_2(ERROR, "NULL pointer passed as either Rx prio ptr 0x%x or Tx prio ptr 0x%x",
	                      input.rx_prio, input.tx_prio);
    return FALSE;
  }
  
  /* If we are not in single Tx DSDA, no need to change priorities... */
  if (is_trm_stx_dsda == TRM_SINGLE_TX_DSDA_MAX)
  {
    /* Retrieve Single-TX DSDA setting
       This should only be queried once */
    trm_info_in.info_type = TRM_SINGLE_TX_CAPABILITY;
    trm_info_in.client_id = 0;
    trm_get_info(&trm_info_in, &trm_info_out);
    LMTSMGR_ASSERT(trm_info_out.info_type == TRM_SINGLE_TX_CAPABILITY);
    is_trm_stx_dsda = trm_info_out.info.single_tx_info.info;
  }
  if (is_trm_stx_dsda != TRM_SINGLE_TX_DSDA_ENABLED)
  {
    /* No change required to priorities */
    return TRUE;
  }
  
  /* We are in single TX DSDA, so lets check if inputs are TIER or priority based... */
  if (input.are_inputs_tier_based == TRUE)
  {
    /* For TIER based, do a MIN() to get the highest TIER */
    final_prio = MIN(*input.rx_prio, *input.tx_prio);
  }  
  else
  {
    /* For prio based, do a MAX() to get the highest priority value */
    final_prio = MAX(*input.rx_prio, *input.tx_prio);
  }
  
  /* Update both Rx and Tx with final prio */
  *input.rx_prio = final_prio;
  *input.tx_prio = final_prio;
  
  return TRUE;
}


