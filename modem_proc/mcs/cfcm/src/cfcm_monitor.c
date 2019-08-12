/*!
  @file
  cfcm_monitor.c

  @brief
  CFCM monitor related implementation.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_monitor.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/15/15   rj      Adding support for PDCP COMPRESSION feature for LTE
12/15/14   sg      QSH related modifications
12/11/14   rj      New Monitors in DSM Large and Dup Pool
11/21/14   rj      Adding MDM_TEMP and VDD_PEAK_CURR monitors
11/20/14   rj      Log Packet support added in CFCM
10/29/14   rj      Adding Bootup crash fix
10/27/14   rj      Adding QTF support for CFCM
10/17/14   rj      Added supoort for Thermal RunAway Monitor
10/09/14   rj      CFCM changes to Process monitor's input in its task context
09/15/14   rj      Include only those monitors in mask which have triggered CFCM cmd
09/08/14   rj      Added support for DSM monitor for IPA client
08/20/14   mm      Fixing call to qsh_invoke_cb()
09/04/14   rj      Ignore FREEZE Command if last Command sent is OFF.
08/11/14   rj      Adding support for QSH in CFCM
08/07/14   rj      Update monitor state for all clients, even when its not reg.
07/28/14   rj      Updating F3s to debugging
07/15/14   rj      Added support for QTF compilation issue
07/07/14   rj      Added support for dsm_reg_mem_event_optimized_level()
07/03/14   rj      BUS BW changes
04/07/14   rj      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <customer.h>
#include <comdef.h>
#include <msg.h>
#include <dsm_pool.h>
#include <dsm_init.h>
#include "cfcm_cfg.h"
#include "cfcm_msg.h"
#include "cfcmi.h"
#include "cfcm_monitor.h"
#include "cfcm_client.h"
#include "cfcm_dbg.h"
#include "cfcm_cpu_monitor.h"
#include "cfcm_msgr.h"
#include "cfcm_qsh.h"
#include "cfcm_log.h"
#include <npa.h>

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief whether the monitor is turned on in the mask
*/
#define CFCM_MONITOR_IN_MASK(mask, monitor)    (((mask) & (1 << (monitor))) != 0)

/*! @brief history buffer size
*/
#define CFCM_MONITOR_HIST_BUF_SZ                                         (1 << 4)

/*! @brief history buffer index mask
*/
#define CFCM_MONITOR_HIST_BUF_IDX_MASK              (CFCM_MONITOR_HIST_BUF_SZ - 1)

/*! @brief BUS BW Default Level setting 
*/
#define CFCM_MONITOR_DEFAULT_BUS_BW_LEVEL              (-1)

/*! @brief CFCM Maximum number of states for Thermal PA monitor
*/
#define CFCM_MONITOR_MAX_THERMAL_PA_STATE              (4)

/*! @brief CFCM Maximum number of Thermal PA monitor (PA/PA_EX/PA_EM)
*/
#define CFCM_MONITOR_MAX_THERMAL_PA_MONITOR            (3)

/*! @brief CFCM Maximum number of states for Thermal Runaway/MDM Temp/VDD Peak Curr monitor
*/
#define CFCM_MONITOR_MAX_NPA_NODE_STATE                (4)

/*! @brief CFCM Maximum number of Thermal Runaway/MDM Temp/VDD Peak Curr monitor (Monitor/Monitor_EX)
*/
#define CFCM_MONITOR_MAX_NPA_NODE_MONITORS             (2)

/*! @brief Whether New command should be ignored based on the last command sent to clients 
*/
#define CFCM_MONITOR_IS_IGNORE_NEW_CMD(last_cmd, new_cmd) (((last_cmd) == CFCM_CMD_FC_OFF) && \
                                                          (((new_cmd) == CFCM_CMD_UP)||((new_cmd) == CFCM_CMD_FREEZE)))


/* Mapping of PA thermal state to CFCM commands interpretation */
static const cfcm_cmd_e cfcm_thermal_pa_state_map[CFCM_MONITOR_MAX_THERMAL_PA_STATE][CFCM_MONITOR_MAX_THERMAL_PA_MONITOR] =
{

  {
    /* State 0 ,  MONITOR_THERMAL_PA         */ CFCM_CMD_UP,
    /* State 0 ,  MONITOR_THERMAL_PA_EX   */ CFCM_CMD_UP,
    /* State 0 ,  MONITOR_THERMAL_PA_EM  */ CFCM_CMD_FC_OFF,
  },

  {
    /* State 1 ,  MONITOR_THERMAL_PA         */ CFCM_CMD_DOWN,
    /* State 1 ,  MONITOR_THERMAL_PA_EX   */ CFCM_CMD_UP,
    /* State 1 ,  MONITOR_THERMAL_PA_EM  */ CFCM_CMD_FREEZE,
  },

  {
    /* State 2 ,  MONITOR_THERMAL_PA         */ CFCM_CMD_DOWN,
    /* State 2 ,  MONITOR_THERMAL_PA_EX   */ CFCM_CMD_DOWN,
    /* State 2 ,  MONITOR_THERMAL_PA_EM  */ CFCM_CMD_FREEZE,
  },

  {
    /* State 3 ,  MONITOR_THERMAL_PA         */ CFCM_CMD_SHUT_DOWN,
    /* State 3 ,  MONITOR_THERMAL_PA_EX   */ CFCM_CMD_DOWN,
    /* State 3 ,  MONITOR_THERMAL_PA_EM  */ CFCM_CMD_SHUT_DOWN,
  }
};

/* Mapping of Runaway thermal state to CFCM commands interpretation */
static const cfcm_cmd_e cfcm_npa_node_state_map[CFCM_MONITOR_MAX_NPA_NODE_STATE][CFCM_MONITOR_MAX_NPA_NODE_MONITORS] =
{

  {
    /* State 0 ,  <MONITOR>         */ CFCM_CMD_UP,
    /* State 0 ,  <MONITOR>_EX   */ CFCM_CMD_UP,
  },

  {
    /* State 1 ,  <MONITOR>         */ CFCM_CMD_DOWN,
    /* State 1 ,  <MONITOR>_EX   */ CFCM_CMD_UP,
  },

  {
    /* State 2 ,  <MONITOR>         */ CFCM_CMD_DOWN,
    /* State 2 ,  <MONITOR>_EX   */ CFCM_CMD_DOWN,
  },

  {
    /* State 3 ,  <MONITOR>         */ CFCM_CMD_SHUT_DOWN,
    /* State 3 ,  <MONITOR>_EX   */ CFCM_CMD_SHUT_DOWN,
  }
};

/*! @brief String representation of monitor enum */
const char * cfcm_monitor_str[CFCM_MONITOR_MAX] =
{
  "THERMAL_PA",
  "CPU",
  "DSM_LARGE_UL",
  "DSM_LARGE_DL",
  "DSM_DUP_UL",
  "DSM_DUP_DL",
  "THERMAL_PA_EX",
  "THERMAL_PA_EM",
  "DSM_SMALL_POOL_DL",
  "BW_THROTTLING",
  "DSM_LARGE_POOL_IPA_DL",
  "THERMAL_RUNAWAY",
  "THERMAL_RUNAWAY_EX",
  "MDM_TEMP",
  "MDM_TEMP_EX",
  "VDD_PEAK_CURR",
  "VDD_PEAK_CURR_EX",
  "DSM_LARGE_POOL_LEVEL1_DL",
  "DSM_DUP_POOL_LEVEL1_DL"
};

/*! @brief String representation of monitor enum */
const char * cfcm_cmd_str[CFCM_CMD_MAX] =
{
  "FC_OFF",
  "FC_UP",
  "FC_FREEZE",
  "FC_DOWN",
  "FC_SET_MIN",
  "FC_SHUT_DOWN",
  "FC_SET_VALUE",
  "UNKNOWN"
};

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  uint32  num_state_change;              /*!< total number of state change */
  uint32          num_down;              /*!< total number of down cmds */
  uint32          num_set_min;           /*!< total number of set min cmds */
  uint32          num_shut_down;         /*!< total number of shut down cmds */
  cfcm_timetick_ms_t  time_down;         /*!< time last down cmd sent */
  cfcm_timetick_ms_t  time_set_min;      /*!< time last set min cmd sent */
  cfcm_timetick_ms_t  time_shut_down;    /*!< time last shut down cmd sent */
} cfcm_monitor_stats_s;

/*! @brief: QMI register data info
*/
typedef struct
{
  qmi_csi_service_handle     *service_handle;
  qmi_idl_service_object_type service_object;
  qmi_cfcm_bus_bw_throttle_level_enum_type_v01 qmi_bw_lvl;
} cfcm_monitor_qmi_data_s;

/*! @brief flow control command that is sent by the CFCM
*/
typedef struct
{
  boolean              cmd_updated; /*!< Is this command updated due to monitor input */

  cfcm_cmd_e           cmd;        /*!< flow control command */

  uint32               step_timer; /*!< step timer in ms, shall be ignored unless 
                                        the cmd is up or down */
  uint32               monitors_active;  /*!<Bit mask of monitors which set command,
                                          e.g. CFCM_CONV_ENUM_TO_BIT_MASK(CFCM_MONITOR_CPU)*/
} cfcm_monitor_last_cmd_type_s;

/*! @brief cfcm monitor information structure
*/
typedef struct
{
  cfcm_monitor_e monitor_id;  /*!< for ease of debugging */
  uint32        num_client; /*!< number of clients  */
  cfcm_client_e  client[CFCM_CLIENT_MAX];/*!<clients to respond to the monitor */
  cfcm_monitor_last_cmd_type_s  last_cmd[CFCM_CLIENT_MAX]; /*!<  last cmd from this monitor per client */
  boolean       registered; /*!< registered with the source monitor */
  cfcm_monitor_stats_s stats; /*!< statistics for the monitor */
  uint32        latest_cmd_hist_idx;/*!< latest command history buffer index */ 
  cfcm_cmd_e     cmd_hist[CFCM_MONITOR_HIST_BUF_SZ];  /*!< command history buffer */
  cfcm_client_e     client_hist[CFCM_MONITOR_HIST_BUF_SZ];  /*!< client's command history buffer */
} cfcm_monitor_info_s;

/*! @brief top level structure for cfcm_monitor
*/
typedef struct
{
  cfcm_monitor_info_s  monitors[CFCM_MONITOR_MAX];/*!<info for all known monitor*/
  uint32              num_errors;               /*!< total number of errors */
  cfcm_monitor_qmi_data_s      qmi_data;    /*!< Monitor's specific data */
} cfcm_monitor_s;

/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/

STATIC cfcm_monitor_s cfcm_monitor;
STATIC cfcm_monitor_s* const cfcm_monitor_ptr = &cfcm_monitor;
/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_npa_change

==============================================================================*/
/*!
    @brief
    handles NPA state change for thermal.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_proc_npa_change
(
  npa_resource_state state   /*!< new NPA state  */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM thermal monitor cfcm_monitor_proc_npa_change state %d", state);

  msg.data_info = CFCM_MONITOR_THERMAL_PA_INPUT;
  msg.monitor_data.npa_node.state = (uint32)state;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_proc_npa_change() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_npa_event_cb

