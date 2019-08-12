/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM-Vx Message

General Description
  This file contains implementations for Vx message creating
  
  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx_msg.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/06    cl     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

#include "msg.h"
#include "pdapibuf.h"
#include "tm_common.h"
#include "tm_data.h"
#include "tm_1x_up.h"
#include "tm_vx_i.h"
#include "tm_vx_msg.h"
#include "tm_vx_ni_client.h"

extern void sign_extend_b
(
  byte*  val,
  word   len
);

extern void sign_extend_d
(
  dword* val,
  word   len
);

extern void sign_extend_w
(
  word*  val,
  word   len
);

/*===========================================================================

FUNCTION tm_vx_msg_get_bs_info

DESCRIPTION
  This is an unction called to get the latest base station information
  (BAND_CLASS, CDMA_CHAN, BASE_ID, SID, NID) where the mobile has registered.
    
DEPENDENCIES

RETURN VALUE
  NONE
  
SIDE EFFECTS
  None

===========================================================================*/
void tm_vx_msg_get_bs_info
(
  mcc_pd_bs_info_type  *info_ptr
    /* pointer to hold the base station information */
)
{
  if(info_ptr)
  {
    mcc_pd_get_bs_info(info_ptr);
  }
  else
  {
    MSG_MED("Null info_ptr", 0, 0, 0);  
  }
  return; 
}

/*=========================================================================
FUNCTION
  tm_vx_gps_total_msec

DESCRIPTION
  Translate system timestamp into GPS millisecond timestamp

DEPENDENCIES
  NA
RETURN VALUE
  The status of the message delivery.

SIDE EFFECTS
  None

=========================================================================*/
void tm_vx_gps_total_msec( qword timestamp, qword milliseconds )
{
  uint32 sub_ms;
  uint32 sub_1_25_ms;

  /* Get "sub 1.25 ms" part of time stamp */
  sub_1_25_ms = timestamp[0] & 0xFFFF;

  /* Rescale so original range spans 1 ms. instead of 1.25 ms. */
  sub_1_25_ms = (sub_1_25_ms * 5) >> 2;

  /*
   * Extract portion of timestamp in units of 1.25 ms. to create a time
   * in milliseconds.
  */
  (void)qw_div_by_power_of_2( milliseconds, timestamp, 16 );

  /* Scale to units of 0.25 ms. */
  qw_mul ( milliseconds, milliseconds, 5);

  /* Extract sub millisecond portion for 1 ms. adjustment to follow. */
  sub_ms = milliseconds[0] & 0x3;

  /* Scale to units of milliseconds. */
  (void)qw_div_by_power_of_2( milliseconds, milliseconds, 2 );

  /*
   * Add a millisecond if sub 1.25 ms. and sub ms. portions add up to
   * more than a millisecond.
  */
  sub_ms *= QUARTER_MS_VAL;
  sub_1_25_ms += sub_ms;
  if (sub_1_25_ms >= 4 * QUARTER_MS_VAL)
  {
      qw_inc( milliseconds, 1 );
  }
} /* end tm_vx_gps_total_msec() */


/*=========================================================================
FUNCTION
  tm_vx_gps_week_ms

DESCRIPTION
  Translate system timestamp into GPS week and millisecond fields

DEPENDENCIES
  NA
RETURN VALUE
  The status of the message delivery.

SIDE EFFECTS
  None

=========================================================================*/
void tm_vx_gps_week_ms( qword ts, uint16 *gps_week, uint32 *gps_ms )
{
  qword temp;
  qword milliseconds;
  tm_vx_gps_total_msec(ts,milliseconds);

  /*
   * The obtain the week two divisions are performed because the argument
   * to qw_div is uint16.
  */
  /* Divide by the number of minutes in a week. */
  (void)qw_div( temp, milliseconds, MINUTES_PER_WEEK );

  /* Divide by the number of milliseconds in a minute. */
  (void)qw_div( temp, temp, MS_PER_MINUTE );

  /* Return the gps week. */
  *gps_week = (uint16) temp[0];

  /* Number of milliseconds based on the total number of whole weeks. */
  qw_mul( temp, temp, MS_PER_WEEK );

  /* Number of milliseconds within the week. */
  qw_sub( temp, milliseconds, temp );

  /* Return the number of milliseconds within the week. */
  *gps_ms = temp[0];
} /* end tm_vx_gps_week_ms() */

/*===========================================================================

FUNCTION tm_vx_msg_xlate_v1_sppreq

DESCRIPTION
  This function converst internal V1 SPPReq to external packed V1 SPPReq format
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_vx_msg_xlate_v1_sppreq
(
  byte                            *output_buf_ptr,
  const tm_vx_int_v1_sppreq_s_type *input_ptr
)
{
  int  i;    
  word buffer_pos = 0;

  if((output_buf_ptr == NULL) || (input_ptr == NULL))
  { 
    MSG_ERROR("Null input pointers", 0, 0, 0);
    return;
  }

  //Version
  b_packb( input_ptr->version, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, version) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, version);

  //Length
  b_packw( input_ptr->length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, length) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, length);
  
  // MS PCAP
  b_packb( input_ptr->mspcap, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, mspcap) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, mspcap);
    
  //app type
  b_packb( input_ptr->app_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, app_type) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, app_type);

  //consent ind
  b_packb( input_ptr->consent_ind, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, consent_ind) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, consent_ind);
    
  //pqos
  b_packb( input_ptr->pqos, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, pqos) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, pqos);

  //Brew id
  b_packd( input_ptr->brew_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, brew_id) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, brew_id);

  //sid
  b_packw( input_ptr->sid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type,sid) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, sid);

  //nid
  b_packw( input_ptr->nid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type,nid) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, nid);

  //bsid
  b_packw( input_ptr->base_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, base_id) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, base_id);

  // base lat
  b_packd( input_ptr->base_lat, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, base_lat) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, base_lat);

  // base long
  b_packd( input_ptr->base_long, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, base_long) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, base_long);

  // Time week
  b_packw( input_ptr->time_week, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v1_sppreq_s_type, time_week) );
  buffer_pos += FSIZ(tm_vx_ext_v1_sppreq_s_type, time_week);

  for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ )
  {
    b_packb( input_ptr->time_sec[i], output_buf_ptr, buffer_pos, FSIZ(gps_time_sec_type, time_sec) );
    buffer_pos += FSIZ(gps_time_sec_type, time_sec);
  }

  // app record length
  b_packw( input_ptr->app_record.id_length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record,id_length) );
  buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id_length);
    
  // app record id
  for ( i = 0; (i < input_ptr->app_record.id_length) && (i < TM_VX_MAX_APP_INFO_LEN ); i++ )
  {
    b_packb( input_ptr->app_record.id[i], output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record, id) );
    buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id);
  }

  return;
}

/*===========================================================================

FUNCTION tm_vx_msg_xlate_v2_sppreq

DESCRIPTION
  This function converst internal V2 SPPReq to external packed V2 SPPReq format
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_vx_msg_xlate_v2_sppreq
(
  byte                            *output_buf_ptr,
  const tm_vx_int_v2_sppreq_s_type *input_ptr
)
{
  int  i;    
  word buffer_pos = 0;

  if((output_buf_ptr == NULL) || (input_ptr == NULL))
  { 
    MSG_ERROR("Null input pointers", 0, 0, 0);
    return;
  }

  //Version
  b_packb( input_ptr->version, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, version) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, version);

  //Msg type
  b_packb( input_ptr->msg_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, msg_type) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, msg_type);

  //Length
  b_packw( input_ptr->length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, length) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, length);

  //MS PCAP
  b_packb( input_ptr->mspcap, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, mspcap) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, mspcap);
    
  //App type
  b_packb( input_ptr->app_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, app_type) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, app_type);

  //Pqos
  b_packb( input_ptr->pqos, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, pqos) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, pqos);

  //duration
  b_packd( input_ptr->duration, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, duration) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, duration);

  //cor id
  b_packb( input_ptr->cor_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, cor_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, cor_id);

  //msid
  buffer_pos +=  FSIZ(tm_vx_ext_v2_sppreq_s_type, ms_id);

  //sid
  b_packw( input_ptr->sid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type,sid) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, sid);

  //nid
  b_packw( input_ptr->nid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type,nid) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, nid);

  //bsid
  b_packw( input_ptr->base_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, base_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, base_id);

  //base lat
  b_packd( input_ptr->base_lat, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, base_lat) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, base_lat);

  //base long
  b_packd( input_ptr->base_long, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, base_long) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, base_long);

  //ref pn
  b_packw( input_ptr->ref_pn, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, ref_pn) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, ref_pn);
    
  //band class
  b_packb( input_ptr->band_class, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, band_class) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, band_class);
    
  //cdma freq
  b_packw( input_ptr->cdma_freq, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, cdma_freq) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, cdma_freq);
    
  //Time week
  b_packw( input_ptr->time_week, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, time_week) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, time_week);
    
  for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ )
  {
    b_packb( input_ptr->time_sec[i], output_buf_ptr, buffer_pos, FSIZ(gps_time_sec_type, time_sec) );
    buffer_pos += FSIZ(gps_time_sec_type, time_sec);
  }
  
  //brew app
  b_packd( input_ptr->brew_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_sppreq_s_type, brew_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_sppreq_s_type, brew_id);
    
  //app record length
  b_packw( input_ptr->app_record.id_length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record,id_length) );
  buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id_length);
    
  //app record id
  for ( i = 0; (i < input_ptr->app_record.id_length) && (i <TM_VX_MAX_APP_INFO_LEN); i++ )
  {
    b_packb( input_ptr->app_record.id[i], output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record, id) );
    buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id);
  }
} /* end paix_xlate_ext_sppreq_req_v2() */

