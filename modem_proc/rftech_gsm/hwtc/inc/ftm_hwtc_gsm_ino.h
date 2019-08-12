#ifndef FTM_HWTC_GSM_INO_H
#define FTM_HWTC_GSM_INO_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM HWTC IQ CAPTURE

GENERAL DESCRIPTION
  This is the header file for the embedded FTM HWTC SBI commands comming from the diag service 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/hwtc/inc/ftm_hwtc_gsm_ino.h#1 $
                              
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
11/05/09   sar     removed deprecated file ino_drv.h and obsolete func 
                   ftm_hwtc_ino_usr_callback.
030708     can     INITIAL VERSION WITH INO: major changes.
09/24/07   can     added enhanced iq_capture code, especially for QSC6270.
03/08/07   rt      Fix compiler warning
10-06-06   rt      Add function to support cleaner functionality of capture algorithm
01-23-03   lcl     Added SBI data structure
11-11-02   ra      Created 

===========================================================================*/


#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC

#include "diagpkt.h"
#include "ftm_hwtc_dispatch.h"

typedef enum
{
  FTM_HWTC_IQ_FAILURE,
  FTM_HWTC_IQ_SUCCESS
} iq_capture_result_type;


/******************************* IQ DATA STRUCTURE **************************/
#define FTM_GSM_MAX_BUFFER  1024
#define SAMPLE_RAM_SIZE     5120

typedef enum
{
  FTM_HWTC_ENH_IQ_CAPTURE_VIA_SAMPSRV,
  FTM_HWTC_ENH_IQ_CAPTURE_VIA_INO
} ftm_hwtc_enh_iq_capture_type;

typedef enum
{
  FTM_HWTC_ENH_GSM_IQ_CAPTURE, 
  FTM_HWTC_ENH_WCDMA_IQ_CAPTURE
} ftm_hwtc_enh_iq_capture_mode;

typedef enum
{
  FTM_HWTC_ENH_IQ_CAPTURE_START_STATUS, 
  FTM_HWTC_ENH_IQ_CAPTURE_STOP_STATUS
} ftm_hwtc_enh_iq_capture_status_type;

typedef struct
{
  uint32 sampsrv_offset;
  uint32 sampsrv_stop_addr;
  ftm_hwtc_enh_iq_capture_mode capture_mode;
} ftm_hwtc_enh_iq_capture_sampsrv_data;

typedef struct
{
  ftm_hwtc_enh_iq_capture_mode capture_mode;
} ftm_hwtc_enh_iq_capture_ino_data;

typedef struct
{
  ftm_hwtc_enh_iq_capture_sampsrv_data sampsrv;
  ftm_hwtc_enh_iq_capture_ino_data ino;
} ftm_hwtc_enh_iq_capture_data;

typedef enum
{
  FTM_HWTC_IQ_CAPTURE_GET_CONFIG,
  FTM_HWTC_IQ_CAPTURE_GET_DATA,
  FTM_HWTC_IQ_CAPTURE_SET_ANTENNA
} enum_iq_test_id_type;

typedef enum
{
  FTM_HWTC_IQ_4_BITS,
  FTM_HWTC_IQ_8_BITS,
  FTM_HWTC_IQ_16_BITS
} enum_iq_sample_bit_format_type;


typedef enum
{
    FTM_HWTC_SAMPSRV_PATH_0,
    FTM_HWTC_SAMPSRV_PATH_1
} ftm_hwtc_sampsrv_path_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_MODE_CONTINUOUS,
    FTM_HWTC_SAMPSRV_CAPTURE_MODE_WINDOW
} ftm_hwtc_sampsrv_capture_mode_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_TYPE_ADDR,
    FTM_HWTC_SAMPSRV_CAPTURE_TYPE_IMMEDIATE
} ftm_hwtc_sampsrv_capture_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_SEL_IQ_DATA,
    FTM_HWTC_SAMPSRV_CAPTURE_SEL_Y1Y2_DATA
} ftm_hwtc_sampsrv_capture_data_type;

typedef struct
{
    boolean                              capture_in_progress;
    ftm_hwtc_sampsrv_capture_mode_type   capture_mode;
    ftm_hwtc_sampsrv_capture_type        capture_type;
    ftm_hwtc_sampsrv_capture_data_type   capture_data_source;
} ftm_hwtc_sampsrv_status;

typedef PACK(struct)
{
  int16 data[FTM_IQ_RECORD_BLOCK_SIZE];
  word recordIndex;
} ftm_hwtc_iq_data_type;


void ftm_hwtc_gsm_ino_gsm(ftm_hwtc_iq_data_type *data);
void ftm_hwtc_gsm_get_iq_data(ftm_hwtc_iq_data_type *data ); 

typedef enum
{
    FTM_HWTC_GSM_RXF_TESTBUS_GREY_CODE_MAPPING_SEL = 0x1,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_1_OUT_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_2_OUT_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_3_OUT_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_4_OUT_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_21BIT_IIR1_I_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_21BIT_IIR1_Q_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_21BIT_IIR2_I_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_21BIT_IIR2_Q_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_23BIT_FIR1_I_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_26BIT_FIR2_I_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_STAGE_5_OUT_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_RESAMPLER_3_OUT_AFTER_TRUNCATE_SEL,
    FTM_HWTC_GSM_RXF_TESTBUS_RESAMPLER_OUT_TO_SAMPLE_SERVER,
    FTM_HWTC_GSM_RXF_TESTBUS_CONTROL_SIGNAL_SEL,
}ftm_hwtc_gsm_rxf_testbus_sel_type;

typedef enum
{
    FTM_HWTC_INO_BURST_CAPTURE = 0x0,
    FTM_HWTC_INO_CONT_CAPTURE,
}ftm_hwtc_ino_capture_type;

typedef PACK(struct)
{
  word maxRecordSize;
  word sampleBitFormat;
  dword samplingFrequency;
} ftm_hwtc_iq_config_type;      

typedef PACK(union)
{
  ftm_hwtc_iq_data_type      iq_data;      
  ftm_hwtc_iq_config_type    iq_config;
  word                       iq_antenna;  
} ftm_hwtc_iq_parms;

/************************* FTM HWTC DATA STRUCTURE **************************/

/* hwtc iq request/response type */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  ftm_cmd_header_type ftm_cmd_header;
  ftm_hwtc_iq_parms iq_params;
} ftm_hwtc_iq_pkt_type;

/*===========================================================================

FUNCTION ftm_hwtc_gsm_capture_ino

DESCRIPTION
   This function uses INO block to capture GSM data.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_hwtc_gsm_capture_ino(ftm_hwtc_ino_capture_type capture_type,
                         ftm_hwtc_gsm_rxf_testbus_sel_type testbus_sel,
                         uint32 actual_ino_buffer_size,
                         boolean timed_capture,
                         uint32 systime);

/*===========================================================================

FUNCTION FTM_HWTC_WAIT_FOR_INO_BUFFER

DESCRIPTION
  This function makes sure no other code is executed until the INO buffer is
  full.

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_hwtc_wait_for_ino_buffer(void);


#endif /* FEATURE_FTM_HWTC */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_GSM_INO_H */

