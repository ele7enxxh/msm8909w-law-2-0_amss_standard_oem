/*!
  @file
  cfcm_client.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2014-15 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_client.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/22/15   rj      New Client for GPS
01/06/15   rj      For loop to run till client's MAX enum
11/20/14   rj      Log Packet support added in CFCM
09/03/14   rj      Added monitor mask to F3
04/07/14   rj      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msg.h>
#include "cfcm_cfg.h"
#include "cfcm_msg.h"
#include "cfcmi.h"
#include "cfcm_client.h"
#include "cfcm_monitor.h"
#include "cfcm.h"
#include "cfcm_msgr.h"
#include "cfcm_qsh.h"


/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

cfcm_client_s cfcm_client;
STATIC cfcm_client_s* const cfcm_client_ptr = &cfcm_client;

/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/

/*! @brief String representation of monitor enum */
const char * cfcm_client_str[CFCM_CLIENT_MAX] =
{
  "LTE_UL",
  "LTE_RLC_DL",
  "A2_UL_PER/UL_PER",
  "LTE_ML1",
  "LTE_PDCP_DL",
  "LTE_PDCP_UL",
  "CM",
  "LTE_ML1_CPU",
  "HDR_DS",
  "WCDMA_RLC_DL",
  "WCDMA_RLC_UL",
  "GSM",
  "TDSCDMA_RLC_DL",
  "TDSCDMA_RLC_UL",
  "PM",
  "DS",
  "LTE_RLC_LEVEL1_DL",
  "GPS_MC"
};

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_client_init_client

==============================================================================*/
/*!
    @brief
    initialized one client.

    @return
    None
*/
/*============================================================================*/
static void cfcm_client_init_client
(
  cfcm_client_e client   /*!< client id */
)
{
  uint32 j;
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(client < CFCM_CLIENT_MAX);
  /*--------------------------------------------------------------------------*/
  cfcm_client_ptr->clients[(uint8)client].client_id           = CFCM_CLIENT_MAX;
  cfcm_client_ptr->clients[(uint8)client].fc_req              = 0;
  cfcm_client_ptr->clients[(uint8)client].monitor_mask        = 0;
  cfcm_client_ptr->clients[(uint8)client].fc_cb_ptr           = NULL;
  cfcm_client_ptr->clients[(uint8)client].last_cmd.cmd        = CFCM_CMD_FC_OFF;
  cfcm_client_ptr->clients[(uint8)client].last_cmd.step_timer = 
    CFCM_MONITOR_DEFAULT_STEP_TIMER;
  cfcm_client_ptr->clients[(uint8)client].last_down_step_timer = 
    CFCM_MONITOR_DEFAULT_STEP_TIMER;
  cfcm_client_ptr->clients[(uint8)client].stats.num_cmd_sent   = 0;
  cfcm_client_ptr->clients[(uint8)client].stats.num_down_cmd_sent = 0;

  for(j = 0; j < CFCM_MONITOR_MAX; j++)
  {
    cfcm_client_ptr->clients[(uint8)client].stats.monitor[j].max_fc_period = 0;
    cfcm_client_ptr->clients[(uint8)client].stats.monitor[j].total_fc_period = 0;
    cfcm_client_ptr->clients[(uint8)client].stats.monitor[j].last_fc_start_time = 0;
    cfcm_client_ptr->clients[(uint8)client].stats.monitor[j].fc_started = FALSE;
  }

  for(j = 0; j < CFCM_CLIENT_HIST_BUF_SZ; j++)
  {
    cfcm_client_ptr->clients[(uint8)client].cmd_hist[j].cmd = CFCM_CMD_FC_OFF;
    cfcm_client_ptr->clients[(uint8)client].cmd_hist[j].step_timer = 
      CFCM_MONITOR_DEFAULT_STEP_TIMER;
  }
  cfcm_client_ptr->clients[(uint8)client].latest_cmd_hist_idx = 
    CFCM_CLIENT_HIST_BUF_IDX_MASK;
} /* cfcm_client_init_client() */




/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_client_init

