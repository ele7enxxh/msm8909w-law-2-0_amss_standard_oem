#ifndef GL1_DEFS_G_H
#define GL1_DEFS_G_H
/*========================================================================
                 GSM GPRS NPL1 DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by both the GSM/GPRS NPL1
   Message Layer and the Frame layer.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_defs_g.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
14/10/13	pa		CR539505:Bolt: Added support for Dynamic Power Scheduling 
29/07/13    sk      CR519663 Partial QBTA bringup changes
20/05/13    cs      TSTS Updates
07/11/12    ky      CR 418406 - Corrected the FIVE DB SNR macro from 81 to 810
19/04/12    ws      CR 353512 - Increase SNR threashold to 5dB if AEQ is enabled for SRB loopback
26/03/12    jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
16/02/12    ws      CR 332030. Implement storage of last good modulation for
                    SRB loopback
11-02-16    tjw     Internal content split out from the original GERAN file
                    previously used by GERAN clients.
========================================================================== */
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "sys_type.h"

#include "geran_multi_sim.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* SNR threshold 322 in Q8 is 1dB */
#define ONE_DB_SNR    322
#define FIVE_DB_SNR   810

#ifdef  FEATURE_GSM_GPRS_MSC33
#define GL1_DEFS_MAX_ASSIGNED_UL_TS  (4)
#define GL1_DEFS_MAX_ASSIGNED_DL_TS  (5)
#else
#define GL1_DEFS_MAX_ASSIGNED_UL_TS   (4)
#define GL1_DEFS_MAX_ASSIGNED_DL_TS   (4)
#endif

#define GL1_DEFS_MAX_UL_MSGS  GL1_DEFS_MAX_ASSIGNED_UL_TS
#define GL1_DEFS_MAX_DL_MSGS  GL1_DEFS_MAX_ASSIGNED_DL_TS

#define GL1_DEFS_INVALID_FN 0xFFFFFF

#if GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS < GL1_DEFS_MAX_DL_MSGS
#error GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS < GL1_DEFS_MAX_DL_MSGS
#endif

#ifdef FEATURE_GSM_DYNMC_PWR_MEAS
/* Maximum power measurement chnages to 12 equals to CM power meas*/
#define MAX_MEAS_PER_FRAME            (12) /* Changed for UOOS */
#else
#define MAX_MEAS_PER_FRAME            (7)
#endif /*FEATURE_GSM_DYNMC_PWR_MEAS*/

typedef struct
{
   uint16 hztopdm_small_freq;
   uint16 hztopdm_large_freq;
} gl1_defs_acq_afc_type;

/* structure specifying the return data from an interference measure */
typedef struct
{
    uint8                     num_tn;
    gl1_defs_tn_type          tn[GL1_DEFS_MAX_ASSIGNED_DL_TS];
    dBx16_T                   measured_pwr_dBm_x16[GL1_DEFS_MAX_ASSIGNED_DL_TS];
    ARFCN_T                   arfcn;
} gl1_defs_int_meas_result_type;

/* Packet callback used to pass back msg header and decoded data */
typedef void (*gl1_defs_rx_pkt_data_callback)
(
   gl1_defs_rx_pkt_data_type  *msg_data,
   boolean                    valid,
   gas_id_t                   gas_id
);

/* Packet callback used to pass back rx metrics for a frame */
typedef void (*gl1_defs_rx_pkt_metrics_callback )

(
   gl1_defs_rx_pkt_metrics_type *metrics_data[ GL1_DEFS_MAX_DL_MSGS ],
   uint8                        num_msgs,
   uint8                        burst_num,
   gas_id_t                     gas_id
);

/* Packet callback used to pass back rx log info for a frame */
typedef void (*gl1_defs_rx_pkt_log_callback) (uint8 burst_num);

/* Packet callback used to pass back discarded rx info */
typedef void (*gl1_defs_rx_pkt_discarded_callback)
(
   gl1_defs_tn_type tn[ GL1_DEFS_MAX_DL_MSGS ],
   uint8            num_msgs
);

/* Structure containing callbacks needed for data, metrics and log */
typedef struct
{
  gl1_defs_rx_pkt_data_callback      data_callback;
  gl1_defs_rx_pkt_metrics_callback   metrics_callback;
  gl1_defs_rx_pkt_discarded_callback discarded_callback;
  gl1_defs_rx_pkt_log_callback       log_callback;
} gl1_defs_rx_pkt_cb_type;

#define SINGLE_GL1_DATA_SPACE 1

/*========================================================================== */
/* Check gl1_defs.h FEATURE assumptions */
/*========================================================================== */

/* tbd FEATURE_FTM_GSM_BER     in custusurf.h */
#ifndef FEATURE_GSM_AMR         /* ON */
  #pragma warning ("FEATURE_GSM_AMR assumed defined but isn't")
#endif

/* Enable HMSC for 2H10 targets with RF driver and FW support*/
/* #if !defined (FEATURE_MODEM_PEGASUS) */
/* #if !defined (T_MDM9X00) */
/* tbd FEATURE_GSM_GPRS_MSC33 */

#ifndef FEATURE_GSM_MDSP_ESAIC  /* Enable ESAIC on all targets */
  #pragma warning ("FEATURE_GSM_MDSP_ESAIC assumed defined but isn't")
#endif


#endif /* GL1_DEFS_G_H */
