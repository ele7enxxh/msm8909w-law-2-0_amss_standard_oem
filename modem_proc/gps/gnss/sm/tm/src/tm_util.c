/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Utils

General Description
  This file contains implementations for TM Utils
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_util.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/15    va     Time utilities branched into tm_time_util.c
07/03/15    jv     Added QZSS Support
05/21/15    nl     Refactor code
04/16/15    jv     Lowering priority of some messages from MSG_ERROR to MSG_MED
10/25/14    jv     Added Galio Support
09/24/14    rk     Added missing steering source type while injecting AcqAssist.
10/16/13    rk     Added generic Glonass AcqAsst processing functions.
12/27/12    rk     Update fix report translation functions - tm_util_pd_info_data_init & 
                   tm_util_convert_lr - to include sensor bias parameter struct.
12/04/12    rk     CR 408753 - Updated PD function to include extended SV parameters in the client notifiation.
02/16/12    gk     Altitude also reported in float now
05/18/11    rc     Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
01/03/11    gk     Eph throttling rework
01/14/10    rb     Set PDSM_PD_UTC_TIME_VALID flag when UTC time is available
12/02/09    gk     intermediate position fixes
11/11/09    gk     Set optional field masks for position
09/01/09    gk     Added a check to validate the directory for EFS operation
08/14/07    ank    Corrected units of velHor and velVert in ext_pos_data.
08/02/07    LT     Added a utility function for handling SVs non-existence case.
07/30/07    ank    Added support for ext_pos_data in Position Report.
05/25/07    lt     Made two functions accessible by external, modified tm_util_pd_info_data_init.
05/09/07    ank    Changed prototype of tm_util_pd_info_data_init
12/11/06    ank    Changes to report time_stamp_msec field.
10/25/06    ank    Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "qw.h"
#include "gps_types.h"
#include "gps_consts.h"
#include "aries_os_api.h"
#include "tm_common.h"
#include "pdapi.h"
#include "math.h"
#include "tm_prtl_iface.h"
#include "gps_fs_api.h"
#include "tm_xtra.h"
#include "tm_data.h"
#include "secapi.h"

/* (Sv line of sight vector calculation works for up to 75 km. The last
   but one entry in the table below is specified in IS-801 as unc. >
   12,288m. Our algorithm works for up to 75Km, so set that entry to
   70000.0 m.) */
const FLT f_PosUncArr[MAX_POS_UNC_IND] =
{0.5,0.75,1.0,1.5,2.0,3.0,4.0,6.0,8.0,12.0,16.0,24.0,32.0,48.0,64.0,96.0,
  128.0,192.0,256.0,384.0,512.0,768.0,1024.0,1536.0,2048.0,3072.0,4096.0,
  6144.0,8192.0,12288.0,70000.0,999999.0};


/* 
 * The struct will contain a specification for the range of valid file names
 * that are allowed to be written to the EFS. Valid file name ranges are
 * specified using the pattern below
 *
 *     <Common Alphabetical Prefix><Optional Numerical Suffix> 
 *
 * E.g. "/CGPS_PE/AlmFile00", .. "/CGPS_PE/AlmFile31"
 *
 * The minimum and the maximum value of the suffix is specified and all file 
 * names with the common alphabetical prefix followed by a suffix between the
 * min and max suffix are valid.
 *
 * In case only a single alphabetical file name is to be specified, the  
 * suffix length is set to 0.
 */

typedef struct 
{
  /* length of valid path */
  uint8    prefix_length;
  /* alphabetical prefix of path */
  char*    path_prefix;
  /* length of the numerical suffix after the path */
  uint8    suffix_length;
  /* the minimum valid suffix */
  uint8    min_suffix;
  /* the maximum valid suffix */
  uint8    max_suffix;
}cgps_valid_efs_path;


static const cgps_valid_efs_path cgps_valid_efs_paths[] =
{
  /* Valid file names: "/CGPS_PE/AlmFile00" to "/CGPS_PE/AlmFile31"                       */
  /* prefix_len,                           prefix,                 suffix :length:min:max */
  {  sizeof("/CGPS_PE/AlmFile") -1,        "/CGPS_PE/AlmFile",             2,     0,  31 },

  /* Valid file names: "/CGPS_PE/GloAlmFile00" to "/CGPS_PE/GloAlmFile23"                 */
  /*  prefix_len,                          prefix,                 suffix :length:min:max */
  {  sizeof("/CGPS_PE/GloAlmFile") -1,     "/CGPS_PE/GloAlmFile",          2,     0,  23 },

  /* Valid file name: "/CGPS_PE/SbasCannedFile"                                           */
  /*  prefix_len,                          prefix,                 suffix :length:min:max */
  {  sizeof("/CGPS_PE/SbasCannedFile") -1, "/CGPS_PE/SbasCannedFile",      0,     0,  0  },

  /* Valid file name: "/CGPS_ME/mcc_bsa.txt"                                              */
  /* prefix_len,                           prefix                  suffix :length:min:max */
  { sizeof("/CGPS_ME/mcc_bsa.txt") -1,     "/CGPS_ME/mcc_bsa.txt",         0,     0,  0  },
    
  /* Valid file name: "/SUPL/Cert0" to "/SUPL/Cert9"                                      */
  /* prefix_len                            prefix                  suffix :length:min:max */
  { sizeof("/SUPL/Cert") -1,               "/SUPL/Cert",                   1,     0,  9  },

  /* Valid file name: "/SUPL/"                                                            */
  /* prefix_len                            prefix                  suffix :length:min:max */
  { sizeof("/SUPL/") -1,                   "/SUPL/",                       0,     0,  0  },

  /* End of array */
  { (uint8)NULL,                                  NULL,                           0,     0,  0  }
    
};

/* This list is used if we allow files with a file name prefix
   Currently file in /cert/ should have a file prefix of gpsone_ for gpsOne certs */
const char* cgps_valid_efs_cert_dir_and_file_name =  "/cert/gpsone_" ;



/*===========================================================================
FUNCTION tm_util_nearest_long

DESCRIPTION
    tm_util_nearest_long() rounds the double floating point var and returns long

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
int32 tm_util_nearest_long(DBL d_X)
{
  return(d_X - floor(d_X)) < 0.5 ? (S32) floor(d_X) : (S32)ceil(d_X);
}

/*===========================================================================
FUNCTION tm_util_select_pos_unc

DESCRIPTION
    tm_util_select_pos_unc() Encodes a Standard deviation for position 
    uncertainty value into an integer based on table 3.2.4.2-10 of the 
    IS-801 spec.

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
int16 tm_util_select_pos_unc(float Std)
{
  int16 i;
  for (i = MAX_POS_UNC_IND - 2; i >= 0; i--)
    if (fabs(Std) > f_PosUncArr[i])
      return(i + 1);

  if (fabs(Std) <= f_PosUncArr[0])
    return 0;

  /* This should never happen. Include to avoid compiler warning of
     implicit return. */
  return 31;
}

/*===========================================================================

FUNCTION TM_UTIL_XLATE_NAV_SOLN_TO_TM_POS_INFO

DESCRIPTION
  This function is used to translate sm_GnssNavSolution strucutre to tm_ext_pos_data_s_type
  structure

RETURN VALUE

DEPENDENCIES

===========================================================================*/

void tm_util_xlate_nav_soln_to_tm_pos_info
(
  pdsm_pd_dload_data_s_type   *dload_info,
  tm_pd_position_info_s_type  *pos_info,
  const sm_GnssNavPosition    *nav_ptr,
  pdsm_pd_dload_e_type        dload_type,
  pdsm_pd_dload_source_e_type dload_src
)
{
  if(dload_info != NULL)
  {
    dload_info->num_data = 1;
    dload_info->dload_info[0].data   = dload_type;
    dload_info->dload_info[0].source = dload_src;
  }

  if((nav_ptr != NULL) && (pos_info != NULL))
  {
  pos_info->positioning_source = 0;
    pos_info->positioning_source |= (nav_ptr->z_PosFlags.b_IsGpsUsed) ?
                                     PDSM_PD_POSITION_SOURCE_GPS : 0;

    pos_info->positioning_source |= (nav_ptr->z_PosFlags.b_IsGloUsed) ?
                                     PDSM_PD_POSITION_SOURCE_GLO : 0;

    pos_info->positioning_source |= (nav_ptr->z_PosFlags.b_IsBdsUsed) ?
                                     PDSM_PD_POSITION_SOURCE_BDS : 0;

    pos_info->positioning_source |= (nav_ptr->z_SmPosFlags.b_IsWifi)      ?
                                     PDSM_PD_POSITION_SOURCE_WIFI    : 0;

    pos_info->positioning_source |= (nav_ptr->z_SmPosFlags.b_IsCellId)    ?
                                     PDSM_PD_POSITION_SOURCE_CELLID  : 0;

    pos_info->positioning_source |= (nav_ptr->z_SmPosFlags.b_IsSensors)   ?
                                     PDSM_PD_POSITION_SOURCE_SENSORS : 0;

    pos_info->positioning_source |= (nav_ptr->z_SmPosFlags.b_IsRefLoc)    ?
                                     PDSM_PD_POSITION_SOURCE_DEFAULT : 0;

    pos_info->positioning_source |= (nav_ptr->z_SmPosFlags.b_IsCoarsePos) ?
                                     PDSM_PD_POSITION_SOURCE_EPI     : 0;

    pos_info->position_type = PDSM_PD_POSITION_TYPE_NEW;

    /* Set gpsUtcOffset in milliseconds */
    if(nav_ptr->q_GpsTimeMs >= nav_ptr->q_UtcTimeMs)
    {
      pos_info->gps_utc_offset = nav_ptr->q_GpsTimeMs - nav_ptr->q_UtcTimeMs;
    }
    else
    {
      pos_info->gps_utc_offset = nav_ptr->q_GpsTimeMs + MS_PER_WEEK - nav_ptr->q_UtcTimeMs; 
    }
  } /*  nav_ptr != NULL */
}

