/*============================================================================
  @file therm_config.c

  Thermal configuration file parsing, monitoring, and mitigation functionality.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/common/therm_config.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "npa.h"
#include "npa_resource.h"
#include "rex.h"
#include "log.h"
#include "timer.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "stdarg.h"
#include "DALStdDef.h"
#include "CoreVerify.h"
#include <stringl/stringl.h>
#include "stdlib.h"
#include "therm_monitor.h"
#include "therm_log.h"


/*=======================================================================

                  STATIC MEMBER / FUNCTION DECLARATIONS / DEFINITIONS

========================================================================*/

static const char *therm_node_fmt_str = "/therm/sensor/%s";

static const char *therm_node_sampling_fmt_str = "/therm/sensor/%s/sampling";

static const char *therm_node_mitigate_fmt_str = "/therm/mitigate/%s";


static const char *therm_monitor_ini_file = "/nv/item_files/therm_monitor/config.ini";

/* Field type enums and field_names[] array are coupled. */

typedef enum
{
  THERM_MONITOR_ZONE,
  THERM_MONITOR_SAMPLING,
  THERM_MONITOR_THRESHOLDS,
  THERM_MONITOR_THRESHOLDS_CLR,
  THERM_MONITOR_ACTIONS,
  THERM_MONITOR_ACTION_INFO,
  THERM_MONITOR_FIELD_IDX_MAX
} therm_monitor_field_type;

typedef union
{
  int   nVal;
  char *pStr;
} therm_monitor_value_type;

static char *field_names[] =
{
  "thermal_zone",
  "sampling",
  "thresholds",
  "thresholds_clr",
  "actions",
  "action_info",
};

/* Actions to perform */
/* Note: Names must align with therm_monitor_action_type enums */
static char *action_names[] =
{
  "none",
  "mitigate",
};

extern const char *therm_monitor_res_avail[];

extern const unsigned int therm_monitor_res_count;


/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief skip_space

  Thermal monitor function skip space.

  @param  : ppos: Buffer string to search in, and pointer to
          advance to with next non-blank character.

  @return : None.

*/
static void skip_space(char **ppos)
{
  char *pos = *ppos;

  while(*pos != '\0')
  {
    if ((*pos == ' ') ||
        (*pos == '\t') ||
        (*pos == '\n'))
    {
      pos++;
    }
    else
        break;
  }
  *ppos = pos;
}

/**
  @brief skip_line

  Thermal monitor function skip blank line.

  @param  : ppos: Buffer string to search in, and pointer to
          advance to with next non-blank character.

  @return : None.

*/
static void skip_line(char **ppos)
{
  char *pos = *ppos;
  char *ptmp;

  ptmp = strchr(pos, '\n');
  if (!ptmp)
  {
    *pos = '\0';
    return;
  }

  *ppos = ptmp + 1;
}

/**
  @brief parse_action

  Thermal monitor parse action from string.

  @param  : pvalue:String to parse.

  @return : Parse action to be taken.

*/
static int parse_action(char *pvalue)
{
  int i;

  for (i = 0; i < ARR_SIZE(action_names); i++)
  {
    if (strcasecmp(action_names[i], pvalue) == 0)
    {
      return i;
    }
  }

  /* Unknown action; assuming report-only */
  therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unknown action %s", pvalue);
  return THERM_MONITOR_NONE;
}

/**
  @brief parse_values

  Thermal monitor parse values from string.

  @param  : ppos:String to start parsing on.
            result:Array of values parsed from string.
            field_type:Type of field values to be parsed.

  @return : Number of values parsed from string.

*/
static int parse_values(char **ppos, therm_monitor_value_type *result,
                        therm_monitor_field_type field_type)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  nresult = 0;

  ptmp = strchr(pos, '\n');
  if (ptmp)
  {
    *ptmp = '\0';
  }

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  while (pvalue != NULL)
  {
    switch (field_type)
    {
      case THERM_MONITOR_ACTIONS:
        result[nresult].nVal = parse_action(pvalue);
        break;

      case THERM_MONITOR_ZONE:
        result[nresult].pStr = pvalue;
        break;

      default:
        result[nresult].nVal = atoi(pvalue);
        break;
    }

    nresult++;

    if (nresult >= MAX_NUM_THERM_STATES)
    {
      break;
    }
    pvalue = strtok_r(NULL, "\t \r\n", &psave);
  }

  *ppos = ptmp + 1;
  return nresult;
}

