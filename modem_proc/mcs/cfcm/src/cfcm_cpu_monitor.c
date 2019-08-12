/*!
  @file
  cfcm_cpu_monitor.c

  @brief
  CPU monitor implementation.

*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_cpu_monitor.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/13/15   sg      Removed mutex in sync call back
01/22/15   rj      Remove mcs_timer, since it is not being used here
11/20/14   rj      Log Packet support added in CFCM
10/27/14   rj      Adding QTF support for CFCM
10/10/14   rj      Remove FEATURE_CFCM_WAIT_FOR_CLM_SUBMIT from CFCM
10/09/14   rj      CFCM changes to Process monitor's input in its task context
10/06/14   ag      Crash the device if CPU busy % is 99 on JO HW
07/16/14   rj      Enable CLM feature for NON-Jolokia PLs
04/11/14   rj      KW error fix
04/07/14   rj      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#ifdef CFCM_ON_TARGET
#include <qurt.h>
#endif
#include <msgr.h>
#include <fs_public.h>
#include "comdef.h"
#include "cfcm_cfg.h"
#include "cfcm_msg.h"
#include "cfcmi.h"
#include "cfcm_cpu_monitor.h"
#include "cfcm.h"
#include "cfcm_monitor.h"
#include "cfcm_msgr.h"
#ifdef CFCM_ON_TARGET
#include <npa.h>
#endif
#include <DDIChipInfo.h>
#include "cfcm_qsh.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief CPU Monitor timer frequency in ms
*/
#define CFCM_CPU_MONITOR_TIMER_PERIODICITY                                50

/*! @brief history buffer size 
*/
#define CFCM_CPU_MONITOR_HIST_BUF_SZ                                     200

/*! @brief This MACRO will be the Max FREQ for CPU Clk for off-target
*/
#define CFCM_CPU_MONITOR_TOP_FREQ_IN_KHZ                           787200
/*! @brief statistics related to CPU monitor
*/
typedef struct
{
  uint32 last_load_index;  /*!< Last index filled in history buffer */
  uint32 cpu_load[CFCM_CPU_MONITOR_HIST_BUF_SZ];
  uint32  max_supported_freq;  /*!< MAX freq supported */
  uint32  max_freq;            /*!< MAX freq CPU monitor reported */
  uint32  load_at_max_freq;    /*!< load when freq is max freq */
  uint32  max_load;            /*!< MAX load based on curr freq and max freq */
  uint32  freq_at_max_load;    /*!< freq when load is max load */
  uint64 avg_freq;             /*!< Average freq */
  uint64 avg_load;             /*!< Average freq */
  uint32 count;                /*!< Count used for the running average */
} cfcm_cpu_monitor_stats_s;


/*! @brief CPU monitor related data structure
*/
typedef struct
{
  cfcm_cpu_monitor_client_cmd_s   status[CFCM_CLIENT_MAX]; /*!< current state of CPU monitor per client */

  cfcm_cpu_monitor_stats_s   stats;         /*!< statistics */

  rex_crit_sect_type         crit_sect;

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
} cfcm_cpu_monitor_s;

/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/

STATIC cfcm_cpu_monitor_s  cfcm_cpu_monitor;

STATIC cfcm_cpu_monitor_s* const cfcm_cpu_monitor_ptr = &cfcm_cpu_monitor;

/*! Number of successive CPU DNE hits */
static uint32 cfcm_num_successive_dne_attempts = 0;

static uint32 cfcm_cpu_max_successive_dne;
static uint32 cfcm_cpu_load_dne_limit;
static uint32 cfcm_clm_cpu_load_overflow_count = 0;
static uint32 cfcm_clm_cpu_load_overflow_value;

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/
static void cfcm_register_with_CLM(
                                    void        *pContext,
                                    unsigned int nEventType,
                                    void        *pNodeName,
                                    unsigned int nNodeNameSize
                                  )
{
  CLM_HandleType cfcm_clm_handle;

#ifdef CFCM_ON_TARGET
#ifdef FEATURE_THOR_MODEM
  cfcm_clm_handle = CLM_RegisterPeriodicClient( "CFCM", CLM_CLIENT_BASIC_CPUUTIL, (CFCM_CPU_MONITOR_TIMER_PERIODICITY*1000), 
               (CLM_CallbackFuncPtr)cfcm_cb_from_clm, NULL);
#else
  cfcm_clm_handle = CLM_RegisterPeriodicClient( "CFCM", CLM_CLIENT_BASIC_CPUUTIL, (CFCM_CPU_MONITOR_TIMER_PERIODICITY*1000), 
               CLM_ATTRIBUTE_DEFAULT, (CLM_CallbackFuncPtr)cfcm_cb_from_clm, NULL);
#endif /* FEATURE_THOR_MODEM */

  /* cfcm_clm_handle must be valid at this point */
  if (!cfcm_clm_handle)
  {
    ERR_FATAL("Not a valid CFCM CLM handle", 0, 0, 0);
  }
#endif /* CFCM_ON_TARGET */

}


