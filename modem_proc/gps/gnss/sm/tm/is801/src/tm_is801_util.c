/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            IS801 Module

General Description
  This file contains implementations for TM Diag Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/src/tm_is801_util.c#1 $  
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/24/14    rk      Added missing steering source type while injecting AcqAssist.
08/11/2014  jv     16-Bit SV ID support for Galileo
05/16/13    mj     Handling Failure from mcc_pd_get_bs_info
09/21/11    pk     Fixing FTCal issues
06/20/11    rh      Added SA/AA data in OEM layer
08/03/10    rb     Set HeadingValid flag to TRUE when velocity is available 
07/13/09    gk     Initialize health when sending the almanac to PE
01/18/08    ank    Changes to scale up the injected PosUnc.
11/17/07    jw     Remove 10chip restriciton on PRM adjustment
08/28/07    ank    Changes to set normal frequency in IS801 PPM to the PPM frequency.
08/07/07    jw     Fixed bug in Meas combining when taking modulo of the CDMA RTC.
06/26/07    jw     When filling in the C/No for the IS801 prm report, round up
                   instead of truncating.
05/31/07    jw     Filled in function to convert is801 SA to MGP SA format
05/01/07    jw     In 1xMSA, don't check for the Don't use flag from MGP.
04/30/07    jw     For PRMs with RMSE >= 112m, set it to be 111m
04/18/07    jw     Set ElevUnc fields in mgp_IS801AA2AzElInfo()
04/04/07    jw     Set RefPn to earliest arriving path pn.
03/26/07    ld     Fixed a bug in measurement combining code. 
                   Modified the measurement qualification code.
03/15/07    jw     Made some modificiations to measurement combining code.
02/22/07    jw     Added measurement combining code
02/05/07    jw     Fixed adding the RFLoss to the PRMs.
02/01/07    cl     Support for UMTS only targets
12/27/06   ank     Added support for IS801 Rx and Tx events.
12/20/06   ank     Changes to send PRM and post-fix PPM together.
                   Added code to send bogus PRMs if MGP reports no SVs.
12/15/06    jw     For now, set the multipath indicator to 0 for the PRMs.
12/13/06    jw     1. Add local clock bias to PRMs.
                   2. Set time_ref of PRMs to use CDMA time reference, so that
                   we can match the PPM report's format.
12/01/06    jw     1. Made changes to support modified struct sm_GpsMeasRptStructType
                   2. In the PRMs, add the RF Loss to the C/No as reported by MGP.
                   3. Propagate the PRMs to the common meas time.
                   4. Check the meas status of the MeasBlk return as MGP can
                   report intermediate results that should not be reported.
11/01/06   br      Added temporary featurization to fix compilation error.
08/29/06   cl      Initail Release
===========================================================================*/
#include "gps_variation.h"
#include "stdio.h"      // snprintf
#include "tm_common.h"
#include "tm_data.h"
#include "tm_is801_util.h"
#include "paixlate.h"
#include "mgp_api.h"
#include "mccdma.h"
#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif
#include "qw.h"
#include "time_svc.h"
#include "math.h"
#include "tm_is801.h"
#include "log.h"

#include "sm_oemdata.h"

/* For logs and events */
#include "sm_dm_event.h"
#include "sm_log.h"

#define IS801_RMSE_EXP_OFST 3 /* No of bits to shift to get to Exponent offset (upper 3 bits) */
#define RMSE_FOR_PPM 1
#define RMSE_FOR_PR 2
#define TIME_REF_MS_MASK 0xdbba00

byte  first_sv_prn_num = 1;


/******************************************************************************
 * mgp_IS801AA2SteeringInfo
 *
 * Function description:
 *
 *  This function is used to construct SV steering info from IS-801 acquisition
 *  assistance data.
 *
 * Parameters: 
 *
 *  p_SvSteer - Pointer to MGP SV steering struct.
 *  p_IS801AA - Pointer to IS801 AA data.
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************/
static void mgp_IS801AA2SteeringInfo
( 
  const is801_aa_var_type  *p_IS801AA,
  gnss_SvSteerStructType   *p_SvSteer
)
{
  p_SvSteer->w_Sv = p_IS801AA->sv_prn_num;
  p_SvSteer->u_Modulo80ms = TRUE;
  /* Express q_Ms as the SV signal phase at the antenna */
  /* Note that PDSM converted sv_code_ph_int to prop_delay earlier. Need to convert it back to phase */
  p_SvSteer->q_Ms = p_IS801AA->gps_bit_num * 20 + (GPS_AA_MAX_SV_CODE_PH_INT - p_IS801AA->sv_code_ph_int);
  p_SvSteer->f_SubMsec = (FLT) (((FLT)(1022 - p_IS801AA->sv_code_ph)) * 1.0/1023.0);
  p_SvSteer->f_SvTimeUnc = (FLT)(p_IS801AA->sv_code_ph_win/2046.0);

  p_SvSteer->f_SvSpeed = (FLT)p_IS801AA->doppler0 * (FLT)(2.5 * C_L1HzToMs * (-1.0));
  p_SvSteer->f_SvSpeedUnc = (FLT)(p_IS801AA->doppler_win_0_5_hz / 4.0) * (FLT)C_L1HzToMs;;
  p_SvSteer->f_SvAccel = (FLT)p_IS801AA->doppler1 * (FLT)(0.015625 * C_L1HzToMs * (-1.0));

} /* mgp_IS801AA2SteeringInfo() */


/******************************************************************************
 * mgp_IS801AA2AzElInfo
 *
 * Function description:
 *
 *  This function is used to construct SV Azimuth and Elevation info from 
 *  IS-801 acquisition assistance data.
 *
 * Parameters: 
 *
 *  p_AzElInfo - Pointer to MGP SV Azimuth and Elevation struct.
 *  p_IS801AA  - Pointer to IS801 AA data.
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************/
static void mgp_IS801AA2AzElInfo
( 
  const is801_aa_var_type    *p_IS801AA,
  gnss_AzElInfoStructType    *p_AzElInfo
)
{
  p_AzElInfo->w_Sv = p_IS801AA->sv_prn_num;
  p_AzElInfo->f_Azimuth = (FLT)(p_IS801AA->azimuth * 11.25 * DEG2RAD);
  p_AzElInfo->f_Elevation = (FLT)(p_IS801AA->elevation * 11.25 * DEG2RAD);
  p_AzElInfo->u_ElevUncValid = 1;
  p_AzElInfo->f_ElevUnc = 0.0F;
} /* mgp_IS801AA2AzElInfo() */

/*===========================================================================

FUNCTION RMSE_IS801_VAL

DESCRIPTION
  This function is used to convert from RMSE input from Searcher to IS801 format.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None 

===========================================================================*/
byte rmse_is801_val( word srch_rmse, byte n )
{
  dword exponent = 0;
  word  mantissa = 0;
  word  value    = 0, temp_val;

  if ( srch_rmse < 1 )
  {
    return 0;
  }
  else if ( srch_rmse == 1 )
  {
    mantissa = 1;
  }
  else if ( (srch_rmse >= (208/n) ) && (srch_rmse < (224/n)) )
  {
    exponent = 0x7;
    mantissa = 0x6;
  }
  else if ( srch_rmse >= (224/n) )
  {
    exponent = 0x7;
    mantissa = 0x7;
  }
  else
  {
    value = 1;
    temp_val = srch_rmse*n;

    while ( (temp_val = temp_val/2) !=NULL )
    {
      exponent++;
      value *= 2;
    }

    mantissa = (( (srch_rmse*n) % value ) * 8) / value;
  }

  value    = ( exponent << ( IS801_RMSE_EXP_OFST ) ) | mantissa ;
  
  return (byte)value;

} /* End rmse_is801_val() */

/*===========================================================================

FUNCTION is801_util_convert_ppm_to_oem_data

DESCRIPTION  convert PPM data before sending to OEM layer
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void is801_util_convert_ppm_to_oem_data
(
  const srch_ppm_rpt_type    *in_ppm_ptr,
  const is801_ppm_resp_type  *in_ppm_resp,
  pdsm_srch_ppm_rpt_type     *out_ppm_data
)
{
  byte i;

  memset((void *)out_ppm_data, 0, sizeof(pdsm_srch_ppm_rpt_type));

  /* Timestamp in qword */
  out_ppm_data->ts[0] = in_ppm_ptr->ts[0];
  out_ppm_data->ts[0] = in_ppm_ptr->ts[1];
  /* Offset included */
  out_ppm_data->offset_incl      = in_ppm_ptr->offset_incl;
  /* MS system time offset */
  out_ppm_data->mob_sys_t_offset = in_ppm_ptr->mob_sys_t_offset;
  /* Time reference PN sequence offset */
  out_ppm_data->ref_pn           = in_ppm_ptr->ref_pn;
  /* Reference pilot strength */
  out_ppm_data->ref_pn_ecio      = in_ppm_ptr->ref_pn_ecio;
  /* Band class */
  out_ppm_data->band_class       = in_ppm_ptr->band_class;
  /* Requency assignment */
  out_ppm_data->cdma_freq        = in_ppm_ptr->cdma_freq;
  /* Base station identification */
  out_ppm_data->base_id          = in_ppm_resp->fix.base_id;
  /* System identification */
  out_ppm_data->sid              = in_ppm_resp->fix.sid;
  /* Network identification */
  out_ppm_data->nid              = in_ppm_resp->fix.nid;
  /* Total received power */
  out_ppm_data->total_rx_pwr     = in_ppm_ptr->total_rx_pwr;
  /* Additional band class */
  out_ppm_data->add_band_class   = in_ppm_ptr->band_class;
  /* Additional cdma frequency */
  out_ppm_data->add_cdma_freq    = in_ppm_ptr->cdma_freq;
  /* Number of pilots */
  out_ppm_data->num_pilots       = in_ppm_ptr->num_pilots;

  for (i = 0; i < in_ppm_ptr->num_pilots; i++ )
  {
    /* Pilot pn phase */
    out_ppm_data->pilot_results[i].pilot_pn_phase = in_ppm_ptr->pilot_results[i].peak_pos;
    out_ppm_data->pilot_results[i].pilot_ecio     = in_ppm_ptr->pilot_results[i].peak_ecio;
    out_ppm_data->pilot_results[i].pilot_rmse     = in_ppm_ptr->pilot_results[i].peak_rmse;
  }
}

