/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                          H D R M U L T I R A T

GENERAL DESCRIPTION
  This module contains utilities and interface components that HDR core modules
  can use to interact with external entities in implementing multi-RAT support.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrmultirat.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/04/15   mbs     Supported QTA using TRM API
03/16/15   wsh     Reduced stack usage to avoid overflow 
01/15/15   arm     Fallback to SS or DSDS mode from DR based on initial 
                   multimode config
01/23/15   sat     Changes to not update Diversity enabled to FW.
01/09/15   sat     Updating FW if total channel_cnt is 0 or diversity is enabled.
12/04/14   arm     Changes to support new unified micro priority changes
11/04/14   arm     Band avoidance: move power info saving to hdrsrch task and
                   enable BA only for DSDA mode.
10/13/14   arm     Provided updated channel id to FW after re-acq.
10/09/14   arm     Band Avoidance crash fix.
10/06/14   arm     Fixed band avoidance compilation warning.
10/01/14   arm     Send channel Id for diversity chain also.
10/01/14   arm     Changes to support HDR band avoidance.
08/27/14   arm     Update hdrmulitrat and MCS if rf device has changed.
08/03/14   arm     Changes to request RX_ANY chain in DR mode for idle.
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
07/09/14   sat     Changes to add more defensive checks with sending DSDX_ENABLE 
                   to FW
07/03/14   arm     Used BG TRAFFIC reason for DR-DSDS traffic.
06/30/14   arm     Changes to support DR-DSDS.
06/04/14   arm     Changes to support DO to G QTA
11/06/13   arm     Send power update to MCS as soon as we receive freq ids & 
                   in traffic state update frequencies once tx is also tuned. 
10/14/13   arm     Send freq id list again to FW in Reacq state.
02/27/14   sat     Sending updated frequency list information to FW after wakeup
02/18/14   bb      Changes to use proper carrier index while retrieving RF power
12/06/13   arm     Supported Div chain frequency and power reporting. 
03/15/14   wsh     Change to support T/T and new RF concurrency concern
04/22/14   sat     Fix to call appropriate function to check if chain is owned.
04/21/14   sat     Changes to update MCS/FW of Num Carriers when in suspended state
03/13/14   bb      Changes to add more defensive checks while sending DSDX_ENABLE
                   command to FW
03/10/14   sat     Checking if MDSP is active just before sending freq. ID to FW.
02/28/14   arm     Sending channel id to FW after every state transition.
03/14/14   rmv     Enabled D-LNA in DSDA depending on band combinations 
11/26/13   arm     Handle SSSS->DSDA transition in online mode.
03/24/14   arm     Merged wakeup manager changes for DSDS.
08/19/13   arm     Merged DSDS feature for triton.
07/17/13   arm     Enabled device only dtx in DSDA depending on band 
                   combinations. 
07/16/13   arm     Added support for  FW DSDA Rx/Tx log packet logging.
06/20/13   arm     Fixed Bolt compiler errors.
06/13/13   arm     Fixed coex tx power reporting bug.
04/22/13   arm     Created module for HDR interaction Multi RAT scenario.

===============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

===============================================================================*/
#include "hdr_variation.h"
#include "hdrmultirat.h"

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
#include "hdrmdsp.h"
#include "hdrsrchrf.h"
#include "hdrsrchset.h"
#include "hdrsrchstate.h"
#include "rfm_cdma.h"
#include "rfm_cdma_agc_types.h"
#include "rfm_hdr.h"
#include "wwcoex_action_iface.h"
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
#include "hdrfw_logging.h"
#include "hdrlogi.h"
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

#include "hdrsrchstate.h"

#ifdef FEATURE_HDR_WAKEUP_MANAGER
#include "wmgr.h"
#include "hdrsrchstates.h"
#endif /* FEATURE_HDR_WAKEUP_MANAGER */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW 

   /*  Static structure type to query dtx action from coex*/
  cxm_highest_action_query_s  hdrmultirat_coex_dtx_query;
    /* Parameter type for cxm query function */
  cxm_action_type hdrmultirat_dtx_actions[ HDRMULTIRAT_MAX_NUM_CARRIERS ];
    /* Action array for the corresponding requesting ids */
  uint32 hdrmultirat_dtx_req_ids[ HDRMULTIRAT_MAX_NUM_CARRIERS ];
    /* FreqId array of the requesting techs... */

   /*  Static structure type to query D-LNA action from coex*/
  cxm_highest_action_query_s  hdrmultirat_coex_dlna_query;
    /* Parameter type for cxm query function */
  cxm_action_type hdrmultirat_dlna_actions[ HDRMULTIRAT_MAX_NUM_CARRIERS ];
    /* Action array for the corresponding requesting ids */
  uint32 hdrmultirat_dlna_req_ids[ HDRMULTIRAT_MAX_NUM_CARRIERS ];
    /* FreqId array of the requesting techs... */
  cxm_coex_power_ind_s hdrmultirat_pwr_cfg_msg;
    /* PowerCfgMsg used to report coex power */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION     : HDRMULTIRAT_POWERUP_INIT

DESCRIPTION  : This function initializes frequency info and other members of 
               hdrmultirat structure.
               
DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrmultirat_powerup_init( void )
{
  sys_modem_device_mode_e_type device_mode;
    /* Device mode enum from CM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize hdrmultirat Critical Section*/
  rex_init_crit_sect( &hdrmultirat.crit_sect );

  /* Check if DSDA is enabled */
  device_mode = cm_get_device_mode();

  if ( device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE )
  {
    hdrmultirat.mode_pref = HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE;
  }
  else
  {
    hdrmultirat.mode_pref = HDRMULTIRAT_SINGLE_STANDBY;
  }

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  hdrmultirat.coex_info.rx.channel_cnt = 0;
  hdrmultirat.coex_info.tx.channel_cnt = 0;
  hdrmultirat.coex_info.div.channel_cnt = 0;

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
    /* leave critical section */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW

  hdrmultirat.is_mode_changed = TRUE;
  hdrmultirat.dsda_rx_log_pkt_is_logged = FALSE;
  hdrmultirat.dsda_tx_log_pkt_is_logged = FALSE;

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

} /* hdrmultirat_powerup_init */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_MODE_PREF

DESCRIPTION  : This function returns device mode pref.
               
DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : Device mode

SIDE EFFECTS : None

=========================================================================*/

hdrmultirat_mode_pref_enum_type hdrmultirat_get_mode_pref ( void )
{
  return hdrmultirat.mode_pref;
  
} /* hdrmultirat_get_mode_pref */
/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_CXM_PRIORITY_TABLE_CB

DESCRIPTION  : This function gets coex manager priority table and saves it.
               
DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrmultirat_get_cxm_priority_table_cb
(
  cxm_tech_type  tech_id,
    /* Tech type: HDR */

  cxm_activity_table_s *activity_tbl
    /* Get priority table from MCS */
)
{
   uint8 i = 0;
   /* Temp loop variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: got cxm priority table" );
  
   /* Copy over the priority table */
   for ( i = 0 ;i < MAX_ACTIVITY_TIERS;  i++ )
   {
     hdrmultirat_cxm_priority_table[i] = activity_tbl[i];
   }
 
} /* hdrmultirat_get_cxm_priority_table_cb*/

/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_CXM_PRIORITY

DESCRIPTION  : This function returns priority for given priority level from 
               the table. 
               
DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : Priority

SIDE EFFECTS : None

=========================================================================*/
uint32 hdrmultirat_get_cxm_priority
(
   hdrmultirat_priority_type pri_level 
)
{
  uint32 priority = 0;
    /* Temp loop variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: priority %d",
                 hdrmultirat_cxm_priority_table[pri_level].priority );

  if ( pri_level <  MAX_ACTIVITY_TIERS )
  {
    priority = hdrmultirat_cxm_priority_table[pri_level].priority;
  }
  else
  {
    priority = hdrmultirat_cxm_priority_table[MAX_ACTIVITY_TIERS-1].priority;

    HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                    "HDR DSDA: get_cxm_prio oob:%d, return lowest prio:%d",
                    pri_level, priority );
  }

  return priority;
  
} /* hdrmultirat_get_cxm_priority */

/*===========================================================================

FUNCTION HDRMULTIRAT_DSDA_MODE_CHANGE

DESCRIPTION
  This function processes mode pref change to DSDA.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_dsda_mode_change( void )
{
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
  uint32                          priority = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmultirat.mode_pref == HDRMULTIRAT_SINGLE_STANDBY )
  {
    /* Make sure that MDSP clock is ON along with FW status, this is to avoid 
     * race conditon in sending message to FW to change state to 
     * STANDBY and sending DSDA messages to FW.
     */
    if ( hdrmdsp_is_mdsp_active() &&
         hdrmdsp_check_dsp_clk() )
    {
      if ( hdrmultirat.is_mode_changed  )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Switch to SSSS!" );

        cxm_register_prio_table_cb( CXM_TECH_HDR, NULL ); 
          /* Deregister for cb for priority table */
        hdrmultirat_send_dsdx_enable_cfg_msg( HDRMULTIRAT_SINGLE_STANDBY );
          /* Send DSDA disable msg to FW  */
        hdrmultirat.is_mode_changed = FALSE;
      }
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: FW not active,"
                                     " not sending DSDA disbale yet!" );
    }
    
  }
  else 
  {
    /* Make sure that MDSP clock is ON along with FW status, this is to avoid 
     * race conditon in sending message to FW to change state to 
     * STANDBY and sending DSDA messages to FW.
     */
    if ( hdrmdsp_is_mdsp_active() &&
         hdrmdsp_check_dsp_clk() )
    {
      if ( hdrmultirat.is_mode_changed  )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR : DSDx mode %d", hdrmultirat.mode_pref );

        cxm_register_prio_table_cb( CXM_TECH_HDR, 
                                    hdrmultirat_get_cxm_priority_table_cb ); 
          /* Register for cb for priority table */

#ifdef FEATURE_HDR_UNIFIED_MICRO_PRIORITY
        priority = hdrmultirat_get_cxm_priority( HDRMULTIRAT_NORMAL_PRIORITY );
          /* Start with lower priority */
#else
        priority = hdrmultirat_get_cxm_priority( HDRMULTIRAT_LOWER_PRIORITY_20 );
#endif /* FEATURE_HDR_UNIFIED_MICRO_PRIORITY */
    
        hdrmultirat_send_dsda_priority_cfg_msg( priority, priority );
          /* Send priority message to FW to start with lowest priority */

        hdrmultirat_send_dsdx_enable_cfg_msg( hdrmultirat.mode_pref );
          /* Send DSDA enable to FW DSDX_ENABLE */

        hdrmultirat.is_mode_changed = FALSE;
    
         /* TODO also send freq id on change */
      }
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: FW not active,"
                                     " not sending mode change yet!" );
    }
    
  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
}/* hdrmultirat_dsda_mode_change */

