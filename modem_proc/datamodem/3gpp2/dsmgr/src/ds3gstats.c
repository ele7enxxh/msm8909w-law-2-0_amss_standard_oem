


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gstats.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/02/15   ms      Introduced this file for modem power statistics calculation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "ps_sys.h"
#include "ps_sys_conf.h"
#include "ps_sys_event.h"
#include "ds3gstats.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define POWER_STATS_CACHE_SIZE 10

uint64 ds3g_power_stats_last_reporting_time;

typedef struct{
  uint64 sleep_ticks;
  uint32 sleep_ms;
  uint64 tx_rx_time_ticks;
  uint32 tx_rx_time_ms;
  uint64 time_stamp;

  uint32 gsm_bins[DS3G_TX_POWER_BIN_MAX];
  uint32 wcdma_bins[DS3G_TX_POWER_BIN_MAX];
  uint32 lte_bins[DS3G_TX_POWER_BIN_MAX];
  uint32 tdscdma_bins[DS3G_TX_POWER_BIN_MAX];
  uint32 cmda_1x_bins[DS3G_TX_POWER_BIN_MAX];
  uint32 combined_bins[DS3G_TX_POWER_BIN_MAX];
}ds3g_power_stats_type;

ds3g_power_stats_type ds3g_power_stats[POWER_STATS_CACHE_SIZE];
uint8 ds3g_power_stats_cache_count;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_STATS_INIT_COMPLETE_CMD

DESCRIPTION   Called once at mobile power-up.  Initializes all statsistic 
              variables and call back functions 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_stats_init_complete_cmd()
{
  int16                   dss_errno                 = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(ds3g_power_stats, 0, 
         sizeof(ds3g_power_stats_type)*POWER_STATS_CACHE_SIZE);
  ds3g_power_stats_cache_count = 0;
  /*------------------------------------------------------------------------- 
     Register for interested System Events soon after Init is complete
  -------------------------------------------------------------------------*/
  if (0 > ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                              PS_SYS_EVENT_MODEM_POWER_STATS_CONFIGURATION,
                              PS_SYS_PRIMARY_SUBS,
                              ds3gi_config_modem_power_stats,
                              NULL,
                              &dss_errno))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Couldn't reg for MODEM POWER STATS CONFIG"
                  "event Err : (%d)", dss_errno);
  }
  dss_errno = 0;

  if (0 > ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                              PS_SYS_EVENT_QUERY_MODEM_POWER_STATS,
                              PS_SYS_PRIMARY_SUBS,
                              ds3gi_query_modem_power_stats,
                              NULL,
                              &dss_errno))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Couldn't reg for MODEM POWER STATS QUERY"
                  "event Err : (%d)", dss_errno);
  }

}/*ds3g_stats_init_complete_cmd*/


/*===========================================================================
FUNCTION      DS3GI_CONFIG_MODEM_POWER_STATS

DESCRIPTION   This function is called from PS framework when client wants to 
              configure modem power statistics 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_config_modem_power_stats
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
)
{
  boolean                      enable_modem_stats      = FALSE;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id            = DS3GSUBSMGR_SUBS_ID_INVALID;
  ps_sys_modem_power_stats_config_type *modem_power_stats_config = NULL; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/
  if (event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL Argument");
    return;
  }

  if (event_name != PS_SYS_EVENT_MODEM_POWER_STATS_CONFIGURATION)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Ignoring Event %d", event_name);
  }

  /*------------------------------------------------------------------------- 
    Get DS 3G SUB ID for default subscription
    -------------------------------------------------------------------------*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(ds3gsubsmgr_get_def_data_subs_id());
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Update modem power statistics configureation
    -------------------------------------------------------------------------*/
  modem_power_stats_config = 
      (ps_sys_modem_power_stats_config_type *)event_info_ptr;
  if(modem_power_stats_config == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL config ptr received");
    return;
  }
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "enable_modem_stats: %d",enable_modem_stats);
  enable_modem_stats =  modem_power_stats_config->enable_power_statistics;
  ds3g_config_modem_power_stats(ds3g_subs_id, enable_modem_stats);
  
  /*-------------------------------------------------------------------------
    Reset local cache
    -------------------------------------------------------------------------*/
  memset(ds3g_power_stats, 0, sizeof(ds3g_power_stats_type)*POWER_STATS_CACHE_SIZE);
  ds3g_power_stats_cache_count = 0;
  
  return;
} /*ds3gi_config_modem_power_stats*/