/*===========================================================================

FUNCTION is801_util_rlink_convert_ppm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_rlink_convert_ppm
(
  srch_ppm_rpt_type     *input_ppm_buf_ptr,
  is801_ppm_resp_type   *output_ppm_buf_ptr
)
{
  /* Simply assume that there is only one part and init it.
  ** This has to modified after initial testing. Sanjeev
  */
  int i;
  qword  ts_tmp;
  uint64 mod_value;
  mcc_pd_bs_info_type  is801_bs_info;

  if((input_ppm_buf_ptr == NULL) ||(output_ppm_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  memset(&is801_bs_info, 0, sizeof(is801_bs_info));
  //<<<+ by Liao] Revist: How do we get bs_info?
  if (mcc_pd_get_bs_info(&is801_bs_info)== FALSE)
  {    
    MSG_ERROR("No Base Station info provided",0,0,0);
    return FALSE;
  }
  //>>>[+ End]

  output_ppm_buf_ptr->fix.base_id = is801_bs_info.base_id;
  output_ppm_buf_ptr->fix.sid = is801_bs_info.sid;
  output_ppm_buf_ptr->fix.nid = is801_bs_info.nid;
  output_ppm_buf_ptr->fix.band_class = input_ppm_buf_ptr->band_class;
  output_ppm_buf_ptr->fix.cdma_freq = input_ppm_buf_ptr->cdma_freq;

  output_ppm_buf_ptr->fix.total_rx_pwr = input_ppm_buf_ptr->total_rx_pwr;
  output_ppm_buf_ptr->fix.num_pilots_p = input_ppm_buf_ptr->num_pilots;

  output_ppm_buf_ptr->fix.add_pilots_incl = FALSE;
  
  /* The time reported by Srch is qword, convert it to 1 ms unit.
  ** Shift out 16 lower bit( we get 1.25 ms count ) and then multiply by ( 5/4 )
  ** to get time in ms. IS801 will take only 24 bits out of that.
  */
  (void)qw_div_by_power_of_2( ts_tmp, input_ppm_buf_ptr->ts, 16 );

  qw_mul( ts_tmp, ts_tmp, ( dword )5);
  qw_inc( ts_tmp, 2);
  (void)qw_div( ts_tmp, ts_tmp, (word) 4 );

  mod_value = *(uint64 *)ts_tmp;

  mod_value = mod_value % TIME_REF_MS_MASK;

  output_ppm_buf_ptr->fix.time_ref_ms = mod_value;

  output_ppm_buf_ptr->fix.offset_incl = input_ppm_buf_ptr->offset_incl;

  if (output_ppm_buf_ptr->fix.offset_incl == TRUE) 
  {
    output_ppm_buf_ptr->fix.mob_sys_t_offset = input_ppm_buf_ptr->mob_sys_t_offset;
  }

  output_ppm_buf_ptr->fix.ref_pn = input_ppm_buf_ptr->ref_pn;

  output_ppm_buf_ptr->fix.ref_pilot_strength = input_ppm_buf_ptr->ref_pn_ecio;

  output_ppm_buf_ptr->fix.part_num = 1;

  output_ppm_buf_ptr->fix.total_parts = 1;

  for ( i=0; i < input_ppm_buf_ptr->num_pilots; i++ )
  {
    output_ppm_buf_ptr->var[i].pilot_pn_phase = 
                      ( input_ppm_buf_ptr->pilot_results[i].peak_pos +
                        (input_ppm_buf_ptr->pilot_results[i].peak_pn << 10) ) & 0x7FFFF;

    output_ppm_buf_ptr->var[i].pilot_strength = input_ppm_buf_ptr->pilot_results[i].peak_ecio;
        
    output_ppm_buf_ptr->var[i].rms_err_phase = rmse_is801_val( input_ppm_buf_ptr->pilot_results[i].peak_rmse, RMSE_FOR_PPM );        

    /* Set the MSTO to be -P(Refpn) */
    if (input_ppm_buf_ptr->ref_pn == input_ppm_buf_ptr->pilot_results[i].peak_pn)
    {
      MSG_HIGH("Setting MSTO to be -P(Refpn): %d, %d",
               output_ppm_buf_ptr->fix.mob_sys_t_offset,
               input_ppm_buf_ptr->pilot_results[i].peak_pos,0);
               
      output_ppm_buf_ptr->fix.mob_sys_t_offset = (int16)(-input_ppm_buf_ptr->pilot_results[i].peak_pos);
    }
  }  

  if (oemData_IsDataTypeOn(OEMDATA_PPM_DATA))
  {
    pdsm_srch_ppm_rpt_type    ext_ppm_data;

    // convert and send PPM data to OEM layer
    is801_util_convert_ppm_to_oem_data(input_ppm_buf_ptr, output_ppm_buf_ptr, &ext_ppm_data);
    (void)oemData_SendDataType(OEMDATA_PPM_DATA,(void *)&ext_ppm_data);
  }

  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_gps_meas_info_to_is801_prm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
#define FULL_CDMA_PN_CIRCLE_CCX8      (262144)  /* in CDMA cx8 */

static void is801_util_gps_meas_info_to_is801_prm
( 
  is801_prm_part_var_type  *p_IS801Prm,
  gnss_MeasStructType      *p_Meas,
  uint16                   w_RfLoss,
  FLT                      f_ClkFreqBias
)
{
  uint8  u_X;
  uint32 q_Prm;
  FLT    f_PrmSigma, f_PrmLimit, f_Y;

  p_IS801Prm->sv_prn_num = p_Meas->w_Sv;

  /* Add 5 (0.1db) to round up */
  p_IS801Prm->sv_cno = (uint8)((p_Meas->w_Cno + 5 + w_RfLoss) * (FLT)0.1);  

  // HexinTODO: Handle ps_doppler pushed outside of int16 range case.
  p_IS801Prm->ps_doppler = (int16)((p_Meas->z_FiltMeas.f_SvSpeed - f_ClkFreqBias)* C_MsToL1Hz * (-5.0));


  /* Calculate the PRM from the adjusted subms (chip time) into 20 bit quantity. 
     The LS 10 bits will contain the fractional chips and the next 10
     bits the whole chips. 1047552 = 1023 * 1024, i.e., convert to
     chip numbers and shift right by 10 bits. */
  q_Prm = (uint32) ((1.0 - p_Meas->z_FiltMeas.z_SvTime.f_SubMs) * 1047552.0);

  /* SV_CODE_PH_WH field. */
  p_IS801Prm->sv_code_ph_wh = (uint16)(q_Prm >> 10);
  /* Take care of rollover. Max # of whole chips is 1022, i.e., whole
     chip numbers are modulo 1023. */
  if (p_IS801Prm->sv_code_ph_wh == 1023) 
    p_IS801Prm->sv_code_ph_wh = 0;

  /* SV_CODE_PH_FR field. */
  p_IS801Prm->sv_code_ph_fr = (int16) (q_Prm & 0x3ff);

  /* Calculate range uncertainty. */
  f_PrmSigma = p_Meas->z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC;

  /* Following legacy code, report XCorrs with Saturated RMSE */
  if (p_Meas->q_MeasStatus & SM_MEAS_STATUS_XCORR)
    p_IS801Prm->ps_range_rms_er = 63;
  /* First do settings at low and high end of the allowed pseudorange unc. range */
  else if (f_PrmSigma < 0.5)
    p_IS801Prm->ps_range_rms_er = 0;
  else if ((f_PrmSigma < 112.0) && (f_PrmSigma >= 104.0))
    p_IS801Prm->ps_range_rms_er = 62;
  else if (f_PrmSigma >= 112.0)
    /* Any value greater than 112m should be mapped to the 62 because this
       will give the PDM the possible chance of using this measurement with a
       lower weight */
    p_IS801Prm->ps_range_rms_er = 62;
  /* IS-801 spec table 3.2.4.2-8: the table gives
         the decoding formula for the RMS but here we need the
          encoding of the RMS into mantissa and exponent. The PRM unc.
     values are divided into intervals. p_IS801Prm->ps_range_rms_er will contain the 
     mantissa and exponent values upon break from the for loop which 
     steps through successive intervals in trying to find which 
     interval  contains f_PrmSigma. */
  else
  {
    for (p_IS801Prm->ps_range_rms_er = 1; p_IS801Prm->ps_range_rms_er < 62; p_IS801Prm->ps_range_rms_er++)
    {
      /* Get the exponent part */
      u_X = p_IS801Prm->ps_range_rms_er >> 3;
      /* Get the mantissa part. */
      f_Y = (FLT) (p_IS801Prm->ps_range_rms_er & 7);
      f_PrmLimit = (0.5F + f_Y * 0.0625F) * (FLT)(1<<u_X);
      if (f_PrmSigma < f_PrmLimit)
        break;
    }
  }

  /* Multipath indicator is overloaded to provide search mode
     information to GPS Analyze. See section 5.2.1 of GPS
     Analyze for Aries memo by Paul Johnson */

  if( p_Meas->u_Observe == 1 && p_Meas->u_GoodObs == 1 )
  {
    /* Early exit passed after first sniff */
    p_IS801Prm->mul_path_ind = 0;  
  }
  else
  if( p_Meas->u_PreInt == 160 )
  {
    /* SA Data was used */
    p_IS801Prm->mul_path_ind = 7;  
  }
  else
  {
    /* TBD Bug? 2560 and 5120 dwell lenghts are not used any more in Aries and definitely not in Callisto.
     * Perhaps from Gemini? Besides, multipath indicator will be set explicitly in Callisto meas packets
     * This needs further investigation and inputs from MC team.
     */
    uint32 q_MeasSrchTime;

    q_MeasSrchTime = p_Meas->u_PreInt * p_Meas->w_PostInt;

    if( q_MeasSrchTime < 2560 )
    {
      p_IS801Prm->mul_path_ind = 4;  
    }
    else
    if( q_MeasSrchTime < 5120 )
    {
      p_IS801Prm->mul_path_ind = 5;  
    }
    else
    {
      p_IS801Prm->mul_path_ind = 6;  
    }
  }

} /* sm_GpsMeasInfo2IS801Prm() */


/*===========================================================================

FUNCTION      is801_math_code_phase_diff

DESCRIPTION

  This function should be used when taking the difference between 2 
  Code phases. The function computes
  
  p1 - p2
  
  But, it is important to use this function because if the difference is
  greater than a 1/2 of the big code phase circle (full_range), 
  the complement of the phase is returned.

DEPENDENCIES
  It is assumed that p1, p2 and full_range are in same code phase unit.

RETURN VALUE
  The signed difference in phase. If positive, then p1
  is a later phase than p2, else p2 is a later phase than p1.

SIDE EFFECTS
  None 

===========================================================================*/
#ifndef ABS
#define ABS(a)          (((a) < 0) ? (-(a)) : (a))
#endif


int32 is801_math_code_phase_diff(uint32 p1, uint32 p2, uint32 full_range)
{
  int32 delta = (int32) p1 - (int32) p2;
  int32 half_range = (int32) (full_range/2);

  if(delta > half_range)
  {
    /*---------------------------------
      This means p2 is a later phase
      than p1. We therefore want the result to
      be negative.
     *-------------------------------*/

    delta -= full_range;
  }
  else if(delta < -half_range)
  {
    /*---------------------------------
      This means p1 is a later phase than 
      p2. We want a positive result.
     *-------------------------------*/
    
    delta += full_range;
  }

  ASSERT(ABS(delta) <= half_range);

  return(delta);
} /* is801_math_code_phase_diff */

/*===========================================================================

FUNCTION is801_util_convert_prm_to_oem_data

DESCRIPTION  convert PRM data before sending to OEM layer
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void is801_util_convert_prm_to_oem_data
(
  qword                       time_ref,
  const is801_prm_resp_type   *in_prm_resp,
  pdsm_srch_gps_sat_rpt_type  *out_prm_data
)
{
  byte i;

  memset((void *)out_prm_data, 0, sizeof(pdsm_srch_gps_sat_rpt_type));

  /* Number of Pseudoranges in this part */
  out_prm_data->num_sats = in_prm_resp->fix.num_ps_ranges_p; 
  /* Time Reference (out_prm_data.time_ref in qword, in_prm_resp.time_ref in uint32/ms) */
  qw_equ(out_prm_data->time_ref, time_ref);
  /* Time reference source */
  out_prm_data->time_ref_src = in_prm_resp->fix.time_ref_src;
  /* Offset included */
  out_prm_data->offset_incl = in_prm_resp->fix.offset_incl;
  /* Time reference PN sequence */        
  out_prm_data->ref_pn = in_prm_resp->fix.ref_pn;
  /* MS system time offset */
  out_prm_data->mob_sys_t_offset = in_prm_resp->fix.mob_sys_t_offset;

  for(i = 0; i < out_prm_data->num_sats; i++)
  {
    /* Satellite PRN number */
    out_prm_data->gps_chan_rpt[i].sv_prn_num = (gnss_sv_id_type)in_prm_resp->var[i].sv_prn_num;
    /* Satellite C/No */
    out_prm_data->gps_chan_rpt[i].sv_cno = in_prm_resp->var[i].sv_cno;
    /* Pseudodoppler */
    out_prm_data->gps_chan_rpt[i].ps_doppler = (int32)in_prm_resp->var[i].ps_doppler;
    /* Satellite code phase, whole chips */
    out_prm_data->gps_chan_rpt[i].sv_code_ph_wh = in_prm_resp->var[i].sv_code_ph_wh;
    /* Satellite code phase - fractional chips */
    out_prm_data->gps_chan_rpt[i].sv_code_ph_fr = in_prm_resp->var[i].sv_code_ph_fr;
    /* Satellite code phase, fract ms, units 2^(-21) ms, range 0 to (1 - 2^(-21) ) */
    out_prm_data->gps_chan_rpt[i].sv_code_ph_fr_ms = 0;    // set to 0
    /* Multipath Indicator */
    out_prm_data->gps_chan_rpt[i].mul_path_ind = in_prm_resp->var[i].mul_path_ind;
    /* Pseudorange RMS error */
    out_prm_data->gps_chan_rpt[i].ps_range_rms_er = (int32)in_prm_resp->var[i].ps_range_rms_er;       
  }
}

/*===========================================================================

FUNCTION is801_util_rlink_convert_prm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean is801_util_rlink_convert_prm
(
  srch_ppm_rpt_type        *input_ppm_buf_ptr,
  sm_GpsMeasRptStructType  *input_prm_buf_ptr,
  is801_prm_resp_type      *output_prm_buf_ptr
)
{
  int i;
  qword cdma_timestamp, total_ms;
  uint64 time_stamp;
  dword mod_val;
  gnss_MeasStructType *p_GpsMeas;
  is801_prm_part_var_type *p_IS801Prm;
  boolean ret_val = TRUE;  /*lint !e506 */
  is801_FtcalDataStruct z_FtcalData;
  FLT f_GpsCodePhSubMs = 0.0;
  static cgps_FtcalGpsStruct z_FTCalGps;
  static cgps_FtcalStruct z_FTCal;

  int32 l_TempCdmaRtcLatchedCCx32;


  if((input_prm_buf_ptr == NULL) ||(output_prm_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  output_prm_buf_ptr->fix.ref_pn = 512; /* Needed only if Offset is included */
  output_prm_buf_ptr->fix.offset_incl = 0;
  output_prm_buf_ptr->fix.num_ps_ranges_p = 0;
  output_prm_buf_ptr->fix.total_parts = 1;
  output_prm_buf_ptr->fix.part_num = 1;

  /* Specify the time reference */

  /* Convert GPS Time to CDMA timestamp */
  gps_to_cdma_time(&input_prm_buf_ptr->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime, 
                   cdma_timestamp);

  /* Propagate the Measurements to the MeasClock reference point */
  gps_MeasBlkPropagate( &input_prm_buf_ptr->z_MeasBlk );

  /* The PPM buffer will be null if we are in MSB and when PPM is not running */
  if (input_ppm_buf_ptr && input_ppm_buf_ptr->num_pilots)
  {
    int32 l_RfCalibGpsCx1024, l_CDMARtcDiffCx16;
    int32 l_AdjustmentGCx1024, l_EarliestArrivingPathCDMACx16;
    uint32 q_CDMARtcAtTTCCx8, q_CDMARtcAtPPMCCx8;
    FLT f_AdjustmentMs, f_SubMs;

    /* Set the timestamp of the PPM using the PRM's timestamp */
    qw_equ(input_ppm_buf_ptr->ts, cdma_timestamp);
    MSG_HIGH("Setting time_ref of Post Fix PPMs",0,0,0);

    /* Get the RF Delay value using the bandclass found in the PPM report */
    l_RfCalibGpsCx1024 = is801_get_rf_delay(input_ppm_buf_ptr->band_class);
    MSG_HIGH("BandClass: %d, RF Delay: %d (GCx1024)",
             input_ppm_buf_ptr->band_class,l_RfCalibGpsCx1024,0);

    /* Find the earliest arriving path */
    l_EarliestArrivingPathCDMACx16 = input_ppm_buf_ptr->pilot_results[0].peak_pos;

    /* Set the PPM refPn to this earliest arriving path */
    input_ppm_buf_ptr->ref_pn = input_ppm_buf_ptr->pilot_results[0].peak_pn;
    input_ppm_buf_ptr->ref_pn_ecio = input_ppm_buf_ptr->pilot_results[0].peak_ecio;

    MSG_MED("PPM: Pn %d, PeakPos %d, Ecio %d",
            input_ppm_buf_ptr->pilot_results[0].peak_pn,
            input_ppm_buf_ptr->pilot_results[0].peak_pos,
            input_ppm_buf_ptr->pilot_results[0].peak_ecio);

    if (input_ppm_buf_ptr->num_pilots > PDSM_SRCH_MAX_PPM_RM_RPT_SAMPLES)
       input_ppm_buf_ptr->num_pilots = PDSM_SRCH_MAX_PPM_RM_RPT_SAMPLES;

    for ( i = 1; i < input_ppm_buf_ptr->num_pilots; i++ )
    {
      MSG_MED("PPM: Pn %d, PeakPos %d, Ecio %d",
              input_ppm_buf_ptr->pilot_results[i].peak_pn,
              input_ppm_buf_ptr->pilot_results[i].peak_pos,
              input_ppm_buf_ptr->pilot_results[i].peak_ecio);

      if (input_ppm_buf_ptr->pilot_results[i].peak_pos < l_EarliestArrivingPathCDMACx16)
      {
        l_EarliestArrivingPathCDMACx16 = input_ppm_buf_ptr->pilot_results[i].peak_pos;

        /* Set the PPM refPn to this earliest arriving path */
        input_ppm_buf_ptr->ref_pn = input_ppm_buf_ptr->pilot_results[i].peak_pn;
        input_ppm_buf_ptr->ref_pn_ecio = input_ppm_buf_ptr->pilot_results[i].peak_ecio;
      }
    }


    MSG_HIGH("PPM: Ref_Pn %d, Ref_Ecio %d",
             input_ppm_buf_ptr->ref_pn,
             input_ppm_buf_ptr->ref_pn_ecio,
             0);

    /* Set pilots relative to the earliest arriving path */
    for ( i = 0; i < input_ppm_buf_ptr->num_pilots; i++ )
    {
      input_ppm_buf_ptr->pilot_results[i].peak_pos = 
                          input_ppm_buf_ptr->pilot_results[i].peak_pos - 
                          l_EarliestArrivingPathCDMACx16;
    }

    /* No MSTO to report */
    input_ppm_buf_ptr->offset_incl = 0;

    /* PPM report cdma_rtc is in CdmaCx8 */
    /* (Ctt - Cpost) mod 80ms (Ctt is CCx8, Cpost is CCx8) */
    q_CDMARtcAtTTCCx8 = input_prm_buf_ptr->z_MeasBlk.z_CDMAClockInfo.q_CdmaRtcSync80CCx8 % FULL_CDMA_PN_CIRCLE_CCX8;
    q_CDMARtcAtPPMCCx8 = input_ppm_buf_ptr->cdma_rtc % FULL_CDMA_PN_CIRCLE_CCX8;
    
    l_CDMARtcDiffCx16 = is801_math_code_phase_diff( q_CDMARtcAtTTCCx8,
                                                    q_CDMARtcAtPPMCCx8,
                                                    FULL_CDMA_PN_CIRCLE_CCX8) * 2 ;
    if ( abs( l_CDMARtcDiffCx16 ) > 32 * 16 )
    {
      MSG_ERROR("CDMA RtcDiff %d (CCx16) too big, maybe clocks stopped",
                l_CDMARtcDiffCx16, 0, 0);
      l_CDMARtcDiffCx16 = 0;
    }

    MSG_HIGH("CDMA RtcDiff %d (CCx16), RTC at TT 0x%x (CCx8), RTC from PPM 0x%x (CCx8)",
             l_CDMARtcDiffCx16,
             input_prm_buf_ptr->z_MeasBlk.z_CDMAClockInfo.q_CdmaRtcSync80CCx8,
             input_ppm_buf_ptr->cdma_rtc);

    MSG_MED("CDMA RTC Diff %d, P(eatoa) %d, RFDelay %d (GCx1024)",
            l_CDMARtcDiffCx16*1705/32,
            l_EarliestArrivingPathCDMACx16*1705/32,
            l_RfCalibGpsCx1024);


    /* Ctt - Cpost */
    l_AdjustmentGCx1024 = l_CDMARtcDiffCx16*1705/32;

    /* Adjust by Earliest arriving path.
       1 CDMA CX8 = 1705/16  GPSChipX1024 */
    l_AdjustmentGCx1024 -= l_EarliestArrivingPathCDMACx16*1705/32;

    /* Adjust by the RF Delay */
    l_AdjustmentGCx1024 += l_RfCalibGpsCx1024;


    /* This code compiled on Callisto engines. This is needed because on 
     * Callisto, the GNSS RTC used is chipx80
     */
    /* Note: q_GpsRtcSubMsSync80GCx32 is incorrect name.
       This field contains GNSS RTC sub-msec value at Sync80 in GPS Chipx1280
       units */
    MSG_MED("Total adjustment %d (GCx1024), GNSSRTCatSync80 %d (GCx1280)",
            l_AdjustmentGCx1024,
            input_prm_buf_ptr->z_MeasBlk.z_CDMAClockInfo.q_GpsRtcSubMsSync80GCx32, 0);
    /* Convert into MS */
    f_AdjustmentMs = (FLT)l_AdjustmentGCx1024 * (1.0f/(1024.0f*1023.0f));

    /* Callisto engine has a gnss rtc that is chipx1280 */
    f_AdjustmentMs -=
      (FLT) input_prm_buf_ptr->z_MeasBlk.z_CDMAClockInfo.q_GpsRtcSubMsSync80GCx32 *
      (1.0f/(1280.0f*1023.0f));

    /* Adjustment for variable Rx FE part. q_CdmaRtcLatchedCCx32 actually
       contains q_CdmaRfFeVariableDelayCCx8192 */
    /* The contents of q_CdmaRtcLatchedCCx32 parameter has been changed from uint32 to
     * int32. However, the SW-FW interface has not changed and it remains as uint32. Hence
     * the conversion to int32 is required before converting to FLT */
    l_TempCdmaRtcLatchedCCx32 = (int32) input_prm_buf_ptr->z_MeasBlk.z_CDMAClockInfo.q_CdmaRtcLatchedCCx32;

    f_AdjustmentMs -= (FLT) l_TempCdmaRtcLatchedCCx32 * (1.0f/(8192.0f*1228.80f));

    /* Limit total adjustments */
    if (f_AdjustmentMs <= -1)
    {
      f_AdjustmentMs += 1;
    }
    else if (f_AdjustmentMs >= 1)
    {
      f_AdjustmentMs -= 1;
    }

    /* reinitialize p_GpsMeas */
    p_GpsMeas = input_prm_buf_ptr->z_MeasBlk.z_Gnss;
    for ( i = 0; i < input_prm_buf_ptr->z_MeasBlk.u_NumSvs; i++ )
    {
      if ( (p_GpsMeas->q_MeasStatus & (SM_MEAS_STATUS_INVALID_FLAGS|SM_MEAS_STATUS_SM_VALID)) 
                                   == SM_MEAS_STATUS_SM_VALID 
         )
      {
        f_SubMs = p_GpsMeas->z_FiltMeas.z_SvTime.f_SubMs;


        /* Apply adjustment */
        f_SubMs -= f_AdjustmentMs;
        if (f_SubMs < 0)
        {
          f_SubMs += 1;
        }
        else if (f_SubMs > 1)
        {
          f_SubMs -= 1;
        }

        MSG_MED("SV %d, Orig CP %d, Adjusted CP %d (GCx1024)",
                 p_GpsMeas->w_Sv,
                 (int32)((1.0f - p_GpsMeas->z_FiltMeas.z_SvTime.f_SubMs) * 1023.0f * 1024.0f),
                 (int32)(f_SubMs * 1023.0f * 1024.0f));

        f_GpsCodePhSubMs = p_GpsMeas->z_FiltMeas.z_SvTime.f_SubMs;
        p_GpsMeas->z_FiltMeas.z_SvTime.f_SubMs = f_SubMs;
      }
      p_GpsMeas++;
    }

  }

  /* The time reported by Srch is qword, convert it to 1 ms unit.
  ** Shift out 16 lower bit( we get 1.25 ms count ) and then multiply by ( 5/4 )
  ** to get time in ms. IS801 will take only 24 bits out of that.
  */
  (void)qw_div_by_power_of_2( total_ms, cdma_timestamp, 16 );

  qw_mul( total_ms, total_ms, ( dword )5);
  qw_inc( total_ms, 2);
  (void)qw_div( total_ms, total_ms, (word) 4 );
  /*lint -e740  Unusual pointer cast (incompatible indirect types) */
  time_stamp = *(uint64 *)(&total_ms[0]);
  mod_val = time_stamp % TIME_REF_MS_MASK;

  output_prm_buf_ptr->fix.time_ref = mod_val;
  output_prm_buf_ptr->fix.time_ref_src = 0x00; /* 0x00 is for CDMA Time Reference */


  /* reinitialize p_GpsMeas */
  p_GpsMeas = input_prm_buf_ptr->z_MeasBlk.z_Gnss;

  for ( i=0; i < input_prm_buf_ptr->z_MeasBlk.u_NumSvs && i < N_ACTIVE_GPS_CHAN; i++ )
  {
    MSG_HIGH("Using RF Loss %d (0.1 db)",input_prm_buf_ptr->z_MeasBlk.z_Gnss[i].w_RfLoss,0,0);
    /* Report only valid measurements */
    if ( p_GpsMeas[i].w_Cno && 
         (p_GpsMeas[i].q_MeasStatus & (SM_MEAS_STATUS_INVALID_FLAGS|SM_MEAS_STATUS_SM_VALID)) 
                               == SM_MEAS_STATUS_SM_VALID 
       )
    {
      is801_util_gps_meas_info_to_is801_prm( &output_prm_buf_ptr->var[output_prm_buf_ptr->fix.num_ps_ranges_p++], 
                                             &p_GpsMeas[i],
                                             input_prm_buf_ptr->z_MeasBlk.z_Gnss[i].w_RfLoss,
                                             input_prm_buf_ptr->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias);
    }

  } /* End for loop for all the satellites */

  /* Fill in Bogus PRM if MGP did not find any SVs */
  if (!output_prm_buf_ptr->fix.num_ps_ranges_p)
  {
    MSG_MED("Zero SVs reported by MGP. Report bogus SV prn=%d",first_sv_prn_num,0,0);
    p_IS801Prm = &output_prm_buf_ptr->var[output_prm_buf_ptr->fix.num_ps_ranges_p++];

    p_IS801Prm->sv_prn_num = first_sv_prn_num;
    p_IS801Prm->sv_cno = 0;  
    p_IS801Prm->ps_doppler = 0;
    p_IS801Prm->sv_code_ph_wh = 0;
    p_IS801Prm->sv_code_ph_fr = 0;
    p_IS801Prm->ps_range_rms_er = 63; /* No Meas*/
    p_IS801Prm->mul_path_ind = 0;  

    ret_val = FALSE;
  }

  if (oemData_IsDataTypeOn(OEMDATA_PRM_DATA))
  {
    pdsm_srch_gps_sat_rpt_type    ext_prm_data;

    // convert and send PRM data to OEM layer
    is801_util_convert_prm_to_oem_data(cdma_timestamp, output_prm_buf_ptr, &ext_prm_data);
    (void)oemData_SendDataType(OEMDATA_PRM_DATA,(void *)&ext_prm_data);
  }

  /* Get Fast TCal parameters to see if this is a Fast TCal session */
  tm_is801_get_ftcal_data( &z_FtcalData );

  if( TRUE == *z_FtcalData.p_FTCalEnabled )
  {
    /* This is a Fast TCal session - use special processing and logging */

    FLT    f_CdmaRtcDiffCCx16;
    FLT    f_CodePhAdjMsec;
    int32  l_CodePhAdjMsec;
    FLT    f_TCalRolloverCorrectionMsec = 0.0;
    int16  x_EventTCal = 0;
    uint8  u_EventTCalUncertainityNsec = 0;    

    z_FTCal.u_ErrorCode = 0;
    z_FTCal.f_TCalAverValueNsec = 0.0;
    z_FTCal.q_TCalUncertaintyNsec = 0xFFFFFFFF;
    z_FTCal.f_TCalMaxValueNsec = 0.0;
    z_FTCal.f_TCalMinValueNsec = 0.0;

    if( input_prm_buf_ptr->z_MeasBlk.u_NumSvs != 1 )
    {
      MSG_ERROR("F-TCal bad num of SVs reported %d",
                input_prm_buf_ptr->z_MeasBlk.u_NumSvs, 0, 0);
      z_FTCal.u_ErrorCode &= C_GNSS_FTCAL_BAD_NUM_OF_SV;
    }

    p_GpsMeas = input_prm_buf_ptr->z_MeasBlk.z_Gnss;

    if( z_FtcalData.p_Ppm->q_PpmNumToaSamplesAccumulated == 0 )
    {
      MSG_ERROR("F-TCal zero PPM meas reported", 0, 0, 0);
      z_FTCal.u_ErrorCode &= C_GNSS_FTCAL_ZERO_PPM_MEAS;
    }

    if( z_FtcalData.p_TT->q_TtNumTTSamplesAccumulated == 0 )
    {
      MSG_ERROR("F-TCal zero Time Transfers reported", 0, 0, 0);
      z_FTCal.u_ErrorCode &= C_GNSS_FTCAL_ZERO_TIME_TXFR;
    }

    if( z_FTCal.u_ErrorCode == 0 )
    {
      f_CdmaRtcDiffCCx16 =
        (FLT)z_FtcalData.p_Ppm->l_PpmSrchWinCenterCCx16 -
        z_FtcalData.p_TT->f_TtRefCdmaRtcS80SubPnRollCCx16;

      f_CodePhAdjMsec =
        f_GpsCodePhSubMs +
        z_FtcalData.p_TT->f_TtGnssRtcS80SubMsecGCx80 / CHIPS_PER_CX80_FLT /GPS_CHIPS_PER_MS_FLT +
        (f_CdmaRtcDiffCCx16 +
         z_FtcalData.p_TT->f_TtRefCdmaRtcS80AdjustSubPnRollCCx16 +
         z_FtcalData.p_Ppm->f_PpmAverToaCCx16) / CHIPS_PER_CX16_FLT / CDMA_CHIPS_PER_MS_FLT;

      /* Calculate sub-millisecond portion */
      l_CodePhAdjMsec = (int32)f_CodePhAdjMsec;
      f_CodePhAdjMsec -= (FLT)l_CodePhAdjMsec;

      /* Adjust phase based on 1x Sync80 phase */
      if( f_CodePhAdjMsec < -0.8333333 || f_CodePhAdjMsec > 0.8333333 )
      {
        if( f_CodePhAdjMsec < 0 )
          f_TCalRolloverCorrectionMsec = 1.0;
        else
          f_TCalRolloverCorrectionMsec = -1.0;
      }
      else if( f_CodePhAdjMsec <= -0.5 || f_CodePhAdjMsec >= 0.5 )
      {
        if( f_CodePhAdjMsec < 0 )
          f_TCalRolloverCorrectionMsec = 0.6666667;
        else
          f_TCalRolloverCorrectionMsec = -0.6666667;
      }
      else if( f_CodePhAdjMsec <= -0.1666666 || f_CodePhAdjMsec >= 0.1666666 )
      {
        if( f_CodePhAdjMsec < 0 )
          f_TCalRolloverCorrectionMsec = 0.3333333;
        else
          f_TCalRolloverCorrectionMsec = -0.3333333;
      }
      else
      {
        f_TCalRolloverCorrectionMsec = 0.0;
      }

      f_CodePhAdjMsec += f_TCalRolloverCorrectionMsec;

      z_FTCal.f_TCalAverValueNsec = (1.0 - f_CodePhAdjMsec) * 1000000.0;

      f_CodePhAdjMsec =
        f_GpsCodePhSubMs +
        z_FtcalData.p_TT->f_TtMinGnssRtcS80SubMsecGCx80 / CHIPS_PER_CX80_FLT / GPS_CHIPS_PER_MS_FLT +
        (f_CdmaRtcDiffCCx16 +
         z_FtcalData.p_TT->f_TtRefCdmaRtcS80AdjustSubPnRollCCx16 +
         z_FtcalData.p_Ppm->x_PpmMinToaCCx16) / CHIPS_PER_CX16_FLT / CDMA_CHIPS_PER_MS_FLT;

      /* Calculate sub-millisecond portion */
      l_CodePhAdjMsec = (int32)f_CodePhAdjMsec;
      f_CodePhAdjMsec -= (FLT)l_CodePhAdjMsec;
      f_CodePhAdjMsec += f_TCalRolloverCorrectionMsec;

      z_FTCal.f_TCalMaxValueNsec = (1.0 - f_CodePhAdjMsec) * 1000000.0;

      f_CodePhAdjMsec =
        f_GpsCodePhSubMs +
        z_FtcalData.p_TT->f_TtMaxGnssRtcS80SubMsecGCx80 / CHIPS_PER_CX80_FLT / GPS_CHIPS_PER_MS_FLT +
        (f_CdmaRtcDiffCCx16 +
         z_FtcalData.p_TT->f_TtRefCdmaRtcS80AdjustSubPnRollCCx16 +
         z_FtcalData.p_Ppm->x_PpmMaxToaCCx16) / CHIPS_PER_CX16_FLT / CDMA_CHIPS_PER_MS_FLT;

      /* Calculate sub-millisecond portion */
      l_CodePhAdjMsec = (int32)f_CodePhAdjMsec;
      f_CodePhAdjMsec -= (FLT)l_CodePhAdjMsec;
      f_CodePhAdjMsec += f_TCalRolloverCorrectionMsec;

      z_FTCal.f_TCalMinValueNsec = (1.0 - f_CodePhAdjMsec) * 1000000.0;

      z_FTCal.q_TCalUncertaintyNsec = (uint32)(z_FTCal.f_TCalMaxValueNsec - z_FTCal.f_TCalMinValueNsec);

      z_FTCalGps.u_GpsSv                = (uint8)p_GpsMeas->w_Sv; //TBD check the size!
      z_FTCalGps.f_GpsPeakCodePhSubMsec = f_GpsCodePhSubMs;
      z_FTCalGps.q_GpsPeakCodePhMsec    = p_GpsMeas->z_FiltMeas.z_SvTime.q_Ms;
      z_FTCalGps.q_GpsPeakFCount        = p_GpsMeas->q_FCount;
      z_FTCalGps.f_GpsPeakCNodBHz       = ((float)(p_GpsMeas->w_Cno + 5 + input_prm_buf_ptr->z_MeasBlk.z_Gnss[0].w_RfLoss)) * 0.1;
      z_FTCalGps.q_GpsPeakLatency       = p_GpsMeas->l_LatencyMs;
      z_FTCalGps.f_GpsSpeedMperSec      = input_prm_buf_ptr->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias - p_GpsMeas->z_FiltMeas.f_SvSpeed;

      z_FTCalGps.q_GpsParams = 0;

      MSG_HIGH("F-TCal TCal %dns max %d min %d", 
               (int32)z_FTCal.f_TCalAverValueNsec,
               (int32)z_FTCal.f_TCalMaxValueNsec,
               (int32)z_FTCal.f_TCalMinValueNsec);
      MSG_HIGH("F-TCal PPM TOA CCx16 %d max %d min %d",
               (int32)z_FtcalData.p_Ppm->f_PpmAverToaCCx16,
               z_FtcalData.p_Ppm->x_PpmMaxToaCCx16,
               z_FtcalData.p_Ppm->x_PpmMinToaCCx16);
      MSG_HIGH("F-TCal PPM Win cent %dCCx16 num smpls %u Rollover corr %dNsec",
               z_FtcalData.p_Ppm->l_PpmSrchWinCenterCCx16,
               z_FtcalData.p_Ppm->q_PpmNumToaSamplesAccumulated,
               (int32)(f_TCalRolloverCorrectionMsec * 1000000.0));
      MSG_HIGH("F-TCal PPM Ec/Io avr %ddB max %d min %d",
               (int32)z_FtcalData.p_Ppm->f_PpmAverEcIodB,
               (int32)z_FtcalData.p_Ppm->f_PpmMaxEcIodB,
               (int32)z_FtcalData.p_Ppm->f_PpmMinEcIodB);
      MSG_HIGH("F-TCal TT GNSS RTC @S80 %dGCx80 max %d min %d",
               (int32)z_FtcalData.p_TT->f_TtGnssRtcS80SubMsecGCx80,
               (int32)z_FtcalData.p_TT->f_TtMaxGnssRtcS80SubMsecGCx80,
               (int32)z_FtcalData.p_TT->f_TtMinGnssRtcS80SubMsecGCx80);
      MSG_HIGH("F-TCal TT Ref CDMA RTC @S80 %d+%dCCx16 num smpls %u",
               (int32)z_FtcalData.p_TT->f_TtRefCdmaRtcS80SubPnRollCCx16,
               (int32)z_FtcalData.p_TT->f_TtRefCdmaRtcS80AdjustSubPnRollCCx16,
               z_FtcalData.p_TT->q_TtNumTTSamplesAccumulated);
      MSG_HIGH("F-TCal GPS CodePh %dGCx80 CodePh %ums FCnt %ums",
               (int32)(f_GpsCodePhSubMs * ( GPS_CHIPS_PER_MS_FLT * CHIPS_PER_CX80_FLT)),
               p_GpsMeas->z_FiltMeas.z_SvTime.q_Ms,
               p_GpsMeas->q_FCount);
      MSG_HIGH("F-TCal GPS C/No %d Latency %dms Speed %dm/s",
               (p_GpsMeas->w_Cno + 5 + input_prm_buf_ptr->z_MeasBlk.z_Gnss[0].w_RfLoss) / 10,
               p_GpsMeas->l_LatencyMs,
               (int32)(input_prm_buf_ptr->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias -
               p_GpsMeas->z_FiltMeas.f_SvSpeed) );
    }

    /* Log  LOG_GNSS_FAST_TCAL_C */
    if ( !sm_log_ftcal_data( z_FtcalData.p_Ppm, 
                             z_FtcalData.p_TT, &z_FTCalGps, &z_FTCal ))
    {
      MSG_ERROR("F-TCal Logging failed",0,0,0);
    }

    /* Event EVENT_GNSS_FAST_TCAL_END  */
   
    /* Validate and Crop the values if required as EVENT can have a max of 32 bytes */
    if (z_FTCal.f_TCalAverValueNsec >= CGPS_MAX_INT16 )
    {
      x_EventTCal = CGPS_MAX_INT16;
    }
    else if (z_FTCal.f_TCalAverValueNsec <= CGPS_MIN_INT16 )
    {
      x_EventTCal = CGPS_MIN_INT16;
    }
    else 
    {
      /* Round off the float value to the nearest integer */
      x_EventTCal = (int16)(z_FTCal.f_TCalAverValueNsec < 0 
                        ? z_FTCal.f_TCalAverValueNsec - 0.5 
                        : z_FTCal.f_TCalAverValueNsec + 0.5);      
    }

    if (z_FTCal.q_TCalUncertaintyNsec >= CGPS_MAX_UINT8 )
    {
      u_EventTCalUncertainityNsec = CGPS_MAX_UINT8;
    }
    else 
    {
      u_EventTCalUncertainityNsec =  (uint8) z_FTCal.q_TCalUncertaintyNsec;  
    }

    /* Report the event through SM */
    sm_report_event_gnss_fast_tcal_end ( x_EventTCal,
                                         u_EventTCalUncertainityNsec,
                                         z_FTCal.u_ErrorCode);

    /* Fast TCal session is done */
    tm_is801_ftcal_enabled( FALSE );
  }

  return ret_val;
}

/*===========================================================================

FUNCTION is801_util_flink_convert_aa

DESCRIPTION: This function converts an is801 aa resp into MGP reqd aa data, i.e.
             steering and SV directions. Its possible to convert 801 aa data into
	     only Steering or only SV directions.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_aa
(
  is801_aa_resp_type               *input_aa_buf_ptr,
  gnss_SvAcqAssistCommonStructType *output_aa_buf_ptr,
  gnss_SvDirStructType             *output_sv_dir_buf_ptr 
)
{
  int i;
  
  if(input_aa_buf_ptr == NULL)
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }


  if(output_aa_buf_ptr != NULL)
  {
    /* Convert the AA Data
    */
    output_aa_buf_ptr->w_RefPn           = input_aa_buf_ptr->fix.reference_pn;
    output_aa_buf_ptr->q_Msec            = 0;
    output_aa_buf_ptr->q_TimeOfApp       = input_aa_buf_ptr->fix.time_of_app;
    output_aa_buf_ptr->u_NumSvs          = input_aa_buf_ptr->fix.num_sv;
    output_aa_buf_ptr->u_RefPnPhaseValid = FALSE;
    output_aa_buf_ptr->u_TimeRefIsGnss   = FALSE;
    output_aa_buf_ptr->w_Week            = 0;
    output_aa_buf_ptr->q_SvSteerSrc      = MGP_GNSS_SV_STEER_SRC_GPS;

    /* Cache the first SV prn_num in case we need to send bogus PRMs */
    if(input_aa_buf_ptr->fix.num_sv > 0)
    {
      first_sv_prn_num = input_aa_buf_ptr->var[0].sv_prn_num;
    }
    for (i = 0; i < output_aa_buf_ptr->u_NumSvs; i++)
    {
      mgp_IS801AA2SteeringInfo(&(input_aa_buf_ptr->var[i]),
                               &(output_aa_buf_ptr->z_SvSteerInfo[i]));
    }
  }
  else
  {
    MSG_HIGH("Null pointer output_aa_buf_ptr", 0, 0, 0);
  }


  /* Convert the SV Direction
  */
  if(output_sv_dir_buf_ptr != NULL)
  {
    output_sv_dir_buf_ptr->u_NumSvs = input_aa_buf_ptr->fix.num_sv;
    output_sv_dir_buf_ptr->q_GnssType = MGP_GNSS_TYPE_GPS;

    for (i = 0; i < output_sv_dir_buf_ptr->u_NumSvs; i++)
    {
      mgp_IS801AA2AzElInfo(&(input_aa_buf_ptr->var[i]),
                           &(output_sv_dir_buf_ptr->z_SvAzElInfo[i]));
    }  
  }
  else
  {
    MSG_HIGH("Null pointer output_sv_dir_buf_ptr", 0, 0, 0);
  }
  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_flink_convert_sa

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_sa
(
  is801_sa_resp_type     *input_sa_buf_ptr,
  gps_SADataStructType   *output_sa_buf_ptr
)
{
uint8 u_DataRecordSize;
uint8 u_SvIndex = 0;
uint16 w_ReceiveTimeNavBitNumber;
int16 w_NavBitDiff;
uint16 w_NavDwordSize;
int32 l_BitDiff;
uint32 q_I, q_J, q_K;
uint32 q_NavDword;
is801_sa_dr_type *p_DataRecord;
gps_SvSADataStructType *p_SvSADataInfo;
time_type z_SaRcvTs, z_Quotient, z_FirstNavBitTsInBit, z_StartTsMs, z_ExpirationTsMs;


  if((input_sa_buf_ptr == NULL) ||(output_sa_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  /*---------------------------------
    Calculate the NAV bit number at the time instance when SA msg 
    was received. 
   *-------------------------------*/
  qw_equ(z_SaRcvTs, input_sa_buf_ptr->timestamp);
  qw_shift(z_SaRcvTs, -20);  /* Convert time stamp to value in 20ms units. */

  /* 1500 bits in GPS Frame, return is z_SaRcvTs mod 1500, z_Quotient returns z_SaRcvTs/1500 */
  w_ReceiveTimeNavBitNumber = qw_div(z_Quotient, z_SaRcvTs, 1500);

  output_sa_buf_ptr->u_NumSvs = input_sa_buf_ptr->num_data_records;

  /* First loop through the data records */
  for (q_I = 0; q_I < input_sa_buf_ptr->num_data_records; q_I++)
  {
    p_DataRecord = &input_sa_buf_ptr->data[q_I];


    /*---------------------------------
      Calculate difference between NAV bit number at the time SA msg 
      was received and reference NAV bit number.
      If w_NavBitDiff is greater than zero, reference NAV bit number
      is ahead of the receive time NAV bit number. 
     *-------------------------------*/
    w_NavBitDiff = (int16) p_DataRecord->ref_bit_num - (int16) w_ReceiveTimeNavBitNumber;
    if(w_NavBitDiff >= 750) 
    {
      w_NavBitDiff -= 1500;
    }
    else if(w_NavBitDiff < -750) 
    {
      w_NavBitDiff += 1500;
    }

    u_DataRecordSize = p_DataRecord->dr_size; /* In 2bit units */
    w_NavDwordSize = (u_DataRecordSize*2 + 31) / 32; 

    /* In each data record, loop through the SVs */
    for (q_J = 0; q_J < p_DataRecord->num_sv_dr && u_SvIndex < N_SV; q_J++)
    {
      p_SvSADataInfo = &output_sa_buf_ptr->z_SvSADataInfo[u_SvIndex];

      p_SvSADataInfo->u_Sv = p_DataRecord->sv_prn_num[q_J];

      if(w_NavDwordSize == 0 || w_NavDwordSize > MAX_NAV_BITS_IN_DWORD)
      {
        /* sanity check */
        MSG_ERROR("SA msg sv#%2d NAV siz %d out of range", 
                   p_SvSADataInfo->u_Sv, w_NavDwordSize, 0);
      }
      else
      {
        /*---------------------------------
          Calculate time stamp corresponding to the start NAV bit. 
          Example. Start time in the future w.r.t the receive time:

              <-------- w_NavBitDiff ------------------------>
              <---- X ----><-- (u_DataRecordSize*2)/2 - 1 --->
           ===|============|==================================|===========>
           RcvBit     FirstBit                             RefBit
           RcvTs      FirstTs
                      StartTs

          RefBit is referenced to the last bit of the first half of the total
          number of bits.  The total number of bits is u_DataRecordSize*2 since
          the units are in 2 bits.  Since we want the last bit index, we would
          subtract 1 giving (u_DataRecordSize*2)/2 - 1.

          Thus, FirstTS = RcvTS + X = RcvTS + w_NavBitDiff - u_DataRecordSize;
                StartTS = FirstTS;

         *-------------------------------*/
        l_BitDiff = w_NavBitDiff - (u_DataRecordSize - 1);  
        qw_equ(z_FirstNavBitTsInBit, z_SaRcvTs);
        if (l_BitDiff < 0)
        {
          /*---------------------------------
            Start time is already in the past.
           *-------------------------------*/
          qw_dec(z_FirstNavBitTsInBit, (uint32)(-l_BitDiff));
        }
        else
        {
          /*---------------------------------
            Start time is in the future w.r.t the receive time.
           *-------------------------------*/
          qw_inc(z_FirstNavBitTsInBit, (uint32) l_BitDiff );
        }

        qw_equ(p_SvSADataInfo->z_FirstNavBitTsInBit,z_FirstNavBitTsInBit);

        /* Calculate the Start Timestamp of the SA data */
        qw_mul(z_StartTsMs, z_FirstNavBitTsInBit, 20);
        gps_week_ms(z_StartTsMs,
                    &p_SvSADataInfo->z_StartTs.w_GpsWeek,
                    &p_SvSADataInfo->z_StartTs.q_GpsMsec);

        /* Print out only once since it is the same for ALL SVs */
        if (q_I == 0)
        {
          MSG_MED("SAData: StartTS Wk %d, ms %d",
                  p_SvSADataInfo->z_StartTs.w_GpsWeek,
                  p_SvSADataInfo->z_StartTs.q_GpsMsec,0);
        }

        /*---------------------------------
          Calculate time stamp corresponding to the expiration NAV bit.
         *-------------------------------*/
        l_BitDiff = w_NavBitDiff + u_DataRecordSize;
        qw_equ(z_ExpirationTsMs, z_SaRcvTs);
        if (l_BitDiff < 0)
        {
          qw_dec(z_ExpirationTsMs, (uint32)(-l_BitDiff));
        }
        else
        {
          qw_inc(z_ExpirationTsMs, (uint32) l_BitDiff);
        }
        qw_mul(z_ExpirationTsMs, z_ExpirationTsMs, 20);

        gps_week_ms(z_ExpirationTsMs,
                    &p_SvSADataInfo->z_ExpirationTs.w_GpsWeek,
                    &p_SvSADataInfo->z_ExpirationTs.q_GpsMsec);

        /* Print out only once since it is the same for ALL SVs */
        if (q_I == 0)
        {
          MSG_MED("SAData: ExpireTS Wk %d, ms %d",
                  p_SvSADataInfo->z_ExpirationTs.w_GpsWeek,
                  p_SvSADataInfo->z_ExpirationTs.q_GpsMsec,0);
        }

        p_SvSADataInfo->u_Valid = TRUE;
        p_SvSADataInfo->w_RefBitNum = p_DataRecord->ref_bit_num;
        p_SvSADataInfo->w_NumDataBits = u_DataRecordSize*2;

        for (q_K = 0; q_K < w_NavDwordSize*4; q_K += 4)
        {
          /*---------------------------------
            Pack NAV Bits according to DSP endianess requirement.
           *-------------------------------*/
          q_NavDword = (((uint32)p_DataRecord->nav_msg_bits[q_K]) << 24) |
                       (((uint32)p_DataRecord->nav_msg_bits[q_K+1]) << 16) |
                       (((uint32)p_DataRecord->nav_msg_bits[q_K+2]) << 8) |
                       ((uint32)p_DataRecord->nav_msg_bits[q_K+3]);
          p_SvSADataInfo->q_NavBits[q_K/4] = q_NavDword;
        }

        u_SvIndex++;
      }

    }


  }


  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_flink_convert_iono

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_iono
(
  is801_eph_resp_type    *input_eph_buf_ptr,
  gps_IonoStructType     *output_iono_buf_ptr
)
{
  if((input_eph_buf_ptr == NULL) ||(output_iono_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  output_iono_buf_ptr->u_Alpha0 = input_eph_buf_ptr->fix.u_Alpha_0;
  output_iono_buf_ptr->u_Alpha1 = input_eph_buf_ptr->fix.u_Alpha_1;
  output_iono_buf_ptr->u_Alpha2 = input_eph_buf_ptr->fix.u_Alpha_2;
  output_iono_buf_ptr->u_Alpha3 = input_eph_buf_ptr->fix.u_Alpha_3;  
  output_iono_buf_ptr->u_Beta0  = input_eph_buf_ptr->fix.u_Beta_0;
  output_iono_buf_ptr->u_Beta1  = input_eph_buf_ptr->fix.u_Beta_1;
  output_iono_buf_ptr->u_Beta2  = input_eph_buf_ptr->fix.u_Beta_2;
  output_iono_buf_ptr->u_Beta3  = input_eph_buf_ptr->fix.u_Beta_3;

  return TRUE;
}



/*===========================================================================

FUNCTION is801_util_flink_convert_eph

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_eph
(
  is801_eph_resp_type    *input_eph_buf_ptr,
  gps_EphStructType      *output_eph_buf_ptr,
  int                    sv_num
)
{
  qword       milliseconds;
  uint32      temp;
  
  if((input_eph_buf_ptr == NULL) ||(output_eph_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  if((sv_num < 0) || (sv_num >= input_eph_buf_ptr->fix.u_TotalSats))
  {
    MSG_ERROR("Invalid sv_num", 0, 0, 0);
    return FALSE;  
  }

  output_eph_buf_ptr->u_Sv       = input_eph_buf_ptr->var[sv_num].u_Sv_Prn_Num;
  output_eph_buf_ptr->u_Iode     = input_eph_buf_ptr->var[sv_num].u_Iode;
  output_eph_buf_ptr->w_Crs      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Rs;
  output_eph_buf_ptr->w_DeltaN   = (uint16)input_eph_buf_ptr->var[sv_num].x_Delta_N;
  output_eph_buf_ptr->q_M0       = (uint32)input_eph_buf_ptr->var[sv_num].l_M0;
  output_eph_buf_ptr->w_Cuc      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Uc;
  output_eph_buf_ptr->q_E        = (uint32)input_eph_buf_ptr->var[sv_num].l_Eccentricity;
  output_eph_buf_ptr->w_Cus      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Us;
  output_eph_buf_ptr->q_SqrtA    = (uint32)input_eph_buf_ptr->var[sv_num].l_A_Sqrt;
  output_eph_buf_ptr->w_Toe      = (uint16)input_eph_buf_ptr->var[sv_num].x_Toe;
  output_eph_buf_ptr->w_Cic      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Ic;
  output_eph_buf_ptr->q_Omega0   = (uint32)input_eph_buf_ptr->var[sv_num].l_Omega_0;
  output_eph_buf_ptr->w_Cis      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Is;
  output_eph_buf_ptr->q_I0       = (uint32)input_eph_buf_ptr->var[sv_num].l_Angle_Inclination;
  output_eph_buf_ptr->w_Crc      = (uint16)input_eph_buf_ptr->var[sv_num].x_C_Rc;
  output_eph_buf_ptr->q_Omega    = (uint32)input_eph_buf_ptr->var[sv_num].l_Omega;
  output_eph_buf_ptr->q_OmegaDot = (uint32)input_eph_buf_ptr->var[sv_num].l_Omegadot;
  output_eph_buf_ptr->w_Idot     = (uint16)input_eph_buf_ptr->var[sv_num].x_Idot;
  output_eph_buf_ptr->w_Toc      = (uint16)input_eph_buf_ptr->var[sv_num].x_Toc;
  output_eph_buf_ptr->u_Af2      = input_eph_buf_ptr->var[sv_num].b_Af2;
  output_eph_buf_ptr->w_Af1      = (uint16)input_eph_buf_ptr->var[sv_num].b_Af1;
  output_eph_buf_ptr->q_Af0      = (uint32)input_eph_buf_ptr->var[sv_num].l_Af0;

  /* Not set following to hardcode value for now */
  output_eph_buf_ptr->u_CodeL2             = 0;
  output_eph_buf_ptr->u_Accuracy           = 0;
  output_eph_buf_ptr->u_Health             = 1;
  output_eph_buf_ptr->w_Iodc               = input_eph_buf_ptr->var[sv_num].u_Iode;   
  output_eph_buf_ptr->u_PcodeFlagIsValid   = FALSE;     
  output_eph_buf_ptr->u_Sf1BitsAreValid    = FALSE;
  output_eph_buf_ptr->u_TgdIsValid         = FALSE;
  output_eph_buf_ptr->u_Tgd                = 0;     
  output_eph_buf_ptr->u_AodoIsValid        = FALSE;

  /* Time of EPH */
  output_eph_buf_ptr->u_GpsWeekIsValid    = TRUE;

  gps_total_msec( input_eph_buf_ptr->timestamp, milliseconds );

  /* Separate out week and msec */
  gps_week_ms( milliseconds, &(output_eph_buf_ptr->w_GpsWeek), &temp);

  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_flink_convert_alm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_alm
(
  is801_alm_resp_type    *input_alm_buf_ptr,
  gps_AlmStructType      *output_alm_buf_ptr,
  int                    sv_num  
)
{
  qword       milliseconds;
  uint32      temp;

  if((input_alm_buf_ptr == NULL) ||(output_alm_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  if((sv_num < 0) || (sv_num >= input_alm_buf_ptr->fix.u_TotalSats))
  {
    MSG_ERROR("Invalid sv_num", 0, 0, 0);
    return FALSE;  
  }  

  
  output_alm_buf_ptr->u_Sv                = input_alm_buf_ptr->var[sv_num].u_Sv_Prn_Num;
  output_alm_buf_ptr->u_DataIdIsValid     = FALSE;
  //output_alm_buf_ptr->u_DataId            = input_alm_buf_ptr->var[sv_num]. 
  output_alm_buf_ptr->w_E                 = input_alm_buf_ptr->var[sv_num].w_E;
  output_alm_buf_ptr->u_Toa               = input_alm_buf_ptr->var[sv_num].u_Toa;
  output_alm_buf_ptr->w_DeltaI            = input_alm_buf_ptr->var[sv_num].w_DeltaI;
  output_alm_buf_ptr->w_OmegaDot          = input_alm_buf_ptr->var[sv_num].w_OmegaDot;
  output_alm_buf_ptr->u_Health            = 0;
  output_alm_buf_ptr->q_SqrtA             = input_alm_buf_ptr->var[sv_num].q_SqrtA;
  output_alm_buf_ptr->q_Omega0            = input_alm_buf_ptr->var[sv_num].q_OmegaZero;
  output_alm_buf_ptr->q_M0                = input_alm_buf_ptr->var[sv_num].q_MZero;
  output_alm_buf_ptr->q_Omega             = input_alm_buf_ptr->var[sv_num].q_Omega;
  output_alm_buf_ptr->w_Af0               = input_alm_buf_ptr->var[sv_num].w_Af0;
  output_alm_buf_ptr->w_Af1               = input_alm_buf_ptr->var[sv_num].w_Af1;
  output_alm_buf_ptr->u_SvGhealthIsValid  = FALSE;
  //output_alm_buf_ptr->q_SvGhealth[12]     = input_alm_buf_ptr->var[sv_num].
  output_alm_buf_ptr->w_GpsWeek           = input_alm_buf_ptr->var[sv_num].u_WeekNum;
  /* akench Temp Hack for now. PE should take care of this. Time of ALM */

  gps_total_msec( input_alm_buf_ptr->timestamp, milliseconds );

  /* Separate out week and msec */
  gps_week_ms( milliseconds, &(output_alm_buf_ptr->w_GpsWeek), &temp);    

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_util_convert_tm_lr_to_is801_lr

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean is801_util_convert_tm_lr_to_is801_lr
(
  sm_GnssFixRptStructType  *input_lr_buf_ptr,
  is801_lr_resp_type *output_lr_buf_ptr
)
{
  DBL                     heading_angle;
  DBL                     vel_east;
  DBL                     vel_north;
  qword                   fix_time;
  gps_TimeStructType      z_Clock;   
  DBL                     f_tmp;

  if ((output_lr_buf_ptr == NULL) ||(input_lr_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  /* Make sure output buffer is cleared prior to filling */
  memset((void*) output_lr_buf_ptr, 0, sizeof(*output_lr_buf_ptr));

  z_Clock.w_GpsWeek = input_lr_buf_ptr->z_NavPos.w_GpsWeek;
  z_Clock.q_GpsMsec = input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;
  gps_to_cdma_time(&z_Clock, output_lr_buf_ptr->timestamp);

  if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
  {
    /* Only GPS constellation was used in fix. */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GPS; 
  }   
 
  /* Update the local copy of qRefFCount with the fix time also because
   * it will be used to generate the time_ref_cdma.
  */

  /* This code is updating the input_lr_buf_ptr data */
  input_lr_buf_ptr->z_NavPos.q_RefFCount = input_lr_buf_ptr->z_NavPos.w_GpsWeek * MS_PER_WEEK + input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;

  /* Position CDMA time stamp in seconds modulo 16384.
   * Assumes that q_RefFCount was derived from CDMA time.
  */
  /* Convert CDMA timestamp to milliseconds */
  gps_total_msec( output_lr_buf_ptr->timestamp, fix_time );

  /* Use only the upper half of the current time along
   * q_RefFCount measured in milliseconds.
  */
  fix_time[0] = input_lr_buf_ptr->z_NavPos.q_RefFCount;

  /* Save msec portion of LR fix time */
  output_lr_buf_ptr->timestamp_msec = (int16)(input_lr_buf_ptr->z_NavPos.q_GpsTimeMs % 1000);
  output_lr_buf_ptr->msec_incl = 1;

  /* Convert to seconds */
  (void) qw_div(fix_time, fix_time, 1000);
  /* Modulo 16384 */
  output_lr_buf_ptr->time_ref_cdma = (uint16) (fix_time[0] & 0x3FFF);

  /* Convert latitude from radians to degrees and scale. */
  output_lr_buf_ptr->latitude = tm_util_nearest_long( input_lr_buf_ptr->z_NavPos.d_PosLla[0] * RAD2DEG *
                                                      LAT_LONG_SCALE );

  /* Convert longitude from radians to degrees and scale. */
  output_lr_buf_ptr->longitude = tm_util_nearest_long( input_lr_buf_ptr->z_NavPos.d_PosLla[1] * RAD2DEG *
                                                       LAT_LONG_SCALE );

#ifdef FEATURE_REDUCE_PDAPI_POSREP_RESOLUTION_TO_GAD
  output_lr_buf_ptr->latitude = 
    tm_util_nearest_long( input_lr_buf_ptr->z_NavPos.d_PosLla[0] * C_LAT_RAD_TO_GAD );
  output_lr_buf_ptr->latitude *= 2;


  output_lr_buf_ptr->longitude = 
    tm_util_nearest_long( input_lr_buf_ptr->z_NavPos.d_PosLla[1] * C_LON_RAD_TO_GAD );
  output_lr_buf_ptr->longitude *= 4;
#endif

  /* Set axis to True North for position uncertainty. */
  output_lr_buf_ptr->loc_uncrtnty_ang = (byte) (input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[0]/5.625F);

  /* Format the LOC_UNCRTNTY_A field. */
  output_lr_buf_ptr->loc_uncrtnty_a = (uint8) tm_util_select_pos_unc( input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1] );

  /* Format the LOC_UNCRTNTY_P field. */
  output_lr_buf_ptr->loc_uncrtnty_p = (uint8) tm_util_select_pos_unc( input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2] );

  /* 0: 2D, 1: 3D */
  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_Is3D)
  {
    output_lr_buf_ptr->fix_type = 1;
  }
  else
  {
    output_lr_buf_ptr->fix_type = 0;
  }

  /* Velocity information is included */
  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsVelValid)
    output_lr_buf_ptr->velocity_incl = 1;
  else
  {
    MSG_HIGH( "Velocity fix failed", 0, 0, 0);
    output_lr_buf_ptr->velocity_incl = 0;
  }

  /* Compute horizontal velocity from components and scale
   * to units of 0.25 m/s.
  */
  vel_east = input_lr_buf_ptr->z_NavPos.f_VelEnu[0];
  vel_north = input_lr_buf_ptr->z_NavPos.f_VelEnu[1];

  f_tmp = sqrt( vel_east * vel_east + vel_north * vel_north );

  
  if (f_tmp < TM_CORE_MIN_VELOCTY)
  {
    output_lr_buf_ptr->velocity_hor = 0;  
  }
  else
  {
    output_lr_buf_ptr->velocity_hor =
      (uint16) tm_util_nearest_long( 4.0 * f_tmp);
  }
  
  tm_util_calculate_heading(input_lr_buf_ptr, &heading_angle);

  output_lr_buf_ptr->heading = (uint16) tm_util_nearest_long( heading_angle * HEADING_SCALE );
  /* heading & uncertainty in radian */
  output_lr_buf_ptr->f_HeadingRad = input_lr_buf_ptr->z_NavPos.f_HeadingRad;
  output_lr_buf_ptr->f_HeadingUncRad = input_lr_buf_ptr->z_NavPos.f_HeadingUncRad;
  output_lr_buf_ptr->f_GnssHeadingRad = input_lr_buf_ptr->z_NavPos.f_GnssHeadingRad;
  output_lr_buf_ptr->f_GnssHeadingUncRad = input_lr_buf_ptr->z_NavPos.f_GnssHeadingUncRad;

  if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
  {
    output_lr_buf_ptr->heading_valid = TRUE;
  }
  else
  {
    output_lr_buf_ptr->heading_valid = FALSE;
  }

  /* If fix_type = 1, set the vertical velocity in units of 0.5 m/s. */
  if ( output_lr_buf_ptr->fix_type )
  {
    output_lr_buf_ptr->velocity_ver =
      (int1) tm_util_nearest_long( 2.0 * input_lr_buf_ptr->z_NavPos.f_VelEnu[2] );
  }

  /* Include clock information. */
  output_lr_buf_ptr->clock_incl = 1;
  output_lr_buf_ptr->clock_bias = (sint31)(IS801_CLOCK_BIAS_OFFSET -
                                           input_lr_buf_ptr->z_NavPos.f_ClockBias / (FLT) LIGHT_MSEC * 1.0E6);
  output_lr_buf_ptr->clock_drift = (sint15)(- input_lr_buf_ptr->z_NavPos.f_ClockDriftRate *
                                            1.0E9 / LIGHT_SEC);

  /* Indicate that the HEIGHT field is included. */
  output_lr_buf_ptr->height_incl = 1;

  /* HEIGHT field */
  /* height above the WGS-84 reference ellipsoid hence add 500m */
  output_lr_buf_ptr->f_altitude = input_lr_buf_ptr->z_NavPos.d_PosLla[2];
  output_lr_buf_ptr->f_altitude += 500.0;
  output_lr_buf_ptr->height = (sint15) tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[2]);
  /* Format the LOC_UNCRTNTY_V field. */
  output_lr_buf_ptr->height += 500;

  /* Format the LOC_UNCRTNTY_V field. */
  output_lr_buf_ptr->loc_uncrtnty_v = (uint8) tm_util_select_pos_unc( input_lr_buf_ptr->z_NavPos.f_PuncVertMeters );

  output_lr_buf_ptr->f_clock_bias_unc = input_lr_buf_ptr->z_NavPos.f_ClockBiasUncMs;
  /* Time source parameters: pdsm_extended_time_src_e_type */
  output_lr_buf_ptr->u_TimeSource = input_lr_buf_ptr->z_NavPos.u_InitGpsTimeSrc;

  output_lr_buf_ptr->e_HoriRelIndicator = input_lr_buf_ptr->z_NavPos.e_HoriRelIndicator;
  output_lr_buf_ptr->e_VertRelIndicator = input_lr_buf_ptr->z_NavPos.e_VertRelIndicator;
  output_lr_buf_ptr->w_SensorAidingIndicator =
    input_lr_buf_ptr->z_NavPos.w_SensorAidingIndicator;
  output_lr_buf_ptr->w_SensorDataUsageMask =
    input_lr_buf_ptr->z_NavPos.w_SensorDataUsageMask;

  /* Check the flag which indicates if the SV use in fix report is valid */
  if (input_lr_buf_ptr->z_NavPos.z_SmPosFlags.b_IsSatellite)
  {
    output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid = TRUE;
  }
  else
  {
    output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid = FALSE;
  }

  /* Copy only the GPS part of the SVInfo structure */
  output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid     =  input_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvMeas    =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvMeas;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix  =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvVelFix  =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvVelFix;
  output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed   =  input_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed;
  output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable =  input_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable;

  memscpy(
    (void*) &output_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo[0], 
    sizeof(output_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo), 
    (const void*) &input_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo[0], 
    sizeof(input_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo));

  return TRUE;
}
/*===========================================================================

FUNCTION is801_util_convert_is801_to_tm_lr

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_convert_is801_to_tm_lr
(
  tm_lr_resp_type          *output_lr_buf_ptr,
  const is801_lr_resp_type *input_lr_buf_ptr
)
{
  if((output_lr_buf_ptr == NULL) ||(input_lr_buf_ptr == NULL))
  {
    return FALSE;
  }
  output_lr_buf_ptr->timestamp[0]       =  input_lr_buf_ptr->timestamp[0];
  output_lr_buf_ptr->timestamp[1]       =  input_lr_buf_ptr->timestamp[1];
  output_lr_buf_ptr->timestamp_msec     =  input_lr_buf_ptr->timestamp_msec;
  output_lr_buf_ptr->msec_incl          =  input_lr_buf_ptr->msec_incl;
  output_lr_buf_ptr->time_ref_cdma      =  input_lr_buf_ptr->time_ref_cdma;
  output_lr_buf_ptr->latitude           =  input_lr_buf_ptr->latitude;
  output_lr_buf_ptr->longitude          =  input_lr_buf_ptr->longitude;
  output_lr_buf_ptr->loc_uncrtnty_ang   =  input_lr_buf_ptr->loc_uncrtnty_ang;
  output_lr_buf_ptr->loc_uncrtnty_a     =  input_lr_buf_ptr->loc_uncrtnty_a;
  output_lr_buf_ptr->loc_uncrtnty_p     =  input_lr_buf_ptr->loc_uncrtnty_p;
  output_lr_buf_ptr->fix_type           =  input_lr_buf_ptr->fix_type;
  output_lr_buf_ptr->velocity_incl      =  input_lr_buf_ptr->velocity_incl;
  output_lr_buf_ptr->velocity_hor       =  input_lr_buf_ptr->velocity_hor;
  output_lr_buf_ptr->heading_valid      =  input_lr_buf_ptr->heading_valid;
  output_lr_buf_ptr->heading            =  input_lr_buf_ptr->heading;
  output_lr_buf_ptr->velocity_ver       =  input_lr_buf_ptr->velocity_ver;
  output_lr_buf_ptr->clock_incl         =  input_lr_buf_ptr->clock_incl;
  output_lr_buf_ptr->clock_bias         =  input_lr_buf_ptr->clock_bias;
  output_lr_buf_ptr->clock_drift        =  input_lr_buf_ptr->clock_drift;
  output_lr_buf_ptr->height_incl        =  input_lr_buf_ptr->height_incl;
  output_lr_buf_ptr->height             =  input_lr_buf_ptr->height;
  output_lr_buf_ptr->loc_uncrtnty_v     =  input_lr_buf_ptr->loc_uncrtnty_v;
  output_lr_buf_ptr->time_src           =  input_lr_buf_ptr->time_src;
  output_lr_buf_ptr->f_GGTBMs           =  input_lr_buf_ptr->f_GGTBMs;
  output_lr_buf_ptr->f_GGTBUncMs        =  input_lr_buf_ptr->f_GGTBUncMs;
  output_lr_buf_ptr->e_HoriRelIndicator =  input_lr_buf_ptr->e_HoriRelIndicator;
  output_lr_buf_ptr->e_VertRelIndicator =  input_lr_buf_ptr->e_VertRelIndicator;
  output_lr_buf_ptr->w_SensorDataUsageMask   =  input_lr_buf_ptr->w_SensorDataUsageMask;
  output_lr_buf_ptr->w_SensorAidingIndicator =  input_lr_buf_ptr->w_SensorAidingIndicator;
  output_lr_buf_ptr->f_clock_bias_unc   =  input_lr_buf_ptr->f_clock_bias_unc;
  output_lr_buf_ptr->u_TimeSource       =  input_lr_buf_ptr->u_TimeSource;
  output_lr_buf_ptr->f_HeadingRad       =  input_lr_buf_ptr->f_HeadingRad;
  output_lr_buf_ptr->f_HeadingUncRad    =  input_lr_buf_ptr->f_HeadingUncRad;
  output_lr_buf_ptr->f_GnssHeadingRad   =  input_lr_buf_ptr->f_GnssHeadingRad;
  output_lr_buf_ptr->f_GnssHeadingUncRad=  input_lr_buf_ptr->f_GnssHeadingUncRad;
  output_lr_buf_ptr->f_altitude         =  input_lr_buf_ptr->f_altitude;
  
  output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid     =  input_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvMeas    =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvMeas;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix  =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix;
  output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvVelFix  =  input_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvVelFix;
  output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed   =  input_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed;
  output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable =  input_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable;

  memscpy(
    (void*) output_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo, 
    sizeof(output_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo), 
    (void*) input_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo, 
    sizeof(input_lr_buf_ptr->z_NavSvInfo.z_GpsSvInfo));
  
  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_pd_info_data_init

DESCRIPTION
  This function is used to initialize the PD related data for sending
  it to clients.

RETURN VALUE
  Events to be set for Client's.

DEPENDENCIES

===========================================================================*/
pdsm_pd_event_type is801_util_pd_info_data_init
(
 pdsm_pd_info_s_type           *cl_info, 
 const is801_lr_resp_type      *lr_ptr, 
 pdsm_client_id_type           client_id,
 pdsm_pd_dload_data_s_type     *pd_dload_info_ptr,
 tm_pd_position_info_s_type    *pd_pos_info_ptr,
 tm_ext_pos_data_s_type        *ext_pos_data_ptr
)
{
  tm_lr_resp_type              *tm_lr_resp = NULL;
  pdsm_pd_event_type	        pd_evt     = 0x0;

  if((cl_info == NULL) || (lr_ptr == NULL) || (pd_dload_info_ptr == NULL) || 
     (pd_pos_info_ptr == NULL) || (ext_pos_data_ptr == NULL))
  {
    MSG_ERROR("Null pointer!", 0, 0, 0);
    return 0;
  }

  tm_lr_resp = os_MemAlloc(sizeof(*tm_lr_resp), OS_MEM_SCOPE_TASK);
  if(tm_lr_resp == NULL)
  {
    MSG_ERROR("Out of Memory!!", 0, 0, 0);
    return 0;
  }
  memset((void*)tm_lr_resp, 0, sizeof(*tm_lr_resp));
  is801_util_convert_is801_to_tm_lr(tm_lr_resp, lr_ptr);
  
  pd_evt = tm_util_pd_info_data_init( cl_info,
                                      tm_lr_resp, 
                                      client_id, 
                                      pd_dload_info_ptr, 
                                      pd_pos_info_ptr, 
                                      ext_pos_data_ptr );
  
  os_MemFree((void **)&tm_lr_resp);

  return(pd_evt);
}

/*===========================================================================

FUNCTION is801_util_flink_convert_seed

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_util_flink_convert_seed
(
  const is801_lr_resp_type    *input_lr_buf_ptr,
  gps_RefLocStructType  *output_seed_buf_ptr
)
{
  qword       milliseconds;
  qword       seconds;
  qword       temp;
  uint32      orig_time;
  float       f_PosUnc;       /* Horizontal or position uncertainty. */
  float       f_temp;
  
  if((input_lr_buf_ptr == NULL) ||(output_seed_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  /*
   * 3D position.
  */
  output_seed_buf_ptr->d_Latitude  = ((DBL) input_lr_buf_ptr->latitude) * DEG2RAD / LAT_LONG_SCALE;

  output_seed_buf_ptr->d_Longitude = ((DBL) input_lr_buf_ptr->longitude) * DEG2RAD / LAT_LONG_SCALE;

  output_seed_buf_ptr->u_AltitudeIsValid = input_lr_buf_ptr->height_incl;
 
  if (input_lr_buf_ptr->height_incl)
  {
    output_seed_buf_ptr->f_Altitude    = (FLT) (input_lr_buf_ptr->height - 500);
    output_seed_buf_ptr->f_AltitudeUnc = (FLT) f_PosUncArr[input_lr_buf_ptr->loc_uncrtnty_v];
  }
  else
  {
    output_seed_buf_ptr->f_Altitude = 0.0;
    output_seed_buf_ptr->f_AltitudeUnc = 0.0; 
  }

  output_seed_buf_ptr->u_VelocityIsValid = input_lr_buf_ptr->velocity_incl;
  
  if(input_lr_buf_ptr->velocity_incl)
  {
    output_seed_buf_ptr->f_VelHor  = (FLT)(input_lr_buf_ptr->velocity_hor * 0.25);
    output_seed_buf_ptr->f_VelVert = (FLT)(input_lr_buf_ptr->velocity_ver* 0.5);
    output_seed_buf_ptr->u_HeadingIsValid = TRUE;
    output_seed_buf_ptr->f_Heading = (FLT)(input_lr_buf_ptr->heading);  
  }
  else
  {
    output_seed_buf_ptr->f_VelHor  = 0.0;
    output_seed_buf_ptr->f_VelVert = 0.0;
    output_seed_buf_ptr->u_HeadingIsValid = FALSE;
    output_seed_buf_ptr->f_Heading = 0.0;
  }  

  /* Horizontal uncertainty. */
  if ((input_lr_buf_ptr->loc_uncrtnty_a > (MAX_POS_UNC_IND - 3)) ||
      (input_lr_buf_ptr->loc_uncrtnty_p > (MAX_POS_UNC_IND - 3)))
  {
    /*
     * The uncertainty from one of the orthogonal components
     * is too large. Use the maximum value.
     */
    f_PosUnc = f_PosUncArr[MAX_POS_UNC_IND - 1];
    output_seed_buf_ptr->f_LocUncrtntySemiMajor = f_PosUnc;
    output_seed_buf_ptr->f_LocUncrtntySemiMinor = f_PosUnc;
    output_seed_buf_ptr->f_LocUncrtntyAng = 0.0;
  }
  else
  {
    /*
     * Compute the horizontal position uncertainty from the orthogonal
     * components.
    */

    /* Scale up the circle size based on the injected % confidence.
    ** 39% confidence 2D ellipse (1-sigma) value being injected
    */
    FLT f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct( (FLT)LOC_UNC_CONFIDENCE/100.0,0.99f);
    output_seed_buf_ptr->f_LocUncrtntySemiMajor = f_PosUncArr[input_lr_buf_ptr->loc_uncrtnty_a] * f_scale_up_to_99pct_conf;
    output_seed_buf_ptr->f_LocUncrtntySemiMinor  = f_PosUncArr[input_lr_buf_ptr->loc_uncrtnty_p] * f_scale_up_to_99pct_conf;

    /* Convert IS-801 loc_uncrtnty_ang which is in units of 5.625 degrees to f_LocUncrtntyAng which is in radian */
    output_seed_buf_ptr->f_LocUncrtntyAng = input_lr_buf_ptr->loc_uncrtnty_ang  *  5.625F * C_PI/180;

    if ( msg_status(MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      char b_Buffer[250];
      (void) snprintf( b_Buffer, sizeof(b_Buffer), "LocUncMajor=%f LocUncMinor=%f LocUncAng =%f\r\n",
                       output_seed_buf_ptr->f_LocUncrtntySemiMajor,output_seed_buf_ptr->f_LocUncrtntySemiMinor, output_seed_buf_ptr->f_LocUncrtntyAng);
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);
    }

    /* Swap A & P axes if needed
    */
    if(output_seed_buf_ptr->f_LocUncrtntySemiMinor > output_seed_buf_ptr->f_LocUncrtntySemiMajor)
    {                                                                                                                
      f_temp = output_seed_buf_ptr->f_LocUncrtntySemiMinor;
      output_seed_buf_ptr->f_LocUncrtntySemiMinor = output_seed_buf_ptr->f_LocUncrtntySemiMajor;
      output_seed_buf_ptr->f_LocUncrtntySemiMajor = f_temp;

      output_seed_buf_ptr->f_LocUncrtntyAng += C_PI/2; /* PI/2 as f_LocUncrtntyAng is in radians */

      if ( msg_status(MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
      {
        char b_Buffer[250];
        (void) snprintf( b_Buffer, sizeof(b_Buffer), "After swap LocUncMajor=%f LocUncMinor=%f LocUncAng =%f\r\n",
                         output_seed_buf_ptr->f_LocUncrtntySemiMajor,output_seed_buf_ptr->f_LocUncrtntySemiMinor, output_seed_buf_ptr->f_LocUncrtntyAng);
        MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);
      }
    }
  }


  /* Time of position */
  gps_total_msec( input_lr_buf_ptr->timestamp, milliseconds );
  /* Convert to seconds */
  (void) qw_div(seconds, milliseconds, 1000);

  /* Save the current time, so we can check for underflow/overflow */
  orig_time = seconds[0];

  /* Remove the last 14 bits */
  seconds[0] &= ~0x3FFF;
  /* Store TIME_REF_CDMA in a temp qword */
  temp[0] = input_lr_buf_ptr->time_ref_cdma;
  temp[1] = 0;
  /* Reconstruct the time for when the solution is valid using the TIME_REF_CDMA */
  qw_add(seconds, seconds, temp);

  /* Check for underfow/overflow */
  if( (int32)(seconds[0] - orig_time) > 0x2000 )
  {
    seconds[0] -= 0x4000;
  }
  else if( (int32)(orig_time - seconds[0]) > 0x2000 )
  {
    seconds[0] += 0x4000;
  }

  /* Convert back to milliseconds*/
  qw_mul(milliseconds, seconds, 1000);
  /* Separate out week and msec */
  gps_week_ms( milliseconds, &(output_seed_buf_ptr->w_GpsWeek), &(output_seed_buf_ptr->q_GpsMsec) );  
  return TRUE;
}

/*===========================================================================
FUNCTION is801_util_log_rlink_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_util_log_rlink_msg(void *log_buf, byte log_buf_length)
{
  log_gps_rev_msg_type*         is801_rev_log_ptr;

  if((log_buf == NULL) || (log_buf_length == 0))
  {
    MSG_ERROR("Got null log_buf, can not proceed with logging", 0 ,0 ,0);
    return;
  }

  if ( log_status( LOG_GPS_REV_MSG_C ) )
  {
    if ((is801_rev_log_ptr = (log_gps_rev_msg_type *)log_alloc( LOG_GPS_REV_MSG_C, log_buf_length + sizeof(log_hdr_type) + sizeof(byte) )) != NULL)
    {
      is801_rev_log_ptr->hdr_len = 0;
      memscpy((void*)is801_rev_log_ptr->msg, sizeof(is801_rev_log_ptr->msg), (const void*)log_buf, log_buf_length);

      log_commit((log_type*) is801_rev_log_ptr);
     }
  }
} /* tlm_is801_rev_log */


/*===========================================================================
FUNCTION is801_util_log_flink_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_util_log_flink_msg(void *log_buf, uint16 log_buf_length)
{
  log_gps_fwd_msg_type*         is801_fwd_log_ptr;

  if((log_buf == NULL) || (log_buf_length == 0))
  {
    MSG_ERROR("Got null log_buf, can not proceed with logging", 0 ,0 ,0);
    return;
  }

  if (log_status( LOG_GPS_FWD_MSG_C ))
  {
    if ((is801_fwd_log_ptr = (log_gps_fwd_msg_type *)
         log_alloc( LOG_GPS_FWD_MSG_C, log_buf_length + sizeof(log_hdr_type) + sizeof(byte) )) != NULL)
    {
      is801_fwd_log_ptr->hdr_len = 0;
      memscpy((void*)is801_fwd_log_ptr->msg, sizeof(is801_fwd_log_ptr->msg), (const void*)log_buf, log_buf_length);

      log_commit((log_type *)is801_fwd_log_ptr);
    }
  }
}

/*===========================================================================
FUNCTION is801_util_map_flink_bmap

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 is801_util_map_flink_bmap(sm_event_is801_msg_e_type msg_type,uint32 bmap)
{
  if(msg_type == IS801_REQ)
  {
    switch(bmap)
    {
      case IS801_FWD_LINK_REQ_MASK_MSI:
        return TM_MS_INFO_RESP_CODE; 

      case IS801_FWD_LINK_REQ_MASK_PPM:
        return TM_PPM_MEAS_RESP_CODE;

      case IS801_FWD_LINK_REQ_MASK_PRM:
        return TM_PR_MEAS_RESP_CODE;

      case IS801_FWD_LINK_REQ_MASK_TOM:
        return TM_MSTO_MEAS_RESP_CODE;

      case IS801_FWD_LINK_REQ_MASK_LR:
        return TM_LOC_RESP_CODE;

      case IS801_FWD_LINK_REQ_MASK_AMWF:
        return TM_AMWF_RESP_CODE;

      case IS801_FWD_LINK_REQ_MASK_CAN:
        return TM_CACK_RESP_CODE;

      default:
        MSG_MED("IS801 flink req bit 0x%x",bmap,0,0);
        return TM_REJ_RESP_CODE;
    }
  }
  else /* Resp */
  {
    switch(bmap)
    {
      case IS801_FWD_LINK_RESP_MASK_AA:
        return TM_GPS_AA_RESP_CODE;

      case IS801_FWD_LINK_RESP_MASK_SA:
        return TM_GPS_SA_RESP_CODE;

      case IS801_FWD_LINK_RESP_MASK_LR:
        return TM_LOC_RESP_CODE;

      case IS801_FWD_LINK_RESP_MASK_REJ:
        return TM_REJ_RESP_CODE;

      case IS801_FWD_LINK_RESP_MASK_EPH:
        return TM_GPS_EPH_RESP_CODE;

      case IS801_FWD_LINK_RESP_MASK_ALM:
        return TM_GPS_ALM_RESP_CODE;

      default:
        MSG_MED("IS801 rlink resp bit 0x%x",bmap,0,0);
        return TM_REJ_RESP_CODE;
    }
  }
}

/*===========================================================================
FUNCTION is801_util_map_rlink_bmap

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 is801_util_map_rlink_bmap(sm_event_is801_msg_e_type msg_type, uint32 bmap)
{
  if(msg_type == IS801_RESP)
  {
    switch(bmap)
    {
      case IS801_REV_LINK_RESP_MASK_MSI:
        return TM_MS_INFO_RESP_CODE; 

      case IS801_REV_LINK_RESP_MASK_PPM:
        return TM_PPM_MEAS_RESP_CODE;

      case IS801_REV_LINK_RESP_MASK_PRM:
        return TM_PR_MEAS_RESP_CODE;

      case IS801_REV_LINK_RESP_MASK_TOM:
        return TM_MSTO_MEAS_RESP_CODE;

      case IS801_REV_LINK_RESP_MASK_LR:
        return TM_LOC_RESP_CODE;

      case IS801_REV_LINK_RESP_MASK_CACK:
        return TM_CACK_RESP_CODE;

      default:
        MSG_MED("IS801 rlink resp bit 0x%x",bmap,0,0);
        return TM_REJ_RESP_CODE;
    }
  }
  else /* Req */
  {
    switch(bmap)
    {
      case IS801_REV_LINK_REQ_MASK_AA:
        return TM_GPS_AA_RESP_CODE;

      case IS801_REV_LINK_REQ_MASK_EPH:
        return TM_GPS_EPH_RESP_CODE;

      case IS801_REV_LINK_REQ_MASK_ALM:
        return TM_GPS_ALM_RESP_CODE;

      case IS801_REV_LINK_REQ_MASK_LR:
        return TM_LOC_RESP_CODE;

      default:
        MSG_MED("IS801 rlink req bit 0x%x",bmap,0,0);
        return TM_REJ_RESP_CODE;
    }
  }
}

/*===========================================================================
FUNCTION is801_util_report_is801_event_tx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_util_report_is801_event_tx(sm_event_is801_msg_e_type msg_type, uint32 mask)
{
  int i;
  uint32 is801_type=0;
  uint32 bmap;
  uint8 max_msg_type_num;
  MSG_HIGH("msg_type %d mask 0x%x",msg_type,mask,0); 

  if(msg_type == IS801_REQ)
  {
    max_msg_type_num = IS801_REV_LINK_REQ_NUM;
  }
  else
  {
    max_msg_type_num = IS801_REV_LINK_RESP_NUM;
  }
  for(i  = 0; i < max_msg_type_num; i++)
  {
    is801_type = mask & (1 << i);

    if(is801_type)
    {
      bmap = is801_util_map_rlink_bmap(msg_type,is801_type);
      sm_report_event_gps_is801_tx(msg_type,bmap);
    }
  }
}

/*===========================================================================
FUNCTION is801_util_report_is801_event_rx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_util_report_is801_event_rx(sm_event_is801_msg_e_type msg_type,uint32 bmap)
{
  uint32 mask;

  MSG_HIGH("msg_type %d bmap 0x%x",msg_type,bmap,0); 
  mask = is801_util_map_flink_bmap(msg_type, bmap);
  sm_report_event_gps_is801_rx(msg_type,mask);
}

/*===========================================================================

FUNCTION is801_util_rlink_create_msinfo

DESCRIPTION
  This function is called is to initialize the MS info buffer to be send to PDE.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None 

===========================================================================*/
boolean is801_util_rlink_create_msinfo
( 
  is801_msi_resp_type     *output_msi_buf_ptr
)
{
    if(output_msi_buf_ptr == NULL)
    {
      MSG_ERROR("Null pointer recv'd", 0, 0, 0);
      return FALSE;
    }

    /* Init MSM specific and session specific Information bits first.
    */
    output_msi_buf_ptr->gps_acq_cap  = GPS_ACQ_ASSIST | GPS_SEN_ASSIST | GPS_ALMANAC | GPS_EPHEMERIS | GPS_USER_PLANE_SESSION;   
    output_msi_buf_ptr->loc_calc_cap = 0x18;  
    
    /* Init generic Information bits.
    */
    output_msi_buf_ptr->ms_ls_rev    = NULL;             /* Standard Revision num should be set as 000000 */
    output_msi_buf_ptr->pilot_ph_cap = SIXTEENTH_CHIP_MEAS_CAP;      /* Eight Chip Measurement Capability */
    output_msi_buf_ptr->ms_mode      = NULL;              /* Digital Mode Indicator should be set to 0000 */

    return TRUE;
} /* is801_msi_resp_init() */

/*===========================================================================

FUNCTION is801_util_rlink_create_prdm_bsp

DESCRIPTION
  This function is to initialize proprietary Base Station System Parameter
  Message to be sent to PDE.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None 

===========================================================================*/
boolean is801_util_rlink_create_prdm_bsp
(
  is801_prdm_bsp_resp_type   *output_prdm_bsp_buf_ptr
)
{
  qword ts;
  mcc_pd_bs_info_type  is801_bs_info;

  if(output_prdm_bsp_buf_ptr == NULL)
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  memset(&is801_bs_info, 0, sizeof(is801_bs_info));

  if (mcc_pd_get_bs_info(&is801_bs_info)== FALSE)
  {    
    MSG_ERROR("No Base Station info provided",0,0,0);
    return FALSE;
  }

  qw_equ(ts,is801_bs_info.ts); 
  /* Macro shifts the ts to obtain the 1.25ms part and then multiplies by 5/4
  ** to get time in ms
  */
  time_to_ms(ts);

  output_prdm_bsp_buf_ptr->time_ref_cdma     = ts[0] & 0x3FFF;  /* 14 bits */       
  output_prdm_bsp_buf_ptr->still_active      = !is801_bs_info.stale_info;

  output_prdm_bsp_buf_ptr->base_id           = is801_bs_info.base_id;
  output_prdm_bsp_buf_ptr->sid               = is801_bs_info.sid;
  output_prdm_bsp_buf_ptr->nid               = is801_bs_info.nid;
  output_prdm_bsp_buf_ptr->band_class        = is801_bs_info.band_class;
  output_prdm_bsp_buf_ptr->cdma_freq         = is801_bs_info.cdma_ch;
  output_prdm_bsp_buf_ptr->pilot_pn          = is801_bs_info.pilot_pn;
  output_prdm_bsp_buf_ptr->base_lat          = is801_bs_info.base_lat;
  output_prdm_bsp_buf_ptr->base_long         = is801_bs_info.base_long;

  MSG_MED("ts: 0x%x, still: %x, band_cl: %x", output_prdm_bsp_buf_ptr->time_ref_cdma, 
            output_prdm_bsp_buf_ptr->still_active, output_prdm_bsp_buf_ptr->band_class);
  MSG_MED("ch: 0x%x, pn: %x, sid: %x", output_prdm_bsp_buf_ptr->cdma_freq, 
            output_prdm_bsp_buf_ptr->pilot_pn, output_prdm_bsp_buf_ptr->sid);
  MSG_MED("nid: 0x%x, base_id: %x", output_prdm_bsp_buf_ptr->nid, 
            output_prdm_bsp_buf_ptr->base_id, 0);
  MSG_MED("base_lat: 0x%x, base_long: %x", output_prdm_bsp_buf_ptr->base_lat, 
            output_prdm_bsp_buf_ptr->base_long, 0);

  return TRUE;
}

/*===========================================================================

FUNCTION is801_util_rlink_convert_msto

DESCRIPTION
  This function is called is to initialize the MSTO buffer to be send to PDE.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None 

===========================================================================*/
boolean is801_util_rlink_convert_msto
(
  srch_ppm_rpt_type    *input_ppm_buf_ptr,
  is801_tom_resp_type  *output_tom_buf_ptr
)
{
  int i;
  boolean found_refpn = FALSE;
  qword ts_tmp;
  uint64 mod_value;

  if(output_tom_buf_ptr == NULL || input_ppm_buf_ptr == NULL)
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }
  /* The time reported by srch is qword, convert it to 1 ms unit.
  ** Shift out 16 lower bit (we get 1.25 ms count) and then multiply by ( 5/4 )
  ** to get time in ms. IS801 will take only 24 bits out of that. */
  (void)qw_div_by_power_of_2( ts_tmp, input_ppm_buf_ptr->ts, 16 );
  qw_mul( ts_tmp, ts_tmp, (dword)5 );
  qw_inc( ts_tmp, (dword)2 );
  (void)qw_div( ts_tmp, ts_tmp, (word)4 );

  mod_value = *(uint64*)ts_tmp;
  mod_value = mod_value % TIME_REF_MS_MASK;

  output_tom_buf_ptr->time_ref_ms = mod_value;

  output_tom_buf_ptr->ref_pn = input_ppm_buf_ptr->ref_pn;

  output_tom_buf_ptr->mob_sys_t_offset = 0;
  for ( i=0; i < input_ppm_buf_ptr->num_pilots; i++ )
  {
    /* Set MSTO to be -P(RefPN) */
    if (output_tom_buf_ptr->ref_pn == input_ppm_buf_ptr->pilot_results[i].peak_pn)
    {
      output_tom_buf_ptr->mob_sys_t_offset = (int16)(-input_ppm_buf_ptr->pilot_results[i].peak_pos);
      /*lint -e{506} Constant value boolean          */
      found_refpn = TRUE;
      break;
    }
  }  

  /* Print out error message if refpn phase was not found */
  if (!found_refpn)
  {
    MSG_ERROR("RefPN %d phase not found",input_ppm_buf_ptr->ref_pn,0,0);
  }

  return TRUE;
} /* is801_UtilRLinkConvertMSTO() */


/*===========================================================================

FUNCTION is801_util_rlink_create_rej_resp

DESCRIPTION
  This function is called is to initialize the Reject Resp buffer to be send to PDE.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None 

===========================================================================*/
boolean is801_util_rlink_create_rej_resp
( 
  is801_rej_resp_type     *output_rej_buf_ptr, uint32 rej_req_type
)
{
    if(output_rej_buf_ptr == NULL)
    {
      MSG_ERROR("Null pointer recv'd", 0, 0, 0);
      return FALSE;
    }

    output_rej_buf_ptr->rej_reason = 1;
    output_rej_buf_ptr->rej_req_type = rej_req_type;
    return TRUE;
}

/*===========================================================================

FUNCTION is801_util_is_resp_within_t2m

DESCRIPTION
 Check all the is801 requests to see if there is any request
 that requires timing-constraint response.

DEPENDENCIES
  none

RETURN VALUE
  True if there is a timing-constraint response for a given request.
  Otherwise False.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean is801_util_is_resp_within_t2m
(
  byte*     payload,   /* msg buffer pointer */
  uint32    length     /* msg length */
)
{
  /* Don't change original input pointer */
  byte*   start = payload;

  int     i;
  boolean    status;
  /* Status to be returned to calling procedure */
  word    buffer_pos = 0;
  /* Position to pack bits in message */
  is801_pddm_hdr_type   pddm_rec_hdr;
  /* Holds received PDDM header info */
  paii_gen_req_hdr     pddm_req_hdr;
  /* Holds a received request header */

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if (NULL == start || length < MIN_PDDM_LENGTH)
  {
    return FALSE;
  }

  status = paix_xlate_ext_pddm_hdr(start, &pddm_rec_hdr);

  buffer_pos += PDDM_HDR_LEN;
  if (TRUE != status)
  {
    return FALSE;
  }

  if (pddm_rec_hdr.num_requests > 0)
  {
    for (i = 0; i < pddm_rec_hdr.num_requests; i++)
    {

      if(paix_xlate_ext_req_header(&(start[buffer_pos]), &pddm_req_hdr, (length - buffer_pos)))
      {
          if ((pddm_req_hdr.req_type == MS_INFO_REQ_CODE)
              || (pddm_req_hdr.req_type == TIME_OFFSET_MEAS_REQ_CODE)
              || (pddm_req_hdr.req_type == CANCEL_REQ_CODE)
             )
          {
            /* Response is within 200ms */
            MSG_HIGH("Request is time constraint!", 0, 0, 0);
            return TRUE;
          }

          buffer_pos += REQ_HDR_LEN + pddm_req_hdr.req_par_len;
      }
    }
  }

  return FALSE;
}
