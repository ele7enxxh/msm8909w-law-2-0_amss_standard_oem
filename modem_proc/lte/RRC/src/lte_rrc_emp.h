
/*!
  @file
  lte_rrc_emp.h

  @brief
  EMP's exposed APIs.
*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_emp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/08   zz      Initial Version
===========================================================================*/

#ifndef LTE_RRC_EMP_H
#define LTE_RRC_EMP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_osys_asn1util.h"
#include <lte_as.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_emp_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_emp_dd_deinit(void);

/* Compare TMGI of sys format and asn format */
boolean lte_rrc_emp_compare_tmgi
(
  lte_rrc_embms_tmgi_info_s tmgi, /*!< TMGI */
  lte_rrc_osys_TMGI_r9 tmgi_r9 /*!< TMGI in asn format */
);

/* Get MCCH PDU ptr from mcch db */
lte_rrc_osys_MCCH_Message *lte_rrc_emp_get_mbsfnareaconfig
(
  lte_mbsfn_area_id_t mbsfn_area_id
);

#endif /* LTE_RRC_EMP_H */
