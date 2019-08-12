/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       F T M  H W T C  D V T  S T R I N G  C O M M A N D S  M O D U L E

GENERAL DESCRIPTION
  This is the FTM HWTC DVT string commands module, contains core command 
  and it also serves as an example for how to add commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2013 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_hwtc_dvt_wcdma_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12-17-03   dp      Created
===========================================================================*/
#ifndef FTM_HWTC_DVT_WCDMA_BER_H
#define FTM_HWTC_DVT_WCDMA_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "queue.h"
#include "l1rrcif.h"


#ifdef FEATURE_FTM_HWTC
#ifdef FEATURE_FTM_WCDMA_BER

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
typedef uint16 ftm_wcdma_arfcn;


typedef struct
{
    l1_dl_cctrch_info_struct_type    *dl_cctrch_info;
    l1_dl_ctfc_info_struct_type      *dl_ctfc_info;
    l1_dl_phychan_db_struct_type     *add_dl_phychan_db;
    boolean                          isInitialized;
} ftm_hwtc_wcdma_ber_dl_db_type;


typedef struct
{
    l1_ul_cctrch_cfg_struct_type      *ul_cctrch_info;
    l1_ul_phychan_cfg_struct_type     *add_ul_phychan_db;
    mac_ul_tfcs_type                  *tfcs_ptr;
    boolean                           isInitialized;
} ftm_hwtc_wcdma_ber_ul_db_type;

















void ftm_hwtc_dvt_wcdma_ber_init( void);

#endif /* FEATURE_FTM_WCDMA_BER */
#endif /* FEATURE_FTM_HWTC */
#endif /* FTM_HWTC_DVT_WCDMA_BER_H */