/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_FREQ_ID_LIST

DESCRIPTION
  Process the frequency id list rxd from coex manager.

DEPENDENCIES
  None

PARAMETERS
  frequency id list

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_freq_id_list 
( 
  hdrmultirat_cxm_freq_id_list_type* freqid_list
)
{
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
  hdrfw_dsdx_frequency_msg_t         dsda_frequency_cfg_msg;
    /* fw dsdx mode enable msg */

  uint8                              demod_id = 0;
    /* Store demod id */

#ifdef FEATURE_HDR_FW_CHAN_ID
   cxm_channel_in_type chan_info = {CXM_TECH_HDR, RFM_INVALID_DEVICE} ;
     /* input argument to get channel_ID */

   cxm_channel_out_type  chan_id = {0,0};
     /* channel ID corresponding to RF Device */
#endif /* FEATURE_HDR_FW_CHAN_ID */

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW  */

  uint8                              cnt = 0;
    /* Temp loop variable */

  coex_freqid_list_s*                freq_data;
    /* Temp reference */

  sys_channel_type                   channel;
    /* Store band channel information */

  uint8                              i = 0;
    /* temp loop variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: received freq id list from MCS" );

  HDR_ASSERT ( freqid_list != NULL );

  freq_data = &freqid_list->freq_data; 

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
  if ( ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE )
#ifdef FEATURE_HDR_QTA
       || ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_QTA */
#ifdef FEATURE_HDR_DR_DSDS
       || ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_DR_DSDS */

     )
  {
#ifdef FEATURE_HDR_QTA
    if ( hdrsrchtc_is_qta_enabled() && (!hdrsrchrf_has_lock(HDRSRCHRF_PRI_CHAIN) ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR QTA: QTA is enabled & no primary chain not sending freq update to FW!" );
      return;
    }
#endif /* FEATURE_HDR_QTA */

    hdrmultirat.frequency_update_pending = FALSE;
    memset( &dsda_frequency_cfg_msg,
            0,
            sizeof( hdrfw_dsdx_frequency_msg_t ) );
      /* Clear the message */

    msgr_init_hdr( &dsda_frequency_cfg_msg.hdr, 
                   MSGR_HDR_MULTIRAT, 
                   HDR_FW_DSDX_FREQUENCY_UPDATE_CMD );
      /* Init the message header */

    for ( cnt = 0; cnt < HDRMULTIRAT_MAX_NUM_CARRIERS; cnt++ )
    {
      dsda_frequency_cfg_msg.txfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
      dsda_frequency_cfg_msg.rxfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
      /* Initialize frequency Id */
    }
      
#ifdef FEATURE_HDR_FW_CHAN_ID
      chan_info.rf_device = hdrmultirat.coex_info.rx.rf_device;
      chan_info.techid = CXM_TECH_HDR;

      if ( chan_info.rf_device == RFM_INVALID_DEVICE )
      {
        dsda_frequency_cfg_msg.rxChannelId[0] = 0;
        dsda_frequency_cfg_msg.rxChannelId[1] = 0;
        dsda_frequency_cfg_msg.txChannelId = 0;          
      }
      else
      {
        chan_id = cxm_get_channel_id( chan_info );
        dsda_frequency_cfg_msg.rxChannelId[0] = chan_id.rx_channel_id;
        dsda_frequency_cfg_msg.txChannelId = chan_id.tx_channel_id;

        /* If Diversity chain, then get channel id for div also*/
        if ( hdrmultirat.coex_info.div.channel_cnt )
        {
          chan_info.rf_device = hdrmultirat.coex_info.div.rf_device;
          chan_info.techid = CXM_TECH_HDR;
          chan_id = cxm_get_channel_id( chan_info );
          dsda_frequency_cfg_msg.rxChannelId[1] = chan_id.rx_channel_id;
        }
        else
        {
          dsda_frequency_cfg_msg.rxChannelId[1] = 0;
        }
      }
      
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "HDR DSDA: Rx chan id %d Div chain id %d Tx chan id %d", 
                      dsda_frequency_cfg_msg.rxChannelId[0],
                      dsda_frequency_cfg_msg.rxChannelId[1],
                      dsda_frequency_cfg_msg.txChannelId );
#endif /* FEATURE_HDR_FW_CHAN_ID */

    rex_enter_crit_sect( &hdrmultirat.crit_sect );
      /* enter critical section */

    for ( cnt = 0; cnt < freq_data->num_link_info_sets; cnt++ )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: freq id %d", 
                     freq_data->link_list[cnt].freqid );
      channel.band = freq_data->link_list[cnt].link_info.band;
      channel.chan_num = freq_data->link_list[cnt].link_info.channel;

      demod_id = hdrsrchset_get_carrier_demod_id( channel );
      
        if ( freq_data->link_list[cnt].link_info.type == CXM_LNK_TYPE_DIVERSITY )
        {
          HDR_MSG_PROT( MSG_LEGACY_MED, "DIV type! " );

          /* Store freq id in our local data structure also */
          for ( i = 0; i < hdrmultirat.coex_info.div.channel_cnt; i++ )
          {
            if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.div.channel[i] ))
            {
              hdrmultirat.coex_info.div.freqid[i] = freq_data->link_list[cnt].freqid;
            }
          }
          
        }
        else if ( freq_data->link_list[cnt].link_info.direction == HDRMULTIRAT_REVERSE_LINK )
          {
        dsda_frequency_cfg_msg.txNumcarriers++;
        
        dsda_frequency_cfg_msg.txfrequencyId[demod_id] = 
          freq_data->link_list[cnt].freqid;
          /* Store freq id in our local data structure also */

        HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DSDA: freq id %d and demod_id %d", 
                         dsda_frequency_cfg_msg.txfrequencyId[demod_id], demod_id );

        for ( i = 0; i < hdrmultirat.coex_info.tx.channel_cnt; i++ )
        {
          if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.tx.channel[i] ))
          {
            hdrmultirat.coex_info.tx.freqid[i] = freq_data->link_list[cnt].freqid;
          }
        }
      }/* if reverse link */
      else if ( freq_data->link_list[cnt].link_info.direction == HDRMULTIRAT_FORWARD_LINK )
      {
        dsda_frequency_cfg_msg.rxNumcarriers++;

        dsda_frequency_cfg_msg.rxfrequencyId[demod_id] = 
            freq_data->link_list[cnt].freqid;

        HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DSDA: freq id %d and demod_id %d", 
                       dsda_frequency_cfg_msg.rxfrequencyId[demod_id], demod_id );

        /* Store freq id in our local data structure also */
        for (i = 0; i < hdrmultirat.coex_info.rx.channel_cnt; i++ )
        {
          if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.rx.channel[i] ))
          {
            hdrmultirat.coex_info.rx.freqid[i] = freq_data->link_list[cnt].freqid;
              hdrmultirat.coex_info.rx.demod_idx[i] = demod_id;
          }
        }
      }/* else if forward link */

    } /* for num_link_info_sets */

    rex_leave_crit_sect( &hdrmultirat.crit_sect );

    /* To make sure FW is still active before sending it freq. ID list */
    if ( hdrmdsp_is_mdsp_active()  )
    {
       /* To check if HDR owns the chain. If chain not owned, 
          then Numcarriers set to 0. */
       if ( !hdrsrchrf_has_lock(HDRSRCHRF_PRI_CHAIN) )
      {
        dsda_frequency_cfg_msg.rxNumcarriers = 0;
        dsda_frequency_cfg_msg.txNumcarriers = 0;

        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR DSDA: Chain not owned! "
                                     "rxNumcarriers: %d txNumcarriers: %d",
                                      dsda_frequency_cfg_msg.rxNumcarriers, 
                                      dsda_frequency_cfg_msg.txNumcarriers );
      } /* If chain is not owned */
      hdrmultirat_send_msg( &dsda_frequency_cfg_msg.hdr, 
                           sizeof( hdrfw_dsdx_frequency_msg_t ) );
    } /* If mdsp is active */
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: FW not active..not sending freq id" );
      hdrmultirat.frequency_update_pending = TRUE;
    }
  } /* If dsda mode*/
  else
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "No DSDA, storing freq IDs only " );

    rex_enter_crit_sect( &hdrmultirat.crit_sect );
      /* enter critical section */
    for (cnt = 0; cnt < freq_data->num_link_info_sets; cnt++ )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: freq id %d", 
                     freq_data->link_list[cnt].freqid );
      channel.band = freq_data->link_list[cnt].link_info.band;
      channel.chan_num = freq_data->link_list[cnt].link_info.channel;

      if ( freq_data->link_list[cnt].link_info.type == CXM_LNK_TYPE_DIVERSITY )
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "DIV type! " );
        for ( i = 0; i < hdrmultirat.coex_info.div.channel_cnt; i++ )
        {
          if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.div.channel[i] ))
          {
            hdrmultirat.coex_info.div.freqid[i] = freq_data->link_list[cnt].freqid;
          }
        }
      }
      else if ( freq_data->link_list[cnt].link_info.direction == HDRMULTIRAT_REVERSE_LINK )
      {
        /* Store freq id in our local data structure also */

        for ( i = 0; i < hdrmultirat.coex_info.tx.channel_cnt; i++ )
        {
          if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.tx.channel[i] ))
          {
            hdrmultirat.coex_info.tx.freqid[i] = freq_data->link_list[cnt].freqid;
          }
        }
      }/* if reverse link */
      else if ( freq_data->link_list[cnt].link_info.direction == HDRMULTIRAT_FORWARD_LINK )
      {
        /* Store freq id in our local data structure also */
        for (i = 0; i < hdrmultirat.coex_info.rx.channel_cnt; i++ )
        {
          if ( SYS_CHAN_CMP_EQUAL(channel, hdrmultirat.coex_info.rx.channel[i] ))
          {
            hdrmultirat.coex_info.rx.freqid[i] = freq_data->link_list[cnt].freqid;
            hdrmultirat.coex_info.rx.demod_idx[i] = hdrsrchset_get_carrier_demod_id( channel );
          }
        }
      }/* else if forward link */

    } /* for num_link_info_sets */

    rex_leave_crit_sect( &hdrmultirat.crit_sect );
      /* leave critical section */

  } /* No DSDA mode*/

  hdrmultirat_report_coex_power( FALSE );
    /* Send one quick power update to MCS */ 

}/* hdrmultirat_process_freq_id_list */
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_FREQ_CONFIG_MSG

