/*!
  @file
  lmtsmgr_diag.c

  @brief
  This file implements the diag functionality of the Limits Manager task.

  @ingroup per_implementation
*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/src/lmtsmgr_diag.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/10/14   jm      Slot Level activity log support for frame boundary
10/07/14   jm      Proper featurization for slot level activity tl log packet
08/25/14   jm      Circular buffer for slot level activity tl
08/12/14   jm      Slot level activity timeline support
04/28/14   jm      L+G Band Avoidance support
01/30/14   rj      Fix for action table not updated in logging
03/02/14   ag      Added type to power info packet and updated state info to v2
                   for the change in action enum order.
05/05/13   ag      Changed log packet name
04/07/13   ag      Added log pkt for power and reduced size of state log pkt
03/28/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"   
#include "lmtsmgr_diag.h"
/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#define WWCOEX_POWER_INFO_LOG_VERSION 2 /*... v2 for adding type field */
#define WWCOEX_STATE_INFO_LOG_VERSION 3 /* ...v3 for spur update       */
#define WWCOEX_BA_SERV_POWER_INFO_LOG_VERSION 1
#define WWCOEX_BA_BLIST_INFO_LOG_VERSION 1
#define WWCOEX_SLOT_ACT_TL_INFO_LOG_VERSION 3
#define WWCOEX_LOG_SUBCODE 1

/*=============================================================================

                                FUNCTIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  wwcoex_log_power

=============================================================================*/
/*!
    @brief
    Logs the power and frequency information for a particular tech
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_power
(
  cxm_tech_type tech_id,
  uint8 subcode
)
{
#ifdef LOG_MCS_WWCOEX_POWER_INFO_C
  wwcoex_power_log_type *log_ptr=NULL;
  lmtsmgr_tech_link_type* link_ptr=NULL;
  int j;
  uint32 log_size = 0;

    /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(LOW, "Invalid tech %d sent for logging", tech_id);
    return;
  }

  /* Find out how much size we need to allocate */
  log_size = FPOS(wwcoex_power_log_type, tech_list.links);
  log_size += lmtsmgr.tech_state[tech_id].currList.num_entries * 
                                    sizeof(wwcoex_log_freq_link_type);

  log_ptr = (wwcoex_power_log_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_WWCOEX_POWER_INFO_C,
                                       log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = WWCOEX_POWER_INFO_LOG_VERSION;
    log_ptr->power_hdr.subcode = subcode;
    log_ptr->power_hdr.tech_id = (uint8)tech_id;
    log_ptr->power_hdr.num_entries = lmtsmgr.tech_state[tech_id].currList.num_entries;

    /* Lets fill freqIds of all the active techs */
    for (j=0; j < log_ptr->power_hdr.num_entries; j++)
    {
      link_ptr = &lmtsmgr.tech_state[tech_id].currList.links[j];

      log_ptr->tech_list.links[j].freqId = link_ptr->freqInfo.freqid;
      log_ptr->tech_list.links[j].frequency = link_ptr->freqInfo.link_info.frequency;
      log_ptr->tech_list.links[j].bandwidth = link_ptr->freqInfo.link_info.bandwidth;
      log_ptr->tech_list.links[j].direction = (uint8)link_ptr->freqInfo.link_info.direction;
      log_ptr->tech_list.links[j].band = (uint8)link_ptr->freqInfo.link_info.band;
      log_ptr->tech_list.links[j].type = (uint8)link_ptr->freqInfo.link_info.type;

      if (log_ptr->tech_list.links[j].direction == (uint32)CXM_LNK_DRCTN_UL)
      {
        log_ptr->tech_list.links[j].power = link_ptr->powerInfo.tx_power;
      }
      else if (log_ptr->tech_list.links[j].direction == (uint32)CXM_LNK_DRCTN_DL)
      {
        log_ptr->tech_list.links[j].power = link_ptr->powerInfo.rx_power;
      }
      else
      {
        log_ptr->tech_list.links[j].power = CXM_UNKNOWN_POWER;
      }
    }

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Could not allocate WWcoex pwr log pkt of size %d",
                          log_size );
  }
#else
  LMTSMGR_UNUSED(tech_id);