/*===========================================================================

FUNCTION TM_UTIL_XLATE_NAV_SOLN_TO_TM_EXT_POS

DESCRIPTION
  This function is used to translate sm_GnssNavSolution structure to tm_ext_pos_data_s_type
  structure

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_util_xlate_nav_soln_to_tm_ext_pos(
 tm_ext_pos_data_s_type        *ext_pos_data_ptr,
 const sm_GnssNavPosition         *nav_ptr)
{
  if(ext_pos_data_ptr == NULL || nav_ptr == NULL)
  {
    MSG_ERROR("null ptr passed %x %x ", ext_pos_data_ptr, nav_ptr, 0);
    return;
  }

  ext_pos_data_ptr->ext_lat             = nav_ptr->d_PosLla[0];
  ext_pos_data_ptr->ext_lon             = nav_ptr->d_PosLla[1];
  ext_pos_data_ptr->ext_loc_uncrtnty_a  = nav_ptr->f_ErrorEllipse[1];
  ext_pos_data_ptr->ext_loc_uncrtnty_p  = nav_ptr->f_ErrorEllipse[2];
  ext_pos_data_ptr->ext_vel_horiz       = sqrt( SQR(nav_ptr->f_VelEnu[0]) + SQR(nav_ptr->f_VelEnu[1]) );
  ext_pos_data_ptr->ext_vel_vert        = nav_ptr->f_VelEnu[2];

  /* calculate horiz and vert velocity unc, in m/s */
  ext_pos_data_ptr->ext_vel_horiz_unc   = tm_util_calc_vel_horiz_uncertainty(nav_ptr);
  ext_pos_data_ptr->ext_vel_vert_unc    = tm_util_calc_vel_vert_uncertainty(nav_ptr);
}

