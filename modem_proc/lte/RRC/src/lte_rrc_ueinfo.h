/*!
  @file
  lte_rrc_ueinfo.h

  @brief
  Header file for RRC UEINFO Module for use by other modules.

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the UEINFO module and its UTF test cases.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_ueinfo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/12      rg    Intial Version

===========================================================================*/

#ifndef LTE_RRC_UEINFO_H
#define LTE_RRC_UEINFO_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_ueinfo.h>
#include "lte_rrc_osys_asn1.h"
#include "lte_rrc_osys_asn1util.h"


/*===========================================================================

                           EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


extern void lte_rrc_ueinfo_dd_init
(
  void
);

extern void lte_rrc_ueinfo_dd_deinit
(
  void
);

extern boolean lte_rrc_ueinfo_get_rpt_status
(
  void
);

extern boolean lte_rrc_ueinfo_is_con_est_failed_rpt_present
(
  void
);
extern void lte_rrc_init_loc_info
(
   lte_rrc_osys_LocationInfo_r10*
);
extern boolean lte_rrc_ueinfo_verify_rl_failure
(
  void
);

extern boolean lte_rrc_ueinfo_rlf_rpt_available
(
  void
);

extern void lte_rrc_ueinfo_set_reest_cellid
(
  void
);

#endif /* LTE_RRC_UEINFO_H */
