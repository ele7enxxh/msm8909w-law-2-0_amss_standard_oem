/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                      T M   R O T   C L I E N T   H A N D L I N G
                           TCXO Manager Rotator Value Client Handling

GENERAL DESCRIPTION

  This module maintains the Recent Good System values and the Rotator Push
  value clients handling.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_rot_client_handling.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
08/04/15   tl       Adjust XO trim in field if not factory calibrated
06/04/15   tl       Record RGS log event time in milliseconds
03/18/15   tl       Check client RGS against absolute limits
01/30/15   tl       Remove obsolete featurization
01/22/15   tl       Add profiling to selected Tcxomgr APIs
12/17/14   tl       Fix handling of tcxomgr.uncertainty during sub & client RGS
09/25/14   sg       Remove call to CGPS API to get time
07/18/14   tl       Zero out RGS data structure before writing to EFS
02/27/14   mn       TCXOMGR should save the AS_ID in subscription RGS when
                     reading RGS from EFS.
02/13/14   tl       Accept marginal RGS at extreme temperatures
10/28/13   mn       Support for client and subscription RGS retrieval log packet.
10/22/13   mn       Fixed a bug in tcxomgr_rpush_get_mask() to come up with the
                    right bitmask based on external TCXOMGR client Id.
10/20/13   mn       Calculate RGS uncertainty based on difference from FT curve.
10/04/13   mn       Support to save and retrieve subscription and system
                    RGS to/from EFS.
09/30/13   mn       TCXOMGR should return FT estimates when no subscription RGS
                    is available.
09/12/13   mn       Initial version: New Rpush API to support multi-SIM targets.

============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "mcs_variation.h"
#include "customer.h"
#include "tcxomgr_i.h"
#include "tcxomgr_log_i.h"
#include "tcxomgr_rot_client_handling_i.h"
#include "tcxomgr_diag.h"
#include "stringl.h"
#include "mcsprofile.h"

/*============================================================================

                   Globals

============================================================================*/

static tcxomgr_rot_handling_data_type tcxomgr_rot_handling_data;
volatile boolean tcxomgr_disable_rgs_flag = FALSE;

/*============================================================================

                   Defines

============================================================================*/

/* Temp change above which RGS uncertainty is extrapolated */
#define TCXOMGR_MIN_TEMP_CHANGE_FROM_RGS  1

#define TCXOMGR_IS_AS_ID_VALID(as_id) ((as_id > TCXOMGR_AS_ID_NONE) && (as_id < TCXOMGR_AS_ID_MAX))
#define TCXOMGR_IS_ROT_CLIENT_VALID(rgs_client) (rgs_client < TCXOMGR_ROT_CLIENT_MAX)

/* Signifies the current version of the RGS EFS data */
#define TCXOMGR_EFS_RGS_DATA_VERSION      1

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_XLATE_ROT_CLIENT_TO_CLIENT_ID

DESCRIPTION
  This functions translates TCXOMGR ROT client to TCXOMGR client Id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_client_id_type tcxomgr_xlate_rot_client_to_client_id
(
  tcxomgr_rot_client_enum_type rgs_client
)
{
  tcxomgr_client_id_type client_id = TCXOMGR_CLIENT_NONE;

  switch ( rgs_client )
  {
  case TCXOMGR_ROT_CLIENT_GPS:
    client_id = TCXOMGR_CLIENT_GPS;
    break;

  case TCXOMGR_ROT_CLIENT_GSM:
    client_id = TCXOMGR_CLIENT_GSM;
    break;

  case TCXOMGR_ROT_CLIENT_WCDMA:
    client_id = TCXOMGR_CLIENT_WCDMA;
    break;

  case TCXOMGR_ROT_CLIENT_CDMA_1X:
    client_id = TCXOMGR_CLIENT_CDMA_1X;
    break;

  case TCXOMGR_ROT_CLIENT_CDMA_HDR:
    client_id = TCXOMGR_CLIENT_CDMA_HDR;
    break;

  case TCXOMGR_ROT_CLIENT_LTE:
    client_id = TCXOMGR_CLIENT_LTE;
    break;

  case TCXOMGR_ROT_CLIENT_TDSCDMA:
    client_id = TCXOMGR_CLIENT_TDSCDMA;
    break;

  case TCXOMGR_ROT_CLIENT_IFC:
    client_id = TCXOMGR_CLIENT_IFC;
    break;

  default:
    client_id = TCXOMGR_CLIENT_NONE;
    DBG_1(ERROR, "tcxomgr_xlate_rot_client_to_client_id: Invalid ROT Client %d", rgs_client);
    break;
  }

  return client_id;
}

/*============================================================================

FUNCTION TCXOMGR_XLATE_CLIENT_ID_TO_ROT_CLIENT

DESCRIPTION
  This functions translates TCXOMGR client Id to TCXOMGR ROT client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_rot_client_enum_type tcxomgr_xlate_client_id_to_rot_client
(
  tcxomgr_client_id_type client_id
)
{
  tcxomgr_rot_client_enum_type rot_client = TCXOMGR_ROT_CLIENT_MAX;

  switch ( client_id )
  {
  case TCXOMGR_CLIENT_GPS:
    rot_client = TCXOMGR_ROT_CLIENT_GPS;
    break;

  case TCXOMGR_CLIENT_GSM:
    rot_client = TCXOMGR_ROT_CLIENT_GSM;
    break;

  case TCXOMGR_CLIENT_WCDMA:
    rot_client = TCXOMGR_ROT_CLIENT_WCDMA;
    break;

  case TCXOMGR_CLIENT_CDMA_1X:
    rot_client = TCXOMGR_ROT_CLIENT_CDMA_1X;
    break;

  case TCXOMGR_CLIENT_CDMA_HDR:
    rot_client = TCXOMGR_ROT_CLIENT_CDMA_HDR;
    break;

  case TCXOMGR_CLIENT_LTE:
    rot_client = TCXOMGR_ROT_CLIENT_LTE;
    break;

  case TCXOMGR_CLIENT_TDSCDMA:
    rot_client = TCXOMGR_ROT_CLIENT_TDSCDMA;
    break;

  case TCXOMGR_CLIENT_IFC:
    rot_client = TCXOMGR_ROT_CLIENT_IFC;
    break;

  default:
    rot_client = TCXOMGR_ROT_CLIENT_MAX;
    DBG_1(ERROR, "tcxomgr_xlate_client_id_to_rot_client: Invalid Client Id %d", client_id);
    break;
  }

  return rot_client;
}

/*============================================================================

FUNCTION TCXOMGR_GET_INT_RGS_UNCERTAINITY

DESCRIPTION
   This function calculates the unertainty based on the following components:
   1. Temperature component
   2. difference from FT curve.

   The temperature uncertainity of the RGS is calculated as:
   If the temp change between last RGS and current temp is:-
   1. <= 1C, the uncertainty table is used to get uncertainty     ELSE
   2. the uncertainty is extrapolated.

DEPENDENCIES
  Input temperature is valid and RGS is valid.

RETURN VALUE
  tcxomgr_ppm_type

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_get_int_rgs_uncertainty
(
  tcxomgr_rgs_struct_type*       rgs,
  tcxomgr_temp_type              curr_temp,
  tcxomgr_uncertainty_type*      uncertainty_ppm,
  tcxomgr_ppb_uncertainty_type*  uncertainty_ppb,
  tcxomgr_rotator_type*          rgs_offset
)
{
  tcxomgr_ext_slope_uncertainty_type slope_uncertainty;
  tcxomgr_ppm_type                   temp_uncertainty = 0;
  tcxomgr_temp_type                  temp_diff;
  tcxomgr_field_cal_info            *field_cal;
  tcxomgr_ppm_type                   ft_uncertainty = 0;
  tcxomgr_ppm_type                   ft_diff = 0;

  /* Step 1: Calculate the temperature uncertainty */
  /* Check if the temperature values are valid */
  if ( ( TCXOMGR_TEMP_TO_RGS_TEMP(rgs->temp) != TCXOMGR_INVALID_TEMP ) &&
       ( curr_temp                           != TCXOMGR_INVALID_TEMP ) )
  {
    /* Calculate the absolute temperature difference from RGS report [in deg C] */
    temp_diff =
        ABS(TCXOMGR_TEMP_TO_RGS_TEMP(rgs->temp) - curr_temp);

    if ( temp_diff <= TCXOMGR_MIN_TEMP_CHANGE_FROM_RGS )
    {
      temp_uncertainty = 0;
    }
    else
    {
      slope_uncertainty = tcxomgr_get_slope_uncertainity();

      /* temp uncertainty [in ppb] =
         slope uncertainty [in ppb/C] * ABS(rgs temp - curr temp) [in degree C] */
      temp_uncertainty = slope_uncertainty.wwan_slope_uncertainty * temp_diff;
    }
  }
  else
  {
    /* Since the temperature values are not valid, assume the max uncertainty due to temperature.
       The worst case uncertainty is calculated as 0.15ppm/C over 4C. */
    temp_uncertainty = 614;
  }

  /* Step 2: Calculate the uncertainty due to the difference from the FT curve */
  if ( rgs->id == TCXOMGR_CLIENT_GPS )
  {
    field_cal = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS]);
  }
  else
  {
    field_cal = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN]);
  }

  /* Add uncertainty component due to dopler and Base station error */
  ft_uncertainty = PPM_HALF_TO_PPM(1);

  if ( ( TCXOMGR_TEMP_TO_RGS_TEMP(rgs->temp) != TCXOMGR_INVALID_TEMP ) &&
       ( ( field_cal->cal_data.ft_qual_ind == TCXOMGR_CAL_FT_QUAL_FT1 ) ||
         ( field_cal->cal_data.ft_qual_ind == TCXOMGR_CAL_FT_QUAL_FT2 ) ) )
  {
    ft_diff        = rgs->rot - tcxomgr_ft_eval_table(rgs->temp, field_cal);
    ft_uncertainty = ft_uncertainty + ABS(ft_diff);
  }

  ft_uncertainty = temp_uncertainty + ft_uncertainty;

  /* Step 3: Populate the uncerainty and offset values */
  if ( uncertainty_ppb != NULL )
  {
    *uncertainty_ppb = ft_uncertainty;
  }

  if ( uncertainty_ppm != NULL )
  {
    *uncertainty_ppm = TCXOMGR_ROUNDED_PPB_TO_PPM(ft_uncertainty);
  }

  if ( rgs_offset != NULL )
  {
    *rgs_offset = 0;

    /* Populate the offset based on the sign of the FT
       difference */
    if ( ft_diff != 0 )
    {
      *rgs_offset = ( ft_diff < 0 ) ?
            ft_uncertainty * -1 :
            ft_uncertainty;
    }
  }
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_FROM_RGS

