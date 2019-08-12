/*!
  @file
  lte_mac_fc.h

  @brief
  This file contains all define structures/function prototypes 
  for LTE MAC Flow Control Module.


*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_fc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/14   mg      CR733800: Fix compile errors on Jolokia
08/03/14   mg      CR702710: QSH analysis and reset
06/27/14   sb      CR629741: Change in LTE to migrate to CFCM
05/31/12   st      CR 342695: CPU based flow control enhancements
12/16/11   ax      feature clean up FEATURE_LTE_MAC_FC_UL
11/11/10   ax      reduce fc_integration period to avoid excessive UL delay
08/05/10   bn      Defined the FC robustness timer
08/02/10   bn      Used the robustness timer instead of periodic timer
07/16/10   bn      Used uint32 for timer tick to prevent overflow
05/10/10   ax      fix compiler warnings
05/06/10   ax      compile in FEATURE_LTE_MAC_FC_UL
05/15/10   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_FC_H
#define LTE_MAC_FC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_mac_diag.h"
#include <msgr.h>
#include <msgr_lte.h>
#include <cfcm.h>
#include <lte_fc.h>
#include "lte_mac_ctrl.h"
#include "qsh.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief define default value of retxed BSR interval. For FC, this value will
    be used. It is a compromise between maximizing SR internal and stastifying
    TCP windows
*/
#define  LTE_MAC_FC_UL_DEFAULT_RXTED_BSR_INT    130

/*! @brief Define the Flow control event */
typedef enum
{
  LTE_MAC_FC_OFF,
  LTE_MAC_FC_THERMAL,
  LTE_MAC_FC_CPU,
  LTE_MAC_FC_BW,
  LTE_MAC_FC_TIMER
} lte_mac_fc_event_e;

/*! @brief MAC UL FC packet build data structure definition
*/
typedef struct
{
  /*! Flag to indicate if UL FC is enable*/
  boolean enable;
  /*! tick count in number of ITI since the last 
  new target data rate*/
  uint32  tick_cnt;
  /*! target data rate in number of byte per TTI or ms*/
  uint32  target_rate;
  /*! Initial BSR buffer size based on the target data rate*/
  uint32  initial_bs;
} lte_mac_fc_ul_pkt_s;

/*! @brief MAC UL FC stats data structure definition
*/
typedef struct
{
  /*! Number of CFM_CMD_DOWN triggered*/
  uint32  num_fc_down;
  /*! Number of CFM_CMD_UP triggered*/
  uint32  num_fc_up;
  /*! Number of CFM_CMD_FREEZE triggered*/
  uint32  num_fc_freeze;
  /*! Number of CFM_CMD_SHUT_DOWN triggered*/
  uint32  num_fc_shutdown;
} lte_mac_fc_stats_s;

/*! @brief MAC FC local data structure definition
*/
typedef struct
{
  /*! FC state */
  cfcm_cmd_e state;
  /*! current state level*/
  uint8     state_level;
  /*! the lowest state level mapped to 
  the lowest target data rate*/
  uint8     state_level_lowest;
  /*! FC UL configuration reads out from EFS */
  lte_fc_cfg_s  ul_cfg;
  /*! CPU FC configuration reads out from EFS */
  lte_fc_cpu_fc_cfg_s  cpu_cfg;
  /*! BW FC configuration reads out from EFS */
  lte_fc_bw_fc_cfg_s  bw_cfg;
  /*! CPU FC default rate level */
  uint8     cpu_fc_default_level;
  /*! CPU FC min rate level */
  uint8     cpu_fc_min_level;
  /*! BW FC default rate level */
  uint8     bw_fc_default_level;
  /*! BW FC min rate level */
  uint8     bw_fc_min_level;
  /*! time counter for step timer */
  uint32  timer_cnt;
  /*! step timer */
  uint32  step_timer;
  /*! Local statistics*/
  lte_mac_fc_stats_s stats;
} lte_mac_fc_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_mac_fc_init

===========================================================================*/
/*!
    @brief
    This function inits all the MAC flow control related data structure.

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_init
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_fc_periodic_process

===========================================================================*/
/*!
    @brief
    This function runs periodically  at QoS period rate. It keeps track 
    the FC timer and if the tmer is expired, update the FC target data 
    rate and trigger a FC BSR 

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_periodic_process
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_fc_deregister_fc_cmd

===========================================================================*/
/*!
    @brief
    This function deregisters FC commands from CFCM.

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_deregister_fc_cmd
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_fc_register_fc_cmd

===========================================================================*/
/*!
    @brief
    This function registers FC commands with CFCM

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_register_fc_cmd
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_fc_command_process

===========================================================================*/
/*!
    @brief
    This function processes all the input commands/indications from the Flow
    Control Manager 

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_command_process
(
  lte_mac_ctrl_msg_u* msg_ptr /*!< input message pointer */ 
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_update_fc_ul_info

===========================================================================*/
/*!
    @brief
    Update the UL Flow Data in QoS Structure when the uplink flow control
    target data rate has changed. Also indicate to the QoS to trigger a 
    FC BSR 
    
    @detail
    
    @return
    None.  

    @see related_function()
    
*/
/*=========================================================================*/
extern void lte_mac_qos_update_fc_ul_info
(
  lte_mac_fc_event_e fc_event,   /*!< UL Flow control event */
  uint32  target_data_rate       /*!< Target data rate */
);

/*==============================================================================

  FUNCTION:  lte_mac_fc_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC FC QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC CTRL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_fc_qsh_analysis_routine 
(
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_fc_qsh_reset_stats

==============================================================================*/
/*!
    @brief
    This API runs the MAC FC QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC CTRL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_fc_qsh_reset_stats
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_fc_reset_state

===========================================================================*/
/*!
    @brief
    This function resets MAC flow control related state

    @detail

    @return
    None

    @note

    @see related_function()
   
*/
/*=========================================================================*/
extern void lte_mac_fc_reset_state
(
  void 
);

#endif /* LTE_MAC_FC_H */