==============================================================================*/
/*!
    @brief
    NPA event change handler.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_npa_event_cb
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  cfcm_monitor_proc_npa_change(event_data_ptr->state);

} /* cfcm_monitor_npa_event_cb() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_npa_available_cb

==============================================================================*/
/*!
    @brief
    callback function when npa is avaiable.

    @return
    None

*/
/*============================================================================*/
void cfcm_monitor_npa_available_cb 
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  npa_event_handle event_client;
  npa_query_type query_result;
  npa_query_status query_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  event_client = npa_create_change_event_cb(CFCM_MONITOR_THERMAL_PA_NPA_NODE,
                                           "PA Thermal CFCM",
                                           cfcm_monitor_npa_event_cb,
                                           NULL);
  CFCM_ASSERT(event_client != NULL);

  CFCM_MSG_0(HIGH, "CFCM thermal monitor npa_create_change_event_cb done");

  query_status = npa_query_by_event(event_client,
                                    NPA_QUERY_CURRENT_STATE,
                                    &query_result);

  CFCM_ASSERT(query_status == NPA_QUERY_SUCCESS);

  cfcm_monitor_proc_npa_change(query_result.data.state);

} /* cfcm_monitor_npa_available_cb() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_thermal_ra_change

==============================================================================*/
/*!
    @brief
    handles NPA state change for thermal Runaway monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_proc_thermal_ra_change
(
  npa_resource_state state   /*!< new NPA state  */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM thermal monitor cfcm_monitor_proc_thermal_ra_change state %d", state);

  msg.data_info = CFCM_MONITOR_THERMAL_RA_INPUT;
  msg.monitor_data.npa_node.state = (uint32)state;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_proc_thermal_ra_change() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_thermal_ra_event_cb

==============================================================================*/
/*!
    @brief
    NPA event change handler for Thermal Runaway monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_thermal_ra_event_cb
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  cfcm_monitor_proc_thermal_ra_change(event_data_ptr->state);

} /* cfcm_monitor_thermal_ra_event_cb() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_thermal_ra_available_cb

==============================================================================*/
/*!
    @brief
    callback function when npa is avaiable.

    @return
    None

*/
/*============================================================================*/
void cfcm_monitor_thermal_ra_available_cb 
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  npa_event_handle event_client;
  npa_query_type query_result;
  npa_query_status query_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  event_client = npa_create_change_event_cb(CFCM_MONITOR_THERMAL_RA_NPA_NODE,
                                           "PA Thermal CFCM",
                                           cfcm_monitor_thermal_ra_event_cb,
                                           NULL);
  CFCM_ASSERT(event_client != NULL);

  CFCM_MSG_0(HIGH, "CFCM thermal Runaway monitor npa_create_change_event_cb done");

  query_status = npa_query_by_event(event_client,
                                    NPA_QUERY_CURRENT_STATE,
                                    &query_result);

  CFCM_ASSERT(query_status == NPA_QUERY_SUCCESS);

  cfcm_monitor_proc_thermal_ra_change(query_result.data.state);

} /* cfcm_monitor_thermal_ra_available_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_mdm_temp_change

==============================================================================*/
/*!
    @brief
    handles NPA state change for MDM Temperature monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_proc_mdm_temp_change
(
  npa_resource_state state   /*!< new NPA state  */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM thermal monitor cfcm_monitor_proc_mdm_temp_change state %d", state);

  msg.data_info = CFCM_MONITOR_MDM_TEMP_INPUT;
  msg.monitor_data.npa_node.state = (uint32)state;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_proc_mdm_temp_change() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_mdm_temp_event_cb

==============================================================================*/
/*!
    @brief
    NPA event change handler for MDM Temp monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_mdm_temp_event_cb
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  cfcm_monitor_proc_mdm_temp_change(event_data_ptr->state);

} /* cfcm_monitor_mdm_temp_event_cb() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_mdm_temp_available_cb

==============================================================================*/
/*!
    @brief
    callback function when npa is avaiable.

    @return
    None

*/
/*============================================================================*/
void cfcm_monitor_mdm_temp_available_cb 
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  npa_event_handle event_client;
  npa_query_type query_result;
  npa_query_status query_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  event_client = npa_create_change_event_cb(CFCM_MONITOR_MDM_TEMP_NPA_NODE,
                                           "MDM Temp CFCM",
                                           cfcm_monitor_mdm_temp_event_cb,
                                           NULL);
  CFCM_ASSERT(event_client != NULL);

  CFCM_MSG_0(HIGH, "CFCM MDM Temp monitor npa_create_change_event_cb done");

  query_status = npa_query_by_event(event_client,
                                    NPA_QUERY_CURRENT_STATE,
                                    &query_result);

  CFCM_ASSERT(query_status == NPA_QUERY_SUCCESS);

  cfcm_monitor_proc_mdm_temp_change(query_result.data.state);

} /* cfcm_monitor_mdm_temp_available_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_vdd_peak_curr_change

==============================================================================*/
/*!
    @brief
    handles NPA state change for VDD Peak Current monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_proc_vdd_peak_curr_change
(
  npa_resource_state state   /*!< new NPA state  */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM thermal monitor cfcm_monitor_proc_vdd_peak_curr_change state %d", state);

  msg.data_info = CFCM_MONITOR_VDD_PEAK_CURR_INPUT;
  msg.monitor_data.npa_node.state = (uint32)state;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_proc_vdd_peak_curr_change() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_vdd_peak_curr_event_cb

==============================================================================*/
/*!
    @brief
    NPA event change handler for VDD Peak Current monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_vdd_peak_curr_event_cb
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  cfcm_monitor_proc_vdd_peak_curr_change(event_data_ptr->state);

} /* cfcm_monitor_vdd_peak_curr_event_cb() */



/*==============================================================================

  FUNCTION:  cfcm_monitor_vdd_peak_curr_available_cb

==============================================================================*/
/*!
    @brief
    callback function when npa is avaiable.

    @return
    None

*/
/*============================================================================*/
void cfcm_monitor_vdd_peak_curr_available_cb 
(
  void         *context,    /*!< context for the event */
  unsigned int  event_type, /*!< event type triggered the callback */
  void         *data,       /*!< pointer to data */
  unsigned int  data_size   /*!< size of data */
)
{
  npa_event_handle event_client;
  npa_query_type query_result;
  npa_query_status query_status;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  event_client = npa_create_change_event_cb(CFCM_MONITOR_VDD_PEAK_CURR_NPA_NODE,
                                           "VDD Peak Curr CFCM",
                                           cfcm_monitor_vdd_peak_curr_event_cb,
                                           NULL);
  CFCM_ASSERT(event_client != NULL);

  CFCM_MSG_0(HIGH, "CFCM VDD Peak Current monitor npa_create_change_event_cb done");

  query_status = npa_query_by_event(event_client,
                                    NPA_QUERY_CURRENT_STATE,
                                    &query_result);

  CFCM_ASSERT(query_status == NPA_QUERY_SUCCESS);

  cfcm_monitor_proc_vdd_peak_curr_change(query_result.data.state);

} /* cfcm_monitor_vdd_peak_curr_available_cb() */


/*==============================================================================

  FUNCTION:  cfcm_process_bus_bw_input

==============================================================================*/
/*!
    @brief
    Handles Bus BW Throttling level input.

    @return
    None
*/
/*============================================================================*/
static void cfcm_process_bus_bw_input( void )
{
  uint8 index = 0;
  qmi_cfcm_bus_bw_throttle_level_enum_type_v01 bw_lvl;
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type         send_status;
  cfcm_cmd_e              pa_cmd;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  bw_lvl = cfcm_monitor_ptr->qmi_data.qmi_bw_lvl;
  
  /* send monitor state change indication to the CFCM */
  switch (bw_lvl)
  {
    case QMI_CFCM_BUS_BW_NO_THROTTLE_V01:
    {
      pa_cmd = CFCM_CMD_UP;
      break;
    }
    case QMI_CFCM_BUS_BW_MED_THROTTLE_V01:
    {
      pa_cmd = CFCM_CMD_DOWN;
      break;
    }
    case QMI_CFCM_BUS_BW_HIGH_THROTTLE_V01:
    {
      pa_cmd = CFCM_CMD_DOWN;
      break;
    }
    case QMI_CFCM_BUS_BW_CRITICAL_THROTTLE_V01:
    {
      pa_cmd = CFCM_CMD_SHUT_DOWN;
      break;
    }
    default:
    {
      CFCM_MSG_1(ERROR, "::CFCM:: cfcm_process_bus_bw_input invalid Input %d", bw_lvl);
      return;
    }
  }


  for(index = 0; index < CFCM_CLIENT_MAX; index++)
  {
    msg.monitor_data.monitor_cmd_info.cmd[index] = pa_cmd;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_BW_THROTTLING;
  msg.monitor_data.monitor_cmd_info.log_data.bw.level= (uint8)bw_lvl;
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_process_bus_bw_input() */
/*=============================================================================

  FUNCTION:  cfcm_qmi_process_req

=============================================================================*/
/*!
    @brief
        Callback function called by QCSI infrastructure when a REQ message to
        CFCM is received
 
    @note
    	QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
   	 qmi_csi_cb_error
*/
/*===========================================================================*/

qmi_csi_cb_error cfcm_qmi_process_req (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  qmi_csi_cb_error      req_cb_retval    = QMI_CSI_CB_NO_ERR;
  qmi_response_type_v01 response;
  qmi_cfcm_bus_bw_throttle_level_enum_type_v01 rcvd_qmi_bw_lvl, old_bw_lvl;
  cfcm_get_bus_bw_throttling_level_resp_msg_v01 get_resp;
  /*-----------------------------------------------------------------------*/

  /* Process message appropriately based on QMI msg_id */
  switch( msg_id )
  {
    /* This request provides the updated Throttling level */
    case QMI_CFCM_BUS_BW_THROTTLING_LEVEL_REQ_MSG_V01:
      {
        /* Store current BW Level locally */
        old_bw_lvl = cfcm_monitor_ptr->qmi_data.qmi_bw_lvl;

        /* Store the new BW Level locally */
        rcvd_qmi_bw_lvl = 
          ((cfcm_set_bus_bw_throttling_level_req_msg_v01 *)req_c_struct)->bus_bw_throttling_level;

         /* Initialize structure holding response */
         memset( &response, 0, sizeof( cfcm_set_bus_bw_throttling_level_resp_msg_v01 ) );

        /* If the BW Level is not within limits, respond with failure... */
        if (!CFCM_IS_BUS_BW_LEVEL_VALID(rcvd_qmi_bw_lvl))
        {
          response.result = QMI_RESULT_FAILURE_V01;
          response.error = QMI_ERR_INVALID_ARG_V01;
        }
        else
        {
          /* Update Bus BW Level */
          cfcm_monitor_ptr->qmi_data.qmi_bw_lvl = rcvd_qmi_bw_lvl;

          /* Send a success response */
          response.result = QMI_RESULT_SUCCESS_V01;
          response.error = QMI_ERR_NONE_V01;
        }

        CFCM_MSG_3(HIGH, "Received new Bus_BW Level %d, current %d, applied %d",
                       rcvd_qmi_bw_lvl, old_bw_lvl, cfcm_monitor_ptr->qmi_data.qmi_bw_lvl);

        /* send response prior to initiating further algorithm  */
        req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
           connection_handle,
           req_handle,
           QMI_CFCM_BUS_BW_THROTTLING_LEVEL_RESP_MSG_V01,
           &response,
           sizeof( cfcm_set_bus_bw_throttling_level_resp_msg_v01 )
           );
        if( req_cb_retval == QMI_CSI_CB_NO_ERR )
        {
          CFCM_MSG_3(MED, "Sent resp msg %d res %d err %d for BUS_BW_THROTTLING_LEVEL req msg",
                        msg_id, response.result, response.error);
        }
        else
        {
          CFCM_MSG_3(ERROR, "Could not send BUS_BW_THROTTLING_LEVEL resp msg %d res %d err %d",
                        msg_id, response.result, response.error);
        }
        
        /* Send messages to interested parties only if the BW Throttling Level has changed */
        if( cfcm_monitor_ptr->qmi_data.qmi_bw_lvl != old_bw_lvl )
        {
          /* Process the change in BW */
          cfcm_process_bus_bw_input();
        }
      }
      break;

     /* This request is used to query Bus BW Throttling */
     case QMI_CFCM_GET_BW_THROTTLING_LEVEL_REQ_MSG_V01:
       {
         /* Initialize structure holding response */
         memset( &get_resp, 0, sizeof( cfcm_get_bus_bw_throttling_level_resp_msg_v01 ) );

         if (cfcm_monitor_ptr->qmi_data.qmi_bw_lvl != CFCM_MONITOR_DEFAULT_BUS_BW_LEVEL)
         {
           get_resp.bus_bw_throttling_level_valid = TRUE;
         }
		 else
         {
           get_resp.bus_bw_throttling_level_valid = FALSE;
         }
         get_resp.bus_bw_throttling_level = cfcm_monitor_ptr->qmi_data.qmi_bw_lvl;

         /* send out response */
         req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
                              connection_handle,
                              req_handle,
                              QMI_CFCM_GET_BW_THROTTLING_LEVEL_RESP_MSG_V01,
                              &get_resp,
                              sizeof( cfcm_get_bus_bw_throttling_level_resp_msg_v01 )
                          );

        if( req_cb_retval == QMI_CSI_CB_NO_ERR )
        {
          CFCM_MSG_3(MED, "Sent Get BUS_BW_THROTTLING_LEVEL %d resp msg %d handle %d", 
                    get_resp.bus_bw_throttling_level, msg_id, req_handle);
        }
        else
        {
          CFCM_MSG_2(ERROR, "Could not send Get BUS_BW_THROTTLING_LEVEL resp msg %d handle %d",
                    msg_id, req_handle);
        }
      }
      break;

    /* error case */
    default:
      /* Initialize structure holding response */
      memset( &response, 0, sizeof( qmi_response_type_v01 ) );			

      response.result = QMI_RESULT_FAILURE_V01;
      response.error  = QMI_ERR_INVALID_MESSAGE_ID_V01;

      req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb( 
                          connection_handle,
                          req_handle,
                          msg_id,
                          &response,
                          sizeof( qmi_response_type_v01 )
                        );
      break;
  } /* switch ( msg_id ) */

  /*-----------------------------------------------------------------------*/
  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  cfcm_qmi_get_ind_offset

=============================================================================*/
/*!
    @brief
    This method maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
int32 cfcm_qmi_get_ind_offset 
(
  const uint32   ind_msg_id  /*!< Indication whose internal offset is needed */
)
{
  /*-----------------------------------------------------------------------*/
  CFCM_UNUSED(ind_msg_id);
  return -1;
}

