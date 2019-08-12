#ifndef FTM_TDSCDMA_LOG_H
#define FTM_TDSCDMA_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM 1X Log

GENERAL DESCRIPTION
  This is the header file for FTM 1X Logging.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/ftm_tdscdma_log.h#1 $ 
  $DateTime: 2016/12/13 07:59:50 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/17/12   hy      Revise log packet comments 
07/16/12   hy      Add TD-SCDMA FTM Rx AGC and Tx AGC logs. 

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

/* This type specifies the data packet associated with a TDSCDMA AGC log */
/* This type specifies the data packet associated with a TDSCDMA AGC enhanced log with 12-bit HDET and Therm*/

/*---------------------------------------------------------------------------*/
/*                        TFW RF AGC Log                                     */
/*---------------------------------------------------------------------------*/
/**
    \defgroup rfAgcLog RF AGC LOG
    \ingroup rfSharedMemoryInterface
    @verbatim
    This section describes the RF AGC log
    @endverbatim
    */
/*\{*/
typedef PACK(struct)
{
  /*! \brief Received power in dBm*256
  rxPowerdBm = ."rxPowerdBmData" / 256.0f*/  
  int32 rxPowerdBm;
  /*! \brief LNA state */
  uint8 lnaState;
  /*! \brief DVGA gain */  
  int16 rxfeDvgaGain;
  
  int16 reserved[5];
  //RSCP
} TFW_RX_AGC_RESULT_PER_CHAIN_T;

typedef PACK(struct)
{
  /*! \brief subframe number */
  uint16 sfn;
  /*! \brief slot number */
  uint16 slot;
  /*! \brief antenna selection, 0:ant0, 1:ant1, 2:both */
  uint8 antennaSelect;
  /*! \brief operation frequency, Hz */
  uint32 operationFreq;
  /*! \brief frequency error, 32Q8, Hz */
  int32 freqError;
  /*! \brief rotator adjust */
  int32 rotatorVal;
  TFW_RX_AGC_RESULT_PER_CHAIN_T chains[2];
} TFW_RX_AGC_RESULT_BUF_T;





typedef PACK(struct)
{
   /*! \brief Desired power from SW dBm*16
   ."desiredTxPwrData" / 16.0f;*/
   int16   desiredTxPwrSW;
   /*! \brief actual TX power dBm*16
   ."actualTxChanPowerData" / 16.0f*/
   int16 actualTxChanPower;
   /*! \brief computed TX power dBm*16
   ."computedTxChanPowerData" / 16.0f*/
   int16 computedTxChanPower;
   /*! \brief  beta gain for data part of the physical channel
   (."betaDataData" / 32768.0f)*/
   int16    betaData;
   /*! \brief  beta gain for midamble
   (."betaMidData" / 32768.0f)*/
   int16    betaMid;
   /*! \brief PA backoff dB*16
    ."paBackoffData" / 16.0f*/
   uint16   paBackoff;
   int16 reserved[5];
} TFW_TX_AGC_RESULT_PER_CHANN_T;

typedef PACK(struct)
{
  /*! \brief subframe number */
  uint16 sfn;
  /*! \brief slot number */
  uint16 slot;
  /*! \brief transmission frequency*/
  uint32 frequency;
  /*! \brief pathloss
  pathloss/16.0 dBm*/  
  uint32 pathloss;
  /*! \brief Total TX power dBm*16
  ."txOutputPowerData" / 16.0f*/ 
  int16 txOutputPower;
  /*! \brief maximum power ratio dBm*512
  mpr/512 dBm*/
  int16 mprValue;
  /*! \brief analog power dBm*512
  analogPwr/512 dBm*/
  int16 analogPwr;
  /*! \brief backoff at DAC dB*512
  dacBackoff/512 dB*/
  int16 dacBackoff;
  /*! \brief power compensation of temperature and frequency dB*512
  tempFreqPwrComp/512 dB*/
  int16 tempFreqPwrComp;
  /*! \brief MTPL, Max transmit power limit dBm*512
  mtpl = ."mtplData" / 512.0f*/
  uint16 mtpl;
  /*! \brief Tx gain entry, 0-63*/
  uint16 rfaGainLutIndex;
  /*! \brief PA Gain State, 0-3*/
  uint16 paGainState;
  /*! \brief Flag indicating min power limits reached*/
  boolean minPwrLimited;
  /*! \brief Flag indicating max power limits reached*/
  boolean maxPwrLimited;
  /*! \brief Flag indicating transmit power override*/
  boolean txPwrOverrideFlag;
  /*! \brief Transmit power override value*/
  int16 txPwrOverrideValue;

  /*! \brief rotator adjust */
  int32 rotatorVal;
  /*! \brief time advance */
  int32 TimingAdvance;

  /*! \brief per chan info*/
  TFW_TX_AGC_RESULT_PER_CHANN_T chans[2];
} TFW_TX_AGC_RESULT_BUF_T;

typedef PACK(struct)
{
  uint8 Version;

  TFW_RX_AGC_RESULT_BUF_T RxAgcStruc;

  int16        reserved[5];
} ftm_log_tdscdma_rx_agc_data_type;

typedef PACK(struct)
{
  uint8 Version;

  TFW_TX_AGC_RESULT_BUF_T TxAgcStruc;

  uint16            hdet_raw;
  int16             hdet;  
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_comp_index;
  byte              temp_comp_rem;
  int16             smps_bias;
  int16             icq_current;
  int16             reserved[4];
} ftm_log_tdscdma_tx_agc_data_type;


typedef enum
{
  FTM_LOG_TDSCDMA_TXPLIM_EVENT_HDET_NO_ADJ       = 0,
  FTM_LOG_TDSCDMA_TXPLIM_EVENT_HDET_INRANGE_ADJ,
  FTM_LOG_TDSCDMA_TXPLIM_EVENT_HDET_OVERRANGE_ADJ,
  FTM_LOG_TDSCDMA_TXPLIM_EVENT_HDET_UNDERRANGE_ADJ
} ftm_log_tdscdma_txplim_event_type;

typedef PACK(struct)
{
  ftm_log_tdscdma_txplim_event_type     event;            /* event that triggered this log */
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
} ftm_log_tdscdma_txplim_data_type;

/* This type specifies the header/data packet for a TDSCDMA AGC log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_tdscdma_rx_agc_data_type data;
} ftm_log_tdscdma_rx_agc_type;

/* This type specifies the header/data packet for a TDSCDMA AGC C1 log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_tdscdma_tx_agc_data_type data;
} ftm_log_tdscdma_tx_agc_type;


typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_log_tdscdma_txplim_data_type  data;
} ftm_log_tdscdma_txplim_type;

void ftm_log_tdscdma_rx_agc_init_func( void);
void ftm_log_tdscdma_rx_agc_exit_func( void);
void ftm_log_tdscdma_rx_agc_cb(int4 interval);

void ftm_log_tdscdma_tx_agc_init_func( void);
void ftm_log_tdscdma_tx_agc_exit_func( void);
void ftm_log_tdscdma_tx_agc_cb(int4 interval);

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_TDSCDMA_LOG_H */
