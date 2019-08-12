#ifndef FTM_COMMON_LOG_H
#define FTM_COMMON_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM LTE Log

GENERAL DESCRIPTION
  This is the header file for FTM LTE Logging.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_log.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/05/13   gvn     Initial version supporting IRAT timing log pkt

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"

#ifdef FTM_HAS_LOGGING
#include "ftm_log.h"

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/

/* Data type for IRAT timing pkt. Question: Does the data need to be per path?*/
typedef PACK(struct)
{
  uint16 num_trx_transactions;
  uint16 num_non_trx_transactions;
  uint16 estimated_exe_time;
} ftm_log_irat_timing_data_type;

/* This type specifies the header/data packet for an FTM IRAT Timing pkt */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_irat_timing_data_type data;
} ftm_log_irat_timing_type;

void ftm_log_common_irat_timing( uint16 num_trx_trans, uint16 num_non_trx_trans, uint16 estimated_exe_time );

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_COMMON_LOG_H */