#endif /* LOG_MCS_WWCOEX_POWER_INFO_C */
}
/*=============================================================================

  FUNCTION:  wwcoex_log_state

=============================================================================*/
/*!
    @brief
    Logs all the WWAN Coex state information
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_state
(
  cxm_tech_type tech1,
  cxm_tech_type tech2,
  wwcoex_tbl_type* result_tbl
)
{
#ifdef LOG_MCS_WWCOEX_STATE_INFO_C
  wwcoex_state_log_type *log_ptr=NULL;
  uint32 t1index, t2index, index;
  uint32 log_size = 0;
  
  if (( tech1 <= CXM_TECH_DFLT_INVLD ) || ( tech1 >= CXM_TECH_MAX ) ||
      ( tech2 <= CXM_TECH_DFLT_INVLD ) || ( tech2 >= CXM_TECH_MAX ) ||
      (result_tbl == NULL) || (result_tbl->is_valid == FALSE) )
  {
    LMTSMGR_MSG_3(ERROR, "Incorrect arguments %d %d, 0x%x to log coex state",
                  tech1, tech2, result_tbl)
    return ;
  }

  /* Find out how much size we need to allocate */
  log_size = FPOS(wwcoex_state_log_type, scenario_tbls.current_tbl);
  log_size += result_tbl->tech1_num_entries * result_tbl->tech2_num_entries *
                                            sizeof(wwcoex_log_arr_entry_type);

  log_ptr = (wwcoex_state_log_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_WWCOEX_STATE_INFO_C,
                                               log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = WWCOEX_STATE_INFO_LOG_VERSION;
    log_ptr->state_hdr.subcode = WWCOEX_LOG_SUBCODE;

    log_ptr->state_hdr.tech1 = (uint8)tech1;
    log_ptr->state_hdr.tech2 = (uint8)tech2; 

    /* Fill up the current table */
    log_ptr->state_hdr.tech1_curr_fid_offset = result_tbl->tech1_fid_offset;
    log_ptr->state_hdr.tech2_curr_fid_offset = result_tbl->tech2_fid_offset;

    log_ptr->state_hdr.num_t1entries = (uint8)result_tbl->tech1_num_entries;
    log_ptr->state_hdr.num_t2entries = (uint8)result_tbl->tech2_num_entries;

    index = 0;
    for (t1index=0; t1index < result_tbl->tech1_num_entries; t1index++)
    {
      for (t2index=0; t2index < result_tbl->tech2_num_entries; t2index++)
      {
        log_ptr->scenario_tbls.current_tbl[index].action = 
                                     result_tbl->arr[t1index][t2index].action;
        log_ptr->scenario_tbls.current_tbl[index].desense = 
                                     result_tbl->arr[t1index][t2index].desense;
        log_ptr->scenario_tbls.current_tbl[index].spur_handle1 = 
                                     result_tbl->arr[t1index][t2index].spur_handle_1;
        log_ptr->scenario_tbls.current_tbl[index].spur_handle2 = 
                                     result_tbl->arr[t1index][t2index].spur_handle_2;
        index++;
      }
    }

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Could not allocate log pkt of size %d", log_size);
  }
#else
  LMTSMGR_UNUSED(result_tbl);
  LMTSMGR_UNUSED(tech1);
  LMTSMGR_UNUSED(tech2);
#endif /* LOG_MCS_WWCOEX_STATE_INFO_C */
}

