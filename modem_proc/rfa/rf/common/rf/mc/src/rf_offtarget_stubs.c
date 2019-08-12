/* 
  File that stores all RF off target stubs. 
*/
/*===========================================================================
Copyright (c) 2012 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rf_offtarget_stubs.c#1 $
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/11/14   shb     Deleted atomic_set_bit to fix oft linker error
03/11/13   sar     Updated file for APQ, GNSS only target.
06/20/12   sty     deleted rfm_init_bbrx_sleep 
06/14/12   zhw     Initial Revision
===========================================================================*/


#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