/*==============================================================================

  FUNCTION:  cfcm_monitor_bus_bw_init

==============================================================================*/
/*!
    @brief
    Initialized Bus Bandwidth Monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_bus_bw_init( void )
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  /* 1. Setup service object */
  cfcm_monitor_ptr->qmi_data.service_object = cfcm_get_service_object_v01();

  /* 2. Register object with QMI */
  (void) qmi_si_register_object ( cfcm_monitor_ptr->qmi_data.service_object,
                                  0, /* Service Instance */
                                  cfcm_get_service_impl_v01() );

  /* 3. Register/Open/Initiate service with Connection Manager infrastructure */
  cfcm_monitor_ptr->qmi_data.service_handle = mqcsi_conn_mgr_service_open (
                                               MQCSI_CFCM_SERVICE_ID,
                                               "CFCM_SERVICE",
                                               &cfcm_tcb,
                                               CFCM_TASK_QMI_SIG,
                                               cfcm_monitor_ptr->qmi_data.service_object, 
                                               cfcm_qmi_process_req,
                                               cfcm_qmi_get_ind_offset,
                                               NULL,
                                               CFCM_V01_IDL_MAJOR_VERS,
                                               CFCM_V01_IDL_MINOR_VERS );
  CFCM_ASSERT( NULL != cfcm_monitor_ptr->qmi_data.service_handle );

  cfcm_monitor_ptr->qmi_data.qmi_bw_lvl = CFCM_MONITOR_DEFAULT_BUS_BW_LEVEL;
} /* cfcm_monitor_bus_bw_init() */


/*==============================================================================

  FUNCTION:  cfcm_update_rlc_clients_dsm_cmd

==============================================================================*/
/*!
    @brief
    Updates clients with RLC DSM Commands

    @return
    None
*/
/*============================================================================*/
static void cfcm_update_rlc_clients_dsm_cmd
( 
  cfcm_cmd_e* cmd,
  cfcm_cmd_e new_cmd
)
{
  cfcm_client_e client;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  for(client = CFCM_CLIENT_LTE_UL; client < CFCM_CLIENT_MAX; client++)
  {
    if (CFCM_IS_RLC_CLIENT(client))
    {
      cmd[client] = new_cmd;
    }
    else
    {
      cmd[client] = CFCM_CMD_INVALID;
    }
  }
} /* cfcm_update_rlc_clients_dsm_cmd() */


/*==============================================================================

  FUNCTION:  cfcm_update_non_rlc_clients_dsm_cmd

==============================================================================*/
/*!
    @brief
    Updates clients with non-RLC DSM Commands

    @return
    None
*/
/*============================================================================*/
static void cfcm_update_non_rlc_clients_dsm_cmd
( 
  cfcm_cmd_e* cmd,
  cfcm_cmd_e new_cmd
)
{
  cfcm_client_e client;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  for(client = CFCM_CLIENT_LTE_UL; client < CFCM_CLIENT_MAX; client++)
  {
    if (!CFCM_IS_RLC_CLIENT(client))
    {
      cmd[client] = new_cmd;
    }
    else
    {
      cmd[client] = CFCM_CMD_INVALID;
    }
  }
} /* cfcm_update_non_rlc_clients_dsm_cmd() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_get_clients_step_timer_value

==============================================================================*/
/*!
    @brief
    Get step timer value for client's monitor 

    @return
    None
*/
/*============================================================================*/
static uint32 cfcm_monitor_get_clients_step_timer_value
(
  cfcm_client_e client,
  cfcm_monitor_e monitor   /*!< monitor */
)
{
  uint32               step_timer = 0;
  

  switch(monitor)
  {
    case CFCM_MONITOR_DSM_LARGE_POOL_UL:
    case CFCM_MONITOR_DSM_LARGE_POOL_DL:
    case CFCM_MONITOR_DSM_DUP_POOL_UL:
    case CFCM_MONITOR_DSM_DUP_POOL_DL:
    case CFCM_MONITOR_DSM_SMALL_POOL_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL:
    case CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_COMP:
    case CFCM_MONITOR_DSM_DUP_POOL_COMP:
    {      
      step_timer  = cfcm_cfg_get_dsm_step_timer(client);
      break;
    }
    case CFCM_MONITOR_THERMAL_PA:    
    case CFCM_MONITOR_THERMAL_PA_EX:
    case CFCM_MONITOR_THERMAL_PA_EM:
    case CFCM_MONITOR_THERMAL_RUNAWAY:
    case CFCM_MONITOR_THERMAL_RUNAWAY_EX:
    case CFCM_MONITOR_MDM_TEMP:
    case CFCM_MONITOR_MDM_TEMP_EX:
    case CFCM_MONITOR_VDD_PEAK_CURR_EST:
    case CFCM_MONITOR_VDD_PEAK_CURR_EST_EX:
    {
      step_timer  = cfcm_cfg_get_thermal_step_timer(client);
      break;
    }
    case CFCM_MONITOR_CPU:
    {
      step_timer = cfcm_cfg_get_cpu_step_timer(client);
      break;
    }
    case CFCM_MONITOR_BW_THROTTLING:
    {
      step_timer  = cfcm_cfg_get_bw_step_timer(client);
      break;
    }
    default:
    {
      step_timer  = CFCM_MONITOR_DEFAULT_STEP_TIMER;
      CFCM_ERR_FATAL("cfcm_monitor_init_monitor monitor %d invalid", monitor, 0, 0);
      break;
    }
  }

  return step_timer;
}

