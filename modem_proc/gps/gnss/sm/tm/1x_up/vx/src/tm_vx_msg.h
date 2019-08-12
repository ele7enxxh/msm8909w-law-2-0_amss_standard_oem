#ifndef TM_VX_MSG_H
#define TM_VX_MSG_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM Vx Message Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM Vx 
  Msg packing/unpacking routines

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx_msg.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/06   cl      Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "qw.h"
#include "bit.h"
#include "mccdma.h"
#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif
#include <stdlib.h>

/*=============================================================================

                               Data Definitions

=============================================================================*/
/* Maximum Requestor ID length */
#define TM_VX_MAX_REQUESTER_ID_LEN  255
#define TM_VX_MAX_APP_INFO_LEN      100

/* Max size for Vx SPPReq/SPPResp/SMS */
#define TM_VX_MAX_MSG_SIZE      200

/* Size of second field in GPS time filed in Vx SPPReq */
#define TM_VX_SIZE_OF_TIME_SEC  3

/* Size of Fixed V1 SPPReq size (in bytes) (app id length field is included but info is not */
#define TM_V1_SPPREQ_FIX_SIZE  38

/* V1 Message Types */
#define TM_V1_PDE_ADDR_SOURCE_SPPRESP       0x1
#define TM_V1_PDE_ADDR_SOURCE_MT_SMS        0x2


/* Size of Fixed V2 SPPReq size (in bytes) */
#define TM_V2_SPPREQ_FIX_SIZE  48

/* Size of Fixed V2 Pos Rpt size (in bits) */
#define TM_V2_POS_RPT_FIX_SIZE_IN_BIT    304
#define TM_V2_POS_RPT_VAR_P_SIZE_IN_BIT  107
#define TM_V2_POS_RPT_VAR_V_SIZE_IN_BIT  27
#define TM_V2_POS_RPT_VAR_H_SIZE_IN_BIT  19

/* Size of Fixed V2 MO Cancel size (in bytes) */
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
#define TM_V2_MO_CANCEL_FIX_SIZE         13
#else
/* 17 + 2 (fixed byte in App ID record) */
#define TM_V2_MO_CANCEL_FIX_SIZE         19
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */

/* V2 Message Types */
#define TM_V2_MESSAGE_TYPE_SPPRESP       0x1
#define TM_V2_MESSAGE_TYPE_POS_RPT_RSP   0x2

/* V2 MO SMS Reject Message Type*/
#define TM_V2_MO_SMS_MESSAGE_TYPE_REJECT  0x1

/* V2 MO SMS Cancel Message Type*/
#define TM_V2_MO_SMS_MESSAGE_TYPE_CANCEL  0x2


/* For V1 TCP Msg Header */
#define TM_V1_TCP_HDR_SIZE       3

/* For V2 TCP Msg Header */
#define TM_V2_TCP_HDR_SIZE       4

/* For V1 SMS Msg Header */
#define TM_V1_SMS_HDR_SIZE       3

/* For V2 SMS Msg Header */
#define TM_V2_SMS_HDR_SIZE       3

/* Maximum URL address length */
#define TM_VX_MAX_URL_LENTH         128

/* Min Msg Length */
#define TM_V1_SMS_MIN_MSG_SIZE      10
#define TM_V2_SMS_MIN_MSG_SIZE      3

/* Message types.  SMS message from MPC to MS
*/
#define TM_V2_MT_SMS_MSG_TYPE_POS_REQUEST 0x01
#define TM_V2_MT_SMS_MSG_TYPE_NI_CANCEL   0x02
#define TM_V2_MT_SMS_MSG_TYPE_MO_CANCEL   0x03

typedef struct 
{
  uint16   id_length;
  byte     id[TM_VX_MAX_APP_INFO_LEN];
} tm_vx_int_ms_mpc_app_id_record;

typedef PACKED struct PACKED_POST 
{
  byte     time_sec[8];
} gps_time_sec_type;

typedef PACKED struct PACKED_POST 
{
  byte     id_length[16];
  byte     id[8];
} tm_vx_ext_ms_mpc_app_id_record;

