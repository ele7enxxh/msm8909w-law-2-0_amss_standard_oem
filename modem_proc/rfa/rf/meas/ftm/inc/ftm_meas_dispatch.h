/*----------------------------------------------------------------------------*/
/*!

  @brief
  This is the header file for the embedded FTM WCDMA Commands 

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/ftm/inc/ftm_meas_dispatch.h#1 $ $DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/09   jps     Created Initial version
*/
/*----------------------------------------------------------------------------*/

#ifndef FTM_MEAS_DISPATCH_H
#define FTM_MEAS_DISPATCH_H

#include "rfa_variation.h"
#include "comdef.h"
#include "ftmicap.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmdiag.h"


typedef PACKED struct 
{
  uint16 error_code;
} ftm_meas_extended_result_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles MEAS FTM requests to run tests and other primitives
   by dispatching the appropriate functions.

   @param
   cmd_ptr - 

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_meas_extended_dispatch(ftm_meas_extended_pkt_type *cmd_ptr);


#endif  /* FEATURE_FACTORY_TESTMODE */
#endif  /* FTM_MEAS_DISPATCH_H */