==============================================================================*/
/*!
    @brief
    initialized internal data structure at start-up.

    @return
    None

    @retval value
    OPTIONAL brief description of a particular return value goes here.
    This tag may be repeated to document a number of specific return values.
    - DELETE this section if unused

    @note
    OPTIONAL 'notes' about a function go here.
    - DELETE this section if unused

    @see related_function()
    OPTIONAL, this tag may be repeated to document a number related entities.
    - DELETE this section if unused

    @deprecated
    OPTIONAL Deprecation of usage can go here.
    - DELETE this section if unused
*/
/*============================================================================*/
EXTERN void cfcm_client_init( void )
{
  uint32 i;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  cfcm_client_ptr->num_errors = 0;
  for(i = 0; i < (uint32)CFCM_CLIENT_MAX; i++)
  {
    cfcm_client_init_client((cfcm_client_e)i);
  }
} /* cfcm_client_init() */


/*==============================================================================

  FUNCTION:  cfcm_client_proc_reg

==============================================================================*/
/*!
    @brief
    process client registration message.

    @detail
    An OPTIONAL detailed description of cfcm_client_proc_reg() goes here.

    And can span multiple paragraphs, if necessary.
    - Bulleted lists are OK
      - Nesting of bullets is OK
        -# Numbered lists work, too
        -# That's it!

    DELETE this section if unused

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_client_proc_reg
(
  const cfcm_reg_req_msg_type_s* msg_ptr   /*!< pointer to the registration message */
)
{
  uint32 client_idx;
  const cfcm_reg_req_type_s*    cfcm_req = NULL;
  /*--------------------------------------------------------------------------*/
  if (IS_CFCM_DISABLED(cfcm.cfcm_disable))
  {
    CFCM_MSG_1( ERROR, "cfcm_client_proc_reg: CFCM Disabled %d ", cfcm.cfcm_disable );
    return;
  }

  CFCM_ASSERT(msg_ptr != NULL);

  cfcm_req = &msg_ptr->cfcm_req;

  CFCM_ASSERT(cfcm_req != NULL);
  CFCM_ASSERT(CFCM_IS_CLIENT_VALID(cfcm_req->client_id));
  /*--------------------------------------------------------------------------*/

  client_idx = (uint32)cfcm_req->client_id;
  /* update the client data structure */
  if(cfcm_client_ptr->clients[client_idx].client_id == CFCM_CLIENT_MAX)
  {
    CFCM_MSG_4(HIGH, "CFCM proc registration from client=%d, cb_ptr = 0x%x, umid=0x%x, mask=0x%x",
              client_idx,
              cfcm_req->req_cb,
              cfcm_req->req_umid,
              cfcm_req->monitor_mask);
  }
  else
  {
    CFCM_MSG_1(ERROR, "CFCM Registration for a client=%d already registed: Ignored",
              client_idx);
    cfcm_client_ptr->num_errors++;
    return;
  }

  cfcm_client_ptr->clients[client_idx].client_id    = cfcm_req->client_id;
  cfcm_client_ptr->clients[client_idx].fc_cb_ptr    = cfcm_req->req_cb;
  cfcm_client_ptr->clients[client_idx].fc_req       = cfcm_req->req_umid;
  cfcm_client_ptr->clients[client_idx].monitor_mask = cfcm_req->monitor_mask;

  /* update monitor(s) about this new client */
  cfcm_monitor_proc_reg(cfcm_req);

  /* update flow control command if necessary */
  cfcm_client_update_fc_cmd(cfcm_req->client_id);
} /* cfcm_client_proc_reg() */

/*==============================================================================

  FUNCTION:  cfcm_client_proc_dereg

==============================================================================*/
/*!
    @brief
    process client deregistration message.

    @detail
    An OPTIONAL detailed description of cfcm_client_proc_reg() goes here.

    And can span multiple paragraphs, if necessary.
    - Bulleted lists are OK
      - Nesting of bullets is OK
        -# Numbered lists work, too
        -# That's it!

    DELETE this section if unused

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_client_proc_dereg
(
  const cfcm_dereg_req_msg_type_s* msg_ptr/*!< pointer to the registration message */
)
{
  uint32 client_idx;
  const cfcm_dereg_req_type_s*  dereg_req = NULL;

  /*--------------------------------------------------------------------------*/
  if (IS_CFCM_DISABLED(cfcm.cfcm_disable))
  {
    CFCM_MSG_1( ERROR, "cfcm_client_proc_dereg: CFCM Disabled %d ", cfcm.cfcm_disable );
    return;
  }

  CFCM_ASSERT(msg_ptr != NULL);

  dereg_req = &msg_ptr->client;

  CFCM_ASSERT(dereg_req != NULL);
  CFCM_ASSERT(CFCM_IS_CLIENT_VALID(dereg_req->client_id));
  /*--------------------------------------------------------------------------*/

  client_idx = (uint32)dereg_req->client_id;
  /* update the client data structure */
  if(cfcm_client_ptr->clients[client_idx].client_id != CFCM_CLIENT_MAX)
  {
    CFCM_MSG_1(HIGH, "CFCM proc deregistration from client=%d", client_idx);
  }
  else
  {
    CFCM_MSG_1(ERROR, "CFCM Deregistration for a client=%d not registed: Ignored",
              client_idx);
    cfcm_client_ptr->num_errors++;
    return;
  }

  cfcm_monitor_proc_dereg(dereg_req,
                         cfcm_client_ptr->clients[client_idx].monitor_mask);

  cfcm_client_init_client(dereg_req->client_id);
} /* cfcm_client_proc_reg() */