DESCRIPTION
  This API resets the RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_from_rgs
(
  uint32 time,
  tcxomgr_temp_set_type temp_struct,
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr,
  tcxomgr_rgs_struct_type*                    rgs_ptr
)
{
  /* Return Freq/Temp information */
  tcxomgr_vco_info_type ft;

  /* Temperature variables */
  tcxomgr_temp_type temp, prev_temp;

  tcxomgr_ppm_type wwan_rot_val_est = 0;
  tcxomgr_ppm_type gps_rot_val_est  = 0;

  /*field state type*/
  tcxomgr_field_state_type  state_wwan = tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data.state;
  tcxomgr_field_state_type  state_gps  = tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data.state;

  /* Temperature correction for XO targets */
  tcxomgr_ppm_type correction = 0;

  /* Initialize all the date in ft */
  tcxomgr_set_default_rgs_info(&ft);

  if (rgs_ptr == NULL)
  {
    return ft;
  }

  /* Get the therm reading that will be used as RGS Therm in
     degree centigrade. Invalid reading should not be converted into degrees*/
  temp      = TCXOMGR_TEMP_TO_RGS_TEMP(temp_struct);
  prev_temp = TCXOMGR_TEMP_TO_RGS_TEMP(rgs_ptr->temp);

  if ( temp == TCXOMGR_INVALID_TEMP )
  {
    /* Invalid temp read, if last RGS update was less than 30 seconds return
       RGS, else return default values */
    if ( ( rgs_ptr->id != TCXOMGR_CLIENT_NONE ) &&
         ( time >= rgs_ptr->time )                  &&
         ( ( time - rgs_ptr->time ) <=
           TCXOMGR_RGS_DELTA_TIME_NO_TEMP_THRESH ) )
    {
      /* Even though we don't know the temperature of the phone, the RGS value
         is not very old.  Temperature should not have changed much, so let's
         still go ahead and use the RGS values. */
      ft.vco_type             = TCXOMGR_VCO_RGS;
      ft.rot_value            = rgs_ptr->rot;
      ft.vco_value            = rgs_ptr->vco;

      /* Update the GPS coeffecients from WWAN as we are relying on RGS */
      ft.gps_vco_type         = ft.vco_type;
      ft.gps_rot_value        = ft.rot_value;

      /* Set the temp and time diff */
      ft.temp_diff            = TCXOMGR_INVALID_TEMP;
      ft.time_diff            = time - rgs_ptr->time;

      DBG_3( ERROR, "Invalid temp ret RGS sys/time:%d/%d Cur time:%d",
             rgs_ptr->id, rgs_ptr->time, time );
    }
  }

  /* We have a temp reading */
  else if (    ( rgs_ptr->id != TCXOMGR_CLIENT_NONE )
            && ( time >= rgs_ptr->time )
            && ( ( time - rgs_ptr->time ) <= TCXOMGR_RGS_DELTA_TIME_THRESH )
            && ( ABS( temp - prev_temp ) <= TCXOMGR_RGS_DELTA_TEMP_THRESH )
          )
  {
    /* Temp and time have changed little, return RGS */
    ft.vco_type   = TCXOMGR_VCO_RGS;
    ft.rot_value  = rgs_ptr->rot;
    ft.vco_value  = rgs_ptr->vco;

    /* Get the correction for change in temparature from RGS to current */
    correction = tcxomgr_get_ft_correction
                   ( temp_struct,
                     rgs_ptr->temp );

    ft.rot_value += correction;

    /* Update the GPS coeffecients from WWAN as we are using RGS */
    ft.gps_vco_type     = ft.vco_type;
    ft.gps_rot_value    = ft.rot_value;

    /* Calculate the Freq vs. Temp PPM error value using the calibrated
         FT curve */
    wwan_rot_val_est = tcxomgr_ft_eval( temp_struct );
    gps_rot_val_est  = tcxomgr_ft_eval_gps( temp_struct );

    /* Set the temp and time diff */
    ft.temp_diff            = ABS( temp - prev_temp );
    ft.time_diff            = time - rgs_ptr->time;
  }

  if ( ft.vco_type   == TCXOMGR_VCO_RGS )
  {
    /* Set the client Id and AS Id */
    ft.id                   = rgs_ptr->id;
    ft.as_id                = rgs_ptr->as_id;

    /* Get the RGS uncertainty */
    tcxomgr_get_int_rgs_uncertainty
    (
      rgs_ptr,
      temp,
      &ft.freq_uncertainty,
      &ft.freq_uncertainty_ppb,
      &ft.rgs_offset
    );

    /* Since we are using RGS, GPS estimates will be same as WWAN estimates */
    ft.gps_freq_uncertainty     = ft.freq_uncertainty;
    ft.gps_freq_uncertainty_ppb = ft.freq_uncertainty_ppb;

    if (log_ptr != NULL)
    {
      /* Populate log data */
      log_ptr->rgs_tech             = rgs_ptr->id;
      log_ptr->temp_xo              = temp_struct.xo;
      log_ptr->temp_pmic            = temp_struct.pmic;
      log_ptr->timestamp_ms         = timetick_get_ms();
      log_ptr->correction           = correction;
      log_ptr->vco_value            = ft.vco_value;
      log_ptr->rot_value            = ft.rot_value;
      log_ptr->wwan_rot_val_est     = wwan_rot_val_est;
      log_ptr->gps_rot_val_est      = gps_rot_val_est;
      log_ptr->vco_type             = ft.vco_type;
      log_ptr->freq_uncertainty     = ft.freq_uncertainty;
      log_ptr->freq_uncertainty_ppb = ft.freq_uncertainty_ppb;
      log_ptr->wwan_data_available  = 0;
      log_ptr->state_wwan           = state_wwan;
      log_ptr->state_gps            = state_gps;
      log_ptr->as_id                = ft.as_id;
    }
  }

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_CLIENT_RGS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_info_type tcxomgr_get_client_rgs (tcxomgr_client_info_struct_type client_info)
{
  tcxomgr_vco_info_type                      ft;
  tcxomgr_crystal_type                       xtal_type = tcxomgr_get_crystal_type();
  tcxomgr_log_rgs_retrieval_data_struct_type rgs_log;
  tcxomgr_rot_client_enum_type               rgs_client;
  tcxomgr_rgs_handling_info_type*            rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;

  MCSMARKER(TCXOMGR_GET_CLIENT_RGS_I);

  /* Set the default values */
  tcxomgr_set_default_rgs_info(&ft);

  if ( xtal_type != XO_CRYSTAL )
  {
    DBG_1(ERROR,"tcxomgr_get_client_rgs: API not supported for : %d", xtal_type);
    ft = tcxomgr_get_stored_vco();
    MCSMARKER(TCXOMGR_GET_CLIENT_RGS_O);
    return ft;
  }

  /* Get the RGS client identifier */
  rgs_client = tcxomgr_xlate_client_id_to_rot_client(client_info.id);


  if ( !TCXOMGR_IS_ROT_CLIENT_VALID(rgs_client) || !TCXOMGR_IS_AS_ID_VALID(client_info.as_id) )
  {
    DBG_2( ERROR, "Invalid client information. Client %d. AS_ID %d",
            client_info.id, client_info.as_id);
    MCSMARKER(TCXOMGR_GET_CLIENT_RGS_O);
    return ft;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Try to see if we can get estimates from RGS */
  ft = tcxomgr_get_stored_vco_xo(&rgs_info->client_rgs[client_info.as_id][rgs_client], &rgs_log);

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  if ( ft.vco_type != TCXOMGR_VCO_RGS )
  {
    /* List out what we are returning */
    DBG_2(HIGH, "tcxomgr_get_client_rgs: Did not find matching RGS for Client: %d AS_ID %d",
                 client_info.id, client_info.as_id);
  }

  /* List out what we are returning */
  DBG_6(HIGH, "tcxomgr_get_client_rgs: Temp: %d Rot: %d VCO Type: %d "
              "Uncertainty: %d system: %d AS_ID: %d",
                 rgs_log.temp_xo,
                 ft.rot_value,
                 ft.vco_type, ft.freq_uncertainty,
                 ft.id, ft.as_id );

  /* Set the API type */
  rgs_log.api_type  =   TCXOMGR_RGS_RETRIEVAL_CLIENT_RGS;

  tcxomgr_log_rgs_data(TCXOMGR_RGS_RETRIEVAL_LOG_CMD, (void *)&rgs_log);

  MCSMARKER(TCXOMGR_GET_CLIENT_RGS_O);

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_SUBSCRIPTION_RGS

DESCRIPTION
  This API can be used to retrieve an AS_ID sepecific RGS from TCXOMGR

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_info_type tcxomgr_get_subscription_rgs (tcxomgr_as_id_enum_type as_id)
{
  tcxomgr_vco_info_type    ft;
  tcxomgr_crystal_type     xtal_type = tcxomgr_get_crystal_type();
  tcxomgr_log_rgs_retrieval_data_struct_type rgs_log;
  tcxomgr_rot_client_enum_type rgs_client = TCXOMGR_ROT_CLIENT_MAX;
  tcxomgr_rgs_handling_info_type*   rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;

  MCSMARKER(TCXOMGR_GET_SUB_RGS_I);

  /* Set the default values */
  tcxomgr_set_default_rgs_info(&ft);

  if ( xtal_type != XO_CRYSTAL )
  {
    DBG_1(ERROR,"tcxomgr_get_client_rgs: API not supported for : %d", xtal_type);
    ft = tcxomgr_get_stored_vco();
    MCSMARKER(TCXOMGR_GET_SUB_RGS_O);
    return ft;
  }

  if ( !TCXOMGR_IS_AS_ID_VALID(as_id) )
  {
    DBG_1( ERROR, "tcxomgr_get_subscription_rgs: Invalid AS_ID %d",
            as_id);
    MCSMARKER(TCXOMGR_GET_SUB_RGS_O);
    return ft;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Get the client Id for AS specific RGS */
  rgs_client = rgs_info->subscription_rgs[as_id];

  if ( TCXOMGR_IS_ROT_CLIENT_VALID(rgs_client) )
  {
    /* Try to see if we can get estimates from RGS */
    ft = tcxomgr_get_stored_vco_xo(&rgs_info->client_rgs[as_id][rgs_client], &rgs_log);
  }
  else
  {
    /* List out what we are returning */
    DBG_1(HIGH, "tcxomgr_get_subscription_rgs: Did not find matching RGS for AS_ID %d",
                 as_id);

    /* Try to get the estimates based on the FT curve */
    ft = tcxomgr_get_stored_vco_xo(NULL, &rgs_log);
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  /* List out what we are returning */
  DBG_6(HIGH, "tcxomgr_get_subscription_rgs: Temp: %d Rot: %d VCO Type: %d "
              "Uncertainty: %d system: %d AS_ID: %d",
                 rgs_log.temp_xo,
                 ft.rot_value,
                 ft.vco_type, ft.freq_uncertainty,
                 ft.id, ft.as_id );

  /* Set the API type */
  rgs_log.api_type  =   TCXOMGR_RGS_RETRIEVAL_SUBSCRIPTION_RGS;

  tcxomgr_log_rgs_data(TCXOMGR_RGS_RETRIEVAL_LOG_CMD, (void *)&rgs_log);

  MCSMARKER(TCXOMGR_GET_SUB_RGS_O);

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_SYSTEM_RGS

DESCRIPTION
  This function retrieves the System RGS values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_system_rgs(tcxomgr_rgs_struct_type* system_rgs)
{
  if ( system_rgs != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    memscpy(system_rgs,
            sizeof(tcxomgr_rgs_struct_type),
            &tcxomgr_rot_handling_data.rgs_handling_info.system_rgs,
            sizeof(tcxomgr_rgs_struct_type));

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
  }
}

/*============================================================================

FUNCTION TCXOMGR_GET_RGS_VALUES

DESCRIPTION
  This function retrieves the Recent Good System values.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_rgs_vals_struct_type *rgs - Return parameter with RGS data.

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_rgs_values
(
  /* Structure to store recent good system values in */
  tcxomgr_rgs_vals_struct_type *rgs
)
{
  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                 current_time;

  /* Current temperature */
  tcxomgr_temp_set_type  temp_set;

  tcxomgr_rgs_struct_type*      system_rgs = \
          &tcxomgr_rot_handling_data.rgs_handling_info.system_rgs;

/*--------------------------------------------------------------------------*/

  /* Get time & temperature */
  /* Todo: this needs to change to a blocking read based on WCDMA requirements */
  /* worst case for cached read is 110ms old stale value. */
  tcxomgr_get_time_and_temp(&current_time, &temp_set);

  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Transfer RGS values */
  rgs->system = system_rgs->id;
  rgs->vco    = system_rgs->vco;
  rgs->rot    = system_rgs->rot;

  if ( tcxomgr_diag_get_rgs_override_enable() )
  {
    rgs->age_secs = tcxomgr_diag_get_rgs_time();

    /* Get the temp difference from diag */
    rgs->temp_diff= tcxomgr_diag_get_rgs_temp_diff();
  }
  else
  {
    /* Compute temperature difference */
    rgs->temp_diff = (int16) (TCXOMGR_TEMP_TO_RGS_TEMP(temp_set) -
                     TCXOMGR_TEMP_TO_RGS_TEMP(system_rgs->temp));

    /* Compute time difference (age) of RGS values */
    if ( current_time >= system_rgs->time )
    {
      /* Capture time is in the past, we can compute the age */
      rgs->age_secs = (uint32)(current_time - system_rgs->time);
    }
    else
    {
      /* Capture time is in the future; don't trust it! */
      rgs->age_secs = TCXOMGR_INVALID_RGS_AGE;
    }
  }

  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_get_rgs_values( ) */

/*============================================================================

FUNCTION TCXOMGR_RESET_RGS_VALUE

DESCRIPTION
  This API resets the RGS value passed as argument.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_reset_rgs_value( tcxomgr_rgs_struct_type* rgs_ptr )
{
  if ( rgs_ptr != NULL )
  {
    /* Start of critical section */
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* Reset all of our RGS data. */
    rgs_ptr->id           = TCXOMGR_CLIENT_NONE;
    rgs_ptr->as_id        = TCXOMGR_AS_ID_NONE;
    rgs_ptr->vco          = tcxomgr.vco.dflt;
    rgs_ptr->rot          = 0;
    rgs_ptr->temp.pa      = TCXOMGR_INVALID_TEMP;
    rgs_ptr->temp.pmic    = TCXOMGR_INVALID_TEMP;
    rgs_ptr->temp.xo      = TCXOMGR_INVALID_TEMP;
    rgs_ptr->time         = 0;
    rgs_ptr->time_in_msec = 0;

    /* End of critical section */
    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
  }
}

/*============================================================================

FUNCTION TCXOMGR_RESET_CLIENT_RGS_VALUES_PROCESSING

DESCRIPTION
  This API resets the client RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_reset_client_rgs_values_processing( void )
{
  tcxomgr_rgs_handling_info_type*   rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;
  uint8                             i, j;

  /* Go over all tech and AS Id combinations and reset the RGS values */
  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    for ( j = 0; j < TCXOMGR_ROT_CLIENT_MAX; j++ )
    {
      tcxomgr_reset_rgs_value(&rgs_info->client_rgs[i][j]);
    }

    /* Reset the latest AS_ID specific RGS client */
    rgs_info->subscription_rgs[i] = TCXOMGR_ROT_CLIENT_MAX;
  }
}

/*===========================================================================

FUNCTION TCXOMGR_EXTRAPOLATE_RGS_VALUES

DESCRIPTION    This function processes the new temp reading and informs the
  various users of temp reads like temp cache, freqest and adc recal.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static void tcxomgr_extrapolate_rgs_values
(
  tcxomgr_rgs_struct_type* rgs_ptr,
  tcxomgr_temp_set_type   temp,
  uint32                  temp_read_time
)
{
  uint32                prev_time_xo = 0;
  tcxomgr_temp_set_type prev_temp;
  tcxomgr_temp_set_type orig_rgs_temp;
  int32                 mult;

  /* store the previous XO temp read value and time */
  prev_temp.xo = tcxomgr.cached_xo_temp.xo;
  prev_time_xo = tcxomgr.cached_xo_temp_time;

  /* Extrapolate the RGS XO temp value if required.
   * Note: Extrapolation of RGS temp value is only needed if GPS is off. When GPS is off,
   * we only perform reads on the XO coarse channel and the PMIC temp value is always set
   * to TCXOMGR_INVALID_TEMP. Hence, we do not need to extrapolate the PMIC temp value. */
  if ( ( rgs_ptr != NULL ) &&
       ( prev_time_xo != 0 ) &&
       ( ( temp_read_time - prev_time_xo ) <= TCXOMGR_CACHED_TEMP_VALUE_TIME_HIGH_THRESH ) )
  {
    /* Check if an RGS update was received between the last time the cached temp was updated and now */
    if ( rgs_ptr->time_in_msec > prev_time_xo && rgs_ptr->time_in_msec <= temp_read_time )
    {
      /* Avoid extrapolation if the temp value is invalid or if temp value is new enough. */
      if ( !TCXOMGR_TEMP_RANGE_XO_INVALID( rgs_ptr->temp ) &&
           ( ( rgs_ptr->time_in_msec - prev_time_xo ) > TCXOMGR_CACHED_TEMP_VALUE_TIME_LOW_THRESH ) )
      {
        /* Extrapolate the RGS XO temp value */
        orig_rgs_temp.xo = rgs_ptr->temp.xo;
        mult = ( (int32) rgs_ptr->time_in_msec - (int32) prev_time_xo ) *
               ( tcxomgr.cached_xo_temp.xo - prev_temp.xo );
        rgs_ptr->temp.xo =
               ( ( mult ) / ( (int32) temp_read_time - (int32) prev_time_xo )  ) + prev_temp.xo;
        DBG_2( HIGH,"RGS temp extrapolated: orig_rgs_temp = %d, new_rgs_temp = %d",
           orig_rgs_temp.xo, rgs_ptr->temp.xo );
      }
    }
  }
}

/*===========================================================================

FUNCTION TCXOMGR_EXTRAPOLATE_RGS

DESCRIPTION    Extrapolates the temperature stored in the RGS values
               if required.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_extrapolate_rgs
(
  tcxomgr_temp_set_type temp,
  uint32                read_time
)
{
  tcxomgr_rgs_handling_info_type*   rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;
  uint8                             i, j;
  tcxomgr_rgs_struct_type* system_rgs = \
          &tcxomgr_rot_handling_data.rgs_handling_info.system_rgs;

  /* Extrapolate the central RGS temp if required */
  tcxomgr_extrapolate_rgs_values(system_rgs, temp, read_time);

  /* Now extrapolate the tech specific RGS temp if required */
  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    for ( j = 0; j < TCXOMGR_ROT_CLIENT_MAX; j++ )
    {
      if ( rgs_info->client_rgs[i][j].id != TCXOMGR_CLIENT_NONE )
      {
        tcxomgr_extrapolate_rgs_values(&rgs_info->client_rgs[i][j], temp, read_time);
      }
    }
  }
}