/*==============================================================================

  FUNCTION:  cfcm_monitor_init_monitor

==============================================================================*/
/*!
    @brief
    initialize a single monitor.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_init_monitor
(
  cfcm_monitor_e monitor   /*!< monitor to be initialized */
)
{
  cfcm_monitor_info_s*   monitor_ptr;
  uint32 j;
  /*--------------------------------------------------------------------------*/

  CFCM_ASSERT(monitor < CFCM_MONITOR_MAX);

  /*--------------------------------------------------------------------------*/
  monitor_ptr = &cfcm_monitor_ptr->monitors[(uint32)monitor];

  monitor_ptr->monitor_id           = monitor;
  monitor_ptr->num_client           = 0;
  monitor_ptr->registered           = FALSE;
  memset(&monitor_ptr->stats, 0, sizeof(cfcm_monitor_stats_s));


  for(j = 0; j < (uint32)CFCM_CLIENT_MAX; j++)
  {
    switch(monitor)
    {
      case CFCM_MONITOR_DSM_LARGE_POOL_UL:
      case CFCM_MONITOR_DSM_LARGE_POOL_DL:
      case CFCM_MONITOR_DSM_DUP_POOL_UL:
      case CFCM_MONITOR_DSM_DUP_POOL_DL:
      case CFCM_MONITOR_DSM_SMALL_POOL_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL:
      case CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_COMP:
      case CFCM_MONITOR_DSM_DUP_POOL_COMP:
      {      
        monitor_ptr->last_cmd[j].step_timer  = cfcm_cfg_get_dsm_step_timer((cfcm_client_e)j);
        break;
      }
      case CFCM_MONITOR_THERMAL_PA:    
      case CFCM_MONITOR_THERMAL_PA_EX:
      case CFCM_MONITOR_THERMAL_PA_EM:
      case CFCM_MONITOR_THERMAL_RUNAWAY:
      case CFCM_MONITOR_THERMAL_RUNAWAY_EX:
      case CFCM_MONITOR_MDM_TEMP:
      case CFCM_MONITOR_MDM_TEMP_EX:
      case CFCM_MONITOR_VDD_PEAK_CURR_EST:
      case CFCM_MONITOR_VDD_PEAK_CURR_EST_EX:
      {
        monitor_ptr->last_cmd[j].step_timer  = cfcm_cfg_get_thermal_step_timer((cfcm_client_e)j);
        break;
      }
      case CFCM_MONITOR_CPU:
      {
        monitor_ptr->last_cmd[j].step_timer = cfcm_cfg_get_cpu_step_timer((cfcm_client_e)j);
        break;
      }
      case CFCM_MONITOR_BW_THROTTLING:
      {
        monitor_ptr->last_cmd[j].step_timer = cfcm_cfg_get_bw_step_timer((cfcm_client_e)j);
        break;
      }
      default:
      {
        monitor_ptr->last_cmd[j].step_timer  = CFCM_MONITOR_DEFAULT_STEP_TIMER;
        CFCM_ERR_FATAL("cfcm_monitor_init_monitor monitor %d invalid", monitor, 0, 0);
        break;
      }
    }

    monitor_ptr->last_cmd[j].cmd = CFCM_CMD_FC_OFF;
    monitor_ptr->client[j] = CFCM_CLIENT_MAX;
  }

  for(j = 0; j < CFCM_MONITOR_HIST_BUF_SZ; j++)
  {
    monitor_ptr->cmd_hist[j] = CFCM_CMD_FC_OFF;
    monitor_ptr->client_hist[j] = CFCM_CLIENT_MAX;
  }
  monitor_ptr->latest_cmd_hist_idx = CFCM_MONITOR_HIST_BUF_IDX_MASK;
} /* cfcm_monitor_init_monitor() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_ul_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_ul_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_LARGE_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_large_pool_ul_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_A2_UL_PER_FEW:
    case DSM_MEM_LEVEL_A2_UL_PER_MANY:
    {
      if(event == DSM_MEM_LEVEL_A2_UL_PER_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_DOWN;
      }
      else // DSM_MEM_LEVEL_A2_UL_PER_MANY
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_large_pool_ul_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_LARGE_POOL_UL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_large_pool_ul_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_ipa_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_ipa_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_LARGE_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_large_pool_ipa_dl_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_IPA_DL_FEW:
    case DSM_MEM_LEVEL_IPA_DL_MANY:
    {
      if(event == DSM_MEM_LEVEL_IPA_DL_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_DOWN;
      }
      else // DSM_MEM_LEVEL_IPA_DL_MANY
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_large_pool_ipa_dl_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_large_pool_ipa_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_ul_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_ul_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DUP_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_dup_pool_ul_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_A2_UL_PER_FEW:
    case DSM_MEM_LEVEL_A2_UL_PER_MANY:
    {
      if(event == DSM_MEM_LEVEL_A2_UL_PER_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_DOWN;
      }
      else // DSM_MEM_LEVEL_A2_UL_PER_MANY
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_register_with_src invalid event %d", event, 0, 0);
    }
    break;
  }

  /* send monitor state change indication to the CFCM */
  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_DUP_POOL_UL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_dup_pool_ul_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_LARGE_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_large_pool_dl_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_LTE_DL_FEW:
    case DSM_MEM_LEVEL_LTE_DL_MANY:
    case DSM_MEM_LEVEL_LTE_DNE:
    {
      if(event == DSM_MEM_LEVEL_LTE_DL_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_DOWN;
      }
      else if(event == DSM_MEM_LEVEL_LTE_DL_MANY)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      else // DSM_MEM_LEVEL_LTE_DNE
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_SHUT_DOWN;
      }
      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    case DSM_MEM_LEVEL_RLC_FEW:
    case DSM_MEM_LEVEL_RLC_MANY:
    case DSM_MEM_LEVEL_LINK_LAYER_DNE:
    {
      if(event == DSM_MEM_LEVEL_RLC_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_SET_MIN;
      }
      else if(event == DSM_MEM_LEVEL_RLC_MANY)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      else // DSM_MEM_LEVEL_LINK_LAYER_DNE
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_SHUT_DOWN;
      }
      /* Update commands for RLC clients for RLC DSM levels */
      cfcm_update_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_large_pool_dl_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_LARGE_POOL_DL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_large_pool_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DUP_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_dup_pool_dl_event_cb event %d", event);

    switch(event)
    {
      case DSM_MEM_LEVEL_LTE_DL_FEW:
      case DSM_MEM_LEVEL_LTE_DL_MANY:
      case DSM_MEM_LEVEL_LTE_DNE:
      {
        /* send monitor state change indication to the CFCM */
        if(event == DSM_MEM_LEVEL_LTE_DL_FEW)
        {
          CFCM_ASSERT(op == DSM_MEM_OP_NEW);
          cmd = CFCM_CMD_DOWN;
        }
        else if(event == DSM_MEM_LEVEL_LTE_DL_MANY)
        {
          CFCM_ASSERT(op == DSM_MEM_OP_FREE);
          cmd = CFCM_CMD_FC_OFF;
        }
        else // DSM_MEM_LEVEL_LTE_DNE
        {
          CFCM_ASSERT(op == DSM_MEM_OP_NEW);
          cmd = CFCM_CMD_SHUT_DOWN;
        }
        /* Update commands for non-RLC clients for DSM levels */
        cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
      }
      break;

      case DSM_MEM_LEVEL_RLC_FEW:
      case DSM_MEM_LEVEL_RLC_MANY:
      {
        /* send monitor state change indication to the CFCM */
        if(event == DSM_MEM_LEVEL_RLC_FEW)
        {
          CFCM_ASSERT(op == DSM_MEM_OP_NEW);
          cmd = CFCM_CMD_SET_MIN;
        }
        else // DSM_MEM_LEVEL_RLC_MANY
        {
          CFCM_ASSERT(op == DSM_MEM_OP_FREE);
          cmd = CFCM_CMD_FC_OFF;
        }
        /* Update commands for RLC clients for RLC DSM levels */
        cfcm_update_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
      }
      break;

      default:
      {
        CFCM_ERR_FATAL("cfcm_monitor_dup_pool_dl_event_cb invalid event %d", event, 0, 0);
      }
      break;
    }


  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_DUP_POOL_DL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_dup_pool_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_dl_level1_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_dl_level1_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_LARGE_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_large_pool_dl_level1_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_DOWN;

      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    case DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_FREE);
      cmd = CFCM_CMD_FC_OFF;

      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_large_pool_dl_level1_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_large_pool_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_dl_level1_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_dl_level1_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DUP_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_dup_pool_dl_level1_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_DOWN;

      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    case DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_FREE);
      cmd = CFCM_CMD_FC_OFF;

      /* Update commands for non-RLC clients for DSM levels */
      cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_dup_pool_dl_level1_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL;
 
  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_dup_pool_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_ul_comp_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_ul_comp_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_LARGE_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_large_pool_ul_comp_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_DOWN;
    }
    break;

    case DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_FREE);
      cmd = CFCM_CMD_FC_OFF;
    }
    break;

    case DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_SHUT_DOWN;
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_large_pool_ul_comp_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  /* Update commands for non-RLC clients for DSM levels */
  cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_LARGE_POOL_COMP;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_large_pool_ul_comp_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_ul_comp_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_ul_comp_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;

  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DUP_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_dup_pool_ul_comp_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_DOWN;
    }
    break;

    case DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_FREE);
      cmd = CFCM_CMD_FC_OFF;
    }
    break;

    case DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE:
    {
      CFCM_ASSERT(op == DSM_MEM_OP_NEW);
      cmd = CFCM_CMD_SHUT_DOWN;
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_dup_pool_ul_comp_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  /* Update commands for non-RLC clients for DSM levels */
  cfcm_update_non_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_DUP_POOL_COMP;
 
  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_dup_pool_ul_comp_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_small_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with small dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_small_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
)
{
  cfcm_monitor_ind_msg_s  msg;
  errno_enum_type        send_status;
  cfcm_cmd_e             cmd;
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(pool_id == DSM_DS_SMALL_ITEM_POOL);
  /*--------------------------------------------------------------------------*/

  CFCM_MSG_1(HIGH, "CFCM DSM monitor cfcm_monitor_small_pool_dl_event_cb event %d", event);

  switch(event)
  {
    case DSM_MEM_LEVEL_RLC_FEW:
    case DSM_MEM_LEVEL_RLC_MANY:
    case DSM_MEM_LEVEL_LINK_LAYER_DNE:
    {
      if(event == DSM_MEM_LEVEL_RLC_FEW)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_SET_MIN;
      }
      else if(event == DSM_MEM_LEVEL_RLC_MANY)
      {
        CFCM_ASSERT(op == DSM_MEM_OP_FREE);
        cmd = CFCM_CMD_FC_OFF;
      }
      else // DSM_MEM_LEVEL_LINK_LAYER_DNE
      {
        CFCM_ASSERT(op == DSM_MEM_OP_NEW);
        cmd = CFCM_CMD_SHUT_DOWN;
      }
      /* Update commands for RLC clients for RLC DSM levels */
      cfcm_update_rlc_clients_dsm_cmd(msg.monitor_data.monitor_cmd_info.cmd, cmd);
    }
    break;

    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_small_pool_dl_event_cb invalid event %d", event, 0, 0);
    }
    break;
  }

  msg.data_info = CFCM_MONITOR_CMDS_UPDATED;
  msg.monitor_data.monitor_cmd_info.monitor = CFCM_MONITOR_DSM_SMALL_POOL_DL;

  msg.monitor_data.monitor_cmd_info.log_data.dsm.pool_id= (int)pool_id;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.event= (int)event;
  msg.monitor_data.monitor_cmd_info.log_data.dsm.op= (int)op;

  /* send monitor state change indication to the CFCM */
  send_status = cfcm_msgr_send_msg(&msg.hdr,
                             MCS_CFCM_MONITOR_IND,
                             sizeof(cfcm_monitor_ind_msg_s));
  CFCM_ASSERT(send_status == E_SUCCESS);

} /* cfcm_monitor_small_pool_dl_event_cb() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_lte_with_dsm_pool

==============================================================================*/
/*!
    @brief
    Register LTE clients with DSM pools.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_reg_lte_with_dsm_pool
(
  cfcm_monitor_e   monitor   /*!< client that triggered this registration */
)
{
  cfcm_dbg_s* cfcm_dbg_ptr;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  cfcm_dbg_ptr = cfcm_dbg_get_data();

  switch(monitor)
  {
    case CFCM_MONITOR_DSM_LARGE_POOL_UL:
    {
      /* large pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_A2_UL_PER_FEW,
                              cfcm_dbg_ptr->large_pool_a2_ul_per_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_A2_UL_PER_MANY,
                              cfcm_dbg_ptr->large_pool_a2_ul_per_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                      	DSM_MEM_LEVEL_A2_UL_PER_FEW,
                                      	DSM_MEM_LEVEL_A2_UL_PER_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_A2_UL_PER_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_ul_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_A2_UL_PER_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_ul_event_cb );
      break;
    }
    case CFCM_MONITOR_DSM_DUP_POOL_UL:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_A2_UL_PER_FEW,
                              cfcm_dbg_ptr->dup_pool_a2_ul_per_few);

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_A2_UL_PER_MANY,
                              cfcm_dbg_ptr->dup_pool_a2_ul_per_many);

      dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                      	DSM_MEM_LEVEL_A2_UL_PER_FEW,
                                      	DSM_MEM_LEVEL_A2_UL_PER_MANY);

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_A2_UL_PER_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_ul_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_A2_UL_PER_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_dup_pool_ul_event_cb );
      break;
    }
    case CFCM_MONITOR_DSM_LARGE_POOL_DL:
    {
      /* large pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DNE,
                              cfcm_dbg_ptr->large_pool_lte_dne);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_FEW,
                              cfcm_dbg_ptr->large_pool_lte_dl_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_MANY,
                              cfcm_dbg_ptr->large_pool_lte_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                      	DSM_MEM_LEVEL_LTE_DL_FEW,
                                      	DSM_MEM_LEVEL_LTE_DL_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_dl_event_cb );
      break;
    }
    case CFCM_MONITOR_DSM_DUP_POOL_DL:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DNE,
                              cfcm_dbg_ptr->dup_pool_lte_dne);      

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_FEW,
                              cfcm_dbg_ptr->dup_pool_lte_dl_few);

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_MANY,
                              cfcm_dbg_ptr->dup_pool_lte_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                      	DSM_MEM_LEVEL_LTE_DL_FEW,
                                      	DSM_MEM_LEVEL_LTE_DL_MANY);

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_dup_pool_dl_event_cb );
      break;
    }
  case CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL:
    {
      /* large pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                              cfcm_dbg_ptr->large_pool_lte_dl_level1_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY,
                              cfcm_dbg_ptr->large_pool_lte_dl_level1_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                        DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                                        DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_dl_level1_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_dl_level1_event_cb );
      break;
    }
   case CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                              cfcm_dbg_ptr->dup_pool_lte_dl_level1_few);

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY,
                              cfcm_dbg_ptr->dup_pool_lte_dl_level1_many);

      dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                        DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                                        DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY);

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_LEVEL1_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_dl_level1_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_DL_LEVEL1_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_dup_pool_dl_level1_event_cb );
      break;
    }
    case CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL:
    {
      /* large pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_IPA_DL_FEW,
                              cfcm_dbg_ptr->large_pool_ipa_dl_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_IPA_DL_MANY,
                              cfcm_dbg_ptr->large_pool_ipa_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                      	DSM_MEM_LEVEL_IPA_DL_FEW,
                                      	DSM_MEM_LEVEL_IPA_DL_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_IPA_DL_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_ipa_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_IPA_DL_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_ipa_dl_event_cb );
      break;
    }
    case CFCM_MONITOR_DSM_LARGE_POOL_COMP:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE,
                              cfcm_dbg_ptr->large_pool_lte_pdcp_comp_dne);      

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                              cfcm_dbg_ptr->large_pool_lte_pdcp_comp_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY,
                              cfcm_dbg_ptr->large_pool_lte_pdcp_comp_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                      	DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                                      	DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_ul_comp_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_ul_comp_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_ul_comp_event_cb );
      break;
    }

    case CFCM_MONITOR_DSM_DUP_POOL_COMP:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE,
                              cfcm_dbg_ptr->dup_pool_lte_pdcp_comp_dne);      

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                              cfcm_dbg_ptr->dup_pool_lte_pdcp_comp_few);

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY,
                              cfcm_dbg_ptr->dup_pool_lte_pdcp_comp_many);

      dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                      	DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                                      	DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY);

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_ul_comp_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_ul_comp_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_LTE_PDCP_COMP_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_dup_pool_ul_comp_event_cb );
      break;
    }

    case CFCM_MONITOR_MAX:
    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_reg_lte_with_dsm_pool invalid monitor %d", monitor, 0, 0);
      break;
    }
  }
} /* cfcm_monitor_reg_lte_with_dsm_pool() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_rlc_with_dsm_pool

==============================================================================*/
/*!
    @brief
    Register RLC clients with DSM pools.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_reg_rlc_with_dsm_pool
(
  cfcm_monitor_e   monitor   /*!< client that triggered this registration */
)
{
  cfcm_dbg_s* cfcm_dbg_ptr;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  cfcm_dbg_ptr = cfcm_dbg_get_data();

  switch(monitor)
  {
    case CFCM_MONITOR_DSM_LARGE_POOL_DL:
    {
      /* large pool */
      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_LINK_LAYER_DNE,
                              cfcm_dbg_ptr->large_pool_rlc_dne);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_FEW,
                              cfcm_dbg_ptr->large_pool_rlc_dl_few);

      dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_MANY,
                              cfcm_dbg_ptr->large_pool_rlc_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_LARGE_ITEM_POOL,
                                      	DSM_MEM_LEVEL_RLC_FEW,
                                      	DSM_MEM_LEVEL_RLC_MANY);

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_LINK_LAYER_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_large_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_large_pool_dl_event_cb );
    }
    break;

    case CFCM_MONITOR_DSM_DUP_POOL_DL:
    {
      /* dup pool */
      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_FEW,
                              cfcm_dbg_ptr->dup_pool_rlc_dl_few);

      dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_MANY,
                              cfcm_dbg_ptr->dup_pool_rlc_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                      	DSM_MEM_LEVEL_RLC_FEW,
                                      	DSM_MEM_LEVEL_RLC_MANY);

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_dup_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DUP_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_dup_pool_dl_event_cb );
    }
    break;

    case CFCM_MONITOR_DSM_SMALL_POOL_DL:
    {
      /* small pool */
      dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                              DSM_MEM_LEVEL_LINK_LAYER_DNE,
                              cfcm_dbg_ptr->small_pool_rlc_dne);      

      dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_FEW,
                              cfcm_dbg_ptr->small_pool_rlc_dl_few);

      dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                              DSM_MEM_LEVEL_RLC_MANY,
                              cfcm_dbg_ptr->small_pool_rlc_dl_many);

      dsm_reg_mem_event_optimized_level(DSM_DS_SMALL_ITEM_POOL,
                                      	DSM_MEM_LEVEL_RLC_FEW,
                                      	DSM_MEM_LEVEL_RLC_MANY);

      dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_LINK_LAYER_DNE,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_small_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_FEW,
                           DSM_MEM_OP_NEW,
                           cfcm_monitor_small_pool_dl_event_cb );

      dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_RLC_MANY,
                           DSM_MEM_OP_FREE,
                           cfcm_monitor_small_pool_dl_event_cb );
    }
    break;

    case CFCM_MONITOR_MAX:
    default:
    {
      CFCM_ERR_FATAL("cfcm_monitor_reg_rlc_with_dsm_pool invalid monitor %d", monitor, 0, 0);
      break;
    }
  }
} /* cfcm_monitor_reg_rlc_with_dsm_pool() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_with_dsm_pool

==============================================================================*/
/*!
    @brief
    Register with DSM pools.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_reg_with_dsm_pool
(
  cfcm_monitor_e   monitor,   /*!< client that triggered this registration */
  cfcm_client_e client   /*!< the client id */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  switch(client)
  {
    /* If client is RLC, we have RLC specific DSM thresholds */
    case CFCM_CLIENT_WCDMA_RLC_DL:
    case CFCM_CLIENT_WCDMA_RLC_UL:
    {
      cfcm_monitor_reg_rlc_with_dsm_pool(monitor);
    }
    break;

    /* Else client is LTE, we have LTE specific DSM thresholds */
    default:
    {
      cfcm_monitor_reg_lte_with_dsm_pool(monitor);
    }
    break;
  }
} /* cfcm_monitor_reg_with_dsm_pool() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_with_thermal

==============================================================================*/
/*!
    @brief
    Register with thermal power amplifer (PA) monitor.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_reg_with_thermal( void )
{

  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_resource_available_cb(CFCM_MONITOR_THERMAL_PA_NPA_NODE,
                            cfcm_monitor_npa_available_cb,
                            NULL);
  CFCM_MSG_0(HIGH, "CFCM thermal monitor registers with NPA");
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_THERMAL_PA].registered 
    = TRUE;
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_THERMAL_PA_EX].registered 
    = TRUE;
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_THERMAL_PA_EM].registered 
    = TRUE;
} /* cfcm_monitor_reg_with_thermal() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_with_thermal_ra

==============================================================================*/
/*!
    @brief
    Register with thermal Runaway (RA) monitor.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_reg_with_thermal_ra( void )
{

  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_resource_available_cb(CFCM_MONITOR_THERMAL_RA_NPA_NODE,
                            cfcm_monitor_thermal_ra_available_cb,
                            NULL);
  CFCM_MSG_0(HIGH, "CFCM thermal Runaway monitor registers with NPA");
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_THERMAL_RUNAWAY].registered 
    = TRUE;
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_THERMAL_RUNAWAY_EX].registered 
    = TRUE;
} /* cfcm_monitor_reg_with_thermal_ra() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_with_mdm_temp

==============================================================================*/
/*!
    @brief
    Register with thermal MDM Temp monitor.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_reg_with_mdm_temp( void )
{

  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_resource_available_cb(CFCM_MONITOR_MDM_TEMP_NPA_NODE,
                            cfcm_monitor_mdm_temp_available_cb,
                            NULL);
  CFCM_MSG_0(HIGH, "CFCM thermal MDM Temp monitor registers with NPA");
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_MDM_TEMP].registered 
    = TRUE;
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_MDM_TEMP_EX].registered 
    = TRUE;
} /* cfcm_monitor_reg_with_mdm_temp() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_reg_with_vdd_peak_curr

==============================================================================*/
/*!
    @brief
    Register with VDD Peak Current monitor.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_reg_with_vdd_peak_curr( void )
{

  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  npa_resource_available_cb(CFCM_MONITOR_VDD_PEAK_CURR_NPA_NODE,
                            cfcm_monitor_vdd_peak_curr_available_cb,
                            NULL);
  CFCM_MSG_0(HIGH, "CFCM VDD Peak Current monitor registers with NPA");
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_VDD_PEAK_CURR_EST].registered 
    = TRUE;
  cfcm_monitor_ptr->monitors[(uint32)CFCM_MONITOR_VDD_PEAK_CURR_EST_EX].registered 
    = TRUE;
} /* cfcm_monitor_reg_with_vdd_peak_curr() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_register_with_src

==============================================================================*/
/*!
    @brief
    Register with the source monitor typcially via a callback function.

    @return
    None
*/
/*============================================================================*/
void cfcm_monitor_register_with_src
(
  cfcm_monitor_e  monitor, /*!< monitor needs to be registered with the source */
  cfcm_client_e client   /*!< the client id */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  if(cfcm_cfg_monitor_enabled(monitor))
  {
    switch(monitor)
    {
      case CFCM_MONITOR_DSM_LARGE_POOL_UL:
      case CFCM_MONITOR_DSM_LARGE_POOL_DL:
      case CFCM_MONITOR_DSM_DUP_POOL_UL:
      case CFCM_MONITOR_DSM_DUP_POOL_DL:
      case CFCM_MONITOR_DSM_SMALL_POOL_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL:
      case CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL:
      case CFCM_MONITOR_DSM_LARGE_POOL_COMP:
      case CFCM_MONITOR_DSM_DUP_POOL_COMP:
      {
        cfcm_monitor_reg_with_dsm_pool(monitor, client);
        break;
      }
      case CFCM_MONITOR_THERMAL_PA:
      case CFCM_MONITOR_THERMAL_PA_EX:
      case CFCM_MONITOR_THERMAL_PA_EM:
      {
        cfcm_monitor_reg_with_thermal();
        break;
      }
      case CFCM_MONITOR_THERMAL_RUNAWAY:
      case CFCM_MONITOR_THERMAL_RUNAWAY_EX:
      {
        cfcm_monitor_reg_with_thermal_ra();
        break;
      }
      case CFCM_MONITOR_CPU:
      {
        break;
      }
      case CFCM_MONITOR_BW_THROTTLING:
      {
        break;
      }
      case CFCM_MONITOR_MDM_TEMP:
      case CFCM_MONITOR_MDM_TEMP_EX:
      {
        cfcm_monitor_reg_with_mdm_temp();
        break;
      }
      case CFCM_MONITOR_VDD_PEAK_CURR_EST:
      case CFCM_MONITOR_VDD_PEAK_CURR_EST_EX:
      {
        cfcm_monitor_reg_with_vdd_peak_curr();
        break;
      }
      default:
      {
        CFCM_ERR_FATAL("cfcm_monitor_register_with_src invalid monitor %d", monitor, 0, 0);
        break;
      }
    }
    cfcm_monitor_ptr->monitors[(uint32)monitor].registered = TRUE;
  }
  else
  {
    CFCM_MSG_1(HIGH, "CFCM monintor =%d not enabled, skip registration",
              (uint32)monitor);
  }
} /* cfcm_monitor_register_with_src() */

