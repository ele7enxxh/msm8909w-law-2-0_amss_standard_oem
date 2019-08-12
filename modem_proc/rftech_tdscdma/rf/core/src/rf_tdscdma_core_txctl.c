/*!
  @file
  rf_tdscdma_core_txctl.c

  @brief
  Provides TX control. Example PA ctl is handled here.

  @details


*/

/*===========================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_txctl.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/22/13   jyu     Merged from NikeL
04/24/12   jfc     Remove unused headers
05/27/11   jhe     Initial version

============================================================================*/

#include "rf_tdscdma.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_txctl.h"
#include "rf_tdscdma_mdsp.h"
#include "ftm.h"



#if 0
// it is included in TxLM static, to delete
/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function enables/disables the PA_ON enable bit for TDSCDMA Tx.

*/
void rf_tdscdma_core_txctl_set_tx_modulator(boolean enable)
{
  /* This should not be here */
  hw_pa_on_enable(enable);
}
#endif



//TODO - for below 2 funct, will remove and let the caller call mdsp api directly

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function freeze/unfreezes the TX accumulator.

  @details
*/
int16 rf_tdscdma_core_txctl_set_tx_freezeaccum(uint8 val)
{
  //rf_tdscdma_mdsp_set_txfreezeaccum(val);

  return 0;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function enables UL closed loop power control by removing the TX AGC
  gain value override.

  @details
*/
void rf_tdscdma_core_txctl_enable_pwr_ctrl(void)
{
  //rf_tdscdma_mdsp_enable_pwr_ctrl();
}