/*============================================================================

FUNCTION TCXOMGR_RESET_CLIENT_RGS_VALUES

DESCRIPTION
  This API resets the client RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_client_rgs_values( tcxomgr_client_info_struct_type client_info )
{
  DBG_2( HIGH, "tcxomgr_reset_client_rgs_values: Client Id: %d, AS_ID: %d",
          client_info.id, client_info.as_id);

  tcxomgr_reset_client_rgs_values_processing();
}

/*============================================================================

FUNCTION TCXOMGR_UPDATE_RGS_VALUE

DESCRIPTION
  This is a utility API used to update an RGS value passed as argument.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_update_rgs_value
(
  uint32                         time_in_sec,
  tcxomgr_temp_set_type          temp,
  tcxomgr_rgs_struct_type*       rgs_ptr,
  tcxomgr_rgs_info_type*         input_rgs
)
{
  if ( rgs_ptr != NULL )
  {
    /* Store the RGS values regardless of the temperature */
    rgs_ptr->id           = input_rgs->id;
    rgs_ptr->as_id        = input_rgs->as_id;
    rgs_ptr->time         = time_in_sec;
    rgs_ptr->time_in_msec = time_in_sec * 1000;
    rgs_ptr->temp         = temp;
    rgs_ptr->vco          = input_rgs->vco_val;
    rgs_ptr->rot          = input_rgs->rot_val;
  }
}