DESCRIPTION
  This function sends band-chan change msg to MCS.
  This function also sends message to FW when total channel count is 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_send_freq_config_msg( void )
{
  
  cxm_wwan_tech_state_ind_s    freq_cfg_msg;       

  uint8                        cnt = 0;

  uint8                        lcnt = 0;

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
  hdrfw_dsdx_frequency_msg_t         dsda_frequency_cfg_msg;
    /* fw dsdx mode enable msg */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: Sending freq id list" );
  memset((void *)&freq_cfg_msg, 0, sizeof(freq_cfg_msg));  

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 

  memset( &dsda_frequency_cfg_msg,
          0,
          sizeof( hdrfw_dsdx_frequency_msg_t ) );
    /* Clear the message */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

  msgr_init_hdr( &freq_cfg_msg.msg_hdr,
                 MSGR_HDR_MULTIRAT,
                 MCS_CXM_WWAN_TECH_STATE_IND );
    /* Init frequesncy config message */

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  freq_cfg_msg.tech_id = CXM_TECH_HDR;
  freq_cfg_msg.tech_data.num_link_info_sets = hdrmultirat.coex_info.rx.channel_cnt
     + hdrmultirat.coex_info.tx.channel_cnt + hdrmultirat.coex_info.div.channel_cnt;
  
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
  
  for ( cnt = 0; cnt < HDRMULTIRAT_MAX_NUM_CARRIERS; cnt++ )
  {
    dsda_frequency_cfg_msg.txfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
    dsda_frequency_cfg_msg.rxfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
    /* Initialize frequency Id */
  }
  
  if (freq_cfg_msg.tech_data.num_link_info_sets == 0)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Total Channel count is 0");

    /* To check if total channel count (no. of frequencies) is 0. If it is 0, 
       then Numcarriers set to 0. */
    if ( hdrmdsp_is_mdsp_active() )
    {
     msgr_init_hdr( &dsda_frequency_cfg_msg.hdr, 
                    MSGR_HDR_MULTIRAT, 
                    HDR_FW_DSDX_FREQUENCY_UPDATE_CMD );
       /* Init the message header */
    
     dsda_frequency_cfg_msg.rxNumcarriers = hdrmultirat.coex_info.rx.channel_cnt;
     dsda_frequency_cfg_msg.txNumcarriers = hdrmultirat.coex_info.tx.channel_cnt;
    
     HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR DSDA: rxNumcarriers: %d txNumcarriers: %d",
                                   dsda_frequency_cfg_msg.rxNumcarriers, 
                                   dsda_frequency_cfg_msg.txNumcarriers );
     hdrmultirat_send_msg( &dsda_frequency_cfg_msg.hdr, 
                        sizeof( hdrfw_dsdx_frequency_msg_t ) );
    }
  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
  
  for ( cnt = 0, lcnt =0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++, lcnt++ )
  {
    /* If forward link get rx freq */
    freq_cfg_msg.tech_data.link_list[lcnt].frequency = 
        rfm_cdma_get_rx_carrier_freq( hdrmultirat.coex_info.rx.channel[cnt] ); 
    
    freq_cfg_msg.tech_data.link_list[lcnt].bandwidth = HDRMULTIRAT_HDR_BANDWIDTH; 
      /* hard coded for now 1.25 Mhz*/
    freq_cfg_msg.tech_data.link_list[lcnt].band = 
      hdrmultirat.coex_info.rx.channel[cnt].band;      
    freq_cfg_msg.tech_data.link_list[lcnt].channel =  
      hdrmultirat.coex_info.rx.channel[cnt].chan_num;   
    freq_cfg_msg.tech_data.link_list[lcnt].rf_device = 
      hdrmultirat.coex_info.rx.rf_device; 
    freq_cfg_msg.tech_data.link_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK; 
  }

  for ( cnt = 0 ; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++, lcnt++ )
  {
    freq_cfg_msg.tech_data.link_list[lcnt].frequency = 
        rfm_cdma_get_tx_carrier_freq( hdrmultirat.coex_info.tx.channel[cnt] ); 
    
    freq_cfg_msg.tech_data.link_list[lcnt].bandwidth = HDRMULTIRAT_HDR_BANDWIDTH; 
      /* hard coded for now 1.25 Mhz*/
    freq_cfg_msg.tech_data.link_list[lcnt].band = 
      hdrmultirat.coex_info.tx.channel[cnt].band;      
    freq_cfg_msg.tech_data.link_list[lcnt].channel =  
      hdrmultirat.coex_info.tx.channel[cnt].chan_num;   
    freq_cfg_msg.tech_data.link_list[lcnt].rf_device = 
      hdrmultirat.coex_info.tx.rf_device; 
    freq_cfg_msg.tech_data.link_list[lcnt].direction = HDRMULTIRAT_REVERSE_LINK; 
  }

  /* If div chain then report that to MCS as well */
    for ( cnt = 0 ; cnt < hdrmultirat.coex_info.div.channel_cnt; cnt++, lcnt++ )
    {
	  freq_cfg_msg.tech_data.link_list[lcnt].frequency = 
        rfm_cdma_get_rx_carrier_freq( hdrmultirat.coex_info.rx.channel[cnt] );

    freq_cfg_msg.tech_data.link_list[lcnt].bandwidth = 
      1250000; /* hard coded for now 1.25 Mhz*/
    freq_cfg_msg.tech_data.link_list[lcnt].band = 
        hdrmultirat.coex_info.rx.channel[cnt].band;      
    freq_cfg_msg.tech_data.link_list[lcnt].channel =  
        hdrmultirat.coex_info.rx.channel[cnt].chan_num;   
    freq_cfg_msg.tech_data.link_list[lcnt].rf_device = 
        hdrmultirat.coex_info.div.rf_device; 
    freq_cfg_msg.tech_data.link_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK; 
    freq_cfg_msg.tech_data.link_list[lcnt].type = CXM_LNK_TYPE_DIVERSITY;
    

  }

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
  /* leave critical section */

  hdrmultirat_send_msg(&freq_cfg_msg, sizeof(freq_cfg_msg));
} /* hdrmultirat_send_freq_config_msg */

/*===========================================================================

FUNCTION HDRMULTIRAT_COMPARE_FREQ_CHANGE

DESCRIPTION
  This function compares if rx frequncy has changed or tuning to same freq
  ad before

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_compare_freq_change
(  
   const sys_channel_type                *channel,
     /* Channel to tune the RF to */

   uint8                                 channel_cnt
     /* Num channels */

)
{
  uint8 i = 0;
    /* temp variable */

  boolean match = 0;
    /* store if channels matches or not */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If num channels do not match return */
  if ( channel_cnt != hdrmultirat.coex_info.rx.channel_cnt )
  {
    return TRUE;
  }
  for ( i = 0;  i < channel_cnt; i++ )
  {
    match = 
        SYS_CHAN_CMP_EQUAL(channel[i], hdrmultirat.coex_info.rx.channel[i]);

    if ( !match )
    {
      return TRUE;
    }

  }
  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: Freq did not change" );

  return FALSE;

} /* hdrmultirat_compare_freq_change */
/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_FREQ_CHANGE

DESCRIPTION
  This function sends band-chan change msg to MCS whenever it changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_freq_change
(
  rfm_device_enum_type                  rf_device,
    /* The logical chain ID */

  const sys_channel_type                *channel,
    /* Channel to tune the RF to */

  cxm_tech_link_direction               direction,
    /* Direction - reverse/forward link */

  uint8                                 channel_cnt
    /* Num channels */
)
{
 
  uint8                          cnt = 0;
    /* temp loop variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */
  /* If we get tx freq changed indication send both tx and  saved rx we have*/
  if ( direction == HDRMULTIRAT_REVERSE_LINK )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DSDA: Sending freq list UL + DL %d", channel_cnt );
   
    hdrmultirat.coex_info.tx.channel_cnt = channel_cnt;
    hdrmultirat.coex_info.tx.rf_device = rf_device;
    for ( cnt = 0; cnt < channel_cnt; cnt++ )
    {
      hdrmultirat.coex_info.tx.channel[cnt] =  channel[cnt];
    }
    hdrmultirat_send_freq_config_msg ( );

#ifdef FEATURE_HDR_BAND_AVOIDANCE
    if( hdrmultirat_is_dsda_enabled() )
    {
      hdrmultirat_send_ba_freq_msg();
    }
#endif /* FEATURE_HDR_BAND_AVOIDANCE */
  } 
  /* If forward link change then send only that if it has changed from current */
  else if ( direction == HDRMULTIRAT_FORWARD_LINK ) 
  {
    /* If no rx freq saved or frequency has changed or device has changed
       send this else nothing has changed donot send */
    /* Send this even if freq ID list was not updated 
       due to MDSP deactivate.*/
    if ( hdrmultirat_compare_freq_change(channel,channel_cnt) ||
         hdrmultirat.frequency_update_pending ||
         ( hdrmultirat.coex_info.rx.rf_device != rf_device ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Freq change DL only " );

      /* Update current configuration and send it */

      hdrmultirat.coex_info.rx.channel_cnt = channel_cnt;
      hdrmultirat.coex_info.rx.rf_device = rf_device;

      for ( cnt = 0; cnt < channel_cnt; cnt++ )
      {
        hdrmultirat.coex_info.rx.channel[cnt] =  channel[cnt];
        hdrmultirat.coex_info.rx.demod_idx[cnt] =  hdrsrchset_get_carrier_demod_id( channel[cnt] );
      }

      /* If we are in connected state dont update MCS just yet,
         wait for tx tune and send update */
      if ( hdrsrch_get_current_state() != HDRSRCH_CONNECTED_STATE )
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: Sending freq list DL only " );

        hdrmultirat_send_freq_config_msg( );
      }
      
    } /* if freq change */   
  } /* If forward link */

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
    /* leave critical section */

} /* hdrmultirat_process_freq_change */

