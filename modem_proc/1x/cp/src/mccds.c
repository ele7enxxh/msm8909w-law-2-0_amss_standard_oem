/*===========================================================================

              MAIN CONTROL DATA SERVICE SUPPORT FUNCTIONS

GENERAL DESCRIPTION
  This module contains utilities to handle requests from Data Service Task.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccds.c_v   1.3.2.0   30 Nov 2001 17:10:54   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccds.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/13   gga     Feature cleanup
01/03/13   nee     Replacement of memcpy() to memscpy()
09/15/12   ppr     FR#2370 - 1x memory optimization changes
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
11/15/10   jtm     Initial SU API split for CP.
04/19/10   ag      Support for 1x Modem statistical report.
05/14/09   jj      CMI Phase-II update.
05/16/08   bb      Fixed an error in populating the Neighbor Pilot info
                               in the SCRM message
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
12/13/07   pg      Fixed featurization problem on FEATURE_IS2000_CHS and
                   FEATURE_IS2000_R_SCH.
04/26/06   an      Fixed Lint Errors
02/16/06   fc      Fixed parser discrepancies.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
09/22/04   va      Added REL D fields in RRM message
08/01/03   yll     Added Dynamic Feature support.
06/26/01   sb      Merged the following from common archive:
           06/14/01   ht      Enclosed cmd_ptr inside feature labels to avoid
                      a compiler warning.
           06/03/01   ht      Added $Header line for revision control.
           05/31/01   ht      Added a missing semi-colon.
05/17/01   lh      Merged RelA changes into this version
05/11/01   jq      Added mccds_reg_chs_notifier() and mccds_notify_chs()
05/02/01   fc      Added support of T_ADD_ABORT.
04/10/01   jq      Moved all SCRM related data structures and functions from
                   mccsch.c to here.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "mccds_v.h"
#include "rxtx_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mcctcsup.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mccap_v.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* SCRM info */
static struct
{
  uint8 t_mulchan;    /* SCRM pilot strength reporting offset */
  uint8 scrm_seq_num; /* SCRM sequence number */
  uint8 sz_req_blob;  /* Size of the request information block of bytes */
  uint8 req_blob[CAI_MAX_SCRM_BLOB_SIZE]; /* Request info block */
} mccds_scrm;

/* Function pointer to store the DS call back funtion which generate resource
   reqirements e.g. SCRM BLOB size etc.*/
mccds_resource_req_type (*mccds_res_generator) ( uint8* sz_blob, uint8* blob, uint16 so );

/* function pointer to hold the DS call back function to set the Control Hold State */
void (*mccds_notify_chs) (boolean chs_on);

/*===========================================================================

FUNCTION mccds_init_scrm

DESCRIPTION
  This function initializes SCRM related variables and functions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_init_scrm(void)
{
  /* Initialize SCRM pilot reporting offset */
  mccds_update_t_mulchan(0);

  /* Initialize SCRM message sequence number */
  mccds_scrm.scrm_seq_num = 0xFF;

} /* mccds_init_scrm */

/*===========================================================================

FUNCTION mccds_clear_scrm_req_blob

DESCRIPTION
  This function clears SCRM BLOB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_clear_scrm_req_blob(void)
{
  mccds_scrm.sz_req_blob = 0;
} /* mccds_clear_scrm_req_blob */

/*===========================================================================

FUNCTION mccds_update_t_mulchan

DESCRIPTION
  This function updates SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_update_t_mulchan
(
  uint8 t_mulchan
)
{
  mccds_scrm.t_mulchan = t_mulchan;
} /* mccds_update_t_mulchan */

/*===========================================================================

FUNCTION mccds_get_t_mulchan

DESCRIPTION
  This function returns SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_t_mulchan(void)
{
  return mccds_scrm.t_mulchan;
} /* mccds_get_t_mulchan */

/*===========================================================================

FUNCTION mccds_get_scrm_seq_num

DESCRIPTION
  This function returns the last used SCRM sequence number.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_scrm_seq_num(void)
{
  return mccds_scrm.scrm_seq_num;
} /* mccds_get_scrm_seq_num */