/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_monitor_init

==============================================================================*/
/*!
    @brief
    initialized internal data structure at start-up.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_init( void )
{
  uint32 i;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  for(i = 0; i < (uint32)CFCM_MONITOR_MAX; i++)
  {
    cfcm_monitor_init_monitor((cfcm_monitor_e)i);
  }

  cfcm_monitor_bus_bw_init();

  cfcm_monitor_ptr->num_errors = 0;

} /* cfcm_monitor_init() */

/*=============================================================================

  FUNCTION:  cfcm_monitor_deinit

=============================================================================*/
/*!
    @brief
        De-Initializes CFCM QMI layer's functionalities
 
    @return
        None
*/
/*===========================================================================*/
void cfcm_monitor_deinit 
(
  void
)
{
  /* Turn off the service */
  mqcsi_conn_mgr_service_close(MQCSI_CFCM_SERVICE_ID);
}

/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_reg

==============================================================================*/
/*!
    @brief
    update monitor internals info in response to client registration.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_proc_reg
(
  const cfcm_reg_req_type_s*     msg_ptr/*!< registration message */
)
{
  uint32 i;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  for(i = 0; i < (uint32)CFCM_MONITOR_MAX; i++)
  {
    if(CFCM_MONITOR_IN_MASK(msg_ptr->monitor_mask, i))
    {
      if(cfcm_monitor_ptr->monitors[i].num_client == 0)
      {
        /* first time monitor is set up */
        cfcm_monitor_ptr->monitors[i].num_client++;
        cfcm_monitor_ptr->monitors[i].client[(uint32)msg_ptr->client_id] = 
          msg_ptr->client_id;

        if(!cfcm_monitor_ptr->monitors[i].registered)
        {
          cfcm_monitor_register_with_src(
            cfcm_monitor_ptr->monitors[i].monitor_id, msg_ptr->client_id);
        }
      }
      else
      {
        cfcm_monitor_ptr->monitors[i].num_client++;
        cfcm_monitor_ptr->monitors[i].client[(uint32)msg_ptr->client_id] = 
          msg_ptr->client_id;
      }
    }
  }
} /* cfcm_monitor_proc_reg() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_dereg

==============================================================================*/
/*!
    @brief
    update the internal info in response to client deregistration message.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_proc_dereg
(
  const cfcm_dereg_req_type_s*     msg_ptr,/*!< deregistration message */
  uint32                         monitor_mask /*!< monitor mask */
)
{
  uint32 i;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  for(i = 0; i < (uint32)CFCM_MONITOR_MAX; i++)
  {
    if(CFCM_MONITOR_IN_MASK(monitor_mask, i))
    {
      if(cfcm_monitor_ptr->monitors[i].client[(uint32)msg_ptr->client_id] == 
         msg_ptr->client_id)
      {
        CFCM_ASSERT(cfcm_monitor_ptr->monitors[i].num_client > 0);
        cfcm_monitor_ptr->monitors[i].num_client--;
        cfcm_monitor_ptr->monitors[i].client[(uint32)msg_ptr->client_id] = 
          CFCM_CLIENT_MAX;
      }
      else
      {
        CFCM_MSG_1(ERROR, "Deregistration for a client=%d already deregisted: Ignored",
                  (uint32)msg_ptr->client_id);
        cfcm_monitor_ptr->num_errors++;
      }
    }
  }
} /* cfcm_monitor_proc_dereg() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_compute_fc_cmd

==============================================================================*/
/*!
    @brief
    compute the flow control command based on the state of monitors specified.

    @detail
    for a given set of monitors a client is designed to respond to, 
    calculate the correct flow control command, and for DOWN command, also
    calculate the step timer.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_compute_fc_cmd
(
  cfcm_cmd_type_s* fc_cmd_ptr, /*!<pointer to the flow control cmd result output */
  uint32        monitor_mask,   /*!< the set of monitors to be considered */
  cfcm_client_e client   /*!< the client id */
)
{
  uint32 i;
  uint32 tmp_cmd;
  uint32 tmp_monitor_active = 0;
  /*--------------------------------------------------------------------------*/

  CFCM_ASSERT(fc_cmd_ptr != NULL);

  /*--------------------------------------------------------------------------*/
  tmp_cmd = (uint32)CFCM_CMD_FC_OFF;
  fc_cmd_ptr->step_timer = CFCM_MONITOR_DEFAULT_STEP_TIMER;

  for(i = 0; i < (uint32)CFCM_MONITOR_MAX; i++)
  {
    if(CFCM_MONITOR_IN_MASK(monitor_mask, i))
    {
      if (tmp_cmd == (uint32)cfcm_monitor_ptr->monitors[i].last_cmd[client].cmd)
      {
        tmp_monitor_active |= cfcm_monitor_ptr->monitors[i].last_cmd[client].monitors_active;
      }
	  else if (tmp_cmd < (uint32)cfcm_monitor_ptr->monitors[i].last_cmd[client].cmd)
      {
        /* find the common denominator, so to speak */
        tmp_cmd |= (uint32)cfcm_monitor_ptr->monitors[i].last_cmd[client].cmd;
        tmp_monitor_active = cfcm_monitor_ptr->monitors[i].last_cmd[client].monitors_active;
      }
    }
  }
  fc_cmd_ptr->cmd = (cfcm_cmd_e)tmp_cmd;
  fc_cmd_ptr->monitors_active = tmp_monitor_active;

  if(fc_cmd_ptr->cmd == CFCM_CMD_DOWN)
  {
    /* set the step timer to the minimum of step timer for monitors that voted
       to go down */
    for(i = 0; i < (uint32)CFCM_MONITOR_MAX; i++)
    {
      if((CFCM_MONITOR_IN_MASK(monitor_mask, i)) &&
         (cfcm_monitor_ptr->monitors[i].last_cmd[client].cmd == CFCM_CMD_DOWN))
      {
        /* take the minimum */
        if(fc_cmd_ptr->step_timer > 
           cfcm_monitor_ptr->monitors[i].last_cmd[client].step_timer)
        {
          fc_cmd_ptr->step_timer = 
            cfcm_monitor_ptr->monitors[i].last_cmd[client].step_timer;
        }
      }
    }
  }
  /*for the case (fc_cmd_ptr->cmd == CFCM_CMD_UP), step timer is configured
    by the cfcm_client */
} /* cfcm_monitor_compute_fc_cmd() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_update_cfcm_cmd

==============================================================================*/
/*!
    @brief
    The function calculates final command for the client if it has register for it and 
    any change in the command due to the triggered monitor.

    @detail
    The function Updates Last Command details for each client for CPU monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_update_cfcm_cmd
(
  cfcm_client_e           client_id,
  cfcm_monitor_e          monitor_id,  /*!< monitor id */
  boolean*                qsh_invoked
)
{
  cfcm_monitor_info_s* monitor_ptr;
  cfcm_client_dbg_s*   client_dbg_ptr;
  
 uint32 curr_time, last_fc_time, curr_fc_period, max_fc_period;
  
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(CFCM_IS_CLIENT_VALID(client_id));
  CFCM_ASSERT(qsh_invoked != NULL);
  /*--------------------------------------------------------------------------*/

  monitor_ptr = &cfcm_monitor_ptr->monitors[monitor_id];
  client_dbg_ptr  = &cfcm_client.clients[client_id].stats.monitor[monitor_id];

  /* Calculate Client's final command if client is registered for this monitor and 
        command is updated for the client due to this monitor */
  if((monitor_ptr->client[client_id] != CFCM_CLIENT_MAX) &&
    (monitor_ptr->last_cmd[client_id].cmd_updated))
  {
    monitor_ptr->latest_cmd_hist_idx++;
    monitor_ptr->latest_cmd_hist_idx &= CFCM_MONITOR_HIST_BUF_IDX_MASK;
    monitor_ptr->cmd_hist[monitor_ptr->latest_cmd_hist_idx] = monitor_ptr->last_cmd[client_id].cmd;
    monitor_ptr->client_hist[monitor_ptr->latest_cmd_hist_idx] = client_id;
    
    monitor_ptr->stats.num_state_change++;
    if (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_DOWN)
    {
      monitor_ptr->stats.num_down ++;
      monitor_ptr->stats.time_down = cfcm_timetick_get_ms();
      if(client_dbg_ptr->fc_started == FALSE)
      {
        client_dbg_ptr->last_fc_start_time = cfcm_timetick_get_ms();
        client_dbg_ptr->fc_started = TRUE;
      }
    }
    else if (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_SET_MIN)
    {
      monitor_ptr->stats.num_set_min ++;
      monitor_ptr->stats.time_set_min = cfcm_timetick_get_ms();
      if(client_dbg_ptr->fc_started == FALSE)
      {
        client_dbg_ptr->last_fc_start_time = cfcm_timetick_get_ms();
        client_dbg_ptr->fc_started = TRUE;
      }
    }
    else if (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_SHUT_DOWN)
    {
      monitor_ptr->stats.num_shut_down ++;
      monitor_ptr->stats.time_shut_down = cfcm_timetick_get_ms();
      if(client_dbg_ptr->fc_started == FALSE)
      {
        client_dbg_ptr->last_fc_start_time = cfcm_timetick_get_ms();
        client_dbg_ptr->fc_started = TRUE;
      }
    }
    else if (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_FREEZE)
    {
      if(client_dbg_ptr->fc_started == FALSE)
      {
        client_dbg_ptr->last_fc_start_time = cfcm_timetick_get_ms();
        client_dbg_ptr->fc_started = TRUE;
      };
    }
    else if ((monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_UP) ||
             (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_FC_OFF))
    {
      /* If the flow control is set to up then calculate how long fc was 
         enabled for and the total fc time per client per monitor */
      curr_time = cfcm_timetick_get_ms();
      last_fc_time = client_dbg_ptr->last_fc_start_time;
      
      /* check if the time got reset to avoid overflow 
         and that UP command is being sent inreponse to previous flow control */
      if((curr_time > last_fc_time) &&
         (client_dbg_ptr->fc_started == TRUE))
      {
        curr_fc_period = curr_time - last_fc_time;
        max_fc_period = client_dbg_ptr->max_fc_period;
        client_dbg_ptr->max_fc_period = (uint32)MAX(curr_fc_period,max_fc_period);
        client_dbg_ptr->total_fc_period += curr_fc_period;
      }
      else
      {
        /* If the time got reset then reset the last fc time
           and do not count this interval towards the stats */
        client_dbg_ptr->last_fc_start_time = 0;
      }
      
      client_dbg_ptr->fc_started = FALSE;
    }
  
    if(cfcm_cfg_monitor_enabled(monitor_id))
      {
      if ((*qsh_invoked == FALSE) && (monitor_ptr->last_cmd[client_id].cmd == CFCM_CMD_SHUT_DOWN) && 
        CFCM_IS_DSM_MONITOR(monitor_id))
      {
        qsh_invoke_params_s params;
        
        if (CFCM_IS_DSM_LARGE_POOL_MONITOR(monitor_id))
        {
          QSH_LOG(
            CFCM_CLT_QSH,
            QSH_CAT_DSM,
            QSH_MSG_TYPE_HIGH,
            "Invoking QSH analysis: DSM large pool running low, free count <= lte (%d) rlc (%d), client %d",
            CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_DNE_MARK, 
            CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_DNE_MARK, client_id);
        }
        else if (CFCM_IS_DSM_DUP_POOL_MONITOR(monitor_id))
        {
          QSH_LOG(
            CFCM_CLT_QSH,
            QSH_CAT_DSM,
            QSH_MSG_TYPE_HIGH,
            "Invoking QSH analysis: DSM dup pool running low, free count <= lte (%d) rlc (%d), client %d",
            CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_DNE_MARK, 
            CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_DNE_MARK, client_id);
        }
        else if (CFCM_IS_DSM_SMALL_POOL_MONITOR(monitor_id))
        {
          QSH_LOG(
            CFCM_CLT_QSH,
            QSH_CAT_DSM,
            QSH_MSG_TYPE_HIGH,
            "Invoking QSH analysis: DSM Small pool running low, free count <= rlc (%d), client %d",
            CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_DNE_MARK, client_id);
        }

        /* if monitor is DSM and cmd is shutdown, invoke QSH analysis for DSM */
        params.client = QSH_CLT_ALL;
        params.action = QSH_ACTION_ANALYSIS;
        params.category_mask = QSH_CAT_DSM | QSH_CAT_TPUT;
        params.async_allowed = TRUE;
        
        qsh_invoke_cb(&params);
        
        *qsh_invoked = TRUE;
      }
  
      cfcm_client_update_fc_cmd(monitor_ptr->client[client_id]);
    }
    else
    {
      CFCM_MSG_3(ERROR,
              "CFCM unproc monitor state change monitor=%d, cmd=%d, step_timer=%d",
              (uint32)monitor_id,
              (uint32)monitor_ptr->last_cmd[client_id].cmd,
              monitor_ptr->last_cmd[client_id].step_timer);
    }
  }
} /* cfcm_monitor_update_cfcm_cmd() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_update_last_cmd

==============================================================================*/
/*!
    @brief
    The function Updates Last Command details for each client for the monitor.

    @detail
    It iterates over all clients that are designed to respond to the monitor and
    triggers flow control command recalculation for them.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_update_last_cmd
(
  cfcm_monitor_e           monitor_id,  /*!< monitor id */
  cfcm_cmd_e*              cmd /*!<command corresponding to the new monitor state */
)
{
  cfcm_monitor_info_s* monitor_ptr;
  uint32              i;
  boolean             qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/


  /*--------------------------------------------------------------------------*/

  monitor_ptr = &cfcm_monitor_ptr->monitors[monitor_id];

  /* Update the monitor's Last cmd details */
  for(i = 0; i < (uint32)CFCM_CLIENT_MAX; i++)
  {
    monitor_ptr->last_cmd[i].cmd_updated = FALSE;
    if((cmd[i] == CFCM_CMD_INVALID) || (monitor_ptr->last_cmd[i].cmd == cmd[i]) ||
       CFCM_MONITOR_IS_IGNORE_NEW_CMD(monitor_ptr->last_cmd[i].cmd, cmd[i]))
    {
      continue;
    }

    /* Update the monitor's Last cmd details */
    monitor_ptr->last_cmd[i].cmd_updated = TRUE;
    monitor_ptr->last_cmd[i].cmd = cmd[i];
    monitor_ptr->last_cmd[i].monitors_active = CFCM_CONV_ENUM_TO_BIT_MASK(monitor_id);
    monitor_ptr->last_cmd[i].step_timer = cfcm_monitor_get_clients_step_timer_value((cfcm_client_e)i, monitor_id);

    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)i, monitor_id, &qsh_invoked);
  }
} /* cfcm_monitor_update_last_cmd() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_update_last_cmd_per_client

==============================================================================*/
/*!
    @brief
    The function Updates Last Command details for a client within requested monitor.

    @detail
    The function Updates Last Command details for a client within requested monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_update_last_cmd_per_client
(
  cfcm_client_e           client_id,
  cfcm_monitor_e          monitor_id,  /*!< monitor id */
  cfcm_cmd_e              cmd /*!<command corresponding to the new monitor state */
)
    {
  cfcm_monitor_info_s* monitor_ptr;
  /*--------------------------------------------------------------------------*/
  CFCM_ASSERT(CFCM_IS_CLIENT_VALID(client_id));
  /*--------------------------------------------------------------------------*/
    
  monitor_ptr = &cfcm_monitor_ptr->monitors[monitor_id];
  monitor_ptr->last_cmd[client_id].cmd_updated = FALSE;

  /* Update the monitor's Last cmd details */
  if((cmd == CFCM_CMD_INVALID) || (monitor_ptr->last_cmd[client_id].cmd == cmd) ||
     CFCM_MONITOR_IS_IGNORE_NEW_CMD(monitor_ptr->last_cmd[client_id].cmd, cmd))
  {
    return;
  }
  
  /* Update the monitor's Last cmd details */
  monitor_ptr->last_cmd[client_id].cmd_updated = TRUE;
  monitor_ptr->last_cmd[client_id].cmd = cmd;
  monitor_ptr->last_cmd[client_id].monitors_active = CFCM_CONV_ENUM_TO_BIT_MASK(monitor_id);
  monitor_ptr->last_cmd[client_id].step_timer = cfcm_monitor_get_clients_step_timer_value(client_id, monitor_id);
} /* cfcm_monitor_update_last_cmd_per_client() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_process_cpu_load_input

==============================================================================*/
/*!
    @brief
    The function Updates Last Command details for each client for CPU monitor.

    @detail
    The function Updates Last Command details for each client for CPU monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_process_cpu_load_input
(
  cfcm_cpu_load_type_s           cpu_info  /*!< cpu_load */
)
{
  cfcm_monitor_info_s* monitor_ptr;
  uint32              i;
  cfcm_cmd_e           cmd;
  boolean             qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/


  /*--------------------------------------------------------------------------*/

  monitor_ptr = &cfcm_monitor_ptr->monitors[CFCM_MONITOR_CPU];

  /* Call cpu monitor API to update to store load */
  cfcm_cpu_monitor_process_cpu_info(cpu_info);
  
  /* Update the monitor's Last cmd details */
  for(i = 0; i < (uint32)CFCM_CLIENT_MAX; i++)
  {
    /* Start with Last command for client */
    cmd = monitor_ptr->last_cmd[i].cmd;
    monitor_ptr->last_cmd[i].cmd_updated = FALSE;
    if (cfcm_cpu_monitor_process_load_percentage((cfcm_client_e)i, cpu_info.cpu_load, &cmd))
    {
      cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)i, CFCM_MONITOR_CPU, cmd);
    }
  
    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)i, CFCM_MONITOR_CPU, &qsh_invoked);
  }
} /* cfcm_monitor_process_cpu_load_input() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_process_thermal_pa

==============================================================================*/
/*!
    @brief
    handles NPA state change for thermal PA monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_process_thermal_pa
(
  uint32 state   /*!< new NPA state  */
)
      {
  uint8 index = 0;
  cfcm_cmd_e              pa_cmd;
  cfcm_cmd_e              pa_ex_cmd;
  cfcm_cmd_e              pa_emergency_cmd = CFCM_CMD_FREEZE;
  boolean                 qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/
  if (state >= CFCM_MONITOR_MAX_THERMAL_PA_STATE)
    {
      CFCM_MSG_1(ERROR, "CFCM thermal PA monitor state %d INVALID ", state);
      return;
    }
  /*--------------------------------------------------------------------------*/

  pa_cmd = cfcm_thermal_pa_state_map[state][0];
  pa_ex_cmd = cfcm_thermal_pa_state_map[state][1];
  pa_emergency_cmd = cfcm_thermal_pa_state_map[state][2];

  for(index = 0; index < CFCM_CLIENT_MAX; index++)
  {
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA, pa_cmd);
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA_EX, pa_ex_cmd);
    if(pa_emergency_cmd != CFCM_CMD_FREEZE)
    {
      cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA_EM, pa_emergency_cmd);
    }
  
    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA, &qsh_invoked);
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA_EX, &qsh_invoked);
    if(pa_emergency_cmd != CFCM_CMD_FREEZE)
    {
      cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_THERMAL_PA_EM, &qsh_invoked);
    }
  }
} /* cfcm_monitor_process_thermal_pa() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_process_thermal_ra

==============================================================================*/
/*!
    @brief
    handles NPA state change for thermal Runaway monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_process_thermal_ra
(
  uint32 state   /*!< new NPA state  */
)
{
  uint8 index = 0;
  cfcm_cmd_e              ra_cmd;
  cfcm_cmd_e              ra_ex_cmd;
  boolean                 qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/
  if (state >= CFCM_MONITOR_MAX_NPA_NODE_STATE)
  {
    CFCM_MSG_1(ERROR, "CFCM thermal Runaway monitor state %d INVALID ", state);
    return;
  }
  /*--------------------------------------------------------------------------*/

  ra_cmd = cfcm_npa_node_state_map[state][0];
  ra_ex_cmd = cfcm_npa_node_state_map[state][1];

  for(index = 0; index < CFCM_CLIENT_MAX; index++)
  {
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_THERMAL_RUNAWAY, ra_cmd);
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_THERMAL_RUNAWAY_EX, ra_ex_cmd);

    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_THERMAL_RUNAWAY, &qsh_invoked);
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_THERMAL_RUNAWAY_EX, &qsh_invoked);
  }
} /* cfcm_monitor_process_thermal_ra() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_process_mdm_temp

==============================================================================*/
/*!
    @brief
    handles NPA state change for MDM Temp monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_process_mdm_temp
(
  uint32 state   /*!< new NPA state  */
)
{
  uint8 index = 0;
  cfcm_cmd_e              mdm_cmd;
  cfcm_cmd_e              mdm_ex_cmd;
  boolean                 qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/
  if (state >= CFCM_MONITOR_MAX_NPA_NODE_STATE)
  {
    CFCM_MSG_1(ERROR, "CFCM MDM Temp monitor state %d INVALID ", state);
    return;
  }
  /*--------------------------------------------------------------------------*/

  mdm_cmd = cfcm_npa_node_state_map[state][0];
  mdm_ex_cmd = cfcm_npa_node_state_map[state][1];

  for(index = 0; index < CFCM_CLIENT_MAX; index++)
  {
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_MDM_TEMP, mdm_cmd);
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_MDM_TEMP_EX, mdm_ex_cmd);

    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_MDM_TEMP, &qsh_invoked);
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_MDM_TEMP_EX, &qsh_invoked);
  }
} /* cfcm_monitor_process_mdm_temp() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_process_vdd_peak_curr

==============================================================================*/
/*!
    @brief
    handles NPA state change for VDD Peak Curr monitor.

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_process_vdd_peak_curr
(
  uint32 state   /*!< new NPA state  */
)
{
  uint8 index = 0;
  cfcm_cmd_e              vdd_cmd;
  cfcm_cmd_e              vdd_ex_cmd;
  boolean                 qsh_invoked = FALSE;
  /*--------------------------------------------------------------------------*/
  if (state >= CFCM_MONITOR_MAX_NPA_NODE_STATE)
  {
    CFCM_MSG_1(ERROR, "CFCM VDD Peak Curr monitor state %d INVALID ", state);
    return;
  }
  /*--------------------------------------------------------------------------*/

  vdd_cmd = cfcm_npa_node_state_map[state][0];
  vdd_ex_cmd = cfcm_npa_node_state_map[state][1];

  for(index = 0; index < CFCM_CLIENT_MAX; index++)
  {
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_VDD_PEAK_CURR_EST, vdd_cmd);
    cfcm_monitor_update_last_cmd_per_client((cfcm_client_e)index, CFCM_MONITOR_VDD_PEAK_CURR_EST_EX, vdd_ex_cmd);

    /* Run the loop for the client due to this monitor change */
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_VDD_PEAK_CURR_EST, &qsh_invoked);
    cfcm_monitor_update_cfcm_cmd((cfcm_client_e)index, CFCM_MONITOR_VDD_PEAK_CURR_EST_EX, &qsh_invoked);
  }
} /* cfcm_monitor_process_vdd_peak_curr() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_update_log_packet_data

==============================================================================*/
/*!
    @brief
    This maps and logs monitor data for which commands is sent to CFCM task 

    @return
    None
*/
/*============================================================================*/
static void cfcm_monitor_update_log_packet_data
(
  cfcm_monitor_e monitor_id,
  cfcm_log_monitor_type* monitor_data,
  cfcm_monitor_cmds_log_data    log_data
)
{
  /*--------------------------------------------------------------------------*/
  if (monitor_data == NULL)
  {
    CFCM_MSG_1(ERROR, "Monitor Data NULL for monitor_id %d ", monitor_id);
    return;
  }
  /*--------------------------------------------------------------------------*/

  switch (monitor_id)
  {
    case CFCM_MONITOR_DSM_LARGE_POOL_UL:
    case CFCM_MONITOR_DSM_LARGE_POOL_DL:
    case CFCM_MONITOR_DSM_DUP_POOL_UL:
    case CFCM_MONITOR_DSM_DUP_POOL_DL:
    case CFCM_MONITOR_DSM_SMALL_POOL_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_IPA_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_LEVEL1_DL:
    case CFCM_MONITOR_DSM_DUP_POOL_LEVEL1_DL:
    case CFCM_MONITOR_DSM_LARGE_POOL_COMP:
    case CFCM_MONITOR_DSM_DUP_POOL_COMP:
    {
      monitor_data->dsm.pool_id = log_data.dsm.pool_id;
      monitor_data->dsm.event = log_data.dsm.event;
      monitor_data->dsm.op = log_data.dsm.op;
      break;
    }
    case CFCM_MONITOR_BW_THROTTLING:
    {
      monitor_data->bw.level = log_data.bw.level;
      break;
    }
    default:
    {
      CFCM_MSG_1(ERROR, "cfcm_monitor_update_log_packet_data Invalid monitor ID %d ",
                  monitor_id);
      return;
    }
  }
} /* cfcm_monitor_process_thermal_ra() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_proc_update

==============================================================================*/
/*!
    @brief
    The function processes the monitor state change indication.

    @detail
    It iterates over all clients that are designed to respond to the monitor and
    triggers flow control command recalculation for them.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_proc_update
(
  cfcm_monitor_ind_msg_s*  msg_ptr/*!< monitor state change indication message */
)
{
  uint32        hist_idx[CFCM_CLIENT_MAX];
  cfcm_log_monitor_type log_monitor;
  cfcm_monitor_e           monitor_id;

  /*--------------------------------------------------------------------------*/

  CFCM_ASSERT(msg_ptr != NULL);
  CFCM_ASSERT(CFCM_IS_MONITOR_DATA_VALID(msg_ptr->data_info));

  /*--------------------------------------------------------------------------*/

  cfcm_client_get_current_history_idxs(hist_idx);
  
  switch (msg_ptr->data_info)
  {
    case CFCM_MONITOR_CMDS_UPDATED:
    {
      /* Command is already know so directly update last command details */
      cfcm_monitor_update_last_cmd(msg_ptr->monitor_data.monitor_cmd_info.monitor,
                                   msg_ptr->monitor_data.monitor_cmd_info.cmd);

      monitor_id = msg_ptr->monitor_data.monitor_cmd_info.monitor;
      cfcm_monitor_update_log_packet_data(monitor_id,
                                   &log_monitor, msg_ptr->monitor_data.monitor_cmd_info.log_data);
      break;
    }
    case CFCM_MONITOR_CPU_INPUT:
    {
      /* For CPU command is not updated 
                so calculate command and later update monitor last cmd data */
      cfcm_monitor_process_cpu_load_input(msg_ptr->monitor_data.cpu_info);
      monitor_id = CFCM_MONITOR_CPU;
      log_monitor.cpu.load = msg_ptr->monitor_data.cpu_info.cpu_load;
      log_monitor.cpu.curr_cpu_freq = msg_ptr->monitor_data.cpu_info.curr_cpu_freq;
      log_monitor.cpu.max_cpu_freq = msg_ptr->monitor_data.cpu_info.max_cpu_freq;
      break;
    }
    case CFCM_MONITOR_THERMAL_PA_INPUT:
    {
      /* For Thermal PA, command is not updated 
                so calculate command and later update monitor last cmd data */
      cfcm_monitor_process_thermal_pa(msg_ptr->monitor_data.npa_node.state);
      monitor_id = CFCM_MONITOR_THERMAL_PA;
      log_monitor.npa_node.state = msg_ptr->monitor_data.npa_node.state;
      break;
    }
    case CFCM_MONITOR_THERMAL_RA_INPUT:
    {
      /* For Thermal Runaway, command is not updated 
                so calculate command and later update monitor last cmd data */
      cfcm_monitor_process_thermal_ra(msg_ptr->monitor_data.npa_node.state);
      monitor_id = CFCM_MONITOR_THERMAL_RUNAWAY;
      log_monitor.npa_node.state = msg_ptr->monitor_data.npa_node.state;
      break;
    }
    case CFCM_MONITOR_MDM_TEMP_INPUT:
    {
      /* For MDM Temp, command is not updated 
                so calculate command and later update monitor last cmd data */
      cfcm_monitor_process_mdm_temp(msg_ptr->monitor_data.npa_node.state);
      monitor_id = CFCM_MONITOR_MDM_TEMP;
      log_monitor.npa_node.state = msg_ptr->monitor_data.npa_node.state;
      break;
    }
    case CFCM_MONITOR_VDD_PEAK_CURR_INPUT:
    {
      /* For VDD Peak Current, command is not updated 
                so calculate command and later update monitor last cmd data */
      cfcm_monitor_process_vdd_peak_curr(msg_ptr->monitor_data.npa_node.state);
      monitor_id = CFCM_MONITOR_VDD_PEAK_CURR_EST;
      log_monitor.npa_node.state = msg_ptr->monitor_data.npa_node.state;
      break;
    }
    default:
    {
      CFCM_MSG_1(ERROR, "cfcm_monitor_proc_update Invalid data info %d ",
                  msg_ptr->data_info);
      return;
    }
  }

  cfcm_log_monitor_input(monitor_id, &log_monitor);
  cfcm_log_command_output(hist_idx, monitor_id, &log_monitor);
} /* cfcm_monitor_proc_update() */


