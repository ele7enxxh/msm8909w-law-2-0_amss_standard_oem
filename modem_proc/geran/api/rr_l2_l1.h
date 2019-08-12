#ifndef RR_L2_L1_H
#define RR_L2_L1_H

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_l2_l1.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-10-10   sjw     Added support for FEATURE_DUAL_SIM
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
07/08/09   SJW     Removed featurisation from this file, added Ext-BCCH mask
02/10/03   JAC     Added halfrate support
07/23/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support

===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


/**********************************/
/* Channel Type parameter         */
/**********************************/

typedef byte l2_channel_type_T;

#define DCCH                  0x00
#define BCCH                  0x01
#define L2_RACH               0x02
#define CCCH                  0x03
#define SACCH                 0x04
#define SDCCH                 0x05
#define FACCH_F               0x06
#define FACCH_H               0x07
#define L2_RACH_WITH_NO_DELAY 0x08

#define EXTENDED_BCCH_TC_MASK 0xE0

typedef byte geran_result_t;

#define GERAN_ACCESS_GRANTED         0x00
#define GERAN_ACCESS_DENIED_BY_TRM   0x01
#define GERAN_ACCESS_BLANKED_BY_CXM  0x02
#define GERAN_ACCESS_IN_IDLE_STATE   0x03
#define GERAN_ACCESS_FAIL_NO_RETRY   0x0F

#endif /* RR_L2_L1_H */
