/*!
  @file
  cfcm_msg_internal.h

  @brief
  Internal CFCM related UMIDs.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_msg_internal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/21/14   rj      Adding MDM_TEMP and VDD_PEAK_CURR monitors
11/20/14   rj      Log Packet support added in CFCM
10/17/14   rj      Added supoort for Thermal RunAway Monitor
09/24/14   rj      initial version
==============================================================================*/

#ifndef CFCM_MSG_INTERNAL_H
#define CFCM_MSG_INTERNAL_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "cfcm.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define CFCM_IS_MONITOR_DATA_VALID(data) (((data) > CFCM_MONITOR_INVALID) && ((data) < CFCM_MONITOR_DATA_MAX))

/*! @brief Inputs which will be sent as part of MCS_CFCM_MONITOR_IND enumeration
*/
typedef enum
{
  CFCM_MONITOR_INVALID = -1,         /* Monitor data invalid */
  CFCM_MONITOR_CMDS_UPDATED,         /* Monitor with commands udpated */
  CFCM_MONITOR_CPU_INPUT,            /* CPU load % Data */
  CFCM_MONITOR_THERMAL_PA_INPUT,     /* Thermal PA monitor Data */
  CFCM_MONITOR_THERMAL_RA_INPUT,     /* Thermal RunAway monitor Data */
  CFCM_MONITOR_MDM_TEMP_INPUT,       /* MDM Temperature monitor Data */
  CFCM_MONITOR_VDD_PEAK_CURR_INPUT,  /* VDD Peak Current monitor Data */
  CFCM_MONITOR_DATA_MAX              /*!<  MAX */
} cfcm_monitor_data_e;

/*! @brief monitor state change indication message format
*/
typedef struct
{
  uint32           cpu_load;      /*!< cpu_load */
  uint32           max_cpu_freq;  /*!< MAX CPU Freq in KHz */
  uint32           curr_cpu_freq; /*!< Current CPU Freq in KHz */
} cfcm_cpu_load_type_s;

typedef struct
{
  uint32           state;  /*!< pa_state */
} cfcm_npa_node_type_s;

typedef struct
{
  uint8           level;  /*!< Thermal RunAway state */
} cfcm_bw_monitor_type_s;

typedef struct
{
  int     pool_id; /*!< pool triggered the event */
  int     event;   /*!< level reached */
  int     op;       /*!< operation that triggered the event */
} cfcm_dsm_monitor_type_s;

/*----------------------------------------------------------------------------
  Union of monitors data for which commands is sent to CFCM task
----------------------------------------------------------------------------*/
typedef union
{
  /* DSM monitor Data */
  cfcm_dsm_monitor_type_s       dsm;

  /* Bus Bandwidth monitor Data */
  cfcm_bw_monitor_type_s        bw;
} cfcm_monitor_cmds_log_data;

typedef struct  
{
  cfcm_monitor_e           monitor;  /*!< monitor id */
  cfcm_cmd_e               cmd[CFCM_CLIENT_MAX]; /*!<command corresponding to the new monitor state */
  cfcm_monitor_cmds_log_data    log_data;
} cfcm_monitor_cmds_type_s;

/*----------------------------------------------------------------------------
  Union for all monitor data type
----------------------------------------------------------------------------*/
typedef union
{
  /* Monitor with commands udpated - Currently used by DSM monitor */
  cfcm_monitor_cmds_type_s    monitor_cmd_info;

  /* CPU load % Data */
  cfcm_cpu_load_type_s        cpu_info;

  /* Thermal PA/RunAway/MDM Temp/VDD Peak Curr monitor Data */
  cfcm_npa_node_type_s        npa_node;

} cfcm_monitor_data;

/*! @brief monitor state change indication message format
*/
typedef struct
{
  /*!< standard msgr header */
  msgr_hdr_struct_type     hdr;

  /* What Monitor data (commands/monitor-input) sent with this indication */
  cfcm_monitor_data_e      data_info;

  /* Union of monitor data sent */
  cfcm_monitor_data        monitor_data;
} cfcm_monitor_ind_msg_s;


/*=============================================================================

                        MSGR UMID Definitions

=============================================================================*/

/*! @brief These Indications are used for CFCM Flow Control 
 */
enum 
{
  /*! MCS_CFCM_MONITOR_IND msg which will 
           be used by CFCM to post monitor indications internally */
  MSGR_DEFINE_UMID( MCS, CFCM, IND, MONITOR,
                    CFCM_MONITOR_ID, cfcm_monitor_ind_msg_s)

};

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



#endif /* CFCM_MSG_INTERNAL_H */