/*==============================================================================

  FUNCTION:  cfcm_client_compute_fc_cmd

==============================================================================*/
/*!
    @brief
    compute the flow control command based on current monitor states.

    @detail
    Also send flow control command if it is different from the last command sent

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_client_update_fc_cmd
(
  cfcm_client_e          client   /*!< the client id */
)
{
  cfcm_cmd_msg_type_s     fc_cmd_msg;
  cfcm_cmd_type_s         fc_cmd;
  cfcm_client_info_s*   client_ptr;
  errno_enum_type      send_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  client_ptr = &cfcm_client_ptr->clients[(uint32)client];

  cfcm_monitor_compute_fc_cmd(&fc_cmd, client_ptr->monitor_mask, client);

  /* Expectation here is that UP command is not followed by SHUTDOWN since 
        a monitor will move SHUTDOWN-->DOWN-->UP format */
  if(fc_cmd.cmd == CFCM_CMD_UP)
  {
    /* Use the step_timer corresponding to the monitor that resulted the last 
    a DOWN or SET_MIN command */
    fc_cmd.step_timer = client_ptr->last_down_step_timer;
  }

  fc_cmd.client_id = client;

  if((fc_cmd.cmd != client_ptr->last_cmd.cmd) ||
     (fc_cmd.step_timer != client_ptr->last_cmd.step_timer)||
     (fc_cmd.monitors_active != client_ptr->last_cmd.monitors_active))
  {
    /* update the last cmd */
    client_ptr->last_cmd.client_id = client;
    client_ptr->last_cmd.cmd = fc_cmd.cmd;
    client_ptr->last_cmd.step_timer = fc_cmd.step_timer;
    client_ptr->last_cmd.monitors_active = fc_cmd.monitors_active;

    /* update statistics */
    client_ptr->stats.num_cmd_sent++;

    if(client_ptr->last_cmd.cmd >= CFCM_CMD_DOWN)
    {
      client_ptr->last_down_step_timer = client_ptr->last_cmd.step_timer;
      client_ptr->stats.num_down_cmd_sent++;
    }

    /* send new flow control command to the client */
    if(cfcm_cfg_client_enabled(client))
    {
      client_ptr->latest_cmd_hist_idx++;
      client_ptr->latest_cmd_hist_idx &= CFCM_CLIENT_HIST_BUF_IDX_MASK;
      client_ptr->cmd_hist[client_ptr->latest_cmd_hist_idx].cmd = fc_cmd.cmd;
      client_ptr->cmd_hist[client_ptr->latest_cmd_hist_idx].client_id = client;
      client_ptr->cmd_hist[client_ptr->latest_cmd_hist_idx].step_timer = 
        fc_cmd.step_timer;
      client_ptr->cmd_hist[client_ptr->latest_cmd_hist_idx].monitors_active = fc_cmd.monitors_active;

      if(client_ptr->fc_cb_ptr == NULL)
      {
        /* notification by message via msgr */
        fc_cmd_msg.fc_cmd = fc_cmd;

        send_status = cfcm_msgr_send_msg(&fc_cmd_msg.hdr,
                                   client_ptr->fc_req,
                                   sizeof(cfcm_cmd_msg_type_s));
        CFCM_ASSERT(send_status == E_SUCCESS);
      }
      else
      {
        (client_ptr->fc_cb_ptr)(&fc_cmd);
      }
#ifndef LOG_MCS_CFCM_COMMAND_INFO_C
      CFCM_MSG_4(HIGH, "CFCM issued FC command to client=%d, cmd=%d, step_timer=%d, monitors=%d",
                (uint32)client,
                (uint32)fc_cmd.cmd,
                fc_cmd.step_timer, fc_cmd.monitors_active);
#endif /* LOG_MCS_CFCM_COMMAND_INFO_C */
    }
    else
    {
      CFCM_MSG_4(ERROR, "CFCM un-issued FC command to client=%d, cmd=%d, step_timer=%d, monitors=%d",
                (uint32)client,
                (uint32)fc_cmd.cmd,
                fc_cmd.step_timer, fc_cmd.monitors_active);
    }
  }
} /* cfcm_client_compute_fc_cmd() */