/*===========================================================================

FUNCTION mccds_build_sup_chan_req_msg

DESCRIPTION
  This function builds a Supplemental Channel Request message from DS task's
  SCRM blob info and/or SRCH's task SCRM pilot strength measurement report.

DEPENDENCIES
  mccds_scrm.sz_req_blob and mccds_scrm.req_blob must already setup
  properly.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_build_sup_chan_req_msg
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Supplemental Channel Request Message */
  mccsrch_rpt_type *srch_rpt_ptr,
    /* SRCH SCRM Pilot Strength Measurements or T_ADD_ABORT report */
  boolean use_scrm_seq_num
    /* Indicator whether to use SCRM sequence number */
)
{
  int i;
  size_t copy_size;
    /* Return value of memscpy function */

  msg_ptr->scrm.msg_type = CAI_SUP_CHAN_REQ_MSG;
  msg_ptr->scrm.sz_req_blob = mccds_scrm.sz_req_blob;

  /* Pack pilot info only if SCRM for assignment or T_ADD_ABORT */
  if (((mccds_scrm.sz_req_blob != 0) || use_scrm_seq_num))
  {
    if (srch_rpt_ptr == NULL) /* Sanity check only */
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Missing pilot info for SCRM");
    }
    else
    {
        copy_size = memscpy(msg_ptr->scrm.req_blob, CAI_MAX_SCRM_BLOB_SIZE*sizeof(byte),
                            mccds_scrm.req_blob, mccds_scrm.sz_req_blob);

        if( copy_size != mccds_scrm.sz_req_blob )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            mccds_scrm.sz_req_blob,
            CAI_MAX_SCRM_BLOB_SIZE*sizeof(byte));
        }

      msg_ptr->scrm.ref_pn = (word) srch_rpt_ptr->scrm_meas_rpt.ref_pn;
      msg_ptr->scrm.pilot_str = srch_rpt_ptr->scrm_meas_rpt.ref_pn_ecio;

      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        msg_ptr->scrm.ref_pil_rec.pilot_rec_incl =
          populate_pil_rec_info_in_rtc_msg(&srch_rpt_ptr->scrm_meas_rpt.ref_pilot_rec_type,
            &srch_rpt_ptr->scrm_meas_rpt.ref_pilot_rec,
            &msg_ptr->scrm.ref_pil_rec.pilot_rec_type,
            &msg_ptr->scrm.ref_pil_rec.aux_pil_rec.pilot);
      }
      #endif

      msg_ptr->scrm.num_act_pn = 0;
      msg_ptr->scrm.num_nghbr_pn = 0;
      for (i=0; i < CAI_MAX_SCRM_RPT_PN &&
           i < srch_rpt_ptr->scrm_meas_rpt.meas_cnt; i++)
      {
        /* Active Set pilots */
        if (srch_rpt_ptr->scrm_meas_rpt.meas[i].pn_in_aset)
        {
          if (msg_ptr->scrm.num_act_pn < CAI_MAX_SCRM_ACT_PN)
          {
            msg_ptr->scrm.act_pn[msg_ptr->scrm.num_act_pn].pn_phase =
              srch_rpt_ptr->scrm_meas_rpt.meas[i].pn_phase;
            msg_ptr->scrm.act_pn[msg_ptr->scrm.num_act_pn].pn_strength =
              srch_rpt_ptr->scrm_meas_rpt.meas[i].pn_ecio;

            #ifdef FEATURE_IS2000_REL_A
            if (P_REV_IS_7_OR_GREATER)
            {
              msg_ptr->scrm.act_pil_rec[msg_ptr->scrm.num_act_pn].pilot_rec_incl =
                populate_pil_rec_info_in_rtc_msg(
                  &srch_rpt_ptr->scrm_meas_rpt.meas[i].pilot_rec_type,
                  &srch_rpt_ptr->scrm_meas_rpt.meas[i].pilot_rec,
                  &msg_ptr->scrm.act_pil_rec[msg_ptr->scrm.num_act_pn].pilot_rec_type,
                  &msg_ptr->scrm.act_pil_rec[msg_ptr->scrm.num_act_pn].aux_pil_rec.pilot);
            }
            #endif

            msg_ptr->scrm.num_act_pn++;
          }
        }
        /* Candidate or Neighbor Set pilots */
        else
        {
          if (msg_ptr->scrm.num_nghbr_pn < CAI_MAX_SCRM_NGHBR_PN)
          {
            msg_ptr->scrm.nghbr_pn[msg_ptr->scrm.num_nghbr_pn].pn_phase =
              srch_rpt_ptr->scrm_meas_rpt.meas[i].pn_phase;
            msg_ptr->scrm.nghbr_pn[msg_ptr->scrm.num_nghbr_pn].pn_strength =
              srch_rpt_ptr->scrm_meas_rpt.meas[i].pn_ecio;

            #ifdef FEATURE_IS2000_REL_A
            if (P_REV_IS_7_OR_GREATER)
            {
               msg_ptr->scrm.nghbr_pil_rec[msg_ptr->scrm.num_nghbr_pn].pilot_rec_incl =
                populate_pil_rec_info_in_rtc_msg(
                  &srch_rpt_ptr->scrm_meas_rpt.meas[i].pilot_rec_type,
                  &srch_rpt_ptr->scrm_meas_rpt.meas[i].pilot_rec,
                  &msg_ptr->scrm.nghbr_pil_rec[msg_ptr->scrm.num_nghbr_pn].pilot_rec_type,
                  &msg_ptr->scrm.nghbr_pil_rec[msg_ptr->scrm.num_nghbr_pn].aux_pil_rec.pilot);
            }
            #endif

            msg_ptr->scrm.num_nghbr_pn++;
          }
        }
      } /* end for */
    }
  }

  /* Use SCRM sequence number */
  if (use_scrm_seq_num)
  {
    /* Increment SCRM sequence number */
    mccds_scrm.scrm_seq_num = (mccds_scrm.scrm_seq_num + 1) % 16;
    msg_ptr->scrm.use_scrm_seq_num = TRUE;
    msg_ptr->scrm.scrm_seq_num = mccds_scrm.scrm_seq_num;
  }
  else
  {
    msg_ptr->scrm.use_scrm_seq_num = FALSE;
  }

  /* Update modem statistics */
  cdma_stats.channel_config_info.scrm_seq_num = mccds_get_scrm_seq_num();
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} /*lint !e818 */ /* mccds_build_sup_chan_req_msg */