typedef enum{
  TM_V1_SPPRESP_POS_STATUS_IND_GPSONE              = 0,
  TM_V1_SPPRESP_POS_STATUS_IND_CACHED              = 1,
  TM_V1_SPPRESP_POS_STATUS_IND_CELL                = 2,
  TM_V1_SPPRESP_POS_STATUS_IND_USER_DENIED_REQ_ACK = 3,
  TM_V1_SPPRESP_POS_STATUS_IND_NO_AUTH             = 4,
  TM_V1_SPPRESP_POS_STATUS_IND_REFUSED             = 5,
  TM_V1_SPPRESP_POS_STATUS_IND_MAX
} tm_v1_sppresp_pos_status_e_type;

typedef enum{
  TM_V2_SPPRESP_POS_STATUS_IND_GPSONE     = 0,
  TM_V2_SPPRESP_POS_STATUS_IND_CACHED     = 1,
  TM_V2_SPPRESP_POS_STATUS_IND_CELL       = 2,
  TM_V2_SPPRESP_POS_STATUS_IND_NO_AUTH    = 3,
  TM_V2_SPPRESP_POS_STATUS_IND_REFUSED    = 4,
  TM_V2_SPPRESP_POS_STATUS_IND_NO_ACTION  = 5,
  TM_V2_SPPRESP_POS_STATUS_IND_UNAVAIL    = 6,
  TM_V2_SPPRESP_POS_STATUS_IND_CANCEL     = 7,
  TM_V2_SPPRESP_POS_STATUS_IND_MAX
} tm_v2_sppresp_pos_status_e_type;


typedef enum{
  TM_VX_MT_SMS_POS_TECH_IND_IS801 = 0,
  TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR,
  TM_VX_MT_SMS_POS_TECH_IND_CACHED,  
  TM_VX_MT_SMS_POS_TECH_IND_MAX
} tm_vx_mt_sms_pos_tech_ind_e_type;

typedef enum {
  TM_VX_MS_MPC_MSG_SPPREQ  = 0x01,
  TM_VX_MS_MPC_MSG_POS_RPT = 0x02,
  TM_VX_MS_MPC_MSG_CANCEL  = 0x03,
  TM_VX_MS_MPC_MSG_MAX
} tm_vx_ms_mpc_msg_e_type;

typedef enum {
  TM_V1_MS_CAP_NONE          = 0x0,
  TM_V1_MS_CAP_CELL_ID       = 0x01,
  TM_V1_MS_CAP_MSA_HYBRID    = 0x02,
  TM_V1_MS_CAP_BOTH_MSA_MSB  = 0x03,  
} tm_v1_ms_cap_e_type;

typedef enum {
  TM_V2_MS_CAP_NONE          = 0x0,
  TM_V2_MS_CAP_CELL_ID       = 0x01,
  TM_V2_MS_CAP_AFLT_ONLY     = 0x02,
  TM_V2_MS_CAP_MSA_HYBRID    = 0x03,
  TM_V2_MS_CAP_BOTH_MSA_MSB  = 0x04,  
} tm_v2_ms_cap_e_type;

typedef enum {
  TM_VX_APP_TYPE_WAP         = 0,
  TM_VX_APP_TYPE_BREW        = 1,
  TM_VX_APP_TYPE_JAVA        = 2,
  TM_VX_APP_TYPE_MS_RESIDENT = 3,
  TM_VX_APP_TYPE_NI          = 4
} tm_vx_app_e_type;

/* 
  Enum for the V2 message containg URL
*/
typedef enum {
  TM_VX_MSG_URL_MSG_TYPE_SPPRESP = 0,
  TM_VX_MSG_URL_MSG_TYPE_POSACK
} tm_vx_msg_url_msg_type_e_type;

typedef struct{
  byte *adrs_length;
  byte *adrs_encoding;
  char *url_adrs;
  dword *pde_ip_adrs;
  word  *pde_port_number;
} tm_vx_msg_url_msg_ptr_s_type;

typedef enum {
  TM_VX_MSG_NOTIFY_AND_VERIFY_NOT_REQ = 0,
  TM_VX_MSG_NOTIFY_AND_VERIFY_REQ = 1,
  TM_VX_MSG_NOTIFY_ONLY = 2
} tm_vx_msg_notify_ind_e_type;