/*==============================================================================

  FUNCTION:  cfcm_monitor_registered

==============================================================================*/
/*!
    @brief
    returns whether or not the specified monitor is registered with the source.

    @return
    TRUE if the monitor is registered. FALSE, otherwise.
*/
/*============================================================================*/
EXTERN boolean cfcm_monitor_registered
(
  cfcm_monitor_e monitor  /*!< REQUIRED short parameter description */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return cfcm_monitor_ptr->monitors[(uint32)monitor].registered;
} /* cfcm_monitor_registered() */

/*==============================================================================

  FUNCTION:  cfcm_monitor_get_cmd_string

==============================================================================*/
/*!
    @brief
    Returns the string associated with a flow control command.
*/
/*============================================================================*/
EXTERN const char * cfcm_monitor_get_cmd_string
(
  cfcm_cmd_e cmd 
)
{
  const char * cmd_str = cfcm_cmd_str[CFCM_CMD_MAX-1];
  uint32 cmd_id = (uint32)cmd;
  uint32 idx = 0;

  while(cmd_id & 0x1)
  {
    idx += 1;
    cmd_id = (cmd_id >> 1);
  }

  if(idx < CFCM_CMD_MAX)
  {
    cmd_str = cfcm_cmd_str[idx];
  }

  return cmd_str;
  
}

