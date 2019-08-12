/*============================================================================
  FILE:         le_consts_trans.h

  OVERVIEW:     LE Constants 
                To be moved fully into gnss/le/src and renamed (to remove
                _trans) after the NF->LE transition is 
                complete.  Constants that still need sharing should be moved to
                e.g. le_iface.h, or gnss/common/inc

  DEPENDENCIES: NA
                
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/inc/le_consts_trans.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-14  wtr  created file

============================================================================*/

#ifndef _LE_CONSTS_TRANS_H
#define _LE_CONSTS_TRANS_H

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// Fixed uncertainty sometimes used for an incoming CPI: 1000m HEPE
#define LE_WIFI_COV_HEPE_FIXED ((FLT)1000.0F)

// For nearly-divide-by-zero sanity checks 
#define LE_SMALL_FLOAT (1.0e-10F)

#endif  /* _LE_CONSTS_TRANS_H */
