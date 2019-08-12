
#ifndef GMM_SUSPEND_H
#define GMM_SUSPEND_H

/*===========================================================================

         G P R S   S U S P E N S I O N   A N D   R E S U M P T I O N 

DESCRIPTION
  This file contains the data types, function prototypes and data declarations
  required for suspension/resumption of GPRS services.


Copyright (c) 1991 - 2003 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmsuspend.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/02   mks     Created file
12/09/02   mks     Removed gmm_saved_substate declaration.
02/10/03   cd      Fixed Lint errors
07/30/03   mks     Added function prototype gmm_handle_gprs_resumption_during_g2w_ho().
===========================================================================*/

/*===========================================================================
                    I N C L U D E D    H E A D E R     F I L E S   
============================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_GPRS

#include "gmm_v.h"
#include "gmm_int_v.h"
#include "mm_rr.h"

/*===========================================================================
                         D A T A   D E C L A R A T I O N S 
============================================================================*/

/*===========================================================================
                       F U N C T I O N   D E C L A R A T I O N S 
============================================================================*/

/**---------------------------------------------------------------------------
 ** This function handles the RR_GMM_GPRS_SUSPENSION notification from GRR.
 ** (When an MS which is attached for GPRS services enters the dedicated mode
 ** and when the MS limitations make it unable to handle both dedicated mode
 ** and packet transfer mode simultaneously, MS performs GPRS suspension)
 **--------------------------------------------------------------------------*/
void gmm_handle_gprs_suspension ( void);

/**---------------------------------------------------------------------------
 ** This function handles the GPRS resumption information when UE comes out of 
 ** dedicated mode that earlier caused GPRS suspension.
 **--------------------------------------------------------------------------*/
void gmm_handle_gprs_resumption ( mm_cmd_type * );

/**---------------------------------------------------------------------------
 ** This function handles the GPRS resumption during GSM to WCDMA handover 
 **--------------------------------------------------------------------------*/
void gmm_handle_gprs_resumption_during_g2w_ho (void);

/**---------------------------------------------------------------------------
**This function gives the gmm saved substate
**---------------------------------------------------------------------------*/
gmm_substate_type gmm_get_saved_substate(void);

#endif /* #ifdef FEATURE_GSM_GPRS */

#endif /* #ifndef GMM_SUSPEND_H */