/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_DIV_CHANGE

DESCRIPTION
  This function informs MCS of any change in diversity mdoe.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_div_change
(
  boolean                               div_mode,
    /* Div ON/OFF */
  
  rfm_device_enum_type                  rf_device
    /* The logical chain ID */
)
{
  uint8                                 cnt = 0;
    /* Loop counting variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Turned div on */
  if ( div_mode )
  {
    if ( rf_device == RFM_INVALID_DEVICE )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR DSDA: Invalid div rfm device!!");
      return;
    }

    /* diversity channel count should be same as primary channel count */
    hdrmultirat.coex_info.div.channel_cnt = 
        hdrmultirat.coex_info.rx.channel_cnt;

    for ( cnt = 0 ; cnt < hdrmultirat.coex_info.div.channel_cnt; cnt++ )
    {
      /* Band channel for diversity should be same as primary only
         rf device will differ */
      hdrmultirat.coex_info.div.channel[cnt].band = 
        hdrmultirat.coex_info.rx.channel[cnt].band;
      hdrmultirat.coex_info.div.channel[cnt].chan_num = 
        hdrmultirat.coex_info.rx.channel[cnt].chan_num;
      hdrmultirat.coex_info.div.rf_device = rf_device;
    }    
  }
  else /* Turned Div off */
  {
    hdrmultirat.coex_info.div.channel_cnt = 0;
    hdrmultirat.coex_info.div.rf_device = RFM_INVALID_DEVICE;
  }

  hdrmultirat_send_freq_config_msg ( );

} /* hdrmultirat_process_div_change */
/*===========================================================================

FUNCTION HDRMULTIRAT_REPORT_COEX_POWER

DESCRIPTION
  This function sends coex rx tx power to MCS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_report_coex_power 
( 
   boolean               save_only
     /* Just save the power or send it to MCS? */
)  
{
 
  uint8                          cnt = 0;
    /* Temporary loop variable */

  uint8                          lcnt = 0;
    /* Temporary loop variable */

  int16                          tx_power =0 ;
    /* Tx power from RF */

  int16                          rx_power = 0;
    /* Rx power from RF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  if ( save_only )
  {
    /* If we are about to sleep just store the power
       to report on next wakeup */

    for ( cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
    {
      hdrmultirat.coex_info.rx.coex_power[cnt] = rfm_hdr_get_filtered_rx_power( 
         hdrmultirat.coex_info.rx.rf_device, hdrmultirat.coex_info.rx.demod_idx[cnt], RFM_CDMA_AGC_FORMAT__DBM10);
    }

    hdrmultirat.coex_info.tx.coex_power[0] = rfm_hdr_get_filtered_tx_power_v2 ( 
       hdrmultirat.coex_info.tx.rf_device, RFM_TX_FILTER_POWER_WHEN_TX_ON );
  }
  else
  {
    memset((void *)&hdrmultirat_pwr_cfg_msg, 0, sizeof(hdrmultirat_pwr_cfg_msg));  

    msgr_init_hdr( &hdrmultirat_pwr_cfg_msg.msg_hdr,
                   MSGR_HDR_MULTIRAT,
                   MCS_CXM_COEX_POWER_IND );
      /* Init frequesncy config message */

    /* fill in all info */
    hdrmultirat_pwr_cfg_msg.tech_id = CXM_TECH_HDR;
    
    /* MCS_CXM_COEX_POWER_IND */
    for ( lcnt = 0,cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++,lcnt++ )
    {      

      hdrmultirat_pwr_cfg_msg.power_data.num_link_info_sets++;

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid = hdrmultirat.coex_info.rx.freqid[cnt];

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK;

      rx_power = rfm_hdr_get_filtered_rx_power( 
         hdrmultirat.coex_info.rx.rf_device, hdrmultirat.coex_info.rx.demod_idx[cnt], RFM_CDMA_AGC_FORMAT__DBM10);

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].rx_power = rx_power;
      
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DSDA: Sending coex rx power info"
                                      " %d demod idx %d", rx_power,hdrmultirat.coex_info.rx.demod_idx[cnt]);      
    }

    for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++,lcnt++ )
    {
      hdrmultirat_pwr_cfg_msg.power_data.num_link_info_sets++;

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid = hdrmultirat.coex_info.tx.freqid[cnt];

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].direction= HDRMULTIRAT_REVERSE_LINK;

      tx_power = rfm_hdr_get_filtered_tx_power_v2 ( 
           hdrmultirat.coex_info.tx.rf_device, RFM_TX_FILTER_POWER_WHEN_TX_ON );

      hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].tx_power = tx_power;
      
      HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: Sending coex tx power info"
                                      " %d", tx_power );
    }

    /* If we are in traffic and div is on then update rx1 power to MCS as well  */
    if ( hdrsrch_get_current_state() == HDRSRCH_CONNECTED_STATE )
    {
      for ( cnt = 0; cnt < hdrmultirat.coex_info.div.channel_cnt; cnt++,lcnt++ )
      {
        hdrmultirat_pwr_cfg_msg.power_data.num_link_info_sets++;
      
        hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid = hdrmultirat.coex_info.div.freqid[cnt];

        hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].direction= HDRMULTIRAT_FORWARD_LINK;

        rx_power = rfm_hdr_get_filtered_rx_power( 
           hdrmultirat.coex_info.div.rf_device, hdrmultirat.coex_info.rx.demod_idx[cnt], RFM_CDMA_AGC_FORMAT__DBM10);

        hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].rx_power = rx_power;
        
        HDR_MSG_PROT_3( MSG_LEGACY_MED, "HDR DSDA: Sending coex DIV power info"
                                        " %d for freq_id %d demod id %d", rx_power,
                         hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid,hdrmultirat.coex_info.rx.demod_idx[cnt]);
      }
    }

    hdrmultirat_send_msg(&hdrmultirat_pwr_cfg_msg, sizeof(hdrmultirat_pwr_cfg_msg));    
    
  } /* not in sleep */

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
    /* leave critical section */
  
} /* hdrmultirat_report_coex_power */

/* ===========================================================================

FUNCTION HDRMULTIRAT_REPORT_COEX_POWER_WITH_BIAS

DESCRIPTION
  This function sends the coes power we saved before goind to sleep + bias
  to MCS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_report_coex_power_with_bias( void )
{
 
  uint8                          cnt = 0;
    /* Temporary loop variable */

  uint8                          lcnt = 0;
    /* Temporary loop variable */

  int16                          tx_power =0;
    /* Tx power from RF */

  int16                          rx_power = 0;
    /* Rx power from RF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  memset((void *)&hdrmultirat_pwr_cfg_msg, 0, sizeof(hdrmultirat_pwr_cfg_msg));  

  msgr_init_hdr( &hdrmultirat_pwr_cfg_msg.msg_hdr,
                 MSGR_HDR_MULTIRAT,
                 MCS_CXM_COEX_POWER_IND );
    /* Init coex power config message */

  /* fill in all info */
  hdrmultirat_pwr_cfg_msg.tech_id = CXM_TECH_HDR;
      
  /* MCS_CXM_COEX_POWER_IND */
  for ( lcnt = 0,cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++,lcnt++ )
  {
    hdrmultirat_pwr_cfg_msg.power_data.num_link_info_sets++;

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid = hdrmultirat.coex_info.rx.freqid[cnt];

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK;

    rx_power = hdrmultirat.coex_info.rx.coex_power[cnt]  - HDRMULTIRAT_COEX_PWR_BIAS; 

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].rx_power = rx_power;

    
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: Sending coex rx power %d"
                                   " (with bias )",rx_power );
      
  }

  for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++,lcnt++ )
  {
    hdrmultirat_pwr_cfg_msg.power_data.num_link_info_sets++;

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].freqid = hdrmultirat.coex_info.tx.freqid[cnt];

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].direction= HDRMULTIRAT_REVERSE_LINK;

    tx_power = hdrmultirat.coex_info.tx.coex_power[cnt]  - HDRMULTIRAT_COEX_PWR_BIAS; 

    hdrmultirat_pwr_cfg_msg.power_data.link_list[lcnt].tx_power = tx_power;

    HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: Sending coex tx power "
                                   "(with bias) %d",tx_power );

  }

  hdrmultirat_send_msg(&hdrmultirat_pwr_cfg_msg, sizeof(hdrmultirat_pwr_cfg_msg));    

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
    /* leave critical section */

} /* hdrmultirat_report_coex_power_wit_bias */

/* ===========================================================================

FUNCTION HDRMULTIRAT_PRIORITY_SWITCH

DESCRIPTION
  This function gets the priority for the activity level and then sends it to
  FW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_priority_switch
( 
   hdrmultirat_priority_type  priority_level
)
{
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
   uint32                        priority = 0;
     /* Variable to store priority */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE ) 
#ifdef FEATURE_HDR_QTA
    || ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_QTA */
#ifdef FEATURE_HDR_DR_DSDS
       || ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_DR_DSDS */
     )
  {
    priority = hdrmultirat_get_cxm_priority( priority_level );
      /* Lower priority after reacq */


    HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA:Sending priority %d",priority );

    hdrmultirat_send_dsda_priority_cfg_msg( priority, priority );
      /* Tell FW about it */

  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */    

} /* hdrmultirat_priority_switch */

