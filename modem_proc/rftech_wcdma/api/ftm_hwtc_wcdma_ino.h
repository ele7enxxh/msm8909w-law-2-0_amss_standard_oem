#ifndef FTM_HWTC_WCDMA_INO_H
#define FTM_HWTC_WCDNA_INO_H

/*!
  @file ftm_hwtc_wcdma_ino.h

  @brief
  This module has WCDMA INO related functions.

  @details

*/

/*===========================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_hwtc_wcdma_ino.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/03/11   dw      QRCT IQ capture support
11/05/09   sar     Removed deprecated file ino_drv.h.
09/13/09   vb      ICI calibration support
08/13/09   ckl     Support IQ capture for Diversity path
07/23/09   ckl     Support IQ capture from RxFE resampler stage
06/10/09   ckl     Ported from LCU code
============================================================================*/

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC

#include "diagpkt.h"
#include "ftm_hwtc_dispatch.h"
#include "ftm_hwtc_iqcapture.h"
#include "ftm_hspa_lib.h"
//#include "ino_drv.h"

#define FTM_HWTC_INO_BUFFER_SIZE_CONST 0x100 /* 1KB until malloc() is used */
#define FTM_HWTC_INO_BUFFER_IQ_CAPTURE_TOOL_SIZE 0x40000 /*64K - must be multiple of 256*32 (0x2000)*/
#define FTM_WCDMA_IQ_LEN 12800

typedef enum
{
  FTM_HWTC_INO_BURST_CAPTURE = 0x0,
  FTM_HWTC_INO_CONT_CAPTURE,
}ftm_hwtc_ino_capture_type;

typedef struct
{
  ftm_hspa_wcdma_rxf_dlog_sel_type dlog_sel;
}ftm_hwtc_wcdma_iq_capture_data_type;

typedef struct
{  
  ftm_hwtc_wcdma_iq_capture_data_type iq_data;
}ftm_hwtc_wcdma_data_type;

void ftm_hwtc_wcdma_get_iq_data_from_ino
(
  ftm_receive_chain_handle_type chain,
  ftm_hwtc_iq_data_type *data
);

void ftm_hwtc_wcdma_set_iq_capture_antenna ( enum_iq_antenna_type ant );

void ftm_hwtc_dvt_wcdma_ino_register (void);

void ftm_hwtc_wcdma_get_ino_config(ftm_hwtc_iq_config_type*);

extern void ftm_hwtc_wcdma_do_ici_cal(ftm_receive_chain_handle_type chain, ftm_ici_result_type *ici_result);

#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_HWTC_WCDMA_INO_H */