/**
  @brief parse_config

  Thermal monitor parse actual config file.

  @param  : fd:File descriptor.
            config_data: Thermal monitor config data.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/
static int parse_config(therm_monitor_type *config_data, int fd)
{
  char                           *buf         = NULL;
  char                           *pos         = buf;
  char                           *maxpos      = NULL;
  DALBOOL                         error_found = FALSE;
  therm_monitor_sensor_info_type *sensor_ptr  = NULL;
  int                             sensor_idx  = config_data->num_valid_sensors - 1;
  therm_monitor_field_type        in_field    = THERM_MONITOR_FIELD_IDX_MAX;
  int                             ret_val     = MONITOR_FAILED;
  char                           *idx         = NULL;
  int                             sz, i;

  do
  {
    if (fd == -1)
    {
      break;
    }

    if (DALSYS_Malloc(CONFIG_FILE_SIZE_MAX, (void**)&buf) != DAL_SUCCESS)
    {
      break;
    }

    buf[CONFIG_FILE_SIZE_MAX-1] = '\0';

    pos = buf;
    therm_log_printf(THERM_LOG_LEVEL_INFO, 0,
                     "Loading configuration file...");

    sz = efs_read(fd, buf, CONFIG_FILE_SIZE_MAX);
    if (sz < 0)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "Failed to read config file");
      break;
    }
    maxpos = pos + sz;

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "Config file read (%d bytes)", sz);

    /* Start parsing file data. */
    while ((pos != NULL) && (*pos != '\0') && (pos < maxpos))
    {
      switch (*pos)
      {
        case '[':
          /* Look for sensor name */
          idx = strchr(++pos, ']');
          if (!idx)
          {
            /* Unterminated closing ']' */
            error_found = TRUE;
            break;
          }
          in_field = THERM_MONITOR_FIELD_IDX_MAX;
          *idx = '\0';

          sensor_idx++;

          if ( sensor_idx < MAX_NUM_THERM_SENSORS)
          {
            sensor_ptr = &(config_data->sensor[sensor_idx]);
            snprintf(sensor_ptr->sensor_resource_path, MAX_RESOURCE_PATH,
                     therm_node_fmt_str, pos);
            snprintf(sensor_ptr->sampling_resource_path, MAX_RESOURCE_PATH,
                     therm_node_sampling_fmt_str, pos);
            therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "Found section %s",
                             sensor_ptr->sensor_resource_path);
            pos = idx + 1;
            config_data->num_valid_sensors++;
          }
          break;

        case '\t':
        case '\r':
        case '\n':
        case ' ':
          /* Advance past empty space */
          pos++;
          break;

        default:
          if (in_field != THERM_MONITOR_FIELD_IDX_MAX)
          {
            int                      num;
            therm_monitor_value_type values[MAX_NUM_THERM_STATES];

            if (in_field != THERM_MONITOR_ACTIONS)
            {
              num = parse_values(&pos, values, in_field);
            }
            else
            {
              num = parse_values(&pos, values, in_field);
            }

            if ((num < 1) || (pos == NULL))
            {
              therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                               "Failed to parse value");
              break;
            }

            switch (in_field)
            {
              case THERM_MONITOR_ZONE:
                snprintf(sensor_ptr->mitigate_state_resource_path,
                         MAX_RESOURCE_PATH, therm_node_mitigate_fmt_str,
                         values[0].pStr);
                therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                                 "Thermal zone %s",
                                 sensor_ptr->mitigate_state_resource_path);
                break;
              case THERM_MONITOR_SAMPLING:
                if (values[0].nVal <= 0)
                {
                    values[0].nVal = DEFAULT_SAMPLING_MS;
                }
                if (sensor_ptr == NULL)
                {
                    break;
                }
                else
                {
                  for (i = 0; i < num; i++)
                  {
                    sensor_ptr->state_data[i].sampling = values[i].nVal;
                    therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                                     "Sampling period idx %d, %d ms", i,
                                     values[i].nVal);
                  }
                }
                break;
              case THERM_MONITOR_THRESHOLDS:
                if (sensor_ptr == NULL)
                {
                  break;
                }
                sensor_ptr->num_of_states = num;
                for (i = 0; i < num; i++)
                {
                  sensor_ptr->state_data[i].thresh = values[i].nVal;
                  therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                                     "Thresholds idx %d, %d C", i, values[i].nVal);
                }
                break;
              case THERM_MONITOR_THRESHOLDS_CLR:
                if (sensor_ptr == NULL)
                {
                  break;
                }
                for (i = 0; i < num; i++)
                {
                  sensor_ptr->state_data[i].thresh_clr = values[i].nVal;
                  therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                                   "Thresh Clr idx %d, %d C", i, values[i].nVal);
                }
                break;
              case THERM_MONITOR_ACTIONS:
                if (sensor_ptr == NULL)
                {
                  break;
                }
                for (i = 0; i < num; i++)
                {
                  sensor_ptr->state_data[i].action =
                    (therm_monitor_action_type)values[i].nVal;
                  therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                                   "Action idx %d, %s", i,
                                   action_names[values[i].nVal]);
                }
                break;
              case THERM_MONITOR_ACTION_INFO:
                if (sensor_ptr == NULL)
                {
                  break;
                }
                for (i = 0; i < num; i++)
                {
                  sensor_ptr->state_data[i].action_info = values[i].nVal;
                  therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                                   "Action info %d", i);
                }
                break;
              default:
                break;
            }
            in_field = THERM_MONITOR_FIELD_IDX_MAX;
            break;
          }

          idx = strpbrk(pos, "\t\r\n ");
          if (!idx)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Error in file");
            error_found = TRUE;
            break;
          }
          *idx = '\0';
          for (i = 0; i < THERM_MONITOR_FIELD_IDX_MAX; i++)
          {
            if (strcasecmp(field_names[i], pos) != 0)
            {
              continue;
            }
            pos = idx + 1;
            skip_space(&pos);
            therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "Found field");
            in_field = (therm_monitor_field_type) i;
            break;
          }
          if (i == THERM_MONITOR_FIELD_IDX_MAX)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Ignoring unknown field");
            pos = idx + 1;
            skip_line(&pos);
          }
      }

      if (error_found)
      {
          break;
      }
    }

    if (error_found == FALSE)
    {
      ret_val = MONITOR_SUCCESS;
    }
  } while (0);

  if (buf != NULL)
  {
    DALSYS_Free(buf);
  }

  return ret_val;
}

