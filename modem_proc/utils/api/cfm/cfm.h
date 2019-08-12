/*!
  @file
  cfm.h

  @brief
  Centralized Flow-control Manager(CFM) header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  gxiao

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/cfm/cfm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/17/14   sb      CR646050: Add support for new monitor "BW Throttle" to limit modem
                   bus bandwidth access 
11/15/13   ax      CR568674 add Data Services as a CFM client 
05/20/12   ax      CR316940 CPU based flow control  
02/25/11   ax      separate API needed outside modem to cfm_ext_api.h
02/10/11   ax      add support for cfm_cap_ul_data_rate() api
01/31/11   ax      add support for pdcp dl flow control
11/10/10   ax      add support for PA_EX monitor and ML1 client for power limit
06/28/10   ax      add support for sending flow control command via callbacks
05/21/10   sm      Removed cfm_set_fc_mode()
05/21/10   sm      Removed fc_enabled related functions
03/30/10   ax      initial version
==============================================================================*/

#ifndef CFM_H
#define CFM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr.h>
#include "cfm_msg.h"
#include "cfm_ext.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief CPU based flow control step timer in ms
*/
#define CFM_CPU_BASED_FC_STEP_TIMER                            200

#ifdef FEATURE_DIMEPM_MODEM
/*! @brief BW based flow control step timer in ms
*/
#define CFM_BW_BASED_FC_STEP_TIMER                             100
#endif

#define CFM_MONITOR_MASK_THERMAL_PA                            0x1
#define CFM_MONITOR_MASK_CPU                                   0x2
#define CFM_MONITOR_MASK_DSM_LARGE_POOL_UL                     0x4
#define CFM_MONITOR_MASK_DSM_LARGE_POOL_DL                     0x8
#define CFM_MONITOR_MASK_DSM_DUP_POOL_UL                       0x10
#define CFM_MONITOR_MASK_DSM_DUP_POOL_DL                       0x20
#define CFM_MONITOR_MASK_THERMAL_PA_EX                         0x40
#define CFM_MONITOR_MASK_THERMAL_PA_EM                         0x80
#ifdef FEATURE_DIMEPM_MODEM
#define CFM_MONITOR_MASK_BW_THROTTLING                         0x100
#endif

/*! @brief CFM aware monitors enumeration
    NOTE: KEEP THIS IN SYNC WITH THE MONITOR MASK ABOVE
*/
typedef enum
{
  CFM_MONITOR_THERMAL_PA = 0,    /*!<  Power Amplifier temperature */
  CFM_MONITOR_CPU,               /*!<  Modem SWQ6 CPU Load monintor */
  CFM_MONITOR_DSM_LARGE_POOL_UL, /*!<  DSM large pool UL monitor */
  CFM_MONITOR_DSM_LARGE_POOL_DL, /*!<  DSM large pool UL monitor */
  CFM_MONITOR_DSM_DUP_POOL_UL,   /*!<  DSM dup pool UL monitor */
  CFM_MONITOR_DSM_DUP_POOL_DL,   /*!<  DSM large pool UL monitor */
  CFM_MONITOR_THERMAL_PA_EX,     /*!<  Power Amplifier extreme temperature */
  CFM_MONITOR_THERMAL_PA_EM,     /*!<  Power Amplifier thermal emergency */
#ifdef FEATURE_DIMEPM_MODEM
  CFM_MONITOR_BW_THROTTLING,     /*!<  BW throttling to limit the BUS bandwidth*/
#endif
  CFM_MONITOR_MAX                /*!<  MAX */
} cfm_monitor_e;

#define CFM_CLIENT_MASK_LTE_UL                            0x1
#define CFM_CLIENT_MASK_LTE_RLC_DL                        0x2
#define CFM_CLIENT_MASK_A2_UL_PER                         0x4
#define CFM_CLIENT_MASK_LTE_ML1                           0x8
#define CFM_CLIENT_MASK_LTE_PDCP_DL                       0x10
#define CFM_CLIENT_MASK_LTE_PDCP_UL                       0x20
#define CFM_CLIENT_MASK_CM                                0x40
#define CFM_CLIENT_MASK_LTE_ML1_CPU                       0x80
#define CFM_CLIENT_MASK_DS                                0x100

/*! @brief CFM aware clients enumeration
     NOTE: KEEP THIS IN SYNC WITH THE CLIENT MASK ABOVE
*/
typedef enum
{
  CFM_CLIENT_LTE_UL = 0,     /*!<  LTE UL */
  CFM_CLIENT_LTE_RLC_DL,     /*!<  LTE DL */
  CFM_CLIENT_A2_UL_PER,      /*!<  A2 UL PER */
  CFM_CLIENT_LTE_ML1,        /*!<  LTE ML1 */
  CFM_CLIENT_LTE_PDCP_DL,    /*!<  LTE PDCP DL */
  CFM_CLIENT_LTE_PDCP_UL,    /*!<  LTE PDCP UL */
  CFM_CLIENT_CM,             /*!<  Call Manager */
  CFM_CLIENT_LTE_ML1_CPU,    /*!<  LTE ML1 CPU FC client */
  CFM_CLIENT_DS,             /*!<  Data Services FC client */
  CFM_CLIENT_MAX             /*!<  MAX */
} cfm_client_e;