/* ===========================================================================

FUNCTION HDRMULTIRAT_CXM_ALLOWS_DTX

DESCRIPTION
  This function queries coex manager if its OK to enable dtx depending on
  C+G band class/channel combinations with G.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if DTX is allowed
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_cxm_allows_dtx( void )
{

  uint8 cnt = 0;
    /* Temporary loop variable */

  boolean enable_dtx = TRUE;
    /* Cxm allows dtx? */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE )
#ifdef FEATURE_HDR_QTA
    || ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_QTA */
#ifdef FEATURE_HDR_DR_DSDS
       || ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_DR_DSDS */
     )
  {
    if ( hdrmultirat.coex_info.tx.channel_cnt == 0 )
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: No tx freq id, allow dtx" );
      enable_dtx = TRUE;
      return enable_dtx;
    }

    hdrmultirat_coex_dtx_query.num_requesting_ids = 
      hdrmultirat.coex_info.tx.channel_cnt;

    for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++ )
    {
      hdrmultirat_dtx_req_ids[cnt] = 
      hdrmultirat.coex_info.tx.freqid[cnt];

      HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: Sending tx freq id  %d",
                                   hdrmultirat_dtx_req_ids[cnt] );
    }

    hdrmultirat_coex_dtx_query.requesting_ids =  hdrmultirat_dtx_req_ids;

    hdrmultirat_coex_dtx_query.actions = hdrmultirat_dtx_actions;


    if ( cxm_query_highest_action( &hdrmultirat_coex_dtx_query ) )
    {
      for (cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++ )
      {
        HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DSDA: Rxd response from cxm "
                                        "freq id %d dtx_action %d",
                        hdrmultirat_coex_dtx_query.requesting_ids[cnt],
                        hdrmultirat_coex_dtx_query.actions[cnt] );

        if ( hdrmultirat_coex_dtx_query.actions[cnt] != ACTION_NONE )
        {
          enable_dtx = FALSE;
        }
      
      }
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR DSDA: "
                                    "CXM DTX QUERY returned ERROR!");    
      enable_dtx = FALSE;
    }
       
  }
   
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: DTX enabled %d ", enable_dtx);

  return enable_dtx;
  
} /* hdrmultirat_cxm_allows_dtx */

/* ===========================================================================

FUNCTION HDRMULTIRAT_CXM_ALLOWS_DLNA

DESCRIPTION
  This function queries coex manager if its OK to enable D-LNA depending on
  C+G band class/channel combinations with G.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if D-LNA is allowed
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_cxm_allows_dlna( void )
{

  uint8 cnt = 0;
    /* Temporary loop variable */

  boolean enable_dlna = TRUE;
    /* Cxm allows dlna? */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE 
#ifdef FEATURE_HDR_QTA
    || ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_QTA */
#ifdef FEATURE_HDR_DR_DSDS
    || ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY )
#endif /* FEATURE_HDR_DR_DSDS */

     )
  {
    if ( hdrmultirat.coex_info.rx.channel_cnt == 0 )
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: No rx freq id, allow dlna" );
      enable_dlna = TRUE;
      return enable_dlna;
    }

    hdrmultirat_coex_dlna_query.num_requesting_ids = 
      hdrmultirat.coex_info.rx.channel_cnt;

    for ( cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
    {
      hdrmultirat_dlna_req_ids[cnt] = 
      hdrmultirat.coex_info.rx.freqid[cnt];

      HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: Sending rx freq id  %d",
                                   hdrmultirat_dlna_req_ids[cnt] );
    }

    hdrmultirat_coex_dlna_query.requesting_ids =  hdrmultirat_dlna_req_ids;

    hdrmultirat_coex_dlna_query.actions = hdrmultirat_dlna_actions;


    if ( cxm_query_highest_action( &hdrmultirat_coex_dlna_query ) )
    {
      for (cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
      {
        HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DSDA: Rxd response from cxm "
                                        "freq id %d dlna_action %d",
                        hdrmultirat_coex_dlna_query.requesting_ids[cnt],
                        hdrmultirat_coex_dlna_query.actions[cnt] );

        if ( hdrmultirat_coex_dlna_query.actions[cnt] != ACTION_NONE )
        {
          enable_dlna = FALSE;
        }
      
      }
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR DSDA: "
                                    "CXM D-LNA QUERY returned ERROR!");    
      enable_dlna = FALSE;
    }
       
  }
   
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: is Ok to enable D-LNA? %d ", enable_dlna);

  return enable_dlna;
  
} /* hdrmultirat_cxm_allows_dlna */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_BAND_AVOIDANCE
/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_BAND_AVOID_IND

DESCRIPTION
  Process the band avoidance indication from MCS.
  Currently for DO no implementation is required to
  force handover due to band avoidance.
  Required to send NACK when MCS expects a response to perform Band Avoidance

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_band_avoid_ind ( void )
{
   cxm_coex_ba_blist_res_s  band_avoid_rsp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void *)&band_avoid_rsp, 0, sizeof(band_avoid_rsp));  

  msgr_init_hdr( &band_avoid_rsp.msg_hdr,
                 MSGR_HDR_MULTIRAT,
                 MCS_CXM_BAND_AVOID_BLIST_RSP );
    /* Init band avoidance response message */

  band_avoid_rsp.blist_res_data.tech_id = CXM_TECH_HDR;
  band_avoid_rsp.blist_res_data.freq_avoid_res = FREQ_AVOID_NACK;

  hdrmultirat_send_msg(&band_avoid_rsp, sizeof(band_avoid_rsp));
    /* send the message  */
  
  HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR BA: received BA Ind from MCS, sending Nack!" );

}/* hdrmultirat_process_band_avoid_ind */

/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_BA_FREQ_MSG

DESCRIPTION
  This function sends band-chan change msg to MCS for Band Avoidance.
  Tech L1 must only send serving frequency in CONNECTED mode
  Tech L1 must send empty serving frequency otherwise


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_send_ba_freq_msg( void )
{
  
  cxm_coex_ba_freq_ind_s    freq_cfg_msg;       

  uint8                        cnt = 0;

  uint8                        lcnt = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR BA: Sending freq id list" );
  memset((void *)&freq_cfg_msg, 0, sizeof(freq_cfg_msg));  

  msgr_init_hdr( &freq_cfg_msg.msg_hdr,
                 MSGR_HDR_MULTIRAT,
                 MCS_CXM_BAND_AVOID_FREQ_IND );
    /* Init frequesncy config message */

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  freq_cfg_msg.serv_freq_data.tech_id = CXM_TECH_HDR;



  /* If not in connected state, send empty serving frequency */
  if ( hdrmultirat.coex_info.tx.rf_device == RFM_INVALID_DEVICE )
  {
    freq_cfg_msg.serv_freq_data.num_entries = 0;

  }
  else
  {
    freq_cfg_msg.serv_freq_data.num_entries = hdrmultirat.coex_info.rx.channel_cnt
     + hdrmultirat.coex_info.tx.channel_cnt + hdrmultirat.coex_info.div.channel_cnt;

    for ( cnt = 0, lcnt =0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++, lcnt++ )
    {
      /* If forward link get rx freq */
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].frequency = 
        rfm_cdma_get_rx_carrier_freq( hdrmultirat.coex_info.rx.channel[cnt] ); 
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].bandwidth = HDRMULTIRAT_HDR_BANDWIDTH;
        /* hard coded for now 1.25 Mhz*/
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].band = 
        hdrmultirat.coex_info.rx.channel[cnt].band;      
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].channel =  
        hdrmultirat.coex_info.rx.channel[cnt].chan_num;   
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK; 
    }

    for ( cnt = 0 ; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++, lcnt++ )
    {
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].frequency = 
        rfm_cdma_get_tx_carrier_freq( hdrmultirat.coex_info.tx.channel[cnt] ); 
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].bandwidth = HDRMULTIRAT_HDR_BANDWIDTH;
        /* hard coded for now 1.25 Mhz*/
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].band = 
        hdrmultirat.coex_info.tx.channel[cnt].band;      
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].channel =  
        hdrmultirat.coex_info.tx.channel[cnt].chan_num;   
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].direction = HDRMULTIRAT_REVERSE_LINK; 
    }

    /* If div chain then report that to MCS as well */
    for ( cnt = 0 ; cnt < hdrmultirat.coex_info.div.channel_cnt; cnt++, lcnt++ )
    {
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].frequency = 
        rfm_cdma_get_rx_carrier_freq( hdrmultirat.coex_info.rx.channel[cnt] );

      freq_cfg_msg.serv_freq_data.freq_list[lcnt].bandwidth = 
       1250000; /* hard coded for now 1.25 Mhz*/
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].band = 
        hdrmultirat.coex_info.rx.channel[cnt].band;      
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].channel =  
        hdrmultirat.coex_info.rx.channel[cnt].chan_num;   
      freq_cfg_msg.serv_freq_data.freq_list[lcnt].direction = HDRMULTIRAT_FORWARD_LINK; 
      //freq_cfg_msg.serv_freq_data.freq_list[lcnt].type = CXM_LNK_TYPE_DIVERSITY;
    }

  }
  
  rex_leave_crit_sect( &hdrmultirat.crit_sect );
  /* leave critical section */

  hdrmultirat_send_msg(&freq_cfg_msg, sizeof(freq_cfg_msg));

} /* hdrmultirat_send_ba_freq_msg(); */

/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_BAND_AVOID_POWER