/*============================================================================

FUNCTION TCXOMGR_UPDATE_RGS_VALUES

DESCRIPTION
  This function updates the Recent Good System values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_update_rgs_values
(
  tcxomgr_rgs_info_type *rgs
)
{
  tcxomgr_temp_set_type             temp;
  uint32                            time_in_sec = 0;
  tcxomgr_as_id_enum_type           as_id;
  tcxomgr_rgs_handling_info_type*   rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;

  tcxomgr_rotator_type              rot_drift = 0;
  tcxomgr_rotator_type              max_rot_drift;
  tcxomgr_ppm_type                  ft_uncertainty;
  tcxomgr_ppm_type                  ft_rot;
  tcxomgr_field_cal_info            *field_cal;
  tcxomgr_log_rgs_data_struct_type  rgs_log;
  tcxomgr_update_rgs_struct_type    rgs_data;

  tcxomgr_client_id_type       id;
  tcxomgr_vco_type             vco_val;
  tcxomgr_rotator_type         rot_val;
  tcxomgr_rot_client_enum_type rgs_client;

/*--------------------------------------------------------------------------*/

  /* Check if the RGS information is valid */
  if(rgs == NULL)
  {
    DBG_0( ERROR, "Update RGS: RGS pointer is invalid");
    return;
  }

  MCSMARKER(TCXOMGR_UPDATE_RGS_I);

  if(rgs->valid_fields & TCXOMGR_RGS_FIELD_ID)
  {
    id = rgs->id;
  }
  else
  {
    DBG_0( ERROR, "Update RGS: Client ID not populated");
    MCSMARKER(TCXOMGR_UPDATE_RGS_ERR);
    return;
  }

  if(rgs->valid_fields & TCXOMGR_RGS_FIELD_ROT)
  {
    rot_val = rgs->rot_val;
  }
  else
  {
    DBG_0( ERROR, "Update RGS: rotator not populated");
    MCSMARKER(TCXOMGR_UPDATE_RGS_ERR);
    return;
  }

  if(rgs->valid_fields & TCXOMGR_RGS_FIELD_VCO)
  {
    vco_val = rgs->vco_val;
  }
  else
  {
    vco_val = 0;
  }

  if(rgs->valid_fields & TCXOMGR_RGS_FIELD_AS_ID)
  {
    as_id = rgs->as_id;

    if ( !TCXOMGR_IS_AS_ID_VALID(as_id) )
    {
      DBG_2( ERROR, "Ignoring rotator from Client %d. Invalid AS_ID %d",
              id, as_id);
      MCSMARKER(TCXOMGR_UPDATE_RGS_ERR);
      return;
    }
  }
  else
  {
    as_id   = TCXOMGR_AS_ID_MAX;
  }

  /* Get the RGS client identifier */
  rgs_client = tcxomgr_xlate_client_id_to_rot_client(id);

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time_in_sec, &temp);

  /* Print the details of RGS update */
  if (temp.pmic != TCXOMGR_INVALID_TEMP)
  {
    DBG_5( HIGH, "Client %d, AS_ID: %d, update RGS: rot=%d temp_xo=%d, temp_pmic=%d",
              id, as_id, rot_val, temp.xo, temp.pmic );
  }
  else
  {
    DBG_4( HIGH, "Client %d, AS_ID: %d, update RGS: rot=%d temp_xo=%d",
              id, as_id, rot_val, temp.xo);
  }

  /* initialize the log struct */
  memset(&rgs_log, 0, sizeof(rgs_log));

  /* Store RGS values into the log struct */
  rgs_log.tech        = id;
  rgs_log.as_id       = rgs->as_id;
  rgs_log.timestamp_ms= timetick_get_ms();
  rgs_log.temp_xo     = temp.xo;
  rgs_log.vco_val     = vco_val;
  rgs_log.rot_val     = rot_val;
  rgs_log.tech_state  = rgs->state;
  if((id ==TCXOMGR_CLIENT_GPS ) &&
     (rgs->valid_fields & TCXOMGR_RGS_FIELD_TIME))
  {
    rgs_log.gps_time    = rgs->time;
  }
  else
  {
    rgs_log.gps_time    = 0;
  }

  if (tcxomgr_get_crystal_type()!= TCXO_CRYSTAL )
  {
    if((rot_val > MAX_ROT_IN_VALUE) || (rot_val < MIN_ROT_IN_VALUE))
    {
      /* Rotator from client exceeds acceptable limits in any mode. Reject it
       * outright. */

      /* Print an error message */
      DBG_3(ERROR, "Ignoring rotator from client %d. rot=%d temp_xo=%d",
                      id, rot_val, temp.xo);

      /* RGS is invalid, but we still log it */
      rgs_log.accepted = 0;
      tcxomgr_log_rgs_data(TCXOMGR_RGS_UPDATE_LOG_CMD, (void *)&rgs_log);

      MCSMARKER(TCXOMGR_UPDATE_RGS_RJCT);
      return;
    }

    if(id == TCXOMGR_CLIENT_GPS)
    {
      field_cal = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS]);
    }
    else
    {
      field_cal = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN]);
    }

    /* Get value from GPS FT curve */
    ft_rot = tcxomgr_ft_eval_table( temp, field_cal );

    /* Fill FT field in log struct */
    rgs_log.ft_est = (int16) ft_rot;

    /* Compute drift */
    rot_drift = ABS(rot_val - ft_rot);

    /* If factory calibration is missing, failed, or invalid: Use the
     * difference between RGS and the FT curve evaluated at the current
     * temperature to estimate a new trim value. */
    if((field_cal->cal_data.ft_qual_ind == TCXOMGR_FT_QUAL_FAILED) ||
       (field_cal->cal_data.ft_qual_ind == TCXOMGR_FT_QUAL_NOT_CAL))
    {
      tcxomgr_schedule_field_trim(rot_val - ft_rot);
    }

    /*
       Only in FT1 or FT2 state, RGS should be discarded if:
       1. The temperature is extreme and rotator is off by more than 10 ppm
          (or the uncertainty calculated from the FT coefficients, whichever
          is greater)
       2. The temperature is moderate (+/- 50C) and rotator is off by more
          than 7 ppm
       3. The temperature is normal (+/- 30C) and rotator is off by more than
          5 ppm
    */
    if((field_cal->cal_data.ft_qual_ind == TCXOMGR_CAL_FT_QUAL_FT1) ||
       (field_cal->cal_data.ft_qual_ind == TCXOMGR_CAL_FT_QUAL_FT2))
    {
      if((temp.xo > 81920) || (temp.xo < -20480))
      {
        max_rot_drift = PPM_FULL_TO_PPM(10);
      }
      else if((temp.xo > 61440) || (temp.xo < 0))
      {
        max_rot_drift = PPM_FULL_TO_PPM(7);
      }
      else
      {
        max_rot_drift = PPM_FULL_TO_PPM(5);
      }

      ft_uncertainty = tcxomgr_get_int_ppb_uncertainty(
          TCXOMGR_XO_TEMP_TO_DEGC(temp.xo),
          field_cal->cal_data.ft_qual_ind,
          field_cal->cal_data.state);

      max_rot_drift = MAX(max_rot_drift, ft_uncertainty);

      if (rot_drift > max_rot_drift)
      {
        /* Print an error message */
        DBG_6( ERROR,"Ignoring rotator from client %d. "
                       "rot=%d temp_xo=%d ft_curve=%d QualInd=%d "
                       "uncertainty=%d",
                        id, rot_val, temp.xo, ft_rot,
                        (int)field_cal->cal_data.ft_qual_ind, max_rot_drift);

        /* RGS is invalid, but we still log it */
        rgs_log.accepted = 0;
        tcxomgr_log_rgs_data(TCXOMGR_RGS_UPDATE_LOG_CMD, (void *)&rgs_log);

        /* Start of critical section */
        TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

        /* Ignore the rotator error for system and sub RGS but update client RGS */
        if ( TCXOMGR_IS_AS_ID_VALID(as_id) && TCXOMGR_IS_ROT_CLIENT_VALID(rgs_client) )
        {
            tcxomgr_update_rgs_value( time_in_sec, temp, &rgs_info->client_rgs[as_id][rgs_client], rgs );
        }

        /* End of critical section */
        TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
        MCSMARKER(TCXOMGR_UPDATE_RGS_RJCT);
        return;
      }
    }
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Dont update RGS if it is overridden by Diag */
  if(tcxomgr_disable_rgs_flag == FALSE)
  {
    /* Update the central RGS value */
    tcxomgr_update_rgs_value( time_in_sec, temp, &rgs_info->system_rgs, rgs );

    /* Update the tech specific RGS value */
    if ( TCXOMGR_IS_AS_ID_VALID(as_id) && TCXOMGR_IS_ROT_CLIENT_VALID(rgs_client) )
    {
      tcxomgr_update_rgs_value( time_in_sec, temp, &rgs_info->client_rgs[as_id][rgs_client], rgs );

      /* Reset the latest AS_ID specific RGS client */
      rgs_info->subscription_rgs[as_id] = rgs_client;
    }
  }

  /* Note that the RGS point was accepted and send to log */
  rgs_log.accepted = 1;
  tcxomgr_log_rgs_data(TCXOMGR_RGS_UPDATE_LOG_CMD, (void *)&rgs_log);

  if ( vco_val !=0 )
  {
    DBG_1( ERROR, "Can't update RGS. vco is not zero %d", vco_val );
  }
  else
  {
    if( tcxomgr_get_crystal_type()!= TCXO_CRYSTAL )
    {
      rgs_data.id             = id;
      rgs_data.rot            = rot_val;
      rgs_data.temp           = temp;
      rgs_data.curr_time      = timetick_get_ms();
      rgs_data.temp_read_time = tcxomgr.cached_xo_temp_time;
      tcxomgr_schedule_rgs_update( &rgs_data );
    }
    else
    {
      /* Update the temp table */
      tcxomgr_update_temp_table( rot_val, time_in_sec, TCXOMGR_TEMP_TO_RGS_TEMP(temp));
    }
  }
  /* reset uncertainty to RGS */
  tcxomgr.uncertainty = TCXOMGR_VCO_RGS;

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  MCSMARKER(TCXOMGR_UPDATE_RGS_O);

} /* tcxomgr_update_rgs_values */