/*===========================================================================

FUNCTION tm_vx_msg_xlate_v2_sppreq

DESCRIPTION
  This function converst internal V2 pos rpt to external packed V2 pos rpt format
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_vx_msg_xlate_v2_pos_rpt
(
  byte                        *output_buf_ptr,
  tm_vx_int_v2_pos_rpt_s_type *input_ptr
)
{
  int  i;    
  word buffer_pos = 0;

  if((output_buf_ptr == NULL) || (input_ptr == NULL))
  { 
    MSG_ERROR("Null input pointers", 0, 0, 0);
    return;
  }

  //Version
  b_packb( input_ptr->version, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, version) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, version);

  //Msg type
  b_packb( input_ptr->message_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, message_type) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, message_type);

  //Length
  b_packw( input_ptr->length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, length) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, length);
  
  // fix num
  b_packw( input_ptr->fix_num, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, fix_num) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, fix_num);
    
  //cor id
  b_packb( input_ptr->cor_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, cor_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, cor_id);

  //msid
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, ms_id);

  // Time week
  b_packw( input_ptr->time_week_cell, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, time_week_cell) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, time_week_cell);
  
  for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ )
  {
    b_packb( input_ptr->time_secs_cell[i], output_buf_ptr, buffer_pos, FSIZ(gps_time_sec_type, time_sec) );
    buffer_pos += FSIZ(gps_time_sec_type, time_sec);
  }
    
  //sid
  b_packw( input_ptr->sid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type,sid) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, sid);

  //nid
  b_packw( input_ptr->nid, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type,nid) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, nid);

  //bsid
  b_packw( input_ptr->base_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_id);

  // base lat
  b_packd( input_ptr->base_lat, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_lat) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_lat);

  // base long
  b_packd( input_ptr->base_long, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_long) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, base_long);

  // ref pn
  b_packw( input_ptr->ref_pn, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, ref_pn) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, ref_pn);
    
  // band class
  b_packb( input_ptr->band_class, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, band_class) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, band_class);
    
  // cdma freq
  b_packw( input_ptr->cdma_freq, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, cdma_freq) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, cdma_freq);
    
  // pos incl
  b_packb( input_ptr->pos_incl, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, pos_incl) );
  buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, pos_incl);
    
  if ( input_ptr->pos_incl ) 
  {
    // Time week
    b_packw( input_ptr->time_week_pos, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, time_week_pos) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, time_week_pos);
    
    for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ )
    {
      b_packb( input_ptr->time_secs_pos[i], output_buf_ptr, buffer_pos, FSIZ(gps_time_sec_type, time_sec) );
      buffer_pos += FSIZ(gps_time_sec_type, time_sec);
    }
    // base lat
    b_packd( input_ptr->lat, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, lat) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, lat);

    // base long
    b_packd( input_ptr->lon, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, lon) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, lon);

    // loc ang
    b_packb( input_ptr->loc_ang, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_ang) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_ang);

    // loc a
    b_packb( input_ptr->loc_a, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_a) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_a);

    // loc p
    b_packb( input_ptr->loc_p, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_p) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_p);

    // velocity incl
    b_packb( input_ptr->velocity_incl, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, velocity_incl) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, velocity_incl);

    if (input_ptr->velocity_incl) 
    {
      // velocity h
      b_packw( input_ptr->velocity_h, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, velocity_h) );
      buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, velocity_h);

      // heading
      b_packw( input_ptr->heading, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, heading) );
      buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, heading);

      // vlocity_ver
      b_packb( input_ptr->velocity_v, output_buf_ptr, buffer_pos, FSIZ( tm_vx_ext_v2_pos_rpt_s_type, velocity_ver ));
      buffer_pos += FSIZ( tm_vx_ext_v2_pos_rpt_s_type, velocity_ver );
    }
    
    // height incl
    b_packb( input_ptr->height_incl, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, height_incl) );
    buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, height_incl);
    
    if (input_ptr->height_incl) 
    {
      // height 
      b_packw( input_ptr->height, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, height) );
      buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, height);
      
      // Loc v
      b_packb( input_ptr->loc_v, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_v) );
      buffer_pos += FSIZ(tm_vx_ext_v2_pos_rpt_s_type, loc_v);
    }
  } /* pos_incl */
  
  return;
} /* end paix_xlate_ext_sppreq_req_v2() */

/*===========================================================================

FUNCTION tm_vx_msg_xlate_v2_mo_cancel

DESCRIPTION
  This function converst internal V2 MO Cancel to external packed V2 Cancel format
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_vx_msg_xlate_v2_mo_cancel
(
  byte                                *output_buf_ptr,
  const tm_vx_int_v2_mo_cancel_s_type *input_ptr
)
{
  int  i = 0;
  word buffer_pos = 0;

  if((output_buf_ptr == NULL) || (input_ptr == NULL))
  { 
    MSG_ERROR("Null input pointers", 0, 0, 0);
    return;
  }

  //Version
  b_packb( input_ptr->version, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, version) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, version);

  //Msg type
  b_packb( input_ptr->message_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, message_type) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, message_type);

  //Length
  b_packw( input_ptr->length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, length) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, length);

#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF

  (void)i;

  //corr id
  b_packb( input_ptr->correlation_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, correlation_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, correlation_id);

  // msid
  buffer_pos +=  FSIZ(tm_vx_ext_v2_mo_cancel_s_type, ms_id);
  
#else

  //app type
  b_packb( input_ptr->app_type, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, app_type) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, app_type);

  //msid
  buffer_pos +=  FSIZ(tm_vx_ext_v2_mo_cancel_s_type, ms_id);

  //brew app
  b_packd( input_ptr->brew_id, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_v2_mo_cancel_s_type, brew_id) );
  buffer_pos += FSIZ(tm_vx_ext_v2_mo_cancel_s_type, brew_id);
    
  //app record length
  b_packw( input_ptr->app_record.id_length, output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record, id_length) );
  buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id_length);

  //app record id
  for ( i = 0; (i < input_ptr->app_record.id_length) && (i <TM_VX_MAX_APP_INFO_LEN); i++ )
  {
    b_packb( input_ptr->app_record.id[i], output_buf_ptr, buffer_pos, FSIZ(tm_vx_ext_ms_mpc_app_id_record, id) );
    buffer_pos += FSIZ(tm_vx_ext_ms_mpc_app_id_record, id);
  }
  
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */
}

