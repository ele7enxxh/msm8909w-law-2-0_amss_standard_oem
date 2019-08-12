#ifndef FTM_LTE_LOG_H
#define FTM_LTE_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM LTE Log

GENERAL DESCRIPTION
  This is the header file for FTM LTE Logging.

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_lte_log.h#1 $ 
  $DateTime: 2016/12/13 07:59:25 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/06/14   ars     Fix enhanced Rx AGC logging to remove device dependency during logging. 
08/15/14   cri     Added support for 16bit ICQ and DPD idx
02/12/14   jf      Add xpt logging for ENH TXAGC
09/12/13   bar     Added support for FTM Enhanced Log packet 
04/23/13   gvn     Remove featurization to fix compiler error 
02/26/13   gvn     Add new LTE FTM log for CA enabled RxAGC 
05/11/12   aca     16SC IQ capture
12/05/11   whc     LTE SMPS logging / Enhanced TXAGC log support
11/30/10   mkv     Updating the LTE Tx Log packet with Tx freq compensation and temp compensation values. 
08/16/10   mkv     Populating the Rx and Tx Log packet structures by calling the MDSP, RF core functions. 
08/10/10   mkv     Adding MPR and Network signaled value entries in the Tx log packet. 
07/28/10   mkv     Intial version of LTE Tx and Rx AGC log packets. 

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"

#ifdef FTM_HAS_LOGGING
#include "ftm_log.h"

#define MAX_NUM_TX_PATHS 4
#define MAX_NUM_RX_PATHS 4
#define MAX_NUM_CA_RX_PATHS 10

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/
typedef enum
{
  FTM_LTE_LOG_TXPLIM_EVENT_HDET_NO_ADJ       = 0,
  FTM_LTE_LOG_TXPLIM_EVENT_HDET_INRANGE_ADJ,
  FTM_LTE_LOG_TXPLIM_EVENT_HDET_OVERRANGE_ADJ,
  FTM_LTE_LOG_TXPLIM_EVENT_HDET_UNDERRANGE_ADJ
} ftm_log_lte_txplim_event_type;

typedef PACK(struct)
{
  ftm_log_lte_txplim_event_type     event;            /* event that triggered this log */
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
} ftm_log_lte_txplim_data_type;

typedef PACK(struct)
{
  uint32 iq_data[128];

} ftm_log_lte_iq_data_type;

typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_iq_data_type  data;
} ftm_log_lte_iq_msg_type;

/* This type specifies the data packet associated with the Tx LTE AGC log */
typedef PACK(struct)
{
 byte              version;         /* Log packet version. As additional paramters are added the version number can increase. */
 byte              tx_chain;        /* Indicates the Tx chain - Primary, Div etc to support MIMO Tx. */
 byte              start_rb_index;  /* Start RB Index. */
 byte              num_rbs;         /* Number of RBs. */
 byte              pa_range;        /* PA Range. */
 uint16            tx_gain_index;   /* Gain Index - equivalent of PDM. */
 int32             tx_agc;          /* Tx AGC value corresponding to the gain index from NV tables. */
 int32             tx_freq_comp;	/* Freq compensation applied to obtain the above tx agc*/
 int32             tx_temp_comp;	/* Temp compensation applied to obtain the above tx agc*/
 int32             mtpl;            /* Max Tx Power Limit. */
 uint16            iq_gain;         /* Digital IQ gain scaling. */
 byte              mpr;             /* Max Power Reduction from the 36.101 specification document. */         
 byte              ampr;            /* Additional Max Power Reduction from the 36.101 specification document. */         
 byte              ns_value;        /* Network signalled value - NS01, NS02, .. NS07 etc. */         
 int16             sar_max_power_limit;       /* SAR Max Power Reduction. */         
 byte              pdet_mode;       /* Indicates the power detector mode. 0 - High Power Mode and 1 - Low Power Mode. */         
 uint16            pdet_adc;        /* Indicates the power detector ADC value. */         
 int16             pdet_dBm;        /* Indicates the power detector ADC value in dBm units from the HDET_VS_AGC NV. */         
 uint16            therm_raw;       /* Raw thermistor 12-bit ADC value. */         
 byte              therm_scaled;    /* Scaled thermistor 8-bit value. */  
 byte              temp_comp_index; /* Temp compensation bin value obtained by therm_scaled/32. */  
 byte              temp_comp_rem;   /* Temp reminder bin value obtained by therm_scaled/32. */ 
} ftm_log_lte_tx_agc_data_path_type;

