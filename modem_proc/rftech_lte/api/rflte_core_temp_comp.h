#ifndef RFLTE_CORE_TEMP_COMP_H
#define RFLTE_CORE_TEMP_COMP_H
/*!
  @file
  rflte_core_temp_comp.h

  @brief
  Provides LTE Temperature Compensation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2010 - 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_core_temp_comp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/22/15   vkm     Autopin PAR support
04/22/14   cri     Update pin pout for therm reads
08/01/13   gvn     Add Feature Flag for Bolt to avoid fw dependency 
07/24/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
07/10/13    pv     Support for Therm read in FTM mode.
07/10/13    pv     Support for Therm bin array.
08/29/13   svi     Core Layer Changes to support UL CA
05/24/13   jf      Add static offset for Pin/Pout comp due to temp at CAL 
05/23/13   bar     Added 2D Matrix support for Tx Limiting vs Temp and vs freq
05/21/13   sbm     K sensor changes   
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
09/20/12   pl      Moved to internal inc directory from API directory
09/07/12   sbm     Updated rflte_core_temp_comp_update_fw_reads_therm();
05/04/12   pl      New MTPL implemenation
12/20/11   cri     Disable HDET and Thermistor reads for NS stop
10/17/11   pl      Support SMPS temp comp
02/28/11   can     Support for ACQ and TRK mode for LTE Thermistor Reads.
10/12/10   can     Support to introduce linear interpolation for LTE temp. comp.
08/11/10   can     Support for LTE TXAGC logging and code clean-up.
05/10/10    dw     Initial version

============================================================================*/
#include "comdef.h"
#include "nv_items.h"

#ifndef FEATURE_BOLT_MODEM
#include "intf_sys.h"
#include "intf_sys_msg.h"
#endif

#include "rfcommon_locks.h"

#define RFLTE_CORE_TEMP_COMP_THERM_RD_RESULT_NUM_TRANS 8
#define RFLTE_CORE_THERM_MAX 255
#define RFLTE_CORE_TEMP_BIN_SIZ (RFLTE_CORE_THERM_MAX / (NV_TEMP_TABLE_SIZ-1))

/*--------------------------------------------------------------------------*/
typedef enum
{
  RFLTE_CORE_TEMP_COMP_MODE_ACQ,
  RFLTE_CORE_TEMP_COMP_MODE_TRK,
  RFLTE_CORE_TEMP_COMP_MODE_INIT,
  RFLTE_CORE_TEMP_COMP_MODE_STOP,
  RFLTE_CORE_TEMP_COMP_MODE_MAX
} rflte_core_temp_comp_mode_type;

/*--------------------------------------------------------------------------*/
typedef struct
{
    uint16 addr; /*only 8bits for now*/
    uint8 value;
} rflte_core_temp_comp_hw_rd_type;

typedef struct
{
    rflte_core_temp_comp_hw_rd_type therm_rd[RFLTE_CORE_TEMP_COMP_THERM_RD_RESULT_NUM_TRANS];
    uint8 num_trans;
} rflte_core_temp_comp_therm_rd_result_type;

/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_init(void);
/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_state_init(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_enable(uint32 carrier_idx, boolean enable);
/*--------------------------------------------------------------------------*/
int16 rflte_core_temp_comp_calc_comp_val(int8 temp_table[], 
                                         uint8 temp_index, 
                                         uint8 temp_rem);
/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_update_fw_reads_therm(uint16 therm_raw_value, 
                                                int8 subframe, 
                                                uint32 carrier_idx,
                                                rflte_core_temp_comp_therm_rd_result_type therm_rd_event_result);
/*--------------------------------------------------------------------------*/
void rflte_core_tdet_temp_comp_update_fw_reads(int8 subframe, 
                                               uint32 carrier_idx,
                                               rflte_core_temp_comp_therm_rd_result_type therm_rd_event_result);
/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_update(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_temp_comp_read_therm(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
#ifndef FEATURE_BOLT_MODEM
void rflte_core_temp_comp_update_iqmc(uint16 therm_raw_value, uint32 carrier_idx, lte_LL1_sys_iqmc_update_req_msg_struct *iqmc_update_req_ptr);
#endif

/* State Machine accessor functions */
void rflte_core_temp_comp_set_timer_enable(uint32 carrier_idx, 
                                           boolean enable);
rflte_core_temp_comp_mode_type rflte_core_temp_comp_get_mode(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
boolean rflte_core_temp_comp_get_update_ready(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
void rflte_core_temp_comp_set_update_ready(uint32 carrier_idx, 
                                           boolean enable);
/*--------------------------------------------------------------------------*/
int16 rflte_core_temp_comp_get_value(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
uint16 rflte_core_temp_comp_get_last_raw(uint32 carrier_idx);
#else
uint16 rflte_core_temp_comp_get_last_raw(void);
#endif 
/*--------------------------------------------------------------------------*/
uint16 rflte_core_temp_comp_get_last_scaled(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_temp_comp_get_last_index(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_temp_comp_get_last_rem(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint16* rflte_core_temp_comp_get_therm_bin_array(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
int8* rflte_core_temp_comp_get_current_pin_offsets(uint32 carrier_index,
                                                   uint8 pa_state);
/*--------------------------------------------------------------------------*/
int8* rflte_core_temp_comp_get_current_pout_offsets(uint32 carrier_index,
                                                   uint8 pa_state);
/*--------------------------------------------------------------------------*/
rf_lock_data_type *rflte_core_temp_comp_get_lock(uint32 carrier_idx);

void rflte_core_temp_comp_update_current_pin_pout
     (
         uint32 carrier_idx, 
        rflte_core_temp_comp_mode_type curr_mode
     );

/* Method to scale a raw therm read value*/
uint16 
rflte_core_temp_comp_raw_to_scaled
(
/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
uint32 carrier_idx,
#endif
uint16 therm_raw_value
);

/* Freq dependent 1D array to hold LTE Static Lim vs freq after interpolating
   over temp from 2D matrix from the NV.
   */
int8 tx_lim_vs_freq_arr[LTE_TX_CAL_CHAN_SIZ];

#endif /* RFLTE_CORE_TEMP_COMP_H */
