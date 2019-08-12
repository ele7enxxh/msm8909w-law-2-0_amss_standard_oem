#ifndef PM_QC_PMIC_H
#define PM_QC_PMIC_H

/**
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_qc_pmic.h PMIC related declaration.
 *
 * @brief This file contains prototype definitions to support interaction
 *        with the QUALCOMM Power Management ICs.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_qc_pmic.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
03/16/12   hs      Removed obsolete files.
02/10/11   hw      Uncomment pmapp_pmic_events.h header
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/28/03   wra     Created as final PMIC library main include file. Named
                   so that it will not have naming collision with other
                   HLOS header files. Most of the contents in this file
                   were formerly in pm.h
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_lib_api.h"         /* Interface files for all library APIs */
#include "pmapp_rtc.h"          /* RTC apps API        */
#include "pmapp_npa.h"          /* Contains the power node architecture support */


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
/**
   Initializes the PMIC driver for operation. It performs the following tasks:
    - Detects the PMIC revision.
    - Intitializes PMIC specific resource settings from device configuration.
    - Initializes Comms.
    - Initializes driver for MPSS owned resources.
    - Initializes PAM table.

@return
  None.

@dependencies
   dalsys,smem_log,npa,rpm drives must be initialized before calling pm_init.

*/
void pm_init(void);

/** @} */ /* end_addtogroup pm_qc_pmic */

#endif    /* PM_QC_PMIC_H */

