#ifndef GERAN_API_RR_L2_H
#define GERAN_API_RR_L2_H

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_api_rr_l2.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11-02-10   sjw     Enhanced support for G+G paging collision
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
10-03-30   tjw     Explicitly include dependent headers to make SU API files
                   stand alone
02/11/03   JAC     Added short header support
02/14/03   RG      Added support for G2W blind Handover
08/05/02   rmc     Changed suspend_reason to forced_suspend for dl_suspend_req_T
07/29/02   Rao     Added suspend reason in dl_suspend_req for InterRAT HO
05/30/02   rmc     Removed #define for N201_MAX already defined in sys_cnst.h
04/29/02   rmc     Changed DL_UNIT_DATA_IND to include TC value

===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

/*********************************/
/* Defines the Frame Number type */
/* This frame number is a modulo */
/* 42432 number                  */
/*********************************/

typedef struct
{
   uint8 msb;
   uint8 lsb;
} FRAME_NUMBER_T;

#endif



