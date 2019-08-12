/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/ftm_umts_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/11   sar     Removed condition #def's for CMI-4.
10-12-05   dp      Created
===========================================================================*/
#ifndef FTM_UMTS_BER_H
#define FTM_UMTS_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"






/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

typedef enum
{
    FTM_WCDMA_BER_STATE_NULL = 0,
    FTM_WCDMA_BER_STATE_IDLE,
    FTM_WCDMA_BER_STATE_ACQ,
    FTM_WCDMA_BER_STATE_DCH_PENDING,
    FTM_WCDMA_BER_STATE_DCH,
    FTM_WCDMA_BER_STATE_DCH_RECONFIG_PENDING,
    FTM_WCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING

} ftm_wcdma_ber_state_type;

ftm_wcdma_ber_state_type ftm_wcdma_ber_get_state( void);





/* This type specifies the possible FTM GSM BER states */
typedef enum
{
    FTM_GSM_BER_STATE_STOPPED      = 0,
    FTM_GSM_BER_STATE_NULL,
    FTM_GSM_BER_STATE_ACQ_BCCH,
    FTM_GSM_BER_STATE_IDLE,
    FTM_GSM_BER_STATE_TCH,

    FTM_GSM_BER_STATE_GPRS_IDLE,
    FTM_GSM_BER_STATE_GPRS_PDTCH, 
    FTM_GSM_BER_INVALID_STATE

} ftm_gsm_ber_state_type;

ftm_gsm_ber_state_type ftm_gsm_ber_get_state( void);




#endif /* FTM_UMTS_BER_H */
