#ifndef RF_TDSCDMA_CORE_TEMP_COMP_H
#define RF_TDSCDMA_CORE_TEMP_COMP_H
/*!
  @file
  rf_tdscdma_core_temp_comp.h

  @brief
  Provides TDSCDMA Temperature Compensation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_core_temp_comp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/09/12   ms      Temp timer
05/27/11   jhe     Initial version

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rf_tdscdma_mdsp.h"


typedef struct
{
  boolean temp_comp_enable_flag;
  uint16 last_raw_temp_value;
  uint8  last_scaled_temp_value;
  uint8 last_temp_comp_index;
  uint8 last_temp_comp_rem;
}rf_tdscdma_core_temp_comp_value_type;


/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_init(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_start(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_stop(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_enable(boolean flag);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_core_temp_comp_is_enabled(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_proc_therm_reading(uint16 therm_raw_value);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_update_txlin(uint16 therm_raw_value,
                                            uint16 tx_chan);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_update_txlim(uint16 therm_raw_value);

/*--------------------------------------------------------------------------*/
rf_tdscdma_core_temp_comp_value_type rf_tdscdma_core_temp_comp_get_therm_data(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_get_pin_pout(uint16 tx_chan,
                                            int16 pin_comp_val[],
                                            int16 pout_comp_val[]);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_core_temp_comp_calc_txlin_vs_temp(int16 txlin_vs_temp[]);

#endif /* RF_TDSCDMA_CORE_TEMP_COMP_H */
