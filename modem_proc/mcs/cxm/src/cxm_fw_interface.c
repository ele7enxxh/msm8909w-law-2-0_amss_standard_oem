/*!
  @file
  cxm_fw_interface.c

  @brief
  CXM APIs to interface with CFW

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_fw_interface.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/14   tak     Initial implementation

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cxm_intf_types.h"
#ifndef FEATURE_COEX_FW_STANDALONE_BUILD
#include "cxm_smem.h"
#include "coex_algos.h"
#endif

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_msg_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_smem_handle_fw_msg.
    Send event indication from FW to WCN via smp2p

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_msg_intf (
  cxm_tech_type tech,
  cxm_port_id_e id,
  boolean assert
)
{

#ifndef FEATURE_COEX_FW_STANDALONE_BUILD
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_smem_handle_fw_msg ( tech,
                           id,
                           assert );
#endif /* FEATURE_MCS_COEX_NOCODE */
#endif
}

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_activity_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_smem_handle_fw_activity.
    Send activity indication to WCN via SMEM. CFW calls this
    when port ID is CXM_PORT_TYPE_SMEM_V2_BASED.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_activity_intf (
  cxm_tech_type tech,
  cxm_rx_tx_activity_info_u activity
)
{
#ifndef FEATURE_COEX_FW_STANDALONE_BUILD
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_smem_handle_fw_activity ( tech,
                                activity );
    
#endif /* FEATURE_MCS_COEX_NOCODE */
#endif
}

/*===========================================================================

  FUNCTION:  cxm_req_boot_params_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_req_boot_params.
    Resend the boot params message. CFW will use this to ask for the boot
    params message. A function call is used here since CFW does not yet
    have a msgr message Q.

  @return
    none
*/
/*=========================================================================*/
void cxm_req_boot_params_intf (
  void
)
{
#ifndef FEATURE_COEX_FW_STANDALONE_BUILD
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_req_boot_params();
#endif /* FEATURE_MCS_COEX_NOCODE */
#endif
}