/*========================================
  Data Structure for V1 SPPReq / SPPResp
========================================*/
/* Internal structure for V1 SPPReq */
/* 
   The sequence of the data fileds (internal) has been adjusted 
   for memory address alignment to reduce the size of the structure 
*/
typedef struct
{
  uint8                           version;
  uint16                          length;    
  uint8                           mspcap;
  uint8                           app_type;
  uint8                           consent_ind;
  uint8                           pqos;
  uint32                          brew_id;
  uint16                          sid;
  uint16                          nid;
  uint16                          base_id;
  uint32                          base_lat;
  uint32                          base_long;
  uint16                          time_week;
  uint8                           time_sec[TM_VX_SIZE_OF_TIME_SEC];
  tm_vx_int_ms_mpc_app_id_record  app_record;
  uint64                          ms_id;
} tm_vx_int_v1_sppreq_s_type;

/* External structure for V1 SPPReq */
typedef PACKED struct PACKED_POST
{
  byte                            version[8];
  byte                            length[16];    
  byte                            mspcap[8];
  byte                            app_type[8];
  byte                            consent_ind[8];
  byte                            pqos[8];
  byte                            brew_id[32];
  byte                            sid[16];
  byte                            nid[16];
  byte                            base_id[16];
  byte                            base_lat[24];
  byte                            base_long[24];
  byte                            time_week[16];
  gps_time_sec_type               time_secs[TM_VX_SIZE_OF_TIME_SEC];
  tm_vx_ext_ms_mpc_app_id_record  app_recored;
  byte                            ms_id[64];
} tm_vx_ext_v1_sppreq_s_type;

/* Internal structure for V1 SPPResp */
/* 
   The sequence of the data fileds (internal) has been adjusted 
   for memory address alignment to reduce the size of the structure 
*/
typedef struct {
  uint8                           version;
  uint8                           pos_status_ind;
  uint8                           pos_incl;
  uint8                           loc_unc_ang;
  uint8                           loc_unc_a;  
  uint8                           loc_unc_p;
  uint8                           velocity_incl;
  uint8                           velocity_ver;  
  uint8                           height_incl;
  uint8                           loc_unc_v;
  uint16                          length; 
  uint16                          pde_port_number;  
  uint16                          time_week;
  uint16                          velocity_hor;
  uint16                          heading; 
  uint16                          height;  
  uint32                          pde_ip_adrs;
  uint32                          lat;
  uint32                          lon;
  uint8                           time_sec[TM_VX_SIZE_OF_TIME_SEC];
} tm_vx_int_v1_sppresp_s_type;

/* External structure for V1 SPPResp */
typedef PACKED struct PACKED_POST 
{
  byte                             version[8];
  byte                             length[16];
  byte                             pos_status_ind[8];
  byte                             pde_ip_adrs[32];
  byte                             pde_port_number[16];
  byte                             pos_incl[8];
  byte                             time_week[16]; 
  gps_time_sec_type                time_secs[TM_VX_SIZE_OF_TIME_SEC];
  byte                             lat[25];
  byte                             lon[26];
  byte                             loc_unc_ang[4];
  byte                             loc_unc_a[5];
  byte                             loc_unc_p[5];
  byte                             velocity_incl[1];
  byte                             velocity_hor[9];
  byte                             heading[10];
  byte                             velocity_ver[8];
  byte                             height_incl[1];
  byte                             height[14];
  byte                             loc_unc_v[5];
} tm_vx_ext_v1_sppresp_s_type;

/* Internal structure for V1 MT SMS */
typedef struct 
{
  uint8                            version;
  uint8                            notification_ind;
  uint8                            position_tech_ind;
  uint8                            correlation_id;
  uint32                           pde_id;
  uint16                           pde_port;
  uint8                            requester_id_len;
  char                             requester_id[TM_VX_MAX_REQUESTER_ID_LEN];
} tm_vx_int_v1_mt_sms_s_type;

typedef PACKED struct PACKED_POST 
{
  uint8                            version;
  uint8                            max_version;
  uint8                            consent_ind;
  uint8                            gpsone_status;
  uint8                            correlation_id;
  uint16                           sid;
  uint16                           nid;
  uint16                           base_id;
  uint8                            base_lat[3];
  uint8                            base_long[3];
  uint16                           weeknumber;
  uint8                            sec[3];
} tm_v1_mo_sms_s_type;