/*============================================================================

FUNCTION TCXOMGR_GET_RPUSH_REQ_MODE

DESCRIPTION
 If GPS is active, RPUSH priority is high
 if IFC has registered for RPUSH and not a single c1 is calculated, RPUSH
 priority is high
 low otherwise

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RPUSH priority is high
  FALSE otherwise

SIDE EFFECTS

============================================================================*/

tcxomgr_rpush_req_mode_enum tcxomgr_get_rpush_req_mode (void)
{
  return tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode;
}

/*============================================================================

FUNCTION TCXOMGR_INVOKE_RPUSH_MODE_CALLBACKS

DESCRIPTION
 This function iterates over all clients and invokes their Rpush mode
 callbacks if they are registered.

DEPENDENCIES
  Needs to be called from within the TCXOMGR critical section.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_invoke_rpush_mode_callbacks( void )
{
  uint8                                   i, j;
  tcxomgr_rpush_mode_cb_data_type         cb_data;
  tcxomgr_rpush_mode_cb_client_info_type* cb_client_info = NULL;

  /* Populate the callback data */
  cb_data.mode = tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode;

  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    for ( j = 0; j < TCXOMGR_ROT_CLIENT_MAX; j++ )
    {
      cb_client_info = &tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode_cb_clients[i][j];

      if ( cb_client_info->func_ptr != NULL )
      {
        cb_client_info->func_ptr(cb_data);
      }
    }
  }
}

