#ifndef FTM_WCDMA_IM2_CAL_H
#define FTM_WCDMA_IM2_CAL_H

/*===========================================================================

              W C D M A  I M 2  C A L I B R A T I O N   
     S P E C I F I C   D E F I N I T I O N S   H E A D E R  F I L E

DESCRIPTION
   This file contains IM2 calibraion definitions which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/inc/ftm_wcdma_im2_cal.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/30/13   ka      Updated FTM interfaces to logical device
02/03/10   kguo    Changed PACKED struct to PACK(struct) 
08/08/07   jfc     Mainlined T_MSM6280 feature
04/13/07   ycl     Add ftm_wcdma_get_im2_power() function prototype.
10/17/06   ycl     Modifications for initial MSM7600 build.
04/06/06   jac     More changes for diversity support.
04/04/06   jac     Added featurization so that things work with MSM6275.
02/23/06   jac     Added path parameters to some functions to support diversity RX.
03/11/04   xw      Created.
===========================================================================*/

#include "rfa_variation.h"
#include "ftm.h"  
  
#ifdef FTM_HAS_IM2_CAL
            
#include "comdef.h"
#include "ftm_gsm.h"
#include "ftm.h"
#include "ftm_wcdma_ctl.h"
#include "rfcom.h"
#include "rfllumts.h"

typedef enum
{
  FTM_RF_SET_IM2_Q_DAC,
  FTM_RF_SET_IM2_I_DAC,
  FTM_RF_ENABLE_IM2,
  FTM_RF_SET_TRANSCONDUCTOR,
  FTM_RF_GET_IM2,
  FTM_RF_GET_IIP2,
  FTM_RF_SET_IQ_MISMATCH,
  FTM_RF_CALIBRATE_IQ_MISMATCH,
  FTM_RF_ENABLE_IQ_MISMATCH,
  FTM_RF_CONFIGURE_SUPPRESSION_LEVEL,
  FTM_RF_GET_SUPPRESSION_LEVEL
} ftm_im2_cal_id_type;

typedef PACK(struct)
{
  dword comp_a;
  dword comp_b;
} ftm_iq_mismatch_type;

typedef PACK(struct)
{
  enum_type                 im2_cal_id;
  PACKED union
  {
    boolean                 bool_data;
    byte                    byte_data;
    word                    word_data;
    dword                   dwrd_data;
    int16                   int2_data;
    ftm_im2_type            im2;
    ftm_iq_mismatch_type    iq_mismatch;
  } rf_data_union;
} ftm_im2_cal_option_type;

typedef struct
{
  byte    iDac;
  byte    qDac;
  double  im2SuppLevel;
} ftm_im2_srch_type;

typedef struct
{
  byte    idac;
  byte    qdac;
  byte    array_index;
  byte    transconductor;
  boolean srch_done_flag;
}ftm_im2_trans_srch_type;

typedef struct
{
  byte    start_idac;
  byte    stop_idac;
  byte    start_qdac;
  byte    stop_qdac;
  byte    srch_step;
} ftm_im2_srch_range_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION FTM_IM2_IQ_SEARCH

DESCRIPTION  
  This function will perform I/Q DAC search that yields the best IM2 suppression.  
        
DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  A pointer to the IM2DAC value.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_im2_iq_search(rf_path_enum_type device, 
                       ftm_im2_srch_range_type srchRange, ftm_im2_srch_type *);

/*===========================================================================

FUNCTION FTM_IM2_CALCLULATE_SEARCH_RANGE

DESCRIPTION  
  This function calculates what's the search window based on current IQ DAC 
  value, search scope and step size.  
        
DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  A pointer to the IM2DAC value.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_im2_calculate_search_range(byte iDacCenter, 
                                    byte qDacCenter, 
                                    byte srchSize,
                                    byte srchStep,
                                    ftm_im2_srch_range_type *);

/*===========================================================================

FUNCTION FTM_IM2_CAL_DISPATCH

DESCRIPTION  
   This function dispatches IM2 calibration commands.

DEPENDENCIES
   SBI clock regime must be enabled.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_im2_cal_dispatch (ftm_im2_cal_option_type* im2_cal);

/*===========================================================================

FUNCTION FTM_IM2_CAPTURE_IQ_DATA

DESCRIPTION  
   
DEPENDENCIES
   SBI clock regime must be enabled.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_wcdma_im2_capture_iq_data(rf_path_enum_type device);

/*===========================================================================

FUNCTION FTM_TRANSCONDUCTOR_SEARCH

DESCRIPTION
    This function searches for transconductor value for the best IM2
    suppression. 
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void ftm_transconductor_search(rf_path_enum_type device, 
                               ftm_im2_trans_srch_type *trans_search);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_IM2

DESCRIPTION
   This function returns IM2 calibration results.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ftm_wcdma_get_im2(rfcom_device_enum_type device, ftm_im2_type *im2_data);

/*===========================================================================

FUNCTION FTM_IM2_DFT_PROC

DESCRIPTION  
   This function captures IQ data, runs Goertzel DFT and return DFT results. 

DEPENDENCIES
   SBI clock regime must be enabled.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
double ftm_im2_dft_proc(rf_path_enum_type device);

/*==========================================================================

FUNCTION ftm_set_rfr_trans_value

DESCRIPTION
  This function programs RFR's transconductor value.

DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  boolean - FALSE if the values passed in where > 6 bits
            TRUE  if the values were withing the 6 bit limit.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean ftm_set_rfr_trans_value( rf_path_enum_type device, byte* trans_value );

/*==========================================================================

FUNCTION ftm_set_rfr_q_value

DESCRIPTION
  This function programs RFR's QDAC value.

DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  boolean - FALSE if the values passed in where > 6 bits
            TRUE  if the values were withing the 6 bit limit.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean ftm_set_rfr_q_value(rf_path_enum_type device, byte* q_value);

/*==========================================================================

FUNCTION ftm_set_rfr_i_value

DESCRIPTION
  This function programs RFR's IDAC value.

DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  boolean - FALSE if the values passed in where > 6 bits
            TRUE  if the values were withing the 6 bit limit.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean ftm_set_rfr_i_value(rf_path_enum_type device, byte* i_value);

/*==========================================================================

FUNCTION ftm_rfr_enable_im2

DESCRIPTION
  This function enables RFR's IM2.

DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  boolean - FALSE if the values passed in where > 6 bits
            TRUE  if the values were withing the 6 bit limit.

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_rfr_enable_im2(rf_path_enum_type device, boolean im2_enable);

/*==========================================================================

FUNCTION ftm_wcdma_get_im2_with_suppression

DESCRIPTION


DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
extern boolean ftm_wcdma_get_im2_with_suppression(rfcom_device_enum_type device, 
                                            ftm_mix_type *im2_cal_data);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_IM2_POWER

DESCRIPTION  
  This function get the IM2 power at the frequency bin of interest.

DEPENDENCIES
   Assume the circuit is calibrated.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
double ftm_wcdma_get_im2_power(rf_path_enum_type device, uint16 ftm_im2_avg);

#endif /* FTM_HAS_IM2_CAL */

#endif /* FTM_WCDMA_IM2_CAL_H */