/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_init

==============================================================================*/
/*!
    @brief
    initiation at task start up.

    @return
    None
*/
/*============================================================================*/
void cfcm_cpu_monitor_init( void )
{
  uint32 i;
  
  #ifdef FEATURE_JOLOKIA_MODEM
  DalChipInfoVersionType chip_version;
  DalChipInfoFamilyType  chip_family;
  #endif
  
  #ifdef CFCM_ON_TARGET
  #ifndef T_WINNT
  npa_query_type queryResult;
  npa_query_handle          npa_handle;    /*!< NPA query handle */
  #endif /* T_WINNT */
  #endif /* CFCM_ON_TARGET */

  cfcm_config_cpu_dne_type cpu_dne_efs_content;
  
  /*--------------------------------------------------------------------------*/


  /*--------------------------------------------------------------------------*/

#ifdef CFCM_ON_TARGET
  npa_resource_available_cb( "/clm/enabled", (npa_callback)cfcm_register_with_CLM, NULL );
#endif /* CFCM_ON_TARGET */

  /* Enter atomic section. Disable interrupts. */
  CFCM_ENTER_ATOMIC_SECT();
  /* Initialize the CPU monitor's critical section */
  CFCM_INIT_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
  /* Leave atomic section. Enable interrupts. */
  CFCM_LEAVE_ATOMIC_SECT();

  for(i = 0; i < CFCM_CLIENT_MAX; i++)
  {
    cfcm_cpu_monitor_ptr->status[i].state = CFCM_CPU_MONITOR_STATE_NORMAL;
    cfcm_cpu_monitor_ptr->status[i].cmd = CFCM_CMD_FC_OFF;
  }

  cfcm_cpu_monitor_ptr->stats.last_load_index = 0;
  cfcm_cpu_monitor_ptr->stats.max_load = 0;
  cfcm_cpu_monitor_ptr->stats.max_freq = 0;
  cfcm_cpu_monitor_ptr->stats.load_at_max_freq = 0;
  cfcm_cpu_monitor_ptr->stats.freq_at_max_load = 0;
  cfcm_cpu_monitor_ptr->stats.count = 0;
  cfcm_cpu_monitor_ptr->stats.avg_freq = 0;
  cfcm_cpu_monitor_ptr->stats.avg_load = 0;
#ifdef CFCM_ON_TARGET
#ifndef T_WINNT
  /* Get the MAX supported frequency from NPA Node */
  npa_handle = npa_create_query_handle("/clk/cpu");
  npa_query(npa_handle,
            NPA_QUERY_RESOURCE_MAX,
            &queryResult);
  cfcm_cpu_monitor_ptr->stats.max_supported_freq = queryResult.data.state;
#endif /* T_WINNT */
#else /* CFCM_ON_TARGET */
  cfcm_cpu_monitor_ptr->stats.max_supported_freq = CFCM_CPU_MONITOR_TOP_FREQ_IN_KHZ;
#endif /* CFCM_ON_TARGET */
  memset(&cfcm_cpu_monitor_ptr->stats.cpu_load, 0, CFCM_CPU_MONITOR_HIST_BUF_SZ*sizeof(uint32));

#ifndef FEATURE_MODEM_CFCM_DIAG_TEST
  cfcm_test_cpu_monitor_via_diag(0);
#else
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */

#ifdef CFCM_ON_TARGET
  #ifdef FEATURE_JOLOKIA_MODEM
  /* Check for JO V1 HW and set the DNE limits */
  chip_family = DalChipInfo_ChipFamily();
  chip_version = DalChipInfo_ChipVersion();
 
  if ( (chip_family == DALCHIPINFO_FAMILY_MSM8909 ) && 
       ((uint32)chip_version == 1) ) 
  {
    cfcm_cpu_load_dne_limit = 99;
    cfcm_cpu_max_successive_dne = 1;
  }
  else
  #endif  /* ! JO Modem */
#endif  /* ! CFCM_ON_TARGET */
  {
    cfcm_cpu_load_dne_limit = 32765;
    cfcm_cpu_max_successive_dne = 32765;
  }  
  
  /* Override with EFS if valid...  */
  cpu_dne_efs_content = cfcm_read_cpu_dne_efs();
  if ( (cpu_dne_efs_content.cpu_max_successive_dne_hits > 0) &&
       (cpu_dne_efs_content.cpu_dne_limit > 0) )
  {
    cfcm_cpu_load_dne_limit = cpu_dne_efs_content.cpu_dne_limit;
    cfcm_cpu_max_successive_dne = cpu_dne_efs_content.cpu_max_successive_dne_hits;
  }
   
} /* cfcm_cpu_monitor_init() */