/*===========================================================================

FUNCTION TM_UTIL_PD_INFO_DATA_INIT

DESCRIPTION
  This function is used to initialize the PD related data for sending
  it to clients.

RETURN VALUE
  Events to be set for Client's.

DEPENDENCIES

===========================================================================*/
pdsm_pd_event_type tm_util_pd_info_data_init
(
 pdsm_pd_info_s_type           *cl_info, 
 const tm_lr_resp_type         *lr_ptr, 
 pdsm_client_id_type           client_id,
 pdsm_pd_dload_data_s_type     *pd_dload_info_ptr,
 tm_pd_position_info_s_type    *pd_pos_info_ptr,
 tm_ext_pos_data_s_type        *ext_pos_data_ptr
)
{
  qword    qwtime, milliseconds;
  uint32   orig_time;
  me_Clock z_Clock;
  pdsm_xtra_download_info_s_type p_download_info ;
  boolean xtra_valid = TRUE;
  
  /* Since Postion is used as Default, we might be able to
  ** do away with this.
  */
  pdsm_pd_event_type   pd_event = PDSM_PD_EVENT_POSITION;

  if (cl_info == NULL || pd_pos_info_ptr == NULL || ext_pos_data_ptr == NULL)
  {
    MSG_ERROR("Null info pointer",0,0,0);
    return PDSM_PD_EVENT_END;
  }
  /* Use the time in packet, no need to do time_get()
  */

  MSG_MED("Formatting Position Data for TM-Core",0,0,0);

  qwtime[0] = lr_ptr->timestamp[0];
  qwtime[1] = lr_ptr->timestamp[1];

  (void) qw_div_by_power_of_2(qwtime, qwtime, 16 ); /* obtain time in 1.25 ms */

  /* obtain time in second's */
  (void) qw_div( qwtime, qwtime, 800 );

  /* Save the current time, so we can check for underflow/overflow */
  orig_time = qwtime[0];

  /* Now clear the lower 14 bits of time stamp */
  qwtime[0] &= 0xffffc000;

  /* Now copy the time reference provided from PDE.
  */
  qwtime[0] |= ( lr_ptr->time_ref_cdma & 0x3fff );

  /* Check for Overflow/Underflow
  */
  if ( (int32)(qwtime[0] - orig_time) > 0x2000 )
  {
    qwtime[0] -= 0x4000;
  }
  else if ( (int32)(orig_time - qwtime[0]) > 0x2000 )
  {
    qwtime[0] += 0x4000;
  }

  cl_info->client_id = client_id;
  cl_info->pd_info.pd_data.meas_data              = NULL;
  cl_info->pd_info.pd_data.pm_data                = NULL;
  cl_info->pd_info.pd_data.lat                    = lr_ptr->latitude;
  cl_info->pd_info.pd_data.lon                    = lr_ptr->longitude;
  cl_info->pd_info.pd_data.loc_uncertainty_a      = lr_ptr->loc_uncrtnty_a;
  cl_info->pd_info.pd_data.loc_uncertainty_ang    = lr_ptr->loc_uncrtnty_ang;
  cl_info->pd_info.pd_data.loc_uncertainty_p      = lr_ptr->loc_uncrtnty_p;

  /* Init the time for user.
  */
  cl_info->pd_info.pd_data.time_stamp = qwtime[0];
  cl_info->pd_info.pd_data.opt_field_mask = PDSM_PD_UNCERTAINTY_VALID;
  cl_info->pd_info.pd_data.ext_opt_field_mask = PDSM_PD_LAT_LONG_VALID;
  cl_info->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_GPS_TIME_VALID;
  cl_info->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;

  /* Copy msec field received from PE
  */
  if ( lr_ptr->msec_incl )
  {
    cl_info->pd_info.pd_data.opt_field_mask |= PDSM_PD_MSEC_VALID;
    cl_info->pd_info.pd_data.time_stamp_msec = lr_ptr->timestamp_msec;
  }

  /* Ext Pos Data */
  cl_info->pd_info.pd_data.opt_field_mask |= PDSM_PD_EXT_STATUS_VALID;
  cl_info->pd_info.pd_data.ext_pos_data.lat = (int32)(ext_pos_data_ptr->ext_lat * C_2_32_OVER_PI);
  cl_info->pd_info.pd_data.ext_pos_data.lon = (int32)(ext_pos_data_ptr->ext_lon * C_2_31_OVER_PI);
  if (ext_pos_data_ptr->ext_loc_uncrtnty_a > CGPS_WHOLE_EARTH_UNC)
  {
    ext_pos_data_ptr->ext_loc_uncrtnty_a = CGPS_WHOLE_EARTH_UNC;
    MSG_ERROR("Error Ellipse along greater than earth radius",0,0,0);
  }
  if (ext_pos_data_ptr->ext_loc_uncrtnty_p > CGPS_WHOLE_EARTH_UNC)
  {
    ext_pos_data_ptr->ext_loc_uncrtnty_p = CGPS_WHOLE_EARTH_UNC;
    MSG_ERROR("Error Ellipse along greater than earth radius",0,0,0);
  }
  cl_info->pd_info.pd_data.ext_pos_data.loc_unc_horizontal = 
      (uint32)sqrt(( ext_pos_data_ptr->ext_loc_uncrtnty_a * 
                     ext_pos_data_ptr->ext_loc_uncrtnty_a) + 
                    (ext_pos_data_ptr->ext_loc_uncrtnty_p * 
                     ext_pos_data_ptr->ext_loc_uncrtnty_p));
  cl_info->pd_info.pd_data.ext_pos_data.alongAxisUnc = CGPS_MAX_UINT16;
  if (ext_pos_data_ptr->ext_loc_uncrtnty_a <  CGPS_MAX_UINT16)
  {
    cl_info->pd_info.pd_data.ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a;
  }
  cl_info->pd_info.pd_data.ext_pos_data.perpAxisUnc = CGPS_MAX_UINT16;
  if (ext_pos_data_ptr->ext_loc_uncrtnty_p <  CGPS_MAX_UINT16)
  {
    cl_info->pd_info.pd_data.ext_pos_data.perpAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_p;
  }
  if (cl_info->pd_info.pd_data.ext_pos_data.perpAxisUnc == CGPS_MAX_UINT16 ||
     cl_info->pd_info.pd_data.ext_pos_data.alongAxisUnc == CGPS_MAX_UINT16)
  {
    cl_info->pd_info.pd_data.opt_field_mask &= ~PDSM_PD_UNCERTAINTY_VALID;
  }

  /* Check if Velocity info is available.
  */
  // always populate velocity fields even if it's invalid 
  cl_info->pd_info.pd_data.velocity_hor       = lr_ptr->velocity_hor;
  cl_info->pd_info.pd_data.heading            = lr_ptr->heading;
  /* Ext Pos Data */    
  if(ext_pos_data_ptr->ext_vel_horiz  < (float)(TM_CORE_MIN_VELOCTY) )
  {
    cl_info->pd_info.pd_data.ext_pos_data.velHor = 0.0;
  }
  else
  {
    cl_info->pd_info.pd_data.ext_pos_data.velHor = 
      (int16) (ext_pos_data_ptr->ext_vel_horiz * 100);
  }

  if (ext_pos_data_ptr->ext_vel_horiz_unc * 100 > (float)CGPS_MAX_UINT16)
  {
    cl_info->pd_info.pd_data.ext_pos_data.velUncHoriz = CGPS_MAX_UINT16;
  }
  else
  {
    cl_info->pd_info.pd_data.ext_pos_data.velUncHoriz = 
      (uint16) (ext_pos_data_ptr->ext_vel_horiz_unc * 100);
  }

  cl_info->pd_info.pd_data.velocity_ver = lr_ptr->velocity_ver;
  cl_info->pd_info.pd_data.ext_pos_data.velVert = 
    (int16) (ext_pos_data_ptr->ext_vel_vert * 100);

  if (ext_pos_data_ptr->ext_vel_vert_unc * 100 > (float)CGPS_MAX_UINT16)
  {
    cl_info->pd_info.pd_data.ext_pos_data.velUncVert = CGPS_MAX_UINT16;
  }
  else
  {
    cl_info->pd_info.pd_data.ext_pos_data.velUncVert = 
      (uint16) (ext_pos_data_ptr->ext_vel_vert_unc * 100);
  }

  if ( lr_ptr->velocity_incl )
  {
    cl_info->pd_info.pd_data.opt_field_mask     |= PDSM_PD_VELOCITY_VALID;
    pd_event |= PDSM_PD_EVENT_VELOCITY;

    if(lr_ptr->heading_valid == TRUE)
    {
      cl_info->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_HEADING_VALID;
    }

    /* Overloaded fix type for VX spec's. 
    */
    if (lr_ptr->fix_type)
    {
      cl_info->pd_info.pd_data.fix_type = TRUE;
    }
    else
    {
      cl_info->pd_info.pd_data.fix_type = FALSE;
    }
  } /* If velocity included is TRUE */

  /* Check if Height info is available.
  */
  if ( lr_ptr->height_incl )
  {
    cl_info->pd_info.pd_data.opt_field_mask     |= PDSM_PD_ALTITUDE_VALID;
    cl_info->pd_info.pd_data.altitude = lr_ptr->height; 
    cl_info->pd_info.pd_data.ext_pos_data.f_altitude = lr_ptr->f_altitude; 
    cl_info->pd_info.pd_data.loc_uncertainty_v  = lr_ptr->loc_uncrtnty_v;
    pd_event                                    |= PDSM_PD_EVENT_HEIGHT;
  }
  else
  {
    cl_info->pd_info.pd_data.ext_pos_data.f_altitude = 0.0; 
    cl_info->pd_info.pd_data.altitude = 0; 
    cl_info->pd_info.pd_data.loc_uncertainty_v  = 0;
  }

  /* Initialize pos info */
  cl_info->pd_info.pd_data.position_mode = pd_pos_info_ptr->position_mode;
  cl_info->pd_info.pd_data.position_type = pd_pos_info_ptr->position_type;
  cl_info->pd_info.pd_data.positioning_source = pd_pos_info_ptr->positioning_source;

  cl_info->pd_info.pd_data.loc_uncertainty_conf = LOC_UNC_CONFIDENCE;
  cl_info->pd_info.pd_data.opt_field_mask      |= PDSM_PD_UTCOFFSET_VALID;
  cl_info->pd_info.pd_data.gpsUtcOffset         = (int8) (pd_pos_info_ptr->gps_utc_offset/1000);

  /* Ext pos data*/
  gps_total_msec( lr_ptr->timestamp, milliseconds );

  gps_week_ms( milliseconds, &(z_Clock.z_Time.w_GpsWeek),
               &(z_Clock.z_Time.q_GpsMsec) );
  /* The above gps time conversion routines may cause some rounding errors
   * that cause the millisecond value not to match what had originally been
   * sent in the PE structure.  The following code checks the value and makes
   * an adjustment if needed
   */
  if (z_Clock.z_Time.q_GpsMsec % 1000 != lr_ptr->timestamp_msec)
  {
      uint32 lTempMS = z_Clock.z_Time.q_GpsMsec / 1000;
      lTempMS *= 1000;
      z_Clock.z_Time.q_GpsMsec = lTempMS + lr_ptr->timestamp_msec;
  }
  /* Check for week rollover */
  gps_MsecWeekLimit((int32 *) &z_Clock.z_Time.q_GpsMsec, &z_Clock.z_Time.w_GpsWeek );

  cl_info->pd_info.pd_data.ext_pos_data.timestamp_tow_ms        = z_Clock.z_Time.q_GpsMsec;
  cl_info->pd_info.pd_data.ext_pos_data.timestamp_gps_week      = z_Clock.z_Time.w_GpsWeek;
  cl_info->pd_info.pd_data.ext_pos_data.pos_reported_to_network = FALSE;
  cl_info->pd_info.pd_data.ext_pos_data.h_dop = (uint32)(ext_pos_data_ptr->h_dop * 10.0 + 0.5);
  cl_info->pd_info.pd_data.ext_pos_data.v_dop = (uint32)(ext_pos_data_ptr->v_dop * 10.0 + 0.5);
  cl_info->pd_info.pd_data.ext_pos_data.p_dop = (uint32)(ext_pos_data_ptr->p_dop * 10.0 + 0.5);
  cl_info->pd_info.pd_data.ext_pos_data.utc_time = ext_pos_data_ptr->utc_time;    

  // safety check for final report's UTC time, "date" in mmddyyyy, 0 is not valid
  if (ext_pos_data_ptr->utc_time.date == 0)
  {
    cl_info->pd_info.pd_data.ext_opt_field_mask &= ~(PDSM_PD_UTC_TIME_VALID);
  }

  cl_info->pd_info.pd_data.ext_pos_data.time_src                     =  lr_ptr->time_src;

  if(lr_ptr->time_src == (PDSM_GNSS_TIME_GPS|PDSM_GNSS_TIME_GLO))
  {
    cl_info->pd_info.pd_data.ext_pos_data.glo_offset_valid  = TRUE;
    /* grab the whole ms offset */
    cl_info->pd_info.pd_data.ext_pos_data.glo_offset        = (uint32) lr_ptr->f_GGTBMs;
  }else
  {
    cl_info->pd_info.pd_data.ext_pos_data.glo_offset_valid  = FALSE;
  }
  
  xtra_valid = tm_xtra_set_xtra_time_and_validity(&p_download_info);
  /* Init the GPS XTRA status for user.
  */
  if (xtra_valid)
  {
    cl_info->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_XTRA_DATA_VALID;
    cl_info->pd_info.pd_data.ext_pos_data.xtra_start_gps_minutes = p_download_info.startGpsMinutes;
    cl_info->pd_info.pd_data.ext_pos_data.xtra_start_gps_week = p_download_info.startGpsWeek;
    cl_info->pd_info.pd_data.ext_pos_data.xtra_valid_duration_hours = p_download_info.validDurationHours;
  }             
  else
  {
    
    cl_info->pd_info.pd_data.ext_pos_data.xtra_start_gps_minutes = 0;
    cl_info->pd_info.pd_data.ext_pos_data.xtra_start_gps_week = 0;
    cl_info->pd_info.pd_data.ext_pos_data.xtra_valid_duration_hours = 0;
  }
  /* Init Dload info.
  */
  if ( pd_dload_info_ptr != NULL )
  {
    memscpy((void *)&cl_info->pd_info.dload_info, sizeof(cl_info->pd_info.dload_info),
           (const void *)pd_dload_info_ptr,sizeof(*pd_dload_info_ptr));
  }

  /* extended time source  & time uncertainty in float */
  cl_info->pd_info.pd_data.ext_pos_data.extended_time_source = (pdsm_extended_time_src_e_type)lr_ptr->u_TimeSource;
  cl_info->pd_info.pd_data.ext_pos_data.gps_time_unc_ms = lr_ptr->f_clock_bias_unc;

  /* heading & heading uncertainty in radians (float) */
  cl_info->pd_info.pd_data.ext_pos_data.heading_rad = lr_ptr->f_HeadingRad;
  cl_info->pd_info.pd_data.ext_pos_data.heading_unc_rad = lr_ptr->f_HeadingUncRad;
  cl_info->pd_info.pd_data.ext_pos_data.gnss_heading_rad = lr_ptr->f_GnssHeadingRad;
  cl_info->pd_info.pd_data.ext_pos_data.gnss_heading_unc_rad = lr_ptr->f_GnssHeadingUncRad;

  if (lr_ptr->time_src == PDSM_GNSS_TIME_GLO)
  {
    // If time is from GLO, mark all GPS-time related fields as invalid for final report
    cl_info->pd_info.pd_data.opt_field_mask &= ~(PDSM_PD_UTCOFFSET_VALID | PDSM_PD_MSEC_VALID);
    cl_info->pd_info.pd_data.ext_opt_field_mask &= ~(PDSM_PD_GPS_TIME_VALID);
  }

  cl_info->pd_info.pd_data.e_HoriRelIndicator = lr_ptr->e_HoriRelIndicator;
  cl_info->pd_info.pd_data.e_VertRelIndicator = lr_ptr->e_VertRelIndicator;
  cl_info->pd_info.pd_data.w_SensorAidingIndicator =
    lr_ptr->w_SensorAidingIndicator;
  cl_info->pd_info.pd_data.w_SensorDataUsageMask =
    lr_ptr->w_SensorDataUsageMask;
  
  cl_info->pd_info.pd_data.z_NavSvInfo = lr_ptr->z_NavSvInfo;

  MSG_MED("tm_util_pd_info_data_init: nv_use_rpt_valid=%d", cl_info->pd_info.pd_data.z_NavSvInfo.u_SvInfoValid, 0, 0);
  MSG_MED("GpsSvMask used=0x%x usable=0x%x",
          cl_info->pd_info.pd_data.z_NavSvInfo.q_GpsSvMaskUsed,
          cl_info->pd_info.pd_data.z_NavSvInfo.q_GpsSvMaskUsable, 0);
  MSG_MED("GloSvMask used=0x%x usable=0x%x",
          cl_info->pd_info.pd_data.z_NavSvInfo.q_GloSvMaskUsed,
          cl_info->pd_info.pd_data.z_NavSvInfo.q_GloSvMaskUsable, 0);
  MSG_MED("BdsSvMask used=0x%08x%08x",
          (uint32)((uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_BdsSvMaskUsed >> 32) & 0x00000000FFFFFFFF),
          (uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_BdsSvMaskUsable & 0x00000000FFFFFFFF), 0);
  MSG_MED("BdsSvMask usable=0x%08x%08x",
          (uint32)((uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_BdsSvMaskUsed >> 32) & 0x00000000FFFFFFFF),
          (uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_BdsSvMaskUsable & 0x00000000FFFFFFFF), 0);
  MSG_MED("GalSvMask used=0x%08x%08x",
          (uint32)((uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_GalSvMaskUsed >> 32) & 0x00000000FFFFFFFF),
          (uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_GalSvMaskUsable & 0x00000000FFFFFFFF), 0);
  MSG_MED("GalSvMask usable=0x%08x%08x",
          (uint32)((uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_GalSvMaskUsed >> 32) & 0x00000000FFFFFFFF),
          (uint32)(cl_info->pd_info.pd_data.z_NavSvInfo.t_GalSvMaskUsable & 0x00000000FFFFFFFF), 0);

  /* Sensor bias parameter struct */
  cl_info->pd_info.pd_data.z_OnDeviceSensorParam = lr_ptr->z_OnDeviceSensorParam;

  /* return the event type to be set for user. */
  return pd_event;
}