/* This type specifies the data packet associated with the Tx LTE AGC ENH log */
typedef PACK(struct)
{
 byte              version;         /* Log packet version. As additional paramters are added the version number can increase. */
 byte              tx_chain;        /* Indicates the Tx chain - Primary, Div etc to support MIMO Tx. */
 byte              start_rb_index;  /* Start RB Index. */
 byte              num_rbs;         /* Number of RBs. */
 byte              pa_range;        /* PA Range. */
 uint16            tx_gain_index;   /* Gain Index - equivalent of PDM. */
 int32             tx_agc;          /* Tx AGC value corresponding to the gain index from NV tables. */
 int32             tx_freq_comp;	/* Freq compensation applied to obtain the above tx agc*/
 int32             tx_temp_comp;	/* Temp compensation applied to obtain the above tx agc*/
 int32             mtpl;            /* Max Tx Power Limit. */
 uint16            iq_gain;         /* Digital IQ gain scaling. */
 byte              mpr;             /* Max Power Reduction from the 36.101 specification document. */         
 byte              ampr;            /* Additional Max Power Reduction from the 36.101 specification document. */         
 byte              ns_value;        /* Network signalled value - NS01, NS02, .. NS07 etc. */         
 int16             sar_max_power_limit;       /* SAR Max Power Reduction. */         
 byte              pdet_mode;       /* Indicates the power detector mode. 0 - High Power Mode and 1 - Low Power Mode. */         
 uint16            pdet_adc;        /* Indicates the power detector ADC value. */         
 int16             pdet_dBm;        /* Indicates the power detector ADC value in dBm units from the HDET_VS_AGC NV. */         
 uint16            therm_raw;       /* Raw thermistor 12-bit ADC value. */         
 byte              therm_scaled;    /* Scaled thermistor 8-bit value. */  
 byte              temp_comp_index; /* Temp compensation bin value obtained by therm_scaled/32. */  
 byte              temp_comp_rem;   /* Temp reminder bin value obtained by therm_scaled/32. */ 
 uint16            pa_smps_pdm;     /* Raw 10 bit PA SMPS value used by FW */
} ftm_log_lte_tx_agc_enh_data_path_type;

typedef PACK(struct)
{
  uint8 current_xpt_mode;
  boolean et_flag;
  boolean xpt_transition;
  uint8 dpd_cfg;
  uint8 am_tbl_size;
  uint8 pm_tbl_size;
  uint8 det_tbl_size;
  uint16 am_offsets;
  uint16 am_bank_id;
  uint16 pm_offsets;
  uint16 pm_bank_id;
  uint16 det_offsets;
  uint16 det_bank_id;
  uint16 iq_gain_lut;
  int32 p_out_comp;
  int32 p_in_comp;
  uint32 selected_rgi_with_pout;
  uint32 selected_rgi_with_pin;
  uint32 env_scale_backoff;
  uint32 iq_backoff;
  int32  delay;
  uint32 dpd_indx;
  uint8 pa_icq_val;
}ftm_log_lte_tx_xpt_data_path_type;

/* This type specifies the data packet for all Tx chains in the LTE Tx AGC log */
typedef PACK(struct)
{
  uint8 num_tx_paths;
  ftm_log_lte_tx_agc_data_path_type data_path[MAX_NUM_TX_PATHS];
}ftm_log_lte_tx_agc_data_type;