DESCRIPTION
  This function sends band_avoidance power update to MCS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_send_band_avoid_power( void )
{
  
  cxm_coex_ba_pwr_ind_s        ba_pwr_msg;    

  uint8                        cnt = 0;

  uint8                        lcnt = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR BA: Sending power" );
  memset((void *)&ba_pwr_msg, 0, sizeof(ba_pwr_msg));  

  msgr_init_hdr( &ba_pwr_msg.msg_hdr,
                 MSGR_HDR_MULTIRAT,
                 MCS_CXM_BAND_AVOID_PWR_IND );
    /* Init frequesncy config message */

  rex_enter_crit_sect( &hdrmultirat.crit_sect );
    /* enter critical section */

  ba_pwr_msg.serv_pwr_data.tech_id = CXM_TECH_HDR;

  ba_pwr_msg.serv_pwr_data.num_entries = hdrmultirat.coex_info.rx.channel_cnt
     + hdrmultirat.coex_info.tx.channel_cnt + hdrmultirat.coex_info.div.channel_cnt;

  if ( ba_pwr_msg.serv_pwr_data.num_entries > CXM_MAX_SUPP_FREQ_LINK_BA )
  {
     HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "HDR BA: ERROR: num entries %d "
                                       "is greater than MAX supported!"
                     , ba_pwr_msg.serv_pwr_data.num_entries );
  }
  
  for ( cnt = 0, lcnt =0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++, lcnt++ )
  {

    ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].direction = CXM_LNK_DRCTN_DL;
    ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].rx_pwr = hdrmultirat.ba.rx_pri_pwr_value[cnt];
    
  }

  for ( cnt = 0 ; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++, lcnt++ )
  {
    ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].direction = CXM_LNK_DRCTN_UL ;
    ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].tx_pwr = hdrmultirat.ba.tx_pwr_value[cnt];
  }

  /* If div chain then report that to MCS as well */
  for ( cnt = 0 ; cnt < hdrmultirat.coex_info.div.channel_cnt; cnt++, lcnt++ )
  {
	  ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].direction = CXM_LNK_DRCTN_DL;
    ba_pwr_msg.serv_pwr_data.pwr_list[lcnt].rx_pwr = hdrmultirat.ba.rx_sec_pwr_value[cnt];
    
  }

  rex_leave_crit_sect( &hdrmultirat.crit_sect );
  /* leave critical section */

  hdrmultirat_send_msg(&ba_pwr_msg, sizeof(ba_pwr_msg));

} /* hdrmultirat_send_band_avoid_power */

/*==========================================================================

FUNCTION        HDRMULTIRAT_INIT_BA_DATA

DESCRIPTION
  This function intializes counts for band avoindace power reporting

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrmultirat_init_ba_data( void )
{
  /* Reset rx/tx counts to 0 */
  hdrmultirat.ba.count = 0;

}
/*===========================================================================
FUNCTION    HDRMULTIRAT_INT32_COMPARE

DESCRIPTION This function compares 2 int32s and returns the appropriate value
  
DEPENDENCIES
  None

RETURN VALUE
  comparision result

SIDE EFFECTS
  None

===========================================================================*/
static int hdrmultirat_int32_compare(const void* a, const void* b)
{
  return ( *( (int32*)a) - *( (int32*)b) );
} /* hdrmultirat_int32_compare */
/*==========================================================================

FUNCTION        HDRMULTIRAT_SAVE_BA_POWER

DESCRIPTION
  This function saves power for band avoidance.
 
  The expectation for filtered Rx power is to select the 10th percentile in
  a sampling over two seconds (the frequency at which the message is to
  be sent). From an implementation perspective, this means that the tech
  must maintain a sorted sampling over these two seconds.
  For example, if 20 samples are taken,
  the tech would report (.10)*20 = 2nd entry in the sorted list.

  The filtered Tx power should be 90th percentile over time.
 
  Algortihm to obtain rx/tx power:

  Save power sample every 20ms.
  When we have 100 samples, do a sort on the array and grab the 10th sample.
  We want the low side of the list for Rx (10% up from the smallest value)
  and the high side of the list for Tx (10% down from the largest value).
  

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
errno_enum_type hdrmultirat_save_ba_power ( void )
{
  
  uint8                        cnt = 0;

  errno_enum_type                       status = E_SUCCESS;
     /* Command completion status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if( hdrmultirat_is_dsda_enabled() )
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "HDR BA: Saving power" );
      /* Capture one of the 100 tx power samples needed */
      for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++ )
      {
        hdrmultirat.ba.tx_pwr[cnt][hdrmultirat.ba.count]= rfm_hdr_get_filtered_tx_power_v2 ( 
         hdrmultirat.coex_info.tx.rf_device, RFM_TX_FILTER_POWER_WHEN_TX_ON );
      }
      
      for ( cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
      {
        hdrmultirat.ba.rx_pri_pwr[cnt][hdrmultirat.ba.count]= rfm_hdr_get_filtered_rx_power( 
           hdrmultirat.coex_info.rx.rf_device, hdrmultirat.coex_info.rx.demod_idx[cnt], RFM_CDMA_AGC_FORMAT__DBM10);
      }
      
      for ( cnt = 0; cnt <hdrmultirat.coex_info.div.channel_cnt; cnt++ )
      {
        hdrmultirat.ba.rx_sec_pwr[cnt][hdrmultirat.ba.count] = rfm_hdr_get_filtered_rx_power( 
               hdrmultirat.coex_info.div.rf_device, hdrmultirat.coex_info.rx.demod_idx[cnt], RFM_CDMA_AGC_FORMAT__DBM10);
      }

      hdrmultirat.ba.count++;

      if ( hdrmultirat.ba.count >= HDRMULTIRAT_BAND_AVOID_ENTRIES )
      {
        for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++ )
        {
          qsort( &(hdrmultirat.ba.tx_pwr[cnt][0]), HDRMULTIRAT_BAND_AVOID_ENTRIES, sizeof(int32), 
                hdrmultirat_int32_compare);
          /* grab the 90th percentile over time (10% value from high side of list) */
          hdrmultirat.ba.tx_pwr_value[cnt] = hdrmultirat.ba.tx_pwr[cnt][HDRMULTIRAT_BAND_AVOID_90PERCENT];
          /* send message */
        }
        for ( cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
        {
          /* sort rx_pri_pwr list */
          qsort( &(hdrmultirat.ba.rx_pri_pwr[cnt][0]), HDRMULTIRAT_BAND_AVOID_ENTRIES, sizeof(int32),
                 hdrmultirat_int32_compare);

          /* grab the 10th percentile over time (10% value from low side of list) */
          hdrmultirat.ba.rx_pri_pwr_value[cnt] = hdrmultirat.ba.rx_pri_pwr[cnt][HDRMULTIRAT_BAND_AVOID_10PERCENT];
        }
        
        if ( hdrmultirat.coex_info.div.channel_cnt > 0)
        {
          qsort( &(hdrmultirat.ba.rx_sec_pwr[cnt][0]), HDRMULTIRAT_BAND_AVOID_ENTRIES, sizeof(int32),
                 hdrmultirat_int32_compare ); 
          hdrmultirat.ba.rx_sec_pwr_value[cnt] = hdrmultirat.ba.rx_sec_pwr[cnt][HDRMULTIRAT_BAND_AVOID_10PERCENT];
        }
        hdrmultirat.ba.count = 0;  
        hdrmultirat_send_band_avoid_power();
      }

      timer_set( &hdrsrch.band_avoid_timer, HDRMULTIRAT_COEX_PWR_TIMER_MS,
                 0, T_MSEC );

    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR BA: Not is DSDA mode!!" );
    }
    return status;
} /* hdrmultirat_save_ba_power */
#endif /* FEATURE_HDR_BAND_AVOIDANCE */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW 
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDX_ENABLE_CFG_MSG

DESCRIPTION
  Send DSDX enable msg to FW

DEPENDENCIES
  None

PARAMETERS
  State to send to FW

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_dsdx_enable_cfg_msg 
( 
   hdrmultirat_mode_pref_enum_type state 
)
{
  hdrfw_dsdx_enable_msg_t         dsda_enable_cfg_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: send dsda cfg msg" );

  memset( &dsda_enable_cfg_msg,
          0,
          sizeof( hdrfw_dsdx_enable_msg_t ) );
    /* Clear the message */

  msgr_init_hdr( &dsda_enable_cfg_msg.hdr, 
                 MSGR_HDR_MULTIRAT, 
                 HDR_FW_DSDX_ENABLE_CMD );
    /* Init the message header */

  dsda_enable_cfg_msg.dsdxMode = ( hdrfw_dsdx_state_t ) state;
  
  hdrmultirat_send_msg( &dsda_enable_cfg_msg.hdr, 
                       sizeof( hdrfw_dsdx_enable_msg_t ) );

} /* hdrmultirat_send_dsdx_enable_cfg_msg */
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDA_PRIORITY_CFG_MSG

DESCRIPTION
  Send DSDA priority msg to FW

DEPENDENCIES
  None

PARAMETERS
  rx & tx priority

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_dsda_priority_cfg_msg 
( 
  uint32 rx_priority,
    /* Rx priority */

  uint32 tx_priority
    /* Tx priority */
)
{
  hdrfw_dsdx_priority_config_msg_t         dsda_priority_cfg_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: Sending dsda priority:");
  memset( &dsda_priority_cfg_msg,
          0,
          sizeof( hdrfw_dsdx_priority_config_msg_t ) );
    /* Clear the message */

  msgr_init_hdr( &dsda_priority_cfg_msg.hdr, 
                 MSGR_HDR_MULTIRAT, 
                 HDR_FW_REGISTER_RX_TX_ACTIVITY_CMD );
    /* Init the message header */

  dsda_priority_cfg_msg.rxPriority = rx_priority;
  dsda_priority_cfg_msg.txPriority = tx_priority;

#ifdef FEATURE_HDR_SELECTIVE_TXD
  dsda_priority_cfg_msg.asdivPriority = 
    hdrmultirat_get_cxm_priority( HDRMULTIRAT_HIGH_PRIORITY_ASDIV );
  /* DSDA ASDiv uses highest priority tier for switching. This info
     is passed to FW before actual switch, and FW handles bumping to
     and restoring asdiv Prio at the right moment */

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "TxD: ASDiv Prio: %d", 
                  dsda_priority_cfg_msg.asdivPriority );
#endif /* FEATURE_HDR_SELECTIVE_TXD */
  
  hdrmultirat_send_msg( &dsda_priority_cfg_msg.hdr, 
                       sizeof( hdrfw_dsdx_priority_config_msg_t ) );

}/* hdrmultirat_send_dsda_priority_cfg_msg */

/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDX_FREQUENCY_CFG_MSG