/*===========================================================================
FUNCTION tm_util_calculate_heading

DESCRIPTION

  This function is used to calculate the heading value from the PE position report

DEPENDENCIES 
  None.

PARAMETERS:
  sm_GpsFixRptStructType  *fix_ptr,
  

RETURN VALUE
  Heading 
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_util_calculate_heading
( const sm_GnssFixRptStructType *fix_ptr,
  DBL* heading_angle
)
{

  DBL  d_Azimuth;

  if(fix_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
  {
    d_Azimuth = fix_ptr->z_NavPos.f_HeadingRad * RAD2DEG;
  }
  else
  {
    d_Azimuth = 0;
  }

  *heading_angle = d_Azimuth ;        /* [x10 deg] */
}

/*============================================================================
FUNCTION tm_util_convert_lr

DESCRIPTION
    Function to convert navsolution into IS801 Location response structure

RETURN VALUE
    None

DEPENDENCIES

LIMITATIONS

=============================================================================*/
boolean tm_util_convert_lr
  (
  sm_GnssFixRptStructType  *input_lr_buf_ptr,
  tm_lr_resp_type         *output_lr_buf_ptr
  )
{
  DBL                     heading_angle;
  DBL                     vel_east;
  DBL                     vel_north;
  qword                   fix_time;
  gps_TimeStructType      z_Clock;   
  DBL                     f_tmp;

  if ((input_lr_buf_ptr == NULL) ||(output_lr_buf_ptr == NULL))
  {
    MSG_ERROR("Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  z_Clock.w_GpsWeek = input_lr_buf_ptr->z_NavPos.w_GpsWeek;
  z_Clock.q_GpsMsec = input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;
  gps_to_cdma_time(&z_Clock, output_lr_buf_ptr->timestamp);

  if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed && 
     input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
  {
    /* Since both constellations were used, GPS to Glonass Bias is valid */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GPS|PDSM_GNSS_TIME_GLO;
    /* convert bias from meters to ms by dividing it with LIGHT_MSEC (distance light travels in a msec) */
    output_lr_buf_ptr->f_GGTBMs    = (FLT) (input_lr_buf_ptr->z_NavPos.z_InterGnssTb[GNSS_TB_GPSGLO].f_FiltMeters    * ONE_OVER_LIGHT_MSEC);
    output_lr_buf_ptr->f_GGTBUncMs = (FLT) (input_lr_buf_ptr->z_NavPos.z_InterGnssTb[GNSS_TB_GPSGLO].f_FiltUncMeters * ONE_OVER_LIGHT_MSEC);
  }
  else if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
  {
    /* Only GPS constellation was used in fix, impossible that we know GPS to Glonass time bias */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GPS; 
  }  
  else if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
  {
    /* Only Glo constellation was used in fix, impossible that we know Glo to Glonass time bias */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GLO; 
  }  
  else if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsBdsUsed)
  {
    /* Only BDS constellation was used in fix */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_BDS;   // TODO FIXME
  }
  else if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGalUsed)
  {
    /* Only GAL constellation was used in fix */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GAL;   
  }  
  
  /* Update the local copy of qRefFCount with the fix time also because
   * it will be used to generate the time_ref_cdma.
  */
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

  /* Copy the entire SVInfo structure */
  output_lr_buf_ptr->z_NavSvInfo = input_lr_buf_ptr->z_NavSvInfo;

  MSG_MED("tm_util_convert_lr: nv_use_rpt_valid=%d", output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid, 0, 0);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
    "GpsSvMask used=0x%08x usable=0x%08x",
    output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed,
    output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
    "GloSvMask used=0x%08x usable=0x%08x",
    output_lr_buf_ptr->z_NavSvInfo.q_GloSvMaskUsed,
    output_lr_buf_ptr->z_NavSvInfo.q_GloSvMaskUsable);

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
    "BdsSvMask used=0x%08x%08x usable=0x%08x%08x",
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsed >> 32),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsed),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsable >> 32),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsable));

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
    "GalSvMask used=0x%08x%08x usable=0x%08x%08x",
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsed >> 32),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsed),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsable >> 32),
    (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsable));

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
    "QzssSvMask used=0x%02x usable=0x%02x",
    output_lr_buf_ptr->z_NavSvInfo.u_QzssSvMaskUsed,
    output_lr_buf_ptr->z_NavSvInfo.u_QzssSvMaskUsable);

  MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
    "Final fix Used SVs (GPS=%u, GLO=%u, BDS=%u, GAL=%u, QZSS=%u)",
    output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix,
    output_lr_buf_ptr->z_NavSvInfo.u_GloNumSvPosFix,
    output_lr_buf_ptr->z_NavSvInfo.u_BdsNumSvPosFix,
    output_lr_buf_ptr->z_NavSvInfo.u_GalNumSvPosFix,
    output_lr_buf_ptr->z_NavSvInfo.u_QzssNumSvPosFix);
  
  /* Sensor (gyro/accel) bias parameter structure */
  memset( &output_lr_buf_ptr->z_OnDeviceSensorParam, 0, sizeof( output_lr_buf_ptr->z_OnDeviceSensorParam ) );
  output_lr_buf_ptr->z_OnDeviceSensorParam.w_SeqenceNumber = input_lr_buf_ptr->z_OnDeviceSensorParam.w_SeqenceNumber;

  if ( input_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask != 0 )
  {
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask = input_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue;
  }

  if ( input_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask != 0 )
  {
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask = input_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue;
    output_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue = input_lr_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION tm_util_post_non_exist_svs

DESCRIPTION

  This function is used by all protocol submodules when the submodule is 
  convinced it has received all the assistance data it can get in the 
  current session, to:

  (1) Determine if it is in a position to deduce which SVs do not exist;
  (2) If the answer to (1) is positive, post such info to TM core; if the
      answer to (1) is negative, no further action will be taken.

DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_alm_list:      bit map of alm. delivered; SV1 on Bit 0.
  q_eph_list:      bit map of eph. delivered; SV1 on Bit 0

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_util_post_non_exist_svs
  ( tm_prtl_type             q_prtl_type, 
    tm_sess_handle_type      session_handle,
    uint32                   q_alm_list, 
    uint32                   q_eph_list  )
{
  tm_post_data_payload_type    z_post_data_payload;
  uint8                        u_num_alm = 0, u_i = 0;
  uint32                       q_non_exist_svs_list = 0;
  uint32                       q_temp = 0;

  if ( 0 == q_alm_list )
  {
    return; /* No GPS Almanac to process. */
  }

  /* count number of alm. received in this session from the network */
  for ( u_i = 0; u_i < N_SV; u_i++ )
  {
    if ( ( q_alm_list >> u_i ) & 0x00000001 )
    {
      u_num_alm++;
    }
  }

  /* The condition under which the protocol submodules can deduce the info.
     on SVs' non-existence: at least C_COMPLETE_CONSTALLATION_ALM_THRESHOLD
     number of alm. received */
  if ( u_num_alm >= C_COMPLETE_CONSTALLATION_ALM_THRESHOLD )
  {
    /* considered a complete constallation, mark the SVs whose alm. were not 
       delivered as non-exist */
    q_non_exist_svs_list = ~q_alm_list;

    /* if any SV in this non-exist list happens to be on the eph.-delivered list,
       remove it from the non-exist list */
    q_temp = q_non_exist_svs_list & q_eph_list; /* q_temp now has list of no-alm but has-eph */

    q_non_exist_svs_list &= ~q_temp; /* remove no-alm but has-eph from the non-exist list */ 

    if ( q_non_exist_svs_list )
    {

      z_post_data_payload.sv_no_exist_data.q_SvNoExistMask = 
        q_non_exist_svs_list;

      (void) tm_post_data ( q_prtl_type,
                            session_handle,
                            TM_POST_DATA_TYPE_SV_NO_EXIST,
                            &z_post_data_payload );

    } /* if there are SVs that are considered not-exist */

  } /* if num. alm. >= C_COMPLETE_CONSTALLATION_ALM_THRESHOLD */

}

/*===========================================================================
FUNCTION tm_util_post_non_exist_glo_svs

DESCRIPTION

  This function is used by all protocol sub-modules to inject Glonass non-existence SVs
  assitance data, when it has received all the Glonass assistance data it can get in the
  current session, to:

  (1) Determine if it is in a position to deduce which Glonass SVs do not exist;
  (2) If the answer to (1) is positive, post such info to TM core; if the
      answer to (1) is negative, no further action will be taken.
 
  GNSS-engine supports injecting Glonass non-existence SVs not non-exist frequency.
 
DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_alm_list:      bit map of alm. delivered; SV1 on Bit 0.
  q_eph_list:      bit map of eph. delivered; SV1 on Bit 0
  p_rrc_glo_eph_freq: Glonass SV/Slot-Id to frequency number mapping info.
 
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_util_post_non_exist_glo_svs( tm_prtl_type q_prtl_type, tm_sess_handle_type session_handle,
                                     uint32 q_glo_alm_list, uint32 q_glo_eph_list)
{
  tm_post_data_payload_type z_post_data_payload;
  uint8 u_num_alm = 0, u_i = 0;
  uint32 q_non_exist_svs_list = 0;
  uint32 q_temp = 0;

  if ( 0 == q_glo_alm_list )
  {
    return; /* No Glo almanac to process. Could be MSA call flow. */
  }

  /* count number of alm. received in this session from the network */
  for ( u_i = 0; u_i < N_GLO_SV; ++u_i )
  {
    if ( (q_glo_alm_list >> u_i) & 0x00000001 )
    {
      u_num_alm++;
    }
  }

  /* The condition under which the protocol submodules can deduce the info.
     on SVs' non-existence: at least C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD
     number of alm. received */
  if ( u_num_alm >= C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD )
  {
    /* considered a complete constallation, mark the SVs whose alm. were not 
       delivered as non-exist */
    q_non_exist_svs_list = ~q_glo_alm_list;

    /* if any SV in this non-exist list happens to be on the eph.-delivered list,
       remove it from the non-exist list */
    q_temp = q_non_exist_svs_list & q_glo_eph_list; /* q_temp now has list of no-alm but has-eph */

    q_non_exist_svs_list &= ~q_temp; /* remove no-alm but has-eph from the non-exist list */

    /* Now q_non_exist_svs_list has non-existence SVs in both alm. & eph. */
    if ( q_non_exist_svs_list & 0x00FFFFFF)
    {
      memset((void*)&z_post_data_payload.sv_no_exist_glo_data, 0, sizeof(z_post_data_payload.sv_no_exist_glo_data)); 

      z_post_data_payload.sv_no_exist_glo_data.e_GnssType = MGP_GNSS_TYPE_GLO;

      z_post_data_payload.sv_no_exist_glo_data.z_Mask.z_FilledMask.u_SvFilledFlag = 1;
      /* q_SvNoExistMask - GLO bit (LSB) 0-23 = > Slot ID:1-24 */
      z_post_data_payload.sv_no_exist_glo_data.t_SvNoExistMask = (q_non_exist_svs_list & 0x00FFFFFF); 

      /* CD doesn't support Glonass AD for non-exist frequency */
      z_post_data_payload.sv_no_exist_glo_data.z_Mask.z_FilledMask.u_FreqFilledFlag = 0; 

      (void)tm_post_data(q_prtl_type,
                         session_handle,
                         TM_POST_DATA_TYPE_GNSS_SV_NO_EXIST,
                         &z_post_data_payload);

    } /* if there are SVs that are considered not-exist */

  } /* if num. alm. >= C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD */
}