/*============================================================================

FUNCTION TCXOMGR_DETERMINE_RPUSH_MODE

DESCRIPTION
 This function determines the current Rpush mode and also triggers an
 Rpush mode callback to clients to notify them of the changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_determine_rpush_mode( void )
{
  tcxomgr_rpush_req_mode_enum             new_rpush_mode = TCXOMGR_RPUSH_MODE_OFF;
  tcxomgr_rpush_req_mode_enum             old_rpush_mode;
  uint8                                   i;
  boolean                                 old_rpush_flag, new_rpush_flag;
  tcxomgr_rpush_handling_info_type*       rpush_info = \
          &tcxomgr_rot_handling_data.rpush_handling_info;

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Back up the value of Rpush mode */
  old_rpush_mode = rpush_info->rpush_mode;
  old_rpush_flag = ( old_rpush_mode != TCXOMGR_RPUSH_MODE_OFF ) ? TRUE : FALSE;

  /* Determine the value of Rpush mode */
  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    if ( CHECK_FLAG_BIT(rpush_info->rpush_mask[i], TCXOMGR_ROT_CLIENT_GPS) )
    {
      new_rpush_mode = TCXOMGR_RPUSH_MODE_HIGH;
    }
    else if ( CHECK_FLAG_BIT(rpush_info->rpush_mask[i], TCXOMGR_ROT_CLIENT_IFC) )
    {
      new_rpush_mode = tcxomgr_ifc_is_rpush_priority_high() ?
                       TCXOMGR_RPUSH_MODE_HIGH : TCXOMGR_RPUSH_MODE_LOW;
    }
    else if ( rpush_info->rpush_mask[i] != 0 )
    {
      new_rpush_mode = TCXOMGR_RPUSH_MODE_LOW;
    }

    /* If the rpush mode was determined to be HIGH, we need not look further */
    if ( new_rpush_mode == TCXOMGR_RPUSH_MODE_HIGH )
    {
      break;
    }
  }

  /* Set the rpush mode */
  rpush_info->rpush_mode = new_rpush_mode;
  new_rpush_flag                         = \
              ( new_rpush_mode != TCXOMGR_RPUSH_MODE_OFF ) ?
              TRUE : FALSE;

  /* If there has been a change in the Rpush mode, initiate
     Rpush mode callbacks to clients */
  if ( new_rpush_mode != old_rpush_mode )
  {
    tcxomgr_invoke_rpush_mode_callbacks();

    /* Invoke the legacy rpush flag callbacks if they are
       registered */
    if ( new_rpush_flag != old_rpush_flag )
    {
      for (i = 0; i < (uint8)TCXOMGR_NUM_CLIENTS; i++)
      {
        if ( clients[i].rpush_flag_cb != NULL )
        {
          clients[i].rpush_flag_cb(new_rpush_flag);
        }
      }
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  DBG_2( HIGH, "tcxomgr_determine_rpush_mode: old_mode: %d, new_mode: %d",
          old_rpush_mode, new_rpush_mode);
}

/*============================================================================

FUNCTION TCXOMGR_REGISTER_RPUSH_CB

DESCRIPTION
 API to register for Rotator Push callbacks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_register_rpush_cb
(
  tcxomgr_rpush_cb_registration_type reg_data
)
{
  tcxomgr_rot_client_enum_type       client_id = \
          tcxomgr_xlate_client_id_to_rot_client(reg_data.client_info.id);
  tcxomgr_as_id_enum_type            as_id     = ( reg_data.client_info.as_id == TCXOMGR_AS_ID_NONE ) ?
          TCXOMGR_AS_ID_1 : reg_data.client_info.as_id;
  tcxomgr_rpush_handling_info_type*  rpush_info = \
          &tcxomgr_rot_handling_data.rpush_handling_info;

  DBG_3( HIGH, "tcxomgr_register_rpush_cb: Client Id: %d, AS_ID: %d, cb: 0x%x",
          client_id, as_id, reg_data.func_ptr);

  if ( TCXOMGR_IS_ROT_CLIENT_VALID(client_id) && TCXOMGR_IS_AS_ID_VALID(as_id) )
  {
    /* Start of critical section */
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* If the callback function is valid, rpush is considered enabled
       else disabled. */
    if (reg_data.func_ptr != NULL)
    {
      rpush_info->rpush_mask[as_id] |= ((uint32)0x1 << (uint8)client_id);
    }
    else
    {
      rpush_info->rpush_mask[as_id] &= ~((uint32)0x1 << (uint8)client_id);
    }

    rpush_info->rpush_cb_clients[as_id][client_id].func_ptr = reg_data.func_ptr;

    /* Determine the rpush mode and call the rpush mode callbacks if necessary */
    tcxomgr_determine_rpush_mode();

    /* End of critical section */
    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
  }
}

/*============================================================================

FUNCTION TCXOMGR_REGISTER_RPUSH_MODE_CB

DESCRIPTION
 API to register for Rpush mode change. TCXOMGR will call this callback
 every time there is a change in Rpush mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_register_rpush_mode_cb
(
  tcxomgr_rpush_mode_cb_registration_type reg_data
)
{
  tcxomgr_rot_client_enum_type            client_id = \
          tcxomgr_xlate_client_id_to_rot_client(reg_data.client_info.id);
  tcxomgr_as_id_enum_type                 as_id     = \
          ( reg_data.client_info.as_id == TCXOMGR_AS_ID_NONE ) ?
          TCXOMGR_AS_ID_1 : reg_data.client_info.as_id;
  tcxomgr_rpush_mode_cb_data_type         cb_data;
  tcxomgr_rpush_mode_cb_client_info_type* cb_client_info = NULL;
  tcxomgr_rpush_handling_info_type*       rpush_info = \
          &tcxomgr_rot_handling_data.rpush_handling_info;

  DBG_2( HIGH, "tcxomgr_register_rpush_mode_cb: Client Id: %d, AS_ID: %d",
          client_id, as_id);

  if ( TCXOMGR_IS_ROT_CLIENT_VALID(client_id) && TCXOMGR_IS_AS_ID_VALID(as_id) )
  {
    /* Start of critical section */
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    cb_client_info           = \
            &rpush_info->rpush_mode_cb_clients[as_id][client_id];

    /* Set the callback function pointer */
    cb_client_info->func_ptr = reg_data.func_ptr;

    /* Invoke the rpush mode callback if it is valid */
    if ( reg_data.func_ptr != NULL )
    {
      /* Populate the callback data */
      cb_data.mode = rpush_info->rpush_mode;

      cb_client_info->func_ptr(cb_data);
    }

    /* End of critical section */
    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
  }
}