/*========================================
  Data Structure for V2 SPPReq / SPPResp
========================================*/
/* Internal structure for V2 SPPReq */
/* 
   The sequence of the data fileds (internal) has been adjusted 
   for memory address alignment to reduce the size of the structure 
*/
typedef struct
{
  uint8                           version;
  uint8                           msg_type;
  uint16                          length;    
  uint8                           mspcap;
  uint8                           app_type;      
  uint8                           pqos;
  uint8                           cor_id;  
  uint8                           band_class;    
  uint8                           time_sec[TM_VX_SIZE_OF_TIME_SEC];  
  uint16                          sid;
  uint16                          nid;  
  uint16                          base_id;  
  uint16                          ref_pn;  
  uint16                          cdma_freq;
  uint16                          time_week;    
  uint32                          duration;
  uint32                          brew_id;
  uint32                          base_lat;
  uint32                          base_long;  
  uint64                          ms_id;
  tm_vx_int_ms_mpc_app_id_record  app_record;
} tm_vx_int_v2_sppreq_s_type;

/* External structure for V2 SPPReq */
typedef PACKED struct PACKED_POST
{
  byte                            version[8];
  byte                            msg_type[8];  
  byte                            length[16];    
  byte                            mspcap[8];
  byte                            app_type[8];
  byte                            pqos[8];
  byte                            duration[32];
  byte                            cor_id[8];
  byte                            ms_id[64];
  byte                            sid[16];
  byte                            nid[16];
  byte                            base_id[16];
  byte                            base_lat[24];
  byte                            base_long[24];
  byte                            ref_pn[16];
  byte                            band_class[8];
  byte                            cdma_freq[16];
  byte                            time_week[16];
  gps_time_sec_type               time_secs[TM_VX_SIZE_OF_TIME_SEC];
  byte                            brew_id[32];
  tm_vx_ext_ms_mpc_app_id_record  app_recored;
} tm_vx_ext_v2_sppreq_s_type;

/* Internal structure for V2 SPPResp */
/* 
   The sequence of the data fileds (internal) has been adjusted 
   for memory address alignment to reduce the size of the structure 
*/
typedef struct {
  uint8                           version;
  uint8                           message_type;
  uint8                           loc_unc_ang;
  uint8                           loc_unc_a;  
  uint8                           loc_unc_p;
  uint8                           velocity_incl;
  uint8                           cor_id;
  uint8                           pos_status_ind;  
  uint8                           adrs_length;
  uint8                           adrs_encoding; 
  uint8                           pos_incl;
  uint8                           velocity_ver;  
  uint8                           height_incl;
  uint8                           loc_unc_v;   
  uint16                          length;  
  uint16                          velocity_hor;
  uint16                          heading;  
  uint16                          height;    
  uint16                          pde_port_number;  
  uint16                          time_week;
  uint32                          pde_ip_adrs;
  uint32                          lat;
  uint32                          lon;
  char                            url_adrs[TM_VX_MAX_URL_LENTH];  
  uint8                           time_sec[TM_VX_SIZE_OF_TIME_SEC];
} tm_vx_int_v2_sppresp_s_type;

/* External structure for V2 SPPResp */
typedef PACKED struct PACKED_POST 
{
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             correlation_id[8];
  byte                             pos_status_ind[8];
  byte                             adrs_length[8];
  byte                             pde_adrs_encoding[8];
  byte                             pde_url_adrs[TM_VX_MAX_URL_LENTH];
  byte                             pde_ip_adrs[32];
  byte                             pde_port_number[16];
  byte                             pos_incl[8];
  byte                             time_week[16]; 
  gps_time_sec_type                time_secs[TM_VX_SIZE_OF_TIME_SEC];
  byte                             lat[25];
  byte                             lon[26];
  byte                             loc_unc_ang[4];
  byte                             loc_unc_a[5];
  byte                             loc_unc_p[5];
  byte                             velocity_incl[1];
  byte                             velocity_hor[9];
  byte                             heading[10];
  byte                             velocity_ver[8];
  byte                             height_incl[1];
  byte                             height[14];
  byte                             loc_unc_v[5];
} tm_vx_ext_v2_sppresp_s_type;