/*===========================================================================
FUNCTION      DS3GI_QUERY_MODEM_POWER_STATS

DESCRIPTION   This function is called from PS framework when client asks for 
              power statistics information 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_query_modem_power_stats
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
)
{
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id            = DS3GSUBSMGR_SUBS_ID_INVALID;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/

  if (event_name != PS_SYS_EVENT_QUERY_MODEM_POWER_STATS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Ignoring Event %d", event_name);
  }

  /*------------------------------------------------------------------------- 
    Get DS 3G SUB ID for default subscription
    -------------------------------------------------------------------------*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(ds3gsubsmgr_get_def_data_subs_id());
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Get modem power statistics 
    -------------------------------------------------------------------------*/
  ds3g_query_modem_power_stats(ds3g_subs_id);
  
  return;
}


/*===========================================================================
FUNCTION      DS3G_CONFIG_MODEM_POWER_STATS

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_config_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  boolean                     enable_modem_power_stats
)
{
  sys_sys_mode_e_type       current_mode      = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr      = NULL;
  time_type                 time_now;
  sys_modem_as_id_e_type    subs_id;
  uint64 current_time = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);

  /*------------------------------------------------------------------------- 
    Call corebsp sleep module api to enable/disable power statistics
    -------------------------------------------------------------------------*/
  sleepStats_trackPowerCollapseTime((uint8)enable_modem_power_stats);

  /*------------------------------------------------------------------------- 
    Call MCPM MPOWER module api to enable/disable power statistics
    -------------------------------------------------------------------------*/
  if (enable_modem_power_stats ==  TRUE) 
  {
      MCPM_Stats_Start_Collection();
      time_get_ms( time_now );
      current_time = QW_CVT_Q2N(time_now); 
      ds3g_power_stats_last_reporting_time = current_time;
  }
  else
  {
    MCPM_Stats_End_Collection();
    ds3g_power_stats_last_reporting_time = 0;
  }

  /*------------------------------------------------------------------------- 
    Check if there is a handler registered as power statstics provider and 
    invoke the call back handler
    -------------------------------------------------------------------------*/
  for(current_mode = SYS_SYS_MODE_NO_SRV; current_mode < SYS_SYS_MODE_MAX;
       current_mode++) 
  {
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

    if ((hdlr_tbl_ptr != NULL) &&
        (hdlr_tbl_ptr->config_modem_power_stats != NULL))
    {
      hdlr_tbl_ptr->config_modem_power_stats(subs_id, enable_modem_power_stats);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW, "No hdlr regd for mode %d subs id:%d",
                      current_mode,ds3g_subs_id);
    }
  }
  
  return;
} /* ds3g_config_modem_power_stats */

/*===========================================================================
FUNCTION      DS3G_QUERY_MODEM_POWER_STATS

DESCRIPTION   This function check if LTE is registered as modem power stats 
              provider, if LTE is available request LTE to send async report
              otherwise start producing report right away.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_query_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
#ifdef FEATURE_DATA_LTE
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr      = NULL;
#endif /* FEATURE_DATA_LTE */
  sys_modem_as_id_e_type    subs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);

  /*------------------------------------------------------------------------- 
    All modules provide synchronous API for querying modem power statistics
    except for LTE. So if LTE handler is available wait for LTE to send async
    response before sending report to the client. Otherwise trigger reporting
    right away
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[SYS_SYS_MODE_LTE][DS_CALL_TYPE_PKT];
  if ((hdlr_tbl_ptr != NULL) &&
        (hdlr_tbl_ptr->request_modem_power_stats != NULL))
  {
    hdlr_tbl_ptr->request_modem_power_stats(subs_id);
  }
  else
  {
    ds3g_report_modem_power_stats(ds3g_subs_id);
  }
#else
  ds3g_report_modem_power_stats(ds3g_subs_id);
#endif  
  return;
} /* ds3g_query_modem_power_stats */