/* Enum for MSG trace in following two functions */
enum
{
  LSM_CONF_CONVERT_FROM = 0,
  LSM_CONF_CONVERT_TO = 1
};

/*===========================================================================

FUNCTION
  convert_pct_limited

DESCRIPTION
  This function converts a float % in the range from 0.00 to 1.00 to a
    to a U8 index of percents in the range from 1-99, 
    warning if the input was out of that range.

RETURN: U8 percent

Note: this function is for the use of tm_util_get_cep_scale_ratio_from_pct
      only and is not exposed outside this module.

===========================================================================*/
static uint8 convert_pct_limited(float f_input_percent, uint8 u_case)
{
  f_input_percent *= (float)100.0;
  f_input_percent += (float)0.5; /* 1/2 a quanta of the bins, s.t. we round to
                               nearest int. */
  if (f_input_percent <= (float)1.0)
  {
    MSG_HIGH("%% conversion input too small %d %%, converting to 1%% (case %d)",
             (S32)(f_input_percent), u_case , 0);
    return 1;
  }
  else if (f_input_percent >= (float)99.0)
  {
    MSG_LOW("%% conversion input too big %d %%, converting to 99%% (case %d)",
             (S32)(f_input_percent), u_case , 0);
    return 99;
  }
  else
  {
    return(uint8) (f_input_percent);
  }
}

/*
 ******************************************************************************
 *
 * tm_util_get_cep_scale_ratio_from_pct
 *
 * Function description:
 *  This function gives the ratio to scale circular uncertainties (CEP's) from
 *  different percentiles.  E.g. 39% conf. "standard" to 99% conf.
 *
 * Parameters:
 *
 *  f_from_percentile: CEP percentile to convert from (range 0.00 to 1.00)
 *  f_to_percentile: CEP percentile to convert to (range 0.00 to 1.00)
 *
 * Return value:
 *
 *  The scale factor to multiply by to scale the CEP<from>% to a CEP<to>% 
 *   value (in meters.)
 *
 ******************************************************************************
*/
float tm_util_get_cep_scale_ratio_from_pct(float f_from_percentile, 
                                           float f_to_percentile)
{
  static const float f_cep_percentile_scalars[ 100 ] =
  {
   /* Value for 0% should be 0f, but preventing divide by zero */
    /*  0% - 9% */                               
    0.0001f, 0.1418f, 0.2010f, 0.2468f, 0.2857f, 
    0.3203f, 0.3518f, 0.3810f, 0.4084f, 0.4343f,
    /* 10% */
    0.4590f, 0.4828f, 0.5056f, 0.5278f, 0.5492f,
    0.5701f, 0.5905f, 0.6105f, 0.6300f, 0.6492f,
    /* 20% */
    0.6680f, 0.6866f, 0.7049f, 0.7230f, 0.7409f,
    0.7585f, 0.7760f, 0.7934f, 0.8106f, 0.8276f,
    /* 30% */
    0.8446f, 0.8615f, 0.8783f, 0.8950f, 0.9116f,
    0.9282f, 0.9448f, 0.9613f, 0.9778f, 0.9943f,
    /* 40% */
    1.0108f, 1.0273f, 1.0438f, 1.0603f, 1.0769f,
    1.0935f, 1.1101f, 1.1268f, 1.1436f, 1.1605f,
    /* 50% */
    1.1774f, 1.1944f, 1.2116f, 1.2288f, 1.2462f,
    1.2637f, 1.2814f, 1.2992f, 1.3172f, 1.3354f,
    /* 60% */
    1.3537f, 1.3723f, 1.3911f, 1.4101f, 1.4294f,
    1.4490f, 1.4689f, 1.4891f, 1.5096f, 1.5305f,
    /* 70% */
    1.5518f, 1.5735f, 1.5956f, 1.6182f, 1.6414f,
    1.6651f, 1.6894f, 1.7145f, 1.7402f, 1.7667f,
    /* 80% */
    1.7941f, 1.8225f, 1.8519f, 1.8825f, 1.9145f,
    1.9479f, 1.9830f, 2.0200f, 2.0593f, 2.1011f,
    /* 90% - 99% */
    2.1460f, 2.1945f, 2.2475f, 2.3062f, 2.3721f,
    2.4477f, 2.5373f, 2.6482f, 2.7971f, 3.0349f
  };

  uint8 u_from_pct, u_to_pct;

  u_from_pct = convert_pct_limited(f_from_percentile, LSM_CONF_CONVERT_FROM);
  u_to_pct =   convert_pct_limited(f_to_percentile,   LSM_CONF_CONVERT_TO);

  MSG_LOW("conversion value %d, %d", u_to_pct, u_from_pct,0);
  return(f_cep_percentile_scalars[u_to_pct] / 
         f_cep_percentile_scalars[u_from_pct]);
}

