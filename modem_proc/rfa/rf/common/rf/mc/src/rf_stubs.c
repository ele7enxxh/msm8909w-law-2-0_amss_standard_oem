/* this is temporary stub file to satisfy the linker errors in 8200 on-target
   build. These functions or variables are referenced from outside of RF Drivers
   */

/*===========================================================================
Copyright (c) 2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rf_stubs.c#1 $
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/11/13   sar     Updated file for APQ, GNSS only target.
12/05/11   sar     Fixed featurization for non-lte build. 
09/21/10   ggs      Moved the api to return rf_warmup time from rf_stubs.c file
                    to rfm_gsm.c
09/21/10   lcl      Changed rf wamup time for GSM to 6ms until RFC structure 
                    is in place 

===========================================================================*/


#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfa_variation.h"
#include "rfm.h"
#include "rfcom.h"
#include "ftm_common.h"
#include "rfm_helper_incs.h"
//#include "clkrgm_modem.h"

/* WCDMA AGC log sample counter */
/*
Error: L6200E: Symbol rf_agc_log_cnt multiply defined (by rf_stubs.o and rfwcdma_core_util.o).
*/
//uint16                      rf_agc_log_cnt = 0;

/* WCDMA AGC log packet */
//WCDMA_AGC_LOG_PACKET_type  rf_agc_log_pkt;
/*
Error: L6200E: Symbol rf_agc_log_pkt multiply defined (by rf_stubs.o and rfwcdma_core_util.o).
*/
//uint16  rf_agc_log_pkt;

typedef struct
{
  uint32   fn;
  uint8    slot_num;
  boolean  bfi;
  uint16   num_bits;
  uint8    *data;
} ftm_gsm_ber_rx_data_ind_type;

typedef enum
{
  RFAGC_LNA_STATE,
  RFAGC_PA_RANGE,
  RFAGC_RX_AGC,
  RFAGC_TX_AGC
} rfagc_dbg_scr_cmd_type; 

typedef struct
{
  uint8     lna_state;
  uint8     pa_range;
  int16     rx_agc;
  int16     tx_agc;
  uint8     lna_state_c1;
  int16     rx_agc_c1;
} rfagc_dbg_scr_params_type;


void rflib_init( void )
{
}

uint16 rfi_get_enc_btf_val(void)
{
    return 10;
}


void rf_turn_off_pa(void)
{
}

/*void ftm_egprs_ber_send_msg_to_ftm( void *msg)
{
Error: L6200E: Symbol ftm_egprs_ber_send_msg_to_ftm multiply defined (by rf_stubs.o and ftm_egprs_ber.o).
}*/

//void *ftm_egprs_ber_get_cmd_buf( void)    
//{
       /* Get a buffer from the Free Q */

//    void *msg = NULL;
//
//    return( msg);
//}

int16 rf_convert_rx_agc_to_dbm_unit( int16 rxagcval )
{
    return 10;
}

int16 rf_convert_tx_agc_to_dbm_unit( int16 txagcval )
{
    return 10;
}

void rfagc_read_after_mdsp_check ( rfagc_dbg_scr_cmd_type cmd, 
                                   rfagc_dbg_scr_params_type *params )
{
}

#ifdef IMAGE_APPS_PROC
extern boolean rfm_get_rf_dbg_scr_param
(
  rfcom_dbg_scr_param_type param,
  rfcom_dbg_scr_param_data_type *pBuf
);
#else
boolean rfm_get_rf_dbg_scr_param
(
  rfcom_dbg_scr_param_type param,
  rfcom_dbg_scr_param_data_type *pBuf
)
#endif
{
    return 1;
}

/*void ftm_put_wl1_cmd(void *cmd_ptr)
{
Symbol ftm_put_wl1_cmd multiply defined (by rf_stubs.o and ftm_wcdma_ber.o). 
}*/

/*void ftm_gsm_ber_send_rx_data_to_ftm( ftm_gsm_ber_rx_data_ind_type *rx_data_ind)
{
Error: L6200E: Symbol ftm_gsm_ber_send_rx_data_to_ftm multiply defined (by rf_stubs.o and ftm_gsm_ber.o)
}*/


void rfm_init_bbrx_sleep(void)
{
/*Stub function to satisfy linker error from mc.c 
Error: L6218E: Undefined symbol rfm_init_bbrx_sleep (referred from mc.o).
*/
}


#ifndef FEATURE_LTE
boolean ftm_lte_iq_logpkt_enable;
void ftm_log_lte_iq(uint32 size_kB)
{
}
#endif


#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