/* Internal structure for V2 Postion Report */
typedef struct 
{
  uint8                            version;
  uint8                            message_type;
  uint16                           length;
  uint16                           fix_num;
  uint8                            cor_id;
  uint64                           ms_id;
  uint16                           time_week_cell;
  uint8                            time_secs_cell[3];
  uint16                           sid;
  uint16                           nid;
  uint16                           base_id;
  uint32                           base_lat;
  uint32                           base_long;
  uint16                           ref_pn;
  uint8                            band_class;
  uint16                           cdma_freq;
  uint8                            pos_incl;
  uint16                           time_week_pos;
  uint8                            time_secs_pos[3];
  uint32                           lat;
  uint32                           lon;
  uint8                            loc_ang;
  uint8                            loc_a;
  uint8                            loc_p;
  uint8                            velocity_incl;
  uint16                           velocity_h;
  uint16                           heading;
  uint8                            velocity_v;
  uint8                            height_incl;
  uint16                           height;
  uint8                            loc_v;
} tm_vx_int_v2_pos_rpt_s_type;

/* External structure for V2 Position Report */
typedef PACKED struct PACKED_POST {
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             fix_num[16];
  byte                             cor_id[8];
  byte                             ms_id[64];
  byte                             time_week_cell[16];
  gps_time_sec_type                time_secs_cell[TM_VX_SIZE_OF_TIME_SEC];
  byte                             sid[16];
  byte                             nid[16];
  byte                             base_id[16];
  byte                             base_lat[24];
  byte                             base_long[24];
  byte                             ref_pn[16];
  byte                             band_class[8];
  byte                             cdma_freq[16];
  byte                             pos_incl[8];
  byte                             time_week_pos[16];
  gps_time_sec_type                time_secs_pos[TM_VX_SIZE_OF_TIME_SEC];
  byte                             lat[25];
  byte                             lon[26];
  byte                             loc_ang[4];
  byte                             loc_a[5];
  byte                             loc_p[5];
  byte                             velocity_incl[1];
  byte                             velocity_h[9];
  byte                             heading[10];
  byte                             velocity_ver[8];
  byte                             height_incl[1];
  byte                             height[14];
  byte                             loc_v[5];
} tm_vx_ext_v2_pos_rpt_s_type;


#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
/* Internal structure for V2 Pos Rpt ack */
typedef struct {
  uint8                            version;
  uint8                            message_type;
  uint16                           length;
  uint8                            cor_id;
  uint8                            report; 
  uint8                            adrs_length;
  uint8                            adrs_encoding;
  char                             url_adrs[TM_VX_MAX_URL_LENTH];
  uint32                           pde_ip_adrs;
  uint16                           pde_port_number;
} tm_vx_int_v2_pos_rpt_resp_s_type;

/* External structure for V2 Pos Rpt ack */
typedef PACKED struct PACKED_POST {
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             cor_id[8];
  byte                             report[8]; 
  byte                             adrs_length[8];
  byte                             pde_adrs_encoding[8];
  byte                             pde_url_adrs[TM_VX_MAX_URL_LENTH * 8];
  byte                             pde_ip_adrs[32];
  byte                             pde_port_number[16];
} tm_vx_ext_v2_pos_rpt_resp_s_type;

#else

/* Internal structure for V2 Pos Rpt ack */
typedef struct {
  uint8                            version;
  uint8                            message_type;
  uint16                           length;
  uint8                            cor_id;
  uint8                            report; 
} tm_vx_int_v2_pos_rpt_resp_s_type;

/* External structure for V2 Pos Rpt ack */
typedef PACKED struct PACKED_POST {
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             cor_id[8];
  byte                             report[8]; 
} tm_vx_ext_v2_pos_rpt_resp_s_type;

#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */

#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF

/* Internal structure for V2 MO Cancel */
typedef struct{
  uint8                            version;
  uint8                            message_type;
  uint16                           length;
  uint8                            correlation_id;
  uint64                           ms_id;
} tm_vx_int_v2_mo_cancel_s_type;