/*===========================================================================
FUNCTION tm_util_check_numerical_suffix

DESCRIPTION
  This function that the input string is a numerical value, that is a 
  specific number of bytes in length and is within the min and max values
  provided.
  

DEPENDENCIES
  None.

RETURN VALUE
   TRUE - Numerical value within bounds
   FALSE - Incorrect string passed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean tm_util_check_numerical_suffix
(
  const char* p_path,
  uint32      min,
  uint32      max,
  uint8       bytes_to_convert
)
{
  uint8  index   = 0;
  uint64 value   = 0;
  
  /* If NULL string, or string is longer than bytes_to_convert, return FALSE */
  if(p_path == NULL || p_path[bytes_to_convert] != '\0')
  {
    return FALSE;
  }
 
  /* If no bytes_to_convert, return TRUE */
  if( bytes_to_convert == 0 )
  {
    return TRUE;
  }
  
  /* examine each byte in string */
  while(p_path[index] != '\0')
  {
    /* If any unexpected characters, return FALSE */
    if(p_path[index] > '9' || p_path[index] < '0')
    {
      return FALSE;
    }
    else
    {
      value += value * 10 + p_path[index] - '0';
    }
    index++;
  }

  /* Convert value must be within the bounds given */
  if( value < min || value > max)
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION tm_util_efs_dir_and_file_name_is_valid

DESCRIPTION
  This function verifies if the directory and file name is one of the valid names
  defined in acceptable list
  The following fields are checked
  - if the file length is > PDSM_PD_EFS_MAX_FILE_LEN_BYTES, returns an error
  - if the name does not match any of the names in the valid list, returns an error
  

DEPENDENCIES
  None.

RETURN VALUE
   TRUE - Valid directory and file name
   FALSE - Invalid directory and file name

SIDE EFFECTS
  None.
===========================================================================*/

boolean tm_util_efs_dir_and_file_name_is_valid
(
  const char *p_dir_and_filename, 
  uint8 file_operation,
  uint32 file_size
)
{
  uint8  u_index         = 0;
  int32  i_prefix_length = 0;
  uint16 valid_path_size = 0; 
  uint16 input_path_size = strlen(p_dir_and_filename);

  /* validate the file operation. Only WRITE, DELETE, PURGE_DIR are permitted */
  if ((file_operation != GPS_FS_DELETE_FILE)       &&
      (file_operation != GPS_FS_CREATE_WRITE_FILE) &&
      (file_operation != GPS_FS_PURGE_DIR))
  {
    MSG_HIGH("TM_UTIL: Invalid file operation", file_operation,0,0);
    return FALSE;
  }

  /* validate the file size */
  if (file_size > PDSM_PD_EFS_MAX_FILE_LEN_BYTES )
  {
    MSG_HIGH("TM_UTIL file length %d > %d", file_size, PDSM_PD_EFS_MAX_FILE_LEN_BYTES,0);
    return FALSE;
  }


  /***************************************************************************
   * go through the list of accepatable files and see if the name matches    *
   * A valid input path name will be of the same length as a known good path *
   * and lexicographically be between the minimum path, and the maximum path *
   ***************************************************************************/
  u_index = 0;
  while (cgps_valid_efs_paths[u_index].prefix_length != 0 )
  {
    /* The input file path size must be same as that of a valid path */
    valid_path_size = cgps_valid_efs_paths[u_index].prefix_length + 
                      cgps_valid_efs_paths[u_index].suffix_length;
    if(input_path_size != valid_path_size)
    {
      u_index++;
      continue;
    }

    /* The input file path must be same as the prefix of a valid path */
    if (strncmp(p_dir_and_filename,
                cgps_valid_efs_paths[u_index].path_prefix, 
                cgps_valid_efs_paths[u_index].prefix_length) != 0)
    {
      u_index++;
      continue;
    }

    i_prefix_length = cgps_valid_efs_paths[u_index].prefix_length;
    /* input file suffix must lie between the min and max suffixes */
    if( tm_util_check_numerical_suffix(
               &p_dir_and_filename[i_prefix_length],
                cgps_valid_efs_paths[u_index].min_suffix,
          cgps_valid_efs_paths[u_index].max_suffix,
    cgps_valid_efs_paths[u_index].suffix_length) == FALSE )
    {
      u_index++;
      continue;
    }

    MSG_LOW("path name valid", 0, 0, 0);
    return TRUE;
  }  
  /* no match against any known file names, return ERROR */
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
                "Invalid Path %s", p_dir_and_filename);
  
  MSG_ERROR("path name invalid", 0,0,0);
  return FALSE;
}

/*===========================================================================
FUNCTION tm_util_post_unhealthy_svs

DESCRIPTION

  This function is used by all protocol submodules when the submodule is 
  convinced it has received all the ephemeris data it can get in the 
  current session. If delivered list of Ephemeris for SVs is not the same
  as the requested list, the remaining SVs are marked unhealthy and sent
  to PE:


DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_eph_req_list:  bit map of eph. requested; SV1 on Bit 0.
  q_eph_delivered_list:      bit map of eph. delivered; SV1 on Bit 0

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_post_unhealthy_svs
( tm_prtl_type             q_prtl_type, 
  tm_sess_handle_type      session_handle,
  uint32                   q_eph_unhealthy_list
)
{
  
  tm_post_data_payload_type    z_post_data_payload;

  if ( q_eph_unhealthy_list )
  {
    MSG_MED("TM Core post RTI/Health list 0x%x", q_eph_unhealthy_list, 0, 0);
    z_post_data_payload.sv_unhealthy_data.q_SvUnhealthyMask = q_eph_unhealthy_list; 
    (void) tm_post_data ( q_prtl_type, session_handle,
                          TM_POST_DATA_TYPE_SV_UNHEALTHY, &z_post_data_payload );
  } 
}


/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long
 *
 * Function description:
 *  This function counts the number of bits set in a uint32 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  q_long_word: bit mask;
 *
 * Return value:
 *
 * number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long(uint32 q_long_word)
{
 uint8 u_cnt = 0;

 while(q_long_word !=0 )
 {
   if (q_long_word & ((uint32)0x1 << 31))
   {
     u_cnt++;
   }
   q_long_word = q_long_word << 1;
 }
 return u_cnt;
}

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long64
 *
 * Function description:
 *  This function counts the number of bits set in a uint64 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  t_long [IN]: bit mask;
 *
 * Return value:
 *
 *  Number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long64( uint64 t_long_bits )
{
  uint8 u_cnt = 0;

  for ( u_cnt = 0; t_long_bits; u_cnt++ )
    {
    t_long_bits &= t_long_bits - 1; // clear the least significant bit set
  }
  return ( u_cnt );
}

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_horiz_uncertainty
 *
 * Function description:
 *  This function calculate the horizontal velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  horizontal velocity uncertainty (m/s)
 ******************************************************************************
*/
FLT tm_util_calc_vel_horiz_uncertainty(const sm_GnssNavPosition *nav_ptr)
{
  if (nav_ptr == NULL)
  {
    MSG_ERROR("null ptr passed %x", nav_ptr, 0, 0);
    return 0;
  }

  // use 3D velocity uncertainty directly
  //return (FLT)(nav_ptr->f_Vunc3dMps);

  /* Calculate velUncHoriz */
  return (FLT)(sqrt(nav_ptr->f_VuncEastNorthMps[0] * 
                    nav_ptr->f_VuncEastNorthMps[0] +
                    nav_ptr->f_VuncEastNorthMps[1] * 
                    nav_ptr->f_VuncEastNorthMps[1]));
}

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_vert_uncertainty
 *
 * Function description:
 *  This function calculate the vertical velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  vertical velocity uncertainty (m/s)
 ******************************************************************************
