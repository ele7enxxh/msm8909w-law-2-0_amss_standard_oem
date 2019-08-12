
/*!
  @file
  ddr_coex_core.c

  @brief
  This file implements the DDR core functions of LIMTSMGR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-15 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/ddr_coex/src/ddr_coex_core.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   sg      Added partial support for WCDMA2 client
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
12/09/14   jm      Remove deprecated TRM API from limitsmgr
11/03/14   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "sys.h"

#include "cxm.h"
#include "wwan_coex.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_task.h"

#include "ddr_coex_i.h"
#include "lmtsmgr_translation.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

#ifdef FEATURE_MODEM_DDR_MGMT
static cxm_tech_type ddr_coex_trm_client_div_mapping
(
  trm_client_enum_t client    /* Client's whose state update is sent */
)
{
  switch (client)
  {
    case TRM_LTE_SECONDARY:
        return CXM_TECH_LTE;

    case TRM_TDSCDMA_SECONDARY:
        return CXM_TECH_TDSCDMA;

    case TRM_1X_SECONDARY:
        return CXM_TECH_ONEX;

    case TRM_HDR_SECONDARY:
        return CXM_TECH_HDR;

    case TRM_GSM_SECONDARY:
                /* TODO: GSM2/GSM3 */
        return CXM_TECH_GSM1;

    case TRM_UMTS_SECONDARY:
        return CXM_TECH_WCDMA;

    default:
        return CXM_TECH_DFLT_INVLD;
  }
}

void ddr_coex_insert_in_order_list
(
  /* The Lmtsmgr client id */
  cxm_tech_type               client_id,
  uint8 priority          /* Priority of client request */
)
{
  /* Order loop position index */
  uint32                          pos;

  /* Client just before position "pos" */
  cxm_tech_type               c;


/*--------------------------------------------------------------------------*/

  /* If Priority is Valid INSERT */
  if ( priority != 0 )
  {

    /* Find client in the ordered list.  Remove it, if present */
    for( pos = 0;  pos < lmtsmgr.num_order; pos++ )
    {
      if ( lmtsmgr.order[pos] == client_id )
      {
        /* It is present!  Remove it from the list ... */
        lmtsmgr.num_order--;;
  
        /* ... by shifting all entries after it one position forward */
        while ( pos < lmtsmgr.num_order )
        {
          lmtsmgr.order[ pos ] = lmtsmgr.order[ pos+1 ];
          pos++;
        }
        break;
      }
    }

    /* Move clients with a lower priority one position backwards */
    for ( pos = lmtsmgr.num_order;  pos > 0 ;  pos-- )
    {
      /* Get the client just before the current position */
      c = lmtsmgr.order[ pos-1 ];

      /* When we find a higher priority client ... */
      if ((LMTSMGR_IS_TECH_VALID(c))&&( lmtsmgr.tech_state[ c ].priority >= priority ))
      {
        /* ... stop shifting */
        break;
      }

      /* Shift backwards one position in list, to current position */
      lmtsmgr.order[ pos ] = c;
    }

    /* Insert client in the newly created hole in priority order list */
    lmtsmgr.order[ pos ] = client_id;
    lmtsmgr.num_order++;
  }
  /* If Priority is NOT Valid than REMOVE */
  else
  {
    /* Find client in the ordered list.  Remove it, if present */
    for( pos = 0;  pos < lmtsmgr.num_order; pos++ )
    {
      if ( lmtsmgr.order[ pos ] == client_id )
      {
        /* It is present!  Remove it from the list ... */
        lmtsmgr.num_order--;
  
        /* ... by shifting all entries after it one position forward */
        while ( pos < lmtsmgr.num_order )
        {
          lmtsmgr.order[ pos ] = lmtsmgr.order[ pos+1 ];
          pos++;
        }
        break;
      }
    }
  }
} /* TRMClientOrderArray::insert( trm_client_enum_t ) */