/*===========================================================================
FUNCTION      DS3G_REPORT_MODEM_POWER_STATS

DESCRIPTION   This function gets power statistics for all lower layer, caches 
              last POWER_STATS_CACHE_SIZE values and process the information
              and send to ps framework

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_report_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  sys_sys_mode_e_type                            current_mode      = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type                       *hdlr_tbl_ptr      = NULL;
  ps_sys_modem_power_stats_info_type             modem_power_stats_info;
  uint32                                         elapsed_time = 0;
  uint32                                         sleep_time = 0;
  uint32                                         total_tx_rx_time = 0;
  uint32                                         total_tx_time = 0;
  const sleepStats_mode_stats                   *sleepStat = NULL;
  DalDeviceHandle                               *phTimetickHandle = NULL;  
  DalTimetickTime64Type                          sleep_time_from_tick = 0;
  uint64                                         mcpm_ticks = 0;
  DalTimetickTime64Type                          mcpm_time = 0;
  uint32                                         tx_bin[DS3G_TX_POWER_BIN_MAX];
  ds3g_tx_power_bin_e_type                       index = DS3G_TX_POWER_BIN_1;
  ps_sys_modem_power_level_enum_type             ps_bin_index = PS_SYS_MODEM_POWER_LEVEL_ONE;
  time_type                                      time_now;
  int16                                          dss_errno                 = 0;
  sys_modem_as_id_e_type                         subs_id;
  uint64                                         current_time = 0;
  int32                                          temp_rx_time=0;
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&modem_power_stats_info, 0, sizeof(ps_sys_modem_power_stats_info_type));

  /*------------------------------------------------------------------------- 
    Prepare ticks to millisecond convertion mechanism
    -------------------------------------------------------------------------*/
  DalTimetick_Attach("SystemTimer", &phTimetickHandle);
  if (NULL == phTimetickHandle)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid phTimetickHandle, return");
    return;
  }
  subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);

  /*------------------------------------------------------------------------- 
    Get Sleep statistics
    -------------------------------------------------------------------------*/
  sleepStat = sleepStats_getPowerCollapseTime();
  if (sleepStat == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "sleepStats_getPowerCollapseTime returned NULL");
    return;
  }
  else
  {
    if(DAL_SUCCESS == DalTimetick_CvtFromTimetick64(phTimetickHandle, 
                                                    sleepStat->in_mode_time,
                                                    T_MSEC,
                                                    &sleep_time_from_tick))
    {
      sleep_time = (uint32)sleep_time_from_tick;
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Could not convert ticks to milliseconds");
      return;
    }
  }
  
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Sleep time %d ms", sleep_time);

  /*------------------------------------------------------------------------- 
    Get MCPM Rx+Tx Total Time
    -------------------------------------------------------------------------*/
  mcpm_ticks = MCPM_Stats_Get_Overall_Active_Time();

  if(DAL_SUCCESS == DalTimetick_CvtFromTimetick64(phTimetickHandle, 
                                                    (DalTimetickTime64Type)mcpm_ticks,
                                                    T_MSEC,
                                                    &mcpm_time))
  {
    total_tx_rx_time = (uint32)mcpm_time;
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Could not convert ticks to milliseconds");
    return;
  }

  ds3g_power_stats[ds3g_power_stats_cache_count].sleep_ticks = sleepStat->in_mode_time;
  ds3g_power_stats[ds3g_power_stats_cache_count].sleep_ms = sleep_time;
  ds3g_power_stats[ds3g_power_stats_cache_count].tx_rx_time_ticks = mcpm_ticks;
  ds3g_power_stats[ds3g_power_stats_cache_count].tx_rx_time_ms = total_tx_rx_time;

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"tx_rx time %d ms", total_tx_rx_time);

  /*------------------------------------------------------------------------- 
    Check all lower RAT for power statistic provider and call thier get method
    to get the values store and sum up for final report
    -------------------------------------------------------------------------*/
  for(current_mode = SYS_SYS_MODE_NO_SRV; current_mode < SYS_SYS_MODE_MAX;
       current_mode++) 
  {
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

    if ((hdlr_tbl_ptr != NULL) &&
        (hdlr_tbl_ptr->get_modem_power_stats != NULL))
    {
      memset(&tx_bin, 0, DS3G_TX_POWER_BIN_MAX);
      hdlr_tbl_ptr->get_modem_power_stats(subs_id, tx_bin);
      for (index = DS3G_TX_POWER_BIN_1; index <DS3G_TX_POWER_BIN_MAX ; index++ )
      {
        DATA_3GMGR_MSG3(MSG_LEGACY_LOW, "TX bin for mode:%d, bin[%d] = %d",
                        current_mode, index, tx_bin[index]);
        total_tx_time += tx_bin[index];
        modem_power_stats_info.tx_mode_duration[index].tx_mode_duration += tx_bin[index];

      }

        switch (current_mode) 
        {
          case SYS_SYS_MODE_GSM:
            memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].gsm_bins,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                    tx_bin,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);
            break;
          case SYS_SYS_MODE_WCDMA:
            memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].wcdma_bins,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                    tx_bin,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);
            break;
          case SYS_SYS_MODE_TDS:
            memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].tdscdma_bins,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                    tx_bin,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);
            break;

          /*------------------------------------------------------------------------- 
            1x and HDR both values are stored in CMDA mode (cumulative)
           -------------------------------------------------------------------------*/
          case SYS_SYS_MODE_CDMA:
            memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].cmda_1x_bins,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                    tx_bin,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);
            break;

          case SYS_SYS_MODE_LTE:
            memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].lte_bins,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                    tx_bin,
                    sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);
            break;
          default:
              break;
        }
      }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"No hdlr regd for mode %d subs id:%d",
                      current_mode, ds3g_subs_id);
    }
  }
  time_get_ms( time_now );
  current_time = QW_CVT_Q2N(time_now); 
  ds3g_power_stats[ds3g_power_stats_cache_count].time_stamp = current_time;

  memscpy(ds3g_power_stats[ds3g_power_stats_cache_count].combined_bins,
                  sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
                  modem_power_stats_info.tx_mode_duration,
                  sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "total tx time %d ms", total_tx_time);

  for (ps_bin_index = PS_SYS_MODEM_POWER_LEVEL_ONE; ps_bin_index < 
        PS_SYS_MODEM_POWER_LEVEL_MAX; ps_bin_index++) 
  {
    modem_power_stats_info.tx_mode_duration[ps_bin_index].tx_mode_level
      = ps_bin_index;
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Final TX bin[%d] = %d",
                    ps_bin_index ,
                    modem_power_stats_info.tx_mode_duration[ps_bin_index].tx_mode_duration);
  }

  modem_power_stats_info.sleep_mode_duration = sleep_time;

  temp_rx_time = total_tx_rx_time - total_tx_time ;

  if (( temp_rx_time < 0) && 
      (temp_rx_time >= -20 ))
  {
     modem_power_stats_info.rx_mode_duration = 0;
  }
  else
  {
  modem_power_stats_info.rx_mode_duration = total_tx_rx_time - total_tx_time;
  }

  
  elapsed_time = current_time - ds3g_power_stats_last_reporting_time;
  ds3g_power_stats_last_reporting_time = current_time;

  modem_power_stats_info.idle_mode_duration = 
                 elapsed_time - (total_tx_rx_time + sleep_time);

  DATA_3GMGR_MSG5(MSG_LEGACY_LOW,
                  "total rx time %d ms "
                  "ds3g_power_stats_last_reporting_time %d ms "
                  "current_time %d ms "
                  "elapsed_time %d ms "
                  "idle_mode_duration %d ms",
                  modem_power_stats_info.rx_mode_duration,
                  ds3g_power_stats_last_reporting_time,
                  current_time,
                  elapsed_time,
                  modem_power_stats_info.idle_mode_duration);

  /*------------------------------------------------------------------------- 
    Send the report to clients
    -------------------------------------------------------------------------*/
  ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                      PS_SYS_CONF_MODEM_POWER_STATISTICS,
                      PS_SYS_PRIMARY_SUBS,
                      &modem_power_stats_info, &dss_errno);

  /*------------------------------------------------------------------------- 
    Circular local cache for POWER_STATS_CACHE_SIZE last query
    -------------------------------------------------------------------------*/

  ds3g_power_stats_cache_count++;
  if (ds3g_power_stats_cache_count >= POWER_STATS_CACHE_SIZE) 
  {
      ds3g_power_stats_cache_count = 0;
  }
  return;
} /* ds3g_report_modem_power_stats*/