/*===========================================================================

FUNCTION tm_vx_msg_translate_app_type

DESCRIPTION
  This function is return Vx app type based on input pdsm client type
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
tm_vx_app_e_type tm_vx_msg_translate_app_type(pdsm_client_type_e_type client_type)
{
  switch(client_type)
  {
    case PDSM_CLIENT_TYPE_BREW:
      return TM_VX_APP_TYPE_BREW;

    case PDSM_CLIENT_TYPE_BROWSER:
      return TM_VX_APP_TYPE_WAP;

    case PDSM_CLIENT_TYPE_JAVA:
      return TM_VX_APP_TYPE_JAVA;

    case PDSM_CLIENT_TYPE_NI:
      return TM_VX_APP_TYPE_NI;

    default:
      return TM_VX_APP_TYPE_MS_RESIDENT;
  }
}

/*===========================================================================

FUNCTION tm_vx_msg_create_v1_sppreq

DESCRIPTION
  This function is called by tm_vx to create V1 SPPReq
  
DEPENDENCIES
  None.

RETURN VALUE
  If V1 SPPReq is successfully created, return the size of the created message
  Otherwise, return -1.

SIDE EFFECTS
  None.

===========================================================================*/
int tm_vx_msg_create_v1_sppreq
(
  byte  *output_buf_ptr,
  uint8 size_of_output_buf
)
{
  tm_vx_int_v1_sppreq_s_type   *int_sppreq_buf_ptr = NULL;
  int                          active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  mcc_pd_bs_info_type          this_bs_info;  
  uint16                       gps_week;
  uint32                       gps_ms;
  uint32                       gps_sec;  
  int                          packed_msg_size;

  MSG_MED("Creaating internal V1 SPPReq", 0, 0, 0);
  
  if( (output_buf_ptr == NULL)
    ||((active_app_idx < 0) || (active_app_idx >= (int)TM_VX_APP_NUM)))
  {
    MSG_ERROR("Null output_buf_ptr or no active app", 0, 0, 0);  
    return -1;
  }

  int_sppreq_buf_ptr = (tm_vx_int_v1_sppreq_s_type *)pdsm_getbuf(sizeof(tm_vx_int_v1_sppreq_s_type));

  if(int_sppreq_buf_ptr == NULL)
  {
    MSG_ERROR("Can not get buffer", 0, 0, 0);
    return -1;
  }
  
  int_sppreq_buf_ptr->version  = tm_vx_gen_info.vx_config.vx_version;

  /* Calculate lenth field later */
  
  int_sppreq_buf_ptr->mspcap      = (uint8)TM_V1_MS_CAP_BOTH_MSA_MSB;

  int_sppreq_buf_ptr->app_type    = (uint8)tm_vx_msg_translate_app_type(tm_vx_gen_info.app_info[active_app_idx].app_type);

  /* No idea why consent_ind is needed in V1 SPPReq, so set it to 0 */
  int_sppreq_buf_ptr->consent_ind = 0;

  int_sppreq_buf_ptr->pqos        = tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_qos.gps_session_timeout;

  if(int_sppreq_buf_ptr->app_type == (uint8)TM_VX_APP_TYPE_BREW)
  {
    int_sppreq_buf_ptr->brew_id = tm_vx_gen_info.app_info[active_app_idx].brew_class_id;
  }  
  else
  {
    int_sppreq_buf_ptr->brew_id = 0;
  }
 
  memset(&this_bs_info, 0, sizeof(this_bs_info)); 

  /* Fill in SID/NID/CDMA info now */
  tm_vx_msg_get_bs_info(&this_bs_info);
  int_sppreq_buf_ptr->sid        = this_bs_info.sid;
  int_sppreq_buf_ptr->nid        = this_bs_info.nid;
  int_sppreq_buf_ptr->base_id    = this_bs_info.base_id;
  int_sppreq_buf_ptr->base_lat   = this_bs_info.base_lat;
  int_sppreq_buf_ptr->base_long  = this_bs_info.base_long;    

  tm_vx_gps_week_ms(this_bs_info.ts, &gps_week, &gps_ms);
  int_sppreq_buf_ptr->time_week = gps_week;
  gps_sec = gps_ms / 1000;
  int_sppreq_buf_ptr->time_sec[2] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_sppreq_buf_ptr->time_sec[1] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_sppreq_buf_ptr->time_sec[0] = ((uint8) gps_sec) & 0xff;  
    
  if( (int_sppreq_buf_ptr->app_type != (uint8)TM_VX_APP_TYPE_BREW) 
    &&(int_sppreq_buf_ptr->app_type != (uint8)TM_VX_APP_TYPE_NI)
    &&(tm_vx_gen_info.app_info[active_app_idx].app_info_length > 0))
  {
    int_sppreq_buf_ptr->app_record.id_length = memscpy((void *)(int_sppreq_buf_ptr->app_record.id), TM_VX_MAX_APP_INFO_LEN,
		   (void *)(tm_vx_gen_info.app_info[active_app_idx].app_info),
		   tm_vx_gen_info.app_info[active_app_idx].app_info_length);
  }
  else
  {
    int_sppreq_buf_ptr->app_record.id_length = 0;
  }

  /* Calculate length field now, length includes version and length fields */
  int_sppreq_buf_ptr->length = TM_V1_SPPREQ_FIX_SIZE + int_sppreq_buf_ptr->app_record.id_length;

  /* Now do packing to external format */
  if(size_of_output_buf >= int_sppreq_buf_ptr->length)
  {
    tm_vx_msg_xlate_v1_sppreq(output_buf_ptr, int_sppreq_buf_ptr);

    /* Now initialize MS ID field, MS ID field is 8 bytes from the end of the message */
    (void)tm_1x_up_create_custom_header(output_buf_ptr + (int_sppreq_buf_ptr->length - 8));
  
    packed_msg_size = int_sppreq_buf_ptr->length;
  }
  else
  {
    packed_msg_size = -1;
  }
  
  (void)pdsm_freebuf((char *)int_sppreq_buf_ptr);

  return packed_msg_size;
}

/*===========================================================================

FUNCTION tm_vx_msg_decode_v1_sppresp

DESCRIPTION
  This function is called by tm_vx to decode incoming V1 SPPResp
  Result is saved in tm_vx_gen_info.sess_info.v1_sppresp.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 SPPResp is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_v1_sppresp
(
  byte *input_buf_ptr,
  uint16 msg_len
)
{
  tm_vx_int_v1_sppresp_s_type *int_sppresp_ptr = &(tm_vx_gen_info.sess_info.v1_sppresp);
  word                        msg_pos = 0;
  int                         i;  
  boolean                     ret_val = TRUE;
  uint16                      min_msg_len = 0;

  MSG_MED("Decoding V1 SPPResp", 0, 0, 0);
  
  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }

  min_msg_len = FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_status_ind ) +
    FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_ip_adrs ) +
    FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_port_number ) +
    FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_incl );
  if (msg_len*8 < min_msg_len)
  {
    MSG_ERROR("V1 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
    return FALSE;
  }
  
  int_sppresp_ptr->pos_status_ind =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_status_ind ));
  msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_status_ind );
  
  int_sppresp_ptr->pde_ip_adrs = 
    b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_ip_adrs ));
  msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_ip_adrs );

  int_sppresp_ptr->pde_port_number = 
    b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_port_number ));
  msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, pde_port_number );

  int_sppresp_ptr->pos_incl = 
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_incl ));
  msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, pos_incl );
  
  if (int_sppresp_ptr->pos_incl != 0) 
  {
    min_msg_len += FSIZ( tm_vx_ext_v1_sppresp_s_type, time_week ) +
      FSIZ( gps_time_sec_type, time_sec ) * TM_VX_SIZE_OF_TIME_SEC +
      FSIZ( tm_vx_ext_v1_sppresp_s_type, lat ) + 
      FSIZ( tm_vx_ext_v1_sppresp_s_type, lon ) +
      FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_ang ) + 
      FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_a ) + 
      FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_p ) +
      FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_incl ) + 
      FSIZ( tm_vx_ext_v1_sppresp_s_type, height_incl );
    if (msg_len*8 < min_msg_len)
    {
      MSG_ERROR("V1 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
      return FALSE;
    }

    int_sppresp_ptr->time_week = 
       b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, time_week ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, time_week );

    for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ ) 
    {
      int_sppresp_ptr->time_sec[i] =
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( gps_time_sec_type, time_sec ));
      msg_pos += FSIZ( gps_time_sec_type, time_sec );
    }

    int_sppresp_ptr->lat = 
      b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, lat ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, lat );
  
    sign_extend_d((dword*)&(int_sppresp_ptr->lat), FSIZ( tm_vx_ext_v1_sppresp_s_type, lat));

    int_sppresp_ptr->lon = 
      b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, lon ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, lon );

    sign_extend_d((dword*)&(int_sppresp_ptr->lon),FSIZ( tm_vx_ext_v1_sppresp_s_type, lon));

    int_sppresp_ptr->loc_unc_ang = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_ang ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_ang );

    int_sppresp_ptr->loc_unc_a = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_a ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_a );
  
    int_sppresp_ptr->loc_unc_p = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_p ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_p );
  
    int_sppresp_ptr->velocity_incl = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_incl ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_incl );
  
    if (TRUE == int_sppresp_ptr->velocity_incl) 
    {
      min_msg_len += FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_hor ) +
        FSIZ( tm_vx_ext_v1_sppresp_s_type, heading ) +
        FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_ver );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V1 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_sppresp_ptr->velocity_hor = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_hor ));
      msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_hor );

      int_sppresp_ptr->heading = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, heading ));
      msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, heading );
      
      int_sppresp_ptr->velocity_ver = 
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_ver ));
      msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_ver );
  
      sign_extend_b((byte*)&(int_sppresp_ptr->velocity_ver), 
                 FSIZ( tm_vx_ext_v1_sppresp_s_type, velocity_ver));
    } /* end velocity_incl */
  
    int_sppresp_ptr->height_incl = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, height_incl ));
    msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, height_incl );

    if (TRUE == int_sppresp_ptr->height_incl) 
    {
      min_msg_len += FSIZ( tm_vx_ext_v1_sppresp_s_type, height ) +
        FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_v );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V1 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_sppresp_ptr->height = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, height ));
      msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, height );

      sign_extend_w((word*)&(int_sppresp_ptr->height), 
                    FSIZ( tm_vx_ext_v1_sppresp_s_type, height));

      int_sppresp_ptr->loc_unc_v = 
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_v ));
      msg_pos += FSIZ( tm_vx_ext_v1_sppresp_s_type, loc_unc_v );
    }
  } /* pos included */    

  return ret_val;
}