*/
FLT tm_util_calc_vel_vert_uncertainty(const sm_GnssNavPosition *nav_ptr)
{
  if (nav_ptr == NULL)
  {
    MSG_ERROR("null ptr passed %x", nav_ptr, 0, 0);
    return 0;
  }

  // use 3D velocity uncertainty directly
  //return (FLT)(nav_ptr->f_Vunc3dMps);

  return (FLT)(nav_ptr->f_VuncVertMps);
}

/*
 ******************************************************************************
 *
 * FUNCTION:
 * tm_util_calculate_position_print
 *
 * DESCRIPTION:
 *  This function calculates position using fix report
 *  Position is calcuated in Latitude and Longitude
 *
 * PARAMETERS:
 *  p_fix:          [Input] ptr to fix report
 *  p_Str:          [Output]ptr to the string where data will be written
 *  q_buf_len:      length of string
 *
 * RETURN VALUE:
 *  q_len           length of string written
 *
 ******************************************************************************
*/
int tm_util_calculate_position_print(const sm_GnssFixRptStructType *p_fix, char *p_Str, int q_buf_len)
{
  double d_Lat, d_Long, d_LatMins, d_LongMins = 0.0;
  char c_LatHem, c_LongHem;
  int q_len = 0;
  
  if ( ( NULL == p_fix) || (NULL == p_Str) || (q_buf_len <= 0))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position(): Invalid Input");
    return (0);
  }
  
  /* Latitude and longitude: */
  d_Lat = p_fix->z_NavPos.d_PosLla[0] * (180.0 / C_PI);
  d_Long = p_fix->z_NavPos.d_PosLla[1] * (180.0 / C_PI);

  if ( d_Lat > 0.0 )
  {
    c_LatHem = 'N';
  }
  else
  {
    c_LatHem = 'S';
    d_Lat *= -1.0;
  }

  d_LatMins = fmod( d_Lat * 60.0 , 60.0 );

  if ( d_Long < 0.0 )
  {
    c_LongHem = 'W';
    d_Long *= -1.0;
  }
  else
  {
    c_LongHem = 'E';
  } 

  d_LongMins = fmod( d_Long * 60.0 , 60.0 );
  
  q_len = snprintf( p_Str, q_buf_len,"%02d%09.6lf,%c,%03d%09.6lf,%c," ,
                        (uint8) floor( d_Lat ) , d_LatMins , c_LatHem ,
                        (uint8) floor( d_Long ) , d_LongMins , c_LongHem);
  
  if(q_len <= 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("tm_util_calculate_position_print: Error in string formatting",0,0,0);  
    return (0);
  }
  return q_len;
  
}	
	
/*
 ******************************************************************************
 *
 * tm_util_get_rand
 *
 * Function description:
 *   Gets a random number from security module.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *  uint32 random number.
 *
 ******************************************************************************
*/
uint32 tm_util_get_rand
(
  void
  )
{
  uint32 rand_num;
  #if (defined(__GRIFFON__) || defined (FEATURE_CGPS_PLAYBACK))
  rand_num = rand();
  #else
  secerrno_enum_type errorVal;

  errorVal = secapi_get_random(SECAPI_SECURE_RANDOM,
                               (uint8 *)&rand_num, sizeof(rand_num));

  if ( errorVal != E_SUCCESS )
  {
    MSG_FATAL("Could not generate random num. Err %d", errorVal, 0, 0);
  }
  #endif

  return ( rand_num );
}

/*===========================================================================

FUNCTION tm_util_map_and_inject_glo_steering_to_tmcore

DESCRIPTION
  Inject Glonass Acquisition Assistance data - Steering - into GNSS engine,
  when deltaT/leap second (from GPS UTCModel or Glonass TimeModel AD), Raw AcqAssist
  (a.k.a Reference Measurement Info) & Glo AuxiInfo is available.

DEPENDENCIES
  Call tm_util_map_glo_acqast_svid_to_freq_num() to associate SvId to FreqNum
  before invoking this function.

RETURN VALUE
  Return when AA Steering isn't injected into GNSS engine.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_util_map_and_inject_glo_steering_to_tmcore( prot_glo_acqast_buffer_s_type *const p_glo_acqasst,
                                                       prot_glo_acqasst_sat_buffer_s_type const *const p_glo_acqasst_subset,
                                                       prot_glo_svcn_buffer_s_type const *const p_glo_auxi_info,
                                                       gnss_time_info const *const p_gnss_time,
                                                       tm_prtl_type prtl_type,
                                                       tm_sess_handle_type sess_handle,
                                                       tm_post_data_payload_type *data_payload_ptr )
{
  uint8 u_index = 0;

  /* Inject Glonass Acquisition Assistance - Steering - if deltaT/leap second (from GPS UTCModel or Glonass TimeModel AD)
  *  + Raw AcqAssist (a.k.a Reference Measurement Info) + Glo AuxiInfo is available.
  */

  /* Glonass Acquisition Assistance - Steering - valid ? */
  if ( (p_glo_acqasst->z_steering_glo_data.u_NumSvs > 0) &&
       (p_glo_acqasst->z_steering_glo_data.u_NumSvs <= N_GLO_SV) )
  {
    /* Glonass time isn't yet used to compute 'gnss msec of week' ? */
    if ( (p_glo_acqasst->m.gnssMsecOfWeekValid != 1) && (1 == p_gnss_time->m.glonassTimeValid) )
    {
      /* Number of Glonass satellite is 24 max */
      for ( u_index = 0; u_index < p_glo_acqasst_subset->u_cnt; ++u_index )
      {
        /* Compute GNSS msec of week from integer code phase */
        p_glo_acqasst->z_steering_glo_data.z_SvSteerInfo[u_index].q_Ms = tm_util_translate_glo_intcodephase(p_glo_acqasst_subset->p_svid_icp[u_index].integer_code_phase,
                                                                                                           p_gnss_time->z_GlonassTime.q_GloMsec);
        p_glo_acqasst->m.gnssMsecOfWeekValid = 1;
      }
    }

    /* Post steering to MGP when both Time & Freq mapping is valid */
    if ( (TRUE == p_glo_acqasst->u_freq_num_valid) && (1 == p_glo_acqasst->m.gnssMsecOfWeekValid) )
    {
      /* Copy over Glonass time & Steering into TM post struct */
      memscpy(&data_payload_ptr->gnss_aa_data, sizeof(data_payload_ptr->gnss_aa_data), &p_glo_acqasst->z_steering_glo_data, sizeof(p_glo_acqasst->z_steering_glo_data));

      /* Fill Glonass time fields */
      data_payload_ptr->gnss_aa_data.u_GloCycleValid = (p_gnss_time->z_GlonassTime.u_FourYear != C_GLO_FOUR_YEAR_UNKNOWN);
      data_payload_ptr->gnss_aa_data.u_GloCalDayValid = (p_gnss_time->z_GlonassTime.w_Days != C_GLO_DAY_UNKNOWN);

      data_payload_ptr->gnss_aa_data.u_GloNumCycles = p_gnss_time->z_GlonassTime.u_FourYear;
      data_payload_ptr->gnss_aa_data.w_GloCalDay = p_gnss_time->z_GlonassTime.w_Days;
      data_payload_ptr->gnss_aa_data.q_Msec = p_gnss_time->z_GlonassTime.q_GloMsec;
      data_payload_ptr->gnss_aa_data.u_TimeRefIsGnss = TRUE;
      data_payload_ptr->gnss_aa_data.q_SvSteerSrc = MGP_GNSS_SV_STEER_SRC_GLO;

      MSG_6(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Glo AA-Steering to post with Glonass Time: %d, %d, %u, %u, %d, %lu, %u",
            data_payload_ptr->gnss_aa_data.u_GloCycleValid,
            data_payload_ptr->gnss_aa_data.u_GloCalDayValid,
            data_payload_ptr->gnss_aa_data.u_GloNumCycles,
            data_payload_ptr->gnss_aa_data.w_GloCalDay,
            data_payload_ptr->gnss_aa_data.q_Msec,
            data_payload_ptr->gnss_aa_data.u_TimeRefIsGnss);

      /* Post Glonass AA data to TM. This would inturn ask LM to turn MGP receiver ON */
      return ( tm_post_data(prtl_type,
                           sess_handle,
                           TM_POST_DATA_TYPE_AA_GLO,
                           data_payload_ptr) );

    }
  }
  else
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Glonass AcqAsst Steering has invalid num of SVs (%u). prtl_type %d",
          p_glo_acqasst->z_steering_glo_data.u_NumSvs, prtl_type);
  }

  return ( FALSE );
}