/*! @brief commands that can be issued by CFM enumeration
*/
typedef enum
{
  CFM_CMD_FC_OFF        = 0,    /*!<turn off flow control */
  CFM_CMD_UP            = 1,    /*!<go up one level, when step timer expires, 
                                    go up one level further unless hit top*/
  CFM_CMD_FREEZE        = 3,    /*!<freeze: stay at the same fc state */
  CFM_CMD_DOWN          = 7,    /*!<go down one level when step timer expires, 
                                    go down one level more unless hit bottom */
  CFM_CMD_SET_MIN       = 15,   /*!<go down all the way to minimal level*/
  CFM_CMD_SHUT_DOWN     = 31    /*!<shut down completely */
} cfm_cmd_e;

/*! @brief flow control command that is sent by the CFM
*/
typedef struct
{
  cfm_cmd_e         cmd;        /*!< flow control command */
  uint32            step_timer; /*!< step timer in ms, shall be ignored unless 
                                 the cmd is up or down */
} cfm_fc_cmd_s;


/*! @brief flow control request message format sent by CFM
*/
typedef struct
{
  msgr_hdr_s     hdr;    /*!< standard msgr header */
  cfm_fc_cmd_s   fc_cmd; /*!<  flow control cmd */
} cfm_fc_cmd_msg_s;

/*! @brief flow control callback for clients registered with 
     cfm_client_reg_with_cb
*/
typedef void(*cfm_fc_evt_cb_type)(cfm_fc_cmd_s *);

/*! @brief client registration indication message format
*/
typedef struct
{
  msgr_hdr_s            hdr;          /*!< standard msgr header */
  cfm_client_e          client;       /*!<  client ID */
  uint32                monitor_mask; /*!<monitors the client is design to 
        respond to, each monitor is assigned a mask bit, the bits can be | . 
        e.g. CFM_MONITOR_MASK_THERMAL_PA | CFM_MONITOR_MASK_DSM_POOL */
  cfm_fc_evt_cb_type    fc_evt_cb_ptr; /*!<  flow control event callback: Set to
                                            NULL if prefer fc command messages 
                                            via message router */
  msgr_umid_type        fc_req_umid;  /*!<flow control request message umid 
                                          unique for the client needed 
                                          only if fc_evt_cb is NULL */
} cfm_reg_ind_msg_s;

/*! @brief client deregistration indication message format
*/
typedef struct
{
  msgr_hdr_s    hdr;     /*!< standard msgr header */
  cfm_client_e  client;  /*!<  client ID */
} cfm_dereg_ind_msg_s;

/*! @brief monitor state change indication message format
*/
typedef struct
{
  msgr_hdr_s     hdr;      /*!< standard msgr header */
  cfm_monitor_e  monitor;  /*!< monitor id */
  cfm_cmd_e      cmd;      /*!<command corresponding to the new monitor state */
} cfm_monitor_ind_msg_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfm_task_is_active

==============================================================================*/
/*!
    @brief
    returns whether or not the CFM task is running.

    @return
    TRUE if CFM task is running; FALSE otherwise.
*/
/*============================================================================*/
extern boolean cfm_task_is_active(void);

/*===========================================================================

  FUNCTION:  cfm_task_init

===========================================================================*/
/*!
  @brief
  This function creates CFM task with the given priority.

  @return
  the CFM thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t cfm_task_init
( 
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  cfm_client_reg

===========================================================================*/
/*!
  @brief
  This function registers with the CFM for the client.

  @detail
  flow control command is sent to the client via messages via message router

  @return
  None.
*/
/*=========================================================================*/
extern void cfm_client_reg
(
  cfm_client_e   client_id,  /*!< client ID, CFM only works with known client */
  msgr_umid_type fc_req_umid,/*!< UMID for the fc command sent to the client */
  uint32         monitor_mask /*!<monitors the client is design to 
        respond to, each monitor is assigned a mask bit, the bits can be | . 
        e.g. CFM_MONITOR_MASK_THERMAL_PA | CFM_MONITOR_MASK_DSM_POOL */
);

/*===========================================================================

  FUNCTION:  cfm_client_reg_with_cb

===========================================================================*/
/*!
  @brief
  This function registers with the CFM for the client.

  @detail
  this is different from cfm_client_reg in the sense that CFM is to notify the
  client of flow control events via call back function.

  @return
  None.
*/
/*=========================================================================*/
extern void cfm_client_reg_with_cb
(
  cfm_client_e     client_id,/*!< client ID, CFM only works with known client */
  cfm_fc_evt_cb_type fc_cb_ptr, /*!< fc command callback function */
  uint32         monitor_mask /*!<monitors the client is design to 
        respond to, each monitor is assigned a mask bit, the bits can be | . 
        e.g. CFM_MONITOR_MASK_THERMAL_PA | CFM_MONITOR_MASK_DSM_POOL */
);

/*===========================================================================

  FUNCTION:  cfm_client_dereg

===========================================================================*/
/*!
  @brief
  This function de-registers with the CFM for the client.

  @return
  None.
*/
/*=========================================================================*/
extern void cfm_client_dereg
(
  cfm_client_e   client_id
);

#endif /* CFM_H */