/*==============================================================================

  FUNCTION:  cfcm_client_get_current_history_idxs

==============================================================================*/
/*!
    @brief
    Return data with current History Index for ALL CLIENTS, so that one can check whether Client command is updated, 
    this is being used for Log packets

    @return
    return data related to current history index

*/
/*============================================================================*/
void cfcm_client_get_current_history_idxs( uint32* hist_idx )
{
  uint32 i;
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(hist_idx != NULL);
  /*--------------------------------------------------------------------------*/
  for(i = 0; i < (uint32)CFCM_CLIENT_MAX; i++)
  {
    hist_idx[i] = cfcm_client_ptr->clients[i].latest_cmd_hist_idx;
  }
} /* cfcm_client_get_current_history_idxs() */


/*==============================================================================

  FUNCTION:  cfcm_client_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_client_qsh_analysis
(
  void
)
{
  uint32 i;
    
  /* Iterate through clients */
  for (i = 0; i < CFCM_CLIENT_MAX; i++)
  {
    /* Log a message only if client has ever been flow controlled */
    if(cfcm_client_ptr->clients[i].stats.num_down_cmd_sent > 0)
    {
      /* The below format is used for easy readability on Qsherlock */
      QSH_LOG(
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "Client %s: Last State=%s Total FC cmd sent=%d",
        cfcm_client_str[i],
        cfcm_monitor_get_cmd_string(cfcm_client_ptr->clients[i].last_cmd.cmd),
        cfcm_client_ptr->clients[i].stats.num_down_cmd_sent);
      
      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_CPU].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_CFG,
        QSH_MSG_TYPE_INFO,
        "  CPU limits(pct): HIGH:%d; MEDIUM:%d; LOW:%d",
        cfcm_cfg.config_efs[i].cpu_thrshld.tier_high,
        cfcm_cfg.config_efs[i].cpu_thrshld.tier_medium,
        cfcm_cfg.config_efs[i].cpu_thrshld.tier_low);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_CPU].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_BW_THROTTLING].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  Max FC time(ms)/Total FC time(ms): CPU:%d/%d; BW:%d/%d; THER_PA:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_CPU].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_CPU].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_BW_THROTTLING].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_BW_THROTTLING].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA].total_fc_period);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EX].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EM].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  Max FC time(ms)/Total FC time(ms): THER_PA_EX:%d/%d; THER_PA_EM:%d/%d; THER_RA:%d/%d; THER_RA_EX:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EX].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EX].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EM].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_PA_EM].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_RUNAWAY].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_RUNAWAY].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_RUNAWAY_EX].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_THERMAL_RUNAWAY_EX].total_fc_period);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP_EX].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST_EX].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  Max FC time(ms)/Total FC time(ms): MDM_TEMP:%d/%d; MDM_TEMP_EX:%d/%d; VDD_PEAK:%d/%d; VDD_PEAK_EX:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP_EX].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_MDM_TEMP_EX].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST_EX].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_VDD_PEAK_CURR_EST_EX].total_fc_period);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_DL].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_DL].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_SMALL_POOL_DL].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  DL DSM Max FC time(ms)/Total FC time(ms): LARGE:%d/%d; DUP:%d/%d; SMALL:%d/%d; IPA:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_DL].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_DL].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_SMALL_POOL_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_SMALL_POOL_DL].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL].total_fc_period);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  DL DSM Max FC time(ms)/Total FC time(ms): LARGE_LVL1:%d/%d; DUP_LVL1:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL].total_fc_period);

      QSH_CHECK_LOG(
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_UL].max_fc_period != 0 ||
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_UL].max_fc_period != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "  UL DSM Max FC time(ms)/Total FC time(ms): LARGE:%d/%d; DUP:%d/%d",
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_UL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_LARGE_POOL_UL].total_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_UL].max_fc_period,
        cfcm_client_ptr->clients[i].stats.monitor[CFCM_MONITOR_DSM_DUP_POOL_UL].total_fc_period);
    }
  }
}