/*=============================================================================

  FUNCTION:  ddr_coex_get_tech_rx_power

=============================================================================*/
/*!
    @brief
    This function calculates and returns Tech Rx Power info.
 
    @return
    None
*/
/*===========================================================================*/
static int16 ddr_coex_get_tech_rx_power
(
  cxm_tech_type techid,
  wwan_ddr_coex_table_entry carrier,
  wwcoex_ddr_threshold_table_val chain
)
{
  lmtsmgr_tech_state_type *tech_st;
  uint32 num_dl_entries, i, dl_index;

  /* Used to determined LTE CA RF */
  trm_get_info_input_type trm_info_input;
  trm_get_info_return_type trm_info_output;

  /* default power to start with... */
  int16 rx_power = CXM_UNKNOWN_POWER;

  tech_st = &lmtsmgr.tech_state[techid];
  num_dl_entries = tech_st->currList.num_dl_entries;


  for (i=0; i<num_dl_entries; i++)
  {
    dl_index = tech_st->currList.dl_freqList[i];

    if (chain == COEX_DDR_TABLE_ENTRY_DIV)
    {
      /* Determine if dl entry is diveristy */
      if (tech_st->currList.links[dl_index].freqInfo.link_info.type == CXM_LNK_TYPE_DIVERSITY)
      {
         /* Select the entry with valid Rx power.. */
         rx_power = tech_st->currList.links[dl_index].powerInfo.rx_power;
         if ( rx_power != CXM_UNKNOWN_POWER)
         {
           break;
         }
      }
    }
    else /* Assume chain == COEX_DDR_TABLE_ENTRY_PRIMARY */
    {
       if (carrier == DDR_LUT_ENTRY_SECONDARY && techid == CXM_TECH_LTE)
       {
         /* Special Case: LTE CA */
         trm_info_input.info_type = TRM_DEVICE_MAPPING_INFO;
         trm_info_input.client_id = TRM_LTE_CA;
         trm_info_input.input_data.band_mapping_info.resource = TRM_RX_CA_PRIMARY;
         trm_info_input.input_data.band_mapping_info.num_bands = 1;
         trm_info_input.input_data.band_mapping_info.band[0] = tech_st->currList.links[dl_index].freqInfo.link_info.band;

         trm_get_info(&trm_info_input, &trm_info_output);

         /* Get the RX power specific to LTE Secondary */
         if(tech_st->currList.links[dl_index].freqInfo.link_info.rf_device == trm_info_output.info.band_mapping_info.device[0])
         {
            rx_power = tech_st->currList.links[dl_index].powerInfo.rx_power;
            break;
         }
       }
       else
       {
         if (tech_st->currList.links[dl_index].freqInfo.link_info.type != CXM_LNK_TYPE_DIVERSITY)
         {
           /* Select the entry with valid Rx power... */
           rx_power = tech_st->currList.links[dl_index].powerInfo.rx_power;
           if ( rx_power != CXM_UNKNOWN_POWER)
           {
             break;
           }
         }
       }
    }
  } // End for

  return rx_power;
}

/*=============================================================================

  FUNCTION:  ddr_coex_reset_order_list

=============================================================================*/
/*!
    @brief
    Resets order list for Limitsmgr.
 
    @return
    None
*/
/*===========================================================================*/
static void ddr_coex_reset_order_list (void)
{
  uint8 index;

  lmtsmgr.num_order = 0;
  for (index = 0; index < (uint8)CXM_TECH_MAX; index++)
  {
    lmtsmgr.order[index] = CXM_TECH_DFLT_INVLD;
  }
}

/*=============================================================================

  FUNCTION:  ddr_coex_update_tech_band_channel

=============================================================================*/
/*!
    @brief
    Updates the tech's Band and Channel only if it has changed. Also sets the
    band/channel changed flag.
 
    @return
    None
*/
/*===========================================================================*/
static void ddr_coex_update_tech_band_channel
(
  cxm_tech_type techid,
  uint32 band,
  uint32 channel,
  boolean band_valid,
  boolean channel_valid
)
{
  lmtsmgr_tech_state_type *tech_st;

  tech_st = &lmtsmgr.tech_state[techid];

  /* Has the band changed ? */
  if ((band_valid == TRUE) && (tech_st->band != band))
  {
    LMTSMGR_MSG_2(MED, "band changed from %d->%d", tech_st->band, band);
    tech_st->band = band;
    tech_st->band_changed = TRUE;
  }

  /* Has the Channel changed ? */
  if ((channel_valid == TRUE) && (tech_st->channel != channel))
  {
    LMTSMGR_MSG_2(MED, "channel changed from %d->%d", tech_st->channel, channel);
    tech_st->channel = channel;
    tech_st->channel_changed = TRUE;
  }
}

