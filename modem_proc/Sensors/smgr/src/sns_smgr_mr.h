#ifndef SNS_SMGR_MR_H
#define SNS_SMGR_MR_H
/*=============================================================================
  @file sns_smgr_mr.h

  This header file contains the data definitions used within the DSPS
  Sensor Manager (SMGR) Message Router

******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_mr.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-09  pn   Updated timestamps from uint32_t to sns_ddf_time_t
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_smgr.h"
#include "sns_smgr_priv.h"
#include "sns_osa.h"
#include "sns_queue.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  sns_ddf_time_t timestamp;   /* timestamp when message is received */
  uint16_t       body_len;
  uint16_t       msg_id;      /* depending on the service */
  uint8_t        msg_type;    /* req, resp, or ind */
  uint8_t        svc_num;     /* SNS_xxxx_SVC_ID_V01 in sns_common_v01.h */
} sns_smgr_mr_header_s;

typedef struct
{
  sns_q_link_s          q_link;
  sns_smgr_mr_header_s  header;
  void*                 body_ptr;
} sns_smgr_mr_msg_s;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
extern void sns_smgr_mr_init(
  OS_FLAG_GRP*                  sig_flags_ptr);

extern sns_smgr_mr_msg_s* sns_smgr_mr_get_msg(void);

sns_err_code_e sns_smgr_mr_send_req(
  const sns_smgr_mr_header_s* msg_header_ptr, 
  void*                       req_ptr,
  void*                       resp_ptr,
  uint16_t                    resp_len,
  void*                       resp_cb_data_ptr);

#endif /* #ifndef SNS_SMGR_MR_H */