/*=============================================================================

  FUNCTION:  wwcoex_log_serv_power

=============================================================================*/
/*!
    @brief
    Logs the serving power and frequency information for a particular tech
    (ie for Band Avoidance)
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_serv_power
(
  cxm_tech_type tech_id,
  uint8 subcode
)
{
#ifdef TEST_FRAMEWORK
  #error code not present
#elif defined(LOG_MCS_WWCOEX_BA_SERV_POWER_INFO_C)
  wwcoex_ba_serv_power_log_type *log_ptr=NULL;
  lmtsmgr_tech_ba_type* serv_freq_list;
  uint32 i, offset, log_size = 0;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(LOW, "Invalid tech %d sent for logging", tech_id);
    return;
  }

  serv_freq_list = &lmtsmgr.band_avoidance.tech_info[tech_id];
  if (subcode == WWCOEX_LOG_FREQUENCY_SUBCODE ||
      subcode == WWCOEX_LOG_POWER_SUBCODE)
  {
    if ((serv_freq_list->num_dl_entries != serv_freq_list->num_ul_entries) ||
        (serv_freq_list->num_dl_entries + serv_freq_list->num_ul_entries !=
         serv_freq_list->num_entries))
    {
      LMTSMGR_MSG_4(LOW, "Invalid entries for tech %d: UL: %d DL: %d Total: %d",
                    tech_id, serv_freq_list->num_ul_entries, serv_freq_list->num_dl_entries,
                    serv_freq_list->num_entries);
      return;
    }

    /* Find out how much size we need to allocate */
    log_size = FPOS(wwcoex_ba_serv_power_log_type, u.serv_freq_list.entries);
    log_size += lmtsmgr.band_avoidance.tech_info[tech_id].num_entries * 
                                      sizeof(wwcoex_log_serv_freq_entry_type);

    log_ptr = (wwcoex_ba_serv_power_log_type *)log_alloc_ex( 
                                         (log_code_type) LOG_MCS_WWCOEX_BA_SERV_POWER_INFO_C,
                                         log_size );

    if ( log_ptr != NULL )
    {
      log_ptr->version = WWCOEX_BA_SERV_POWER_INFO_LOG_VERSION;
      log_ptr->power_hdr.subcode = subcode;
      log_ptr->power_hdr.tech_id = (uint8)tech_id;
      log_ptr->power_hdr.num_entries = serv_freq_list->num_entries;

      /* Enter entries for UL */
      for (i=0; i<serv_freq_list->num_ul_entries; i++)
      {
        log_ptr->u.serv_freq_list.entries[i].frequency = serv_freq_list->serv_info_ul_list[i].frequency;
        log_ptr->u.serv_freq_list.entries[i].bandwidth = serv_freq_list->serv_info_ul_list[i].bandwidth;
        log_ptr->u.serv_freq_list.entries[i].band = (uint8) serv_freq_list->serv_info_ul_list[i].band;
        log_ptr->u.serv_freq_list.entries[i].power = serv_freq_list->serv_info_ul_list[i].power;
        log_ptr->u.serv_freq_list.entries[i].direction = (uint8) CXM_LNK_DRCTN_UL;
      }

      /* Enter entries for DL */
      for (i=0; i<serv_freq_list->num_dl_entries; i++)
      {
        offset = i + serv_freq_list->num_ul_entries;
        log_ptr->u.serv_freq_list.entries[offset].frequency = serv_freq_list->serv_info_dl_list[i].frequency;
        log_ptr->u.serv_freq_list.entries[offset].bandwidth = serv_freq_list->serv_info_dl_list[i].bandwidth;
        log_ptr->u.serv_freq_list.entries[offset].band = (uint8) serv_freq_list->serv_info_dl_list[i].band;
        log_ptr->u.serv_freq_list.entries[offset].power = serv_freq_list->serv_info_dl_list[i].power;
        log_ptr->u.serv_freq_list.entries[offset].direction = (uint8) CXM_LNK_DRCTN_DL;
      }

      /* submit the log to DIAG */
      log_commit( log_ptr );
    }
    else
    {
      LMTSMGR_MSG_1(MED, "Could not allocate WWcoex serv pwr log pkt of size %d",
                            log_size );
    }
  }
  else if (subcode == WWCOEX_LOG_BLIST_RSP_SUBCODE)
  {
    /* Find out how much size we need to allocate */
    log_size = FPOS(wwcoex_ba_serv_power_log_type, u.bl_rsp); /* Reservation for Header */
    log_size += sizeof(wwcoex_log_tech_blist_rsp_type);

    log_ptr = (wwcoex_ba_serv_power_log_type *)log_alloc_ex( 
                                         (log_code_type) LOG_MCS_WWCOEX_BA_SERV_POWER_INFO_C,
                                         log_size );

    if ( log_ptr != NULL )
    {
      log_ptr->version = WWCOEX_BA_SERV_POWER_INFO_LOG_VERSION;
      log_ptr->power_hdr.subcode = subcode;
      log_ptr->power_hdr.tech_id = (uint8)tech_id;
      log_ptr->power_hdr.num_entries = 1;
      log_ptr->u.bl_rsp.bl_id = (uint8) serv_freq_list->blist_rsp.bl_id;
      log_ptr->u.bl_rsp.response = (uint8) serv_freq_list->blist_rsp.response;

      /* submit the log to DIAG */
      log_commit( log_ptr );
    }
    else
    {
      LMTSMGR_MSG_1(MED, "Could not allocate WWcoex BList Rsp log pkt of size %d",
                            log_size );
    }
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Invalid WWcoex Serv Power Info subcode: %d", subcode);
  }
#else
  LMTSMGR_UNUSED(tech_id);
  LMTSMGR_UNUSED(subcode);
#endif /* LOG_MCS_WWCOEX_BA_SERV_POWER_INFO_C */
}