#endif /* DDR */

/*=============================================================================

  FUNCTION:  ddr_coex_init

=============================================================================*/
/*!
    @brief
    Initialize DDR_Coex related functionalities.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_init (void)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  /* If this is ASSERTed means update new tech in ddr_default_freq_plan_of_tech_type 
        in LimtsMgrNvDefinition.xml */
  LMTSMGR_COMPILE_ASSERT(CXM_TECH_MAX == 9);

  ddr_coex_npa_init();
  ddr_coex_reset_order_list();
#endif
}

/*=============================================================================

  FUNCTION:  ddr_coex_trm_tech_state_update_hdlr

=============================================================================*/
/*!
    @brief
    DDR coex function to handle TRM tech_state indication.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_trm_tech_state_update_hdlr
(
  trm_state_info_type trm_state_info
)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  cxm_tech_type client_id;
  lmtsmgr_tech_state_type *tech_st;

  /* Enter Critical Section */
  LMTSMGR_ENTER_CRIT_SECT(lmtsmgr.crit_sect);

  client_id = lmtsmgr_trans_trm_to_cxm_client(trm_state_info.client);
  if (LMTSMGR_IS_TECH_VALID(client_id))
  {
    tech_st = &lmtsmgr.tech_state[client_id];
    tech_st->reason = trm_state_info.reason;
    ddr_coex_update_tech_band_channel(client_id, trm_state_info.band, 0, TRUE, FALSE);
    tech_st->priority = trm_state_info.priority;
    ddr_coex_insert_in_order_list(client_id, trm_state_info.priority);

    LMTSMGR_MSG_3(LOW, "reason %d band %d priority %d ", 
                        trm_state_info.reason, trm_state_info.band, trm_state_info.priority);

    /* Call DDR coex process function */
    ddr_coex_process_ddr_input();
  }
  else
  {
     /* Check for Diversity */
     client_id = ddr_coex_trm_client_div_mapping(trm_state_info.client);
     if (LMTSMGR_IS_TECH_VALID(client_id))
     {
        tech_st = &lmtsmgr.tech_state[client_id];
        tech_st->div_hold_prev = tech_st->div_hold_cur;
        if (trm_state_info.reason == TRM_NUM_REASONS)
        {
          tech_st->div_hold_cur = FALSE;
        }
        else
        {
          tech_st->div_hold_cur = TRUE;
        }

        LMTSMGR_MSG_2(MED, "CXM Client: %d Diversity On: %d", 
                      client_id, tech_st->div_hold_cur);

        /* Call DDR coex process function */
        ddr_coex_process_ddr_input();
     }

     /* Check for GPS */
     if (trm_state_info.client == TRM_GPS)
     {
        if (trm_state_info.reason == TRM_NUM_REASONS)
        {
           lmtsmgr.gps_tech_state_on = FALSE;
        }
        else
        {
           lmtsmgr.gps_tech_state_on = TRUE;
        }

        LMTSMGR_MSG_1(MED, "GPS On: %d", lmtsmgr.gps_tech_state_on);

         /* Call DDR coex process function */
        ddr_coex_process_ddr_input();
     }
  }

  /* Leave Critical Section */
  LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
#else
  LMTSMGR_UNUSED(trm_state_info.client);
#endif /* DDR_MGMT */
}