/*==============================================================================

  FUNCTION:  cfcm_monitor_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
EXTERN void cfcm_monitor_qsh_analysis
(
  void
)
{
  unsigned int i;
  uint32 idx;
  const char * cmd_str;

  /* iterate through monitors */
  for (i = 0; i < CFCM_MONITOR_MAX; i ++)
  {
    idx = cfcm_monitor_ptr->monitors[i].latest_cmd_hist_idx;
    cmd_str = cfcm_monitor_get_cmd_string(cfcm_monitor_ptr->monitors[i].cmd_hist[idx]);
    /* If shutdown was triggered then use HIGH message.
       Otherwise use medium message.
       High messages are used by QSherlock to detect problem areas 
    */
    if(cfcm_monitor_ptr->monitors[i].stats.num_shut_down > 0)
    {
      QSH_CHECK_LOG(
        cfcm_monitor_ptr->monitors[i].stats.num_down != 0 ||
        cfcm_monitor_ptr->monitors[i].stats.num_set_min != 0 ||
        cfcm_monitor_ptr->monitors[i].stats.num_shut_down != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_HIGH,
        "%s: Down=%d; SetMin=%d; Shutdown=%d; CurrState: %s",
        cfcm_monitor_str[i],
        cfcm_monitor_ptr->monitors[i].stats.num_down,
        cfcm_monitor_ptr->monitors[i].stats.num_set_min,
        cfcm_monitor_ptr->monitors[i].stats.num_shut_down,
        cmd_str);
    }
    else
    {
      QSH_CHECK_LOG(
        cfcm_monitor_ptr->monitors[i].stats.num_down != 0 ||
        cfcm_monitor_ptr->monitors[i].stats.num_set_min != 0 ||
        cfcm_monitor_ptr->monitors[i].stats.num_shut_down != 0,
        CFCM_CLT_QSH,
        QSH_CAT_TPUT,
        QSH_MSG_TYPE_MEDIUM,
        "%s: Down=%d; SetMin=%d; Shutdown=%d; CurrState: %s",
        cfcm_monitor_str[i],
        cfcm_monitor_ptr->monitors[i].stats.num_down,
        cfcm_monitor_ptr->monitors[i].stats.num_set_min,
        cfcm_monitor_ptr->monitors[i].stats.num_shut_down,
        cmd_str);
    }
  }
}

/*==============================================================================

  FUNCTION:  cfcm_monitor_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
EXTERN void cfcm_monitor_qsh_reset
(
  void
)
{
  unsigned int i;
  
  for (i = 0; i < CFCM_MONITOR_MAX; i ++)
  {
    memset(
      &cfcm_monitor_ptr->monitors[i].stats, 
      0, 
      sizeof(cfcm_monitor_stats_s));
  }
}

/*==============================================================================

                                UNIT TEST

==============================================================================*/
#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
#error code not present
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */


static void cfcm_test_dsm_rlc_monitor_via_diag
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
} /* cfcm_test_dsm_rlc_monitor_via_diag() */

static void cfcm_test_dsm_lte_monitor_via_diag
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
} /* cfcm_test_dsm_lte_monitor_via_diag() */


void cfcm_test_thermal_monitor_via_diag
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


void cfcm_test_dsm_monitor_via_diag
(
  uint8 input 
)
{
#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#else
  cfcm_test_dsm_rlc_monitor_via_diag(input);
  cfcm_test_dsm_lte_monitor_via_diag(input);
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
  return;
} /* cfcm_test_dsm_monitor_via_diag() */

