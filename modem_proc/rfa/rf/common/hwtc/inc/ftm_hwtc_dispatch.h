#ifndef FTM_HWTC_DISPATCH_H
#define FTM_HWTC_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F T M  H W T C  D I S P A T C H   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM HWTC Commands 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/hwtc/inc/ftm_hwtc_dispatch.h#1 $ $DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/10/14   vv      Added support for THERM
05/12/14   hs      Added support for WTR DEVICE COMMON  
03/27/14   zhw     Added support for HDET 
03/18/14   vv      Added support for QCOUPLER
02/21/14   yb      Added support for QTUNER
11/20/13   tks     Added support for WTR GSM 
10/29/13   tks     Added support for WTR TDSCDMA
10/29/13   sr      Added support for WTR LTE
10/25/13   sr      Added support for WTR CDMA and WCDMA
09/30/13   aca     Device verification HWTC API
09/14/12   jfc     Move RFFE HWTC to regular FTM common area 
08/17/12   jfc     Add RFFE command code 
09/01/09   tnt     Move INO code from hwtc to ftm
08/20/09   tnt     Add FTM_HWTC_INO_C for INO block
03/21/05    xw     Removed FTM_HAS_ASYNC_PKT
09/09/04   lcl     Changed ftm_hwtc_dispatch() prototype.
01/30/03    xw     Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC

#include "ftmdiag.h"
#include "ftm.h"

#define   FTM_HWTC_DVT_STR      3
#define   FTM_HWTC_IQ_CAPTURE_C 4
#define   FTM_HWTC_AGPS_C       5
#define   FTM_HWTC_SBI_C        6
#define   FTM_HWTC_DC           7
#define   FTM_HWTC_ASM          8
#define   FTM_HWTC_PA           9
#define   FTM_HWTC_PAPM         10
#define   FTM_HWTC_TRX_CDMA     11
#define   FTM_HWTC_TRX_WCDMA    12
#define   FTM_HWTC_TRX_LTE      13
#define   FTM_HWTC_TRX_TDSCDMA  14
#define   FTM_HWTC_TRX_GSM      15
#define   FTM_HWTC_ATUNER       16
#define   FTM_HWTC_COUPLER      17
#define   FTM_HWTC_CDMA_HDET    18 
#define   FTM_HWTC_XSW          19 
#define   FTM_HWTC_TRX_COMMON   20 
#define   FTM_HWTC_THERM        21 

ftm_rsp_pkt_type ftm_hwtc_dispatch(void  *cmd_ptr);

#endif /* FEATURE_FTM_HWTC */
#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_HWTC_DISPATCH_H
