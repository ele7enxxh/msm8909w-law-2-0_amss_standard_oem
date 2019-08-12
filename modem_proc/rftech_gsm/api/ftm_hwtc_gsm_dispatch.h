/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

					 F T M  H W T C  G S M  D I S P A T C H
                              

GENERAL DESCRIPTION
  This is the header file for the embedded FTM GSM Dispatch commands 
  comming from the diag service 

Copyright (c) 1994 - 2014      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_hwtc_gsm_dispatch.h#1 $                                  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/14   zbz     removed gsm iq capture function prototype
11/30/12   tws     Correct the dispatch function prototype.
05/11/12   sb      Added changes for SBI dump  
11/29/11   sb      Added functionality for GSM IQ CAPTURE 
07/27/09   rsr     Added debug support for linear.
01/29/03   xw      Created 
===========================================================================*/

#ifndef FTM_HWTC_GSM_DISPATCH_H
#define FTM_HWTC_GSM_DISPATCH_H

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC
#include "ftm_hwtc_cmd.h"

void ftm_hwtc_gsm_dispatch( rfm_device_enum_type rfm_dev,
                            ftm_hwtc_pkt_type *ftm_hwtc);

#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_HWTC_GSM_DISPATCH_H */