/*===========================================================================

FUNCTION tm_vx_msg_decode_incoming_v1_mt_sms

DESCRIPTION
  This function is called by tm_vx to decode incoming V1 MT SMS.
  Result is saved in tm_vx_gen_info.sess_info.incoming_v1_mt_sms.

  The incoming V1 MT SMS needs to be validated before it can be copied
  to tm_vx_gen_info.sess_info.v1_mt_sms, which the tm-vx module uses to execute
  v1 call flows.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V1 MT SMS is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_incoming_v1_mt_sms
(
  byte   *input_buf_ptr,
  uint16 data_length
)
{
  uint32                      pde_id;
  int                         i = 0;
  uint16                      min_msg_len = TM_V1_SMS_MIN_MSG_SIZE;
  
  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }

  if (data_length < min_msg_len)
  {
    MSG_ERROR("Msg Len too small: %d", data_length, 0, 0);
    return FALSE;
  }
  
  /* Version */
  if( *input_buf_ptr == (byte)TM_VX_VERSION_V1_ONLY)
  {
    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.version           = input_buf_ptr[i++];
    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.notification_ind  = input_buf_ptr[i++];
    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind = input_buf_ptr[i++];
    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.correlation_id    = input_buf_ptr[i++];

    pde_id = input_buf_ptr[i++];
    pde_id = (pde_id << 8) | input_buf_ptr[i++];
    pde_id = (pde_id << 8) | input_buf_ptr[i++];
    pde_id = (pde_id << 8) | input_buf_ptr[i++];

    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.pde_id = pde_id;

    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.pde_port = (input_buf_ptr[i++] << 8);

    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.pde_port |= input_buf_ptr[i++];

    tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id_len = data_length - i;

    if (tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id_len >= TM_VX_MAX_REQUESTER_ID_LEN) 
    {
      tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id_len = TM_VX_MAX_REQUESTER_ID_LEN;
    }

    min_msg_len += tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id_len;
    if (data_length < min_msg_len)
    {
      MSG_ERROR("Msg Len too small: %d", data_length, 0, 0);
      return FALSE;
    }

    memscpy( tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id, TM_VX_MAX_REQUESTER_ID_LEN,
            (void*)&(input_buf_ptr[i]),
            tm_vx_gen_info.sess_info.incoming_v1_mt_sms.requester_id_len);

    return TRUE;
  }
  else
  {
    MSG_ERROR("Version is invalid %d", *input_buf_ptr, 0, 0);
    return FALSE;  
  }
  /* MT SMS message type */
}

/*===========================================================================

FUNCTION tm_vx_msg_create_v2_sppreq

DESCRIPTION
  This function is called by tm_vx to create V2 SPPReq
  
DEPENDENCIES
  None.

RETURN VALUE
  If V2 SPPReq is successfully created, return the size of the created message
  Otherwise, return -1.
  
SIDE EFFECTS
  None.

===========================================================================*/
int tm_vx_msg_create_v2_sppreq
(
  byte  *output_buf_ptr,
  uint8 size_of_output_buf
)
{
  tm_vx_int_v2_sppreq_s_type   *int_sppreq_buf_ptr = NULL;
  uint32                       tracking_duration;
  int                          active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  mcc_pd_bs_info_type          this_bs_info;
  uint16                       gps_week;
  uint32                       gps_ms;
  uint32                       gps_sec;  
  int                          packed_msg_size;  

  MSG_MED("Creaating internal V2 SPPReq", 0, 0, 0);
  
  if( (output_buf_ptr == NULL)
    ||((active_app_idx < 0) || (active_app_idx >= (int)TM_VX_APP_NUM)))
  {
    MSG_ERROR("Null output_buf_ptr or no active app", 0, 0, 0);  
    return -1;
  }

  int_sppreq_buf_ptr = (tm_vx_int_v2_sppreq_s_type *)pdsm_getbuf(sizeof(tm_vx_int_v2_sppreq_s_type));

  if(int_sppreq_buf_ptr == NULL)
  {
    MSG_ERROR("Can not get buffer", 0, 0, 0);
    return -1;
  }

  /* Now fill information in internal sppreq buffer for packing routine (to exernal format) */
  int_sppreq_buf_ptr->version  = tm_vx_gen_info.vx_config.vx_version;
  int_sppreq_buf_ptr->msg_type = (uint8)TM_VX_MS_MPC_MSG_SPPREQ;

  /* Fill in length field after */
  
  int_sppreq_buf_ptr->mspcap   = (uint8)TM_V2_MS_CAP_BOTH_MSA_MSB;
  int_sppreq_buf_ptr->app_type = (uint8)tm_vx_msg_translate_app_type(tm_vx_gen_info.app_info[active_app_idx].app_type);
  int_sppreq_buf_ptr->pqos     = tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_qos.gps_session_timeout;

  if(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.fix_rate.time_between_fixes_ms <= TM_VX_TRACKING_MIN_TBF_MS)
  {
    tracking_duration =  tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.fix_rate.num_fixes * TM_VX_TRACKING_MIN_TBF_MS;
  }
  else
  {
    tracking_duration = tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.fix_rate.time_between_fixes_ms
                        *(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.fix_rate.num_fixes - 1)
                        + TM_VX_TRACKING_MIN_TBF_MS;
  }

  /* convert duration to seconds; round up */
  tracking_duration = (tracking_duration + 999) / 1000;

  /* Only set duration to non-zero value for tracking session */
  if(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session == PDSM_PD_SESS_TYPE_TRACK_IND) 
  {
    int_sppreq_buf_ptr->duration = (tracking_duration < tm_vx_gen_info.vx_config.vx_mo_max_duration) ? tracking_duration : tm_vx_gen_info.vx_config.vx_mo_max_duration;
  }
  else
  {
    int_sppreq_buf_ptr->duration = 0;
  }

  tm_vx_gen_info.app_info[active_app_idx].auth_duration = int_sppreq_buf_ptr->duration;

  int_sppreq_buf_ptr->cor_id = tm_vx_gen_info.sess_info.correlation_id;

  /* Fill in ms id record at the very end */
  memset(&this_bs_info, 0, sizeof(this_bs_info));
  /* Fill in SID/NID/CDMA info now */
  tm_vx_msg_get_bs_info(&this_bs_info);
  int_sppreq_buf_ptr->sid        = this_bs_info.sid;
  int_sppreq_buf_ptr->nid        = this_bs_info.nid;
  int_sppreq_buf_ptr->base_id    = this_bs_info.base_id;
  int_sppreq_buf_ptr->base_lat   = this_bs_info.base_lat;
  int_sppreq_buf_ptr->base_long  = this_bs_info.base_long;    
  int_sppreq_buf_ptr->ref_pn     = this_bs_info.pilot_pn;
  int_sppreq_buf_ptr->band_class = this_bs_info.band_class;
  int_sppreq_buf_ptr->cdma_freq  = this_bs_info.cdma_ch;  

  tm_vx_gps_week_ms(this_bs_info.ts, &gps_week, &gps_ms);
  int_sppreq_buf_ptr->time_week = gps_week;
  gps_sec = gps_ms / 1000;
  int_sppreq_buf_ptr->time_sec[2] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_sppreq_buf_ptr->time_sec[1] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_sppreq_buf_ptr->time_sec[0] = ((uint8) gps_sec) & 0xff;
  

  if(int_sppreq_buf_ptr->app_type == (uint8)TM_VX_APP_TYPE_BREW)
  {
    int_sppreq_buf_ptr->brew_id = tm_vx_gen_info.app_info[active_app_idx].brew_class_id;
  }  
  else
  {
    int_sppreq_buf_ptr->brew_id = 0;
  }
  
  if( (int_sppreq_buf_ptr->app_type != (uint8)TM_VX_APP_TYPE_BREW) 
    &&(int_sppreq_buf_ptr->app_type != (uint8)TM_VX_APP_TYPE_NI)
    &&(tm_vx_gen_info.app_info[active_app_idx].app_info_length > 0))
  {
    int_sppreq_buf_ptr->app_record.id_length = memscpy((void *)(int_sppreq_buf_ptr->app_record.id),
		TM_VX_MAX_APP_INFO_LEN,
		(void *)(tm_vx_gen_info.app_info[active_app_idx].app_info),
		tm_vx_gen_info.app_info[active_app_idx].app_info_length);
  }
  else
  {
    int_sppreq_buf_ptr->app_record.id_length = 0;
  }

  /* Calculate msg_length, length field does not include version, msg_type and length field (total 4 bytes) */
  int_sppreq_buf_ptr->length = TM_V2_SPPREQ_FIX_SIZE - 4 + int_sppreq_buf_ptr->app_record.id_length;

  /* Now do packing to external format */
  if(size_of_output_buf >= (TM_V2_SPPREQ_FIX_SIZE + int_sppreq_buf_ptr->app_record.id_length))
  {
    tm_vx_msg_xlate_v2_sppreq(output_buf_ptr, int_sppreq_buf_ptr);

    /* Now initialize MS ID field, MS ID field is 12 bytes from the start */
    (void)tm_1x_up_create_custom_header(output_buf_ptr + 12);
  
    packed_msg_size = TM_V2_SPPREQ_FIX_SIZE + int_sppreq_buf_ptr->app_record.id_length;
  }
  else
  {
    packed_msg_size = -1;
  }
  
  (void)pdsm_freebuf((char *)int_sppreq_buf_ptr);

  return packed_msg_size;
}