DESCRIPTION
  Send DSDA frequency ID list to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_dsdx_frequency_cfg_msg ( void )
{
   hdrfw_dsdx_frequency_msg_t  dsda_frequency_cfg_msg;
    /* fw dsdx frequency config msg */

   int cnt;
   /* Temp loop variable */

#ifdef FEATURE_HDR_FW_CHAN_ID
   cxm_channel_in_type chan_info = {CXM_TECH_HDR, RFM_INVALID_DEVICE} ;
     /* input argument to get channel_ID */

   cxm_channel_out_type  chan_id = {0,0};
     /* channel ID corresponding to RF Device */
#endif /* FEATURE_HDR_FW_CHAN_ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DSDA: Sending dsda frequency Ids to FW:");

  /* Make sure FW is still active */
  if ( hdrmdsp_is_mdsp_active()  )
  {
    /* Make sure we have Id to send to FW */
    if ( ( hdrmultirat.coex_info.tx.channel_cnt == 0 ) && 
         ( hdrmultirat.coex_info.rx.channel_cnt == 0 ))
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR DSDA: "
                    "No frequency information to send to FW!");
      return;
    }

    memset( &dsda_frequency_cfg_msg,
            0,
            sizeof( hdrfw_dsdx_frequency_msg_t ) );
      /* Clear the message */

    msgr_init_hdr( &dsda_frequency_cfg_msg.hdr, 
                   MSGR_HDR_MULTIRAT, 
                   HDR_FW_DSDX_FREQUENCY_UPDATE_CMD );
      /* Init the message header */

    for ( cnt = 0; cnt < HDRMULTIRAT_MAX_NUM_CARRIERS; cnt++ )
    {
      dsda_frequency_cfg_msg.txfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
      dsda_frequency_cfg_msg.rxfrequencyId[cnt]= HDRMULTIRAT_DEFAULT_FREQ_ID;
      /* Initialize frequency Id */
    }

    rex_enter_crit_sect( &hdrmultirat.crit_sect );
      /* enter critical section */   
    
#ifdef FEATURE_HDR_FW_CHAN_ID
      chan_info.rf_device = hdrmultirat.coex_info.rx.rf_device;
      chan_info.techid = CXM_TECH_HDR;

      if ( chan_info.rf_device == RFM_INVALID_DEVICE )
      {
        dsda_frequency_cfg_msg.rxChannelId[0] = 0;
        dsda_frequency_cfg_msg.rxChannelId[1] = 0;
        dsda_frequency_cfg_msg.txChannelId = 0;          
      }
      else
      {
        chan_id = cxm_get_channel_id( chan_info );
        dsda_frequency_cfg_msg.rxChannelId[0] = chan_id.rx_channel_id;
        dsda_frequency_cfg_msg.txChannelId = chan_id.tx_channel_id;

        /* If Diversity chain, then get channel id for div also*/
        if ( hdrmultirat.coex_info.div.channel_cnt )
        {
          chan_info.rf_device = hdrmultirat.coex_info.div.rf_device;
          chan_info.techid = CXM_TECH_HDR;
          chan_id = cxm_get_channel_id( chan_info );
          dsda_frequency_cfg_msg.rxChannelId[1] = chan_id.rx_channel_id;
        }
        else
        {
          dsda_frequency_cfg_msg.rxChannelId[1] = 0;
        }
      }
      
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "HDR DSDA: Rx chan id %d Div chain id %d Tx chan id %d", 
                      dsda_frequency_cfg_msg.rxChannelId[0],
                      dsda_frequency_cfg_msg.rxChannelId[1],
                      dsda_frequency_cfg_msg.txChannelId );
#endif /* FEATURE_HDR_FW_CHAN_ID */
  
      for ( cnt = 0; cnt < hdrmultirat.coex_info.tx.channel_cnt; cnt++ )
      {
        dsda_frequency_cfg_msg.txNumcarriers++;
        dsda_frequency_cfg_msg.txfrequencyId[cnt] = hdrmultirat.coex_info.tx.freqid[cnt];
        HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: freq id %d", 
                     dsda_frequency_cfg_msg.txfrequencyId[cnt] );
      }

      for ( cnt = 0; cnt < hdrmultirat.coex_info.rx.channel_cnt; cnt++ )
      {
        dsda_frequency_cfg_msg.rxNumcarriers++;
        dsda_frequency_cfg_msg.rxfrequencyId[cnt] = hdrmultirat.coex_info.rx.freqid[cnt];
        HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDA: freq id %d", 
                     dsda_frequency_cfg_msg.rxfrequencyId[cnt] );
      }
      
      rex_leave_crit_sect( &hdrmultirat.crit_sect );

      hdrmultirat_send_msg( &dsda_frequency_cfg_msg.hdr, 
                           sizeof( hdrfw_dsdx_frequency_msg_t ) );
    } /* If mdsp is active */
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: FW not active..not sending freq id" );
    }

}/* hdrmultirat_send_dsdx_frequency_cfg_msg */

#ifdef FEATURE_HDR_QTA
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_QTA_CFG_MSG

DESCRIPTION
  Send QTA start/stop msg to FW

DEPENDENCIES
  None

PARAMETERS
  State to send to FW

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_qta_cfg_msg 
( 
   hdrmultirat_qta_state_enum_type state 
)
{
  hdrfw_qta_cfg_msg_t         qta_cfg_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DSDA: send QTA cfg msg %d", state );

  memset( &qta_cfg_msg,
          0,
          sizeof( hdrfw_qta_cfg_msg_t ) );
    /* Clear the message */

  msgr_init_hdr( &qta_cfg_msg.hdr, 
                 MSGR_HDR_MULTIRAT, 
                 HDR_FW_QTA_CFG_CMD );
    /* Init the message header */

  qta_cfg_msg.qtaCmd = ( hdrfw_qta_cfg_t ) state;
  
  hdrmultirat_send_msg( &qta_cfg_msg.hdr, sizeof( hdrfw_qta_cfg_msg_t ) );

} /* hdrmultirat_send_qta_cfg_msg */

#ifdef FEATURE_HDR_QTA_THRU_TRM
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_QTA_STATUS_QUERY_MSG

DESCRIPTION
  Sends QTA status query msg to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_qta_status_query_msg ()
{
  hdrfw_qta_status_query_msg_t         qta_query_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: send QTA query msg" );

  memset( &qta_query_msg,
          0,
          sizeof( hdrfw_qta_status_query_msg_t ) );
    /* Clear the message */

  msgr_init_hdr( &qta_query_msg.hdr, 
                 MSGR_HDR_MULTIRAT, 
                 HDR_FW_QTA_STATUS_QUERY_CMD );
    /* Init the message header */

  hdrmultirat_send_msg( &qta_query_msg.hdr, sizeof( hdrfw_qta_status_query_msg_t ) );

} /* hdrmultirat_send_qta_status_query_msg */
#endif /* FEATURE_HDR_QTA_THRU_TRM */
#endif /* FEATURE_HDR_QTA */

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

/*===========================================================================

FUNCTION HDRMULTIRAT_ENABLE_RX_FW_LOGGING

DESCRIPTION
  Send enable/disable rx logging msg to FW

DEPENDENCIES
  None

PARAMETERS
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_enable_rx_fw_logging ( void )
{
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW

  hdrfw_diag_log_config_msg_t     dsda_enable_rx_logging;
    /* FW Message to enable/disable Rx DSDA log packet */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ( hdrmultirat.mode_pref != HDRMULTIRAT_SINGLE_STANDBY ) && 
       hdrmdsp_check_dsp_clk() )
  {
    if ( log_status( HDRFW_DIAG_LOG_DSDA_RX_PKT_ID ) )
      /* Check QXDM log mask, if enabled go here */
    {
      if ( !hdrmultirat.dsda_rx_log_pkt_is_logged )
       /* Check if HDR already is not logging the packet? */
      {
        hdrmultirat.dsda_rx_log_pkt_is_logged = TRUE;
        msgr_init_hdr ( &( dsda_enable_rx_logging.hdr ), 
                        MSGR_HDR_MULTIRAT,
                        HDR_FW_CMD_DIAG_LOG_CONFIG );
        dsda_enable_rx_logging.diagLogID = HDRFW_DIAG_LOG_DSDA_RX_PKT_ID;
        dsda_enable_rx_logging.enable = TRUE; 
        hdrmultirat_send_msg( &dsda_enable_rx_logging, 
                              sizeof( dsda_enable_rx_logging ) );
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Sent enable RX DSDA "
                                       "Logpacket to FW");
      }
    } else /* Checked QXDM log mask, if disabled go here */
    {
      if ( hdrmultirat.dsda_rx_log_pkt_is_logged )
        /* Check if HDR already is logging the packet? */
      {
        hdrmultirat.dsda_rx_log_pkt_is_logged = FALSE;
        msgr_init_hdr ( &(dsda_enable_rx_logging.hdr), 
                        MSGR_HDR_MULTIRAT,
                        HDR_FW_CMD_DIAG_LOG_CONFIG );

        dsda_enable_rx_logging.diagLogID = HDRFW_DIAG_LOG_DSDA_RX_PKT_ID;
        dsda_enable_rx_logging.enable = FALSE; 
        hdrmultirat_send_msg( &dsda_enable_rx_logging, 
                             sizeof( dsda_enable_rx_logging ) );
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSA: Sent disable RX DSDA "
                                       "Logpacket to FW");
      }
    }
  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
} /* hdrmultirat_enable_rx_fw_logging */

