/*!
  @file
  lte_fc.h

  @brief
  LTE flow control related common header file.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_fc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/27/14   sb      CR629741: Change in LTE to migrate to CFCM
05/20/12   ax      CR316940 CPU based flow control     
03/30/10   ax      initial version
==============================================================================*/

#ifndef LTE_FC_H
#define LTE_FC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr_lte.h>
#include "cfcm.h"
#include "lte_fc_msg.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief flow control state enumeration
*/
typedef enum
{
  LTE_FC_STATE_0 = 0,   /*!< highest state, default/flow control is OFF */
  LTE_FC_STATE_1,       /*!< 2nd highest state  */
  LTE_FC_STATE_2,       /*!< 2rd highest state  */
  LTE_FC_STATE_3,       /*!< 4th highest state  */
  LTE_FC_STATE_4,       /*!< 5th highest state  */
  LTE_FC_STATE_5,       /*!< 6th highest state  */
  LTE_FC_STATE_6,       /*!< 7th highest state  */
  LTE_FC_STATE_7,       /*!< 8th highest state  */
  LTE_FC_STATE_8,       /*!< 9th highest state  */
  LTE_FC_STATE_9,       /*!< 10th highest state/lowest rate state  */
  LTE_FC_STATE_MAX      /*!< max number of state */
} lte_fc_state_e;

/*! @brief flow control configuration
*/
typedef struct
{
  uint8           num_state;                      /*!< number of states*/
  lte_fc_state_e  default_state;         /*!< default state when fc triggers */
  uint32          target_rate[LTE_FC_STATE_MAX];  /*!< target rate each state*/   
} lte_fc_cfg_s;

/*! @brief CPU based flow control parameters
*/
typedef struct
{
  uint32  default_rate;  /*!< target data rate when entering CPU based FC */
  uint32  min_rate;      /*!< minimum data rate for CPU based FC */
  uint32  step_timer;    /*!< step timer in ms for CPU based FC */
} lte_fc_cpu_fc_cfg_s;

/*! @brief BW based flow control parameters
*/
typedef struct
{
  uint32  default_rate;  /*!< target data rate when entering CPU based FC */
  uint32  min_rate;      /*!< minimum data rate for CPU based FC */
  uint32  step_timer;    /*!< step timer in ms for CPU based FC */
} lte_fc_bw_fc_cfg_s;

/*! @brief flow control shut down indication, ought to be captured by RRC
*/
typedef struct
{
  msgr_hdr_s    hdr;     /*!< standard msgr header */
} lte_fc_shutdown_ind_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_fc_client_reg
(
  cfcm_client_e   client_id,
  msgr_umid_type fc_req_umid
);

extern void lte_fc_client_dereg
(
  cfcm_client_e   client_id
);

extern void lte_fc_get_macul_cfg
(
  lte_fc_cfg_s*      cfg_info_ptr
);

extern void lte_fc_get_cpu_based_fc_cfg
(
  lte_fc_cpu_fc_cfg_s* cpu_fc_cfg_ptr
);

extern void lte_fc_get_bw_based_fc_cfg
(
  lte_fc_bw_fc_cfg_s* bw_fc_cfg_ptr
);

extern void lte_fc_shut_down( void );

#endif /* LTE_FC_H */