/*===========================================================================

FUNCTION tm_vx_msg_create_v2_pos_rpt

DESCRIPTION
  This function is called by tm_vx to create V2 Pos Report
  
DEPENDENCIES
  None.

RETURN VALUE
  If V2 Pos Rpt is successfully created, return the size of the created message
  Otherwise, return -1.
  
SIDE EFFECTS
  None.

===========================================================================*/
int tm_vx_msg_create_v2_pos_rpt
(
  byte   *output_buf_ptr,
  uint8  size_of_output_buf
)
{
  tm_vx_int_v2_pos_rpt_s_type  *int_pos_rpt_buf_ptr = NULL;
  mcc_pd_bs_info_type          this_bs_info;  
  uint16                       gps_week;
  uint32                       gps_ms;
  uint32                       gps_sec;  
  word                         total_msg_len_in_bits;
  uint8                        total_msg_len_in_bytes;
  int                          packed_msg_size;
  
  if(output_buf_ptr == NULL)
  {
    MSG_ERROR("Null output_buf_ptr", 0, 0, 0);  
    return -1;
  }

  int_pos_rpt_buf_ptr = (tm_vx_int_v2_pos_rpt_s_type *)pdsm_getbuf(sizeof(tm_vx_int_v2_pos_rpt_s_type));

  if(int_pos_rpt_buf_ptr == NULL)
  {
    MSG_ERROR("Can not get buffer", 0, 0, 0);
    return -1;
  }

  /* Now fill information in internal position report buffer for packing routine (to exernal format) */
  int_pos_rpt_buf_ptr->version  = tm_vx_gen_info.vx_config.vx_version;
  int_pos_rpt_buf_ptr->message_type = (uint8)TM_VX_MS_MPC_MSG_POS_RPT;

  /* Fill in length field after */

  int_pos_rpt_buf_ptr->fix_num = tm_vx_ni_session_info.current_fix_count;
  int_pos_rpt_buf_ptr->cor_id  = tm_vx_gen_info.sess_info.correlation_id;
  
  /* Fill in ms id record at the very end */

  memset(&this_bs_info, 0, sizeof(this_bs_info));
  tm_vx_gps_week_ms(this_bs_info.ts, &gps_week, &gps_ms);
  int_pos_rpt_buf_ptr->time_week_cell = gps_week;
  gps_sec = gps_ms/1000;
  int_pos_rpt_buf_ptr->time_secs_cell[2] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_pos_rpt_buf_ptr->time_secs_cell[1] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  int_pos_rpt_buf_ptr->time_secs_cell[0] = ((uint8) gps_sec) & 0xff;
          
  tm_vx_msg_get_bs_info(&this_bs_info); 
  int_pos_rpt_buf_ptr->sid        = this_bs_info.sid;
  int_pos_rpt_buf_ptr->nid        = this_bs_info.nid;
  int_pos_rpt_buf_ptr->base_id    = this_bs_info.base_id;
  int_pos_rpt_buf_ptr->base_lat   = this_bs_info.base_lat;
  int_pos_rpt_buf_ptr->base_long  = this_bs_info.base_long;    
  int_pos_rpt_buf_ptr->ref_pn     = this_bs_info.pilot_pn;
  int_pos_rpt_buf_ptr->band_class = this_bs_info.band_class;
  int_pos_rpt_buf_ptr->cdma_freq  = this_bs_info.cdma_ch;  


  if(  (tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lat != 0) 
     ||(tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lon != 0)) 
  {
    tm_vx_gps_week_ms(tm_vx_gen_info.sess_info.lr_time, &gps_week, &gps_ms);
    int_pos_rpt_buf_ptr->time_week_pos = gps_week;
    gps_sec = gps_ms/1000;
    int_pos_rpt_buf_ptr->time_secs_pos[2] = ((uint8) gps_sec) & 0xff;
    gps_sec >>= 8;
    int_pos_rpt_buf_ptr->time_secs_pos[1] = ((uint8) gps_sec) & 0xff;
    gps_sec >>= 8;
    int_pos_rpt_buf_ptr->time_secs_pos[0] = ((uint8) gps_sec) & 0xff;
            
    int_pos_rpt_buf_ptr->pos_incl = 1;
    int_pos_rpt_buf_ptr->lat      = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lat;
    int_pos_rpt_buf_ptr->lon      = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lon;
    int_pos_rpt_buf_ptr->loc_ang  = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.loc_uncertainty_ang;
    int_pos_rpt_buf_ptr->loc_a    = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.loc_uncertainty_a;
    int_pos_rpt_buf_ptr->loc_p    = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.loc_uncertainty_p;
            
    if (tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.opt_field_mask & PDSM_PD_VELOCITY_VALID) 
    {
      int_pos_rpt_buf_ptr->velocity_incl = 1;
      int_pos_rpt_buf_ptr->velocity_h = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.velocity_hor;
      int_pos_rpt_buf_ptr->heading    = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.heading;

      if(tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.fix_type & 0x1) 
      {
        int_pos_rpt_buf_ptr->velocity_v = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.velocity_ver;
      }
      else
      {
        int_pos_rpt_buf_ptr->velocity_v = 0;
      }
    }
            
    if(tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.opt_field_mask & PDSM_PD_ALTITUDE_VALID)
    {
      int_pos_rpt_buf_ptr->height_incl = 1;
      int_pos_rpt_buf_ptr->height = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.altitude;
      int_pos_rpt_buf_ptr->loc_v  = tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.loc_uncertainty_v;
    }
    else
    {
      int_pos_rpt_buf_ptr->height_incl = 0;
    }
  }
  else
  {
    MSG_HIGH("Don't include position",0,0,0);
    int_pos_rpt_buf_ptr->pos_incl = 0;
  }

  /* Calculate entire msg_length */
  total_msg_len_in_bits = TM_V2_POS_RPT_FIX_SIZE_IN_BIT;
  total_msg_len_in_bits += (int_pos_rpt_buf_ptr->pos_incl == 1) ? TM_V2_POS_RPT_VAR_P_SIZE_IN_BIT : 0;
  total_msg_len_in_bits += (int_pos_rpt_buf_ptr->velocity_incl == 1) ? TM_V2_POS_RPT_VAR_V_SIZE_IN_BIT : 0;
  total_msg_len_in_bits += (int_pos_rpt_buf_ptr->height_incl == 1) ? TM_V2_POS_RPT_VAR_H_SIZE_IN_BIT : 0;

  total_msg_len_in_bytes = (total_msg_len_in_bits + 7 ) / 8;
  
  /* 
    Calculate the value in the length field 
    Notice that it does not include version, msg_type and length field (total 4 bytes) 
  */
  int_pos_rpt_buf_ptr->length = total_msg_len_in_bytes - 4;
  
  /* Now do packing to external format */
  if(size_of_output_buf >= total_msg_len_in_bytes)
  {
    tm_vx_msg_xlate_v2_pos_rpt(output_buf_ptr, int_pos_rpt_buf_ptr);

    /* Now initialize MS ID field, MS ID field is 7 bytes from the start */
    (void)tm_1x_up_create_custom_header(output_buf_ptr + 7);
  
    packed_msg_size = total_msg_len_in_bytes;
  }
  else
  {
    packed_msg_size = -1;
  }
  
  (void)pdsm_freebuf((char *)int_pos_rpt_buf_ptr);

  return packed_msg_size;
}