/*=============================================================================

  FUNCTION:  wwcoex_log_blacklist

=============================================================================*/
/*!
    @brief
    Logs the blacklist information (ie for Band Avoidance)
 
    @details
    This is used for MCS->WWAN
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_blacklist
(
  cxm_tech_type tech_id
)
{

#ifdef TEST_FRAMEWORK
  #error code not present
#elif defined(LOG_MCS_WWCOEX_BA_BLACKLIST_INFO_C)
  wwcoex_ba_blist_log_type *log_ptr=NULL;
  coex_ba_blist_data* curr_bl;
  uint32 i, log_size = 0;

    /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(LOW, "Invalid tech %d sent for logging", tech_id);
    return;
  }

  curr_bl = &lmtsmgr.band_avoidance.currBlacklist;

  if (curr_bl == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "NULL Blacklist!");
    return;
  }

  /* Find out how much size we need to allocate */
  log_size = FPOS(wwcoex_ba_blist_log_type, blacklist.entries);
  log_size += curr_bl->num_entries * sizeof(wwcoex_log_ba_list_entry_type);

  log_ptr = (wwcoex_ba_blist_log_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_WWCOEX_BA_BLACKLIST_INFO_C,
                                       log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = WWCOEX_BA_BLIST_INFO_LOG_VERSION;
    log_ptr->blist_hdr.tech_id = (uint8) tech_id;
    log_ptr->blist_hdr.blacklist_id = (uint8) curr_bl->blacklist_id;
    log_ptr->blist_hdr.serv_freq_state = (uint8) curr_bl->serv_freq_state;
    log_ptr->blist_hdr.serv_freq_mode = (uint8) curr_bl->serv_freq_mode;
    log_ptr->blist_hdr.num_entries = (uint8) curr_bl->num_entries;

    for (i=0; i<curr_bl->num_entries; i++)
    {
      log_ptr->blacklist.entries[i].band = (uint8) curr_bl->freq_list[i].band;
      log_ptr->blacklist.entries[i].freq_lower = curr_bl->freq_list[i].freq_lower;
      log_ptr->blacklist.entries[i].freq_upper = curr_bl->freq_list[i].freq_upper;
      log_ptr->blacklist.entries[i].desense_mode = (uint8) curr_bl->freq_list[i].desense_mode;
      log_ptr->blacklist.entries[i].rxpwr_thresh = curr_bl->freq_list[i].rxpwr_thresh;
      log_ptr->blacklist.entries[i].txpwr_thresh = curr_bl->freq_list[i].txpwr_thresh;
    }

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Could not allocate WWcoex BA Blacklist log pkt of size %d",
                          log_size );
  }
#else
  LMTSMGR_UNUSED(tech_id);
#endif /* LOG_MCS_WWCOEX_BA_BLACKLIST_INFO_C */
}