/*==============================================================================

  FUNCTION:  cfcm_cb_from_clm

==============================================================================*/
/*!
    @brief
    Callback function for CLM to check Threshold for various clients.

    @return
    None
*/
/*============================================================================*/
void cfcm_cb_from_clm(CLM_LoadInfoBasicStructType * clmInfo, void * clientData)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  /*--------------------------------------------------------------------------*/

  /* Check Inputs params */
  if(clmInfo == NULL)  
  {
    CFCM_MSG_0( ERROR, "cfcm_cb_from_clm: NULL input parameter ");
    return;
  }

  /*--------------------------------------------------------------------------*/

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
    if( clmInfo->updateReason == CLM_UPDATE_REASON_PERIODIC )
    {
      if (clmInfo->utilPctAtMaxClk >= cfcm_cpu_load_dne_limit)
      {
        cfcm_num_successive_dne_attempts++;
        if (cfcm_num_successive_dne_attempts >= cfcm_cpu_max_successive_dne)
        {
          CFCM_ERR_FATAL("Reached CPU busy %d percentage %d successive times... current load %d percent", 
          cfcm_cpu_load_dne_limit, cfcm_num_successive_dne_attempts, clmInfo->utilPctAtMaxClk );
        }
        else
        {
          CFCM_MSG_4(FATAL, "Reached CPU busy %d percentage %d (out of %d) successive times... current load %d percent", 
                     cfcm_cpu_load_dne_limit, cfcm_num_successive_dne_attempts,
                     cfcm_cpu_max_successive_dne, clmInfo->utilPctAtMaxClk);
        }
      }
      else
      {
#ifndef LOG_MCS_CFCM_COMMAND_INFO_C
        CFCM_MSG_3(HIGH, "CFCM using CLM: CPU percentage utilization is %d, dne_attempts %d, Current Q6 Clock : %d KHz ", 
                   clmInfo->utilPctAtMaxClk,
                   cfcm_num_successive_dne_attempts, clmInfo->currentClkKhz);
#endif /* LOG_MCS_CFCM_COMMAND_INFO_C */
        cfcm_num_successive_dne_attempts = 0;
      }
  
      msg.data_info = CFCM_MONITOR_CPU_INPUT;
      msg.monitor_data.cpu_info.cpu_load = clmInfo->utilPctAtMaxClk;
      msg.monitor_data.cpu_info.curr_cpu_freq = clmInfo->currentClkKhz;
      msg.monitor_data.cpu_info.max_cpu_freq = cfcm_cpu_monitor_ptr->stats.max_supported_freq;
    
      // Store CPU load value 
      cfcm_cpu_monitor_ptr->stats.cpu_load[cfcm_cpu_monitor_ptr->stats.last_load_index] = clmInfo->utilPctAtMaxClk;
      cfcm_cpu_monitor_ptr->stats.last_load_index++;
      cfcm_cpu_monitor_ptr->stats.last_load_index %= CFCM_CPU_MONITOR_HIST_BUF_SZ;

     /* Ignore the reading if CPU load is invalid */
      if(clmInfo->utilPctAtMaxClk > 100)
      {
        cfcm_clm_cpu_load_overflow_count += 1;
        cfcm_clm_cpu_load_overflow_value = clmInfo->utilPctAtMaxClk;
        
        QSH_LOG(
            CFCM_CLT_QSH,
            QSH_CAT_TPUT,
            QSH_MSG_TYPE_HIGH,
            "Invalid CLM entry - CPU Load: %d; Count: %d",
            clmInfo->utilPctAtMaxClk, 
            cfcm_clm_cpu_load_overflow_count);

        return;
      }
      send_status = cfcm_msgr_send_msg(&msg.hdr,
                                 MCS_CFCM_MONITOR_IND,
                                 sizeof(cfcm_monitor_ind_msg_s));
      CFCM_ASSERT(send_status == E_SUCCESS);
    }
