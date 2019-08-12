#ifndef CXM_SMEM_H
#define CXM_SMEM_H
/*!
  @file
  cxm_smem.h

  @brief
  APIs for internal WLAN communication over SMEM

*/

/*===========================================================================

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/14   tak     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <IxErrno.h>
#include "cxm_msgr.h"
#include "wcn_coex_mgr.h"
#include "cxm_intf_types.h"
#include "coex_smem_intf.h"

/*===========================================================================

                             DEFINES

===========================================================================*/
/* state of FW tech activity */
typedef struct
{
  atomic_word_t writing;
  uint8 next_entry;
} cxm_smem_fw_activity_s;

/* state of each tech FW */
typedef struct
{
  cxm_smem_fw_activity_s act[CXM_TECH_MAX];
} cxm_smem_tech_fw_activity_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*=============================================================================

  FUNCTION:  cxm_get_smem_data_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global cxm_smem_data pointer for diag logging.
    A pointer to a pointer is necessary because cxm_smem_data is a static
    pointer variable declared in cxm_smem.c which can change value during
    runtime.  The cxm log LUTs are initialized only once.

    @return
    cxm_smem_data_s **
*/
/*===========================================================================*/
cxm_smem_data_s **cxm_get_smem_data_ptr( void );

/*=============================================================================

  FUNCTION:  cxm_get_smem_fw_act_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to FW activity state so that the logging module knows
    what index in the FW buffer was just updated.

    @return
    cxm_smem_tech_fw_activity_s*
*/
/*===========================================================================*/
cxm_smem_tech_fw_activity_s* cxm_get_smem_fw_act_ptr( void );

/*===========================================================================

  FUNCTION:  cxm_smem_init

===========================================================================*/
/*!
  @brief
    Initialize SMP2P and SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_init (
  void
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_sleep_wakeup

===========================================================================*/
/*!
  @brief
    Send sleep/wake-up information over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_sleep_wakeup (
  cxm_coex_tech_sleep_wakeup_duration_ind_s *msg 
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_frame_timing

===========================================================================*/
/*!
  @brief
    Send frame timing information over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_frame_timing_ind (
  void *rcv_msgr_msg_ptr
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_tx_adv

===========================================================================*/
/*!
  @brief
    Send tx advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_lte_tx_adv (
  cxm_lte_tx_adv_ntc_info_s *msg,
  boolean thres_check_passed 
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_activity_adv_ntc

===========================================================================*/
/*!
  @brief
    Send activity advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_activity_adv_ntc (
  cxm_coex_tech_activity_adv_ntc_ind_s *msg
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_activity_timeline

===========================================================================*/
/*!
  @brief
    Send activity timeline message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_activity_timeline (
  cxm_timing_info_s *msg
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_high_prio

===========================================================================*/
/*!
  @brief
    Send activity advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_high_prio (
  cxm_coex_high_prio_ind_s *msg
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_msg

===========================================================================*/
/*!
  @brief
    Send event indication from FW to WCN via smp2p

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_msg (
  cxm_tech_type tech,
  cxm_port_id_e id,
  boolean assert
);

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_activity

===========================================================================*/
/*!
  @brief
    Sends FW activity to WCN via SMEM. This function gets called
    when port ID is CXM_PORT_TYPE_SMEM_V2_BASED.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_activity (
  cxm_tech_type tech,
  cxm_rx_tx_activity_info_u activity
);

#endif /* CXM_SMEM_H */