/*============================================================================

FUNCTION TCXOMGR_ROTATOR_PUSH_CMD_HANDLER

DESCRIPTION
  This function handles Rpush data command for TCXOMGR task. As a part of the
  handling, this function will iterate over the clients registered for Rpush
  report callbacks and invoke their callback functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_rotator_push_cmd_handler
(
  tcxomgr_rpush_data_type* rpush_data
)
{
  uint8                                   i, j;
  tcxomgr_rpush_cb_data_type              cb_data;
  tcxomgr_rpush_cb_client_info_type*      cb_client_info = NULL;

  if ( rpush_data == NULL )
  {
    DBG_0( ERROR, "tcxomgr_rotator_push_cmd_handler: Invalid data ptr!!");
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Populate the callback data */
  cb_data.client_info.id                 = rpush_data->client_info.id;
  cb_data.client_info.as_id              = rpush_data->client_info.as_id;
  cb_data.vco_val                        = tcxomgr_get_current_vco();
  cb_data.rot_val                        = rpush_data->rot_val;
  cb_data.num_samples                    = rpush_data->num_samples;
  cb_data.begin_acq_time                 = rpush_data->begin_acq_time;
  cb_data.end_acq_time                   = rpush_data->end_acq_time;
  cb_data.state                          = rpush_data->state;

  /* get temperature in degree centigrade */
  /* Use the cached value instead of direct ADC read */
  /* this function is being called in INT context. */
  /* adc_read could take upto 4ms */
  cb_data.temp = (int32)TCXOMGR_XO_TEMP_TO_DEGC( tcxomgr.cached_xo_temp.xo );

  /* Iterate over the rpush clients and invoke the rpush callback if valid */
  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    for ( j = 0; j < TCXOMGR_ROT_CLIENT_MAX; j++ )
    {
      /* Dont send GPS rpush back to GPS */
      if (rpush_data->client_info.id == TCXOMGR_CLIENT_GPS)
      {
        if ( j == TCXOMGR_ROT_CLIENT_GPS)
        {
          continue;
        }
      }

      cb_client_info = \
              &tcxomgr_rot_handling_data.rpush_handling_info.rpush_cb_clients[i][j];

      if ( cb_client_info->func_ptr != NULL )
      {
        cb_client_info->func_ptr(cb_data);
      }
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
}

/*============================================================================

FUNCTION TCXOMGR_ROTATOR_PUSH

DESCRIPTION
 API to report Rpush to TCXOMGR. TCXOMGR will report Rpush to all clients that have
 registered for Rpush callbacks using the tcxomgr_register_rpush_cb API.

 RAT Layer1s can use any of the following mechanisms to check is Rpush is needed.
 * Calling tcxomgr_get_rpush_req_mode API
 * Register for Rpush mode change callback using tcxomgr_register_rpush_mode_info_cb()
   API.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_rotator_push
(
  tcxomgr_rpush_data_type rpush_data
)
{
  /* pointer to command buffer */
  tcxomgr_cmd_type*                       cmd_ptr;

  /* get an command buffer */
  cmd_ptr = tcxomgr_get_cmd_buf();

  if ( cmd_ptr == NULL )
  {
    DBG_0( ERROR, "tcxomgr_register_rpush_mode_cb: Tcxomgr Command queue buffer allocation failed!!");
    return;
  }

  /* Populate the contents of the command */
  cmd_ptr->cmd_id                            = TCXOMGR_CMD_RPUSH_DATA;
  memscpy(&cmd_ptr->cmd.rpush_data.data, sizeof(tcxomgr_rpush_data_type),
          &rpush_data, sizeof(tcxomgr_rpush_data_type));

  /* Send the packet on its way */
  tcxomgr_send_cmd(cmd_ptr);
}

