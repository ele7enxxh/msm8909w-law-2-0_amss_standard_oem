#ifndef TM_IS801_UTIL_H
#define TM_IS801_UTIL_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  IS801 utility module

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_is801_util.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/06   ank     Added support for IS801 Rx and Tx events.
12/20/06   ank     Changes to send PRM and post-fix PPM together.
12/13/06   jw      Exposed a util function.
08/29/06   cl      Initial version

============================================================================*/
#include "tm_is801.h"
#include "tm_prtl_iface.h"
#include "log.h"
#include "log_dmss.h"
#include "sm_dm_event.h"
#include "pdapi.h"

/*---------------------------------------------------------------------------
                   IS801 Logging Stucture Types
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                    IS-801 GPS FORWARD LINK MESSAGE TYPE
---------------------------------------------------------------------------*/

#define IS801_FWD_MSG_LEN 255

LOG_RECORD_DEFINE(LOG_GPS_FWD_MSG_C)

  byte          hdr_len;                   /* Message payload header length */
  byte          msg[ IS801_FWD_MSG_LEN ];  /* Message payload */

LOG_RECORD_END

typedef LOG_GPS_FWD_MSG_C_type  log_gps_fwd_msg_type;

/*---------------------------------------------------------------------------
                    IS-801 GPS REVERSE LINK MESSAGE TYPE
---------------------------------------------------------------------------*/

#define IS801_REV_MSG_LEN 255

LOG_RECORD_DEFINE(LOG_GPS_REV_MSG_C)

  byte          hdr_len;                   /* Message payload header length */
  byte          msg[ IS801_REV_MSG_LEN ];  /* Message payload */

LOG_RECORD_END

typedef LOG_GPS_REV_MSG_C_type  log_gps_rev_msg_type;


typedef union
{
    log_hdr_type          hdr;                       /* Log header */
    log_gps_fwd_msg_type  fwd;
    log_gps_rev_msg_type  rev;
} is801_log_entry;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 MS RESPONSE TYPES                               */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define TM_REJ_RESP_CODE               0x00 
#define TM_MS_INFO_RESP_CODE           0x02
#define TM_AMWF_RESP_CODE              0x03
#define TM_PR_MEAS_RESP_CODE           0x04
#define TM_PPM_MEAS_RESP_CODE          0x05
#define TM_MSTO_MEAS_RESP_CODE         0x06
#define TM_CACK_RESP_CODE              0x07

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 PDE RESPONSE TYPES                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef enum 
{
  TM_LOC_RESP_CODE = 0x01,
  TM_BS_CAP_RESP_CODE = 0x02,
  TM_BS_ALM_RESP_CODE = 0x03,
  TM_GPS_AA_RESP_CODE = 0x04,    
  TM_GPS_SA_RESP_CODE = 0x05,
  TM_GPS_LA_S_RESP_CODE = 0x06,
  TM_GPS_LA_C_RESP_CODE = 0x07,
  TM_GPS_ALM_RESP_CODE = 0x08,
  TM_GPS_EPH_RESP_CODE = 0x09,
  TM_GPS_NAV_MSG_RESP_CODE = 0x0a,
  TM_GPS_ALM_COR_RESP_CODE = 0x0b,
  TM_GPS_SAT_HLTH_RESP_CODE = 0x0c,
  
  TM_MAX_PDE_TO_MS_RESP_CODE = TM_GPS_SAT_HLTH_RESP_CODE
} tm_pde_to_ms_resp_type;

/*===========================================================================

FUNCTION is801_util_rlink_convert_ppm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_rlink_convert_ppm
(
  srch_ppm_rpt_type       *input_ppm_buf_ptr,
  is801_ppm_resp_type     *output_ppm_buf_ptr
);

/*===========================================================================

FUNCTION is801_util_convert_is801_to_tm_lr

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_convert_is801_to_tm_lr
(
  tm_lr_resp_type       *output_lr_buf_ptr,
  const is801_lr_resp_type    *input_lr_buf_ptr
);

/*===========================================================================

FUNCTION is801_util_rlink_convert_prm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_rlink_convert_prm
(
  srch_ppm_rpt_type       *input_ppm_buf_ptr,
  sm_GpsMeasRptStructType *input_prm_buf_ptr,
  is801_prm_resp_type     *output_prm_buf_ptr
);

/*===========================================================================

FUNCTION is801_util_flink_convert_aa

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_aa
(
  is801_aa_resp_type               *input_aa_buf_ptr,
  gnss_SvAcqAssistCommonStructType *output_aa_buf_ptr,
  gnss_SvDirStructType             *output_sv_dir_buf_ptr 
);

/*===========================================================================

FUNCTION is801_util_flink_convert_sa

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_sa
(
  is801_sa_resp_type     *input_sa_buf_ptr,
  gps_SADataStructType   *output_sa_buf_ptr
);

/*===========================================================================

FUNCTION is801_util_flink_convert_iono

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_iono
(
  is801_eph_resp_type    *input_eph_buf_ptr,
  gps_IonoStructType     *output_iono_buf_ptr
);


/*===========================================================================

FUNCTION is801_util_flink_convert_eph

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_eph
(
  is801_eph_resp_type    *input_eph_buf_ptr,
  gps_EphStructType      *output_eph_buf_ptr,
  int                    sv_num
);

/*===========================================================================

FUNCTION is801_util_flink_convert_alm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_alm
(
  is801_alm_resp_type    *input_alm_buf_ptr,
  gps_AlmStructType      *output_alm_buf_ptr,
  int                    sv_num  
);

/*===========================================================================

FUNCTION is801_util_flink_convert_seed

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_flink_convert_seed
(
  const is801_lr_resp_type    *input_lr_buf_ptr,
  gps_RefLocStructType  *output_seed_buf_ptr
);

/*===========================================================================
FUNCTION is801_util_log_flink_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_util_log_flink_msg(void *log_buf, uint16 log_buf_length);

/*===========================================================================
FUNCTION is801_util_log_rlink_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_util_log_rlink_msg(void *log_buf, byte log_buf_length);

/*===========================================================================
FUNCTION is801_util_report_is801_event_tx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_util_report_is801_event_tx(sm_event_is801_msg_e_type msg_type, uint32 bmap);

/*===========================================================================
FUNCTION is801_util_report_is801_event_rx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_util_report_is801_event_rx(sm_event_is801_msg_e_type msg_type,uint32 bmap);

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
extern boolean is801_util_rlink_create_msinfo
(
  is801_msi_resp_type     *output_msi_buf_ptr
);

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
extern boolean is801_util_rlink_create_prdm_bsp
(
  is801_prdm_bsp_resp_type   *output_prdm_bsp_buf_ptr
);

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

extern boolean is801_util_rlink_convert_msto
(
 srch_ppm_rpt_type   *input_ppm_buf_ptr,
 is801_tom_resp_type *output_tom_buf_ptr
);

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
extern boolean is801_util_rlink_create_rej_resp (is801_rej_resp_type  *output_rej_buf_ptr, uint32 rej_req_type);

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
);

/*===========================================================================

FUNCTION is801_util_convert_tm_lr_to_is801_lr

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_util_convert_tm_lr_to_is801_lr
(
  sm_GnssFixRptStructType  *input_lr_buf_ptr,
  is801_lr_resp_type *output_lr_buf_ptr
);

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
);

#ifdef __cplusplus
}
#endif

#endif /* TM_IS801_UTIL_H */