/*===========================================================================

FUNCTION tm_util_map_glo_acqast_svid_to_freq_num

DESCRIPTION
 Map Glonass Acquisition Assistance SvId to frequency number using the information
 in stored SV/ChannelNumber buffer structure.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_map_glo_acqast_svid_to_freq_num( prot_glo_acqast_buffer_s_type *const p_GloAcqAsstBuf,
                                              prot_glo_svcn_buffer_s_type const *const p_GloSvCnMapBuf )
{

  uint8 u_sv_iter = 0, u_glo_num_svs = 0;
  uint16 w_SvId = 0;

  /* Glonass Acq Assistance & Sv/Channel# map information should be available */
  if (( p_GloAcqAsstBuf->gnss_sv_dir.u_NumSvs > 0 ) && ( p_GloSvCnMapBuf->u_n > 0 ))
  {
    /* Ensure the total SV count for SV-Dir & Steering are same. This help avoid looping twice. */
    u_glo_num_svs = p_GloAcqAsstBuf->gnss_sv_dir.u_NumSvs;
    if ( p_GloAcqAsstBuf->gnss_sv_dir.u_NumSvs != p_GloAcqAsstBuf->z_steering_glo_data.u_NumSvs )
    {
      if ( p_GloAcqAsstBuf->gnss_sv_dir.u_NumSvs > p_GloAcqAsstBuf->z_steering_glo_data.u_NumSvs )
      {
        u_glo_num_svs = p_GloAcqAsstBuf->z_steering_glo_data.u_NumSvs;
      }

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SV-Dir & Steering SV count differs %u, %u. Using only %u AA SVs",
            p_GloAcqAsstBuf->gnss_sv_dir.u_NumSvs, p_GloAcqAsstBuf->z_steering_glo_data.u_NumSvs, u_glo_num_svs);
    }

    /* Loop through AA SVs & set the corresponding FreqNum */
    for ( u_sv_iter = 0; u_sv_iter < u_glo_num_svs; ++u_sv_iter )
    {
      /* AA-SvDir */
      /* local u_Sv (65..88) & protocol sv_id (0..23) */
      w_SvId = p_GloAcqAsstBuf->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].w_Sv - (GLO_SLOTID_OFFSET + 1);
      if ( TRUE == p_GloSvCnMapBuf->z_glo_sat_elem[w_SvId].m.channelNumValid )
      {
        p_GloAcqAsstBuf->u_freq_num_valid = TRUE;
        /* Per GLONASS ICD page 10, after year 2005, GLO SV would use frequency ID -7 to +6. Freq channel +7 to +13 would not be used. */
        p_GloAcqAsstBuf->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].b_GloFreqIndex = p_GloSvCnMapBuf->z_glo_sat_elem[w_SvId].channel_num & 0xFF;
      }


      /* AA-Steering */
      /* local u_Sv (65..88) & protocol sv_id (0..23) */
      w_SvId = p_GloAcqAsstBuf->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].w_Sv - (GLO_SLOTID_OFFSET + 1);
      if ( TRUE == p_GloSvCnMapBuf->z_glo_sat_elem[w_SvId].m.channelNumValid )
      {
        p_GloAcqAsstBuf->u_freq_num_valid = TRUE;
        p_GloAcqAsstBuf->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].b_GloFreqIndex = p_GloSvCnMapBuf->z_glo_sat_elem[w_SvId].channel_num & 0xFF;
      }
    } /* Loop through AA data */

    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Possibly mapped SV-to-FreqNum for %3u SVs", u_glo_num_svs);
  }
}

/*===========================================================================

FUNCTION tm_util_translate_glo_intcodephase

DESCRIPTION
 Compute the Glonass milli-second of the week from interger code phase &
 Glonass Time Of Day in milli-seconds.

DEPENDENCIES

RETURN VALUE
  GNSS milli-second of the week

SIDE EFFECTS
  None

===========================================================================*/
uint32 tm_util_translate_glo_intcodephase(uint32 const q_integer_code_phase, uint32 const q_GloMsecInDay)
{
  if (q_GloMsecInDay < q_integer_code_phase)
  {
    return ((uint32)((q_GloMsecInDay + DAY_MSECS - q_integer_code_phase) % 80));
  }
  else
  {
    return ((uint32)((q_GloMsecInDay - q_integer_code_phase) % 80));
  }
}

/*===========================================================================

FUNCTION tm_util_cp_msa_gps_mandatory_ad_needed

DESCRIPTION
 Return the mandatory GPS assistance data needed from network/LS. The returned
 list is used by protocol module to request missing GPS AD from network/LS.
 Use this function only in A-GPS only call flow.

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_cp_msa_gps_mandatory_ad_needed( uint32 *p_GpsAdNeeded, uint32 const q_GpsAdDelivered )
{
  if (p_GpsAdNeeded == NULL)
  {
    return; /* Failure */
  }

  /* Init */
  *p_GpsAdNeeded = 0;

  /* Currently there is no difference in mandatory AD for E911 vs. VAS services */

  /* For GPS - GPS refTime & GPS AcqAsst will be requested if not available. */
  *p_GpsAdNeeded |= (C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_ACQ);

  /* Remove GPS AD that is already received from network/LS */
  *p_GpsAdNeeded &= ~q_GpsAdDelivered;
}

/*===========================================================================

FUNCTION tm_util_cp_msa_gnss_mandatory_ad_needed

DESCRIPTION
 Return the mandatory GNSS assistance data needed from network/LS. The returned
 list is used by protocol module to request missing AD from network/LS.
 Use this function only in A-GNSS call flow.

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_cp_msa_gnss_mandatory_ad_needed(uint32 *p_GpsAdNeeded, uint32 *p_GloAdNeeded, uint32 const q_GpsAdDelivered, uint32 const q_GloAdDelivered)
{
  if ( (p_GpsAdNeeded == NULL) || (p_GloAdNeeded == NULL) )
  {
    return; /* Failure */
  }

  /* Init */
  *p_GpsAdNeeded = 0;
  *p_GloAdNeeded = 0;

  /* Currently there is no difference in mandatory AD for E911 vs. VAS services */

  /* For GPS - GPS refTime, GPS AcqAsst, GPS UTCModel will be requested if not available.
     For Glonas - GLO AcqAsst, GLO AuxiInfo, GLO TimeModel will be requested. */
  *p_GpsAdNeeded |= (C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_ACQ | C_GPS_ASSIS_UTC);
  *p_GloAdNeeded |= (C_GNSS_ASSIST_ACQ | C_GNSS_ASSIST_TIME_MODELS | C_GNSS_ASSIST_AUXI_INFO);

  /* Remove GPS AD that is already received from network/LS */
  *p_GpsAdNeeded &= ~q_GpsAdDelivered;

  /* Remove GLONASS AD that is already received from network/LS */
  *p_GloAdNeeded &= ~q_GloAdDelivered;
}


/*===========================================================================

FUNCTION tm_util_clean_qzss_sbas_meas_rpt

DESCRIPTION
Remove SBAS or QZSS PRNS from GNSS_MEAS_BLK_SRC_QZSS_SBAS. If v_keep_sbas is TRUE
QZSS prns are removed. If v_keep_sbas is FALSE SBAS prns are removed.
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_util_clean_qzss_sbas_meas_rpt(sm_GpsMeasRptStructType* p_measReport,boolean v_keep_sbas)
{
  if(p_measReport == NULL || ((p_measReport->z_MeasBlk).u_NumSvs == 0))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Measurement Report is empty.");
    return;
  }

  if( (p_measReport->z_MeasBlk).e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_QZSS_SBAS )
  {
    uint8 temp_u_NumSvs = 0; 
    uint8 u_itr = 0; 
    uint8 u_itr_max = N_ACTIVE_CHAN;
    uint16 w_sv_min=0;
    uint16 w_sv_max=0;

    gnss_MeasStructType temp_z_Qzss[N_ACTIVE_CHAN];
    memset((void*)&temp_z_Qzss,0,sizeof(temp_z_Qzss));

    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW,"Original u_NumSvs = %u",(p_measReport->z_MeasBlk).u_NumSvs);

    if((p_measReport->z_MeasBlk).u_NumSvs < N_ACTIVE_CHAN)
    {
      u_itr_max = (p_measReport->z_MeasBlk).u_NumSvs;
    }

    if(TRUE == v_keep_sbas)
    {
       w_sv_min = PDSM_PD_SV_ID_SBAS_MIN;
       w_sv_max = PDSM_PD_SV_ID_SBAS_MAX;
    }
    else
    {
       w_sv_min = PDSM_PD_SV_ID_QZSS_MIN;
       w_sv_max = PDSM_PD_SV_ID_QZSS_MAX;
    }

    for(u_itr = 0; u_itr < u_itr_max; u_itr++ )
    {
      uint16 w_Sv = (p_measReport->z_MeasBlk).z_Gnss[u_itr].w_Sv;
      if( (w_Sv >= w_sv_min) && (w_Sv <= w_sv_max) )
      {
        temp_z_Qzss[temp_u_NumSvs] = (p_measReport->z_MeasBlk).z_Gnss[u_itr];
       
        MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Adding SV %u",
                 temp_z_Qzss[temp_u_NumSvs].w_Sv);

        temp_u_NumSvs++;
      }

    }

    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Qzss, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);

   (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;

    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "New u_NumSvs = %u",(p_measReport->z_MeasBlk).u_NumSvs);
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Non QZSS/SBAS measurement block, nothing done");
  }

  return;

}