/*=============================================================================

  FUNCTION:  ddr_coex_update_tech_info

=============================================================================*/
/*!
    @brief
    Updates the tech's channel and Band only if it has changed. Also sets the
    band/channel changed flag.
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_update_tech_info
(
  cxm_tech_type tech_id
)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  lmtsmgr_tech_state_type *tech_st;
  uint32 dl_index;

  tech_st = &lmtsmgr.tech_state[tech_id];

  /* Get the band & channel info...*/
  if (tech_st->currList.num_entries == 0)
  {
    /* turned inactive */
    ddr_coex_update_tech_band_channel(tech_id, SYS_BAND_CLASS_MAX, 0, TRUE, TRUE);
  }
  else if (tech_st->currList.num_ul_entries > 0)
  {
    /* Tx is On: Get the band from first UL freq. */
    dl_index = tech_st->currList.dl_freqList[0];
    ddr_coex_update_tech_band_channel(tech_id, 
                                      tech_st->currList.links[dl_index].freqInfo.link_info.band, 
                                      tech_st->currList.links[dl_index].freqInfo.link_info.channel,
                                      TRUE, TRUE);
  }
  else 
  {
    /* Only Rx is On: so use the band from the first entry */
    ddr_coex_update_tech_band_channel(tech_id, 
                                      tech_st->currList.links[0].freqInfo.link_info.band, 
                                      tech_st->currList.links[0].freqInfo.link_info.channel,
                                      TRUE, TRUE);
  }
#else
  LMTSMGR_UNUSED(tech_id);
#endif
}

