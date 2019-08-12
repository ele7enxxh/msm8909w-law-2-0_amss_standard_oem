#ifndef RF_TDSCDMA_CORE_TXCTL_H
#define RF_TDSCDMA_CORE_TXCTL_H

/*!
   @file
   rf_tdscdma_core_txctl.h

   @brief
   Provides TX control. Example PA ctl is handled here.

   @details


*/

/*===========================================================================

Copyright (c) 2011, 2013, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/inc/rf_tdscdma_core_txctl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/14   jyu     Added support for therm read 
02/22/13   jyu     Merged from NikeL 
05/27/11   jhe     Initial version

============================================================================*/
#include "comdef.h"
#include "rfcom.h"


/*----------------------------------------------------------------------------*/
void rf_tdscdma_core_txctl_enable_pwr_ctrl(void);

/*----------------------------------------------------------------------------*/
int16 rf_tdscdma_core_txctl_set_tx_freezeaccum(uint8 val);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_timer(int4 t_unused);

#endif /* RF_TDSCDMA_CORE_TXCTL_H */