/* This type specifies the data packet for all Tx chains in the LTE Tx AGC log */
typedef PACK(struct)
{
  uint8 num_tx_paths;
  ftm_log_lte_tx_agc_enh_data_path_type data_path[MAX_NUM_TX_PATHS];
  ftm_log_lte_tx_xpt_data_path_type xpt_path[MAX_NUM_TX_PATHS];
  uint32 ext_pa_icq_val[MAX_NUM_TX_PATHS];
}ftm_log_lte_tx_agc_enh_data_type;

/* This type specifies the header/data packet for a LTE Tx AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_tx_agc_enh_data_type data;
} ftm_log_tx_agc_enh_type;

/* This type specifies the header/data packet for a LTE Tx AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_tx_agc_data_type data;
} ftm_log_tx_agc_type;

/* This type specifies the data packet associated with the Rx LTE AGC log */
typedef PACK(struct)
{
  byte              version;       /* Log packet version. As additional paramters are added the version number can increase. */
  byte              rx_chain;      /* Indicates the Rx chain - Primary, Div etc to support MIMO Rx. */
  byte              lna_state;     /* Reciever gain state. */
  int16             rx_agc;        /* Rx AGC value. */
} ftm_log_lte_rx_agc_data_path_type;

typedef PACK(struct)
{
  uint8 num_rx_paths;
  ftm_log_lte_rx_agc_data_path_type data_path[MAX_NUM_RX_PATHS];    /* Rx AGC Log packet per path. */
} ftm_log_lte_rx_agc_data_type;

/* This type specifies the header/data packet for a LTE Rx AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_rx_agc_data_type data;
} ftm_log_rx_agc_type;

/* This type specifies the data packet associated with the CA Enabled Rx LTE AGC log */
typedef PACK(struct)
{
  byte              version;       /* Log packet version. As additional paramters are added the version number can increase. */
  byte              rx_chain;      /* Indicates the Rx chain - Primary, Div etc to support MIMO Rx. */
  byte              lna_state;     /* Reciever gain state. */
  int16             rx_agc;        /* Rx AGC value. */
  uint32            reserved;      /* 4 bytes reserved for future use */
} ftm_log_lte_ca_enabled_rx_agc_data_path_type;

typedef PACK(struct)
{
  uint8 num_ca_rx_paths;
  ftm_log_lte_ca_enabled_rx_agc_data_path_type data_path[MAX_NUM_CA_RX_PATHS];    /* Rx AGC Log packet per path. */
} ftm_log_lte_ca_enabled_rx_agc_data_type;

/* This type specifies the header/data packet for CA enabled LTE Rx AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_ca_enabled_rx_agc_data_type ca_data;
} ftm_log_ca_enabled_rx_agc_type;

typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_lte_txplim_data_type  data;
} ftm_log_lte_txplim_type;

void ftm_log_lte_tx_agc_init_func( void);
void ftm_log_lte_tx_agc_exit_func( void);
void ftm_log_lte_tx_agc_cb(int4 interval);

void ftm_log_lte_tx_agc_enh_init_func( void);
void ftm_log_lte_tx_agc_enh_exit_func( void);
void ftm_log_lte_tx_agc_enh_cb(int4 interval);

void ftm_log_lte_rx_agc_init_func( void);
void ftm_log_lte_rx_agc_exit_func( void);
void ftm_log_lte_ca_enabled_rx_agc_init_func( void );
void ftm_log_lte_ca_enabled_rx_agc_exit_func( void );
void ftm_log_lte_rx_agc_cb(int4 interval);
void ftm_log_lte_ca_enabled_rx_agc_cb(int4 interval);
void ftm_log_lte_ca_enabled_rx_agc_initialize( void );
void ftm_log_dc_cb(int4 interval);
void ftm_log_lte_iq(uint32 size_kB);

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_LTE_LOG_H */
