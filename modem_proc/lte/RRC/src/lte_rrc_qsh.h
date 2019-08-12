/*!
  @file
  lte_rrc_qsh.h

  @brief
  Header file for RRC QSH Module for use by other modules.

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the RRC QSH module and its UTF test cases.

*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_qsh.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/16   rtindola      Initial Version: CL reference #941365
===========================================================================*/

#ifndef LTE_RRC_QSH_H
#define LTE_RRC_QSH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte_rrc_int_msg.h>


/*===========================================================================

                           EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_QSH_EVENT_METRIC
extern void lte_rrc_qsh_log_rrc_call_end_stat
(
 lrrc_qsh_metric_conn_end_info_s call_end
 
);

__attribute__((section(".uncompressible.text")))
extern void lte_rrc_qsh_cb_handler
(
  qsh_client_cb_params_s cb_params
);


extern void lte_rrc_qsh_init
(
);
#endif
#endif /* LTE_RRC_QSH_H */
