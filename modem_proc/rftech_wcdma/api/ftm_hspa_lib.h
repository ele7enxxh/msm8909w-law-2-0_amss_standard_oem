#ifndef FTM_HSPA_LIB_H
#define FTM_HSPA_LIB_H

/*===========================================================================

              W C D M A  H S P A  C A L I B R A T I O N   
     S P E C I F I C   D E F I N I T I O N S   H E A D E R  F I L E

DESCRIPTION
   This file contains HSPA calibraion definitions. 

Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_hspa_lib.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/13   vs      Add IQ capture support for 3rd carrier 
01/30/13   ka      Updated FTM interfaces to logical device
11/05/12   tks     Increased HSPA IQ sample size to 16k samples 
07/01/11   dw      Fix IQ capture on Nikel
01/19/10   ka      Added support for multi-carrier ICI calibration.
05/11/09   jfc     Add function declarations to header file
01/13/09   ka      Created
===========================================================================*/

#define FTM_HSPA_IQ_LEN 16384

typedef enum
{
  FTM_HSPA_WCDMA_RXF_DLOG_DISABLED = 0x0,
  /*TX_YQ, TX_YI, RX_Y2Q, RX_Y1Q, RX_Y2I, RX_Y1I*/
  FTM_HSPA_WCDMA_RXF0_DLOG_TXRX_Y1Y2_DATA,
  /*two set of RX_Y[1|2][I|Q] values at fs/2 rate*/
  FTM_HSPA_WCDMA_RXF0_DLOG_RX_Y1Y2_DATA_AT_HALF_FS,
  FTM_HSPA_WCDMA_RXF0_DLOG_BBF_OUT_I,
  FTM_HSPA_WCDMA_RXF0_DLOG_BBF_OUT_Q,
  FTM_HSPA_WCDMA_RXF0_DLOG_RESAMPLE_OUT_I,
  FTM_HSPA_WCDMA_RXF0_DLOG_RESAMPLE_OUT_Q,
  FTM_HSPA_WCDMA_RXF0_DLOG_DVGA_OUT_IQ,
  FTM_HSPA_WCDMA_RXF0_NARROWBAND_C0_IQ_FLOATING_SAMPLES,
  FTM_HSPA_WCDMA_RXF0_NARROWBAND_C1_IQ_FLOATING_SAMPLES,
  FTM_HSPA_WCDMA_RXF1_DLOG_TXRX_Y1Y2_DATA,
  /*two set of RX_Y[1|2][I|Q] values at fs/2 rate*/
  FTM_HSPA_WCDMA_RXF1_DLOG_RX_Y1Y2_DATA_AT_HALF_FS,
  FTM_HSPA_WCDMA_RXF1_DLOG_BBF_OUT_I,
  FTM_HSPA_WCDMA_RXF1_DLOG_BBF_OUT_Q,
  FTM_HSPA_WCDMA_RXF1_DLOG_RESAMPLE_OUT_I,
  FTM_HSPA_WCDMA_RXF1_DLOG_RESAMPLE_OUT_Q,
  FTM_HSPA_WCDMA_RXF1_DLOG_DVGA_OUT_IQ,
  FTM_HSPA_WCDMA_RXF1_NARROWBAND_C0_IQ_FLOATING_SAMPLES,
  FTM_HSPA_WCDMA_RXF1_NARROWBAND_C1_IQ_FLOATING_SAMPLES,
  /* RxF for A0 C2 and A1 C2*/
  FTM_HSPA_WCDMA_RXF0_NARROWBAND_C2_IQ_FLOATING_SAMPLES,
  FTM_HSPA_WCDMA_RXF1_NARROWBAND_C2_IQ_FLOATING_SAMPLES,
  FTM_HSPA_WCDMA_RXF_DLOG_SEL_MAX,
}ftm_hspa_wcdma_rxf_dlog_sel_type;

typedef enum
{
  FTM_HSPA_INO_BURST_CAPTURE = 0x0,
  FTM_HSPA_INO_CONT_CAPTURE,
}ftm_hspa_ino_capture_type;




/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ftm_hspa_capture_ino

DESCRIPTION
   This function uses the INO block to capture WCDMA data.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_hspa_capture_ino(void *msg_ptr);

/*===========================================================================

FUNCTION ftm_hspa_do_ici_cal

DESCRIPTION  
    This function captures IQ data for ICI calibration and returns it in a
    sequence of log packets.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_hspa_do_ici_cal(rfcom_device_enum_type, ftm_ici_result_type *);

/*===========================================================================

FUNCTION ftm_hspa_iq_mismatch_cal

DESCRIPTION
    This function calculates IQ mismatch calibration.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_hspa_iq_mismatch_cal(rfcom_device_enum_type, ftm_iqm_result_type *);

/*===========================================================================

FUNCTION ftm_hspa_do_iq_ino

DESCRIPTION  
    This function captures IQ data at a user defined point in the RxFE chain and returns it in a
    sequence of log packets.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_hspa_do_iq_ino(rfcom_device_enum_type device, void *req_pkt);

#endif /* FTM_HSPA_LIB_H */
