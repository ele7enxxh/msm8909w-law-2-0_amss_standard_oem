#ifndef TDSL1_QSH_EXT_H
#define TDSL1_QSH_EXT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            TDSL 1 QSH EXT . H                 

GENERAL DESCRIPTION
   Contains declarations for function which are QUALCOMM proprietary 
   and may or may not be shipped as source code.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
2/08/14   AH     Initial version for tdsl1_qsh_ext.h

===========================================================================*/
//#include "tdscdma_variation.h"
#include "comdef.h"
#include <string.h>
#include "trm.h"
#include <qsh.h>
	
#define TDSL1_QSH_MAJOR_VER 0
#define TDSL1_QSH_MINOR_VER 1

#ifdef FEATURE_QSH_DUMP

/*placeholder for minidump support if needed. */

#endif /* FEATURE_QSH_DUMP */


#ifdef FEATURE_QSH_EVENT_METRIC
/*-------------------------------------------------------------------------*/
/*                  Debug event/metrics info Struct type declaration                      */
/*-------------------------------------------------------------------------*/

typedef enum
{
  TDSL1_QSH_METRIC_SERV_CELL_RF,
  TDSL1_QSH_METRIC_DL_BLER,
  TDSL1_QSH_METRIC_UL_RACH,
  TDSL1_QSH_METRIC_UL_PRACH_PWR,
  TDSL1_QSH_METRIC_CIR_SNR,
  TDSL1_QSH_METRIC_UL_DPCH_TX_PWR,
  TDSL1_QSH_METRIC_SERV_CELL_PWR,
  TDSL1_QSH_METRIC_MAX
}tdsl1_qsh_metric_e;

typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
   /* primary frequency */
  uint16  primary_freq; 
  /* working frequency */
  uint16  working_freq;
  /* Cell Parameter id */
  uint16  cpi;
  /* serving cell's RSCP,unit 1/256dBm(we can also make the unit to dBm according to requirment) */
  int16 tdsl1_serv_cell_rscp;
}tdsl1_qsh_metric_serv_cell_rf_s;


typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  uint32 dch_crc_count;
  uint32 dch_crc_error_count;

  uint32 bch_crc_count;
  uint32 bch_crc_error_count;

  uint32 pch_crc_count;
  uint32 pch_crc_error_count;

  uint32 fach_crc_count;
  uint32 fach_crc_error_count;

  uint32 pdsch_crc_count;
  uint32 pdsch_crc_error_count;
}tdsl1_qsh_metric_dl_bler_s;


/* Struct to store the PRACH statistics for event 
** logging information for a given sample period
*/
typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  /* Number of PRACH  attempted per Sampling time */
  uint8   num_prach_attempts_in_sample;						
  /* Number of PRACH Preambles with NO ACK per Sampling time */
  uint8   num_fpach_no_ack_in_sample;				
  
}tdsl1_qsh_metric_ul_rach_s; 

typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

  /* Average TX power used for PRACH in the sampling period in dBm */
  int16    avg_prach_tx_pwr_per_sample;   
  
}tdsl1_qsh_metric_ul_prach_pwr_s;

/*Structure to store TDS CIR/SNR statistics for
**event logging information for a given sample period
*/
typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  /*searcher team to fill in */
  /* 1/10dBm after divide 512  range[-150,127] */
  int16 tdsl1_serv_cell_snr;

}tdsl1_qsh_metric_cir_snr_s;		

/* Struct to store the Average TX Power, UL DPCCH Pwr, UPH and 
** EUL SG and NACK Information in a given sample rate of 100ms
*/
typedef struct {
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  /* Average UL DPCH TX Power per Sampling time in dBm */
  int16    avg_ul_dpch_pwr_per_sample;
}tdsl1_qsh_metric_ul_dpch_tx_pwr_s;   

/*Structure to store serving cell RSCP/RSSI statistics for
**event logging information for a given sample period
*/
typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  /* serving cell's RSCP. Valid value ranges (-120..-25) dbm*/
  int16 serv_cell_rscp;
  /*UTRA carrier RSSI. Valid value ranges (-100..-25) dbm*/
  int8  rssi;
}tdsl1_qsh_metric_serv_cell_pwr_s;		


#endif /* FEATURE_QSH_EVENT_METRIC */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Functions                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#endif /* TDSL1_QSH_EXT_H */

