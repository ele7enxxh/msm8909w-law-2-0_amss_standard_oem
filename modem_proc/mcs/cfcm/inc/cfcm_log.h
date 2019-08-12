#ifndef __CFCM_LOG_H__
#define __CFCM_LOG_H__
/*!
  @file cfcm_log.h

  @brief
   Diag interface used by CFCM module

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/14   rj      Adding MDM_TEMP and VDD_PEAK_CURR monitors
11/03/14   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "log_codes.h"
#include "log.h"
#include <dsm_pool.h>
#include "cfcm.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define FEATURE_LOG_TECH_STATE
#define FEATURE_LOG_SCENARIO_TBL


/*=============================================================================

                             TYPEDEFS

=============================================================================*/
/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/
#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

typedef PACK(struct)
{
  uint32  load; /*!< Load: CPU Load percentage */
  uint32  max_cpu_freq;  /*!< MAX CPU Freq in KHz */
  uint32  curr_cpu_freq; /*!< Current CPU Freq in KHz */
}cfcm_log_cpu_monitor_type;

typedef PACK(struct)
{
  uint32  state; /*!< State (0,1,2,3) & monitor_id could be use to diff between 
                                        (Thermal PA, RA, MDM Temp or Peak Curr) */
}cfcm_log_npa_node_type;

typedef PACK(struct)
{
  uint8  level; /*!< Bus Bandwidth Throttle level */
}cfcm_log_bw_monitor_type;

typedef PACK(struct)
{
  int   pool_id; /*!< pool triggered the event */
  int  event;   /*!< level reached */
  int     op;       /*!< operation that triggered the event */
}cfcm_log_dsm_monitor_type;

typedef PACK(union)
{
  cfcm_log_cpu_monitor_type       cpu;
  cfcm_log_npa_node_type         npa_node;
  cfcm_log_dsm_monitor_type       dsm;
  cfcm_log_bw_monitor_type        bw;
}cfcm_log_monitor_type;

typedef PACK (struct)
{
  boolean is_valid;
  uint8   tier_high;
  uint8   tier_medium;
  uint8   tier_low;
}cfcm_log_cpu_thrshld_type;


typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  uint8 monitor_id;
  cfcm_log_monitor_type monitor;
}cfcm_log_monitor_info_type;

typedef PACK(struct)
{
  uint8                       client_id;     /*!< client ID, CFCM only works with known client */
  uint32                      cmd;        /*!< flow control command */
  uint32                      step_timer; /*!< step timer in ms, shall be ignored unless the cmd is up or down */
  uint32                      monitors_active;  /*!<Bit mask of monitors which set command*/
  uint32                      data_rate; /*!< This will be valid only when CMD is CFCM_CMD_SET_VALUE */
  cfcm_log_cpu_thrshld_type   cpu_thrshld; /*!< CPU Thresholds for particular client */
}cfcm_log_client_cmd_data_type;

typedef PACK(struct)
{
  uint8 monitor_id;
  cfcm_log_monitor_type monitor;
  cfcm_log_client_cmd_data_type client_cmd[CFCM_CLIENT_MAX];
}cfcm_log_output_data_type;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  cfcm_log_output_data_type output;
}cfcm_log_cmd_info_output_type;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

extern cfcm_log_cpu_thrshld_type cpu_thrshld[CFCM_CLIENT_MAX];

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  cfcm_log_monitor_input

=============================================================================*/
/*!
    @brief
    Logs the power and frequency information for a particular tech
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_monitor_input
(
  cfcm_monitor_e monitor_id,
  cfcm_log_monitor_type* monitor_data
);

/*=============================================================================

  FUNCTION:  cfcm_log_command_output

=============================================================================*/
/*!
    @brief
    Logs all CFCM Output Command
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_command_output
( 
  uint32* hist_idx,
  cfcm_monitor_e monitor_id,
  cfcm_log_monitor_type* monitor_data
);

/*=============================================================================

  FUNCTION:  cfcm_log_update_cpu_thrshlds

=============================================================================*/
/*!
    @brief
    Updates CPU thresholds for logging purpose
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_update_cpu_thrshlds
(
  cfcm_log_cpu_thrshld_type* cpu_thrshld
);
#endif /* __CFCM_LOG_H__ */