#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
} /* cfcm_cb_from_clm() */

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_process_cpu_info

==============================================================================*/
/*!
    @brief
    Function to process CPU info and store it for QSH debugging.

    @return
    none
*/
/*============================================================================*/
void cfcm_cpu_monitor_process_cpu_info
(
  cfcm_cpu_load_type_s cpu_info
)
{ 
  cfcm_cpu_monitor_stats_s* cpu_stats_ptr = &cfcm_cpu_monitor_ptr->stats;

  CFCM_ENTER_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
  
  /* Copy the highest Freq and highest Load
     Also note down the highest load during highest frequency and
     highest frequency during highest load 
  */
  if(cpu_info.cpu_load > cpu_stats_ptr->max_load)
  {
    cpu_stats_ptr->max_load = cpu_info.cpu_load;
    cpu_stats_ptr->freq_at_max_load = cpu_info.curr_cpu_freq;
  }
  else if(cpu_info.cpu_load == cpu_stats_ptr->max_load)
  {
    if(cpu_info.curr_cpu_freq > cpu_stats_ptr->freq_at_max_load)
    {
      cpu_stats_ptr->freq_at_max_load = cpu_info.curr_cpu_freq;
    }
  }

  if(cpu_info.curr_cpu_freq > cpu_stats_ptr->max_freq)
  {
    cpu_stats_ptr->max_freq = cpu_info.curr_cpu_freq;
    cpu_stats_ptr->load_at_max_freq = cpu_info.cpu_load;
  }
  else if(cpu_info.curr_cpu_freq == cpu_stats_ptr->max_freq)
  {
    if(cpu_info.cpu_load > cpu_stats_ptr->load_at_max_freq)
    {
      cpu_stats_ptr->load_at_max_freq = cpu_info.cpu_load;
    }
  }

  /* Calculate average CPU and load*/
  if(cpu_info.cpu_load > 0)
  {
    cpu_stats_ptr->avg_freq += (uint64)(cpu_info.curr_cpu_freq/1000);
    cpu_stats_ptr->avg_load += cpu_info.cpu_load;
    cpu_stats_ptr->count += 1;
  }
  

  CFCM_LEAVE_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
}

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_process_load_percentage

