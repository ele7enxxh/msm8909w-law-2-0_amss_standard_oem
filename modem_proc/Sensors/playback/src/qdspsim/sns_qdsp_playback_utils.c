/******************************************************************************
  @file    sns_qdsp_playback_util.c
  @brief   SAM client for QDSP SIM Playback


  ---------------------------------------------------------------------------
  Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 *******************************************************************************/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-26  ps   Updated license header
  2013-11-21  ae   Initial version

============================================================================*/
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "sns_smgr_sensor_config.h"
#include "sns_em.h"
#include "sns_qdsp_playback.h"


uint32_t sns_qdsp_sim_low_tick(void)
{
  char line[256], key[60], fname[60];
  FILE *config_fp = NULL;
  FILE *input_fd[SNS_SMGR_NUM_SENSORS_DEFINED];
  uint32_t first_tick[SNS_SMGR_NUM_SENSORS_DEFINED], sensor_odr[SNS_SMGR_NUM_SENSORS_DEFINED];
  uint32_t min_tick = 0;
  uint32_t qdsp_sim_low_tick = 0;
  int num_input_sensors = 0;
  int min_odr = 1;
  int odr, i; 

  
  config_fp = fopen(PLAYBACK_CONFIG_FILE, "r");
  if (config_fp == NULL)
  {
     return qdsp_sim_low_tick;
  }

  while (fgets(line, sizeof(line), config_fp) != NULL)
  {
     /* skip comments */
     if (strstr(line, "//"))
        continue;

     if (sscanf(line, "%s %s %d", key, fname, &odr) == 3)
     { 
        if ((num_input_sensors < SNS_SMGR_NUM_SENSORS_DEFINED) && 
            (!strcmp(key, "INPUT_ACCEL") || !strcmp(key, "INPUT_MAG") ||
            !strcmp(key, "INPUT_GYRO") || !strcmp(key, "INPUT_PRESSURE") ||
            !strcmp(key, "INPUT_PROXIMITY") || !strcmp(key, "INPUT_AMBIENT")))
         {
            input_fd[num_input_sensors] = NULL;
            input_fd[num_input_sensors] = fopen(fname, "r");
            if(input_fd[num_input_sensors]) 
            {
               fscanf(input_fd[num_input_sensors], "%d,",
                      (int*)&first_tick[num_input_sensors]);
               fclose(input_fd[num_input_sensors]);
               sensor_odr[num_input_sensors] = odr;
               ++num_input_sensors;
            }
        }
     }
  }

  if (num_input_sensors)
  {
    min_tick = first_tick[0];
    min_odr = sensor_odr[0]; 

    for (i=0; i < num_input_sensors; i++ ) 
    {
      if (first_tick[i] <= min_tick)
      {
        min_tick = first_tick[i];
      }

      if (sensor_odr[i] <= min_odr)
      {
        min_odr = sensor_odr[i];
      }
    }

    qdsp_sim_low_tick = min_tick - (DSPS_SLEEP_CLK/min_odr);
  }

  if (config_fp) 
  {
    fclose(config_fp);
  }

  return qdsp_sim_low_tick;
}
