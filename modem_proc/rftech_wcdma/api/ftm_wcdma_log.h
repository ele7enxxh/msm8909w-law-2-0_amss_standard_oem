#ifndef FTM_WCDMA_LOG_H
#define FTM_WCDMA_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM 1X Log

GENERAL DESCRIPTION
  This is the header file for FTM 1X Logging.

Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_wcdma_log.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/04/14   kai     Add xpt logging support
10/04/11   dw      Change APT logging for backwards compatibility
09/06/11   dw      Added APT logging support
02/03/10   kguo    Changed PACKED struct to PACK(struct) 
12/10/09   bn      FTM WCDMA ENH log packet
01/21/08   ad      Changed tx_agc_adj to uint16
02/06/07   ycl     Added log data type support for wcdma txplim.
11/16/06   vm/jfc  Allow FTM logging to be mode independent     
09/16/03   ra      Initial revision based on Saber log source file.	

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"

#ifdef FTM_HAS_LOGGING
#include "ftm_log.h"


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/

/* This type specifies the data packet associated with a WCDMA AGC log */
typedef PACK(struct)
{
  int16             rx_agc;
  int16             tx_agc;
  uint16            tx_agc_adj;
  byte              lna_state;
  byte              pa_state;
  byte              hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_comp_index;
  byte              temp_comp_rem;
} ftm_log_wcdma_agc_data_type;

typedef PACK(struct)
{
  int16             rx_agc;
  int16             tx_agc;
  uint16            tx_agc_adj;
  byte              lna_state;
  byte              pa_state;
  byte              hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_comp_index;
  byte              temp_comp_rem;
  int16             rx_agc_c1;
  byte              lna_state_c1;
} ftm_log_wcdma_agc_c1_data_type;

/* This type specifies the data packet associated with a WCDMA AGC enhanced log with 12-bit HDET and Therm*/
/* This log also contains IQ_gain scale factor, MTPL and TxPDM */
typedef PACK(struct)
{
  int16             rx_agc; 
  int16             tx_agc; 
  uint16            tx_pdm;
  int16             mtpl;    /* Max Tx Power Limit */
  uint16            iq_gain; /* Digital IQ gain scaling */
  byte              lna_state;
  byte              pa_state;
  uint16            hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_comp_index;
  byte              temp_comp_rem;
  int16             hdet;
  int16             smps_bias;
  uint16            icq_current;
  int16             env_scale;
  uint8             xpt_mode;
  uint16            dpd_index;
  int16             pin_comp;
  int16             pout_comp;
  int16             xpt_delay;
} ftm_log_wcdma_agc_enh_data_type;

typedef enum
{
  FTM_LOG_TXPLIM_EVENT_HDET_NO_ADJ       = 0,
  FTM_LOG_TXPLIM_EVENT_HDET_INRANGE_ADJ,
  FTM_LOG_TXPLIM_EVENT_HDET_OVERRANGE_ADJ,
  FTM_LOG_TXPLIM_EVENT_HDET_UNDERRANGE_ADJ
} ftm_log_wcdma_txplim_event_type;

typedef PACK(struct)
{
  ftm_log_wcdma_txplim_event_type     event;            /* event that triggered this log */
  uint8                         hdet_off;         /* current freq adjusted hdet_off */
  uint8                         hdet_span;        /* current freq adjusted hdet_span */
  uint16                        tx_lim_vs_temp;   /* current interpolated lim vs temp */
  int16                         tx_lim_vs_freq;   /* current interpolated lim vs freq */
  uint16                        utran_tx_lim;     /* utran limiting value */
  uint16                        txagc_current;    /* current agc reading */
  uint16                        exp_agc_vs_hdet;  /* agc expected for current hdet reading, offset by 768 */
  uint8                         hdet_reading;     /* current hdet reading */
  int16                         agc_error;        /* measured agc_error based on hdet feedback */
  int16                         beta_correction;  /* beta correction factor for HS */
  int16                         agc_adjustment;   /* adjustment made to txplim */
  uint16                        txplim_next;      /* new txplim value based on hdet adjustment */
  int16                         pa_range_adj;     /* adjustment to pa range values */
} ftm_log_wcdma_txplim_data_type;

/* This type specifies the header/data packet for a WCDMA AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_wcdma_agc_data_type data;
} ftm_log_wcdma_agc_type;

/* This type specifies the header/data packet for a WCDMA AGC C1 log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_wcdma_agc_c1_data_type data;
} ftm_log_wcdma_agc_c1_type;

/* This type specifies the header/data packet for a WCDMA AGC enhanced log with 12-bit HDET and Therm*/
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_wcdma_agc_enh_data_type data;
} ftm_log_wcdma_agc_enh_type;

typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_wcdma_txplim_data_type  data;
} ftm_log_wcdma_txplim_type;

void ftm_log_wcdma_agc_init_func( void);
void ftm_log_wcdma_agc_exit_func( void);
void ftm_log_wcdma_agc_cb(int4 interval);

void ftm_log_dc_cb(int4 interval);

void ftm_log_wcdma_agc_c1_init_func( void);
void ftm_log_wcdma_agc_c1_exit_func( void);
void ftm_log_wcdma_agc_c1_cb(int4 interval);

void ftm_log_wcdma_agc_enh_init_func( void);
void ftm_log_wcdma_agc_enh_exit_func( void);
void ftm_log_wcdma_agc_enh_cb(int4 interval);

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_WCDMA_LOG_H */