==============================================================================*/
/*!
    @brief
    Callback function for CLM to check Threshold for various clients.

    @return
    TRUE/FALSE
*/
/*============================================================================*/
boolean cfcm_cpu_monitor_process_load_percentage
(
  cfcm_client_e client_id,
  uint32        cpu_pct,
  cfcm_cmd_e*   cmd
)
{
  boolean state_updated = FALSE;
  cfcm_cpu_monitor_client_cmd_s   status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  status.state = CFCM_CPU_MONITOR_STATE_NORMAL;
  if (cfcm_cfg_cpu_monitor_client_cmd_status(client_id, cpu_pct, &status))
  {
    /* if the state is updated/changed then we should run FC for all clients */
    if (status.state != cfcm_cpu_monitor_ptr->status[client_id].state)
    {
      state_updated = TRUE;
      cfcm_cpu_monitor_ptr->status[client_id].cmd = status.cmd;
      *cmd = status.cmd;
      cfcm_cpu_monitor_ptr->status[client_id].state = status.state;
    }
  }

  return state_updated;
} /* cfcm_cpu_monitor_process_load_percentage() */

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
EXTERN void cfcm_cpu_monitor_qsh_analysis
(
  void
)
{
  /* Set average frequency to a dummy value */
  uint32 avg_freq = 115;
  uint32 avg_load = 0;
  
  /* Calculate average frequency and load */
  if(cfcm_cpu_monitor_ptr->stats.count > 0)
  {
    avg_freq = 
     (uint32)cfcm_cpu_monitor_ptr->stats.avg_freq/cfcm_cpu_monitor_ptr->stats.count;
    avg_load = 
    (uint32)cfcm_cpu_monitor_ptr->stats.avg_load/cfcm_cpu_monitor_ptr->stats.count;
  }
  
  /* Log a message if CPU freq is known */
  QSH_CHECK_LOG(
      cfcm_cpu_monitor_ptr->stats.max_freq != 0,
      CFCM_CLT_QSH,
      QSH_CAT_TPUT,
      QSH_MSG_TYPE_MEDIUM,
      "CPUFreq(MHz) - MaxFreq: %d; AvgFreq/AvgLoad: %d/%d; " 
      "MaxActualFreq/Load: %d/%d; MaxLoad/Freq: %d/%d",
      (uint32)cfcm_cpu_monitor_ptr->stats.max_supported_freq/1000,
      avg_freq,
      avg_load,
      (uint32)cfcm_cpu_monitor_ptr->stats.max_freq/1000,
      cfcm_cpu_monitor_ptr->stats.load_at_max_freq,
      cfcm_cpu_monitor_ptr->stats.max_load,
      (uint32)cfcm_cpu_monitor_ptr->stats.freq_at_max_load/1000);

}

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_analysis_async

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
EXTERN void cfcm_cpu_monitor_qsh_analysis_async
(
  void
)
{
  /* Set average frequency to a dummy value */
  uint32 avg_freq = 115;
  uint32 avg_load = 0;
  
  CFCM_ENTER_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);

  /* Calculate average frequency and load */
  if(cfcm_cpu_monitor_ptr->stats.count > 0)
  {
    avg_freq = 
     (uint32)cfcm_cpu_monitor_ptr->stats.avg_freq/cfcm_cpu_monitor_ptr->stats.count;
    avg_load = 
    (uint32)cfcm_cpu_monitor_ptr->stats.avg_load/cfcm_cpu_monitor_ptr->stats.count;
  }
  /* Log a message if CPU freq is known */
  QSH_CHECK_LOG(
      cfcm_cpu_monitor_ptr->stats.max_freq != 0,
      CFCM_CLT_QSH,
      QSH_CAT_TPUT,
      QSH_MSG_TYPE_MEDIUM,
      "CPUFreq(MHz) - MaxFreq: %d; AvgFreq/AvgLoad: %d/%d; " 
      "MaxActualFreq/Load: %d/%d; MaxLoad/Freq: %d/%d",
      (uint32)cfcm_cpu_monitor_ptr->stats.max_supported_freq/1000,
      avg_freq,
      avg_load,
      (uint32)cfcm_cpu_monitor_ptr->stats.max_freq/1000,
      cfcm_cpu_monitor_ptr->stats.load_at_max_freq,
      cfcm_cpu_monitor_ptr->stats.max_load,
      (uint32)cfcm_cpu_monitor_ptr->stats.freq_at_max_load/1000);

  CFCM_LEAVE_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
}


/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
EXTERN void cfcm_cpu_monitor_qsh_reset
(
  void
)
{
  CFCM_ENTER_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
  
  cfcm_cpu_monitor_ptr->stats.max_freq = 0;
  cfcm_cpu_monitor_ptr->stats.max_load = 0;
  cfcm_cpu_monitor_ptr->stats.load_at_max_freq = 0;
  cfcm_cpu_monitor_ptr->stats.freq_at_max_load = 0;
  cfcm_cpu_monitor_ptr->stats.count = 0;
  cfcm_cpu_monitor_ptr->stats.avg_freq = 0;
  cfcm_cpu_monitor_ptr->stats.avg_load = 0;

  CFCM_LEAVE_CRIT_SECT(cfcm_cpu_monitor_ptr->crit_sect);
}

/*==============================================================================

                                UNIT TEST

==============================================================================*/

void cfcm_test_cpu_monitor_via_diag
(
  uint8 input 
)
{
#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#else
  CFCM_UNUSED(input);
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
  return;
} /* cfcm_test_thermal_monitor_via_diag() */