/*============================================================================

FUNCTION TCXOMGR_RPUSH_SET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

void tcxomgr_rpush_set_flag
(
  tcxomgr_client_id_type    id,
  boolean                   rpush_flag
)
{
  tcxomgr_rot_client_enum_type       client_id = \
          tcxomgr_xlate_client_id_to_rot_client(id);
  tcxomgr_rpush_handling_info_type*  rpush_info = \
          &tcxomgr_rot_handling_data.rpush_handling_info;
  uint32                             rpush_mask     = \
          rpush_info->rpush_mask[TCXOMGR_AS_ID_1];

/*--------------------------------------------------------------------------*/

  if ( id >= TCXOMGR_NUM_CLIENTS )
  {
    DBG_0( ERROR, "Invalid Client %d" );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if (rpush_flag)
  {
    rpush_mask |= ((uint32)0x1 << (uint8)client_id);
  }
  else
  {
    rpush_mask &= ~((uint32)0x1 << (uint8)client_id);
  }

  rpush_info->rpush_mask[TCXOMGR_AS_ID_1] = rpush_mask;
  DBG_3( LOW, "Client %d, set rpush flag to %d, rpush_mask: %d",
            id, rpush_flag, rpush_mask );

  /* Try and see if rpush mode callbacks are to be invoked */
  tcxomgr_determine_rpush_mode();

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_rpush_set_flag */


/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

boolean tcxomgr_rpush_get_flag( void )
{
  boolean ret_value = FALSE;

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode != TCXOMGR_RPUSH_MODE_OFF )
  {
    ret_value = TRUE;
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return ret_value;
} /* tcxomgr_rpush_get_flag */

/*============================================================================

FUNCTION TCXOMGR_GET_RPUSH_UPDATE_INTERVAL

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  RPUSH update frequency in ms

SIDE EFFECTS

============================================================================*/

uint16 tcxomgr_get_rpush_update_interval(void)
{
  /* Return the frequency of RPUSH upadtes as 100ms */
  return (uint16)100;
} /* tcxomgr_get_rpush_update_interval */


/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_MASK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

uint32 tcxomgr_rpush_get_mask( void )
{
  uint32 ret_val = 0;
  uint32 rpush_mask;
  tcxomgr_rot_client_enum_type id;

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Get the value of the rpush mask */
  rpush_mask = tcxomgr_rot_handling_data.rpush_handling_info.rpush_mask[TCXOMGR_AS_ID_1];

  /* Check if rpush is enabled and then figure out the return value */
  if ( rpush_mask > 0 )
  {
    for ( id = 0; id < TCXOMGR_ROT_CLIENT_MAX; id++ )
    {
      /* Check if rpush has been enabled for this client and set the corresponding bit
         in the return value */
      if ( CHECK_FLAG_BIT(rpush_mask, id ) )
      {
        ret_val |= ((uint32)0x1 << (uint8)tcxomgr_xlate_rot_client_to_client_id(id));
      }
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return ret_val;
} /* tcxomgr_rpush_get_mask */


/*============================================================================

FUNCTION TCXOMGR_RPUSH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

void tcxomgr_rpush
(
  tcxomgr_client_id_type    id,
  tcxomgr_rotator_type      rot_val,
  uint32                    num_samples,
  timetick_type             begin_acq_time,
  timetick_type             end_acq_time,
  tcxomgr_client_state_type state
)
{
  int                                i;           /* loop variable */
  int32                              temp = 0;    /* temperature value */
  tcxomgr_rpush_data_type            rpush_data;

  /*--------------------------------------------------------------------------*/

  /* Range check */
  if ( state >= TCXOMGR_NUM_STATES )
  {
    DBG_2( ERROR, "Client %d, rot push state invalid:  %d",
              id, state );
    return;
  }

  MCSMARKER(TCXOMGR_RPUSH_I);

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Only send the value on if rotator push is enabled */
  if ( tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode != TCXOMGR_RPUSH_MODE_OFF )
  {
    tcxomgr_vco_type vco_val = tcxomgr_get_current_vco();

    /* get temperature in degree centigrade */
    /* Use the cached value instead of direct ADC read */
    /* this function is being called in INT context. */
    /* adc_read could take upto 4ms */
    temp = (int32)TCXOMGR_XO_TEMP_TO_DEGC( tcxomgr.cached_xo_temp.xo );

    /* Cycle through and pass the values to those with registered rpush_cb */
    for (i = 0; i < (uint8)TCXOMGR_NUM_CLIENTS; i++)
    {
      if (clients[i].rpush_cb != NULL)
      {
        /* Dont send GPS rpush back to GPS */
        if (id == TCXOMGR_CLIENT_GPS)
        {
          if ( i == TCXOMGR_CLIENT_GPS)
          {
            continue;
          }
        }
        clients[i].rpush_cb(id, vco_val, rot_val, num_samples, begin_acq_time, end_acq_time, temp, state);
      }
    }

    rpush_data.client_info.id        = id;
    rpush_data.client_info.as_id     = TCXOMGR_AS_ID_NONE;
    rpush_data.rot_val               = rot_val;
    rpush_data.num_samples           = num_samples;
    rpush_data.begin_acq_time        = begin_acq_time;
    rpush_data.end_acq_time          = end_acq_time;
    rpush_data.state                 = state;

    /* For WCDMA, 1x, HDR, LTE and TDSCDMA, the AS Id is always AS Id 1
       since they always lie on the multimode subscription */
    if ( id == TCXOMGR_CLIENT_WCDMA    ||
         id == TCXOMGR_CLIENT_CDMA_1X  ||
         id == TCXOMGR_CLIENT_CDMA_HDR ||
         id == TCXOMGR_CLIENT_LTE      ||
         id == TCXOMGR_CLIENT_TDSCDMA )
    {
      rpush_data.client_info.as_id   = TCXOMGR_AS_ID_1;
    }

    tcxomgr_rotator_push(rpush_data);
  }
  else
  {
    DBG_1( LOW, "Client %d, rot push attempted when disabled", id );
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  MCSMARKER(TCXOMGR_RPUSH_O);

} /* tcxomgr_rpush */

/*============================================================================

FUNCTION TCXOMGR_XLATE_TO_EFS_RGS_TYPE

DESCRIPTION
  This function translates internal RGS data
  to EFS RGS data

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_xlate_to_efs_rgs_type
(
  tcxomgr_efs_rgs_struct_type* output,
  tcxomgr_rgs_struct_type*     input
)
{
  if (input != NULL && output != NULL )
  {
    output->id           = input->id;
    output->rot          = input->rot;
    output->temp         = input->temp.xo;
    output->time         = input->time;
    output->time_in_msec = input->time_in_msec;
    output->vco          = input->vco;
  }
}

/*============================================================================

FUNCTION TCXOMGR_XLATE_TO_INTERNAL_RGS_TYPE

DESCRIPTION
  This function translates EFS RGS data
  to internal RGS data

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_xlate_to_internal_rgs_type
(
  tcxomgr_rgs_struct_type*         output,
  tcxomgr_efs_rgs_struct_type*     input
)
{
  if (input != NULL && output != NULL )
  {
    output->id           = input->id;
    output->rot          = input->rot;
    output->temp.xo      = input->temp;
    output->temp.pmic    = TCXOMGR_INVALID_TEMP;
    output->temp.pa      = TCXOMGR_INVALID_TEMP;
    output->time         = input->time;
    output->time_in_msec = input->time_in_msec;
    output->vco          = input->vco;
  }
}

/*============================================================================

FUNCTION TCXOMGR_EFS_RGS_DATA_WRITE

DESCRIPTION
  This function writes RGS data from EFS.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_rgs_data_write( void )
{
  tcxomgr_efs_rgs_data_type       rgs_efs_data;
  int                             val;
  tcxomgr_rgs_handling_info_type* rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;
  uint8                           i;
  tcxomgr_rot_client_enum_type    rot_client;

  /* --------------------------------------------------------------------- */

  /* Populate the EFS data before writing */
  memset(&rgs_efs_data, 0, sizeof(rgs_efs_data));
  rgs_efs_data.version       = TCXOMGR_EFS_RGS_DATA_VERSION;

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  tcxomgr_xlate_to_efs_rgs_type(&rgs_efs_data.system_rgs,
          &rgs_info->system_rgs);

  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    rot_client = rgs_info->subscription_rgs[i];

    if ( TCXOMGR_IS_ROT_CLIENT_VALID(rot_client) )
    {
      tcxomgr_xlate_to_efs_rgs_type
      (
        &rgs_efs_data.subscription_rgs[i],
        &rgs_info->client_rgs[i][rot_client]
      );
    }
    else
    {
      rgs_efs_data.subscription_rgs[i].id = TCXOMGR_CLIENT_NONE;
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  /* Read WWAN from the file */
  val = efs_put(TCXOMGR_RGS_DATA_FILE, (void *)(&rgs_efs_data),
                sizeof(rgs_efs_data),
                O_CREAT|O_AUTODIR, ALLPERMS);

  /* Check if the data was read correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_1(ERROR, "tcxomgr_efs_rgs_data_write: Error writing EFS file %d", val);
  }
}

/*============================================================================

FUNCTION TCXOMGR_EFS_RGS_DATA_READ

DESCRIPTION
  This function reads RGS data from EFS.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_efs_rgs_data_read( void )
{
  tcxomgr_efs_rgs_data_type       rgs_efs_data;
  int                             val;
  tcxomgr_rgs_handling_info_type* rgs_info = \
          &tcxomgr_rot_handling_data.rgs_handling_info;
  uint8                           i;
  tcxomgr_rot_client_enum_type    rot_client;
  tcxomgr_client_id_type          client_id;


  /* --------------------------------------------------------------------- */

  /* Read WWAN from the file */
  val = efs_get(TCXOMGR_RGS_DATA_FILE,
                (void *)(&rgs_efs_data),
                sizeof(rgs_efs_data));

  /* Check if the data was read correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_1(ERROR, "tcxomgr_efs_rgs_data_read: Error reading EFS file %d", val);
    tcxomgr_reset_all_rgs_values(TRUE);
    return;
  }

  if ( rgs_efs_data.version != TCXOMGR_EFS_RGS_DATA_VERSION )
  {
    /* Version mismatch. We will reset the RGS data and write it to the EFS file */
    DBG_2(ERROR, "tcxomgr_efs_rgs_data_read: Version mismatch. Expected %d, Found: %d",
          TCXOMGR_EFS_RGS_DATA_VERSION,
          rgs_efs_data.version);
    tcxomgr_reset_all_rgs_values(TRUE);
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* First reset all the RGS values without flushing to EFS */
  tcxomgr_reset_all_rgs_values(FALSE);

  /* Copy the system RGS data */
  tcxomgr_xlate_to_internal_rgs_type(&rgs_info->system_rgs,
          &rgs_efs_data.system_rgs);

  /* Copy the subscription RGS data */
  for ( i = 0; i < TCXOMGR_AS_ID_MAX; i++ )
  {
    /* Get the indexing information */
    client_id  = rgs_efs_data.subscription_rgs[i].id;
    rot_client = tcxomgr_xlate_client_id_to_rot_client(client_id);

    /* If the indices are valid, restore the subscription RGS */
    if ( TCXOMGR_IS_ROT_CLIENT_VALID(rot_client) )
    {
      tcxomgr_xlate_to_internal_rgs_type
      (
        &rgs_info->client_rgs[i][rot_client],
        &rgs_efs_data.subscription_rgs[i]
      );
      
      /* Save the AS_ID */
      rgs_info->client_rgs[i][rot_client].as_id = (tcxomgr_as_id_enum_type) i;

      rgs_info->subscription_rgs[i] = rot_client;
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
}

/*============================================================================

FUNCTION TCXOMGR_RESET_ALL_RGS_VALUES

DESCRIPTION
  This API resets the RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_all_rgs_values( boolean flush_to_efs )
{
  DBG_0( HIGH, "tcxomgr_reset_all_rgs_values() Resetting RGS values..." );

  /* Reset all of our RGS data. */
  tcxomgr_reset_rgs_value(&tcxomgr_rot_handling_data.rgs_handling_info.system_rgs);

  /* Reset all client RGS values */
  tcxomgr_reset_client_rgs_values_processing();

  if ( flush_to_efs != FALSE )
  {
    tcxomgr_efs_rgs_data_write();
  }
}

/*============================================================================

FUNCTION TCXOMGR_INIT_ROT_CLIENT_DATA

DESCRIPTION
  This API initializes the rotator client data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_init_rot_client_data( void )
{
  /* Initialize the RGS information */
  tcxomgr_efs_rgs_data_read();

  /* Reset the Rpush handling information */
  memset(&tcxomgr_rot_handling_data.rpush_handling_info, 0, sizeof(tcxomgr_rot_handling_data.rpush_handling_info));
  tcxomgr_rot_handling_data.rpush_handling_info.rpush_mode = TCXOMGR_RPUSH_MODE_OFF;
}