/*===========================================================================

FUNCTION tm_vx_msg_create_v2_mo_cancel

DESCRIPTION
  This function is called by tm_vx to create V2 mo cancel message
  
DEPENDENCIES
  None.

RETURN VALUE
  If V2 mo cancel is successfully created, return the size of the created message
  Otherwise, return -1.
  
SIDE EFFECTS
  None.

===========================================================================*/
int tm_vx_msg_create_v2_mo_cancel
(
  byte   *output_buf_ptr,
  uint8  size_of_output_buf
)
{
  tm_vx_int_v2_mo_cancel_s_type  *int_mo_cancel_buf_ptr = NULL;
  uint16                         app_info_length = 0;  
  int                            active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  int                            packed_msg_size = -1;  

  
  if(output_buf_ptr == NULL)
  {
    MSG_ERROR("Null output_buf_ptr", 0, 0, 0);  
    return -1;
  }

  int_mo_cancel_buf_ptr = (tm_vx_int_v2_mo_cancel_s_type *)pdsm_getbuf(sizeof(tm_vx_int_v2_mo_cancel_s_type));

  if(int_mo_cancel_buf_ptr == NULL)
  {
    MSG_ERROR("Can not get buffer", 0, 0, 0);
    return -1;
  }

  if((active_app_idx > 0) && (active_app_idx < (int)TM_VX_APP_NUM))
  {
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF

   (void)app_info_length;

    /* Now fill information in internal position report buffer for packing routine (to exernal format) */
    int_mo_cancel_buf_ptr->version        = tm_vx_gen_info.vx_config.vx_version;
    int_mo_cancel_buf_ptr->message_type   = (uint8)TM_VX_MS_MPC_MSG_CANCEL;
    int_mo_cancel_buf_ptr->length         = TM_V2_MO_CANCEL_FIX_SIZE - 4;
    int_mo_cancel_buf_ptr->correlation_id = tm_vx_gen_info.sess_info.correlation_id;

    /* Fill in ms id record at the very end */
    
#else
  
    int_mo_cancel_buf_ptr->version        = tm_vx_gen_info.vx_config.vx_version;
    int_mo_cancel_buf_ptr->message_type   = (uint8)TM_VX_MS_MPC_MSG_CANCEL;

    /* Fill in length later */
    
    int_mo_cancel_buf_ptr->app_type = (uint8)tm_vx_msg_translate_app_type(tm_vx_gen_info.app_info[active_app_idx].app_type);

    /* Fill in ms id record at the very end */
   

    /* Fill in BREW class ID and App info record now */
    if(int_mo_cancel_buf_ptr->app_type.app_id == TM_VX_APP_TYPE_BREW) 
    {
      int_mo_cancel_buf_ptr->brew_id = tm_vx_gen_info.app_info[active_app_idx].brew_class_id;
      int_mo_cancel_buf_ptr->app_record.id_length = 0;
    }
    else
    {
      int_mo_cancel_buf_ptr->brew_id = 0;

      if( (int_mo_cancel_buf_ptr->app_type != TM_VX_APP_TYPE_NI)
        &&(tm_vx_gen_info.app_info[active_app_idx].app_info_length != 0))
      {
         int_mo_cancel_buf_ptr->app_record.id_length = memscpy((void *)&(int_mo_cancel_buf_ptr->app_record.id),
		 	TM_VX_MAX_APP_INFO_LEN,
		 	(void *)&(tm_vx_gen_info.app_info[active_app_idx].app_info),
		 	tm_vx_gen_info.app_info[active_app_idx].app_info_length);
      }
      else
      {
        int_mo_cancel_buf_ptr->app_record.id_length = 0;      
      }
    }

    /* Now calculate length */
    int_mo_cancel_buf_ptr->length = TM_V2_MO_CANCEL_FIX_SIZE - 4 + int_mo_cancel_buf_ptr->app_record.id_length;
    
#endif /* Handoff */

    /* Now do packing to external format */
    if(size_of_output_buf >= (int_mo_cancel_buf_ptr->length + 4))
    {
      tm_vx_msg_xlate_v2_mo_cancel(output_buf_ptr, int_mo_cancel_buf_ptr);

      /* Now initialize MS ID field, MS ID field is 5 bytes from the start */
      (void)tm_1x_up_create_custom_header(output_buf_ptr + 5);
  
      packed_msg_size = int_mo_cancel_buf_ptr->length + 4;
    }
    else
    {
      packed_msg_size = -1;
    }
    
  }

  (void)pdsm_freebuf((char *)int_mo_cancel_buf_ptr);

  return packed_msg_size;  
}