/*=============================================================================

  FUNCTION:  wwcoex_log_sl_act_tl

=============================================================================*/
/*!
    @brief
    Logs the slot level activity timeline
 
    @details
    L1->MCS sends the bulk of this information but MCS adds
    potential conflict type to each activity slot
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_sl_act_tl
(
  cxm_tech_type rec_tech
)
{
#ifdef LOG_MCS_WWCOEX_SLOT_ACTIVITY_TIMELINE_INFO_C
  wwcoex_timing_info_log_type *log_ptr=NULL;
  cxm_timing_info_s* timing_info;
  uint32 i, j, pos = 0, log_size = 0;
  uint32 total_num_entries = 0;

  /* Check if tech_id is valid */
  if (( rec_tech <= CXM_TECH_DFLT_INVLD ) || ( rec_tech >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(LOW, "Invalid tech %d sent for logging", rec_tech);
    return;
  }

  /* Reduce the number of loggings */
  if (lmtsmgr.tech_timing_info_index != LMTSMGR_TIMING_HIST_RECS)
  {
    return;
  }

  /* Find out how much size we need to allocate */
  log_size = FPOS(wwcoex_timing_info_log_type, sl_act_list);

  for (i=0; i<LMTSMGR_TIMING_HIST_RECS; i++)
  {
    timing_info = &lmtsmgr.tech_timing_info[i];
    total_num_entries += timing_info->num_entries;
    
  }
  log_size += total_num_entries * sizeof(wwcoex_act_tl_list_entry_type); 

  /* Increase log size for the the markers */
  log_size += LMTSMGR_TIMING_HIST_RECS * sizeof(wwcoex_act_tl_list_entry_type); 

  log_ptr = (wwcoex_timing_info_log_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_WWCOEX_SLOT_ACTIVITY_TIMELINE_INFO_C,
                                       log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = WWCOEX_SLOT_ACT_TL_INFO_LOG_VERSION;

    for (i=0; i<LMTSMGR_TIMING_HIST_RECS; i++ )
    {
      timing_info = &lmtsmgr.tech_timing_info[i];

      log_ptr->timing_hdr[i].send_tech_id = (uint8)timing_info->tech_id; 
      log_ptr->timing_hdr[i].rec_tech_id = (uint8) rec_tech;
      log_ptr->timing_hdr[i].in_dtx_state = (uint8) timing_info->dtx_info.in_dtx_state;
      log_ptr->timing_hdr[i].dtx_switch_time = timing_info->dtx_info.dtx_switch_time;
      log_ptr->timing_hdr[i].dtx_time = timing_info->dtx_info.dtx_time;
      log_ptr->timing_hdr[i].num_entries = (uint8) timing_info->num_entries;

      /* Fill in tech specific metrics */
      switch (timing_info->tech_id)
      {
        case CXM_TECH_GSM1:
        case CXM_TECH_GSM2:
          log_ptr->timing_hdr[i].tech_metrics.gsm_metrics.call_type = 
              (uint8) timing_info->tech_metrics.gsm_metrics.call_type;
          log_ptr->timing_hdr[i].tech_metrics.gsm_metrics.new_block_time =
              timing_info->tech_metrics.gsm_metrics.new_block_time;
          log_ptr->timing_hdr[i].tech_metrics.gsm_metrics.metrics_valid_mask = 
              timing_info->tech_metrics.gsm_metrics.metrics_valid_mask;
          for (j=0; j<CXM_MAX_GSM_METRIC_TYPES; j++)
          {
             log_ptr->timing_hdr[i].tech_metrics.gsm_metrics.cond_metics[j] = 
               timing_info->tech_metrics.gsm_metrics.cond_metics[j];
          }
          for (j=0; j<CXM_MAX_GSM_LOOK_AHEAD_FRAMES; j++)
          {
            log_ptr->timing_hdr[i].tech_metrics.gsm_metrics.frame_start_time[j] = 
              timing_info->tech_metrics.gsm_metrics.frame_start_time[j];
          }
          break; 

        default:
          break;
      }

      /* Fill in the slot entry information */
      for (j=0; j<timing_info->num_entries; j++)
      {
        if (pos < WWCOEX_LOG_MAX_TL_ENTRIES)
        {
          log_ptr->sl_act_list.entries[pos].freqid = timing_info->slot_list[j].freqid; 
          log_ptr->sl_act_list.entries[pos].frequency = timing_info->slot_list[j].frequency;
          log_ptr->sl_act_list.entries[pos].band = (uint8) timing_info->slot_list[j].band;
          log_ptr->sl_act_list.entries[pos].direction = (uint8) timing_info->slot_list[j].direction;
          log_ptr->sl_act_list.entries[pos].link_type = (uint8) timing_info->slot_list[j].link_type;
          log_ptr->sl_act_list.entries[pos].start_time = timing_info->slot_list[j].start_time;
          log_ptr->sl_act_list.entries[pos].end_time = timing_info->slot_list[j].end_time;
          log_ptr->sl_act_list.entries[pos].micro_prio = (uint8) timing_info->slot_list[j].micro_prio;
          log_ptr->sl_act_list.entries[pos].conflict_mask = timing_info->slot_list[j].conflict_mask;
          pos++;
        }
      }

      /* Fill an entry with a marker to distinguish between different messages */
      if (pos < WWCOEX_LOG_MAX_TL_ENTRIES)
      {
        log_ptr->sl_act_list.entries[pos].freqid = 0; 
        log_ptr->sl_act_list.entries[pos].frequency = 0;
        log_ptr->sl_act_list.entries[pos].band = (uint8) 0;
        log_ptr->sl_act_list.entries[pos].direction = (uint8) 0;
        log_ptr->sl_act_list.entries[pos].link_type = (uint8) 0;
        log_ptr->sl_act_list.entries[pos].start_time = CXM_INVALID_USTMR_TIME;
        log_ptr->sl_act_list.entries[pos].end_time = CXM_INVALID_USTMR_TIME;
        log_ptr->sl_act_list.entries[pos].micro_prio = (uint8) 0;
        log_ptr->sl_act_list.entries[pos].conflict_mask = 0;
        pos++;
      }
    }

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
  else
  {
    LMTSMGR_MSG_1(MED, "Could not allocate WWcoex SL Act TL log pkt of size %d",
                          log_size );
  }
#else
  LMTSMGR_UNUSED(rec_tech);
#endif /* LOG_MCS_WWCOEX_SLOT_ACTIVITY_TIMELINE_INFO_C */
}