/*=============================================================================

  FUNCTION:  ddr_coex_process_ddr_input

=============================================================================*/
/*!
    @brief
    Processes any change in the DDR coex input variables
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_process_ddr_input(void)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  lmtsmgr_tech_state_type *tech_st, *save_tech_st = NULL;
  cxm_tech_type   hprio_tech;
  boolean is_TxON = FALSE;
  boolean is_anyTxON = FALSE;
  boolean is_anyInChannel = FALSE;
  boolean is_anyDesense = FALSE;
  uint32  pwr_update_duration, i;
  wwan_ddr_coex_table_entry carrier_idx;
  wwcoex_ddr_threshold_table_val chain_idx;
  int16 rx_power;
  int16 save_rx_power = CXM_UNKNOWN_POWER;

  wwan_ddr_efs_lookup_in ddr_input;
  wwan_ddr_efs_lookup_out ddr_output, save_ddr_info;
  uint32 gps_ddr_freq = 0;

  memset(&save_ddr_info, 0, sizeof(wwan_ddr_efs_lookup_out));

  if(IS_DDR_MITIGATION_DISABLED(lmtsmgr.mitigation_mask))
  {
     LMTSMGR_MSG_0(MED, "DDR Mitigation is diasbled.");
     return;
  }

  /* Priority: Tech1 Primary, T2 Primary, T1 Secondary, T2 Secondary, T1 Diversity, T2 Diversity */
  for (chain_idx = 0; chain_idx<COEX_DDR_TABLE_ENTRY_MAX; chain_idx++) 
  {
     for (carrier_idx = 0; carrier_idx<DDR_LUT_ENTRY_MAX; carrier_idx++)
     {
        for (i = 0; i<lmtsmgr.num_order; i++)
        {
          hprio_tech = lmtsmgr.order[i];
          if (!LMTSMGR_IS_TECH_VALID(hprio_tech))
          {
             continue;
          }

          tech_st = &lmtsmgr.tech_state[hprio_tech];

          if (tech_st->band >= SYS_BAND_CLASS_MAX)
          {
             continue;
          }

          /* Update input to the DDR EFS */
          ddr_input.tech = hprio_tech;
          ddr_input.carrier = carrier_idx;
          ddr_input.chain = chain_idx;

          /* Convert to RF band */
          ddr_input.band = lmtsmgr_trans_sys_bands_to_rf_bands(tech_st->band, hprio_tech);

          if (FALSE == wwan_ddr_efs_lookup(&ddr_input, &ddr_output))
          {
             /* If there was a failure in reading EFS entry for a particular tech */
             continue;
          }

          if (!ddr_output.band_spec_plan_valid)
          {
             /* If tech does not have a valid EFS entry*/
             continue;
          }

          if ((!IS_GSM_TECH(hprio_tech) && (carrier_idx == DDR_LUT_ENTRY_PRIMARY) &&
               !IS_IN_CHANNEL_RANGE(tech_st->channel, ddr_output.channel_min, ddr_output.channel_max)) ||
              ((hprio_tech == CXM_TECH_LTE) && (carrier_idx == DDR_LUT_ENTRY_SECONDARY) &&
               !IS_IN_CHANNEL_RANGE(tech_st->lte_ca_channel, ddr_output.channel_min, ddr_output.channel_max)))
          {
             /* If non-GSM tech is out of channel desense range */
             continue;
          }
          
          if (IS_GSM_TECH(hprio_tech) ||
              (!IS_GSM_TECH(hprio_tech) && 
               IS_IN_CHANNEL_RANGE(tech_st->channel, ddr_output.channel_min, ddr_output.channel_max)))
          {
            /* Set if any tech is within channel range (non-GSM) or is GSM */
             is_anyInChannel = TRUE;
          }

          if (tech_st->currList.num_ul_entries > 0 && (chain_idx == COEX_DDR_TABLE_ENTRY_PRIMARY))
          {
             is_TxON = TRUE;
             is_anyTxON = TRUE;
          }
          else
          {
             is_TxON = FALSE;
          }

          /* 
             Mitigate DDR if the following is true:
               - Acquision Mode (non-Diversity)
               - Band change (GSM only) or Channel within range (non-GSM)
          */
          if (!is_TxON && (chain_idx == COEX_DDR_TABLE_ENTRY_PRIMARY) &&
              (((tech_st->band_changed == TRUE) && IS_GSM_TECH(hprio_tech)) ||
               (!IS_GSM_TECH(hprio_tech) && IS_IN_CHANNEL_RANGE(tech_st->channel, ddr_output.channel_min, ddr_output.channel_max))
             ))
          {
            if (IS_GSM_TECH(hprio_tech))
            {
              LMTSMGR_MSG_5(MED, "tech %d, chain: %d, is_TxON %d, band %d, band change %d", 
                           hprio_tech, chain_idx, is_TxON, tech_st->band, tech_st->band_changed);
            }
            else
            {
              LMTSMGR_MSG_5(MED, "tech %d, chain: %d, is_TxON %d, channel %d, channel change %d", 
                            hprio_tech, chain_idx, is_TxON, tech_st->channel, tech_st->channel_changed);
            }
            ddr_coex_npa_update_freq_plan(ddr_output.ddr_freq);
            goto process_ddr_coex_cleanup;
            return;
          }

          rx_power = ddr_coex_get_tech_rx_power(hprio_tech, ddr_input.carrier, ddr_input.chain);

          LMTSMGR_MSG_6(MED, "tech %d, carrier: %d, chain: %d, power change: %d, Rx power %d, rx_thrshd %d", 
                               hprio_tech,
                               carrier_idx,
                               chain_idx,
                               tech_st->power_change,
                               rx_power, 
                               ddr_output.rxpower_threshold);

          if (rx_power == CXM_UNKNOWN_POWER)
          {
             continue;
          }

          /* If power has changed, perform time hystersis. */
          if (tech_st->power_change == TRUE)
          { 
             if (rx_power < ddr_output.rxpower_threshold)
             {
                 is_anyDesense = TRUE;
                 /* Neutral is required when the Power threshold is crossed 1st time, 
                              it should be updated as per */
                 if ((tech_st->thrshd_switchover_sign == LMTSMGR_DDR_RX_THRSHD_NEUTRAL) ||
                               (tech_st->thrshd_switchover_sign == LMTSMGR_DDR_RX_THRSHD_POSITIVE))
                 {
                   tech_st->pwr_thrshd_switchover_time = tech_st->curr_pwr_update_time;
                   tech_st->thrshd_switchover_sign = LMTSMGR_DDR_RX_THRSHD_NEGATIVE;
                 }
                 else if (tech_st->curr_pwr_update_time < tech_st->pwr_thrshd_switchover_time)
                 {
                   /*There is a rollover happended in time hence updating switchover time accordingly */
                   tech_st->pwr_thrshd_switchover_time = tech_st->curr_pwr_update_time;
                 }

                 pwr_update_duration = 
                       (uint32)((tech_st->curr_pwr_update_time - tech_st->pwr_thrshd_switchover_time) 
                                & 0xFFFFFFFFuL);

                 LMTSMGR_MSG_4(MED, "time Rx thrshd NOT crossed duration %x (%d) current 0x%llx old 0x%llx", 
                               pwr_update_duration, pwr_update_duration, 
                               tech_st->curr_pwr_update_time, 
                               tech_st->pwr_thrshd_switchover_time);

                 if (pwr_update_duration > ddr_output.lower_threshold)
                 {
                   ddr_coex_npa_update_freq_plan(ddr_output.ddr_freq);
                   goto process_ddr_coex_cleanup;
                 }
             }
             else
             {
                /* Store Rx and LUT info */
                save_tech_st = tech_st;
                save_rx_power = rx_power;
                save_ddr_info = ddr_output;
             }
          }

        } // End For (i)
     } // End For (carrier_idx)
  } // End for (chain_idx);

  LMTSMGR_MSG_3(LOW, "anyTxOn: %d, anyInChannel %d, anyDesense %d",
                is_anyTxON, is_anyInChannel, is_anyDesense);

  if ((lmtsmgr.gps_tech_state_on == TRUE) && (wwan_ddr_efs_lookup_gps(&gps_ddr_freq) == TRUE))
  {
     /* If GPS is ON and EFS entry is valid, utilize associated DDR Frequency Plan. */
     LMTSMGR_MSG_1(MED, "GPS is enabled. Set DDR Frequency to %d", gps_ddr_freq);
     ddr_coex_npa_update_freq_plan(gps_ddr_freq);
  }
  /* If no tech is active or any tech is within channel range, set to default */
  else if(!is_anyTxON || !is_anyInChannel)
  {
     ddr_coex_npa_update_freq_plan(0);
  }
  else if (!is_anyDesense && (save_rx_power != CXM_UNKNOWN_POWER) && (save_tech_st != NULL))
  {
     /* All techs do not require desense. Switch back to default freq plan if passes time hystersis */
     if (save_rx_power >= save_ddr_info.rxpower_threshold)
     {
         if ((save_tech_st->thrshd_switchover_sign == LMTSMGR_DDR_RX_THRSHD_NEUTRAL) ||
             (save_tech_st->thrshd_switchover_sign == LMTSMGR_DDR_RX_THRSHD_NEGATIVE))
         {
           save_tech_st->pwr_thrshd_switchover_time = save_tech_st->curr_pwr_update_time;
           save_tech_st->thrshd_switchover_sign = LMTSMGR_DDR_RX_THRSHD_POSITIVE;
         }
         else if (save_tech_st->curr_pwr_update_time <save_tech_st->pwr_thrshd_switchover_time)
         {
           /*There is a rollover happended in time hence updating switchover time accordingly */
           save_tech_st->pwr_thrshd_switchover_time = save_tech_st->curr_pwr_update_time;
         }
         
         pwr_update_duration = 
               (uint32)((save_tech_st->curr_pwr_update_time - save_tech_st->pwr_thrshd_switchover_time) 
                       & 0xFFFFFFFFuL);
         
         LMTSMGR_MSG_4(MED, "time Rx thrshd crossed duration %x (%d) current 0x%llx old 0x%llx", 
                      pwr_update_duration, pwr_update_duration, 
                      save_tech_st->curr_pwr_update_time, 
                      save_tech_st->pwr_thrshd_switchover_time);
         
         if (pwr_update_duration > save_ddr_info.upper_threshold)
         {
           /* If the power update is too long, set frequency plan to default */
           ddr_coex_npa_update_freq_plan(ddr_output.default_ddr_freq);
         }
     }
  }

process_ddr_coex_cleanup:

  /* Reset the flag for all techs */
  for (i = 0; i<lmtsmgr.num_order; i++)
  {
    hprio_tech = lmtsmgr.order[i];
    if (!LMTSMGR_IS_TECH_VALID(hprio_tech))
    {
      continue;
    }

    tech_st = &lmtsmgr.tech_state[hprio_tech];
#ifndef FEATURE_MODEM_COEXISTENCE_SW
    tech_st->freq_change = FALSE;
    tech_st->power_change = FALSE;
#endif
    tech_st->band_changed = FALSE;
    tech_st->channel_changed = FALSE;
  }

#endif /* DDR */
}