/*===========================================================================

FUNCTION tm_vx_msg_decode_v2_url

DESCRIPTION
  This function is called to decode v2 URL address (SPPResp or Pos Ack)
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if URL is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_v2_url
(
  byte *buf_ptr,
  uint16 msg_len,
  void *int_ptr,
  word *msg_pos,
  tm_vx_msg_url_msg_type_e_type msg_type
)
{
  tm_vx_msg_url_msg_ptr_s_type dst_msg;
  int  i = 0;

  MSG_MED("Decoding V2 URL from meg type %d", msg_type, 0, 0);
  
  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_pos == NULL))
  {
    MSG_ERROR("Null ptr received", 0, 0, 0);  
    return FALSE;
  }

  if(msg_type == TM_VX_MSG_URL_MSG_TYPE_SPPRESP)
  {
    dst_msg.adrs_length     = &((tm_vx_int_v2_sppresp_s_type *)int_ptr)->adrs_length;
    dst_msg.adrs_encoding   = &((tm_vx_int_v2_sppresp_s_type *)int_ptr)->adrs_encoding;
    dst_msg.pde_ip_adrs     = &((tm_vx_int_v2_sppresp_s_type *)int_ptr)->pde_ip_adrs;
    dst_msg.pde_port_number = &((tm_vx_int_v2_sppresp_s_type *)int_ptr)->pde_port_number;   
    dst_msg.url_adrs        = ((tm_vx_int_v2_sppresp_s_type *)int_ptr)->url_adrs;
  }
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF  
  else if (msg_type == TM_VX_MSG_URL_MSG_TYPE_POSACK)
  {
    dst_msg.adrs_length     = &((tm_vx_int_v2_pos_rpt_resp_s_type *)int_ptr)->adrs_length;
    dst_msg.adrs_encoding   = &((tm_vx_int_v2_pos_rpt_resp_s_type *)int_ptr)->adrs_encoding;
    dst_msg.pde_ip_adrs     = &((tm_vx_int_v2_pos_rpt_resp_s_type *)int_ptr)->pde_ip_adrs;
    dst_msg.pde_port_number = &((tm_vx_int_v2_pos_rpt_resp_s_type *)int_ptr)->pde_port_number;   
    dst_msg.url_adrs        = ((tm_vx_int_v2_pos_rpt_resp_s_type *)int_ptr)->url_adrs;
  }
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */  
  else
  {
    MSG_ERROR("invalid msg", 0 ,0 ,0);
    return FALSE;
  }

  /* Validate that the buffer has enough date before reading */
  if ((*(dst_msg.adrs_length) > TM_VX_MAX_URL_LENTH) || (*msg_pos/8 + *(dst_msg.adrs_length) > msg_len))
  {
    MSG_ERROR("Invalid msg length: %d", dst_msg.adrs_length ,0 ,0);
    return FALSE;
  }
  
  memset(dst_msg.url_adrs, 0, TM_VX_MAX_URL_LENTH);
  
  /*
  adrs_length also includes adrs_encoding(1 byte)
  url format is "www.yahoo.com:4911", 
  now url_adrs is only set to 32. Pay attention to buffer overflow 
  */
  for(i = 0; i < *(dst_msg.adrs_length) - 1; i++)
  {
    if(i < TM_VX_MAX_URL_LENTH)
    {
      dst_msg.url_adrs[i] = b_unpackb( buf_ptr, *msg_pos, 8);
      *msg_pos += 8;
    }
    else
    {
      MSG_ERROR("URL address length exceeds maxium length", 0 ,0 ,0);
      *msg_pos += ((*(dst_msg.adrs_length) - 1) - i) * 8;
      *(dst_msg.adrs_encoding) = 0;
      *(dst_msg.pde_ip_adrs) = 0;
      *(dst_msg.pde_port_number) = 0;
      *(dst_msg.adrs_length) = 0;
      return FALSE;
    }   
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_vx_msg_decode_v2_sppresp

DESCRIPTION
  This function is called by tm_vx to decode incoming V2 SPPResp
  Result is saved in tm_vx_gen_info.sess_info.v2_sppresp.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 SPPResp is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_v2_sppresp
(
  byte *input_buf_ptr,
  uint16 msg_len
)
{
  tm_vx_int_v2_sppresp_s_type *int_sppresp_ptr = &(tm_vx_gen_info.sess_info.v2_sppresp);
  word                        msg_pos = 0;
  int                         i;  
  boolean                     ret_val = TRUE;
  uint16                      min_msg_len = 0;

  MSG_MED("Decoding V2 SPPResp", 0, 0, 0);
  
  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }

  min_msg_len = FSIZ( tm_vx_ext_v2_sppresp_s_type, correlation_id ) +
    FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_status_ind ); // Min length should be 2

  if (msg_len*8 < min_msg_len)
  {
    MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
    return FALSE;
  }
  
  int_sppresp_ptr->cor_id =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, correlation_id ));
  msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, correlation_id );
  
  int_sppresp_ptr->pos_status_ind =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_status_ind ));
  msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_status_ind );
  

  if(msg_len*8 ==  min_msg_len)
  { 
     return TRUE;
  }
  else
  {
     min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, adrs_length ) +
     FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_incl );
  }

  if (msg_len*8 < min_msg_len)
  {
    MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
    return FALSE;
  } 

  int_sppresp_ptr->adrs_length = 
    b_unpackw( input_buf_ptr, msg_pos, (word) FSIZ( tm_vx_ext_v2_sppresp_s_type, adrs_length ));
  msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, adrs_length );
  
  if (int_sppresp_ptr->adrs_length) 
  {
    min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_adrs_encoding );
    if (msg_len*8 < min_msg_len)
    {
      MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
      return FALSE;
    }

    int_sppresp_ptr->adrs_encoding = 
      b_unpackw( input_buf_ptr, msg_pos, (word) FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_adrs_encoding ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_adrs_encoding );
  
    if(int_sppresp_ptr->adrs_encoding == 1)
    {
       min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_ip_adrs ) +
        FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_port_number );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_sppresp_ptr->pde_ip_adrs = b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_ip_adrs ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_ip_adrs );  

      int_sppresp_ptr->pde_port_number = b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_port_number ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, pde_port_number );
    }
    /* MPC sends URL */
    else
    {
      ret_val = tm_vx_msg_decode_v2_url(input_buf_ptr, msg_len, (void *)int_sppresp_ptr, &msg_pos, TM_VX_MSG_URL_MSG_TYPE_SPPRESP);
    }
  } /* end length */
  else
  {
    MSG_HIGH("No pde adrs, use 0,0",0,0,0);
    int_sppresp_ptr->adrs_encoding = 0;
    int_sppresp_ptr->pde_ip_adrs = 0;
    int_sppresp_ptr->pde_port_number = 0;
    int_sppresp_ptr->adrs_length = 0;
  }

  int_sppresp_ptr->pos_incl = 
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_incl ));
  msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, pos_incl );
  
  if (int_sppresp_ptr->pos_incl == 0x01) 
  {
    min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, time_week ) +
      FSIZ( gps_time_sec_type, time_sec ) * TM_VX_SIZE_OF_TIME_SEC +
      FSIZ( tm_vx_ext_v2_sppresp_s_type, lat ) + 
      FSIZ( tm_vx_ext_v2_sppresp_s_type, lon ) +
      FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_ang ) + 
      FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_a ) +
      FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_p ) + 
      FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_incl ) +
      FSIZ( tm_vx_ext_v2_sppresp_s_type, height_incl );
    if (msg_len*8 < min_msg_len)
    {
      MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
      return FALSE;
    }

    int_sppresp_ptr->time_week = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, time_week ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, time_week );
    
    for ( i = 0; i < TM_VX_SIZE_OF_TIME_SEC; i++ ) 
    {
      int_sppresp_ptr->time_sec[i] =
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( gps_time_sec_type, time_sec ));
      msg_pos += FSIZ( gps_time_sec_type, time_sec );
    }
    
    int_sppresp_ptr->lat = 
      b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, lat ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, lat );
      
    sign_extend_d((dword*)&(int_sppresp_ptr->lat), FSIZ( tm_vx_ext_v2_sppresp_s_type, lat)); /*lint -e718 */
    
    int_sppresp_ptr->lon = 
      b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, lon ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, lon );
    
    sign_extend_d((dword*)&(int_sppresp_ptr->lon),FSIZ( tm_vx_ext_v2_sppresp_s_type, lon));
    
    int_sppresp_ptr->loc_unc_ang = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_ang ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_ang );
    
    int_sppresp_ptr->loc_unc_a = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_a ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_a );
      
    int_sppresp_ptr->loc_unc_p = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_p ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_p );
      
    int_sppresp_ptr->velocity_incl = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_incl ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_incl );
      
    if (TRUE == int_sppresp_ptr->velocity_incl) /*lint !e731 */
    {
      min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_hor ) + 
        FSIZ( tm_vx_ext_v2_sppresp_s_type, heading ) +
        FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_ver );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_sppresp_ptr->velocity_hor = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_hor ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_hor );
    
      int_sppresp_ptr->heading = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, heading ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, heading );
          
      int_sppresp_ptr->velocity_ver = 
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_ver ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_ver );
      
      sign_extend_b((byte*)&(int_sppresp_ptr->velocity_ver), FSIZ( tm_vx_ext_v2_sppresp_s_type, velocity_ver)); /*lint -e718 */
    } /* end velocity_incl */
      
    int_sppresp_ptr->height_incl = 
      b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, height_incl ));
    msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, height_incl );
    
    if (TRUE == int_sppresp_ptr->height_incl) /*lint !e731 */
    {
      min_msg_len += FSIZ( tm_vx_ext_v2_sppresp_s_type, height ) +
        FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_v );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_sppresp_ptr->height = 
        b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, height ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, height );
    
      sign_extend_w((word*)&(int_sppresp_ptr->height), FSIZ( tm_vx_ext_v2_sppresp_s_type, height)); /*lint -e718 */
    
      int_sppresp_ptr->loc_unc_v = 
        b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_v ));
      msg_pos += FSIZ( tm_vx_ext_v2_sppresp_s_type, loc_unc_v );
    }
  } /* end pos_inc */

  return ret_val;
} /* tm_vx_msg_decode_v2_sppresp() */