/*===========================================================================

FUNCTION HDRMULTIRAT_ENABLE_TX_FW_LOGGING

DESCRIPTION
  Send enable/disable TX msg to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_enable_tx_fw_logging ( void )
{
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
  hdrfw_diag_log_config_msg_t     dsda_enable_tx_logging;
    /* FW Message to enable/disable Rx DSDA log packet */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( hdrmultirat.mode_pref != HDRMULTIRAT_SINGLE_STANDBY )
        && hdrmdsp_check_dsp_clk() )
  {
    if ( log_status ( HDRFW_DIAG_LOG_DSDA_TX_PKT_ID ) )
      /* Check QXDM log mask, if enabled go here */
    {
      if ( !hdrmultirat.dsda_tx_log_pkt_is_logged )
       /* Check if HDR already is not logging the packet and state is Tx*/
      {
        hdrmultirat.dsda_tx_log_pkt_is_logged = TRUE;
        msgr_init_hdr ( &(dsda_enable_tx_logging.hdr), 
                        MSGR_HDR_MULTIRAT,
                        HDR_FW_CMD_DIAG_LOG_CONFIG );
        dsda_enable_tx_logging.diagLogID = HDRFW_DIAG_LOG_DSDA_TX_PKT_ID;
        dsda_enable_tx_logging.enable = TRUE; 
        hdrmultirat_send_msg( &dsda_enable_tx_logging,
                              sizeof( dsda_enable_tx_logging ) );
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Sent enable Tx DSDA "
                                       "Logpacket to FW");
      }
    } else /* Checked QXDM log mask, if disabled go here */
    {
      if ( hdrmultirat.dsda_tx_log_pkt_is_logged )
        /* Check if HDR already is logging the packet? */
      {
        hdrmultirat.dsda_tx_log_pkt_is_logged = FALSE;
        msgr_init_hdr ( &( dsda_enable_tx_logging.hdr ), 
                        MSGR_HDR_MULTIRAT,
                        HDR_FW_CMD_DIAG_LOG_CONFIG );
        dsda_enable_tx_logging.diagLogID = HDRFW_DIAG_LOG_DSDA_TX_PKT_ID;
        dsda_enable_tx_logging.enable = FALSE; 
        hdrmultirat_send_msg( &dsda_enable_tx_logging,
                              sizeof( dsda_enable_tx_logging ) );
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA: Sent disable Tx DSDA "
                                       "Logpacket to FW");
      }
    }
  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
} /* hdrmultirat_enable_tx_fw_logging */

#ifdef FEATURE_HDR_DUAL_SIM

#ifdef FEATURE_HDR_DR_DSDS
/*===========================================================================
FUNCTION HDRMULTIRAT_DR_MODE_CHANGE

DESCRIPTION
  This function is called to inform HDRSRCH DR-DSDS <-> DSDS mode changed.

DEPENDENCIES
  None
 
PARAMETERS 
  hdrmultirat_mode_pref_enum_type
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmultirat_dr_mode_change
(
  hdrmultirat_mode_pref_enum_type mode
)
{
  hdrmultirat_ds_pref_enum_type        ds_pref;
    /* What is current DR preference? */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( mode )
  {
     case HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY:
     {
       /* DR is being disabled
          Check here if we should fallback to DSDS or SS mode */

       rex_enter_crit_sect( &hdrmultirat.crit_sect );
         /* enter critical section */

       ds_pref = hdrmultirat.ds_pref;

       rex_leave_crit_sect( &hdrmultirat.crit_sect );
         /* leave critical section */

       /* Debug */
       HDR_MSG_PROT_2( MSG_LEGACY_ERROR, "HDR DR-DSDS: ds_pref %d %d", ds_pref , hdrmultirat.ds_pref);

       if ( ds_pref == HDRMULTIRAT_DUAL_SIM )
       {
         hdrmultirat.mode_pref = HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY;
       }
       else if ( ds_pref == HDRMULTIRAT_SINGLE_SIM )
       {
         hdrmultirat.mode_pref = HDRMULTIRAT_SINGLE_STANDBY;
       }
       else
       {
         HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "HDR DR-DSDS: unknown ds_pref %d", ds_pref );
         return;
       }

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
        hdrmultirat.is_mode_changed = TRUE;
        hdrmultirat_dsda_mode_change();
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */      

      }
      break;

     case HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY:
     {
       
        hdrmultirat.mode_pref = HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY;
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
        hdrmultirat.is_mode_changed = TRUE;
        hdrmultirat_dsda_mode_change();
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */      

      }
      break;

     default:
      /* SYS_MODEM_DS_PREF_NO_CHANGE or invalid values */
      break;
  }

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DR-DSDS: mode changed: %d", hdrmultirat.mode_pref );
} /* hdrmultirat_dr_mode_change( )  */

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DR_DSDS_ENABLED

DESCRIPTION
  This function returns if DR-DSDS is enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DR-DSDS is enabled
  False - if DR-DSDS is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dr_dsds_enabled( void )
{
  return ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY );
} /* hdrmultirat_is_dsds_enabled( ) */

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_RF_TX_CAPABLE

DESCRIPTION
  This function checks with TRM if the chain we have is TX capable.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if we currently own TX capable chain
  False - if we currently own RX only chain

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_rf_tx_capable( void )
{

 /*--------------------------------------------------------------------------*/

  /* If in DR check if we have Dev0 which is tx capable*/
  if ( hdrmultirat.mode_pref == HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DR-DSDS: device : %d", 
                    hdrmultirat.coex_info.rx.rf_device );

   if ( hdrmultirat.coex_info.rx.rf_device == RFM_DEVICE_0 )
    {
      return TRUE;
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "HDR DR-DSDS: device : %d TX not supported!", 
                      hdrmultirat.coex_info.rx.rf_device );

      return FALSE;
    }

  }
  else
    return TRUE;

} /* hdrmultirat_is_rf_tx_capable( ) */
#endif /* FEATURE_HDR_DR_DSDS */

/*===========================================================================
FUNCTION HDRMULTIRAT_STANDBY_PREF_CHGD

DESCRIPTION
  This function is called to inform HDRSRCH of standby preference
  has changed. 

DEPENDENCIES
  This function must be called when HDR is offline.
 
PARAMETERS 
  ds_pref - standby preference
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmultirat_standby_pref_chgd
(
  sys_modem_dual_standby_pref_e_type ds_pref /* New standby preferece */
)
{
  sys_modem_device_mode_e_type device_mode;
    /* Device mode enum from CM */

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DSDx: standby_pref_chgd: %d", ds_pref);

  /* Get device mdoe to distinguish between DSDS/DSDA */
  device_mode = cm_get_device_mode();

  switch ( ds_pref )
  {
     case SYS_MODEM_DS_PREF_SINGLE_STANDBY:
       rex_enter_crit_sect( &hdrmultirat.crit_sect );
         /* enter critical section */
       hdrmultirat.ds_pref = HDRMULTIRAT_SINGLE_SIM;
       rex_leave_crit_sect( &hdrmultirat.crit_sect );
         /* leave critical section */

#ifdef FEATURE_HDR_WAKEUP_MANAGER
      /* De-Register with wakeup manager */
      (void) wmgr_register_rat_cb ( WMGR_CLIENT_HDR,NULL,NULL );
       HDR_MSG_SRCH( MSG_LEGACY_HIGH, "De-Registered with wakeup mgr " );   
#endif /* FEATURE_HDR_WAKEUP_MANAGER */
      /* If switching from  DSDA -> SSSS let FW know */
      if( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE )
      {  
        hdrmultirat.mode_pref = HDRMULTIRAT_SINGLE_STANDBY;

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
        hdrmultirat.is_mode_changed = TRUE;
        hdrmultirat_dsda_mode_change();
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */      

      }
      else
      {
        hdrmultirat.mode_pref = HDRMULTIRAT_SINGLE_STANDBY;
      }
      break;

     case SYS_MODEM_DS_PREF_DUAL_STANDBY:
      
      rex_enter_crit_sect( &hdrmultirat.crit_sect );
         /* enter critical section */
      hdrmultirat.ds_pref = HDRMULTIRAT_DUAL_SIM;
      rex_leave_crit_sect( &hdrmultirat.crit_sect );
         /* leave critical section */
      if ( device_mode ==  SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY )
       {
         hdrmultirat.mode_pref = HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY;
         HDR_MSG_SRCH( MSG_LEGACY_HIGH, "HDR DSDS Mode" );
#ifdef FEATURE_HDR_WAKEUP_MANAGER
         /* Register with wakeup manager */
         (void) wmgr_register_rat_cb ( WMGR_CLIENT_HDR,
                                       hdrsrchsleep_wakeup_manager_cb,
                                       NULL );
         HDR_MSG_SRCH( MSG_LEGACY_HIGH, "Registered with wakeup mgr " );   
#endif /* FEATURE_HDR_WAKEUP_MANAGER */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
        hdrmultirat.is_mode_changed = TRUE;
        hdrmultirat_dsda_mode_change();
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */      

       }
       else if ( device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE ) 
       {
         hdrmultirat.mode_pref = HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE;
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
         hdrmultirat.is_mode_changed = TRUE;
         hdrmultirat_dsda_mode_change();
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */
       }
      break;

     default:
      /* SYS_MODEM_DS_PREF_NO_CHANGE or invalid values */
      break;
  }
} /* hdrmultirat_standby_pref_chgd( )  */

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DSDS_ENABLED

DESCRIPTION
  This function returns if DSDS is enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DSDS is enabled
  False - if DSDS is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dsds_enabled( void )
{
  return ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY );
} /* hdrmultirat_is_dsds_enabled( ) */

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DSDA_ENABLED

DESCRIPTION
  This function returns if DSDA enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DSDS is enabled
  False - if DSDS is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dsda_enabled( void )
{
  return ( hdrmultirat.mode_pref == HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE );
} /* hdrmultirat_is_dsda_enabled( ) */

#endif /* FEATURE_HDR_DUAL_SIM */

/*=========================================================================

FUNCTION     : HDRMULTIRAT_SEND_MSG

DESCRIPTION  : This function sends the message to MSGR.

DEPENDENCIES : None

INPUTS       : msg - The message pointer
               len - The message length 

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrmultirat_send_msg
( 
  void                       *msg,
    /* Message to send */

  uint32                     len
    /* The message lenght */
)
{
  errno_enum_type        e_retval;
    /* error code */

/*-----------------------------------------------------------------------*/

  e_retval = msgr_send( (msgr_hdr_struct_type *) msg, len );

  if ( e_retval != E_SUCCESS )
  {
    ERR_FATAL( "Send msg: 0x%8x retval %d", 
               ( ( msgr_hdr_struct_type *) msg)->id, e_retval, 0 );
  }
}  /* hdrmultirat_send_msg */

