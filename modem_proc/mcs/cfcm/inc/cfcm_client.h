/*!
  @file
  cfcm_client.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_client.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/15/14   sg      QSH related modifications
11/20/14   rj      Log Packet support added in CFCM
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_CLIENT_H
#define CFCM_CLIENT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "cfcm.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief history buffer size
*/
#define CFCM_CLIENT_HIST_BUF_SZ                                         (1 << 4)

/*! @brief history buffer index mask
*/
#define CFCM_CLIENT_HIST_BUF_IDX_MASK                (CFCM_CLIENT_HIST_BUF_SZ - 1)

/*! @brief statistics about a client
*/
typedef struct
{
  uint32  max_fc_period;
  uint32  total_fc_period;
  uint32  last_fc_start_time;
  boolean fc_started;
} cfcm_client_dbg_s;

/*! @brief statistics about a client
*/
typedef struct
{
  uint32  num_cmd_sent;      /*!< total number of commands sent to the client */
  uint32  num_down_cmd_sent; /*!< total number of down commands sent */
  cfcm_client_dbg_s   monitor[CFCM_MONITOR_MAX];  /*!< debug information per monitor */
} cfcm_client_stats_s;

/*! @brief cfcm client information structure
*/
typedef struct
{
  cfcm_client_e    client_id;  /*!< for ease of debugging */
  cfcm_fc_evt_cb_type fc_cb_ptr; /*!< flow control event callback function */
  msgr_umid_type  fc_req;     /*!<flow control REQ umid unique for the client */
  uint32          monitor_mask; /*!<mask of monitors client respond to */
  cfcm_cmd_type_s    last_cmd;      /*!<  last cmd sent to this client */
  uint32          last_down_step_timer; /*!< step timer of the last down,
                                             set_min command*/
  cfcm_client_stats_s stats;      /*!< statistics */
  uint32        latest_cmd_hist_idx;/*!< latest command history buffer index */ 
  cfcm_cmd_type_s       cmd_hist[CFCM_CLIENT_HIST_BUF_SZ]; /*!< command history */
} cfcm_client_info_s;             

/*! @brief top level structure for cfcm_client
*/
typedef struct
{
  uint32             num_errors;               /*!< total errors */
  cfcm_client_info_s  clients[CFCM_CLIENT_MAX];  /*!< info for each know client */
} cfcm_client_s;

/* Global CFCM Client state variables */
extern cfcm_client_s cfcm_client;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void cfcm_client_init( void );

/*==============================================================================

  FUNCTION:  cfcm_client_compute_fc_cmd

==============================================================================*/
/*!
    @brief
    compute the flow control command based on current monitor states.

    @detail
    Also send flow control command if it is different from the last command sent

    @return
    None
*/
/*============================================================================*/
extern void cfcm_client_update_fc_cmd
(
  cfcm_client_e          client
);

extern void cfcm_client_proc_reg
(
  const cfcm_reg_req_msg_type_s*     msg_ptr
);

extern void cfcm_client_proc_dereg
(
  const cfcm_dereg_req_msg_type_s*   msg_ptr
);

/*==============================================================================

  FUNCTION:  cfcm_client_get_current_history_idxs

==============================================================================*/
/*!
    @brief
    Return data with current History Index for ALL CLIENTS, so that one can check whether Client command is updated, 
    this is being used for Log packets

    @return
    return data related to current history index

*/
/*============================================================================*/
void cfcm_client_get_current_history_idxs( uint32* hist_idx );

/*==============================================================================

  FUNCTION:  cfcm_client_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
extern void cfcm_client_qsh_analysis
(
  void
);

/*==============================================================================

  FUNCTION:  cfcm_client_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
extern void cfcm_client_qsh_reset
(
  void
);

#endif /* CFCM_CLIENT_H */