/*==============================================================================

  FUNCTION:  cfcm_client_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
EXTERN void cfcm_client_qsh_reset
(
  void
)
{
  uint32 i, j;
 
  for (i = 0; i < CFCM_CLIENT_MAX; i ++)
  {
    for (j = 0; j < CFCM_MONITOR_MAX; j++)
    {
      cfcm_client_ptr->clients[i].stats.monitor[j].max_fc_period = 0;
      cfcm_client_ptr->clients[i].stats.monitor[j].total_fc_period = 0;
      cfcm_client_ptr->clients[i].stats.monitor[j].last_fc_start_time = 0;
      cfcm_client_ptr->clients[i].stats.monitor[j].fc_started = FALSE;
    }
  }
}

/*==============================================================================

  FUNCTION:  cfcm_cap_ul_data_rate

==============================================================================*/
/*!
    @brief
    Cap the UL data rate for the specified modem radio access technoloy

    @detail
    - this API is intended for external entity to control the UL data rate for
    the specified radio access technology. The external entity is expected to
    assume all responsibility for thermal mitigation on the modem for the RAT. 

    - When this API is called, it is expected that LTE internal thermal 
    mitigation mechanism is not enabled. Otherwise, data_rate will not be 
    applied and the functino will return FALSE. 

    - The data rate convergence will be a best effort appoximation over a
    period of time in hundreds ms. SRB and RLC control PDUs are excluded, 
    in other words, only application level data is accounted for.

    - The data rate is sticky. At power UP, CFCM_MAX_DATA_RATE is used, it is 
    going to be changed with this API call only. 

    - setting data rate to 0 will effectively prevent application data from
    being transmitted. Use with caution.

    @return
    TRUE if data rate is successfully set. FALSE otherwise. 
*/
/*============================================================================*/
EXTERN boolean cfcm_cap_ul_data_rate
(
  cfcm_modem_rat_e rat, /*!< the radio access technology rate is applied to */
  uint32          data_rate/*!< maximum data rate in number of byte per ms.  
                   Using CFCM_MAX_DATA_RATE  will turn off flow control */
)
{
  cfcm_cmd_msg_type_s     fc_cmd_msg;
  errno_enum_type      send_status;
  cfcm_client_info_s*   client_ptr;
  boolean              status = TRUE;
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(rat == CFCM_MODEM_RAT_LTE);
  /*--------------------------------------------------------------------------*/
  client_ptr = &cfcm_client_ptr->clients[(uint32)CFCM_CLIENT_LTE_UL];

  if(client_ptr->client_id == CFCM_CLIENT_LTE_UL)
  {
    /* LTE MAC has registered */
    if(!cfcm_monitor_registered(CFCM_MONITOR_THERMAL_PA))
    {
      /* proceed only if internal TM is not intended to be used */
      
      fc_cmd_msg.fc_cmd.cmd = CFCM_CMD_SET_VALUE;
      fc_cmd_msg.fc_cmd.data_rate = data_rate;

      send_status = cfcm_msgr_send_msg(&fc_cmd_msg.hdr,
                                   client_ptr->fc_req,
                                   sizeof(cfcm_cmd_msg_type_s));
      CFCM_ASSERT(send_status == E_SUCCESS);

      CFCM_MSG_1(HIGH, "cfcm_cap_ul_data_rate data rate = %d", data_rate);
    }
    else
    {
      status = FALSE;
      cfcm_client_ptr->num_errors++;
      CFCM_ERR_FATAL("cfcm_cap_ul_data_rate failure: internal TM enabled", 0, 0, 0);
    }
  }
  else
  {
    status = FALSE;
    cfcm_client_ptr->num_errors++;
    CFCM_MSG_0(HIGH, "cfcm_cap_ul_data_rate failure: LTE MAC task not ready");
  }
  return status;
} /* cfcm_cap_ul_data_rate() */


/*==============================================================================

                                UNIT TEST

==============================================================================*/