/* External structure for V2 MO Cancel */
typedef PACKED struct PACKED_POST{
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             correlation_id[8];
  byte                             ms_id[64];
} tm_vx_ext_v2_mo_cancel_s_type;

#else

/* Internal structure for V2 MO Cancel */
typedef struct{
  uint8                            version;
  uint8                            message_type;
  uint16                           length;
  uint8                            app_type;
  uint64                           ms_id;
  uint32                           brew_id;
  tm_vx_int_ms_mpc_app_id_record   app_record;
} tm_vx_int_v2_mo_cancel_s_type;

/* External structure for V2 MO Cancel */
typedef PACKED struct PACKED_POST{
  byte                             version[8];
  byte                             message_type[8];
  byte                             length[16];
  byte                             app_type[8];
  byte                             ms_id[64];
  byte                             brew_id[32];
  tm_vx_ext_ms_mpc_app_id_record   app_record;
} tm_vx_ext_v2_mo_cancel_s_type;

#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */


/* Internal structure for V2 MT SMS */
typedef struct 
{
  uint8                           version;
  uint8                           message_type;
  uint8                           length;
  uint8                           notification_ind;
  uint8                           position_tech_ind;
  uint8                           qos_incl;
  uint8                           qos;  
  uint16                          num_fixes;
  uint16                          tbf;
  uint8                           is801_mode;
  uint8                           correlation_id;
  uint8                           requester_id_encoding;
  uint8                           requester_id_len;
  char                            requester_id[TM_VX_MAX_REQUESTER_ID_LEN];
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
  uint8                           cause_code;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */
} tm_vx_int_v2_mt_sms_s_type;

typedef PACKED struct PACKED_POST 
{
  uint8                           version;
  uint8                           msg_type;
  uint8                           length;
  uint8                           correlation_id;
  uint8                           rej_reason;
  uint16                          sid;
  uint8                           cell_incl;
  uint16                          nid;
  uint16                          base_id;
  uint8                           base_lat[3];
  uint8                           base_long[3];
  uint16                          weeknumber;
  uint8                           sec[3];
} tm_v2_mo_sms_rej_s_type;

typedef PACKED struct PACKED_POST
{
  uint8                           version;
  uint8                           msg_type;
  uint8                           length;
  uint8                           correlation_id;
  uint8                           cancel_reason;
} tm_v2_mo_sms_cancel_s_type;

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
extern void tm_vx_gps_week_ms( qword ts, uint16 *gps_week, uint32 *gps_ms );

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
extern int tm_vx_msg_create_v1_sppreq
(
  byte  *output_buf_ptr,
  uint8 size_of_output_buf
);

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
extern boolean tm_vx_msg_decode_v1_sppresp
(
  byte *input_buf_ptr,
  uint16 msg_len
);

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
extern boolean tm_vx_msg_decode_incoming_v1_mt_sms
(
  byte   *input_buf_ptr,
  uint16 data_length
);

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
extern int tm_vx_msg_create_v2_sppreq
(
  byte  *output_buf_ptr,
  uint8 size_of_output_buf
);

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
extern int tm_vx_msg_create_v2_pos_rpt
(
  byte   *output_buf_ptr,
  uint8  size_of_output_buf 
);


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
extern int tm_vx_msg_create_v2_mo_cancel
(
  byte   *output_buf_ptr,
  uint8  size_of_output_buf
);

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
extern boolean tm_vx_msg_decode_v2_sppresp
(
  byte *input_buf_ptr,
  uint16 msg_len
);

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
extern boolean tm_vx_msg_decode_v2_pos_rpt_resp
(
  byte *input_buf_ptr,
  uint16 msg_len
);

/*===========================================================================

FUNCTION tm_vx_msg_decode_incoming_v2_mt_sms

DESCRIPTION
  This function is called by tm_vx to decode incoming V2 MT SMS
  Result is saved in tm_vx_gen_info.sess_info.v2_mt_sms.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if V2 MT SMS is decoded correctly
  FALSe, otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_vx_msg_decode_incoming_v2_mt_sms
(
  byte *input_buf_ptr,
  uint16 msg_len
);

#ifdef __cplusplus
}
#endif

#endif /* TM_VX_MSG_H */