/*===========================================================================

FUNCTION mccds_reg_resource_generator

DESCRIPTION
  This function binds a SCRM blob generator to a function pointer. If an old
  SCRM blob generator already exists, it overwrites it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_reg_resource_generator
(
  mccds_resource_req_type (*ds_resource_generator)
  (
    uint8* sz_blob, uint8* blob, uint16 so
  )
)
{
  mccds_res_generator = ds_resource_generator;
} /* mccds_reg_resource_generator */

/*===========================================================================

FUNCTION mccds_process_resource_request

DESCRIPTION
  This function handles resource request from Data Service Task.

  SCRM request: It gets SCRM blob from DS task. If the sise of SCRM blob is not
  zero, it commands SRCH task to send SCRM pilot strength measurement report.
  Otherwise, it sends SCRM right away.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_process_resource_request(void)
{

  rxtx_cmd_type *cmd_ptr;
  mccds_resource_req_type resource;

  /* See if DS resource generator has registered */
  if (mccds_res_generator)
  {
    resource = mccds_res_generator(&mccds_scrm.sz_req_blob, mccds_scrm.req_blob,
        cdma.so_cur_pri);
    /* See if DS still wants SCRM */
    if (resource == MCCDS_RES_SCRM)
    {
      if (mccds_scrm.sz_req_blob == 0)
      {
#ifdef FEATURE_1X_CP_MEM_OPT
        if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Memory allocation failed for RXTX Buf" );
        }
#else /* !FEATURE_1X_CP_MEM_OPT */
        if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on rxtx_mc_free_q" );
        }
#endif /* FEATURE_1X_CP_MEM_OPT */
        else
        {
          /* Build Supplemental Channel Request Message */
          mccds_build_sup_chan_req_msg(&cmd_ptr->tc_msg.msg, NULL, FALSE);
          cmd_ptr->tc_msg.ack_req = TRUE;

          /* Fill in the command header */
          mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
          rxtx_cmd( cmd_ptr );
        }
      }
      else
      {
        /* Command SRCH to send SCRM pilot strength measurment report */
        mcc_srch_buf.scrm_meas.hdr.cmd = SRCH_SCRM_MEAS_F;
        mcc_srch_buf.scrm_meas.t_mulchan = mccds_scrm.t_mulchan;
        mcc_srch_cmd(&mcc_srch_buf);
      } /* end if (mccsch_)scrm.sz_req_blob == 0) */
    }

    if (mccap_item_is_supported(MCCAP_DF_CHS) && (resource == MCCDS_RES_RRM))
    {
      /* RRM needs to be sent, since it has no payload, we will sent this
         simple mag right here. It can be moved a seperate function if
         needed later. */

      /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
        return;
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
        return;
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      cmd_ptr->tc_msg.msg.rrm.msg_type = CAI_RES_REQ_MSG;
      cmd_ptr->tc_msg.ack_req = TRUE;
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
      M1X_MSG( DCP, LEGACY_MED,
        "Send RRM");
      rxtx_cmd (cmd_ptr);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "No resource generator registered");
  } /* end if (mccds_res_generator) */
} /* mccds_process_resource_request */


/*===========================================================================

FUNCTION mccds_reg_chs_notifier

DESCRIPTION
  This function binds a DS function to modify the Control Hold State status
  inside DS to synchronize state changes in MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_reg_chs_notifier
(
  void (*ds_chs_notifier) ( boolean chs_on )
)
{
  mccds_notify_chs = ds_chs_notifier;
}
