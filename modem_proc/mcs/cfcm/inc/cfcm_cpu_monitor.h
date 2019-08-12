/*!
  @file
  cfcm_cpu_monitor.h

  @brief
  CPU monitor implementation.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_cpu_monitor.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/15/14   sg      QSH related modifications
10/10/14   rj      Remove FEATURE_CFCM_WAIT_FOR_CLM_SUBMIT from CFCM
10/03/14   rj      CFCM changes to Process monitor's input in its task context
10/07/14   ag      Remove JO featurization 
09/06/14   rj      Changes to send OFF command after UP due to CPU
07/16/14   rj      Enable CLM feature for NON-Jolokia PLs
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_CPU_MONITOR_H
#define CFCM_CPU_MONITOR_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "cfcm.h"
#include "cfcm_msg_internal.h"

/* CPU Load Monitor Module */
#include "CLM.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief state of CPU monitor
*/
typedef enum
{
  CFCM_CPU_MONITOR_STATE_NOLOAD = 0,      /*!< No load hence FC OFF */
  CFCM_CPU_MONITOR_STATE_UNDERLOAD,       /*!< Underloaded */
  CFCM_CPU_MONITOR_STATE_NORMAL,          /*!< normal state */
  CFCM_CPU_MONITOR_STATE_OVERLOAD,        /*!< Overloaded */
  CFCM_CPU_MONITOR_STATE_MAX              /*!< MAX */
} cfcm_cpu_monitor_state_e;

typedef struct
{
  cfcm_cpu_monitor_state_e   state; /*!< current state of CPU monitor */

  cfcm_cmd_e   cmd; /*!<command corresponding to the new monitor state */

} cfcm_cpu_monitor_client_cmd_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void cfcm_cpu_monitor_init( void );

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
void cfcm_cb_from_clm(CLM_LoadInfoBasicStructType * clmInfo, void * clientData);

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
);
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
);

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
extern void cfcm_cpu_monitor_qsh_analysis
(
  void
);

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_analysis_async

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
extern void cfcm_cpu_monitor_qsh_analysis_async
(
  void
);

/*==============================================================================

  FUNCTION:  cfcm_cpu_monitor_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
extern void cfcm_cpu_monitor_qsh_reset
(
  void
);

/*==============================================================================

                                UNIT TEST

==============================================================================*/
void cfcm_test_cpu_monitor_via_diag
(
  uint8 input 
);

#endif /* CFCM_CPU_MONITOR_H */
