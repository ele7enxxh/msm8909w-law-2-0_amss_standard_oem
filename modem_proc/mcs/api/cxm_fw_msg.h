#ifndef __CXM_FW_MSG_H__
#define __CXM_FW_MSG_H__
/*!
  @file cxm_fw_msg.h

  @brief
   CXM-FW msgr payloads and UMIDS

*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/13   tak     Initial revision

=============================================================================*/

/*=============================================================================

                   COMMENTS/NOTES FOR USAGE/UPDATES

   In order to backward compatibility as well as being able to expose
   essential data types to lower layers here are some ground rules,
   - Only include generic headers and not any tech-specific headers
   - ENUMs & DEFINEs for mask-bits are not version controlled and new values
       can be added as future design needs come up
   - Structures must be versioned to enable support for multiple techs
 
=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
/* To expose compiler supported data types */
#include <comdef.h>
/* MSGR API */
#include "msgr.h"
#include "cxm_intf_types.h"

/*=============================================================================

                       CONSTANTS, DEFINES & ENUMS

=============================================================================*/

/*! @brief Masks for determining which FW counters to reset. This 
           matches up with cxm_fw_counters_e below*/
#define CXM_RESET_CNT_FRAME_SYNC               0x00000001      
#define CXM_RESET_CNT_TX_ACTIVE                0x00000002
#define CXM_RESET_CNT_RX_PRIO                  0x00000004
#define CXM_RESET_CNT_WCN_PRIO_BLANK_REQ       0x00000008
#define CXM_RESET_CNT_WCN_PRIO_BLANK_DENIED    0x00000010

/*! @brief IDs for FW msgr messages. Please refer to cxm.h for next available ID 
           before adding to this list*/
#define COEX_FW_COUNTERS_RESET_ID    0x29
#define COEX_TRIGGER_WCN_PRIO_ID     0x2A
#define COEX_CONFIG_PARAMS_ID        0x2C

/*=============================================================================

                             DATA TYPES

=============================================================================*/

/*! @brief Indicates the CXM system-wide configuration for various WWAN-techs to follow */
typedef uint32 cxm_coex_protocol_t;

/*! @brief Reset FW counters */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;    /*!< msgr header containing msg_id */
  cxm_tech_type        tech;       /*!< tech to reset */ 
  uint32               reset_mask; /*!< specifies which counters to reset */
}cxm_reset_fw_counters_ind_s;

/*! @brief Trigger WCN prio */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;        /*!< msgr header containing msg_id */
  uint32               l2vic_bit_pos;  /*!< l2vic position*/ 
  uint32               assert_time;    /*!< duration to assert WCN prio in microseconds */
}cxm_trigger_wcn_prio_ind_s;

/*! @brief Message sent by CxM controller to all WWAN techs at boot to configure 
    @note  This message is version controlled as it is also used by WWAN tech LLs */
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;     /*!< msgr header containing msg_id */
  cxm_coex_protocol_t   protocol;    /*!< system-wide config for WWAN techs - deprecated 
                                          since all modules now use sys_behavior */
  cxm_boot_config_v01_s boot_config; /*!< system-wide boot params for WWAN techs */
}cxm_config_params_ind_s;

/*=============================================================================

                        MSGR UMID Definitions

=============================================================================*/
/*! @note Each UMID definition should be on a single line so msgr script will parse them correctly */
enum
{
  /* Reset FW counters */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_FW_COUNTERS_RESET, COEX_FW_COUNTERS_RESET_ID, cxm_reset_fw_counters_ind_s ),
  /* Trigger WCN prio */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TRIGGER_WCN_PRIO, COEX_TRIGGER_WCN_PRIO_ID, cxm_trigger_wcn_prio_ind_s ),
  /* Config params */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_CONFIG_PARAMS, COEX_CONFIG_PARAMS_ID, cxm_config_params_ind_s )
};
#endif /* __CXM_FW_MSG_H__ */
