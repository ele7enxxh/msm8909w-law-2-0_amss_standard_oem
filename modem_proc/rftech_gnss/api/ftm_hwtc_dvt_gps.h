/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       F T M  H W T C  GPS  D V T  M O D U L E

GENERAL DESCRIPTION
  This DVT module contains commands for testing the Jaguar/ZRF6500 RX
  Front End

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003-2009 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/api/ftm_hwtc_dvt_gps.h#1 $
  $DateTime: 2016/12/13 07:59:49 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/11   dbz    Add DVT string for PGA gain trakcing loop logic
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
12/13/09   gy     Added a dvt to disable bp1 notches
10/15/09   hp     Added DVT string support to access ELNA API
                  ftm_hwtc_dvt_set_ext_lna_api_on & ftm_hwtc_dvt_set_ext_lna_api_off
09/13/09   gy     Added Pause and Resume dvt string
08/30/09   gy     Added GLONASS Test Mode dvt string
08/14/09   gy     Added Gen8 dvt strings
02/28/07   ycl    Fix compiler warnings.
01/16/07   ra     Created from FTM HWTC RX FRONT DVT MODULE
===========================================================================*/


#ifndef FTM_HWTC_DVT_GPS_H
#define FTM_HWTC_DVT_GPS_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GPS_DC_CANCELLATION

DESCRIPTION
  This function call a library function which will run GPS DC Cacellation
  algorithm and update the DAC if necessary.

DEPENDENCIES
  none

===========================================================================*/
char* ftm_hwtc_dvt_gps_dc_cancellation (char* str_params);
/*===========================================================================

FUNCTION       ftm_hwtc_dvt_gps_dc_cancel_once

DESCRIPTION
  This function call a library function which will cancel GPS DC
  algorithm and update the DAC if necessary.

DEPENDENCIES
  none

===========================================================================*/
char* ftm_hwtc_dvt_gps_dc_cancel_once (char* str_params);
/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GPS_SET_DC_CANCEL_THRESHOLD

DESCRIPTION
  This function sets the GPS DC Cancellation threshold used by the DC cancellation
  algorithm.

DEPENDENCIES
  none

===========================================================================*/
char* ftm_hwtc_dvt_gps_set_dc_cancel_threshold (char* str_params);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_RXF_REGISTER

DESCRIPTION
  This function registers the RX Front DVT commands

DEPENDENCIES
  none

===========================================================================*/
void ftm_hwtc_dvt_gps_register (void);

char* ftm_hwtc_dvt_enter_gnss_mode1(char* str_params);
char* ftm_hwtc_dvt_enter_gnss_mode2(char* str_params);
char* ftm_hwtc_dvt_enter_gnss_mode5(char* str_params);
char* ftm_hwtc_dvt_enter_gnss_glo_test_mode(char* str_params);
char* ftm_hwtc_dvt_exit_gnss(char* str_params);
char* ftm_hwtc_dvt_set_ext_lna_on(char* str_params);
char* ftm_hwtc_dvt_set_ext_lna_off(char* str_params);
char* ftm_hwtc_dvt_set_periodic_dc_on(char* str_params);
char* ftm_hwtc_dvt_set_periodic_dc_off(char* str_params);
char* ftm_hwtc_dvt_print_premean_mon(char* str_params);
char* ftm_hwtc_dvt_set_linearity_low(char* str_params);
char* ftm_hwtc_dvt_set_linearity_high(char* str_params);
char* ftm_hwtc_dvt_print_bp1sch2_ampl(char* str_params);
char* ftm_hwtc_dvt_meas_period_config(char* str_params);
char* ftm_hwtc_dvt_bc_gain_config(char* str_params);
char* ftm_hwtc_dvt_gnss_adc_read_write(char* str_params);
char* ftm_hwtc_dvt_pause_gnss(char* str_params);
char* ftm_hwtc_dvt_resume_gnss(char* str_params);
char* ftm_hwtc_dvt_set_ext_lna_api_on(char* str_params);
char* ftm_hwtc_dvt_set_ext_lna_api_off(char* str_params);
char* ftm_hwtc_dvt_disable_bp1_notches(char* str_params);
char* ftm_hwtc_dvt_lte_b13_tx_on(char* str_params);
char* ftm_hwtc_dvt_lte_b13_tx_off(char* str_params);
char* ftm_hwtc_dvt_pga_loop(char* str_params);

#endif /* FTM_HWTC_DVT_GPS_H */

