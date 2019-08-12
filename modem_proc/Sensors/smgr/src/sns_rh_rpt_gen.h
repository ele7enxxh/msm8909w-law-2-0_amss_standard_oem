#ifndef SNS_RH_RPT_GEN_H
#define SNS_RH_RPT_GEN_H
/*============================================================================

  @file sns_rh_rpt_gen.h

  @brief
  This file contains definitions for the Report Generation functions
  of the Request Handler

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_rpt_gen.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $*/
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-09-11  pn    Updated signature of sns_rh_rpt_gen_send_buffering_indications()
  2015-08-21  bd    Code cleanup
  2015-06-15  pn    Updated sns_rh_rpt_gen_save_indication() signature
  2015-05-27  bd    SMGR flow control feature
  2014-04-23  pn    Initial version
============================================================================*/
#include "sns_rh.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern bool sns_rh_rpt_gen_send_query_indication(
  sns_rh_rpt_spec_s* rpt_ptr);

extern bool sns_rh_rpt_gen_send_empty_query_indication(
  const sns_rh_query_s* query_ptr);

extern bool sns_rh_rpt_gen_generate_periodic_report(
  sns_rh_rpt_spec_s* rpt_spec_ptr);

extern bool sns_rh_rpt_gen_send_buffering_indications(
  sns_rh_rpt_spec_s* rpt_spec_ptr,
  uint8_t            last_ind_type);

extern bool sns_rh_rpt_gen_send_report_indication(
   sns_rh_rpt_spec_s* rpt_spec_ptr,
   void*              ind_msg_ptr,
   uint16_t           msg_id, 
   uint16_t           len,
   bool               free_me);

extern void sns_rh_rpt_gen_save_indication(
   sns_rh_rpt_spec_s* rpt_spec_ptr,
   void*              ind_msg_ptr,
   uint16_t           len);

extern void sns_rh_rpt_gen_send_saved_ind( 
  sns_rh_rpt_spec_s* rpt_spec_ptr);

extern const sns_smgr_sample_s* sns_rh_rpt_gen_init_item_depot_iter(
  sns_rh_rpt_item_s* item_ptr);

extern void sns_rh_process_conn_resume(void);

#endif /* SNS_RH_RPT_GEN_H */

