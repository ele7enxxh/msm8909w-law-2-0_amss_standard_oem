/*!
   @file rfc_vreg_mgr.cpp

   @brief


   @details
   This file implements the rfc_vreg_mgr abstract class.

*/
/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/common/src/rfc_vreg_mgr.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/19/12   aak    Changes to change to C++ interface 
04/02/12   sr     Initial revision.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "msg.h"
#include "pm.h"
}
#include "rfc_vreg_mgr.h"


extern "C" {
    extern rf_hw_type rfcommon_nv_get_rf_config(void);
}

/* initialize the single instance static vrge_mgr_ptr to NULL */ 
rfc_vreg_mgr *rfc_vreg_mgr::vreg_mgr_ptr = NULL; 

/*===========================================================================
                           Functions
===========================================================================*/
rfc_vreg_mgr * rfc_vreg_mgr::get_instance(void)
{
  return(vreg_mgr_ptr);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_vreg_mgr::rfc_vreg_mgr(void)
{
    for (uint8 i = RFM_PARKED_MODE; i < RFM_NUM_MODES; i++) 
    {
        rfc_voltage_scaling_state[i] = RFC_VOLTAGE_SCALE_ON;
    }

    rf_hw_id = rfcommon_nv_get_rf_config();
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Destructor.

   @details

*/
rfc_vreg_mgr::~rfc_vreg_mgr()
{
  // Do nothing
}