/**
  @brief validate_config

  Thermal monitor validate settings read from config file.

  @param  : config_data: Thermal monitor config data.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/
static int validate_config(therm_monitor_type *config_data)
{
  therm_monitor_sensor_info_type *sensor_ptr  = NULL;
  int i, j;
  int ret_val = MONITOR_FAILED;
  DALBOOL error_detected = FALSE;

  do
  {
    /* Validate that there were sensors present in config file */
    if (config_data->num_valid_sensors == 0)
    {
      break;
    }

    for (i = 0; i < config_data->num_valid_sensors; i++)
    {
      sensor_ptr = &config_data->sensor[i];
      if (sensor_ptr->num_of_states == 0)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Invalid number of states sensor group %d.", i);
        error_detected = TRUE;
        break;
      }

      /* Validate sampling resource path strings */
      for (j = 0; j < therm_monitor_res_count; j++)
      {
         if (strcasecmp(therm_monitor_res_avail[j],
                        sensor_ptr->sampling_resource_path) == 0)
         {
           break;
         }
      }

      if (j >= therm_monitor_res_count)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Invalid sampling path %s.",
                         sensor_ptr->sampling_resource_path);
        error_detected = TRUE;
        break;
      }

      /* Validate sensor resource path strings */
      for (j = 0; j < therm_monitor_res_count; j++)
      {
         if (strcasecmp(therm_monitor_res_avail[j],
                        sensor_ptr->sensor_resource_path) == 0)
         {
           break;
         }
      }

      if (j >= therm_monitor_res_count)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Invalid sensor path %s.",
                         sensor_ptr->sensor_resource_path);
        error_detected = TRUE;
        break;
      }

      /* Validate Mitigate resource path strings */
      for (j = 0; j < therm_monitor_res_count; j++)
      {
         if (strcasecmp(therm_monitor_res_avail[j],
                        sensor_ptr->mitigate_state_resource_path) == 0)
         {
           break;
         }
      }

      if (j >= therm_monitor_res_count)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Invalid mitigate path %s.",
                         sensor_ptr->mitigate_state_resource_path);
        error_detected = TRUE;
        break;
      }

    }

    if (error_detected)
    {
      break;
    }

    ret_val = MONITOR_SUCCESS;
  } while (0);

  return ret_val;
}

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/

/**
  @brief therm_monitor_load_config

  Thermal monitor load config file.

  @param  : None.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/
int therm_monitor_load_config(therm_monitor_type *config_data)
{
  int config_fd = -1;
  int ret_val = MONITOR_FAILED;

  do
  {
    config_fd = efs_open(therm_monitor_ini_file, O_RDONLY, 0);
    if (config_fd < 0)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "Unable to open config file.");
      break;
    }

    if (parse_config(config_data, config_fd) != MONITOR_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "Unable to open config file.");
      break;
    }

    if (validate_config(config_data) != MONITOR_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid config file.");
      break;
    }

    ret_val = MONITOR_SUCCESS;
  } while(0);

  if (config_fd >= 0)
  {
    efs_close(config_fd);
  }

  return ret_val;
}