#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
/*===========================================================================

FUNCTION tm_vx_msg_decode_v2_pos_rpt_resp

DESCRIPTION
  This function is called by tm_vx to decode incoming V2 Position Report Response
  Result is saved in tm_vx_gen_info.sess_info.v2_pos_rpt_resp.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 Pos Report Response is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_v2_pos_rpt_resp
(
  byte *input_buf_ptr,
  uint16 msg_len
)
{
  tm_vx_int_v2_pos_rpt_resp_s_type *int_pos_rpt_resp_ptr = &(tm_vx_gen_info.sess_info.v2_pos_rpt_resp);
  word                        msg_pos = 0;
  boolean                     ret_val = TRUE;
  uint16                      min_msg_len = 0;

  MSG_MED("Decoding V2 Pos Report Response", 0, 0, 0);
              
  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }

  min_msg_len = FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id ) +
    FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report );
  if (msg_len*8 < min_msg_len)
  {
    MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
    return FALSE;
  }
  
  int_pos_rpt_resp_ptr->cor_id =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id ));
  msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id );

  int_pos_rpt_resp_ptr->report =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report ));
  msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report );

  if(int_pos_rpt_resp_ptr->report == 2)
  {
    min_msg_len += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, adrs_length );
    if (msg_len*8 < min_msg_len)
    {
      MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
      return FALSE;
    }

    int_pos_rpt_resp_ptr->adrs_length = 
      b_unpackw( input_buf_ptr, msg_pos, (word) FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, adrs_length ));
    msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, adrs_length );

    if (int_pos_rpt_resp_ptr->adrs_length) 
    {
      min_msg_len += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_adrs_encoding );
      if (msg_len*8 < min_msg_len)
      {
        MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
        return FALSE;
      }

      int_pos_rpt_resp_ptr->adrs_encoding = 
        b_unpackw( input_buf_ptr, msg_pos, (word) FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_adrs_encoding ));
      msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_adrs_encoding );
  
      if(int_pos_rpt_resp_ptr->adrs_encoding == 1)
      {
        min_msg_len += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_ip_adrs ) +
          FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_port_number );
        if (msg_len*8 < min_msg_len)
        {
          MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
          return FALSE;
        }

        int_pos_rpt_resp_ptr->pde_ip_adrs = b_unpackd( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_ip_adrs ));
        msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_ip_adrs );  

        int_pos_rpt_resp_ptr->pde_port_number = b_unpackw( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_port_number ));
        msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, pde_port_number );
      }
      else
      {
        (void) tm_vx_msg_decode_v2_url(input_buf_ptr, msg_len, (void *)int_pos_rpt_resp_ptr, &msg_pos, TM_VX_MSG_URL_MSG_TYPE_POSACK);
      }
    } /* end length */
    else
    {
      MSG_MED("No pde adrs, use 0,0",0,0,0);
      int_pos_rpt_resp_ptr->adrs_encoding = 0;
      int_pos_rpt_resp_ptr->pde_ip_adrs = 0;
      int_pos_rpt_resp_ptr->pde_port_number = 0;
      int_pos_rpt_resp_ptr->adrs_length = 0;
      ret_val = FALSE;
    }
  }
  return ret_val;
}

#else

/*===========================================================================

FUNCTION tm_vx_msg_decode_v2_pos_rpt_resp

DESCRIPTION
  This function is called by tm_vx to decode incoming V2 Position Report Response
  Result is saved in tm_vx_gen_info.sess_info.v2_pos_rpt_resp.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 Pos Report Response is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_v2_pos_rpt_resp
(
  byte *input_buf_ptrr,
  uint16 msg_len
)
{
  tm_vx_int_v2_pos_rpt_resp_s_type *int_pos_rpt_resp_ptr = &(tm_vx_gen_info.sess_info.v2_pos_rpt_resp);
  word                        msg_pos = 0;
  uint16                      min_msg_len = 0;

  MSG_MED("Decoding V2 Pos Report Response", 0, 0, 0);
  
  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }

  min_msg_len = FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id ) +
    FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report );
  if (msg_len*8 < min_msg_len)
  {
    MSG_ERROR("V2 SPPResp msg len too small: %d (bits), required: %d", msg_len*8, min_msg_len, 0);
    return FALSE;
  }

  int_pos_rpt_resp_ptr->cor_id =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id ));
  msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, cor_id );

  int_pos_rpt_resp_ptr->report =
    b_unpackb( input_buf_ptr, msg_pos, FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report ));
  msg_pos += FSIZ( tm_vx_ext_v2_pos_rpt_resp_s_type, report );
  
  return TRUE;
}
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */

/*===========================================================================

FUNCTION tm_vx_msg_decode_incoming_v2_mt_sms

DESCRIPTION
  This function is called by tm_vx to decode incoming V2 MT SMS.
  Result is saved in tm_vx_gen_info.sess_info.incoming_v2_mt_sms.

  The incoming V2 MT SMS needs to be validated before it can be copied
  to tm_vx_gen_info.sess_info.v2_mt_sms, which the tm-vx module uses to execute
  v2 call flows.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 MT SMS is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_msg_decode_incoming_v2_mt_sms
(
  byte *input_buf_ptr,
  uint16 data_length
)
{
  uint16  min_msg_len = 0;

  if(input_buf_ptr == NULL)
  {
    MSG_ERROR("Null input_buf_ptr", 0, 0, 0);
    return FALSE;
  }
  
  if (data_length < min_msg_len)
  {
    MSG_ERROR("Msg Len too small: %d", data_length, 0, 0);
    return FALSE;
  }
  
  /* Version */
  if( *input_buf_ptr == (byte)TM_VX_VERSION_V2_ONLY)
  {
    min_msg_len+=3;
    if (data_length < min_msg_len)
    {
      MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
      return FALSE;
    }

    tm_vx_gen_info.sess_info.incoming_v2_mt_sms.version      = *(input_buf_ptr++);
    tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type = *(input_buf_ptr++);
    tm_vx_gen_info.sess_info.incoming_v2_mt_sms.length       = *(input_buf_ptr++);

    if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type == TM_V2_MT_SMS_MSG_TYPE_POS_REQUEST)
    {
      MSG_MED("Decoding Pos Req MT SMS", 0, 0, 0);
      min_msg_len+=11;
      if (data_length < min_msg_len)
      {
        MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
        return FALSE;
      }

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.notification_ind  = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.position_tech_ind = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.qos_incl          = *(input_buf_ptr++);
              
      if (tm_vx_gen_info.sess_info.incoming_v2_mt_sms.qos_incl) 
      {
        min_msg_len+=1;
        if (data_length < min_msg_len)
        {
          MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
          return FALSE;
        }

        tm_vx_gen_info.sess_info.incoming_v2_mt_sms.qos = *(input_buf_ptr++);;
      }
      else
      {
        MSG_MED("Using default QOS = %d", TM_VX_DEFAULT_QOS, 0, 0);
        tm_vx_gen_info.sess_info.incoming_v2_mt_sms.qos = TM_VX_DEFAULT_QOS; /* Use default QOS */
      }

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.num_fixes = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.num_fixes = (tm_vx_gen_info.sess_info.incoming_v2_mt_sms.num_fixes << 8) | *(input_buf_ptr++);

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.tbf       = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.tbf       = (tm_vx_gen_info.sess_info.incoming_v2_mt_sms.tbf << 8) | *(input_buf_ptr++);
      
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.is801_mode            = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id        = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_encoding = *(input_buf_ptr++);
      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_len      = *(input_buf_ptr++);

      if (tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_len >= TM_VX_MAX_REQUESTER_ID_LEN) 
      {
        tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_len = TM_VX_MAX_REQUESTER_ID_LEN;
      }

      min_msg_len+=tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_len;
      if (data_length < min_msg_len)
      {
        MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
        return FALSE;
      }

      memscpy( tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id, TM_VX_MAX_REQUESTER_ID_LEN,
              (void*)input_buf_ptr,
              tm_vx_gen_info.sess_info.incoming_v2_mt_sms.requester_id_len);

      return TRUE;
    }
    else if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type == TM_V2_MT_SMS_MSG_TYPE_NI_CANCEL)
    {
      min_msg_len+=1;
      if (data_length < min_msg_len)
      {
        MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
        return FALSE;
      }

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id = *(input_buf_ptr++);
      
      MSG_MED("Received NI Cancel MT SMS with correlation id = %d", tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id, 0, 0);
                 
      return TRUE;
    }
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
    else if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type == TM_V2_MT_SMS_MSG_TYPE_MO_CANCEL)
    {
      min_msg_len+=2;
      if (data_length < min_msg_len)
      {
        MSG_ERROR("Msg Len too small: %d, required: %d", data_length, min_msg_len, 0);
        return FALSE;
      }

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id = *(input_buf_ptr++);

      tm_vx_gen_info.sess_info.incoming_v2_mt_sms.cause_code = *(input_buf_ptr++);
      
      MSG_MED("Received MO Cancel MT SMS with correlation id = %d, cause code = %d",
               tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id,
               tm_vx_gen_info.sess_info.incoming_v2_mt_sms.cause_code, 0);
      
      return TRUE;    
    }    
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */
    else
    {
      MSG_ERROR("Received uknown MT SMS message type %d", tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type, 0, 0);
      return FALSE;      
    }
  }
  else
  {
    MSG_ERROR("Version is invalid %d", *input_buf_ptr, 0, 0);
    return FALSE;  
  }
  /* MT SMS message type */
}

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
